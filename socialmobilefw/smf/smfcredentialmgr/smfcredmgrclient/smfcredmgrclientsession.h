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
#ifndef RSMFCREDMGRCLIENTSESSION_H_
#define RSMFCREDMGRCLIENTSESSION_H_

#ifdef Q_OS_SYMBIAN
#include <smfcredmgrcommon.h>
#include <e32std.h>
#include <e32std.h>
#include <e32cmn.h>
#endif

/**
 * Class for client side handle and to create a channel with the server. 
 * Derives from RSessionBase.
 */
class RSmfCredMgrClientSession : public RSessionBase
	{
public:
	/**
	 * Constructor
	 */
	RSmfCredMgrClientSession();

	/**
	 * Connection to server to be made using CreateSession()
	 */
	TInt connectToServer();

	/**
	 * Method to retrieve the Version of the Server
	 */
	TVersion Version() const;

	/**
	 * Close the session
	 */
	void Close();

	/**
	 * Calls SendReceive with requestType opcode and packaged data
	 * depending on requestType
	 */
	void RequestAsyncService(TCredentialServerRequestID aRequestType,
			TRequestStatus& aStatus);

	/**
	 * Issue request to server using SendReceive()
	 */
	void RequestService(TCredentialServerRequestID aRequestType,
			const TIpcArgs &aArgs);

	};

#endif /* RSMFCREDMGRCLIENTSESSION_H_ */
