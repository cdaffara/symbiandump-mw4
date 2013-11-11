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
* Description:  upnpconnectionmanager / client side interface
*
*/

#ifndef R_UPNPCONNECTIONMANAGERSESSION_H
#define R_UPNPCONNECTIONMANAGERSESSION_H

#include "upnpconnectionmanagernetworkevent.h"
#include "upnpsessionbase.h"

/**
 * Client session to UPnP Connection Manager
 *
 * @lib upnpconnectionmanagersession
 * 
 * @since S60 5.0
 */
NONSHARABLE_CLASS( RUpnpConnectionManagerSession ) : public RUpnpSessionBase
    {
public:

    /**
     * Constructor.
     */
    RUpnpConnectionManagerSession();

    /**
     * Establish connection with server
     */
    TInt Connect();

    /**
     * Start RConnection if not started, otherwise do nothing
     *
     * @param aResult error code
     */
    TInt RequestEnsureStartRConnection( TInt& aResult );

    /**
     * Can be used to check whether RConnection has been started
     *
     * @return aResult ETrue if connection has been started
     */
    void RequestIsRConnectionStarted( TBool& aResult );

    /**
     * Get current iap
     *
     * @return iap
     */
    TInt RequestActiveIap();
    
    /**
     * Get current address
     *
     * @return address
     */
    TInetAddr RequestLocalAddress();        

    /**
     * Subscribes to network event
     *
     * @param aStatus subscribtion connection
     * @param aNetworkEvent network event
     */
    void RequestSubscribeToNetworkEvents( TRequestStatus& aStatus, 
                                          TUpnpConnectionManagerNetworkEvent& aNetworkEvent );

    /**
     * Cancel subscribtion for network event
     *
     */
    void RequestCancelSubscribeToNetworkEvents();

private:

    /**
     * Pointer for the first parameter
     */
    TPtr8       iParameter0;
    
    /**
     * Pointer for the network event 
     */
    TPtr8       iNetworkEventParameter;
    
    };

#endif // R_UPNPCONNECTIONMANAGERSESSION_H

// End of File
