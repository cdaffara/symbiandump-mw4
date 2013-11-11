// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// @file
// @internalComponent
// 
//

#include <e32base.h>
#include <rmemchunk.h>

#include "test05allocdeallocperformance.h"

TVerdict CTest05AllocDeAllocPerformance::doTestStepL ()
	{
	__UHEAP_MARK;
	
	TInt numBlocks = 200, numLoops = 10;
	TUint8 KSmallBlockSize = 128;
	
	CleanupStack::PushL ( CreateChunkMgrL ( ( TUint ) ( 3 * KSmallBlockSize * numBlocks ) ) );
	RMemoryAllocator allocator ( iChkMgr );
	
	TTime startTime, stopTime;
	TInt64 timeSpent;
	for ( TInt i = 0; i < numLoops; i++ )
		{
		INFO_PRINTF2 ( _L ( "\n********** Loop #%d **********" ), i + 1 );
		INFO_PRINTF1 ( _L ( "\n Alloc memChunk and Free it" ) );
		INFO_PRINTF1 ( _L ( "\n...Repeat the same 1000 times and print time spent" ) );
		RMemChunk chunk;
		startTime.UniversalTime ();
		
		for ( TInt j = 0; j < 1000; j++ )
			{
			TInt ret = chunk.Alloc ( KSmallBlockSize * numBlocks, allocator );
			if ( ret != KErrNone )
				{
				INFO_PRINTF1 ( _L ( "\nError: Couldn't allocate" ) );
				User::Leave ( EFail );
				}
			
			chunk.Free ();
			}
		stopTime.UniversalTime ();
		
		timeSpent = stopTime.MicroSecondsFrom ( startTime ).Int64 () / 1000;
		INFO_PRINTF2 ( _L ( "...Info: The time spent on 1000 alloc and free is %d microSecs" ), timeSpent );
		}
	
    CleanupStack::PopAndDestroy ( iChkMgr );
	
	__UHEAP_MARKEND;
	
	return EPass;
	}
