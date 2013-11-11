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


#ifndef GOOMCLOSEAPP_H_
#define GOOMCLOSEAPP_H_


#include <apgtask.h>

#include "goomaction.h"

class CGOomAppCloseTimer;
class CGOomAppCloseWatcher;
class TApaTask;
class TActionRef;

/*
 * The OOM action of closing an application in order to free up memory.
 *
 *  @lib oommonitor.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS(CGOomCloseApp) : public CGOomAction
    {
public:
    /**
     * Two-phased constructor.
     * @param aStateChangeObserver an observer to the state of the application
     * @param aWs a connected handle to a window server session
     */
    static CGOomCloseApp* NewL(MGOomActionObserver& aStateChangeObserver, RWsSession& aWs);
        
    //from CGOomAction
    
    /**
     * Close the application in order to free memory
     * Call the CGOomAction::MemoryFreed when it is done
     * @param aBytesRequested not used for clsoe app actions
     */
    virtual void FreeMemory(TInt aBytesRequested, TBool aUseSwRendering);
    
    ~CGOomCloseApp();
    
    /**
     * Callback from CGOomAppCloseWatcher and CGOomAppCloseTimer
     */
    void CloseAppEvent();
    
    inline TBool IsRunning();

    /**
     * Configure, or reconfigure the CGOomCloseApp object
     * Action objects are reused to minimize any memory allocation when memory is low
     */
    void Reconfigure(const TActionRef& aRef);

    inline TUint WgId() const;
    
    
    /** Ask nice */
    void ConditionalClose();
    
    /** When the gentle push is not enough (i.e. force is required) **/
    void KillTask();
    
    void KillTaskWaitDone();
    
private:
    
    CGOomCloseApp(MGOomActionObserver& aStateChangeObserver, RWsSession& aWs);
    
    void ConstructL();
    
    TUint iWgId;
    TInt iCloseTimeout; 
    TInt iWaitAfterClose; 
        
    TBool iAppCloserRunning;
    TApaTask iCurrentTask;

    /**
     * A timeout for an app close action
     * Own
     */
    CGOomAppCloseTimer* iAppCloseTimer;

    /**
     * A watcher for the application closing
     * Own
     */
    CGOomAppCloseWatcher* iAppCloseWatcher; 
    
    TBool iAlreadyGaveUp;
    };

#include "goomcloseapp.inl"

#endif /*GOOMCLOSEAPP_H_*/
