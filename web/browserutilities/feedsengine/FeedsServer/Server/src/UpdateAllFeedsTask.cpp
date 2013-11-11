/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  A task to update all Feeds.
*
*/


#include "FolderAttributes.h"
#include "FeedsDatabase.h"
#include "FeedsServer.h"
#include "LeakTracker.h"
#include "Logger.h"
#include "PackedFeed.h"
#include "PackedFolder.h"
#include "UpdateAllFeedsTask.h"
#include <sysutil.h>


// -----------------------------------------------------------------------------
// CUpdateAllFeedsTask::NewL
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpdateAllFeedsTask* CUpdateAllFeedsTask::NewL(CFeedsServer& aFeedsServer,
        TInt aFolderListId, MUpdateAllFeedsTaskObserver& aObserver)
    {
    CUpdateAllFeedsTask* self = new (ELeave) CUpdateAllFeedsTask(aFeedsServer, 
            aFolderListId, aObserver, EFalse);
    
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }
    
        
// -----------------------------------------------------------------------------
// CUpdateAllFeedsTask::NewL
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpdateAllFeedsTask* CUpdateAllFeedsTask::NewL(CFeedsServer& aFeedsServer,
        TInt aFolderListId, const RArray<TInt>& aFeedIds, MUpdateAllFeedsTaskObserver& aObserver)
    {
    CUpdateAllFeedsTask* self = new (ELeave) CUpdateAllFeedsTask(aFeedsServer, 
            aFolderListId, aObserver, ETrue);
    
    CleanupStack::PushL(self);
    self->ConstructL();
    
    for (TInt i = 0; i < aFeedIds.Count(); i++)
        {
        // Add it to the list of entries to update (duplicate ids are ignored).
        (void) self->iFeedIds.InsertInOrder(aFeedIds[i]);
        }
        
    CleanupStack::Pop();

    return self;
    }
    
        
// -----------------------------------------------------------------------------
// CUpdateAllFeedsTask::CUpdateAllFeedsTask
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpdateAllFeedsTask::CUpdateAllFeedsTask(CFeedsServer& aFeedsServer,
        TInt aFolderListId, MUpdateAllFeedsTaskObserver& aObserver, TBool aSelected):
        CTask(aFeedsServer), iLeakTracker(CLeakTracker::EUpdateAllFeedsTask), 
        iObserver(aObserver), iFolderListId(aFolderListId), iSelected(aSelected)
    {
    }
        

// -----------------------------------------------------------------------------
// CUpdateAllFeedsTask::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpdateAllFeedsTask::ConstructL()
    {
    BaseConstructL(ETrue);
    iCheckDiskSpace = CCheckDiskSpace::NewL(this);
    iCheckDiskSpace->StartListening();
    }        


// -----------------------------------------------------------------------------
// CUpdateAllFeedsTask::~CUpdateAllFeedsTask
// Deconstructor.
// -----------------------------------------------------------------------------
//
CUpdateAllFeedsTask::~CUpdateAllFeedsTask()
    {
    iFeedIds.Close();
    delete iUpdateFeedTask;
    delete iCheckDiskSpace;
    iCheckDiskSpace = NULL;
    }


// -----------------------------------------------------------------------------
// CUpdateAllFeedsTask::StartTaskL.
//
// Starts the task.
// -----------------------------------------------------------------------------
//
void CUpdateAllFeedsTask::StartTaskL()
    {

    TInt drive( EDriveC );
    TBool isSpace( EFalse );
    RFs                 rfs;

    User::LeaveIfError(rfs.Connect());
    CleanupClosePushL(rfs);

    TRAP_IGNORE( isSpace = !SysUtil::DiskSpaceBelowCriticalLevelL(
                                                &rfs,
                                                KMinFreebytes,
                                                drive ));

    CleanupStack::PopAndDestroy();  //rfs
    // Abort the updation of feeds under low memory.
    if(!isSpace)
        {
        iObserver.UpdateAllFeedsCompleted(KErrNoMemory);
        return;
        }

    // Extract all of the ids if not selected update.
    if ( !iSelected )
        {
        if( iFeedIds.Count() != 0 )
            {
            FEED_LOG1(_L("Feeds"), _L("Feeds_Errors.log"), 
                EFileLoggingModeAppend, _L("CUpdateAllFeedsTask::StartTaskL iFeedIds count is %d when updating all"), 
                iFeedIds.Count());
            }
        iFeedsServer.Database().AllFeedIdsL( iFeedIds, iFolderListId );
        }
    // Otherwise just use the feed ids already in iFeedIds.
    
    // Start the first update.
    UpdateNextFeedL();
    }


// -----------------------------------------------------------------------------
// CUpdateAllFeedsTask::StartWait
//
// Notifies the observer that the UrlHandler is about to start a lengthy operation.
// In response the observer could display somekind of status indicator.
// -----------------------------------------------------------------------------
//
void CUpdateAllFeedsTask::StartWait()
    {
    //iObserver.StartWait();
    }


// -----------------------------------------------------------------------------
// CUpdateAllFeedsTask::Completed
//
// Called upon completion of the task.
// -----------------------------------------------------------------------------
//
void CUpdateAllFeedsTask::Completed(CPackedFeed* aPackedFeed, const TDesC& aFeedUrl,TInt aFeedId, 
        TInt aStatusCode)
    {
    TInt err;
    
    // Delete the task.
    iUpdateFeedTask->AutoDelete();
    iUpdateFeedTask = NULL;
    
    // A new packed feed is ready update the database.
    if (aPackedFeed != NULL)
        {
        // Update the database.
        TRAP_IGNORE(iFeedsServer.Database().UpdateFeedL(*aPackedFeed, aFeedUrl,aFeedId, iFolderListId));
        iFeedIds.Remove(iNextFeedIndex);
        delete aPackedFeed;
        }
    else
        {
        iFeedIds.Remove(iNextFeedIndex);
        }
    
    if(aStatusCode != KErrCancel && aStatusCode != KErrNoMemory)
    	{
        // Update the database.
        TRAP_IGNORE(iFeedsServer.Database().UpdateFeedStatusL(aFeedId,aStatusCode));
    	}
    // Stop updating if a fatal error ocurrs.
    // TODO: What other errors should stop this?
    if ((aStatusCode == KErrCancel) || (aStatusCode == KErrNoMemory))
        {
        iObserver.UpdateAllFeedsCompleted(aStatusCode);
        }
        
    // Otherwise start the next update.  Note: this is done even if the last 
    // update failed.
    else
        {        
        TRAP(err, UpdateNextFeedL());
        
        // Just stop updating if an error occurred.
        if (err != KErrNone)
            {
            iObserver.UpdateAllFeedsCompleted(err);
            }
        }
    }
    
    
// -----------------------------------------------------------------------------
// CUpdateAllFeedsTask::UpdateNextFeed
//
// Starts the update of the next feed.
// -----------------------------------------------------------------------------
//
void CUpdateAllFeedsTask::UpdateNextFeedL()
    {
    TBool  done = EFalse;
    
    // TODO: Allow several feeds to be updated at the same time.

    while (!done)
        {        
        // Update the next feed.
        if (iNextFeedIndex < iFeedIds.Count())
            {
            HBufC*  feedUrl = NULL;
            
            if(iFeedIds[iNextFeedIndex] > 0)
                {
                // Get feed's url from the datbase.
                (void) iFeedsServer.Database().UrlFromFeedIdL(
                        iFeedIds[iNextFeedIndex], feedUrl);
                CleanupStack::PushL(feedUrl);
                
                // Create a new task to update the feed.
                iUpdateFeedTask = CUpdateFeedTask::NewL(iFeedsServer, 
                        *feedUrl,iFeedIds[iNextFeedIndex], *this);        
                TRAPD(err, iUpdateFeedTask->StartTaskL());
                
                // Done if the task was started.
                if (err == KErrNone)
                    {
                    done = ETrue;
                    }
                    
                // Otherwise the task wasn't started so delete it.
                else
                    {
                    if(err != KErrCancel && err != KErrNoMemory)
    	                {
    		            iFeedsServer.Database().UpdateFeedStatusL(iFeedIds[iNextFeedIndex],err);
            	        }
                    delete iUpdateFeedTask;
                    iUpdateFeedTask = NULL;
                    }
                
                CleanupStack::PopAndDestroy(feedUrl);
                }
            }

        // Otherwise notify the observer that there are no more feeds to update.
        else
            {
#if defined(_DEBUG)
            iFeedsServer.Database().DebugPrintItemTableL();
#endif
            iObserver.UpdateAllFeedsCompleted(KErrNone);
            done = ETrue;
            }
        }
    }
// -----------------------------------------------------------------------------
// CUpdateAllFeedsTask::AddFeedL
//
// return integer
// -----------------------------------------------------------------------------
//
TInt CUpdateAllFeedsTask::AddFeedL( RArray<TInt> aFeedIds)
    {
    TBool taskRunning = EFalse;

    if(iFeedIds.Count())
        {
        taskRunning = ETrue;    
        }   

    for(TInt i=0 ; i< aFeedIds.Count(); i++)
        {
        if( iFeedIds.Find(aFeedIds[i]) < 0 )
            {
            iFeedIds.Append(aFeedIds[i]);
            }
        }
    if(!taskRunning)
        {
        iNextFeedIndex = 0;
        UpdateNextFeedL();
        }

    return KErrNone;
    }
// -----------------------------------------------------------------------------
// CUpdateAllFeedsTask::HttpConnection
//
// return connection
// -----------------------------------------------------------------------------
//
CHttpConnection& CUpdateAllFeedsTask::HttpConnection()
    {
    return iObserver.HttpConnection();
    }

// -----------------------------------------------------------------------------
// CUpdateAllFeedsTask::UpdateAllFeedsCompleted
//
// return connection
// -----------------------------------------------------------------------------
//
void CUpdateAllFeedsTask::UpdateAllFeedsCompleted(TInt aStatusCode)
    {
    iObserver.UpdateAllFeedsCompleted(aStatusCode);
    }

// -----------------------------------------------------------------------------
// CCheckDiskSpace::NewL
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
CCheckDiskSpace* CCheckDiskSpace::NewL(CUpdateAllFeedsTask* aUpdateAllFeedsTask)
    {
    CCheckDiskSpace* self = new (ELeave) CCheckDiskSpace();
    CleanupStack::PushL(self);
    self->ConstructL(aUpdateAllFeedsTask);
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CCheckDiskSpace::~CCheckDiskSpace
//
// Destructor
// -----------------------------------------------------------------------------
CCheckDiskSpace::~CCheckDiskSpace()
    {
    if(iIsListening)
        {
        Cancel();
        iIsListening = EFalse;
        }
    iRfs.Close();
    }

// -----------------------------------------------------------------------------
// CCheckDiskSpace::ConstructL
//
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
void CCheckDiskSpace::ConstructL(CUpdateAllFeedsTask* aUpdateAllFeedsTask)
    {
    CActiveScheduler::Add(this);
    iUpdateAllFeedsTask = aUpdateAllFeedsTask;  
    User::LeaveIfError(iRfs.Connect());
    }

// -----------------------------------------------------------------------------
// CCheckDiskSpace::CCheckDiskSpace
//
//C++ default constructor can NOT contain any code that
// might leave.
// -----------------------------------------------------------------------------
CCheckDiskSpace::CCheckDiskSpace():CActive(EPriorityStandard ), iIsListening(EFalse)
    {
    }

// -----------------------------------------------------------------------------
// CCheckDiskSpace::RunL
//
// To Receive event from Subscribe and publish server and
// based on the C: drive memory. 
// -----------------------------------------------------------------------------
void CCheckDiskSpace::RunL()
    {
    if(iStatus == KErrNone)
        {
        TInt drive( EDriveC );
        TBool isSpace( EFalse );

        TRAP_IGNORE( isSpace = !SysUtil::DiskSpaceBelowCriticalLevelL(
                                                    &iRfs,
                                                   KMinFreebytes,
                                                   drive ));
        if(!isSpace)
            {
            iUpdateAllFeedsTask->UpdateAllFeedsCompleted(KErrNoMemory);
            }
        else
            {
            StartListening();
            }    
        }
    else
        {
        iIsListening = EFalse;
        }
    }

// -----------------------------------------------------------------------------
// CCheckDiskSpace::RunError
// 
// -----------------------------------------------------------------------------
void CCheckDiskSpace::RunError()
    {
    }

// -----------------------------------------------------------------------------
// CCheckDiskSpace::DoCancel
// 
// -----------------------------------------------------------------------------
void CCheckDiskSpace::DoCancel()
    {
    iRfs.NotifyDiskSpaceCancel(iStatus);
    
    }

// -----------------------------------------------------------------------------
// CCheckDiskSpace::StartListening
//
// This function subscribe to receive events on C: drive memory activites.
// -----------------------------------------------------------------------------
void CCheckDiskSpace::StartListening()
    {

    if(!IsActive())
        {
        TInt drive( EDriveC );

        iRfs.NotifyDiskSpace( KMinFreebytes, drive, iStatus);
        iIsListening = ETrue;

        SetActive();
        }
    }
