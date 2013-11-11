/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of hotspot management API
*
*/



// INCLUDE FILES
#include <e32std.h>
#include <implementationproxy.h>
#include "hssmgmtimpl.h"
#include "hssmgmtscaninfoimpl.h"
#include "am_debug.h"

// LOCAL CONSTANTS
/**
* Pairs ECom implementation UIDs with a pointer to the instantiation 
* method for that implementation. Required for all ECom implementation
* collections.
*/
const TImplementationProxy ImplementationTable[] =
    {
        {{0x10282ece}, reinterpret_cast<TProxyNewLPtr>(CHssMgmtImpl::NewL)},
        {{0x10282ed0}, reinterpret_cast<TProxyNewLPtr>(CHssMgmtScanInfoImpl::NewL)}
    };

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CHssMgmtImpl::CHssMgmtImpl() : 
	iClientNotification( NULL )	
    {
    DEBUG( "CHssMgmtImpl::CHssMgmtImpl()" );
    }

// Symbian 2nd phase constructor can leave.
void CHssMgmtImpl::ConstructL()
    {
    DEBUG( "CHssMgmtImpl::ConstructL()" );
    User::LeaveIfError( iServer.Connect() );
    }

// Static constructor.
CHssMgmtImpl* CHssMgmtImpl::NewL()
    {
    DEBUG( "CHssMgmtImpl::NewL()" );
    CHssMgmtImpl* self = new (ELeave) CHssMgmtImpl;
    CleanupStack::PushL( self );    
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// Destructor
CHssMgmtImpl::~CHssMgmtImpl()
    {
    DEBUG( "CHssMgmtImpl::~CHssMgmtImpl()" );
    iServer.Close();
    }

// ---------------------------------------------------------
// CHssMgmtImpl::ActivateNotificationsL
// ---------------------------------------------------------
//
void CHssMgmtImpl::ActivateNotificationsL( MHssMgmtNotifications& aCallback )
    {
    DEBUG( "CHssMgmtImpl::ActivateNotificationsL()" );   
	iClientNotification = &aCallback;
	iServer.ActivateNotificationsL( *this );
    }

// ---------------------------------------------------------
// CHssMgmtImpl::CancelNotifications
// ---------------------------------------------------------
//
void CHssMgmtImpl::CancelNotifications()
    {
    DEBUG( "CHssMgmtImpl::CancelNotifications()" );   
	iServer.CancelNotifications();
	iClientNotification = NULL;
    }

// ---------------------------------------------------------
// CHssMgmtImpl::GetScanResults
// ---------------------------------------------------------
//
TInt CHssMgmtImpl::GetScanResults( CHssMgmtScanInfo& aResults )
    {
    DEBUG( "CHssMgmtImpl::GetScanResults() " );
    // Because CHssMgmtScanInfo has been initialized through ECom
    // interface, the object is actually CHssMgmtScanInfoImpl.
    CHssMgmtScanInfoImpl* impl = reinterpret_cast<CHssMgmtScanInfoImpl*>(&aResults);
    
    TInt ret(KErrNone);
    ret = iServer.GetScanResults( impl->GetInternalList() );
    if ( ret != KErrNone )
        {
        DEBUG1( "GetScanResults() returned with %d", ret );
        return ret;
        }
    TRAP( ret, impl->UpdateResultsL() );
    if ( ret != KErrNone )
        {
        DEBUG1( "UpdateResultsL() returned with %d", ret );
        }
    return ret;
    }


// ---------------------------------------------------------
// CHssMgmtImpl::Register
// ---------------------------------------------------------
//
TUint CHssMgmtImpl::Register( const TUid aUid,
                              const TDesC& aIapName, 
                              const TUint aWaitNote )
    {
    DEBUG( "CHssMgmtImpl::Register()" );
    return iServer.Register( aUid, aIapName, aWaitNote );
    }
        
// ---------------------------------------------------------
// CHssMgmtImpl::UnRegister
// ---------------------------------------------------------
//
TInt CHssMgmtImpl::UnRegister( const TUid aUid,
                               const TUint aIapId, 
                               const TUint aNetworkId )
    {
    DEBUG( "CHssMgmtImpl::UnRegister()" );
    return iServer.UnRegister( aIapId, aNetworkId, aUid );	
    }

// ---------------------------------------------------------
// CHssMgmtImpl::ChangeSettings
// ---------------------------------------------------------
//
TInt CHssMgmtImpl::ChangeSettings( const TUint aIapId, const THssIapSettings& aSettings )
    {
    DEBUG( "CHssMgmtImpl::ChangeSettings()" );
    return iServer.ChangeSettings( aIapId, aSettings );	
    }

// ---------------------------------------------------------
// CHssMgmtImpl::Join
// ---------------------------------------------------------
//
TInt CHssMgmtImpl::Join( const TUint aIapId )
    {
    DEBUG( "CHssMgmtImpl::Join()" );
    return iServer.Join( aIapId );	
    }

// ---------------------------------------------------------
// CHssMgmtImpl::CancelStart
// ---------------------------------------------------------
//
TInt CHssMgmtImpl::CancelStart( const TUint aIapId )
    {
    DEBUG( "CHssMgmtImpl::CancelStart()" );
    return iServer.CancelStart( aIapId );		
    }

// ---------------------------------------------------------
// CHssMgmtImpl::Stop
// ---------------------------------------------------------
//
TInt CHssMgmtImpl::Stop( const TUint aIapId )
    {
    DEBUG( "CHssMgmtImpl::Stop()" );
    return iServer.Stop( aIapId );
    }

// ---------------------------------------------------------
// CHssMgmtImpl::LoginComplete
// ---------------------------------------------------------
//
TInt CHssMgmtImpl::LoginComplete( const TUint aIapId, const TInt aStatus )
    {
    DEBUG( "CHssMgmtImpl::LoginComplete()" );
    return iServer.LoginComplete( aIapId, aStatus );
    }

// ---------------------------------------------------------
// CHssMgmtImpl::LogoutComplete
// ---------------------------------------------------------
//
TInt CHssMgmtImpl::LogoutComplete( const TUint aIapId )
    {
	DEBUG( "CHssMgmtImpl::LogoutComplete()" );
    return iServer.LogoutComplete( aIapId );
    }

// ---------------------------------------------------------
// CHssMgmtImpl::CheckBackgroundScan
// ---------------------------------------------------------
//
TUint32 CHssMgmtImpl::CheckBackgroundScan()
    {
    DEBUG( "CHssMgmtImpl::CheckBackgroundScan()" );
    TInt ret(KErrNone);
    TRAPD( err, ret = iServer.CheckBackgroundScanL() );
    if ( err != KErrNone )
        {
        return err;
        }
    DEBUG1( "CheckBackgroundScan() returned with %d", ret );
    return ret;	
    }

// ---------------------------------------------------------
// CHssMgmtImpl::GetConnectionBssid
// ---------------------------------------------------------
//
TInt CHssMgmtImpl::GetConnectionBssid( THssBssid& aBssId )
    {
    DEBUG( "CHssMgmtImpl::GetConnectionBssid()" );
    return iServer.GetConnectionBssid( aBssId ); 
    }

// ---------------------------------------------------------
// CHssMgmtImpl::AddIapSsidList
// ---------------------------------------------------------
//
TInt CHssMgmtImpl::AddIapSsidList(
        TUint aIapId,
        const CArrayFixFlat<THssSsid>& aSsidList )
    {
    DEBUG( "CHssMgmtImpl::AddIapSsidList()" );
    return iServer.AddIapSsidList( aIapId, aSsidList ); 
    }

// ---------------------------------------------------------
// CHssMgmtImpl::NewNetworksDetected
// ---------------------------------------------------------
//
void CHssMgmtImpl::NewNetworksDetected( TUint aIapId )
    {
    DEBUG( "CHssMgmtImpl::NewNetworksDetected()" );
    iClientNotification->NewNetworksDetected( aIapId );
    }

// ---------------------------------------------------------
// CHssMgmtImpl::OldNetworksLost
// ---------------------------------------------------------
//    
void CHssMgmtImpl::OldNetworksLost( TUint aIapId )
    {
    DEBUG( "CHssMgmtImpl::OldNetworksLost()" );
    iClientNotification->OldNetworksLost( aIapId );
    }

// ---------------------------------------------------------
// CHssMgmtImpl::SetTimerValues
// ---------------------------------------------------------
//
TInt CHssMgmtImpl::SetTimerValues(
        TUid aClientUid,
        TUint aLoginTimerValue,
        TUint aLogoutTimerValue )
    {
    DEBUG( "CHssMgmtImpl::SetTimerValues()" );
    return iServer.SetTimerValues( aClientUid, aLoginTimerValue, aLogoutTimerValue );
    }

// ---------------------------------------------------------
// CHssMgmtImpl::GetClientIaps
// ---------------------------------------------------------
//
TInt CHssMgmtImpl::GetClientIaps( const TUid aUid, RArray<TUint>& aIapIdArray )
    {
    DEBUG( "CHssMgmtImpl::GetClientIaps()" );
    TRAPD( err, iServer.GetClientIapsL( aUid, aIapIdArray ) );
    DEBUG1( "CHssMgmtImpl::GetClientIaps() returned with %d", err );
    return err;	    
    }
    
// ================= OTHER EXPORTED FUNCTIONS ==============

// ---------------------------------------------------------
// Returns an instance of the proxy table.
// Returns: KErrNone
// ---------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof( ImplementationTable) / sizeof( TImplementationProxy );
    return ImplementationTable;
    }

//  End of File  
