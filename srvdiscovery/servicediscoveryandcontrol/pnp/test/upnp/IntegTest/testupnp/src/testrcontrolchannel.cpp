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
// testupnpmanager.cpp
// Contains implementation of CTestRControlChannel class
// 
//

/**
 @file
 @internalTechnology
*/

#include "testrcontrolchannel.h"
#include "testupnpmanager.h"
#include <e32base.h>

/*
  Constructor:
  @internalTechnology
  @test
*/
CTestRControlChannel::CTestRControlChannel()
:	CTestStep()
	{
	}

/*
   Initializes all member vaiables.
   @param		None.
   @return		None.
*/
void CTestRControlChannel::InitializeL()
	{
	_LIT(KInfoLogFile, "CTestRControlChannel::InitializeL().... \n");
	INFO_PRINTF1(KInfoLogFile);
	iScheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iScheduler);
	iTimer = NULL;
	iTimer  = CPeriodic::NewL(CActive::EPriorityStandard);
	}

/*
   Destructor
   @internalTechnology
   @test
*/
CTestRControlChannel::~CTestRControlChannel()
	{
	_LIT(KInfoLogFile, "~CTestRControlChannel().... \n");
	INFO_PRINTF1(KInfoLogFile);
	iPublisher.Close();
//	CActiveScheduler::Install(NULL); //Testing for ONB hangs
	delete iScheduler;
	iScheduler = NULL;
	_LIT(KInfoLogFile1, "CTestRControlChannel active Scheduler.... \n");
	INFO_PRINTF1(KInfoLogFile1);
	CleanUpObserverArray();
	if (iTimer != NULL)
		{
		delete iTimer;
		iTimer = NULL;
		}
	_LIT(KInfoLogFile2, "~CTestRControlChannel() End.... \n");
	INFO_PRINTF1(KInfoLogFile2);
	
	iConnection.Close ();
	iSocketServ.Close ();
	}

void CTestRControlChannel::SavePreviousLogFileL()
	{
	_LIT(KInfoLogFile, "Saving Previous logs If any.... \n");
	INFO_PRINTF1(KInfoLogFile);
	RFs fileserv;
	TInt error = fileserv.Connect();
	if(!error)
		{
		_LIT(KlogFile, "C:\\logs\\log.txt");
		_LIT(KPrefixLog, "C:\\logs\\");
		_LIT(KSuffix, "_UDEB");
		_LIT(KExtn, ".txt");
		
		CFileMan* fMan = NULL;
		TRAP_IGNORE(fMan = CFileMan::NewL(fileserv));
		if(fMan != NULL)
			{
			CleanupStack::PushL(fMan);
			RBuf newFileName;
			const TInt KTwenty = 20;
			newFileName.Create(ConfigSection().Length() + KTwenty );
			newFileName.Copy(KPrefixLog);
			//Script file name
			iScriptName.Copy(ConfigSection());
			newFileName.Append(iScriptName);
			newFileName.Append(KSuffix);
			newFileName.Append(KExtn);
			
			TInt error = fMan->Rename(KlogFile, newFileName);
			if (error != KErrNone)
				{
				_LIT(KErrInfo, "Unable to Save Previous logs...May be NO Previous logs exist \n");
				ERR_PRINTF1(KErrInfo);
				}
			else
				{
				_LIT(KSavingInfo, "Saved Previous logs with File Name: %S \n");
				INFO_PRINTF2(KSavingInfo, &newFileName);
				}
			CleanupStack::PopAndDestroy(fMan);
			newFileName.Close();
			}
		}
	fileserv.Close();
	_LIT(KInfoLogFile1, "Saving Previous logs End.... \n");
	INFO_PRINTF1(KInfoLogFile1);
	}

void CTestRControlChannel::CleanUpObserverArray()
	{
	_LIT(KInfoLogFile, "CleanUpObserverArray....\n");
	INFO_PRINTF1(KInfoLogFile);
	if (iObserverArray.Count() > 0 )
		{
		iObserverArray.ResetAndDestroy();
		}
	else
		{
		iObserverArray.Close();
		}
	_LIT(KInfoLogFile1, "CleanUpObserverArray End....\n");
	INFO_PRINTF1(KInfoLogFile1);
	}

/*
   Implementation of CTestStep base class virtual and this is used for doing all
   initialisation common
    to derived classes here.
   @param  None
   @return TVerdict
 */
TVerdict CTestRControlChannel::doTestStepPreambleL()
	{
	InitializeL();
	SavePreviousLogFileL();
	return CTestStep::doTestStepPreambleL();
	}

/*
   Implementation of CTestStep base class virtual and it is used for doing all
   after test treatment common to derived classes in here.
   @return TVerdict
 */
TVerdict CTestRControlChannel::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL();
	}


/*
	Opens a handle to a RPnPServicePublisher object and append the same to Publisher array
	@param			None.
	@return			None.
 */
TVerdict CTestRControlChannel::doTestStepL()
	{
	SequenceOperationsL();
	return TestStepResult();
	}
	
void CTestRControlChannel::InitiateControlL()
	{
	RControlChannel controlChannel;
	TInt err =-1;
	err = iPublisher.InitiateControl(controlChannel);
	_LIT(KInfoLogFile, "Initiating Publisher control returned value %d .... \n");
	INFO_PRINTF2(KInfoLogFile,err);
	CUPnPRControlChannelAO *rControlAO;
    rControlAO = new(ELeave) CUPnPRControlChannelAO(controlChannel,this);
	rControlAO->RecvdataL();
   	CActiveScheduler::Start();
	_LIT(KInfoLogFile1, "CTestRControlChannel::doTestStepL() End.... \n");
	INFO_PRINTF1(KInfoLogFile1);
	controlChannel.Close();
//	delete rControlAO;

	}
/*
   This function performs operations based on sequence specified in the ini file
   @param  None
   @return None
*/
void CTestRControlChannel::SequenceOperationsL()
	{
	_LIT(KInfoLogFile, "CTestRControlChannel::SequenceOperationsL().... \n");
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
//		SetCurrentSection(sequenceList[i]);
		PerformDesiredOperationsL(sequenceList[i]);
		}
	CleanupStack::PopAndDestroy(); 
	_LIT(KInfoLogFile1, "CTestRControlChannel::SequenceOperationsL() End.... \n");
	INFO_PRINTF1(KInfoLogFile1);
	}
	
	
/*
   Parses a comma separated string and constructs a list out of the values
   @param	aString a reference to a string to be tokenized
   @param	aList is an out parameter to store a list of tokens in an arry
   @param	aSeparator is a character used to delimit the tokens
  */
void CTestRControlChannel::TokenizeStringL(const TDesC& aString, RArray<TPtrC>& aList, TChar aSeparator)
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
   This function performs desired operations specified as a parameter
   @param  aOperation string representing operation to be performed
   @return None
*/
void CTestRControlChannel::PerformDesiredOperationsL(const TDesC& aOperation)
	{
	const TInt KMatch = 0;
	TPtrC operationType;
	_LIT(KOperationType, "operationtype");
	GetStringFromConfig(aOperation, KOperationType, operationType);

	if ( iConnection.SubSessionHandle () == 0 )
		{
		TestUtils::LoadESockL ( iSocketServ, iConnection );
		}
		
	if(operationType.Compare(KPublishService) == KMatch)
		{
		PublishServiceL(aOperation);
		}
	}


void CTestRControlChannel::OpenPublisherL()
	{
	_LIT(KInfoLogFile, "CTestRControlChannel::OpenPublisherL.... \n");
	INFO_PRINTF1(KInfoLogFile);
 	
	TInt openStatus = 0;
	for(TInt i=0;i<3;i++)	
		{
		openStatus = iPublisher.Open(KTierId);
		if(openStatus == KErrNone)
			{
			break;
			}
		}
	if( openStatus != KErrNone ) 
		{
		_LIT(KOpenDiscovererFailed, "Publisher open failed with error %d :");
		ERR_PRINTF2(KOpenDiscovererFailed, openStatus);
		}
	else
		{
		_LIT(KOpenPublisherSuccess, "Service publisher was successfully started with return value %d :");
		INFO_PRINTF2(KOpenPublisherSuccess, openStatus);
		}
	}

/*
	Tests whether Publishing a Service/Device is performing as specified in UPnP specifications.
	@param			aOperationType is reference to a section name in ini file where required parameters
					needs to be referred for this operation.
	@return			None.
 */
void CTestRControlChannel::PublishServiceL (const TDesC& aOperationType)
	{
	_LIT(KInfoLogFile, "CTestRControlChannel::PublishServiceL ().... \n");
	INFO_PRINTF1(KInfoLogFile);
	CRControlChannelObserver* upnpObserver = CRControlChannelObserver::NewL(this);
	CleanupStack::PushL( upnpObserver );
	iObserverArray.Append(upnpObserver);

	RPnPParameterBundle pnpBundle ;
	pnpBundle.Open();
    CleanupClosePushL( pnpBundle );
    pnpBundle.SetPnPObserver((MPnPObserver*)upnpObserver);
    RParameterFamily family = pnpBundle.CreateFamilyL(EUPnPServiceRegisterParamSet);
   	CUPnPServiceRegisterParamSet* registerServiceParamSet = CUPnPServiceRegisterParamSet::NewL(family );

   	_LIT(KParentDeviceUid, "ParentDeviceUid");
   	TPtrC parentDeviceUid;
	GetStringFromConfig(aOperationType, KParentDeviceUid, parentDeviceUid);

	RBuf8 parentDeviceUidBuf;
	parentDeviceUidBuf.Create(parentDeviceUid.Length());
	parentDeviceUidBuf.Copy(parentDeviceUid);
	registerServiceParamSet->SetDeviceUidL (parentDeviceUidBuf);


	TPtrC serviceType;
	GetStringFromConfig(aOperationType, KServiceType, serviceType);
	RBuf8 serviceTypeBuf;
	serviceTypeBuf.Create(serviceType.Length());
	serviceTypeBuf.Copy(serviceType);
	registerServiceParamSet->SetUriL ( serviceTypeBuf );


	TInt duration;
	GetIntFromConfig(aOperationType, KCacheControl, duration);
	registerServiceParamSet->SetCacheControlData (duration);

	ExtractServiceDescriptionL (aOperationType, *registerServiceParamSet);

	_LIT8(KInitialMessage, "Initial notification message");
	registerServiceParamSet->SetInitialMessageL(KInitialMessage);
	
	OpenPublisherL();

	iPublisher.Publish( pnpBundle );
	CActiveScheduler::Start();

	serviceTypeBuf.Close();
	parentDeviceUidBuf.Close();
	CleanupStack::PopAndDestroy( &pnpBundle );
	CleanupStack::Pop( upnpObserver );
	
	InitiateControlL();
	_LIT(KInfoLogFile1, "CTestRControlChannel::PublishServiceL () Stop.... \n");
	INFO_PRINTF1(KInfoLogFile1);
	
	}


/*
	Extract service description available in a file, copy it to the buffer and append the same to service register paramset.
	@param			aConfigSection - reference to a section name in ini file where required parameters that needs to be referred for this operation.
					aServiceRegisterParamSet - Storage for the description buffer
	@return			None.
 */

void CTestRControlChannel::ExtractServiceDescriptionL (const TDesC& aConfigSection, CUPnPServiceRegisterParamSet& aServiceRegisterParamSet)
	{
	RFs fs;
	RFile file;
	RBuf8 buf;

	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	TPtrC descriptionPath;
	_LIT(KDescriptionPath, "Description_Path");
	GetStringFromConfig(aConfigSection, KDescriptionPath, descriptionPath);

	TInt err = file.Open(fs, descriptionPath, EFileShareReadersOnly);
	
	// For Hardware system path is c:, so descriptionPath value present in '.ini' is referring 'c:'
	if ( err == KErrPathNotFound )
		{				
		RBuf fileName;
		TDriveName aSystemDrive;
		TDriveUnit driveunit(RFs::GetSystemDrive());
		aSystemDrive.Zero();
		aSystemDrive=driveunit.Name();				
		fileName.CreateL ( descriptionPath.Length () );
		fileName.Zero();
		fileName.Append(aSystemDrive);
		fileName.Append ( descriptionPath.Mid ( aSystemDrive.Length () ) );				
		
		err = file.Open(fs, fileName, EFileShareReadersOnly);
		}
	if (err != KErrNone)
		{
		User::LeaveIfError(err);
		}

	CleanupClosePushL(file);
	TInt fileSize = 0;
	file.Size(fileSize);

	buf.Create(fileSize);

	err = file.Read(buf, fileSize);
	aServiceRegisterParamSet.SetServiceDescriptionL ( buf );

	CleanupStack::PopAndDestroy(2 );
	buf.Close();
	_LIT(KInfoLogFile, "CRControlChannelObserver::ExtractServiceDescriptionL End.... \n");
	INFO_PRINTF1(KInfoLogFile);
	}


CUPnPRControlChannelAO::CUPnPRControlChannelAO(RControlChannel& aControlChannel,CTestRControlChannel* aManager)
	:CActive(EPriorityStandard),iControlChannel(aControlChannel),iManager(aManager)
    {
	CActiveScheduler::Add(this);
    }    

void CUPnPRControlChannelAO::RecvdataL()
    {
    // create the timer object
	iTimer = CPeriodic::NewL ( CActive::EPriorityLow );
	
	_LIT(KInfoLogFile, "CUPnPRControlChannelAO::RecvdataL().... \n");
	iManager->INFO_PRINTF1(KInfoLogFile);
    iIncomingActionBuffer.CreateMaxL( 1024 );
   	iInComingAction.SetMessageDes (iIncomingActionBuffer);
	iControlChannel.Recv(iInComingAction, iStatus);
	iState = ERecv;
	SetActive();
	
	TCallBack callback ( &CUPnPRControlChannelAO::TimerCallbackL, this );
	const TInt timeout = 60436607; // rounded-conversion of 600seconds to clockticks
	iTimer->Start( TTimeIntervalMicroSeconds32 ( timeout ), TTimeIntervalMicroSeconds32 ( timeout ), callback );
	
	_LIT(KInfoLogFile1, "CUPnPRControlChannelAO::RecvdataL() End.... \n");
	iManager->INFO_PRINTF1(KInfoLogFile1);
    }

TInt CUPnPRControlChannelAO::TimerCallbackL ( TAny* aPtr )
	{
	CUPnPRControlChannelAO* self = static_cast<CUPnPRControlChannelAO*> ( aPtr );
	if ( self->iState == ERecv )
		{
		CActiveScheduler::Stop ();
		_LIT ( KInfoLogFile1, "CUPnPRControlChannelAO::RunL() Stop.... \n" );
		self->iManager->INFO_PRINTF1 ( KInfoLogFile1 );
		self->iTimer->Cancel ();
		}
		
	return KErrNone;
	}
	

void CUPnPRControlChannelAO::SenddataL()
    {
	_LIT(KInfoLogFile, "CUPnPRControlChannelAO::SenddataL().... \n");
	iManager->INFO_PRINTF1(KInfoLogFile);
    ExtractSoapResponse(iOutgoingActionBuffer);
    iInComingAction.SetMessageDes (iOutgoingActionBuffer);
    iInComingAction.SetLast();
    TInt newLen = iOutgoingActionBuffer.Length( );
    iInComingAction.SetMaxLength(newLen);
    iControlChannel.Send(iInComingAction, iStatus);
	SetActive();
	_LIT(KInfoLogFile1, "CUPnPRControlChannelAO::SenddataL() End.... \n");
	iManager->INFO_PRINTF1(KInfoLogFile1);
    }

CUPnPRControlChannelAO::~CUPnPRControlChannelAO()
    {
    iTimer->Cancel ();
	delete iTimer;	
    }

void CUPnPRControlChannelAO::RunL()
    {
     switch ( iState )
    	 {
	     case ERecv:
	    	 {
	    	 iTimer->Cancel ();
	    	 if( iInComingAction.MaxLength() != KErrUnknown )
	             {
	             if ( iCompleteBuffer.Length() == 0 )
		       		{
		    		iCompleteBuffer.CreateL(iInComingAction.MessageDes().Length());
		   	       	}
		   	     else
		   	     	{
		   	     	TInt oldLen = iCompleteBuffer.Length();
		   	     	iCompleteBuffer.ReAlloc ( oldLen + iInComingAction.MessageDes().Length() );
		   	     	}
	    	     iCompleteBuffer.Append(iInComingAction.MessageDes());
	
	    	     if( iCompleteBuffer.Length() == iInComingAction.MaxLength() )
	    	       	{
					iInComingAction.ClearFlags();
	    	       	SenddataL();
	    	       	iState = EStop;
	    	       	}
	    	     else
	    	       	{
	    	       	iControlChannel.Recv(iInComingAction, iStatus);
	    	       	iState = ERecv;
	    	       	SetActive();
	    	       	}
	             }
	          else
	    	     {
	    	     iExepecteBufLen = iInComingAction.MaxLength();
	    	     RBuf8 completeData;
	    	     completeData.CreateL( iExepecteBufLen );
	    	     completeData.Append(iInComingAction.MessageDes());
	    	     iControlChannel.Recv(iInComingAction, iStatus);
	    	     SenddataL();
	    	     iState = EStop;
	    	     }
			_LIT(KInfoLogFile, "CUPnPRControlChannelAO::RunL().... \n");
			iManager->INFO_PRINTF1(KInfoLogFile);
	    	 }
	    break;

       case EStop:
    	   {
			CActiveScheduler::Stop();
			_LIT(KInfoLogFile1, "CUPnPRControlChannelAO::RunL() Stop.... \n");
			iManager->INFO_PRINTF1(KInfoLogFile1);    	   
    	   }
        break;
        }
    }


void CUPnPRControlChannelAO::DoCancel()
	{
	//Nothing
	}

void CUPnPRControlChannelAO::ExtractSoapResponse(RBuf8& aSoapResponse)
	{
	aSoapResponse.Create(iCompleteBuffer);
	}


/*
  Constructor:
  @internalTechnology
  @test
*/
CRControlChannelObserver::CRControlChannelObserver()
	{
	}

/*
  Destructor
  @internalTechnology
  @test
*/
CRControlChannelObserver::~CRControlChannelObserver()
	{
	}


/*
	Static factory constructor. Uses two phase construction and leaves nothing on the
	CleanupStack. Creates a CTestControlPointService object.
	@param			aManager	A pointer to a CTestRControlChannel object.
	@return			A pointer to the newly created CPnPObserver object.
	@post			A fully constructed and initialized CPnPObserver object.
 */

CRControlChannelObserver* CRControlChannelObserver::NewL(CTestRControlChannel* aManager)
	{
	CRControlChannelObserver* self = new(ELeave) CRControlChannelObserver;
	CleanupStack::PushL(self);
	self->ConstructL(aManager);
	CleanupStack::Pop(self);
	return self;
	}

/*
	Second phase of two-phase construction method. Does any allocations required to fully construct
	the object.
	@pre 		First phase of construction is complete.
	@param		aManager	A pointer to a CTestRControlChannel object.
	@post		The object is fully constructed and initialized.
 */
void CRControlChannelObserver::ConstructL(CTestRControlChannel* aManager)
	{
	iManager = aManager;
	}



/*
   This method returns a reference to the CTestRControlChannel object.
   @param None
   return a reference to the CTestRControlChannel object
 */
CTestRControlChannel& CRControlChannelObserver::Manager()
	{
	return *iManager;
	}

/*
   This is called when callback hits. And this method stores the bundle results
   and meta information related to that results into results array
   @param  aParameterBundle a reference to CPnPParameterBundleBase
   @return None
 */
void CRControlChannelObserver::OnPnPEventL ( RPnPParameterBundleBase& aParameterBundle)
	{
	RParameterFamily family = aParameterBundle.GetFamilyAtIndex(0);	
	_LIT(KInfoLogFile, "Service Published successfully...\n");
	Manager().INFO_PRINTF1(KInfoLogFile);

	CActiveScheduler::Stop();
	_LIT(KInfoLogFile1, "CRControlChannelObserver::OnPnPEventL.... \n");
	Manager().INFO_PRINTF1(KInfoLogFile1);
	}


void CRControlChannelObserver::OnPnPError(TInt aError)
	{
	TInt error = aError;
	_LIT(KErrorMessage, "Error occurred in the UPnP Observer : %d \n");
	Manager().INFO_PRINTF2(KErrorMessage, error);
	}


