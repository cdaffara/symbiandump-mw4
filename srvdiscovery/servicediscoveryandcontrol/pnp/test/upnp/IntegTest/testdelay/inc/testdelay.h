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
// testupnpmanager.h
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

public:
	CActiveScheduler*					iScheduler;
	TRequestStatus 						iStatus;	
	};
		
/**
CUPnPTimer class is derived from CActive. It to provides a Timer AO
@file
@internalTechnology
*/	
class CUPnPTimer:public CActive
	{
	public:
		static CUPnPTimer* NewL ();
		~CUPnPTimer(); 
		void IssueRequestL(TTimeIntervalMicroSeconds32 anInterval);
		TBool PendingRequest();	

		void RunL();
		void DoCancel(); 

	private:
		CUPnPTimer();
		void ConstructL();
	
	private:
		RTimer iTimer;
		TBool iRequestPending;
	}; 
	

#endif //__TEST_UPNP_MANAGER_H__
