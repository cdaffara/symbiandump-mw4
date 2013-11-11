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
#include "screensaverpropertylistener.h"

#include <platform/mw/ScreensaverInternalPSKeys.h>


CScreenSaverPropertyListener::CScreenSaverPropertyListener(TCallBack aCallback) : CActive(EPriorityStandard), iCallback(aCallback)     
    {
    CActiveScheduler::Add(this);
    }

CScreenSaverPropertyListener::~CScreenSaverPropertyListener()
    {
    Cancel();
    iSCProperty.Close();
    }

CScreenSaverPropertyListener* CScreenSaverPropertyListener::NewL(TCallBack aCallback)
    {
    CScreenSaverPropertyListener* me = new (ELeave)CScreenSaverPropertyListener(aCallback);
    CleanupStack::PushL(me);
    me->ConstructL();
    CleanupStack::Pop();
    return me;
    }

void CScreenSaverPropertyListener::ConstructL()
    {
    User::LeaveIfError(iSCProperty.Attach(KPSUidScreenSaver, KScreenSaverOn));
    }

void CScreenSaverPropertyListener::RunL()
    {
    if (iStatus.Int() != KErrCancel)
        {
        IssueRequest();
        iSCProperty.Get(iSCStatus);
        iCallback.CallBack();
        }
    }

void CScreenSaverPropertyListener::DoCancel()
    {
    iSCProperty.Cancel();
    }

void CScreenSaverPropertyListener::IssueRequest()
    {
    iSCProperty.Subscribe(iStatus);
    SetActive();
    }

TInt CScreenSaverPropertyListener::GetScreenSaverStatus()
    {
    return iSCStatus;
    }
    


