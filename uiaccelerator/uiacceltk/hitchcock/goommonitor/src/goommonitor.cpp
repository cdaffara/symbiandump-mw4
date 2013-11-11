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


#include "goommonitor.h"
#include "goommemorymonitor.h"
#include "goommonitorclientserver.h"
#include "goomtraces.h"

const TInt KStackSize = 0x2000;

_LIT(KGOOMWatcherThreadName, "GOOM FW");


// Implements just Error() to avoid panic
NONSHARABLE_CLASS(CSimpleScheduler) : public CActiveScheduler
    {
    void Error( TInt ) const{} // From CActiveScheduler
    };

// thread function for GOOM watcher
GLDEF_C TInt GOOMWatcherThreadFunction( TAny* )
    {
    FUNC_LOG;
    RDebug::Print(_L("GOOM: WatcherThreadFunction"));

    TInt err( KErrNone );

    CTrapCleanup* cleanup = CTrapCleanup::New();
    CActiveScheduler* scheduler = new CSimpleScheduler();
    CMemoryMonitor* goom = NULL;

    if ( !cleanup || !scheduler)
        {
        err = KErrNoMemory;
        }
    else
        {
        CActiveScheduler::Install( scheduler );
        TRAPD( err,
            {
            RThread me;
            me.SetPriority(EPriorityAbsoluteHigh);
            goom = CMemoryMonitor::NewL();
            } )

        if ( err == KErrNone )
            {
            // start the watchers
            CActiveScheduler::Start();
            }
        }

    delete cleanup;
    delete scheduler;
    delete goom;

	RDebug::Print(_L("GOOM: WatcherThreadFunction EXIT err %d"), err);
    return err;
    }

// Creates thread for GOOM watchers
EXPORT_C void CreateGOOMWatcherThreadL()
    {
    FUNC_LOG;
	RDebug::Print(_L("GOOM: CreateGoomWatcherThread"));
    RThread thread;
    TInt ret = thread.Create( KGOOMWatcherThreadName,
                              GOOMWatcherThreadFunction,
                              KStackSize, // stack size
                              NULL, // uses caller thread's heap
                              NULL );

    if ( ret == KErrNone )
        {
        thread.Resume();
        thread.Close();
        }

	RDebug::Print(_L("GOOM: CreateGoomWatcherThread created %d"), ret);
    User::LeaveIfError( ret );
    }

void PanicClient(const RMessagePtr2& aMessage,TGOomMonitorClientPanic aPanic)
    {
    FUNC_LOG;

    aMessage.Panic(KGraphicsMemoryMonitorServerName, aPanic);
    }

// End of file.
