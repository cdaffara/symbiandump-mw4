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
// testcontrolchannel.h
// Contains declarations of CTestRControlChannel class
// 
//

#ifndef __TEST_RCONTROL_CHANNEL_H__
#define __TEST_RCONTROL_CHANNEL_H__

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
#include <upnp/rcontrolchannel.h>


// User Includes
#include "constants.h"

class CRControlChannelObserver;
class CUPnPRControlChannelAO;

/*
CTestRControlChannel class which is derived from the CTestStep and provides the
functionalities to perform Create (and open) an individual TestStep (testcase or part testcase), Run a TestStep
with defined pre and post processing, Abort a TestStep, Close a TestStep session.
Also performed to run as control point or service point, performs a sequence of upnp operations
and get back the results to compare them against expected results.
*/
class CTestRControlChannel : public CTestStep
	{
public:
	CTestRControlChannel();
	~CTestRControlChannel();

	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();


private:
	void InitializeL();
	void SequenceOperationsL();
	void PerformDesiredOperationsL(const TDesC& aSequence);

	void OpenPublisherL();
	void PublishDeviceL (TDesC& aOperationType);
	void PublishServiceL (const TDesC& aOperationType);
	void ExtractServiceDescriptionL (const TDesC& aConfigSection, CUPnPServiceRegisterParamSet& aServiceRegisterParamSet);
	void TokenizeStringL(const TDesC& aString, RArray<TPtrC>& aList, TChar aSeparator = ',');
	void SendNotificationsL();
	void InitiateControlL();
	void CleanUpObserverArray();
	void SavePreviousLogFileL();
	
private:
	TBuf<KMaxBufLength> 				iScriptName;
	RPnPServicePublisher				iPublisher;
	RPointerArray<CRControlChannelObserver>	 	iObserverArray;
	CPeriodic*			             	iTimer;
	CActiveScheduler*					iScheduler;
	
	RConnection							iConnection;
	RSocketServ							iSocketServ;
	};

/*
CRControlChannelObserver class which is derived from CBase and MPnPObserver to provide the
following functionalities. It supports the methods capture the results when an
upnp event is hit after performing any upnp operation.
*/
class CRControlChannelObserver: public CBase, MPnPObserver
	{
public:
	static CRControlChannelObserver* NewL(CTestRControlChannel* aManager);
	~CRControlChannelObserver();
	void OnPnPEventL (RPnPParameterBundleBase& aServiceEventInfo);
	void OnPnPError (TInt aError);
	CTestRControlChannel& Manager();
	void CopyResultBundlesL(const RPnPParameterBundle& aPnpBundle);
private:
	CRControlChannelObserver();
	void ConstructL(CTestRControlChannel* aManager);

private:
	CTestRControlChannel* 					iManager;
	};


class CUPnPRControlChannelAO:public CActive
    {
public:
     
     CUPnPRControlChannelAO(RControlChannel& aControlChannel,CTestRControlChannel* aManager);
     ~ CUPnPRControlChannelAO(); 
     void ExtractSoapResponse(RBuf8& aSoapResponse);        
	 void RecvdataL();
	 void SenddataL();
	 void RunL();
	 void DoCancel();     
    static TInt TimerCallbackL ( TAny* aPtr );
    
private:
     enum Tstate
	 {
    	 ERecv,
    	 EStop
	 };
         
     Tstate iState;
     RControlChannel& iControlChannel;
     TControlMessage 		iInComingAction;
     RBuf8 iIncomingActionBuffer;
     RBuf8 iOutgoingActionBuffer;
     RBuf8 iCompleteBuffer;
     TInt  iExepecteBufLen;
	CTestRControlChannel* 	iManager; 
	CPeriodic*				iTimer;
    };

#endif //__TEST_RCONTROL_CHANNEL_H__
