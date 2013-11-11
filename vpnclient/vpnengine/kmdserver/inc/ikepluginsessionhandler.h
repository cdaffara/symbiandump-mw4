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
* Description:  Handler of an IKE protocol plugin session
*
*/


#ifndef C_IKEPLUGINSESSIONHANDLER_H
#define C_IKEPLUGINSESSIONHANDLER_H

#include <e32base.h>
#include "vpnmandefs.h"
#include "ikesocketdefs.h"

// FORWARD DECLARATIONS
class CErrorObserver;
class CIaChangeObserver;
class CIkeData;
class CIkeConnectionInterface;
class CIkePluginSessionCloser;
class CInternalAddress;
class MIkeDataInterface;
class MIkeDebug;
class MIkePluginIf;
class MIkePluginSessionIf;
class TInetAddr;

/**
 *  IKE plugin session handler callback interface.
 *  Callback interface which is used by CIkePluginSessionHandler object to
 *  notify about completion of negotiate and delete session requests.
 * 
 *  @lib internal (kmdserver.exe)
 */
class MIkePluginSessionHandlerCallback
    {
public:
    /**
     * Notifies about completion of negotiate request.
     * @param aStatus Completion status
     * @param aInternalAddress Internal address
     */    
    virtual void NegotiationStarted( TInt aStatus,
                                     const TVPNAddress& aInternalAddress ) = 0;
    /**
     * Notifies about completion of delete session request.
     * @param aStatus Completion status
     */    
    virtual void IkePluginSessionClosed( TInt aStatus ) = 0;
    
    /**
     * Notifies about IKE plugin session error.
     * @param aStatus Completion status
     */
    virtual void IkePluginSessionError( TInt aStatus ) = 0;    
    };

/**
 *  IKE plugin session handler.
 *  Provides functionality for handling IKE plugin session.   
 *  
 *  @lib internal (kmdserver.exe)
  */
class CIkePluginSessionHandler : public CActive
    {
public:
    /**
     * Two-phased constructor.
     * @param aVpnIapId VPN IAP id
     * @param aVpnNetId VPN NET id
     * @param aVpnInterfaceIndex VPN interface index
     * @param aIkeVersion IKE version
     * @param aIpVersion IP version
     * @param aDnsServerAddr DNS server address from IKE policy
     * @param aConnection IKE connection interface
     * @param aIkePlugin IKE plugin
     * @param aCallback Callback interface
     * @param aDebug Debug trace interface
     */
    static CIkePluginSessionHandler* NewLC( TUint32 aVpnIapId,
                                            TUint32 aVpnNetId,
                                            TUint32 aVpnInterfaceIndex,
                                            IkeSocket::TIkeMajorVersion aIkeVersion,
                                            IkeSocket::TIpVersion aIpVersion,
                                            const TInetAddr& aDnsServerAddr,
                                            CIkeConnectionInterface& aConnection,
                                            MIkePluginIf& aIkePlugin,
                                            MIkePluginSessionHandlerCallback& aCallback,
                                            MIkeDebug& aDebug );
    /**
     * Destructor. IKE plugin session object is deleted.
     */
    ~CIkePluginSessionHandler();
                        
    /**
     * Starts negotiation with a remote host.
     * @param aIkeData IKE policy data
     */
    void NegotiateWithHost( const CIkeData& aIkeData );
    
    /**
     * Cancels negotiation request.
     */
    void CancelNegotiateWithHost();
    
    /**
     * Deletes session. Silent close can be requested when normal close is
     * already active. IKE plugin session object is not deleted.
     * @param aSilentClose Specified if a silent close in question (Delete
     * payloads not transmitted to remote host)
     */
    void DeleteSession( TBool aSilentClose );
    
    /**
     * Cancels session deletion requests. IKE/IPSec SA:s are deleted. IKE
     * plugin session object is not deleted.
     */
    void CancelDeleteSession();
    
    /**
     * Returns VPN IAP Id.
     * @return VPN IAP Id
     */
    TInt VpnIapId() const;
    
// from base class CActive
    
    /**
     * From CActive
     * Handles completion of asynchronous request.
     */    
    void RunL();
    
    /**
     * From CActive
     * Handles cancellation of asynchronous request.
     */    
    void DoCancel();
    
private:
    enum TState
        {
        EIdle,
        ENegotiatingWithHost,
        ENegotiated,
        EDeletingSession        
        };
    
    CIkePluginSessionHandler( TUint32 aVpnIapId,
                              IkeSocket::TIkeMajorVersion aIkeVersion,
                              IkeSocket::TIpVersion aIpVersion,
                              MIkePluginSessionHandlerCallback& aCallback,
                              MIkeDebug& aDebug );
    
    void ConstructL( TUint32 aVpnNetId,
                     TUint32 aVpnInterfaceIndex,
                     const TInetAddr& aDnsServer,
                     CIkeConnectionInterface& aConnection,
                     MIkePluginIf& aIkePlugin );
    
private: // data    
    
    /**
     * State.
     * Own.
     */            
    TState                            iState;
    
    /**
     * VPN IAP Id.
     * Own.
     */            
    TUint32                            iVpnIapId;
    
    /**
     * IKE version.
     * Own.
     */            
    IkeSocket::TIkeMajorVersion        iIkeVersion;
    
    /**
     * IP version.
     * Own.
     */            
    IkeSocket::TIpVersion               iIpVersion;

    /**
     * IKE plugin session interface.
     * Own.
     */        
    MIkePluginSessionIf*                iIkePluginSession;
    
    /**
     * Internal address.
     * Own.
     */        
    TVPNAddress                        iInternalAddress;
    
    /**
     * IKE plugin session error observer.
     * Own.
     */        
    CErrorObserver*                     iErrorObserver;

    /**
     * Internal address change observer.
     * Own.
     */        
    CIaChangeObserver*                  iIaChangeObserver;
    
    /**
     * IKE data interface.
     * Not own.
     */    
    MIkeDataInterface*                  iIkeDataInterface;
    
    /**
     * Callback interface.
     * Not own.
     */    
    MIkePluginSessionHandlerCallback&   iCallback;
    
    /**
     * Debug trace interface.
     * Not own.
     */    
    MIkeDebug&                          iDebug;
    };


#endif // C_IKEPLUGINSESSIONHANDLER_H
