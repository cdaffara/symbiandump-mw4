/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  AlfSynchronizer waits for synchronization to complete
*
*/


#include "alfsynchronizer.h"
#include "alfrenderstageutils.h"

// Timeout in microseconds - 500 ms.
const TInt KAlfSyncTimeout = 500000;

// --------------------------------------------------------------------------
// CAlfSynchronizer::NewL
// --------------------------------------------------------------------------
//    
CAlfSynchronizer* CAlfSynchronizer::NewL()
    {
    CAlfSynchronizer* self = new (ELeave) CAlfSynchronizer;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }
    
// --------------------------------------------------------------------------
// CAlfSynchronizer::~CAlfSynchronizer
// --------------------------------------------------------------------------
//
CAlfSynchronizer::~CAlfSynchronizer()
    {
    Cancel();
    
    iProperty.Close();
    
    delete iWait;
    delete iTimeout;
    }

// --------------------------------------------------------------------------
// CAlfSynchronizer::Start
// --------------------------------------------------------------------------
//
void CAlfSynchronizer::Start(TInt aId)
    {
    iProperty.Subscribe( iStatus );
    SetActive();
    
    TInt value = 0;
    iProperty.Get(value);
    
    if ( value != aId )
        {
        // Launch timeout AO in case there happens to be deadlock in coretoolkit etc.
        iTimeout->Start( KAlfSyncTimeout, KAlfSyncTimeout, 
            TCallBack( CallbackSyncTimeout, this ) );

        // Start wait
        iSynchId = aId;
        iWait->Start();
        }
    }
    
// --------------------------------------------------------------------------
// CAlfSynchronizer::RunL
// --------------------------------------------------------------------------
//
void CAlfSynchronizer::RunL()
    {
    if ( !iAsyncStopDone && iStatus.Int() == KErrNone )
        {
        iProperty.Subscribe( iStatus );
        SetActive();
        }
    
    TInt value = 0;
    iProperty.Get(value);
        
    if ( !iAsyncStopDone && value == iSynchId )
        {
        iWait->AsyncStop();
        iAsyncStopDone = ETrue;
        }
    }

// --------------------------------------------------------------------------
// CAlfSynchronizer::CallbackSyncTimeout
// --------------------------------------------------------------------------
//
TInt CAlfSynchronizer::CallbackSyncTimeout( TAny* aPtr )
    {
    ((CAlfSynchronizer*)aPtr)->DoCallbackSyncTimeout();
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CAlfSynchronizer::DoCallbackSyncTimeout
// --------------------------------------------------------------------------
//
void CAlfSynchronizer::DoCallbackSyncTimeout()
    {        
    iTimeout->Cancel();
    
    if (!iAsyncStopDone)
        {
        iWait->AsyncStop();
        iAsyncStopDone = ETrue;
        }
    }

// --------------------------------------------------------------------------
// CAlfSynchronizer::DoCancel
// --------------------------------------------------------------------------
//
void CAlfSynchronizer::DoCancel()
    {
    iProperty.Cancel();
    }

// --------------------------------------------------------------------------
// CAlfSynchronizer::CAlfSynchronizer
// --------------------------------------------------------------------------
//
CAlfSynchronizer::CAlfSynchronizer()
    : CActive( EPriorityHigh )
    {
    CActiveScheduler::Add(this);
    }

// --------------------------------------------------------------------------
// CAlfSynchronizer::ConstructL
// --------------------------------------------------------------------------
//
void CAlfSynchronizer::ConstructL()
    {
    User::LeaveIfError( iProperty.Attach( KAlfPSUidSynchronizer, KAlfPSKeySynchronizer ) );

    iWait = new (ELeave) CActiveSchedulerWait;
    
    iTimeout = CPeriodic::NewL( CActive::EPriorityHigh );
    }
