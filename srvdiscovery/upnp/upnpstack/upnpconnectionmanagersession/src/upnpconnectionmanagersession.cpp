/** @file
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  upnpconnectionmanager / client side
*
*/

#include <e32math.h>
#include "upnpconnmanagercommon.h"
#include "upnpconnectionmanagersession.h"
#include "upnpcustomlog.h"

const TInt KServerMessageSlots = 1; //Number of message slots available per session.
const TInt KServerRetryCount = 2;

// -----------------------------------------------------------------------------
// RUpnpConnectionManagerSession::RUpnpConnectionManagerSession
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
RUpnpConnectionManagerSession::RUpnpConnectionManagerSession() :
    RUpnpSessionBase(), iParameter0( NULL, 0, 0 ), iNetworkEventParameter( NULL, 0, 0 )
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// RUpnpConnectionManagerSession::Connect
// Establish connection to server.
// -----------------------------------------------------------------------------
//
TInt RUpnpConnectionManagerSession::Connect()
    {
    return RUpnpSessionBase::Connect( KUpnpConnectionManagerName,
             TVersion( KUpnpConnectionManagerMajorVersionNumber,
                       KUpnpConnectionManagerMinorVersionNumber,
                       KUpnpConnectionManagerBuildVersionNumber ),
             KServerMessageSlots,
             KServerRetryCount,
             KUpnpConnectionManagerFilename,
             KServerUid3 );
    }

// -----------------------------------------------------------------------------
// RUpnpConnectionManagerSession::RequestEnsureStartRConnection
// Client requests server to ensure RConnection is started.
// -----------------------------------------------------------------------------
//
TInt RUpnpConnectionManagerSession::RequestEnsureStartRConnection(
    TInt& aAccessPoint )
    {
    LOG_FUNC_NAME;
    iParameter0.Set( reinterpret_cast<TUint8*>( &aAccessPoint ),
                     sizeof( TInt ),
                     sizeof( TInt ) );

    return SendReceive( EEnsureStartRConnection, TIpcArgs( &iParameter0 ) );
    }

// -----------------------------------------------------------------------------
// RUpnpConnectionManagerSession::RequestIsRConnectionStarted
// Client requests server whether RConnection has been started.
// -----------------------------------------------------------------------------
//
void RUpnpConnectionManagerSession::RequestIsRConnectionStarted(
    TBool& aResult )
    {
    LOG_FUNC_NAME;
    iParameter0.Set( reinterpret_cast<TUint8*>( &aResult ), 
                     sizeof( TBool ),
                     sizeof( TBool ) );

    SendReceive( EIsRConnectionStarted, TIpcArgs( &iParameter0 ) );
    }

// -----------------------------------------------------------------------------
// RUpnpConnectionManagerSession::RequestActiveIap
// Client requests server for active Iap
// -----------------------------------------------------------------------------
//
TInt RUpnpConnectionManagerSession::RequestActiveIap()
    {
    LOG_FUNC_NAME;
    
    TInt ret = KErrNotFound;
    
    iParameter0.Set( reinterpret_cast<TUint8*>( &ret ), 
                     sizeof( TInt ),
                     sizeof( TInt ) );

    SendReceive( ECmActiveIap, TIpcArgs( &iParameter0 ) );
    
    return ret;
    }
// -----------------------------------------------------------------------------
// RUpnpConnectionManagerSession::RequestLocalAddress
// Client requests server for active Iap
// -----------------------------------------------------------------------------
//
TInetAddr RUpnpConnectionManagerSession::RequestLocalAddress()
    {
    LOG_FUNC_NAME;
    
    TInetAddr address = INET_ADDR( 0,0,0,0 );
    
    iParameter0.Set( reinterpret_cast<TUint8*>( &address ), 
                     sizeof( TInetAddr ),
                     sizeof( TInetAddr ) );

    SendReceive( ECmGetAddress, TIpcArgs( &iParameter0 ) );
    
    return address;
    }

// -----------------------------------------------------------------------------
// RUpnpConnectionManagerSession::RequestSubscribeToNetworkEvents
// Client requests subscribe to network events.
// -----------------------------------------------------------------------------
//    
void RUpnpConnectionManagerSession::RequestSubscribeToNetworkEvents( TRequestStatus& aStatus,
                                            TUpnpConnectionManagerNetworkEvent &aNetworkEvent )
    {
    LOG_FUNC_NAME;

    iNetworkEventParameter.Set( reinterpret_cast<TUint8*>( &aNetworkEvent ),
                                sizeof( TUpnpConnectionManagerNetworkEvent ),
                                sizeof( TUpnpConnectionManagerNetworkEvent ) );
    SendReceive( EEventSubscribe, TIpcArgs( &iNetworkEventParameter ), aStatus );
    }

// -----------------------------------------------------------------------------
// RUpnpConnectionManagerSession::RequestCancelSubscribeToNetworkEvents
// Client requests cancel subscribe to network events.
// -----------------------------------------------------------------------------
//  
void RUpnpConnectionManagerSession::RequestCancelSubscribeToNetworkEvents()
    {
    LOG_FUNC_NAME;
    SendReceive( ECancelEventSubscribe ); //ignoring returned value
    }
    
// End of file
