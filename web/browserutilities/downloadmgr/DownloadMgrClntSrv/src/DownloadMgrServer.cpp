/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*     This file contains the declaration of the Download Mgr Server.
*
*/


// INCLUDE FILES
#include "DownloadMgrLogger.h"
#include "DownloadMgrServer.h"
#include "DownloadMgrDef.h"

#include <HttpClientAppInstance.h>
#include <HttpDownload.h>
#include <e32svr.h>
#include <basched.h>
//#include <e32uid.h>

// CONSTANTS

const TUint KDMgrServerPolicyRangeCount = 3;
const TInt  KDMgrServerPolicyRanges[KDMgrServerPolicyRangeCount] =
	{
    0, // EHttpDownloadMgrInitialize,                   // = 0
    1, // EHttpDownloadMgrCreateDownload,               // = 1
    /*
    EHttpDownloadMgrCheckUrl,                     // = 2
    EHttpDownloadMgrAttach,                       // = 3
    EHttpDownloadMgrPauseAll,                     // = 4
    EHttpDownloadMgrStartAllL,                    // = 5
    EHttpDownloadMgrResetAll,                     // = 6
    EHttpDownloadMgrDeleteAll,                    // = 7
    EHttpDownloadMgrDisconnect,                   // = 8

    EHttpDownloadMgrGetIntAttribute,              // = 9
    EHttpDownloadMgrGetBoolAttribute,             // =10
    EHttpDownloadMgrGetStringAttribute,           // =11
    EHttpDownloadMgrGetString8Attribute,          // =12

    EHttpDownloadMgrSetIntAttribute,              // =13
    EHttpDownloadMgrSetBoolAttribute,             // =14
    EHttpDownloadMgrSetStringAttribute,           // =15
    EHttpDownloadMgrSetString8Attribute,          // =16
    EHttpDownloadMgrSessionEventSubscription,     // =17
    EHttpDownloadMgrSessionEventCancel,           // =18

    EHttpDownloadAttach,                          // =19
    EHttpDownloadStart,                           // =20
    EHttpDownloadPause,                           // =21
    EHttpDownloadReset,                           // =22
    EHttpDownloadDelete,                          // =23
    EHttpDownloadMove,                            // =24
    EHttpDownloadClose,                           // =25
    EHttpDownloadEventSubscription,               // =26
    EHttpDownloadEventCancel,                     // =27

    EHttpDownloadGetIntAttribute,                 // =28
    EHttpDownloadGetBoolAttribute,                // =29
    EHttpDownloadGetStringAttribute,              // =30
    EHttpDownloadGetString8Attribute,             // =31

    EHttpDownloadSetIntAttribute,                 // =32
    EHttpDownloadSetBoolAttribute,                // =33
    EHttpDownloadSetStringAttribute,              // =34
    EHttpDownloadSetString8Attribute,             // =35
    EHttpDownloadBufferAttributes,                // =36
    
    EHttpDownloadGetRFile,                        // =37
    EHttpDownloadSetRFile,                        // =38
    EHttpDownloadSetOnError,                      // =39
    EHttpDownloadAttachToDownload                 // =40
    EHttpDownloadCloseCompleted,				  // =41
    EHttpDownloadDataAttribute,                   // =42
    EHttpDownloadTrackAttribute,				  // =43
 
	*/
    44  // EHttpDownloadMgrNotSupported           // =44
	};

enum TDownloadMgrSecurityPolicy
	{
	EDmgrSecurityNetworkServices		= 0
	};

const CPolicyServer::TPolicyElement dMgrServerPolicyElements[] =
	{
	// Check Network Services 		-> EDmgrSecurityNetworkServices	= 0
    {_INIT_SECURITY_POLICY_C1(ECapabilityNetworkServices), CPolicyServer::EPanicClient}
	};
	
const TUint8 dMgrServerPolicyElementsIndex[] =
	{
	EDmgrSecurityNetworkServices,	// applies to req  0
	CPolicyServer::EAlwaysPass,		// applies to req  1 - 39
	CPolicyServer::ENotSupported 	// applies to req 40 -
	};
	
const CPolicyServer::TPolicy dMgrServerPolicy =
	{
	CPolicyServer::EAlwaysPass,
	KDMgrServerPolicyRangeCount,
	KDMgrServerPolicyRanges,
	dMgrServerPolicyElementsIndex,
	dMgrServerPolicyElements
	};

// GLOBAL FUNCTIONS

// start the server thread
// This is called from the client.
EXPORT_C TInt StartDownloadMgrServer( TAny* /*anArg*/ )
	{
	CLOG_CREATE_NULL;
	CLOG_NAME_NULL( _L("Server") );
    CLOG_WRITE_NULL( "StartDownloadMgrServer" )

	TBuf<32> name( KDownloadMgrMainThreadName );

    RThread().RenameMe( name );
    
    CLOG_WRITE_NULL( "RThread().RenameMe( name );" );
    
	TInt result = KErrNone;

	CTrapCleanup* cleanup = CTrapCleanup::New();
    CLOG_WRITE_FORMAT_NULL( "%x = CTrapCleanup::New();", cleanup );
	if ( cleanup == NULL )
		{
		result = KErrNoMemory;
		}
	else
		{
		CActiveScheduler* scheduler = new CActiveScheduler;
		if ( scheduler == NULL )
			{
			result = KErrNoMemory;
			}
		else
			{
			CActiveScheduler::Install( scheduler );

			CDownloadMgrServer* server = NULL;
			TRAP( result, server = CDownloadMgrServer::NewL() );
            CLOG_WRITE_FORMAT_NULL( "%x = CDownloadMgrServer::NewL();", server );
			if ( result == KErrNone )
				{
    			CLOG_ATTACH_NULL( NULL, server );
    			
                RProcess::Rendezvous( result );
                CLOG_WRITE_NULL( "RThread::Rendezvous( result );" );
				// It starts an infinite loop from which we can break out
				// only if we stop the active scheduler
				CActiveScheduler::Start();

				// finished
				delete server;
				}
            else
                {

                }
			delete scheduler;
			}
		delete cleanup;
		}

    CLOG_WRITE_NULL( "StartDownloadMgrServer" )
    CLOG_CLOSE_NULL;

	return result;
    }

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CDownloadMgrServer::PanicServer
// ---------------------------------------------------------
//
void CDownloadMgrServer::PanicServer( TDownloadMgrSvrPanic aPanic )
	{
    CLOG_ENTERFN_NULL( "CDownloadMgrServer::PanicServer" )
    _LIT( KTxtDownloadMgrServer,"CDownloadMgrServer" );
	User::Panic( KTxtDownloadMgrServer ,aPanic );
	}

// ---------------------------------------------------------
// CDownloadMgrServer::NewL
// ---------------------------------------------------------
//
CDownloadMgrServer* CDownloadMgrServer::NewL()
	{
    CLOG_ENTERFN_NULL( "CDownloadMgrServer::NewL" )
	CDownloadMgrServer* self = new (ELeave) CDownloadMgrServer();
	CleanupStack::PushL( self );
	self->StartL( KDownloadMgrServerName );	
    CLOG_WRITE_NULL( "self->StartL( KDownloadMgrServerName );" );	
	self->ConstructL();	
	CleanupStack::Pop( self ); // self
	return self;
	}


// ---------------------------------------------------------
// CDownloadMgrServer::NewContainerL
// ---------------------------------------------------------
//
CObjectCon* CDownloadMgrServer::NewContainerL()
	{
    CLOG_WRITE( "CDownloadMgrServer::NewContainerL" )
	return iContainerIndex->CreateL();
	}

// ---------------------------------------------------------
// CDownloadMgrServer::NewContainerL
// ---------------------------------------------------------
//
void CDownloadMgrServer::RemoveContainer( CObjectCon* aCon )
    {
    CLOG_ENTERFN( "CDownloadMgrServer::RemoveContainer" )
    iContainerIndex->Remove( aCon );
    }

// ---------------------------------------------------------
// CDownloadMgrServer::CDownloadMgrServer
// ---------------------------------------------------------
//
CDownloadMgrServer::CDownloadMgrServer()
: CPolicyServer( EPriorityStandard, dMgrServerPolicy )
, iSessionCount( 0 )
	{
	__DECLARE_NAME( _S( "CDownloadMgrServer" ) );
	}


// ---------------------------------------------------------
// CDownloadMgrServer::ConstructL
// ---------------------------------------------------------
//
void CDownloadMgrServer::ConstructL()
	{
    CLOG_ENTERFN( "CDownloadMgrServer::ConstructL" )
	iContainerIndex = CObjectConIx::NewL();
    iDmEngine = CHttpDownloadManagerServerEngine::NewL();
    iShutdown = CDownloadMgrShutdown::NewL( this );
	}


// ---------------------------------------------------------
// CDownloadMgrServer::~CDownloadMgrServer
// ---------------------------------------------------------
//
CDownloadMgrServer::~CDownloadMgrServer()
	{
    CLOG_ENTERFN( "CDownloadMgrServer::~CDownloadMgrServer" )
	// delete the object container index.
	delete iContainerIndex;
    if( iDmEngine )
        {
        delete iDmEngine;
        iDmEngine = NULL;
        }
    delete iShutdown;
	}


// ---------------------------------------------------------
// CDownloadMgrServer::NewSessionL
// ---------------------------------------------------------
//
CSession2* CDownloadMgrServer::NewSessionL( const TVersion& aVersion, const RMessage2& /*aMessage*/ ) const
	{
    CLOG_ENTERFN( "CDownloadMgrServer::NewSessionL" )
    iShutdown->Cancel();
	// check version is ok
	TVersion v( KDownloadMgrMajorVersionNumber,
                KDownloadMgrMinorVersionNumber,
                KDownloadMgrBuildVersionNumber );

	if( !User::QueryVersionSupported( v, aVersion ) )
        {
		User::Leave( KErrNotSupported );
        }

	// make new session
    CLOG_WRITE( "__DMGRSRV_IPC_V2__" );
    return CDownloadMgrSession::NewL( ( CDownloadMgrServer* )this );
	}

// ---------------------------------------------------------
// CDownloadMgrServer::CreateNewClientAppInstanceL
// ---------------------------------------------------------
//
CHttpClientAppInstance* CDownloadMgrServer::CreateNewClientAppInstanceL( 
                                        TUint32 aAppUid,
                                        MDownloadStateObserver& aObserver,
                                        TBool aGetAllPendingDownloads )
    {
    CLOG_WRITE( "CDownloadMgrServer::CreateNewClientAppInstanceL" )
    return iDmEngine->CreateNewClientAppInstanceL( 
        aAppUid, &aObserver, aGetAllPendingDownloads );
    }

// ---------------------------------------------------------
// CDownloadMgrServer::CloseClientInstance
// ---------------------------------------------------------
//
void CDownloadMgrServer::CloseClientInstance( CHttpClientAppInstance* aAppInst )
    {
    CLOG_ENTERFN( "CDownloadMgrServer::CloseClientInstance" )
    iDmEngine->CloseClientInstance( aAppInst );
    }

// ---------------------------------------------------------
// CDownloadMgrServer::IcrementSessions
// ---------------------------------------------------------
//
void CDownloadMgrServer::IcrementSessions()
    {
    CLOG_ENTERFN( "CDownloadMgrServer::IcrementSessions" )
    iSessionCount++;
    iShutdown->Cancel();
    }

// ---------------------------------------------------------
// CDownloadMgrServer::DecrementSessions
// ---------------------------------------------------------
//
void CDownloadMgrServer::DecrementSessions()
    {
    CLOG_ENTERFN( "CDownloadMgrServer::DecrementSessions" )
    iSessionCount--;
    if( iSessionCount > 0 )
        {
        return;
        }
    iShutdown->Start();
    }

// ---------------------------------------------------------
// CDownloadMgrServer::NextSessionId
// ---------------------------------------------------------
//
TInt CDownloadMgrServer::NextSessionId()
    {
    return ++iNextSessionId;
    }

// ---------------------------------------------------------
// CDownloadMgrServer::ActiveDownloads
// ---------------------------------------------------------
//
TInt CDownloadMgrServer::ActiveDownloads() const
    {
    return iDmEngine->ActiveDownloads();
    }

// ---------------------------------------------------------
// CDownloadMgrShutdown::NewL
// ---------------------------------------------------------
//
CDownloadMgrShutdown* CDownloadMgrShutdown::NewL( CDownloadMgrServer* aServer )
    {
    CLOG_ENTERFN_NULL( "CDownloadMgrShutdown::NewL" )
	CDownloadMgrShutdown* self = new (ELeave) CDownloadMgrShutdown( aServer );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self ); // self
	return self;
    }

// ---------------------------------------------------------
// CDownloadMgrShutdown::CDownloadMgrShutdown
// ---------------------------------------------------------
//
CDownloadMgrShutdown::CDownloadMgrShutdown( CDownloadMgrServer* aServer )
:CActive( EPriorityStandard )
    {
    iServer = aServer;
    }

// ---------------------------------------------------------
// CDownloadMgrShutdown::~CDownloadMgrShutdown
// ---------------------------------------------------------
//
CDownloadMgrShutdown::~CDownloadMgrShutdown()
    {
    CLOG_ENTERFN( "CDownloadMgrShutdown::~CDownloadMgrShutdown" )
    Cancel();
    iTimer.Close();
    }

// ---------------------------------------------------------
// CDownloadMgrShutdown::ConstructL
// ---------------------------------------------------------
//
void CDownloadMgrShutdown::ConstructL()
    {
    CLOG_ENTERFN( "CDownloadMgrShutdown::ConstructL" )
    User::LeaveIfError( iTimer.CreateLocal() );
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// CDownloadMgrShutdown::Start
// ---------------------------------------------------------
//
void CDownloadMgrShutdown::Start()
    {
    CLOG_ENTERFN( "CDownloadMgrShutdown::Start" )
    if( !IsActive() )
        {
        iTimer.After( iStatus, KDownloadMgrShutdownInterval );
        SetActive();
        }
    }

// ---------------------------------------------------------
// CDownloadMgrShutdown::DoCancel
// ---------------------------------------------------------
//
void CDownloadMgrShutdown::DoCancel()
    {
    CLOG_ENTERFN( "CDownloadMgrShutdown::DoCancel" )
    iTimer.Cancel();
    }

// ---------------------------------------------------------
// CDownloadMgrShutdown::RunL
// ---------------------------------------------------------
// 
void CDownloadMgrShutdown::RunL()
    {
    CLOG_ENTERFN( "CDownloadMgrShutdown::RunL" )
    if( !iServer->ActiveDownloads() )
        {
        CActiveScheduler::Stop();
        }
    else
        {
        Start();
        }
    }
