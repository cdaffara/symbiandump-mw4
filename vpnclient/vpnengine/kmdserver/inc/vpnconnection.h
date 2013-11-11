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
* Description:  VPN connection specific structures
*
*/


#ifndef C_VPNCONNECTION_H
#define C_VPNCONNECTION_H

#include <e32base.h>
#include <in_sock.h>
#include "vpnmandefs.h"
#include "disconnectionobserver.h"
#include "eventmediatorapi.h"
#include "ikepluginsessionhandler.h"
#include "ikesocketdefs.h"

// FORWARD DECLARATIONS
class CIkeConnectionInterface;
class CIkeData;
class CIkePluginSessionInterface;
class CKmdServer;
class MIkeDebug;

/**
 *  VPN connection.
 *  VPN connection class provides VPN connection specific functionality for
 *  managing real network connection and connection with remote IKE host.
 *
 *  @lib internal (kmdserver.exe)
 */
class CVpnConnection : public CBase,
                       public MDisconnectionObserverCallback,
                       public MIkePluginSessionHandlerCallback
    {
public:    
    /**
     * Two-phased constructor.
     * @param aVpnIapId VPN IAP Id
     * @param aServer KMD server
     * @param aDebug Debug trace interface
     */
    static CVpnConnection* NewLC( TUint32 aVpnIapId,
                                  CKmdServer& aServer,
                                  MIkeDebug& aDebug );
    /**
     * Destructor.
     */
    ~CVpnConnection();
    
    /**
     * Starts real network connection.
     * @param aStatus Completion status (returned)
     */
    void StartRealConnection( TRequestStatus& aStatus );

    /**
     * Cancels connection starting.
     */
    void CancelStartRealConnection();
        
    /**
     * Resolves an IP address from FQDN address.
     * @param aFqdn FQDN address
     * @param aNameEntry Result of name resolution (returned)
     * @param aStatus Completion status (returned)
     */
    void ResolveAddress( const TDesC& aFqdn,
                         TNameEntry& aNameEntry,
                         TRequestStatus& aStatus );
    
    /**
     * Cancels resolving.
     */
    void CancelResolveAddress();
            
    /**
     * Starts negotiation with a remote host.
     * @param aIkeData IKE policy data
     * @param aVpnInterfaceIndex
     * @param aIpVersion IP version
     * @param aInternalAddress Internal address (returned)
     * @param aStatus Completion status (returned)
     */
    void NegotiateWithHost( CIkeData& aIkeData,
                            TUint32 aVpnInterfaceIndex,
                            IkeSocket::TIpVersion aIpVersion,
                            TVPNAddress& aInternalAddress,
                            TRequestStatus& aStatus );
    
    /**
     * Cancels negotiation.
     */
    void CancelNegotiateWithHost();
    
    /**
     * Stops VPN connection.
     * @param aSilentClose Specified if a silent close in question (Delete
     *                     payloads not transmitted to remote host)
     * @param aStatus Completion status (returned)
     */
    void StopVpnConnection( TBool aSilentClose,
                            TRequestStatus& aStatus );

    /**
     * Cancels VPN connection stopping. VPN connection is stopped silently.
     */
    void CancelStopVpnConnection();    
    
    /**
     * Gets local IP address of real network interface.
     *
     * @param aIpVersion IP version of local IP address
     * @param aLocalIp Local IP address (returned)
     * @return Error status. KErrNotFound if address is not found.
     */
    TInt GetLocalAddress( const IkeSocket::TIpVersion aIpVersion,
                          TInetAddr& aLocalIp );

    /**
     * Returns VPN IAP Id.
     * @return VPN IAP Id
     */
    TInt VpnIapId() const;

    /**
     * Returns real IAP Id.
     * @return Real IAP Id
     */
    TInt RealIapId() const;

    /**
     * Returns real NET Id.
     * @return Real NET Id
     */
    TInt RealNetId() const;

// from base class MDisconnectionObserverCallback
    
    /**
     * Notification about link disconnection. VPN connection is stopped silently.
     * @param aStatus Completion status
     */
    void DisconnectIndication( TInt aStatus );
    
// from base class MIkePluginSessionHandlerCallback
    
    /**
     * From MIkePluginSessionHandlerCallback
     * Notification about completion of negotiate request.
     * @param aStatus Completion status
     * @param aInternalAddress Internal address.
     */    
    void NegotiationStarted( TInt aStatus,
                             const TVPNAddress& aInternalAddress );    
    /**
     * From MIkePluginSessionHandlerCallback
     * Notification about completion of delete session request. Real
     * network connection is closed.
     * @param aStatus Completion status
     */    
    void IkePluginSessionClosed( TInt aStatus );    

    /**
     * From MIkePluginSessionHandlerCallback
     * Notification about IKE plugin session error.
     * @param aStatus Completion status
     */
    void IkePluginSessionError( TInt aStatus );    
    
    
private:
    CVpnConnection( TUint32 aVpnIapId,
                    CKmdServer& aServer,
                    MIkeDebug& aDebug );
    void ConstructL();
    
    /**
     * Cancels negotiation..
     */
    void DoCancelNegotiateWithHost();
    
    /**
     * Stops VPN connection.
     */
    void DoStopVpnConnection( TBool aSilentClose );
    
    /**
     * Cancels VPN connection stopping. VPN connection is stopped silently.
     */
    void DoCancelStopVpnConnection();

    /**
     * Deletes IKE plugin session.
     * @param aSilentClose Specified if a silent close in question (Delete
     *                     payloads not transmitted to remote host)
     */    
    void DoDeleteSession( TBool aSilentClose );

    /**
     * Cancels session deletion.
     */
    void DoCancelDeleteSession();
        
    /**
     * Stops real network connection.
     * @param aStatus Disconnect event status to be reported.
     */
    void DoStopRealConnection( TInt aStatus );
    
    /**
     * Reports disconnect event via Event Mediator API.
     * @param aStatus Completion status
     */
    void DoReportDisconnectEvent( TInt aStatus );    

private: // data
    
    /**
     * KMD server.
     * Not own.
     */
    CKmdServer&                 iServer;
    
    /**
     * IKE version.
     * Own.
     */
    TInt iIkeVersion;
    
    /**
     * VPN IAP id.
     * Own.
     */
    TUint32                     iVpnIapId;
            
    /**
     * VPN NET id.
     * Own.
     */
    TUint32                     iVpnNetId;
    
    /**
     * Real IAP id.
     * Own.
     */
    TInt                        iRealIapId;

    /**
     * Real NET id.
     * Own.
     */
    TInt                        iRealSnapId;
    
    /**
     * Informs if disconnect event has been received.
     * Own.
     */
    TInt                        iDisconnectEventReceived;

    /**
     * Disconnection status.
     * Own.
     */
    TInt                        iDisconnectionStatus;

    /**
     * IKE connection interface.
     * Own.
     */    
    CIkeConnectionInterface*    iIkeConnection;
    
    /**
     * Disconnection observer.
     * Own.
     */    
    CDisconnectionObserver*     iDisconnectionObserver;

    /**
     * IKE plugin session handler.
     * Not own.
     */    
    CIkePluginSessionHandler*   iIkePluginSessionHandler;
                
    /**
     * Event mediator.
     * Own.
     */    
    REventMediator              iEventMediator;
    
    /**
     * Client's request status for starting of negotiation.
     * Not own.
     */    
    TRequestStatus*             iClientStatusNegotiate;
    
    /**
     * Client's internal address.
     * Not own.
     */    
    TVPNAddress*                iClientInternalAddress;
    
    /**
     * Client's request status for VPN connection's stopping.
     * Not own.
     */    
    TRequestStatus*             iClientStatusStopVpnConnection;
    
    /**
     * Debug trace interface.
     * Not own.
     */
    MIkeDebug&                  iDebug;
    };

#endif // C_VPNCONNECTION_H
