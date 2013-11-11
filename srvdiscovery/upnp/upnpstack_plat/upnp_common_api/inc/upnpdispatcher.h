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
* Description:  Message dispacther
*
*/



#ifndef C_CUPNPDISPATCHER_H
#define C_CUPNPDISPATCHER_H

// INCLUDES
#include <e32base.h>
#include <badesca.h>
#include "upnpnotifytimer.h"
#include "upnpcommonstructs.h"
#include "upnphttpmessage.h"
#include "upnpdispatcherengineobserver.h"

// FORWARD DECLARATIONS
class CUpnpDispatcherEngine;
class MUpnpDispatcherCustomer;
class MUpnpDiscoveryObserver;
class MUpnpHttpServerTransactionCreator;

// CONSTANTS

// CLASS DECLARATION

/**
    @brief A message dispatcher unit cooperating with Message Handler

    This class represents a logical unit between UPnP devices / control
    points and Message Handler.
    Message Handler is a Symbian Server - thus, dispatcher follows the
    same client/server framework and is a client that opens a session
    to Message Handler. This is not done by class CUpnpDispatcher;
    the class RUpnpDispatcherSession represents the client interface.

    Using AddCustomer/RemoveCustomer functions it is possible to set
    the active services. Dispatcher is able to forward messages
    to its customers.

    Also SendMessage function is very essential part of the dispatcher.
 */
class CUpnpDispatcher : public CBase,
            public MDispatcherEngineObserver
{
public:
    /**
    * Create a CUpnpDispatcher object using two phase construction,
    * and return a pointer to the created object.
    *
    *A session with Message Handler is initiated during the construction.
    *
    * @param aParent
    * @return a pointer to the created instance of CUpnpDispatcher
    */
    IMPORT_C static CUpnpDispatcher* NewL( MUpnpDiscoveryObserver* aParent );


    /**
    * Create a CUpnpDispatcher object using two phase construction,
    * and return a pointer to the created object
    *
    * A session with Message Handler is initiated during the construction.
    *
    * @param aParent the object to be used to handle updates from the server
    * @return a pointer to the created instance of CUpnpDispatcher
    */
    IMPORT_C static CUpnpDispatcher* NewLC( MUpnpDiscoveryObserver* aParent ) ;

    /**
    * Destroy the object and release all memory objects
    */
    IMPORT_C virtual ~CUpnpDispatcher();

private:
    /**
    * @function CUpnpDispatcher
    *
    * @discussion Perform the first phase of two phase construction
    * @param aObserver the object to be used to handle updates from the server
    */
    CUpnpDispatcher( MUpnpDiscoveryObserver* aParent );

    /**
    * @function ConstructL
    *
    * @discussion Perform the second phase construction of a CUpnpDispatcher object
    */
    void ConstructL();

public:
    /**
    * Sends a SSDP Search with 8-bit searchString.
    * @param aSearchString String to use in SSDP ST -header.
    */
    IMPORT_C void SearchL( const TDesC8& aSearchString );

    /**
    * Sends a SSDP Search with 16-bit searchString. This method makes a
    * conversion to 8-bit and then makes the search
    * @param aSearchString Stfing to use in SSDP ST -header.
    */
    IMPORT_C void SearchL( const TDesC16& aSearchString );

    /**
    * Sends a stop filtering request. Device notification will not be
    * stoped in ssdp.
    * @param aUuids UUids.
    */
    IMPORT_C void StopFilteringL( const TDesC8& aUuids );

    /**
    * Send a message. This function pushes the message to the Message Handler.
    * @param aMessage The message to be sent.
    * @param aIsLocal ETrue if is local device, else EFalse
    *    if device is local, then action will be  send without network layer
    *   - aMessage has to be EAction type.
    */
    IMPORT_C void SendMessageL( CUpnpHttpMessage* aMessage,
                                MUpnpDispatcherCustomer& aCustomer,
                                TBool aIsLocal = EFalse);

    /**
    * Add a customer to the customer list of this dispatcher. In practice,
    * means that messages start being forwarded to that customer.
    * @param aCustomer The customer to be added.
    */
    IMPORT_C void AddCustomer( const MUpnpDispatcherCustomer& aCustomer );

    /**
    * Remove a customer from the customer list of this dispatcher. In practice,
    * means that messages stop being forwarded to that customer.
    * @param aCustomer The customer to be removed.
    */
    IMPORT_C void RemoveCustomer( const MUpnpDispatcherCustomer& aCustomer );

    /**
    * Add a customer to the customer list of this dispatcher. In practice,
    * means that messages start being forwarded to that customer.
    * @param aCustomer The customer to be added.
    * @param aIsCP The customer is control point.
    */
    IMPORT_C void AddCustomerL( const MUpnpDispatcherCustomer& aCustomer, TBool aIsCP );

    /**
    * Remove a customer from the customer list of this dispatcher. In practice,
    * means that messages stop being forwarded to that customer.
    * @param aCustomer The customer to be removed.
    * @param aIsCP The customer is control point.
    */
    IMPORT_C void RemoveCustomer( const MUpnpDispatcherCustomer& aCustomer, TBool aIsCP );

    /**
    * Adds a local device to Message Handler's device library.
    * @param aUuid UUID of the device.
    * @param aDeviceType Device type of the device (e.g MediaServer:1)
    * @param aDescriptionPath URL to device description.
    * @param aServices List of services this device provides.
    * @param aIsRootDevice ETrue, if this device is UPnP root device, \
    * EFalse otherwise.
    */
    IMPORT_C void AddLocalDeviceL( const TDesC8& aUuid,
                                   const TDesC8& aDeviceType,
                                   const TDesC8& aDescriptionPath,
                                   const CDesC8Array& aServices,
                                   const TBool aIsRootDevice );

    /**
    * Removes a local device from Message Handler's device library.
    * @param aUuid UUID of the device that is to be removed.
    */
    void RemoveLocalDeviceL( const TDesC8& aUuid, TBool aSilent );

    /**
    *
    * @param aUpdateId
    */
    virtual void DeviceListReceivedL(TInt aUpdateId);

public:

    /**
    * Returns an active interface's Hardware (MAC) address.
    * @return Buffer that contains the address.
    */
    const HBufC8* MacAddressL();

    /**
    * Returns HTTP server address.
    * @return Buffer that contains the address.
    */
    IMPORT_C TInetAddr HttpServerAddress();

    /**
    * Informs upper layers about newly found devices.
    * @param aNewId New update id that needs to be stored.
    */
    IMPORT_C void UpdateDevicesL(TInt aNewId);
    
    /**
    * Sets handle to transaction creator interface.
    * @since Series60 5.0 
    * @param aTransactionCreator pointer to object implementing MUpnpHttpServerTransactionCreator
    *     interface.
    **/
    void SetTransactionCreator( MUpnpHttpServerTransactionCreator* aTransactionCreator );
    
    /**
    *Starts http server
    *@param aRegister determines if starting error in message handler process will be checked
    *@param aPort starts server on fixed port
    */        
    IMPORT_C void StartHttpServerL( TBool aRegister, const TInt aPort );

    /**
    * Stops http server
    */
    IMPORT_C void StopHttpServer();

    /*****IPFIltering**********/
    IMPORT_C void StartIPFilteringL( );
    IMPORT_C void StopIPFiltering();
    /*************************/

private:

    enum TState
    {
        EReceiving,
        ESending
    };

    /*! @var iParent observer which handles updates from the server */
    MUpnpDiscoveryObserver*  iParent;

    /*! @var iEngine Pointer to engine class. */
    CUpnpDispatcherEngine*  iEngine;

    /*! @var iUpdateId device list update id. */
    TInt iUpdateId;
    };

#endif // C_CUPNPDISPATCHER_H

// End of File