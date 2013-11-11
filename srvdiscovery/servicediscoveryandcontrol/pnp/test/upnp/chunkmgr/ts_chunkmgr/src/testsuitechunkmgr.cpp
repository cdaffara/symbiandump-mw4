// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Description:
// @file
// @internalComponent
// 
//


#include <e32base.h>
#include <e32std.h>
#include <f32file.h>
#include "testsuitechunkmgr.h"

#include "test01createdeletechunkMgr.h"
#include "test02allocdealloc.h"
#include "test03allocerror.h"
#include "test04defaultheapfreecheck.h"
#include "test05allocdeallocperformance.h"
#include "test06alloccheckblocksize.h"
#include "test07exhaustchunkpools.h"
#include "test08multiplethreads.h"


#if ( !defined EKA2 )
// The system-wide unique name for the test-server
_LIT ( KServerName, "ts_chunkmgr" );
#endif


CChunkMgrTestSuite*  CChunkMgrTestSuite::NewL ()
	{
	CChunkMgrTestSuite* server = new ( ELeave ) CChunkMgrTestSuite ();
	CleanupStack::PushL ( server );

	server->StartL ( server->ServerName () );

	CleanupStack::Pop ( server );
	return server;
	}
	
CChunkMgrTestSuite::~CChunkMgrTestSuite ()
	{
	delete iTestStep;
	}
	
const TPtrC CChunkMgrTestSuite::ServerName ()
	{
#if (!defined EKA2 )
	return KServerName ();
#else
	TParsePtrC serverName ( RProcess ().FileName () );
	return serverName.Name ();
#endif
	}

CTestStep* CChunkMgrTestSuite::CreateTestStep ( const TDesC& aStepName )
	{
	if ( aStepName == KTest01 )
		{
		iTestStep = new CTest01CreateDeleteChunkMgr ();
		}
	else if ( aStepName == KTest02 )
		{
		iTestStep = new CTest02AllocDealloc ();
		}
	else if ( aStepName == KTest03 )
		{
		iTestStep = new CTest03AllocError ();
		}
	else if ( aStepName == KTest04 )
		{
		iTestStep = new CTest14DefaultHeapFreeCheck ();
		}	
	else if ( aStepName == KTest05 )
		{
		iTestStep = new CTest05AllocDeAllocPerformance ();
		}
	else if ( aStepName == KTest06 )
		{
		iTestStep = new CTest06AllocCheckBlockSize ();
		}
	else if ( aStepName == KTest07 )
		{
		iTestStep = new CTest07ExhaustChunkPools ();
		}
	else if ( aStepName == KTest08 )
		{
		iTestStep = new CTest08MultipleThreads ();
		}	
	
	return iTestStep;
	}

LOCAL_C void MainL ()
	{
	// For platform security
#if ( defined __DATA_CAGING__ )
	RProcess ().DataCaging ( RProcess::EDataCagingOn );
	RProcess ().SecureApi ( RProcess::ESecureApiOn );
#endif
	CActiveScheduler* sched = NULL;
	sched = new ( ELeave ) CActiveScheduler;
	CActiveScheduler::Install ( sched );
	CChunkMgrTestSuite* server = NULL;
	
	// Create the test-server
	TRAPD ( err, server = CChunkMgrTestSuite::NewL () );
	
	if ( !err )
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous ( KErrNone );
		sched->Start ();
		}
		
	delete server;
	delete sched;
	}

GLDEF_C TInt E32Main ()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New ();
	if ( cleanup == NULL )
		{
		return KErrNoMemory;
		}
	TInt err = KErrNone;
	TRAP ( err, MainL () );
	delete cleanup;
	__UHEAP_MARKEND;
	return KErrNone;
	}
