/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: SIT thread death observer.
*
*/



#include "sitdeathobserver.h"

CSitDeathObserver::CSitDeathObserver(const TThreadId aThreadId, MSitDeathListener* aSitDeathListener)
    : CActive(EPriorityNormal), iThreadId(aThreadId), iSitDeathListener(aSitDeathListener)
    {
    CActiveScheduler::Add(this);
    }

CSitDeathObserver::~CSitDeathObserver()
    {
    Cancel();
    }
    
void CSitDeathObserver::StartObservingL()
    {
    User::LeaveIfError(iThreadToObserve.Open(iThreadId));
    iThreadToObserve.Logon(iStatus);
    SetActive();
    }

void CSitDeathObserver::DoCancel()
    {
    iThreadToObserve.LogonCancel(iStatus);
    iThreadToObserve.Close();
    }

void CSitDeathObserver::RunL()
    {
    iSitDeathListener->SitDied();
    iThreadToObserve.Close();
    }
