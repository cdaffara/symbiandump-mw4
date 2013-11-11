/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*
*
*/
#include "StopScheduler.h"
#include "MemoryPool.h"
#include "fast_malloc.h"

// MEMBER FUNCTIONS
static const TUint KLessRAM = ( 2*1024*1024 );
static const TUint KMinimumRAM = (1024 * 1024);
static const TTimeIntervalMicroSeconds32 KMemoryCheckIntervalSlow = 1000000;        // 1 second
static const TTimeIntervalMicroSeconds32 KMemoryCheckIntervalFast = 500000;         // 0.5 second

//-----------------------------------------------------------------------------
// CStopScheduler::CStopScheduler
//-----------------------------------------------------------------------------
CStopScheduler::CStopScheduler(CMemoryPool& aMemoryPool)
  : CActive( CActive::EPriorityHigh ), iMemoryPool( aMemoryPool ), iState( EIdle ), iNextStop( EAllStop )
    {
    CActiveScheduler::Add( this );
    iCheckTimer.CreateLocal();
    }

//-----------------------------------------------------------------------------
// CStopScheduler::~CStopScheduler
//-----------------------------------------------------------------------------
CStopScheduler::~CStopScheduler()
    {
    iCheckTimer.Close();
    Cancel();
    }

//-----------------------------------------------------------------------------
// CStopScheduler::Start
//-----------------------------------------------------------------------------
void CStopScheduler::Start( TSchedulerState aState, TInt aMemSize )
    {
    // stopping has a higher priority than collecting
    if( aState <= iState ) return;

    Cancel();

    iMemRequested = aMemSize;
    iState = aState;
    SelfComplete();

    iNextStop = EAllStop;
    }

void CStopScheduler::SelfComplete()
    {
    TRequestStatus* status = &iStatus;
    SetActive();
    User::RequestComplete( status, iState );    
    }

//-----------------------------------------------------------------------------
// CStopScheduler::RunL
//-----------------------------------------------------------------------------
void CStopScheduler::RunL()
    {
    if( iState == EStopLoading )
        {
        StopLoading( EOOM_PriorityLow );
        iNextStop &= ~ENormalStop;
        CheckMemoryDefered( KMemoryCheckIntervalFast );
        }
    else if( iState == ECheckMemory )
        {
        TFreeMem freeMem;
        TInt total = iMemoryPool.FreeMemory( freeMem );

        // see if free memory is enough to restore all collectors
        if( freeMem.iHal >= KLessRAM )
            {
            iMemoryPool.RestoreCollectors( EOOM_PriorityLow );
            iState = EIdle;
            iNextStop = EAllStop;

            // recover the rescue buffer, if it is already released.
            iMemoryPool.RestoreRescueBuffer();
            }
        else if( freeMem.iHal >= KMinimumRAM/2 )
            {
            iMemoryPool.RestoreCollectors( EOOM_PriorityMiddle );
            CheckMemoryDefered( KMemoryCheckIntervalSlow );
            iNextStop = EAllStop;
            }
        else if( freeMem.iHal >= KMinimumRAM/4 )
            {
            if( iNextStop & ENormalStop ) 
                {
                StopLoading( EOOM_PriorityLow );
                iNextStop &= ~ENormalStop;
                }
            CheckMemoryDefered( KMemoryCheckIntervalFast );
            }
        else
            {
            if( iNextStop & EEmergencyStop ) 
                {
                StopLoading( EOOM_PriorityHigh );
                iNextStop &= ~EEmergencyStop;
                }
            CheckMemoryDefered( KMemoryCheckIntervalFast );
            }
        }
    }

//-----------------------------------------------------------------------------
// CStopScheduler::DoCancel
//-----------------------------------------------------------------------------
void CStopScheduler::DoCancel()
    {
    iState = EIdle;
    }

//-----------------------------------------------------------------------------
// CStopScheduler::StopLoading
//-----------------------------------------------------------------------------
void CStopScheduler::StopLoading( TOOMPriority aPriority )
    {
    // stop operations
    for( TInt i=0; i<iMemoryPool.Stoppers().Count(); ++i )
        {
        MOOMStopper* stopper = iMemoryPool.Stoppers()[i];
        if( stopper->Priority() == aPriority )
            stopper->Stop();
        }
    iMemoryPool.SetStopping( EFalse );
    }

//-----------------------------------------------------------------------------
// CStopScheduler::CheckMemoryDefered
//-----------------------------------------------------------------------------
void CStopScheduler::CheckMemoryDefered( TTimeIntervalMicroSeconds32 aDelay )
    {
    iState = ECheckMemory;
    if( !IsActive() )
        {
        iCheckTimer.After( iStatus, aDelay );
        SetActive();
        }
    }
// END OF FILE
