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


#ifndef C_CUPNPDEVICELIBRARYELEMENT_H
#define C_CUPNPDEVICELIBRARYELEMENT_H

// INCLUDES
#include <e32std.h>
#include <cdbcols.h>
#include <badesca.h>
#include "upnptimeoutelement.h"
#include "upnpcommonstructs.h"

// FORWARD DECLARATIONS
class CUpnpSsdpMessage;

// CLASS DECLARATION

/**
*  This class has two main functionalities:
*  1. Timeout from CUpnpTimeoutElement
*  2. Buffers to store the data about a UPnP device
*
*  @since Series60 2.0
*/
class CUpnpDeviceLibraryElement : public CUpnpTimeoutElement
    {
    public: // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aParent The parent of the element (e.g. device library) 
        * to handle timeout callbacks
        * @return A new CUpnpDeviceLibraryElement instance
        */
        IMPORT_C static CUpnpDeviceLibraryElement* NewL(
            MUpnpTimeoutElementParent& aParent);
        
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CUpnpDeviceLibraryElement();
        
    public: // New functions

        /**
        * Add data of a SSDP message to the device library
        * @since Series60 2.0
        * @param aMessage A SSDP message
        * @param aUpdateId Reference to current update id. If required \
            the value of this update id is changed when device is added to \
            device library.
        * @return ETrue if device changed
        */
        IMPORT_C TBool AddInfoL( CUpnpSsdpMessage* aMessage, TInt& aUpdateId );

        /**
        * Add data got from local device to the device library.
        * @since Series60 2.0
        * @param aIndex A indexing structure for the data buffer
        * @param aBuffer A buffer containing the actual data
        * @param aAddr Address of the device that is to be added.
        */
        IMPORT_C void AddInfoL( const TUpnpAddLocalDevice* aIndex, 
                                const TDesC8& aBuffer, 
                                const TInetAddr& aAddr );

        /**
        * Add data got from local device to the device library.
        * @since Series60 2.0
        * @param aIndex A indexing structure for the data buffer
        * @param aBuffer A buffer containing the actual data
        */
        IMPORT_C void AddInfoL( const TUpnpAddLocalDevice* aIndex, 
                                const TDesC8& aBuffer);

    public:

        /**
        * Sets iAdvertised to ETrue.
        * @since Series60 3.2
        */
        void Advertising();

        /**
        * Sets iAdvertised to EFalse.
        * @since S60 5.1
        */
        void AdvertisingFinished();
        
        /**
        * Is advertised.
        * @since Series60 3.2
        * @return TBool
        */
        TBool Advertised();

        /**
        * Set root device.
        * @since Series60 2.0
        * @param aRootDevice root device
        */
        void SetRootDevice( TBool aRootDevice );

        /**
        * Return local.
        * @since Series60 2.0
        * @return TBool
        */
        IMPORT_C TBool Local() const;

        /**
        * Set local.
        * @since Series60 2.0
        * @param aLocal if local or not
        */
        void SetLocal( TBool aLocal );

        /**
        * Is alive.
        * @since Series60 2.0
        * @return TBool
        */
        IMPORT_C TBool Alive() const;

        /**
        * SetAlive.
        * @since Series60 2.0
        */
        IMPORT_C void SetAlive(TBool aAlive);

        /**
        * Should be filtered.
        * @since Series60 3.2
        * @return TBool
        */
        TBool Filter() const;

        /**
        * SetFilter.
        * @since Series60 2.0
        */
        void SetFilter(TBool aFilter);

        /**
        * Idicate if device cache-control time expired.
        * @since Series60 3.2
        * @return ETrue if expired, otherwise EFalse
        */
        TBool Expired() const;

        /**
        * Setter.
        * @since Series60 3.2
        */
        void SetExpired( TBool aExpired );

        /**
        * Set update id.
        * @since Series60 2.0
        * @param aUpdateId update ID
        */
        IMPORT_C void SetUpdateId( TInt aUpdateId );

        /**
        * Return update ID.
        * @since Series60 2.0
        * @return TInt update ID
        */
        IMPORT_C TInt UpdateId() const;

        /**
        * ?member_description.
        * @since Series60 2.0
        * @return ?description
        */
        IMPORT_C TUpnpDevice* AsTDeviceL() const;

        /**
        * Returns true, if the element describes a root device.
        * @since Series60 2.0
        * @return ETrue, if root device; EFalse otherwise
        */
        IMPORT_C TBool IsRootDevice() const;
        
        /**
        * Returns the UUID of this element.
        * @since Series60 2.0
        * @return Descriptor containing the UUID.
        */
        IMPORT_C TDesC8& UUID();

        /**
        * Returns the description URL of this element.
        * @since Series60 2.0
        * @return Descriptor containing the description URL.
        */
        IMPORT_C TDesC8& DescriptionUrl();

        /**
        * Returns the domain of this element.
        * @since Series60 2.0
        * @return Descriptor containing the domain.
        */
        IMPORT_C TDesC8& Domain();

        /**
        * Returns the device type of this element.
        * @since Series60 2.0
        * @return Descriptor containing the device type.
        */
        IMPORT_C TDesC8& DeviceType();

        /**
        * Returns an array containing the service types of the services 
        * of this element.
        * @since Series60 2.0
        * @return Descriptor array containing the service types.
        */
        IMPORT_C CDesC8Array& ServiceList();
        
    private:
        
        /**
        * C++ default constructor.
        * @param aParent The parent of the element (e.g. device library) 
        * to handle timeout callbacks
        */
        CUpnpDeviceLibraryElement( MUpnpTimeoutElementParent& aParent );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
    private:
        
        /**
        * Sets the UUID of a device to aUUID.
        * @since Series60 2.0
        * @param aUUID UUID value that is to be set for this element.
        */
        void SetUuidL( const TDesC8& aUUID );

        /**
        * Sets the url to this device's description file.
        * @since Series60 2.0
        * @param aDescriptionURL New value of description url.
        */
        void SetDescriptionUrlL( const TDesC8& aDescriptionURL );
        
        /**
        * Sets the url to this device's description file.
        * @param aDescriptionURL New value of description url.
        * @param aAddr Address of the device.
        * @param aPort Port of the device.
        */
        void SetDescriptionUrlL( const TDesC8& aDescriptionURL,
                                 const TInetAddr& aAddr,
                                 const TDesC8& aPort );

        /**
        * Sets the domain value for this element.
        * @since Series60 2.0
        * @param aDomain New value for Domain.
        */
        void SetDomainL( const TDesC8& aDomain );

        /**
        * Sets the device's type (for example MediaServer:1).
        * @since Series60 2.0
        * @param aDeviceType New value for device type.
        */
        void SetDeviceTypeL( const TDesC8& aDeviceType );

        /**
        * Adds a service to this element. Service type is aServiceType
        * (ConnectionManager:1 for example).
        * @since Series60 2.0
        * @param aServiceType Name of service that is to be added.
        */
        void AddServiceTypeL( const TDesC8& aServiceType );
        
        /**
        * Add data (only from Notification Type Header) of a SSDP message to the device library
        * @param aMessage A SSDP message
        * @return ETrue if device changed
        */
        TBool AddInfoFromNotificationTypeHeaderL( CUpnpSsdpMessage* aMessage);

        /**
        * Check if a aDescriptor has prefix aPrefix 
        * @param aDescriptor
        * @param aPrefix
        * @return TBool
        */
        static TBool HasPrefix( const TDesC8& aDescriptor, const TDesC8& aPrefix );

    protected:  
        TInt iUpdateId;
    private:    
        TBool iAlive;
        TBool iFilter;
        TBool iLocal;
        TBool iExpired;
        TBool iRootDevice;
        HBufC8* iUUID;
        HBufC8* iDeviceType;
        HBufC8* iDomain;
        HBufC8* iDescriptionURL;
        CDesC8ArrayFlat* iServiceList;
        TBool iAdvertised;
    };

#endif      // C_CUPNPDEVICELIBRARYELEMENT_H

// End of File