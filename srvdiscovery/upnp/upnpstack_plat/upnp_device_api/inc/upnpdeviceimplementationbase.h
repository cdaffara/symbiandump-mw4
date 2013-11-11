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
* Description:  Declares the CUpnpDeviceImplementationBase class
*
*/

#ifndef C_CUPNPDEVICEIMPLEMENTATIONBASE_H
#define C_CUPNPDEVICEIMPLEMENTATIONBASE_H

// INCLUDES
#include <e32std.h>
#include <badesca.h>
#include <upnpcons.h>
#include "upnpservice.h"
#include "upnpdiscoveryobserver.h"
#include "upnpdescriptionproperty.h"
#include "upnpdevice.h"


// FORWARD DECLARATIONS
class TUpnpDevice;
class TUpnpService;
class CUpnpIcon;
class CUpnpDesciptionProperty;
class CUpnpDeviceDescriptionStore;
class CUpnpHttpServerTransactionHandler;
class MUpnpDeviceDescriptionProvider;

/**
*  Device
*
*  Prior using the CUpnpDevice class, client application has to create the network
*  connection. If connection is not up and running, the
*  CUpnpDispatcherEngine class construction will fail.
*  See UPnP Interface Design document for instuctions.
*
*  @since Series60 5.0_HN
*/
class CUpnpDeviceImplementationBase : public CUpnpDevice
    {
    
public: // Constructors and destructor
    
    /**
    * Destructor.
    */
    IMPORT_C virtual ~CUpnpDeviceImplementationBase();

    /**
    * Starts the device.
    * Pure virtual function.
    * @since Series60 3.2
    * @param aPort Port on which HTTP server should be started.
    */
    virtual void StartL( const TInt aPort = KRandomPort ) = 0;
   
    /**
    * Stops the device.
    * Pure virtual function.
    * @since Series60 3.2
    */
    virtual void StopL() = 0;
    
    /**
    * Sets description property for the device.
    * This method changes only internal attributes of the Device object
    * they are automatically saved in the description file by the StartL method
    * @since Series60 3.0
    */
    IMPORT_C void SetDescriptionPropertyL( const TDesC8& aPropertyName,
                                           const TDesC8& aPropertyValue );
    
    /**
    * Removes tag from device description xml
    * @param aPropertyName the name of the element that should be removed
    */
    IMPORT_C void RemoveDescriptionPropertyL( const TDesC8& aPropertyName );
    
    /**
    * Removes icon of given url.  
    * Device performs the following operations:
    * - deletes respective icon element from description XML flie 
    * @param aUrl icon url
    */
    IMPORT_C void RemoveIconL( const TDesC8& aUrl);

    /**
    * Removes all icons
    * Device performs the following operations:
    * - deletes all icon elements from description XML flie 
    */
    IMPORT_C void RemoveIconsL();

    /**
    * Adds new icon to a device description. 
    * Device performs the following operations:
    * - adds icon element to its device description XML file 
    * @param aIcon icon reference
    */   
    IMPORT_C void AddIconL( const CUpnpIcon& aIcon );

    /**
    * Updates icon of given url.  
    * Device performs the following operations:
    * - replaces respective icon element from description XML flie
    *   url value remains unchanged 
    * @param aIcon icon reference
    */
    void UpdateIconL( const CUpnpIcon& aIcon );    
              
    /**
    * Generates Universally Unique Identifier using good algorithm.
    * @since Series60 3.2
    * @return A new UUID in following format:
    * "uuid:01234567-0123-0123-0123-012345678901"
    */
    HBufC8* GenerateUuidL();
               
    /**
    * Creates dispatcher to iDispatcher-member.
    * @since Series60 3.2
    */
    void CreateDispatcherL();

    /**
    * C++ default constructor.
    */
    CUpnpDeviceImplementationBase();
                                      
    /**
    * Getter for address of the device
    * @since Series S60 3.1
    * @return TInetAddr
    */        
    IMPORT_C virtual TInetAddr Address();    
   
    /**
    * Adds the service to dispatcher
    * @since Series S60 3.1
    * @param aService 
    */
    void AttachServiceL( CUpnpService *aService );
            
    /**
    * For internal use. Sets the dispatcher pointer of this device
    * @since Series S60 3.1
    * @param aDispatcher an instance of dispatcher 
    */
    void SetDispatcher( CUpnpDispatcher* aDispatcher ); 
    
    /**************IPFILTERING*************/
    
    IMPORT_C void StartIPFilteringL();
    IMPORT_C void StopIPFiltering();
	/**************************************/

    MUpnpDeviceDescriptionProvider& DescritptionProvider();
    
protected:    // New functions

    /**
    * Starts HTTP Server if the device is a root device.
    * @since Series60 5.0_HN
    * @param aPort Port on which HTTP server should be started.
    */
    void StartHttpServerL( const TInt aPort );

    /**
    * Stops HTTP Server.
    * @since Series60 5.0_HN
    */
    void StopHttpServer();
       
    /**
    * Generates a new uuid for the device if it already does not exist.
    * @since Series60 3.2
    * @param aFileName Device description.
    */
    void CheckUuidGenL( const TDesC16& aFileName );
  
    /**
    * Getter for dispatcher.
    * @since Series60 3.2
    * @return Pointer used dispatcher.
    */
    CUpnpDispatcher* Dispatcher();
           
    /**
    * Symbian 2nd phase constructor can leave
    * @since Series60 5.0_HN
    * @param aFilename File to create device from
    */
    void BaseConstructL( const TDesC8& aDeviceDescriptioUri,
                         CUpnpDeviceDescriptionStore& aDescriptionStore, 
                         MUpnpDeviceDescriptionProvider& aProvider );
    
    /**
    * Recursively generates udns
    * @since Series60 3.2
    * @param aDevice device implementation
    */
    void GenerateUdnsL( CUpnpDeviceImplementationBase* aDevice );
        
    /**
    * Assigns a dispatcher to all embedded devices
    * @since Series60 3.2
    */
    void RegisterEmbeddedDevices();
    
    /**
    * Removes all embedded devices from dispatcher
    * @since Series60 3.2
    */
    void UnregisterEmbeddedDevicesL();

    /**
    * Saves description store.
    * @since Series60 5.0_HN
    */
    void SaveDescriptionStoreL();

protected:    // Data
    
    // dipatcher
    // owned
    CUpnpDispatcher* iDispatcher;

    // path
    // owned
    HBufC8* iPath;
        
    // owned
    CUpnpHttpServerTransactionHandler* iServerTransactionHandler; 

    // description store
    // not owned
    CUpnpDeviceDescriptionStore* iDescriptionStore;   

    // not owned
    MUpnpDeviceDescriptionProvider* iDescriptionProvider;
    
    };

#endif  // C_CUPNPDEVICEIMPLEMENTATIONBASE_H

// End of File
