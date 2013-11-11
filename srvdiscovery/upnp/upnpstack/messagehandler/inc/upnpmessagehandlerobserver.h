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


#ifndef C_MUPNPMESSAGEHANDLEROBSERVER_H
#define C_MUPNPMESSAGEHANDLEROBSERVER_H

#include <e32base.h>

#include <badesca.h>
#include "upnpmessagehandler.h"


class CUpnpMessageHandler;


class MMessageHandlerObserver
    {
public:
    virtual void DeviceListChangeL() = 0;
    
    /*   IPFIltering */
    
    virtual void IPListChange() = 0;
    /*****************/
    
    };
#endif //C_MUPNPMESSAGEHANDLEROBSERVER_H
