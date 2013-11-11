/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:           
*
*/











#include "wsstarrstresponse.h"
#include "wsstarsessioncontext.h"
#include "senwsdescription.h"
#include <SenServiceConnection.h>
#include <SenDateUtils.h>
#include "sencryptoutils.h"
#include "wsstarpolicy.h"
#include "senlogger.h"
#include "wsstarservicesession.h"
#include "msenremoteservicesession.h"  // internal Framework\inc

using namespace WSStarContextKeys;
using namespace WSStarContextValues;

CWSStarSessionContext* CWSStarSessionContext::NewL(CSenXmlReader* aParser, MSenServiceDescription* aSD, CWSStarPolicyHandler* aPolicyHandler)
    {
    CWSStarSessionContext* self = NewLC(aParser, aSD, aPolicyHandler);
    CleanupStack::Pop(self);
    return self;
    }
CWSStarSessionContext* CWSStarSessionContext::NewLC(CSenXmlReader* aParser, MSenServiceDescription* aSD, CWSStarPolicyHandler* aPolicyHandler)
    {
    CWSStarSessionContext* self = new (ELeave) CWSStarSessionContext(aParser);
    CleanupStack::PushL (self);
    self->ConstructL(aSD, aPolicyHandler);
    return self;
    }

// Constructor
CWSStarSessionContext::CWSStarSessionContext(CSenXmlReader* aParser):
    iDescriptions(EFalse,EFalse),
    iInts(EFalse,ETrue),
    iDescs(EFalse,ETrue),
    iParser(aParser)
    {
    }

// Constructor
void CWSStarSessionContext::ConstructL(MSenServiceDescription* aSD, CWSStarPolicyHandler* /*aPolicyHandler*/)
    {
    if (CWSStarPolicy::CheckPolicyValueL(WSPolicy::KSoap12, aSD))
        {
        Update(KSoapVersion, ESOAP12);
        }
    else
        {
        Update(KSoapVersion, ESOAP11);
        }   
    HBufC8* addrVer = CWSStarPolicy::GetPolicyValueL(WSPolicy::KAddressing, aSD);
    if (addrVer)
        {
        Update(KAddressingVersion, *addrVer);
        delete addrVer;
        addrVer = NULL;
        }  
            
    }

// Destructor
CWSStarSessionContext::~CWSStarSessionContext()
    {
    iDescriptions.Reset();
    iInts.Reset();
    iDescs.Reset();
    if( ipSignedParts )
        {
        ipSignedParts->ResetAndDestroy();
        delete ipSignedParts;
        ipSignedParts = NULL;
        }
    delete iBody;
    iBody = NULL;
    }
    
TInt CWSStarSessionContext::UpdatePolicyL(CWSStarPolicyHandler* aPolicyHandler, MSenServiceDescription* aSD)
    {
    CWSStarPolicyHandler* pPolicyHandler = (CWSStarPolicyHandler*)aPolicyHandler;
    if (pPolicyHandler)
        {
        RPointerArray<HBufC8>* pElements = new (ELeave) RPointerArray<HBufC8>();
        CleanupStack::PushL( pElements );
        CWSStarPolicy::GetPolicyValueL( WSPolicy::KSignedParts, KNullDesC8, pPolicyHandler, aSD, *pElements );
        if ( ipSignedParts )
            {
            ipSignedParts->ResetAndDestroy();
            delete ipSignedParts;
            ipSignedParts = NULL;
            }
        ipSignedParts = pElements; // takes the ownership of the elements array
        CleanupStack::Pop( pElements );
        }
    return KErrNone;
    }
// -----------------------------------------------------------------------------
// Sets token context keys based on RST response.
// -----------------------------------------------------------------------------
//
TInt CWSStarSessionContext::SetTokenKeysL(CWSStarRSTResponse& aRSTResponse,
        HBufC8* aClientEntropy, TBool aSignalConsumers, TBool aSCT)
    {

//1)TOKEN
    CWSStarServiceSession* session = (CWSStarServiceSession*)
                GetSenRemoteServiceSessionL(KServiceSession);
    CSenCredential* credential = aRSTResponse.Credential();
    if (credential)
        {
        if (!aSCT)
            {
            if (aRSTResponse.CredentialExpires().Length())
                {
                User::LeaveIfError(Update(KTimestampExpires, aRSTResponse.CredentialExpires()));    
                }

            User::LeaveIfError(Update(KMTIsReplacedBySCT, EFalse));
            TTime clientTime;
            clientTime.UniversalTime();
            TBuf8<SenDateUtils::KXmlDateTimeMaxLength> ts;
            TInt leaveCode(KErrNone);
            TRAP(leaveCode, SenDateUtils::ToXmlDateTimeUtf82L(ts, clientTime);)
            if (!leaveCode)
                {
                User::LeaveIfError(Update(KPhoneTimeWhenMTResolved, ts));                        
                }

            if (aRSTResponse.CredentialCreated().Length())
                {
                User::LeaveIfError(Update(KTimestampCreated, aRSTResponse.CredentialCreated()));

                TTime serverTime = SenDateUtils::FromXmlDateTimeL(aRSTResponse.CredentialCreated());
                if (serverTime != Time::NullTTime())
                    {
                    TTimeIntervalMicroSeconds interval = serverTime.MicroSecondsFrom(clientTime);
                    ((CSenWebServiceSession*)session)->SetClientServerIntervalL(interval);	//codescannerwarnings
                    }
                }

            //adding credential also calculate new status
            ((CSenWebServiceSession*)session)->AddCredentialL(*credential);
            session->AddPropertiesFromSessionContextToCredentialL();
            }
         else
            {
            User::LeaveIfError(Update(KMTIsReplacedBySCT, ETrue));    
            }
            
        //Co-Branding CR
        if(aSignalConsumers)
            {
            session->SetStatusL();
            if (((CSenServiceSession*)session)->StatusL() == KSenConnectionStatusExpired)
                {
                return KErrNotReady;
                }
            }
            
        HBufC8* securityToken = credential->AsXmlL();
        if (securityToken)
            {
            if (!aSCT)
                {
                CleanupStack::PushL(securityToken);
                User::LeaveIfError(Update(KTokenType, aRSTResponse.TokenType()));
                HBufC8* binarySecurityToken =
                        SenCryptoUtils::CreateEncodedBinarySecretL(*securityToken, aRSTResponse.TokenType());
                CleanupStack::PushL(binarySecurityToken);
                User::LeaveIfError(Update(WSStarContextKeys::KSecurityToken,
                        *binarySecurityToken));
                User::LeaveIfError(Update(WSStarContextKeys::KSecurityTokenBackup,
                        *binarySecurityToken));
                CleanupStack::PopAndDestroy(binarySecurityToken);                
                CleanupStack::PopAndDestroy(securityToken);                
                }
            else
                {//SCT is not encoded
                CleanupStack::PushL(securityToken);
                User::LeaveIfError(Update(WSStarContextKeys::KSecurityToken,
                        *securityToken));
                CleanupStack::PopAndDestroy(securityToken);
                }
            }    
        }
    else
        {
        return KErrNotFound;
        }
//2)KEY TOKEN
    if (!aSCT)//if SCT comes, we still use old pop
        {
        const TInt KByteSize = 8;
        TPtrC8 popBase64 = aRSTResponse.POPBase64();
        HBufC8* newPOPBase64 = NULL;
        //if no (POP/BinarySecret) from server
        if (!popBase64.Length())
            {

            if (
                //IOP 2007 improve, determine by ComputedKey tag instead optional nonce value.
                //binaryType is optional attribute so it doesn't determine flow, better is checking ComputedKey tag
                /*(aRSTResponse.BinaryType() == WSTrust::KBinaryTypeNonceValue)*/
                aRSTResponse.ComputedKey().Length()
                &&
                aRSTResponse.EntropySTS().Length())
                    {
                    if (aClientEntropy)
                        {
                        //calcualte new POP using algorithm
                        TPtrC8 computedKey = aRSTResponse.ComputedKey();
                        if (computedKey == WSTrust::KComputedKeyPSHA1Value)
                            {
                            HBufC8* clientBin = SenCryptoUtils::DecodeBase64L(
                                    *aClientEntropy);
                            CleanupStack::PushL(clientBin);
                            HBufC8* STSBin = SenCryptoUtils::DecodeBase64L(
                                    aRSTResponse.EntropySTS());
                            CleanupStack::PushL(STSBin);
                            TInt keySize = aRSTResponse.KeySize();
                            if (!keySize)
                                {
                                TLSLOG(KSenFaultsLogChannel, KSenFaultsLogLevel, WSStarTrustFault::KBadKeySize());
                                keySize = WSTrust::KSizeOfComputedKey;
                                }
                            if (keySize % KByteSize)
                                {
                                TLSLOG(KSenFaultsLogChannel, KSenFaultsLogLevel, WSStarTrustFault::KBadKeySize());
                                }
                            keySize = keySize / KByteSize; //byte = 8 bits
                            HBufC8* newEntropy = 
                                SenCryptoUtils::GetPSHA1HashL( *clientBin, *STSBin, keySize);
                                
                            CleanupStack::PopAndDestroy(2, clientBin);
                            CleanupStack::PushL(newEntropy);
                            newPOPBase64 = SenCryptoUtils::EncodeBase64L(
                                    *newEntropy);
                            CleanupStack::PopAndDestroy(newEntropy);
                            CleanupStack::PushL(newPOPBase64);
                            newEntropy = NULL;
                            popBase64.Set(*newPOPBase64);
                            }
                        else
                        //http://schemas.xmlsoap.org/ws/2005/02/trust/CK/HASH not supported
                            {
                            //still possible to talk with service without POP (we just will not sign anything)
                            //return KErrNotFound;
                            }
                        }
                       else
                        {
                        //server response with cmupting key but client doesnt provide entropy. wrong scenario
                        //still possible to talk with service without POP (we just will not sign anything)
                        }
                    }
        //no from server nor calculated so last chance that
        //6.2.3 If the requestor provides entropy and the responder doesn't 
        //(issuer uses the requestor's key),
        //then a proof-of-possession token need not be returned.
                else if (aClientEntropy)
                    {
                    popBase64.Set(*aClientEntropy);
                    }
                else
                    {
                    //still possible to talk with service without POP (we just will not sign anything)
                    //return KErrNotFound;
                    }
            }
        if (popBase64.Length())
            {
            User::LeaveIfError(Update(KPOPBase64, popBase64));
            if (newPOPBase64) CleanupStack::PopAndDestroy(newPOPBase64);
            
            //binary type always will be (default simmetric)
            if (aRSTResponse.BinaryType()== WSTrust::KBinaryTypeSimmetricValue)
                {
                User::LeaveIfError(Update(KBinaryType,   KPOPTypeSimmetric()));
                }
            else if (aRSTResponse.BinaryType()== WSTrust::KBinaryTypeAsimmetricValue)
                {
                User::LeaveIfError(Update(KBinaryType, KPOPTypeAsimmetric()));
                }
            else if (aRSTResponse.BinaryType()== WSTrust::KBinaryTypeNonceValue)
                {
                User::LeaveIfError(Update(KBinaryType, KPOPTypeNonce()));
                }
            //adding KeyInfo (its identifier)
            TPtrC8 str = aRSTResponse.STR();
            if (str.Length())
                {
                User::LeaveIfError(Update(KSTR, str));
                }
            }
        }
    session->AddPropertiesFromSessionContextToCredentialL();
    return KErrNone;
    }

//-----------------------------------------------------
//              from MSessionContext
//-----------------------------------------------------
const CSenWSDescription* CWSStarSessionContext::GetSession()
    {
    return NULL;
    };
const CSenWSDescription* CWSStarSessionContext::GetInitializer()
    {
    return NULL;
    };

//descriptions    
TInt CWSStarSessionContext::Add(const TDesC8& aKey, CSenWSDescription* aValue)
    {
    return iDescriptions.Append(&aKey,aValue);
    };

TInt CWSStarSessionContext::Update(const TDesC8& aKey, CSenWSDescription* aValue)
    {
    iDescriptions.RemoveByKey(aKey);
    return iDescriptions.Append(&aKey,aValue);
    };

const CSenWSDescription* CWSStarSessionContext::GetSenWSDescriptionL(const TDesC8& aKey)
    {
    TInt index = iDescriptions.Find(aKey);
    if (index!=KErrNotFound)
        {
        return iDescriptions.ValueAt(index);
        }
    else
        {
        return NULL;
        };        
    };


//sessions    
TInt CWSStarSessionContext::Add(const TDesC8& aKey, MSenRemoteServiceSession& aValue)
    {
    return Update(aKey, aValue);
    };
TInt CWSStarSessionContext::Update(const TDesC8& /*aKey*/, MSenRemoteServiceSession& aValue)
    {
    iSession = &aValue;
    return KErrNone;
    };
const MSenRemoteServiceSession* CWSStarSessionContext::GetSenRemoteServiceSessionL(const TDesC8& /*aKey*/)
    {
    return iSession;
    };    



//-----------------------------------------------------
//              from MContext
//-----------------------------------------------------

SenContext::TContextType CWSStarSessionContext::Type() const  
    {
    return SenContext::ESessionContext;
    }

SenContext::TContextDirection CWSStarSessionContext::Direction() const 
    {
    return SenContext::EBoth;
    }



//-----------------descs
TInt CWSStarSessionContext::Add(const TDesC8& aKey, const TDesC8& aValue)
    {   
        //version without copy
        //return iDescs.Append(&aKey,&aValue);
        
        //version with making of copy
        if (!aValue.Length()) return KErrUnderflow;
        HBufC8* newValue = NULL;
        TInt error(KErrNone);
        TRAP(error, newValue = aValue.AllocL());
        if (!error)
            {
            error = iDescs.Append(&aKey,newValue);
            }
        return 
            error;
    }
TInt CWSStarSessionContext::Update(const TDesC8& aKey, const TDesC8& aValue)
    {
        //return iDescs.UpdateValue(&aKey,&aValue);
        
        //version with makeing of copy
        TInt error(KErrNone);
        if (!aValue.Length())
            {
            error = Remove(aKey);
            if (error == KErrNotFound)
                return KErrNone;
            else
                return error;
            }
        HBufC8* newValue = NULL;
        
        TRAP(error, newValue = aValue.AllocL());
        
        if (!error)
            {
            error = iDescs.UpdateValue(&aKey,newValue);
            }
        return 
            error;
    }
const TDesC8* CWSStarSessionContext::GetDesC8L(const TDesC8& aKey)
    {
    TInt index = iDescs.Find(aKey);
    if (index != KErrNotFound)
        {
        return iDescs.ValueAt(index);
        }
    else
        {
        return NULL;
        };        
    }
TInt CWSStarSessionContext::Remove(const TDesC8& aKey)
    {
    return iDescs.RemoveByKey(aKey);
    }


//-----------------TInts
TInt CWSStarSessionContext::Add(const TDesC8& aKey, TInt aValue)
    {
    TInt* a = NULL;
    a = new TInt(aValue);
    if (a) 
        {
        return iInts.Append(&aKey, a);
        }
    else
        {
        return KErrNoMemory;
        }
        
    }
TInt CWSStarSessionContext::Update(const TDesC8& aKey, TInt aValue)
    {
    TInt* a = NULL;
    a = new TInt(aValue);
    if (a) 
        {
        return iInts.UpdateValue(&aKey, a);
        }
    else
        {
        return KErrNoMemory;
        }
    }
const TInt* CWSStarSessionContext::GetIntL(const TDesC8& aKey)
    {
    TInt index = iInts.Find(aKey);
    if (index!=KErrNotFound)
        {
        return iInts.ValueAt(index);
        }
    else
        {
        return NULL;
        }
    }

//-----------------elements
TInt CWSStarSessionContext::Add(const TDesC8& aKey, CSenElement* aValue)
    {
    return Update(aKey,aValue);
    }
TInt CWSStarSessionContext::Update(const TDesC8& aKey, CSenElement* aValue)
    {
    if (aKey == KBodyElement)
        {
        delete iBody;
        iBody = NULL;
        iBody = aValue;
        return KErrNone;
        }
    return KErrNotFound;
    }
const CSenElement* CWSStarSessionContext::GetSenElementL(const TDesC8& aKey)
    {
    if (aKey == KBodyElement)
        {
        return iBody;
        }
    return NULL;
    }

//-----------------TAnys
TInt CWSStarSessionContext::Add(const TDesC8& aKey, TAny* aValue)
    {
    TInt err(KErrNone);
    if (aKey == SenContext::KSenCurrentSoapMessageCtxKey())
        {
        if (!iCurrentEnvelope)
            {
            iCurrentEnvelope = (CSenSoapEnvelope*)aValue;
            }
        else
            {
            err = KErrAlreadyExists;
            }
        }
    else if ( aKey == SenContext::KRemoteConsumer() )
        {
        if ( !ipConsumer )
            {
            ipConsumer = aValue;
            }
         else
            {
            err = KErrAlreadyExists;
            }
        }
    else 
        {
        err = KErrNotFound;
        }        
    return err;
    }
TInt CWSStarSessionContext::Update(const TDesC8& aKey, TAny* aValue)
    {
    TInt err(KErrNone);
    if (aKey == SenContext::KSenCurrentSoapMessageCtxKey())
        {
        iCurrentEnvelope = (CSenSoapEnvelope*)aValue;
        }
    else if ( aKey == SenContext::KRemoteConsumer() )
        {
        ipConsumer = aValue;
        }
    else
        {
        err = KErrNotFound;
        }
    return err;
    }
TAny* CWSStarSessionContext::GetAnyL(const TDesC8& aKey)	//codescannerwarnings
    {
    if (aKey == SenContext::KSenCurrentSoapMessageCtxKey)
        {
        return iCurrentEnvelope;
        }
    else if ( aKey == WSStarContextKeys::KRemoteConsumer )
        {
        return ipConsumer;
        }
    else if(aKey == WSStarContextKeys::KSignedPartsArray)
        {
        return ipSignedParts;
        }
    else
        {
        return NULL;
        }
    }


//-----------------Rest
const CSenXmlReader* CWSStarSessionContext::GetParser()
    {
    return iParser;
    }
TInt CWSStarSessionContext::Count() const
    { 
    return 0;
    }
TPtrC8 CWSStarSessionContext::KeyAtL(TInt /*aIndex*/)
    { 
    return KNullDesC8();
    }        
void CWSStarSessionContext::Reset()
    {
    }
//-----------------------new    
void CWSStarSessionContext::UpdateDesC8L(CWSStarMessageContext& aMessageContext, const TDesC8& aKey)
    {
    const TDesC8* ptr =
            aMessageContext.GetDesC8L(aKey);
    if (ptr)
        {
        User::LeaveIfError(Update(aKey, *ptr));
        }
    else
        {
        Remove(aKey);
        }
    
    }
    
void CWSStarSessionContext::UpdateFromMessageInContextL(
        CWSStarMessageContext& aMessageContext)
    {
    const TDesC8* ptr =
            aMessageContext.GetDesC8L(KAction);
    if (ptr)
        {
        User::LeaveIfError(Update(KResponseAction, *ptr));
        }
    else
        {
        Remove(KResponseAction);
        }
    
    UpdateDesC8L(aMessageContext, KAddressingNsHolder);
    UpdateDesC8L(aMessageContext, KAddressingEndpointTagHolder);
    const TInt* ptrInt =
            aMessageContext.GetIntL(KReAuthNeeded);
    if (ptrInt)
        {
        User::LeaveIfError(Update(KReAuthNeeded, *ptrInt));
        }
    const TDesC8* ptrBrandID =
        aMessageContext.GetDesC8L(KMainBrandID);
    if (ptrBrandID)
        {
        User::LeaveIfError(Update(KMainBrandID, *ptrBrandID));
        }
    const TDesC8* ptrBrandIDList =
        aMessageContext.GetDesC8L(KBrandIDList);
    if (ptrBrandIDList)
        {
        User::LeaveIfError(Update(KBrandIDList, *ptrBrandIDList));
        }
    }

void CWSStarSessionContext::UpdateFromMessageOutContextL(
        CWSStarMessageContext& aMessageContext)
    {
    if (!aMessageContext.GetSenElementL(KBodyElement))
        {
        iBody = NULL;    
        }
    UpdateDesC8L(aMessageContext, KSTR);
    UpdateDesC8L(aMessageContext, KBody);
    UpdateDesC8L(aMessageContext, KMessageID);
    }

TInt CWSStarSessionContext::ShareTokenWithL(CWSStarSessionContext* aWSStarSessionCtxDst, TBool& aMTwasReplaceBySCT)
    {
    //first check SCT
    const TBool* isreplaced = NULL;
    const TDesC8* desc = NULL;
    isreplaced = GetIntL(KMTIsReplacedBySCT);
    if (isreplaced && *isreplaced == TRUE)
        {
        aMTwasReplaceBySCT = ETrue;
        }
    else
        {
        aMTwasReplaceBySCT = EFalse;
        }
    TInt error = KErrNone;
    desc = GetDesC8L(KTimestampExpires);
    if (desc)
        {
        error = aWSStarSessionCtxDst->Update(KTimestampExpires, *desc);
        if (error) return error;
        }
    desc = GetDesC8L(KTimestampCreated);
    if (desc)
        {
        error = aWSStarSessionCtxDst->Update(KTimestampCreated, *desc);    
        if (error) return error;
        }    
    desc = GetDesC8L(KPhoneTimeWhenMTResolved);
    if (desc)
        {
        error = aWSStarSessionCtxDst->Update(KPhoneTimeWhenMTResolved, *desc);    
        if (error) return error;
        }
    if (aMTwasReplaceBySCT)
        {
        CREDLOG_L(KSenCredsLogLevelNormal,"....this session has SCT so share it also");
        error = aWSStarSessionCtxDst->Update(KMTIsReplacedBySCT, aMTwasReplaceBySCT);    
        if (error) return error; 
        }
    desc = GetDesC8L(KSecurityToken);
    if (desc)//here we possible copy SCT, flag KMTIsReplacedBySCT determine
        {
        //some MS email "...1 SCT to access all WL for mobile services) but is not likely to remain that way..."
        //in future SCT will be not copied
        error = aWSStarSessionCtxDst->Update(KSecurityToken, *desc);    
        if (error) return error;
        }
    desc = GetDesC8L(KSecurityTokenBackup);
    if (desc)
        {
        error = aWSStarSessionCtxDst->Update(KSecurityTokenBackup, *desc);    
        if (error) return error;
        }
    //POP
    desc = GetDesC8L(KPOPBase64);
    if (desc)
        {
        error = aWSStarSessionCtxDst->Update(KPOPBase64, *desc);    
        if (error) return error;
        }  
    desc = GetDesC8L(KTokenType);
    if (desc)
        {
        error = aWSStarSessionCtxDst->Update(KTokenType, *desc);    
        if (error) return error;
        }  
    desc = GetDesC8L(KBinaryType);
    if (desc)
        {
        error = aWSStarSessionCtxDst->Update(KBinaryType, *desc);    
        if (error) return error;
        } 
    desc = GetDesC8L(KBinaryType);
    if (desc)
        {
        error = aWSStarSessionCtxDst->Update(KSTR, *desc);    
        if (error) return error;
        }                        
    return error;    
    }
// END OF FILE

