/** @file
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CUpnpConnectionProxy
*
*/

#ifndef C_CUPNPCONNECTIONPROXY_H
#define C_CUPNPCONNECTIONPROXY_H

// INCLUDE FILES
#include <e32base.h>
#include <es_sock.h>
#include <es_enum.h>
#include <in_sock.h>
#include "upnpconnectionmanagersession.h"

// CLASS DECLARATION

class CUpnpConnectionManagerNetworkEventProvider;
class MUpnpNetworkEventObserver;

/**
 * This class is a client handle to CUpnpConnectionManager.
 * One should use it rather than RUpnpConnectionManagerSession directly
 * @since S60 5.0
 */
NONSHARABLE_CLASS( CUpnpConnectionManagerProxy ) : public CBase
    {
public: // Constructors and destructor

    /**
     * Two-phased constructor.
     * @param aSocketServ Pointer to used Socket Server.
     * @param aIap the IAP ID of the connection.
     * @since Series60 3.2
     * @return A new CUpnpConnectionProxy object.
     */
    IMPORT_C static CUpnpConnectionManagerProxy* NewL( RSocketServ& aSocketServer );

    /**
     * Two-phased constructor.
     * @param aSocketServ Pointer to used Socket Server.
     * @param aIap the IAP ID of the connection.
     * @since Series60 3.2
     * @return A new CUpnpConnectionProxy object.
     */
    IMPORT_C static CUpnpConnectionManagerProxy* NewLC( RSocketServ& aSocketServer );

    /**
     * Destructor.
     * @since Series60 3.2
     */
    virtual ~CUpnpConnectionManagerProxy();

public: // New functions

    /**
     * After call of this method the caller is sure that
     * RConnection was started.
     */
    IMPORT_C TInt EnsureStart();

    /**
     * Returns RConnection instance.
     * Leaves only if ensure start wasn't invoked, and
     * internal EnsureStart call fail
     */
    IMPORT_C RConnection& ConnectionL();

    /**
     * Returns active connection IAP.
     */
    IMPORT_C TInt ActiveIap();

    /**
     * Returns active connection IP address.
     */
    IMPORT_C TInetAddr LocalAddress();

    /**
     * Subscribes for network events nitifications.
     */
    IMPORT_C void SubscribeForNetworkEventsL( MUpnpNetworkEventObserver *aObserver );
    
    /**
     * Static function for querying of active connection IAP
     * without creating new object (it is created and closed internally)
     * @return KErrNontFound if connection is not started else active IAP,
     *         in case of Always ask (IAP=0) IAP selected by user (IAP>0)
     */
    IMPORT_C static TInt GetActiveIapL();
    
private:

    /**
     * Constructor.
     * @since Series60 3.2
     */
    CUpnpConnectionManagerProxy();

    /**
     * By default Symbian 2nd phase constructor is private.
     * @param aSocketServ Pointer to used Socket Server.
     */
    void ConstructL( RSocketServ& aSocketServer );

    /**
     * Attaches connection
     */
    TInt Attach( TInt aAccessPoint );

    /**
     * Checks whether EnsureStart has been invoked.
     */
    TBool IsStarted();

private: 

    // Connection
    RConnection iConnection;

    // Session to ConnectionManager in MessageHandler process
    RUpnpConnectionManagerSession iConnectionManagerSession;

    // Active Iap variable.
    TInt iActiveIap;
    
    // Class used for network event notifications.
    CUpnpConnectionManagerNetworkEventProvider *iNetworkEventProvider;
    };

#endif // C_CUPNPCONNECTIONPROXY_H

// End of file
