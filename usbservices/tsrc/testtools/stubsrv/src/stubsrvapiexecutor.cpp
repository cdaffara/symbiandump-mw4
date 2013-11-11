/*
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

#include "stubsrvapiexecutor.h"
#include "stubsrvsession.h"

CApiExecutor* CApiExecutor::NewL()
    {
    CApiExecutor* self = new (ELeave) CApiExecutor();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CApiExecutor::~CApiExecutor() 
    {
    Cancel();
    iTimer.Close();
    if (iMessage.Handle())
        {
        iMessage.Complete(KErrCancel);
        }    
    }

const TApiBehavior& CApiExecutor::ApiBehavior() const
    {
    return iBeh;
    }

TApiInvokeMode CApiExecutor::Mode() const
    {
    return iMode;
    }

void CApiExecutor::ExecuteL(const RMessage2& aMessage, CStubSrvSess& aSession, TUint aLib, TUint aApi)
    {
    iMessage = aMessage;
    iSess = &aSession;
    iBeh = TApiBehavior(aLib, aApi);
    iMode = (TApiInvokeMode) iMessage.Int1();
    if (iMode == ECompleteIfNoBehaviorSet)
        {
        TRequestStatus* status = &iStatus;
        User::RequestComplete(status, KErrNone);
        SetActive();
        }
    }

void CApiExecutor::ExecuteL(const RMessage2& aMessage, CStubSrvSess& aSession, const TApiBehavior& aBeh, TInt aExeDuration)
    {
    iMessage = aMessage;
    iSess = &aSession;
    iMode = ECompleteIfNoBehaviorSet;
    iBeh = aBeh;
    iTimeout = aExeDuration;
    if (iTimeout == 0)
        {
        TRequestStatus* status = &iStatus;
        User::RequestComplete(status, KErrNone);
        SetActive();
        }
    else if (iTimeout > 0)
        {
        iTimer.After(iStatus, iTimeout);
        SetActive();
        }
    }

void CApiExecutor::Resume(const TApiBehavior& aBeh, TInt aExeDuration)
    {
    iBeh = aBeh;
    iTimeout = aExeDuration;
    iMode = ECompleteIfNoBehaviorSet;
    if (iTimeout == 0)
        {
        TRequestStatus* status = &iStatus;
        User::RequestComplete(status, KErrNone);
        SetActive();
        }
    else if (iTimeout > 0)
        {
        iTimer.After(iStatus, iTimeout);
        SetActive();
        }
    }

void CApiExecutor::RunL()
    {
    TInt err = KErrNone;
    TApiBehaviorPckgBuf pckg(iBeh);
    err = iMessage.Write(0, pckg);
    iMessage.Complete(err);
    iSess->InvokeApiComplete();
    }
    
TInt CApiExecutor::RunError(TInt /*aReason*/)
    {
    return KErrNone;
    }

void CApiExecutor::DoCancel()
    {
    iTimer.Cancel();
    if (iMessage.Handle())
        {
        iMessage.Complete(KErrCancel);
        }
    }

CApiExecutor::CApiExecutor() 
 : CActive(CActive::EPriorityStandard)
    {
    CActiveScheduler::Add(this);
    }

void CApiExecutor::ConstructL()
    {
    iTimer.CreateLocal();
    }


