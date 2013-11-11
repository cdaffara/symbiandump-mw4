/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
#include "wsovimessagecontext.h"
#include "wsovisessioncontext.h"
#include "SenXmlReader.h"


CWSOviMessageContext* CWSOviMessageContext::NewL(SenContext::TContextDirection aDirection)
    {
    CWSOviMessageContext* self = NewLC(aDirection);
    CleanupStack::Pop(self);
    return self;
    }

CWSOviMessageContext* CWSOviMessageContext::NewLC(SenContext::TContextDirection aDirection)
    {
    CWSOviMessageContext* self = new (ELeave) CWSOviMessageContext(aDirection);
    CleanupStack::PushL (self);
    self->ConstructL();
    return self;
    }

CWSOviMessageContext* CWSOviMessageContext::NewLC(
        SenContext::TContextDirection aDirection,
        CWSOviSessionContext& aSessionContext)
    {
    CWSOviMessageContext* self =
            new (ELeave) CWSOviMessageContext(aDirection);
    CleanupStack::PushL (self);
    self->ConstructL(aSessionContext);
    return self;
    }

// Constructor
CWSOviMessageContext::CWSOviMessageContext(SenContext::TContextDirection aDirection)
    :iDirection(aDirection),
    iDescs(EFalse, ETrue),
    iOAuthProperties(EFalse, ETrue),
    iInts(EFalse, ETrue)
    {
    }

// Constructor
void CWSOviMessageContext::ConstructL()
    {
    }

void CWSOviMessageContext::ConstructL(CWSOviSessionContext& aSessionContext)
    {
    UpdateFromSessionContextL(aSessionContext);
    }

// Destructor
CWSOviMessageContext::~CWSOviMessageContext()
    {
    iDescs.Reset();
    iInts.Reset();
    iOAuthProperties.Reset();
    }
    
//from SessionContext 

CSenSoapMessage* CWSOviMessageContext::GetCurrentSoapMessage()
    {
    return NULL;
    };
TPtrC8 CWSOviMessageContext::GetMessageL()
    {
    return KNullDesC8();
    };
void CWSOviMessageContext::UpdateDesC8L(CWSOviSessionContext& aSessionContext, const TDesC8& aKey)
    {
    const TDesC8* ptr =
            aSessionContext.GetDesC8L(aKey);
    if (ptr && !GetDesC8L(aKey))
        {
        User::LeaveIfError(Update(aKey, *ptr));
        }
    }
void CWSOviMessageContext::UpdateIntL(CWSOviSessionContext& aSessionContext, const TDesC8& aKey)
    {
    const TInt* ptr =
            aSessionContext.GetIntL(aKey);
    if (ptr &&!GetIntL(aKey))
        {
        User::LeaveIfError(Update(aKey, *ptr));    
        } 
    }
    
void CWSOviMessageContext::UpdateFromSessionContextL(
        CWSOviSessionContext& aSessionContext)
    {
    //because of resending, we need keep message specific info within session ctx
    // (see ASYNC handleerror)
    if (Direction() == SenContext::EOutgoing)
        {
        UpdateDesC8L(aSessionContext, WSOviContextKeys::KToken);
        UpdateDesC8L(aSessionContext, WSOviContextKeys::KTokenSecret);
        UpdateIntL(aSessionContext, WSOviContextKeys::KRetryNeeded);
        }
    }

TInt CWSOviMessageContext::Add(const TDesC8& /*aKey*/, CSenSoapEnvelope* /*aValue*/)
    {
    return KErrNotSupported;
    };
TInt CWSOviMessageContext::Update(const TDesC8& /*aKey*/, CSenSoapEnvelope* /*aValue*/)
    {
    return KErrNotSupported;
    };
const CSenSoapEnvelope* CWSOviMessageContext::GetSenSoapEnvelopeL(const TDesC8& /*aKey*/)
    {
    return NULL;
    };



//from MContext       
SenContext::TContextType CWSOviMessageContext::Type() const
    {
    return SenContext::EMessageContext;
    }
    
SenContext::TContextDirection CWSOviMessageContext::Direction() const
    {
    return iDirection;
    }
const CSenXmlReader* CWSOviMessageContext::GetParser()
    {
    return NULL;
    }

//-----------------descs        
TInt CWSOviMessageContext::Add(const TDesC8& aKey, const TDesC8& aValue)
    {
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
TInt CWSOviMessageContext::Update(const TDesC8& aKey, const TDesC8& aValue)
    {
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

const TDesC8* CWSOviMessageContext::GetDesC8L(const TDesC8& aKey)
    {
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

TInt CWSOviMessageContext::Remove(const TDesC8& aKey)
    {
    return iDescs.RemoveByKey(aKey);
    }
//-------------Ints
TInt CWSOviMessageContext::Add(const TDesC8& aKey, TInt aValue)
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

TInt CWSOviMessageContext::Update(const TDesC8& aKey, TInt aValue)
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

const TInt* CWSOviMessageContext::GetIntL(const TDesC8& aKey)
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
TInt CWSOviMessageContext::Add(const TDesC8& aKey, CSenElement* aValue)
    {
    return Update(aKey,aValue);
    }

TInt CWSOviMessageContext::Update(const TDesC8& /*aKey*/, CSenElement* /*aValue*/)
    {
    return KErrNotSupported;
    }

const CSenElement* CWSOviMessageContext::GetSenElementL(const TDesC8& /*aKey*/)
    {
    return NULL;
    }
//-----------------TAnys
TInt CWSOviMessageContext::Add(const TDesC8& /*aKey*/, TAny* /*aValue*/)
    {
    return KErrNotSupported;
    }

TInt CWSOviMessageContext::Update(const TDesC8& /*aKey*/, TAny* /*aValue*/)
    {
    return KErrNotSupported;
    }

TAny* CWSOviMessageContext::GetAnyL(const TDesC8& /*aKey*/)	//codescannerwarnings
    {
    return NULL;
    }

TInt CWSOviMessageContext::Count() const
    {
    return KErrNotSupported;
    }

TPtrC8 CWSOviMessageContext::KeyAtL(TInt /*aIndex*/)
    {
    return KNullDesC8();
    }
        
void CWSOviMessageContext::Reset() 
    {
    }
    
MSenMessage* CWSOviMessageContext::MessageL()	//codescannerwarnigs
    {
    return NULL;
    }
    
TInt CWSOviMessageContext::SetMessage(MSenMessage* /* apMessage */, TBool /* aOwned */)
    {
    return KErrNotSupported;
    }
    
TInt CWSOviMessageContext::SetProperties(MSenProperties* /* apTransportProperties */, TBool /* aOwned */)
    {
    return KErrNotSupported;
    }
    
MSenProperties* CWSOviMessageContext::Properties()
    {
    return NULL;    
    }
    
TInt CWSOviMessageContext::Add(const TDesC8& /* aKey */, MSenMessage* /* apMessage */, TBool /* aOwned */)
    {
    return KErrNotSupported;
    }
    
TInt CWSOviMessageContext::Update(const TDesC8& /* aKey */, MSenMessage* /* apMessage */, TBool /* aOwned */)
    {
    return KErrNotSupported;
    }
TInt CWSOviMessageContext::UpdateTP(CSenTransportProperties*& aTP)
    {
    iTP = aTP;
    return KErrNone;
    }
RMapDescriptors& CWSOviMessageContext::OAuthProperies()
    {
    return iOAuthProperties;
    }
CSenTransportProperties* CWSOviMessageContext::TP()
    {
    return iTP;
    }   
MSenMessage* CWSOviMessageContext::GetMessageL(const TDesC8& /* aKey */)	// codescannerwarnings
    {
    return NULL;
    }
    
    
CSenChunk* CWSOviMessageContext::ChunkL()	//codescannerwarnings
    {
    return NULL;
    }
    
TInt CWSOviMessageContext::SetChunk(CSenChunk* /* apMsgAsChunk */, TBool /* aOwned */)
    {
    return KErrNotSupported;
    }
    
TInt CWSOviMessageContext::SetDirection( SenContext::TContextDirection /* aDirection */ ) 
    {
    return KErrNotSupported; 
    }
    
   
// END OF FILE

