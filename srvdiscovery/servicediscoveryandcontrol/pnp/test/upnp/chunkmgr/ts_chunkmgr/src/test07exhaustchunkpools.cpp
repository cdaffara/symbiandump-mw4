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

#include "test07exhaustchunkpools.h"


TVerdict CTest07ExhaustChunkPools::doTestStepL ()
	{
	__UHEAP_MARK;
	
	CleanupStack::PushL ( CreateChunkMgrL ( KMinHeapSize / 2) );
	RMemoryAllocator allocator ( iChkMgr );	
	iChkMgr->AddPoolL ( 128, 128, 64, 40 );
	iChkMgr->AddPoolL ( 256, 64, 7, 6 );
	iChkMgr->AddPoolL ( 512, 32, 6, 5 );
	iChkMgr->AddPoolL ( 1024, 16, 5, 4 );
	iChkMgr->AddPoolL ( 2048, 8, 4, 3 );	
	INFO_PRINTF1 ( _L ( "\nStart allocating 500-bytes long RMemChunks" ) );
	RArray<RMemChunk> chunkArr128, chunkArr256, chunkArr512, chunkArr1024, chunkArr2048;
	TInt ret = KErrNone;
	
	TInt allocCount128 = 0, allocCount258 = 0, allocCount512 = 0, allocCount1024 = 0, allocCount2048 = 0;
	while ( ret == KErrNone )
		{
		RMemChunk chunk;
		TInt ret = chunk.Alloc ( 500, allocator );		
		if ( ret != KErrNone )
			{
			break;
			}
		switch ( chunk.First ()->Size () )
			{
			case 128:
				{
				INFO_PRINTF1 ( _L ( "\nAllocated in 128 block Pool" ) );
				chunkArr128.Append ( chunk );
				break;
				}
			case 256:
				{
				INFO_PRINTF1 ( _L ( "\nAllocated in 256 block Pool" ) );
				chunkArr256.Append ( chunk );
				break;
				}
			case 512:
				{
				INFO_PRINTF1 ( _L ( "\nAllocated in 512 block Pool" ) );
				chunkArr512.Append ( chunk );
				break;
				}
			case 1024:
				{
				INFO_PRINTF1 ( _L ( "\nAllocated in 1024 block Pool" ) );				
				chunkArr1024.Append ( chunk );
				break;
				}
			case 2048:
				{
				INFO_PRINTF1 ( _L ( "\nAllocated in 2048 block Pool" ) );				
				chunkArr2048.Append ( chunk );
				break;
				}
			}
		}

	INFO_PRINTF2 ( _L ( "\nTotal allocation in 128 block Pool - %d" ), chunkArr128.Count () );
	INFO_PRINTF2 ( _L ( "\nTotal allocation in 256 block Pool - %d" ), chunkArr256.Count () );
	INFO_PRINTF2 ( _L ( "\nTotal allocation in 512 block Pool - %d" ), chunkArr512.Count () );
	INFO_PRINTF2 ( _L ( "\nTotal allocation in 1024 block Pool - %d" ), chunkArr1024.Count () );
	INFO_PRINTF2 ( _L ( "\nTotal allocation in 2048 block Pool - %d" ), chunkArr2048.Count () );
	
	
	TBool firstSuccess = ETrue;
	TBool secondSuccess = ETrue;
	TBool thirdSuccess = ETrue;	
		
	INFO_PRINTF1 ( _L ( "Allocate 750-byte long RMemChunk after deallocating 3 128-byte and 2 512-byte long RMemChunks" ) );
	chunkArr128[0].Free ();
	chunkArr128.Remove ( 0 );
	chunkArr128[0].Free ();
	chunkArr128.Remove ( 0 );	
	chunkArr128[0].Free ();
	chunkArr128.Remove ( 0 );
	
	
	chunkArr512[0].Free ();
	chunkArr512.Remove ( 0 );	
		
	RMemChunk chunk1;	
	ret = chunk1.Alloc ( 750, allocator );
	
	if ( ret != KErrNone )
		{
		INFO_PRINTF1 ( _L ( "Error: Couldn't allocate" ) );
		firstSuccess = EFalse;
		}	
	else
		{
		if ( chunk1.First ()->Size () != 128 )
			{
			INFO_PRINTF1 ( _L ( "\nAllocated in 1024 block Pool" ) );				
			firstSuccess = EFalse;
			}
		chunkArr128.Append ( chunk1 );
		}	
	
		
	INFO_PRINTF1 ( _L ( "Allocate 500-byte long RMemChunk" ) );
	
	RMemChunk chunk2;
	ret = chunk2.Alloc ( 500, allocator );
	if ( ret != KErrNone )
		{
		INFO_PRINTF1 ( _L ( "Error: Couldn't allocate" ) );
		secondSuccess = EFalse;
		}
	else
		{
		if ( chunk2.First ()->Size () != 512 )
			{
			INFO_PRINTF1 ( _L ( "\nAllocated in 1024 block Pool" ) );				
			firstSuccess = EFalse;
			}
		chunkArr512.Append ( chunk2 );
		}	
	
	
		
	INFO_PRINTF1 ( _L ( "Allocate 2100-byte long RMemChunk after deallocating 2 2048-byte long RMemChunk" ) );	
	chunkArr128[0].Free ();
	chunkArr128.Remove ( 0 );
	
	chunkArr128[0].Free ();
	chunkArr128.Remove ( 0 );
	
	
	RMemChunk chunk3;
	ret = chunk3.Alloc ( 2100, allocator );
	if ( ret == KErrNone )
		{		
		thirdSuccess = EFalse;
		chunkArr512.Append ( chunk3 );
		}		
	
	chunkArr128.Close();
	chunkArr256.Close();
	chunkArr512.Close();
	chunkArr1024.Close();
	chunkArr2048.Close();
	
    CleanupStack::PopAndDestroy ( iChkMgr );
	
	__UHEAP_MARKEND;

	if ( ret != KErrNone || !firstSuccess || !secondSuccess || !thirdSuccess)
		{
		return EFail;
		}
	return EPass;
	}
