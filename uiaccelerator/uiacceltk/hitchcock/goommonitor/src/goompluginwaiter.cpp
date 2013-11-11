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


#include "goompluginwaiter.h"
#include "goomrunplugin.h"
#include "goomtraces.h"

const TInt KMicrosecondsInMillisecond = 1000;

CGOomPluginWaiter* CGOomPluginWaiter::NewL(TInt aMillisecondsToWait, CGOomRunPlugin& aCallbackClass)
    {
    FUNC_LOG;

    CGOomPluginWaiter* self = new (ELeave) CGOomPluginWaiter(aMillisecondsToWait, aCallbackClass);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CGOomPluginWaiter::CGOomPluginWaiter(TInt aMillisecondsToWait, CGOomRunPlugin& aCallbackClass) : CTimer(EPriorityStandard), iMillisecondsToWait(aMillisecondsToWait), iCallbackClass(aCallbackClass)
    {
    FUNC_LOG;

    CActiveScheduler::Add(this);
    }

// Start the timer, it will call the plugin back when it expires
void CGOomPluginWaiter::Start()
    {
    FUNC_LOG;

    HighRes(iMillisecondsToWait * KMicrosecondsInMillisecond);
    }
    
void CGOomPluginWaiter::RunL()
    {
    FUNC_LOG;

    iCallbackClass.WaitCompleted();
    }
