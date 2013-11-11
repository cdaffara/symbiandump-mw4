/** @file
 * Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
 * Description:  Declares RUpnpDispatcherSession class
 *
 */

#ifndef C_RUPNPDISPATCHERSESSION_H
#define C_RUPNPDISPATCHERSESSION_H

// INCLUDES
#include "upnpsessionbase.h"
#include "upnpcommonstructs.h"

// FORWARD DECLARATIONS


// CLASS DECLARATION

/**
 @brief Client-side interface to Message Handler

 This class provides the client-side interface to the message handler session
 */
class RUpnpDispatcherSession : public RUpnpSessionBase
    {
public:

    /**
     * Construct the object
     */
    RUpnpDispatcherSession();

    /**
     * Establish connection with server
     */
    TInt Connect();

    /**
     * Requests Message Handler to start SSDP server. This server uses
     * aIap as Internet Access Point. This way all servers are bound to
     * same interface. 
     */
    void RequestStartSsdpL() const;

    /**
     * Requests Message Handler to get state SSDP server. This server uses
     */
    void RequestCheckError( TInt& aIsStarted );

    /**
     * Requests Message Handler to tell what IAP number it is using
     * and returns it to client.
     * @param aStatus The TRequestStatus variable of the dispatcher
     * @param aIap Reference to TInt that will hold the correct IAP
     * value after call finishes.
     */
    void RequestActiveIap( TRequestStatus& aStatus, TInt& aIap );

    /**
     * Asks Message Handler to send SSDP search with given string as SSDP
     * ST -header.
     * @param aStatus The TRequestStatus variable of the dispatcher
     * @param aSearch value of ST -header.
     * @param aMX values
     */
    void RequestSsdpSearch( TRequestStatus& aStatus, TDesC8& aSearch,
        TDesC8& aMX );

    /**
     * Request the Message Handler to add a local device to the network. 
     * @param aStatus The TRequestStatus variable of the dispatcher
     * @param aDevice Structure containing buffer indexes to parse the buffer
     * @param aBuffer A buffer containing the actual data about the device
     */
    void RequestAddLocalDevice( TRequestStatus& aStatus,
        TUpnpAddLocalDevice& aDevice, TPtr8& aBuffer );

    /**
     * Request the Message Handler to remove a local device. 
     * @param aStatus The TRequestStatus variable of the dispatcher
     * @param aUuid UUID of the device that will be removed from 
     * device library.
     */
    void RequestRemoveLocalDevice( TRequestStatus& aStatus,
        const TDesC8& aUuid );

    /**
     * Request the Message Handler to remove silent a local device. 
     * @param aStatus The TRequestStatus variable of the dispatcher
     * @param aUuid UUID of the device that will be removed from 
     * device library.
     */
    void RequestRemoveSilentLocalDevice( TRequestStatus& aStatus,
        const TDesC8& aUuid );

    /**
     * Request the Message Handler to add a local control point client. 
     */
    void RequestAddControlPoint( TRequestStatus& aStatus );

    /**
     * Request the Message Handler to remove a local control point client. 
     */
    void RequestRemoveControlPoint( TRequestStatus& aStatus );

    /**
     * Request the Message Handler to get device list size. 
     * @param aUuids Uuids of devices that should not be filtered.
     */
    void RequestStopFiltering( const TDesC8& aUuids );

    /**
     * Request the Message Handler to get device list size. 
     * @param aStatus The TRequestStatus variable of the dispatcher
     * @param aSize Reference to structure that will contain numbers of
     * new devices and services.
     */
    void RequestGetDeviceListSize( TRequestStatus& aStatus,
        TUpnpGetDeviceListSize& aSize );

    /**
     * Request the Message Handler to get device list. 
     * @param aStatus The TRequestStatus variable of the dispatcher
     * @param aDevPtr Pointer to buffer that will be used to store
     * information about new devices.
     * @param aServPtr Pointer to buffer that will be used to store
     * information about new services.
     */
    void RequestGetDeviceList( TRequestStatus& aStatus, TPtr8& aDevPtr,
        TPtr8& aServPtr ) const;

    /**
     * Request stop
     */
    void RequestStop();

private:

    /**Pointer for first parameter */
    TPtr8 iParameter0;
    };

#endif // C_RUPNPDISPATCHERSESSION_H
// End of File
