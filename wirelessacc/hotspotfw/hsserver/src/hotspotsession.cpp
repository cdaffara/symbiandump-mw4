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
* Description:   Implementation of HotSpot Server Session
*
*/



// INCLUDE FILES
#include "hotspotserver.h"
#include "hotspotsession.h"
#include "hssnotif.h"
#include "hsslogintimer.h"
#include "hsslogouttimer.h"
#include "hssclientinterface.h"
#include "am_debug.h"
#include <WlanCdbCols.h>
#include <starterclient.h>
#include <e32std.h>
#include <ecom.h>
#include <f32file.h>

// Forward declarations
class CWlanMgmtClient;
class MWlanMgmtNotifications;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHotSpotPluginSession
// -----------------------------------------------------------------------------
//
CHotSpotSession::CHotSpotSession( CHotSpotServer& aServer ) :
    iServer( aServer ), iClient( NULL ), iSrvNotifications ( NULL ), 
    iNotificationHandle( NULL ), iAllowNotifications( ETrue ), iHotspotExtension( ETrue )
    {
    iIapSettingsHandler=NULL;
    iNotifications=NULL;
    iLoginTimer=NULL;
    iLogoutTimer=NULL;
    iMgtClient=NULL;
    iIapId=0;
    iNetId=0;
    iNotificationHandle=NULL;
    iIsNotificationRequestPending =false;
    iClientUid.iUid =0;

    }

// -----------------------------------------------------------------------------
// ConstructL
// -----------------------------------------------------------------------------
//
void CHotSpotSession::ConstructL()
    {
    DEBUG( "CHotSpotSession::ConstructL()" );
    iIapSettingsHandler = CHssIapSettingsHandler::NewL();
    iLoginTimer = CHssLoginTimer::NewL( *this );
    iLogoutTimer = CHssLogoutTimer::NewL( *this );
    
    iMgtClient = CWlanMgmtClient::NewL();
    }

// -----------------------------------------------------------------------------
// NewL
// -----------------------------------------------------------------------------
//
CHotSpotSession* CHotSpotSession::NewL( CHotSpotServer& aServer )
    {
    CHotSpotSession* self = new( ELeave ) CHotSpotSession( aServer );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self ); // Should this class have a 2-way constructor?
    return self;
    }
    
// ----------------------------------------------------------------------------------------
// ~CHotSpotSession
// ----------------------------------------------------------------------------------------
//
CHotSpotSession::~CHotSpotSession()
    {
    DEBUG( "CHotSpotSession::~CHotSpotSession()" );
    iPendingNotifications.Close();
    if ( iMgtClient != NULL )
        {
        iMgtClient->CancelNotifications();
        delete iMgtClient;
        }
        
    if ( iIapSettingsHandler != NULL )
        {
        delete iIapSettingsHandler;
        }
    
    if ( iClient != NULL )
        {
        delete iClient;
        }
    
    if ( iNotifications != NULL )
        {
        delete iNotifications;
        }
    
    if ( iLoginTimer != NULL )
        {
        iLoginTimer->Cancel();
        delete iLoginTimer;
        }
    
    if ( iLogoutTimer != NULL )
        {
        iLogoutTimer->Cancel();
        delete iLogoutTimer;
        }

    DEBUG( "CHotSpotSession::~CHotSpotSession() Done" );
    }
    
// ----------------------------------------------------------------------------------------
// ServiceL
// ----------------------------------------------------------------------------------------
//
void CHotSpotSession::ServiceL( const RMessage2& aMessage )
    {
    DEBUG1( "CHotSpotSession::ServiceL message: %d", aMessage.Function() );
    TRAPD( err, DispatchMessageL( aMessage) );
    if (err != KErrNone)
        {
        // Something went wrong. Complete message to let
        // the client to continue
        aMessage.Complete(err);
        } 
    }
// ----------------------------------------------------------------------------------------
// ServiceL
// ----------------------------------------------------------------------------------------
//
void CHotSpotSession::DispatchMessageL( const RMessage2& aMessage )
    {
    DEBUG( "CHotSpotSession::DispatchMessageL()" );
    TInt value( NULL );
    TInt err( KErrNone );
    TInt indx( KErrNone );
    TPckgBuf<TInt> iapPackage( iIapId );
    
    switch ( aMessage.Function() )
        {
        case EHssActivateNotifications :
            DEBUG( "CHotSpotSession::ActivateNotificationsL" );
            iAllowNotifications = EFalse;
            
            if ( iNotifications == NULL )
                {
                iNotifications = new ( ELeave ) HssNotifications( *this );
                iMgtClient->ActivateNotificationsL( *iNotifications );
                }
            HandleOrderNotifications( aMessage );
            break;
        case EHssCancelNotifications :
            DEBUG( "CHotSpotSession::CancelNotifications" );
            iAllowNotifications = EFalse;
            iMgtClient->CancelNotifications( );
        
            if ( iNotifications != NULL )
                {
                delete iNotifications;
                }
            iNotifications = NULL;
            HandleCancelNotifications( aMessage );
            break;
        case EHssRegister :
            ProcessRegisterL( aMessage );
            break;
        case EHssUnRegister :
            ProcessUnRegister( aMessage );
            break;
        case EHssJoin :
            iAllowNotifications = EFalse;
            iIapId = ( TInt )aMessage.Int0();
     
            indx = iServer.FindMessage( iIapId, EHssStart );
            if ( indx >= 0 )
                {
                iServer.CompleteMessage( indx , KErrNone );
                }
            else
                {
                indx = iServer.FindMessage( iIapId, EHssStartAgain );
                if ( indx >= 0 )
                    {
                    iServer.CompleteMessage( indx , KErrNone );
                    }
                }
            aMessage.Complete( KErrNone );
            break;
        case EHssCancelStart :
            iServer.SetLogoutFlag( ETrue );
            // Do not send association status to client
            iServer.SetAssociationFlag( EFalse ); 
            iIapId = ( TInt )aMessage.Int0();
    
            indx = iServer.FindMessage(iIapId, EHssStart );
            if ( indx >= 0 )
                {
                iServer.CompleteMessage( indx , KErrAbort);
                }
            else
                {
                indx = iServer.FindMessage(iIapId, EHssStartAgain );
                if ( indx >= 0 )
                    {
                    iServer.CompleteMessage( indx , KErrAbort );
                    }
                }
            aMessage.Complete( KErrNone );
            break;
        case EHssStop :
            iAllowNotifications = EFalse;
            iServer.SetLogoutFlag( ETrue );
            iLoginTimer->Cancel();
            iLogoutTimer->Cancel();
            iIapId = ( TInt )aMessage.Int0();
    
            for ( TInt counter = EHssGetScanResults; 
                  counter <EHssServerShutdown ;counter++ )
                {
                indx = iServer.FindMessage( iIapId, 
                                            THotSpotCommands( counter ) );
                if ( indx >= 0 )
                    {
                    iServer.CompleteMessage( indx , KErrCancel );
                    }
                }
            aMessage.Complete( KErrNone );
            break;
        case EHssLoginComplete :
            iAllowNotifications = EFalse;
            iLoginTimer->Cancel();
            iIapId = ( TInt )aMessage.Int0();
            value = ( TInt )aMessage.Int1();
            
            DEBUG1( "EHssLoginComplete return value: %d", value );
            indx = iServer.FindMessage( iIapId, EHssStartLogin );
            if ( KErrNotFound != indx )
                {
                if ( value == KErrNone )
                    {
                    DEBUG( "EHssLoginComplete1" );
                    iServer.CompleteMessage( indx, KErrNone );
                    }
                else
                    {
                    DEBUG( "EHssLoginComplete2" );
                    iServer.CompleteMessage( indx, KErrCancel );
                    iServer.SetLogoutFlag( ETrue );
                    }
                }
            
            aMessage.Complete( KErrNone );
            break;
        case EHssLogoutComplete :
            iAllowNotifications = EFalse;
            iLogoutTimer->Cancel();
            iIapId = ( TInt )aMessage.Int0();
            
            indx = iServer.FindMessage( iIapId, EHssCloseConnection );
            if ( KErrNotFound != indx )
                {
                iServer.CompleteMessage( indx, KErrNone );    
                }
            
            aMessage.Complete( KErrNone );
            break;
        case EHssStartLogin :
            // Do not send association status, since it's already done.
            iServer.SetAssociationFlag( EFalse ); 
            iIapId = ( TInt )aMessage.Int0();
            iNetId = ( TInt )aMessage.Int1();
     
            err = iServer.SaveMessage( iIapId, aMessage, EHssStartLogin ); 
            if ( KErrNone != err )
                {
                aMessage.Complete( err );  
                }
            else
                {
                err = ProcessStartLogin( iIapId, iNetId );
                // If client not found, an error was returned. 
                // Otherwise message completed elsewhere. 
                if ( KErrNone != err )
                    {
                    indx = iServer.FindMessage(iIapId, EHssStartLogin );
                    if ( indx >= 0 )
                        {
                        iServer.CompleteMessage( indx , KErrNone );
                        }
                    else
                        {
                        aMessage.Complete( KErrNone );
                        }
                    }
                }
            break;
        case EHssCancelLogin :
            iLoginTimer->Cancel();
            // if client doesn't exist (is NULL), Login(.) has not been 
            // called to client -> that is CancelLogin() not needed to call
            if ( iClient != NULL )
                {
                iClient->CancelLogin( iIapId );
                }
            
            indx = iServer.FindMessage(iIapId, EHssStartLogin );
            if ( indx >= 0 )
                {
                iServer.CompleteMessage( indx , err );
                }
            aMessage.Complete( KErrNone );
            break;
        case EHssStart:
            iServer.SetLoginFlag( ETrue );
            iIapId = ( TInt )aMessage.Int0();
            err = iServer.SaveMessage( iIapId, aMessage, EHssStart ); 
    
            if ( err != KErrNone)
                {
                aMessage.Complete( err );  
                }
            else
                {
                TRAPD( startLeaved, err = ProcessStartL( iIapId ) );
                if ( startLeaved != KErrNone)
                    {
                    indx = iServer.FindMessage(iIapId, EHssStart );
                    if ( indx >= 0 )
                        {
                        iServer.CompleteMessage( indx , KErrNotSupported );
                        }
                    else
                        {
                        aMessage.Complete( KErrNotSupported );
                        }
                    }
                else if ( err != KErrNone)
                    {
                    indx = iServer.FindMessage(iIapId, EHssStart );
                    if ( indx >= 0 )
                        {
                        iServer.CompleteMessage( indx , KErrNotSupported );
                        }
                    else
                        {
                        aMessage.Complete( KErrNotSupported );
                        }
                    }
                }
            break;
        case EHssStartAgain:
            iServer.SetLoginFlag( ETrue );
            iIapId = ( TInt )aMessage.Int0();
            err = iServer.SaveMessage( iIapId, aMessage, EHssStartAgain ); 
            
            if ( err != KErrNone)
                {
                aMessage.Complete( err );  
                }
            else
                {
                err = ProcessStartAgain( iIapId );
                if (   err != KErrNone)
                    {
                    indx = iServer.FindMessage(iIapId, EHssStartAgain );
                    if ( indx >= 0 )
                        {
                        iServer.CompleteMessage( indx , err );
                        }
                    else
                        {
                        aMessage.Complete( err );
                        }
                    }
                }
            break;
        case EHssCancel:
            iLoginTimer->Cancel();
            iLogoutTimer->Cancel();
            iIapId = ( TInt )aMessage.Int0();
            
            if ( iServer.GetAssociationFlagValue() )
                {
                // We are in association phase and Agent failed it for some reason
                ProcessAssociationStatus( iIapId, EFalse );
                }
            
            indx = iServer.FindMessage(iIapId, EHssStart );
            if ( indx >= 0 )
                {
                iServer.CompleteMessage( indx , KErrCancel );
                }
            indx = iServer.FindMessage(iIapId, EHssStartAgain );
            if ( indx >= 0 )
                {
                iServer.CompleteMessage( indx , KErrCancel );
                }
            indx = iServer.FindMessage(iIapId, EHssCloseConnection );
            if ( indx >= 0 )
                {
                iServer.CompleteMessage( indx , KErrCancel );
                }
            aMessage.Complete( KErrNone );
            break;
        case EHssCloseConnection:
            iLoginTimer->Cancel();
            iLogoutTimer->Cancel();           
            iIapId = ( TInt )aMessage.Int0();

            if ( iServer.GetAssociationFlagValue() )
                {
                // We are in association phase and Agent failed it for some reason
                ProcessAssociationStatus( iIapId, EFalse );
                }
            
            err = iServer.SaveMessage( iIapId, aMessage, EHssCloseConnection ); 
            if ( KErrNone != err )
                {
                aMessage.Complete( err );  
                }
            else
                {
                err = ProcessCloseL( iIapId );
                // If client not found, an error was returned. 
                // Otherwise message completed elsewhere. 
                if ( KErrNone != err )
                    {
                    indx = iServer.FindMessage(iIapId, EHssCloseConnection );
                    if ( indx >= 0 )
                        {
                        iServer.CompleteMessage( indx , KErrNone );
                        }
                    else
                        {
                        aMessage.Complete( KErrNone );
                        }
                    }
                }
            break;
        case EHssServerShutdown:
            ProcessServerShutdown( aMessage );
            break;
        case EHssGetIAP:
            aMessage.WriteL( 0, iapPackage );
            aMessage.Complete( KErrNone );
            break;
        case EHssUiState:
            aMessage.Complete( KErrNotSupported );
            break;
        case EHssStartBrowser:
            {
            aMessage.Complete( KErrNotSupported );
            break;
            }
        case EHssSetTimerValues:
            {
            TUid clientUid( TUid::Uid( aMessage.Int0() ) );
            TBuf<KUidLength> uid;
            uid.Copy( clientUid.Name() );
            ModifyClientUid( uid );
            TUint loginTimerValue = aMessage.Int1();
            TUint logoutTimerValue = aMessage.Int2();
            iServer.SetTimerValuesL( uid, loginTimerValue, logoutTimerValue );
            aMessage.Complete( KErrNone );
            break;
            }
        default:
            aMessage.Complete( KErrNotSupported );  
            break;
        }
       iAllowNotifications = TRUE;
    }

// ---------------------------------------------------------
// HandleOrderNotifications
// ---------------------------------------------------------
//
void CHotSpotSession::HandleOrderNotifications( const RMessage2& aMessage )
    {
    iPendingNotificationRequest = aMessage;
    iIsNotificationRequestPending = ETrue;
    if( !iNotificationHandle ) // == NULL i.e. is not activated
        {
        TRAPD( err, iNotificationHandle = CSessionNotification::NewL( *this ) );
        if ( err == KErrNone )
            {
            iServer.NotifyAdd( *iNotificationHandle );
            }
        else
            {
            iIsNotificationRequestPending = EFalse;
            aMessage.Complete( err );
            return;
            }
        }
    HandleNotification(); // check is there any unsent notifications
    }

// ---------------------------------------------------------
// HandleCancelNotifications
// ---------------------------------------------------------
//
void CHotSpotSession::HandleCancelNotifications( const RMessage2& aMessage )
    {
    if( iIsNotificationRequestPending )
        {
        iPendingNotificationRequest.Complete( KErrNone /*EWLMNotificationsCancelled*/ );
        iIsNotificationRequestPending = EFalse;
        iPendingNotifications.Reset();
        }
    if( iNotificationHandle )
        {
        iServer.NotifyRemove( *iNotificationHandle );
        delete iNotificationHandle;
        iNotificationHandle = NULL;
        }
    aMessage.Complete( KErrNone );
    }
        
// ---------------------------------------------------------
// AddNotification
// ---------------------------------------------------------
//
void CHotSpotSession::AddNotification( TInt aNotification, TDes8& aData )
    {
    DEBUG( "CHotSpotSession::AddNotification" );

    TNotification notif;
    notif.id = aNotification;
    notif.data = aData;
    if ( iIsNotificationRequestPending )
        {
        DEBUG( "CHotSpotSession::AddNotification added to array. Request found..." );
        iPendingNotifications.Append( notif );
        HandleNotification(); // check is there client waiting for notification
        }
    }

// ---------------------------------------------------------
// HandleNotification
// ---------------------------------------------------------
//
void CHotSpotSession::HandleNotification()
    {
    DEBUG( "CHotSpotSession::HandleNotification" );

    // Check if we allow notifications
    if (iAllowNotifications == TRUE)
        {
        // Check is there message to wait notification and 
        // notification that is not sent.
        if ( iIsNotificationRequestPending && iPendingNotifications.Count() != 0 )
            {
            DEBUG( "CHotSpotSession::HandleNotification - sending response..." );
            iIsNotificationRequestPending = EFalse;

            THssPckgData data;
            TPckg<THssPckgData> pckg( data );
            data.data = iPendingNotifications[0].data;
            TInt ret( iPendingNotificationRequest.Write( 0, pckg ) );
            if ( ret != KErrNone )
                {
                iPendingNotificationRequest.Complete( ret );
                return;
                }
            iPendingNotificationRequest.Complete( iPendingNotifications[0].id );
            iPendingNotifications.Reset();
            }
        }
    }

// -----------------------------------------------------------------------------
// LoginTimeout
// -----------------------------------------------------------------------------
//   
void CHotSpotSession::LoginTimeout()
    {
    DEBUG1("CHotSpotSession::LoginTimeout iIapId :%d", iIapId );
    iServer.SetLogoutFlag( ETrue );
    
    // if message is not found, client has not completed it (LoginComplete(..))
    TInt indx = iServer.FindMessage( iIapId, EHssStartLogin );
    if ( KErrNotFound != indx )
        {
        iServer.CompleteMessage( indx, KErrCancel );    
        }
        
    TBuf8<KExtensionAPILength> extAPI;
    TBuf<KUidLength> clientUid;
    TInt err = iServer.GetClientUid( iIapId, clientUid );
        
    if ( err != KErrNotFound )
        {
        DEBUG("CHotSpotSession::LoginTimeout clientUid = CLIENT");
        ConvertTBufToTUid( clientUid ); 
        TBuf8<KExtensionAPILength> nullBuf;
        
        TInt ret = CreateClient( iClientUid, nullBuf );
        DEBUG1("CHotSpotSession::LoginTimeout CreateClient ret: %d", ret);
        if ( KErrNone == ret )
            {
            iClient->CancelLogin( iIapId );           
            }
        }

    DEBUG("CHotSpotSession::LoginTimeout Done");
    }
    
// -----------------------------------------------------------------------------
// LogoutTimeout
// -----------------------------------------------------------------------------
//   
void CHotSpotSession::LogoutTimeout()
    {
    DEBUG("CHotSpotSession::LogoutTimeout");
    // if message is not found, client has not completed it (LogoutComplete(..))
    TInt indx = iServer.FindMessage( iIapId, EHssCloseConnection );
    if ( KErrNotFound != indx )
        {
        iServer.CompleteMessage( indx, KErrNone );    
        }
    }

// ---------------------------------------------------------
// ProcessRegister
// ---------------------------------------------------------
//
void CHotSpotSession::ProcessRegisterL( const RMessage2& aMessage )
    {
    DEBUG("CHotSpotSession::ProcessRegisterL");
    
    iAllowNotifications = EFalse;
    TBufC< KIapNameLength > iapName;
    TPckgBuf< TIapName > iapPckg;
    TUid clientUid;
    TPckgBuf< TClientUid > uidPckg;
    TPckgBuf<TInt> iapPackage( iIapId );
    
    // Read message
    aMessage.ReadL( 0, uidPckg );
    clientUid = uidPckg().ClientUid();
    aMessage.ReadL( 1, iapPckg );
    iapName = iapPckg().IapName();
    
    TBuf<KIapNameLength> bufUid;
    bufUid.Copy( clientUid.Name() );
    ModifyClientUid( bufUid );
        
    TUint32 iapId( 0 );
    
    TInt ret( KErrNone );
    TRAP( ret, iIapSettingsHandler->CreateClientIapL( iapName, iapId, bufUid ));
    DEBUG1( "CHotSpotSession::EHssRegister iapId: %d", iapId );
    DEBUG1( "CHotSpotSession::EHssRegister ret: %d", ret );
    if ( KErrNone == ret )
        {
        iServer.SetClientIapL( iapId, bufUid );
        aMessage.Complete( iapId );
        }

    else 
        {
        // Error, we are returning 0 to client
        // and no IAP is created
        aMessage.Complete( KErrNone );
        }
    }

// ---------------------------------------------------------
// ProcessUnRegister
// ---------------------------------------------------------
//
void CHotSpotSession::ProcessUnRegister( const RMessage2& aMessage )
    {
    DEBUG("CHotSpotSession::ProcessUnRegister");
    iAllowNotifications = EFalse;
    TInt ret( KErrNone );

    // Read message
    TInt iapId = ( TInt )aMessage.Int0();
    iServer.RemoveClientIap( iapId );
   
    TRAPD( err, iIapSettingsHandler->DeleteIapL( iapId ) );
    // return KErrGeneral if IAP removal is not successful
    if ( err != KErrNone )
        {
        ret = KErrGeneral;
        }
    aMessage.Complete( ret ); 
    DEBUG("CHotSpotSession::ProcessUnRegisterL DONE");
    }

// -----------------------------------------------------------------------------
// ProcessStartLogin
// -----------------------------------------------------------------------------
//   
TInt CHotSpotSession::ProcessStartLogin( const TUint aIapId, const TUint aNetId )
    {
    DEBUG("CHotSpotSession::ProcessStartLogin");
    TInt ret( KErrNotFound );
    TBuf8<KExtensionAPILength> extAPI;
    iIapId = aIapId;
    
    TBuf<KUidLength> clientUid;
    TInt err = iServer.GetClientUid( aIapId, clientUid );
    
    if ( err != KErrNotFound )
        {
        DEBUG("CHotSpotSession::ProcessStartLogin clientUid = CLIENT");
        ConvertTBufToTUid( clientUid ); 
        TBuf8<KExtensionAPILength> nullBuf;
        
        ret = CreateClient( iClientUid, nullBuf );
        
        if ( KErrNone == ret && iServer.GetLoginFlagValue() )
            {
            iLoginTimer->After( iServer.GetLoginTimeMicroSecs( clientUid ) );
            DEBUG("CHotSpotSession::ProcessStartLogin iClient->Login( iIapId, iNetId );");
            iClient->Login( aIapId, aNetId );
            iServer.SetLogoutFlag( EFalse );           
            }
        else
            {
            ret = KErrNotFound;
            }
        }
    DEBUG("CHotSpotSession::ProcessStartLogin DONE");
    return ret;
    }

// -----------------------------------------------------------------------------
// ProcessStartL
// -----------------------------------------------------------------------------
//   
TInt CHotSpotSession::ProcessStartL( const TUint aIapId )
    {
    DEBUG("CHotSpotSession::ProcessStart");
    
    TInt ret( KErrNone );
    TBuf8<KExtensionAPILength> extAPI;
    TBuf<KUidLength> clientUid;
    
    TInt err = iServer.GetClientUid( aIapId, clientUid );
    if ( err != KErrNone )
        {
        DEBUG("CHotSpotSession::ProcessStartL clientUid = EMPTY");
        
        ret = KErrNotSupported;
        }
    else
        {
        DEBUG("CHotSpotSession::ProcessStartL clientUid = CLIENT");
        ConvertTBufToTUid( clientUid ); 
        TBuf8<KExtensionAPILength> nullBuf;
        
        // Try first with API extension defined
        ret = CreateClient( iClientUid, extAPI );
        
        if ( ret != KErrNone )
            {
            // Client exists, but hasn't implemented API extension
            // Let's try again without API extension definition
            TBuf8<KExtensionAPILength> nullBuf;
            ret = CreateClient( iClientUid, nullBuf );
            }
        
        if ( ret == KErrNone )
            {
            iServer.SetAssociationFlag( ETrue ); 
            iMgtClient->ActivateNotificationsL( *this );
            iClient->Start( aIapId );    
            }
        }
    DEBUG1("CHotSpotSession::ProcessStartL DONE ret%d", ret);
    return ret;
    }
    
// -----------------------------------------------------------------------------
// ProcessStartAgain
// -----------------------------------------------------------------------------
//   
TInt CHotSpotSession::ProcessStartAgain( const TUint aIapId )
    {
    DEBUG("CHotSpotSession::ProcessStartAgain");
    TInt ret( KErrNone );
      
    // Client exists if StartAgain is called.
    if ( iClient == NULL )
        {    
        TBuf8<KExtensionAPILength> nullBuf;
        ret = CreateClient( iClientUid, nullBuf );
        }
    
    if (  ret == KErrNone )
        {
        iServer.SetAssociationFlag( ETrue ); // Send association status
        iClient->Update( aIapId );   
        }    

    return ret;
    }
// -----------------------------------------------------------------------------
// ProcessAssociationStatus
// -----------------------------------------------------------------------------
//   
TInt CHotSpotSession::ProcessAssociationStatus( const TUint aIapId, TBool aResult )
    {
    DEBUG("CHotSpotSession::ProcessAssociationStatus");
    TInt ret( KErrNone );
    iServer.SetAssociationFlag( EFalse );
    if ( iHotspotExtension )
        { 
        iHotspotExtension = EFalse;
        if ( iClient != NULL )
            {
            DEBUG("CHotSpotSession::ProcessAssociationStatus sent to client");
            iClient->WlanAssociationStatus( aIapId, aResult );
            }
       }
    DEBUG("CHotSpotSession::ProcessAssociationStatus Done");
    return ret;
    }

// -----------------------------------------------------------------------------
// CreateClient
// -----------------------------------------------------------------------------
//   
TInt CHotSpotSession::CreateClient( const TUid aUid, TDesC8& aUidText )
    {
    DEBUG("CHotSpotSession::CreateClient");
    TInt err( KErrNone );
    if ( aUidText == KNullDesC8 )
        {
        DEBUG("CHotSpotSession::CreateClient iHotspotExtension = EFalse;");
        iHotspotExtension = EFalse;
        TRAP( err, iClient = CHssClientPlugin::NewL( aUid, aUidText ) );
        DEBUG1("CHotSpotSession::CreateClient err: %d", err );
        }
    else
        {
        DEBUG("CHotSpotSession::CreateClient iHotspotExtension = ETrue;");
        iHotspotExtension = ETrue;
        TRAP( err, iClient = CHssClientPlugin::NewL( aUid, aUidText ) );
        DEBUG1("CHotSpotSession::CreateClient err: %d", err );
        }
    return err;
    }
    
// -----------------------------------------------------------------------------
// ProcessClose
// -----------------------------------------------------------------------------
//   
TInt CHotSpotSession::ProcessCloseL( const TUint aIapId )
    {
    DEBUG("CHotSpotSession::ProcessCloseL");
    TInt ret( KErrNone );
    TBuf8<KExtensionAPILength> extAPI;
    TBuf<KUidLength> clientUid;
    
    TInt err = iServer.GetClientUid( aIapId, clientUid );
    if ( err != KErrNone )
        {
        DEBUG("CHotSpotSession::ProcessCloseL clientUid = EMPTY");
        // do nothing
        ret = KErrNotSupported;
        }
    else
        {
        DEBUG("CHotSpotSession::ProcessCloseL clientUid = CLIENT");
        if ( iClient == NULL )
            {
            //Convert TBuf to TUid
            TLex lex( clientUid );
            TUint value( 0 );
            lex.Val( value, EHex );
            iClientUid = TUid::Null();
            iClientUid.iUid = value;
            TBuf8<KExtensionAPILength> nullBuf;

            ret = CreateClient( iClientUid, nullBuf );
            }
        else
            {
            ret = KErrNone;   
            }
        iLogoutTimer->After( iServer.GetLogoutTimeMicroSecs( clientUid ) );
        if ( ret == KErrNone && !iServer.GetLogoutFlagValue() )
            {
            DEBUG("CHotSpotSession::ProcessCloseL send Logout()");
            iClient->Logout( aIapId );
            iServer.SetLogoutFlag( ETrue );
            iServer.SetLoginFlag( EFalse );
            }
        else
            {
            ret = KErrNotFound;
            }    
      
        }
    return ret;
    }
 
// -----------------------------------------------------------------------------
// ProcessServerShutdown
// -----------------------------------------------------------------------------
//    
void CHotSpotSession::ProcessServerShutdown( const RMessage2& aMessage )
    {
    DEBUG("CHotSpotSession::ProcessServerShutdown");
    TInt shutdown;
    
    shutdown = ( TInt )aMessage.Int0();
    if ( KHssShutdown == shutdown )
        {
        DEBUG("CHotSpotSession::ProcessServerShutdown shutdown ");
        aMessage.Complete( KErrNone );
        RProcess server; // Sets the handle-number to the KCurrentProcessHandle
        server.Kill( KErrNone );
        }
    else
        {
        DEBUG("CHotSpotSession::ProcessServerShutdown else ");
        aMessage.Complete( KErrNone );
        }
    }
    
// -----------------------------------------------------------------------------
// ModifyClientUid
// -----------------------------------------------------------------------------
//
void CHotSpotSession::ModifyClientUid( TDes& aUid )
    {
    DEBUG("CHotSpotSession::ModifyClientUid");
    TInt indx = aUid.Find( KMark1 );
    if ( KErrNotFound != indx )
        {
        aUid.Delete( indx, 1 );
        indx = aUid.Find( KMark2 );
        if ( KErrNotFound != indx )
            {
            aUid.Delete( indx, 1 );
            }
        }
    }

// -----------------------------------------------------------------------------
// ConvertTBufToTUid
// -----------------------------------------------------------------------------
//
void CHotSpotSession::ConvertTBufToTUid( TDes& aUid )
    {
    DEBUG("CHotSpotSession::ConvertTBufToTUid");
    TLex lex( aUid );
    TUint value( 0 );
    lex.Val( value, EHex );
    iClientUid = TUid::Null();
    iClientUid.iUid = value;
    }

// -----------------------------------------------------------------------------
// ConnectionStateChanged
// -----------------------------------------------------------------------------
//
void CHotSpotSession::ConnectionStateChanged( TWlanConnectionMode  aNewState ) 
    {
    DEBUG1( "CHotSpotSession::ConnectionStateChanged() aNewState=%d", aNewState );
    switch ( aNewState )
        {
        case EWlanConnectionModeSearching:
            {
            break;
            }
        case EWlanConnectionModeInfrastructure:
        case EWlanConnectionModeSecureInfra:
            {
            iMgtClient->CancelNotifications();
            ProcessAssociationStatus( iIapId, ETrue );
            break;
            }
        case EWlanConnectionModeAdhoc:
        case EWlanConnectionModeNotConnected:
            {
            iMgtClient->CancelNotifications();
            iServer.SetAssociationFlag( EFalse );
            break;
            }
        default:
            {
            iMgtClient->CancelNotifications();
            iServer.SetAssociationFlag( EFalse );
            }
        }

    }

// end of file

