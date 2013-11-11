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
* Description:  active object, that monitors the real connection starting
*
*/


// INTERNAL HEADERS
#include "vpnconnection.h"

// CLASS HEADER
#include "connectionstarter.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CConnectionStarter* CConnectionStarter::NewL( CVpnConnection& aConnection,
                                              MConnectionStarterCallback& aCallback )
    {
    CConnectionStarter* self = new ( ELeave ) CConnectionStarter( aConnection,
                                                                  aCallback );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CConnectionStarter::~CConnectionStarter()
    {
    Cancel();
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CConnectionStarter::CConnectionStarter( CVpnConnection& aConnection,
                                        MConnectionStarterCallback& aCallback )
 : CActive( EPriorityHigh ), // Higher priority than in CDisconnectionObserver
   iConnection( aConnection ),
   iCallback( aCallback )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// Starts connection asynchronously. 
// ---------------------------------------------------------------------------
//
void CConnectionStarter::StartRealConnection()
    {
    iConnection.StartRealConnection( iStatus );
    SetActive();
    }

// ---------------------------------------------------------------------------
// From class CActive
// Handles completion of asynchronous connection starting. 
// ---------------------------------------------------------------------------
//
void CConnectionStarter::RunL()
    {
    TInt realIapId = 0;
    TInt realNetworkId = 0;
    
    if ( iStatus.Int() == KErrNone )
        {
        realIapId = iConnection.RealIapId();
        realNetworkId = iConnection.RealNetId();
        }
    
    iCallback.RealConnectionStarted( iStatus.Int(),
                                     realIapId,
                                     realNetworkId );
    }

// ---------------------------------------------------------------------------
// From class CActive
// Handles cancellation of asynchronous connection starting. 
// ---------------------------------------------------------------------------
//
void CConnectionStarter::DoCancel()
    {
    iConnection.CancelStartRealConnection();
    }

    
