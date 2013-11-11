/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Classes for executing GOOM actions (e.g. closing applications and running plugins).
*
*/


#include "goomappclosewatcher.h"
#include "goomcloseapp.h"
#include "goomtraces.h"

CGOomAppCloseWatcher::CGOomAppCloseWatcher(CGOomCloseApp& aMonitor) : CActive(CActive::EPriorityStandard), iMonitor(aMonitor)
    {
    FUNC_LOG;

    CActiveScheduler::Add(this);
    }

CGOomAppCloseWatcher::~CGOomAppCloseWatcher()
    {
    FUNC_LOG;

    Cancel();
    }

void CGOomAppCloseWatcher::Start(const TApaTask& aTask)
    {
    FUNC_LOG;

    if (!IsActive())
        {
        TInt err = iThread.Open(aTask.ThreadId());
        if (err == KErrNone)
            {
            iOriginalProcessPriority = iThread.ProcessPriority();
            iThread.SetProcessPriority(EPriorityForeground);
            iThread.Logon(iStatus);
            SetActive();
            }
        else
            {
            TRequestStatus* s = &iStatus;
            User::RequestComplete(s, err);
            SetActive();
            }
        }
    }

void CGOomAppCloseWatcher::DoCancel()
    {
    FUNC_LOG;

    iThread.LogonCancel(iStatus);
    iThread.SetProcessPriority(iOriginalProcessPriority);
    iThread.Close();
    }

void CGOomAppCloseWatcher::RunL()
    {
    FUNC_LOG;

    if (iThread.Handle())
        iThread.SetProcessPriority(iOriginalProcessPriority);
    iThread.Close();
    // Experimentation shows that memory may take up to 40ms
    // to be released back to the system after app thread close.
    // Using this delay should minimise the number of apps that
    // need to be closed to recover the necessary memory.
    const TInt KAppTidyUpDelay = 40000;
    User::After(KAppTidyUpDelay);
    iMonitor.CloseAppEvent();
    }
