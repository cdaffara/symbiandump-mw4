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


#include "ikedebug.h"
#include "ikeplugindefs.h"
#include "ikepluginif.h"
#include "ikepluginsessionhandler.h"
#include "kmdeventloggerif.h"

// CLASS HEADER
#include "ikepluginhandler.h"

_LIT( KIkePluginPaths, "" ); // No additional paths.
_LIT( KIkeV1Library, "ikev1lib" ); 
_LIT( KIkeV2Library, "ikev2lib" ); 

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CIkePluginHandler* CIkePluginHandler::NewL( TInt aIkeVersion,
                                            MKmdEventLoggerIf& aEventLogger,
                                            MIkeDebug& aDebug )
    {
    CIkePluginHandler* self = new ( ELeave ) CIkePluginHandler( aIkeVersion,
                                                                aEventLogger,
                                                                aDebug );
    return self;
    }
    
// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CIkePluginHandler::~CIkePluginHandler()
    {
    __ASSERT_DEBUG( iIkePluginSessions.Count() == 0,
                    User::Invariant() );
    iIkePluginSessions.Close();    
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CIkePluginHandler::CIkePluginHandler( TInt aIkeVersion,
                                      MKmdEventLoggerIf& aEventLogger,
                                      MIkeDebug& aDebug ) 
 : iIkeVersion( aIkeVersion ),
   iEventLogger( aEventLogger ),
   iDebug( aDebug )
    {
    __ASSERT_DEBUG( ( iIkeVersion == KIkeV1 ||
                      iIkeVersion == KIkeV2 ),
                      User::Invariant() );
    }    

// ---------------------------------------------------------------------------
// Creates IKE plugin session.
// ---------------------------------------------------------------------------
//
CIkePluginSessionHandler& CIkePluginHandler::CreateIkePluginSessionL( TUint32 aVpnIapId,
                                                                      TUint32 aVpnNetId,
                                                                      TUint32 aVpnInterfaceIndex,
                                                                      IkeSocket::TIpVersion aIpVersion,
                                                                      const TInetAddr& aDnsServer,
                                                                      CIkeConnectionInterface& aConnection,
                                                                      MIkePluginSessionHandlerCallback& aCallback )            
    {
    CIkePluginSessionHandler* sessionHandler = NULL;    
    TRAPD( err, sessionHandler = &DoCreateIkePluginSessionL( aVpnIapId,
                                                             aVpnNetId,
                                                             aVpnInterfaceIndex,
                                                             aIpVersion,
                                                             aDnsServer,
                                                             aConnection,
                                                             aCallback ) );
    
    if ( err != KErrNone )
        {        
        DeleteIkePluginSession( aVpnIapId );
        User::Leave( err );
        }
    
    return *sessionHandler;
    }

// ---------------------------------------------------------------------------
// Deletes IKE plugin session. IKE plugin is unloaded if there are no more
// sessions.
// ---------------------------------------------------------------------------
//
void CIkePluginHandler::DeleteIkePluginSession( TUint32 aVpnIapId )
    {
    TInt count = iIkePluginSessions.Count();
    
    for ( TInt i=0; i<count; i++ )
        {
        if ( iIkePluginSessions[i]->VpnIapId() == aVpnIapId )
            {
            CIkePluginSessionHandler* sessionHandler = iIkePluginSessions[i];
            iIkePluginSessions.Remove( i );
            delete sessionHandler;
            sessionHandler = NULL;            
            break;
            }
        }
    
    if ( iIkePluginSessions.Count() == 0 )
        {
        UnloadIkePlugin();
        }
    }
	
// ---------------------------------------------------------------------------
// Creates IKE plugin session. IKE plugin is loaded if not yet loaded.
// ---------------------------------------------------------------------------
//
CIkePluginSessionHandler& CIkePluginHandler::DoCreateIkePluginSessionL( TUint32 aVpnIapId,
                                                                        TUint32 aVpnNetId,
                                                                        TUint32 aVpnInterfaceIndex,
                                                                        IkeSocket::TIpVersion aIpVersion,
                                                                        const TInetAddr& aDnsServerAddr,
                                                                        CIkeConnectionInterface& aConnection,
                                                                        MIkePluginSessionHandlerCallback& aCallback )            
    {
    if ( iIkePluginSessions.Count() == 0 )
        {
        LoadIkePluginL();
        }
    
    CIkePluginSessionHandler* sessionHandler = CIkePluginSessionHandler::NewLC( aVpnIapId,
                                                                                aVpnNetId,
                                                                                aVpnInterfaceIndex,
                                                                                IkeSocket::TIkeMajorVersion(iIkeVersion),
                                                                                aIpVersion,
                                                                                aDnsServerAddr,
                                                                                aConnection,
                                                                                *iIkePlugin,
                                                                                aCallback,
                                                                                iDebug );   
    iIkePluginSessions.AppendL( sessionHandler );
    CleanupStack::Pop( sessionHandler );
    
    return *sessionHandler;
    }

// ---------------------------------------------------------------------------
// Loads IKE plugin.
// ---------------------------------------------------------------------------
//
void CIkePluginHandler::LoadIkePluginL()
    {	
    TInt status( KErrNone );
        
    switch( iIkeVersion )
        {
        case KIkeV1:
            {
            TUidType uidType( KIkePluginUid1, KIkePluginUid2, KIkeV1PluginUid3 );
            status = iLibrary.Load( KIkeV1Library, KIkePluginPaths, uidType );            
            break;
            }
        case KIkeV2:
            {
            TUidType uidType( KIkePluginUid1, KIkePluginUid2, KIkeV2PluginUid3 );
            status = iLibrary.Load( KIkeV2Library, KIkePluginPaths, uidType );            
            break;            
            }
        default:
            {
            status = KErrNotSupported;
            break;
            }        
        }
    
    DEBUG_LOG2( _L("Loading IKE plugin library, IKE version=%d, status=%d"), iIkeVersion, status );    
    User::LeaveIfError( status );
    
    CreateIkePluginL factoryMethodL  = reinterpret_cast<CreateIkePluginL>( iLibrary.Lookup(1) );    
    TRAPD( err, ( iIkePlugin = factoryMethodL( iEventLogger, iDebug ) ) );
    if ( err != KErrNone )
        {
        DEBUG_LOG1( _L("Could not create IKE plugin, err=%d"), err );
        UnloadIkePlugin();
        User::Leave( err );
        }    
    }

// ---------------------------------------------------------------------------
// Unloads IKE plugin.
// ---------------------------------------------------------------------------
//
void CIkePluginHandler::UnloadIkePlugin()
    {
    DEBUG_LOG1( _L("Unloading IKE Plugin library, IKE version=%d"), iIkeVersion );
    delete iIkePlugin;
    iIkePlugin = NULL;
    iLibrary.Close();    
    }

