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
 * Header file for Credential Manager Server.
 */

#ifndef __SMFCREDMGRSERVER_H__
#define __SMFCREDMGRSERVER_H__

//  Include Files
#include <e32base.h>

// Forward declaration
class CSmfCredMgrDb;

//  Function Prototypes
GLDEF_C TInt E32Main();

// ----------------------------------------------------------------------------------------
// Server's policy
// ----------------------------------------------------------------------------------------
static const TUint rangeCount = 1;

static const TInt ranges[rangeCount] =
	{
	0
	};

static const TUint8 elementsIndex[rangeCount] =
	{
	CPolicyServer::EAlwaysPass
	};

static const CPolicyServer::TPolicy policy =
	{
	CPolicyServer::EAlwaysPass, //specifies all connect attempts should pass
			rangeCount,
			ranges,
			elementsIndex,
			NULL
	};

/**
 * Class for Credential Manager Server.
 * Derives from CPolicyServer.
 */
class CSmfCredMgrServer : public CPolicyServer
	{
public:
	/**
	 * NewL Method
	 * @return The constructed CSmfCredMgrServer instance
	 */
	static CSmfCredMgrServer * NewL();
	
	/**
	 * NewLC Method
	 * @return The constructed CSmfCredMgrServer instance
	 */
	static CSmfCredMgrServer* NewLC();

	/**
	 * Destructor
	 */
	~CSmfCredMgrServer();

	/**
	 * Creates a server-side session object.The session represents a 
	 * communication link between a client and a server, and its creation 
	 * is initiated by the client through a call to one of the 
	 * RSessionBase::CreateSession() variants.
	 */
	CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;

private:
	/**
	 * Constructor
	 */
	CSmfCredMgrServer();

	/**
	 * Two-phase constructor
	 */
	void ConstructL();

private:
	CSmfCredMgrDb* iDbCreator;
	};

#endif  // __SMFCREDMGRSERVER_H__
