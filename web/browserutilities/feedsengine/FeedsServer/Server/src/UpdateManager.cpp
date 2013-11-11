/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  UpdateManager it controls the updation of feed.
*
*/


#include "UpdateManager.h"
#include "UpdateQueue.h"
#include "FeedsServer.h"
#include "FeedsDatabase.h"
#include "ServerHttpConnection.h"

const TInt KWmlSettingsAutomaticUpdatingNotSet = 32767;
const TInt KMinFrequency = 15; // IN MINS

// -----------------------------------------------------------------------------
// CUpdateManager::NewL
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
CUpdateManager* CUpdateManager::NewL( TInt aFolderListId, TUint32 aAutoUpdateAP, TInt aAutoUpdateFreq, 
                	                            TBool aAutoUpdateWhileRoaming, CFeedsServer& aFeedsServer)                                        
    {
    CUpdateManager* self = new (ELeave) CUpdateManager( aFolderListId, aAutoUpdateFreq, aAutoUpdateWhileRoaming, aFeedsServer );

    CleanupStack::PushL(self);
    self->ConstructL( aAutoUpdateAP );
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CUpdateManager::ConstructL
//
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------	
void CUpdateManager::ConstructL( TUint32 aAutoUpdateAP)
    {
    CTimer::ConstructL();
    CActiveScheduler::Add(this);
    iUpdateAllFeedTask = CUpdateAllFeedsTask::NewL( iFeedsServer, iFolderListId , *this ); 
    iLazyCaller = CIdle::NewL(CActive::EPriorityIdle );
    iHttpConnection = CServerHttpConnection::NewL( aAutoUpdateAP );
    iRoamingInfo = CRoamingInfo::NewL(this);
    iAutoUpdateAp = aAutoUpdateAP;
    iLastAutoUpdate.HomeTime();
    }

 // -----------------------------------------------------------------------------
// CUpdateManager::CUpdateManager
//
// constructor.
// -----------------------------------------------------------------------------
 CUpdateManager::CUpdateManager( TInt aFolderListId, TInt aAutoUpdateFreq, TBool aAutoUpdateWhileRoaming, CFeedsServer& aFeedsServer ):
        CTimer(EPriorityIdle), iFeedsServer(aFeedsServer), iUpdateAllFeedTask(NULL)
    {
    iFolderListId = aFolderListId;
    iAutoUpdateFreq = aAutoUpdateFreq; 
    iAutoUpdateWhileRoaming = aAutoUpdateWhileRoaming;
    }

// -----------------------------------------------------------------------------
// CUpdateManager::~CUpdateManager
//
// Deconstructor.
// ----------------------------------------------------------------------------- 
CUpdateManager::~CUpdateManager()
    {
    Stop();
    for( int i=0; i<iQueueArray.Count(); i++ )
        {
        delete iQueueArray[ i ];
        }
    if( iUpdateAllFeedTask == NULL )
        {
        delete iUpdateAllFeedTask;
        iUpdateAllFeedTask = NULL;    
        }
    delete iLazyCaller;
    delete iHttpConnection;
    delete iRoamingInfo;
    iHttpConnection = NULL;
    iRoamingInfo = NULL;
   	Deque();
    }

// -----------------------------------------------------------------------------
// CUpdateManager::Stop
//
// Handles the stoping of AutoUpdate. 
// -----------------------------------------------------------------------------     
void CUpdateManager::Stop()
    {
    if(IsActive())
        {
        Cancel();
        }

    if(iLazyCaller->IsActive())
        {
        iLazyCaller->Cancel();
        }
    }

// -----------------------------------------------------------------------------
// CUpdateManager::RunL
//
// Handles an active object's timer event.
// -----------------------------------------------------------------------------  
void CUpdateManager::RunL()
    {
    TTime   currentTime;
    currentTime.HomeTime();
    TTimeIntervalMinutes diff;
    currentTime.MinutesFrom(iLastAutoUpdate,diff);

    if (diff.Int() < 0)
        {
        for(TInt i =0 ; i < iQueueArray.Count(); i++)
            {
            iQueueArray[i]->ResetTimers(); 
            }
        }
    iLastAutoUpdate.HomeTime();

    if (iStatus.Int() == KErrNone || iStatus.Int() == KErrAbort)
        {      
        StartTimer();
        if(iAutoUpdateWhileRoaming)
            {
            UpdateL();
            }
        else
            {
            iRoamingInfo->CheckForRoaming();
            }
        }      
    }

// -----------------------------------------------------------------------------
// CUpdateManager::StartL
// 
// Handles the Update of feed
// -----------------------------------------------------------------------------     
TInt CUpdateManager::StartL()
    {
    iCurrentFeedCount = 0;

    iFeedsServer.Database().AllFeedIdsL( iFeedIds, iFolderListId );
    iLazyCaller->Start(TCallBack(CUpdateManager::LazyCallBack,this));
    }

// -----------------------------------------------------------------------------
// CUpdateManager::UpdateAllFeedsProgress
// 
// Called to notify the obsever how many feeds remain to be updated
// -----------------------------------------------------------------------------      
void CUpdateManager::UpdateAllFeedsProgress(TInt /*aMaxCount*/, TInt /*aRemaining*/)
    {
     // Ignore.
    }

// -----------------------------------------------------------------------------
// CUpdateManager::UpdateAllFeedsCompleted
// 
// Called upon completion of the task.
// -----------------------------------------------------------------------------      
void CUpdateManager::UpdateAllFeedsCompleted(TInt aStatusCode)
    {
    // S60 bug fix <RFON-6QKRV7>: Disconnect after done with auto update
    iHttpConnection->Disconnect();
    if (aStatusCode == KErrNone)
        {
        iFeedsServer.NotifyFolderListChanged(iFolderListId);
        }    
    }

// -----------------------------------------------------------------------------
// CUpdateManager::HttpConnection
// 
// Returns the http-connection instance
// -----------------------------------------------------------------------------      
CHttpConnection& CUpdateManager::HttpConnection()
    {
    return (CHttpConnection&) *iHttpConnection;
    }

// -----------------------------------------------------------------------------
// CUpdateManager::FolderListId
//
// Returns the folder list ID.
// -----------------------------------------------------------------------------
//
TInt CUpdateManager::FolderListId() 
    {
    return iFolderListId;
    }

// -----------------------------------------------------------------------------
// CUpdateManager::Update
//
// Starts Update
// -----------------------------------------------------------------------------
//
 TInt CUpdateManager::UpdateL()
    {
    for(TInt i =0 ; i < iQueueArray.Count(); i++)
        {
        iQueueArray[i]->UpdateL(iMins);    
        }    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CUpdateManager::StartTimer
//
// Start timer.
// -----------------------------------------------------------------------------
//
void CUpdateManager::StartTimer()
    {
    TTime  time;
    time.HomeTime();
    TDateTime dateTime = time.DateTime();
    iMins = dateTime.Hour() * 60 + dateTime.Minute();
    TTimeIntervalMinutes interval(KMinFrequency - iMins % KMinFrequency);
    time += interval;

    At(time);
    }

// -----------------------------------------------------------------------------
// CUpdateManager::UpdateFeedL
//
// Update Feed in queue.
// -----------------------------------------------------------------------------
//    
void CUpdateManager::UpdateFeedL(TInt aFeedId, TBool aDeleteFeed)
    {
    for(TInt k=0 ; k < iQueueArray.Count(); k++)
        {
        iQueueArray[k]->RemoveFeed(aFeedId);
        if(iQueueArray[k]->Count() <= 0)
            {
            delete iQueueArray[k];
            iQueueArray.Remove(k);
            }
        }
    if(!aDeleteFeed)
        {
        if(iLazyCaller->IsActive())
            {
            iFeedIds.Append(aFeedId);
            }
        else
            {
            AddFeedL(aFeedId);
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpdateManager::LazyCallBack
//
// 
// -----------------------------------------------------------------------------
//
TBool CUpdateManager::LazyCallBack(TAny* aPtr)
    {
    CUpdateManager* ptr = (CUpdateManager*)aPtr;
    for(TInt i = 0; i < 10; i++)
        {
        if(ptr->iCurrentFeedCount< ptr->iFeedIds.Count())
            {
            TRAP_IGNORE(ptr->AddFeedL(ptr->iFeedIds[ptr->iCurrentFeedCount++]));
            }
        else
            {
            ptr->iCurrentFeedCount = 0;
            ptr->iFeedIds.Reset();
            ptr->StartTimer();
            return EFalse;
            }    
        }
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CUpdateManager::AddFeedL
//
// Add Feed IDs in queue.
// -----------------------------------------------------------------------------
//  
void CUpdateManager::AddFeedL(TInt aFeedId)
    {
    TInt freq;
    iFeedsServer.Database().FreqFromFeedIdL(aFeedId,freq);
    freq = iAutoUpdateFreq == KWmlSettingsAutomaticUpdatingNotSet ? freq : iAutoUpdateFreq;
    if(freq > 0)
        {
        TBool found = EFalse;
        for(TInt k=0 ; k < iQueueArray.Count(); k++)
            {
            if(freq == iQueueArray[k]->GetFreq())
                {
                iQueueArray[k]->AddFeed(aFeedId);
                found = ETrue;
                }
            }
        if(!found)
            {
            CUpdateQueue* updateQueue = CUpdateQueue::NewL( iUpdateAllFeedTask, freq); 
            iQueueArray.Append( updateQueue);
            updateQueue->AddFeed( aFeedId);
            }
        }
    }

// -----------------------------------------------------------------------------
// CRoamingInfo::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CRoamingInfo* CRoamingInfo::NewL(CUpdateManager *aBackgroundUpdater)                                        
    {
    CRoamingInfo* self = new (ELeave) CRoamingInfo(aBackgroundUpdater);

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CRoamingInfo::ConstructL
//
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CRoamingInfo::ConstructL()
    {
    CActiveScheduler::Add(this);
    iTelephony = CTelephony::NewL();
    }

// -----------------------------------------------------------------------------
// CRoamingInfo::CRoamingInfo
//
// C++ default constructor can NOT contain any code that
// might leave.
// -----------------------------------------------------------------------------
//
CRoamingInfo::CRoamingInfo(CUpdateManager *aUpdateManager)
	:CActive(CActive::EPriorityStandard),iUpdateManager(aUpdateManager)
    {
    }

// -----------------------------------------------------------------------------
// CRoamingInfo::~CRoamingInfo
//
// Destructor
// -----------------------------------------------------------------------------
//
CRoamingInfo::~CRoamingInfo()
    {
    Cancel();
    delete iTelephony;
    iTelephony = NULL;
    }

// -----------------------------------------------------------------------------
// CRoamingInfo::CheckForRoaming
// 
// Check for roaming.
// -----------------------------------------------------------------------------
void CRoamingInfo::CheckForRoaming()
    {

#ifndef __WINSCW__

    CTelephony::TNetworkRegistrationV1Pckg RegStatusPkg(iRegStatus);
    iTelephony->GetNetworkRegistrationStatus(iStatus, RegStatusPkg);
    SetActive();

#else
    TRAP_IGNORE( iUpdateManager->UpdateL() );
#endif
    }

// -----------------------------------------------------------------------------
// CRoamingInfo::RunL
//
// Handles an active object's request completion event.
// -----------------------------------------------------------------------------
//
void CRoamingInfo::RunL()
    {  
    if (iStatus.Int() == KErrNone)
        {
        if(iRegStatus.iRegStatus != CTelephony::ERegisteredRoaming)
        	{
        	iUpdateManager->UpdateL();
        	}
		}
    }

// -----------------------------------------------------------------------------
// CRoamingInfo::DoCancel
//
// Implements cancellation of an outstanding request.
// -----------------------------------------------------------------------------
//
void CRoamingInfo::DoCancel()
    {
    iTelephony->CancelAsync(CTelephony::EGetCurrentNetworkInfoCancel );
    }
