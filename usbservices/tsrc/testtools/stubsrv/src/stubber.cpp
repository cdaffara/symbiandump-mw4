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

#include "stubber.h"
#include "stubclient.h"


EXPORT_C CStubber* CStubber::NewL()
    {
    CStubber* self = new (ELeave) CStubber();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

EXPORT_C CStubber::~CStubber()
    {
    Cancel();
    if (iClient)
        iClient->Close();
    delete iClient;
    }

EXPORT_C void CStubber::EnqueBehavior(const TApiBehavior& aBeh, TInt aExeDuration)
    {
    iClient->EnqueBehavior(aBeh, aExeDuration);
    }

EXPORT_C void CStubber::DeleteBehaviors(TUint aLib, TUint aApi)
    {
    iClient->DeleteBehaviors(aLib, aApi);
    }

EXPORT_C void CStubber::InvokeApi(TApiBehavior& aBeh, TApiInvokeMode aMode)
    {
    aBeh = TApiBehavior(aBeh.iLib, aBeh.iApi);
    TApiBehaviorPckg pckg(aBeh);
    iClient->InvokeApi(pckg, aMode);
    }

EXPORT_C void CStubber::InvokeApiL(MApiInvoker& aInvoker, TUint aLib, TUint aApi, TApiInvokeMode aMode)
    {
    if (IsActive())
        {
        User::Panic(_L("Stubber"), KErrInUse);
        }
    iInvoker = &aInvoker;
    iBeh = TApiBehavior(aLib, aApi);
    iClient->InvokeApiL(iBehPckg, iStatus, aMode);
    SetActive();
    }

EXPORT_C void CStubber::InvokeApiCancel()
    {
    Cancel();
    }

void CStubber::RunL()
    {
    if (iStatus != KErrNone)
        {
        iBeh.iAsyncCompleteCode = iStatus.Int();
        }
    iInvoker->InvokeApiComplete(iBeh);
    }

void CStubber::DoCancel()
    {
    iClient->InvokeApiCancel(iBeh.iLib, iBeh.iApi);
    }

TInt CStubber::RunError(TInt /*aReason*/)
    {
    return KErrNone;
    }

CStubber::CStubber() : 
    CActive(EPriorityStandard), iBehPckg(iBeh)
    {
    CActiveScheduler::Add(this);
    }

void CStubber::ConstructL()
    {
    iClient = new (ELeave) RStubClient();
    User::LeaveIfError(iClient->Connect());
    }
