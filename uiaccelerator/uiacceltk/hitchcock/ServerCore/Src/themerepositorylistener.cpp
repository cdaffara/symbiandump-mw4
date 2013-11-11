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
#include "themerepositorylistener.h"

#include <platform/mw/pslninternalcrkeys.h>
_LIT(KBgAnimExeName, "z:\\sys\\bin\\backgroundanimhost.exe");

CThemeRepositoryListener::CThemeRepositoryListener() : CActive(EPriorityStandard)
    {
    CActiveScheduler::Add(this);
    }

CThemeRepositoryListener::~CThemeRepositoryListener()
    {
    Cancel();
    delete iThemesRepository;
    }

CThemeRepositoryListener* CThemeRepositoryListener::NewL()
    {
    CThemeRepositoryListener* me = new (ELeave)CThemeRepositoryListener;
    CleanupStack::PushL(me);
    me->ConstructL();
    CleanupStack::Pop();
    return me;
    }

void DoStartBgAnim()
    {
    // start the app
    RProcess image;
    TInt err = image.Create(KBgAnimExeName,KNullDesC);
    if (!err)
        {
        image.Resume();
        }
    image.Close();
    }

void CThemeRepositoryListener::ConstructL()
    {
    iThemesRepository = CRepository::NewL(KCRUidThemes);
    User::LeaveIfError(iThemesRepository->Get(KThemesAnimBackgroundSupport, iPreviousValue));
    }


void CThemeRepositoryListener::RunL()
    {
    TInt value = 0;
    iThemesRepository->Get(KThemesAnimBackgroundSupport, value);
#if !defined(__WINS__)
    if (!value && value != iPreviousValue)
        {
        DoStartBgAnim();
        }
#endif
    iPreviousValue = value;
    
    if (iStatus.Int() != KErrCancel)
        {
        IssueRequest();
        }
    }

void CThemeRepositoryListener::DoCancel()
    {
    iThemesRepository->NotifyCancelAll();    
    }

void CThemeRepositoryListener::IssueRequest()
    {
    TInt err = iThemesRepository->NotifyRequest(KThemesAnimBackgroundSupport,iStatus);
    if (!err)
        {
        SetActive();
        }
    }

    


