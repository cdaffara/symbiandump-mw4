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
* ICTS Wlan Login Interface stubbing for WLAN Qt Utilities.
*/

#include "ictswlanlogininterface.h"

#include "wlanqtutilstestcontext.h"

extern WlanQtUtilsTestContext testContext;

/*!
    Constructor.
*/
IctsWlanLoginInterface::IctsWlanLoginInterface(QObject *parent)
{
    Q_UNUSED(parent);
}

/*!
    Destructor.
*/
IctsWlanLoginInterface::~IctsWlanLoginInterface()
{ 
}

/*!
    Function for launching Wlan login application.
    
    @param [in] iapId Internet accesspoint id to be tested
    @param [in] netId network identifier of the connection to be used
*/
void IctsWlanLoginInterface::start(int iapId, int netId)
{    
    Q_UNUSED(iapId);
    Q_UNUSED(netId);
}

/*!
    Function for stopping launched Wlan login application.
*/
void IctsWlanLoginInterface::stop()
{ 
}
