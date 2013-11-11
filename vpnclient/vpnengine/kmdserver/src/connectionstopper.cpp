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
* Description:  active object, that monitors stopping of connection
*
*/


// INTERNAL INCLUDES
#include "vpnconnection.h"

// CLASS HEADER
#include "connectionstopper.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CConnectionStopper* CConnectionStopper::NewL( CVpnConnection& aVpnConnection,
                                              MConnectionStopperCallback& aCallback )
    {
    CConnectionStopper* self = new ( ELeave ) CConnectionStopper( aVpnConnection,
                                                                  aCallback );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CConnectionStopper::~CConnectionStopper()
    {    
    Cancel();    
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CConnectionStopper::CConnectionStopper( CVpnConnection& aVpnConnection,
                                        MConnectionStopperCallback& aCallback )
 : CActive( EPriorityStandard ),
   iVpnConnection( aVpnConnection ),
   iCallback( aCallback )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// Stops VPN connection asynchronously. 
// ---------------------------------------------------------------------------
//
void CConnectionStopper::StopVpnConnection( TBool aSilentClose )
    {
    iVpnConnection.StopVpnConnection( aSilentClose, iStatus );
    SetActive();
    }

// ---------------------------------------------------------------------------
// From class CActive
// Handles completion of asynchronous connection stopping. 
// ---------------------------------------------------------------------------
//
void CConnectionStopper::RunL()
    {
    iCallback.VpnConnectionStopped( iStatus.Int()  );
    }

// ---------------------------------------------------------------------------
// From class CActive
// Handles cancellation of asynchronous connection stopping. 
// ---------------------------------------------------------------------------
//
void CConnectionStopper::DoCancel()
    {
    iVpnConnection.CancelStopVpnConnection();
    }
    
