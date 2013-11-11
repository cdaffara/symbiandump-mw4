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
* Description:  Declares ControlPoint Discovery class.
*
*/


#ifndef C_CUPNPCPBDESCRIPTIONAGENT_H
#define C_CUPNPCPBDESCRIPTIONAGENT_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CUpnpDevice;
class CUpnpDispatcher;
class CUpnpControlPoint;
class CUpnpHttpMessage;
class CUpnpCpbDeviceRepository;
class CUpnpDeviceContentHandler;
class CUpnpCpbDeviceDescription;
class CUpnpCpbSimpleDeviceDescription;
class CUpnpCpbEmbeddedDeviceDescription;
class MUpnpCpbHttpMessageController;
class CUpnpNotifyTimer;
class MUpnpNotifyTimerObserver;
class CUpnpContentHandlersController;

/**
 * Mediator class handling whole discovery process.
 * It gets device description and decides how it should be treated
 * and how to discorery it.
 *
 * @lib controlpointbase.lib
 * @since S60 v5.0
 */
NONSHARABLE_CLASS( CUpnpCpbDescriptionAgent ) : public CBase, public MUpnpNotifyTimerObserver
    {
    public:
    /**
     * Two-phased constructor
     * @since S60 v5.0
     */
    static CUpnpCpbDescriptionAgent* CUpnpCpbDescriptionAgent::NewL(
            CUpnpControlPoint& aBaseCP,
            MUpnpCpbHttpMessageController& aMessanger,
            CUpnpCpbDeviceRepository& aRepository);

    /**
     * Destructor
     * @since S60 v5.0
     */
    ~CUpnpCpbDescriptionAgent();

     /**
      * New device found.
      *
      * @since S60 v5.0
      * @param aDevice The target device.
      */
    void DeviceAliveNotificationL( CUpnpDevice* aDevice );

     /**
      * Process device message
      * @param aHttpMessage Message to process
      */
    void ProcessDeviceMessageL(CUpnpHttpMessage* aHttpMessage);

    /**
      * Process service message
      * @param aHttpMessage Message to process
      */
    void ProcessServiceMessageL(CUpnpHttpMessage* aHttpMessage);

    public: // From MNotifyTimerObserve
    /**
     * Callback function used to inform about the timer events
     */
    void TimerEventL( CUpnpNotifyTimer* aTimer );

    private:
    /**
     * Constructor
     * @since S60 v5.0
     */
    CUpnpCpbDescriptionAgent(
        CUpnpControlPoint& aBaseCP,
        MUpnpCpbHttpMessageController& aMessanger,
        CUpnpCpbDeviceRepository& aRepository);

    /**
     * Get next device description
     *
     * @since S60 v5.0
     */
    void DiscoverNextDeviceL();

    /**
     * Get device description
     *
     * @since S60 v5.0
     * @param aDevice The target device.
     */
    void GetDeviceDescriptionL(CUpnpDevice* aDevice);

    /**
     * Choose description processor
     *
     * @since S60 v5.0
     * @param aDevice The target device.
     */
    void ChooseDesriptionProcesor(CUpnpDevice* aDevice);

    /**
     * Function processing disscovery result
     *
     * @since S60 v5.0
     * @param aResult result
     */
    void HandleDiscoveryResultL(TInt aResult);

    /**
     * Send stop ignoring message to MH
     *
     * @since S60 v5.0
     * @param aUuids Uuids that should not be filtered
     */
    void StopIgnoringUuidL(const TDesC8& aUuids);

    /**
     * Check if device is valid
     *
     * @since S60 v5.0
     * @param aDevice The target device.
     */
    TBool IsDeviceValid( CUpnpDevice* aDevice );

    /**
     * Copy results to repository
     *
     * @since S60 v5.0
     * @return ETrue when devices was copied
     */
    TBool CUpnpCpbDescriptionAgent::CopyResult();

    /**
     * Copy results to repository
     *
     * @since S60 v5.0
     */
    void CUpnpCpbDescriptionAgent::CopyResultL();

    /**
     * Report discovered devices to the client
     *
     * @since S60 v5.0
     */
    void ReportDiscoveredDevicesL();

    /**
     * Second phase constructor
     * @since S60 v5.0
     */
    void ConstructL();

    /**
     * Find network for device tree
     *
     * @since S60 v5.0
     */
    CUpnpDevice::TUpnpDeviceNetwork
        FindNetworkForDeviceTree( RPointerArray<CUpnpDevice>& aDevices );


    /**
     * Construct and validate device tree
     *
     * @since S60 v5.0
     */
    void ConstructAndValidateDeviceTreeL( const TDesC8& aDeviceDescXml,
                                          const TInetAddr& aAddr,
                                          const TDesC8& aDeviceDescPath );

    /**
     * This function sets UrlBase value based on the relative path of the description URL.
     */
    void SetUrlBaseL( CUpnpDevice* aDevice, const TDesC8& aPath );


    private:
    // Dispatcher module is responsible for the messaging, not owned
    CUpnpControlPoint& iBaseCP;
    // Massenger handling HTTP messages. NOT OWNED
    MUpnpCpbHttpMessageController& iMessanger;
    // Ropository with devices, DO NOT DELETE
    CUpnpCpbDeviceRepository& iDeviceRepository;

    // Sax parsers for device description
    CUpnpContentHandlersController* iSaxController;
    // List with incoming devices
    RPointerArray<CUpnpDevice> iIncomingDevices;
    // Flag is TRUE when discovery is in progress
    TBool iPendingDiscovery;
    // Discovery timer
    CUpnpNotifyTimer* iTimer;
    // Root device uuid
    RBuf8 iUuid;
    // Current session number
    TInt iDescriptionSession;

    // Strategy used for current device, DO NOT DELETE
    CUpnpCpbDeviceDescription* iDiscoveryEngine;
    // Strategy used for simple devices
    CUpnpCpbSimpleDeviceDescription* iSimpleDiscoveryEngine;
    // Strategy used for embedded devices
    CUpnpCpbEmbeddedDeviceDescription* iEmbeddedDiscoveryEngine;

    };

#endif // C_CUPNPCPBDESCRIPTIONAGENT_H
//  End of File
