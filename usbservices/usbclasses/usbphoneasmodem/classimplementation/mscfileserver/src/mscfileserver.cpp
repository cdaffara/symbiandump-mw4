// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description: Implements a Symbian OS server that exposes the RUsbMassStorage API
// 
// 

#include <e32svr.h>
#include "usbmscfileshared.h"
#include "mscfileserver.h"
#include "mscfilesession.h"
#include "mscfileserversecuritypolicy.h"
#include "mscfilecontroller.h"
#include "debug.h"

/**
 Constructs a USB mass storage Server
 
 @return  a pointer to CMscFileServer object
 */
CMscFileServer* CMscFileServer::NewLC()
	{
	CMscFileServer* r = new (ELeave) CMscFileServer();
	CleanupStack::PushL(r);
	r->ConstructL();
	return r;
	}

/**
 Destructor
 */
CMscFileServer::~CMscFileServer()
	{
	TRACE_FUNC

    delete iController;
	}

/**
 Constructor

 @param aController a USB mass storage controller reference
 */
CMscFileServer::CMscFileServer()
     : CPolicyServer(EPriorityHigh, KMscFileServerPolicy)
	{
	}

void CMscFileServer::ConstructL()
    {
    TRACE_FUNC

    StartL( KMscFileServerName );

    iController = CMscFileController::NewL();
    }
/**
 Create a new session on this server
 
 @param	&aVersion	Version of client
 @return	A pointer to a session object to be used for the client
 */
CSession2* CMscFileServer::NewSessionL(const TVersion &aVersion, const RMessage2& /*aMessage*/) const
	{
	TRACE_FUNC
	TVersion v( KUsbMsSrvMajorVersionNumber,
	            KUsbMsSrvMinorVersionNumber,
	            KUsbMsSrvBuildVersionNumber );

	if ( !User::QueryVersionSupported( v, aVersion ) )
	    {
        User::Leave( KErrNotSupported );
	    }

	CMscFileServer* ncThis = const_cast<CMscFileServer*>(this);
	CMscFileSession* sess = CMscFileSession::NewL(*ncThis);
	return sess;
	}


/**
 Inform the client there has been an error.
 
 @param	aError	The error that has occurred
 */
void CMscFileServer::Error(TInt aError)
	{
	TRACE_STATE(( _L("!! CMscFileServer::Error( %d )"), aError ))

	Message().Complete(aError);
	ReStart();
	}

/**
 Increment the open session count (iSessionCount) by one.
 
 @post	the number of open sessions is incremented by one
 */
void CMscFileServer::IncrementSessionCount()
	{
	TRACE_STATE(( _L(">< CMscFileServer::IncrementSessionCount( %d )"), iSessionCount ))
	__ASSERT_DEBUG(iSessionCount >= 0, User::Panic(KUsbMsSvrPncCat, EUsbMsPanicIllegalIPC));
	
	++iSessionCount;
	}

/**
 Decrement the open session count (iSessionCount) by one.
 
 @post		the number of open sessions is decremented by one
 */
void CMscFileServer::DecrementSessionCount()
	{
	TRACE_STATE(( _L(">< CMscFileServer::DecrementSessionCount( %d )"), iSessionCount ))
	__ASSERT_DEBUG(iSessionCount > 0, User::Panic(KUsbMsSvrPncCat, EUsbMsPanicIllegalIPC));

	--iSessionCount;
	}

void CMscFileServer::ThreadFunctionL()
    {
    TRACE_FUNC_ENTRY

    // Naming the server thread after the server helps to debug panics
    User::LeaveIfError( User::RenameThread( KMscFileServerName ) );
    
    // Construct active scheduler
    CActiveScheduler* activeScheduler = new ( ELeave ) CActiveScheduler;
    CleanupStack::PushL( activeScheduler ) ;

    // Install active scheduler
    // We don't need to check whether an active scheduler is already installed
    // as this is a new thread, so there won't be one
    CActiveScheduler::Install( activeScheduler );

    // Construct our server
    CMscFileServer::NewLC(); // Anonymous

    RProcess::Rendezvous( KErrNone );

    // Start handling requests
    CActiveScheduler::Start();

    //===== thread stopped =====
    TRACE_INFO(( _L( "   MscFileServer thread stopped" ) ))
    CleanupStack::PopAndDestroy( 2, activeScheduler ); // Anonymous CMscFileServer
    
    TRACE_FUNC_EXIT
    }


TInt CMscFileServer::ThreadFunction( TAny* /*aNone*/ )
    {
    TRACE_FUNC_ENTRY
    CTrapCleanup* cleanupStack = CTrapCleanup::New();
    if ( !cleanupStack )
        {
        RThread::Rendezvous( KErrNoMemory );
        }

    TRAPD( err, ThreadFunctionL() )
    if ( err != KErrNone )
        {
        TRACE_ERROR(( _L( "*** Error! Thread leaves w/ %d ***" ), err ))
        RThread::Rendezvous( KErrNoMemory );        
        }

    delete cleanupStack;
    cleanupStack = NULL;

    TRACE_FUNC_EXIT
    return KErrNone;
    }


TInt E32Main()
    {
    return CMscFileServer::ThreadFunction( NULL );
    }


