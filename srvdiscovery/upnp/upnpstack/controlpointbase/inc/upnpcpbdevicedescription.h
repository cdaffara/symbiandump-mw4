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
* Description:  Base class - declares device description process.
*
*/


#ifndef C_CUPNPCPBDEVICEDESCRIPTION_H
#define C_CUPNPCPBDEVICEDESCRIPTION_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CUpnpDevice;
class CUpnpControlPoint;
class CUpnpContentHandlersController;
class CUpnpCPBDiscovery;
class MUpnpCpbHttpMessageController;
class CUpnpCpbDeviceRepository;

// CONST
const TInt KDisscoveryFinished = 0;
const TInt KDisscoveryIncorrect = 1;
const TInt KDisscoveryInProgress = 2;

/**
 * Class handling discovery process of simple device.
 * It is checking only services of device. Subdevices are not processed.
 *
 * @lib controlpointbase.lib
 * @since S60 v5.0
 */
NONSHARABLE_CLASS( CUpnpCpbDeviceDescription ) : public CBase
    {
    public:
    /**
     * Destructor
     * @since S60 v5.0
     */
    virtual ~CUpnpCpbDeviceDescription();

    /**
     * Start processing device.
     *
     * @since S60 v5.0
     * @param aDevice Device that should be discovered
     * @return One of symbian errors or
     *                 0 - if discovery process is finished correctly
     *                 1 - if discovery was not finished correctly
     *                 2 - if discovery is in progress
     */
    virtual TInt DiscoverDeviceL( CUpnpDevice* aDevice ) = 0;

    /**
     * Function processing service description.
     *
     * @since S60 v5.0
     * @param aMsg Service message
     * @return One of symbian errors or
     *                 0 - if discovery process is finished correctly
     *                 1 - if discovery was not finished correctly
     *                 2 - if discovery is in progress
     */
    virtual TInt ServiceDescriptionL( CUpnpHttpMessage* aMsg ) = 0;

    /**
     * Get currently processed device.
     *
     * @since S60 v5.0
     * @return Pointer to processed device
     */
    CUpnpDevice* GetTargetDevice();

    /**
     * Get currently processed device, if it is inclomplete one.
     *
     * @since S60 v5.0
     * @return Pointer to processed device
     */
    virtual CUpnpDevice* GetIncompliteRootDevice() = 0;

    /**
     * Set target device as null.
     *
     * @since S60 v5.0
     */
    virtual void NullTargetDevice();

    /**
     * Delete target device.
     *
     * @since S60 v5.0
     */
    virtual void DeleteTargetDevice();

    /**
     * Return incorect uuids
     *
     * @since S60 v5.0
     * @return Uuids pointer
     */
    const TPtrC8 FailedUuidsL();

    /**
     * Append discovered devices to array
     *
     * @since S60 v5.0
     * @param aArray array where devices should be appended
     * @param aRemove determines if already discovered devices should be deleted from iAllDevicesInRoot array
     */
    virtual void GetDiscoveredDeviceL(
        RPointerArray<CUpnpDevice>& aArray, TBool aRemove = EFalse ) = 0;

    /**
     * Append not discovered (does not match the type) devices to array
     *
     * @since S60 v5.0
     * @param aArray array where devices should be appended
     * @param aRemove determines if already discovered devices should be deleted from iNotNeededDevices array
     */
    virtual void GetUnnededDeviceL(
        RPointerArray<CUpnpDevice>& aArray, TBool aRemove = EFalse ) = 0;

    /**
     * Append discovered but not metching target types devices to array
     *
     * @since S60 v5.0
     * @param aArray array where devices should be appended
     * @param aRemove determines if already discovered devices should be deleted from iDiscoveredDevices array
     */
    virtual void GetUninterestingDeviceL(
        RPointerArray<CUpnpDevice>& aArray, TBool aRemove = EFalse ) = 0;

    protected:
    /**
     * Constructor
     * @since S60 v5.0
     */
    CUpnpCpbDeviceDescription( MUpnpCpbHttpMessageController& aMessanger,
                              CUpnpCpbDeviceRepository& aRepository );

    /**
     * Second phase constructor
     * @since S60 v5.0
     */
    virtual void ConstructL();

    /**
     * Add next uuid to uuid list
     * @since S60 v5.0
     * @param aUuid Uuid that should be added
     */
    void AddToUuidsL( const TDesC8& aUuid );

    /**
     * Get next service that should be discovered
     * @param aDevice
     * @return service description url
     */
    const TPtrC8 GetNextServiceType( CUpnpDevice* aDevice );

    protected:
    // incorrect uuids
    RBuf8 iUuids;
    // Current processed root device. NOT OWNED
    CUpnpDevice* iRootDevice;
    // Sax parsers for service description
    CUpnpContentHandlersController* iSaxController;
    // Current state of discovery engine
    TInt iResult;
    // Massenger handling HTTP messages. NOT OWNED
    MUpnpCpbHttpMessageController& iMessanger;
    // device repository, not owned
    CUpnpCpbDeviceRepository& iRepository;
    };

#endif // C_CUPNPCPBDEVICEDESCRIPTION_H
