/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Private implementation of wrapper for Symbian Connection Monitor
* library's scan interface.
*/

// System includes

#include <rconnmon.h>
#include <nifvar.h>

// User includes

#include "wlanqtutils.h"
#include "wlanqtutilsconnection.h"
#include "wlanqtutilsconmonwrapper.h"
#include "wlanqtutilsconmonwrapperinfo_symbian.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "wlanqtutilsconmonwrapperinfo_symbianTraces.h"
#endif

/*!
    \class WlanQtUtilsConMonWrapperInfo
    \brief Private wrapper for Symbian Connection Monitor library.

    Provides functionality to request information about connections.
*/

// External function prototypes

// Local constants

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

/*!
    Constructor.
    
    @param [in] wrapper Wrapper to report progress to.
*/

WlanQtUtilsConMonWrapperInfo::WlanQtUtilsConMonWrapperInfo(
    WlanQtUtilsConMonWrapper *wrapper) :
    iMonitoringConnection(0),
    q_ptr(wrapper)
{
    OstTraceFunctionEntry1(WLANQTUTILSCONMONWRAPPERINFO_WLANQTUTILSCONMONWRAPPERINFO_ENTRY, this);

    // Errors in Connection Monitor Server connection are fatal so just
    // throw them as exceptions
    QT_TRAP_THROWING(
        iMonitor.ConnectL();
        iMonitor.NotifyEventL(*this);
        );

    OstTraceFunctionExit1(WLANQTUTILSCONMONWRAPPERINFO_WLANQTUTILSCONMONWRAPPERINFO_EXIT, this);
}

/*!
    Destructor.
*/

WlanQtUtilsConMonWrapperInfo::~WlanQtUtilsConMonWrapperInfo()
{
    OstTraceFunctionEntry1(DUP1_WLANQTUTILSCONMONWRAPPERINFO_WLANQTUTILSCONMONWRAPPERINFO_ENTRY, this);

    iMonitor.CancelNotifications();
    iMonitor.Close();

    OstTraceFunctionExit1(DUP1_WLANQTUTILSCONMONWRAPPERINFO_WLANQTUTILSCONMONWRAPPERINFO_EXIT, this);
}

/*!
   Getter for active connection information.

   @return Information of active connection, if one exists.
*/ 

WlanQtUtilsConnection *WlanQtUtilsConMonWrapperInfo::ActiveConnection()
{
    OstTraceFunctionEntry1(WLANQTUTILSCONMONWRAPPERINFO_ACTIVECONNECTION_ENTRY, this);

    // Get number of active connections
    TRequestStatus status;

    WlanQtUtilsConnection *activeConn = NULL;
    TUint numConnections = 0;
    iMonitor.GetConnectionCount(numConnections, status);
    User::WaitForRequest(status);
    if (status.Int() == KErrNone) {
        // Get connection info of this connection
        for (TUint i = 1; i <= numConnections; i++) {
            TUint numSubConnections = 0;
            TUint connectionId = 0;
            TInt ret = iMonitor.GetConnectionInfo(
                i,
                connectionId,
                numSubConnections);
            if (ret == KErrNone) {
                activeConn = ConnectionInfo(connectionId);
                if (activeConn != NULL) {
                    // ConnectionInfo() only returns WLAN connections,
                    // and there may be only one active WLAN connection,
                    // so this is the one we want to return and also
                    // remember later.
                    Q_ASSERT(iMonitoringConnection == 0 
                        || iMonitoringConnection == connectionId);
                    iMonitoringConnection = connectionId;
                    break;
                }
            }
        }
    }

    OstTraceFunctionExit1(WLANQTUTILSCONMONWRAPPERINFO_ACTIVECONNECTION_EXIT, this);
    return activeConn;
}

/*!
   Returns information of a connection with the given connection ID.

   @param[in] connectionId Connection ID.
   @return Information of the given connection, if one exists.
*/

WlanQtUtilsConnection *WlanQtUtilsConMonWrapperInfo::ConnectionInfo(
    uint connectionId)
{
    OstTraceFunctionEntry1(WLANQTUTILSCONMONWRAPPERINFO_CONNECTIONINFO_ENTRY, this);
    
    WlanQtUtilsConnection *activeConn = NULL;

    if (IsWlanConnection(connectionId)) {
        activeConn = new WlanQtUtilsConnection();
        activeConn->setConnectionId(connectionId);
        if (!ConnectionInfoDetails(activeConn)) {
            delete activeConn;
            activeConn = NULL;
        }
    }

    OstTraceFunctionExit1(WLANQTUTILSCONMONWRAPPERINFO_CONNECTIONINFO_EXIT, this);
    return activeConn;
}

/*!
   Gets detailed information of a given connection.

   @param[out] connection Where to store info. Connection ID must already
                          be valid.
                          
   @return Returns ETrue if detail fetching succeeded.
*/

TBool WlanQtUtilsConMonWrapperInfo::ConnectionInfoDetails(
    WlanQtUtilsConnection *connection)
{
    OstTraceFunctionEntry1(WLANQTUTILSCONMONWRAPPERINFO_CONNECTIONINFODETAILS_ENTRY, this);
    Q_ASSERT(connection != NULL);

    TBool success = EFalse;
    TUint connectionId = connection->connectionId();
    TRequestStatus status;

    // Get connection IAP ID.
    TUint iapId = 0;
    iMonitor.GetUintAttribute(
        connectionId,
        0,
        KIAPId,
        iapId,
        status);
    User::WaitForRequest(status);
    if (status.Int() == KErrNone) {
        connection->setIapId(iapId);

        // Get connection status.
        TInt connectionStatus = 0;
        iMonitor.GetIntAttribute(
            connectionId,
            0,
            KConnectionStatus,
            connectionStatus,
            status);
        User::WaitForRequest(status);
        if (status.Int() == KErrNone) {
            connection->setConnectionStatus(
                ConnMonConnStatusMap(connectionStatus));
            success = ETrue;
        }
    }

    OstTraceFunctionExit1(WLANQTUTILSCONMONWRAPPERINFO_CONNECTIONINFODETAILS_EXIT, this);
    return success;
}

/*!
   Handler of Connection Monitor Server events.
   
   @param [in] connMonEvent Connection monitor event.
*/

void WlanQtUtilsConMonWrapperInfo::EventL(const CConnMonEventBase& connMonEvent)
{
    OstTraceFunctionEntry1(WLANQTUTILSCONMONWRAPPERINFO_EVENTL_ENTRY, this);

    TUint connectionId = connMonEvent.ConnectionId();
    
    OstTraceExt2(
        TRACE_NORMAL,
        WLANQTUTILSCONMONWRAPPERINFO_EVENTL_EVENTTYPE,
        "WlanQtUtilsConMonWrapperInfo::EventL;connectionId=%u;aConnMonEvent.EventType()=%{ConnMonEvent}",
        connectionId,
        connMonEvent.EventType());

    switch (connMonEvent.EventType()) {
    case EConnMonCreateConnection:
        // Filter out other than WLAN connections
        if (IsWlanConnection(connectionId)) {
            iMonitoringConnection = connectionId;
            emit q_ptr->connCreatedEventFromWrapper(connectionId);
        }
        break;
        
    case EConnMonDeleteConnection:
        if (iMonitoringConnection == connectionId) {
            iMonitoringConnection = 0;
            emit q_ptr->connDeletedEventFromWrapper(connectionId);
        }
        break;
        
    case EConnMonConnectionStatusChange:
        if (iMonitoringConnection == connectionId) {
            // Find out the new status for the connection
            const CConnMonConnectionStatusChange *connMonStatusEvent;
            connMonStatusEvent =
                static_cast< const CConnMonConnectionStatusChange* >(&connMonEvent);
            WlanQtUtils::ConnStatus connectionStatus;
            connectionStatus =
                ConnMonConnStatusMap(connMonStatusEvent->ConnectionStatus());
            
            OstTraceExt2(
                TRACE_NORMAL,
                WLANQTUTILSCONMONWRAPPERINFO_EVENTL_CONNSTATUS,
                "WlanQtUtilsConMonWrapperInfo::EventL;connectionId=%u;eventConnectionStatusChange->ConnectionStatus()=%{ConnMonConnStatus}",
                connectionId,
                connMonStatusEvent->ConnectionStatus());
    
            if (connectionStatus != WlanQtUtils::ConnStatusNone) {
                emit q_ptr->connStatusEventFromWrapper(
                    connectionId,
                    connectionStatus);
            }
        }
        break;
        
    default:
        // Not interesting
        break;
    }

    OstTraceFunctionExit1(WLANQTUTILSCONMONWRAPPERINFO_EVENTL_EXIT, this);
}

/*!
   Maps Connection Monitor's connection status value into
   WlanQtUtilsConnectionStatus.
   
   Note: Mapping is only valid for WLAN connections.

   @param connStatus Connection status as used by Connection
                     Monitor server. Defined in nifvar.h.
   @return Connection status in Wlan Qt Utilities style.
*/

WlanQtUtils::ConnStatus WlanQtUtilsConMonWrapperInfo::ConnMonConnStatusMap(
    TInt connStatus)
    {    
    OstTraceFunctionEntry1(WLANQTUTILSCONMONWRAPPERINFO_CONNMONCONNSTATUSMAP_ENTRY, this);

    WlanQtUtils::ConnStatus utilsConnStatus;
    switch (connStatus) {
    // KConnectionOpen is not final status for WLAN, because DHCP is
    // run after that
    case KConnectionOpen:
    case KStartingConnection:
        utilsConnStatus = WlanQtUtils::ConnStatusConnecting;
        break;
        
    case KLinkLayerOpen:
        utilsConnStatus = WlanQtUtils::ConnStatusConnected;
        break;
        
    case KLinkLayerClosed:
        utilsConnStatus = WlanQtUtils::ConnStatusDisconnected;
        break;
        
    default:
        // Ignore all other values
        utilsConnStatus = WlanQtUtils::ConnStatusNone;
        break;
    }
    
    OstTraceFunctionExit1(WLANQTUTILSCONMONWRAPPERINFO_CONNMONCONNSTATUSMAP_EXIT, this);
    return utilsConnStatus;
}

/*!
    Checks the bearer of given connection.
    Meant for filtering handling only for WLAN IAP's.
    
    @param [in] connectionId Connection ID.
    
    @return Returns ETrue if connection a WLAN connection, otherwise false. 
*/

TBool WlanQtUtilsConMonWrapperInfo::IsWlanConnection(TUint connectionId)
{
    OstTraceFunctionEntry1(WLANQTUTILSCONMONWRAPPERINFO_ISWLANCONNECTION_ENTRY, this);
    
    TBool result = EFalse;
    TRequestStatus status;

    TInt bearer = 0;
    iMonitor.GetIntAttribute(
        connectionId,
        0,
        KBearer,
        bearer,
        status);
    User::WaitForRequest(status);

    if (status.Int() == KErrNone && bearer == EBearerWLAN) {
        result = ETrue;
    }
    
    OstTraceFunctionExit1(WLANQTUTILSCONMONWRAPPERINFO_ISWLANCONNECTION_EXIT, this);
    return result;
}
