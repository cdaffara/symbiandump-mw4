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
#include "cchunkmanager.h"
#include "cchunkallocator.h"
#include "cmemchunk.h"
#include "rchunkpool.h"
#include "rmemcell.h"

EXPORT_C CChunkManager* CChunkManager::NewL ( TInt aHeapSize )
	{
	CChunkManager* self = new ( ELeave ) CChunkManager ( aHeapSize );
	CleanupStack::PushL ( self );
	self->ConstructL ();
	CleanupStack::Pop ( self );
	return self;
	}

CChunkManager::CChunkManager ( TInt aHeapSize )
	:iHeapSize ( aHeapSize )
	{
	}
	
EXPORT_C CChunkManager::~CChunkManager ()
	{
	delete iChunkAllocator;
	for ( TInt i = 0; i < iChunkPools.Count (); i++ )
		{
		iChunkPools[i].Close ();
		}
	iChunkPools.Close ();
	iLock.Close ();
	}
	
void CChunkManager::ConstructL ()
	{
	User::LeaveIfError ( iLock.CreateLocal () );
	iChunkAllocator = CChunkAllocator::NewL ( iHeapSize );
	}
	
CChunkAllocator& CChunkManager::Allocator ()
	{
	return *iChunkAllocator;
	}
	
EXPORT_C void CChunkManager::AddPoolL ( TInt aBlockSize, TInt aNumBlocks, TInt aThresholdValue, TInt aMinGrowth )
	{
	__ASSERT_ALWAYS ( aBlockSize > 0, User::Invariant () ); // TODO
    __ASSERT_ALWAYS ( aNumBlocks > 0, User::Invariant () );
    __ASSERT_ALWAYS ( aThresholdValue >= 0, User::Invariant () );
   	__ASSERT_ALWAYS ( aMinGrowth >= 0, User::Invariant () );
   	    
    // aling the block size
   	const TUint8 KMemAlignment = 8;
   	TInt blockPadding = aBlockSize % KMemAlignment;
	if ( blockPadding != 0 )
    	{
	 	aBlockSize += KMemAlignment - blockPadding;
        }
    if ( aMinGrowth == 0 )
      	{
        aMinGrowth = 1;
        }
 	
 	
 	//maintains iChunkPools in blocks size order
    TInt found = KErrNotFound;
    TInt index = 0;
    RChunkPool tempPool ( *this, aBlockSize );
    found  = iChunkPools.FindInOrder ( tempPool, index, TLinearOrder<RChunkPool>(RChunkPool::Compare) );
    
    // create a new pool chain
    if ( found == KErrNotFound )
   		{
        RChunkPool pool ( aBlockSize, aThresholdValue, aMinGrowth, *this );
        //maintains iChunkPools in blocks size order
        User::LeaveIfError ( iChunkPools.Insert ( pool, index ) );
        
        RChunkPool* chunkPool = &iChunkPools[index];
		chunkPool->OpenL ();
		
        // Lock the freelist, inorder to add new blocks to the free list
        chunkPool->Wait ();
        TInt err = AllocChunk ( *chunkPool, aNumBlocks );
        chunkPool->Signal ();
		
        User::LeaveIfError ( err );
    	}
	else
    	{
        // update existing values
        iChunkPools[found].SetGrowth ( aMinGrowth );
        iChunkPools[found].SetThreshold ( aThresholdValue );
        }
	}
	
TInt CChunkManager::AllocChunk ( RChunkPool& aChunkPool, TInt aNumBlocks )
	{
    // verify if maximum size is exceeded
    if ( ( iUsedSpace + aNumBlocks * aChunkPool.BlockSize () ) > iHeapSize )
        {
        return KErrNoMemory;
        }
	
    CMemChunk* newChunk = CMemChunk::New ( *iChunkAllocator, aChunkPool, aNumBlocks, aChunkPool.BlockSize () );
	if ( newChunk != NULL )
        {
        iLock.Wait ();
        aChunkPool.Append ( newChunk );
        iLock.Signal ();
		
		// RChunkPool lock is already acquired
        aChunkPool.AppendToFreeList ( newChunk );
		
        iUsedSpace += sizeof ( CMemChunk ) + ( aNumBlocks * aChunkPool.BlockSize () ) ;
        return KErrNone;
        }
	
	return KErrNoMemory;
	}

RMemCell* CChunkManager::Alloc ( TInt aSize )
	{
	TInt poolsCount = iChunkPools.Count ();
	__ASSERT_DEBUG ( poolsCount != 0, User::Invariant () ); //TODO
	
	RMemCellList list;
	// Search and get the avaliable memCell list
	SearchAndAlloc ( list, aSize );
	
	return list.First ();
	}
	
void CChunkManager::SearchAndAlloc ( RMemCellList& aList, TInt aSize )
	{
	if ( aSize == 0 ) 
		{
		__ASSERT_ALWAYS ( 0, User::Invariant () ); //TODO
		}
	
	RChunkPool* poolChosen = NULL;
	RChunkPool* startPool = &iChunkPools[0];
	RChunkPool* pool = &iChunkPools[iChunkPools.Count () - 1];
	
	TInt minUnusedBytes = 0, minBlocks = 0;
	
	do
		{
		TInt blockSize = pool->BlockSize ();
		
		TInt unusedBytes = aSize % blockSize;
		TInt neededBlocks = aSize / blockSize;
		
		if ( unusedBytes != 0 )
			{
			unusedBytes = blockSize - unusedBytes;
			neededBlocks += 1;
			}
		
		if ( poolChosen == NULL ) //for the first time this will be true
			{
			poolChosen = pool;
			minUnusedBytes = unusedBytes;
			minBlocks = neededBlocks;
			}
		else
			{
			if ( unusedBytes == minUnusedBytes )
				{
				if ( neededBlocks < minBlocks )
					{
					poolChosen = pool;
					minUnusedBytes = unusedBytes;
					minBlocks = neededBlocks;
					}
				}
			else if ( unusedBytes < minUnusedBytes )
				{
				poolChosen = pool;
				minUnusedBytes = unusedBytes;
				minBlocks = neededBlocks;
				}
			}
		} while ( pool-- != startPool );
	
	if ( poolChosen == NULL )
		{
		__ASSERT_DEBUG ( 0, User::Invariant () );
		}
	
	TUint allocatedBytes = 0;
	TBool growthFailed = EFalse;
	if ( aSize <= poolChosen->FreeSpace () )
		{
		allocatedBytes = poolChosen->Alloc ( aList, aSize );
		}
	else
		{
		allocatedBytes = poolChosen->GrowAndAlloc ( aList, aSize, minBlocks );
				
		if ( allocatedBytes == 0 )
			{
			// need to search for the next best match and however, this time
			// requesting pool to grow may not be worth :), as only error condition is KErrNoMemory
			growthFailed = ETrue;
			}
		}
	
	if ( allocatedBytes >= aSize )
		{
		return;
		}
	else if ( growthFailed )
		{
		// just get the pool that can satisfy the request, no more best,good,worst match
		for ( TInt i = 0; i < iChunkPools.Count (); i++ )
			{			
			if ( aSize <= iChunkPools[i].FreeSpace () )
				{
				allocatedBytes = iChunkPools[i].Alloc ( aList, aSize );
				break;
				}
			}
		}
	}
	
TInt CChunkManager::BytesAvailable ()
	{
	TInt freeSpace = iHeapSize - iUsedSpace;
	
	freeSpace -= sizeof ( CMemChunk );
	
	return freeSpace <= 0 ? KErrNoMemory : freeSpace;
	}
	
// return a chain of MBufs to the pool
void CChunkManager::Free ( RMemCell* aPtr )
	{
	RChunkPool* pool ( aPtr->Pool () );
	pool->Wait ();
	
	while ( aPtr != NULL )
		{
		RMemCell* nextPtr = aPtr->Next ();
		
		// reset length to the size of the block and break the link
		aPtr->SetLength ( aPtr->Size () );
		aPtr->Link ( NULL );
		
		// adds single block back to its free list and adjusts its free space
		pool->FreeToPool ( aPtr );
		
		aPtr = nextPtr;
		}
	pool->Signal ();
	}


