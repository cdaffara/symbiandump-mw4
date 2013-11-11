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
* Description:  CUpnpConnectionManagerNetworkEventProvider
*
*/

#ifndef C_UPNPCONNECTIONMANAGERNETWORKEVENTPROVIDER_H
#define C_UPNPCONNECTIONMANAGERNETWORKEVENTPROVIDER_H

// INCLUDES

#include <e32base.h>
#include "upnpconnectionmanagernetworkevent.h"
#include <upnpnetworkeventobserver.h>

// CLASS DECLARATION

class CUpnpNetworkEventSubscriberError;
class RUpnpConnectionManagerSession;


/**
 * Watches connection status
 * @since S60 5.0
 */
NONSHARABLE_CLASS( CUpnpConnectionManagerNetworkEventProvider ) : public CActive
    {
public:
    /**
     * Two-phased constructor. Subscribes events for connection status
     * session to server is maintained by the class
     * @since Series 60 5.0 HN
     * @param aObserver event observer    
     * @return watcher object
     */
    IMPORT_C static CUpnpConnectionManagerNetworkEventProvider* NewL( 
                                   MUpnpNetworkEventObserver& aObserver );
    
    /**
     * Two-phased constructor. Subscribes events for connection status
     * session to server is maintained outside the class
     * @since Series 60 5.0 HN
     * @param aClient that must be opened and closed outside the class
     * @param aObserver event observer
     * @return network provider object
     */
    static CUpnpConnectionManagerNetworkEventProvider* NewL( 
                                   RUpnpConnectionManagerSession& aClient,
                                   MUpnpNetworkEventObserver& aObserver );

    /**
     * Destructor. Usubscribes
     */                                 
    ~CUpnpConnectionManagerNetworkEventProvider();
    
    /**
     * Subscribes to server
     * called during construction
     * @since Series 60 5.0 HN
     */
    IMPORT_C void Subscribe();
    
private:
    /**
     * Constructor.
     */
    CUpnpConnectionManagerNetworkEventProvider( 
                                  MUpnpNetworkEventObserver& aObserver );

    /**
     * Constructor.
     */
    CUpnpConnectionManagerNetworkEventProvider( RUpnpConnectionManagerSession& aClient,
                                  MUpnpNetworkEventObserver& aObserver );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

    /**
     * Dispatch response from connection server
     * @since Series 60 5.0 HN
     */
    void DispatchResponseNetworkEventL();
    
private:    // from CActive

    /**
     * RunL
     */
    void RunL();
    
    /**
     * RunError
     */
    TInt RunError( TInt aError );
    
    /**
     * DoCancel
     */
    void DoCancel();
   

private:    
    
    // current network event that will be passed to client by ipc communication
    TUpnpConnectionManagerNetworkEvent iCurrentNetworkEvent;
    
    // determines if session to server is maintaned by the class
    TBool iSessionOwner;
    
    // connection manager client    
    RUpnpConnectionManagerSession* iConnectionManagerSession;
    
    // event observer
    MUpnpNetworkEventObserver& iNetworkEventObserver;

    // In case of KErrNoMemory error during creation of object
    CUpnpNetworkEventSubscriberError* iNetworkEventSubscriberError;
    };     
    
#endif // C_UPNPCONNECTIONMANAGERNETWORKEVENTPROVIDER_H

// End of File
