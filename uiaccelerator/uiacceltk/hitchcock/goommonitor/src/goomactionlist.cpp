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
* Description:  Classes for executing OOM actions (e.g. closing applications and running plugins).
*
*/


#include <hal.h>
#include <apgwgnam.h>

#include "goommonitorplugin.h"
#include "goommonitorplugin.hrh"
#include "goomactionlist.h"
#include "goomwindowgrouplist.h"
#include "goompanic.h"
#include "goomtraces.h"
#include "goomconstants.hrh"
#include "goommemorymonitor.h"
#include "goommemorymonitorserver.h"
#include "goomrunplugin.h"
#include "goomcloseapp.h"
#include "goomconfig.h"
#include "goomactionref.h"
#include "goomapplicationconfig.h"
#include "goomcloseappconfig.h"
#include "goomrunpluginconfig.h"

template <class T>
CGOomPluginList<T>::CGOomPluginList()
    {
    FUNC_LOG;
    }

template <class T>
CGOomPluginList<T>::~CGOomPluginList()
    {
    FUNC_LOG;

    TInt count = iPlugins.Count();
    for (TInt ii=0; ii<count; ii++)
        {
        TPlugin& plugin = iPlugins[ii];
        if (plugin.iImpl)    // only if successfully added
            REComSession::DestroyedImplementation(plugin.iDtorUid);
        }
    iPlugins.Close();
    }

template <class T>
void CGOomPluginList<T>::ConstructL(TInt aInterfaceUid)
    {
    FUNC_LOG;

    RImplInfoPtrArray implArray;
    CleanupClosePushL(implArray);
    REComSession::ListImplementationsL(TUid::Uid(aInterfaceUid), implArray);

    TInt count = implArray.Count();
    iPlugins.ReserveL(count);

    for (TInt ii=0; ii<count; ii++)
        {
        iPlugins.AppendL(TPlugin());
        TPlugin& plugin = iPlugins[ii];
        TUid uid(implArray[ii]->ImplementationUid());
        plugin.iImpl = static_cast<T*>(REComSession::CreateImplementationL(uid, plugin.iDtorUid, NULL));
        plugin.iUid = uid.iUid;
        }

    CleanupStack::PopAndDestroy(&implArray);
    }

template <class T>
CGOomPluginList<T>::TPlugin::TPlugin()
: iImpl(0)
    {
    FUNC_LOG;
   }

template <class T>
CGOomPluginList<T>* CGOomPluginList<T>::NewL(TInt aInterfaceUid)
    {
    FUNC_LOG;

    CGOomPluginList* self = new (ELeave) CGOomPluginList();
    CleanupStack::PushL(self);
    self->ConstructL(aInterfaceUid);
    CleanupStack::Pop(self);
    return self;
    }

CGOomActionList* CGOomActionList::NewL(CMemoryMonitor& aMonitor, CMemoryMonitorServer& aServer, RWsSession& aWs, CGOomConfig& aConfig)
    {
    FUNC_LOG;

    CGOomActionList* self = new (ELeave) CGOomActionList(aMonitor, aServer, aWs);
    CleanupStack::PushL(self);
    self->ConstructL(aConfig);
    CleanupStack::Pop();
    return self;
    }

CGOomActionList::~CGOomActionList()
    {
    FUNC_LOG;

    iCloseAppActions.ResetAndDestroy();
    iCloseAppActions.Close();
    iRunPluginActions.ResetAndDestroy();
    iRunPluginActions.Close();

    iActionRefs.Close();

    delete iPluginList;
	
    }

// Creates a list of actions based on the contents of the config and the current window group list
// Any old actions will be replaced.
// This function may leave, however enough memory should be reserved for this process so that at least
// some actions have been created for freeing memory, these can then be run by calling FreeMemory.
// Note that this function will only leave in extreme circumstances, in normal usage we should have
// enough reserved memory to build the complete action list.
void CGOomActionList::BuildPluginActionListL(CGOomWindowGroupList& aWindowGroupList, CGOomConfig& aConfig)
    {
    FUNC_LOG;

    if (iFreeingMemory)
        {
        TRACES("OOMWATCHER:CGOomActionList::BuildActionListL Memory is currently being freed, do not build action list");
        TRACES2("OOMWATCHER:CGOomActionList::BuildActionListL iCurrentActionIndex = %d out of %d actions in progress", iCurrentActionIndex, iActionRefs.Count());
        return;
        }

    iActionRefs.Reset();
    iCurrentActionIndex = 0;

    TInt actionsIndex = 0;
    
    iAppsProtectedByPlugins.Reset();
    
    iRunningKillAppActions = EFalse;
    
    //
    // There is only one kind of plugin, the graphics plugin which is based on the oom plugin
	// references to v2 plugin types removed as these are not yet used by GOOM
    // we rely on the pluginlist not having changed since construction

    // Go through each plugin in the plugin list, create a run-plugin action for each one
    TInt pluginIndex = iPluginList->Count();
    while (pluginIndex--)
        {
        // Get the config for this plugin
        CGOomRunPluginConfig& pluginConfig = aConfig.GetPluginConfig(iPluginList->Uid(pluginIndex));

        //get skip plugin config for foreground app
        TUint foregroundUid = iMonitor.ForegroundAppUid();
        if(aConfig.GetApplicationConfig(foregroundUid).iSkipPluginId == iPluginList->Uid(pluginIndex))
            {
            TRACES2("Skiping plugin %x, configured for app %x", iPluginList->Uid(pluginIndex), foregroundUid);
            actionsIndex++;
            continue ; //skip this and continue with next plugin
            }
        
        TUint activeClientId = iMonitor.ActiveClientId();
        if(activeClientId!=0 && activeClientId!=foregroundUid)
            {
            if(aConfig.GetApplicationConfig(activeClientId).iSkipPluginId == iPluginList->Uid(pluginIndex))
                        {
                        TRACES2("Skiping plugin %x, configured for app %x", iPluginList->Uid(pluginIndex), foregroundUid);
                        actionsIndex++;
                        continue ; //skip this and continue with next plugin
                        }
            }

        actionsIndex--;
        CGOomRunPluginConfig * nextConfigForSamePlugin = &pluginConfig; 
        while(nextConfigForSamePlugin)
            {
            TInt priority = nextConfigForSamePlugin->CalculatePluginPriority(aWindowGroupList);
            TGOomSyncMode syncMode = nextConfigForSamePlugin->iSyncMode;
            TInt ramEstimate = nextConfigForSamePlugin->iRamEstimate;
            TActionRef::TActionType actionType;
            if (nextConfigForSamePlugin->PluginType() == EGOomAppPlugin)
                actionType = TActionRef::EAppPlugin;
            else
                actionType = TActionRef::ESystemPlugin;

            TActionRef ref = TActionRef(actionType, priority, syncMode, ramEstimate, *(iRunPluginActions[++actionsIndex]), aWindowGroupList.GetIndexFromAppId(nextConfigForSamePlugin->TargetApp()));
            iAppsProtectedByPlugins.Append(nextConfigForSamePlugin->TargetApp());
            TRACES2("Creating Plugin Action Item %x , TargetAppId %x", iPluginList->Uid(pluginIndex), nextConfigForSamePlugin->TargetApp());
            //It is valid to have plugins with equal priority
            User::LeaveIfError(iActionRefs.InsertInOrderAllowRepeats(ref, ComparePriorities));
            nextConfigForSamePlugin = nextConfigForSamePlugin->iNextConfig;
            }

        actionsIndex++;
        }
    
    TRACES1("BuildActionListL: Action list built with %d Plugin items",iActionRefs.Count());
    }

void CGOomActionList::BuildKillAppActionListL(CGOomWindowGroupList& aWindowGroupList, CGOomConfig& aConfig)
    {
    FUNC_LOG;
    
    // we can't reset action index here because plugins would miss memory good events

    iAppIndex = 0;
    
    aWindowGroupList.RefreshL(iTryOptional);
/*    
    for (TInt i = 0; aWindowGroupList.LowOnMemWgs(i) != KErrNotFound ; i++ )
        {
        if ( iLowOnMemWgs.Find(aWindowGroupList.LowOnMemWgs(i)) == KErrNotFound)
            {
            iLowOnMemWgs.Append(aWindowGroupList.LowOnMemWgs(i));    
            }
        }
*/        
    iRunningKillAppActions = ETrue;
    
    TInt oldcount = iActionRefs.Count();
    
    if (aWindowGroupList.Count())
            {
            // Go through each item in the wglist, create an app close action for this application
            TInt wgIndex = 0;
            
            TRACES1("BuildActionListL: Windowgroup list count %d ",aWindowGroupList.Count());
    
            // Fix for when fast swap has focus, or pen input server has put itself into the foreground:
            // the wg at index 1 should be considered as the foreground app.
            // stopIndex is calculated to take this into account.
            TUid foregroundUid = TUid::Uid(iMonitor.ForegroundAppUid());
    
            TRACES1("BuildActionListL: Foreground App %x ", foregroundUid);
            
            while (wgIndex < aWindowGroupList.Count())
                {
                CGOomCloseAppConfig* appCloseConfig = NULL;
    
                // Get the app ID for the wglist item
                // This sets the window group name
                TInt32 appId = aWindowGroupList.AppId(wgIndex, ETrue);
                
                if(AppCloseActionAlreadyExists(aWindowGroupList, appId))
                    {
                    TRACES2("CGOomActionList::BuildKillAppActionListL - Action item already exists for this app %x wgid %d", appId, aWindowGroupList.WgId(wgIndex).iId)
                    wgIndex++;
                    continue;
                    }
                    
                CApaWindowGroupName* wgName = aWindowGroupList.WgName();
                __ASSERT_DEBUG(wgName, GOomMonitorPanic(KInvalidWgName));

                    
                TBool skipped = EFalse;
                if ( !appId  || foregroundUid.iUid ==appId || (iAppsProtectedByPlugins.Find(appId) != KErrNotFound))
                    {
                    //If the UID is NULL at this point, we assume the process is not an application
                    //and therefore is not a suitable candidate for closure.
                    //We also do not close system or hidden apps.
                    TRACES3("BuildActionListL: Not adding process to action list; UID = %x, wgIndex = %d, wgid = %d", appId, wgIndex, aWindowGroupList.WgId(wgIndex).iId);
                    TRACES1("BuildActionListL: Foregroundapp %x", foregroundUid);
                    skipped = ETrue;
                    }
    
                else if (aWindowGroupList.IsBusy(wgIndex) || wgName->IsBusy())
                    // If the application has been flagged as busy then look up the configuration for busy apps in the config file
                    {
                    // Find the app close config for this app ID
                    appCloseConfig = aConfig.GetApplicationConfig(KGOomBusyAppId).GetAppCloseConfig();
                    }
                else if (aWindowGroupList.IsDynamicHighPriority(wgIndex))
                    // If the application has been flagged as busy then look up the configuration for busy apps in the config file
                    {
                    // Find the app close config for this app ID
                    appCloseConfig = aConfig.GetApplicationConfig(KGOomHighPriorityAppId).GetAppCloseConfig();
                    }
                else
                    // If the application hasn't been flagged as busy or high priority then look up the priority according to the config
                    {
                    // Find the app close config for this app ID
                    appCloseConfig = aConfig.GetApplicationConfig(appId).GetAppCloseConfig();
                    }
                
                if(!appCloseConfig && !skipped)
                    {
                    appCloseConfig = aConfig.GetApplicationConfig(KGOomDefaultAppId).GetAppCloseConfig();
                    }
    
                // Create the app close action and add it to the action list
                if (appCloseConfig)
                    {
                    TUint priority = appCloseConfig->CalculateCloseAppPriority(aWindowGroupList, wgIndex);
                    TInt wgId = aWindowGroupList.WgId(wgIndex).iId;
                    TGOomSyncMode syncMode = appCloseConfig->iSyncMode;
                    TInt ramEstimate = appCloseConfig->iRamEstimate;
                    TActionRef ref = TActionRef(TActionRef::EAppClose, priority, syncMode, ramEstimate, wgId, wgIndex, appCloseConfig->iCloseTimeout, appCloseConfig->iWaitAfterClose);
    
                    //AppClose Actions should always have a unique prioirity determined by the application's z order.
                    TInt err = iActionRefs.InsertInOrder(ref, ComparePriorities);
                    if ((err != KErrNone) && (err != KErrAlreadyExists))
                        {
                        TRACES3("BuildActionListL: Adding app to action list, Uid = %x, wgId = %d, err = %d", appId, wgId, err);
                        User::Leave(err);
                        }
                    TRACES3("BuildActionListL: Adding app to action list, Uid = %x, wgId = %d, wgIndex = %d", appId, wgId, wgIndex);
                    }
    
                wgIndex++;
                }
            }
            
        TRACES1("BuildActionListL: Action list built with %d  new items",iActionRefs.Count()- oldcount);
    }    


TBool CGOomActionList::AppCloseActionAlreadyExists(CGOomWindowGroupList& aWindowGroupList, TInt32 appId)
    {
    for(TInt i = 0 ; i < iActionRefs.Count() ; i++)
        {
        TActionRef ref = iActionRefs[i];
        if(ref.Type() == TActionRef::EAppClose )
            {
            if(aWindowGroupList.AppIdfromWgId(ref.WgId(), ETrue) == appId)
                return ETrue;
            }
        }
        return EFalse;
    }

// Execute the OOM actions according to their priority
// Run batches of OOM actions according to their sync mode
void CGOomActionList::FreeMemory(TInt aMaxPriority)
    {
    FUNC_LOG;

    if (iFreeingMemory)
            {
            TRACES("GOOMWATCHER:CGOomActionList::FreeMemory Memory is currently being freed, do not start any more actions");
            return;
            }
    
    iMaxPriority = aMaxPriority;

    TBool memoryFreeingActionRun = EFalse;

    // Get the configured maximum number of applications that can be closed at one time
    const CGOomGlobalConfig& globalConfig = CMemoryMonitor::GlobalConfig();
    TInt maxBatchSize = globalConfig.iMaxCloseAppBatch;
    TInt numberOfRunningActions = 0;

    TInt memoryEstimate = iMonitor.GetFreeMemory(); // The amount of free memory we expect to be free after the currently initiated operations

    TRACES2("GOOMWATCHER:CGOomActionList::FreeMemory Memory currentActionIndex %d iActionrefsCount %d", iCurrentActionIndex, iActionRefs.Count());
    
    while (iCurrentActionIndex < iActionRefs.Count())
        {
        if(iActionRefs[iCurrentActionIndex].Priority() > aMaxPriority)
            {
            TRACES1("Busy App wgid %d, spared by GOOM", iActionRefs[iCurrentActionIndex].WgId());
            iCurrentActionIndex++;
            continue;
            }
        
        TActionRef ref = iActionRefs[iCurrentActionIndex];
        CGOomAction* action = NULL;
        if (ref.Type() == TActionRef::EAppClose )
            {     
            iAppIndex%=iCloseAppActions.Count();
            TRACES2("Proceeding with app action from index %d, out of %d", iAppIndex, iCloseAppActions.Count() );
            action = iCloseAppActions[iAppIndex];
            iAppIndex++;
            static_cast<CGOomCloseApp*>(action)->Reconfigure(ref);
            
            ref.iAppPlugin = action;
            
            TInt32 appId = iMonitor.GetWindowGroupList()->AppIdfromWgId(ref.WgId(), ETrue);
            
            if(!IsOkToKillApp(appId))
                {
                iCurrentActionIndex++;
                if (iCurrentActionIndex >= iActionRefs.Count())
                    {
                    StateChanged();
                    return;
                    }
                else
                    {    
                    continue;
                    }
                }
            }
        else
            {
            action = &(ref.RunPlugin());
            iCurrentPluginRun = ref.RunPlugin().Id();
            }

        iFreeingMemory = ETrue;
        TRACES2("CGOomActionList::FreeMemory: Running action %d which has priority %d", iCurrentActionIndex,ref.Priority());
        iCurrentActionIndex++;
        action->FreeMemory(iCurrentTarget - memoryEstimate, iUseSwRendering);
        iCurrentPluginRun = 0;
        memoryFreeingActionRun = ETrue;

        // Actions with EContinueIgnoreMaxBatchSize don't add to the tally of running actions
        if (ref.SyncMode() != EContinueIgnoreMaxBatchSize)
            numberOfRunningActions++;

        // Update our estimate of how much RAM we think we'll have after this operation
        memoryEstimate += ref.RamEstimate();

        // Do we estimate that we are freeing enough memory (only applies if the sync mode is "esimtate" for this action)
        TBool estimatedEnoughMemoryFreed = EFalse;
        if ((ref.SyncMode() == EEstimate)
            && (memoryEstimate >= iCurrentTarget))
            {
            estimatedEnoughMemoryFreed = ETrue;
            }

        if ((ref.SyncMode() == ECheckRam)
                || (numberOfRunningActions >= maxBatchSize)
                || estimatedEnoughMemoryFreed
                || globalConfig.ForceCheckAtPriority(iActionRefs[iCurrentActionIndex-1].Priority()))
            // If this actions requires a RAM check then wait for it to complete
            // Also force a check if we've reached the maximum number of concurrent operations
            // Also check if we estimate that we have already freed enough memory (assuming that the sync mode is "estimate"
            {
            // Return from the loop - we will be called back (in CGOomActionList::StateChanged()) when the running actions complete
            TRACES("CGOomActionList::FreeMemory: Exiting run action loop");
            return;
            }
        // ... otherwise continue running actions, don't wait for any existing ones to complete
        
        if (iCurrentActionIndex >= iActionRefs.Count())
            {
            StateChanged();
            return;
            }
        }


    if (!memoryFreeingActionRun)
        {
        // No usable memory freeing action has been found, so we give up
        TInt freeMemory;

        if ( !FreeMemoryAboveTarget(freeMemory) && !iTryOptional && !iOptionalTried && freeMemory < 25*1024*1024 ) // magic, should read this from config
            { 
            iTryOptional = ETrue;
            iOptionalTried = ETrue;
            iMonitor.RunCloseAppActions(iMaxPriority);
            }
        else
            {
            iTryOptional = EFalse;
            iOptionalTried = EFalse;
            TRACES("CGOomActionList::FreeMemory: No usable memory freeing action has been found");
            iFreeingMemory = EFalse;
            iServer.CloseAppsFinished(freeMemory, EFalse);
            iMonitor.WaitAndSynchroniseMemoryState();
            }
        }
    }

TBool CGOomActionList::IsOkToKillApp(TInt aAppId)
    {
    
    //Double checking again if this app is now in foreground, if yes then we dont kill
    TUid fgAppuid = TUid::Uid(iMonitor.ForegroundAppUid());
    TInt32 fgApp = fgAppuid.iUid;
    TRACES1("Foreground Appuid %x", fgApp);
        
    if (aAppId == fgApp)
        {
        TRACES1("Foreground App wgid %x, spared by GOOM", aAppId);
        return EFalse;
        }

    //check if this is not parent of foreground app
    TBool spared = EFalse;
    TRACES2("CGOomActionList::FreeMemory - Going to kill Appid %x, Foreground app %x ", aAppId, fgApp);
    TInt prevWgId = 0;
    
    CApaWindowGroupName::FindByAppUid(fgAppuid, iWs, prevWgId);
    TInt i = 0;
    while ((prevWgId == KErrNotFound) && (i++ < 3))   //try 3 times before quiting. It takes time to get the wgid info when app is starting
            {
            TRACES1("Cannot find any more parent, trying again %d",i);
            User::After(200000);
            prevWgId = 0;
            CApaWindowGroupName::FindByAppUid(fgAppuid, iWs, prevWgId);
            }
   
    while (prevWgId != KErrNotFound)
        {
        TInt parentId = 0;
        TRAPD(err, parentId = iMonitor.GetWindowGroupList()->FindParentIdL(prevWgId));
        TRACES3("CGOomActionList::FreeMemory - Foreground App AppId %x, wgid %d, parent wgid %d", fgApp, prevWgId, parentId);
        if (err == KErrNone && parentId > 0)
            {
            TInt32 parentAppId = iMonitor.GetWindowGroupList()->AppIdfromWgId(parentId, ETrue);
            if (parentAppId == aAppId)
                {
                TRACES3("Parent App %x (wgId %d), of Foreground App %x, spared by GOOM", parentAppId, parentId, fgApp);
                spared = ETrue;
                break;
                }
            }
        CApaWindowGroupName::FindByAppUid(fgAppuid, iWs, prevWgId);
        }
    return !spared;
    }

// Should be called when the memory situation is good
// It results in notifications of the good memory state to all plugins with an outstanding FreeMemory request
void CGOomActionList::MemoryGood()
    {
    FUNC_LOG;
    if(!ALWAYS_SW_REND)
        {
    TInt actionRefIndex = iActionRefs.Count();

    // Go through each of the action references, if it's a plugin action then call MemoryGood on it
    // Note that this only results in a call to the plugin if FreeMemory has been called on this plugin since that last call to MemoryGood
    while (actionRefIndex--)
        {
        if ((iActionRefs[actionRefIndex].Type() == TActionRef::EAppPlugin)
                || (iActionRefs[actionRefIndex].Type() == TActionRef::ESystemPlugin))
            {
            iActionRefs[actionRefIndex].RunPlugin().MemoryGood();
            }
        }
    // notify window groups which were triggered to low mem that
    iMonitor.SwitchMemMode(CMemoryMonitor::EGOomGoodMemMode);
        }
/*    
    TWsEvent event;
    event.SetType(KGoomMemoryGoodEvent); // naive

    for (TInt i = iLowOnMemWgs.Count()-1; i>=0; i--)
        {
#ifdef SYMBIAN_GRAPHICS_WSERV_QT_EFFECTS
        iWs.SendEventToWindowGroup(iLowOnMemWgs[i], event);
#endif // #ifdef SYMBIAN_GRAPHICS_WSERV_QT_EFFECTS
        iLowOnMemWgs.Remove(i);
		}
*/		    
    }

TBool CGOomActionList::FreeMemoryAboveTarget(TInt& aFreeMemory)
    {
    FUNC_LOG;

    aFreeMemory = iMonitor.GetFreeMemory();

    TRACES2("CGOomActionList::FreeMemoryAboveTarget: Free RAM now %d, currentTarget %d",aFreeMemory, iCurrentTarget);

    return (aFreeMemory >= iCurrentTarget);
    }

TInt CGOomActionList::ComparePriorities(const TActionRef& aPos1, const TActionRef& aPos2 )
    {
    FUNC_LOG;

    if (aPos1.Priority()< aPos2.Priority())
        {
        return -1;
        }
    if (aPos1.Priority() > aPos2.Priority())
        {
        return 1;
        }

    // If priorities are equal then we use hardcoded rules to determine which one is run...

    // All other actions are run in preference to application closures
	if ((aPos1.Type() != TActionRef::EAppClose)
			&& ((aPos2.Type() == TActionRef::EAppClose)))
		{
		return -1;
		}
	if ((aPos1.Type() == TActionRef::EAppClose)
			&& ((aPos2.Type() != TActionRef::EAppClose)))
		{
		return 1;
		}
	// If both actions are application closures then the Z order is used to determine which one to run (furthest back application will be closed first)
	if ((aPos1.Type() == TActionRef::EAppClose)
			&& ((aPos2.Type() == TActionRef::EAppClose)))
		{
		if (aPos1.WgIndex() < aPos2.WgIndex())
			{
			return 1;
			}
		else
			{
			return -1;
			}
		//Two Apps should not have equal window group indexes, we panic below if this is the case.
		}

	// Application plugins will be run in preference to system plugins
	if ((aPos1.Type() == TActionRef::EAppPlugin)
			&& ((aPos2.Type() == TActionRef::ESystemPlugin)))
		{
		return -1;
		}
	if ((aPos1.Type() == TActionRef::ESystemPlugin)
			&& ((aPos2.Type() == TActionRef::EAppPlugin)))
		{
		return 1;
		}

	// If both actions are application plugins then the Z order of the target app is used to determine which one to run (the one with the furthest back target app will be closed first)
	// If the target app is not running then the plugin is run in preference to target apps where the target app is running
	if ((aPos1.Type() == TActionRef::EAppPlugin)
			&& ((aPos2.Type() == TActionRef::EAppPlugin)))
		{
		// When the target app is not running then the plugin will be run ahead of plugins where the target app is running
		if ((aPos1.WgIndex() == KAppNotInWindowGroupList) && (aPos2.WgIndex() != KAppNotInWindowGroupList))
			{
			return -1;
			}
		if ((aPos1.WgIndex() != KAppNotInWindowGroupList) && (aPos2.WgIndex() == KAppNotInWindowGroupList))
			{
			return 1;
			}
		// If the target apps for both plugins are running then compare the Z order
		if ((aPos1.WgIndex() != KAppNotInWindowGroupList) && (aPos2.WgIndex() != KAppNotInWindowGroupList))
			{
			if (aPos1.WgIndex() < aPos2.WgIndex())
				{
				return 1;
				}
			else
				{
				return -1;
				}
			}
		// If the target app for neither plugin is running then they are of equal priority
		}
    //App Close Actions must have a unique priority.
    __ASSERT_DEBUG((aPos1.Type() != TActionRef::EAppClose) && (aPos2.Type() != TActionRef::EAppClose), GOomMonitorPanic(KAppCloseActionEqualPriorities));
    return 0;
    }

void CGOomActionList::AppNotExiting(TInt aWgId)
    {
    FUNC_LOG;

    TInt index = 0;

    while (index < iCloseAppActions.Count())
        {
        CGOomCloseApp* action = iCloseAppActions[index];
        TRACES3("CGOomCloseApp::AppNotExiting: recvd from %d , checking against %d , isRunning %d", aWgId, action->WgId(), action->IsRunning());
        
        if ( (action->WgId() == aWgId) && (action->IsRunning()) )
            {
            TRACES1("CGOomCloseApp::AppNotExiting: App with window group id %d has responded to the close event", aWgId);
            action->CloseAppEvent();
            break;
            }
        
        index++;
        }
    }

// From MGOomActionObserver
// An action has changed state (possibly it has completed freeing memory)
void CGOomActionList::StateChanged()
    {
    FUNC_LOG;

    TBool allActionsComplete = ETrue;

    TInt index = iActionRefs.Count();
	while ((index--) && (allActionsComplete))
        {
        if (iActionRefs[index].IsRunning())
            {
            TRACES1("CGOomActionList::StateChanged. Action %d STILL RUNNING.", index);
            allActionsComplete = EFalse;
            }
        }

    TRACES1("CGOomActionList::StateChanged. Current Target = %d", iCurrentTarget);
    
    if (allActionsComplete)
        {
        iFreeingMemory = EFalse;
        // If all of the actions are complete then check memory and run more if necessary
        TInt freeMemory;
        TBool freeMemoryAboveTarget = FreeMemoryAboveTarget(freeMemory);
        TRACES1("CGOomActionList::StateChanged. Free Memory = %d", freeMemory);
        if (!freeMemoryAboveTarget)
            // If we are still below the good-memory-threshold then continue running actions
            {            
            TRACES2("CGOomActionList::StateChanged: Finished Action %d out of %d",iCurrentActionIndex, iActionRefs.Count());

            if (iCurrentActionIndex < iActionRefs.Count())
                {
                // There are still more actions to try, so we continue
                TRACES1("CGOomActionList::StateChanged: All current actions complete, running more actions. freeMemory=%d", freeMemory);
                return FreeMemory(iMaxPriority);
                } 
                                   
            if(iRunningKillAppActions)
	            {
                iRunningKillAppActions = EFalse;

                if (!iTryOptional && !iOptionalTried && freeMemory < 25*1024*1024 ) // magic, should read this from config
                    { 
                    iTryOptional = ETrue;
                    iOptionalTried = ETrue;
                    iMonitor.RunCloseAppActions(iMaxPriority);
                    }
                else
                    {
                    // There are no more actions to try, so we give up
                    TRACES1("CGOomActionList::StateChanged: All current actions complete, below good threshold with no more actions available. freeMemory=%d", freeMemory);
                    iTryOptional = EFalse;       
                    iServer.CloseAppsFinished(freeMemory, EFalse);
                    iMonitor.WaitAndSynchroniseMemoryState();
                    }
                }
            else
                {
                iMonitor.SwitchMemMode(CMemoryMonitor::EGOomLowMemMode);
                TRACES1("CGOomActionList::StateChanged: All current Plugin actions complete, below good threshold, Time to kill bad guys. freeMemory=%d", freeMemory);
                iRunningKillAppActions = ETrue;
                iMonitor.RunCloseAppActions(iMaxPriority);
                }
        
            }
        else
            {
            TRACES1("CGOomActionList::StateChanged: All current actions complete, memory good. freeMemory=%d", freeMemory);
            iRunningKillAppActions = EFalse;
            iServer.CloseAppsFinished(freeMemory, ETrue);
            iMonitor.WaitAndSynchroniseMemoryState();
            }
        }

    // If some actions are not yet in the idle state then we must continue to wait for them (they will notify us of a state change via a callback)
    }

CGOomActionList::CGOomActionList(CMemoryMonitor& aMonitor, CMemoryMonitorServer& aServer, RWsSession& aWs)
    : iWs(aWs), iMonitor(aMonitor), iServer(aServer)
    {
    FUNC_LOG;
    }

void CGOomActionList::ConstructL(CGOomConfig& aConfig)
    {
    FUNC_LOG;

    iCurrentActionIndex = 0;
    iFreeingMemory = EFalse;

    // Get the list of plugins available to the system

    iPluginList = CGOomPluginList<CGOomMonitorPlugin>::NewL(KGOomPluginInterfaceUidValue);
    // Go through each plugin in the plugin list, create a run-plugin action for each one
    TInt pluginIndex = iPluginList->Count();
    while (pluginIndex--)
        {
        // Get the config for this plugin
        CGOomRunPluginConfig& pluginConfig = aConfig.GetPluginConfig(iPluginList->Uid(pluginIndex));

        // Create an action acording to the config
        CGOomRunPlugin* action = CGOomRunPlugin::NewL(iPluginList->Uid(pluginIndex), pluginConfig, *this, iPluginList->Implementation(pluginIndex));

        iRunPluginActions.AppendL(action);
        
        CGOomRunPluginConfig * nextConfigForSamePlugin = pluginConfig.iNextConfig; 
        while(nextConfigForSamePlugin)
            {
            CGOomRunPlugin* action = CGOomRunPlugin::NewL(iPluginList->Uid(pluginIndex), *(nextConfigForSamePlugin), *this, iPluginList->Implementation(pluginIndex));
            iRunPluginActions.AppendL(action);
            nextConfigForSamePlugin = nextConfigForSamePlugin->iNextConfig; 
            }
        }

	//references to v2 plugin types removed as these are not yet used by GOOM
	
    //allocate empty CGOomCloseApp objects
    TInt appCloseIndex = aConfig.GlobalConfig().iMaxCloseAppBatch;
    while (appCloseIndex--)
        {
        CGOomCloseApp* action = CGOomCloseApp::NewL(*this, iWs);
        iCloseAppActions.AppendL(action);
        }
    }


void CGOomActionList::SetPriority(TInt aWgId, TInt aPriority)
    {
    FUNC_LOG;

    TInt idx = iActionRefs.Count()-1;
    while(idx >= 0)
        {
        if(iActionRefs[idx].WgId() == aWgId)
            {
            break;
            }
        idx--;
        }
    
    if(idx >= 0)
        {
        TRACES2("CGOomActionList::SetPriority Setting app wgid %d, index %d as busy", aWgId, idx);
        iActionRefs[idx].SetPriority(aPriority);
        }
    else
        {
        TRACES1("CGOomActionList::SetPriority wgd %d not in the hitlist", aWgId);
        }
    }


TUint CGOomActionList::CurrentPluginRun()
    {
    return iCurrentPluginRun;
    }

TBool CGOomActionList::IsRunningKillAppActions()
    {
    return iRunningKillAppActions;
    }

void CGOomActionList::SetUseSwRendering(TBool aUseSwRendering)
    {
    iUseSwRendering = aUseSwRendering;
    }

TBool CGOomActionList::UseSwRendering()
    {
    return iUseSwRendering;
    }
