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









// INCLUDE FILES
#include <SenXmlElement.h> // check if this include is needed(?)
#include <SenXmlUtils.h>

#include "wsstarrstresponse.h"
#include "SenDateUtils.h"
#include "senlogger.h"

using namespace WSStarTrustFault;

// Local CONSTANTS
const TInt KStateParsingRSTResponse  = 502; // ignore state   (even number)
const TInt KStateParsingPOP = 503;
const TInt KStateParsingLifeTime = 505;
const TInt KStateParsingEntropy = 507;
const TInt KStateParsingAppliesTo = 509;
const TInt KStateParsingAttachedReference = 510;//even becouse of parsing delegation
const TInt KStateParsingKeySize = 511;
const TInt KStateParsingSearchHeader = 512;
const TInt KStateParsingHeader = 514;
const TInt KStateParsingTokenType = 515;


CWSStarRSTResponse* CWSStarRSTResponse::NewL(TBool aParseIssuedTokens, const TDesC8& aAppliedTo, const TDesC8* aVersion)
    {
    CWSStarRSTResponse* pNew = NewLC(aParseIssuedTokens, aAppliedTo, aVersion);
    CleanupStack::Pop(pNew);
    return pNew;
    }
CWSStarRSTResponse* CWSStarRSTResponse::NewLC(TBool aParseIssuedTokens, const TDesC8& aAppliedTo, const TDesC8* aVersion)
    {
    CWSStarRSTResponse* pNew =
            new (ELeave) CWSStarRSTResponse(aParseIssuedTokens, aAppliedTo);
    CleanupStack::PushL(pNew);
    pNew->ConstructL(aVersion);
    return pNew;
    }

CWSStarRSTResponse::CWSStarRSTResponse(TBool aParseIssuedTokens, const TDesC8& aAppliedTo)
    {
    iContextToken = EFalse;
    iIsResponse = EFalse;
    iCollection = EFalse;
    iTokenEstablished = EFalse;
    iDefTokenEstablished = EFalse;
    iParseIssuedTokens = aParseIssuedTokens;
    iAppliedTo.Set(aAppliedTo);
    if (iParseIssuedTokens)
        {
        iState = KStateParsingSearchHeader;
        }
    }

void CWSStarRSTResponse::ConstructL(const TDesC8* aVersion)
    {
    iActiveXmlns = aVersion;
    //unlimited value
    iNotOnOrAfter = Time::MaxTTime();
    CSenBaseFragment::BaseConstructL(
        *aVersion,
        WSTrust::KRSTResponseTag
        );

    }

CWSStarRSTResponse::~CWSStarRSTResponse()
    {
    delete iCredential;
    delete iSTRFr;
    delete iSTR;
    delete iPOPencodedAsBase64;
    delete iTokenType;
    delete iTypeOfBinary;
    delete iEntropySTS;
    delete iComputedKey;
    delete iCredentialExpires;
    delete iCredentialCreated;
    delete iKeySize;
    delete iCredentialDef;
    delete iSTRDef;
    delete iPOPencodedAsBase64Def;
    delete iTokenTypeDef;
    delete iTypeOfBinaryDef;
    delete iEntropySTSDef;
    delete iComputedKeyDef;
    delete iCredentialExpiresDef;
    delete iCredentialCreatedDef;    
    delete iKeySizeDef;
    }

void CWSStarRSTResponse::StartElementL(
    const TDesC8& aNsUri,
    const TDesC8& aLocalName,
    const TDesC8& aQName,
    const RAttributeArray& aAttributes )
    {
    switch (iState)
        {
        case KStateIgnore:
            {    
            if ((aNsUri == *iActiveXmlns)&& (!iTokenEstablished))
                {
                iParsingApliedTo = EFalse;
                if  (aLocalName == WSTrust::KRSTResponseTag)
                    {
                    iState = KStateParsingRSTResponse;
                    iIsResponse = ETrue;
                    }
                else if(aLocalName == WSTrust::KRSTResponseCollectionTag)
                    {
                    iCollection = ETrue;
                    }
                }
            break;
            }
        case KStateParsingRSTResponse:
            {            
            if (aNsUri == *iActiveXmlns)
                {
                if (aLocalName == WSTrust::KRequestedSecurityTokenTag)
                    {
                    iState = KStateParsingSingleCredential;
                    iContextToken = EFalse;
                    }
                else if((aLocalName == WSTrust::KLifetimeTag)
                        ||
                        (aLocalName == WSTrust::KLifeTimeTag))
                    {
                    iState = KStateParsingLifeTime;
                    }
                else if(aLocalName == WSTrust::KProofTokenTag)
                    {
                    iState = KStateParsingPOP;
                    }            
                else if(aLocalName == WSTrust::KTokenTypeTag)
                    {
                    iState = KStateParsingTokenType;
                    }            
                else if(aLocalName == WSTrust::KEntropyTag)
                    {
                    iState = KStateParsingEntropy;
                    }            
                else if(aLocalName == WSTrust::KKeySize)
                    {
                    ResetContentL();
                    iState = KStateParsingKeySize;
                    }            
                else if(((aLocalName ==
                          WSTrust::KRequestedAttachedReferenceTag) &&
                         (aNsUri == WSTrust::KTrustNamespace200502)) ||
                        ((aLocalName ==
                          WSTrust::KRequestedTokenReferenceTag) &&
                         (aNsUri == WSTrust::KTrustNamespace200404)))
                    {
                    iState = KStateParsingAttachedReference;
                    }
                }
            else if(iPolicyVersion && aNsUri == *iPolicyVersion)
                {
                if (aLocalName == WSPolicy::KAppliesToTag)
                    {
                    iState = KStateParsingAppliesTo;
                    }
                }
                                
                
            break;
            }
        case KStateParsingSingleCredential:
            {
            if ((aNsUri == WSSecureConversation::KSecureConversationNamespace()
                || aNsUri == WSSecureConversation::KSecureConversationNamespaceMS())
                && (aLocalName == WSSecureConversation::KSecurityContextToken())
               )
                {
                iContextToken = ETrue;
                }
            CSenCredential* pCredential = CSenCredential::NewL(
               aNsUri, aLocalName, aQName, aAttributes);
            delete iCredential;
            iCredential = NULL;
            iCredential = pCredential;
            DelegateParsingL(*iCredential);
            //in case when credential parsed after lifetime
            //  if first credentail is parsed this assigment is reduntant, 
            //  but it costs less than checking time
            iCredential->DetachL();
            iCredential->SetValidUntil(iNotOnOrAfter);
            break;
            }
        case KStateParsingAttachedReference:
            {
            //parsing STR
            delete iSTRFr;
            iSTRFr = NULL;
            iSTRFr = CSenBaseFragment::NewL(aNsUri, aLocalName, aQName,
                    aAttributes);
            DelegateParsingL(*iSTRFr);
            break;
            }
        case KStateParsingPOP:
            {
            // We clean the buffer so that we can save the element content
            // At endElement we will know what member of the resource offering
            // to set.
             if (aNsUri == *iActiveXmlns && aLocalName == WSTrust::KBinarySecretTag)
                {
                delete iTypeOfBinary;
                iTypeOfBinary = NULL;
                iTypeOfBinary = SenXmlUtils::AttrValue(aAttributes, WSTrust::KBinaryTypeAttr).AllocL();
                ResetContentL();
                }
             else if (aNsUri == *iActiveXmlns && aLocalName == WSTrust::KComputedKeyTag)
                {
                ResetContentL();
                }            
                break;
            }
        case KStateParsingEntropy:
            {
            if (aNsUri == *iActiveXmlns && aLocalName == WSTrust::KBinarySecretTag)
                {
                delete iTypeOfBinary;
                iTypeOfBinary = NULL;
                iTypeOfBinary = SenXmlUtils::AttrValue(aAttributes, WSTrust::KBinaryTypeAttr).AllocL();
                ResetContentL();
                }
            break;
            }
        case KStateParsingTokenType:
            {
            ResetContentL();
            }            
            break;
        case KStateParsingLifeTime:
            {
            if (aNsUri == WSTrust::KWsuNamespace && 
                ((aLocalName == WSTrust::KExpiresTag) || (aLocalName == WSTrust::KCreatedTag)))
                {
                ResetContentL();
                }
            break;
            }
        case KStateParsingSearchHeader:
            {
            if (((aNsUri == KSenSoapEnvelopeXmlns) || (aNsUri == KSenSoap12EnvelopeXmlns)) &&
                (aLocalName == KSenSoapEnvelopeHeaderName))
                {
                iState = KStateParsingHeader;
                }
            break;
            }
        case KStateParsingHeader:
            {
            if ((aNsUri == *iActiveXmlns) &&
                (aLocalName == WSTrust::KIssuedTokens))
                {
                iState = KStateIgnore;
                }
            else
                {
                iState = KStateParsingSearchHeader;
                }
            break;
            }
        case KStateParsingAppliesTo:
            {
            if (iAddressingNs 
                && iAddressingEndpointTag
                && aNsUri == *iAddressingNs
                && aLocalName == *iAddressingEndpointTag)
                {
                ResetContentL();
                }
            break;
            }                            
        default:
            break;
        }
    }

void CWSStarRSTResponse::EndElementL(
    const TDesC8& aNsUri,
    const TDesC8& aLocalName,
    const TDesC8& /*aQName*/
    )
    {
     switch (iState)
        {
        case KStateIgnore:
            {
            break;
            }
        case KStateParsingRSTResponse:
            {
            if (aNsUri == *iActiveXmlns && aLocalName == WSTrust::KRSTResponseTag)
                {
                iState = KStateIgnore;
                if (iParsingApliedTo)
                    {
                    iTokenEstablished = ETrue;
                    iDefTokenEstablished = EFalse;
                    }
                else if(!iDefTokenEstablished)
                    {
                    iDefTokenEstablished = ETrue;
                    iCredentialDef = iCredential;
                    iCredential = NULL;
                    iKeySizeDef = iKeySize;
                    iKeySize = NULL;
                    iSTRDef = iSTR;
                    iSTR = NULL;
                    iPOPencodedAsBase64Def = iPOPencodedAsBase64;
                    iPOPencodedAsBase64 = NULL;
                    iTokenTypeDef = iTokenType;
                    iTokenType = NULL;
                    iTypeOfBinaryDef = iTypeOfBinary;
                    iTypeOfBinary = NULL;
                    iEntropySTSDef = iEntropySTS;
                    iEntropySTS = NULL;
                    iComputedKeyDef = iComputedKey;
                    iComputedKey = NULL;
                    iCredentialExpiresDef = iCredentialExpires;
                    iCredentialExpires = NULL;
                    iCredentialCreatedDef = iCredentialCreated;    
                    iCredentialCreated = NULL;
                    }
                }
            break;
            }     
        case KStateParsingSingleCredential:
            {
            if (aNsUri == *iActiveXmlns && aLocalName == WSTrust::KRequestedSecurityTokenTag)
                {
                iState = KStateParsingRSTResponse;
                }
            break;
            }
        case KStateParsingAttachedReference:
            {
            delete iSTR;
            iSTR = NULL;
            if (iSTRFr->LocalName() == KSecurityTokenReference)
                {
                iSTRFr->DetachL();
                iSTR = iSTRFr->AsXmlL();
                }
            else
                {
                TLSLOG(KSenFaultsLogChannel, KSenFaultsLogLevel, KBadSCT());
                }
            delete iSTRFr;
            iSTRFr = NULL;
            iState = KStateParsingRSTResponse;
            break;
            }
        case KStateParsingTokenType:
                {
                if (aNsUri == *iActiveXmlns && aLocalName == WSTrust::KTokenType)
                    {
                    delete iTokenType;
                    iTokenType = NULL;
                    iTokenType = Content().AllocL();
                    }
                iState = KStateParsingRSTResponse;
                break;   
                }
        case KStateParsingPOP:
            {
            if (aNsUri == *iActiveXmlns && aLocalName == WSTrust::KBinarySecretTag)
                {
                iState = KStateParsingRSTResponse;
                delete iPOPencodedAsBase64;
                iPOPencodedAsBase64 = NULL;
                iPOPencodedAsBase64 = Content().AllocL();
                }
            else if (aNsUri == *iActiveXmlns && aLocalName == WSTrust::KComputedKeyTag)
                {
                iState = KStateParsingRSTResponse;
                delete iComputedKey;
                iComputedKey = NULL;
                iComputedKey = Content().AllocL();
                }
            break;
            }
        case KStateParsingEntropy:
            {
            if (aNsUri == *iActiveXmlns && aLocalName == WSTrust::KBinarySecretTag)
                {
                iState = KStateParsingRSTResponse;
                delete iEntropySTS;
                iEntropySTS = NULL;
                iEntropySTS = Content().AllocL();
                }
            break;
            }
        case KStateParsingKeySize:
            {
            iState = KStateParsingRSTResponse;
            delete iKeySize;
            iKeySize = NULL;
            iKeySize = new (ELeave) TInt(0);
            TLex8 lex(Content());
            if (lex.Val(*iKeySize) != KErrNone)
                {
                delete iKeySize;
                iKeySize = NULL;
                };
            break;
            }
        case KStateParsingLifeTime:
            {
            if (aNsUri == WSTrust::KWsuNamespace() && aLocalName == WSTrust::KExpiresTag)
                {
                delete iCredentialExpires;
                iCredentialExpires = NULL;
                iCredentialExpires = Content().AllocL();
                iNotOnOrAfter =  SenDateUtils::FromXmlDateTimeL(*iCredentialExpires);
                iState = KStateParsingRSTResponse;
                //in case if credential has been already parsed
                if (iCredential)
                    {
                    iCredential->SetValidUntil(iNotOnOrAfter);
                    }
                }
            else if(aNsUri == WSTrust::KWsuNamespace() && aLocalName == WSTrust::KCreatedTag)
                {
                delete iCredentialCreated;
                iCredentialCreated = NULL;
                iCredentialCreated = Content().AllocL();
                //check if only created tag, what hapeens with states & parsing
                }
            break;
            }
        case KStateParsingAppliesTo:            
            {
            if(aNsUri == *iPolicyVersion && aLocalName == WSPolicy::KAppliesToTag)
                {
                iState = KStateParsingRSTResponse;
                }
            else if (iAddressingNs 
                && iAddressingEndpointTag
                && aNsUri == *iAddressingNs
                && aLocalName == *iAddressingEndpointTag)
                {
                //if no collection, we take anyway, we will try
                if ((iAppliedTo != Content()) && (iAppliedTo != KNullDesC8)) 
                    {
                    Reset();
                    iState = KStateIgnore;
                    }
                else
                    {
                    iParsingApliedTo = ETrue;
                    }
                }
            break;
            }
        default:
            break;
        }
    }

CSenCredential* CWSStarRSTResponse::Credential()
    {
    return iDefTokenEstablished ? iCredentialDef : iCredential;
    }

TPtrC8 CWSStarRSTResponse::POPBase64()
    {
    HBufC8* POPencodedAsBase64 = iDefTokenEstablished ? iPOPencodedAsBase64Def :
            iPOPencodedAsBase64;
    if (POPencodedAsBase64)
        {
        return *POPencodedAsBase64;    
        }
    else 
        {
        return KNullDesC8();
        }
    }
TPtrC8 CWSStarRSTResponse::TokenType()
    {
    HBufC8* tokenType = iDefTokenEstablished ? iTokenTypeDef :
            iTokenType;
    if (tokenType)
        {
        return *tokenType;    
        }
    else 
        {
        return KNullDesC8();
        }
    }
TPtrC8 CWSStarRSTResponse::BinaryType()
    {
    HBufC8* typeOfBinary = iDefTokenEstablished ? iTypeOfBinaryDef :
            iTypeOfBinary;
    if (typeOfBinary && typeOfBinary->Length())
        {
        return *typeOfBinary;    
        }
    else
        {
        return WSTrust::KBinaryTypeSimmetricValue(); 
        }
       
    }
TPtrC8 CWSStarRSTResponse::EntropySTS()
    {
    HBufC8* entropySTS = iDefTokenEstablished ? iEntropySTSDef : iEntropySTS;
    if (entropySTS)
        {
        return *entropySTS;    
        }
    else
        {
        return KNullDesC8();
        }
    }
TPtrC8 CWSStarRSTResponse::ComputedKey()
    {
    HBufC8* computedKey = iDefTokenEstablished ? iComputedKeyDef : iComputedKey;
    if (computedKey)
        {
        return *computedKey;    
        }
    else
        {
        return KNullDesC8();
        }
    }
TInt CWSStarRSTResponse::KeySize()
    {
    TInt* keySize = iKeySizeDef ? iKeySizeDef : iKeySize;
    if (keySize)
        {
        return *keySize;    
        }
    else
        {
        return 0;
        }
    }
TPtrC8 CWSStarRSTResponse::CredentialExpires()
    {
    HBufC8* credentialExpires = iDefTokenEstablished ? iCredentialExpiresDef :
            iCredentialExpires;
    if (credentialExpires)
        {
        return *credentialExpires;    
        }
    else
        {
        return KNullDesC8();
        }
    }
TPtrC8 CWSStarRSTResponse::CredentialCreated()
    {
    HBufC8* credentialCreated = iDefTokenEstablished ? iCredentialCreatedDef :
            iCredentialCreated;
    if (credentialCreated)
        {
        return *credentialCreated;    
        }
    else
        {
        return KNullDesC8(); 
        }
    }
TPtrC8 CWSStarRSTResponse::STR()
    {
    HBufC8* str = iDefTokenEstablished ? iSTRDef : iSTR;
    return str ? *str : KNullDesC8();
    }
TBool CWSStarRSTResponse::IsContextToken()
    {
    return iContextToken;
    }

TBool CWSStarRSTResponse::IsResponse()
    {
    return iIsResponse;
    }

void CWSStarRSTResponse::UpdateAddressingHoldersL(MSenSessionContext& aCtx)
    {
    //priority
    //  1)version declared by facet in SD scope (we get value by KAddressingVersion)
    //  2)version of addressing handler (we get value by KAddressinNsHolder)
    iAddressingNs = aCtx.GetDesC8L(WSStarContextKeys::KAddressingVersion);    
    if (!iAddressingNs)
        {
        iAddressingNs = aCtx.GetDesC8L(WSStarContextKeys::KAddressingNsHolder);    
        }
    iAddressingEndpointTag = aCtx.GetDesC8L(WSStarContextKeys::KAddressingEndpointTagHolder);
    }
void CWSStarRSTResponse::Reset()    
    {
    delete iCredential;
    iCredential = NULL;
    
    delete iSTR;
    iSTR = NULL;
    
    delete iPOPencodedAsBase64;
    iPOPencodedAsBase64 = NULL;
    
    delete iTokenType;
    iTokenType = NULL;
    
    delete iTypeOfBinary;
    iTypeOfBinary = NULL;
    
    delete iEntropySTS;
    iEntropySTS = NULL;
    
    delete iComputedKey;
    iComputedKey = NULL;
    
    delete iCredentialExpires;
    iCredentialExpires = NULL;
    
    delete iCredentialCreated;    
    iCredentialCreated = NULL;
    }
void CWSStarRSTResponse::SetPolicyVersion(const TDesC8* aVersion)
    {
    iPolicyVersion = aVersion;
    }    
// End of File

