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
* Description:  Defines CUpnpConnManagerEngine
*
*/


// INCLUDE FILES
#include "upnpconnmanagerengine.h"
#include "upnpsettings.h"
#include "upnpconnmanagersession.h"

#define KLogFile _L("UPnPConnManager.txt")
#include "upnpcustomlog.h"

const TTimeIntervalMicroSeconds32 KUnsuccesfulStartCacheTimeout( 2000000 );
const TTimeIntervalMicroSeconds32 KWlanLostTimeout( 2000000 );
const TInt KErrWlanLost = KErrNotReady;


// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CUpnpConnManagerEngine::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpConnManagerEngine* CUpnpConnManagerEngine::NewL( TDblQueIter<CSession2>& aIterator )
    {
    LOG_FUNC_NAME;
    CUpnpConnManagerEngine* self = new( ELeave ) CUpnpConnManagerEngine( aIterator );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpConnManagerEngine::CUpnpConnManagerEngine
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpConnManagerEngine::CUpnpConnManagerEngine( TDblQueIter<CSession2>& aIterator )
    : iActiveIap( KErrNotFound ), iSessionIter( aIterator )
    {
    }


// -----------------------------------------------------------------------------
// CUpnpConnManagerEngine::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpConnManagerEngine::ConstructL()
    {
    LOG_FUNC_NAME;

    // create a socket server connection
    User::LeaveIfError( iSocketServ.Connect() );

    iIfWatcher = CUpnpConnInterfaceWatcher::NewL( *this, iSocketServ, iConnection );

    iUnsuccesfulStartCacheTimer = CUpnpNotifyTimer::NewL( this );
    }

// -----------------------------------------------------------------------------
// CUpnpConnManagerEngine::CUpnpConnManagerEngine
// Destructor.
// -----------------------------------------------------------------------------
//
CUpnpConnManagerEngine::~CUpnpConnManagerEngine()
    {
    LOG_FUNC_NAME;

    delete iUnsuccesfulStartCacheTimer;
    
    delete iIfWatcher;
    CancelAllSubscription();

    iConnection.Close();
    iSocketServ.Close();
    }

// -----------------------------------------------------------------------------
// CUpnpConnManagerEngine::EnsureStart
// After call of this method the caller is sure that
// RConnection was started.
// -----------------------------------------------------------------------------
//
TInt CUpnpConnManagerEngine::EnsureStart( TInt& aAccessPoint )
    {
    TInt startError;
    if ( !iStartAttemptDone )
        {
        TRAP( startError, EnsureStartL() );
        if( KErrNone != startError )
            {
            iConnection.Close();
            }
        
        SetStartAttemptStatus( startError, KUnsuccesfulStartCacheTimeout );        
        }
    else
        {
        startError = iCachedStartError;
        }
    aAccessPoint = iActiveIap;
    return startError;
    }

// -----------------------------------------------------------------------------
// CUpnpConnManagerEngine::EnsureStartL
// RConnection starting logic.
// -----------------------------------------------------------------------------
//
void CUpnpConnManagerEngine::EnsureStartL()
    {
    TUint32 activeIap;
    _LIT( KIAPId, "IAP\\Id" );

    SetupConnectionPreferencesL();

    User::LeaveIfError( iConnection.Open( iSocketServ ) );
    User::LeaveIfError( iConnection.Start( iCommDbConnPref ) );
    
    // because iCommDbConnPref is not updated by Start method
    // we retrieve active iap here
    // it is crucial for always ask case when predefined
    // wlan is chosen from unavailable wlan list
    User::LeaveIfError( iConnection.GetIntSetting( KIAPId, activeIap ) );

    iIfWatcher->ListenAddressChangeL( activeIap );
    iIfWatcher->SubscribeForConnectionServiceChangeNotifications();
    
    iActiveIap = activeIap;
    }

// -----------------------------------------------------------------------------
// CUpnpConnectionManager::SetupConnectionPreferencesL
// Gets Iap from central repository and sets iCommDbConnPref.
// -----------------------------------------------------------------------------
//
void CUpnpConnManagerEngine::SetupConnectionPreferencesL()
    {
    iCommDbConnPref.SetIapId( CUpnpSettings::GetIapL() );
    iCommDbConnPref.SetDialogPreference( ECommDbDialogPrefDoNotPrompt );
    iCommDbConnPref.SetBearerSet( ECommDbBearerWLAN );
    }

// -----------------------------------------------------------------------------
// CUpnpConnectionManager::IsStarted
// Indicates whether RConnection was started.
// -----------------------------------------------------------------------------
//
TBool CUpnpConnManagerEngine::IsStarted()
    {
    return iStartAttemptDone 
           && ( KErrNone == iCachedStartError ) 
           && ( iActiveIap > KErrNotFound );
    }

// -----------------------------------------------------------------------------
// CUpnpConnectionManager::ActiveIap
// Returns active iap
// -----------------------------------------------------------------------------
//
TInt CUpnpConnManagerEngine::ActiveIap()
    {
    return iActiveIap;
    }

// -----------------------------------------------------------------------------
// CUpnpConnManagerEngine::LocalAddress
// -----------------------------------------------------------------------------
//  
const TInetAddr& CUpnpConnManagerEngine::LocalAddress()
    {
    return iIfWatcher->LocalAddress();
    }

// -----------------------------------------------------------------------------
// CUpnpConnectionManager::SetStartAttemptStatus
// -----------------------------------------------------------------------------
//
void CUpnpConnManagerEngine::SetStartAttemptStatus( TInt aError,
                                                    TTimeIntervalMicroSeconds32 aUSec )
    {
    iStartAttemptDone = ETrue;
    iCachedStartError = aError;

    if ( iCachedStartError )
        {
        // start timer to clean start attempt status after aUSec microseconds
        iUnsuccesfulStartCacheTimer->Cancel();
        iUnsuccesfulStartCacheTimer->Start( aUSec );
        }
    }

// -----------------------------------------------------------------------------
// CUpnpConnectionManager::ClearStartAttemptStatus
// -----------------------------------------------------------------------------
//
void CUpnpConnManagerEngine::ClearStartAttemptStatus()
    {
    iStartAttemptDone = EFalse;
    }

// -----------------------------------------------------------------------------
// CUpnpConnManagerEngine::TimerEventL
// -----------------------------------------------------------------------------
//
void CUpnpConnManagerEngine::TimerEventL( CUpnpNotifyTimer* /*aTimer*/ )
    {
    ClearStartAttemptStatus();
    }

// -----------------------------------------------------------------------------
// CUpnpConnManagerEngine::WlanNetworkLost
// -----------------------------------------------------------------------------
//
void CUpnpConnManagerEngine::WlanNetworkLost()
    {
    LOG_FUNC_NAME;
    NotifyAllAboutWlanLost();
    SetStartAttemptStatus( KErrWlanLost, KWlanLostTimeout );
    }
    
// -----------------------------------------------------------------------------
// CUpnpConnManagerEngine::NotifyAllAboutWlanLostL
// -----------------------------------------------------------------------------
//
void CUpnpConnManagerEngine::NotifyAllAboutWlanLost()
    {
    iSessionIter.SetToFirst();
    CUpnpConnManagerSession* ses = 
              reinterpret_cast<CUpnpConnManagerSession*>( iSessionIter++ );
    while ( ses )
        {
        TRAP_IGNORE( ses->WLanLostOccuredL() );
        ses = reinterpret_cast<CUpnpConnManagerSession*>( iSessionIter++ );    
        }
    }    

// -----------------------------------------------------------------------------
// CUpnpConnManagerEngine::InterfaceAddressChange
// -----------------------------------------------------------------------------
//
void CUpnpConnManagerEngine::InterfaceAddressChange( TInetAddr& aAddr )
    {
    LOG_FUNC_NAME;
    if ( IsStarted() )
        {
        NotifyAllAboutAddressChange( aAddr );
        }
    }

// -----------------------------------------------------------------------------
// CUpnpConnManagerEngine::NotifyAllAboutAddressChangeL
// -----------------------------------------------------------------------------
//
void CUpnpConnManagerEngine::NotifyAllAboutAddressChange( TInetAddr& aAddr )
    {
    iSessionIter.SetToFirst();
    CUpnpConnManagerSession* ses = 
              reinterpret_cast<CUpnpConnManagerSession*>( iSessionIter++ );
    while ( ses )
        {
        TRAP_IGNORE( ses->AddressChangeOccuredL( aAddr ) );
        ses = reinterpret_cast<CUpnpConnManagerSession*>( iSessionIter++ );    
        }
    }    

// -----------------------------------------------------------------------------
// CUpnpConnManagerEngine::CancelAllSubscription
// -----------------------------------------------------------------------------
//    
void CUpnpConnManagerEngine::CancelAllSubscription()
    {
    iSessionIter.SetToFirst();
    CUpnpConnManagerSession* ses = 
              reinterpret_cast<CUpnpConnManagerSession*>( iSessionIter++ );
    while ( ses )
        {
        ses->CancelSubscription();        
        ses = reinterpret_cast<CUpnpConnManagerSession*>( iSessionIter++ );    
        }
    }

// -----------------------------------------------------------------------------
// CUpnpConnManagerEngine::CloseConnection
// -----------------------------------------------------------------------------
//    
void CUpnpConnManagerEngine::CloseConnection()
    {
    iIfWatcher->CancelSubscriptionForConnectionServiceChangeNotifications();
    iConnection.Close();
    iActiveIap = KErrNotFound;
    
    ClearStartAttemptStatus();
    NotifyAllAboutConnectionClosed();
    }

// -----------------------------------------------------------------------------
// CUpnpConnManagerEngine::CheckAndCloseConnection
// -----------------------------------------------------------------------------
//    
void CUpnpConnManagerEngine::CheckAndCloseConnection(
                                             CUpnpConnManagerSession* aSession )
    {
    if ( !IsStarted() )
        {
        return;
        }
    
    iSessionIter.SetToFirst();
    CUpnpConnManagerSession* ses = 
              reinterpret_cast<CUpnpConnManagerSession*>( iSessionIter++ );
    while ( ses )
        {
        if ( ses != aSession && ses->SessionUseConnection() )
            {
            return;
            }
        ses = reinterpret_cast<CUpnpConnManagerSession*>( iSessionIter++ );    
        }

    CloseConnection();
    }

// -----------------------------------------------------------------------------
// CUpnpConnManagerEngine::NotifyAllAboutConnectionClosed
// -----------------------------------------------------------------------------
//    
void CUpnpConnManagerEngine::NotifyAllAboutConnectionClosed()
    {
    iSessionIter.SetToFirst();
    CUpnpConnManagerSession* ses = 
              reinterpret_cast<CUpnpConnManagerSession*>( iSessionIter++ );
    while ( ses )
        {
        TRAP_IGNORE( ses->ConnectionClosedL() );
        ses = reinterpret_cast<CUpnpConnManagerSession*>( iSessionIter++ );    
        }
    }

// End of File
