/** @file
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
*/


#ifndef MUPNPCPBHTTPMESSAGECONTROLLER_H_
#define MUPNPCPBHTTPMESSAGECONTROLLER_H_

#include "upnpdispatchercustomer.h"

class CUpnpAction;
class CUpnpServiceInfo;
class CUpnpDevice;

class MUpnpCpbHttpMessageController :
    public MUpnpDispatcherCustomer
    {

public:

    virtual void SendL(CUpnpHttpMessage* aHttpMessage) = 0;

    virtual void SendActionL(CUpnpAction* aAction) = 0;

    virtual void SendSubscribeL(CUpnpServiceInfo* aService) = 0;

    virtual void SendUnsubscribeL(CUpnpServiceInfo* aService) = 0;

    virtual TInt GetServiceDescriptionL(CUpnpDevice* aDevice,
            const TDesC8& aServiceUrl) = 0;

    virtual TInt GetDeviceDescriptionL(CUpnpDevice* aDevice) = 0;

    virtual void SearchL( const TDesC8& aSearchString ) = 0;

    virtual void StopIgnoringEmbeddedL( CUpnpDevice* aRoot ) = 0;

    virtual void StopIgnoringUuidL( const TDesC8& aUuids ) = 0;

    /**
     * Stops ignoring all device tree
     * @since Series60 3.2
     * @param aRoot Root device
     */
    virtual void StopIgnoringL( CUpnpDevice* aDevice ) = 0;

    };

#endif /*UNPNPCPBHTTPMESSAGECONTROLLER_H_*/
