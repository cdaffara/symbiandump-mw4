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
* Description:  active object, that monitors link disconnection
*
*/


// INTERNAL HEADERS
#include "ikeconnectioninterface.h"
#include "vpnconnection.h"

// CLASS HEADER
#include "disconnectionobserver.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CDisconnectionObserver* CDisconnectionObserver::NewL( CIkeConnectionInterface& aIkeConnectionInterface,
                                                      MDisconnectionObserverCallback& aCallback )
    {
    CDisconnectionObserver* self = new (ELeave) CDisconnectionObserver( aIkeConnectionInterface,
                                                                        aCallback );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CDisconnectionObserver::~CDisconnectionObserver()
    {
    Cancel();
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CDisconnectionObserver::CDisconnectionObserver( CIkeConnectionInterface& aIkeConnectionInterface,
                                                MDisconnectionObserverCallback& aCallback )
 : CActive( EPriorityStandard ),
   iIkeConnectionInterface( aIkeConnectionInterface ),
   iCallback( aCallback )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// Requests asynchronous link disconnection notification.
// ---------------------------------------------------------------------------
//
void CDisconnectionObserver::StartObserving()
    {
    iIkeConnectionInterface.NotifyDisconnect( iStatus );
    SetActive();
    }

// ---------------------------------------------------------------------------
// From class CActive
// Handles completion of asynchronous notification request. 
// ---------------------------------------------------------------------------
//
void CDisconnectionObserver::RunL()
    {
    iCallback.DisconnectIndication( iStatus.Int() );
    }

// ---------------------------------------------------------------------------
// From class CActive
// Handles cancellation of asynchronous notification request. 
// ---------------------------------------------------------------------------
//
void CDisconnectionObserver::DoCancel()
    {
    iIkeConnectionInterface.CancelNotifyDisconnect();
    }

