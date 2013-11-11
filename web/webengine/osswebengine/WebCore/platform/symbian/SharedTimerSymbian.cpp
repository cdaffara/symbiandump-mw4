/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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


#include "config.h"
#include "SharedTimer.h"
#include <wtf/Assertions.h>
#include <e32std.h>
#include <e32base.h>
#include <Timer.h>

static TInt64 remainingMicro = 0;
static bool shutdownInProgress = false;

namespace WebCore {

static CPeriodic* sharedTimer;
static void (*sharedTimerFiredFunction)();

void setSharedTimerFiredFunction(void (*f)())
    {
    ASSERT(!sharedTimerFiredFunction || sharedTimerFiredFunction == f);

    sharedTimerFiredFunction = f;
    }

static TInt timerFired( TAny* /*aPtr*/ )
    {
    if (shutdownInProgress)
        {
        return KErrNone;
        }
    if( remainingMicro == 0 )
        {
        sharedTimerFiredFunction();
        }
    else
        {
        setSharedTimerFireTime( -1 );
        }
    return KErrNone;
    }

void setSharedTimerFireTime(double fireTime)
    {
    if (shutdownInProgress)
        {
        return;
        }
    ASSERT(sharedTimerFiredFunction);

    if (sharedTimer)
        {
        sharedTimer->Cancel();
        delete sharedTimer;
        sharedTimer = NULL;
        }
    if (fireTime != -1)
        remainingMicro = 0;

    sharedTimer = CPeriodic::New( CActive::EPriorityStandard );
    if( sharedTimer )
        {
        TInt64 interval( remainingMicro );
        if( remainingMicro == 0 )
            {
            // fireTime comes in second resolution
            TTime fireDate( TTime(fireTime * 1000000 ).Int64() );

            TTime time;
            time.HomeTime();
            interval = fireDate.Int64() - time.Int64();
            }
        interval = interval < 0 ? 0 : interval;
        //
        TInt t;
        if (interval<(TInt)(KMaxTInt32))
            {
            t = interval;
            remainingMicro = 0;
            }
        else
            {
            t = KMaxTInt32;
            remainingMicro = interval - KMaxTInt32;
            }
        sharedTimer->Start( t, 0, timerFired);
        }
    }

void stopSharedTimer()
    {
    if (sharedTimer)
        {
        sharedTimer->Cancel();
        delete sharedTimer;
        sharedTimer = NULL;
        }
    remainingMicro = 0;
    /*
     * The static boolean variable shutdownInProgress, must be reset in scenario's where a browser control instance is deleted and a new
     * instance is created without actually closing the application.
     */
    shutdownInProgress = false ;
    }

void shutdownSharedTimer()
    {
    shutdownInProgress = true;
    stopSharedTimer();
    TimerBase::deleteTimerHeap();
    }

void initSharedTimer()
    {
    shutdownInProgress = false;
    }

}
