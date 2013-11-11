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
// Contains declaration of CTestUPnPManager, CUPnPObserver, CUPnPTimer, 
// and CAction classes
// 
//

#ifndef __TEST_UPNP_MANAGER_H__
#define __TEST_UPNP_MANAGER_H__

// System Includes
#include <testexecutestepbase.h>
#include <e32base.h>
#include <e32cmn.h>
#include <e32std.h>
#include <upnpparamset.h>
#include <upnp/rpnpservicediscovery.h>
#include <upnp/rpnpservicepublisher.h>
#include <upnpparamset.h>
#include <upnp/mpnpobserver.h>
#include <upnp/pnpparameterbundle.h>
#include <uri8.h>
#include <inetprottextutils.h>
#include <f32file.h>
#include <es_sock.h>
#include <in_sock.h>
#include <commdbconnpref.h>
#include <e32des8.h>
#include <cinidata.h>
#include <upnpdescriptionschema.h>
#include <cstringpoolmanager.h>
#include <cupnpdescriptionparser.h>



// User Includes
#include "constants.h"


// Forward declarations
class CUPnPObserver;
class CAction;


/*
CTestUPnPManager class which is derived from the CTestStep and provides the
functionalities to perform Create (and open) an individual TestStep (testcase or part testcase), Run a TestStep
with defined pre and post processing, Abort a TestStep, Close a TestStep session.
Also performs the creation control point or service point, performs a sequence of upnp operations
and get back the results to compare them against expected results.
*/
class CTestUPnPManager : public CTestStep
	{
public:
	CTestUPnPManager();
	~CTestUPnPManager();

	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();

	void SetCurrentSection(const TDesC& aCurrentSection);
	TDesC& GetCurrentSection();
	
	// Operation  related functions
	void InitializeL();
	void SequenceOperationsL();
	void PerformDesiredOperationsL(const TDesC& aSequence);

	// UPnP API related functions
	TInt OpenPublisherL();
	void OpenMultiplePublisherL();	
	void OpenDiscovererL();
	void ClosePublisher();
	void DiscoverL(const TDesC& aOperationType);
	void PublishDeviceL(const TDesC& aOperationType);
	void PublishServiceL(const TDesC& aOperationType);
	void DescribeL();
	void DescribeServiceL();
	void DescribeIconL();
	void RegisterForAnnouncementsL(const TDesC& aOperationType);
	void InitiateActionL(const TDesC& aOperationType);
	void SubscribeForStateChangeNotificationsL();
	void ReSubscribeForStateChangeNotificationsL();
	TBool SendNotificationsL (CUPnPObserver* aObserver);

	// Cancellation related functions
	void CancelPublish();
	void CancelDiscoverL(const TDesC& aOperationType);
	void CancelNotifyAnnouncementL (const TDesC& aOperationType);
	void UnsubscribeForStateChangeNotificationsL(const TDesC& aOperationType);
	void CancelDescribeL(const TDesC& aOperationType);
	void CancelAllDeviceDescribeL();
	void CancelAllServiceDescribeL();

	// Cleanup related functions
	void CleanUpObserverArray();
	void CloseDiscoverer();
	void ScpdUriArrayCleanUp();
	void ControlUriArrayCleanUp();
	void EventUriArrayCleanUp();
	void DeleteSeriveUrlsList(TInt aIndex);
	void CleanUpResultsArray();

	// Utility functions
	void ResolveAllUrisL();
	void ResolveScpdUrisL(TInt aIndex);
	void ResolveEventUrisL(TInt aIndex);
	void ResolveControlUrisL(TInt aIndex);
	void AppendScpdUriL(RBuf8& aScpdUri);
	void AppendEventUriL(RBuf8& aEventUri);
	void AppendControlUriL(RBuf8& aControlUri);
	void DescriptionSuiteCleanUp();
	void GetServiceUrlsListL(TInt aIndex);
	void GetEmbeddedDeviceServicesL(TInt aIndex, CUPnPDevice* aDevice);
	void GetServiceValuesL(TInt aIndex, CUPnPDevice* aDevice);
	void ExtractServiceDescriptionL (const TDesC& aConfigSection, CUPnPServiceRegisterParamSet& aServiceRegisterParamSet);
	void TokenizeStringL(const TDesC& aString, RArray<TPtrC>& aList, TChar aSeparator = ',');
	void StartTimerL(const TDesC& aOperationType);
	void AppendBundlesL(RPnPParameterBundleBase& aPnPBundle);
	//void AppendBundlesL(CPnPParameterBundleBase* aPnPBundle);
	void AppendDeviceLocationL(TDesC8& aDevLocPointer);
	void AppendDescriptionL(RBuf8& aBaseUri,RBuf8& aDescription);
	TInt GetPublisherIndex (TInt aObserverIndex);
	TPublisherControl GetPublisherControl (TInt aObserverIndex);
	TInt NotifyCount();
	void SavePreviousLogFileL();
	const TDesC& ScriptFileName();
	
	void GetIconUrlsL (  TInt aBaseDescIdx, CUPnPDevice* aRootDevice );
	void ResolveIconUrlL ( TInt aBaseDescIdx, const TDesC8& aUrl );
	
	// Functions related to Server
	void  OpenServerL();
	void  ListenL();
	void  RecvDataL();
	void  SendDataL();

	// Functions related to Client
	void OpenSocketL();
	void DoConnectL();
	void SendClientDataL(RSocket& aSocket);
	void RecvClientDataL(RSocket& aSocket);

	// Print related functions
	//void PrintDiscoveryResultsL(CUPnPParameterFamily* aParamFamily);
	void PrintDiscoveryResultsL(RParameterFamily& aParamFamily);
	void PrintAndValidateResultsL(const TDesC& aOperationType);
	void SetDiscoveryStatus(TBool aSuccess);
	TBool IsDiscoverySuccess();
	// Validation related functions
	void ValidateDiscoveryResultsL(const TDesC& aOperationType);
	TPublisherControl PublisherType (TInt aPublisherIndex);
	
private:
	struct TPublisherObserverMap
		{
		TInt 				iPublisherIndex;
		TInt 				iObserverIndex;
		TPublisherControl	iPublisherControl;
		};

	struct TServiceUrl
		{
		RBuf8 serviceType;
		RBuf8 scpdUrl;
		RBuf8 controlUrl;
		RBuf8 eventSubUrl;
		};

	struct TDescriptionSuite
		{
		//Base Url for which Description is retrieved
		RBuf8 							iBaseUrl;
		//Description of the url
		RBuf8							iDescription;
		//Retrieved urls from description
		RArray<TServiceUrl> 			iServiceUrls;
		};
	
	RArray<TPublisherObserverMap>			iPublisherObserverMap;
	RArray<TDescriptionSuite>				iDescriptionSuiteArray;
	RStringPool								iStringPool;
    CStringPoolManager* 					iStringPoolMgr;
	RPnPServiceDiscovery					iControlPoint;
	RArray<RPnPServicePublisher>			iPublisherArray;
	RControlChannel							iControlChannel;
	RPointerArray<CUPnPObserver>	 		iObserverArray;
	RArray<RPnPParameterBundleBase>		iResultsArray;
	CIniData*								iCIniDataHandle;
	CAction*								iAction;
	TInt									iNotifyCount;
	TBool									iDiscoverySuccess;
public:
	CActiveScheduler*					iScheduler;
	TBuf<KMaxBufLength>					iCurrentSection;
	TBuf<KMaxBufLength> 				iScriptName;
	TBool								iExpectedData;
	RArray<TPtrC8>						iDeviceLocationArray;
	RArray<RBuf8>						iScpdUriArray;
	RArray<RBuf8>						iControlUriArray;
	RArray<RBuf8>						iEventUriArray;
	RArray<RBuf8>						iIconUriArray;
	TBool								iCancelDiscovery;
	TInt								iPublishCount;
	TInt								iDescribeResponseCount;
	TInt 								iRenewCount;
	TInt 								iFailedNumUris;
	
	RConnection							iConnection;
	RSocketServ							iSocketServ;
	};

/*
CPnPObserver class which is derived from CBase and MPnPObserver to provide the
following functionalities. It supports the methods capture the results when an
upnp event is hit after performing any upnp operation.
*/
class CUPnPObserver: public CBase, MPnPObserver
	{
public:
	static CUPnPObserver* NewL(CTestUPnPManager* aManager);
	~CUPnPObserver();
	void OnPnPEventL (RPnPParameterBundleBase& aServiceEventInfo);
	void OnPnPError (TInt aError);
	CTestUPnPManager& Manager();
	void CopyResultBundlesL(const RPnPParameterBundleBase& aPnpBundle);
	void SequenceOperationsL();
	void ExecuteL(const TDesC& aSequence);

private:
	CUPnPObserver();
	void ConstructL(CTestUPnPManager* aManager);

private:
	CTestUPnPManager* 					iManager;
	TUint								iSequence;
	TInt 								iResultsCount;
	RFs 								iFileServ;
	};

class TestUtils
	{
public:
	void static LoadESockL ( RSocketServ& aSocketServ, RConnection& aConnection );
	};

/*
CUPnPTimer class is derived from CActive. It to provides a Timer AO
*/
class CUPnPTimer:public CActive
	{
public:
	static CUPnPTimer* NewL (CTestUPnPManager* aManager);
	~CUPnPTimer();
	void IssueRequestL(TTimeIntervalMicroSeconds32 anInterval);

	void RunL();
	void DoCancel();

private:
	CUPnPTimer(CTestUPnPManager* aManager);
	void ConstructL();

private:
	RTimer iTimer;
	CTestUPnPManager* 	iManager;
	};



class CAction : public CActive
	{
public:
	enum TActionState
		{
		ESendData,
		EReceiveData
		};
	static CAction* NewL(CTestUPnPManager* aManager);
	~CAction();
	void RunL()	;
	void InitiateActionControlL(TDesC8& aUri,RPnPServiceDiscovery& aControlPoint);

private:
	CAction(CTestUPnPManager* aManager);
	void ConstructL();
	void DoCancel();

private:
	CTestUPnPManager* 	iManager;
	RControlChannel 	iControlChannel;
	TActionState 		iActionState;
	TControlMessage 	iCtrlMsg;
	RBuf8 				iIncomingActionBuffer;
    RBuf8 				iOutgoingActionBuffer;
    RBuf8 				iCompleteBuffer;
	RFs 				iFs;
    RFile				iFile;
	};

#endif //__TEST_UPNP_MANAGER_H__

