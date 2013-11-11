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
* Description:  Message dispacther engine
*
*/


#ifndef C_CUPNPDISPATCHERENGINE_H
#define C_CUPNPDISPATCHERENGINE_H

// INCLUDES
#include <e32base.h>
#include "upnpdispatchersession.h"
#include "upnpdevice.h"
#include "upnphttpserver.h"

// FORWARD DECLARATIONS
class CUpnpDispatcherRequest;
class MDispatcherEngineObserver;
class CUpnpHttpServerSession;

// CLASS DECLARATION

/**
@brief A message dispatcher unit cooperating with Message Handler

This class represents a logical unit between UPnP devices / control points
and Message Handler. Message Handler is a Symbian Server - thus, dispatcher
follows the same client/server framework and is a client that opens a session
to Message Handler. This is not done by class CUpnpDispatcherEngine;
the class RUpnpDispatcherSession represents the client interface.

Using AddCustomer/RemoveCustomer functions it is possible to set the active
services. Dispatcher is able to forward messages to its customers.

Also SendMessage function is very essential part of the dispatcher.
 */
class CUpnpDispatcherEngine : public CActive, public MUpnpHttpServerObserver
    {
public:
    /**
    * Create a CUpnpDispatcherEngine object using two phase construction,
    *and return a pointer to the created object.
    *
    * A session with Message Handler is initiated during the construction.
    *
    * @param aObserver
    * @return a pointer to the created instance of CUpnpDispatcherEngine
    */
    static CUpnpDispatcherEngine* NewL( MDispatcherEngineObserver& aObserver );

    /**
    * Create a CUpnpDispatcherEngine object using two phase construction,
    * and return a pointer to the created object
    *
    * A session with Message Handler is initiated during the construction.
    *
    * @param aObserver
    * @return a pointer to the created instance of CUpnpDispatcherEngine
    */
    static CUpnpDispatcherEngine* NewLC( MDispatcherEngineObserver& aObserver );

    /**
    * Destroy the object and release all memory objects
    */
    virtual ~CUpnpDispatcherEngine();

private:
    /**
    * CUpnpDispatcherEngine
    *
    * @discussion Perform the first phase of two phase construction
    * @param aParent the object to be used to handle updates from the server
    * @param aObserver
    */
    CUpnpDispatcherEngine( MDispatcherEngineObserver& aObserver );

    /**
    * ConstructL performs the second phase construction of a CUpnpDispatcherEngine
    * object
    */
    void ConstructL();

    /**
    * Creates http server
    */    
    void ConstructHttpL();

public:

    /**
    * Send a message. This function pushes the message to the Message Handler.
    * @param aMessage The message to be sent.
    */
    void SendMessageL( CUpnpHttpMessage* aMessage );

    /**
    * Add a customer to the customer list of this dispatcher. In practice,
    * means that messages start being forwarded to that customer.
    * @param aCustomer The customer to be added.
    */
    void AddCustomer( const MUpnpDispatcherCustomer& aCustomer );

    /**
    * Remove a customer from the customer list of this dispatcher. In practice,
    * means that messages stop being forwarded to that customer.
    * @param aCustomer The customer to be removed.
    */
    void RemoveCustomer( const MUpnpDispatcherCustomer& aCustomer );

    /**
    *Starts http server
    *@since Series60
    *@param aRegister determines if starting error in message handler process will be checked
    *@param aPort starts server on fixed port    
    */
    void StartHttpServerL( TBool aRegister, const TInt aPort );

    /**
    *Stops http server
    *@since Series60
    */

    void StopHttpServer();

public: // from CActive

    /**
    * Active object RunL, pending request completed.
    */
    void RunL();

    /**
    * Active object RunL, pending request completed.
    * @param aError code from RunL.
    * @result KErrNone.
    */
    TInt RunError( TInt aError );

    /**
    * Active object DoCancel, cancel pending requests.
    */
    void DoCancel();

public:

    // Enum that is used when finding out last request
    enum TDispatcherRequest
    {
    EUnknown = -1,
    ENewDevices,
    EDeviceList
    };


    /**
    * Return the MAC addess of this CUpnpDispatcherEngine's HTTP server.
    * @result Buffer that will contain the MAC address.
    */
    const HBufC8* MacAddressL();

    /**
    * Removes a device from Message Handler's device library,
    * which has an UUID of aUuid.
    * @param aUuid UUID of device that is to be removed.
    * @result None.
    */
    void RemoveLocalDeviceL( const TDesC8& aUuid, TBool aSilent );

    /**
    * Return the IP addess of this CUpnpDispatcherEngine's HTTP server.
    * @result TInetAddr that will contain the IP address of the server.
    */
    TInetAddr HttpServerAddress();

    /**
    * Callback from HTTP server that a HTTP message has been received.
    * @param aMessage Received message.
    * @result None.
    */
    void HttpEventLD( CUpnpHttpMessage* aMessage );

    /**
    * Sends a SSDP search for aString
    * @param aString Search message we want to send
    * @result None.
    */
    void SsdpSearchL( const TDesC8& aString );

    /**
    * Sends a SSDP uuids that should not be filtered
    * @param aString Uuids
    */
    void SsdpStopFilteringL( const TDesC8& aString );

    /**
    * Adds a local device to Message Handler's device library with
    * given information.
    * @param aUuid UUID of the device
    * @param aDeviceType Device type of the device
    * @param aDescriptionPath URL to this device's desctiption
    * @param aServices List of services provided by this device.
    * @param aIsRootDevice ETrue, if this device is UPnP root device, \
    *       EFalse otherwise.
    * @result None.
    */
    void AddLocalDeviceL( const TDesC8& aUuid,
                          const TDesC8& aDeviceType,
                          const TDesC8& aDescriptionPath,
                          const CDesC8Array& aServices,
                          const TBool aIsRootDevice );

     /**
    * Add control point client to message handler.
    */
    void AddControlPointClientL( );

     /**
    * Add control point client to message handler.
    */
    void RemoveControlPointClientL( );

    /**
    * This requests Message Handler for new device list.
    * @param aUpdateId Dispatcher's last known update id.
    */
    void UpdateDevicesL( TInt aUpdateId );

    /**
    * Returns a list of new devices found on last query.
    * @return List of CUpnpDevice's.
    */
    const RPointerArray<CUpnpDevice>& NewDevices() const;

    /**
    * Clears the list of newly found devices.
    * @result None.
    */
    void RemoveDevices();

    /**
    * Deletes current request from list of requests.
    * @result KErrNone, if successful. KErrNotFound if no requests
    *       in list.
    */
    TInt RemoveHandledRequest();

    /**
    * Cancels current pending request, if there is a need to send
    * for example a SSDP search.
    * @result None.
    */
    void StopRequestL();
  
    /**
    * This function sets handle to transaction creator interface.
    * @since Series60 5.0 
    * @param aTransactionCreator pointer to object implementing MUpnpHttpServerTransactionCreator
    *     interface.
    **/
    void SetTransactionCreator( MUpnpHttpServerTransactionCreator* aTransactionCreator );

private:
    /**
    * Makes a request for devices to Message Handler.
    * @return ENewDevices, if request is made for new devices,
    * returns EDeviceList, if request is made for new devices list size.
    */
    TDispatcherRequest GetDevicesL();

    /**
    * Function gets called when new devices are found.
    * @return Last update id.
    */
    TInt DevicesReceivedL();

    /**
    * Sets the Dispatcher to handle next request and remove
    * the old one.
    */
    void HandlePendingRequestL();

    /**
    *Check if start was succesfull
    *@sience Series60 5.0
    *@param aRegister
    */
    void CheckErrorL( TBool aRegister );

private:

    /*! @var iHttpServerSession Pointer to this dispatcher's HTTP server session*/
    CUpnpHttpServerSession* iHttpServerSession;

    /*! @var iSession the message handler session */
    RUpnpDispatcherSession                  iSession;

    /*! @var iCustomers list of dispatcher customers */
    RPointerArray<MUpnpDispatcherCustomer>  iCustomers;

    /*! @var iNewDevices List of new devices that were found on this query */
    RPointerArray<CUpnpDevice>              iNewDevices;

    /*! @var iAddLocalDevice Struct of TInt's that will \
        inform what is the length of each value in the data buffer */
    TUpnpAddLocalDevice                     iAddLocalDevice;

    /*! @var iAddLocalPtr Pointer to buffer that will contain device \
       information. */
    TPtr8                   iAddLocalPtr;

    /*! @var iAddLocalBuffer Buffer that holds the information about \
        device to be added to Message Handler's device library. */
    HBufC8*                 iAddLocalBuffer;

    enum TGetDeviceListState
    {
        EGetListSize = 0,
        EGetList,
        EListReceived,
        EOtherOperation,
        ENoOperation,
    };

    /*! @var iDeviceGet Current state of DispatcherEngine. */
    TGetDeviceListState                    iDeviceGetState;

    /*! @var iSize Structure that holds information \
        about number of new services and devices. */
    TUpnpGetDeviceListSize                 iSize;

    /*! @var iDevs Pointer to list of new devices. */
    TUpnpDevice*                           iDevs;

    /*! @var iServs Pointer to list of new services. */
    TUpnpService*                          iServs;

    /*! @var iOldUpdateId Last known update id of this CUpnpDispatcher. */
    TInt                                   iOldUpdateId;

    /*! @var iObserver Reference to this CUpnpDispatcherEngine's parent for \
        callbacks. */
    MDispatcherEngineObserver&             iObserver;

    /*! @var iPendingRequests List of requests to Message Handler that \
        ahve not yet been sent. */
    RPointerArray<CUpnpDispatcherRequest>  iPendingRequests;

    /*! @var iActiveRequest Request that is currently active \
        to Message Handler. */
    CUpnpDispatcherRequest*                iActiveRequest;

    /*! @var iSearch Buffer that holds a search string that will be \
        sent as SSDP message. */
    HBufC8*                                iSearch;

    /*! @var iGetDevPtr Pointer that is used to receive new devices \
        from Message Handler. */
    TPtr8                                  iGetDevPtr;

    /*! @var iGetServPtr Pointer that is used to receive new services \
        from Message Handler. */
    TPtr8                                  iGetServPtr;

    /*! @var iRemoveUuid Pointer to list of new services. */
    HBufC8*                                iRemoveUuid;

    /*! @var iRemoveUuidPtr Buffer that is used to hold UUID of the \
        device that needs to be removed from Message Handler's device \
        library. */
    TPtrC8                                 iRemoveUuidPtr;

    /* @var Handle counts for logging */
    TInt                iProcessHandlesInStart;
    //
    TInt                iThreadHandlesInStart;
    //
    TInt                iReqsInStart;
    //
    TInt                iProcessHandlesInEnd;
    //

    TInt                iThreadHandlesInEnd;
    //
    TInt                iReqsInEnd;

    HBufC8* iMXString;

    HBufC8* iUuids;

    TBool iIsSessionOpen;
    };

#endif // C_CUPNPDISPATCHERENGINE_H

// End of File