/**
 * Copyright (c) 2010 Sasken Communication Technologies Ltd.
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of the "Eclipse Public License v1.0"
 * which accompanies  this distribution, and is available
 * at the URL "http://www.eclipse.org/legal/epl-v10.html"
 *
 * Initial Contributors:
 * Chandradeep Gandhi, Sasken Communication Technologies Ltd - Initial contribution
 * 
 * Contributors:
 * Pritam Roy Biswas, Sasken Communication Technologies Ltd
 *
 * Description:
 * Routines for Credential Manager Server
 */

//  Include Files  
#include <e32base.h>
#include <e32std.h>
#include <smfcredmgrcommon.h>

#include "smfcredmgrserver.h"
#include "smfcredmgrserversession.h"
#include "smfcredmgrdb.h"

// Perform all server initialisation, in particular creation of the
// scheduler and server and then run the scheduler
static void RunServerL()
	{
	// naming the server thread after the server helps to debug panics
	User::LeaveIfError(User::RenameThread(KCredMgrServerName));

	// create and install the active scheduler we need
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	// create the server (leave it on the cleanup stack)
	CSmfCredMgrServer* server = CSmfCredMgrServer::NewL();
	CleanupStack::PushL(server);

	// Initialisation complete, now signal the client
	RProcess::Rendezvous(KErrNone);

	// Run (will block until server exits)
	CActiveScheduler::Start();

	// Cleanup the server and scheduler
	CleanupStack::PopAndDestroy(server);
	CleanupStack::PopAndDestroy(scheduler);
	}

// Server process entry-point
TInt E32Main()
	{
	__UHEAP_MARK;

	CTrapCleanup* cleanup = CTrapCleanup::New();
	TInt r = KErrNoMemory;
	if (cleanup)
		{
			TRAP( r, RunServerL() );
		delete cleanup;
		}

	__UHEAP_MARKEND;
	return r;
	}

/**
 * CSmfCredMgrServer::NewL()
 * Two-phased constructor.
 * @return The constructed CSmfCredMgrServer instance
 */
CSmfCredMgrServer* CSmfCredMgrServer::NewL()
	{
	CSmfCredMgrServer* Server = CSmfCredMgrServer::NewLC();
	CleanupStack::Pop(Server);
	return Server;
	}

/**
 * CSmfCredMgrServer::NewLC()
 * Two-phased constructor.
 * @return The constructed CSmfCredMgrServer instance
 */
CSmfCredMgrServer* CSmfCredMgrServer::NewLC()
	{
	CSmfCredMgrServer* Server = new (ELeave) CSmfCredMgrServer();
	CleanupStack::PushL(Server);
	Server->ConstructL();
	return Server;
	}

/**
 * Constructor
 */
CSmfCredMgrServer::CSmfCredMgrServer() :
	CPolicyServer(CActive::EPriorityStandard, policy, EUnsharableSessions)
	{

	}

/**
 * CSmfCredMgrServer::ConstructL()
 * Symbian 2nd phase constructor can leave.
 */
void CSmfCredMgrServer::ConstructL()
	{
	iDbCreator = CSmfCredMgrDb::NewL();
	StartL(KCredMgrServerName);
	RDebug::Printf("in constructor CSmfCredMgrServer");
	}

/**
 * CSmfCredMgrServer::~CSmfCredMgrServer()
 * Destructor.
 */
CSmfCredMgrServer::~CSmfCredMgrServer()
	{
	delete iDbCreator;
	RDebug::Printf("in destructor CSmfCredMgrServer");
	}

/**
 * CSmfCredMgrServer::NewSessionL
 * Creates a new session with the server.
 */
CSession2* CSmfCredMgrServer::NewSessionL(const TVersion& aVersion,
		const RMessage2& /*aMessage*/) const
	{
	// Check that the version is OK
	TVersion v(KSecureServMajorVersionNumber, KSecureServMinorVersionNumber,
			KSecureServBuildVersionNumber);
	if (!User::QueryVersionSupported(v, aVersion))
		User::Leave(KErrNotSupported);

	return CSmfCredMgrServerSession::NewL(
			*const_cast<CSmfCredMgrServer*> (this));
	}

