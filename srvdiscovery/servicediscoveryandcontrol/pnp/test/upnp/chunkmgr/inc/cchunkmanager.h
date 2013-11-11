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

#ifndef __CCHUNKMANAGER_H_
#define __CCHUNKMANAGER_H_

#include <e32def.h>

//Forward declaration
class CChunkAllocator;
class RChunkPool;
class RMemCellList;
class RMemCell;

class CChunkManager : public CBase
	{
public:
	IMPORT_C static CChunkManager* NewL ( TInt aHeapSize );
	IMPORT_C ~CChunkManager ();
	
	IMPORT_C void AddPoolL ( TInt aBlockSize, TInt aNumBlocks, TInt aThresholdValue, TInt aMinGrowth );
	
	CChunkAllocator& Allocator ();
	TInt AllocChunk ( RChunkPool& aChunkPool, TInt aNumBlocks );
	RMemCell* Alloc ( TInt aSize );
	void Free ( RMemCell* aPtr );
	
	// returns free space that can be used for CMemChunk after excluding sizeof CMemChunk
	TInt BytesAvailable ();
	
private:
	CChunkManager ( TInt aHeapSize );
	void ConstructL ();	
	void SearchAndAlloc ( RMemCellList& aList, TInt aSize );	
	
private:
	CChunkAllocator* 	iChunkAllocator;
	RArray<RChunkPool> 	iChunkPools;
	RFastLock 			iLock;
	TInt				iHeapSize;
	TInt				iUsedSpace;
	};

#endif /*__CCHUNKMANAGER_H_*/
