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
* Description:  Declares the CUpnpSilentDeviceImplementation class
*
*/


#ifndef C_CUPNPSILENTDEVICEIMPLEMENTATION_H
#define C_CUPNPSILENTDEVICEIMPLEMENTATION_H


// INCLUDES
#include <e32std.h>
#include <badesca.h>
#include "upnpdeviceimplementationbase.h"


//FORWARD DECLARATIONS
class MUpnpDeviceDescriptionProvider;

/**
 *  CUpnpSilentDeviceImplementation
 *
 *  Prior using the CUpnpDevice class, client application has to create the network
 *  connection. If connection is not up and running, the
 *  CUpnpDispatcherEngine class construction will fail.
 *  See UPnP Interface Design document for instuctions.
 *
 *  @since Series60 3.2
 */
class CUpnpSilentDeviceImplementation : public CUpnpDeviceImplementationBase
    {
public:

    /**
     * Two-phased constructor.
     * The most important operations invoked inside this method:
     * 1. Start Upnp Stack (if not started yet)
     * 2. Parse device description to DOM for further updating purpose.
     *
     * @since Series60 3.2
     * @param aFilename File to create device from, name include full path.
     * @return A new CUpnpDevice instance.
     */
    IMPORT_C static CUpnpSilentDeviceImplementation* NewL( 
            const TDesC8& aUri, 
            CUpnpDeviceDescriptionStore& aDescriptionStore,
            MUpnpDeviceDescriptionProvider& aProvider );

    /**
     * Called after the device is constructed.
     * The most important operations invoked inside this method:
     * 1. Start the HTTP server.
     * 2. Generate the UUID (if missing in the device description)
     * 3. Save the device description to the file.
     * 
     * @since Series60 3.2
     * @return none
     */
    IMPORT_C virtual void StartL( const TInt aPort = KRandomPort );

    /**
     * Stops the device.
     * @since Series60 3.2
     */
    IMPORT_C virtual void StopL();

    /**
     * C++ default constructor.
     */
    CUpnpSilentDeviceImplementation();

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CUpnpSilentDeviceImplementation();

private:

    /**
     * Symbian 2nd phase constructor can leave
     * @since Series60 3.2
     * @param aFilename File to create device from
     */
    void ConstructL( const TDesC8& aUri, 
                     CUpnpDeviceDescriptionStore& aDescriptionStore,
                     MUpnpDeviceDescriptionProvider& aProvider );

    /**
     * Internal: Stops device and HTTP server
     * @since Series60 3.2
     */
    void StopDeviceL();

    };

#endif  // C_CUPNPSILENTDEVICEIMPLEMENTATION_H

// End of File
