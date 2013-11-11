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
#include <upnpconnectionmanagernetworkeventprovider.h>
#include "upnpconnectionmanagersession.h"
#include "upnpcustomlog.h"

// -----------------------------------------------------------------------------
// CUpnpConnectionManagerNetworkEventProvider::NewL
// Two-phased constructor.
//
// -----------------------------------------------------------------------------
EXPORT_C CUpnpConnectionManagerNetworkEventProvider* CUpnpConnectionManagerNetworkEventProvider::
                                    NewL( MUpnpNetworkEventObserver& aObserver )
    {
    CUpnpConnectionManagerNetworkEventProvider *self = new ( ELeave )
                                     CUpnpConnectionManagerNetworkEventProvider( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpConnectionManagerNetworkEventProvider::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpConnectionManagerNetworkEventProvider* CUpnpConnectionManagerNetworkEventProvider::NewL( 
        RUpnpConnectionManagerSession& aClient,
        MUpnpNetworkEventObserver& aObserver )
    {
    CUpnpConnectionManagerNetworkEventProvider *self = new ( ELeave ) 
                           CUpnpConnectionManagerNetworkEventProvider( aClient, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpConnectionManagerNetworkEventProvider::~CUpnpConnectionManagerNetworkEventProvider
// C++ destructor
// -----------------------------------------------------------------------------
//                                 
CUpnpConnectionManagerNetworkEventProvider::~CUpnpConnectionManagerNetworkEventProvider()   
    {
    Cancel();
    if ( iSessionOwner && iConnectionManagerSession )
        {
        iConnectionManagerSession->Close();
        delete iConnectionManagerSession;
        }
    }

// -----------------------------------------------------------------------------
// CUpnpConnectionManagerNetworkEventProvider::CUpnpConnectionManagerNetworkEventProvider
// C++ constructor
// -----------------------------------------------------------------------------
//
CUpnpConnectionManagerNetworkEventProvider::CUpnpConnectionManagerNetworkEventProvider( 
    RUpnpConnectionManagerSession& aClient, MUpnpNetworkEventObserver& aObserver )
    : CActive( CActive::EPriorityStandard ), iConnectionManagerSession( &aClient ), 
      iNetworkEventObserver( aObserver )
    {
    }

// -----------------------------------------------------------------------------
// CUpnpConnectionManagerNetworkEventProvider::CUpnpConnectionManagerNetworkEventProvider
// C++ constructor
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpConnectionManagerNetworkEventProvider::CUpnpConnectionManagerNetworkEventProvider( 
    MUpnpNetworkEventObserver& aObserver )
    : CActive( CActive::EPriorityStandard ), iSessionOwner( ETrue ), 
      iNetworkEventObserver( aObserver )
    {    
    }

// -----------------------------------------------------------------------------
// CUpnpConnectionManagerNetworkEventProvider::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpConnectionManagerNetworkEventProvider::ConstructL()
    {
    if ( iSessionOwner )
        {
        iConnectionManagerSession = new (ELeave) RUpnpConnectionManagerSession;
        User::LeaveIfError( iConnectionManagerSession->Connect() );
        }
    CActiveScheduler::Add( this );
    Subscribe();
    }

// -----------------------------------------------------------------------------
// CUpnpConnectionManagerNetworkEventProvider::Subscribe
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpConnectionManagerNetworkEventProvider::Subscribe()
    {
    if( !IsActive() )
        {
        iConnectionManagerSession->RequestSubscribeToNetworkEvents( iStatus, 
                                                                    iCurrentNetworkEvent );
        SetActive();
        }
    }

// -----------------------------------------------------------------------------
// CUpnpConnectionManagerNetworkEventProvider::DispatchResponseNetworkEventL
// -----------------------------------------------------------------------------
//
void CUpnpConnectionManagerNetworkEventProvider::DispatchResponseNetworkEventL()
    {
    switch ( iCurrentNetworkEvent.Type() )
        {
        case EConnectionClosedEvent:
                {
                CUpnpNetworkEventBase* networkEvent =
                      new (ELeave) CUpnpNetworkEventBase( EConnectionClosedEvent );
                iNetworkEventObserver.NetworkEvent( networkEvent );
                delete networkEvent;
                }
            break;
        case EWlanLostEvent:
                {
                CUpnpNetworkEventBase* networkEvent = 
                              new (ELeave) CUpnpNetworkEventBase( EWlanLostEvent );
                iNetworkEventObserver.NetworkEvent( networkEvent );
                delete networkEvent;
                }
            break;
        case EAddressChangeEvent:
                {
                CUpnpNetworkEventAddressChange* networkEvent = new (ELeave) 
                    CUpnpNetworkEventAddressChange( iCurrentNetworkEvent.InetAddr() );
                iNetworkEventObserver.NetworkEvent( networkEvent );
                delete networkEvent;
                }
            break;
        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
// CUpnpConnectionManagerNetworkEventProvider::RunL
// -----------------------------------------------------------------------------
//
void CUpnpConnectionManagerNetworkEventProvider::RunL()
    {
    TInt error = iStatus.Int();
    if ( error >= KErrNone )
        {
        TRAP( error, DispatchResponseNetworkEventL(); 
                     Subscribe() );
        }
    if ( error < KErrNone )
        {
        CUpnpNetworkEventBase* networkEvent = 
                       new (ELeave) CUpnpNetworkEventBase( error );
        iNetworkEventObserver.NetworkEvent( networkEvent );
        delete networkEvent;
        }    
    }

// -----------------------------------------------------------------------------
// CUpnpConnectionManagerNetworkEventProvider::RunError
// -----------------------------------------------------------------------------
//    
TInt CUpnpConnectionManagerNetworkEventProvider::RunError( TInt /*aError*/ )
    {
    LOG_FUNC_NAME;

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CUpnpConnectionManagerNetworkEventProvider::DoCancel
// -----------------------------------------------------------------------------
//    
void CUpnpConnectionManagerNetworkEventProvider::DoCancel()
    {
    iConnectionManagerSession->RequestCancelSubscribeToNetworkEvents();
    }


// End of file
