/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Link connection status observer
*
*/


// INCLUDE FILES
#include "connobserver.h"
#include "ikedebug.h"
#include "ikesocketassert.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CConnObserver* CConnObserver::NewL( RConnection& aConnection,
                                    MConnObserverCallback& aCallback,
                                    MIkeDebug& aDebug )
    {
    CConnObserver* self = new (ELeave) CConnObserver( aConnection,
                                                      aCallback,
                                                      aDebug );
    
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    
    return self;
    }
    
// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CConnObserver::~CConnObserver()
    {
    DEBUG_LOG( _L("CConnObserver::~CConnObserver") );    
    Cancel();
    }

// ---------------------------------------------------------------------------
// Requests notification about link disconnection.
// ---------------------------------------------------------------------------
//
void CConnObserver::NotifyDisconnect()
    {
    IKESOCKET_ASSERT( !IsActive() );

    iConnection.ProgressNotification( iProgressBuf,
                                      iStatus,
                                      KLinkLayerClosed );
    DEBUG_LOG( _L("RConnection::ProgressNotification() started") );    
    SetActive();
    }

// ---------------------------------------------------------------------------
// Cancels link disconnection notification request.
// ---------------------------------------------------------------------------
//
void CConnObserver::CancelNotify()
    {
    Cancel();    
    }
    
// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CConnObserver::CConnObserver( RConnection& aConnection,
                              MConnObserverCallback& aCallback,
                              MIkeDebug& aDebug ) 
 :CActive( EPriorityStandard ),
 iConnection( aConnection ),
 iCallback( aCallback ),
 iDebug( aDebug )
    {
    CActiveScheduler::Add(this);    //Added to the Active Scheduler
    }

// ---------------------------------------------------------------------------
// Second phase construction.
// ---------------------------------------------------------------------------
//
void CConnObserver::ConstructL()
    {
    DEBUG_LOG( _L("CConnObserver::ConstructL") );
    }

// ---------------------------------------------------------------------------
// From class CActive
// Handles completion of progress notification.
// ---------------------------------------------------------------------------
//
void CConnObserver::RunL()
    {
    DEBUG_LOG2( _L("CConnObserver::RunL(), iStatus=%d, error=%d"),
            iStatus.Int(), iProgressBuf().iError );
    TInt status = iStatus.Int();
    if ( status == KErrNone )
        {
        status = iProgressBuf().iError;
        }
    
    iCallback.LinkDisconnected( status );
    }

// ---------------------------------------------------------------------------
// From class CActive
// Cancels progress notification.
// ---------------------------------------------------------------------------
//
void CConnObserver::DoCancel()
    {
    iConnection.CancelProgressNotification();
    DEBUG_LOG( _L("RConnection::CancelProgressNotification() called") );    
    }

