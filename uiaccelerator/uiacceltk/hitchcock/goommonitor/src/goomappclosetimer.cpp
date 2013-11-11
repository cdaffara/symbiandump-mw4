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


#include "goomappclosetimer.h"
#include "goomcloseapp.h"
#include "goomtraces.h"

CGOomAppCloseTimer* CGOomAppCloseTimer::NewL(CGOomCloseApp& aMonitor)
    {
    FUNC_LOG;

    CGOomAppCloseTimer* self = new (ELeave) CGOomAppCloseTimer(aMonitor);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CGOomAppCloseTimer::CGOomAppCloseTimer(CGOomCloseApp& aMonitor)
: CTimer(CActive::EPriorityStandard), iMonitor(aMonitor)
    {
    FUNC_LOG;

    CActiveScheduler::Add(this);
    }

void CGOomAppCloseTimer::RunL()
    {
    FUNC_LOG;
    if(iState == EGOomAppClosing)
        {
        iMonitor.KillTask();
        }
    else
        {
        iMonitor.KillTaskWaitDone();
        }
    }

void CGOomAppCloseTimer::SetState(TGOomAppCloseState aState)
    {
    FUNC_LOG;
    iState=aState;
    }
