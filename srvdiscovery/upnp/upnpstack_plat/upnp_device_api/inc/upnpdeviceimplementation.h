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
* Description:  Declares the CUpnpDevice class
*
*/

#ifndef C_CUPNPDEVICEIMPLEMENTATION_H
#define C_CUPNPDEVICEIMPLEMENTATION_H

// INCLUDES
#include <e32std.h>
#include <badesca.h>
#include "upnpdeviceimplementationbase.h"

class MUpnpDeviceDescriptionProvider;

/**
*  Device
*
*  Prior using the CUpnpDevice class, client application has to create the network
*  connection. If connection is not up and running, the
*  CUpnpDispatcherEngine class construction will fail.
*  See UPnP Interface Design document for instuctions.
*
*  @since Series60 3.2
*/
class CUpnpDeviceImplementation : public CUpnpDeviceImplementationBase
    {
public:
     
    enum TStopMode
        {
        EStopNormal,
        EStopSilent
        };

    /**
    * Two-phased constructor.
    * The most important operations invoked inside this method:
    * 1. Start Upnp Stack (if not started yet)
    * 2. Parse device description to DOM for further updating purpose.
    *
    * @since Series60 3.2
    * @param aDescriptionStore Device description store reference
    * @param aDeviceDescriptioUri Uri of device description xml used in 
    *        discovery and description layers of Upnp device architecture 
    * @param aProvider description provider which resolves file paths for
    *        description uri's: device and service descriptions and icons       
    * @return A new CUpnpDeviceImplementation instance.
    */
    IMPORT_C static CUpnpDeviceImplementation* NewL( const TDesC8& aDeviceDescriptioUri,
                                                     CUpnpDeviceDescriptionStore& aDescriptionStore,
                                                     MUpnpDeviceDescriptionProvider& aProvider );

    /**
    * Called after the device is constructed.
    * The most important operations invoked inside this method:
    * 1. Start the HTTP server.
    * 2. Generate the UUID (if missing in the device description)
    * 3. Save the device description to the file.
    * 4. Advertise the device in a network 
    *    (send two ssdp:bye-bye/ and two ssdp:alive messages).
    * 
    * @since Series60 3.2
    */
    IMPORT_C virtual void StartL( const TInt aPort = KRandomPort );        

    /**
    * Stops the device. It equals to the Stop( EStopNormal ). 
    * @since Series60 3.2
    */
    IMPORT_C virtual void StopL();

    /**
    * Stops the device.
    * Most important operations:
    * 1. Device is removed from Message Handler/SSDP Server device library. 
    * 2. HTTP server is stopped.
    *
    * @param aMode one of the following modes:
    *    - EStopNormal - ssdp:bye-bye messages are sent before 
    *                    the device is removed from the library     
    *    - EStopSilent - device is removed from the library silently 
    *                    without ssdp:bye-bye messages    
    * @since Series60 3.2
    */
    IMPORT_C void StopL( TStopMode aMode );
    
    /**
    * C++ default constructor.
    */
    CUpnpDeviceImplementation();

    /**
    * Destructor.
    */
    IMPORT_C virtual ~CUpnpDeviceImplementation();

private:

    /**
    * Symbian 2nd phase constructor can leave
    * @since Series60 3.2
    * @param aFilename File to create device from
    */
    void ConstructL( const TDesC8& aDeviceDescriptioUri, 
                     CUpnpDeviceDescriptionStore& aDescriptionStore,
                     MUpnpDeviceDescriptionProvider& aProvider );

    /**
    * Internal: Stops device and HTTP server
    * @since Series60 3.2
    */    
    void StopDeviceL( TStopMode aMode );

     /**
    * Internal: Adds device
    * @since Series60 3.2
    */ 
    void AddLocalDeviceL();

     /**
    * Internal: Removes device
    * @since Series60 3.2
    */ 
    void RemoveLocalDeviceL( const TDesC8& aUuid, TBool aSilent );
    };

#endif  // C_CUPNPDEVICEIMPLEMENTATION_H

// End of File
