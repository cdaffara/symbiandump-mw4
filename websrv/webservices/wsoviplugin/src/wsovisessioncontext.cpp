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








#include "wsovisessioncontext.h"
#include "senwsdescription.h"
#include <SenServiceConnection.h>
#include <SenCredential.h>
#include <SenDateUtils.h>
#include "senlogger.h"
#include "wsoviservicesession.h"
#include "msenremoteservicesession.h"  

using namespace WSOviContextKeys;
using namespace WSOviContextValues;

CWSOviSessionContext* CWSOviSessionContext::NewL()
    {
    CWSOviSessionContext* self = NewLC();
    CleanupStack::Pop(self);
    return self;
    }
CWSOviSessionContext* CWSOviSessionContext::NewLC()
    {
    CWSOviSessionContext* self = new (ELeave) CWSOviSessionContext();
    CleanupStack::PushL (self);
    self->ConstructL();
    return self;
    }

// Constructor
CWSOviSessionContext::CWSOviSessionContext():
    iDescriptions(EFalse,EFalse),
    iInts(EFalse,ETrue),
    iDescs(EFalse,ETrue)
    {
    }

// Constructor
void CWSOviSessionContext::ConstructL()
    {
    }

// Destructor
CWSOviSessionContext::~CWSOviSessionContext()
    {
    iDescriptions.Reset();
    iInts.Reset();
    iDescs.Reset();
    }
// -----------------------------------------------------------------------------
// Sets token context keys based on RST response.
// -----------------------------------------------------------------------------
//
TInt CWSOviSessionContext::SetTokenKeysL(const TDesC8& aToken)
    {

//1)TOKEN
    CWSOviServiceSession* session = (CWSOviServiceSession*)
                                            GetSenRemoteServiceSessionL(KServiceSession);
    session->SetMaxValidUntil();
    ((CSenWebServiceSession*)session)->SetSecurityL(aToken);
    session->AddPropertiesFromSessionContextToCredentialL();
    session->SetStatusL();
    if (((CSenServiceSession*)session)->StatusL() == KSenConnectionStatusExpired)
        {
        return KErrNotReady;
        }
    User::LeaveIfError(Update(WSOviContextKeys::KToken,
           aToken));
    return KErrNone;
    }

//-----------------------------------------------------
//              from MSessionContext
//-----------------------------------------------------
const CSenWSDescription* CWSOviSessionContext::GetSession()
    {
    return NULL;
    };
const CSenWSDescription* CWSOviSessionContext::GetInitializer()
    {
    return NULL;
    };

//descriptions    
TInt CWSOviSessionContext::Add(const TDesC8& aKey, CSenWSDescription* aValue)
    {
    return iDescriptions.Append(&aKey,aValue);
    };

TInt CWSOviSessionContext::Update(const TDesC8& aKey, CSenWSDescription* aValue)
    {
    iDescriptions.RemoveByKey(aKey);
    return iDescriptions.Append(&aKey,aValue);
    };

const CSenWSDescription* CWSOviSessionContext::GetSenWSDescriptionL(const TDesC8& aKey)
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
TInt CWSOviSessionContext::Add(const TDesC8& aKey, MSenRemoteServiceSession& aValue)
    {
    return Update(aKey, aValue);
    };
TInt CWSOviSessionContext::Update(const TDesC8& /*aKey*/, MSenRemoteServiceSession& aValue)
    {
    iSession = &aValue;
    return KErrNone;
    };
const MSenRemoteServiceSession* CWSOviSessionContext::GetSenRemoteServiceSessionL(const TDesC8& /*aKey*/)
    {
    return iSession;
    };    



//-----------------------------------------------------
//              from MContext
//-----------------------------------------------------

SenContext::TContextType CWSOviSessionContext::Type() const  
    {
    return SenContext::ESessionContext;
    }

SenContext::TContextDirection CWSOviSessionContext::Direction() const 
    {
    return SenContext::EBoth;
    }



//-----------------descs
TInt CWSOviSessionContext::Add(const TDesC8& aKey, const TDesC8& aValue)
    {   
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
TInt CWSOviSessionContext::Update(const TDesC8& aKey, const TDesC8& aValue)
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
        return 
            error;
    }
const TDesC8* CWSOviSessionContext::GetDesC8L(const TDesC8& aKey)
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
TInt CWSOviSessionContext::Remove(const TDesC8& aKey)
    {
    return iDescs.RemoveByKey(aKey);
    }


//-----------------TInts
TInt CWSOviSessionContext::Add(const TDesC8& aKey, TInt aValue)
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
TInt CWSOviSessionContext::Update(const TDesC8& aKey, TInt aValue)
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
const TInt* CWSOviSessionContext::GetIntL(const TDesC8& aKey)
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
TInt CWSOviSessionContext::Add(const TDesC8& aKey, CSenElement* aValue)
    {
    return Update(aKey,aValue);
    }
TInt CWSOviSessionContext::Update(const TDesC8& /*aKey*/, CSenElement* /*aValue*/)
    {
    return KErrNotSupported;
    }
const CSenElement* CWSOviSessionContext::GetSenElementL(const TDesC8& /*aKey*/)
    {
    return NULL;
    }

//-----------------TAnys
TInt CWSOviSessionContext::Add(const TDesC8& aKey, TAny* aValue)
    {
    TInt err(KErrNone);
    if ( aKey == SenContext::KRemoteConsumer() )
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
TInt CWSOviSessionContext::Update(const TDesC8& aKey, TAny* aValue)
    {
    TInt err(KErrNone);
    if ( aKey == SenContext::KRemoteConsumer() )
        {
        ipConsumer = aValue;
        }
    else if ( aKey == WSOviContextKeys::KTP() )
        {
        iTP = aValue;
        }
    else
        {
        err = KErrNotFound;
        }
    return err;
    }
TAny* CWSOviSessionContext::GetAnyL(const TDesC8& aKey)
    {
    if ( aKey == WSOviContextKeys::KRemoteConsumer )
        {
        return ipConsumer;
        }
    else if ( aKey == WSOviContextKeys::KTP() )
        {
        return iTP;
        }
    else
        {
        return NULL;
        }
    }


//-----------------Rest
const CSenXmlReader* CWSOviSessionContext::GetParser()
    {
    return NULL;
    }
TInt CWSOviSessionContext::Count() const
    { 
    return 0;
    }
TPtrC8 CWSOviSessionContext::KeyAtL(TInt /*aIndex*/)
    { 
    return KNullDesC8();
    }        
void CWSOviSessionContext::Reset()
    {
    }
//-----------------------new    
void CWSOviSessionContext::UpdateDesC8L(CWSOviMessageContext& aMessageContext, const TDesC8& aKey)
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
    
void CWSOviSessionContext::UpdateFromMessageInContextL(
        CWSOviMessageContext& /*aMessageContext*/)
    {
    
    }

void CWSOviSessionContext::UpdateFromMessageOutContextL(
        CWSOviMessageContext& /*aMessageContext*/)
    {
    }
TInt CWSOviSessionContext::ShareTokenWithL(CWSOviSessionContext* aWSOviSessionCtxDst)
    {
    TInt error = KErrNone;
    const TDesC8* desc = NULL;

    desc = GetDesC8L(KTokenCreationTime);
    if (desc)
        {
        error = aWSOviSessionCtxDst->Update(KTokenCreationTime, *desc);
        if (error) return error;
        }
    desc = GetDesC8L(KTokenValidUntilTime);
    if (desc)
        {
        error = aWSOviSessionCtxDst->Update(KTokenValidUntilTime, *desc);
        if (error) return error;
        }
    desc = GetDesC8L(KTTL);
    if (desc)
        {
        error = aWSOviSessionCtxDst->Update(KTTL, *desc);    
        if (error) return error;
        }    
    desc = GetDesC8L(KTokenSecret);
        if (desc)
            {
            error = aWSOviSessionCtxDst->Update(KTokenSecret, *desc);    
            if (error) return error;
            }
    desc = GetDesC8L(KToken);
        if (desc)
            {
            error = aWSOviSessionCtxDst->Update(KToken, *desc);    
            if (error) return error;
            }
            
    /*desc = GetDesC8L(KSecurityTokenBackup);
    if (desc)
        {
        error = aWSStarSessionCtxDst->Update(KSecurityTokenBackup, *desc);    
        if (error) return error;
        }*/
    return error;    
    }
// END OF FILE

