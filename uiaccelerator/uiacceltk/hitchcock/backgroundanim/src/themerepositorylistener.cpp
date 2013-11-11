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


CThemeRepositoryListener::CThemeRepositoryListener(TBool* aValueToChange) : CActive(EPriorityStandard),     
    iValueToChange(aValueToChange)
    {
    CActiveScheduler::Add(this);
    }

CThemeRepositoryListener::~CThemeRepositoryListener()
    {
    Cancel();
    delete iThemesRepository;
    }

CThemeRepositoryListener* CThemeRepositoryListener::NewL(TBool* aValueToChange)
    {
    CThemeRepositoryListener* me = new (ELeave)CThemeRepositoryListener(aValueToChange);
    CleanupStack::PushL(me);
    me->ConstructL();
    CleanupStack::Pop();
    return me;
    }

void CThemeRepositoryListener::ConstructL()
    {
    iThemesRepository = CRepository::NewL(KCRUidThemes);
    iThemesRepository->Get(KThemesAnimBackgroundSupport, iPreviousValue);
    }

void CThemeRepositoryListener::RunL()
    {
    TInt value = 0;
    iThemesRepository->Get(KThemesAnimBackgroundSupport, value);
    // we are only interested in the value if it
    // is something else than 0
    // as it means that we should stop...
    if (value)
        {
        *iValueToChange = EFalse;
        }
    else
        {
        *iValueToChange = ETrue;
        }

    
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

void CThemeRepositoryListener::GPuMemLow(TBool aLow)
    {
    iPreviousValue = aLow ? KMaxTInt : 0;
    iThemesRepository->Set(KThemesAnimBackgroundSupport, iPreviousValue );
    }
    


