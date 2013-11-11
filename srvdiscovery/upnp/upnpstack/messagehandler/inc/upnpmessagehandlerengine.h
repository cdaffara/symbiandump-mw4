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
 * Description:  Defines MessageHandlerEngine class
 *
 */

#ifndef C_CUPNPMESSAGEHANDLERENGINE_H
#define C_CUPNPMESSAGEHANDLERENGINE_H

// INCLUDES
#include <upnpnetworkeventobserver.h>
#include "upnpdevicelibrary.h"
#include "upnphttpmessage.h"
#include "upnphttpserverobserver.h"
#include "upnpssdpserverobserver.h"
#include "upnpmessagehandlersession.h"
#include "upnpmessagehandlerengineobserver.h"
#include "upnpcompvariant.hrh"

// LITERALS

static const TInt KSSDPAddressLength=40;
_LIT( KMessagehandlerPanic, "UPnP messagehandler Panic" );
_LIT8( KEmptyMessage, " " );
_LIT8( KMulticastPacket, " " );

// FORWARD DECLARATIONS
class CUpnpHttpServer;
class CUpnpSsdpServer;
class MMessageHandlerEngineObserver;
class CUpnpMSearchTimer;
class CUpnpConnectionManagerProxy;
class CUpnpSsdpHandlerBase;
class CUpnpSsdpHandlerUp;
class CUpnpSsdpHandlerDown;

#ifdef RD_UPNP_REMOTE_ACCESS
class CUpnpIPFiltering;
#endif

// CLASS DECLARATION

/*! 
 \brief The UPnP engine of Message Handler
 
 The message handler is divided into three modules: 
 CUpnpMessageHandler (having the server functionality), 
 CUpnpMessageHandlerSession (having the session functionality) and 
 CUpnpMessageHandlerEngine (having the actual message handling engine).

 Thus, this class represents the engine of the message handling. 
 
 */
class CUpnpMessageHandlerEngine : public CBase,
    public MUpnpDeviceLibraryObserver, public MUpnpSsdpServerObserver,
    public MUpnpNetworkEventObserver
    {
private:
    enum TMessagehandlerPanics
        {
        ENullDeviceLibrary,
        ENullSsdpServer,
        };
public:

    /**
     A two-phased constructor function
     @param aObserver to observe engine
     @result A new instance of CUpnpMessageHandlerEngine
     */
    static CUpnpMessageHandlerEngine* NewL(
        MMessageHandlerEngineObserver& aObserver );
    
    /** 
     Destructor function
     */
    virtual ~CUpnpMessageHandlerEngine();

public:

    /**
     Starts ssdp
     @result error
     */
    TInt StartSsdpL();

    /**
     This function is called from SSDP server after receiving a SSDP message
     @param aMessage The SSDP message that is just received
     @result None.
     */
    void SsdpEventL( CUpnpSsdpMessage* aMessage );
    
    /**
     * Returns device library     
     */
    CUpnpDeviceLibrary* DeviceLibrary();
    
    /**
     Adds a new local device to the device library
     @param aDevice A structure containing indexes to the data buffer
     @param aBuffer A data buffer containing the actual information
     @result KErrNone or one of the system-wide error codes
     */
    TInt AddLocalDeviceL( TUpnpAddLocalDevice& aDevice, TDesC8& aBuffer );

    /**
     Removes a local device from the device library
     @param aUuid UUID of the target device
     @result None.
     */
    void RemoveLocalDeviceL( const TDesC8& aUuid, TBool aSilent );

    /**
     Adds a new local control point client
     */
    TInt AddLocalControlPoint();

    /**
     Removes a local control point client
     */
    TInt RemoveLocalControlPoint();
    
    /**
     It stops using filtering for a target device
     @param aUuid UUID of the target device
     @result None.
     */
    void StopFilteringDeviceL( const TDesC8& aUuid );

    /**
     Returns the device library elements in a pointer array
     @result A pointer to an array containing pointers to the device library 
     elements
     */
    RPointerArray<CUpnpDeviceLibraryElement>& DeviceList();

    /**
     callback method
     */
    void InterfaceDown();
           
public:
    // From MUpnpDeviceLibraryObserver

    /**
     Implementation of MUpnpDeviceLibraryObserver::AdvertiseDevice). 
     
     Advertises a device using SSDP server to 
     send SSDP advertisements to the network.
     @param aLive Tells whether to use ssdp:alive or ssdp:byebye
     @param anElement The device to advertise
     @result None.
     */
    void AdvertiseDeviceL( TInt aLive, CUpnpDeviceLibraryElement& aElement );

    /** 
     Returns the id of the active IAP. 
     @result The id of the IAP
     */
    TInt ActiveIap() const;

    /**
     Makes a SSDP search.
     @param aSearchString The SSDP search string, that is placed to the 
     ST header of the SSDP message.
     @result None.
     */
    void SsdpSearchL( TDesC8& aSearchString, TDesC8& aMX );

    /**
     Inform observer that device list changed.
     @result None.
     */
    void DeviceListChangedL();

#ifdef RD_UPNP_REMOTE_ACCESS
    void AddAddressL( const TInetAddr& aAddress );
    void AddAddressL( const RMessage2& aMessage, const TDesC8& aRemoteName );
    void RemoveAddressL( const TInetAddr& aAddress );
    void RemoveAddressL( const RMessage2& aMessage, const TDesC8& aRemoteName );
    void RemoveAll();
    void StartIPFiltering();
    void StopIPFiltering();
    TBool IsIPFilteringStarted() const;
    TBool IsIPAllowed( const TInetAddr& aAddress ) const;
    void IPFilteringStatus( TInt& aListSize ) const;
    const TDesC8& GetIPFilterList();
#endif

private:    // from MUpnpNetworkEventObserver

    /**
     * It is called from bottom layer when network event occurs [callback method]     
     * @result None
     */
    void NetworkEvent( CUpnpNetworkEventBase* aEvent );

private:

    /**
     Default constructor.
     @param aObserver to observe engine
     @param aHandle the message handler thread log handle
     */
    CUpnpMessageHandlerEngine( MMessageHandlerEngineObserver& aObserver );

    /**
     Default (leaving) constructor.
     */
    void ConstructL();

    /**
     Create session id property and signal id semaphore.
     */
    void CreateSessionIdPropertyL();

    /**
     * Callback function used to inform about the timer events
     */
    void TimerEventL( CUpnpNotifyTimer* aTimer );

private:
    // current state of ssdp servers
    CUpnpSsdpHandlerBase* iCurrentSsdpHandler;

    // ssdp up and runnig state handler 
    CUpnpSsdpHandlerUp* iSsdpHandlerUp;

    // ssdp stopped state handler     
    CUpnpSsdpHandlerDown* iSsdpHandlerDown;

    // The device library 
    CUpnpDeviceLibrary* iDeviceLibrary;

    // The active socket server connection 
    RSocketServ iSocketServ;

    // The MessageHandler observer 
    MMessageHandlerEngineObserver& iObserver;

    // True if SSDP server is ready for shutdown 
    TBool iSsdpReadyForShutdown;

    //Local control points counter 
    TInt iCPCounter;

    // Object used for WLAN lost and address change events and getting local address
    CUpnpConnectionManagerProxy *iConnectionManagerProxy;
    
    // session id access semaphore
    RSemaphore iAccessSemaphore;
    
#ifdef RD_UPNP_REMOTE_ACCESS    
    //Local control points counter 
    CUpnpIPFiltering* iIPFilterRepository;
#endif    
    };

#endif // C_CUPNPMESSAGEHANDLERENGINE_H
