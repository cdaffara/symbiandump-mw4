/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  IKE socket connection
*
*/


#include "ikeconnection.h"
#include "datatransfer.h"
#include "localaddressresolver.h"
#include "ikedebug.h"
#include "ikesocketassert.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CIkeConnection* CIkeConnection::NewL( MIkeDebug& aDebug )
    {
    CIkeConnection* self = new (ELeave) CIkeConnection( aDebug );  
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);    
    return self;            
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CIkeConnection::~CIkeConnection()
    {
    DEBUG_LOG1( _L("CIkeConnection::~CIkeConnection this=0x%x"), this );

    if ( iDataTransfer )
        {
        iDataTransfer->CloseSockets();
        }
        
    DoCancelResolveFQDNAddress();
    DoCancelStartConnection();
                
    delete iLocalAddressResolver;
    delete iDataTransfer;
    delete iLinkObserver;
    
    iConnection.Close();
    iSocketServer.Close();    
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CIkeConnection::CIkeConnection( MIkeDebug& aDebug )
 : CIkeConnectionInterface( EPriorityStandard ),
   iState( EIdle ),
   iExtendedPrefs(),
   iDebug( aDebug )
    {
    CActiveScheduler::Add( this ); // Added to the Active Scheduler
    }

// ---------------------------------------------------------------------------
// Second phase construction.
// ---------------------------------------------------------------------------
//
void CIkeConnection::ConstructL()
    {
    DEBUG_LOG1( _L("CIkeConnection::ConstructL, this=0x%x"), this );    
    
    User::LeaveIfError( iSocketServer.Connect() );    
    iLocalAddressResolver = CLocalAddressResolver::NewL( iSocketServer,
                                                         iConnection,
                                                         iDebug );    
    iDataTransfer = CDataTransfer::NewL( iSocketServer,
                                         iConnection,
                                         *iLocalAddressResolver,
                                         *this,
                                         iDebug );    
    iLinkObserver = CConnObserver::NewL( iConnection,
                                         *this,
                                         iDebug );    
    }

// ---------------------------------------------------------------------------
// Opens data interface.
// ---------------------------------------------------------------------------
//
MIkeDataInterface& CIkeConnection::OpenDataInterfaceL( const TIkeMajorVersion aIkeMajorVersion,
                                                       const TIpVersion aIpVersion )
    {        
    IKESOCKET_ASSERT( aIpVersion == EIPv4 || aIpVersion == EIPv6 );
    DEBUG_LOG2( _L("CIkeConnection::OpenDataInterfaceL, IKE version=%d, IP version=%d"),
            aIkeMajorVersion, aIpVersion );
    
    // Store IP version.
    iIpVersion = aIpVersion;
    
    // Get local IP address.
    User::LeaveIfError( iLocalAddressResolver->RefreshLocalAddresses() );    
    TBool hasIPAddr = iLocalAddressResolver->HasIPAddr( aIpVersion );
    if ( !hasIPAddr )
        {
        User::Leave( KErrNotFound );
        }
    TInetAddr localIp = iLocalAddressResolver->IPAddr( aIpVersion );
    
    // Open sockets.
    User::LeaveIfError( iDataTransfer->OpenSockets( localIp ) );                  
    
    // Set IKE major version.
    iDataTransfer->SetIkeMajorVersion( aIkeMajorVersion );

    // Set IP version
    iDataTransfer->SetIpVersion( aIpVersion );
    
    DEBUG_LOG( _L("Data interface open.") );
    
    // Return data interface.
    return *iDataTransfer;
    }

// ---------------------------------------------------------------------------
// Starts connection.
// ---------------------------------------------------------------------------
//
void CIkeConnection::StartConnection( const TUint32 aIapId,
                                      const TUint32 aSnapId,
                                      TRequestStatus& aStatus,
                                      const TBool aForcedRoaming )
    {
    IKESOCKET_ASSERT( iState == EIdle );
    IKESOCKET_ASSERT( iClientStatus == NULL );

    DEBUG_LOG3( _L("CIkeConnection::StartConnection, IAP id=%d, SNAP id=%d, forced roaming=%d"),
            aIapId, aSnapId, aForcedRoaming );    
    
    iState = EConnecting;
    
    iClientStatus = &aStatus;
    *iClientStatus = KRequestPending;
    iIapId = aIapId;
    iSnapId = aSnapId;
    
    TInt err( iConnection.Open( iSocketServer ) );
    
    if ( err == KErrNone )
        {
        // Start connection.
        TRAP( err, CreatePreferencesL( iIapId,
                                       iSnapId,
                                       aForcedRoaming ) );
        if ( err == KErrNone )
            {
            iConnection.Start( iConnPrefList, iStatus );
            }
        }
    
    if ( err != KErrNone )
        {
        TRequestStatus* ownStatus = &iStatus;
        *ownStatus = KRequestPending;
        SetActive();
        
        User::RequestComplete( ownStatus, err );
        return;
        }
    
    SetActive();
    }

// ---------------------------------------------------------------------------
// Cancels connection starting.
// ---------------------------------------------------------------------------
//
void CIkeConnection::CancelStartConnection()
    {
    DEBUG_LOG( _L("CIkeConnection::CancelStartConnection") );
    
    DoCancelStartConnection();
    }

// ---------------------------------------------------------------------------
// Stops connection.
// ---------------------------------------------------------------------------
//
void CIkeConnection::StopConnection()
    {
    IKESOCKET_ASSERT( iLinkObserver );
    IKESOCKET_ASSERT( iDataTransfer );
    
    DEBUG_LOG( _L("CIkeConnection::StopConnection") );

    iLinkObserver->CancelNotify();
    
    CancelResolveFQDNAddress();
    CancelStartConnection();
    
    iDataTransfer->CloseSockets();
    iConnection.Close();
    
    iState = EIdle;
    }

// ---------------------------------------------------------------------------
// Resolves FQDN address.
// ---------------------------------------------------------------------------
//
void CIkeConnection::ResolveFQDNAddress( const TDesC& aFQDN,
                                         TNameEntry& aNameEntry,
                                         TRequestStatus& aStatus )
    {
    IKESOCKET_ASSERT( iState == EConnected );    
    DEBUG_LOG1( _L("CIkeConnection::ResolveAddress, aFQDN=%S"), &aFQDN );
    
    iState = EResolvingFQDN;
    
    iClientStatus = &aStatus;
    *iClientStatus = KRequestPending;    
    
    TInt err = iResolver.Open( iSocketServer,
                               KAfInet,
                               KProtocolInetUdp,
                               iConnection );
    
    if ( err )
        {
        TRequestStatus* ownStatus = &iStatus;
        *ownStatus = KRequestPending;
        SetActive();
        
        User::RequestComplete( ownStatus, err );
        return;
        }
    
    iResolver.GetByName( aFQDN, aNameEntry, iStatus );
    SetActive();
    }

// ---------------------------------------------------------------------------
// Cancels FQDN address resolving.
// ---------------------------------------------------------------------------
//
void CIkeConnection::CancelResolveFQDNAddress()
    {
    DEBUG_LOG( _L("CIkeConnection::CancelResolveFQDNAddress") );
    
    DoCancelResolveFQDNAddress();
    }

// ---------------------------------------------------------------------------
// Request notification about disconnection.
// ---------------------------------------------------------------------------
//
void CIkeConnection::NotifyDisconnect( TRequestStatus& aStatus )
    {
    IKESOCKET_ASSERT( iClientStatusNotifyDisconnect == NULL );
    DEBUG_LOG( _L("CIkeConnection::NotifyDisconnect") );
    
    iClientStatusNotifyDisconnect = &aStatus;
    *iClientStatusNotifyDisconnect = KRequestPending;
    }

// ---------------------------------------------------------------------------
// Cancels disconnect notification request.
// ---------------------------------------------------------------------------
//
void CIkeConnection::CancelNotifyDisconnect()
    {
    IKESOCKET_ASSERT( iClientStatusNotifyDisconnect );
    DEBUG_LOG( _L("CIkeConnection::CancelNotifyDisconnect") );
    
    User::RequestComplete( iClientStatusNotifyDisconnect, KErrCancel );
    iClientStatusNotifyDisconnect = NULL;
    }

// ---------------------------------------------------------------------------
// Returns IAP id.
// ---------------------------------------------------------------------------
//
TUint32 CIkeConnection::IapId() const
    {
    return iIapId;
    }

// ---------------------------------------------------------------------------
// Returns Net id.
// ---------------------------------------------------------------------------
//
TUint32 CIkeConnection::NetId() const
    {
    return iNetId;
    }

// ---------------------------------------------------------------------------
// Returns SNAP id.
// ---------------------------------------------------------------------------
//
TUint32 CIkeConnection::SnapId() const
    {
    return iSnapId;
    }

// ---------------------------------------------------------------------------
// Gets local IP address.
// ---------------------------------------------------------------------------
//
TInt CIkeConnection::GetLocalAddress( const TIpVersion aIpVersion,
                                      TInetAddr& aLocalIp )
    {
    IKESOCKET_ASSERT( aIpVersion == EIPv4 || aIpVersion == EIPv6 );
    return iLocalAddressResolver->GetLocalAddress( aIpVersion, aLocalIp );
    }

// ---------------------------------------------------------------------------
// Creates connection preferences. Connection preferences list is constructed.
// ---------------------------------------------------------------------------
//
void CIkeConnection::CreatePreferencesL( const TUint32 aIapId,
                                         const TUint32 aSnapId,
                                         const TBool aForcedRoaming )
    {
    CleanPreferences();
    
    if ( aIapId != 0 )
        {
        iExtendedPrefs.SetIapId( aIapId );
        }
    else
        {
        iExtendedPrefs.SetSnapId( aSnapId );
        iExtendedPrefs.SetForcedRoaming( aForcedRoaming );
        }
    
    iExtendedPrefs.SetNoteBehaviour( TExtendedConnPref::ENoteBehaviourConnDisableNotes );
    
    iConnPrefList.AppendL( &iExtendedPrefs );  
    }

// ---------------------------------------------------------------------------
// Cleans connection preferences created for SNAP usage.
// ---------------------------------------------------------------------------
//
void CIkeConnection::CleanPreferences()
    {
    while( iConnPrefList.Count() > 0 )
        {
        iConnPrefList.Remove( 0 );
        }       
    }

// ---------------------------------------------------------------------------
// Updates IAP id and NET id.
// ---------------------------------------------------------------------------
//
void CIkeConnection::UpdateRealIapData()
    {
    _LIT( KIapId, "IAP\\Id" );
    _LIT( KNetId, "IAP\\IAPNetwork" );
    
    iConnection.GetIntSetting( KIapId, iIapId );
    iConnection.GetIntSetting( KNetId, iNetId );
    
    DEBUG_LOG2( _L("CIkeConnection::UpdateRealIapData, IAP id=%d, NET id=%d"),
            iIapId, iNetId );
    }

// ---------------------------------------------------------------------------
// Handles completion of asynchronous request in EConnecting state.
// ---------------------------------------------------------------------------
//
void CIkeConnection::DoStateAfterConnecting()
    {
    IKESOCKET_ASSERT( iLinkObserver );
    IKESOCKET_ASSERT( iState == EConnecting );
    
    CleanPreferences();

    TInt err( iStatus.Int() );

    if ( err == KErrNone )
        {
        // Update IAP and Net ids.
        UpdateRealIapData();
            
        // Start observing when link is disconnected.
        iLinkObserver->NotifyDisconnect();
        
        iState = EConnected;        
        }
    else
        {        
        iConnection.Close();        
        iState = EIdle;
        }
    
    User::RequestComplete( iClientStatus, err );
    iClientStatus = NULL;
    }

// ---------------------------------------------------------------------------
// Handles completion of asynchronous request in EResolvingFQDN state.
// ---------------------------------------------------------------------------
//
void CIkeConnection::DoStateAfterResolvingFQDN()
    {
    IKESOCKET_ASSERT( iState == EResolvingFQDN );
    
    // Back to connected state.
    iState = EConnected;    
    iResolver.Close();

    User::RequestComplete( iClientStatus, iStatus.Int() );
    iClientStatus = NULL;
    }
    
// ---------------------------------------------------------------------------
// Implements cancellation of connection starting.
// ---------------------------------------------------------------------------
//
void CIkeConnection::DoCancelStartConnection()
    {
    if ( iState == EConnecting )
        {
        IKESOCKET_ASSERT( iClientStatus );
        
        Cancel();
        
        iState = EIdle;
        iConnection.Close();
        
        CleanPreferences();

        User::RequestComplete( iClientStatus, KErrCancel );
        iClientStatus = NULL;
        }
    }

// ---------------------------------------------------------------------------
// Implements cancellation of FQDN address resolving.
// ---------------------------------------------------------------------------
//
void CIkeConnection::DoCancelResolveFQDNAddress()
    {
    if ( iState == EResolvingFQDN )
        {
        IKESOCKET_ASSERT( iClientStatus );
        
        Cancel();
        
        iState = EConnected;
        iResolver.Close();
        
        User::RequestComplete( iClientStatus, KErrCancel );
        iClientStatus = NULL;        
        }
    }

// ---------------------------------------------------------------------------
// From CActive
// Handles request completion event about asynchronous connection starting or
// FQDN address resolving.
// ---------------------------------------------------------------------------
//
void CIkeConnection::RunL()
    {    
    DEBUG_LOG2( _L("CIkeConnection::RunL, iState=%d, iStatus=%d"),
            iState, iStatus.Int() );
    
    switch ( iState )
        {
        case EConnecting:
            DoStateAfterConnecting();
            break;
        case EResolvingFQDN:
            DoStateAfterResolvingFQDN();
            break;
        default:
            IKESOCKET_ASSERT( EFalse );
            break;
        }    
    }

// ---------------------------------------------------------------------------
// From CActive
// Implements cancellation of asynchronous connection starting or FQDN address
// resolving.
// ---------------------------------------------------------------------------
//
void CIkeConnection::DoCancel()
    {    
    DEBUG_LOG1( _L("CIkeConnection::DoCancel, iState=%d"),
            iState );

    switch ( iState )
        {
        case EConnecting:
            iConnection.Stop();
            break;
        case EResolvingFQDN:
            iResolver.Cancel();
            break;
        default:
            IKESOCKET_ASSERT( EFalse );
            break;
        }
    }

// ---------------------------------------------------------------------------
// Handles notifcation about fatal data transfer error. 
// ---------------------------------------------------------------------------
//
void CIkeConnection::DataTransferError( const TInt aError,
                                        const TErrorType /*aErrorType*/ )
    {    
    DEBUG_LOG1( _L("CIkeConnection::DataTransferError, aError=%d"),
            aError );
    
    // Disconnect link and notify client about disconnection.
    LinkDisconnected( aError );    
    }

// ---------------------------------------------------------------------------
// Handles notifcation about link disconnection. 
// ---------------------------------------------------------------------------
//
void CIkeConnection::LinkDisconnected( const TInt aStatus )
    {    
    // Stop connection.
    StopConnection();
    
    if ( iClientStatusNotifyDisconnect )
        {
        User::RequestComplete( iClientStatusNotifyDisconnect, aStatus );
        iClientStatusNotifyDisconnect = NULL;
        }    
    }
