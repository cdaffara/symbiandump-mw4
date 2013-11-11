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
* WLAN Qt Utilities WLAN connection status handler.
*/

#ifndef WLANQTUTILSCONNECTION_H
#define WLANQTUTILSCONNECTION_H

// System includes

#include <QObject>

// User includes

#include "wlanqtutils.h"

// Forward declarations

// External data types

// Constants

// Class declaration

class WlanQtUtilsConnection
{
    
public:

    // Data types

    WlanQtUtilsConnection();

    virtual ~WlanQtUtilsConnection();
    
    uint connectionId() const;

    void setConnectionId(uint connectionId);

    uint iapId() const;

    void setIapId(uint iapId);

    WlanQtUtils::ConnStatus connectionStatus() const;

    void setConnectionStatus(
        WlanQtUtils::ConnStatus connectionStatus);

signals:

public slots:

protected:

protected slots:

private:

private slots:

private: // data

    //! Connection Monitor Server specific connection ID.
    uint mConnectionId;

    //! IAP ID.
    uint mIapId;
    
    //! Connection status.
    WlanQtUtils::ConnStatus mConnectionStatus;

    // Friend classes
};

#endif // WLANQTUTILSCONNECTION_H
