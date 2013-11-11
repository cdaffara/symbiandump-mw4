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

#ifndef __RMEMCHUNK_
#define __RMEMCHUNK_

#include <e32base.h>
#include <cchunkmanager.h>

//forward Declaration
class RMemoryAllocator;

class RMemChunk
	{
public:
	inline RMemChunk ()
		{
		iNextCell = NULL;
		}
		
	IMPORT_C TInt Alloc ( TInt aSize, RMemoryAllocator& aAllocator );
	IMPORT_C TInt Create ( const TDesC8& aDes, RMemoryAllocator& aAllocator );
	IMPORT_C void CreateL ( const TDesC8& aDes,RMemoryAllocator& aAllocator );
	IMPORT_C void Free ();
	IMPORT_C RMemCell* Remove();
	IMPORT_C TInt NumBufs() const;
	IMPORT_C TInt Length() const;
	IMPORT_C void TrimStart(TInt anOffset);
	IMPORT_C void CopyIn(const TDesC8& aDes, TInt aOffset=0);
	IMPORT_C void CopyOut(TDes8 &aDes, TInt aOffset=0) const;
	IMPORT_C TBool Goto(TInt anOffset, RMemCell* &resBuf, TInt& resOffset, TInt& resLength, RMemCell* &resPrevBuf) const;
	IMPORT_C void Assign(RMemChunk& aChain);
	IMPORT_C void Append(RMemChunk& aChain);
	IMPORT_C RMemCell* Last() const;
	inline RMemCell* First ();
	inline TBool IsEmpty() const;
	inline void Init();
	
private:
	RMemCell*	iNextCell;
	};
	
inline TBool RMemChunk::IsEmpty() const
	{ 
	return iNextCell==NULL; 
	}

inline void RMemChunk::Init()
	{ 
	iNextCell = NULL;	
	}

class RMemoryAllocator
	{
public:
	RMemoryAllocator ( CChunkManager* aManager )
		: iManager ( aManager )
		{	}
	
	CChunkManager* ChunkManager ()
		{
		return iManager;
		}
	
	RMemoryAllocator (  )
		: iManager ( NULL )
			{	}
	
	void SetChunkManager (CChunkManager* aChunkMgr)
		{
		iManager = aChunkMgr;
		}
private:
	CChunkManager* iManager;	
	};	

inline RMemCell* RMemChunk::First ()
	{
	return iNextCell;
	}
	

class TMemCellIterator
	{
	public:
		TMemCellIterator();
		inline TMemCellIterator(RMemCell* aChain);
		inline TMemCellIterator(RMemChunk& aMemChunk);
		IMPORT_C RMemCell* operator++(TInt);
	private:
		RMemCell* iMemCell;
		
	};

inline TMemCellIterator::TMemCellIterator(RMemChunk& aMemChunk)
	: iMemCell(aMemChunk.First()) 
	{ }

inline TMemCellIterator::TMemCellIterator(RMemCell* aChain)
	: iMemCell(aChain)
	{ }

#endif /*__RMEMCHUNK_*/
