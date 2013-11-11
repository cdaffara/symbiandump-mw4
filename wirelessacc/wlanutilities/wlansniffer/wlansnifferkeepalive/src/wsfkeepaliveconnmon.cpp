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
 * Connection Monitor interface implementation.
 */

// System include files

#include <rconnmon.h>
#include <nifvar.h>

// User include files

#include "wsfkeepalive.h"
#include "wsfkeepaliveconnmon.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "wsfkeepaliveconnmonTraces.h"
#endif

// External function prototypes

// Local constants

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

CWsfKeepaliveConnMon* CWsfKeepaliveConnMon::NewL( CWsfKeepalive& aNotify )
    {
    OstTraceFunctionEntry0( CWSFKEEPALIVECONNMON_NEWL_ENTRY );
    
    CWsfKeepaliveConnMon* me = new ( ELeave ) CWsfKeepaliveConnMon( aNotify );
    CleanupStack::PushL( me );
    me->ConstructL();
    CleanupStack::Pop( me );
    
    OstTraceFunctionExit0( CWSFKEEPALIVECONNMON_NEWL_EXIT );
    return me;
    }

CWsfKeepaliveConnMon::~CWsfKeepaliveConnMon()
    {
    OstTraceFunctionEntry0(
        DUP1_CWSFKEEPALIVECONNMON_CWSFKEEPALIVECONNMON_ENTRY );

    // Stop monitoring observer events
    iMonitor.CancelNotifications();
    iMonitor.Close();
    
    OstTraceFunctionExit0(
        DUP1_CWSFKEEPALIVECONNMON_CWSFKEEPALIVECONNMON_EXIT );
    }

void CWsfKeepaliveConnMon::GetClientInfo(
    TConnMonClientEnumBuf& aClientInfo,
    TUint aConnectionId )
    {
    OstTraceFunctionEntry0( CWSFKEEPALIVECONNMON_GETCLIENTINFO_ENTRY );
    
    // Get all clients of this connection
    TRequestStatus status;
    iMonitor.GetPckgAttribute(
        aConnectionId,
        0, 
        KClientInfo, 
        aClientInfo, 
        status );
    User::WaitForRequest( status );
    if ( status.Int() != KErrNone )
        {
        // Make sure we don't try to use the result buffer since its
        // contents are not ok
        aClientInfo().iCount = 0;
        }
    
#ifdef OST_TRACE_COMPILER_IN_USE
    for ( TInt i( 0 ); i < aClientInfo().iCount; ++i )
        {
        OstTrace1(
            TRACE_NORMAL,
            CWSFKEEPALIVECONNMON_GETCLIENTINFO,
            "CWsfKeepaliveConnMon::GetClientInfo Client ID;aClientInfo().iUid[i].iUid=%x",
            aClientInfo().iUid[i].iUid );
        }
#endif

    OstTraceFunctionExit0( CWSFKEEPALIVECONNMON_GETCLIENTINFO_EXIT );
    }

// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
CWsfKeepaliveConnMon::CWsfKeepaliveConnMon( CWsfKeepalive& aNotify ) :
    iNotify( aNotify ),
    iMonitoringConnectionId( CWsfKeepalive::KInvalidConnectionId )
    {
    OstTraceFunctionEntry0( CWSFKEEPALIVECONNMON_CWSFKEEPALIVECONNMON_ENTRY );
    OstTraceFunctionExit0( CWSFKEEPALIVECONNMON_CWSFKEEPALIVECONNMON_EXIT );
    }

// ---------------------------------------------------------------------------
// Leaving constructor
// ---------------------------------------------------------------------------
//
void CWsfKeepaliveConnMon::ConstructL()
    {
    OstTraceFunctionEntry0( CWSFKEEPALIVECONNMON_CONSTRUCTL_ENTRY );
    
    iMonitor.ConnectL();
    // Start monitoring observer events
    iMonitor.NotifyEventL( *this );
    
    OstTraceFunctionExit0( CWSFKEEPALIVECONNMON_CONSTRUCTL_EXIT );
    }
 
// ---------------------------------------------------------------------------
// Event method is called every time an observer event occures
// ---------------------------------------------------------------------------
//
void CWsfKeepaliveConnMon::EventL( const CConnMonEventBase& aConnMonEvent )
    {
    OstTraceFunctionEntry0( CWSFKEEPALIVECONNMON_EVENTL_ENTRY );
    
    TUint connectionId = aConnMonEvent.ConnectionId();
    TInt bearer;
    TRequestStatus status;

    OstTraceExt2(
        TRACE_NORMAL,
        CWSFKEEPALIVECONNMON_EVENTL_EVENTTYPE,
        "CWsfKeepaliveConnMon::EventL;connectionId=%u;aConnMonEvent.EventType()=%{ConnMonEvent}",
        connectionId,
        aConnMonEvent.EventType() );
    
    const CConnMonConnectionStatusChange* connectionStatusChangeEvent;
    switch ( aConnMonEvent.EventType() )
        {
        case EConnMonDeleteConnection:
            // We are only interested in one connection
            if ( connectionId == iMonitoringConnectionId )
                {
                iNotify.WlanConnectionClosed();
                iMonitoringConnectionId = CWsfKeepalive::KInvalidConnectionId;
                }
            break;
            
        case EConnMonConnectionStatusChange:
            connectionStatusChangeEvent =
                static_cast< 
                const CConnMonConnectionStatusChange* >( &aConnMonEvent );

            OstTrace1(
                TRACE_NORMAL,
                CWSFKEEPALIVECONNMON_EVENTL_CONNSTATUS,
                "CWsfKeepaliveConnMon::EventL;connectionStatusChangeEvent->ConnectionStatus()=%{ConnMonConnStatus}",
                connectionStatusChangeEvent->ConnectionStatus() );
            
            // We are only interested in connected event
            if ( connectionStatusChangeEvent->ConnectionStatus()
                == KLinkLayerOpen )
                {
                // Get the connection bearer type.
                iMonitor.GetIntAttribute(
                    connectionId,
                    0,
                    KBearer,
                    bearer,
                    status );
                User::WaitForRequest( status );

                // We are only interested in WLAN connections
                if ( status.Int() == KErrNone && bearer == EBearerWLAN )
                    {
                    // Get connection IAP ID.
                    TUint iapId = 0;
                    iMonitor.GetUintAttribute(
                        connectionId,
                        0,
                        KIAPId,
                        iapId,
                        status );
                    User::WaitForRequest( status );
    
                    if ( status == KErrNone )
                        {
                        iNotify.WlanConnectionOpenedL( connectionId, iapId );
                        // Remember this connection
                        iMonitoringConnectionId = connectionId;
                        }
                    }
                }
            break;
            
        default:
            // Not interesting
            break;
        }
    
    OstTraceFunctionExit0( CWSFKEEPALIVECONNMON_EVENTL_EXIT );
    }
