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
#include <ss_std.h>
#include <rmemchunk.h>

#include "test08multiplethreads.h"


TVerdict CTest08MultipleThreads::doTestStepL ()
	{
	__UHEAP_MARK;
	iMainTryCount = 0;
	iChildTryCount = 0;
	
	CleanupStack::PushL ( CreateChunkMgrL ( KMinHeapSize ) );	
	RMemoryAllocator allocator ( iChkMgr );
	
	INFO_PRINTF1 ( _L ( "Create high priority thread and resume it" ) );
	
	RThread childThread;
	TInt err = childThread.Create ( _L ( "testChildThread" ),
		                       		ChildThreadStep,
 							   		KDefaultStackSize,
							   		KDefaultHeapSize,
							   		KMaxHeapSize,
							   		(TAny*)this,
							   		EOwnerProcess );
	if ( err != KErrNone )
		{
		INFO_PRINTF2 ( _L ( "\nError:%d Could not create child thread" ), err );		 
		User::Leave ( EFail );
		}
	childThread.SetPriority ( EPriorityAbsoluteHigh );
	childThread.Resume ();
	
	INFO_PRINTF1 ( _L ( "\nDo alloc/free in loop that ends when the child task completes" ) );	
	TInt ret;
	volatile TExitType exitType;
	do {
		RMemChunk chunk;
		ret = chunk.Alloc ( 500, allocator );
		if ( ret != KErrNone )
			{
			INFO_PRINTF1 ( _L ( "\nError: Couldn't allocate RMemChunk" ) );
			User::Leave ( EFail );
			}
		iMainTryCount++;
		chunk.Free ();
		// Check if the child task is completed
		exitType = childThread.ExitType ();
		}
	while ( exitType == EExitPending );
	
	CleanupStack::PopAndDestroy ( iChkMgr );
	childThread.Close ();
	if ( iChildTryCount < 1000 )
		{
		INFO_PRINTF1 ( _L ( "\nError: Child thread ended before 1000 loops completetion" ) );
		return EFail;
		}

	INFO_PRINTF2 ( _L ( "\nChild thread Alloc/Dealloc's %d" ), iChildTryCount );	
	INFO_PRINTF2 ( _L ( "\nMain Thread Alloc/Dealloc's %d" ), iMainTryCount );
	
	__UHEAP_MARKEND;
	
	return EPass;
	}

TInt CTest08MultipleThreads::ChildThreadStep ( TAny* aParams )
	{
	CTest08MultipleThreads* testObj = ( CTest08MultipleThreads* ) aParams;
	RMemoryAllocator allocator ( testObj->iChkMgr );
	
	CTrapCleanup* trapHandler = CTrapCleanup::New ();
    CActiveScheduler* scheduler = new CActiveScheduler;
	if ( scheduler == NULL )
		{
		return KErrNoMemory;
		}
	CActiveScheduler::Install ( scheduler );
	
	RTimer timer;
	TRequestStatus status;
	timer.CreateLocal ();
	TInt ret = KErrNone;
	for ( TInt i = 0 ; i < 1000; i++ )
		{
		RMemChunk chunk;
		ret = chunk.Alloc ( 500, allocator );
		if ( ret != KErrNone )
			break;
		
		testObj->iChildTryCount++;
		
		chunk.Free ();
	 		 	
	 	timer.After ( status, 500 ); // sleep for 5ms
		User::WaitForRequest ( status );
		}
	
	CActiveScheduler::Install ( NULL );
	delete scheduler;
	delete trapHandler;
	
	return ret;
	}
