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
//

#include "cupnpmessagetest.h"
#include "cupnpresponseparsertest.h"
#include "cupnprequestcomposertest.h"
#include "cupnprequestparsertest.h"
#include "cupnpresponsecomposertest.h"
#include "ccodecdecodetest.h"
#include "ccodecencodetest.h"
#include <e32base.h>
#include <e32std.h>
#include <f32file.h>


#if (!defined EKA2)
// The system-wide unique name for the test-server
_LIT(KServerName, "upnpmessagetest");
#endif


CUpnpMessageTest*  CUpnpMessageTest::NewL()
	{
	CUpnpMessageTest* server = new(ELeave) CUpnpMessageTest();
	CleanupStack::PushL(server);

	server->StartL(server->ServerName());

	CleanupStack::Pop(server);
	return server;
	}

CUpnpMessageTest::CUpnpMessageTest()
	{
	}

CUpnpMessageTest::~CUpnpMessageTest()
	{
	delete iTestStep;
	}

const TPtrC CUpnpMessageTest::ServerName()
	{
#if (!defined EKA2)
	return KServerName();
#else
	TParsePtrC serverName(RProcess().FileName());
	return serverName.Name();
#endif
	}

CTestStep* CUpnpMessageTest::CreateTestStep(const TDesC& aStepName)
	{
	if (aStepName == KResponseParserTest)
		{
		iTestStep = new CUpnpResponseParserTest();
		}

	else if(aStepName == KRequestComposerTest)
		{
		iTestStep = new CUpnpRequestComposerTest();
		}

	else if(aStepName == KRequestParserTest)
		{
		iTestStep = new CUpnpRequestParserTest();
		}

	else if(aStepName == KResponseComposerTest)
		{
		iTestStep = new CUpnpResponseComposerTest();
		}

	else if(aStepName == KCodecDecodeTest)
		{
		iTestStep = new CCodecDecodeTest();
		}

	else if(aStepName == KCodecEncodeTest)
		{
		iTestStep = new CCodecEncodeTest();
		}

	return iTestStep;
	}


LOCAL_C void MainL()
	{
	// For platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().SecureApi(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched = NULL;
	sched = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CUpnpMessageTest* server = NULL;

	// Create the test-server
	TRAPD(err, server = CUpnpMessageTest::NewL());

	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if (cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TInt err = KErrNone;
	TRAP(err, MainL());
	delete cleanup;
	__UHEAP_MARKEND;
	return KErrNone;
	}



