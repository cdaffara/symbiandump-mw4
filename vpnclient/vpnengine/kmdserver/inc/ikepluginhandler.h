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
* Description:  Handler of an IKE protocol plugin
*
*/


#ifndef C_IKEPLUGINHANDLER_H
#define C_IKEPLUGINHANDLER_H

#include <e32base.h>
#include "ikepluginsessionhandler.h"
#include "ikesocketdefs.h"

// FORWARD DECLARATIONS
class CIkePluginSessionHandler;
class MIkeDebug;
class MIkePluginIf;
class MKmdEventLoggerIf;

/**
 *  IKE plugin handler.
 *  IKE plugin specific handler for creating and deleting IKE plugin sessions. 
 *  
 *  @lib internal (kmdserver.exe)
  */
class CIkePluginHandler : public CBase
    {
public:
    /**
     * Two-phased constructor.
     * @param aIkeVersion IKE version
     * @param aEventLogger Event logger
     * @param aDebug Debug trace interface
     */
    static CIkePluginHandler* NewL( TInt aIkeVersion,
                                    MKmdEventLoggerIf& aEventLogger,
                                    MIkeDebug& aDebug );

    /**
     * Destructor.
     */
    ~CIkePluginHandler();
    
    /**
     * Creates IKE plugin session.
     * @param aVpnIapId VPN IAP id
     * @param aVpnNetId VPN NET id
     * @param aVpnInterfaceIndex VPN interface index
     * @param aIpVersion IP version
     * @param aDnsServer DNS server from IKE policy
     * @param aConnection IKE connection interface
     * @param aCallback Callback interface
     * @return Handler of IKE plugin session
     */
    CIkePluginSessionHandler& CreateIkePluginSessionL( TUint32 aVpnIapId,
                                                       TUint32 aVpnNetId,
                                                       TUint32 aVpnInterfaceIndex,
                                                       IkeSocket::TIpVersion aIpVersion,
                                                       const TInetAddr& aDnsServer,
                                                       CIkeConnectionInterface& aConnection,
                                                       MIkePluginSessionHandlerCallback& aCallback );    

    /**
     * Deletes IKE plugin session which matches VPN IAP id. Does nothing if
     * session is not found.
     * @param aVpnIapId VPN IAP Id
     */
    void DeleteIkePluginSession( TUint32 aVpnIapId );
        			
private:
    CIkePluginHandler( TInt aIkeVersion,
                       MKmdEventLoggerIf& aEventLogger,
                       MIkeDebug& aDebug );
    
    /**
     * Creates IKE plugin session.
     * @param aVpnIapId VPN IAP id
     * @param aVpnNetId VPN NET id
     * @param aVpnInterfaceIndex VPN interface index
     * @param aIpVersion IP version
     * @param aDnsServerAddr DNS server address from IKE policy
     * @param aConnection IKE connection interface
     * @param aCallback Callback interface
     */
	CIkePluginSessionHandler& DoCreateIkePluginSessionL( TUint32 aVpnIapId,
	                                                     TUint32 aVpnNetId,
	                                                     TUint32 aVpnInterfaceIndex,
                                                         IkeSocket::TIpVersion aIpVersion,
                                                         const TInetAddr& aDnsServerAddr,
                                                         CIkeConnectionInterface& aConnection,
                                                         MIkePluginSessionHandlerCallback& aCallback );
	
    /**
     * Loads IKE plugin.
     */
	void LoadIkePluginL();

    /**
     * Unloads IKE plugin.
     */
	void UnloadIkePlugin();		
	
private: // data	
    
    /**
     * IKE version.
     * Own.
     */
    TInt                                        iIkeVersion;
    
    /**
     * Handle for IKE plugin library.
     * Own.
     */
    RLibrary                                    iLibrary;
	
    /**
     * IKE plugin.
     * Own.
     */
    MIkePluginIf*                               iIkePlugin;
    	
    /**
     * IKE plugin session handlers.
     * Own.
     */
	RPointerArray<CIkePluginSessionHandler>     iIkePluginSessions;
		
    /**
     * Event logger.
     * Not own.
     */
    MKmdEventLoggerIf&                          iEventLogger;	
	
    /**
     * Debug trace interface.
     * Not own.
     */
    MIkeDebug&                                  iDebug;
    };


#endif // C_IKEPLUGINHANDLER_H
