/** @file
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CUpnpConnectionManagerProxy
*
*/


// INCLUDE FILES
#include "upnpconnectionmanagernetworkeventprovider.h"
#include "upnpconnectionmanagerproxy.h"
#include "upnpcustomlog.h"


// -----------------------------------------------------------------------------
// CUpnpConnectionManagerProxy::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpConnectionManagerProxy* CUpnpConnectionManagerProxy::NewL(
    RSocketServ &aSocketServer )
    {
    CUpnpConnectionManagerProxy* self =
        CUpnpConnectionManagerProxy::NewLC( aSocketServer );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpConnectionManagerProxy::NewLC
// Two-phased constructor, leave object on the cleanup stack.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpConnectionManagerProxy* CUpnpConnectionManagerProxy::NewLC(
    RSocketServ &aSocketServer )
    {
    CUpnpConnectionManagerProxy* self = new( ELeave ) CUpnpConnectionManagerProxy();
    CleanupStack::PushL( self );
    self->ConstructL( aSocketServer );
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpConnectionManagerProxy::CUpnpConnectionManagerProxy
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CUpnpConnectionManagerProxy::CUpnpConnectionManagerProxy() : iActiveIap( KErrNotFound )
    {
    }

// -----------------------------------------------------------------------------
// CUpnpConnectionManagerProxy::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpConnectionManagerProxy::ConstructL( RSocketServ& aSocketServer )
    {
    User::LeaveIfError( iConnection.Open( aSocketServer ) );
    User::LeaveIfError( iConnectionManagerSession.Connect() );
    }

// -----------------------------------------------------------------------------
// CUpnpConnectionManagerProxy::~CUpnpConnectionManagerProxy
// Destructor.
// -----------------------------------------------------------------------------
//
CUpnpConnectionManagerProxy::~CUpnpConnectionManagerProxy()
    {
    delete iNetworkEventProvider;
    iConnection.Close();
    iConnectionManagerSession.Close();
    }

// -----------------------------------------------------------------------------
// CUpnpConnectionManagerProxy::EnsureStart
// Calls remote process to ensure that interface is started.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpConnectionManagerProxy::EnsureStart()
    {
    TInt error = iConnectionManagerSession.RequestEnsureStartRConnection( iActiveIap );
    if ( error == KErrNone )
        {
        error = Attach( iActiveIap );
        }
    else if ( error < KErrNone )
        {
        iActiveIap = KErrNotFound;
        }

    return error;
    }

// -----------------------------------------------------------------------------
// CUpnpConnectionManagerProxy::IsStarted
// Checks whether EnsureStart has been invoked.
// -----------------------------------------------------------------------------
//
TBool CUpnpConnectionManagerProxy::IsStarted()
    {
    TBool result = EFalse;
    iConnectionManagerSession.RequestIsRConnectionStarted( result );
    return result;
    }

// -----------------------------------------------------------------------------
// CUpnpConnectionManagerProxy::ConnectionL
// -----------------------------------------------------------------------------
//
EXPORT_C RConnection& CUpnpConnectionManagerProxy::ConnectionL()
    {
    if ( !IsStarted() )
        {
        User::LeaveIfError( EnsureStart() );
        }
    return iConnection;
    }

// -----------------------------------------------------------------------------
// CUpnpConnectionManagerProxy::Attach
// Internal function.
// -----------------------------------------------------------------------------
//
TInt CUpnpConnectionManagerProxy::Attach( TInt aAccessPoint )
    {
    TUint connNumber;
    TInt attachError = KErrNotFound;
    TInt error = iConnection.EnumerateConnections( connNumber );

    if ( error == KErrNone && connNumber > 0 )
        {
        TPckgBuf<TConnectionInfo> connInfo;

        for ( TInt i = 1; i <= connNumber; i++ )
            {
            error = iConnection.GetConnectionInfo( i, connInfo );

            if ( error == KErrNone && connInfo().iIapId == aAccessPoint )
                {
                attachError = iConnection.Attach( connInfo,
                                                  RConnection::EAttachTypeNormal );

                if ( attachError == KErrNone || attachError == KErrInUse )
                    {
                    attachError = KErrNone; //KErrInUse means already attached
                    break;
                    }
                }
            }
        }

    return attachError;
    }

// -----------------------------------------------------------------------------
// CUpnpConnectionManagerProxy::ActiveIap
// Returns active Iap or KErrNotFound if not connected.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpConnectionManagerProxy::ActiveIap()
    {
    if ( iActiveIap == KErrNotFound )
        {
        iActiveIap = iConnectionManagerSession.RequestActiveIap();
        }

    return iActiveIap;
    }

// -----------------------------------------------------------------------------
// CUpnpConnectionManagerProxy::LocalAddress
// Returns local address
// -----------------------------------------------------------------------------
//
EXPORT_C TInetAddr CUpnpConnectionManagerProxy::LocalAddress()
    {    
    return iConnectionManagerSession.RequestLocalAddress();
    }

// -----------------------------------------------------------------------------
// CUpnpConnectionManagerProxy::SubscribeForNetworkEventsL
// Subscribe for network events.
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpConnectionManagerProxy::SubscribeForNetworkEventsL( 
                                           MUpnpNetworkEventObserver *aObserver )
    {
    delete iNetworkEventProvider;
    iNetworkEventProvider = NULL;
    if ( aObserver )
        {
        iNetworkEventProvider = CUpnpConnectionManagerNetworkEventProvider::NewL( 
                                                iConnectionManagerSession, *aObserver );
        }
    }

// -----------------------------------------------------------------------------
// CUpnpConnectionManagerProxy::GetActiveIapL
// Static function for straightforward way of obtaining IAP from server.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpConnectionManagerProxy::GetActiveIapL()
    {
    RSocketServ socketServer;
    User::LeaveIfError( socketServer.Connect() );
    CleanupClosePushL( socketServer );    
    CUpnpConnectionManagerProxy *proxy = CUpnpConnectionManagerProxy::NewLC( socketServer );
    
    TInt iap = proxy->ActiveIap();
        
    CleanupStack::PopAndDestroy( proxy );
    CleanupStack::PopAndDestroy( &socketServer );
    
    return iap;
    }

// End of file
