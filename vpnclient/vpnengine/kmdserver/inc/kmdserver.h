/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  KMD server
*
*/


#ifndef C_KMDSERVER_H
#define C_KMDSERVER_H

#include <e32base.h>

#include "ikesocketdefs.h"
#include "vpnmandefs.h"

_LIT( KKmdServerName, "!KMD server" );
_LIT( KKmdServerImg, "kmdserver" );

const TUid KServerUid3 = { 0x1000088A };

const TInt KMyServerStackSize = 0x2000;           //  8KB
const TInt KMyServerInitHeapSize = 0x1000;        //  4KB
const TInt KMyServerMaxHeapSize = 0x1000000;      // 16MB

const TInt KKmdServMajorVersion = 1;
const TInt KKmdServMinorVersion = 0;
const TInt KKmdServBuildVersion = 0;

// FORWARD DECLARATIONS
class CIkeConnectionInterface;
class CIkePluginHandler;
class CIkePluginSessionHandler;
class CKmdSession;
class CVpnConnection;
class CKmdDebugLogger;
class CKmdEventLogger;
class CSecpolReader;
class MIkeDebug;
class MIkePluginSessionHandlerCallback;

/**
 *  KMD server.
 *  Implementation of KMD (Key Management Daemon) server. Handles creating of
 *  server-side client sessions.
 *
 *  @lib internal (kmdserver.exe)
 */
class CKmdServer : public CPolicyServer
    {
public:

    /**  KMD commands */
    enum 
        { 
        KKmdStartConnection,
        KKmdCancelStartConnection, 
        KKmdActivateAsync,
        KKmdCancelActivateAsync,        
        KKmdStopConnection,
        KKmdResolveAddress,
        KKmdCancelResolveAddress        
        };
    
    /**
     * Two-phased constructor. Called from kmd_starter.
     */    
    static CKmdServer* NewL();
    
    /**
     * Destructor.
     */
    ~CKmdServer();
    
    /**
     * Informs KMD server that KMD session has been closed.
     */
    void KmdSessionClosed();
    
    /**
     * Creates VPN connection object.
     * @param aVpnIapId VPN IAP Id
     */
    CVpnConnection& CreateVpnConnectionL( TUint32 aVpnIapId );
    
    /**
     * Gets VPN connection object.
     * @param aVpnIapId VPN IAP Id
     */
    CVpnConnection& GetVpnConnectionL( TUint32 aVpnIapId );
    
    /**
     * Deletes VPN connection object.
     * @param aVpnIapId VPN IAP Id
     */
    void DeleteVpnConnection( TUint32 aVpnIapId );
    
    /**
     * Creates IKE plugin session.
     * @param aIkeVersion IKE version
     * @param aIpVersion IP version
     * @param aConnection IKE connection interface
     * @param aVpnIapId VPN IAP id
     * @param aVpnNetId VPN NET id
     * @param aVpnInterfaceIndex VPN interface index
     * @param aDnsServerAddr DNS server address from IKE policy.
     * @param aCallback Callback interface
     * @return IKE plugin session handler
     */
    CIkePluginSessionHandler& CreateIkePluginSessionL( TInt aIkeVersion,
                                                       IkeSocket::TIpVersion aIpVersion,
                                                       CIkeConnectionInterface& aConnection,
                                                       TUint32 aVpnIapId,
                                                       TUint32 aVpnNetId,
                                                       TUint32 aVpnInterfaceIndex,
                                                       const TInetAddr& aDnsServerAddr,
                                                       MIkePluginSessionHandlerCallback& aCallback );    
    /**
     * Deletes IKE plugin session.
     * @param aIkeVersion IKE version
     * @param aVpnIapId VPN IAP Id
     */
    void DeleteIkePluginSession( TInt aIkeVersion,
                                 TUint32 aVpnIapId );
    
    /**
     * Returns debug trace interface.
     */
    MIkeDebug& Debug();

// from base class CPolicyServer

    CSession2* NewSessionL( const TVersion& aVersion,
                            const RMessage2& aMessage ) const;

private:
    CKmdServer();
    void ConstructL();            

    /**
     * Stops KMD server if there are no more KMD sessions or VPN connections.
     */
    void StopKmdServer();

private: // data
    
    /**
     * KMD session count.
     * Own.
     */
    mutable TInt32                              iSessionCount;
    
    /**
     * VPN connections.
     * Own.
     */
    RPointerArray<CVpnConnection>               iVpnConnections;
    
    /**
     * IKE plugin handlers.
     * Own.
     */
    CIkePluginHandler*                          iIkePluginHandlers[2];
    
    /**
     * Debug logger.
     * Own.
     */
    CKmdDebugLogger*                            iDebugLogger;
    
    /**
     * Event logger.
     * Own.
     */
    CKmdEventLogger*                            iEventLogger;
    
    /**
     * Secpol reader.
     * Own.
     */
    CSecpolReader*                              iSecpolReader;
    
    /**
     * Policy server.
     * Own.
     */    
    RIpsecPolicyServ                            iIpsecPolicyServ;

    /**
     * KMD server range count.
     * Own.
     */
    static const TUint                          iKmdServerRangeCount;
    
    /**
     * KMD server ranges.
     * Own.
     */
    static const TInt                           iKmdServerRanges[];
    
    /**
     * KMD server element index.
     * Own.
     */
    static const TUint8                         iKmdServerElementIndex[];

    /**
     * KMD server elements.
     * Own.
     */
    static const CPolicyServer::TPolicyElement  iKmdServerElements[];
    
    /**
     * KMD server policy.
     * Own.
     */
    static const CPolicyServer::TPolicy         iKmdServerPolicy;
    };

#endif // C_KMDSERVER_H
