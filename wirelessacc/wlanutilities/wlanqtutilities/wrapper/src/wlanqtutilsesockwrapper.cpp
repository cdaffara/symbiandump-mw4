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
* Wrapper for Symbian Esock library.
*/

// System includes

#include <QScopedPointer>

// User includes

#include "wlanqtutilsesockwrapper.h"
#include "wlanqtutilsesockwrapper_symbian.h"

/*!
    \class WlanQtUtilsEsockWrapper
    \brief Wrapper for Symbian Esock library.

    Provides functionality to connect and disconnect IAPs.
*/

// External function prototypes

// Local constants

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

/*!
    Constructor.
    
    @param [in] parent Parent object.
 */

WlanQtUtilsEsockWrapper::WlanQtUtilsEsockWrapper(QObject *parent) :
    QObject(parent),
    d_ptr(new WlanQtUtilsEsockWrapperPrivate(this)),
    mLastStatusCode(KErrNone)
{
}

/*!
    Destructor.
 */

WlanQtUtilsEsockWrapper::~WlanQtUtilsEsockWrapper()
{
}

/*!
   Handles connection status update event.

   @param [in] isOpened Was the connection opened or not?
   @param [in] platformStatusCode Platform specific status code.
 */

void WlanQtUtilsEsockWrapper::updateConnection(
    bool isOpened,
    int platformStatusCode)
{
    mLastStatusCode = platformStatusCode;
	emit connectionStatusFromWrapper(isOpened);
}

/*!
   Returns last received connection creation status code. Clears status.

   @return Platform specific status code of the last connection attempt.
 */

int WlanQtUtilsEsockWrapper::lastStatusCode()
{
    // Return current status and clear it
    int status = mLastStatusCode;
    mLastStatusCode = KErrNone;
    return status;
}

/*!
   Starts connection creation to given IAP.

   @param [in] iapId IAP ID to connect.
 */

void WlanQtUtilsEsockWrapper::connectIap(int iapId)
{
    d_ptr->ConnectIap(iapId);
}

/*!
   Disconnects connection, if one is active.
 */

void WlanQtUtilsEsockWrapper::disconnectIap()
{
    d_ptr->DisconnectIap();
}
