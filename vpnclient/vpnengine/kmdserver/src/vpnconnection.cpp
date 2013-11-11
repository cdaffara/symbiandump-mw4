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


#include <cmmanagerext.h>
#include <cmpluginvpndef.h>

#include "disconnectionobserver.h"
#include "ikeconnectioninterface.h"
#include "ikedebug.h"
#include "ikeplugindefs.h"
#include "ikepluginsessionhandler.h"
#include "ikepolparser.h"
#include "ikesocketdefs.h"
#include "kmdapi.h" // For error codes
#include "kmdserver.h"

// CLASS HEADER
#include "vpnconnection.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CVpnConnection* CVpnConnection::NewLC( TUint32 aVpnIapId,
                                       CKmdServer& aServer,
                                       MIkeDebug& aDebug )
    {
    CVpnConnection* self = new ( ELeave ) CVpnConnection( aVpnIapId,
                                                          aServer,
                                                          aDebug );
    CleanupStack::PushL( self );
    self->ConstructL();    
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CVpnConnection::~CVpnConnection()
    {
    DEBUG_LOG1( _L("CVpnConnection::~CVpnConnection, VPN IAP id=%d"),
            iVpnIapId );

    delete iDisconnectionObserver;
    delete iIkeConnection;   
    
    iEventMediator.Close();
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CVpnConnection::CVpnConnection( TUint32 aVpnIapId,
                                CKmdServer& aServer,
                                MIkeDebug& aDebug )
 : iServer( aServer ),
   iVpnIapId( aVpnIapId ),
   iDisconnectEventReceived( EFalse ),
   iIkePluginSessionHandler( NULL ),
   iDebug( aDebug )
    {
    DEBUG_LOG1( _L("CVpnConnection::CVpnConnection, VPN IAP id=%d"),
            iVpnIapId );
    }

// ---------------------------------------------------------------------------
// Second phase construction.
// ---------------------------------------------------------------------------
//
void CVpnConnection::ConstructL()
    {    
    User::LeaveIfError( iEventMediator.Connect() );
    
    using namespace CMManager;
    
    RCmManagerExt cmManagerExt;
    cmManagerExt.OpenL();        
    CleanupClosePushL( cmManagerExt );     

    RCmConnectionMethodExt vpnConnectionMethod = cmManagerExt.ConnectionMethodL( iVpnIapId );
    CleanupClosePushL( vpnConnectionMethod );

    iVpnNetId = vpnConnectionMethod.GetIntAttributeL( ECmNetworkId );
    iRealIapId  = vpnConnectionMethod.GetIntAttributeL( EVpnIapId ); 
    iRealSnapId = vpnConnectionMethod.GetIntAttributeL( ECmNextLayerSNAPId );

    CleanupStack::PopAndDestroy( &vpnConnectionMethod );
    CleanupStack::PopAndDestroy( &cmManagerExt );

    __ASSERT_DEBUG( iRealIapId != 0 || iRealSnapId != 0, User::Invariant() );        
    
    iIkeConnection = CIkeConnectionInterface::NewL( iDebug );
    iDisconnectionObserver = CDisconnectionObserver::NewL( *iIkeConnection, *this );    
    iDisconnectionObserver->StartObserving();
    }

// ---------------------------------------------------------------------------
// Starts real network connection.
// ---------------------------------------------------------------------------
//    
void CVpnConnection::StartRealConnection( TRequestStatus& aStatus )
    {
    __ASSERT_DEBUG( iRealIapId != 0 || iRealSnapId != 0, User::Invariant() );
    iIkeConnection->StartConnection( iRealIapId, iRealSnapId, aStatus );
    }
        
// ---------------------------------------------------------------------------
// Cancels real connection starting.
// ---------------------------------------------------------------------------
//    
void CVpnConnection::CancelStartRealConnection()
    {
    iIkeConnection->CancelStartConnection();
    }

// ---------------------------------------------------------------------------
// Resolves an IP address from FQDN address.
// ---------------------------------------------------------------------------
//  
void CVpnConnection::ResolveAddress( const TDesC& aFqdn,
                                     TNameEntry& aNameEntry,
                                     TRequestStatus& aStatus )
    {
    iIkeConnection->ResolveFQDNAddress( aFqdn, aNameEntry, aStatus );
    }

// ---------------------------------------------------------------------------
// Cancels resolving.
// ---------------------------------------------------------------------------
//  
void CVpnConnection::CancelResolveAddress()
    {
    iIkeConnection->CancelResolveFQDNAddress();
    }    
    
// ---------------------------------------------------------------------------
// Starts negotiation with a remote host.
// ---------------------------------------------------------------------------
//  
void CVpnConnection::NegotiateWithHost( CIkeData& aIkeData,
                                        TUint32 aVpnInterfaceIndex,
                                        IkeSocket::TIpVersion aIpVersion,
                                        TVPNAddress& aInternalAddress,
                                        TRequestStatus& aStatus )
    {  
    __ASSERT_DEBUG( iClientStatusNegotiate == NULL,
                    User::Invariant() );
        
    // Store client's request status and internal address.
    iClientStatusNegotiate = &aStatus;
    *iClientStatusNegotiate = KRequestPending;
    iClientInternalAddress = &aInternalAddress;
    
    TInt err( KErrNone );
    if ( aIkeData.iIkeVersion == KIkeV1 || aIkeData.iIkeVersion == KIkeV2 )
        {
        // Create IKE plugin session.
        iIkeVersion = aIkeData.iIkeVersion;

        if ( iIkePluginSessionHandler == NULL )
            {
            TRAP( err, iIkePluginSessionHandler = &iServer.CreateIkePluginSessionL( iIkeVersion,
                                                                                    aIpVersion,
                                                                                    *iIkeConnection,
                                                                                    iVpnIapId,
                                                                                    iVpnNetId,
                                                                                    aVpnInterfaceIndex,
                                                                                    aIkeData.iDnsServer.Address(),
                                                                                    *this ) );
            }
        }
    else
        {
        err = KKmdIkePolicyFileErr;
        }
    
    if ( err != KErrNone )
        {
        User::RequestComplete( iClientStatusNegotiate, err );
        iClientStatusNegotiate = NULL;
        return;
        }
        
    if ( aIkeData.iAddr.Family() == KAfInet )
        {
        aIkeData.iAddr.ConvertToV4Mapped();
        aIkeData.iMask.ConvertToV4Mapped();               
        }       
    aIkeData.iAddr.SetScope( RealNetId() );
    
    // Start negotiation.
    iIkePluginSessionHandler->NegotiateWithHost( aIkeData );                                                 
    }

// ---------------------------------------------------------------------------
// Cancels negotiation.
// ---------------------------------------------------------------------------
//  
void CVpnConnection::CancelNegotiateWithHost()
    {
    DoCancelNegotiateWithHost();
    }


// ---------------------------------------------------------------------------
// Stops VPN connection.
// ---------------------------------------------------------------------------
//  
void CVpnConnection::StopVpnConnection( TBool aSilentClose,
                                        TRequestStatus& aStatus )
    {
    __ASSERT_DEBUG( iClientStatusStopVpnConnection == NULL,
                    User::Invariant() );
        
    // Store client's request status.
    iClientStatusStopVpnConnection = &aStatus;
    *iClientStatusStopVpnConnection = KRequestPending;

    DoStopVpnConnection( aSilentClose );    
    }

// ---------------------------------------------------------------------------
// Cancels VPN connection stoppping. VPN Connection is stopped silently.
// ---------------------------------------------------------------------------
//  
void CVpnConnection::CancelStopVpnConnection()
    {   
    DoCancelStopVpnConnection();    
    }

// ---------------------------------------------------------------------------
// Gets local address of real network interface.
// ---------------------------------------------------------------------------
//  
TInt CVpnConnection::GetLocalAddress( const IkeSocket::TIpVersion aIpVersion,
                                      TInetAddr& aLocalIp )
    {
    return iIkeConnection->GetLocalAddress( aIpVersion, aLocalIp );
    }

// ---------------------------------------------------------------------------
// Returns VPN IAP Id.
// ---------------------------------------------------------------------------
//  
TInt CVpnConnection::VpnIapId() const
    {
    return iVpnIapId;
    }

// ---------------------------------------------------------------------------
// Returns real IAP Id.
// ---------------------------------------------------------------------------
//  
TInt CVpnConnection::RealIapId() const
    {
    return iIkeConnection->IapId();
    }
        
// ---------------------------------------------------------------------------
// Returns real NET Id.
// ---------------------------------------------------------------------------
//  
TInt CVpnConnection::RealNetId() const
    {
    return iIkeConnection->NetId();
    }    

// ---------------------------------------------------------------------------
// From class MDisconnectionObserverCallback.
// Notification about link disconnection. VPN connection is stopped.
// ---------------------------------------------------------------------------
//  
void CVpnConnection::DisconnectIndication( TInt aStatus )
    {
    // Store disconnection status for reporting it to client.
    iDisconnectEventReceived = ETrue;
    iDisconnectionStatus = aStatus;
    
    DEBUG_LOG1( _L("Link disconnected, status=%d"),
            iDisconnectionStatus );    

    if ( iDisconnectionStatus == KErrNone )
        {
        iDisconnectionStatus = KErrDisconnected;
        }
    
    // Stop VPN connection silently.
    DoStopVpnConnection( ETrue );
    }

// ---------------------------------------------------------------------------
// From class MIkePluginSessionHandlerCallback.
// Notification about completion of negotiate request.
// ---------------------------------------------------------------------------
//  
void CVpnConnection::NegotiationStarted( TInt aStatus,
                                         const TVPNAddress& aInternalAddress )
    {
    __ASSERT_DEBUG( iIkePluginSessionHandler != NULL,
                    User::Invariant() );
    
    DEBUG_LOG1( _L("NegotiateWithHost completed, status=%d"),
            aStatus );
        
    // Use disconnection status for reporting if set.
    if ( iDisconnectEventReceived )
        {
        aStatus = iDisconnectionStatus;
        }
    
    if ( iClientStatusNegotiate != NULL )
        {
        *iClientInternalAddress = aInternalAddress;
        
        // Complete client's request.
        User::RequestComplete( iClientStatusNegotiate, aStatus );        
        iClientInternalAddress = NULL;
        iClientStatusNegotiate = NULL;        
        }
    }

// ---------------------------------------------------------------------------
// From class MIkePluginSessionHandlerCallback.
// Notification about completion of delete session request. IKE plugin
// session object is deleted and real connection stopped.
// ---------------------------------------------------------------------------
//  
void CVpnConnection::IkePluginSessionClosed( TInt aStatus )
    {
    __ASSERT_DEBUG( iIkePluginSessionHandler != NULL,
                    User::Invariant() );
    
    DEBUG_LOG1( _L("IKE plugin session closed, status=%d"),
            aStatus );

    iServer.DeleteIkePluginSession( iIkeVersion, iVpnIapId );
    iIkePluginSessionHandler = NULL;
    
    DoStopRealConnection( aStatus );
    }

// ---------------------------------------------------------------------------
// From class MIkePluginSessionHandlerCallback.
// Notification about IKE plugin session error.
// ---------------------------------------------------------------------------
//  
void CVpnConnection::IkePluginSessionError( TInt aStatus )
    {
    // Stop VPN connection silently.
    DisconnectIndication( aStatus );
    }

// ---------------------------------------------------------------------------
// Cancels negotiation.
// ---------------------------------------------------------------------------
//  
void CVpnConnection::DoCancelNegotiateWithHost()
    {
    if ( iIkePluginSessionHandler )
        {
        iIkePluginSessionHandler->CancelNegotiateWithHost();
        }
    }

// ---------------------------------------------------------------------------
// Stops VPN connection.
// ---------------------------------------------------------------------------
//  
void CVpnConnection::DoStopVpnConnection( TBool aSilentClose )
    {    
    if ( iIkePluginSessionHandler == NULL )
        {
        // Stop real connection immediately.
        DoStopRealConnection( KErrNone );    
        }
    else
        {
        DoCancelNegotiateWithHost();
        
        // Delete IKE plugin session first.
        DoDeleteSession( aSilentClose );
        }                
    }

// ---------------------------------------------------------------------------
// Cancels VPN connection stopping. VPN Connection is stopped silently.
// ---------------------------------------------------------------------------
//  
void CVpnConnection::DoCancelStopVpnConnection()
    {
    DoCancelDeleteSession();
    
    if ( iIkePluginSessionHandler != NULL )
        {
        iServer.DeleteIkePluginSession( iIkeVersion, iVpnIapId );
        iIkePluginSessionHandler = NULL;
        }
    
    DoStopRealConnection( KErrCancel );       
    }

// ---------------------------------------------------------------------------
// Starts deletion of IKE plugin session.
// ---------------------------------------------------------------------------
//  
void CVpnConnection::DoDeleteSession( TBool aSilentClose )
    {
    __ASSERT_DEBUG( iIkePluginSessionHandler != NULL,
                    User::Invariant() );
    
    iIkePluginSessionHandler->DeleteSession( aSilentClose );
    }

// ---------------------------------------------------------------------------
// Cancels deletion of IKE plugin session. IKE plugin session is deleted
// silently.
// ---------------------------------------------------------------------------
//  
void CVpnConnection::DoCancelDeleteSession()
    {
    if ( iIkePluginSessionHandler != NULL )
        {
        iIkePluginSessionHandler->CancelDeleteSession();
        }
    }

// ---------------------------------------------------------------------------
// Stops real network connection.
// ---------------------------------------------------------------------------
//  
void CVpnConnection::DoStopRealConnection( TInt aStatus )
    {
    iDisconnectionObserver->Cancel();        
    iIkeConnection->StopConnection();
    
    // Use disconnection status for reporting if set.
    if ( iDisconnectEventReceived )
        {
        aStatus = iDisconnectionStatus;
        }

    if ( iClientStatusStopVpnConnection )
        {
        // Complete client's request.
        User::RequestComplete( iClientStatusStopVpnConnection, aStatus );
        iClientStatusStopVpnConnection = NULL;
        }                    
    DoReportDisconnectEvent( aStatus );
        
    // Delete VPN connection object.
    iServer.DeleteVpnConnection( iVpnIapId );
    }

// ---------------------------------------------------------------------------
// Reports disconnect event via Event Mediator API.
// ---------------------------------------------------------------------------
//  
void CVpnConnection::DoReportDisconnectEvent( TInt aStatus )
    {
    TPckg<TUint32> connInfoDes( iVpnIapId );
    TEventData info;
    info.iTaskStatus = aStatus;
    TPckg<TEventData> infoDes( info );
    
    DEBUG_LOG2( _L("Report disconnect event via event mediator, VPN IAP Id=%d, status=%d"),
            iVpnIapId, aStatus );

    iEventMediator.ReportEvent( EKmdRealIapConnDownEvent,
                                connInfoDes,
                                infoDes );    
    }
