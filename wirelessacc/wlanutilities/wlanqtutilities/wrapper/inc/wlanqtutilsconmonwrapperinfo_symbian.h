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
* library's connection info interface.
*/

#ifndef WLANQTUTILSCONMONWRAPPERINFO_SYMBIAN_H
#define WLANQTUTILSCONMONWRAPPERINFO_SYMBIAN_H

// System includes

#include <rconnmon.h>

// User includes

#include "wlanqtutils.h"

// Forward declarations

class WlanQtUtilsConnection;
class WlanQtUtilsConMonWrapper;

// External data types

// Constants

// Class declaration

class WlanQtUtilsConMonWrapperInfo : public MConnectionMonitorObserver
{
public:
    
    // Data types

    WlanQtUtilsConMonWrapperInfo(WlanQtUtilsConMonWrapper *wrapper);

    ~WlanQtUtilsConMonWrapperInfo();

    WlanQtUtilsConnection *ActiveConnection();

    WlanQtUtilsConnection *ConnectionInfo(uint connectionId);

protected:

private:
    
    TBool ConnectionInfoDetails(WlanQtUtilsConnection *connection);

    void EventL(const CConnMonEventBase& connMonEvent);

    WlanQtUtils::ConnStatus ConnMonConnStatusMap(TInt connStatus);

    TBool IsWlanConnection(TUint connectionId);

private: // data
    
    // Owned data

    RConnectionMonitor iMonitor; //!< Handle to Connection Monitor Server

    TRequestStatus iStatus; //!< Request status

    //! ID of the connection we are currently monitoring
    TUint iMonitoringConnection;

    // Not owned data
    
    //! Public implementation to report progress to
    WlanQtUtilsConMonWrapper *q_ptr;

    // Friend classes

    // TestWlanQtUtils is defined as a friend class in order to be able to
    // call event handlers of wrappers.
    friend class TestWlanQtUtils;
};

#endif // WLANQTUTILSCONMONWRAPPERINFO_SYMBIAN_H
