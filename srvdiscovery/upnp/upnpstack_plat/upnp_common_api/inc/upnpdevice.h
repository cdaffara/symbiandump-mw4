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


#ifndef C_CUPNPDEVICE_H
#define C_CUPNPDEVICE_H

// INCLUDES

#include <e32std.h>
#include <badesca.h>
#include "upnpservice.h"
#include "upnpdescriptionproperty.h"

// FORWARD DECLARATIONS
class TUpnpDevice;
class TUpnpService;
class CUpnpIcon;
class CUpnpDescriptionProperty;

// CONSTANTS
const TInt KDefaultDescriptionUrlPort = 80;

/**
 *  Device
 *
 *  Prior using the CUpnpDevice class, client application has to create the network
 *  connection. If connection is not up and running, the
 *  CUpnpDispatcherEngine class construction will fail.
 *  See UPnP Interface Design document for instuctions.
 *
 *  @since Series60 2.6
 */
class CUpnpDevice : public CBase
    {

public:
    /**
     * Localization of device (local, home network or remote)
     */
    enum TUpnpDeviceNetwork
        {
        EUnknown = 0,
        ELocalDevice,
        EHomeNetwork,
        ERemoteDevice
        };
public:
    // Constructors and destructor

    /**
     * Two-phased constructor.
     * @param aDevice Device to create new device from.
     * @param aServices Services for the new device.
     * @return A new CUpnpDevice instance.
     */
    IMPORT_C static CUpnpDevice* NewL( const TUpnpDevice* aDevice,
            const RPointerArray<TUpnpService>& aServices );

    /**
     * Destructor.
     */
    virtual ~CUpnpDevice();

public: // New functions

    // Device description XML-inferface functions

    /**
     * XML-inferface function.
     * @since Series60 2.6
     * @return Content of deviceType-tags in device description. KNullDesC8
     * if not found.
     */
    IMPORT_C const TPtrC8 DeviceType();

    /**
     * XML-inferface function.
     * @since Series60 2.6
     * @return Content of friendlyName-tags in device description.
     * KNullDesC8 if not found.
     */
    IMPORT_C const TPtrC8 DescriptionProperty(const TDesC8& aPropertyName);

    // servicelist

    /**
     * XML-inferface function.
     * @since Series60 2.6
     * @return Array of listed servicetyped in device description
     * service. KNullDesC8 if not found.
     */
    IMPORT_C CDesC8Array& ServiceTypesL();

    /**
     * XML-inferface function.
     * @since Series60 2.6
     * @param aServiceType Type of the service.
     * @return Content of serviceId-tags in device description of asked
     * service. KNullDesC8 if not found.
     */
    IMPORT_C const TPtrC8 ServiceId( const TDesC8& aServiceType );

    /**
     * XML-inferface function.
     * @since Series60 2.6
     * @param aServiceType Type of the service.
     * @return Content of SCDPURL-tags in device description of asked
     * service. KNullDesC8 if not found.
     */
    IMPORT_C const TPtrC8 ServiceDescriptionUrl( const TDesC8& aServiceType );

    // end servicelist

    /**
     * Getter.
     * @since Series60 2.6
     * @return iServiceList.
     */
    IMPORT_C RPointerArray<CUpnpService>& ServiceList();

    /**
     * Getter (embedded devices).
     * @since Series60 2.6
     * @return iDeviceList.
     */
    IMPORT_C RPointerArray<CUpnpDevice>& DeviceList();

    /**
     * Gets whole embedded hierarchy as a flat list.
     * @since Series60 2.6
     * @return device list.
     */
    IMPORT_C void GetAllDevices( RPointerArray<CUpnpDevice>& aDevices );

    /**
     * Getter.
     * @since Series60 2.6
     * @return iIcons.
     */
    IMPORT_C RPointerArray<CUpnpIcon>& Icons();

    /**
     * Getter.
     * @since Series60 2.6
     * @return ETrue if alive.
     */
    IMPORT_C TBool Alive() const;

    /**
     * Setter.
     * @since Series60 2.6
     * @param aAlive Desired TBool.
     */
    IMPORT_C void SetAlive( TBool aAlive );

    /**
     * Getter.
     * @since Series60 3.2
     * @return ETrue if device expired.
     */
    IMPORT_C TBool Expired() const;

    /**
     * Setter.
     * @since Series60 3.2
     * @param aAlive Desired TBool.
     */
    IMPORT_C void SetExpired( TBool aAlive );

    /**
     * Getter.
     * @since Series60 2.6
     * @return ETrue if local.
     */
    IMPORT_C TBool Local() const;

    /**
     * Setter.
     * @since Series60 2.6
     * @param aLocal Desired TBool.
     */
    IMPORT_C void SetLocal( TBool aLocal );

    /**
     * Getter.
     * @since Series60 5.0
     * @return ETrue if remote.
     */
    IMPORT_C TBool Remote() const;

    /**
     * Getter.
     * @since Series60 5.0
     * @return Device network type.
     */
    IMPORT_C TUpnpDeviceNetwork DeviceNetwork() const;

    /**
     * Setter.
     * @since Series60 5.0
     * @param aNetworkType Desired network.
     */
    IMPORT_C void SetDeviceNetwork( TUpnpDeviceNetwork aNetworkType );

    /**
     * Getter.
     * @since Series60 2.6
     * @return UUID of the device.
     */
    IMPORT_C const TPtrC8 Uuid();

    /**
     * Setter.
     * @since Series60 3.2
     * @param aUUID New device UUID.
     */
    void SetUuidL( const TDesC8& aUUID );
	
	/**
     * Adds device
	 * @param device 
     * @since Series60 2.6
     */
    void AddDeviceL( CUpnpDevice* device );

    /**
     * Getter.
     * @since Series60 2.6
     * @return iDescriptionUrl. KNullDesC8 if not set.
     */
    IMPORT_C const TDesC8& DescriptionUrl() const;
	
	/**
     * Sets description Url
	 * @param aDescriptionUrl 
     * @since Series60 2.6
     */
    void SetDescriptionUrlL( const TDesC8& aDescriptionUrl);
    
    /**
     * Getter
     * @since Series60 2.6
     * @return DescriptionUrlPath.
     * 
     * @note Method for internal use.
     */
    IMPORT_C const TPtrC8 DescriptionUrlPath() const;

    /**
     * Getter.
     * @since Series60 2.6
     * @return DescriptionUrlAddressL.
     */
    IMPORT_C TInetAddr DescriptionUrlAddressL() const;

    /**
     * Getter.
     * @since Series60 2.6
     * @return DescriptionUrlPort. KErrNotFound if invalid.
     * 
     * @note Method for internal use.
     */
    IMPORT_C TInt DescriptionUrlPort() const;

    /**
     * Add service to dispatcher.
     * @since Series60 2.6
     * @param aService Attached service.
     */
    void AttachServiceL( CUpnpService* aService );

    /**
     * Sets the address of this device.
     * @since Series60 2.6
     * @param aAddr Address.
     */
    IMPORT_C void SetAddress( const TInetAddr& aAddr );

    /**
     * Returns the address of this device.
     * @since Series60 2.6
     * @return Address of device.
     */
    IMPORT_C virtual TInetAddr Address();

    /**
     * Adds aSessionId to list, that contains sessionIds
     * of currently pending service HTTP GET requests.
     * @since Series60 2.6
     * @param aSessionId Session ID.
     */
    IMPORT_C void WaitServiceDescriptionL( TInt aSessionId );

    /**
     * Returns count of currently pending service HTTP GET
     * requests.
     * @since Series60 2.6
     * @return Number of service descriptions which aren't yet received.
     */
    IMPORT_C TInt WaitServiceDescriptionCount();

    /**
     * Returns a sessionId of HTTP GET in array, that has
     * index of aIndex.
     * @since Series60 2.6
     * @param aIndex Index of sessionId.
     * @return SessionId.
     */
    IMPORT_C TInt WaitServiceDescriptionSessionId(TInt aIndex);

    /**
     * Removes session id from device's service receiving list.
     * This sessionId is in array position of aIndex.
     * @since Series60 2.6
     * @param aIndex Index of sessionId.
     */
    IMPORT_C void WaitServiceDescriptionRemoveSessionId( TInt aIndex );

    enum TServiceAdd
        {
        ENotServiceAdded,
        EServiceAdded,
        EAllServicesAdded
        };

    /**
     * For Control point. State of icon receiving.
     * @since Series60 2.6
     * @return Refer to enum TServiceAdd for possible values.
     */
    TServiceAdd ServiceDescriptionReceiveState() const
        {
        return iServiceDescriptionReceiveState;
        }

    /**
     * Adds aSessionId into list of currently pending icon requests.
     * @since Series60 2.6
     * @param aSessionId Session ID.
     */
    IMPORT_C void WaitIconL( TInt aSessionId );

    /**
     * Adds a service to this device. Returns EAllServicesAdded, if
     * information about all services on this device has been received.
     * Else returns EServiceAdded.
     * @since Series60 2.6
     * @param aSessionId Session ID.
     * @param aHttpMsgBody   Body of a HTTP-message.
     * @return Refer to enum TServiceAdd for possible values.
     */
    IMPORT_C TServiceAdd AddServiceL( TInt aSessionId,
            CUpnpService* aService );

    enum TIconAdd
        {
        ENotIconAdded,
        EIconAdded,
        EAllIconsAdded
        };

    /**
     * For Control point. State of icon receiving.
     * @since Series60 2.6
     * @return Refer to enum TIconAdd for possible values.
     */
    TIconAdd IconReceiveState() const
        {
        return iIconReceiveState;
        }

    /**
     * Adds a icon to this device. Returns EAllIconsAdded, if
     * all icons for this device has been downloaded. Else returns
     * EIconAdded.
     * @since Series60 2.6
     * @param aSessionId Session ID.
     * @return Refer to enum TIconAdd for possible values.
     */
    IMPORT_C TIconAdd AddIcon(TInt aSessionId);

    /**
     * Concatenate given URL with URLBase if exists
     * @param URL to concatanate
     * @return concatenated URL
     */
    IMPORT_C HBufC8* ConcatWithUrlBaseL(const TDesC8& aUrl);

    /**
     * Get information from device description.
     * Given parameter is name of XML-tag in device description.
     * @param aProperty name of element in device description
     * @return value of given element or empty value
     */
    IMPORT_C TPtrC8 GetProperty(const TDesC8& aProperty);
	
	/**
     * Sets type of device
	 * @param aType 
     * @since Series60 2.6
     */
    void SetTypeL(const TDesC8& aType);
	
	/**
     * Adds a specific device property
	 * @param aDeviceProperty 
     * @since Series60 2.6
     */
    void AddDevicePropertyL( CUpnpDescriptionProperty* aDeviceProperty);

	/**
     * Adds icon
	 * @param aIcon 
     * @since Series60 2.6
     */
    IMPORT_C void AddIconL( CUpnpIcon* aIcon);

    /**
     * Get information about service IDs    
     * @return array of service ids
     */
    IMPORT_C CDesC8Array& ServiceIdL();
	
	/**
     * Makes a copy of device
     * @since Series60 2.6
     * @return CUpnpDevice
     */
    IMPORT_C CUpnpDevice* CloneL();

    /**
     * Removes service from iServiceList.
     * @since Series60 3.2
     * @param aService Service to detach.
     */
    void DetachService( CUpnpService *aService );

public: // Functions from base classes

    /**
     * Removes IP address in front of the url leaving only path to file.
     * @since Series60 2.6
     * @return The path part of url.
     */
    static TPtrC8 RemoveIpAddress( const TDesC8& aAddress );
	
	/**
     * Marks out that the device is root
	 * @param aIsRoot
     * @since Series60 2.6
     */
    void SetRootDevice(TBool aIsRoot);

    /**
     * C++ default constructor.
     */
    CUpnpDevice();

private:

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL( const TUpnpDevice* aDevice,
            const RPointerArray<TUpnpService>& aServices );

    /**
     * Two-phased constructor.
     */
    static CUpnpDevice* NewL( const TDesC8& aDescription );

    /**
     * This function is used for removing old icon request session 
     * id from list and adding a new one.
     * @since Series60 2.6
     * @param aOldSessionId Old session id that is to be removed from list.
     * @param aNewSessionId New session id that is to be added to list.
     * @return KErrNone if ok, otherwise error code.
     */
    TInt ChangeIconSessionIdL( const TInt aOldSessionId,
            const TInt aNewSessionId );

    /**
     * This function checks if given session id exists in 
     * @since Series60 2.6
     * @param aSessionId Session id that is searched from the list.
     * @return index in array or KErrNotFound.
     */
    TInt CheckIconSessionIdExist( const TInt aSessionId );

public: //Getters for some xml content.

    /**
     * This function gives <URLBase> tag value.
     */
    IMPORT_C const TPtrC8 UrlBase();

    /**
     * This function gives <presentationUrl> value. 
     */
    IMPORT_C const TPtrC8 PresentationUrl();

    /**
     * This function set UrlBase value if it is empty in xml document.
     */
    IMPORT_C void SetUrlBaseL(const TDesC8& aUrlBase);

    /**
     *
     * @since Series S60 3.1
     * @return ETrue if it is an embbeded device
     */
    IMPORT_C TBool IsEmbeddDevice();

    /**
     * General initializations, internal usage.
     * @since Series60 2.6
     */
    void Init();

    /**
     * Cleanups array
     * @since Series S60 3.2
     */
    void CleanupServiceArray();

protected: // Data

    // time value
    TInt64 iTimeValue;

    // list of session IDs of icons not yet arrived
    RArray<TInt> iIconSessionIds;

    // list of session IDs of Service Descriptions not yet arrived
    RArray<TInt> iServiceDescriptionSessionIds;

    // list of icons this device has.
    RPointerArray<CUpnpIcon> iIcons;

    // this is a pointer list to service infos contained in device description
    // RPointerArray<CUpnpService> iServices;

    // this is a pointer list to the CUpnpDevice structures
    // contains embedded devices of this device
    RPointerArray<CUpnpDevice> iDeviceList;

    // this is a pointer list to the CUpnpService structures; 
    // real service structures based on service descriptions
    RPointerArray<CUpnpService> iServiceList;

    // State of adding services. When all services has been added
    // to this device, iServiceDescriptionReceiveState is EAllServicesAdded
    TServiceAdd iServiceDescriptionReceiveState;

    // State of adding icons. When all icons has been received,
    // iIconReceiveState is EAllIconsAdded.
    TIconAdd iIconReceiveState;

    // member variables for CUpnpDevice that has the data of device description
    CDesC8ArrayFlat* iServiceTypes;

    // alive flag
    TBool iAlive;

    // Indicate if device cache-control time expired
    TBool iExpired;

    // icons flag
    TBool iIsAllIconsArrived;

    // Network type (local, home or remote).
    TUpnpDeviceNetwork iNetworkType;

    // Url to this device's Device Description Document.
    HBufC8* iDescriptionURL;

    // device type
    HBufC8* iDeviceType;

    // domain
    HBufC8* iDomain;

    // device UDN without "uuid:"-prefix
    HBufC8* iUUID;

    // seq
    HBufC8* iClockSeq;

    // address
    TInetAddr iAddress;

    // for logging into right thread log
    TInt iHandle;

    // root device flag
    TBool iIsRootDevice;

    // Device description file information
    HBufC* iDeviceDescription;
    // properties
    RPointerArray<CUpnpDescriptionProperty> iProperties;

    /*UrlBase valueu if in document is empty*/
    HBufC8* iUrlBase;

    // services id     
    CDesC8ArrayFlat* iServicesId;
    };

#endif  // C_CUPNPDEVICE_H
// End of File