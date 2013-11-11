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
* Description:  defines CUpnpConnManagerSession
*
*/

#ifndef C_CUPNPCONNMANAGERSESSION_H
#define C_CUPNPCONNMANAGERSESSION_H

#include <e32base.h>
#include "upnpconnmanagercommon.h" 
#include "upnpconnectionmanagernetworkevent.h"

class CUpnpConnectionManager;
class CUpnpConnManagerEngine;

/**
 * An instance of class CUpnpConnManagerSession is created for each client
 * @since S60 5.0
 */
class CUpnpConnManagerSession : public CSession2
    {

public: // New methods

    /**
     * Create a CUpnpConnManagerSession object using two phase construction,
     * and return a pointer to the created object
     * @param aEngine the message handler engine
     * @result pointer to new session
     */
    static CUpnpConnManagerSession* NewL( CUpnpConnManagerEngine& aEngine );

    /**
     * Create a CUpnpConnManagerSession object using two phase construction,
     * and return a pointer to the created object
     * (leaving a pointer to the object on the cleanup stack)
     * @param anEngine the message handler engine
     * @result pointer to new session
     */
    static CUpnpConnManagerSession* NewLC( CUpnpConnManagerEngine& aEngine );

    /**
     * Destroy the object and release all memory objects
     */
    virtual ~CUpnpConnManagerSession();

public: // From CSession

    /**
     * Called after a service request from client; from class CSession
     * @param aMessage message from client (containing requested operation and any data)
     */
    void ServiceL( const RMessage2& aMessage );
            
    /**
     * Cancels subscription for network events
     */ 
    void CancelSubscription();
    
    /**
     * Called when WLan Lost occurs or someone stops the connection
     */ 
    void WLanLostOccuredL();
    
    /**
     * Called when interface IP Address has changed
     * @param new address
     */ 
    void AddressChangeOccuredL( TInetAddr& aAddr );
    
    /**
     * Returns value of iSessionUseConnection variable
     */ 
    TBool SessionUseConnection();

    /**
     * Called when connection was closed by server
     */ 
    void ConnectionClosedL();

private: // New methods

    /**
     * Perform the first phase of two phase construction
     * @param aServer the message handler server
     * @param aEngine the message handler engine
     */
    CUpnpConnManagerSession( CUpnpConnManagerEngine& aEngine );

    /**
     * Causes the client thread to panic
     * @param panic code
     */
    void PanicClient( const RMessage2& aMessage, TInt aPanic ) const;

private:

    // Events service functions
    TInt EventEnsureStartRConnectionL( const RMessage2& aMessage );
    void EventIsRConnectionStartedL( const RMessage2& aMessage );
    void EventSubscribeToNetworkEventL( const RMessage2& aMessage );
    void EventCancelSubscribeToNetworkEvent( );        
    void EventActiveIapL( const RMessage2& aMessage );               
    void EventLocalAddressL( const RMessage2& aMessage );
    
    // Conpletions of NetworkEvent events
    void WLanLostCompleteL();
    void AddressChangeCompleteL();
    void ConnectionClosedCompleteL();

private:

    /* @var iEngine Reference to the message handler engine */
    CUpnpConnManagerEngine& iConnEngine;
    
    /* @var iSubsriberMessage subscriber's message */
    RMessage2 iSubscriberMessage;
    
    /* @var iLastEvent the last network event */
    TUpnpConnectionManagerNetworkEvent iLastEvent;

    /* @var iSessionUseConnection Indicates if session called EnsureStart() */
    TBool iSessionUseConnection;
    
    };

#endif // C_CUPNPCONNMANAHERSESSION_H
