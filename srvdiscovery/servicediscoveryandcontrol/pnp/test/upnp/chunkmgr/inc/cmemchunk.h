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


#ifndef __CMEMCHUNK_
#define __CMEMCHUNK_
#include "rmemcell.h"

class RMemCellList
	{
public:
	RMemCellList ()
		{
		Init ();
		}
	RMemCellList ( RMemCell* aFirst, RMemCell* aLast )
		: iNext ( aFirst ), iLast ( aLast )
		{
		
		}
	
	void Append ( RMemCell* aPtr );
	void Append ( RMemCellList& aList );
	TInt Transfer ( RMemCellList& aList, TInt aSize );
	
	void Init ()
		{
		iNext = iLast = NULL;		
		}
	
	TBool IsEmpty ()
		{
		return ( iNext == NULL ) ? ETrue : EFalse;	
		}
	RMemCell* First ()
		{
		return iNext;		
		}
	
	RMemCell* Last ()
		{
		return iLast;
		}
		
private:
	RMemCell* iNext;
	RMemCell* iLast;		
	};

class CMemChunk : public CBase
	{
public:
	static CMemChunk* New ( CChunkAllocator& aAllocator, RChunkPool& aChunkPool, TInt aNumBlocks , TInt aBlockSize );
	~CMemChunk ();
	RMemCellList& List ()
		{
		return iQueue;
		}
	TInt NumBlocks ()
		{
		return iNumBlocks;
		}
		
private:
	CMemChunk ( CChunkAllocator& aAllocator, TInt aNumBlocks );
	TAny* operator new (TUint aNumBlocks, TInt aExtra, CChunkAllocator& aAllocator );
	void operator delete ( TAny* aPtr, TInt aExtra, CChunkAllocator& aAllocator );
	void CMemChunk::operator delete ( TAny* aPtr );
	
private:
	RMemCellList		iQueue;
	CChunkAllocator&	iChunkAllocator;	
	TUint				iNumBlocks;
	};

#endif /*__CMEMCHUNK_*/
