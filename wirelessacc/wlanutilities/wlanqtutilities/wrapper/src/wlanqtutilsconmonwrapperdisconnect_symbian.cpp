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
 * library's connection disconnect interface.
 */

// System includes

// User includes

#include "wlanqtutilsconmonwrapper.h"
#include "wlanqtutilsconmonwrapperdisconnect_symbian.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "wlanqtutilsconmonwrapperdisconnect_symbianTraces.h"
#endif

/*!
    \class WlanQtUtilsConMonWrapperDisconnect
    \brief Private wrapper for Symbian Connection Monitor library.

    Provides functionality to disconnect connections regardless of how
    many applications are using the connection.
*/

// External function prototypes

// Local constants

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

/*!
    Constructor.
    
    @param [in] wrapper Wrapper to report progress to.
 */

WlanQtUtilsConMonWrapperDisconnect::WlanQtUtilsConMonWrapperDisconnect(
    WlanQtUtilsConMonWrapper *wrapper) :
    q_ptr(wrapper)
{
    OstTraceFunctionEntry1(WLANQTUTILSCONMONWRAPPERDISCONNECT_WLANQTUTILSCONMONWRAPPERDISCONNECT_ENTRY, this);
    
    // Errors in Connection Monitor Server connection are fatal so just
    // throw them as exceptions
    QT_TRAP_THROWING(iMonitor.ConnectL());

    OstTraceFunctionExit1(WLANQTUTILSCONMONWRAPPERDISCONNECT_WLANQTUTILSCONMONWRAPPERDISCONNECT_EXIT, this);
}

/*!
    Destructor.
 */

WlanQtUtilsConMonWrapperDisconnect::~WlanQtUtilsConMonWrapperDisconnect()
{
    OstTraceFunctionEntry1(DUP1_WLANQTUTILSCONMONWRAPPERDISCONNECT_WLANQTUTILSCONMONWRAPPERDISCONNECT_ENTRY, this);

    iMonitor.Close();

    OstTraceFunctionExit1(DUP1_WLANQTUTILSCONMONWRAPPERDISCONNECT_WLANQTUTILSCONMONWRAPPERDISCONNECT_EXIT, this);
}

/*!
    Disconnects the connection with the given IAP ID.
    
    @param [in] wrapper Wrapper to report progress to.
 */

void WlanQtUtilsConMonWrapperDisconnect::DisconnectConnection(int iapId)
{
    OstTraceFunctionEntry1(WLANQTUTILSCONMONWRAPPERDISCONNECT_DISCONNECTCONNECTION_ENTRY, this);

    OstTrace1(
        TRACE_NORMAL,
        WLANQTUTILSCONMONWRAPPERDISCONNECT_DISCONNECTCONNECTION,
        "WlanQtUtilsConMonWrapperDisconnect::DisconnectConnection;iapId=%d", iapId);
    
    // Loop through connections to find the one with the IAP ID we want
    TUint count;
    TRequestStatus status;
    iMonitor.GetConnectionCount(count, status);
    User::WaitForRequest(status);
    if (status.Int() == KErrNone) {
        for (TInt i = 1; i <= count; i++) {
            TUint numSubConnections;
            TUint iap = 0;
            TUint connectionId = 0;
            TInt ret = iMonitor.GetConnectionInfo(
                i,
                connectionId,
                numSubConnections);
            if (ret == KErrNone) {
                iMonitor.GetUintAttribute(connectionId, 0, KIAPId, iap, status);
                User::WaitForRequest(status);
                if (status.Int() == KErrNone) {
                    if (iap == iapId) {
                        OstTrace1(
                            TRACE_NORMAL,
                            WLANQTUTILSCONMONWRAPPERDISCONNECT_DISCONNECTCONNECTION_STOP,
                            "WlanQtUtilsConMonWrapperDisconnect::DisconnectConnection Stopping connection;connectionId=%u",
                            connectionId);
                        
                        // Match found, stop connection
                        TInt KErr = iMonitor.SetBoolAttribute(
                            connectionId,
                            0,
                            KConnectionStop,
                            ETrue);
    
                        OstTrace1(
                            TRACE_NORMAL,
                            WLANQTUTILSCONMONWRAPPERDISCONNECT_DISCONNECTCONNECTION_RESULT,
                            "WlanQtUtilsConMonWrapperDisconnect::DisconnectConnection Result;KErr=%d", KErr);
                    }
                }
            }
        }
    }
    
    OstTraceFunctionExit1( WLANQTUTILSCONMONWRAPPERDISCONNECT_DISCONNECTCONNECTION_EXIT, this);
}
