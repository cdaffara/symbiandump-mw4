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
* Description:        
*
*/









#include "senasynctimeout.h"
#include <http.h>
#include "sentxnstate.h"

CSenAsyncTimeOut::CSenAsyncTimeOut(CSenTxnState* aTxnState) :
    CTimer(EPriorityStandard),
    iTxnState(aTxnState)
    // Standard priority
    {
    }

CSenAsyncTimeOut* CSenAsyncTimeOut::NewLC(CSenTxnState* aTxnState)
    {
    CSenAsyncTimeOut* self = new ( ELeave ) CSenAsyncTimeOut(aTxnState);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

CSenAsyncTimeOut* CSenAsyncTimeOut::NewL(CSenTxnState* aTxnState)
    {
    CSenAsyncTimeOut* self = CSenAsyncTimeOut::NewLC(aTxnState);
    CleanupStack::Pop(); // self;
    return self;
    }

void CSenAsyncTimeOut::ConstructL()
    {
    CTimer::ConstructL();
    CActiveScheduler::Add(this);
    }

CSenAsyncTimeOut::~CSenAsyncTimeOut()
    {
    Cancel(); // Cancel any request, if outstanding
    }


void CSenAsyncTimeOut::StartL(TInt aTimeOutSec)
    {
    Cancel(); // Cancel any request, just to be sure
    After(aTimeOutSec * 1000000); // Set for later
    }

void CSenAsyncTimeOut::RunL()
    {
    //cancel trans
    iTxnState->Cancel();
    iTxnState->ResponseErrorL(KErrTimedOut);
    }

TInt CSenAsyncTimeOut::RunError(TInt aError)
    {
    return aError;
    }
