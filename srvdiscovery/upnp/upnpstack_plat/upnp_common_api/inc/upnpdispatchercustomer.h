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
* Description:  Message dispacther customer/observer
*
*/


#ifndef C_MUPNPDISPATCHERCUSTOMER_H
#define C_MUPNPDISPATCHERCUSTOMER_H


// FORWARD DECLARATIONS
class CUpnpHttpMessage;


// CLASS DECLARATION

/**
    @brief An interface from dispatcher to its customer (service)
  */
class MUpnpDispatcherCustomer
{
public:
    /**
    * A callback function to receive HTTP messages. Dispatcher checks the 
    * mailbox periodically and calls this function if 
    * a message to this customer is received
    * @param aMessage A pointer containing the received message
    */
    virtual void MessageReceivedLD( CUpnpHttpMessage* aMessage ) = 0;
    
    /** 
    * A function that should return the path identifying this customer
    * @return The identifier path (e.g. service name without version)
    */
    virtual const TDesC8 & Path() = 0;
};

#endif // C_MUPNPDISPATCHERCUSTOMER_H

// End of File
