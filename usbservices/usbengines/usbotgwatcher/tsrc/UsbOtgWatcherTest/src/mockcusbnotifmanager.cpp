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

#include <usbuinotif.h> 
#include <NormalHardcodedAssert.h>

#include "stifassertmacros.h"

#include "mockcusbnotifmanager.h"

#include "debug.h"
#include "panic.h"

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CUsbNotifManager::CUsbNotifManager() :
    iNotifId(INVALID_VALUE), 
    iObserver(INVALID_VALUE),
    iUsbIconBlinking(EFalse),
    iUsbIconShown(EFalse)
	{
	iCat.iUid = INVALID_VALUE;
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
void CUsbNotifManager::ConstructL()
    {
	LOG(  ( "[USBOTGWATCHER]\tMockCUsbNotifManager::ConstructL" ) );
	
    }
    
CUsbNotifManager* CUsbNotifManager::NewL(CUsbOtgWatcher& aOtgWatcher)
	{	
	LOG(  ( "[USBOTGWATCHER]\tMockCUsbNotifManager::NewL" ) );
	
    CUsbNotifManager* self = new( ELeave ) CUsbNotifManager();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;		
	}
		
// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CUsbNotifManager::~CUsbNotifManager()
    {
    LOG(  ( "[USBOTGWATCHER]\tMockCUsbNotifManager::~CUsbNotifManager" ) );

    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
void CUsbNotifManager::ShowNotifierL(TUid aCat, TUint aNotifId, MWaitNotifierObserver* aObserver ) 
{    
    LOG3 ("[USBOTGWATCHER]\tMockCUsbNotifManager::ShowNotifier aCat = 0x%X aNotifId = %d, aObserver=0x%X", aCat, aNotifId, aObserver);
    iCat = aCat;
    iNotifId = aNotifId;
    iObserver = aObserver;
}
    
void CUsbNotifManager::BlinkIndicatorL(TBool aBlinking)
    {
    LOG1 ( "[USBOTGWATCHER]\tMockCUsbNotifManager::BlinkIndicatorL, aBlinking=%d", aBlinking);

    iUsbIconBlinking = aBlinking;
    }

void CUsbNotifManager::ShowIndicatorL(TBool aVisible)
    {
    LOG1 ( "[USBOTGWATCHER]\tMockCUsbNotifManager::ShowIndicatorL, aVisible=%d", aVisible);

    iUsbIconShown = aVisible;
    iUsbIconBlinking = EFalse;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CUsbNotifManager::NotifierShowCompletedL(CUsbNotifier& aWaitNotifier, TInt aResult, TInt aFeedback)
{
	LOG1 ( "[USBOTGWATCHER]\tMockCUsbNotifManager::NotifierShowCompleted aResult = %d", aResult);

}

void CUsbNotifManager::CloseAllNotifiers()
{
    LOG(  ( "[USBOTGWATCHER]\tMockCUsbNotifManager::CloseAllNotifiers" ) );
    
}

