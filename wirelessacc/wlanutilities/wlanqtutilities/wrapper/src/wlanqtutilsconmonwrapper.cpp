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
* Wrapper for Symbian Connection Monitor library.
*/

// System includes

#include <QSharedPointer>
#include <QScopedPointer>

// User includes

#include "wlanqtutilsconmonwrapperdisconnect_symbian.h"
#include "wlanqtutilsconmonwrapperinfo_symbian.h"
#include "wlanqtutilsconmonwrapper.h"

/*!
    \class WlanQtUtilsConMonWrapper
    \brief Wrapper for Symbian Connection Monitor library.

    Provides functionality to retrieve connection information,
    and to disconnect connections.
*/

// External function prototypes

// Local constants

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

/*!
    Constructor.
    
    @param [in] parent Parent object.
*/

WlanQtUtilsConMonWrapper::WlanQtUtilsConMonWrapper(QObject *parent) :
    QObject(parent),
    d_ptrInfo(new WlanQtUtilsConMonWrapperInfo(this)),
    d_ptrDisconnect(new WlanQtUtilsConMonWrapperDisconnect(this))
{
}

/*!
    Destructor.
*/

WlanQtUtilsConMonWrapper::~WlanQtUtilsConMonWrapper()
{
}

/*!
   Return active connection information.

   @return Information of active connection, 0 if not found.
*/ 

WlanQtUtilsConnection *WlanQtUtilsConMonWrapper::activeConnection() const
{
    return d_ptrInfo->ActiveConnection();
}

/*!
   Returns information of a connection with the given connection ID.

   @param [in] connectionId Connection ID.

   @return Information of the given connection, 0 if not found.
*/ 

WlanQtUtilsConnection* WlanQtUtilsConMonWrapper::connectionInfo(
    uint connectionId) const
{
    return d_ptrInfo->ConnectionInfo(connectionId);
}

/*!
   Stops given connection regardless of how many applications are using it.

   @param [in] iapId IAP ID to disconnect.
*/ 

void WlanQtUtilsConMonWrapper::disconnectIap(int iapId)
{
   d_ptrDisconnect->DisconnectConnection(iapId);
}
