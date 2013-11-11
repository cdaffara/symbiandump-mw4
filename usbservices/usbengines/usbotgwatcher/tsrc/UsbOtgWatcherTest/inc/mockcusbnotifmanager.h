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

#ifndef C_USBNOTIFMANAGER_H
#define C_USBNOTIFMANAGER_H

#include <e32base.h>

#include <StifTestModule.h>
#include <StifLogger.h>

#include "tassertbase.h"
#include "cusbnotifier.h"
#include "cusbwarningnotifier.h"
#include "cusbotgwatcher.h"

#define INVALID_VALUE NULL

// forward declaration
class CUsbWaitNotifier; 
class CUsbNotifier;

class MWaitNotifierObserver
{
public:
	
	// feedback to observer
	virtual void WaitNotifierCompletedL(TInt aFeedback) = 0;
};

/**
 *  Function of the class is own and share RNotifier, own list of CWaitNotifier(s),
 *	show notifiers, which are synchronous, and cancel it with a delay 
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CUsbNotifManager : public TAssertBase
    {
friend class CtUsbOtgWatcher;
    friend class CUsbOtgWatcherTest;

public:    
	static CUsbNotifManager* NewL(CUsbOtgWatcher& aOtgWatcher);
	
	virtual ~CUsbNotifManager();

	void ShowNotifierL(TUid aCat, TUint aNotifId, MWaitNotifierObserver* aObserver = NULL );
	
    void BlinkIndicatorL(TBool aBlinking = ETrue);
    
    void ShowIndicatorL(TBool aVisible = ETrue);
	
	void CloseAllNotifiers();
	
	TBool GetUsbIconBlinking() { return iUsbIconBlinking; };
	
private:
	
    CUsbNotifManager();
    
    void ConstructL();
    
    // call back. Notifier show is over
    void NotifierShowCompletedL(CUsbNotifier& aWaitNotifier, TInt aResult, TInt aFeedback);

private: // data
    TUid iCat;
    TUint iNotifId;
    MWaitNotifierObserver* iObserver;
    
    TBool iUsbIconBlinking;
    TBool iUsbIconShown;
    };

#endif // C_USBNOTIFMANAGER_H
