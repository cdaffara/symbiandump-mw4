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
* Description:  Classes for executing Graphics OOM actions (e.g. closing applications and running plugins).
*
*/


#ifndef GOOMACTIONLIST_H_
#define GOOMACTIONLIST_H_

#include <e32def.h>
#include <e32base.h>

#include "goommonitorplugin.h"
#include "goommemorymonitor.h"

class CGOomWindowGroupList;
class CMemoryMonitorServer;
class CGOomCloseApp;
class TActionRef;
class CGOomRunPlugin;
class CGOomConfig;

/*
 * Interface for reporting a change of state in a GOOM action
 * e.g. that the action has changed from a freeing-memory state to an idle state
 * 
 * @lib oommonitor.lib
 * @since S60 v5.0
 */
class MGOomActionObserver
    {
public:
    virtual void StateChanged() = 0;
    virtual TInt ClientId() = 0;
    };


template <class T> 
/**
 *  A class for getting instances of all of the GOOM ECom plugins
 *  This class is templated because we actually need two types of list
 *  One list for V1 plugins
 *  One list for V2 plugins
 *  The functionality of each list is nearly identical, hence the templated class
 *  
 * @lib oommonitor.lib
 * @since S60 v5.0
 */
NONSHARABLE_CLASS(CGOomPluginList) : public CBase
    {
public:
    
    /**
     * Two-phased constructor.
     * @param aInterfaceUid The interface of the plugin (either V1 or V2) depending on the templated type
     */
    static CGOomPluginList* NewL(TInt aInterfaceUid);
    inline TInt Count();
    inline T& Implementation(TInt aIndex);
    inline TInt32 Uid(TInt aIndex);
    
    ~CGOomPluginList();
    
private:
    CGOomPluginList();
    void ConstructL(TInt aInterfaceUid);
    
private: // data
    
    struct TPlugin
        {
        TPlugin();
        T* iImpl;
        TUid iDtorUid;
        TInt32 iUid;
        };
    
    RArray<TPlugin> iPlugins;
    };

    
/*
 * The list of possible OOM actions to be run.
 * 
 * This class is responsible for identifying the best action(s) to execute at a given time.
 * This decision is based on the properties of each action and idle time of the target apps.
 *
 *  @lib oommonitor.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS(CGOomActionList) : public CBase, public MGOomActionObserver
    {
public:

    /**
     * Two-phased constructor.
     * @param aMonitor the owning Memory Monitor
     * @param aServer 
     * @param aWs a handle to a window server session, opened by the Memory Monitor
     * @param aConfig
     */
    static CGOomActionList* NewL(CMemoryMonitor& aMonitor, CMemoryMonitorServer& aServer, RWsSession& aWs, CGOomConfig& aConfig);

    ~CGOomActionList();
    
    /**
     * Creates a list of actions based on the contents of the config and the current window group list
     * Any old actions will be replaced.
     * 
     * @since S60 5.0
     * @param aWindowGroupList
     * @param aConfig
     */
    void BuildPluginActionListL(CGOomWindowGroupList& aWindowGroupList, CGOomConfig& aConfig);    
    void BuildKillAppActionListL(CGOomWindowGroupList& aWindowGroupList, CGOomConfig& aConfig);
    
    /**
     * Execute the OOM actions according to their priority
     * Actions are run in batches according to their sync mode
     * 
     * @since S60 5.0
     * @param aMaxPriority The maximum priority of actions to run 
     */     
    void FreeMemory(TInt aMaxPriority);
    
    /**
     * Sets the target; the maximum prioirity up to which actions are run when an GOomMonitor event 
     * occurs
     * 
     * @since S60 5.0
     * @param aCurrentTarget the prio
     */    
    inline void SetCurrentTarget(TUint aCurrentTarget);
    
    /**
     * Should be called when the memory situation is good, i.e. the actions run have released enough memory 
     * so that the device is above the current target.
     * It results in notifications of the good memory state to all plugins
     * 
     * @since S60 5.0
     */     
    void MemoryGood();
        
    /**
     * Checks the current memory level of the device and compares this with the current target
     * 
     * @since S60 5.0
     * @param aFreeMemory the amount of free memory left on the device
     * @return ETrue if the free memory is above the current target, else EFalse
     */    
    TBool FreeMemoryAboveTarget(TInt& aFreeMemory);
        
    /**
     * Compares priorites of two actions, hard-coded rules are used to determine the order 
     * of actions with equal priority:
     * - application plugins are run in preference to system plugins
     * - appliction plugins where the target app is not running are run first
     * - Z order of the target app determines order of the rest of the application plugins (furthest back first)
     * - system plugins are run before app close actions
     * - Z order determines the prioirty of app close actions (furthest back first)
     * 
     * @since S60 5.0
     */    
    static TInt ComparePriorities(const TActionRef& aPos1, const TActionRef& aPos2 );

    /**
     * A callback from the UI framework that an applications has failed to respond to the close event.
     * Allows us to cleanup the related app close action object and move on the next available action.
     * 
     * @since S60 5.0
     * @param aWgId the window group ID of the application that has not closed.
     */    
    void AppNotExiting(TInt aWgId);
    
    /**
     * Sets the flag to free memory using sw rendering. For Alf and other plugins that use sw rendering to reduces gpu mem usage.
     * @since S60 5.0
     * @param aUseSwRendering ETrue if sw rendering to be used.
     */   
    void SetUseSwRendering(TBool aUseSwRendering);
    
    /**
     * Returns the flag to free memory using sw rendering. For Alf and other plugins that use sw rendering to reduces gpu mem usage.
     * @since S60 5.0
     */ 
    TBool UseSwRendering();
    
// from MGOomActionObserver

    /**
     * 
     * from MGOomActionObserver
     * 
     * An action has changed state (possibly it has completed freeing memory)
     */
    void StateChanged();    
    
    /** The secure id of a client that requested free memory **/
    TInt ClientId()
        {
        return iMonitor.ActiveClientId();
        }
    
    void SetPriority(TInt aWgId, TInt aPriority);
    
    TBool IsRunningKillAppActions();
    
    TUint CurrentPluginRun();
            
private:
    
    CGOomActionList(CMemoryMonitor& aMonitor, CMemoryMonitorServer& aServer, RWsSession& aWs);
    
    void ConstructL(CGOomConfig& aConfig);
    
    TBool IsOkToKillApp(TInt aAppId);
    
    TBool AppCloseActionAlreadyExists(CGOomWindowGroupList& aWindowGroupList, TInt32 appId);
    
private: //data    
    
    RWsSession& iWs;
    
    RPointerArray<CGOomCloseApp> iCloseAppActions;
    RPointerArray<CGOomRunPlugin> iRunPluginActions;
    RArray<TActionRef> iActionRefs;
    
    TInt iCurrentActionIndex;
        
    TUint iCurrentTarget;

    /*
     * Flag specifying that a GOomMonitor event is in progress.
     */
    TBool iFreeingMemory;
    
    CMemoryMonitor& iMonitor;

    /**
     * The list of plugins
     * Own
     */
    CGOomPluginList<CGOomMonitorPlugin>* iPluginList;

    
    /**
     * The maximum priority of actions that should be run by an GOomMonitor Event
     */
    TInt iMaxPriority; 
    
    CMemoryMonitorServer& iServer;
    
    RArray<TUint> iAppsProtectedByPlugins;
    
    TBool iRunningKillAppActions;
    
    TUint iCurrentPluginRun;
//    RArray<TInt> iLowOnMemWgs;
    TBool iUseSwRendering;
    TBool iTryOptional; // we did everything we could but still missing some bits, try again with different app targets
    TBool iOptionalTried; // no need to overperform though
    TUint iAppIndex;
    };

#include "goomactionlist.inl"

#endif /*GOOMACTIONLIST_H_*/
