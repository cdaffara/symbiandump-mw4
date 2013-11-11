/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: IKEv1 protocol plugin interface.
*
*/

#ifndef C_IKEV1PLUGIN_H
#define C_IKEV1PLUGIN_H

#include <e32base.h>

#include "ikepluginif.h"
#include "pfkeysocketif.h"

// FORWARD DECLARATIONS
class CIkev1PluginSession;
class CIpsecPolicyUtil;
class CIpsecSaSpecList;
class CPFKeySocketIf;
class MIkeDataInterface;
class MIkeDebug;
class MKmdEventLoggerIf;
class TIpsecSPI;
class TInetAddr;

/** 
* Ikev1 protocol plugin 
* @internalComponent
*/
/**
 *  IKEv1 protocol plugin.
 *  
 *  @lib internal (ikev1lib.lib)
 */

NONSHARABLE_CLASS(CIkev1Plugin) : public CBase,
                                  public MIkePluginIf,
                                  public MPFKeyMessageListener
    {
public:
    /**
     * Two-phased constructor.
     * @param aEventLogger Event logger interface
     * @param aDebug Debug trace interface
     */
	static CIkev1Plugin* NewL( MKmdEventLoggerIf& aEventLogger,
	                           MIkeDebug& aDebug );
    /**
     * Destructor.
     */
	~CIkev1Plugin();
		
    /**
     * Handles IKE plugin session deletion.
     * @param aPluginSession IKE plugin session
     */
	void IkePluginSessionDeleted( CIkev1PluginSession* aPluginSession );
	
    /**
     * Gets acceptable IPsec policies for specified selectors.
     * 
     * @param aLocalAddr IP address, including possible port, of the local end selector
     * @param aLocalMask Local end selector mask
     * @param aRemoteAddr IP address, including possible port, of the remote end selector
     * @param aRemoteMask Remote end selector mask
     * @param aProtocol Protocol id
     * @param aVpnNetId VPN net id
     */
	CIpsecSaSpecList* GetIpseSaSpecListLC( const TInetAddr& aLocalAddr, const TInetAddr& aLocalMask, 
	                                       const TInetAddr& aRemoteAddr, const TInetAddr& aRemoteMask,
	                                       TInt aProtocol, TUint32 aVpnNetId );
    
    /** 
     * Returns UID.
     */
    TUint32 Uid();
    
    /** 
     * Returns event logger interface.
     */
    MKmdEventLoggerIf& EventLogger();
    
	
// Methods to build and send PFKEY API primitives to IPsec
    
    void AcquireSAError( TIpsecSAData& aSAData,
                         TInt aError );
    
    void UpdateSAL( TIpsecSAData& aSaData );
    
    void AddSAL( TIpsecSAData& aSaData );
    
    void DeleteIpsecSA( TIpsecSPI& aIpsecSpi );
    
    void DeleteIpsecSA( TUint32 aSPI,
                        TInetAddr& aSrc, 
                        TInetAddr& aDst,
                        TUint8 aProtocol );
           		
// from base class MIkePluginIf
	
    /**
     * From MIkePluginIf.
     * Creates IKEv1 plugin session.
     * @param aVpnIapId VPN IAP id
     * @param aVpnNetId VPN NET id
     * @param aVpnInterfaceIndex VPN interface index
     * @param aDataInterface IKE data interface.
     * @return IKEv1 plugin session. Ownership transferred.
     */
    MIkePluginSessionIf* CreateSessionL( TUint32 aVpnIapId,
                                         TUint32 aVpnNetId,
                                         TUint32 aVpnInterfaceIndex,
                                         MIkeDataInterface& aDataInterface );    
	
// from base class MPFKeyMessageListener    

    /**
     * From MPFKeyMessageListener.
     * Handles received PFKEY message.
     * @param aPfkeyMessage PFKEY message
     */
    void PfkeyMessageReceived( const TPfkeyMessage& aPfkeyMessage );

private:	
    CIkev1Plugin( MKmdEventLoggerIf& aEventLogger,
                  MIkeDebug& aDebug );
    void ConstructL();    		
    
	
private:
	
    /**
     * VPN PF key socket.
     * Own.
     */
    CPFKeySocketIf*                 iPFKeySocket;
    
    /**
     * IPSec policy util.
     * Own.
     */
    CIpsecPolicyUtil*               iIpsecPolicyUtil;

    /**
     * IKEv1 plugin sessions.
     * Not own.
     */
    RArray<CIkev1PluginSession*>    iPluginSessions;
        
    /**
     * Event logger.
     * Not own.
     */
    MKmdEventLoggerIf&              iEventLogger;
    
    /**
     * Debug trace interface.
     * Not own.
     */
	MIkeDebug&                      iDebug;
    };


#endif // C_IKEV1PLUGIN_H
