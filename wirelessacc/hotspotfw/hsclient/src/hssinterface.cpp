/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of interface for HotSpot services.
*
*/



// INCLUDES
#include "hssinterface.h"
#include "hotspotclientserver.h"
#include "am_debug.h"
#include <s32mem.h>
#include <es_enum.h>
#include <commdb.h>

// CONSTS
const TUint32 KHssWlanUserSettings  = 1;
_LIT( KHssWlanDeviceSettings,        "WLANDeviceTable" );
_LIT( KHssWlanDeviceSettingsType,    "WlanDeviceSettingsType" );
_LIT( KHssBgScanInterval,            "WlanBgScanInterval" );

// -----------------------------------------------------------------------------
// RHssInterface::ActivateNotificationsL
// -----------------------------------------------------------------------------
//
EXPORT_C void RHssInterface::ActivateNotificationsL( MHssSrvNotifications& aCallback )
    {
    DEBUG( "RHssInterface::ActivateNotificationsL()" );
    if( iNotify != NULL )
        {
        iNotify->Activate( aCallback );
        }
    else
        {
        iNotify = CHssSrvNotifications::NewL( aCallback, *this );        
        }
    }
 
   

// -----------------------------------------------------------------------------
// RHssInterface::CancelNotifications
// -----------------------------------------------------------------------------
//
EXPORT_C void RHssInterface::CancelNotifications()
    {
    DEBUG( "RHssInterface::CancelNotifications()" );
    if( iNotify == NULL )
        {
        return;
        }
    if( iNotify->IsActive() )
        {
        // Notifications activated, request is pending
        iNotify->Cancel();
        }
    else
        {
        // Notifications activated, currently executing a notification

        //(i.e. in RunL of iNotify)

        iNotify->SetCancelled();
        }
    }

// -----------------------------------------------------------------------------
// RHssInterface::Close
// -----------------------------------------------------------------------------
//
EXPORT_C void RHssInterface::Close()
    {
    DEBUG( "RHssInterface::Close()" );
    if ( iNotify != NULL )
        {
        delete iNotify;    
        }
    iNotify = NULL;
    
    RHandleBase::Close();
    DEBUG( "RHssInterface::Close() Done" );
    }

// -----------------------------------------------------------------------------
// RHssInterface::Connect
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RHssInterface::Connect()
    {
    TInt error = StartServer();
    DEBUG1( "RHssInterface::Connect() StartServer: %d", error ); 
	iNotify = NULL;
		
    if ( KErrNone == error )
        {
        error = CreateSession( KHotSpotDataServerName,
                               Version(),
                               KDefaultMsgSlots );
        DEBUG1( "RHssInterface::Connect() CreateSession: %d", error );  
        }
    return error;
    }

// -----------------------------------------------------------------------------
// RHssInterface::GetScanResults
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RHssInterface::GetScanResults( HssScanList& aResults )
    {
    DEBUG( "RHssInterface::GetScanResults(HssScanList&)" );
    TInt ret( KErrNone );
    CHssScanHandler* scanHandler( NULL );
    TRAP( ret, scanHandler = CHssScanHandler::NewL() );
    if (ret == KErrNone)
        {
        ret = scanHandler->ScanRequest( aResults );
        delete scanHandler;
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// RHssInterface::Register
// -----------------------------------------------------------------------------
//
EXPORT_C TUint RHssInterface::Register (const TUid aUID,
                                        const TDesC& aIapName, 
                                        const TUint aWaitnote)
    {
    DEBUG( "RHssInterface::Register()" );
    TPckgBuf< TClientUid > uidPckg;
    uidPckg().SetUid( aUID );
    TPckgBuf< TIapName > iapPckg;
    iapPckg().SetIapName( aIapName );
    TIpcArgs args( &uidPckg, &iapPckg,  aWaitnote );
    iIapId = SendReceive( EHssRegister, args );
    return iIapId;
    }

// -----------------------------------------------------------------------------
// RHssInterface::UnRegister
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RHssInterface::UnRegister (const TUint aIapID, 
                                         const TUint aNetworkId,
                                         const TUid aUID)
    {
    DEBUG( "RHssInterface::UnRegister()" );
    TRAPD( err, StopConnectionL( aIapID, aNetworkId ) );
    
    if ( err == KErrNone )
        {
        TPckgBuf< TClientUid > uidPckg;
        uidPckg().SetUid( aUID );
        TIpcArgs args( aIapID, &uidPckg );
        err =  SendReceive( EHssUnRegister, args );
        }
    
    return err;
    }

// -----------------------------------------------------------------------------
// RHssInterface::ChangeSettings
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RHssInterface::ChangeSettings ( const TUint aIapID, 
                                              const THssIapSettings& aSettings )
    {
    DEBUG( "RHssInterface::ChangeSettings()" );
    TInt ret( KErrNone );
    CHssIapHandler* iapHandler( NULL );
    TRAP( ret, iapHandler = CHssIapHandler::NewL());
    if ( ret != KErrNone )
        {
        DEBUG1( "RHssInterface::ChangeSettings() leave1: %d", ret );
        }
    else
        {
        TRAPD( err, ret = iapHandler->ChangeSettingsL( aIapID, aSettings ) );
        if ( err != KErrNone )
            {
            DEBUG1( "RHssInterface::ChangeSettings() leave2: %d", err );
            ret = err;
            }
        if ( iapHandler != NULL )
            {
            delete iapHandler;
            }
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// RHssInterface::Stop
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RHssInterface::Stop ( const TUint aIapID )
    {
    DEBUG( "RHssInterface::Stop()" );
    
    TIpcArgs args( aIapID );
    TInt ret = Send( EHssStop, args );
    
    TUint32 netId;
    CHssIapHandler* iapHandler(NULL);
    
    TRAPD( err, iapHandler = CHssIapHandler::NewL() );
    if (err == KErrNone)
        {
        TRAP( err, iapHandler->GetNetworkIdL( aIapID, netId ) );
        delete iapHandler;
        if (err == KErrNone)
            {
            TRAP( err, StopConnectionL( aIapID, netId ) );
            // Possible error value is neglected?!
            }
        }    

    return ret;
    }

// -----------------------------------------------------------------------------
// RHssInterface::CancelStart
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RHssInterface::CancelStart ( const TUint aIapID )
    {
    DEBUG( "RHssInterface::CancelStart()" );
    TIpcArgs args( aIapID );
    return Send( EHssCancelStart, args );
    }

// -----------------------------------------------------------------------------
// RHssInterface::LoginComplete
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RHssInterface::LoginComplete ( const TUint aIapID, const TInt aStatus )
    {
    DEBUG( "RHssInterface::LoginComplete()" );
    iArgs = TIpcArgs( aIapID, aStatus );
    return Send( EHssLoginComplete, iArgs );
    }
    
// -----------------------------------------------------------------------------
// RHssInterface::LogoutComplete
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RHssInterface::LogoutComplete ( const TUint aIapID )
    {
    DEBUG( "RHssInterface::LogoutComplete()" );
    TIpcArgs args( aIapID );
    return Send( EHssLogoutComplete, args );
    }

// -----------------------------------------------------------------------------
// RHssInterface::Join
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RHssInterface::Join( const TUint aIapId )
    {
    DEBUG( "RHssInterface::Join()" );
    TIpcArgs args( aIapId );
    return Send( EHssJoin, args );
    }


// -----------------------------------------------------------------------------
// RHssInterface::StartLogin
// -----------------------------------------------------------------------------
//
EXPORT_C void RHssInterface::StartLogin( const TUint aIapId, 
                                         const TUint aNetworkId,
                                         TRequestStatus& aStatus )
    {
    DEBUG( "RHssInterface::StartLogin()" );
    iArgs = TIpcArgs( aIapId, aNetworkId );
    SendReceive( EHssStartLogin, iArgs, aStatus );
    }

// -----------------------------------------------------------------------------
// RHssInterface::CancelLogin
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RHssInterface::CancelLogin()
    {
    DEBUG( "RHssInterface::CancelLogin()" );
    return Send( EHssCancelLogin );
    }  

// -----------------------------------------------------------------------------
// RHssInterface::Start
// -----------------------------------------------------------------------------
//
EXPORT_C void RHssInterface::Start( const TUint aIapId, 
                                    TRequestStatus& aStatus )
    {
    DEBUG( "RHssInterface::Start()" );
    iArgs = TIpcArgs( aIapId );
    SendReceive( EHssStart, iArgs, aStatus );
    }

// -----------------------------------------------------------------------------
// RHssInterface::StartAgain
// -----------------------------------------------------------------------------
//
EXPORT_C void RHssInterface::StartAgain( const TUint aIapId, 
                                         TRequestStatus& aStatus )
    {
    DEBUG( "RHssInterface::StartAgain()" );
    iArgs = TIpcArgs( aIapId );
    SendReceive( EHssStartAgain, iArgs, aStatus );
    }

// -----------------------------------------------------------------------------
// RHssInterface::CloseConnection
// -----------------------------------------------------------------------------
//
EXPORT_C void RHssInterface::CloseConnection( const TUint aIapId,
                                              TRequestStatus& aStatus )
    {
    DEBUG( "RHssInterface::CloseConnection()" );
    iArgs = TIpcArgs( aIapId );
    SendReceive( EHssCloseConnection, iArgs, aStatus );
    }    

// -----------------------------------------------------------------------------
// RHssInterface::Cancel
// -----------------------------------------------------------------------------
//
EXPORT_C void RHssInterface::Cancel( const TUint aIapId )
    {
    DEBUG( "RHssInterface::Cancel()" );
    TIpcArgs args( aIapId );
    Send( EHssCancel, args );
    }

// -----------------------------------------------------------------------------
// RHssInterface::ShutdownServerL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RHssInterface::ShutdownServerL()
    {
    DEBUG( "RHssInterface::ShutdownServerL" );
    TInt ret( KErrNone );
    TInt iapId;
    TInt netId;
    TPckgBuf<TInt> iapPckg;
    TPckgBuf<TInt> netPckg;
    TIpcArgs args( &iapPckg, &netPckg );
    ret = SendReceive( EHssServerShutdown, args );
    
    if ( ret == KErrInUse )
        {
        iapId = iapPckg();
        netId = netPckg();
        DEBUG1("iapID RHSSINTERFACE : %d", iapId );
        ret = StopConnectionL( iapId, netId );
        
        if ( ret == KErrNone )
            {
            ret =  SendReceive( EHssServerShutdown, args );
            }
        }
    
    TIpcArgs args2( KHssShutdown ); 
    ret =  SendReceive( EHssServerShutdown, args2 );
        
    return ret;
    }

// -----------------------------------------------------------------------------
// RHssInterface::SetUiState
// -----------------------------------------------------------------------------
//
EXPORT_C void RHssInterface::SetUiState( const TUint aIapId,
                                         const TUint aState )
    {
    DEBUG( "RHssInterface::SetUiState()" );
    TIpcArgs args( aIapId, aState );
    SendReceive( EHssUiState, args );	
	}
    
// -----------------------------------------------------------------------------
// RHssInterface::Version
// Needed only when activating the server.
// -----------------------------------------------------------------------------
//
TVersion RHssInterface::Version() const
    {
    return( TVersion(
            KHotSpotMajorVersionNumber,
            KHotSpotMinorVersionNumber,
            KHotSpotBuildVersionNumber ) );
    }

// -----------------------------------------------------------------------------
// RHssInterface::StartServer()
// Starts the server if it is not already running
// -----------------------------------------------------------------------------
//
TInt RHssInterface::StartServer()
    {
    TInt res( KErrNone );
    // create server - if one of this name does not already exist
    TFindServer findServer( KHotSpotDataServerName );
    TFullName name;

    if ( findServer.Next( name ) != KErrNone ) // we don't exist already
        {
        RProcess server;
        // Create the server process
        res = server.Create( KHotSpotServerExe, KNullDesC );

        if ( res != KErrNone ) // thread created ok - now start it going
            {
            return res;
            }

        // Rendezvous is used to detect server start
        TRequestStatus status;
        server.Rendezvous( status );                                                 
                                                                                   
        if ( status != KRequestPending )                                             
            {
            DEBUG1( "StartServer Rendezvous ERROR: %d", status.Int() );
            server.Kill( 0 );    // abort startup                                 
            }                                                                      
        else                                                                       
            {                                                                      
            server.Resume();    // logon OK - start the server                     
            }                                                                      
        
        DEBUG( "StartServer wait for start" );
        User::WaitForRequest( status ); // wait for start or death

        // We can't use the 'exit reason' if the server paniced as this                                                          
        // is the panic 'reason' and may be '0' which cannot be distinguished                                        
        // from KErrNone                                                                                             
        res = ( server.ExitType() == EExitPanic ) ? KErrGeneral : status.Int();
        
        // we're no longer interested in the other process
        server.Close(); 
        }
        
    return res;

    }

// ---------------------------------------------------------
// RHssInterface::WaitForNotification
// When the message is completed at the server side, the
// CHssSrvNotifications::RunL() is been called. This is used by the
// notification service.
// ---------------------------------------------------------
//
void RHssInterface::WaitForNotification(
    TRequestStatus& aReturnValue, 
    TDes8& aReturnData )
    {
    DEBUG( "RHssInterface::WaitForNotification()" );
    TIpcArgs params( &aReturnData );
    SendReceive( EHssActivateNotifications, params, aReturnValue );
    }
    
// ---------------------------------------------------------
// RHssInterface::CancelWaitForNotification
// CHssSrvNotifications class uses this method to cancel pending 
// message at server side.
// ---------------------------------------------------------
//
void RHssInterface::CancelWaitForNotification()
    {
    DEBUG( "RHssInterface::CancelWaitForNotification()" );

    // Check is notify service activated.
    if( iNotify != NULL )
        {
        // Send synchronous request to the data server
        DEBUG( "RHssInterface::CancelWaitForNotification() Cancelling..." );
        Send( EHssCancelNotifications, TIpcArgs() );
        DEBUG( "Done!" );
        }
    else
        {
        DEBUG( "RHssInterface::CancelWaitForNotification() Notification service was not started." );
        }
    }

// ---------------------------------------------------------
// RHssInterface::StopConnectionL
// ---------------------------------------------------------
//
TInt RHssInterface::StopConnectionL( const TUint aIapId, const TUint aNetworkId )
    {
    DEBUG( "RHssInterface::StopConnectionL()" );

    TInt ret( KErrNone );
    RSocketServ socketServ;
    RConnection conn;

    User::LeaveIfError( socketServ.Connect() );
    CleanupClosePushL( socketServ );
    
    // Open connection
    User::LeaveIfError( conn.Open(socketServ) );
    CleanupClosePushL( conn );

    TConnectionInfo info;
    info.iIapId = aIapId;
    info.iNetId = aNetworkId;

    TInt err = conn.Attach(
            TPckg< TConnectionInfo >( info ),
            RConnection::EAttachTypeNormal );
    DEBUG1( "RHssInterface::conn.Attach: %d", err );

    if ( err == KErrNone )
        {
        ret = conn.Stop( RConnection::EStopAuthoritative );
        DEBUG1( "RHssInterface::connection.Stop: %d", ret );
        }

    /* Note: In previous version, tried to PopAndDestroy conn (RConnection)
     * from cleanup stack...
     * => Somehow this broke Sniffer connection opening, no idea why.
     */
    CleanupStack::Pop( &conn );
    conn.Close();
    CleanupStack::PopAndDestroy( &socketServ );
    
    return ret;
    }

// ---------------------------------------------------------
// RHssInterface::CheckBackgroundScanL
// ---------------------------------------------------------
//
EXPORT_C TUint32 RHssInterface::CheckBackgroundScanL()
	{
	DEBUG( "RHssInterface::CheckBackgroundScanL()" );
    CCommsDatabase*    commDB = NULL;
    CCommsDbTableView* table  = NULL;

    // Open commDB
    commDB = CCommsDatabase::NewL();
    CleanupStack::PushL( commDB );

    table = commDB->OpenViewMatchingUintLC( KHssWlanDeviceSettings,
	                                        KHssWlanDeviceSettingsType,
	                                        KHssWlanUserSettings );

    TInt err = table->GotoFirstRecord();

    if ( err )
        {
        User::Leave( err );  
        }
	
	TUint32 scanInterval;
    table->ReadUintL( KHssBgScanInterval, scanInterval ); 

    // cleanup
    CleanupStack::PopAndDestroy( table );
    CleanupStack::PopAndDestroy( commDB );
    
    return (scanInterval);
	
	}

// ---------------------------------------------------------
// RHssInterface::GetIap
// ---------------------------------------------------------
//
EXPORT_C TUint32 RHssInterface::GetIap( TIpcArgs aArgs )
	{
	DEBUG( "RHssInterface::GetIap()" );
	return SendReceive ( EHssGetIAP, aArgs );	
	}

// ---------------------------------------------------------
// RHssInterface::StartBrowser
// ---------------------------------------------------------
//
EXPORT_C void RHssInterface::StartBrowser( const TDesC& aString, 
										   const TUint aIapId,
										   const TUint aNetId,
										   TRequestStatus& aStatus )
	{
	DEBUG( "RHssInterface::StartBrowser()" );

	iArgs = TIpcArgs( &aString, aIapId, aNetId ) ;
	SendReceive( EHssStartBrowser, iArgs, aStatus );
	}

// ---------------------------------------------------------
// RHssInterface::GetConnectionBssid
// ---------------------------------------------------------
//
EXPORT_C TInt RHssInterface::GetConnectionBssid( THssBssid& aBssId )
    {
    DEBUG( "RHssInterface::GetConnectionBssid()" );
    TInt ret( KErrNone );
    CHssScanHandler* scanHandler( NULL );
    TRAP( ret, scanHandler = CHssScanHandler::NewL() );
    if (KErrNone != ret )
        {
        DEBUG1( "RHssInterface::GetConnectionBssid() err:", ret );
        }
    else
        {
        ret = scanHandler->GetConnectionBssid( aBssId );
        DEBUG1( "RHssInterface::GetConnectionBssid() ret:", ret );
        delete scanHandler;
        }
    return ret;
    }

// ---------------------------------------------------------
// RHssInterface::AddIapSsidList
// ---------------------------------------------------------
//
EXPORT_C TInt RHssInterface::AddIapSsidList(
        TUint aIapId,
        const CArrayFixFlat<THssSsid>& aSsidList )
    {
    DEBUG( "RHssInterface::AddIapSsidList()" );
    TInt ret( KErrNone );
    CHssScanHandler* scanHandler( NULL );
    TRAP( ret, scanHandler = CHssScanHandler::NewL() );
    if (KErrNone != ret )
        {
        DEBUG1( "RHssInterface::AddIapSsidList() err:", ret );
        }
    else
        {    
        ret = scanHandler->AddIapSsidList( aIapId, aSsidList );
        DEBUG1( "RHssInterface::AddIapSsidList() ret:", ret );
        delete scanHandler;    
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// RHssInterface::SetTimerValues
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RHssInterface::SetTimerValues (
        TUid aClientUid,
        TUint aLoginTimerValue,
        TUint aLogoutTimerValue )
    {
    DEBUG( "RHssInterface::SetTimerValues()" );
    TIpcArgs args( aClientUid.iUid, aLoginTimerValue, aLogoutTimerValue );
    return Send( EHssSetTimerValues, args );
    }

// ---------------------------------------------------------
// RHssInterface::GetClientIapsL
// ---------------------------------------------------------
//
EXPORT_C void RHssInterface::GetClientIapsL( const TUid aUid, RArray<TUint>& aIapIdArray )
    {
    DEBUG( "RHssInterface::GetClientIapsL()" );
    CHssIapHandler* iapHandler( NULL );
    iapHandler = CHssIapHandler::NewL();
    CleanupStack::PushL( iapHandler );
    iapHandler->GetClientIapsL( aUid, aIapIdArray );
    CleanupStack::PopAndDestroy( iapHandler );
    }

// End of File

