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
// Contains implementation of CTestPnPManager class
// Contains implemenatation of CTestPnPManager, CTestPnPObserver, CTestPnPTimer, 
// and CAction classes
// 
//

#include "testpnpmanager.h"


/*
  Constructor:
  @test
*/
CTestPnPManager::CTestPnPManager()
:	CTestStep()
	{
	iCancelDiscovery = EFalse;
	}

/*
   Initializes all member vaiables.
   @param		None.
   @return		None.
*/
void CTestPnPManager::InitializeL()
	{
    iScheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iScheduler);
	}

/*
   Destructor
   @test
*/
CTestPnPManager::~CTestPnPManager()
	{
	_LIT(KInfoDesctructorBegin, "Begin: In Destructor CTestPnPanager....\n");
	INFO_PRINTF1(KInfoDesctructorBegin);
	delete iScheduler;
	iScheduler = NULL;	
	iControlChannel.Close();	
	iControlPoint.Close();
	iServicePoint.Close();
	}



/*
   Implementation of CTestStep base class virtual and this is used for doing all
   initialisation common
    to derived classes here.
   @param  None
   @return TVerdict
 */
TVerdict CTestPnPManager::doTestStepPreambleL()
	{
    InitializeL();
	return CTestStep::doTestStepPreambleL();
	}


/*
 return Script File name..if any
*/
const TDesC& CTestPnPManager::ScriptFileName()
	{
	return iScriptName;
	}


/*
   Implementation of CTestStep base class virtual and it is used for doing all
   after test treatment common to derived classes in here.
   @return TVerdict
 */
TVerdict CTestPnPManager::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL();
	}


TVerdict CTestPnPManager::doTestStepL()
	{
	SequenceOperationsL();
	return TestStepResult();
	}

/*
   This function performs operations based on sequence specified in the ini file
   @param  None
   @return None
*/
void CTestPnPManager::SequenceOperationsL()
	{
	_LIT(KInfoLogFile, "SequenceOperationsL().... \n");
	INFO_PRINTF1(KInfoLogFile);
	RArray<TPtrC> sequenceList;
	CleanupClosePushL(sequenceList);
	TPtrC listOfSequences;
	_LIT(KListOfSequences, "listofsequences");
	GetStringFromConfig(ConfigSection(), KListOfSequences, listOfSequences);

	TokenizeStringL(listOfSequences, sequenceList);
	TInt numOperations = sequenceList.Count();

	for(TInt i(0); i < numOperations; i++)
		{
		SetCurrentSection(sequenceList[i]);
		PerformDesiredOperationsL(sequenceList[i]);
		}
	CleanupStack::PopAndDestroy(); 
	_LIT(KInfoLogFile1, "SequenceOperationsL() End.... \n");
	INFO_PRINTF1(KInfoLogFile1);
	}


/*
   This function performs desired operations specified as a parameter
   @param  aOperation string representing operation to be performed
   @return None
*/
void CTestPnPManager::PerformDesiredOperationsL(const TDesC& aOperation)
	{
	const TInt KMatch = 0;
	TPtrC operationType;
	_LIT(KOperationType, "operationtype");
	GetStringFromConfig(aOperation, KOperationType, operationType);
	
	if(operationType.Compare(KOpenDiscoverer) == KMatch)
		{
		OpenDiscovererL();
		}
	else if(operationType.Compare(KOpenPublisher) == KMatch)
		{
		OpenPublisherL();
		}
	else if(operationType.Compare(KDiscovery) == KMatch)
		{
		DiscoverL();
		}
	else if(operationType.Compare(KCancelDiscover) == KMatch)
		{
		CancelDiscoverL();
		}
	else if(operationType.Compare(KDescribe) == 0)
	    {
    	DescribeServiceL();
    	}
    else if(operationType.Compare(KCancelDescribe) == KMatch)
		{
		CancelDescribeL();
		}
	else if(operationType.Compare(KPublishService) == KMatch)
		{
		PublishServiceL(aOperation);
		}
	else if(operationType.Compare(KSubscribe) == KMatch)
		{
		SubscribeForStateChangeNotificationsL();
		}
	else if(operationType.Compare(KRegisterAnnouncement) == KMatch)
		{
		RegisterForAnnouncementsL();
		}
	else if(operationType.Compare(KCancelNotify) == KMatch)
		{
		CancelNotifyAnnouncementL();
		}
	else if(operationType.Compare(KNotify) == KMatch)
		{
	    SendNotificationsL();
		}
	else if(operationType.Compare(KOpenDiscovererFail) == KMatch)
	   {
	   OpenDiscovererFailed();
	   }
    else if(operationType.Compare(KOpenPublisherFail) == KMatch)
	   {
	   OpenPublisherFailed();   
	   }
	}

/*
   Parses a comma separated string and constructs a list out of the values
   @param	aString a reference to a string to be tokenized
   @param	aList is an out parameter to store a list of tokens in an arry
   @param	aSeparator is a character used to delimit the tokens
  */
void CTestPnPManager::TokenizeStringL(const TDesC& aString, RArray<TPtrC>& aList, TChar aSeparator)
	{
	TLex lexer(aString);

	while(!lexer.Eos())
		{
		lexer.SkipSpaceAndMark();

		while(!lexer.Eos() && lexer.Get() != aSeparator)
			{
			}

		if(!lexer.Eos())
			{
			lexer.UnGet();
			}
		aList.AppendL(lexer.MarkedToken());
		lexer.Inc();
		}
	}


/*
   Sets the current config section.
   @param	aCurrentSection - a reference string that represent current config section
   @return	None
 */
void CTestPnPManager::SetCurrentSection(const TDesC& aCurrentSection)
	{
	iCurrentSection = aCurrentSection;
	}

/*
   Retrieve the current config section.
   @param
   @return	Reference to current config section
 */
TDesC& CTestPnPManager::GetCurrentSection()
	{
	return	iCurrentSection;
	}

/*
	Tests whether discovering services/devices is performed
	@param		aOperationType is reference to a section name in ini file where required parameters
				needs to be referred for this operation.
	@return		None.
 */
void CTestPnPManager::DiscoverL()
	{
	RArray<TPtrC> serviceTypeList;
	TPtrC listOfServiceTypes;
	_LIT(KListOfServiceTypes, "listofservicetypes");
	_LIT(KInfoLogFile, "DiscoverL.... \n");
	INFO_PRINTF1(KInfoLogFile);
	CTestPnPObserver* pnpObserver = CTestPnPObserver::NewL(this);
	
	RPnPParameterBundle pnpBundle;
	pnpBundle.Open ();
	CleanupClosePushL( pnpBundle );
	pnpBundle.SetPnPObserver((MPnPObserver*)pnpObserver);

	RParameterFamily pnpFamily = pnpBundle.CreateFamilyL (ETestPnPDiscoverRequestParamSet);
	if(!GetStringFromConfig(GetCurrentSection(), KListOfServiceTypes, listOfServiceTypes))
		{
		_LIT(KErrInfo, "Problem in reading values from ini. \n Expected fields are: \n%S \n");
		ERR_PRINTF2( KErrInfo, &KListOfServiceTypes );
		return;
		}

	TokenizeStringL(listOfServiceTypes, serviceTypeList);
	TInt numUris = serviceTypeList.Count();

	serviceTypeList.Close();
	iControlPoint.Discover(pnpBundle);

	_LIT(KInfoLogFile1, "DiscoverL End... \n");
	INFO_PRINTF1(KInfoLogFile1);
	CleanupStack::PopAndDestroy( &pnpBundle );
	}


/*
	Tests whether Publishing a Service/Device is performed
	@param			aOperationType is reference to a section name in ini file where required parameters
					needs to be referred for this operation.
	@return			None.
 */
void CTestPnPManager::PublishServiceL (const TDesC& aOperationType)
	{
	_LIT(KInfoLogFile, "PublishServiceL().... \n");
	INFO_PRINTF1(KInfoLogFile);
	CTestPnPObserver* pnpObserver = CTestPnPObserver::NewL(this);
	RPnPParameterBundle pnpBundle ;
	pnpBundle.Open();
	
    CleanupClosePushL( pnpBundle );
    pnpBundle.SetPnPObserver((MPnPObserver*)pnpObserver);
    RParameterFamily family = pnpBundle.CreateFamilyL(ETestPnPServiceRegisterParamSet);
    
    TPtrC serviceType;
	GetStringFromConfig(aOperationType, KServiceType, serviceType);
	RBuf8 serviceTypeBuf;
	serviceTypeBuf.Create(serviceType.Length());
	serviceTypeBuf.Copy(serviceType);
	
	iServicePoint.Publish( pnpBundle );
    serviceTypeBuf.Close();
    
	_LIT(KInfoLogFile1, "PublishServiceL() End.... \n");
	INFO_PRINTF1(KInfoLogFile1);
	CleanupStack::PopAndDestroy( &pnpBundle );
	}


void CTestPnPManager::SendNotificationsL ()
	{
	_LIT(KInfoLogFile, "SendNotificationsL.... \n");
	INFO_PRINTF1(KInfoLogFile);
 
    RPnPParameterBundle pnpBundle;
    pnpBundle.CreateL();   
    CleanupClosePushL( pnpBundle );

	RParameterFamily pnpFamily = pnpBundle.CreateFamilyL(ETestPnPServiceNotificationParamSet);

	iServicePoint.SendNotify( pnpBundle );
    _LIT(KInfoLogFile1, "SendNotificationsL End.... \n");
	INFO_PRINTF1(KInfoLogFile1);
	CleanupStack::Pop( &pnpBundle );	
	}


/*	Opens a handle to a RPnPServiceDiscovery object
	@param			None.
	@return			None.
 */
void CTestPnPManager::OpenDiscovererL()
	{
	_LIT(KInfoLogFile, "OpenDiscovererL().... \n");
	INFO_PRINTF1(KInfoLogFile);
	
	TBool cpStatus = EFalse;
	TInt openStatus = 0;
	_LIT(KCpStatus, "cpstatus");
	GetBoolFromConfig(GetCurrentSection(), KCpStatus, cpStatus);
	if(!cpStatus)
		{
		_LIT(KInfoDiscoverer, "Control Point should not be Created \n");
		INFO_PRINTF1(KInfoDiscoverer);
		return;
		}

	openStatus = iControlPoint.Open(KTestTierId);
	
	if(openStatus != KErrNone) 
		{
		_LIT(KOpenDiscovererFailed, "Control Point discovery open failed with error %d ");
		ERR_PRINTF2(KOpenDiscovererFailed, openStatus);
		SetTestStepError(openStatus);
		}
	else
		{
		_LIT(KOpenDiscovererSuccess, "Control Point Discovery was successfully started with return value %d :");
		INFO_PRINTF2(KOpenDiscovererSuccess, openStatus);
		}
		
	_LIT8(KUri, "Prakash");
	TBuf8<255> buf(KUri);
	iControlPoint.InitiateControl(iControlChannel,buf);
	iMsg.SetMessagePtr(buf);
    TPtrC8 iPtr=iMsg.MessagePtr();	
    if ( !iMsg.IsLast () )
	      {
	      iMsg.ClearFlags();
	      iControlChannel.Send(iMsg,iStatus);
	      CTestTimer* testTimer = CTestTimer::NewL(*this);
	      testTimer->After(2000000);
	      CActiveScheduler::Start();
	      delete testTimer;
		  }
	}


/*
	Opens a handle to a RPnPServicePublisher object 
	@param			None.
	@return			TBool representing success or failure of the operation.
 */
void CTestPnPManager::OpenPublisherL()
	{
	_LIT(KInfoLogFile, "OpenPublisherL().... \n");
	INFO_PRINTF1(KInfoLogFile);
	
	TBool spStatus = EFalse;
	TInt openStatus = 0;
	_LIT(KSpStatus, "spstatus");
	GetBoolFromConfig(GetCurrentSection(), KSpStatus, spStatus);

	openStatus = iServicePoint.Open(KTestTierId);

  	if( openStatus != KErrNone )
  		{
 		_LIT(KOpenDiscovererFailed, "Publisher open failed with error %d ");
 		ERR_PRINTF2(KOpenDiscovererFailed, openStatus);
 		SetTestStepError(openStatus);
 		}
 	else
 		{
 		_LIT(KOpenPublisherSuccess, "Service publisher was successfully started with return value %d :");
  		INFO_PRINTF2(KOpenPublisherSuccess, openStatus);
 		}
 		
 	iServicePoint.InitiateControl(iControlChannel);
	_LIT8(KUri, "Rajesh");
	TBufC8<255> buf(KUri);	
	iMsg.SetMessageDes ( buf );    
    iMsg.SetLast ();
    TPtrC8 iPtr= iMsg.MessageDes();
	TInt newLen = buf.Length();
	iMsg.SetMaxLength (newLen);
	TInt maxLen =iMsg.MaxLength();
	iControlChannel.Recv(iMsg,iStatus);
    CTestTimer* testTimer = CTestTimer::NewL(*this);
	testTimer->After(2000000);
	CActiveScheduler::Start();
	delete testTimer;
	}
	

/*
Test for Register announcements ssdp:alive and ssdp:byebye messages
	@param			aOperationType is reference to a section name in ini file where required parameters
					needs to be referred for this operation.
	@return			None.
*/
void  CTestPnPManager::RegisterForAnnouncementsL ()
	{
	_LIT(KInfoLogFile, "RegisterForAnnouncementsL().... \n");
	INFO_PRINTF1(KInfoLogFile);
	RArray<TPtrC> uriTypeList;
	TPtrC listOfUriTypes;
	_LIT(KListOfUriTypes, "listofuritypes");
	CTestPnPObserver* pnpObserver = CTestPnPObserver::NewL(this);
	
	RPnPParameterBundle pnpBundle;
	pnpBundle.CreateL ();
	CleanupClosePushL( pnpBundle );
	pnpBundle.SetPnPObserver((MPnPObserver*)pnpObserver);
	RParameterFamily pnpFamily = pnpBundle.CreateFamilyL (ETestPnPRegisterAnnouncementParamSet);
	
	if(!GetStringFromConfig(GetCurrentSection(), KListOfUriTypes, listOfUriTypes))
		{
		_LIT(KErrInfo, "Problem in reading values from ini. \n Expected fields are: %S \n");
		ERR_PRINTF2( KErrInfo, &KListOfUriTypes);
		return;
		}

	TokenizeStringL(listOfUriTypes, uriTypeList);
	TInt numUris = uriTypeList.Count();

	uriTypeList.Close();
	iControlPoint.RegisterNotify( pnpBundle );
	CleanupStack::PopAndDestroy( &pnpBundle );
	_LIT(KInfoLogFile1, "RegisterForAnnouncementsL() End.... \n");
	INFO_PRINTF1(KInfoLogFile1);	
	}

/*
	Tests whether subscribtion to event change notifications is performed
	@param			None.
	@return			None.
 */
void  CTestPnPManager::SubscribeForStateChangeNotificationsL()
	{
	_LIT(KInfoLogFile, "SubscribeForStateChangeNotificationsL() start.... \n");
	INFO_PRINTF1(KInfoLogFile);
	CTestPnPObserver* pnpObserver = CTestPnPObserver::NewL(this);
	CleanupStack::PushL( pnpObserver );
	RPnPParameterBundle pnpBundle;
	pnpBundle.CreateL ();
	CleanupClosePushL( pnpBundle );
	pnpBundle.SetPnPObserver((MPnPObserver*)pnpObserver);
	RParameterFamily pnpFamily = pnpBundle.CreateFamilyL (ETestPnPSubscribeRequestParamSet);

	iControlPoint.Subscribe( pnpBundle );
	CleanupStack::PopAndDestroy( &pnpBundle );
	CleanupStack::PopAndDestroy( pnpObserver );
	_LIT(KInfoLogFile1, "SubscribeForStateChangeNotificationsL() End.... \n");
	INFO_PRINTF1(KInfoLogFile1);
	}


/*	Test for Cancelling Discovery Operation
	@param			aOperationType is reference to a section name in ini file where required parameters
					needs to be referred for this operation.
	@return			None.
*/
void CTestPnPManager::CancelDiscoverL()
	{
	iCancelDiscovery = ETrue;
	_LIT(KInfoLogFile, "CancelDiscoverL().... \n");
	INFO_PRINTF1(KInfoLogFile);
	
	CTestPnPObserver* pnpObserver = CTestPnPObserver::NewL(this);
	
	RPnPParameterBundle pnpBundle;
	pnpBundle.CreateL ();
	CleanupClosePushL( pnpBundle );
	pnpBundle.SetPnPObserver((MPnPObserver*)pnpObserver);

	RParameterFamily pnpFamily = pnpBundle.CreateFamilyL (ETestPnPCancelDiscoverParamSet);

	iControlPoint.Cancel ( pnpBundle );
	CleanupStack::PopAndDestroy( &pnpBundle );
	
	_LIT(KInfoLogFile1, "CancelDiscoverL() End.... \n");
	INFO_PRINTF1(KInfoLogFile1);
	}

void CTestPnPManager::CancelDescribeL()
	{
	_LIT(KInfoLogFile, "CancelDescribeL().... \n");
	INFO_PRINTF1(KInfoLogFile);
	CTestPnPObserver* pnpObserver = CTestPnPObserver::NewL(this);
	
	RPnPParameterBundle pnpBundle;
	pnpBundle.CreateL ();
	CleanupClosePushL( pnpBundle );
	pnpBundle.SetPnPObserver((MPnPObserver*)pnpObserver);

	RParameterFamily pnpFamily = pnpBundle.CreateFamilyL (ETestPnPCancelDescribeParamSet);

	iControlPoint.Cancel ( pnpBundle );
	CleanupStack::PopAndDestroy( &pnpBundle );
	_LIT(KInfoLogFile1, "CancelDescribeL() End.... \n");
	INFO_PRINTF1(KInfoLogFile1);
	}



/*	Test for Cancelling Register Announcement
	@param			aOperationType is reference to a section name in ini file where required parameters
					needs to be referred for this operation.
	@return			None.
 */
void  CTestPnPManager::CancelNotifyAnnouncementL()
	{
	_LIT(KInfoLogFile, "CancelNotifyAnnouncementL().... \n");
	INFO_PRINTF1(KInfoLogFile);
	CTestPnPObserver* pnpObserver = CTestPnPObserver::NewL(this);
		
	RPnPParameterBundle pnpBundle;
	pnpBundle.CreateL ();
	CleanupClosePushL( pnpBundle );
	pnpBundle.SetPnPObserver((MPnPObserver*)pnpObserver);
	RParameterFamily pnpFamily = pnpBundle.CreateFamilyL ( ETestPnPCancelRegisterNotifyParamSet );
	
	iControlPoint.Cancel ( pnpBundle);
	CleanupStack::PopAndDestroy( &pnpBundle );
	_LIT(KInfoLogFile1, "CancelNotifyAnnouncementL() End.... \n");
	INFO_PRINTF1(KInfoLogFile1);
	}

/*
  Constructor:
*/
CTestPnPObserver::CTestPnPObserver()
	:iSequence(0)
	{
	}

/*
  Destructor
*/
CTestPnPObserver::~CTestPnPObserver()
	{
	}


/*
	Static factory constructor. Uses two phase construction and leaves nothing on the
	CleanupStack. Creates a CTestControlPointService object.
	@param			aManager	A pointer to a CTestPnPManager object.
	@return			A pointer to the newly created CPnPObserver object.
	@post			A fully constructed and initialized CPnPObserver object.
 */
CTestPnPObserver* CTestPnPObserver::NewL(CTestPnPManager* aManager)
	{
	CTestPnPObserver* self = new(ELeave) CTestPnPObserver;
	CleanupStack::PushL(self);
	self->ConstructL(aManager);
	CleanupStack::Pop(self);
	return self;
	}

/*
	Second phase of two-phase construction method. Does any allocations required to fully construct
	the object.
	@pre 		First phase of construction is complete.
	@param		aManager	A pointer to a CTestPnPManager object.
	@post		The object is fully constructed and initialized.
 */
void CTestPnPObserver::ConstructL(CTestPnPManager* aManager)
	{
	iManager = aManager;
	}



/*
   This method returns a reference to the CTestPnPManager object.
   @param None
   return a reference to the CTestPnPManager object
 */
CTestPnPManager& CTestPnPObserver::Manager()
	{
	return *iManager;
	}

/*
   This is called when callback hits. And this method stores the bundle results
   and meta information related to that results into results array maintained by manager
   for later verification.
   @param  aParameterBundle a reference to CPnPParameterBundleBase
   @return None
 */
void CTestPnPObserver::OnPnPEventL ( RPnPParameterBundleBase& aParameterBundle)
	{
	_LIT(KInfoLogFile, "OnPnPEventL ().... \n");
	Manager().INFO_PRINTF1(KInfoLogFile);

	RParameterFamily paramFamily = aParameterBundle.GetFamilyAtIndex(0);
	
	if( paramFamily.IsNull ())
		{
		switch ( paramFamily.Id() )
			{
			case ETestPnPDiscoverResponseParamSet:
				{				
				_LIT(KInfoLogFile, "Discovery Response\n \n");
	            Manager().INFO_PRINTF1(KInfoLogFile);
				}
			break;
			
			case ETestPnPPresenceAnnouncementParamSet:
				{
				_LIT(KInfoLogFile, "Presence Announcement Response\n");
	            Manager().INFO_PRINTF1(KInfoLogFile);
				}
			break;

			case ETestPnPAbsenceAnnouncementParamSet:
				{				
				_LIT(KInfoLogFile, "Absence Announcement Response\n");
	            Manager().INFO_PRINTF1(KInfoLogFile);
				}
			break;

			case ETestPnPPublishResponseParamSet:
				{
                _LIT(KInfoLogFile, "Publish Response\n");
	            Manager().INFO_PRINTF1(KInfoLogFile);
				}
			break;
			
			case ETestPnPDescribeResponseParamSet:
				{							
				_LIT(KInfoLogFile, "Describe Response\n");
	            Manager().INFO_PRINTF1(KInfoLogFile);
			    }
			break;
			
			case ETestPnPSubscribeResponseParamSet:
				{				
                _LIT(KInfoLogFile, "Subscribe Response\n");
	            Manager().INFO_PRINTF1(KInfoLogFile);
             	}
			break;
			
			case ETestPnPNotifyEventParamSet:
				{//receives event notifications sent at client
				_LIT(KInfoLogFile, "Notify Event Response\n");
	            Manager().INFO_PRINTF1(KInfoLogFile);
				}
			break;
			
			default:
			break;
			}
		}
	}

/*
   Error handling,asynchronous one.
   @param  aError Error Code
   @return None
 */
void CTestPnPObserver::OnPnPError(TInt aError)
	{
	_LIT(KPositiveCase,"positivecase");
	TBool positiveCase = ETrue;
	Manager().GetBoolFromConfig(Manager().GetCurrentSection(), KPositiveCase, positiveCase);
	// Note down the error and stop scheduler
	_LIT(KErrorMessage, "Error occurred in the Test PnP Observer : %d \n");
	Manager().ERR_PRINTF2(KErrorMessage, aError);
	}


void CTestPnPManager::DescribeServiceL()
	{
	_LIT(KInfoLogFile, "CTestPnPManager::DescribeServiceL().... \n");
	INFO_PRINTF1(KInfoLogFile);

	CTestPnPObserver* pnpObserver = CTestPnPObserver::NewL(this);
	CleanupStack::PushL( pnpObserver );	
	
	RPnPParameterBundle pnpBundle;
	pnpBundle.Open ();
	CleanupClosePushL( pnpBundle );
	pnpBundle.SetPnPObserver((MPnPObserver*)pnpObserver);

	RParameterFamily pnpFamily = pnpBundle.CreateFamilyL (ETestPnPDescribeRequestParamSet);	

	iControlPoint.Describe( pnpBundle );
	_LIT(KInfoLogFile1, "CTestPnPManager::DescribeServiceL() End.... \n");
	INFO_PRINTF1(KInfoLogFile1);
	
	CleanupStack::PopAndDestroy( &pnpBundle );
	CleanupStack::PopAndDestroy( pnpObserver );
	}


/*Fails to open a handle to a RPnPServiceDiscovery object
  @param			None.
  @return			None.
 */
void CTestPnPManager::OpenDiscovererFailed()
	{
	_LIT(KInfoLogFile, "OpenDiscovererFailed().... \n");
	INFO_PRINTF1(KInfoLogFile);
	
	TInt openStatus = 0;
	openStatus = iControlPoint.Open(KInvalidTestTierId);
	
	if(openStatus != KErrNone) 
		{
		_LIT(KOpenDiscovererFailed, "Control Point discovery open failed with error %d ");
		ERR_PRINTF2(KOpenDiscovererFailed, openStatus);
		SetTestStepError(openStatus);
		}
	else
		{
		_LIT(KOpenDiscovererSuccess, "Control Point Discovery was successfully started with return value %d :");
		INFO_PRINTF2(KOpenDiscovererSuccess, openStatus);
		}
	}
	
	
/*Fails to open a handle to a RPnPServicePublisher object
  @param			None.
  @return			None.
 */
void CTestPnPManager::OpenPublisherFailed()
	{
	_LIT(KInfoLogFile, "OpenPublisherFailed().... \n");
	INFO_PRINTF1(KInfoLogFile);
	
	TInt openStatus = 0;	
	openStatus = iServicePoint.Open(KInvalidTestTierId);
		
	if( openStatus != KErrNone )
  		{
 		_LIT(KOpenPublisherFailed, "Publisher open failed with error %d ");
 		ERR_PRINTF2(KOpenPublisherFailed, openStatus);
 		SetTestStepError(openStatus);
 		}
 	else
 		{
 		_LIT(KOpenPublisherSuccess, "Service publisher was successfully started with return value %d :");
  		INFO_PRINTF2(KOpenPublisherSuccess, openStatus);
 		}
	}
	
void CTestPnPManager::TimedOut()
	{
	iControlChannel.CancelRecv();
	iControlChannel.CancelSend();
	CActiveScheduler::Stop();	
	}
	
// CTestTimer //
CTestTimer* CTestTimer::NewL(MTimerObserver& aObserver)
	{
	return new(ELeave) CTestTimer(aObserver);	
	}

CTestTimer::~CTestTimer()
	{
	Cancel();
	iTimer.Close();
	}

CTestTimer::CTestTimer(MTimerObserver& aObserver)
:CActive(EPriorityStandard), iObserver(aObserver)
	{
	CActiveScheduler::Add(this);
	iTimer.CreateLocal();
	}

void CTestTimer::RunL()
	{
	iObserver.TimedOut();
	}

void CTestTimer::DoCancel()
	{
	iTimer.Cancel();	
	}
	
void CTestTimer::After(TTimeIntervalMicroSeconds32 aInterval)
	{
	iTimer.After(iStatus,  aInterval);
	SetActive();
	}
	
	
	
