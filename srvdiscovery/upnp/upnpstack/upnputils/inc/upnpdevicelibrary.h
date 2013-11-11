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
* Description:  Declares main application class.
*
*/



#ifndef C_CUPNPDEVICELIBRARY_H
#define C_CUPNPDEVICELIBRARY_H

// INCLUDES
#include "upnpdevicelibraryelement.h"

// CONSTANTS
static const TInt KDeviceTimeout=120;

// FORWARD DECLARATIONS
class TBufferIndex;
class CUpnpSsdpMessage;

// CLASS DECLARATION

/**
*  An interface from device library to Message Handler.
*  This interface class presents two pure virtual functions that are required
*  by device library.
*
*  @since Series60 2.0
*/
class MUpnpDeviceLibraryObserver
    {
    public: // New functions
        
        /**
        * A pure virtual function.
        * Advertise a device (advertisement type 
        * selected depending on parameter aLive)
        * @since Series60 2.0
        * @param aLive Expresses the advertisement type 
        * (ssdp:alive or ssdp:byebye)
        * @param anElement The device to be advertised
        */
        virtual void AdvertiseDeviceL(TInt aLive, CUpnpDeviceLibraryElement& anElement) = 0;
        
        /**
        * A pure virtual function. Advertises the device list has changed.
        * @since Series60 2.0
        */
        virtual void DeviceListChangedL() = 0;
    };

// CLASS DECLARATION

/**
*  The device library main class.
*  This class contains an array of device library elements 
*  (of type CUpnpDeviceLibraryElement). 
*  This list is maintained up-to-date by reading received SSDP messages. 
*  Every element in the library has a timeout, and remote devices are removed 
*  from the library after the timeout.
*
*  @since Series60 2.0
*/
class CUpnpDeviceLibrary : public CBase, public MUpnpTimeoutElementParent
    {
    public:

        /**
        * Two-phased constructor.
        * @param aObserver The observer of the library. 
        * @param aHandle the thread log handle
        * @return The new instance.
        */
        IMPORT_C static CUpnpDeviceLibrary* NewL(
                        MUpnpDeviceLibraryObserver& aObserver,
                        TInt aHandle );
        
        /**
        * Destructor.
        */
        IMPORT_C ~CUpnpDeviceLibrary();
        
        /**
        * Adds the data of the local device to the library.
        * @since Series60 2.0
        * @param aIndex An indexing structure to parse the actual data
        * @param aBuffer A buffer containing the actual data
        * @param aLocalAddr Local address 
        */
        IMPORT_C void AddInfoL( const TUpnpAddLocalDevice* aIndex, 
                                const TDesC8& aBuffer, 
                                const TInetAddr& aLocalAddr);

        /**
        * Adds the data of the local device to the library.
        * @since Series60 2.0
        * @param aIndex An indexing structure to parse the actual data
        * @param aBuffer A buffer containing the actual data
        */
        IMPORT_C void AddInfoL( const TUpnpAddLocalDevice* aIndex, 
                                const TDesC8& aBuffer);
        
        /**
        * Adds the data of the SSDP message to the library.
        * @since Series60 2.0
        * @param aMessage A SSDP message containing data
        */
        IMPORT_C void AddInfoL( CUpnpSsdpMessage* aMessage );
        
        /**
        * Returns the index of an element with matching UUID.
        * @since Series60 2.0
        * @param aUuid The UUID of searched device
        * @return The index of the found element; if not found, -1
        */
        IMPORT_C TInt Find( const TDesC8& aUuid );
        
        /**
        * Returns an array containing pointers to the library elements
        * @since Series60 2.0
        * @return An array containing pointers to the library elements
        */
        IMPORT_C RPointerArray<CUpnpDeviceLibraryElement>& DeviceList();
        
        /**
        * Removes an element from the library.
        * @since Series60 2.0
        * @param aUuid The UUID of the element to be removed
        */
        IMPORT_C void RemoveL( const TDesC8& aUuid );
        
         /**
        * Removes an element from the library.
        * @since Series60 2.0
        * @param aUuid The UUID of the element to be removed
        */
        IMPORT_C void RemoveSilentL( const TDesC8& aUuid );
        /**
        * Prepare for shutdown.
        * @since Series60 2.0
        */
        IMPORT_C void PrepareShutdown();
        
        /**
        * ?member_description.
        * @since Series60 2.0
        * @param aUpdateId
        * @param aDevices
        * @param aServices
        * @param aDeviceCount 
        * @param aServiceCount
        */
        IMPORT_C void GetUpdate( TInt& aUpdateId, 
                                 TUpnpDevice*& aDevices,
                                 TUpnpService*& aServices,
                                 TInt& aDeviceCount,
                                 TInt& aServiceCount );
        
        
        /**
        * Removes all remote devices from the device library,
        * used for ip address changes
        * @since Series60 3.1
        */
        IMPORT_C void RemoveAllDevicesL();
        
       /**
        * Set device notification filtering
        * @since Series60 3.2
        * @param aUuid Device Uuid
        */
        IMPORT_C void StopFilteringDeviceL( const TDesC8& aUuid );
        
    private: // Functions from base classes

        /**
        * From MUpnpTimeoutElementParent Callback function.
        * Implementation of MUpnpTimeoutElementParent::TimeoutExpired.
        * If the element is local, it is advertised and new timeout is set. 
        * If the element describes a remote device, the device is removed 
        * from the library and observer is informed about the disappearing.
        * @since Series60 2.0
        * @param aElement The element whose timeout has ran out
        */
        void TimeoutExpiredL( CUpnpTimeoutElement* aElement );
        
    protected:  // New functions

        /**
        * Remove Device.
        * @since Series60 2.0
        * @param aMessage
        */
        void RemoveDeviceL( CUpnpSsdpMessage* aMessage );

    private:    // New functions

        /**
        * Return element.
        * @since Series60 2.0
        * @param aUUID 
        * @return CUpnpDeviceLibraryElement pointer
        */
        CUpnpDeviceLibraryElement* Element( TDesC8& aUUID );
        
        /**
        * Add device.
        * @since Series60 2.0
        * @param aMessage
        * @return TBool
        */
        TBool AddDeviceL( CUpnpSsdpMessage* aMessage );
    /**
     * Creates and appends local device.
     * @param aIndex descriptiors positions structure        
             needed by deserialization algorithm    
     * @param aBuffer aBuffer CUpnpDeviceLibraryElement object in serialized form 
     * @param aLocalAddr local address of device being added
     */
    void  AppendLocalDeviceL( const TUpnpAddLocalDevice* aIndex, 
                     const TDesC8& aBuffer, 
                     const TInetAddr& aLocalAddr );
                     
    /**
     * Invalidates non local device with the same UID as
     * local device being added 
     * @param aElem
     */
    void InvalidateNonLocalDevice( CUpnpDeviceLibraryElement& aElem );

    private:

        /**
        * C++ default constructor.
        * @param aObserver The observer of the library. 
        * @param aHandle the thread log handle
        */
        CUpnpDeviceLibrary( MUpnpDeviceLibraryObserver& aObserver, TInt aHandle );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    protected:  

        TInt iUpdateId;
        
    private:    

        MUpnpDeviceLibraryObserver& iObserver;

        RPointerArray<CUpnpDeviceLibraryElement> iElementArray;

	    // for logging into right thread log
        TInt iHandle;

    };


#endif      //  DEVICELIBRARY_H

// End of File