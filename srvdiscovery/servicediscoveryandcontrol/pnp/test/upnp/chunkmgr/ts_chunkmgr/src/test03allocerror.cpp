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

#include "test03allocerror.h"

TVerdict CTest03AllocError::doTestStepL(void)
	{
	__UHEAP_MARK;
			
	TVerdict verdict ( EFail );
	
	// -------------- step 1: Create heap with 15K
	CleanupStack::PushL ( CreateChunkMgrL ( 65536 ) );	
	RMemoryAllocator allocator ( iChkMgr );	
	// -------------- step 2: Allocate 42000 bytes long memchunks
	RMemChunk memChunks[14];
	TInt ret;	
	for ( TInt i = 0; i < sizeof ( memChunks ) / sizeof ( memChunks[0] ); i++ )
		{
		ret = memChunks[i].Alloc ( 3000, allocator );
		if ( ret != KErrNone )
			{
			User::Leave(EFail);
			}
		}
	// -------------- step 3: Allocate 16384-bytes long memchunk
	RMemChunk chunk;	
	ret = chunk.Alloc ( 16384, allocator );	
	if ( ret != KErrNone )
		{
		if ( ret == KErrNoMemory )
			verdict =  EPass;
		else
			{
			verdict =  EFail;
			}
		}		
	// -------------- step 4: Cleanup
	for ( TInt i = 0; i < sizeof ( memChunks ) / sizeof ( memChunks[0] ); i++ )
		{
		memChunks[i].Free ();
		}
	chunk.Free ();
	
	CleanupStack::PopAndDestroy ( iChkMgr );
	
	__UHEAP_MARKEND;
	return verdict;
	}
