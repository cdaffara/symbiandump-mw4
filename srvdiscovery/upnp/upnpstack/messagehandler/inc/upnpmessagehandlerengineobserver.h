/** @file
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available 
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*    
*
*/


#ifndef C_MUPNPMESSAGEHANDLERENGINEOBSERVER_H
#define C_MUPNPMESSAGEHANDLERENGINEOBSERVER_H

#include <e32base.h>

#include <badesca.h>
#include "upnpmessagehandlerengine.h"


class CUpnpMessageHandlerEngine;


class MMessageHandlerEngineObserver
    {
public:

    virtual void DeviceListUpdateL() = 0;
    
    /****IPFIltering***/
    
    virtual void IPListChange() = 0;
    /*******/
    
    // return symbian server session array
    virtual TDblQueIter< CSession2 >& Sessions() = 0;
    };

#endif //C_MUPNPMESSAGEHANDLERENGINEOBSERVER_H
