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
#include "cchunkallocator.h"
#include "rmemcell.h"
#include "cmemchunk.h"

#define MASK 0x03
#define WORD_ALIGN(x) ( ( ( (x) + (MASK) )& ~MASK ) ) // aligns 'x' to the nearest word size
#define IS_WORD_ALIGNED(x) ( ( (TUint32)(x) & MASK ) == 0 )


CMemChunk* CMemChunk::New ( CChunkAllocator& aAllocator, RChunkPool& aChunkPool, TInt aNumBlocks , TInt aBlockSize )
	{
	// allocates chunk on shared Heap using ::new operator
	
	// round of RMemCell & aBlockSize to the nearest word size.
	TInt memCellSize = WORD_ALIGN ( sizeof ( RMemCell ) );
	TInt blockSize = WORD_ALIGN ( aBlockSize );
	
	CMemChunk* chunk = new ( aNumBlocks * ( memCellSize + blockSize ), aAllocator ) CMemChunk ( aAllocator, aNumBlocks );
	
	// In the complete memory allocated on shared heap; first bytes are of CMemChunk,
	// then start the memory required for RMemCell * NumBlocks and at the end contains
	// the actual data buffer.
	
	if ( chunk != NULL )
		{
		// memory required for CMemChunk and for its parameters storage
		TUint8* chunkSize = ( ( TUint8* ) chunk ) + sizeof ( CMemChunk );
	 	__ASSERT_DEBUG ( IS_WORD_ALIGNED ( chunkSize ), User::Invariant () ); // TODO Proper panic code
		
	 	// memory required for RMemCell and its parameters storage
		TUint8* dataBufStart = chunkSize + ( aNumBlocks * memCellSize );
		
		// now place each RMemCell and its databuffer in the address specified
		for ( TInt i = 0; i < aNumBlocks ; i++ )
			{
			// address assigned for RMemCell 'i'
			RMemCell* hdr = ( RMemCell* ) ( chunkSize + i * memCellSize );
			
			// overloaded ::new, that makes in-place allocation of MemCell at specified address
			( void ) new ( hdr ) RMemCell ( &aChunkPool, dataBufStart + ( i * aBlockSize ), aBlockSize );
			
			// completed construction of a RMemCell.
			chunk->iQueue.Append ( hdr );
			}
		}
	
	return chunk;
	}

CMemChunk::CMemChunk ( CChunkAllocator& aAllocator, TInt aNumBlocks )
	: iChunkAllocator ( aAllocator ), iNumBlocks ( aNumBlocks )
	{
	}

CMemChunk::~CMemChunk ()
	{	
	}

TAny* CMemChunk::operator new ( TUint aSize, TInt aExtra, CChunkAllocator& aAllocator )
	{
	return aAllocator.Alloc ( aSize + aExtra );
	}

void CMemChunk::operator delete ( TAny* aPtr, TInt /*aExtra*/, CChunkAllocator& /*aAllocator*/ )
	{
	CMemChunk::operator delete ( aPtr );
	}

void CMemChunk::operator delete ( TAny* aPtr )
	{
	if ( aPtr )
		{
		( ( CMemChunk* ) aPtr )->iChunkAllocator.Free ( aPtr );
		}
	}

void RMemCellList::Append ( RMemCell* aPtr )
	{
	if ( IsEmpty () )	
		{
		iNext = iLast = aPtr;
		}
	else
		{
		iLast->Link ( aPtr );
		iLast = aPtr;
		}
	}
	
void RMemCellList::Append ( RMemCellList& aList )
	{
	if ( aList.IsEmpty () )
		{
		return;
		}
	
	if ( this->IsEmpty () )
		{
		iNext = aList.First ();
		iLast = aList.Last ();		
		}
	else
		{
		iLast->Link ( aList.First () );
		iLast = aList.Last ();
		}
	aList.Init ();
	}
	
TInt RMemCellList::Transfer ( RMemCellList& aList, TInt aSize )
	{
	__ASSERT_DEBUG ( aSize >= 0, User::Invariant () ); //TODO
	TInt transferedBytes = 0;
	
	if ( !IsEmpty () )
		{
		RMemCell* first;
		RMemCell* next;
		RMemCell* last;
		
		// catch hold of first link
		first = iNext;
		next = first;
		last = first;
		
		while ( aSize >= 0 && next != NULL )
			{
			transferedBytes += next->Size ();
			aSize -= next->Size ();
			
			last = next;
			
			next = next->Next ();
			
			if ( aSize == 0 )
				{
				break;
				}
			}
		last->SetLength(aSize + last->Size());
		
		if ( next == NULL ) // all the blocks are transfferred
			{
			Init ();
			}
		else 				// still left with more blocks, so adjust the start & End links
			{
			last->Link ( NULL );
			iNext = next;
			}
		
		// now the last link is ready to use
		RMemCellList newList ( first, last );
		aList.Append ( newList );
		}
	
	return transferedBytes;
	}


