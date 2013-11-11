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
* Description:  Declares storage for devices known by control point.
*
*/


#ifndef C_CUPNPCPBDEVICEREPOSITORY_H
#define C_CUPNPCPBDEVICEREPOSITORY_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CUpnpDevice;

/**
 * Storage for devices known by control point
 *
 * @lib controlpointbase.lib
 * @since S60 v5.0
 */
NONSHARABLE_CLASS( CUpnpCpbDeviceRepository ) : public CBase
    {
    public:
    /**
     * Two-phased constructor
     * @since S60 v5.0
     */
    static CUpnpCpbDeviceRepository* CUpnpCpbDeviceRepository::NewL(
        const CDesC8Array& aTargetDeviceTypes );

    /**
     * Destructor
     * @since S60 v5.0
     */
    ~CUpnpCpbDeviceRepository();

    /**
     * Check if device type is interesting for control point
     *
     * @since S60 v5.0
     * @param aDevice Device type that should be match
     * @return ETrue if type is correct one, EFalse if not
     */
    TBool MatchTargetDevice(const TDesC8& aDeviceType);

    /**
     * Check if device is in repository
     *
     * @since S60 v5.0
     * @param aDevice Device uuid that should be match
     * @return ETrue if device is know, EFalse if not
     */
    TBool IsKnownDevice(const TDesC8& aDeviceUuid);

    /**
     * Check if device is discovered already
     *
     * @since S60 v5.0
     * @param aDevice Device uuid that should be match
     * @return ETrue if device is know, EFalse if not
     */
    TBool IsDiscoveredDevice(const TDesC8& aDeviceUuid);

    /**
     * Check if device is discovered but type is not matching
     * needed devices types
     *
     * @since S60 v5.0
     * @param aDevice Device uuid that should be match
     * @return ETrue if device is know, EFalse if not
     */
    TBool IsUninterestingDevice(const TDesC8& aDeviceUuid);

    /**
     * Check if device is unneded (type is different then needed).
     *
     * @since S60 v5.0
     * @param aDevice Device uuid that should be match
     * @return ETrue if device is know, FALSE if not
     */
    TBool IsUnneededDevice(const TDesC8& aDeviceUuid);

    /**
     * Add device to discovered device list
     *
     * @since S60 v5.0
     * @param aDevice Target device
     * @return ETrue if device was added
     */
    TBool AddDiscoveredDeviceL(CUpnpDevice* aDevice);

    /**
     * Add device to unneded device list
     *
     * @since S60 v5.0
     * @param aDevice Target device
     * @return ETrue if device was added
     */
    TBool AddUnneddedDeviceL(CUpnpDevice* aDevice);

    /**
     * Add device to incomlite root device list
     *
     * @since S60 v5.0
     * @param aDevice Target device
     * @return ETrue if device was added
     */
    TBool AddIncompliteDeviceL(CUpnpDevice* aDevice);

    /**
     * Add device to discovered device list
     *
     * @since S60 v5.0
     * @param aDevices Target device
     */
    void AddDiscoveredDevicesL(RPointerArray<CUpnpDevice>& aDevices);

    /**
     * Add device to unneded device list
     *
     * @since S60 v5.0
     * @param aDevices Target device
     */
    void AddUninterestingDevicesL(RPointerArray<CUpnpDevice>& aDevices);

    /**
     * Add device to unneded device list
     *
     * @since S60 v5.0
     * @param aDevices Target device
     */
    void AddUnneddedDevicesL(RPointerArray<CUpnpDevice>& aDevices);

    /**
     * Getter. Returns discovered device list.
     *
     * @since S60 v5.0
     * @return List with discovered devices.
     */
    const RPointerArray<CUpnpDevice>& DiscoveredDeviceList() const;

    /**
     * Getter. Returns unnedded device list.
     *
     * @since S60 v5.0
     * @return List with unnedded devices.
     */
    const RPointerArray<CUpnpDevice>& UnneddedDeviceList() const;

    /**
     * Getter. Returns uninteresting device list.
     *
     * @since S60 v5.0
     * @return List with uninteresting devices.
     */
    const RPointerArray<CUpnpDevice>& UninterestingDeviceList() const;

    /**
     * Getter. Returns incomplite root device list.
     *
     * @since S60 v5.0
     * @return List with incomplite root devices.
     */
    const RPointerArray<CUpnpDevice>& IncompliteDeviceList() const;

    /**
     * Getter. Find device in repository.
     *
     * @since S60 v5.0
     * @return Device pointer if found.
     */
    CUpnpDevice* FindDevice( const TDesC8& aDeviceUuid );

    /**
     * Getter. Find incomplite device in repository.
     *
     * @since S60 v5.0
     * @return Device pointer if found.
     */
    CUpnpDevice* FindIncompliteDevice(const TDesC8& aDeviceUuid);

    /**
     * Remove device from all list
     *
     * @since S60 v5.0
     * @param aDevice Target device
     */
    void RemoveDevice( CUpnpDevice* aDevice );

    /**
     * Add device incoming from SSDP layer
     * final device built from device description is synchronized with that
     *
     * @since S60 v5.0
     * @param aDevice Target device
     */
    TBool AddIncomingDeviceL( CUpnpDevice* aDevice );

    /**
     * Remove device from incoming list
     *
     * @since S60 v5.0
     * @param aDevice Target device
     */
    CUpnpDevice* RemoveIncomingDevice( const TDesC8& aUuid  );

    /**
      * Finds root of given device among dicovered and being discovered devices
      * @since Series60 3.2
      * @param aDeviceUuid UUID of device
      * @return Device if found or NULL
      */
    CUpnpDevice* FindRoot( const TDesC8& aDeviceUuid );


private:
    /**
     * Constructor
     * @since S60 v5.0
     */
    CUpnpCpbDeviceRepository();

    /**
     * Second phase constructor
     * @since S60 v5.0
     */
    void ConstructL(const CDesC8Array& aTargetDeviceTypes);

private:

    // Target device types
    CDesC8ArrayFlat* iTargetDeviceTypes;
    // List with discovered devices (only device that match target types)
    RPointerArray<CUpnpDevice> iDiscoveredDevices;
    // List with discovered devices but not metching target types
    // As subdevices of interesting devices needs to be discovered
    RPointerArray<CUpnpDevice> iUninterestingDevices;
    // List with devices that was not discovered as type
    // is not interesting
    RPointerArray<CUpnpDevice> iUnneededDevices;
    // List with inclomplite root devices (some sub devices are
    // discovered so we need to remember also root device)
    RPointerArray<CUpnpDevice> iIncomliteRootDevices;
    // List with inclomplite root devices (some sub devices are
    // discovered so we need to remember also root device)
    RPointerArray<CUpnpDevice> iIncomingDevices;
    };


#endif // C_CUPNPCPBDEVICEREPOSITORY_H
