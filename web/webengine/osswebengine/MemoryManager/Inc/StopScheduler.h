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
*/

#ifndef __STOPPER_H__
#define __STOPPER_H__

// INCLUDES

#include "MemoryManager.h"

// CONSTANTS

// DATA TYPES

// CLASS DECLARATION

// FORWARD DECLARATIONS
class CMemoryPool;

/**
*  An active object to schedule the stop of all on-going operations
*  @lib memman.lib
*  @since 3.1
*/
class CStopScheduler : public CActive
	{
	public:
        enum TNextStop
            {
            ENormalStop     = 0x01,
            EEmergencyStop  = 0x02,
            EAllStop        = 0x03
            };

        enum TSchedulerState
            {
            EIdle = 0,
            ECheckMemory,
            EStopLoading
            };

		CStopScheduler( CMemoryPool& aAllocator );	
		~CStopScheduler();
		
        /**
        * from CAcitve
        * @since 3.1
        * @param 
        * @return 
        */   
		void RunL();

        /**
        * start the stop scheduler
        * @since 3.1
        * @param aMemSize amount of memory causes the last allocation fail
        * @return 
        */   
		void Start( TSchedulerState aState, TInt aMemSize );

        /**
        * from CActive
        * @since 3.1
        * @param 
        * @return 
        */   
		void DoCancel();
		
	private:		
        void SelfComplete();
        void StopLoading( TOOMPriority aPriority );
        void CheckMemoryDefered( TTimeIntervalMicroSeconds32 aDelay );

		CMemoryPool&    iMemoryPool;
		TInt 		    iMemRequested;
        TSchedulerState iState;
        RTimer          iCheckTimer;
        TInt            iNextStop;
	};

#endif