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
#include <e32std.h>
#include <rmemchunk.h>

#include "test04defaultheapfreecheck.h"

TVerdict CTest14DefaultHeapFreeCheck::doTestStepL ()
	{
#if defined ( _DEBUG )
	__UHEAP_MARK;
	
	//-------------- step 1
	CleanupStack::PushL ( CreateChunkMgrL ( KMinHeapSize ) );
	RMemoryAllocator allocator ( iChkMgr );	
	
	TBuf8<1000> *aDes1;
    CleanupStack::PushL ( aDes1 = new ( ELeave ) TBuf8<1000> );
    aDes1->SetLength ( 1000 );
	
	//-------------- step 2: Set User heap to fail at the next alloc
	User::__DbgSetAllocFail ( RHeap::EUser, RHeap::EFailNext, 0 );

	//-------------- step 2 --------------------
	RMemChunk chunk;
	TInt ret = chunk.Alloc ( 1000, allocator );
	if ( ret != KErrNone )
		{
		User::__DbgSetAllocFail ( RHeap::EUser, RHeap::ENone, 0 );
		User::Leave ( EFail );
		}

	//-------------- step 3 Cleanup
	chunk.Free ();
	
	User::__DbgSetAllocFail ( RHeap::EUser, RHeap::ENone, 0 );
    
    CleanupStack::PopAndDestroy ( aDes1 );
    CleanupStack::PopAndDestroy ( iChkMgr );

   	__UHEAP_MARKEND;
	return EPass;
#else
	return EPass;
#endif
	}
