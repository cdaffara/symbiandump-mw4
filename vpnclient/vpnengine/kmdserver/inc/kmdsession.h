/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Server side session of KMD server
*
*/


#ifndef C_KMDSESSION_H
#define C_KMDSESSION_H

#include <e32base.h>
#include <es_sock.h>

#include "activationstarter.h"
#include "connectionstarter.h"
#include "connectionstopper.h"
#include "fqdnresolver.h"

// FORWARD DECLARATIONS
class CKmdServer;
class CKmdSessionHandler;
class MIkeDebug;
class TVPNAddress;

/**
 *  KMD session.
 *  Server side session of KMD server. Handles client requests.
 *
 *  @lib internal (kmdserver.exe)
 */
class CKmdSession : public CSession2,
                    public MActivationStarterCallback,
                    public MConnectionStarterCallback,
                    public MConnectionStopperCallback,
                    public MFqdnResolverCallback
    {
public: 
               
    /**
     * Two-phased constructor.
     * @param aServer KMD server
     * @param aDebug Debug trace interface
     */
    static CKmdSession* NewL( CKmdServer& aServer,
                              MIkeDebug& aDebug );    

    /**
     * Destructor.
     */
    ~CKmdSession();
    
// from base class CSession2
    
    /**
     * From CSession2
     * Handles client request.
     *
     * @param aMessage Message request
     */
    void ServiceL( const RMessage2& aMessage );
        
// from base class MConnectionStarterCallback
    
    /**
     * From MConnectionStarterCallback
     * Notification about completion of real connection starting.
     *
     * @param aStatus Completion status
     * @param aRealIapId IAP Id of started connection
     * @param aRealNetId Net Id of started connection
     */
    void RealConnectionStarted( TInt aStatus,
                                TInt aRealIapId,
                                TInt aRealNetId );
    
// from base class MConnectionStopperCallback
    
    /**
     * From MConnectionStopperCallback
     * Notification about completion of VPN connection stopping.
     *
     * @param aStatus Completion status
     */
    void VpnConnectionStopped( TInt aStatus );
    
// from base class MFqdnResolverCallback
    
    /**
     * From MFqdnResolverCallback
     * Notifies about completion of FQDN address resolving.
     *
     * @param aStatus Completion status
     * @param aNameEntry Result of name resolution
     */
    void AddressResolveCompleted( TInt aStatus,
                                  TNameEntry aNameEntry );

// from base class MActivationStarterCallback
    
    /**
     * From MActivationStarterCallback
     * Notification about completion of activation.
     *
     * @param aStatus Completion status
     * @param aVirtualIp Internal address info
     */
    void ActivationCompleted( TInt aStatus,
                              const TVPNAddress& aVirtualIp );
        
private:
    CKmdSession( CKmdServer& aServer,
                 MIkeDebug& aDebug );      
    
    /**
     * Cancels real connection starting.
     */
    void DoCancelStartConnection();
    
    /**
     * Cancels activating
     */
    void DoCancelActivate();
    
    /**
     * Cancels FQDN address resolving.
     */
    void DoCancelResolveAddress();
        
private: // data    
    /**
     * KMD server.
     * Not own.
     */
    CKmdServer&         iServer;    
    
    /**
     * VPN IAP Id.
     * Not own.
     */
    TUint32             iVpnIapId;
    
    /**
     * Connection starter active object.
     * Own.
     */
    CConnectionStarter* iConnectionStarter;  

    /**
     * Connection stopper active object.
     * Own.
     */
    CConnectionStopper*  iConnectionStopper;

    /**
     * FQDN address resolver active object.
     * Own.
     */
    CFqdnResolver*      iFqdnResolver;
    
    /**
     * Activation starter active object.
     * Own.
     */
    CActivationStarter* iActivationStarter;
    
    /**
     * Pending start connection message.
     * Own.
     */
    RMessage2           iPendingStartConnection;

    /**
     * Pending stop connection message.
     * Own.
     */
    RMessage2           iPendingStopConnection;

    /**
     * Pending FQDN resolve message.
     * Own.
     */
    RMessage2           iPendingFqdnResolve;

    /**
     * Pending activate message.
     * Own.
     */
    RMessage2           iPendingActivate;
        
    /**
     * Debug trace interface.
     * Not own.
     */
    MIkeDebug&          iDebug;

    };

#endif // C_KMDSESSION_H
