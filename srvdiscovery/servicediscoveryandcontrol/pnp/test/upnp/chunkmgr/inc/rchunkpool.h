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

#ifndef __RCHUNKPOOL_H_
#define __RCHUNKPOOL_H_

#include "cmemchunk.h"

class RChunkPool
	{
public:
	RChunkPool ( TInt aBlockSize, TInt aThresholdValue, TInt aMinGrowth, CChunkManager& aChunkManager );
	RChunkPool ( CChunkManager& aChunkManager, TInt aBlockSize );
	
	void OpenL ();
	void Close ();
	
	TInt Alloc ( RMemCellList& aList, TInt aSize );
	TInt GrowAndAlloc ( RMemCellList& aList, TInt aSize, TInt aNumBlocks );
	void FreeToPool ( RMemCell* aMemCell );
	void AppendToFreeList ( CMemChunk* aChunk );
	static TInt Compare ( const RChunkPool& aLHSPool, const RChunkPool& aRHSPool );
	
	TInt BlockSize () const
		{
		return iBlockSize;
		}
		
	void Wait ()
		{
		iFreeListLock.Wait ();
		}
	
	void Signal ()
		{
		iFreeListLock.Signal ();
		}
		
	void SetThreshold ( TInt aThreshold )
		{
		iThresholdValue = aThreshold;
		}
		
	void SetGrowth ( TInt aGrowth )
		{
		iMinGrowth = aGrowth;
		}
		
	void Append ( CMemChunk* aNewChunk )
		{
		iAllocatedChunks.Append ( aNewChunk );
		}
		
	TInt FreeSpace ()
		{
		return iFreeSpace;
		}
		
	CChunkManager& ChunkManager ()
		{
		return iChunkManager;
		}
		
private:	
	TInt GrowPool ( TUint aNumBlocks );
	
private:
	TInt						iBlockSize;
	RPointerArray<CMemChunk>	iAllocatedChunks;
	CChunkManager&				iChunkManager;
	RMemCellList				iFreeBlocks;
	RFastLock					iFreeListLock;
	TInt						iFreeSpace;
	TInt						iUsedSpace; //TODO: can be removed???
	TInt 						iThresholdValue;
	TInt 						iMinGrowth;
	};

#endif /*__RCHUNKPOOL_H_*/
