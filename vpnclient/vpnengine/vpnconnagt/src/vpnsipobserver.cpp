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
* Description : P&S key monitor for communication between VPN client
*               and SIP Profile server
* Name        : vpnsipobserver.cpp
* Part of     : VPN client / VPN Connection Agent
* Version     : 1.0
*
*/

// SYSTEM INCLUDES
#include <e32property.h>                  // For P&S key
#include <vpnsipobserverpskeys.h>         // For P&S key definition

// USER INCLUDES
#include "vpnsipobserver.h"

// ---------------------------------------------------------------------------
// two phased constructor
// ---------------------------------------------------------------------------
//             
CVpnSipObserver* CVpnSipObserver::NewL( CVPNConnAgt& aAgent )
    {// dkangchecked
    CVpnSipObserver* self = new( ELeave ) CVpnSipObserver( aAgent );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------------------------
// second phased constructor
// ---------------------------------------------------------------------------
//        
void CVpnSipObserver::ConstructL()
    {
    // KVpnSipState P&S key is used for communication between VPN client
    // and SIP Profile Server regarding SIP de/re-registration.
    // KPSVpnSipUid is defined in a SIP System State Monitor plugin
    // (CSipVpnMonitorAo).
    User::LeaveIfError( iSIPProperty.Attach( KPSVpnSipUid, KVpnSipState ) );

    // Start monitoring the P&S key.
    Subscribe();
    }

// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//
CVpnSipObserver::CVpnSipObserver( CVPNConnAgt& aAgent )
    : CActive( EPriorityStandard ),
      iAgent( aAgent )
    {
    CActiveScheduler::Add( this );
    }
                 
// ---------------------------------------------------------------------------
// C++ destructor
// ---------------------------------------------------------------------------
//             
CVpnSipObserver::~CVpnSipObserver()
    {
    Cancel();
    iSIPProperty.Close();
    }

// ---------------------------------------------------------------------------
// CVpnSipObserver::RequestDeregister
// ---------------------------------------------------------------------------
//             
TInt CVpnSipObserver::RequestDeregister()
    {
    // Tells SIP Profile Server that a VPN session is about to be started.
    // This will trigger SIP deregistration process by SIP Profile Server.
    // Should not leave if failed. This will be handled by VPN Conn Agt.
    return iSIPProperty.Set( KPSVpnSipUid, KVpnSipState, EVpnInitiating );
    }

// ---------------------------------------------------------------------------
// CVpnSipObserver::RequestRegister
// ---------------------------------------------------------------------------
//             
TInt CVpnSipObserver::RequestRegister()
    {
    // Tells SIP Profile Server that a VPN session ended.
    // This will trigger SIP re-registration process by SIP Profile Server.	
    // Should not leave if failed. This will be handled by VPN Conn Agt.
    return iSIPProperty.Set( KPSVpnSipUid, KVpnSipState, EVpnTerminated );
    }
             
// ---------------------------------------------------------------------------
// CVpnSipObserver::RunL
// ---------------------------------------------------------------------------
//             
void CVpnSipObserver::RunL()
    {
    LOG_1( "CVpnSipObserver::RunL iStatus = %d", iStatus.Int());
    if ( iStatus == KErrNone ) 
        {
        TInt val = 0;
        // SIP Profile Server notified completion of SIP deregistration.
        TInt err = iSIPProperty.Get( KPSVpnSipUid, KVpnSipState, val );        
        if ( err == KErrNone && val == ESipDeregisterCompleted )
            {
            // If SIP is deregistered, let the VPN Connection Agent to 
            // proceed VPN session start.
            LOG_( "CVpnSipObserver::RunL SIP is deregistered. VPN Connection Agent continue with VPN start." );
            iAgent.ProceedServiceStart();            
            }
        else
            {
            // Keep monitoring.
            Subscribe();
            }
        }
    // Check if observer can be restarted.
    else if ( iStatus != KErrCancel 
           && iStatus != KErrServerTerminated
           && iStatus != KErrNotSupported )
        {
        // Keep monitoring.
        Subscribe();
        }
    else
        {
        // Error.
        LOG_1( "CVpnSipObserver::RunL Unknown error situation, iStatus = %d", iStatus.Int() );
        }
    }

// ---------------------------------------------------------------------------
// CVpnSipObserver::DoCancel
// ---------------------------------------------------------------------------
//                
void CVpnSipObserver::DoCancel()
    {
    iSIPProperty.Cancel();
    }

// ---------------------------------------------------------------------------
// CVpnSipObserver::Subscribe
// ---------------------------------------------------------------------------
//        
void CVpnSipObserver::Subscribe()
    {
    iSIPProperty.Subscribe( iStatus );
    SetActive();
    }
