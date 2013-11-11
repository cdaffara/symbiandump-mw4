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

/**
 @file
 @internalTechnology
*/

#include "testdelay.h"


/*
  Constructor:
  @internalTechnology
  @test
*/
CTestUPnPManager::CTestUPnPManager()
:	CTestStep()
	{
	}

/*
   Initializes all member vaiables.
   @param		None.
   @return		None.
*/
void CTestUPnPManager::InitializeL()
	{
	iScheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iScheduler);
	}

/*
   Destructor
   @internalTechnology
   @test
*/
CTestUPnPManager::~CTestUPnPManager()
	{
	CActiveScheduler::Install(NULL);
	delete iScheduler;
	}

/*
   Implementation of CTestStep base class virtual and this is used for doing all
   initialisation common
    to derived classes here.
   @param  None
   @return TVerdict
 */
TVerdict CTestUPnPManager::doTestStepPreambleL()
	{
	InitializeL();
	return CTestStep::doTestStepPreambleL();
	}

/*
   Implementation of CTestStep base class virtual and it is used for doing all
   after test treatment common to derived classes in here.
   @return TVerdict
 */
TVerdict CTestUPnPManager::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL();
	}


TVerdict CTestUPnPManager::doTestStepL()
	{
	return TestStepResult();	
	}



CUPnPTimer::CUPnPTimer()
: CActive(EPriorityIdle), iRequestPending(EFalse)
    {
    CActiveScheduler::Add(this);
    }


CUPnPTimer::~CUPnPTimer()
    {
	if(PendingRequest())
		{
		Cancel();
		}
    }


CUPnPTimer* CUPnPTimer::NewL()
    {
	CUPnPTimer* self = new (ELeave) CUPnPTimer();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
    }

void CUPnPTimer::ConstructL()
	{
	}


void CUPnPTimer::IssueRequestL(TTimeIntervalMicroSeconds32 anInterval)
    {
	iTimer.CreateLocal();
    iTimer.After(iStatus, anInterval);
 	iRequestPending = ETrue;
    SetActive();
    }

TBool CUPnPTimer::PendingRequest()
    {
    return iRequestPending;
    }


void CUPnPTimer::RunL()
	{
	iRequestPending = EFalse;
	CActiveScheduler::Stop();
	}

void CUPnPTimer::DoCancel()
	{
	if(PendingRequest())
		{
		Cancel();
		}
	}
