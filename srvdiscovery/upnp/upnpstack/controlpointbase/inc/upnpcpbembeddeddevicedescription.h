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
* Description:  Declares simple device description process.
*
*/


#ifndef C_CUPNPCPBEMBEDDEDDEVICEDESCRIPTION_H
#define C_CUPNPCPBEMBEDDEDDEVICEDESCRIPTION_H

// INCLUDES
#include <e32base.h>

#include "upnpcpbdevicedescription.h"

// FORWARD DECLARATIONS
class CUpnpDevice;
class CUpnpContentHandlersController;
class CUpnpCpbDeviceRepository;

/**
 * Class handling discovery process of simple device.
 * It is checking only services of device. Subdevices are not processed.
 *
 * @lib controlpointbase.lib
 * @since S60 v5.0
 */
NONSHARABLE_CLASS( CUpnpCpbEmbeddedDeviceDescription ) : public CUpnpCpbDeviceDescription
    {
    public:
    /**
     * Two-phased constructor
     * @since S60 v5.0
     */
    static CUpnpCpbEmbeddedDeviceDescription*
            CUpnpCpbEmbeddedDeviceDescription::NewL(MUpnpCpbHttpMessageController& aMessanger,
                                            CUpnpCpbDeviceRepository& aRepository);

    /**
     * Start processing device.
     *
     * @since S60 v5.0
     * @param aDevice Device that should be discovered
     * @return 0 - if discovery process is finished correctly
     *                 1 - if discovery was not finished correctly
     *                 2 - if discovery is in progress
     */
    TInt DiscoverDeviceL(CUpnpDevice* aDevice);

    /**
     * Function processing service description.
     *
     * @since S60 v5.0
     * @param aMsg Service message
     * @return 0 - if discovery process is finished correctly
     *                 1 - if discovery was not finished correctly
     *                 2 - if discovery is in progress
     */
    TInt ServiceDescriptionL(CUpnpHttpMessage* aMsg);

    /**
     * Append discovered devices to array
     *
     * @since S60 v5.0
     * @param aArray array where devices should be appended
     */
    void GetDiscoveredDeviceL(RPointerArray<CUpnpDevice>& aArray, TBool aRemove = EFalse );

    /**
     * Append not discovered (does not match the type) devices to array
     *
     * @since S60 v5.0
     * @param aArray array where devices should be appended
     */
    void GetUnnededDeviceL(RPointerArray<CUpnpDevice>& aArray, TBool aRemove = EFalse );

    /**
     * Append discovered but not metching target types devices to array
     *
     * @since S60 v5.0
     * @param aArray array where devices should be appended
     */
    void GetUninterestingDeviceL(RPointerArray<CUpnpDevice>& aArray, TBool aRemove = EFalse );

    /**
     * Destructor
     * @since S60 v5.0
     */
    ~CUpnpCpbEmbeddedDeviceDescription();

    private:
    /**
     * Constructor
     * @since S60 v5.0
     */
    CUpnpCpbEmbeddedDeviceDescription(MUpnpCpbHttpMessageController& aMessanger,
                                            CUpnpCpbDeviceRepository& aRepository);

    /**
     * Parses service description and add it to device
     *
     * @since S60 v5.0
     * @param aMsg Service message
     * @return 0 - if discovery process is finished correctly
     *                 1 - if discovery was not finished correctly
     *                 2 - if discovery is in progress
     */
    TInt ProcessServiceDescriptionL(CUpnpDevice* aDevice, CUpnpHttpMessage* aMsg);

    /**
     * Second phase constructor
     * @since S60 v5.0
     */
    void ConstructL();

    /**
     * Build interesting device list
     * @since S60 v5.0
     * @param aDevice Target device
     */
    void BuildDevicesListL(CUpnpDevice* aDevice);

    /**
     * Get first service description
     * @since S60 v5.0
     * @param aDevice Target device
     */
    void GetServiceDescriptionL(CUpnpDevice* aDevice);

    /**
     * Get currently processed device, if it is inclomplete one.
     *
     * @since S60 v5.0
     * @return Pointer to processed device
     */
    CUpnpDevice* GetIncompliteRootDevice();

    /**
     * Build result list
     * @since S60 v5.0
     * @param aDevice Target device
     * @return TRUE if device is discovered correctly
     */
    TBool CUpnpCpbEmbeddedDeviceDescription::BuildResultListsL(CUpnpDevice* aDevice);

    /**
     * Count result and create Uuids
     * @since S60 v5.0
     */
    void CreateUuidsL();

    private:
    // List with discovered devices (only device that match target types)
    RPointerArray<CUpnpDevice> iDiscoveredDevices;
    // List with all devices in current target
    RPointerArray<CUpnpDevice> iAllDevicesInRoot;
    // List with devices not needed for correct discovery
    // (not matching type and not being subdevice)
    RPointerArray<CUpnpDevice> iNotNeededDevices;
    };

#endif // C_CUPNPCPBEMBEDDEDDEVICEDESCRIPTION_H
