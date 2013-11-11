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
* Description:  CUpnpConnManagerEngine
*
*/

#ifndef C_CUPNPCONNMANAGERENGINE_H
#define C_CUPNPCONNMANAGERENGINE_H

// INCLUDES

#include <e32base.h>
#include <es_sock.h>
#include <commdbconnpref.h>

#include "upnpnotifytimer.h"
#include "upnpconninterfacewatcher.h"

// FORWARD DECLARATIONS
class CUpnpConnManagerSession;

/**
 * Class used to wrap Symbian RConnection class.
 * Fixes performance problem when RConnection::Start() used symultanously by
 * two or more processes/threads.
 * @see CUpnpConnectionManagerProxy for more information.
 * @since S60 5.0
 */
class CUpnpConnManagerEngine : public CBase, public MUpnpNotifyTimerObserver,
                               public MUpnpConnInterfaceWatcherObserver
    {
public:

    /**
     * A two-phased constructor function
     * @param aIterator sessions iterator
     * @result A new instance of CUpnpConnManagerEngine
     */
    static CUpnpConnManagerEngine* NewL( TDblQueIter<CSession2>& aIterator );

    /**
     * Destructor function
     */
    virtual ~CUpnpConnManagerEngine();

public:

    /**
     * After call of this method the caller is sure that
     * RConnection was started.
     * @param aAccessPoint id of current accessPoint on which RConnection was started
     * @return starting error
     */
    TInt EnsureStart( TInt& aAccessPoint );

    /**
     * @since Series60 5.0 HN
     * @return indicates whether RConnection is started.
     */
    TBool IsStarted();

    /**
     * @since Series60 5.0 HN
     * @return active iap
     */
    TInt ActiveIap();
    
    /**
     * Returns inet address
     * @since Series60 5.0 HN
     * @return local IP address
     */
    const TInetAddr& LocalAddress();
    
    /**
     * Checks if left any session which called EnsureStart(), 
     * when not then RConnection is closed
     * @since Series60 5.0 HN
     * @param Session wich call this function
     * @return None
     */
    void CheckAndCloseConnection( CUpnpConnManagerSession* aSession );
    
private:
    /**
     * Default constructor.
     */
    CUpnpConnManagerEngine( TDblQueIter<CSession2>& aIterator );

    /**
     * 2nd phase constructor.
     */
    void ConstructL();

    /**
     * Opens and connects RConnection, subscribes for IP event notyfications 
     * (for address change) and Connection closed notyfication. In case of error
     * Leaves.
     * @since Series60 5.0 HN
     * @return None
     */
    void EnsureStartL();

    /**
     * Set state that one attempt to start the RConnection
     * @param aError error code of starting
     * @param aUSec microsecond for timeout
     */
    void SetStartAttemptStatus( TInt aError, TTimeIntervalMicroSeconds32 aUSec );

    /**
     * Reset state. One will be able to attempt to start RConnection again
     * @since Series60 5.0 HN
     * @return None
     */
    void ClearStartAttemptStatus();
    
    /**
     * Cancel all subscription to network event
     * @since Series60 5.0 HN
     * @return None
     */
    void CancelAllSubscription();
    
    /**
     * Notify all clients about wlan lost event
     * @since Series60 5.0 HN
     * @return None
     */                                 
    void NotifyAllAboutWlanLost();
            
    /**
     * Notify all clients about address change event
     * @since Series60 5.0 HN
     * @return None
     */                                 
    void NotifyAllAboutAddressChange( TInetAddr& aAddr );

    /**
     * Notify all clients about connection closed event
     * @since Series60 5.0 HN
     * @return None
     */                                 
    void NotifyAllAboutConnectionClosed();

    /**
     * Closes RConnection
     * @since Series60 5.0 HN
     * @return None
     */                                 
    void CloseConnection();

    /**
     * Gets Iap from central repository and sets iCommDbConnPref.
     * @since Series60 5.0 HN
     * @return None
     */                                 
    void SetupConnectionPreferencesL();
            
private: // from MUpnpConnInterfaceWatcherObserver    
        
    /**
     * Marks connection interface as not vaild
     */
    void WlanNetworkLost();

    /**
     * Invoked when address changes
     * @param new address
     */
    void InterfaceAddressChange( TInetAddr& aAddr );
        
private: // from MUpnpNotifyTimerObserver

    void TimerEventL( CUpnpNotifyTimer* aTimer );

private:
    // Connection preferences
    TCommDbConnPref iCommDbConnPref;

    // Connection
    RConnection iConnection;

    // SocketServer
    RSocketServ iSocketServ;

    // have we tried to start the interface
    TInt iStartAttemptDone;

    // error code of starting
    TInt iCachedStartError;

    // hysteresis timer preventing double asking 
    // when first attempt fails e.g. when user cancels starting of IAP
    CUpnpNotifyTimer* iUnsuccesfulStartCacheTimer;
    
    // it is needed only for "always ask" case
    TInt iActiveIap;

    // connection interface watcher
    CUpnpConnInterfaceWatcher* iIfWatcher;
    
    // session iterator
    TDblQueIter<CSession2>& iSessionIter;
    
    };


#endif // C_CUPNPCONNMANAGERENGINE_H
