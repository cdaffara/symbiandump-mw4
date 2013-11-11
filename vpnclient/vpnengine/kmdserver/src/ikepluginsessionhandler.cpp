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
* Description:  Handler of an IKE protocol plugin session
*
*/


#include <in_sock.h>

#include "errorobserver.h"
#include "iachangeobserver.h"
#include "ikeconnectioninterface.h"
#include "ikedebug.h"
#include "ikepluginif.h"
#include "ikepluginsessionif.h"
#include "ikesocketdefs.h"
#include "internaladdress.h"

// CLASS HEADER
#include "ikepluginsessionhandler.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CIkePluginSessionHandler* CIkePluginSessionHandler::NewLC( TUint32 aVpnIapId,
                                                           TUint32 aVpnNetId,
                                                           TUint32 aVpnInterfaceIndex,
                                                           IkeSocket::TIkeMajorVersion aIkeVersion,
                                                           IkeSocket::TIpVersion aIpVersion,
                                                           const TInetAddr& aDnsServerAddr,
                                                           CIkeConnectionInterface& aConnection,
                                                           MIkePluginIf& aIkePlugin,            
                                                           MIkePluginSessionHandlerCallback& aCallback,
                                                           MIkeDebug& aDebug )
    {
    CIkePluginSessionHandler* self = new (ELeave) CIkePluginSessionHandler( aVpnIapId,
                                                                            aIkeVersion,
                                                                            aIpVersion,
                                                                            aCallback,
                                                                            aDebug );
    CleanupStack::PushL( self );
    self->ConstructL( aVpnNetId,
                      aVpnInterfaceIndex,
                      aDnsServerAddr,
                      aConnection,
                      aIkePlugin );
    return self;
    }
    
// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CIkePluginSessionHandler::~CIkePluginSessionHandler()
    {
    Cancel();

    delete iErrorObserver;
    delete iIaChangeObserver;
    delete iIkePluginSession;
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CIkePluginSessionHandler::CIkePluginSessionHandler( TUint32 aVpnIapId,
                                                    IkeSocket::TIkeMajorVersion aIkeVersion,
                                                    IkeSocket::TIpVersion aIpVersion,
                                                    MIkePluginSessionHandlerCallback& aCallback,
                                                    MIkeDebug& aDebug ) 
 : CActive( EPriorityStandard ),
   iVpnIapId( aVpnIapId ),
   iIkeVersion( aIkeVersion ),
   iIpVersion( aIpVersion ),
   iCallback( aCallback ),
   iDebug( aDebug )
    {
    CActiveScheduler::Add( this );
    }    

// ---------------------------------------------------------------------------
// Second phase construction.
// ---------------------------------------------------------------------------
//
void CIkePluginSessionHandler::ConstructL( TUint32 aVpnNetId,
                                           TUint32 aVpnInterfaceIndex,
                                           const TInetAddr& aDnsServerAddr,
                                           CIkeConnectionInterface& aConnection,
                                           MIkePluginIf& aIkePlugin )
    {
    if ( iIkeDataInterface == NULL )
        {
        iIkeDataInterface = &aConnection.OpenDataInterfaceL( iIkeVersion,
                                                             iIpVersion );
        }    
    
    iIkePluginSession = aIkePlugin.CreateSessionL( iVpnIapId,
                                                   aVpnNetId,
                                                   aVpnInterfaceIndex,
                                                   *iIkeDataInterface );  
    
    iErrorObserver = CErrorObserver::NewL( *iIkePluginSession,
                                           iCallback,
                                           iDebug );
    
    iIaChangeObserver = CIaChangeObserver::NewL( iVpnIapId,
                                                 aDnsServerAddr,
                                                 *iIkePluginSession,
                                                 iDebug );
    
    DEBUG_LOG(_L("IKE plugin session created."));
    }       

// ---------------------------------------------------------------------------
// Starts negotiation with a remote host asynchronously.
// ---------------------------------------------------------------------------
//
void CIkePluginSessionHandler::NegotiateWithHost( const CIkeData& aIkeData )
    {
    __ASSERT_DEBUG( iState == EIdle,
                    User::Invariant() );
    iIkePluginSession->NegotiateWithHost( aIkeData,
                                          iInternalAddress,
                                          iStatus );
    SetActive();
    iState = ENegotiatingWithHost;
    }

// ---------------------------------------------------------------------------
// Cancels negotiation request.
// ---------------------------------------------------------------------------
//
void CIkePluginSessionHandler::CancelNegotiateWithHost()
    {
    if ( iState == ENegotiatingWithHost &&
         IsActive() )
        {
        Cancel();
        TVPNAddress empty;
        iCallback.NegotiationStarted( KErrCancel,
                                      empty );
        }
    }

// ---------------------------------------------------------------------------
// Deletes session. 
// ---------------------------------------------------------------------------
//
void CIkePluginSessionHandler::DeleteSession( TBool aSilentClose )
    {
    iErrorObserver->Cancel();
    iIaChangeObserver->Cancel();
    
    if ( iState == ENegotiated )
        {
        // Delete session asynchronously.
        iIkePluginSession->DeleteSession( aSilentClose, iStatus );
        SetActive();
        iState = EDeletingSession;
        }
    else if ( iState == EDeletingSession )
        {
        // Session deletion is already in progress. Cancelling it deletes
        // session silently.
        CancelDeleteSession();
        }
    else if ( iState == EIdle )
        {
        // Session deletion can be requested, if negotiate request has failed.
        // In this case, it is enough to complete request.
        TRequestStatus* ownStatus = &iStatus;
        *ownStatus = KRequestPending;
        SetActive();
                
        User::RequestComplete( ownStatus, KErrNone );
        }
    else
        {
        ASSERT( EFalse );
        }
    }

// ---------------------------------------------------------------------------
// Cancels session deletion request.
// ---------------------------------------------------------------------------
//
void CIkePluginSessionHandler::CancelDeleteSession()
    {
    if ( IsActive() )
        {
        Cancel();
        iCallback.IkePluginSessionClosed( KErrCancel );
        }
    }

// ---------------------------------------------------------------------------
// Returns VPN IAP Id.
// ---------------------------------------------------------------------------
//
TInt CIkePluginSessionHandler::VpnIapId() const
    {
    return iVpnIapId;
    }

// ---------------------------------------------------------------------------
// From class CActive
// Handles completion of asynchronous request. 
// ---------------------------------------------------------------------------
//
void CIkePluginSessionHandler::RunL()
    {
    DEBUG_LOG2( _L("CIkePluginSessionHandler::RunL, iState=%d, iStatus=%d"),
            iState, iStatus.Int() );

    switch ( iState )
        {
        case ENegotiatingWithHost:
            if ( iStatus.Int() == KErrNone )
                {
                iErrorObserver->StartObserving();
                iIaChangeObserver->StartObserving();
                iState = ENegotiated;
                }
            else
                {
                iState = EIdle;
                }
            // Ownership of internal address transferred.
            iCallback.NegotiationStarted( iStatus.Int(),
                                          iInternalAddress );
            break;
        case EDeletingSession: // Fall through
        case EIdle:        
            iState = EIdle;
            iCallback.IkePluginSessionClosed( iStatus.Int() );
            break;
        default:           
            ASSERT( EFalse );
            break;
        }        
    }

// ---------------------------------------------------------------------------
// From class CActive
// Handles cancellation of asynchronous request. 
// ---------------------------------------------------------------------------
//
void CIkePluginSessionHandler::DoCancel()
    {
    DEBUG_LOG1( _L("CIkePluginSessionHandler::DoCancel, iState=%d"),
            iState );

    switch ( iState )
        {
        case ENegotiatingWithHost: 
            iIkePluginSession->CancelNegotiateWithHost();
            iInternalAddress = TVPNAddress();
            // Session deletion is needed later, if negotiate request has
            // already been completed from IKE plugin session.            
            iState = ENegotiated;           
            break;
        case EDeletingSession:
            iIkePluginSession->CancelDeleteSession();
            iState = EIdle;
            break;
        case EIdle:                                    
            break;
        default:
            ASSERT( EFalse );
            break;
        }    
    }
