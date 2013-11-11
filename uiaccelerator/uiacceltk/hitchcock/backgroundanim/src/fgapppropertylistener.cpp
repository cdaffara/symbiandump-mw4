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
* Description: 
*
*/
#include <e32debug.h>
#include "fgapppropertylistener.h"

const TUid KPSSysCategory = { KUidSystemCategoryValue };
const TUint32 KAlfPSKeyHSVisible = 0x2002EA91;

CFGAppPropertyListener::CFGAppPropertyListener(TCallBack aCallback) : CActive(EPriorityStandard), iCallback(aCallback)     
    {
    CActiveScheduler::Add(this);
    }

CFGAppPropertyListener::~CFGAppPropertyListener()
    {
    Cancel();
    iHSFGProperty.Close();
    }

CFGAppPropertyListener* CFGAppPropertyListener::NewL(TCallBack aCallback)
    {
    CFGAppPropertyListener* me = new (ELeave)CFGAppPropertyListener(aCallback);
    CleanupStack::PushL(me);
    me->ConstructL();
    CleanupStack::Pop();
    return me;
    }

void CFGAppPropertyListener::ConstructL()
    {
    User::LeaveIfError(iHSFGProperty.Attach(KPSSysCategory, KAlfPSKeyHSVisible));
    }

void CFGAppPropertyListener::RunL()
    {
    if (iStatus.Int() != KErrCancel)
        {
        IssueRequest();
        iHSFGProperty.Get(iHSFGStatus);
        iCallback.CallBack();
        }
    }

void CFGAppPropertyListener::DoCancel()
    {
    iHSFGProperty.Cancel();
    }

void CFGAppPropertyListener::IssueRequest()
    {
    iHSFGProperty.Subscribe(iStatus);
    SetActive();
    }

TInt CFGAppPropertyListener::GetHSFGStatus()
    {
    return iHSFGStatus;
    }
    


