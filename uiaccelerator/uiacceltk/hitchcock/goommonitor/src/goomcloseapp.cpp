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
* Description:  Classes for executing GOOM actions (e.g. closing applications and running plugins).
*
*/


#include <apgtask.h>

#include "goommemorymonitor.h"
#include "goomwindowgrouplist.h"
#include "goomcloseapp.h"
#include "goomtraces.h"
#include "goomappclosetimer.h"
#include "goomappclosewatcher.h"
#include "goomactionref.h"
#include "goommonitorsession.h"


CGOomCloseApp* CGOomCloseApp::NewL(MGOomActionObserver& aStateChangeObserver, RWsSession& aWs)
    {
    FUNC_LOG;

    CGOomCloseApp* self = new (ELeave) CGOomCloseApp(aStateChangeObserver, aWs);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// Close the application in order to free memory
// Call the CGOomAction::MemoryFreed when it is done
void CGOomCloseApp::FreeMemory(TInt, TBool)
    {
    FUNC_LOG;

    if ( iAlreadyGaveUp )	//will this ever be true ??? iAlredyGaveup is set true in closeAppEvent, which is called only if app gets notified by goom, which happens only after this
        {
        if (iAppCloseTimer)
        	{
			iAppCloseTimer->Cancel();
    		}
			
    	if (iAppCloseWatcher)
        	{
			iAppCloseWatcher->Cancel();
			}
			
		RDebug::Printf("GOOM: I should not activate actions for app that already refused to exit !!!!");
        MemoryFreed(KErrNone);
        return;
        }    

    iAppCloserRunning = ETrue;
    
    // Set the TApaTask to the app
    iCurrentTask.SetWgId(iWgId);
    
    // Start a timer and the thread watcher 
    /*
	iAppCloseTimer->SetState(CGOomAppCloseTimer::EGOomAppClosing);
    iAppCloseTimer->After(iCloseTimeout * 1000);
    iAppCloseWatcher->Start(iCurrentTask);
    // Tell the app to close
    TRACES2("CGOomCloseApp::FreeMemory: Closing app with window group id %d Timeout = %d",iWgId, iCloseTimeout);
    iCurrentTask.EndTask();
	*/
	ConditionalClose();
    }

CGOomCloseApp::~CGOomCloseApp()
    {
    FUNC_LOG;

    if (iAppCloseTimer)
        iAppCloseTimer->Cancel();
    
    if (iAppCloseWatcher)
        iAppCloseWatcher->Cancel();
    
    delete iAppCloseTimer;    
    delete iAppCloseWatcher;
    }

// Callback from CGOomAppCloseWatcher and CGOomAppCloseTimer
void CGOomCloseApp::CloseAppEvent()
    {
    FUNC_LOG;

    // The application has closed (or we have a timeout)
    iAppCloserRunning = EFalse;
    iAlreadyGaveUp = ETrue;
    
    if (iAppCloseTimer)
        {
        iAppCloseTimer->Cancel();
        iAppCloseTimer->SetState(CGOomAppCloseTimer::EGOomAppKilled);
        iAppCloseTimer->After(iWaitAfterClose * 1000);
        }
    
    if (iAppCloseWatcher)
        {
        iAppCloseWatcher->Cancel(); 
        }    
    }

void CGOomCloseApp::Reconfigure(const TActionRef& aRef)
    {
    FUNC_LOG;

    iWgId = aRef.WgId();    
    iCloseTimeout = aRef.CloseTimeout();
    iWaitAfterClose = aRef.WaitAfterClose();
	iAlreadyGaveUp = EFalse;
    }

void CGOomCloseApp::ConstructL()
    {
    FUNC_LOG;

    iAppCloseTimer = CGOomAppCloseTimer::NewL(*this);
    iAppCloseWatcher = new(ELeave) CGOomAppCloseWatcher(*this);
    }
        
CGOomCloseApp::CGOomCloseApp(MGOomActionObserver& aStateChangeObserver, RWsSession& aWs)
                                : CGOomAction(aStateChangeObserver), iCurrentTask(aWs)
    {
    FUNC_LOG;
    }

TBool IsConsumingMemory(TInt aWgId)
    {
    FUNC_LOG;
    // Something more efficient could be done here
    CMemoryMonitor* globalMemoryMonitor = static_cast<CMemoryMonitor*>(Dll::Tls());
    globalMemoryMonitor->GetWindowGroupList()->Refresh();
    return (globalMemoryMonitor->GetWindowGroupList()->GetIndexFromWgId(aWgId) != KErrNotFound);
    }    
    
void CGOomCloseApp::ConditionalClose()
    {
    FUNC_LOG;
    // Start a timer and the thread watcher 
    iAppCloseTimer->SetState(CGOomAppCloseTimer::EGOomAppClosing);
    iAppCloseTimer->After(iCloseTimeout * 1000);
    iAppCloseWatcher->Start(iCurrentTask);
    // Tell the app to close
	// We are not asking system applications to exit anyway, so we'll send legacy event only
	// even we have powermgmt capability 
    TRACES2("CGOomCloseApp::FreeMemory: Closing app with window group id %d Timeout = %d",iWgId, iCloseTimeout);
	TWsEvent event;
	event.SetType(EEventUser);
	TInt* eventData = (TInt*)(event.EventData());
	*eventData = EApaSystemEventShutdown;
	eventData++;
	*eventData = KGoomMemoryLowEvent;
	
	// should proxy the session..
    CMemoryMonitor* globalMemoryMonitor = static_cast<CMemoryMonitor*>(Dll::Tls());
    globalMemoryMonitor->iWs.SendEventToWindowGroup(iWgId, event);
    }    


// ----------------------------------------------
// Callback from iAppCloseTimer
// App refused to exit gracefully on given time
// ----------------------------------------------
//
void CGOomCloseApp::KillTask()
    {
    FUNC_LOG;
    if (iAppCloseWatcher)
        {
        iAppCloseWatcher->Cancel(); 
        }
        
    if(IsConsumingMemory(iWgId))    
        {
        TRACES1("REST IN PEACE - App wgid %d", iWgId);
        iCurrentTask.KillTask();
        iAppCloserRunning = EFalse; // not sure if intended (?)
        iAppCloseTimer->SetState(CGOomAppCloseTimer::EGOomAppKilled);
        iAppCloseTimer->After(iWaitAfterClose * 1000);
        }
    else
        { // application has released its graphics resources -> we are no more interested about it
        //CloseAppEvent();
        iAppCloserRunning = EFalse; 
        MemoryFreed(KErrNone);
        }    
    }

void CGOomCloseApp::KillTaskWaitDone()
    {
    FUNC_LOG;
    MemoryFreed(KErrNone);
    }

