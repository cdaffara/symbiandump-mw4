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











#include "SenSoapEnvelope.h"
#include "wsstarmessagecontext.h"
#include "wsstarsessioncontext.h"
#include "SenXmlReader.h"

#include "senlogger.h"


CWSStarMessageContext* CWSStarMessageContext::NewL(SenContext::TContextDirection aDirection, CSenXmlReader*& aParser)
    {
    CWSStarMessageContext* self = NewLC(aDirection, aParser);
    CleanupStack::Pop(self);
    return self;
    }

CWSStarMessageContext* CWSStarMessageContext::NewLC(SenContext::TContextDirection aDirection, CSenXmlReader*& aParser)
    {
    CWSStarMessageContext* self = new (ELeave) CWSStarMessageContext(aDirection, aParser);
    CleanupStack::PushL (self);
    self->ConstructL();
    return self;
    }

CWSStarMessageContext* CWSStarMessageContext::NewLC(
        SenContext::TContextDirection aDirection, CSenXmlReader*& aParser,
        CWSStarSessionContext& aSessionContext)
    {
    CWSStarMessageContext* self =
            new (ELeave) CWSStarMessageContext(aDirection, aParser);
    CleanupStack::PushL (self);
    self->ConstructL(aSessionContext);
    return self;
    }

// Constructor
CWSStarMessageContext::CWSStarMessageContext(SenContext::TContextDirection aDirection, CSenXmlReader*& aParser)
    :iDirection(aDirection),
    iOwnedEnvelope(EFalse),
    iParser(aParser),
    iDescs(EFalse, ETrue),
    iInts(EFalse, ETrue)
    {
    }

// Constructor
void CWSStarMessageContext::ConstructL()
    {
    }

void CWSStarMessageContext::ConstructL(CWSStarSessionContext& aSessionContext)
    {
    UpdateFromSessionContextL(aSessionContext);
    }

// Destructor
CWSStarMessageContext::~CWSStarMessageContext()
    {
    if (iOwnedEnvelope)
        {
        delete iCurrentEnvelope;
        }
    iDescs.Reset();
    iInts.Reset();
    }
    
//from SessionContext 

CSenSoapMessage* CWSStarMessageContext::GetCurrentSoapMessage()
    {
    if (iCurrentEnvelope)
        {
        return (CSenSoapMessage*)iCurrentEnvelope;
        }
    else
        {
        return NULL;
        }
};
TPtrC8 CWSStarMessageContext::GetMessageL()
    {
    return iCurrentEnvelopeTxt;
    };
void CWSStarMessageContext::UpdateDesC8L(CWSStarSessionContext& aSessionContext, const TDesC8& aKey)
    {
    const TDesC8* ptr =
            aSessionContext.GetDesC8L(aKey);
    if (ptr && !GetDesC8L(aKey))
        {
        User::LeaveIfError(Update(aKey, *ptr));
        }
    }
void CWSStarMessageContext::UpdateIntL(CWSStarSessionContext& aSessionContext, const TDesC8& aKey)
    {
    const TInt* ptr =
            aSessionContext.GetIntL(aKey);
    if (ptr &&!GetIntL(aKey))
        {
        User::LeaveIfError(Update(aKey, *ptr));    
        } 
    }
    
void CWSStarMessageContext::UpdateFromSessionContextL( CWSStarSessionContext& aSessionContext )
    {
    //becouse of resending we need keep veen message specific stuff inside session ctx
    // (see ASYNC handkleerror)
    if (Direction() == SenContext::EOutgoing)
        {
        const TDesC8* ptr =
        aSessionContext.GetDesC8L( WSStarContextKeys::KTo );
        if ( ptr )
            {
            TInt error = Remove( WSStarContextKeys::KTo );
            if( error )
                {
                TLSLOG_L( KSenCoreServiceManagerLogChannelBase, KMinLogLevel, "CWSStarMessageContext::UpdateFromSessionContextL( CWSStarSessionContext& aSessionContext ):" );
                TLSLOG_FORMAT(( KSenCoreServiceManagerLogChannelBase, KMinLogLevel, _L8("- Remove( WSStarContextKeys::KTo ) returned an error(!): %d => going to 'throw a LEAVE'"), error ));
                User::LeaveIfError( error );
                }
            }        
        UpdateDesC8L(aSessionContext, WSStarContextKeys::KTo);
        UpdateDesC8L(aSessionContext, WSStarContextKeys::KAction);
        UpdateDesC8L(aSessionContext, WSStarContextKeys::KAddressingVersion);
        UpdateDesC8L(aSessionContext, WSStarContextKeys::KTimestampExpires);
        UpdateDesC8L(aSessionContext, WSStarContextKeys::KTimestampCreated);
        UpdateDesC8L(aSessionContext, WSStarContextKeys::KPhoneTimeWhenMTResolved);
        UpdateDesC8L(aSessionContext, WSStarContextKeys::KPOPBase64);
        UpdateDesC8L(aSessionContext, WSStarContextKeys::KBinaryType);
        UpdateIntL(aSessionContext, WSStarContextKeys::KSoapVersion);
        UpdateIntL(aSessionContext, WSStarContextKeys::KPassportEnabled);
        UpdateIntL(aSessionContext, WSStarContextKeys::KMTIsReplacedBySCT);
        UpdateDesC8L(aSessionContext, WSStarContextKeys::KSecurityToken);
        UpdateDesC8L(aSessionContext, WSStarContextKeys::KBody);
        iBody = aSessionContext.GetSenElementL(WSStarContextKeys::KBodyElement);
        UpdateDesC8L(aSessionContext, WSStarContextKeys::KSTR);
        ipSignedParts = (RPointerArray<HBufC8>*)aSessionContext.GetAnyL( WSStarContextKeys::KSignedPartsArray );
              
#ifdef _SENDEBUG                
        if( !ipSignedParts ) // ipSignedParts == NULL!
            {
            TLSLOG_L( KSenCoreServiceManagerLogChannelBase, KMinLogLevel, "CWSStarMessageContext::UpdateFromSessionContextL( CWSStarSessionContext& aSessionContext ):" );
            TLSLOG_L( KSenCoreServiceManagerLogChannelBase, KMinLogLevel, "- aSessionContext.GetAnyL( WSStarContextKeys::KSignedPartsArray ) returned NULL(!)" );
            }
#endif // _SENDEBUG            
        
        UpdateIntL(aSessionContext, WSStarContextKeys::KAppInfoPresent);
        UpdateDesC8L(aSessionContext, WSStarContextKeys::KApplicationName);
        UpdateDesC8L(aSessionContext, WSStarContextKeys::KApplicationVendor);
        UpdateDesC8L(aSessionContext, WSStarContextKeys::KApplicationVersion);
        UpdateDesC8L(aSessionContext, WSStarContextKeys::KLocale);
        UpdateDesC8L(aSessionContext, WSStarContextKeys::KManufacturer);
        UpdateDesC8L(aSessionContext, WSStarContextKeys::KModel);
        UpdateDesC8L(aSessionContext, WSStarContextKeys::KDeviceName);
        UpdateDesC8L(aSessionContext, WSStarContextKeys::KDeviceVersion);
        UpdateDesC8L(aSessionContext, WSStarContextKeys::KIMSI);
        }
    else
        {
        UpdateIntL(aSessionContext, WSStarContextKeys::KPassportEnabled);
        }
    }

TInt CWSStarMessageContext::Add(const TDesC8& aKey, CSenSoapEnvelope* aValue)
    {
    TInt results = KErrNone;
    if (aKey == SenContext::KSenCurrentSoapMessageCtxKey())
        {
        if (!iCurrentEnvelope)
            {
            iCurrentEnvelope = aValue;
            }
        else
            {
            results = KErrAlreadyExists;
            }
        
        }
    return results;
    };
TInt CWSStarMessageContext::Update(const TDesC8& aKey, CSenSoapEnvelope* aValue)
    {
    if (aKey == SenContext::KSenCurrentSoapMessageCtxKey())
        {
        if (iOwnedEnvelope)
            {
            delete iCurrentEnvelope;
            }
        iCurrentEnvelope = aValue;
        }
    return KErrNone;
    };
const CSenSoapEnvelope* CWSStarMessageContext::GetSenSoapEnvelopeL(const TDesC8& aKey)
    {
    if (aKey == SenContext::KSenCurrentSoapMessageCtxKey)
        {
        return iCurrentEnvelope;
        }
    else
        {
        return NULL;
        }
    };



//from MContext       
SenContext::TContextType CWSStarMessageContext::Type() const
    {
    return SenContext::EMessageContext;
    }
    
SenContext::TContextDirection CWSStarMessageContext::Direction() const
    {
    return iDirection;
    }
//-----------------descs        
TInt CWSStarMessageContext::Add(const TDesC8& aKey, const TDesC8& aValue)
    {
    if (aKey == SenContext::KSenCurrentSoapMessageCtxKey())
        {
        if (! iCurrentEnvelopeTxt.Length())
            {
            iCurrentEnvelopeTxt.Set(aValue);
            return KErrNone;
            }
        else
            {
            return KErrAlreadyExists;
            }
        }
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
        return error;
    }

//it add(if not exist) or update. So , it is like Set
TInt CWSStarMessageContext::Update(const TDesC8& aKey, const TDesC8& aValue)
    {
    if (aKey == SenContext::KSenCurrentSoapMessageCtxKey())
        {
        iCurrentEnvelopeTxt.Set(aValue);
        return KErrNone;
        }
    
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
    return error;
    }

const TDesC8* CWSStarMessageContext::GetDesC8L(const TDesC8& aKey)
    {
    if (aKey == SenContext::KSenCurrentSoapMessageCtxKey())
        {
        return NULL;
        }
    TInt index = iDescs.Find(aKey);
    if (index != KErrNotFound)
        {
        return iDescs.ValueAt(index);
        }
    else
        {
        return NULL;
        }
    }

TInt CWSStarMessageContext::Remove(const TDesC8& aKey)
    {
    if (aKey == WSStarContextKeys::KBodyElement)
        {
        iBody = NULL;
        return KErrNone;
        }
    else
        {
        return iDescs.RemoveByKey(aKey);
        }
    }
//-------------Ints
TInt CWSStarMessageContext::Add(const TDesC8& aKey, TInt aValue)
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

TInt CWSStarMessageContext::Update(const TDesC8& aKey, TInt aValue)
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

const TInt* CWSStarMessageContext::GetIntL(const TDesC8& aKey)
    {
    TInt index = iInts.Find(aKey);
    if (index!=KErrNotFound)
        {
        return iInts.ValueAt(index);
        }
    else
        {
        return NULL;
        };        
    }

//-----------------elements
TInt CWSStarMessageContext::Add(const TDesC8& aKey, CSenElement* aValue)
    {
    return Update(aKey,aValue);
    }

TInt CWSStarMessageContext::Update(const TDesC8& aKey, CSenElement* aValue)
    {
    if (aKey == WSStarContextKeys::KBodyElement)
        {
        iBody = aValue;
        return KErrNone;
        }
    return KErrNotFound;
    }

const CSenElement* CWSStarMessageContext::GetSenElementL(const TDesC8& aKey)
    {
    if (aKey == WSStarContextKeys::KBodyElement)
        {
        return iBody;
        }
    return NULL;
    }
//-----------------TAnys
TInt CWSStarMessageContext::Add(const TDesC8& /*aKey*/, TAny* /*aValue*/)
    {
    return KErrNotSupported;
    }

TInt CWSStarMessageContext::Update(const TDesC8& /*aKey*/, TAny* /*aValue*/)
    {
    return KErrNotSupported;
    }

TAny* CWSStarMessageContext::GetAnyL(const TDesC8& aKey)	//codescannerwarnings
    {
    if(aKey == WSStarContextKeys::KSignedPartsArray)
        {
        return ipSignedParts;
        }
     else
        {
        return NULL;
        }
    }

const CSenXmlReader* CWSStarMessageContext::GetParser()
    {
    return iParser;
    }

TInt CWSStarMessageContext::Count() const
    {
    return KErrNotSupported;
    }

TPtrC8 CWSStarMessageContext::KeyAtL(TInt /*aIndex*/)
    {
    return KNullDesC8();
    }
        
void CWSStarMessageContext::Reset() 
    {
    }
    
MSenMessage* CWSStarMessageContext::MessageL()	//codescannerwarnigs
    {
    return NULL;
    }
    
TInt CWSStarMessageContext::SetMessage(MSenMessage* /* apMessage */, TBool /* aOwned */)
    {
    return KErrNotSupported;
    }
    
TInt CWSStarMessageContext::SetProperties(MSenProperties* /* apTransportProperties */, TBool /* aOwned */)
    {
    return KErrNotSupported;
    }
    
MSenProperties* CWSStarMessageContext::Properties()
    {
    return NULL;    
    }
    
TInt CWSStarMessageContext::Add(const TDesC8& /* aKey */, MSenMessage* /* apMessage */, TBool /* aOwned */)
    {
    return KErrNotSupported;
    }
    
TInt CWSStarMessageContext::Update(const TDesC8& /* aKey */, MSenMessage* /* apMessage */, TBool /* aOwned */)
    {
    return KErrNotSupported;
    }
    
MSenMessage* CWSStarMessageContext::GetMessageL(const TDesC8& /* aKey */)	// codescannerwarnings
    {
    return NULL;
    }
    
    
CSenChunk* CWSStarMessageContext::ChunkL()	//codescannerwarnings
    {
    return NULL;
    }
    
TInt CWSStarMessageContext::SetChunk(CSenChunk* /* apMsgAsChunk */, TBool /* aOwned */)
    {
    return KErrNotSupported;
    }
    
TInt CWSStarMessageContext::SetDirection( SenContext::TContextDirection /* aDirection */ ) 
    {
    return KErrNotSupported; 
    }
    
   
// END OF FILE

