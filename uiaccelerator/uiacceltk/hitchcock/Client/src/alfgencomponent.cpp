/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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



#include "alf/alfgencomponent.h"
#include "alf/alfconstants.h"
#include "alf/alfenv.h"
#include "alfclient.h"
#include "alflogger.h"

#include <uiacceltk/HuiUtil.h>

class CAlfMessageObserver;
 
NONSHARABLE_CLASS(RAlfSubsessionSimple): public RSubSessionBase
    {
public:
    TInt Open(CAlfEnv& aEnv, TInt aImplementationId, TInt aImplementationUid, const TDesC8& aConstuctionParams);
    TInt SendSynch(TUint aOp, const TDesC8& aInputBuf, TDes8& aOutBuf);
    TInt SendNoReply(TUint aOp, const TDesC8& aInputBuf);
    void SendAsync(const TDesC8& aInputBuf, TDes8& aOutBuf, CAlfMessageObserver* aObserver); 
    void Cancel(TInt aCmdId);
    void Close();
    TInt Identifier()
        {
        return SubSessionHandle();
        }
    
private:
    RAlfClient* iClient;
    };


NONSHARABLE_CLASS(CAlfMessageObserver):public CActive
    {
    public:
    static CAlfMessageObserver* NewRequestLC(RAlfSubsessionSimple& aClient, MAlfAsyncOpObserver* aObserver, TInt aCmdId, CAlfGenComponent& aComms)
        {
        CAlfMessageObserver* me = new (ELeave) CAlfMessageObserver(aClient, aObserver, aCmdId, aComms);
        me->SetActive();
        CleanupStack::PushL(me);
        return me;
        }
    
    TRequestStatus& Status() 
        {
        return iStatus;
        }

    ~CAlfMessageObserver()
        {
        Cancel();
        if (iObserver)
            {
            iObserver->AlfAsyncOpCompleted(TInt(this), KErrCancel);
            }
        }

    const TDesC8& Des() const
        {
        return iAsDescriptor;
        }

    private:
    CAlfMessageObserver(RAlfSubsessionSimple& aClient, MAlfAsyncOpObserver* aObserver, TInt aCmdId, CAlfGenComponent& aComms)
        :CActive(CActive::EPriorityHigh),iClient(aClient), iObserver(aObserver), iComms( aComms ),
        iParametersAsInts(0,0),
        iAsDescriptor( iParametersAsInts )
        {
        ASSERT(aClient.Identifier());
        CActiveScheduler::Add(this);
        iParametersAsInts.iInt1 = aCmdId;
        iParametersAsInts.iInt2 = reinterpret_cast<TInt>(this);
        }
        
    // ---------------------------------------------------------------------------
    // from CActive, called when asych request completed by server
    // ---------------------------------------------------------------------------
    //    
    void RunL()
        {
        iObserver->AlfAsyncOpCompleted(TInt(this), iStatus.Int());
        iObserver = 0;
        
        iComms.CommandCompleted( this ); // function call deletes 'this'
        }
        
    // ---------------------------------------------------------------------------
    // from CActive, called if Cancel called while active
    // ---------------------------------------------------------------------------
    //
    void DoCancel()
        {
        iClient.Cancel(TInt(this));        
        }

    // ---------------------------------------------------------------------------
    // from CActive, will be called if RunL leaves (in this case note allowed )
    // ---------------------------------------------------------------------------  
    //
    TInt RunError(TInt /*aError*/)
        {
        // client leaved in AlfAsyncOpCompleted, just panic it
        USER_INVARIANT();
        return KErrNone;
        }

    RAlfSubsessionSimple& iClient;
    MAlfAsyncOpObserver* iObserver;
    CAlfGenComponent& iComms;
    
    TInt2 iParametersAsInts;
    TPckgC<TInt2> iAsDescriptor;
    };


// ---------------------------------------------------------------------------
// creates new subsession
// ---------------------------------------------------------------------------
//
TInt RAlfSubsessionSimple::Open(CAlfEnv& aEnv, TInt aImplementationId, TInt aImplementationUid, const TDesC8& aConstuctionParams)
    {
    iClient = &aEnv.Client();
    (void)iClient->FlushBatchBuffer();
    TAlfImplementationInformation info(aImplementationId,aImplementationUid, aEnv.ApiVersion());
    TPckgC<TAlfImplementationInformation> infoBuf(info);
    return CreateSubSession(*iClient, EAlfCreateSubSession, TIpcArgs(&infoBuf, &aConstuctionParams));
    }

// ---------------------------------------------------------------------------
// Synchronous server request
// ---------------------------------------------------------------------------
//
TInt RAlfSubsessionSimple::SendSynch(TUint aOp, const TDesC8& aInputBuf, TDes8& aOutBuf)
    {
    (void)iClient->FlushBatchBuffer();
    return SendReceive(EAlfDoSubSessionCmd, TIpcArgs(aOp,&aInputBuf,&aOutBuf));
    }
	 
TInt RAlfSubsessionSimple::SendNoReply(TUint aOp, const TDesC8& aInputBuf )
    {
	return(iClient->SendNoReply(aOp, aInputBuf, SubSessionHandle()));
    }

// ---------------------------------------------------------------------------
// Sets asynchronous request
// ---------------------------------------------------------------------------
//
void RAlfSubsessionSimple::SendAsync(const TDesC8& aInputBuf, TDes8& aOutBuf, CAlfMessageObserver* aObserver)
    {
    (void)iClient->FlushBatchBuffer();
    SendReceive(EAlfDoAsyncSubSessionCmd, TIpcArgs(&aObserver->Des(),&aInputBuf,&aOutBuf), aObserver->Status());
    }

// ---------------------------------------------------------------------------
// cancels asynch operation if active
// ---------------------------------------------------------------------------
//
void RAlfSubsessionSimple::Cancel(TInt aCmdId)
    {
    if (SubSessionHandle())
        {
        SendReceive(EAlfSubSCancelAsynchRequest, TIpcArgs(aCmdId));
        }
    }

// ---------------------------------------------------------------------------
// terminates server side resources
// ---------------------------------------------------------------------------
//
void RAlfSubsessionSimple::Close()
    {
    if ( SubSessionHandle() )
        {
        (void)iClient->FlushBatchBuffer();
        }
    CloseSubSession(EAlfCloseSubSession);    
    }

// private data
NONSHARABLE_CLASS(CAlfGenComponent::TPrivateData)
    {
    public:
    
    RAlfSubsessionSimple iClient;
    RPointerArray<CAlfMessageObserver> iObservers;
    };

// ---------------------------------------------------------------------------
// Public constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfGenComponent* CAlfGenComponent::NewL(
            CAlfEnv& aEnv,
            TInt aImplementationId, 
            TInt aImplementationUid, 
            const TDesC8& aConstructionParams)
    {
    CAlfGenComponent* me = new (ELeave) CAlfGenComponent(aEnv);
    CleanupStack::PushL(me);
    me->ConstructL( aImplementationId, aImplementationUid,aConstructionParams);
    CleanupStack::Pop(me);
    return me;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//    
EXPORT_C CAlfGenComponent::~CAlfGenComponent()
    {
    if (iData)
        {
        iData->iObservers.ResetAndDestroy(); // will cancel outstanding requests 
        iData->iObservers.Close();
        iData->iClient.Close();
        }
        
    delete iData;
    }

// ---------------------------------------------------------------------------
// performs synchronous request
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfGenComponent::DoSynchronousCmd( TUint aOp, 
                               const TDesC8& aInputBuf, TDes8& aOutBuf)
    {
    ASSERT(iData);
    return iData->iClient.SendSynch(aOp, aInputBuf, aOutBuf);
    }

// ---------------------------------------------------------------------------
// performs synchronous request without waiting the reply from the server
// ---------------------------------------------------------------------------
//    
EXPORT_C  TInt CAlfGenComponent::DoCmdNoReply( TUint aOp, const TDesC8& aInputBuf )
    {
    ASSERT(iData);
    TInt err = iData->iClient.SendNoReply(aOp, aInputBuf);
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfGenComponent::DoCmdNoReply buffer error %d - using DoSynchronousCmd", err )
        
        TBuf8<1> dum;
        err = DoSynchronousCmd( aOp, aInputBuf, dum );
        }
    return err;
    }

// ---------------------------------------------------------------------------
// Raises async request to server
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfGenComponent::DoAsynchronousCmdL(TUint aOp, const TDesC8& aInputBuf, 
                               TDes8& aOutBuf, MAlfAsyncOpObserver* aObserver)
    {
    ASSERT(iData);
    // ASSERT(aObserver); // todo: does client need to provide observer if it is not interested about response

    CAlfMessageObserver* obsEntry = CAlfMessageObserver::NewRequestLC(iData->iClient, aObserver, aOp, *this);
    iData->iObservers.AppendL(obsEntry);
    CleanupStack::Pop();

    iData->iClient.SendAsync(aInputBuf, aOutBuf, obsEntry);
    
    return TInt(obsEntry);
    }

// ---------------------------------------------------------------------------
// Request to cancel possible async request
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfGenComponent::CancelAsynchCmd(TInt aCmdId)
    {
    if (iData)
        {
        const TInt index = iData->iObservers.Find(reinterpret_cast<CAlfMessageObserver*>(aCmdId));
        if (index != KErrNotFound)
            {
            CAlfMessageObserver* item = iData->iObservers[index];
            iData->iObservers.Remove(index);
            delete item;
            }
        }
    }

EXPORT_C TInt CAlfGenComponent::Identifier()
    {
    if (iData)
        return iData->iClient.Identifier();
    
    return 0;
    }

// ---------------------------------------------------------------------------
// Default construtor, private
// ---------------------------------------------------------------------------
//
CAlfGenComponent::CAlfGenComponent(CAlfEnv& aEnv)
    :iEnv(aEnv)
    {
    __ASSERT_ALWAYS(aEnv.Client().Handle(), USER_INVARIANT());

    }

// ---------------------------------------------------------------------------
// 2nd phase ctr, opens subsession to envs client
// ---------------------------------------------------------------------------
//    
void CAlfGenComponent::ConstructL(
            TInt aImplementationId, 
            TInt aImplementationUid, 
            const TDesC8& aConstructionParams)
    {
    iData = new (ELeave) TPrivateData();
    User::LeaveIfError(iData->iClient.Open(iEnv, aImplementationId,aImplementationUid,aConstructionParams));
    iData->iObservers.Reset();
    }

// ---------------------------------------------------------------------------
// Called when message completes
// ---------------------------------------------------------------------------
//        
void CAlfGenComponent::CommandCompleted( CAlfMessageObserver* aMessageObserver )
    {
    ASSERT(iData);
    
    const TInt index = iData->iObservers.Find(aMessageObserver);
    ASSERT(index != KErrNotFound);

    delete iData->iObservers[index];
    iData->iObservers.Remove(index);
    }
    
// end of file
