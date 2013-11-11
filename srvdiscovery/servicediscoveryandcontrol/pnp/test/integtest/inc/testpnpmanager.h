// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Contains declaration of CTestPnPManager, CTestPnPObserver 
// 
//

#ifndef __TEST_PNP_MANAGER_H__
#define __TEST_PNP_MANAGER_H__

// System Includes
#include <testexecutestepbase.h>
#include <e32base.h>
#include <e32cmn.h>
#include <e32std.h>
#include <pnp/rpnpservicediscovery.h>
#include <pnp/rpnpservicepublisher.h>
#include <pnp/mpnpobserver.h>
#include <pnp/pnpparameterbundle.h>
#include <e32des8.h>


// User Includes
#include "constants.h"
#include "ctestcontrolchannel.h"
#include "testpnpparamset.h"

// Forward declarations
class CTestPnPObserver;
class CTestTimer;

class MTimerObserver
	{
public:
	virtual void TimedOut() = 0;		
	};

/*
CTestPnPManager class which is derived from the CTestStep and provides the
functionalities to perform Create (and open) an individual TestStep (testcase or part testcase), Run a TestStep
with defined pre and post processing, Abort a TestStep, Close a TestStep session.
Also performs the creation control point or service point, performs a sequence of pnp operations
and get back the results to compare them against expected results.
*/
class CTestPnPManager : public CTestStep,
						public MTimerObserver
	{
public:
	CTestPnPManager();
	~CTestPnPManager();

	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();

	void SetCurrentSection(const TDesC& aCurrentSection);
	TDesC& GetCurrentSection();
	
	// Operation  related functions
	void InitializeL();
	void SequenceOperationsL();
	void PerformDesiredOperationsL(const TDesC& aSequence);

	// TestPnP API related functions
	void OpenPublisherL();	
	void OpenDiscovererL();
	void DiscoverL();
	void PublishServiceL(const TDesC& aOperationType);
	void DescribeL();
	void DescribeServiceL();
	void RegisterForAnnouncementsL();
	void InitiateActionL();
	void SubscribeForStateChangeNotificationsL();
	void SendNotificationsL();
    void OpenDiscovererFailed();
	void OpenPublisherFailed();
	
	// Cancellation related functions
	void CancelDiscoverL();
	void CancelNotifyAnnouncementL ();	
	void CancelDescribeL();

	// Utility functions
	void TokenizeStringL(const TDesC& aString, RArray<TPtrC>& aList, TChar aSeparator = ',');
	const TDesC& ScriptFileName();
	
	// from MTimerObserver
	void TimedOut();

	
private:
    
    CActiveScheduler*					    iScheduler;
	RPnPServiceDiscovery					iControlPoint;
	RPnPServicePublisher   		        	iServicePoint;
	RControlChannel							iControlChannel;	
	TControlMessage                         iMsg;
	TRequestStatus                          iStatus;	

private:	
	TBuf<KMaxBufLength>					iCurrentSection;
	TBuf<KMaxBufLength> 				iScriptName;
	TBool								iCancelDiscovery;
	
	};

/*
CTestPnPObserver class which is derived from CBase and MPnPObserver to provide the
following functionalities. It supports the methods capture the results when an
pnp event is hit after performing any pnp operation.
*/
class CTestPnPObserver: public CBase, MPnPObserver
	{
public:
	static CTestPnPObserver* NewL(CTestPnPManager* aManager);
	~CTestPnPObserver();
	void OnPnPEventL (RPnPParameterBundleBase& aServiceEventInfo);
	void OnPnPError (TInt aError);
	CTestPnPManager& Manager();
	void SequenceOperationsL();

private:
	CTestPnPObserver();
	void ConstructL(CTestPnPManager* aManager);

private:
	  CTestPnPManager* 					iManager;
	  TUint								iSequence;
	};
	

class CTestTimer:public CActive
	{
public:
	static CTestTimer* NewL(MTimerObserver& aObserver);
	~CTestTimer();
	void After(TTimeIntervalMicroSeconds32 aInterval);
	
private:
	CTestTimer(MTimerObserver& aObserver);
			
public:
	void RunL();
	void DoCancel();
	
private:
	RTimer		iTimer;
	MTimerObserver& iObserver;	
	};	


#endif //__TEST_PNP_MANAGER_H__

