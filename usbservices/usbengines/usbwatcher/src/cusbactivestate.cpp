/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This implements CUsbActiveState class.
*
*/


// INCLUDE FILES
#include <usbman.h>
#include "debug.h"
#include "cusbactivestate.h"
#include "cusbwatcher.h"

// CONSTANTS
const TUint KUsbAllStates = 0xFFFFFFFF;

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that might leave.
// ----------------------------------------------------------------------------
//
CUsbActiveState::CUsbActiveState( RUsb& aUsbMan, CUsbWatcher& aOwner )
    : CActive( EPriorityStandard )
    , iUsbMan( aUsbMan )
    , iOwner( aOwner )
    , iPreviousState( EUsbDeviceStateUndefined )
    {
    LOG_FUNC

    CActiveScheduler::Add( this );
    }

// ----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CUsbActiveState::ConstructL()
    {
    LOG_FUNC

    LEAVEIFERROR( iUsbMan.GetDeviceState( iCurrentState ) );
    LOG1( "Intial UsbDeviceState = %d" , iCurrentState );
        
    // start USB if cable is pluged-in at bootup
    if( EUsbDeviceStateUndefined != iCurrentState )
        {
        iOwner.StateChangeNotifyL( iPreviousState, iCurrentState );
        iPreviousState = iCurrentState;
        }
    iUsbMan.DeviceStateNotification( KUsbAllStates, iCurrentState, iStatus );
    SetActive();
    }

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CUsbActiveState* CUsbActiveState::NewL( RUsb& aUsbMan, CUsbWatcher& aOwner )
    {
    LOG_FUNC

    CUsbActiveState* self = new ( ELeave ) CUsbActiveState( aUsbMan, aOwner );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(); // pop self
    return self;
    }

// Destructor
CUsbActiveState::~CUsbActiveState()
    {
    LOG_FUNC

    Cancel();
    }

// ----------------------------------------------------------------------------
// This function is called when device state is changed.
// ----------------------------------------------------------------------------
//
void CUsbActiveState::RunL()
    {
    LOG_FUNC

    LEAVEIFERROR( iStatus.Int() ); // Close process if error happens here

    TUsbDeviceState newState = iCurrentState;
    iUsbMan.DeviceStateNotification( KUsbAllStates, iCurrentState,
            iStatus );
    SetActive();
    
    // Notify only if there is a change
    if ( newState != iPreviousState )
        {
        LOG2( "USB device state changed: %d ==> %d", iPreviousState,
            newState );
        iOwner.StateChangeNotifyL( iPreviousState, newState );
        iPreviousState = newState;
        }
     else
        {
        LOG2("USB device change ignored: %d -> %d", iPreviousState,
            newState ); 
        }
    }
 
// ----------------------------------------------------------------------------
// Standard active object cancellation function.
// ----------------------------------------------------------------------------
//
void CUsbActiveState::DoCancel()
    {
    LOG_FUNC

    iUsbMan.DeviceStateNotificationCancel();
    }

// ----------------------------------------------------------------------------
// Get current device state.
// ----------------------------------------------------------------------------
//
TUsbDeviceState CUsbActiveState::CurrentState()
    {
    LOG_FUNC

    return iCurrentState;
    }

// ----------------------------------------------------------------------------
// Get previous device state.
// ----------------------------------------------------------------------------
//
TUsbDeviceState CUsbActiveState::PreviousState()
    {
    LOG_FUNC

    return iPreviousState;
    }
	
// End of file
