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

#include "test02allocdealloc.h"

TVerdict CTest02AllocDealloc::doTestStepL ( void )
	{
	__UHEAP_MARK;

	//-------------- step 1 --------------------
	CleanupStack::PushL ( CreateChunkMgrL ( KMinHeapSize ) );

	TBool firstFail = EFalse;
	TBool secondFail = EFalse;
	
	//-------------- step 2 --------------------
	RMemChunk memChunk;
	RMemoryAllocator allocator ( iChkMgr );
	TInt ret = memChunk.Alloc ( 5000, allocator );
	if ( ret != KErrNone )
		{
		firstFail = ETrue;
		}
	else
		{
		memChunk.Free ();
		}
	
	//-------------- step 3 --------------------
	TInt ret1 = memChunk.Alloc ( 0,  allocator );
	if ( ret1 != KErrNone )
		{
		secondFail = ETrue;
		}
	else
		{
		/* // TODO uncomment this, once Length API is implemented.
		
		if ( memChunk.Length () != 0 )
			{
			secondFail = ETrue;
			}
		*/
		memChunk.Free ();
		}
	
	CleanupStack::PopAndDestroy ( iChkMgr );
	
    __UHEAP_MARKEND;
    
    if ( firstFail || secondFail )
    	{
    	return EFail;
    	}
    
	return EPass;		
	}
