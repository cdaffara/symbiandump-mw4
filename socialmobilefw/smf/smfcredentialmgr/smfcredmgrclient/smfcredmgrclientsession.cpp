/**
 * Copyright (c) 2010 Sasken Communication Technologies Ltd.
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of the "Eclipse Public License v1.0"
 * which accompanies  this distribution, and is available
 * at the URL "http://www.eclipse.org/legal/epl-v10.html"
 *
 * Initial Contributors:
 * Pritam Roy Biswas, Sasken Communication Technologies Ltd - Initial contribution
 *
 * Description:
 *  Client-side handle to a session with a server. Derives from RSessionBase to 
 *   create a communicating channel with the server.
 *
 */

#include "smfcredmgrclientsession.h"

// The number of connection attempts.
const TInt KConnectAttempts = 2;

//  Global Functions
static TInt StartServer()
	{

	RProcess server;
	TInt r = server.Create(KCredMgrServerName, KNullDesC);
	if (r != KErrNone)
		{
		return r;
		}
	TRequestStatus stat = KRequestPending;
	server.Rendezvous(stat);
	if (stat != KRequestPending)
		server.Kill(0);
	else
		server.Resume();
	
	User::WaitForRequest(stat);
	TExitType et = server.ExitType();
	TInt code = stat.Int();
	r = (et == EExitPanic) ? KErrGeneral : code;
	server.Close();

	return KErrNone;
	}

RSmfCredMgrClientSession::RSmfCredMgrClientSession()
	{

	}

TInt RSmfCredMgrClientSession::connectToServer()
	{
	TInt retry = KConnectAttempts;
	TInt r = 0;
	for (;;)
		{
		r = CreateSession(KCredMgrServerName, Version(), 1);
		if (r == KErrNone)
			{
			break;
			}
		else if (r != KErrNotFound && r != KErrServerTerminated)
			{
			return r;
			}

		if (--retry == 0)
			{
			return r;
			}

		r = StartServer();
		if (r != KErrNone && r != KErrAlreadyExists)
			{
			return r;
			//error 
			}
		}

	return r;

	}

TVersion RSmfCredMgrClientSession::Version() const
	{
	return (TVersion(KSecureServMajorVersionNumber,
			KSecureServMinorVersionNumber, KSecureServBuildVersionNumber));
	}

void RSmfCredMgrClientSession::Close()
	{
	RSessionBase::Close();
	}

void RSmfCredMgrClientSession::RequestAsyncService(
		TCredentialServerRequestID aRequestType, TRequestStatus& aStatus)
	{
	SendReceive(aRequestType, aStatus);
	}

void RSmfCredMgrClientSession::RequestService(
		TCredentialServerRequestID aRequestType, const TIpcArgs &aArgs)
	{
	TInt err = SendReceive(aRequestType, aArgs);

	}

