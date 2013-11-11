/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Subscribe to global system state events
*
*/


#include <startupdomainpskeys.h> //for global system state
#include "cusbglobalsystemstateobserver.h"
#include "cusbwatcher.h"

// ----------------------------------------------------------------------------
// Two-phase constructor
// ----------------------------------------------------------------------------
//
CUsbGlobalSystemStateObserver* CUsbGlobalSystemStateObserver::NewL(
        CUsbWatcher& aUsbWatcher )
    {
    LOG_FUNC

    CUsbGlobalSystemStateObserver* me = CUsbGlobalSystemStateObserver::NewLC(
            aUsbWatcher );
    CleanupStack::Pop();
    return me;
    }

// ----------------------------------------------------------------------------
// Two-phase constructor, instance put in cleanupstack
// ----------------------------------------------------------------------------
//
CUsbGlobalSystemStateObserver* CUsbGlobalSystemStateObserver::NewLC(
        CUsbWatcher& aUsbWatcher )
    {
    LOG_FUNC

    CUsbGlobalSystemStateObserver* me = new ( ELeave )
            CUsbGlobalSystemStateObserver( aUsbWatcher );
    CleanupStack::PushL( me );
    me->ConstructL();
    return me;
    }

// ----------------------------------------------------------------------------
// 2nd Phase Construction
// ----------------------------------------------------------------------------
//
void CUsbGlobalSystemStateObserver::ConstructL()
    {
    LOG_FUNC

    User::LeaveIfError( iProperty.Attach( KPSUidStartup,
            KPSGlobalSystemState ) );
    }

// ----------------------------------------------------------------------------
// C++ Constructor
// ----------------------------------------------------------------------------
//
CUsbGlobalSystemStateObserver::CUsbGlobalSystemStateObserver(
        CUsbWatcher& aUsbWatcher )
    : CActive( EPriorityNormal )
    , iUsbWatcher( aUsbWatcher )
    {
    LOG_FUNC

    CActiveScheduler::Add( this );
    }

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
CUsbGlobalSystemStateObserver::~CUsbGlobalSystemStateObserver()
    {
    LOG_FUNC

    Cancel();
    iProperty.Close();
    }

// ----------------------------------------------------------------------------
// Return global system state
// ----------------------------------------------------------------------------
//
TInt CUsbGlobalSystemStateObserver::GlobalSystemState()
    {
    LOG_FUNC

    TInt state;
    RProperty::Get( KPSUidStartup, KPSGlobalSystemState, state );
    return state;
    }

// ----------------------------------------------------------------------------
// Start observing the state P&S key
// ----------------------------------------------------------------------------
//
void CUsbGlobalSystemStateObserver::Subscribe()
    {
    LOG_FUNC

    if( IsActive() )
        {
        LOG( "Already observing" );
        return; // already observing
        }

    iProperty.Subscribe( iStatus );
    SetActive();
    }

// ----------------------------------------------------------------------------
// Stop observing the state P&S key
// ----------------------------------------------------------------------------
//
void CUsbGlobalSystemStateObserver::DoCancel()
    {
    LOG_FUNC

    iProperty.Cancel();
    }

// ----------------------------------------------------------------------------
// Called when the state changes
// ----------------------------------------------------------------------------
//
void CUsbGlobalSystemStateObserver::RunL()
{
    LOG_FUNC
    LOG1( "iStatus = %d", iStatus.Int() );

    Subscribe();
    TInt systemState = 0;
    User::LeaveIfError( iProperty.Get( systemState ) );
    LOG1( "SystemState= %d", systemState );

    if (  ( systemState == ESwStateChargingToNormal )
       || ( systemState == ESwStateAlarmToNormal )  )
        {
        iUsbWatcher.StopPersonality();
        }
    else if ( systemState == ESwStateCharging )
        {
        //USBWatcher started before charging state
        iUsbWatcher.StartPersonality();
        }
    else if (
        (systemState == ESwStateNormalRfOn) ||
        (systemState == ESwStateNormalRfOff) ||
        (systemState == ESwStateNormalBTSap) )
        {
        //after ESwStateChargingToNormal
        Cancel();
        iUsbWatcher.StartPersonality();
        }
}

// ----------------------------------------------------------------------------
// Handle RunL leaving
// ----------------------------------------------------------------------------
//
TInt CUsbGlobalSystemStateObserver::RunError( TInt aError )
    {
    LOG_FUNC

    LOG1( "aError = %d", aError );
    return ( KErrNone );
    }

// End of file
