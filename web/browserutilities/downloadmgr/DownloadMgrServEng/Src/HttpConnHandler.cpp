/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  ?Description
*
*/



// INCLUDE FILES
#include "HttpClientApp.h"
#include "HttpClientAppInstance.h"
#include "HttpDownload.h"
#include "HttpConnHandler.h"
#include "HttpDownloadManagerServerEngine.h"
#include "FileExt.h"
#include "HttpDownloadMgrLogger.h"

#include <in_sock.h>
#include <commdbconnpref.h>
#include <httpfilterauthenticationinterface.h>
#include <uaproffilter_interface.h>
#include <httpfiltercommonstringsext.h>
#include <cdblen.h>
//#include <DeflateFilterInterface.h>
#include <cookiefilterinterface.h>

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
const TInt KShutDownTimer = 60*1000000;  //60 sec - Updated to 60 secs as part of error fix JSIN-7JSE6H

// MACROS
//#define ?macro ?macro_def

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------
// CHttpConnShutdownTimer::NewL
// ---------------------------------------------------------
//
CHttpConnShutdownTimer* CHttpConnShutdownTimer::NewL( MShutdownObserver* aObserver )
    {
	CHttpConnShutdownTimer* self = new (ELeave) CHttpConnShutdownTimer( aObserver );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self ); // self
	return self;
    }

// ---------------------------------------------------------
// CHttpConnShutdownTimer::CHttpConnShutdownTimer
// ---------------------------------------------------------
//
CHttpConnShutdownTimer::CHttpConnShutdownTimer( MShutdownObserver* aObserver )
    :CActive( EPriorityStandard )
    ,iObserver( aObserver )
    {
    CLOG_WRITE_1( "Shutdown_timer(0x%x)", this);
    }

// ---------------------------------------------------------
// CHttpConnShutdownTimer::~CHttpConnShutdownTimer
// ---------------------------------------------------------
//
CHttpConnShutdownTimer::~CHttpConnShutdownTimer()
    {
    Cancel();
    iTimer.Close();
    CLOG_WRITE_1( "Shutdown destroyed (0x%x)", this);
    }

// ---------------------------------------------------------
// CHttpConnShutdownTimer::ConstructL
// ---------------------------------------------------------
//
void CHttpConnShutdownTimer::ConstructL()
    {
    User::LeaveIfError( iTimer.CreateLocal() );
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// CHttpConnShutdownTimer::Start
// ---------------------------------------------------------
//
void CHttpConnShutdownTimer::Start( TInt aTimeOut )
    {
    CLOG_WRITE("Shutdown timer started");
    if( IsActive() )
        {
        return;
        }

    iTimer.After( iStatus, aTimeOut );
    SetActive();
    }

// ---------------------------------------------------------
// CHttpConnShutdownTimer::DoCancel
// ---------------------------------------------------------
//
void CHttpConnShutdownTimer::DoCancel()
    {
    iTimer.Cancel();
    CLOG_WRITE("Shutdown timer canceled");
    }

// ---------------------------------------------------------
// CHttpConnShutdownTimer::RunL
// ---------------------------------------------------------
// 
void CHttpConnShutdownTimer::RunL()
    {
    LOGGER_ENTERFN("CHttpConnShutdownTimer::RunL()");
    iObserver->ShutDown();
    }

// -----------------------------------------------------------------------------
// CHttpConnStageNotifier::CHttpConnStageNotifier
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CHttpConnStageNotifier::CHttpConnStageNotifier( CHttpConnHandler* aConnHandler )
    :CActive( EPriorityStandard )
    ,iConnHandler( aConnHandler )
    {
    CLOG_WRITE_1( "Stage notifier created (0x%x)", this );
    }

// -----------------------------------------------------------------------------
// CHttpConnStageNotifier::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHttpConnStageNotifier::ConstructL()
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CHttpConnStageNotifier::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHttpConnStageNotifier* 
                CHttpConnStageNotifier::NewL( CHttpConnHandler* aConnHandler )
    {
    CHttpConnStageNotifier* self = 
                            new( ELeave ) CHttpConnStageNotifier( aConnHandler );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

    
// Destructor
CHttpConnStageNotifier::~CHttpConnStageNotifier()
    {
    Cancel();
    
    CLOG_WRITE_1( "Stage notifier destroy: (0x%x)", this );
    }

// -----------------------------------------------------------------------------
// CHttpConnStageNotifier::Start
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpConnStageNotifier::Start()
    {
    LOGGER_ENTERFN( "Start" );

    if( IsActive() )
        // Already observed
        {
        return;
        }

    iConnHandler->Connection().ProgressNotification( iProgressBuf, iStatus );
    SetActive();
    }

// -----------------------------------------------------------------------------
// CHttpConnHandler::DoCancel
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpConnStageNotifier::DoCancel()
    {
    if( iConnHandler->Connection().SubSessionHandle() )
        {
        iConnHandler->Connection().CancelProgressNotification();
        }
    }

// -----------------------------------------------------------------------------
// CHttpConnHandler::RunL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpConnStageNotifier::RunL()
    {
    if( iStatus == KErrNone )
        {
        TInt stage = iProgressBuf().iStage;

        iConnHandler->ConnectionStageChanged( stage );

        if( stage > KConnectionUninitialised )
            // connection is still alive
            {
            iConnHandler->Connection().ProgressNotification( iProgressBuf, iStatus );
            SetActive();
            }
        }
    else
        {
        iConnHandler->ConnectionError( iStatus.Int() );
        }
    }

// -----------------------------------------------------------------------------
// CHttpConnHandler::CHttpConnHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CHttpConnHandler::CHttpConnHandler( CHttpClientApp* aClientApp )
    : CActive( EPriorityStandard )
    , iClientApp( aClientApp )
    {
    CLOG_CREATE;
    }

// -----------------------------------------------------------------------------
// CHttpConnHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHttpConnHandler::ConstructL()
    {
    CLOG_NAME_2( _L("CHttpConnHandler_%x_c%x"), iClientApp->AppUid(), (TInt)this );
    LOGGER_ENTERFN( "ConstructL" );

    CActiveScheduler::Add( this );

    iConnNotif = CHttpConnStageNotifier::NewL( this );
    CLOG_ATTACH( iConnNotif, this );
    iShutDown = CHttpConnShutdownTimer::NewL( this );
    CLOG_ATTACH( iShutDown, this );

    iHttpSession.OpenL();
    CLOG_WRITE8( "Session open" );
    InitSessionL();
    }

// -----------------------------------------------------------------------------
// CHttpConnHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHttpConnHandler* CHttpConnHandler::NewL( CHttpClientApp* aClientApp )
    {
    CHttpConnHandler* self = new( ELeave ) CHttpConnHandler( aClientApp );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

    
// Destructor
CHttpConnHandler::~CHttpConnHandler()
    {
    if( IsActive() )
        {
        Cancel();
        }

    ShutDown( ETrue );

    iHttpSession.Close();

    delete iConnNotif;
    delete iShutDown;
    
    CLOG_CLOSE;
    }


// -----------------------------------------------------------------------------
// CHttpConnHandler::ConnectL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpConnHandler::ConnectL()
    {
    LOGGER_ENTERFN( "ConnectL" );

    TBool doComplete( ETrue );

    iShutDown->Cancel();

    if( !iConnection.SubSessionHandle() )
        // only the first connection request does this initialization
        {
        CLOG_WRITE( "No subsession" );
        // forget the previous notifier
        if( iConnName )
            {
            CLOG_WRITE_1("ConnName set: [%S]", iConnName);
            
            TName connName;

            connName.Copy( *iConnName );

            User::LeaveIfError( iConnection.Open( iClientApp->Engine()->SocketServ(), connName ) );

            CLOG_WRITE( "connection open" );
            
            TNifProgress progress;

            iConnection.Progress( progress );

            iConnStage = progress.iStage;
            CLOG_WRITE_1("Stage: %d", iConnStage);

            UpdateIapId();
            }
        else
            {
            User::LeaveIfError( iConnection.Open( iClientApp->Engine()->SocketServ() ) );
            CLOG_WRITE8_1( "Start: %d", iIapId );
            if( iIapId )
                {
                iPref.SetDialogPreference( ECommDbDialogPrefDoNotPrompt );
                iPref.SetIapId( iIapId );
                }
            else
                {
                iPref.SetDialogPreference( ECommDbDialogPrefPrompt );
                }

            iConnection.Start( iPref, iStatus );

            // RConnection will complete us.
            doComplete = EFalse;
            }

        iConnNotif->Start();

        iNewConnection = ETrue;
        }

    CLOG_WRITE_1( "stage: %d", iConnStage );
    if( !IsActive() )
        {
        CLOG_WRITE( "Not active" );

        SetActive();

        CLOG_WRITE_1( "doComplete: %d", doComplete );
        if( doComplete )
            {
            TRequestStatus* dummy = &iStatus;

            User::RequestComplete( dummy, KErrNone );
            }
        }
    }

// -----------------------------------------------------------------------------
// CHttpConnHandler::Disconnect
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpConnHandler::Disconnect( TBool aAtOnce, CHttpDownload* aDownload )
    {
    LOGGER_ENTERFN( "Disconnect()" );
    CLOG_WRITE8_1( "Exit: %d", aAtOnce );

    // to remove this download from the referenced ones
    if( aDownload )
        {
        aDownload->SetConnHandler( NULL );
        }

    TInt refs = Referencies();

    CLOG_WRITE8_1( "aAtOnce: %d", aAtOnce );
    CLOG_WRITE8_1( "refs: %d", refs );

    if( aAtOnce || (!refs && !iConnName) )
        // no more reference to this connection -> real disconnect
        // but only if the connection name was not set from outside
        // In that case only CHttpClientAppInstance::Disconnect can
        // disconnect (it uses aAtOnce ETrue).
        {
        CLOG_WRITE_1( "conn handle: %d", iConnection.SubSessionHandle() );
        CLOG_WRITE_1( "stage: %d", iConnStage );

        if( !aAtOnce 
            && iConnection.SubSessionHandle() 
            && iConnStage == KLinkLayerOpen )
            {
			CLOG_WRITE_1( "ShutDown->Start: atOnce=%d", aAtOnce );
			iConnStage = KConnectionStartingClose;
            iShutDown->Start( KShutDownTimer );
            }
        else
            {
            ShutDown();
            }
        }
    }

// -----------------------------------------------------------------------------
// CHttpConnHandler::IapId
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint32 CHttpConnHandler::IapId() const
    {
    return iIapId;
    }

// -----------------------------------------------------------------------------
// CHttpConnHandler::ClientAppInst
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CHttpClientApp* CHttpConnHandler::ClientApp() const
    {
    return iClientApp;
    }

// -----------------------------------------------------------------------------
// CHttpConnHandler::ClientAppInst
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CHttpClientAppInstance* CHttpConnHandler::ClientAppInst() const
    {
    return iClientInst;
    }

// -----------------------------------------------------------------------------
// CHttpConnHandler::Session
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
RHTTPSession& CHttpConnHandler::Session()
    {
    return iHttpSession;
    }

// -----------------------------------------------------------------------------
// CHttpConnHandler::Connection
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
RConnection& CHttpConnHandler::Connection()
    {
    return iConnection;
    }

// -----------------------------------------------------------------------------
// CHttpConnHandler::ConnectionNameL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
HBufC* CHttpConnHandler::ConnectionNameL( TBool& aDelete )
    {
    if( iConnName )
        {
        aDelete = EFalse;
        return iConnName;
        }
    else if( iConnection.SubSessionHandle() )
        {
        TName connName;

        aDelete = ETrue;

        iConnection.Name( connName );

        return connName.AllocL();
        }

    aDelete = EFalse;
    return NULL;
    }

// -----------------------------------------------------------------------------
// CHttpConnHandler::APNameL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpConnHandler::APNameL( TPtrC16& aValue )
    {
// TODO: APNameL
/*
    if( iConnection.SubSessionHandle() )
        {
        TBuf<KCommsDbSvrMaxFieldLength> name;
        TBuf<20> query;

        query.Format( _L("%s\\%s"), IAP, COMMDB_NAME );
        iConnection.GetDesSetting( query, name );
        }
*/
    aValue.Set( KNullDesC );
    }

// -----------------------------------------------------------------------------
// CHttpConnHandler::IsConnected
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CHttpConnHandler::IsConnected()
    {
    return iConnStage == KLinkLayerOpen;
    }

// -----------------------------------------------------------------------------
// CHttpConnHandler::SetClientAppInst
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpConnHandler::SetClientAppInst( CHttpClientAppInstance* aClientInst )
    {
    iClientInst = aClientInst;
    }

// -----------------------------------------------------------------------------
// CHttpConnHandler::SetIapId
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpConnHandler::SetIapId( TUint32 aIapId )
    {
    CLOG_WRITE8_1( "SetIAPId: %d", aIapId );
    __ASSERT_DEBUG( !IsConnected() || (IsConnected() && aIapId == iIapId), DMPanic( KErrInUse ) );

    if( IsConnected() )
        // Do not use this new IAP id as long as connected
        {
        return;
        }

    iIapId = aIapId;
    }

// -----------------------------------------------------------------------------
// CHttpConnHandler::SetConnectionNameL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpConnHandler::SetConnectionNameL( const TDesC& aConnName )
    {
    CLOG_WRITE_1( "connName: %S", &aConnName );

    if( !IsConnected() )
        {
        if( aConnName.Length() )
            {
            if( !iConnName )
                {
                iConnName = HBufC::NewL( KMaxName );
                }

            iConnName->Des().Copy( aConnName );

            ConnectL();
            }
        else
            // forget the connection name
            {
            delete iConnName; iConnName = NULL;
            }
        }
    else
        {
        CLOG_WRITE( "Connected!!!" );
        User::Leave( KErrInUse );
        }
    }

// -----------------------------------------------------------------------------
// CHttpConnHandler::DoCancel
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpConnHandler::DoCancel()
    {
    LOGGER_ENTERFN( "DoCancel" );

    CArrayPtrFlat<CHttpDownload>* downloads = 
                                iClientApp->Downloads();
    for( TInt i = 0; i < downloads->Count(); ++i )
        {
        if( (*downloads)[i]->ConnHandler() == this )
            {
            (*downloads)[i]->ConnectionFailed( KErrCancel );
            }
        }
    }

// -----------------------------------------------------------------------------
// CHttpConnHandler::RunL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpConnHandler::RunL()
    {
    LOGGER_ENTERFN( "RunL()" );
    CLOG_WRITE8_1( "%d", iStatus.Int() );
    if( iStatus.Int() != KErrNone )
        {
        iNewConnection = EFalse;

        CArrayPtrFlat<CHttpDownload>* downloads = 
                                    iClientApp->Downloads();
        for( TInt i = 0; i < downloads->Count(); ++i )
            {
            if( (*downloads)[i]->ConnHandler() == this )
                {
                (*downloads)[i]->ConnectionFailed( iStatus.Int() );
                }
            }
        }
    else
        {
        // Have to check the connection stage if the it was cloned.
        TNifProgress progress;

        iConnection.Progress( progress );

        iConnStage = progress.iStage;
        CLOG_WRITE_1( "Stage: %d", iConnStage );

        if( iConnStage == KLinkLayerOpen )
            // Connect request was issued by download when connection was already 
            // open -> RConnection.Start() wasn't called, only the request
            // was completed.
            {
            Connected();
            }
        }
    }

// -----------------------------------------------------------------------------
// CHttpConnHandler::ConnectionStageChanged
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpConnHandler::ConnectionStageChanged( TInt aStage )
    {
    CLOG_WRITE8_1( "Stage: %d", aStage );

    iConnStage = aStage;

    if( iConnStage == KConnectionUninitialised || 
        iConnStage == KDataTransferTemporarilyBlocked
        )
        {
        __ASSERT_DEBUG( iClientApp, DMPanic( KErrCorrupt ) );
        CArrayPtrFlat<CHttpDownload>* downloads = 
                                    iClientApp->Downloads();
        for( TInt i = 0; i < downloads->Count(); ++i )
            {
            if( (*downloads)[i]->ConnHandler() == this )
                {
                if( iConnStage == KConnectionUninitialised )
                    {
                    // from now on this name is invalid -> forget it!
                    delete iConnName; iConnName = NULL;

                    (*downloads)[i]->Disconnected();
                    }
                else
                    {
                    (*downloads)[i]->Suspended();
                    }
                }
            }

        if( iConnStage == KConnectionUninitialised )
            {
            ShutDown();
            }
        if ( iConnStage == KDataTransferTemporarilyBlocked )
            {
            iShutDown->Start( KShutDownTimer );
            }     
        }
    else if( iConnStage == KLinkLayerOpen )
        // connection open
        {
        Connected();
        }
    }

// -----------------------------------------------------------------------------
// CHttpConnHandler::ConnectionError
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpConnHandler::ConnectionError( TInt aError )
    {
    __ASSERT_DEBUG( iClientApp, DMPanic( KErrCorrupt ) );
    CArrayPtrFlat<CHttpDownload>* downloads = 
                                iClientApp->Downloads();

    for( TInt i = 0; i < downloads->Count(); ++i )
        {
        if( (*downloads)[i]->ConnHandler() == this )
            {
            (*downloads)[i]->ConnectionFailed( aError );
            }
        }
    }

// -----------------------------------------------------------------------------
// CHttpConnHandler::ShutDown
//
// Connection notification is not canceled here, because need to know
// when the connection is really closed. Forget the connection name, set by
// the client app only if the connection is really closed.
// -----------------------------------------------------------------------------
//
void CHttpConnHandler::ShutDown( TBool aFromDestructor )
    {
    CLOG_WRITE( "ShutDown" );
    CLOG_WRITE8_1( "dest: %d", aFromDestructor );

    iNewConnection = EFalse;

    if( iShutDown )
        {
        iShutDown->Cancel();
        }

    if( iConnNotif )
        {
        iConnNotif->Cancel();
        }
    
    Cancel();
    CLOG_WRITE( "Canceled" );
    iConnStage = KConnectionUninitialised;

	CLOG_WRITE( "Closing connection" );
    iConnection.Close();
    CLOG_WRITE( "Conn closed" );

    // Pause the downloads  
    CArrayPtrFlat<CHttpDownload>* downloads = 
                                iClientApp->Downloads();
    for( TInt i = 0; i < downloads->Count(); ++i )
        {
        if( (*downloads)[i]->ConnHandler() == this )
            {
            TRAP_IGNORE( (*downloads)[i]->PauseL( ETrue ) );
            }
        }
    if( !iClientInst && !aFromDestructor )
        // Client instance already exited and all download disconnected ->
        // no need for this connhandler anymore.
        // the next client will create a new connhandler and assigned downloads
        // will use that one.
        {
        CLOG_WRITE( "Destroy me" );
        // DO NOT USE 'this' after this call!!!
        iClientApp->DestroyConnHandler( this );
        }
    }

// -----------------------------------------------------------------------------
// CHttpConnHandler::InitSessionL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpConnHandler::InitSessionL()
    {
    LOGGER_ENTERFN( "InitSessionL" );

    iHttpSession.StringPool().OpenL(HttpFilterCommonStringsExt::GetTable());

    // Set the disconnect notification 
    iHttpSession.ConnectionInfo().SetPropertyL
        (
        iHttpSession.StringPool().StringF( HTTP::ENotifyOnDisconnect, RHTTPSession::GetTable() ), 
        iHttpSession.StringPool().StringF( HTTP::EEnableDisconnectNotification, RHTTPSession::GetTable() )
        ); 

    CHttpFilterAuthenticationInterface::InstallFilterL(iHttpSession, ETrue);
    CHttpFilterAuthenticationInterface::InstallFilterL(iHttpSession, EFalse);
    CHttpUAProfFilterInterface::InstallFilterL(iHttpSession);
    CHttpCookieFilter::InstallFilterL(iHttpSession);
    }

// -----------------------------------------------------------------------------
// CHttpConnHandler::SetConnectionInfoL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpConnHandler::SetConnectionInfoL()
    {
    RStringPool strPool = iHttpSession.StringPool();

    // Remove first session properties just in case.
    RHTTPConnectionInfo connInfo = iHttpSession.ConnectionInfo();
    
    // Clear RConnection and Socket Server instances
    connInfo.RemoveProperty(strPool.StringF(HTTP::EHttpSocketServ,RHTTPSession::GetTable()));
    connInfo.RemoveProperty(strPool.StringF(HTTP::EHttpSocketConnection,RHTTPSession::GetTable()));
    
    // Clear the proxy settings
//    THTTPHdrVal proxyUsage(strPool.StringF(HTTP::EUseProxy,RHTTPSession::GetTable()));
    connInfo.RemoveProperty(strPool.StringF(HTTP::EProxyUsage,RHTTPSession::GetTable()));
    connInfo.RemoveProperty(strPool.StringF(HTTP::EProxyAddress,RHTTPSession::GetTable()));
    
    // RConnection and Socket Server
    connInfo.SetPropertyL ( strPool.StringF(HTTP::EHttpSocketServ, 
                                    RHTTPSession::GetTable()), 
                            THTTPHdrVal (iClientApp->Engine()->SocketServ().Handle()) );
    
    TInt connPtr1 = REINTERPRET_CAST(TInt, &iConnection);
    connInfo.SetPropertyL ( strPool.StringF(HTTP::EHttpSocketConnection, 
                            RHTTPSession::GetTable() ), THTTPHdrVal (connPtr1) );    
    }

// -----------------------------------------------------------------------------
// CHttpConnHandler::Connected
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpConnHandler::Connected()
    {
    if( iNewConnection )
        {
        iNewConnection = EFalse;

        if( iStatus.Int() == KErrNone )
            {
            if( !iIapId )
                {
                UpdateIapId();
                }

            TRAP_IGNORE( SetConnectionInfoL() );
            }
        }

    __ASSERT_DEBUG( iClientApp, DMPanic( KErrCorrupt ) );
    CArrayPtrFlat<CHttpDownload>* downloads = 
                                iClientApp->Downloads();
    for( TInt i = 0; i < downloads->Count(); ++i )
        {
        if( (*downloads)[i]->ConnHandler() == this )
            {
            iShutDown->Cancel();
            (*downloads)[i]->Connected();
            }
        }
    }

// -----------------------------------------------------------------------------
// CHttpConnHandler::Referencies
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CHttpConnHandler::Referencies()
    {
    TInt refs( 0 );

    __ASSERT_DEBUG( iClientApp, DMPanic( KErrCorrupt ) );
    CArrayPtrFlat<CHttpDownload>* downloads = 
                                iClientApp->Downloads();

    for( TInt i = 0; i < downloads->Count(); ++i )
        {
        if( (*downloads)[i]->ConnHandler() == this )
            {
            ++refs;
            }
        }

    return refs;
    }

// -----------------------------------------------------------------------------
// CHttpConnHandler::UpdateIapId
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpConnHandler::UpdateIapId()
    {
    if( IsConnected() )
        {
        // 20 = it's more than enough for the format string
        TBuf<20> query;

        query.Format( _L("%s\\%s"), IAP, COMMDB_ID );
        iConnection.GetIntSetting( query, iIapId );
        }
    }

//  End of File  
