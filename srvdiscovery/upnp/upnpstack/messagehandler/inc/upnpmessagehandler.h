/** @file
 * Copyright (c)  Nokia Corporation and/or its subsidiary(-ies). 
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
 * Description: 
 *     Declares MessageHandler Class
 *
 */

#ifndef C_CUPNPMESSAGEHANDLER_H
#define C_CUPNPMESSAGEHANDLER_H

// INCLUDES

#include <e32base.h>
#include "upnpmessagehandlerengineobserver.h"
#include "upnpmessagehandler.pan"
#include "upnpmessagehandlerobserver.h"
#include <upnpsymbianserverbase.h>


// FORWARD DECLARATIONS

class CUpnpMessageHandlerEngine;

// CLASS DECLARATION

/**
 *brief The main class of Message Handler.
 * 
 * An instance of class CUpnpMessageHandler is the main server class. Message Handler
 * is used to send and receive SSDP messages. CUpnpMessageHandler owns 
 * CUpnpMessageHandlerEngine object that is used as engine class of main functionalities 
 * of Message Handler.
 */

class CUpnpMessageHandler : public CUpnpSymbianServerBase,
    public MMessageHandlerEngineObserver
    {
public:
    // New methods

    /**
     * Create a CUpnpMessageHandler object using two phase construction,
     * and return a pointer to the created object, leaving a pointer to the
     * object on the cleanup stack
     * @result pointer to created CUpnpMessageHandler object
     */
    static CUpnpSymbianServerBase* NewLC();

    /**
     * Destroy the object and release all memory objects
     */
    ~CUpnpMessageHandler();

    /**
     * Informs every Observer (every session) that the device list has changed
     */
    void DeviceListUpdateL();

    /**
     * Add Observer (session) to the list
     */
    void AddObserverL( MMessageHandlerObserver* aObserver );

    /**
     * Removes Observer (session) from list
     */
    void RemoveObserver( MMessageHandlerObserver* aObserver );

    /**
     * Tests if SSDP servers are started
     */
    TBool IsSsdpStarted();

    /**
     * Getter of starting error
     */
    TInt StartingError();

    /**
     * Setter of SSDP state
     */
    void SetSsdpStarted( TBool aStarted );

private:
    // New methods

    /**
     * Performs the first phase of two phase construction 
     */
    CUpnpMessageHandler();

    /**
     * Performs the second phase construction of a CUpnpMessageHandler object
     */
    void ConstructL();

    /**
     * callback from observer
     */
    void IPListChange(); 
	
	/**
     * Returns sessions
     */
    TDblQueIter< CSession2 >& Sessions();

private:
    // From CServer

    /**
     * Create a time server session, and return a pointer to the created object
     * @param aVersion the client version 
     * @result pointer to new session
     */
    CSession2 * NewSessionL( const RMessage2& aMessage ) const;

private: // from CUpnpSymbianServerBase
    
    /**
     * Return name of a server
     * @result name of a server
     */
    const TDesC& ServerName() const;

    /**
     * Returns version that is supported by this server.
     */
    virtual TVersion SupportedVersion() const;

    /**
     * Process any errors
     * @param aError the leave code reported.
     * @result return KErrNone if leave is handled
     */
    TInt RunError( TInt aError );

private:

	// pointer to engine
    CUpnpMessageHandlerEngine* iEngine;

	// observers array
    RPointerArray<MMessageHandlerObserver> iObservers;

	// starting flag
    TBool iSsdpStarted;

	// start error flag
    TInt iStartingError;
    };

#endif // C_CUPNPMESSAGEHANDLER_H
