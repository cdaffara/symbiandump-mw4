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
* Description:       Implementation of HotSpot Server
*
*/



// INCLUDE FILES
#include <e32std.h>
#include <f32file.h>
#include <wlanmgmtclient.h>
#include <wlanmgmtinterface.h>
#include <e32std.h>
#include <commsdattypesv1_1.h>

#include <cmconnectionmethod.h>
#include <cmconnectionmethoddef.h>
#include <cmpluginwlandef.h>
#include <cmmanager.h>
#include <cmmanagerdef.h>
#include <cmdestination.h>

#include "hotspotserver.h"
#include "am_debug.h"
#include "hotspotsession.h"
#include "hssclientplugin.h"
#include "hssiapsettingshandler.h"

// LOCAL FUNCTION PROTOTYPES
GLDEF_C TInt E32Main();

using namespace CMManager;

// CONSTANTS
// Panic codes for HotSpot server
const TInt KHotSpotPanicCleanupStackCreationFailed  = 1;

// Default timeout values
const TUint KMillion = 1000000;
const TUint KHssDefaultLoginTimeMicroSecs = 180 * KMillion;  // 180 seconds (3 mins)
const TUint KHssMinLoginTime =      5;   // 5 seconds
const TUint KHssMaxLoginTime =      1200; // 1200 seconds (20 mins)
const TUint KHssDefaultLogoutTimeMicroSecs  = 12 * KMillion;   // 12 seconds
const TUint KHssMinLogoutTime =     1;    // 1 seconds
const TUint KHssMaxLogoutTime =     30;   // 30 seconds

// Panic category of HotSpot server 
_LIT( KHotSpotModuleName, "HOTSPOTSRV" );

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// RunServerL
// -----------------------------------------------------------------------------
// 
static void RunServerL()
    {
    // Create and install the active scheduler
    CActiveScheduler *scheduler = new (ELeave) CActiveScheduler;
    CleanupStack::PushL( scheduler );
    CActiveScheduler::Install( scheduler );
     
    // Create server
    CHotSpotServer::NewLC(); 
     
    // Initialisation complete, now signal the client
    User::LeaveIfError( RThread().RenameMe( KHotSpotServerName ) );
    RProcess::Rendezvous( KErrNone );
    
    // Ready to run
    DEBUG("**** HotspotServer: server fully running");
    CActiveScheduler::Start();
     
    // Cleanup the server and scheduler
    CleanupStack::PopAndDestroy( 2, scheduler );
    REComSession::FinalClose();
    }

// -----------------------------------------------------------------------------
// PanicClient
// -----------------------------------------------------------------------------
//    
void PanicClient(const RMessagePtr2& aMessage,THotspotPanic aPanic)
    {
    _LIT(KPanic,"HotspotServer");
    aMessage.Panic(KPanic,aPanic);
    }

// ============================ MEMBER FUNCTIONS ===============================
    
// -----------------------------------------------------------------------------
// CHotSpotServer
// -----------------------------------------------------------------------------
//
CHotSpotServer::CHotSpotServer()
    :CPolicyServer( EPriorityStandard, THotSpotServerPlatSecPolicy, ESharableSessions )
    {
    iAssociationValue=false;
    iLogoutSent=false ;
    iLoginValue=false;
    iIapCheckValue=false;
    iMgtClient=NULL;
    iClientIapsChecked =0;
    }

// -----------------------------------------------------------------------------
// NewLC
// -----------------------------------------------------------------------------
//
CHotSpotServer* CHotSpotServer::NewLC()
    {
    DEBUG("**** HotSpotServer: CHotSpotServer::NewLC");
    CHotSpotServer* self = new(ELeave) CHotSpotServer;
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// ~CHotSpotServer
// -----------------------------------------------------------------------------
//
CHotSpotServer::~CHotSpotServer()
    {
    DEBUG( "**** HotSpotServer: CHotSpotServer::~CHotSpotServer" );
    
    iMap.Close();
    iNotificationArray.Close();
    iLoginLogoutTimerArray.Close();
    iClientIaps.Close();
    
     if ( iMgtClient != NULL )
        {
#ifndef __WINS__    
        iMgtClient->CancelNotifications();
#endif
        delete iMgtClient;
        }
    }

// -----------------------------------------------------------------------------
// ConstructL
// -----------------------------------------------------------------------------
//
void CHotSpotServer::ConstructL()
    {
    DEBUG( "**** HotSpotServer: CHotSpotServer::ConstructL" );
    StartL( KHotSpotServerName );

    iIapCheckValue = EFalse;
    iLogoutSent = EFalse;
    iLoginValue = ETrue;
    iAssociationValue = EFalse;
    iClientIapsChecked = KErrNone;
    
    TRAP( iClientIapsChecked, FindClientIapsL() );
 
    // Activate notifications for IAP check purposes. Done with every server startup.
    // When EWlanConnectionModeNotConnected is received we can cancel this.
    iMgtClient = CWlanMgmtClient::NewL();
#ifndef __WINS__
    iMgtClient->ActivateNotificationsL( *this );
#endif 
    }

// -----------------------------------------------------------------------------
// ConnectionStateChanged
// -----------------------------------------------------------------------------
//
void CHotSpotServer::ConnectionStateChanged( TWlanConnectionMode  aNewState ) 
    {
    DEBUG1( "CHotSpotServer::ConnectionStateChanged() aNewState=%d", aNewState );
    if ( aNewState == EWlanConnectionModeNotConnected )
        {
        if ( iMgtClient != NULL )
            {
#ifndef __WINS__
            iMgtClient->CancelNotifications();
#endif
            }
        if ( iClientIapsChecked != KErrNone )
            {
            TRAP_IGNORE( FindClientIapsL() );
            }
        TRAPD(err, CheckIapsL());
        if ( err != KErrNone )
            {
            DEBUG1("CHotSpotServer::ConnectionStateChanged(): %d", err);
            }
        }
    }

// -----------------------------------------------------------------------------
// FindClientIapsL
// -----------------------------------------------------------------------------
//
void CHotSpotServer::FindClientIapsL()
    {
    DEBUG("CHotSpotServer::FindClientIapsL()");
    RCmManager cmManager;
    cmManager.OpenL();
    CleanupClosePushL(cmManager);
           
    RArray<TUint32> cmArray;
    CleanupClosePushL( cmArray );
    TBool supportedBearersOnly = ETrue;
    TBool legacyCmsOnly = EFalse;
    
    cmManager.ConnectionMethodL( cmArray, supportedBearersOnly, legacyCmsOnly );
    DEBUG1("CHotSpotServer::FindClientIapsL count: %d", cmArray.Count());  
    
    for( TInt i = 0; i < cmArray.Count(); i++ )
        {
        RCmConnectionMethod cm = cmManager.ConnectionMethodL( cmArray[i] );
        CleanupClosePushL( cm );
        HBufC* daemonName = cm.GetStringAttributeL( 
                                                ECmConfigDaemonManagerName );
        CleanupStack::PushL( daemonName );
        if ( daemonName )
            {
            if ( daemonName->Compare( KHotSpotPlugin ) == 0 )
                {
                DEBUG("CHotSpotServer::FindClientIapsL() Client found");
                TBuf<KIapNameLength> clientUid;
                TUid uid(TUid::Null());
                TUint iapId = cm.GetIntAttributeL( ECmIapId  );
                HBufC* client = cm.GetStringAttributeL( 
                                            EWlanServiceExtensionTableName );
                clientUid.Copy( client->Des() );
                delete client;
                SetClientIapL( iapId, clientUid );
                DEBUG1("CHotSpotServer::FindClientIapsL() iapId: %d", iapId );
                }
            }
        CleanupStack::PopAndDestroy( daemonName );
        CleanupStack::PopAndDestroy( &cm );
        }
    CleanupStack::PopAndDestroy( &cmArray );
    CleanupStack::PopAndDestroy( &cmManager );
    DEBUG("CHotSpotServer::FindClientIapsL() Done");
    }

// -----------------------------------------------------------------------------
// CheckIapsL
// -----------------------------------------------------------------------------
//
void CHotSpotServer::CheckIapsL()
    {
    DEBUG("CHotSpotServer::CheckIapsL");
    // This is needed to be checked only once per boot
    iIapCheckValue = ETrue;
    TInt count = iClientIaps.Count();

    // Check the array. Not supposed to be many elements.
    for( TInt i = 0; i < count; i++ )
        {
        TLex lex( iClientIaps[i].iClientUid );
        TUint value( 0 );
        lex.Val( value, EHex );
        TUid client = TUid::Null();
        client.iUid = value;
        
        // Try to find if plugin exists
        CHssClientPlugin* plugin(NULL);
        TBuf8<KExtensionAPILength> nullBuf;
        TRAPD( error, plugin = CHssClientPlugin::NewL( client, nullBuf ) );
        delete plugin;

        DEBUG1("CHotSpotServer::CheckIapsL find client error: %d", error );
        if ( error == KErrNotFound )
            {
            // Remove from database
            RCmManager cmManager;
            cmManager.OpenL();
            CleanupClosePushL(cmManager);
            
            RCmConnectionMethod cm;
            cm = cmManager.ConnectionMethodL( iClientIaps[i].iIapId );
            CleanupClosePushL( cm );
            cmManager.RemoveAllReferencesL( cm );
            cm.DeleteL();
            
            CleanupStack::PopAndDestroy( &cm );
            CleanupStack::PopAndDestroy( &cmManager );
            
            // Remove from array
            RemoveClientIap( iClientIaps[i].iIapId );
            count--;
            i--;
            }
        }
    DEBUG("CHotSpotServer::CheckIapsL Done");
    }

// -----------------------------------------------------------------------------
// NewSessionL
// -----------------------------------------------------------------------------
//
CSession2* CHotSpotServer::NewSessionL( const TVersion& aVersion, 
                                        const RMessage2& /* aMessage */ ) const
    {
    TVersion version( KHotSpotMajorVersionNumber,
                      KHotSpotMinorVersionNumber,
                      KHotSpotBuildVersionNumber );

    if ( !User::QueryVersionSupported( version, aVersion ) )
        {
        User::Leave( KErrNotSupported );
        }

    DEBUG("**** CHotSpotServer::NewSessionL");
    CHotSpotSession* session = 
        CHotSpotSession::NewL( const_cast<CHotSpotServer&>( *this ) );
    return session;
    }

// ---------------------------------------------------------
// NotifyAdd
// ---------------------------------------------------------
//
void CHotSpotServer::NotifyAdd(
    CNotificationBase& aNotification )
    {
    DEBUG( "CHotSpotServer::NotifyAdd()" );
    iNotificationArray.Insert( &aNotification, 0 );
    }

// ---------------------------------------------------------
// NotifyRemove
// ---------------------------------------------------------
//
void CHotSpotServer::NotifyRemove(
    CNotificationBase& aNotification )
    {
    DEBUG( "CHotSpotServer::NotifyRemove()" );
    TInt index = iNotificationArray.Find( &aNotification );
    iNotificationArray.Remove( index );
    }

// -----------------------------------------------------------------------------
// SaveMessage
// -----------------------------------------------------------------------------
//
TInt CHotSpotServer::SaveMessage( TUint aIapId, const RMessage2& aMessage, 
                                  THotSpotCommands aCommand )
    {
    DEBUG("**** HotspotServer: SaveMessage");
    TInt ret( KErrNone );
    SRequestMapEntry entry;
    entry.iMessage = aMessage;
    entry.iFunction = aCommand;
    entry.iIapId = aIapId;
    ret = iMap.Append(entry);
    return ret;
    }

// -----------------------------------------------------------------------------
// CompleteMessage
// -----------------------------------------------------------------------------
//
void CHotSpotServer::CompleteMessage( TInt aIndex, TInt aResult )
    {
    DEBUG("**** HotspotServer: CompleteMessage");
    SRequestMapEntry entry = iMap[aIndex];
    
    RMessagePtr2 message = entry.iMessage;
    message.Complete( aResult );
    iMap.Remove( aIndex );
    }

// -----------------------------------------------------------------------------
// EditMessage
// -----------------------------------------------------------------------------
//
TInt CHotSpotServer::EditMessage( TInt aIndex, TPckg<HssScanList>& aData)
    {
    DEBUG("**** HotspotServer: EditMessage");
    TInt ret( KErrNone );
    
    iMap[aIndex].iMessage.Write(0, aData);
    
    DEBUG1("**** HotspotServer: EditMessage ret: %d", ret);
    return ret;
    }


// -----------------------------------------------------------------------------
// FindMessage
// -----------------------------------------------------------------------------
//
TInt CHotSpotServer::FindMessage( TUint aIapId, THotSpotCommands aCommand )
    {
    DEBUG("**** HotspotServer: FindMessage");
    TInt ret( KErrNotFound );
    SRequestMapEntry entry;
    
    for ( TInt i = 0; i < iMap.Count(); i++ ) 
        {
        entry = iMap[i];
        if ( ( entry.iFunction == aCommand ) && ( entry.iIapId == aIapId ) )
            {
            // Correct message found
            // Now stop loop and return index value, so that CompeteMessage can
            // be called with that
            ret = i;
            i = iMap.Count();
            }
        }
    DEBUG1("**** HotspotServer: FindMessage ret: %d", ret);
    return ret;
    }

// -----------------------------------------------------------------------------
// SetLogoutFlag
// -----------------------------------------------------------------------------
//
void CHotSpotServer::SetLogoutFlag( TBool aValue )
    {
    DEBUG("HotspotServer::SetLogoutFlag");
    iLogoutSent = aValue;
    }
    
// -----------------------------------------------------------------------------
// GetLogoutFlagValue
// -----------------------------------------------------------------------------
//
TBool CHotSpotServer::GetLogoutFlagValue()
    {
    DEBUG("HotspotServer::GetLogoutFlagValue()");
    return iLogoutSent;
    }

// -----------------------------------------------------------------------------
// SetLoginFlag
// -----------------------------------------------------------------------------
//
void CHotSpotServer::SetLoginFlag( TBool aValue )
    {
    DEBUG("HotspotServer::SetLoginFlag");
    iLoginValue = aValue;
    }
    
// -----------------------------------------------------------------------------
// GetLoginFlagValue
// -----------------------------------------------------------------------------
//
TBool CHotSpotServer::GetLoginFlagValue()
    {
    DEBUG("HotspotServer::GetLoginFlagValue()");
    return iLoginValue;
    }


// -----------------------------------------------------------------------------
// SetAssociationFlag
// -----------------------------------------------------------------------------
//
void CHotSpotServer::SetAssociationFlag( TBool aValue )
    {
    DEBUG("HotspotServer::SetAssociationFlag");
    iAssociationValue = aValue;
    }
        
// -----------------------------------------------------------------------------
// GetAssociationFlagValue
// -----------------------------------------------------------------------------
//
TBool CHotSpotServer::GetAssociationFlagValue()
    {
    DEBUG("HotspotServer::GetAssociationFlagValue()");
    return iAssociationValue;
    }
  
// -----------------------------------------------------------------------------
// RunError
// -----------------------------------------------------------------------------
//
TInt CHotSpotServer::RunError( TInt aError )
    {
    DEBUG1( "HotspotServer::RunError = %d", aError );
    // error from CHotSpotSession::ServiceL
    Message().Complete( aError );
    // Continue  reading client requests
    ReStart();
    return (KErrNone);
    }

// -----------------------------------------------------------------------------
// GetLoginTimerMicroSecs
// -----------------------------------------------------------------------------
//
TUint CHotSpotServer::GetLoginTimeMicroSecs( TDes& aClientUid )
    {
    DEBUG("HotspotServer::GetLoginTimeMicroSecs()");
    // Default timer value will be returned if matching client UID isn't found.
    TUint retval = KHssDefaultLoginTimeMicroSecs ;
    
    TInt ret = FindClientUid( aClientUid );
    if (ret != KErrNotFound)
        {
        // ret is the matching element's index.
        retval = iLoginLogoutTimerArray[ret].iLoginTimeMicroSecs;
        }

    return retval;
    }

// -----------------------------------------------------------------------------
// GetLogoutTimerMicroSecs
// -----------------------------------------------------------------------------
//
TUint CHotSpotServer::GetLogoutTimeMicroSecs( TDes& aClientUid )
    {
    DEBUG("HotspotServer::GetLogoutTimeMicroSecs()");
    // Default timer value will be returned if matching client UID isn't found.
    TUint retval = KHssDefaultLogoutTimeMicroSecs ;
    
    TInt ret = FindClientUid( aClientUid );
    if (ret != KErrNotFound)
        {
        // ret is the matching element's index.
        retval = iLoginLogoutTimerArray[ret].iLogoutTimeMicroSecs;
        }

    return retval;
    }

// -----------------------------------------------------------------------------
// SetTimerValuesL
// -----------------------------------------------------------------------------
//
void CHotSpotServer::SetTimerValuesL(
        TDes& aClientUid,
        TUint aLoginTimerValue,   // in seconds
        TUint aLogoutTimerValue ) // in seconds
    {    
    DEBUG("HotspotServer::SetTimerValuesL()");
    TInt ret = FindClientUid( aClientUid );
    TUint loginTimeMicroSecs = KHssDefaultLoginTimeMicroSecs ;
    TUint logoutTimeMicroSecs = KHssDefaultLogoutTimeMicroSecs ;
    
    // Check that values are in bounds and modify them into micro seconds.
    if( aLoginTimerValue >= KHssMinLoginTime &&
            aLoginTimerValue <= KHssMaxLoginTime )
        {
        loginTimeMicroSecs = KMillion * aLoginTimerValue;
        }

    if( aLogoutTimerValue >= KHssMinLogoutTime &&
            aLogoutTimerValue <= KHssMaxLogoutTime )
        {
        logoutTimeMicroSecs = KMillion * aLogoutTimerValue;
        }

    if (ret != KErrNotFound)
        {
        DEBUG("CHotSpotServer::SetTimerValuesL(): Existing client modified.");
        // ret is the matching element's index.
        iLoginLogoutTimerArray[ret].iLoginTimeMicroSecs = loginTimeMicroSecs;
        iLoginLogoutTimerArray[ret].iLogoutTimeMicroSecs = logoutTimeMicroSecs;
        }
    else
        {
        DEBUG("CHotSpotServer::SetTimerValuesL(): New Client added.");
        // Create a new element and append it to the array.
        const SLoginLogoutTimers addedElement = {
                aClientUid,
                loginTimeMicroSecs,
                logoutTimeMicroSecs };
        iLoginLogoutTimerArray.AppendL( addedElement );
        }
    }

// -----------------------------------------------------------------------------
// FindClientUid
// -----------------------------------------------------------------------------
//
TInt CHotSpotServer::FindClientUid( TDes& aClientUid )
    {
    DEBUG("CHotspotServer::FindClientUid()");
    TInt count = iLoginLogoutTimerArray.Count();
    TInt i = 0;
    TInt ret = KErrNotFound;
    
    while ( i < count )
        {
        if ( aClientUid.Compare( iLoginLogoutTimerArray[i].iClientUid ) == 0 )
            {
            DEBUG("CHotspotServer::FindClientUid() FOUND");
            ret = i;
            break;
            }
        i++;
        }

    return ret;    
    }

// -----------------------------------------------------------------------------
// GetClientUid
// -----------------------------------------------------------------------------
//
TInt CHotSpotServer::GetClientUid( TUint aIapId,  TDes& aUid )
    {
    DEBUG("CHotspotServer::GetClientUid()");
    TInt ret( KErrNotFound );
    TInt count = iClientIaps.Count();
    TInt i = 0;
    
    while ( i < count )
        {
        if ( aIapId == iClientIaps[i].iIapId )
            {
            DEBUG("CHotspotServer::GetClientUid() FOUND");
            aUid = iClientIaps[i].iClientUid;
            ret = KErrNone;
            break;
            }
        i++;
        }   
      
    return ret;
    }

// -----------------------------------------------------------------------------
// SetClientIapL
// -----------------------------------------------------------------------------
//
void CHotSpotServer::SetClientIapL( TUint aIapId, TDes& aUid )
    {
    DEBUG("CHotspotServer::SetClientIap()");
    SClientIaps clientElement = { aIapId, aUid };
    iClientIaps.AppendL( clientElement );
    }

// -----------------------------------------------------------------------------
// RemoveClientIap
// -----------------------------------------------------------------------------
//
void CHotSpotServer::RemoveClientIap( TUint aIapId )
    {
    DEBUG("CHotspotServer::RemoveClientIap()");
    TInt count = iClientIaps.Count();
    TInt i = 0;
    
    while ( i < count )
        {
        if ( aIapId == iClientIaps[i].iIapId )
            {
            DEBUG("CHotspotServer:::RemoveClientIap() FOUND");
            iClientIaps.Remove( i );
            break;
            }
        i++;
        }
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// E32Main implements the executable entry function.
// Note that because the target type of the IPWServer Hss
// is EXEDLL, the entry point has different signature depending
// on the build platform.
// Creates a cleanup stack and runs the server. 
// Returns: Zero
// -----------------------------------------------------------------------------
//
GLDEF_C TInt E32Main()
    {
    __UHEAP_MARK;
    DEBUG("**** HotspotServer: E32Main");
    CTrapCleanup* cleanup=CTrapCleanup::New();
    TInt r=KErrNoMemory;
    if (cleanup)
        {
        TRAP(r,RunServerL());
        delete cleanup;
        }
    else
        {
        User::Panic( KHotSpotModuleName, KHotSpotPanicCleanupStackCreationFailed );
        }
    __UHEAP_MARKEND;
    return r;
    }
 
// end of file
