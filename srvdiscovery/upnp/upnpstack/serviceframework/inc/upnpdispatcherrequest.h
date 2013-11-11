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
* Description:  Declares CUpnpdispacherrequset class
*
*/

#ifndef C_CUPNPDISPATCHERREQUEST_H
#define C_CUPNPDISPATCHERREQUEST_H

// INCLUDES
#include <e32base.h>
#include "upnpmhcommon.h"

// FORWARD DECLARATIONS
class CDesC8ArrayFlat;

// CONSTANTS

// CLASS DECLARATION

/** 
@brief A class that represents single request to the Message Handler

This class represents a logical unit between UPnP devices / control points and
Message Handler. CUpnpDispatcherRequest wraps different requests, 
so they can be queued as in array.
*/

class CUpnpDispatcherRequest: public CBase
{
public:

    /**
    * Create a CUpnpDispatcherRequest object using two phase construction, 
    * and return a pointer to the created object
    *
    * A session with Message Handler is initiated during the construction.
    *
    * @param aRequest from client using dispatcher request
    * @result a pointer to the created instance of CUpnpDispatcherRequest
    */
    static CUpnpDispatcherRequest* NewLC( TMessageHandlerRqst aRequest ) ;

    /**
    * Destroy the object and release all memory objects
    */
    virtual ~CUpnpDispatcherRequest();

private:
    /**
    * @function CUpnpDispatcherRequest
    *
    * @discussion Perform the first phase of two phase construction
    * @param aRequest from client using dispatcher request
    */
    CUpnpDispatcherRequest( TMessageHandlerRqst aRequest );

    /**
     * Second phase of construction
     */
    void ConstructL();
    
public:


    /**
    * Returns a reference to given request's argument list. 
    * @since Series60 2.6
    * @return Reference to given request's argument list.
    */    
    CDesC8ArrayFlat& Arguments();
    
    /**
    * Adds a argument to argument list.
    * @since Series60 2.6
    * @param aArgument Argument to add.
    */
    void AddArgumentL(const TDesC8& aArgument);

    /**
    * When trying to make request on adding device, this function 
    * is used for storing service list to request.
    * @since Series60 2.6
    * @param aArgument Array of arguments to add.
    */
    void AddServices(CDesC8ArrayFlat* aArgument);

    /**
    * Returns the list of services on this request.
    * @since Series60 2.6
    * @return Reference to given request's argument list.
    */  
    CDesC8ArrayFlat& Services();

    /**
    * Returns request type.
    * @since Series60 2.6
    * @return Returns request type.
    */  
    TMessageHandlerRqst Request();

private:    

    /*! @var iRequest Type of request. */
    TMessageHandlerRqst     iRequest;

    /*! @var iArguments Array that contains arguments of this request. */
    CDesC8ArrayFlat*       iArguments;

    /*! @var iServices Array that contains services if adding local device. */
    CDesC8ArrayFlat*        iServices;

};

#endif // C_CUPNPDISPATCHERREQUEST_H

// End of File
