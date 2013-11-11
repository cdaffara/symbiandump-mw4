/** @file
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
*
*/

#ifndef UPNPCONNINTERFACEWATCHER_H_
#define UPNPCONNINTERFACEWATCHER_H_

// INCLUDES
#include <e32base.h>
#include <in_sock.h>
#include "upnpipeventnotifier.h"

const TInt KServiceTypeMaxLength = 128;

// FORWARD DECLARATIONS
class CUpnpIPEventNotifier;

/**
 * CUpnpConnInterfaceWatcher observer (callback interface).
 * @since S60 5.0
 */
class MUpnpConnInterfaceWatcherObserver
    {
public:

    /**
     * Invoked when network interface goes down
     */
    virtual void WlanNetworkLost() = 0;
    
    /**
     * Invoked when address changes
     */
    virtual void InterfaceAddressChange( TInetAddr& aAddr ) = 0;
    
    };

/**
 * Class is responsible for network interface change handling.
 * @since S60 5.0
 */
class CUpnpConnInterfaceWatcher : public CActive, public MUpnpIPEventObserver
    {
public:

    /**
     * A two-phased constructor function
     */
    static CUpnpConnInterfaceWatcher* NewL( MUpnpConnInterfaceWatcherObserver& aIfObserver,
                                            RSocketServ& aSockServ, 
                                            RConnection& aConnection );
    /**
     * Destructor
     */
    virtual ~CUpnpConnInterfaceWatcher();

	/**
     * Returns inet address
     */
    const TInetAddr& LocalAddress();
    
    /**
     * Creates and subscribes to CUpnpIPEventNotifier instance
     */
    void ListenAddressChangeL( TUint aIapId );
    
    /**
     * Get local ip address
     * @param aInterfaceIndex reference that will be set to interface index
     */ 

    TInetAddr LocalIpAddress(TInt& aInterfaceIndex);

    /**
     * Subscribes to Service Change Notification from RConnection.
     * This notification completes with KErrCancel when connection
     * is stopped (by WLan Lost or by calling RConnection::Stop())
     */
    void SubscribeForConnectionServiceChangeNotifications();

    /**
     * Cancels pending subscription
     */
    void CancelSubscriptionForConnectionServiceChangeNotifications();

private:

    /**
     * Constructor.
     */
    CUpnpConnInterfaceWatcher( MUpnpConnInterfaceWatcherObserver& aIfObserver, 
                               RSocketServ& aSockServ, 
                               RConnection& aConnection );

    /**
     * Second phase constructors
     */
    void ConstructL();

private:

    /**
     * This function will be invoke if IP Event occurs 
     * Checks if IP address from used interface was changed since last time    
     */
    virtual void IPEventOccursL();
    
    /**
     * Sets local address
     * @param aNew address
     */    
    void SetLocalAddress( const TInetAddr& aAddress );
    
private:    // from CActive

    /**
     * RunL
     * Active object state machine.
     * @since Series60 5.0 HN
     */
    void RunL();
    
    /**
     * DoCancel
     * Cancel outstanding request(s) and reset the iActive flag.
     * @since Series60 5.0 HN
     */
    void DoCancel();
    
    /**
     * RunError
     * RunError in case RunL leaves.
     * @since Series60 5.0 HN
     */
    TInt RunError( TInt aError );
    
private:

    /* @var iIfObserver Callback interface */
    MUpnpConnInterfaceWatcherObserver& iIfObserver;

    /* @var iSockServ Socket server session */
    RSocketServ& iSockServ;
       
    /* @var iIapId IAP identifier */
    TUint iIapId;
    
    /* @var iIPEvent IP events notifier */
    CUpnpIPEventNotifier* iIPEvent;
        
    /* @var iLocalIpAddress local IP address */
    TInetAddr iLocalIpAddress;
    
    /* @var iNewISPId variable used in Service Change Notification */
    TUint32 iNewISPId;

    /* @var iNewServiceType variable used in Service Change Notification */
    TBuf< KServiceTypeMaxLength > iNewServiceType;
    
    /* @var iConnection RConnection reference */
    RConnection& iConnection;
    
    };

#endif /*UPNPCONNINTERFACEWATCHER_H_*/
