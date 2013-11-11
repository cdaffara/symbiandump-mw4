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
* Description:
* Main implementation of Wlan Sniffer Keepalive application.
*/

// System include files

#include <rconnmon.h>

// User include files

#include "wsfkeepalivetimer.h"
#include "wsfkeepalivecmm.h"
#include "wsfkeepaliveconnmon.h"
#include "wsfkeepaliveconnmondisc.h"
#include "wsfkeepaliveesock.h"
#include "wsfkeepalive.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "wsfkeepaliveTraces.h"
#endif

// External function prototypes

// Local constants

// We need to poll for client info every 5 seconds
static const int KKeepalivePollInterval = 5000000;

// We can close a connection after 5 minutes of inactivity
static const int KKeepaliveInactivityInterval = 300;

// UID of Wlan sniffer application
static const TUid KWlanSnifferUid = { 0x10281CAA };

// UID of Wlan login application
static const TUid KWlanloginUid = { 0x2002E6D0 };

// List of UIDs of clients not considered as "real"
static const TUid KDiscardedClientUids[] = 
    {
        { 0x2002FF5F }, // Sniffer keepalive process, that is, us
        KWlanSnifferUid, // Sniffer application (wlansniffer.exe)
        KWlanloginUid, //   Wlan Login application (wlanlogin.exe)
        { 0x101fD9C5 }  // DHCP server (dhcpserv.exe)
    };

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

CWsfKeepalive* CWsfKeepalive::NewL()
    {
    OstTraceFunctionEntry0( CWSFKEEPALIVE_NEWL_ENTRY );
    
    CWsfKeepalive* me = new ( ELeave ) CWsfKeepalive();
    CleanupStack::PushL( me );
    me->ConstructL();
    CleanupStack::Pop( me );
    
    OstTraceFunctionExit0( CWSFKEEPALIVE_NEWL_EXIT );
    return me;
    }

CWsfKeepalive::~CWsfKeepalive()
    {
    OstTraceFunctionEntry0( DUP1_CWSFKEEPALIVE_CWSFKEEPALIVE_ENTRY );

    delete iEsock;
    delete iConnMonDisc;
    delete iCmMgr;
    delete iConnMon;
    delete iTimer;
    
    OstTraceFunctionExit0( DUP1_CWSFKEEPALIVE_CWSFKEEPALIVE_EXIT );
    }

void CWsfKeepalive::TimerExpired( TInt aError )
    {
    OstTraceFunctionEntry0( CWSFKEEPALIVE_TIMEREXPIRED_ENTRY );

    TBool restartTimer = ETrue; // Should we restart timer or not?
    if ( aError == KErrNone )
        {
        // Timer successfully completed, handle it
        if ( NoRealClients( iConnectionId ) )
            {
            TTime now;
            now.UniversalTime();
            
            // Check whether we are moving from EActiveWithClients to
            // EActiveNoClients
            if ( iState == EActiveWithClients )
                {
                OstTrace0(
                    TRACE_NORMAL,
                    CWSFKEEPALIVE_TIMEREXPIRED_RESET,
                    "CWsfKeepalive::TimerExpired Keepalive time reset" );
        
                // Connection had clients, but doesn't anymore. Keepalive
                // inactivity time starts now
                iKeepaliveStart = now;
                }
            // Check whether keepalive time has been reached
            else if ( iKeepaliveStart +
                TTimeIntervalSeconds( KKeepaliveInactivityInterval ) <= now )
                {
                OstTrace0(
                    TRACE_NORMAL,
                    CWSFKEEPALIVE_TIMEREXPIRED_DONE,
                    "CWsfKeepalive::TimerExpired Keepalive time expired" );
                
                // Keepalive time limit expired, connection should be stopped
                iEsock->Disconnect();
                iConnMonDisc->Disconnect( iConnectionId );
                restartTimer = EFalse;
                }
            
            // There are now no real clients for the connection
            SetState( EActiveNoClients );
            }
        else
            {
            // One or more real clients are using the connection
            SetState( EActiveWithClients );
            }
        }
    else
        {
        // Timer not successful, probably because we stopped it
        restartTimer = EFalse;
        }

    if ( restartTimer )
        {
        TTimeIntervalMicroSeconds32 interval( KKeepalivePollInterval );
        iTimer->After( interval );
        }
    
    OstTraceFunctionExit0( CWSFKEEPALIVE_TIMEREXPIRED_EXIT );
    }

void CWsfKeepalive::WlanConnectionOpenedL( TUint aConnectionId, TUint aIapId )
    {
    OstTraceFunctionEntry0( CWSFKEEPALIVE_WLANCONNECTIONOPENEDL_ENTRY );
    
    OstTraceExt2(
        TRACE_NORMAL,
        CWSFKEEPALIVE_WLANCONNECTIONOPENED,
        "CWsfKeepalive::WlanConnectionOpened;aConnectionId=%u;aIapId=%u",
        aConnectionId,
        aIapId );

    // We are only interested in connections opened by the Wlan Sniffer
    if ( OpenedByWlanSniffer( aConnectionId ) )
        {
        // Start to monitor this connection, and add us as a user to the
        // connection
        iConnectionId = aConnectionId;
        iIapId = aIapId;
        iEsock->ConnectL( aIapId );

        // Assume there are no real clients yet. Setup timer for polling
        // when real clients might be added to the connection
        SetState( EActiveNoClients );
        iKeepaliveStart.UniversalTime();
        
        OstTrace0(
            TRACE_NORMAL,
            CWSFKEEPALIVE_WLANCONNECTIONOPENED_RESET,
            "CWsfKeepalive::WlanConnectionOpened Keepalive time reset" );
        
        TTimeIntervalMicroSeconds32 interval( KKeepalivePollInterval );
        iTimer->After( interval );
        }
    
    OstTraceFunctionExit0( CWSFKEEPALIVE_WLANCONNECTIONOPENEDL_EXIT );
    }

void CWsfKeepalive::WlanConnectionClosed()
    {
    OstTraceFunctionEntry0( CWSFKEEPALIVE_WLANCONNECTIONCLOSED_ENTRY );

    // No need to monitor anything anymore
    SetState( EInactive );
    iConnectionId = KInvalidConnectionId;
    // Stop also the polling timer
    iTimer->Stop();
    
    OstTrace1(
        TRACE_NORMAL,
        CWSFKEEPALIVE_WLANCONNECTIONCLOSED_IAPID,
        "CWsfKeepalive::WlanConnectionClosed iapId=%d",
        iIapId );
    
    // If connected to hotspot IAP, the IAP must be deleted
    if ( iCmMgr->GetHotspotInfoL( iIapId ) )
        {
        OstTrace0(
            TRACE_NORMAL,
            CWSFKEEPALIVE_WLANCONNECTIONCLOSED_HOTSPOTDETECTED,
            "CWsfKeepalive::WlanConnectionClosed Hotspot IAP detected" );

        if ( !iCmMgr->DeleteHotspotIapL( iIapId ) )
            {
            OstTrace0(
                TRACE_NORMAL,
                CWSFKEEPALIVE_WLANCONNECTIONCLOSED_HOTSPOTDELETEFAILED,
                "CWsfKeepalive::WlanConnectionClosed Hotspot delete failed" );
            }
        iIapId = 0;
        }
    
    OstTraceFunctionExit0( CWSFKEEPALIVE_WLANCONNECTIONCLOSED_EXIT );
    }

// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
CWsfKeepalive::CWsfKeepalive() :
    iConnectionId( KInvalidConnectionId ),
    iState( EInactive ),
    iIapId( 0 )
    {
  	OstTraceFunctionEntry0( CWSFKEEPALIVE_CWSFKEEPALIVE_ENTRY );
	  iCmMgr = NULL;
    iConnMon = NULL;
    iConnMonDisc = NULL;
    iEsock = NULL;
    iTimer = NULL;
	  OstTraceFunctionExit0( CWSFKEEPALIVE_CWSFKEEPALIVE_EXIT );
    }

// ---------------------------------------------------------------------------
// Leaving constructor
// ---------------------------------------------------------------------------
//
void CWsfKeepalive::ConstructL()
    {
    OstTraceFunctionEntry0( CWSFKEEPALIVE_CONSTRUCTL_ENTRY );
    
    iTimer = CWsfKeepaliveTimer::NewL( *this );
    iConnMon = CWsfKeepaliveConnMon::NewL( *this );
    iConnMonDisc = CWsfKeepaliveConnMonDisc::NewL();
    iEsock = CWsfKeepaliveEsock::NewL();
    iCmMgr = CWsfKeepaliveCmm::NewL();
    
    OstTraceFunctionExit0( CWSFKEEPALIVE_CONSTRUCTL_EXIT );
    }
 
// ---------------------------------------------------------------------------
// Checks whether the given connection was opened by the Wlan Sniffer
// application
// ---------------------------------------------------------------------------
//
TBool CWsfKeepalive::OpenedByWlanSniffer( TUint aConnectionId )
    {
    OstTraceFunctionEntry0( CWSFKEEPALIVE_OPENEDBYWLANSNIFFER_ENTRY );
    
    TBool retVal = EFalse;

    // Get all clients of this connection
    TConnMonClientEnumBuf clientInfo;
    iConnMon->GetClientInfo( clientInfo, aConnectionId );

    // Check whether Wlan sniffer is one of the clients
    for ( TInt i( 0 ); i < clientInfo().iCount; ++i )
        {
        if ( clientInfo().iUid[i] == KWlanSnifferUid )
            {
            // Match found, stop looking
            retVal = ETrue;
            break;
            }
        }
    
    OstTraceExt2(
        TRACE_NORMAL,
        CWSFKEEPALIVE_OPENEDBYWLANSNIFFER,
        "CWsfKeepalive::OpenedByWlanSniffer;aConnectionId=%u;retVal=%u",
        aConnectionId,
        retVal );
    
    OstTraceFunctionExit0( CWSFKEEPALIVE_OPENEDBYWLANSNIFFER_EXIT );
    return retVal;
    }

// ---------------------------------------------------------------------------
// Checks whether there are any real clients using the given connection
// ---------------------------------------------------------------------------
//
TBool CWsfKeepalive::NoRealClients( TUint aConnectionId )
    {
    OstTraceFunctionEntry0( CWSFKEEPALIVE_NOREALCLIENTS_ENTRY );
    
    // Get all clients of this connection
    TConnMonClientEnumBuf clientInfo;
    iConnMon->GetClientInfo( clientInfo, aConnectionId );

    // Get the client count
    TInt clientCount = clientInfo().iCount;

    TInt discardedClientCount = sizeof( KDiscardedClientUids ) / sizeof( TUid );
    
    // Decrease count by each non-real client we must discard
    for ( TInt i( 0 ); i < clientInfo().iCount; ++i )
        {
        for ( TInt j( 0 ); j < discardedClientCount; ++j )
            {
            if ( clientInfo().iUid[i] == KDiscardedClientUids[j] )
                {
                OstTrace1(
                    TRACE_NORMAL,
                    CWSFKEEPALIVE_NOREALCLIENTS_DISCARD,
                    "CWsfKeepalive::NoRealClients Client discarded;clientInfo().iUid[i].iUid=%x",
                    clientInfo().iUid[i].iUid );
                --clientCount;
                break;
                }
            }
        }

    // If we reached zero, there were no real clients
    TBool retVal = clientCount == 0 ? ETrue : EFalse;
    OstTraceExt2(
        TRACE_NORMAL,
        CWSFKEEPALIVE_NOREALCLIENTS,
        "CWsfKeepalive::NoRealClients;aConnectionId=%u;retVal=%u",
        aConnectionId,
        retVal );
    
    OstTraceFunctionExit0( CWSFKEEPALIVE_NOREALCLIENTS_EXIT );
    return retVal;
    }

// ---------------------------------------------------------------------------
// Sets the given state and traces the transition, if state changed.
// ---------------------------------------------------------------------------
//
void CWsfKeepalive::SetState( TUint aState )
    {
    OstTraceFunctionEntry0( CWSFKEEPALIVE_SETSTATE_ENTRY );

#ifdef OST_TRACE_COMPILER_IN_USE
    if ( aState != iState )
        {
        OstTrace1(
            TRACE_NORMAL,
            CWSFKEEPALIVE_SETSTATE,
            "CWsfKeepalive::SetState;aState=%{State}",
            aState );
        }
#endif
    iState = aState;
    
    OstTraceFunctionExit0( CWSFKEEPALIVE_SETSTATE_EXIT );
    }
