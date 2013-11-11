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
* Description:  Main classes for Graphics Out of Memory Monitor
*
*/


#include <akneiksrvc.h>
#include <apgwgnam.h>
#include "goomwserveventreceiver.h"
#include "goommemorymonitor.h"
#include "goomtraces.h"
#include "goomconstants.hrh"

CWservEventReceiver::CWservEventReceiver(CMemoryMonitor& aMonitor, RWsSession& aWs)
: CActive(CActive::EPriorityStandard), iMonitor(aMonitor), iWs(aWs), iWg(aWs)
    {
    FUNC_LOG;

    CActiveScheduler::Add(this);
    }

CWservEventReceiver::~CWservEventReceiver()
    {
    FUNC_LOG;

    Cancel();
    iWg.Close();
    }

void CWservEventReceiver::ConstructL()
    {
    FUNC_LOG;

    User::LeaveIfError(iWg.Construct((TUint32)this, EFalse));
    iWg.SetOrdinalPosition(0, ECoeWinPriorityNeverAtFront);
    iWg.EnableFocusChangeEvents();
    iWg.AutoForeground(EFalse);
    iWg.EnableReceiptOfFocus(EFalse); 
    
    // first aid for triggering poll
    //iWg.EnableGroupChangeEvents();
    Queue();
    }

void CWservEventReceiver::Queue()
    {
    FUNC_LOG;

    iWs.EventReady(&iStatus);
    SetActive();
    }

void CWservEventReceiver::DoCancel()
    {
    FUNC_LOG;

    iWs.EventReadyCancel();
    }

void CWservEventReceiver::RunL()
    {
    FUNC_LOG;

    TWsEvent event;
    iWs.GetEvent(event);
    if (event.Type() == EEventFocusGroupChanged || event.Type() == EEventWindowGroupsChanged )
        {
        // The following is done in order to avoid changing application threshholds when fastswap is activated
        // 1) get the new focused app's details
        // 2) we check to see if the fastswap uid caused the change
        // 3a) If it didn't we proceed as normal: handleFocusedWgChangeL is called
        // 3b) If not, we skip handleFocusedWgChangeL
        CApaWindowGroupName* aWgName = CApaWindowGroupName::NewLC(iWs, iWs.GetFocusWindowGroup());
        if(aWgName->AppUid() != KUidFastSwap)
            {
            iMonitor.HandleFocusedWgChangeL(aWgName->AppUid().iUid);
            }
        CleanupStack::PopAndDestroy(aWgName);
        }
    
    Queue();
    }
