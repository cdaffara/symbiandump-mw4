/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   IKEv1 protocol plugin 
*
*/

#include "ikedebug.h"
#include "ikeplugindefs.h"
#include "ikev1pluginsession.h"
#include "ikev1SAdata.h"
#include "ipsecpolicyutil.h"
#include "pfkeysocketif.h"

// CLASS HEADER
#include "ikev1plugin.h"

// ======== GLOBAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// Creates IKEv1 plugin instance.
// ---------------------------------------------------------------------------
//
EXPORT_C MIkePluginIf* Ikev1PlugInL( MKmdEventLoggerIf& aEventLogger,
                                     MIkeDebug& aDebug )
    {
    CIkev1Plugin* plugin = CIkev1Plugin::NewL( aEventLogger, aDebug );
    return plugin;
    }

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CIkev1Plugin* CIkev1Plugin::NewL( MKmdEventLoggerIf& aEventLogger,
                                  MIkeDebug& aDebug )
    {
    CIkev1Plugin* self = new ( ELeave ) CIkev1Plugin( aEventLogger, aDebug );
    CleanupStack::PushL( self );                      
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;    
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CIkev1Plugin::~CIkev1Plugin()
    {    
    __ASSERT_DEBUG( iPluginSessions.Count() == 0,
                    User::Invariant() );
    iPluginSessions.Close();    
    delete iPFKeySocket;
    delete iIpsecPolicyUtil;
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CIkev1Plugin::CIkev1Plugin( MKmdEventLoggerIf& aEventLogger,
                            MIkeDebug& aDebug )
 : iEventLogger( aEventLogger ),
   iDebug( aDebug )
    {    
    }

// ---------------------------------------------------------------------------
// Second phase construction.
// ---------------------------------------------------------------------------
//
void CIkev1Plugin::ConstructL()
    {
    iPFKeySocket = CPFKeySocketIf::NewL( this,
                                         iDebug );
    
    iIpsecPolicyUtil = CIpsecPolicyUtil::NewL();
    }
    
// ---------------------------------------------------------------------------
// Removes IKE plugin session from array.
// ---------------------------------------------------------------------------
//
void CIkev1Plugin::IkePluginSessionDeleted( CIkev1PluginSession* aPluginSession )
    {
    for ( TInt i=0; i<iPluginSessions.Count(); i++ )
        {
        if ( iPluginSessions[i] == aPluginSession )
            {
            iPluginSessions.Remove( i );
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// Gets acceptable IPsec policies for specified selectors.
// ---------------------------------------------------------------------------
//
CIpsecSaSpecList* CIkev1Plugin::GetIpseSaSpecListLC( const TInetAddr& aLocalAddr, const TInetAddr& aLocalMask, 
                                                     const TInetAddr& aRemoteAddr, const TInetAddr& aRemoteMask,
                                                     TInt aProtocol, TUint32 aVpnNetId )
    {
    return iIpsecPolicyUtil->GetIpseSaSpecListLC( aLocalAddr, aLocalMask, 
                                                  aRemoteAddr, aRemoteMask,
                                                  aProtocol, aVpnNetId );
    }

// Methods to build and send PFKEY API primitives to IPsec

void CIkev1Plugin::AcquireSAError( TIpsecSAData& aSAData,
                                   TInt aError )
    {
    iPFKeySocket->AcquireSAError( aSAData, aError );
    }

void CIkev1Plugin::UpdateSAL( TIpsecSAData& aSaData )
    {
    iPFKeySocket->UpdateSAL( aSaData );
    }

void CIkev1Plugin::AddSAL( TIpsecSAData& aSaData )
    {
    iPFKeySocket->AddSAL( aSaData );
    }

void CIkev1Plugin::DeleteIpsecSA( TIpsecSPI& aIpsecSpi )
    {
    iPFKeySocket->DeleteSA( aIpsecSpi.iSPI,
                            aIpsecSpi.iSrcAddr,
                            aIpsecSpi.iDstAddr, 
                            aIpsecSpi.iProtocol );  
    }

void CIkev1Plugin::DeleteIpsecSA( TUint32 aSPI,
                                  TInetAddr& aSrc, 
                                  TInetAddr& aDst,
                                  TUint8 aProtocol )
    {
    iPFKeySocket->DeleteSA( aSPI, aSrc, aDst, aProtocol );
    }

// ---------------------------------------------------------------------------
// Creates IKEv1 plugin session.
// ---------------------------------------------------------------------------
//
MIkePluginSessionIf* CIkev1Plugin::CreateSessionL( TUint32 aVpnIapId,
                                                   TUint32 aVpnNetId,
                                                   TUint32 aVpnInterfaceIndex,
                                                   MIkeDataInterface& aDataInterface )
    {
    CIkev1PluginSession* pluginSession = CIkev1PluginSession::NewL( aVpnIapId,
                                                                    aVpnNetId,
                                                                    aVpnInterfaceIndex,
                                                                    aDataInterface,
                                                                    *this,
                                                                    *iPFKeySocket,
                                                                    iDebug ); 
    TInt err = iPluginSessions.Append( pluginSession );
    
    if ( err != KErrNone )
        {
        delete pluginSession;
        pluginSession = NULL;
        User::Leave( err );
        }
    
    return pluginSession;
    }

// ---------------------------------------------------------------------------
// Handles PFKEY message.
// ---------------------------------------------------------------------------
//
void CIkev1Plugin::PfkeyMessageReceived( const TPfkeyMessage& aPfkeyMessage  )
    {
    switch ( aPfkeyMessage.iBase.iMsg->sadb_msg_type )
        {
        case SADB_ACQUIRE:
            {
            for ( TInt i=0; i< iPluginSessions.Count(); i++ )
                {
                if ( iPluginSessions[i]->MatchDestinationAddress( aPfkeyMessage.iDstAddr.Address() ) )                
                    {
                    iPluginSessions[i]->PfkeyMessageReceived( aPfkeyMessage );
                    break;
                    }
                }
            break;
            }
            
        case SADB_EXPIRE:
            {
            for ( TInt i=0; i< iPluginSessions.Count(); i++ )
                {
                if ( iPluginSessions[i]->MatchDestinationAddress( aPfkeyMessage.iSrcAddr.Address() ) )                
                    {
                    iPluginSessions[i]->PfkeyMessageReceived( aPfkeyMessage );
                    break;
                    }
                }
            break;                        
            }
        default:
            break;
        }
    }


// ---------------------------------------------------------------------------
// Returns UID.
// ---------------------------------------------------------------------------
//
TUint32 CIkev1Plugin::Uid()
    {
    return KIkeV1PluginUid3.iUid;
    }

// ---------------------------------------------------------------------------
// Returns event logger interface.
// ---------------------------------------------------------------------------
//
MKmdEventLoggerIf& CIkev1Plugin::EventLogger()
    {
    return iEventLogger;
    }

