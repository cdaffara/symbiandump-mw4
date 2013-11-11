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
#include "cmemchunk.h"

CChunkAllocator* CChunkAllocator::NewL ( TInt aHeapSize )
	{
	CChunkAllocator* chunkAllocator = new ( ELeave ) CChunkAllocator;
	CleanupStack::PushL ( chunkAllocator );
	chunkAllocator->ConstructL ( aHeapSize );
	CleanupStack::Pop ();
	
	return chunkAllocator;
	}

CChunkAllocator::CChunkAllocator ()
	{
	}

CChunkAllocator::~CChunkAllocator ()
	{
	iHeap->Close ();
	}

void CChunkAllocator::ConstructL ( TInt aHeapSize )
	{
	iHeap = UserHeap::ChunkHeap ( NULL, 0, aHeapSize );
	if ( NULL == iHeap )
		{
		User::Leave ( KErrNoMemory );
		}
	}

TAny* CChunkAllocator::Alloc ( TInt aSize )
	{
	return iHeap->Alloc ( aSize );
	}

void CChunkAllocator::Free ( TAny* aPtr )
	{
	CMemChunk* chunk = ( CMemChunk* ) aPtr;
	iHeap->Free ( chunk );
	}
