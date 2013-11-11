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
#include "discoverytesttimer.h"
#include "tmservertest.h"

const TInt KMicroSecond1 = 1000000;

CDiscoveryTestTimer* CDiscoveryTestTimer::NewL(CTmServerTest& aTmServerTest)
    {
    return new (ELeave) CDiscoveryTestTimer(aTmServerTest);  
    }

CDiscoveryTestTimer::~CDiscoveryTestTimer()
    {
    Cancel();
    iDiscoveryTimer.Close(); 
    }

void CDiscoveryTestTimer::AfterDiscovery(const TInt aDuration)
    {
    iDiscoveryTimer.After(iStatus, aDuration*KMicroSecond1);
    SetActive();    
    }

CDiscoveryTestTimer::CDiscoveryTestTimer(CTmServerTest& aTmServerTest)
:CActive(EPriorityStandard), iTmServerTest(aTmServerTest)
    {
    CActiveScheduler::Add(this);
    iDiscoveryTimer.CreateLocal();   
    }

void CDiscoveryTestTimer::RunL()
    {
     iTmServerTest.TestTimerDiscoveryTimedOut();
    }

void CDiscoveryTestTimer::DoCancel()
    {
    iDiscoveryTimer.Cancel();
    }

