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

#include <e32base.h>
#include <NormalHardcodedAssert.h>

#include "stifassertmacros.h"

#include "mockcusbtimer.h"

#include "debug.h"

CUsbTimer::CUsbTimer(MUsbTimerObserver& aObserver, TUsbTimerId aTimerId) :
    iObserver(&aObserver), 
    iActive(EFalse),
    iTimerId(aTimerId)
    {
    }

CUsbTimer::~CUsbTimer()
    {
    LOG(  ( "[USBOTGWATCHER]\tMockCUsbTimer::~CUsbTimer" ) );
    }

void CUsbTimer::ConstructL()
    {
    }

CUsbTimer* CUsbTimer::NewL(MUsbTimerObserver& anObserver, TUsbTimerId aTimerId)
    {
    LOG(  ( "[USBOTGWATCHER]\tMockCUsbTimer::NewL" ) );

    CUsbTimer* self = new ( ELeave ) CUsbTimer(anObserver, aTimerId);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // pop self
    return self;
    }

void CUsbTimer::After(TInt aMilliseconds)
    {
    LOG1 ( "[USBOTGWATCHER]\tMockCUsbTimer::After aMilliseconds %d", aMilliseconds);
    iActive = ETrue;
    }

void CUsbTimer::Cancel()
    {
    }

TBool CUsbTimer::IsActive()
    {
    return iActive;
    }

void CUsbTimer::TriggerTimerElapsedL()
    {
    //STIF_ASSERT_NOT_NULL(iObserver);
    LOG1( "[USBOTGWATCHER]\tMockCUsbTimer::TriggerTimerElapsedL  iObserver: %d", (iObserver ? 1 : 0 ));
    LOG1( "[USBOTGWATCHER]\tMockCUsbTimer::TriggerTimerElapsedL  iTimerId: %d", (iTimerId ? 1 : 0 ));
    
    iObserver->TimerElapsedL(iTimerId);
    LOG(  ( "[USBOTGWATCHER]\tMockCUsbTimer::TriggerTimerElapsedL" ) );
    iActive = EFalse;
    }
