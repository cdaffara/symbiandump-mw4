/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Socket Interaction Thread implementation.
*
*/



#ifndef __SIT_H__
#define __SIT_H__

#include <e32std.h>
#include <e32base.h>

#include "eventmediatorapi.h"
#include "sitdeathobserver.h"

#ifndef TRAP_IGNORE
#define TRAP_IGNORE(_s) {TInt _ignore;TTrap __t;if (__t.Trap(_ignore)==0){_s;TTrap::UnTrap();}}
#endif

_LIT(KSitName, "SocketInteractionThread");

static const TUid KUidSit = {0x101FD28D};

enum TSitPanic
    {
    EPanicUnknownEventType = 1,
    EPanicInvalidTaskHandlerState,
    EPanicUnexpectedEventOccured                             
    };

const TUint KSitMaxHeapSize = 0x80000;  // 512 kB

/**
 * Socket Interaction Thread implementation
 */
class CSit : public CBase, public MSitDeathListener
    {
/**
 ** @internalComponent
 */
public:
    IMPORT_C CSit(MSitDeathListener* aSitDeathListener);
    IMPORT_C ~CSit();

    IMPORT_C void StartL();
    IMPORT_C static TBool EventRequiresSit(TEventType aEventType);
    IMPORT_C TThreadId ThreadId();
    IMPORT_C static TEventType FindTaskRequestEventType(TEventType aCancelEventType);
    IMPORT_C static TEventType FindCancelEventType(TEventType aTaskRequestEventType);
    IMPORT_C static TBool IsTaskCancellationObservationRequest(TEventType aEventType);
    
public: // From MSitDeathListener    
    void SitDied();
    
public:    
    static HBufC16* To16BitL(const TDesC8& aDes);

private:
    void StartWorkingL();
    void StartThreadL();
   
    static TInt ThreadFunction(TAny* aParameters);

private:
    RThread* iTaskThread;

    CSitDeathObserver* iSitDeathObserver;
    MSitDeathListener* iSitDeathListener;

    TEventType iEventType;
    };

#endif // __SIT_H__
