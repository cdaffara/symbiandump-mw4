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
* Description:  A wrapper for the window group list, adding additional functionality required by GOOM Monitor
*
*/


#include <w32std.h>
#include <e32std.h>
#include <apgtask.h>
#include <apgwgnam.h>

//#include "eglext.h"
#include <EGL/egl.h>

#include "goomwindowgrouplist.h"
#include "goomtraces.h"
#include "goomconstants.hrh"

_LIT(KDummyWgName, "20");
const TInt KPreallocatedSpaceForAppList = 50;

const TInt KAllowedMemUsageForApps = 50000;     //Allow upto 50K usage by app when killing apps

const TUint KGOomTicksPerSecond = 1000;

typedef EGLBoolean (*NOK_resource_profiling)(EGLDisplay, EGLint, EGLint*, EGLint, EGLint*);

CGOomWindowGroupList::TGOomWindowGroupProperties::TGOomWindowGroupProperties() :  iIdleTickTime(0), iDynamicPriority(EGOomPriorityNormal)
    {
    FUNC_LOG;
    }

void CGOomWindowGroupList::GetListOfWindowGroupsWSurfaces(RArray<TInt>& aLowOnMemWgs)
    {
    if (!iAlfClient.Handle())
       {
       if(iAlfClient.Connect() != KErrNone)
           return;
       }
    
    iAlfClient.GetListOfWindowGroupsWSurfaces(&aLowOnMemWgs);
    }
   

// Update the list of window groups
void CGOomWindowGroupList::Refresh(TBool aOptionalOnly)
    {
    FUNC_LOG;
    
#ifdef _DEBUG
    TRAPD(err, RefreshL(aOptionalOnly));
    if (err)
        {
        TRACES1("CGOomWindowGroupList::Refresh(): RefreshL leave %d", err);
        }
#else
    TRAP_IGNORE(RefreshL(aOptionalOnly));
    // Ignore any error
    // Errors are very unlikely, the only possibility is OOM errors (which should be very unlikely due to pre-created, re-reserved lists)
    // The outcome of any error is that the most foreground operations will be missing from the list
    // meaning that they will not be considered candidates for closing
#endif    
    }

// Update the list of window groups
// Should be called whenever the 
void CGOomWindowGroupList::RefreshL(TBool aOptionalOnly)
    {
    FUNC_LOG;
  
    if (!iAlfClient.Handle())
        {
        User::LeaveIfError(iAlfClient.Connect());
        }
    //iLowOnMemWgs.Reset();
    RArray<TInt> inactiveSurfaces;

    // ignore possible errors, we have information from profiling extension anyway
    if (!aOptionalOnly)
        {
        iAlfClient.GetListOfInactiveWindowGroupsWSurfaces(&inactiveSurfaces);    
        //iAlfClient.GetListOfWindowGroupsWSurfaces(&iLowOnMemWgs);
        }
    else
        { // first iteration: try to cope with window group ID's only
          // Most likely that needs to be revisited because apps fail to name their window 
          // groups properly on external screens...  
        iAlfClient.GetOptionalGraphicsMemUsers(&inactiveSurfaces);    
        iOptionalUids.Reset();
        TInt count = inactiveSurfaces.Count(); 
        for (TInt i = 0; i<count-1; i++)
            {
            iOptionalUids.Append(inactiveSurfaces[i+1]);
            inactiveSurfaces.Remove(i+1);
            count--;
            }   
        }
        
    TRACES1("Optional list composed: %d", aOptionalOnly);     
    TRACES1("Inactive surfaces count %d", inactiveSurfaces.Count());     
    TRACES1("Windowgroups w/ surfaces count %d", iLowOnMemWgs.Count());     

    RArray<TUint64> processIds;
    RArray<TUint> privMemUsed;
    RArray<TUint64> sparedProcessIds;
    
   if (!aOptionalOnly)
      {
      NOK_resource_profiling eglQueryProfilingData = (NOK_resource_profiling)eglGetProcAddress("eglQueryProfilingDataNOK");
    
      if (!eglQueryProfilingData && inactiveSurfaces.Count() == 0)
        {
        TRACES("RefreshL EGL_NOK_resource_profiling not available");
        return;
        }
    
     if (eglQueryProfilingData)
        {
        EGLint data_count;
        EGLint* prof_data;
        TInt i(0);
        
        EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);

        /* Find out how much profiling data is available */
        eglQueryProfilingData(dpy, EGL_PROF_QUERY_GLOBAL_BIT_NOK|
                                EGL_PROF_QUERY_MEMORY_USAGE_BIT_NOK,
                                NULL, 0, &data_count);
    
        /* Allocate room for the profiling data */
        prof_data = (EGLint*)User::Alloc(data_count * sizeof(EGLint));
        if (prof_data == NULL)
		   {
		   return;
		   }

        /* Retrieve the profiling data */
        eglQueryProfilingData(dpy,   EGL_PROF_QUERY_GLOBAL_BIT_NOK|
                                 EGL_PROF_QUERY_MEMORY_USAGE_BIT_NOK,
                                 prof_data,
                                 data_count,
                                 &data_count);
    
        /* Iterate over the returned data */
        TUint64 process_id;  
        while (i < data_count)
            {
            TRACES2("RefreshL EGL_NOK_resource_profiling - index: %d data: %x", i, prof_data[i]);
            switch (prof_data[i++])
                {   
                case EGL_PROF_PROCESS_ID_NOK:
                    {
                    if (sizeof(EGLNativeProcessIdTypeNOK) == 8)
                        {
                        process_id = TUint64(prof_data[i]);
                        process_id += (TUint64(prof_data[i + 1]) << 32);
                        i+=2;
                        }
                    else
                        {
                        process_id = prof_data[i];
                        i++;
                        }
                    break;
                    }
                case EGL_PROF_PROCESS_USED_PRIVATE_MEMORY_NOK:
                    {
                    TUint mem = prof_data[i];
                    privMemUsed.Append(mem);

                    TRACES1("Memory Usage by app is %d", mem);
                    if(mem > KAllowedMemUsageForApps)
                        processIds.Append(process_id);
                    else
                        sparedProcessIds.Append(process_id);
                
                    i++;
                    break;
                    }
                case EGL_PROF_PROCESS_USED_SHARED_MEMORY_NOK:
                    {
                    TUint mem = prof_data[i];
                    TRACES1("Shared Memory Usage by app is %d", mem);
                    if(mem > KAllowedMemUsageForApps)
                        processIds.Append(process_id);
                    i++;
                    break;
                    }
                case EGL_PROF_USED_MEMORY_NOK:
                case EGL_PROF_TOTAL_MEMORY_NOK:
                default:
                    {
                    TRACES2("RefreshL index %d, data: %d\n", i, prof_data[i]);
                    i++;
                    break;
                    }
                }
            }
       
       /* Free allocated memory */
        User::Free(prof_data);
    
        TRACES1("RefreshL : %d Processes use gfx memory", processIds.Count());
    
        ///////////////////////////////////////////////////////////////////
        // Optimization, no need to construct list if ALF is the only one
        ///////////////////////////////////////////////////////////////////
    
        if( (processIds.Count() == 1) && (inactiveSurfaces.Count() == 1))
            {
            RProcess process;
            TInt err =  process.Open(processIds[0]);
            if(!err)
                {
                TInt secureId = process.SecureId();
                process.Close();
                if(secureId == 0x10003B20) // magic, wserv 
                    {
                    processIds.Close();
                    privMemUsed.Close();
                    TRACES("Only WServ using GFX mem, no need for app actions");
                    iWgIds.Reset();
                    return;
                    }
                }
            }
        }
      }            
    // Refresh window group list
    // get all window groups, with info about parents
    TInt numGroups = iWs.NumWindowGroups();
    iWgIds.Reset();
    iWgIds.ReserveL(numGroups);
    User::LeaveIfError(iWs.WindowGroupList(&iWgIds));
    
    // Remove all child window groups, promote parents to foremost child position
    CollapseWindowGroupTree(inactiveSurfaces);

    // Note the current foreground window ID (if there is one)
    TBool oldForegroundWindowExists = EFalse;

    TInt oldForegroundWindowId;
    if (iWgIds.Count() > 0)
        {
        oldForegroundWindowId = iWgIds[0].iId;
        oldForegroundWindowExists = ETrue;
        }    
    
    // Cleanup the idletime hash map to remove idle times for any windows that have closed
    RemovePropertiesForClosedWindowsL();
    
    // Update the idle tick on the old foreground application (which might now be in the background)
    // This will be set to the current system tick count and will be used later to determine the idle time
    if (oldForegroundWindowExists)
        {
        TGOomWindowGroupProperties* wgProperties = iWgToPropertiesMapping.Find(oldForegroundWindowId);
        if (wgProperties)
            {
            wgProperties->iIdleTickTime = User::NTickCount();
            }
        
        // If there is no idle tick entry for this window ID then it will be created in the next step...
        }
    
    TInt index = iWgIds.Count();
    
    while (index--)
        {
        //Remove if process is not in list of processes using gfx mem
        TUint secureId = AppId(index,ETrue);
        TBool found = 0;
        TInt i = 0;
        //TRACES1("Checking WG ID : %d", iWgIds[index].iId); 
        //todo - do we really need to check this list , when we have all ids in inactiveSurfaces[]
        for(i = 0; i < processIds.Count(); i++)
            {
            RProcess process;
            TInt er =  process.Open(processIds[i]);
            if(er != KErrNone)
                {
                TRACES2("RefreshL Error opening process handle %d, err %d", index, er);
                continue;
                }
            /*static _LIT_SECURITY_POLICY_S0(mySidPolicy, secureId);
            if(mySidPolicy().CheckPolicy(process))*/
            TInt psecid = process.SecureId();
            if(secureId == psecid)
                {
                found = 1;
                process.Close();
                TRACES3("RefreshL Process id %x, wgid %d, using gfx memory %d. Added to list", psecid, iWgIds[index].iId, privMemUsed[i]);
                break;
                }
            process.Close();
            }
        
        if(!found)
            {            
            for(TInt ii = 0; ii < inactiveSurfaces.Count(); ii++)
                {
                if (iWgIds[index].iId == inactiveSurfaces[ii] )
                    {
                    found = ETrue;
                    TRACES3("Found %d , AppId %x, isSystem: %d",inactiveSurfaces[ii], secureId, iWgName->IsSystem())            
                    }     
                }
            }
        
        if(!found)
            {
            iWgIds.Remove(index);
            continue;
            }
      
        //check if it is system app
        if(iWgName->IsSystem() /*|| iWgName->Hidden()*/)
            {
            TRACES3("System/Hidden app found %x, ISystem %d, IsHidden %d",secureId, iWgName->IsSystem()?1:0, iWgName->Hidden()?1:0);  
            sparedProcessIds.Append(secureId);
            }
      
        }
    
    inactiveSurfaces.Close();
    //CleanupStack::PopAndDestroy(); //   CleanupClosePushL(inactiveSurfaces);
    processIds.Close();
    privMemUsed.Close();      
    
    //check if any system apps are included
    index = iWgIds.Count();
    while (index--)
        {
        TBool skipped = EFalse;
        for(TInt i = 0; i < sparedProcessIds.Count(); i++)
            {
            if(AppId(index,ETrue) == sparedProcessIds[i])
                {
                TRACES2("WgId %d belongs to system app %x. Removing from Kill List",iWgIds[index].iId, sparedProcessIds[i]);
                iWgIds.Remove(index);
                skipped = ETrue;
                break;
                }
            }
        
        if(skipped)
            continue;
        
        // See if there is a tick count entry for each window in the list
        TGOomWindowGroupProperties* wgProperties = iWgToPropertiesMapping.Find(iWgIds[index].iId);
        
        if (!wgProperties)
            {
            TGOomWindowGroupProperties wgProperties;
            wgProperties.iIdleTickTime = User::NTickCount();
            // If there is no idle tick entry for this window then add one
            iWgToPropertiesMapping.InsertL(iWgIds[index].iId, wgProperties);
            }
        }
    TRACES1("Number of applications using graphics mem: %d", iWgIds.Count());    
    sparedProcessIds.Close();
    }



void CGOomWindowGroupList::RemovePropertiesForClosedWindowsL()
    {
    FUNC_LOG;
    
    // First, clear the existing set of window IDs (it would be quicker to delete it BUT we have reserved memory for it and don't want to be allocating in low memory conditions)
    RHashSet<TInt>::TIter windowIdSetIter(iExistingWindowIds);
    while (windowIdSetIter.Next())
        {
        windowIdSetIter.RemoveCurrent();
        }
    
    // Create the set of existing window IDs (this saves expensive/repeated searching later on)
    TInt index = iWgIds.Count();
    while (index--)
        {
        iExistingWindowIds.InsertL(iWgIds[index].iId);
        }
    
    // Iterate the idle-time hash map - remove any items where the window no longer exists
    RHashMap<TInt, TGOomWindowGroupProperties>::TIter wgToIdleIterator(iWgToPropertiesMapping);
    while (wgToIdleIterator.NextKey())
        {
        // If the current key (window ID) does not exist in the set then remove the idle-time as it is no longer relevant
        if (!iExistingWindowIds.Find(*(wgToIdleIterator.CurrentKey())))
                wgToIdleIterator.RemoveCurrent();
        }
    }


TUint CGOomWindowGroupList::AppId(TInt aIndex, TBool aResolveFromThread) const
    {
    FUNC_LOG;

    if (aIndex < 0 || aIndex >= iWgIds.Count())
        {
        return 0; // somewhat equals to KNullUid
        }
		
    //TRACES2("CGOomWindowGroupList::AppId(%d), count: %d ",aIndex,iWgIds.Count());

    TInt wgId = iWgIds[aIndex].iId;
    
    return AppIdfromWgId(wgId, aResolveFromThread);
    }

TUint CGOomWindowGroupList::AppIdfromWgId(TInt aWgId, TBool aResolveFromThread) const
    {
    // get the app's details
    TPtr wgPtr(iWgNameBuf->Des());
    
    TUid uid;
    
    TInt wgId = aWgId;
    
    TInt err = iWs.GetWindowGroupNameFromIdentifier(wgId, wgPtr);
    
    if (KErrNone != err)
        // If there is an error then set the UID to 0;
        {
        uid.iUid = 0;
        }
    else 
        {
        iWgName->SetWindowGroupName(iWgNameBuf); // iWgName takes ownership of iWgNameBuf
        uid = iWgName->AppUid(); // This UID comes from the app, not the mmp!
        if (aResolveFromThread && uid.iUid == 0)
            {
            TApaTask task(iWs);
            task.SetWgId(wgId);
            TThreadId threadId = task.ThreadId();
            
            TUint resolvedUid = 0;
            RThread appThread;
            TInt err = appThread.Open( threadId );
            if ( err == KErrNone )
                {
                resolvedUid = appThread.SecureId().iId;
                }
            appThread.Close();
            //TRACES2("CGOomWindowGroupList::AppId: NULL wg UID, taking from thread; resolvedUid = %x aIndex = %d", resolvedUid, aIndex);            
            return resolvedUid;               
            }
        }
    
    return uid.iUid;
    }
    

TTimeIntervalSeconds CGOomWindowGroupList::IdleTime(TInt aIndex) const
    {
    FUNC_LOG;

    TUint32 currentTickCount = User::NTickCount();
        
    const TGOomWindowGroupProperties* wgProperties = 0;
    if (aIndex >= 0 && aIndex < iWgIds.Count())
        {
        wgProperties = iWgToPropertiesMapping.Find(iWgIds[aIndex].iId);
        }
    
    TTimeIntervalSeconds idleTime = 0;
    
    if (wgProperties)
        {
        // This should also handle the case where the current tick count has wrapped to a lower value than the idle tick time
        // It will only work if it has wrapped once, but
        TUint32 differenceBetweenTickCounts = currentTickCount - wgProperties->iIdleTickTime;
        idleTime = differenceBetweenTickCounts / KGOomTicksPerSecond;
        }
    
    return idleTime;
    }
  


void CGOomWindowGroupList::CollapseWindowGroupTree(RArray<TInt>& aWgsHavingSurfaces)
    {
    FUNC_LOG;

    // start from the front, wg count can reduce as loop runs
    for (TInt ii=0; ii<iWgIds.Count();)
        {
        RWsSession::TWindowGroupChainInfo& info = iWgIds[ii];
        if (info.iParentId > 0)        // wg has a parent
            {
            // Look for the parent position
            TInt parentPos = ii;        // use child pos as not-found signal
            TInt count = iWgIds.Count();
            
            for (TInt kk = aWgsHavingSurfaces.Count()-1; kk >=0; kk--)
                {
                if (aWgsHavingSurfaces[kk] ==info.iId && aWgsHavingSurfaces.Find(info.iParentId) == KErrNotFound )
                    { // replace child if with parent ID    
					aWgsHavingSurfaces.Append(info.iParentId);
					ii=-1; // need to start all over again
                    break;
                    }
                }
				
			if (ii == -1)
				{
				ii++;
				continue;	
				}
				
            for (TInt jj=0; jj<count; jj++)
                {
                if (iWgIds[jj].iId == info.iParentId)
                    {
                    parentPos = jj;
                    break;
                    }
                }

            if (parentPos > ii)  // parent should be moved forward
                {
                iWgIds[ii] = iWgIds[parentPos];
                iWgIds.Remove(parentPos);
                }
            else if (parentPos < ii)  // parent is already ahead of child, remove child
                iWgIds.Remove(ii);
            else                    // parent not found, skip
                ii++;
            }
        else    // wg does not have a parent, skip
            ii++;
        }    
    }



CGOomWindowGroupList::CGOomWindowGroupList(RWsSession& aWs) : iWs(aWs)
    {
    FUNC_LOG;
    }



void CGOomWindowGroupList::ConstructL()
    {
    FUNC_LOG;

    // Reserve enough space to build an app list later.
    iWgIds.ReserveL(KPreallocatedSpaceForAppList);
    iUncollapsedWgIds.ReserveL(KPreallocatedSpaceForAppList);
    
    // Reserve enough space for the WG to idle tick mapping
    iWgToPropertiesMapping.ReserveL(KPreallocatedSpaceForAppList);
    
    // Reserve enough space for CApaWindowGroupName.
    iWgName = CApaWindowGroupName::NewL(iWs);
    iWgNameBuf = HBufC::NewL(CApaWindowGroupName::EMaxLength);
    (*iWgNameBuf) = KDummyWgName;
    iWgName->SetWindowGroupName(iWgNameBuf);    // iWgName takes ownership of iWgNameBuf
    }



CGOomWindowGroupList* CGOomWindowGroupList::NewL(RWsSession& aWs)
    {
    FUNC_LOG;

    CGOomWindowGroupList* self = new (ELeave) CGOomWindowGroupList(aWs);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }



CGOomWindowGroupList::~CGOomWindowGroupList()
    {
    FUNC_LOG;

    iWgIds.Close();
    iUncollapsedWgIds.Close();
    iWgToPropertiesMapping.Close();
    iExistingWindowIds.Close();
    delete iWgName;
    iAlfClient.Close();
    iLowOnMemWgs.Close();
    }


void CGOomWindowGroupList::SetPriorityBusy(TInt aWgId)
    {
    FUNC_LOG;
    
    TInt parentId;
    TRAPD(err, parentId = FindParentIdL(aWgId));
    if (err)
        {
        parentId = aWgId;
        }
        
    TRACES2("CGOomWindowGroupList::SetPriorityBusy aWgId = %d, parentId = %d", aWgId, parentId);
        
    TGOomWindowGroupProperties* wgProperties = iWgToPropertiesMapping.Find(parentId);
    if (wgProperties)
        {
        wgProperties->iDynamicPriority = EGOomPriorityBusy;
        }
        
    // If we can't find the window group then ignore it
    }


TInt CGOomWindowGroupList::FindParentIdL(TInt aWgId)
    {
    TInt numGroups = iWs.NumWindowGroups(0);
    iUncollapsedWgIds.ReserveL(numGroups);
    User::LeaveIfError(iWs.WindowGroupList(0, &iUncollapsedWgIds));

    TInt parentPos = KErrNotFound;
   
    //loop through the window group list
    for (TInt i=0; i<numGroups; i++)
        {
        //find the index for the required aWgId
        if (iUncollapsedWgIds[i].iId == aWgId)
            {
            parentPos = i;
            break;
            }
        }

    if (parentPos >=0 )
        {
        while (iUncollapsedWgIds[parentPos].iParentId > 0)
            {
            // find the index for the parent
            for (TInt j=0; j<numGroups; j++)
                {
                if (iUncollapsedWgIds[j].iId == iUncollapsedWgIds[parentPos].iParentId)
                    {
                    parentPos = j;
                    break; // break out of inner loop
                    }
                }
            }
        return iUncollapsedWgIds[parentPos].iId;
        }
    else 
        {
        return KErrNotFound;
        }
    }


TBool CGOomWindowGroupList::IsBusy(TInt aWgIndex)
    {
    FUNC_LOG;
    if (aWgIndex < 0 || aWgIndex >= iWgIds.Count())
        {
        return EFalse;
        }
            
    TBool isBusy = EFalse;
    TGOomWindowGroupProperties* wgProperties = iWgToPropertiesMapping.Find(iWgIds[aWgIndex].iId);
    if (wgProperties)
        {
        isBusy = (wgProperties->iDynamicPriority == EGOomPriorityBusy);
        }
    
    return isBusy;
    }

// Returns ETrue if an application has registered itself as high priority at runtime
TBool CGOomWindowGroupList::IsDynamicHighPriority(TInt aWgIndex)
    {
    FUNC_LOG;
    if (aWgIndex < 0 || aWgIndex >= iWgIds.Count())
        {
        return EFalse;
        }

    TBool isHighPriority = EFalse;
    TGOomWindowGroupProperties* wgProperties = iWgToPropertiesMapping.Find(iWgIds[aWgIndex].iId);
    if (wgProperties)
        {
        isHighPriority = (wgProperties->iDynamicPriority == EGOomPriorityHigh);
        }
    
    return isHighPriority;
    }

CApaWindowGroupName* CGOomWindowGroupList::WgName() const
    {
    return iWgName;
    }

void CGOomWindowGroupList::SetPriorityNormal(TInt aWgId)
    {
    FUNC_LOG;

    TInt parentId;
    TRAPD(err, parentId = FindParentIdL(aWgId));
    if (err)
        {
        parentId = aWgId;
        }
    
    TGOomWindowGroupProperties* wgProperties = iWgToPropertiesMapping.Find(parentId);
    if (wgProperties)
        {
        wgProperties->iDynamicPriority = EGOomPriorityNormal;
        }
    
    // If we can't find the window group then ignore it
    }


void CGOomWindowGroupList::SetPriorityHigh(TInt aWgId)
    {
    FUNC_LOG;

    TInt parentId;
    TRAPD(err, parentId = FindParentIdL(aWgId));
    if (err)
        {
        parentId = aWgId;
        }
    
    TGOomWindowGroupProperties* wgProperties = iWgToPropertiesMapping.Find(parentId);
    if (wgProperties)
        {
        wgProperties->iDynamicPriority = EGOomPriorityHigh;
        }
    
    // If we can't find the window group then ignore it
    }

// Find the specificed application in the window group list and return the index
TInt CGOomWindowGroupList::GetIndexFromAppId(TUint aAppId) const
    {
    FUNC_LOG;

    TInt indexInGroupList = Count();
    TBool appFoundInWindowGroupList = EFalse;
    
    while (indexInGroupList--)
        {
        if (AppId(indexInGroupList, ETrue) == aAppId)
            {
            appFoundInWindowGroupList = ETrue;
            break;
            }
        }
    
    if (!appFoundInWindowGroupList)
        indexInGroupList = KAppNotInWindowGroupList;

    return indexInGroupList;
    }

// Find the specificed application in the window group list and return the index
TInt CGOomWindowGroupList::GetIndexFromWgId(TInt aWgId) const
    {
    FUNC_LOG;

    TInt indexInGroupList = Count();
    TBool appFoundInWindowGroupList = EFalse;
    
    while (indexInGroupList--)
        {
        if (iWgIds[indexInGroupList].iId == aWgId)
            {
            appFoundInWindowGroupList = ETrue;
            break;
            }
        }
    
    if (!appFoundInWindowGroupList)
        indexInGroupList = KAppNotInWindowGroupList;

    return indexInGroupList;
    }

void CGOomWindowGroupList::GetAllWgIdsMatchingAppId(TInt aWgId, RArray<TInt> & WgIdList) const
    {
    TInt32 appId = AppIdfromWgId(aWgId, ETrue);
    TInt indexInGroupList = Count();
    WgIdList.Reset();
    
    while (indexInGroupList--)
        {
        if (AppIdfromWgId(iWgIds[indexInGroupList].iId, ETrue) == appId)
            {
            WgIdList.Append(iWgIds[indexInGroupList].iId);
            }
        }
    }
