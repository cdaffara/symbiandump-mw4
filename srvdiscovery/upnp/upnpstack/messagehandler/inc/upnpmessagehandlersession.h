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
 * Description:  defines MessageHandlerSession class
 *
 */

#ifndef C_CUPNPMESSAGEHANDLERSESSION_H
#define C_CUPNPMESSAGEHANDLERSESSION_H

// INCLUDES

#include <e32base.h>

#include "upnpmessagehandler.h"
#include "upnpcommonstructs.h"
#include "upnpmhcommon.h"
#include "upnpmessagehandlerobserver.h"

static const TInt KMaxUuidLength=43;
// FORWARD DECLARATIONS

class CUpnpHttpMessage;
class MMessageHandlerObserver;
class CUpnpMessageHandler;
class CUpnpMessageHandlerEngine;

#ifdef RD_UPNP_REMOTE_ACCESS
class CUpnpIPFiltering;
#endif

// CLASS DECLARATION

/** 
 An instance of class CUpnpMessageHandlerSession is created for each client
 */

class CUpnpMessageHandlerSession : public CSession2,
    public MMessageHandlerObserver
    {

public:
    // New methods

    /**
     Create a CUpnpMessageHandlerSession object using two phase construction,
     and return a pointer to the created object
     @param aClient client's thread
     @param aServer the message handler server
     @param aEngine the message handler engine
     @param aHandle the message handler thread log handle
     @result pointer to new session
     */
    static CUpnpMessageHandlerSession* NewL( CUpnpMessageHandler& aServer,
        CUpnpMessageHandlerEngine& aEngine );

    /**
     Create a CUpnpMessageHandlerSession object using two phase construction,
     and return a pointer to the created object
     (leaving a pointer to the object on the cleanup stack)
     @param aClient client's thread
     @param aServer the message handler server
     @param anEngine the message handler engine
     @param aHandle the message handler thread log handle
     @result pointer to new session
     */
    static CUpnpMessageHandlerSession* NewLC( CUpnpMessageHandler& aServer,
        CUpnpMessageHandlerEngine& anEngine );

    /**
     Destroy the object and release all memory objects
     */
    virtual ~CUpnpMessageHandlerSession();
    
    /**
     Callback method
     @result None
     */
    void DeviceListChangeL();
    
    /**
     Cancells outstanding requests
     @result None
     */
    void CancelPendingRequests();

public:
    // From CSession
    TInt EventStartSsdpL();

    /**
     Called after a service request from client; from class CSession
     @param aMessage message from client (containing requested operation and any data)
     @result None.
     */
    void ServiceL( const RMessage2& aMessage );

private:
    // New methods

    /**
     Is Ssdp stateted ? 
     */
    void EventCheckErrorL( const RMessage2& aMessage );

    /**
     Perform the first phase of two phase construction 
     @param aServer the message handler server
     @param aEngine the message handler engine
     @param aHandle the message handler thread log handle
     @result None.
     */
    CUpnpMessageHandlerSession( CUpnpMessageHandler& aServer,
        CUpnpMessageHandlerEngine& anEngine );

    /**
     Perform the second phase construction of a CUpnpMessageHandlerSession object
     @result None.
     */
    void ConstructL();

    /**
     Causes the client thread to panic
     @param panic code
     */
    void PanicClient( const RMessage2& aMessage, TInt aPanic ) const;

private:

    // functions for various events from client

    /**
     This function is called when a dispatcher has requested that a local device 
     should be added to the device library. 
     @param aMessage The inter-process message we've got
     @result None.
     */
    void EventAddLocalDeviceL( const RMessage2& aMessage );

    /**
     This function is called when a dispatcher has requested that a local device 
     should be removed from the device library. 
     @param aMessage The inter-process message we've got
     @result None.
     */
    void EventRemoveLocalDeviceL( const RMessage2& aMessage, TBool aSilent );

    /**
     This function is called when a dispatcher has requested that a local control point
     should be added. 
     */
    void EventAddLocalControlPoint( const RMessage2& aMessage );

    /**
     This function is called when a dispatcher has requested that a local control point
     should be removed. 
     */
    void EventRemoveLocalControlPoint( const RMessage2& aMessage );

    /**
     This function is called when a dispatcher has requested that device notifications
     should not be filtered by ssdp server. 
     */
    void EventStopFilteringDeviceL( const RMessage2& aMessage );

    /**
     This function is called when a dispatcher has requested that it would like 
     to get a list of updated devices. This is the second phase of device list update.
     @param aMessage The inter-process message we've got
     @result None.
     */
    void EventGetDeviceListL( const RMessage2& aMessage );

    /**
     This function is called when a dispatcher has requested that it would like 
     to get the actual list of updated devices; this is the first phase of device 
     list update.
     @param aMessage The inter-process message we've got
     @result None.
     */
    void EventGetDeviceListSizeL( const RMessage2& aMessage );

    /**
     This function is called when a dispatcher has requested that it would like 
     to get the id of the active iap.
     @param aMessage The inter-process message we've got
     @result None.
     */
    void EventActiveIapL( const RMessage2& aMessage ) const;

    /**
     This function is called when a dispatcher has requested that a ssdp search should be done.
     @param aMessage The inter-process message we've got
     @result None.
     */
    void EventSsdpSearchL( const RMessage2& aMessage );

    void CompletePendingRequest();

    void CopyMessage( RMessage2& aMessage );

    void EventAddFilterAddrByIpL( const RMessage2& aMessage );
    void EventAddFilterAddrByHostnameL( const RMessage2& aMessage );
    void EventRemoveFilterAddrByIpL( const RMessage2& aMessage );
    void EventRemoveFilterAddrByHostnameL( const RMessage2& aMessage );
    void EventRemoveAllIPsAddressesL( const RMessage2& aMessage );
    TBool EventIsIPAddressAllowedL( const RMessage2& aMessage );
    void EventSubscriveIPListChanges( RMessage2& aMessage );
    void EventCancelIPListSubscription( const RMessage2& aMessage );
    void EventGetIPFilterListL( const RMessage2& aMessage );
    void IPListChange();//from observer

private:

    /* @var iAwaitingUpdateDevices This pointer is used temporarily during device list updates */
    TUpnpDevice* iAwaitingUpdateDevices;

    /* @var iAwaitingUpdateServices This pointer is used temporarily during device list updates */
    TUpnpService* iAwaitingUpdateServices;

    /* @var iSize A variable that is used temporarily during device list updates */
    TUpnpGetDeviceListSize iSize;

    /* @var iServer Reference to the server (message handler) */
    CUpnpMessageHandler& iMessageHandler;

    /* @var iEngine Reference to the message handler engine */
    CUpnpMessageHandlerEngine& iEngine;

    // @var iDeviceListSizeMessage Reference to a DeviceListSizeMessage used for responding as device list is updated. 
    // IMPORTANT: This variable is not owned by this module
    RMessage2 iDeviceListSizeMessage;

#ifdef RD_UPNP_REMOTE_ACCESS
    /*******IPFiltering*************/
    RMessage2 iIPListSizeMessage;
    TInt iSeqNo;
    /*******************************/
#endif

    TBool iMoreNewDevices;
    TBool iDeviceListSizeRequested;
    TInt iOldId;

    /* @var iAddedDevices Devices that have been added thru this Message Handler session. If the session
     is disconnected, devices added thru this session are also removed. This array contains the UUIDs
     of devices added. */
    CDesC8ArrayFlat iAddedDevices;
    };

#endif // C_CUPNPMESSAGEHANDLERSESSION_H
