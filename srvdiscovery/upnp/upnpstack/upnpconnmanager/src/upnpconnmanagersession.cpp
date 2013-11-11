/** @file
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Defines the CUpnpConnManagerSession class
*
*/


// INCLUDE FILES
#include "upnpconnmanagersession.h"
#include "upnpconnmanagerengine.h"
#include "upnpconnmanager.h"
#include <upnpsymbianserverbase.h>

#define KLogFile _L("UPnPConnManager.txt")
#include "upnpcustomlog.h"

// ================= MEMBER FUNCTIONS =======================
// -----------------------------------------------------------------------------
// CUpnpConnManagerSession::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpConnManagerSession* CUpnpConnManagerSession::NewL(
    CUpnpConnManagerEngine& aEngine )
    {
    LOG_FUNC_NAME;
    CUpnpConnManagerSession* self =
        CUpnpConnManagerSession::NewLC( aEngine );
    CleanupStack::Pop( self ) ;
    return self ;
    }

// -----------------------------------------------------------------------------
// CUpnpConnManagerSession::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpConnManagerSession* CUpnpConnManagerSession::NewLC(
    CUpnpConnManagerEngine& aEngine )
    {
    LOG_FUNC_NAME;
    CUpnpConnManagerSession* self =
        new( ELeave ) CUpnpConnManagerSession( aEngine );
    CleanupStack::PushL( self );
    return self ;
    }

// -----------------------------------------------------------------------------
// CUpnpConnManagerSession::CUpnpConnManagerSession
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpConnManagerSession::CUpnpConnManagerSession( CUpnpConnManagerEngine& aEngine )
        : CSession2(),
        iConnEngine( aEngine ),
        iLastEvent( EUnknownEvent )
    {
#ifdef _DEBUG
    TInt reqsInStart = RThread().RequestCount();
    TInt processHandlesInStart;
    TInt threadHandlesInStart;
    RThread().HandleCount( processHandlesInStart, threadHandlesInStart );
    LOGS1( "reqsInStart: %d", reqsInStart);
    LOGS1( "processHandlesInStart: %d", processHandlesInStart );
    LOGS1( "threadHandlesInStart: %d", threadHandlesInStart );
#endif // _DEBUG
    }

// -----------------------------------------------------------------------------
// CUpnpConnManagerSession::~CUpnpConnManagerSession
// Destructor.
// -----------------------------------------------------------------------------
//
CUpnpConnManagerSession::~CUpnpConnManagerSession()
    {
    LOG_FUNC_NAME;
    
    iConnEngine.CheckAndCloseConnection( this );
    }

// -----------------------------------------------------------------------------
// CUpnpConnManagerSession::ServiceL
// Handle client requests.
// -----------------------------------------------------------------------------
//
void CUpnpConnManagerSession::ServiceL( const RMessage2& aMessage )
    {
    LOG_FUNC_NAME;
    TInt err;
    switch ( aMessage.Function() )
        {
        case EEnsureStartRConnection:
            {
            err = EventEnsureStartRConnectionL( aMessage );
            aMessage.Complete( err );
            break;
            }
        case EIsRConnectionStarted:
            {
            EventIsRConnectionStartedL( aMessage );
            aMessage.Complete( KErrNone );
            break;
            }
        case EEventSubscribe:
            {
            // subscribe to: 1) wlan lost or 2) address change
            EventSubscribeToNetworkEventL( aMessage );
            break;
            }
        case ECancelEventSubscribe:
            {
            EventCancelSubscribeToNetworkEvent();
            aMessage.Complete( KErrNone );
            break;
            }    
        case ECmActiveIap:
            {
            EventActiveIapL( aMessage );
            aMessage.Complete( KErrNone );
            break;
            }  
        case ECmGetAddress:
            {
            EventLocalAddressL( aMessage );
            aMessage.Complete( KErrNone );
            break;
            }
            
        default:
            PanicClient( aMessage, CUpnpSymbianServerBase::EBadRequest );
            break;
        }
    }

// -----------------------------------------------------------------------------
// CUpnpConnManagerSession::PanicClient
// Panic client.
// -----------------------------------------------------------------------------
//
void CUpnpConnManagerSession::PanicClient( const RMessage2& aMessage, TInt aPanic ) const
    {
    LOG_FUNC_NAME;
    static_cast<const CUpnpSymbianServerBase*>( Server() )->PanicClient( aMessage, aPanic );
    }

// -----------------------------------------------------------------------------
// CUpnpConnManagerSession::EventEnsureStartRConnectionL
// Device list change.
// -----------------------------------------------------------------------------
//
TInt CUpnpConnManagerSession::EventEnsureStartRConnectionL( const RMessage2& aMessage )
    {
    LOG_FUNC_NAME;

    TInt accessPoint;
    TInt result = iConnEngine.EnsureStart( accessPoint );
    TPtr8 ptr( reinterpret_cast<TUint8*>( &accessPoint ), sizeof( TInt ), sizeof( TInt ) );
    aMessage.WriteL( 0, ptr );
    
    iSessionUseConnection = ETrue;
    
    return result;
    }

// -----------------------------------------------------------------------------
// CUpnpConnManagerSession::EventIsRConnectionStartedL
// Checks if connection is started
// -----------------------------------------------------------------------------
//
void CUpnpConnManagerSession::EventIsRConnectionStartedL( const RMessage2& aMessage )
    {
    LOG_FUNC_NAME;

    TBool result = iConnEngine.IsStarted();
    TPtr8 ptr( reinterpret_cast<TUint8*>( &result ), sizeof( TBool ),
               sizeof( TBool ) );
    aMessage.WriteL( 0, ptr );
    }
    
// -----------------------------------------------------------------------------
// CUpnpConnManagerSession::EventSubscribeToNetworkEventL
// Subscribes to Wlan lost event
// -----------------------------------------------------------------------------
//
void CUpnpConnManagerSession::EventSubscribeToNetworkEventL( const RMessage2& aMessage )
    {
    LOG_FUNC_NAME;
    if ( !iSubscriberMessage.IsNull() )
        {
        aMessage.Complete( KErrServerBusy );
        return;
        }
    else 
        {
        iSubscriberMessage = aMessage;
        }    
        
    switch( iLastEvent.Type() )
        {
        case EWlanLostEvent: 
                WLanLostCompleteL();
            break;
        case EAddressChangeEvent: 
                AddressChangeCompleteL();
            break;
        default: 
            break;
        }
    }

// -----------------------------------------------------------------------------
// CUpnpConnManagerSession::EventCancelSubscribeToNetworkEvent
// Cancel subscription
// -----------------------------------------------------------------------------
//
void CUpnpConnManagerSession::EventCancelSubscribeToNetworkEvent()
    {
    LOG_FUNC_NAME;
    CancelSubscription();
    }

// -----------------------------------------------------------------------------
// CUpnpConnManagerSession::EventActiveIap
// Returns active Iap.
// -----------------------------------------------------------------------------
//
void CUpnpConnManagerSession::EventActiveIapL( const RMessage2 &aMessage )
    {
    LOG_FUNC_NAME;

    TInt iap = iConnEngine.ActiveIap();
    TPtr8 ptr( reinterpret_cast<TUint8*>( &iap ), sizeof( TInt ), sizeof( TInt ) );

    aMessage.WriteL( 0, ptr );
    }

// -----------------------------------------------------------------------------
// CUpnpConnManagerSession::EventLocalAddressL
// Returns local address
// -----------------------------------------------------------------------------
//
void CUpnpConnManagerSession::EventLocalAddressL( const RMessage2 &aMessage )
    {
    LOG_FUNC_NAME;

    TInetAddr localAddress = iConnEngine.LocalAddress();
    TPtr8 ptr( reinterpret_cast<TUint8*>( &localAddress ), 
               sizeof( TInetAddr ), 
               sizeof( TInetAddr ) );

    aMessage.WriteL( 0, ptr );       
    }

// -----------------------------------------------------------------------------
// CUpnpConnManagerSession::CancelSubscription
// Cancel subscription
// -----------------------------------------------------------------------------
//
void CUpnpConnManagerSession::CancelSubscription()
    {
    if ( !iSubscriberMessage.IsNull() )
        {
        iSubscriberMessage.Complete( KErrCancel );
        }
    iLastEvent.SetType( EUnknownEvent );    
    }
    
// -----------------------------------------------------------------------------
// CUpnpConnManagerSession::WLanLostOccured
// Ends subscription with Wlan lost code
// -----------------------------------------------------------------------------
//
void CUpnpConnManagerSession::WLanLostOccuredL()
    {
    iLastEvent.SetType( EWlanLostEvent );
    if ( !iSubscriberMessage.IsNull() )
        {
        WLanLostCompleteL();
        }    
    }

// -----------------------------------------------------------------------------
// CUpnpConnManagerSession::WLanLostComplete
// Notify client about WLan lost event
// -----------------------------------------------------------------------------
//
void CUpnpConnManagerSession::WLanLostCompleteL()
    {
    TPtr8 ptr( reinterpret_cast<TUint8*>( &iLastEvent ),
               sizeof( TUpnpConnectionManagerNetworkEvent ),
               sizeof( TUpnpConnectionManagerNetworkEvent ) );
               
    iSubscriberMessage.WriteL( KNetworkEventSlot, ptr );
    iSubscriberMessage.Complete( KErrNone );
    iLastEvent.SetType( EUnknownEvent );
    }

// -----------------------------------------------------------------------------
// CUpnpConnManagerSession::AddressChangeOccuredL
// Ends subscription with address change code
// -----------------------------------------------------------------------------
//
void CUpnpConnManagerSession::AddressChangeOccuredL( TInetAddr& aAddr )
    {
    iLastEvent.SetType( EAddressChangeEvent );
    iLastEvent.SetAddress( aAddr );
        
    if ( !iSubscriberMessage.IsNull() )
        {
        AddressChangeCompleteL();
        }    
    }

// -----------------------------------------------------------------------------
// CUpnpConnManagerSession::AddressChangeCompleteL
// Notify client about address change event
// -----------------------------------------------------------------------------
//
void CUpnpConnManagerSession::AddressChangeCompleteL()
    {
    TPtr8 ptr( reinterpret_cast<TUint8*>( &iLastEvent ),
               sizeof( TUpnpConnectionManagerNetworkEvent ),
               sizeof( TUpnpConnectionManagerNetworkEvent ) );
           
    iSubscriberMessage.WriteL( KNetworkEventSlot, ptr );
    iSubscriberMessage.Complete( KErrNone );
    iLastEvent.SetType( EUnknownEvent );
    }

// -----------------------------------------------------------------------------
// CUpnpConnManagerSession::SessionUseConnection
// -----------------------------------------------------------------------------
//
TBool CUpnpConnManagerSession::SessionUseConnection()
    {
    return iSessionUseConnection;
    }

// -----------------------------------------------------------------------------
// CUpnpConnManagerSession::ConnectionClosedL
// Ends subscription with connection close code
// -----------------------------------------------------------------------------
//
void CUpnpConnManagerSession::ConnectionClosedL()
    {
    if ( iLastEvent.Type() != EConnectionClosedEvent )
        {
        iLastEvent.SetType( EConnectionClosedEvent );
        
        if ( !iSubscriberMessage.IsNull() )
            {
            ConnectionClosedCompleteL();
            }    
        }
    }

// -----------------------------------------------------------------------------
// CUpnpConnManagerSession::ConnectionClosedCompleteL
// Notify client about connection close event
// -----------------------------------------------------------------------------
//
void CUpnpConnManagerSession::ConnectionClosedCompleteL()
    {
    TPtr8 ptr( reinterpret_cast<TUint8*>( &iLastEvent ),
               sizeof( TUpnpConnectionManagerNetworkEvent ),
               sizeof( TUpnpConnectionManagerNetworkEvent ) );
               
    iSubscriberMessage.WriteL( KNetworkEventSlot, ptr );
    iSubscriberMessage.Complete( KErrNone );
    }

// End of File
