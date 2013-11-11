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

// System includes

// User includes

#include "wlanqtutilsconnection.h"

/*!
    \class WlanQtUtilsConnection
    \brief WLAN connection status handler for WLAN Qt Utilities.
*/

// External function prototypes

// Local constants

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

/*!
    Constructor. 
*/

WlanQtUtilsConnection::WlanQtUtilsConnection() :
    mConnectionId(0),
    mIapId(0),
    mConnectionStatus(WlanQtUtils::ConnStatusNone)
{
}

/*!
    Destructor. 
*/

WlanQtUtilsConnection::~WlanQtUtilsConnection()
{
}

/*!
    Connection ID getter.

    @return Connection ID specific for Connection Monitor Server.
*/

uint WlanQtUtilsConnection::connectionId() const
{
    return mConnectionId;
}

/*!
    Connection ID setter.

    @param [in] connectionId Connection Monitor Server Connection ID.
*/

void WlanQtUtilsConnection::setConnectionId(uint connectionId)
{
    mConnectionId = connectionId;
}

/*!
    IAP ID getter.

    @return IAP ID.
*/

uint WlanQtUtilsConnection::iapId() const
{
    return mIapId;
}

/*!
    IAP ID setter.

    @param [in] iapId IAP ID.
*/

void WlanQtUtilsConnection::setIapId(uint iapId)
{
    mIapId = iapId;
}

/*!
    Connection status getter.

    @return Status of the connection.
*/

WlanQtUtils::ConnStatus WlanQtUtilsConnection::connectionStatus() const
{
    return mConnectionStatus;
}

/*!
    Connection status setter.

    @param [in] connectionStatus Status of the connection.
*/

void WlanQtUtilsConnection::setConnectionStatus(
    WlanQtUtils::ConnStatus connectionStatus)
{
    mConnectionStatus = connectionStatus;
}
