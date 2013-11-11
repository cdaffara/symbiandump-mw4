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


#if (!defined __TESTSERVERBASE_H__)
#define __TESTSERVERBASE_H__

#include <es_sock.h>
#include <testexecuteserverbase.h>

_LIT(KServerName,"te_serverbase");

class CTestServerBase : public CTestServer	//this is the main test server which instantiates the test step classes 
	{
	public:
		static CTestServer* NewL ();
		CTestServerBase ();
		~CTestServerBase ();
		
		CTestStep* CreateTestStep ( const TDesC& aStepName );
		
	private:
	};

#endif /* __TESTSERVERBASE_H__ */
