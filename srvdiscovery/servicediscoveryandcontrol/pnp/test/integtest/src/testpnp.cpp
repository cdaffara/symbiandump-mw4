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
// Contains implementation of CTestPnPManager class
// 
//

#include "testpnp.h"
#include "testpnpmanager.h"

/*
	Static factory constructor. Uses two phase construction and leaves nothing on the 
	CleanupStack. Creates a CTestPnP object.
	@param  None
	@return Instance of the test server
 */
CTestPnP* CTestPnP::NewL()
	{
	CTestPnP *	server = new (ELeave) CTestPnP();
	CleanupStack::PushL(server);
	// CServer base class call
	RProcess	handle = RProcess();
	TParsePtrC	serverName(handle.FileName());
	server->ConstructL(serverName.Name());
	CleanupStack::Pop(server);
	return server;
	}

/*
   Function to create and start the CTestServer derived server.
   Secure variant.Much simpler, uses the new Rendezvous() call to sync with the client
   @param  None
   @return None
*/
LOCAL_C void MainL()
	{
	// Leave the hooks in for platform security
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().DataCaging(RProcess::ESecureApiOn);
	CActiveScheduler* sched = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CTestPnP*	server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTestPnP::NewL());
	if(err == KErrNone)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	}

/**
   Secure variant only
   Process entry point. Called by client using RProcess API.	
   @param  None
   @return Standard Epoc error code on process exit
 */
GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAPD( error, MainL() );
	delete cleanup;
	__UHEAP_MARKEND;
	return error;
    }


CTestStep* CTestPnP::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;
	TRAPD(err,testStep=CreateTestStepL(aStepName));
	if(err == KErrNone)
		{
		return testStep;
		}
	else
		{
		return NULL;
		}
	}

/**
   Secure and non-secure variants
   Implementation of CTestServer pure virtual
   @param  aStepName a reference to string
   @return A CTestStep derived instance
 */
CTestStep* CTestPnP::CreateTestStepL(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;
	// They are created "just in time" when the worker thread is created
	// More test steps can be added below
	if(aStepName == KTestPnPManager)
		{
		testStep = new CTestPnPManager();
        }
	return testStep;
	}



