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
* Description:  UpdateQueue controls the feeds to be updated.
*
*/


#include "UpdateManager.h"
#include "UpdateQueue.h"
#include "FeedsServer.h"

// -----------------------------------------------------------------------------
// CUpdateQueue::NewL
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
CUpdateQueue* CUpdateQueue::NewL(CUpdateAllFeedsTask* aUpdateAllFeedTask,TInt aFreq)
    {
    CUpdateQueue* self = new (ELeave) CUpdateQueue( aUpdateAllFeedTask, aFreq);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CUpdateQueue::ConstructL
//
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------	
void CUpdateQueue::ConstructL()
    {
    iLastAutoUpdate.HomeTime();
    TDateTime dateTime = iLastAutoUpdate.DateTime();
    TInt mins;
    switch(iFreq)
        {
        case 10080:
            mins = iLastAutoUpdate.DayNoInWeek()*60*24 + dateTime.Hour() * 60 + dateTime.Minute();
            break;
        default:
            mins = dateTime.Hour() * 60 + dateTime.Minute();
            break;
        }
    iLastAutoUpdate = iLastAutoUpdate - TTimeIntervalMinutes(mins % iFreq);
    }

// -----------------------------------------------------------------------------
// CUpdateQueue::CUpdateQueue
//
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
 CUpdateQueue::CUpdateQueue( CUpdateAllFeedsTask* aUpdateAllFeedTask, TInt aFreq ):
            iFreq( aFreq), iCurrentState( aFreq), iUpdateAllFeedsTask( aUpdateAllFeedTask)
    {
    }

// -----------------------------------------------------------------------------
// CUpdateQueue::~CUpdateQueue
//
// Deconstructor.
// ----------------------------------------------------------------------------- 
CUpdateQueue::~CUpdateQueue()
    {
   	iFeedIds.Reset();
    }

// -----------------------------------------------------------------------------
// CUpdateQueue::AddFeed
// 
// Add feed to the Update feed list
// -----------------------------------------------------------------------------     
TInt CUpdateQueue::AddFeed( TInt aFeedId)
    {
    return iFeedIds.Append( aFeedId);
    }

// -----------------------------------------------------------------------------
// CUpdateQueue::RemoveFeed
// 
// Remove feed from list
// ----------------------------------------------------------------------------- 
TInt CUpdateQueue::RemoveFeed( TInt aFeedId)
    {
	TInt found = -1;
	if((found = iFeedIds.Find( aFeedId)) != KErrNotFound)
        {
        iFeedIds.Remove(found);
        }
	return found;
    }

// -----------------------------------------------------------------------------
// CUpdateQueue::UpdateL
// 
// Decrement the counter
// ----------------------------------------------------------------------------- 
TInt CUpdateQueue::UpdateL(TInt /*aMinutes*/)
    {

    TTime currentTime;
    currentTime.HomeTime();
    TTimeIntervalMinutes diff;
    currentTime.MinutesFrom(iLastAutoUpdate,diff);

    if(diff.Int() + 1 >= iFreq)
        {
        iUpdateAllFeedsTask->AddFeedL(iFeedIds);
        iLastAutoUpdate.HomeTime();
        TDateTime dateTime = iLastAutoUpdate.DateTime();
        TInt mins;
        switch(iFreq)
            {
            case 10080:
                mins = iLastAutoUpdate.DayNoInWeek()*60*24 + dateTime.Hour() * 60 + dateTime.Minute();
                break;
            default:
                mins = dateTime.Hour() * 60 + dateTime.Minute();
                break;
            }
        iLastAutoUpdate = iLastAutoUpdate - TTimeIntervalMinutes(mins % iFreq);
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CUpdateQueue::GetFreq
// 
// return the frequency
// ----------------------------------------------------------------------------- 
TInt CUpdateQueue::GetFreq()
    {
    return iFreq;
    }

// -----------------------------------------------------------------------------
// CUpdateQueue::Count
// 
// return the count
// -----------------------------------------------------------------------------     
TInt CUpdateQueue::Count()
    {
    return iFeedIds.Count();
    }

// -----------------------------------------------------------------------------
// CUpdateQueue::ResetTimers
// 
// Resets all the auto update timers
// -----------------------------------------------------------------------------

void CUpdateQueue::ResetTimers()
{
    iLastAutoUpdate.HomeTime();
    TDateTime dateTime = iLastAutoUpdate.DateTime();
    TInt mins;

    switch(iFreq)
        {
        case 10080:
            mins = iLastAutoUpdate.DayNoInWeek()*60*24 + dateTime.Hour() * 60 + dateTime.Minute();
            break;
        default:
            mins = dateTime.Hour() * 60 + dateTime.Minute();
            break;
        }

    iLastAutoUpdate = iLastAutoUpdate - TTimeIntervalMinutes(mins % iFreq);
}
