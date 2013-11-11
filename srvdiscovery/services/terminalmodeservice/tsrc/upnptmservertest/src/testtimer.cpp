/**
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


// INCLUDE FILES
#include "testtimer.h"
#include "tmservertest.h"

const TInt KMicroSecond = 1000000;

CTestTimer* CTestTimer::NewL(CTmServerTest& aTmServerTest)
    {
    return new (ELeave) CTestTimer(aTmServerTest);  
    }

CTestTimer::~CTestTimer()
    {
    Cancel();
    iTimer.Close(); 
    }

void CTestTimer::After(const TInt aDuration)
    {
    iTimer.After(iStatus, aDuration*KMicroSecond);
    SetActive();    
    }

CTestTimer::CTestTimer(CTmServerTest& aTmServerTest)
:CActive(EPriorityStandard), iTmServerTest(aTmServerTest)
    {
    CActiveScheduler::Add(this);
    iTimer.CreateLocal();   
    }

void CTestTimer::RunL()
    {
     iTmServerTest.TestTimerTimedOutL();
    }

void CTestTimer::DoCancel()
    {
    iTimer.Cancel();
    }

