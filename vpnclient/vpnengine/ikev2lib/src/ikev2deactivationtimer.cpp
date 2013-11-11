/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Timer to trigger cancellation of session deletion.
*
*/


#include "ikev2deactivationtimer.h"

// Deactivation timeout value.
static const TUint KDeactivationTimeout = 2000000; //microseconds  

CIkev2DeactivationTimer* CIkev2DeactivationTimer::NewL(MIkev2DeactivationTimerCallback& aCallback)
    {
    CIkev2DeactivationTimer* self = new (ELeave) CIkev2DeactivationTimer(aCallback);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    
    return self;
    }


CIkev2DeactivationTimer::CIkev2DeactivationTimer(MIkev2DeactivationTimerCallback& aCallback)
 : CTimer(EPriorityStandard),
   iCallback(aCallback)
    {
	CActiveScheduler::Add(this);   
    }

CIkev2DeactivationTimer::~CIkev2DeactivationTimer()
    {
	Cancel();
    }

void CIkev2DeactivationTimer::IssueRequest()
    {
	After(KDeactivationTimeout);
    }


void CIkev2DeactivationTimer::RunL()
    {
	iCallback.DeactivationTimeout();
    }

