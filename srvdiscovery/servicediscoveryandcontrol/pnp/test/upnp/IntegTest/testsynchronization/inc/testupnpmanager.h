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
// Contains implementation of CTestUPnPManager class
// 
//

#ifndef __TEST_UPNP_MANAGER_H__
#define __TEST_UPNP_MANAGER_H__

// System Includes
#include <testexecutestepbase.h>
#include <e32base.h>
#include <e32cmn.h>
#include <e32std.h>
#include <es_sock.h>
#include <in_sock.h>
#include <commdbconnpref.h>
#include <e32des8.h>


// Constant literals
_LIT(KTestUPnPManager, "TestUPnPManager");

/**
CTestUPnPManager class which is derived from the CTestStep and TTestUPnPUtilities provides the 
functionalities to perform Create (and open) an individual TestStep (testcase or part testcase), Run a TestStep
with defined pre and post processing, Abort a TestStep, Close a TestStep session.
Also performs the creation of threads to run as control point or service point, performs a sequence of upnp operations 
and get back the results to compare them against expected results.
@file
@internalTechnology
*/
class CTestUPnPManager : public CTestStep
	{
public:
	CTestUPnPManager();
	~CTestUPnPManager();

	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
	
private:
	void  InitializeL();
	void  ConnectClientL();
	void  ConnectServerL();	

public:
	CActiveScheduler*					iScheduler;
	TRequestStatus 						iStatus;	
	};


#endif //__TEST_UPNP_MANAGER_H__