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
 * Description:  Implementation
 *
 */

#include <usbuinotif.h> 

#include "cusbnotifmanager.h"
#include "cusbwaitnotifier.h"
#include "cusbwarningnotifier.h"
#include "cusbindicatornotifier.h"

#include "debug.h"
#include "panic.h"

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CWaitNotifierInfo::CWaitNotifierInfo(CUsbNotifier* aWaitNotifier,
        MWaitNotifierObserver& aObserver) :
    iWaitNotifier(aWaitNotifier), iObserver(aObserver)
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CWaitNotifierInfo::ConstructL()
    {
    LOG_FUNC
    // owenrship for iWaitNotifier transferred in default constructor.
    // this object is responsible for deletion of the iWaitNotifier then	
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CWaitNotifierInfo* CWaitNotifierInfo::NewL(CUsbNotifier* aWaitNotifier,
        MWaitNotifierObserver& aObserver)
    {
    LOG_FUNC

    CWaitNotifierInfo* self = new (ELeave) CWaitNotifierInfo(aWaitNotifier,
            aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CWaitNotifierInfo::~CWaitNotifierInfo()
    {
    LOG_FUNC
    delete iWaitNotifier;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbNotifier* CWaitNotifierInfo::WaitNotifier() const
    {
    return iWaitNotifier;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
MWaitNotifierObserver* CWaitNotifierInfo::Observer() const
    {
    return &iObserver;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbNotifManager* CUsbNotifManager::NewL(CUsbOtgWatcher& aOtgWatcher)
    {
    LOG_FUNC

    CUsbNotifManager* self = new (ELeave) CUsbNotifManager();
    CleanupStack::PushL(self);
    self->ConstructL(aOtgWatcher);
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbNotifManager::ConstructL(CUsbOtgWatcher& aOtgWatcher)
    {
    LOG_FUNC

    LEAVEIFERROR(iNotifier.Connect());

    iIndicatorNotifier = CUsbIndicatorNotifier::NewL(*this, aOtgWatcher);

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbNotifManager::CUsbNotifManager()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbNotifManager::~CUsbNotifManager()
    {
    LOG_FUNC

    CloseAllNotifiers();

    delete iIndicatorNotifier;

    iNotifier.Close();

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbNotifManager::ShowNotifierL(TUid aCat, TUint aNotifId,
        MWaitNotifierObserver* aObserver)
    {
    LOG_FUNC

    LOG3( "aCat = 0x%X aNotifId = %d, aObserver=0x%X" , aCat, aNotifId, aObserver);

    // remove non-feedback notifiers from the list
    for (TUint i(0); i < iWaitNotifiers.Count(); ++i)
        {
        CWaitNotifierInfo* waitNotifierInfo = iWaitNotifiers[i];
        if (!(waitNotifierInfo->WaitNotifier()->IsFeedbackNeeded()))
            {
            delete waitNotifierInfo;
            iWaitNotifiers.Remove(i);
            }
        }
    //If not suspened a short while between closing a present note and showing a new one,
    //error - CUsbWaitNotifier::RunL iStatus = 4 - will happen. Necessary here to pause a while 
    //to allow notfier framework to do some cleaning work.
    //Should find another way to solve this problem.
    const TUint KTenthOfASecond = 100000;
    User::After(TTimeIntervalMicroSeconds32(KTenthOfASecond));

    // can not have switch-case selector here, due to constants are of type Uids
    // notifier requires feedback => create CWaitNotifier for it
    // As notifiers can be more than two types such as warning, error, indicator ...
    // to create concret notifiers according to categories instead of checking aObserver null
    CUsbNotifier* notifier(NULL);
    if (aCat == KUsbUiNotifOtgError)
        {
        ASSERT_PANIC(aObserver != NULL, EWrongNotifierCategory);
        notifier = CUsbWaitNotifier::NewL(iNotifier, *this, aNotifId);
        }
    else if (aCat == KUsbUiNotifOtgWarning)
        {
        notifier = CUsbWarningNotifier::NewL(iNotifier, *this, aNotifId);
        }
    else
        {
        LOG1("Unexpected aCat = 0x%X", aCat );
        PANIC(EWrongNotifierCategory);
        }

    CleanupStack::PushL(notifier);

    iWaitNotifiers.AppendL(CWaitNotifierInfo::NewL(notifier, *aObserver));

    notifier->ShowL();

    CleanupStack::Pop(notifier);
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CUsbNotifManager::NotifierShowCompletedL(CUsbNotifier& aWaitNotifier,
        TInt aResult, TInt aFeedback)
    {
    LOG_FUNC

    LOG1( "aResult = %d", aResult);

    // remove Notifier from the list
    for (TUint i(0); i < iWaitNotifiers.Count(); ++i)
        {
        if (&aWaitNotifier == iWaitNotifiers[i]->WaitNotifier())
            {
            MWaitNotifierObserver* observer = iWaitNotifiers[i]->Observer();

            delete iWaitNotifiers[i];
            iWaitNotifiers.Remove(i);

            if (observer)
                observer->WaitNotifierCompletedL(aFeedback);
            }
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbNotifManager::CloseAllNotifiers()
    {
    LOG_FUNC

    iWaitNotifiers.ResetAndDestroy();
    iIndicatorNotifier->Close();
    }
