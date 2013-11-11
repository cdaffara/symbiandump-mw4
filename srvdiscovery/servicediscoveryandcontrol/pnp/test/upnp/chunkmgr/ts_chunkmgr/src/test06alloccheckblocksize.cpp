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
#include <rmemcell.h>

#include "test06alloccheckblocksize.h"


TVerdict CTest06AllocCheckBlockSize::doTestStepL ()
	{
	__UHEAP_MARK;
	
	CleanupStack::PushL ( CreateChunkMgrL ( KMinHeapSize ) );
	RMemoryAllocator allocator ( iChkMgr );
	
	iChkMgr->AddPoolL ( 2048, 8, 4, 3 );
	iChkMgr->AddPoolL ( 1024, 16, 5, 4 );
	iChkMgr->AddPoolL ( 512, 32, 6, 5 );
	iChkMgr->AddPoolL ( 256, 64, 7, 6 );
	iChkMgr->AddPoolL ( 128, 128, 64, 40 );
	
	TBool firstTestFail = EFalse;
	TBool secondTestFail = EFalse;
	TBool thirdTestFail = EFalse;
	// --------------First Test
	RMemChunk chunk;
	INFO_PRINTF1 ( _L ( "\nAllocate 5000-bytes long RMemChunk" ) );
	TInt ret = chunk.Alloc ( 5000, allocator );
	if ( ret != KErrNone )
		{
		INFO_PRINTF1 ( _L ( "\nError: Couldn't allocate" ) );
		User::Leave ( EFail );
		}
	
	//Check Block size is really 512
	RMemCell* block = chunk.First ();
	if ( block->Size () != 1024 )
		{
		INFO_PRINTF1 ( _L ( "Error: Block size is not as expected" ) );
		firstTestFail = ETrue;
		}
	
	INFO_PRINTF1 ( _L ( "Deallocating RMemChunk" ) );
	chunk.Free ();
	
	// --------------Second Test
	INFO_PRINTF1 ( _L ( "Allocate 50-bytes long RMemChunk" ) );	
	ret = chunk.Alloc ( 50, allocator );
	if ( ret != KErrNone )
		{
		INFO_PRINTF1 ( _L ( "Error: Couldn't allocate" ) );
		User::Leave ( EFail );
		}
	
	//Check Block size is really 128
	block = chunk.First ();
	if ( block->Size () != 128 )
		{
		INFO_PRINTF1 ( _L ( "Error: Block size is not as expected" ) );
		secondTestFail = ETrue;
		}
	
	INFO_PRINTF1 ( _L ( "Deallocating RMemChunk" ) );
	chunk.Free ();
	
	// --------------Third Test
	INFO_PRINTF1 ( _L ( "Allocate 1024-bytes long RMemChunk" ) );
	ret = chunk.Alloc ( 1024, allocator );
	if ( ret != KErrNone )
		{
		INFO_PRINTF1 ( _L ( "Error: Couldn't allocate" ) );
		User::Leave ( EFail );
		}
	
	// Check Block size is really 1024
	block = chunk.First ();
	if ( block->Size () != 1024 )
		{
		INFO_PRINTF1 ( _L ( "Error: Block size is not as expected" ) );
		thirdTestFail = ETrue;
		}
	
	INFO_PRINTF1 ( _L ( "Deallocating RMemChunk" ) );
	chunk.Free ();
	
	CleanupStack::PopAndDestroy ( iChkMgr );
	
	__UHEAP_MARKEND;
	
	if ( firstTestFail || secondTestFail || thirdTestFail )
		return EFail;
			
	return EPass;
	}
