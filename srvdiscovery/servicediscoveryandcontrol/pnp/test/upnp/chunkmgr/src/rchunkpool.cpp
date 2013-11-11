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
#include "rmemcell.h"
#include "cmemchunk.h"
#include "rchunkpool.h"


RChunkPool::RChunkPool ( TInt aBlockSize, TInt aThresholdValue, TInt aMinGrowth, CChunkManager& aChunkManager )
		: iBlockSize ( aBlockSize ),
	      iChunkManager ( aChunkManager ),
		  iThresholdValue ( aThresholdValue ),
	      iMinGrowth ( aMinGrowth )
	{
	iFreeBlocks.Init ();
	iFreeSpace = 0;
	}

RChunkPool::RChunkPool ( CChunkManager& aChunkManager, TInt aBlockSize )
	: iBlockSize ( aBlockSize ),
	  iChunkManager ( aChunkManager )
	{	
	}
	
void RChunkPool::OpenL ()
	{
	User::LeaveIfError ( iFreeListLock.CreateLocal () );
	}
	
void RChunkPool::Close ()
	{
	iFreeListLock.Close ();
	iAllocatedChunks.Close ();
	}
	
TInt RChunkPool::Compare ( const RChunkPool& aLHSPool, const RChunkPool& aRHSPool )
	{
	if ( aLHSPool.BlockSize () < aRHSPool.BlockSize () )
		{
		return -1;
		}
	else if ( aLHSPool.BlockSize () > aRHSPool.BlockSize ()  )
		{
		return 1;
		}
	return 0;
	}
		
void RChunkPool::AppendToFreeList ( CMemChunk* aChunk )
	{
	iFreeSpace += iBlockSize * aChunk->NumBlocks ();
	iFreeBlocks.Append ( aChunk->List () );
	}	


// transfer's blocks from free list and makes self growth if necessary
TInt RChunkPool::Alloc ( RMemCellList& aList, TInt aSize )
	{
	// Lock the freelist so that 
	// Transfer of free blocks will be an atomic operation
	iFreeListLock.Wait ();
	
	TInt bytesTransfered = iFreeBlocks.Transfer ( aList, aSize );
	iFreeSpace -= bytesTransfered;
	
	TInt freeHeapSpace = iChunkManager.BytesAvailable ();
	
	if ( freeHeapSpace ) // makes sure that heap has memory atleast for CMemChunk object, but not for its chunk
		{
		// check if freeblocks is down the threshold value and make self growth if necessary
		// determine min number of blocks to allocate
		TInt minGrowth = iThresholdValue - ( iFreeSpace / iBlockSize );
		
		if ( minGrowth >= 0 )
			{
			if ( GrowPool ( iMinGrowth ) == KErrNoMemory )
				{
			 	// unable to grow the minimum value set by the client
			 	if ( minGrowth < iMinGrowth )
				 	{
			 		// so, now try with the min value adjusted with threshold value
			 		GrowPool ( minGrowth );
				 	}
			 	}
			}
		}
	
	// free the freelistlock
	iFreeListLock.Signal ();
	
	return bytesTransfered;
	}
	
TInt RChunkPool::GrowPool ( TUint aNumBlocks )
	{
	return iChunkManager.AllocChunk ( *this, aNumBlocks );
	}
	
TInt RChunkPool::GrowAndAlloc ( RMemCellList& aList, TInt aSize, TInt aNumBlocks )
	{
	// Lock the freelist so that
	// Transfer of free blocks will be an atomic operation
	TInt bytesTransfered = 0;
	iFreeListLock.Wait ();
	
	if ( GrowPool ( aNumBlocks ) != KErrNoMemory )
	 	{
	 	bytesTransfered = iFreeBlocks.Transfer ( aList, aSize );
		iFreeSpace -= bytesTransfered;
	 	}
	
	// free the freelistlock
	iFreeListLock.Signal ();
	
	return bytesTransfered;
	}
	
void RChunkPool::FreeToPool ( RMemCell* aMemCell )
	{
	iFreeBlocks.Append ( aMemCell );
	iFreeSpace += aMemCell->Size ();
	}


