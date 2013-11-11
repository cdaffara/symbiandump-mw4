/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/



#include <e32base.h>

// Test system includes
#include "te_serverbase.h"
#include "te_serverbaseteststep.h"


LOCAL_C void MainL()
/**
 * Much simpler, uses the new Rendezvous() call to sync with the client
 */
	{
	// Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().SecureApi(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);

	CTestServer* server = NULL;
	// Create the CTestServer derived server

	TRAPD(err,server = CTestServerBase::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	}

// Only a DLL on emulator for typhoon and earlier

GLDEF_C TInt E32Main()
/**
 * @return - Standard Epoc error code on exit
 */
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAPD(err,MainL());
	delete cleanup;
	if(KErrNone != err)
		{
		return err;
		}
	return KErrNone;
    }

// Create a thread in the calling process
// Emulator typhoon and earlier

//---------------------------------------------------------------------------------------------------------------------------
//this is the test server which instantiates the test steps

CTestServer* CTestServerBase::NewL ()
	{
	CTestServerBase* server = new ( ELeave ) CTestServerBase;
	CleanupStack::PushL(server);
	server->ConstructL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}

CTestServerBase::~CTestServerBase ()
	{
	}

CTestServerBase::CTestServerBase ()
	{
	}

CTestStep* CTestServerBase::CreateTestStep(const TDesC& aStepName)
	{
	// removed ELeave as harness will test ptr. This is more efficient
	// than using TRAP_IGNORE
	CTestServerTestStep* ret = new CTestServerTestStep;			//creating the test step
	//CTestServerTestStep derives from CTestStep
	if(ret)
	{
		ret->SetTestStepName(aStepName);
	}
	return ret;
	}
