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


// INTERNAL INCLUDES
#include "ikedebug.h"
#include "ikeplugindefs.h"
#include "ikepluginhandler.h"
#include "kmddebuglogger.h"
#include "kmdeventlogger.h"
#include "kmdsession.h"
#include "secpolreader.h"
#include "vpnconnection.h"

// CLASS HEADER
#include "kmdserver.h"

const TUint CKmdServer::iKmdServerRangeCount = 2;
  
const TInt CKmdServer::iKmdServerRanges[iKmdServerRangeCount] = 
    {
    CKmdServer::KKmdStartConnection,
    CKmdServer::KKmdCancelResolveAddress+1
    };

const TUint8 CKmdServer::iKmdServerElementIndex[iKmdServerRangeCount] = 
    {
    0,
    CPolicyServer::ENotSupported
    };

const CPolicyServer::TPolicyElement CKmdServer::iKmdServerElements[] =
    {
    {_INIT_SECURITY_POLICY_C1(ECapabilityNetworkControl), 
      CPolicyServer::EFailClient},
    };

const CPolicyServer::TPolicy CKmdServer::iKmdServerPolicy =
    {
    0,                      // All connect attempts are checked
    iKmdServerRangeCount,   // Count of ranges
    iKmdServerRanges,       // 0-6, 7...
    iKmdServerElementIndex, // Only range 0-6 are checked
    iKmdServerElements      // The list of policy elements
    };

const TInt KIkeV1PluginHandlerIndex( 0 );
const TInt KIkeV2PluginHandlerIndex( 1 );

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CKmdServer* CKmdServer::NewL()
    {
    CKmdServer* self = new ( ELeave ) CKmdServer;
    CleanupStack::PushL( self );
    self->ConstructL();    
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CKmdServer::~CKmdServer()
    {    
    iIpsecPolicyServ.Close();
    iVpnConnections.ResetAndDestroy();
    iVpnConnections.Close();    
    delete iIkePluginHandlers[0];
    delete iIkePluginHandlers[1];
    delete iSecpolReader;
    delete iEventLogger;
    
#ifdef _DEBUG
    if ( iDebugLogger != NULL )
        {
        iDebugLogger->LogWrite(_L("KMD server stopped."));
        }
#endif        
    delete iDebugLogger;
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CKmdServer::CKmdServer() : 
 CPolicyServer( EPriorityStandard, iKmdServerPolicy )
    {
    }

// ---------------------------------------------------------------------------
// Second phase construction.
// ---------------------------------------------------------------------------
//
void CKmdServer::ConstructL()
    {
    iDebugLogger = CKmdDebugLogger::NewL();
    
#ifdef _DEBUG
    iDebugLogger->LogWrite(_L("------------------------------------------------------"));
    iDebugLogger->LogWrite(_L("Starting KMD server."));
    iDebugLogger->LogWrite(_L("------------------------------------------------------"));
#endif    

    User::LeaveIfError( iIpsecPolicyServ.Connect() );        
    iEventLogger = CKmdEventLogger::NewL( *iDebugLogger );
    iSecpolReader = CSecpolReader::NewL( *iEventLogger,
                                         *iDebugLogger );
    iIkePluginHandlers[KIkeV1PluginHandlerIndex] = CIkePluginHandler::NewL( KIkeV1,
                                                                            *iEventLogger,
                                                                            *iDebugLogger );
    iIkePluginHandlers[KIkeV2PluginHandlerIndex] = CIkePluginHandler::NewL( KIkeV2,
                                                                            *iEventLogger,
                                                                            *iDebugLogger );
    
    StartL( KKmdServerName );
    
#ifdef _DEBUG
    iDebugLogger->LogWrite(_L("KMD server started."));
#endif        
    }

// ---------------------------------------------------------------------------
// Information that KMD session has been closed.
// ---------------------------------------------------------------------------
//
void CKmdServer::KmdSessionClosed()
    {
    iSessionCount--;    
    __ASSERT_DEBUG(iSessionCount >= 0, User::Invariant());
    
    StopKmdServer();
    }

// ---------------------------------------------------------------------------
// Creates VPN connection object.
// ---------------------------------------------------------------------------
//
CVpnConnection& CKmdServer::CreateVpnConnectionL(TUint32 aVpnIapId)
    {
#ifndef _DEBUG
    for ( TInt i = 0; i < iVpnConnections.Count(); ++i )
        {
        __ASSERT_DEBUG( iVpnConnections[i]->VpnIapId() != aVpnIapId, User::Invariant() );
        }
#endif //_DEBUG    
    
    CVpnConnection* newConnection = CVpnConnection::NewLC( aVpnIapId,
                                                           *this,
                                                           *iDebugLogger );
    User::LeaveIfError( iVpnConnections.Append( newConnection ) );
    CleanupStack::Pop( newConnection );
    
    return *newConnection;        
    }

// ---------------------------------------------------------------------------
// Gets VPN connection object.
// ---------------------------------------------------------------------------
//
CVpnConnection& CKmdServer::GetVpnConnectionL( TUint32 aVpnIapId )
    {
    CVpnConnection* connection = NULL;
    for ( TInt i = 0; i < iVpnConnections.Count(); ++i )
        {
        CVpnConnection* c = iVpnConnections[i];
        if ( c->VpnIapId() == aVpnIapId )
            {
            connection = c;
            break;
            }
        }
    
    if ( connection == NULL )
        {
        User::Leave( KErrNotFound );
        }
    return *connection;            
    }

// ---------------------------------------------------------------------------
// Deletes VPN connection object.
// ---------------------------------------------------------------------------
//
void CKmdServer::DeleteVpnConnection( TUint32 aVpnIapId )
    {    
    TInt connectionCount = iVpnConnections.Count();
    for ( TInt i = 0; i < connectionCount; ++i )
        {
        CVpnConnection* c = iVpnConnections[i];
        if ( c->VpnIapId() == aVpnIapId )
            {
            iVpnConnections.Remove( i );
            delete c;
            break;
            }
        }
    
    StopKmdServer();
    }

// ---------------------------------------------------------------------------
// Creates IKE plugin session.
// ---------------------------------------------------------------------------
//
CIkePluginSessionHandler& CKmdServer::CreateIkePluginSessionL( TInt aIkeVersion,
                                                               IkeSocket::TIpVersion aIpVersion,
                                                               CIkeConnectionInterface& aConnection,
                                                               TUint32 aVpnIapId,
                                                               TUint32 aVpnNetId,
                                                               TUint32 aVpnInterfaceIndex,
                                                               const TInetAddr& aDnsServerAddr,
                                                               MIkePluginSessionHandlerCallback& aCallback )
    {
    TInt index( 0 );
    switch ( aIkeVersion )
        {
        case KIkeV1:
            {
            index = KIkeV1PluginHandlerIndex;
            break;
            }
        case KIkeV2:
            {
            index = KIkeV2PluginHandlerIndex;
            break;
            }
        default:
            {
            ASSERT( EFalse );
            }        
        }
    return iIkePluginHandlers[index]->CreateIkePluginSessionL( aVpnIapId,
                                                               aVpnNetId,
                                                               aVpnInterfaceIndex,
                                                               aIpVersion,
                                                               aDnsServerAddr,
                                                               aConnection,
                                                               aCallback );
    }

// ---------------------------------------------------------------------------
// Deletes IKE plugin session.
// ---------------------------------------------------------------------------
//
void CKmdServer::DeleteIkePluginSession( TInt aIkeVersion,
                                         TUint32 aVpnIapId )
    {
    TInt index( 0 );
    switch ( aIkeVersion )
        {
        case KIkeV1:
            {
            index = KIkeV1PluginHandlerIndex;
            break;
            }
        case KIkeV2:
            {
            index = KIkeV2PluginHandlerIndex;
            break;
            }
        default:
            {
            ASSERT( EFalse );
            break;
            }        
        }
    iIkePluginHandlers[index]->DeleteIkePluginSession( aVpnIapId );
    }

// ---------------------------------------------------------------------------
// Returns debug trace interface.
// ---------------------------------------------------------------------------
//
MIkeDebug& CKmdServer::Debug()
    {
    __ASSERT_DEBUG( iDebugLogger != NULL,
                    User::Invariant() );
    
    return *iDebugLogger;
    }

// ---------------------------------------------------------------------------
// Creates a server-side session object.
// ---------------------------------------------------------------------------
//
CSession2* CKmdServer::NewSessionL( const TVersion& /*aVersion*/,
                                    const RMessage2& /*aMessage*/) const
    {
    CKmdSession* session = CKmdSession::NewL( *const_cast<CKmdServer*>( this ),
                                              *iDebugLogger );
    iSessionCount++;
    return session;
    }

// ---------------------------------------------------------------------------
// Stops KMD server if there exist neither client sessions nor active
// connections.
// ---------------------------------------------------------------------------
//
void CKmdServer::StopKmdServer()
    {
    if ( iSessionCount == 0 &&
         iVpnConnections.Count() == 0 )   
        {
        // Stop scheduluder that was started by RunServerL().
        CActiveScheduler::Stop();        
        }
    }   
