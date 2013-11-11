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

#if ( !defined __TESTSUITECHUNKMGR_H__ )
#define __TESTSUITECHUNKMGR_H__

#include <e32base.h>
#include <test/testexecuteserverbase.h>


class CChunkMgrTestSuite: public CTestServer
	{
public:
	static CChunkMgrTestSuite* NewL ();
	~CChunkMgrTestSuite ();

	CTestStep* CreateTestStep ( const TDesC& aStepName );
	
private:
	const TPtrC  ServerName ();
	
private:
	CTestStep *iTestStep;
	};
	
//  Function Prototypes
GLDEF_C TInt E32Main ();


#endif /* __TESTSUITECHUNKMGR_H__ */
