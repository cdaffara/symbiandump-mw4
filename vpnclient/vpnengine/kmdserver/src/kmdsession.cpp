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
* Description:  Server side session of KMDServer
*
*/


#include "ikedebug.h"
#include "ikepolparser.h"
#include "kmdserver.h"
#include "kmdapi.h"
#include "kmdserver.pan"

// CLASS HEADER
#include "kmdsession.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CKmdSession* CKmdSession::NewL( CKmdServer& aServer,
                                MIkeDebug& aDebug )
    {
    CKmdSession* self = new ( ELeave ) CKmdSession( aServer, aDebug );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CKmdSession::~CKmdSession()
    {
    DEBUG_LOG( _L("CKmdSession::~CKmdSession") );
    
    DoCancelStartConnection();
    DoCancelActivate();
    DoCancelResolveAddress();    
    
    iServer.KmdSessionClosed();
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CKmdSession::CKmdSession( CKmdServer& aServer,
                          MIkeDebug& aDebug )
 : iServer( aServer ),
   iDebug( aDebug )
    {
    DEBUG_LOG( _L("CKmdSession::CKmdSession") );
    }

// ---------------------------------------------------------------------------
// From class CSession2.
// Handles the servicing of a client request from KMD API.
// ---------------------------------------------------------------------------
//
void CKmdSession::ServiceL( const RMessage2& aMessage )
    {        
    switch ( aMessage.Function() ) 
        {
        case CKmdServer::KKmdStartConnection: 
            {
            DEBUG_LOG(_L("CKmdSession::ServiceL, KKmdStartConnection"));

            if ( iConnectionStarter != NULL )
                {
                aMessage.Panic( KKmdPanicCategory,
                                EKmdPanicRequestAlreadyPending );
                }
            else
                {                    
                __ASSERT_DEBUG( iPendingStartConnection.IsNull(),
                                User::Invariant() );             
                iVpnIapId = aMessage.Int0();
                
                // Create new VPN connection object and start connection.
                CVpnConnection& vpnConnection = iServer.CreateVpnConnectionL( iVpnIapId );       
                TRAPD( err, iConnectionStarter = CConnectionStarter::NewL( vpnConnection, *this ) );
                if ( err != KErrNone )
                    {
                    iServer.DeleteVpnConnection( iVpnIapId );
                    User::Leave( err );
                    }
                iPendingStartConnection = aMessage;                                    
                iConnectionStarter->StartRealConnection();      
                }
            break;
            }
        case CKmdServer::KKmdCancelStartConnection:
            {
            DEBUG_LOG(_L("CKmdSession::ServiceL, KKmdCancelStartConnection"));
            
            DoCancelStartConnection();            
            aMessage.Complete( KErrNone );
            break;
            }
        case CKmdServer::KKmdActivateAsync:
            {
            DEBUG_LOG(_L("CKmdSession::ServiceL, KKmdActivateAsync"));
            
            if ( iActivationStarter != NULL )
                {
                aMessage.Panic( KKmdPanicCategory,
                                EKmdPanicRequestAlreadyPending );
                }
            else
                {                    
                __ASSERT_DEBUG( iPendingActivate.IsNull(),
                                User::Invariant() );             
                iVpnIapId = aMessage.Int0();
                CVpnConnection* vpnConnection = NULL;
                TRAPD( err, vpnConnection = &iServer.GetVpnConnectionL( iVpnIapId ) );

                if ( err !=  KErrNone )
                    {
                    __ASSERT_DEBUG( err == KErrNotFound, User::Invariant() );
                    aMessage.Complete( KErrArgument );
                    }
                else
                    {
                    // Read VPN interface name.
                    HBufC* vpnIfName = HBufC::NewLC( aMessage.GetDesLength( 1 ) );
                    TPtr vpnIfNameDes = vpnIfName->Des();
                    aMessage.ReadL( 1, vpnIfNameDes );
                    
                    CIkeDataArray* ikeList = CIkeDataArray::NewL( 1 );
                    CleanupStack::PushL( ikeList );

                    // Read 8 bit IKE policy data.
                    HBufC8* ikePolicy8 = HBufC8::NewLC( aMessage.GetDesLength( 2 ) );                        
                    TPtr8 policyDes8 = ikePolicy8->Des();
                    aMessage.ReadL( 2, policyDes8 );                

                    // Copy read IKE policy data to 16 bit buffer.
                    HBufC* ikeConf = HBufC::NewL( policyDes8.Length() );
                    TPtr ikeConfPtr = ikeConf->Des();
                    ikeConfPtr.Copy( policyDes8 );
                    CleanupStack::PopAndDestroy( ikePolicy8 );
                    CleanupStack::PushL( ikeConf );
                    
                    // Parse IKE policy data.
                    TIkeParser ikeParser( *ikeConf );
                    ikeParser.MainParseL( ikeList );                      
                    CleanupStack::PopAndDestroy( ikeConf );
                    
                    // Get first IKE policy section.
                    CIkeData* ikeData = NULL;
                    if (ikeList->Count() > 0)
                        {
                        ikeData = (*ikeList)[0];
                        }
                    else
                        {
                        User::Leave( KKmdIkePolicyFileErr );
                        }
                    
                    // Start negotiation.
                    iActivationStarter = CActivationStarter::NewL( *vpnConnection,
                                                                   *this,
                                                                   iServer.Debug() );                       
                    iPendingActivate = aMessage;                    
                    iActivationStarter->Activate( *ikeData,
                                                  vpnIfNameDes );
                    
                    CleanupStack::PopAndDestroy( ikeList );
                    CleanupStack::PopAndDestroy( vpnIfName );
                    }
                }
            break;
            }       
        case CKmdServer::KKmdCancelActivateAsync:
            {
            DEBUG_LOG(_L("CKmdSession::ServiceL, KKmdCancelActivateAsync"));
            
            DoCancelActivate();
            aMessage.Complete( KErrNone );
            break;
            }
        case CKmdServer::KKmdStopConnection:
            {                    
            DEBUG_LOG(_L("CKmdSession::ServiceL, KKmdStopConnection"));
            
            if ( iConnectionStopper != NULL )
                {
                aMessage.Panic( KKmdPanicCategory,
                                EKmdPanicRequestAlreadyPending );
                }
            else
                {                    
                __ASSERT_DEBUG( iPendingStopConnection.IsNull(),
                                User::Invariant() );
                
                TUint32 vpnIapId = aMessage.Int0();
                TKmdStopConnection::TType type = (TKmdStopConnection::TType)aMessage.Int1();
                
                CVpnConnection& connection = iServer.GetVpnConnectionL( vpnIapId );
                iConnectionStopper = CConnectionStopper::NewL( connection, *this );
                
                iPendingStopConnection = aMessage;
                iConnectionStopper->StopVpnConnection( type == TKmdStopConnection::EForced );
                }
            break;
            }            
        case CKmdServer::KKmdResolveAddress:
            {
            DEBUG_LOG(_L("CKmdSession::ServiceL, KKmdResolveAddress"));
            
            if ( iFqdnResolver != NULL )
                {
                aMessage.Panic( KKmdPanicCategory,
                                EKmdPanicRequestAlreadyPending );
                }
            else
                {
                __ASSERT_DEBUG( iPendingFqdnResolve.IsNull(),
                                User::Invariant() );
                iVpnIapId = aMessage.Int0();                
                CVpnConnection* vpnConnection = NULL;
                TRAPD( err, vpnConnection = &iServer.GetVpnConnectionL( iVpnIapId ) );
                if ( err !=  KErrNone )
                    {
                    __ASSERT_DEBUG( err == KErrNotFound, User::Invariant() );
                    aMessage.Complete( KErrArgument );
                    }
                else
                    {
                    HBufC* fqdn = HBufC::NewLC( aMessage.GetDesLengthL( 1 ) );
                    TPtr fqdnDes = fqdn->Des();
                    aMessage.ReadL( 1, fqdnDes );
                    
                    iFqdnResolver = CFqdnResolver::NewL( *vpnConnection, *this );   
                    iPendingFqdnResolve = aMessage;
                    iFqdnResolver->ResolveAddress( fqdn );
                    CleanupStack::Pop( fqdn );
                    }
                }
            break;
            }
        case CKmdServer::KKmdCancelResolveAddress:
            {
            DEBUG_LOG(_L("CKmdSession::ServiceL, KKmdCancelResolveAddress"));
            
            DoCancelResolveAddress();
            aMessage.Complete( KErrNone );
            break;
            }
        default:
            {
            DEBUG_LOG1(_L("CKmdSession::ServiceL, illegal command=%d"),
                    aMessage.Function());
            
            aMessage.Panic( KKmdPanicCategory, EKmdPanicIllegalCommand );
            break;
            }
        }

    }

// ---------------------------------------------------------------------------
// From class MConnectionStarterCallback.
// Notification about completion of real connection starting.
// ---------------------------------------------------------------------------
//
void CKmdSession::RealConnectionStarted( TInt aStatus,
                                         TInt aRealIap,
                                         TInt aRealNetwork )
    {
    DEBUG_LOG3(_L("Real connection started, status=%d, IAP id=%d, NET id=%d"),
            aStatus, aRealIap, aRealNetwork );

    __ASSERT_DEBUG( !iPendingStartConnection.IsNull(), User::Invariant() );
    
    delete iConnectionStarter;
    iConnectionStarter = NULL;

    if ( aStatus == KErrNone )
        {        
        TVpnRealConnectionParams realConfig = { aRealIap,  aRealNetwork };
        TPckg<TVpnRealConnectionParams> realConfigPckg( realConfig );
        
        aStatus = iPendingStartConnection.Write( 1, realConfigPckg );
        }
    iPendingStartConnection.Complete( aStatus );    
    }

// ---------------------------------------------------------------------------
// From class MConnectionStopperCallback.
// Notification about completion of VPN connection stopping.
// ---------------------------------------------------------------------------
//
void CKmdSession::VpnConnectionStopped( TInt aStatus )
    {
    DEBUG_LOG1(_L("VPN connection stopped, status=%d"), aStatus );
    
    __ASSERT_DEBUG( !iPendingStopConnection.IsNull(), User::Invariant() );
    
    delete iConnectionStopper;
    iConnectionStopper = NULL;

    iPendingStopConnection.Complete( aStatus );    
    }

// ---------------------------------------------------------------------------
// From class MFqdnResolverCallback.
// Notifies about completion of FQDN address resolving.
// ---------------------------------------------------------------------------
//
void CKmdSession::AddressResolveCompleted( TInt aStatus,
                                           TNameEntry aNameEntry )
    {
    DEBUG_LOG1(_L("FQDN address resolving completed, status=%d"), aStatus );
    
    __ASSERT_DEBUG( !iPendingFqdnResolve.IsNull(),
                    User::Invariant() );
    
    delete iFqdnResolver;
    iFqdnResolver = NULL;
    
    if ( aStatus == KErrNone )
        {
        aStatus = iPendingFqdnResolve.Write( 2, aNameEntry );
        }
    
    iPendingFqdnResolve.Complete( aStatus );
    }

// ---------------------------------------------------------------------------
// From class MActivationStarterCallback.
// Notification about completion of activation.
// ---------------------------------------------------------------------------
//
void CKmdSession::ActivationCompleted( TInt aStatus,
                                       const TVPNAddress& aVirtualIp )
    {
    DEBUG_LOG1(_L("Activation completed, status=%d"), aStatus );
    
    __ASSERT_DEBUG( !iPendingActivate.IsNull(),
                    User::Invariant() );
        
    if ( aStatus == KErrNone )
        {
        TVPNAddressPckg addrPkcg( aVirtualIp );
        aStatus = iPendingActivate.Write( 3, addrPkcg );
        }
    iPendingActivate.Complete( aStatus );
    
    delete iActivationStarter;
    iActivationStarter = NULL;    
    }

// ---------------------------------------------------------------------------
// Cancels real connection starting.
// ---------------------------------------------------------------------------
//
void CKmdSession::DoCancelStartConnection()
    {
    if ( iConnectionStarter )
        {
        __ASSERT_DEBUG( !iPendingStartConnection.IsNull(),
                        User::Invariant() );

        delete iConnectionStarter; // Cancels ongoing connection starting.
        iConnectionStarter = NULL;
        
        iPendingStartConnection.Complete( KErrCancel );
        
        // Delete VPN connection object.
        iServer.DeleteVpnConnection( iVpnIapId );                
        }    
    }

// ---------------------------------------------------------------------------
// Cancels activating.
// ---------------------------------------------------------------------------
//
void CKmdSession::DoCancelActivate()
    {
    if ( iActivationStarter )
        {
        __ASSERT_DEBUG( !iPendingActivate.IsNull(),
                        User::Invariant() );

        delete iActivationStarter; // Cancels ongoing activation.
        iActivationStarter = NULL;
        
        iPendingActivate.Complete( KErrCancel );
        }    
    }

// ---------------------------------------------------------------------------
// Cancels FQDN address resolving.
// ---------------------------------------------------------------------------
//
void CKmdSession::DoCancelResolveAddress()
    {
    if ( iFqdnResolver )
        {
        __ASSERT_DEBUG( !iPendingFqdnResolve.IsNull(),
                        User::Invariant() );                

        delete iFqdnResolver; // Cancels ongoing resolving.
        iFqdnResolver = NULL;
        
        iPendingFqdnResolve.Complete( KErrCancel );
        }    
    }

