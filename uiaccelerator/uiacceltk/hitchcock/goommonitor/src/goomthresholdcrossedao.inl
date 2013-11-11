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
* Description:  A Synch Object thread for EGL resource monitoring
*
*/


typedef void* EGLSync;
typedef EGLSync (*eglCreateSync)(EGLDisplay dpy, EGLenum type, const EGLint *attrib_list);
typedef EGLBoolean (*eglDestroySync)(EGLDisplay dpy, EGLSync sync);
typedef EGLint (*eglClientWaitSync)(EGLDisplay dpy, EGLSync sync, EGLint flags, TUint64 timeout);
#ifndef EGL_SYNC_CONDITION_KHR
#define EGL_SYNC_CONDITION_KHR  0x30f8
#endif


_LIT(KGoomAsyncWaitThreadName,"goom_asy");

NONSHARABLE_CLASS(CGoomThresholdCrossed): public CActive
    {
    public:
    CGoomThresholdCrossed(CMemoryMonitor& aMonitor, TUint aAction, TInt aThreshold)
        :CActive(CActive::EPriorityHigh), 
        iMarkerState(KErrNotFound),
        iActiveThreshold(aThreshold),
        iAction(aAction),
        iThread(RThread().Id()),
        iMonitor(aMonitor)
        {
        FUNC_LOG;
        CActiveScheduler::Add(this);
        iSema.CreateLocal();
        iStop.CreateLocal();
        iThreadName.Append(KGoomAsyncWaitThreadName);
        iThreadName.AppendNum(aAction); // assuming single observer for an action
        RequestNotifications();
        }

    // ---------------------------------------------------------------------------
    // Thread name
    // Thread name consists on constant part and action that triggers the ao 
    // ---------------------------------------------------------------------------
    //
    const TDesC& ThreadName()
        {
        FUNC_LOG;
        return iThreadName;
        }
    
    void Stop()
        {
        if (!iPaused)
            {
            iStop.Wait();
            iPaused = ETrue;
            ResetThresholds();
            }
         }   
    void Continue()
        {
        if (iPaused)
            {
            iStop.Signal();
            iPaused = EFalse;
            }
        }
    // ---------------------------------------------------------------------------
    // Destructor
    // Standard destructor, cancels pending request and releases egl wait
    // ---------------------------------------------------------------------------
    //
    ~CGoomThresholdCrossed()
        {
        FUNC_LOG;
        Cancel();
        iSema.Close();
        iStop.Close();
        }        
 
    // ---------------------------------------------------------------------------
    // SetThreshold
    // Destroys the synch object so EGL busy loop will get new values 
    // ---------------------------------------------------------------------------
    //
    void SetThreshold(TInt aThreshold)
        {
        FUNC_LOG;
        if (iActiveThreshold != aThreshold)
            {
            iActiveThreshold = aThreshold;
            ResetThresholds();
            }
        }

    // ---------------------------------------------------------------------------
    // ResetThresholds
    // See SetThreshold. Releases synch object 
    // ---------------------------------------------------------------------------
    //
    void ResetThresholds()
        {
        FUNC_LOG;
        if (iMarkerState == KErrNone)
            {
            iMarkerState = KErrDied;
            eglDestroySync_fptr(iDpy, iMarker);
            }  
        }
            
    // ---------------------------------------------------------------------------
    // StartRequestEvents
    // Busy loop that is blocked by egl wait. exit on Cancel. 
    // Runs on separate thread.
    // ---------------------------------------------------------------------------
    //
    void StartRequestEvents()
        {
        FUNC_LOG;
        iDpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        EGLint major, minor;
        eglInitialize(iDpy, &major, &minor);
        
        eglCreateSync_fptr = (eglCreateSync)eglGetProcAddress("eglCreateSyncKHR");
        eglDestroySync_fptr = (eglDestroySync)eglGetProcAddress("eglDestroySyncKHR");
        eglClientWaitSync_fptr = (eglClientWaitSync)eglGetProcAddress("eglClientWaitSyncKHR");
        if (!eglCreateSync_fptr || !eglDestroySync_fptr || !eglClientWaitSync_fptr )
            {
            /* Free allocated memory */
            eglTerminate(iDpy);
            return;
            }
            
        while(!iCancelled)
            {
            iStop.Wait();
            iStop.Signal();
            TUint usedThreshold = Min(iActiveThreshold,32*1024*1024) ;
            TUint action = iAction;
            const EGLint sync_attribs[] =
                {
                EGL_SYNC_CONDITION_KHR,              action,
                EGL_PROF_MEMORY_USAGE_THRESHOLD_NOK, 32*1024*1024-usedThreshold,
                EGL_NONE
                };

            /* Create a resource profiling sync */
            iMarker = eglCreateSync_fptr(iDpy,
                                         EGL_SYNC_RESOURCE_PROFILING_NOK,
                                         sync_attribs);
            iMarkerState = KErrNone;

            /* Wait until graphics memory usage exceeds the given threshold */
            eglClientWaitSync_fptr(iDpy, iMarker, 0, EGL_FOREVER_KHR);
            
            if (iMarkerState != KErrDied)
                {
                eglDestroySync_fptr(iDpy, iMarker);
                Trigger(action/*, usedThreshold*/);
                }                
            }            

        /* Free allocated memory */
        eglTerminate(iDpy);
        iMarkerState = KErrNotFound;
        }
 
    // ---------------------------------------------------------------------------
    // RequestNotifications
    // Setting listener for changes, no queue at the moment
    // ---------------------------------------------------------------------------
    //
    void RequestNotifications(TBool aWasPaused = EFalse)
        {
        FUNC_LOG;
        iSema.Wait();
        iStatus = KRequestPending;
        SetActive();
        iSema.Signal();
        if (!aWasPaused)
            {
            Continue();
            }            
        }
    
    // ---------------------------------------------------------------------------
    // Trigger
    // Letting mainthread to know that threshold was crossed
    // ---------------------------------------------------------------------------
    //
    void Trigger(TInt aReason)
        {
        FUNC_LOG;
        iSema.Wait();    
        if (IsActive() && iStatus == KRequestPending)
            {
            Stop();
            RThread t;
            TInt err = t.Open(iThread);
            if (err)
                {
                RDebug::Print(_L("CGoomThresholdCrossed::Trigger() RThread::Open() error: %d"), err );
                User::Invariant();
                }
            TRequestStatus* status = &iStatus;
            t.RequestComplete(status, aReason);
            t.Close();
            }
        iSema.Signal();
        }
        
    // ---------------------------------------------------------------------------
    // RunL()
    // Thread safe way to hanlde crossed threshold in main thread
    // ---------------------------------------------------------------------------
    //
    void RunL()
        {
        FUNC_LOG;
        switch (iStatus.Int())
            {
        case KErrNone:
            break;
        case KErrCancel:
            return;
        default:
            break;
            }
		
	TInt thresholdType = iStatus.Int();
		
        TRAP_IGNORE(iMonitor.FreeMemThresholdCrossedL(thresholdType, iAction));
        
        RequestNotifications(iPaused);
        }
    
    // ---------------------------------------------------------------------------
    // Cancel
    // Cancels the busy loop even the Object was not active
    // ---------------------------------------------------------------------------
    //
    void Cancel()
        {
        FUNC_LOG;
        iCancelled = ETrue;
        ResetThresholds();
        CActive::Cancel();
        }
        
    // ---------------------------------------------------------------------------
    // DoCancel
    // Cancels the active object
    // ---------------------------------------------------------------------------
    //
    void DoCancel()
        {
        FUNC_LOG;
        Trigger(KErrCancel);
        }
    
    EGLSync iMarker;
    TInt iMarkerState;
    TUint iActiveThreshold;
    TUint iAction;
    TBool iCancelled;
    RCriticalSection iSema;
    TThreadId iThread;
    eglCreateSync  eglCreateSync_fptr;
    eglDestroySync eglDestroySync_fptr;
    eglClientWaitSync eglClientWaitSync_fptr ; 
    CMemoryMonitor& iMonitor;
    TBuf<16> iThreadName;
    EGLDisplay iDpy;
    TBool iPaused;
    RCriticalSection iStop;
    };

// ---------------------------------------------------------------------------
// Entry point into the new thread
// ---------------------------------------------------------------------------
//   
GLDEF_C TInt EGLSynchObjThreadStartFunction(TAny* aBridge)
    {
    CTrapCleanup* trapCleanup = CTrapCleanup::New();
    if (!trapCleanup)
        {
        return KErrNoMemory;
        }
        
    CGoomThresholdCrossed* ao = static_cast<CGoomThresholdCrossed*>(aBridge);
    TInt err = User::RenameThread(ao->ThreadName());
    RThread().SetPriority(EPriorityAbsoluteHigh);
    ao->StartRequestEvents();

    delete trapCleanup;
    return err;
    }

CGoomThresholdCrossed* CreateThresholdCrossedThreadL(CMemoryMonitor& aMonitor, TUint aAction, TInt aThreshold)
    {
    FUNC_LOG;
    CGoomThresholdCrossed* ao = new (ELeave) CGoomThresholdCrossed(aMonitor, aAction, aThreshold);
    CleanupStack::PushL(ao);
    RThread syncThread;

    User::LeaveIfError(syncThread.Create(
            ao->ThreadName(),
            EGLSynchObjThreadStartFunction,
            16384, // magic
            0, // uses same heap
            (TAny*)ao, 
            EOwnerThread));

    syncThread.Resume();
    syncThread.Close();
    CleanupStack::Pop();
    return ao;
    }

