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
#include "stubsrvsession.h"
#include "stubsrv.h"
#include "stubsrvapiexecutor.h"
#include "stubclientsrv.h"

CStubSrvSess* CStubSrvSess::NewL(CStubSrv& aServer)
    {
    CStubSrvSess* self = new (ELeave) CStubSrvSess (aServer);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CStubSrvSess::~CStubSrvSess()
    {
    delete iExecutor;
    iServer.ClientClosed(*this);
    }

void CStubSrvSess::InvokeApiComplete()
    {
    delete iExecutor;
    iExecutor = NULL;
    }

TInt CStubSrvSess::ConsumeApiBehavior(const TApiBehavior& aBeh, TInt aExeDuration)    
    {
    if (iExecutor && iExecutor->Mode() == ESuspendIfNoBehaviorSet &&
        iExecutor->ApiBehavior().MatchId(aBeh))
        {
        iExecutor->Resume(aBeh, aExeDuration);
        return KErrNone;
        }
    return KErrNotFound;
    }
    
void CStubSrvSess::ServiceL(const RMessage2& aMessage)
    {
    TRAPD(err, DispatchMessageL( aMessage ));
    if (err != KErrNone)
        {
        aMessage.Panic(KStubSrvName, err);
        }
    }

void CStubSrvSess::DispatchMessageL(const RMessage2& aMessage)
    {
    switch (aMessage.Function())
        {
        case EStubSrvEnqueApiBehavior:
            {
            iServer.EnqueueL(aMessage);
            aMessage.Complete(KErrNone);
            break;
            }
        case EStubSrvDeleteApiBehaviors:
            {
            iServer.DeleteQueues(aMessage);
            aMessage.Complete(KErrNone);
            break;
            }
        case EStubSrvInvokeApi:
            {
            if (iExecutor)
                {
                aMessage.Panic(KStubSrvName, EStubfApiAlreadyStarted);
                break;
                }
            TApiBehavior  beh;
            TApiBehaviorPckg pckg(beh);
            aMessage.ReadL(0, pckg);
            TInt transactionId = aMessage.Int2();
            TInt dur = 0;
            TInt err = iServer.Dequeue(beh, dur, transactionId);
            iExecutor = CApiExecutor::NewL();
            if (err)
                {
                iExecutor->ExecuteL(aMessage, *this, beh.iLib, beh.iApi);
                }
            else
                {
                iExecutor->ExecuteL(aMessage, *this, beh, dur);
                }
            break;
            }
        case EStubSrvInvokeApiCancel:
            {
            delete iExecutor;
            iExecutor = NULL;
            aMessage.Complete(KErrNone);
            break;
            }
        case EStubSrvGetApiCompletionCode:
            {
            iServer.GetApiCompleteCodeL(aMessage);
            break;
            }
        default:
            {
            aMessage.Panic(KStubSrvName, EStubfBadRequest);
            break;
            }
        }
    }

CStubSrvSess::CStubSrvSess(CStubSrv& aServer) : iServer(aServer)
    {
    }

void CStubSrvSess::ConstructL()
    {
    iServer.ClientOpened(*this);
    }


// End of File
