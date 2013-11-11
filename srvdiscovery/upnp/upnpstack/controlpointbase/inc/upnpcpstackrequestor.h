/** @file
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Declares MUpnpCpStackRequestor class.
*
*/

#ifndef MUPNPCPSTACKREQUESTOR_H_
#define MUPNPCPSTACKREQUESTOR_H_

// INCLUDES
#include <e32base.h>
#include <in_sock.h>

// FORWARD DECLARATIONS
class MUpnpDispatcherCustomer;
class CUpnpHttpMessage;
class CUpnpDispatcher;

/**
* @brief Class defines interface which separates controlpointbase
* and serviceframework modules.
* It makes possible switch serviceframework implementation to another one.
*
*/
class MUpnpCpStackRequestor
    {
public:

    /**
    * Sends a SSDP Search with 8-bit search string.
    * @param aSearchString String to use in SSDP ST header.
    */
    virtual void SearchL( const TDesC8& aSearchString ) = 0;

    /**
    * Sends a SSDP Search with 16-bit search string. This method makes a
    * conversion to 8-bit and then makes the search.
    * @param aSearchString String to use in SSDP ST header.
    */
    virtual void SearchL( const TDesC16& aSearchString ) = 0;

    /**
    * Add a customer to the customer list of this dispatcher.
    * It means that messages start being forwarded to that customer.
    * @param aCustomer The customer to be added.
    */
    virtual void AddCustomerL( const MUpnpDispatcherCustomer& aCustomer ) = 0;

    /**
    * Remove a customer from the customer list of this dispatcher.
    * Messages are not forwarded to that customer.
    * @param aCustomer The customer to be removed.
    */
    virtual void RemoveCustomer( const MUpnpDispatcherCustomer& aCustomer ) = 0;

    /**
    * Returns HTTP server address.
    * @return Buffer that contains the address.
    */
    virtual TInetAddr HttpServerAddress() = 0;    

    /**
    * Send a request via serviceframework loopback. Used for SOAP communication
    * between local control point and local device.
    * @param aMessage The message to be sent.
    * @param aCustomer Pointer to the callback interface.
    */
    virtual void SendLocalRequestL( CUpnpHttpMessage* aMessage,
                                    MUpnpDispatcherCustomer& aCustomer ) = 0;

    /**
    * Send a HTTP client request. Used for description fetching, SOAP actions and
    * subscription requests.
    * @param aMessage The message to be sent.
    * @param aCustomer pointer to observer
    * @param aIsLocal
    */
    virtual void SendMessageL( CUpnpHttpMessage* aMessage,
                               MUpnpDispatcherCustomer& aCustomer,
                               TBool aIsLocal ) = 0;

    /**
    * Stops ignoring incoming SSDP notification
    * for discovered device.
    * @param aUuids UUids.
    */
    virtual void StopIgnoringL( const TDesC8& aUuids ) = 0;

    virtual void StartIPFilteringL() = 0;
    virtual void StopIPFiltering() = 0;

    /**
    * Stops htpp server    
    */
    virtual void StopHttpServer() = 0;
    
    /**
    * Starts htpp server    
    */
    virtual void StartHttpServerL() =0;
    };

#endif /*MUPNPCPSTACKREQUESTOR_H_*/
