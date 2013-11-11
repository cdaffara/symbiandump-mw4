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
* Description:  Declares utils function for devices list class.
*
*/


#ifndef C_UPNPCPBDEVICELISTUTILS_H
#define C_UPNPCPBDEVICELISTUTILS_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CUpnpDevice;

/**
 * Class with utils functions for device list
 *
 * @lib controlpointbase.lib
 * @since S60 v5.0
 */
class UpnpCpbDeviceListUtils
    {
public:
    /**
     * Get device from list
     *
     * @since S60 v5.0
     * @param aUuid Device uuid that should be return
     * @param aList List with devices
     * @return Pointer to the device
     */
    static CUpnpDevice* GetFromList(
        const TDesC8& aUuid, RPointerArray<CUpnpDevice>& aList);

    /**
     * Get root device from list
     *
     * @since S60 v5.0
     * @param aUuid Device uuid that parent root device should be return
     * @param aList List with devices
     * @return Pointer to the device or NULL if root device on specified list not found
     */
    static CUpnpDevice* GetRootFromList(
        const TDesC8& aUuid, RPointerArray<CUpnpDevice>& aList );

    /**
     * Check list contains the device
     *
     * @since S60 v5.0
     * @param aUuid Device uuid that should be search
     * @param aList List with devices
     * @return ETrue if array contains device, EFalse if not
     */
    static TBool ExistOnList(
        const TDesC8& aUuid, RPointerArray<CUpnpDevice>& aList );

    /**
     * Add device to list
     *
     * @since S60 v5.0
     * @param aDevice Device that should be added
     * @param aList List with devices
     * @return ETrue if device is added, EFalse if not
     */
    static TBool AddDeviceL(
        CUpnpDevice* aDevice, RPointerArray<CUpnpDevice>& aList );

    /**
     * Add device to list
     *
     * @since S60 v5.0
     * @param aDevice Device that should be added
     * @param aList List with devices
     * @param aError result of append function
     * @return ETrue if device is added, EFalse if not
     */
    static TBool UpnpCpbDeviceListUtils::AddDevice(
        CUpnpDevice* aDevice, RPointerArray<CUpnpDevice>& aList, TInt& aError);

    /**
     * Add devices to devices list
     *
     * @since S60 v5.0
     * @param aSource List with devices
     * @param aTarget Target list
     */
    static void AddDevicesL(
        RPointerArray<CUpnpDevice>& aSource, RPointerArray<CUpnpDevice>& aTarget );
    };

#endif // C_UPNPCPBDEVICELISTUTILS_H
