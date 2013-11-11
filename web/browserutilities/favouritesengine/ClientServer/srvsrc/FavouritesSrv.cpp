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
*      Implementation of class FavouritesSrv
*      
*
*/


// INCLUDE FILES

#include "FavouritesSession.h"
#include "FavouritesSrv.h"
#include "FavouritesSrvSession.h"
#include "FavouritesDef.h"
#include "FavouritesLogger.h"
#include "Timeout.h"
#include "FavouritesUtil.h"
#include "BackupObserver.h"
#include "FavouritesMsg.h"

#ifdef __MYSERVER_NO_PROCESSES__

// ==================== LOCAL FUNCTIONS ====================

/**
* WINS thread entry function.
* @param aPtr Not used.
* @return Error code.
*/
LOCAL_C TInt ThreadEntryFunc( TAny* /*aPtr*/ )
    {
    return FavouritesSrv::Run();
    }

#endif

/**
* Stop the Active Scheduler.
* @param aPtr Not used.
* @return KErrNone.
*/
LOCAL_C TInt StopScheduler( TAny* /*aPtr*/ )
    {
    // Called by the exit timer, after all clients disconnected (plus a small
    // delay). Stop the scheduler, this will enable he thread exit.
    FLOG(( _L("StopScheduler") ));
    CActiveScheduler::Stop();
    return KErrNone;
    }

/**
* Create a server.
* @param Pointer to created server (if created) returned here.
* @return Error code.
*/
LOCAL_C TInt CreateServer( CFavouritesSrv*& aServer )
    {
    // Server is not allowed to be created if Secure Backup is in progress
    
    TBool backupInProgress = EFalse; // This useless initialization is done 
        // because of RVCT compiler warning so do not delete it.

    TRAPD( err, backupInProgress = FavouritesUtil::IsBackupInProgressL());

    if(!err)
        {
        if(!backupInProgress)
            {
            // The TRAP is not working in the same stack frame where the
            // CTrapCleanup was created. This is why we need this function.
            TRAP( err, aServer = CFavouritesSrv::NewL() );
            }
        else
            {
            err = KErrLocked;
            }
        }

    return err;
    }

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// FavouritesSrv::Start
// ---------------------------------------------------------
//
TInt FavouritesSrv::Start()
    {
    FLOG(( _L("FavouritesSrv::Start") ));
    TInt err( KErrNone );
#ifdef __MYSERVER_NO_PROCESSES__
    RThread server;
    err = server.Create
        (
        KFavouritesSrvName,
        ThreadEntryFunc,
        KFavouritesSrvStackSize,
        KFavouritesSrvMinHeapSize,
        KFavouritesSrvMaxHeapSize,
        NULL,
        EOwnerProcess
        );
    FLOG(( _L("  thread created (%d)"), err ));
#else
    RProcess server;
    err = server.Create
        (
        KFavouritesSrvExe,
        KNullDesC,
        TUidType( KNullUid, KNullUid, KFavouritesUid ),
        EOwnerThread
        );
    FLOG(( _L("  process created (%d)"), err ));
#endif
    if ( !err )
        {
        TRequestStatus status;
        server.Rendezvous( status );
        if ( status != KRequestPending )
            {
            FLOG(( _L("FavouritesSrv::Start: abort startup") ));
            server.Kill( 0 );   // Abort startup.
            }
        else
            {
            FLOG(( _L("FavouritesSrv::Start: resume server") ));
            server.Resume();    // Logon OK - start the server.
            }
        User::WaitForRequest( status ); // Wait for start or death.
        // We can't use the 'exit reason' if the server panicked as this
        // is the panic 'reason' and may be '0' which cannot be distinguished
        // from KErrNone.
        err = (server.ExitType() == EExitPanic) ? KErrGeneral : status.Int();
        server.Close();
        }
    FLOG(( _L("FavouritesSrv::Start: returns (%d)"), err ));
    return err;
    }

// ---------------------------------------------------------
// FavouritesSrv::Run
// ---------------------------------------------------------
//
EXPORT_C TInt FavouritesSrv::Run()
    {
    FLOG(( _L("FavouritesSrv::Run") ));
    __UHEAP_MARK;

    CTrapCleanup* trapCleanup = NULL;
    CActiveScheduler* activeScheduler = NULL;
    CFavouritesSrv* server = NULL;

    TInt err = User::RenameThread( KFavouritesSrvName );
    if ( !err )
        {
        // Create a trap cleanup, make and install an active scheduler.
        err = KErrNoMemory;
        trapCleanup = CTrapCleanup::New();
        if ( trapCleanup )
            {
            activeScheduler = new CActiveScheduler();
            if ( activeScheduler )
                {
                CActiveScheduler::Install( activeScheduler );
                err = CreateServer( server );   // Not pushed (no leaving).
                if ( !err )
                    {
                    err = server->Start( KFavouritesSrvName );
                    FLOG(( _L("  server started: (%d)"), err ));
                    }
                }
            }
        }
    // Let the caller know how it went.
#ifdef __MYSERVER_NO_PROCESSES__
    RThread::Rendezvous( err );
#else
    RProcess::Rendezvous( err );
#endif
    if ( !err )
        {
        FLOG(( _L("FavouritesSrv::Run: starting active scheduler") ));
        CActiveScheduler::Start();
        FLOG(( _L("FavouritesSrv::Run: active scheduler stopped") ));
        }
        
    CActiveScheduler::Install( NULL );
    delete activeScheduler;
    
    FLOG(( _L("delete server") ));
    
    delete server;
    
    delete trapCleanup;

    __UHEAP_MARKEND;
    FLOG(( _L("FavouritesSrv::Run returns (%d)"), err ));
    return err;
    }

// ================= MEMBER FUNCTIONS =======================

// Platform security
	
// Custom check is applied to all IPCs. As IPC ids contain not only the ids
// but other information is embadded into them.
	
static const int KRangeCount = 1;

static const TInt SecurityRanges[KRangeCount] = 
	{
	EFavengNullFunction,
	};
	
static const TUint8 SecurityRangesPolicy[KRangeCount] =
	{
	CPolicyServer::ECustomCheck
	};

static const CPolicyServer::TPolicy Policy =
	{
	CPolicyServer::EAlwaysPass,
	KRangeCount,
	SecurityRanges,
	SecurityRangesPolicy,
	NULL,
	};

// ---------------------------------------------------------
// CFavouritesSrv::CustomSecurityCheckL
// ---------------------------------------------------------
//
CPolicyServer::TCustomResult CFavouritesSrv::CustomSecurityCheckL(
	const RMessage2& aMsg, TInt& /*aAction*/, TSecurityInfo& /*aMissing*/)
	{
		TFavouritesMsg msg( aMsg.Function() );
		
		TFavouritesFunction func = msg.Function();
		
		TCustomResult ret = EFail;
		
		if(func > EFavengTestCapabilityStart && 
		   func < EFavengTestCapabilityEnd)
			{
			if(aMsg.HasCapability(ECapabilityAllFiles))
               	{
               	ret = EPass;
               	}
			}
		else if(func > EFavengReadCapabilityStart && 
		   func < EFavengReadCapabilityEnd)
			{
			if(aMsg.HasCapability(ECapabilityReadUserData))
               	{
               	ret = EPass;
               	}
			}
		else if(func > EFavengWriteCapabilityStart && 
		   func < EFavengWriteCapabilityEnd)
			{
			if(aMsg.HasCapability(ECapabilityWriteUserData))
               	{
               	ret = EPass;
               	}
			}
        else
            {
            // Left empty by design
            }
		
		return ret;
	}
	
// ---------------------------------------------------------
// CFavouritesSrv::NewL
// ---------------------------------------------------------
//
CFavouritesSrv* CFavouritesSrv::NewL()
    {
    CFavouritesSrv* srv = new (ELeave) CFavouritesSrv();
    CleanupStack::PushL( srv );
    srv->ConstructL();
    CleanupStack::Pop();
    return srv;
    }

// ---------------------------------------------------------
// CFavouritesSrv::~CFavouritesSrv
// ---------------------------------------------------------
//
CFavouritesSrv::~CFavouritesSrv()
    {
    delete iExitTimer;
    delete iBackupObserver;
    }

// ---------------------------------------------------------
// CFavouritesSrv::SessionClosed
// ---------------------------------------------------------
//
void CFavouritesSrv::SessionClosed()
    {
    FLOG(( _L("CFavouritesSrv::SessionClosed") ));
    iSessionIter.SetToFirst();
    iSessionIter++;
    if ( iSessionIter++ == 0 )
        {
        // Schedule exit when last client has closed.
        iExitTimer->Cancel();   // Safety code: cancel if running.
        FLOG(( _L("  start exit timer") ));
        iExitTimer->After
            ( TTimeIntervalMicroSeconds32( KFavouritesSrvExitDelay ) );
        }
    }

// ---------------------------------------------------------
// CFavouritesSrv::PanicClient()
// ---------------------------------------------------------
//
void CFavouritesSrv::PanicClient( TInt aCode )
    {
    iReceivedMessage.Panic( KFavouritesSrvName, aCode );
    }

// ---------------------------------------------------------
// CFavouritesSrv::CFavouritesSrv
// ---------------------------------------------------------
//
CFavouritesSrv::CFavouritesSrv() : 
	CPolicyServer(CActive::EPriorityStandard, Policy, ESharableSessions)
    {
    }

// ---------------------------------------------------------
// CFavouritesSrv::ConstructL
// ---------------------------------------------------------
//
void CFavouritesSrv::ConstructL()
    {
    FLOG(( _L("CFavouritesSrv::ConstructL") ));
    iExitTimer = CTimeout::NewL
        ( CActive::EPriorityStandard, TCallBack( StopScheduler, NULL ) );
    FLOG(( _L("  start exit timer") ));
    iExitTimer->After
        ( TTimeIntervalMicroSeconds32( KFavouritesSrvExitDelay ) );

    iBackupObserver = CBackupObserver::NewL();
    }

// ---------------------------------------------------------
// CFavouritesSrv::NewSessionL
// ---------------------------------------------------------
//
CSession2* CFavouritesSrv::NewSessionL
( const TVersion& aVersion, const RMessage2& /*aMessage*/ ) const
    {
    FLOG(( _L("CFavouritesSrv::NewSessionL") ));
    if ( !User::QueryVersionSupported
        ( RFavouritesSession::Version(), aVersion ) )
        {
        User::Leave( KErrNotSupported );
        }
    CSession2* session = CFavouritesSession::NewL();
    iExitTimer->Cancel();   // We have a client, cancel exit (if pending).
    return session;
    }

//  End of File  
