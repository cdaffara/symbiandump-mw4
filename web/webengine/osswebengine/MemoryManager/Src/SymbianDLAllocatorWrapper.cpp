/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  
*
*
*/

#include "FastAllocator.h"
#include "MemoryManager.h"
#include "MemoryPool.h"
#include <e32std.h>
#include <e32debug.h>

RSymbianDlAllocatorWrapper::RSymbianDlAllocatorWrapper(CNewSymbianHeapPool* aPool) : iPool( aPool )
{
#ifdef TRACK_ALLOCATIONS
	iNextCellId = 0;
#endif
}

RSymbianDlAllocatorWrapper::~RSymbianDlAllocatorWrapper()
{
#ifdef OOM_LOGGING
    iPool->DumpHeapLogs();
#endif
}

TAny* RSymbianDlAllocatorWrapper::Alloc(TInt aSize)
{
#ifdef TRACK_ALLOCATIONS
	return TraceAlloc(iPool, &aSize, &iNextCellId);
#else
    return iPool->Allocate( aSize );
#endif
}

void RSymbianDlAllocatorWrapper::Free(TAny* aPtr)
{
#ifdef TRACK_ALLOCATIONS
	TraceFree(&aPtr);
#endif	
	iPool->Free( aPtr );
}

TAny* RSymbianDlAllocatorWrapper::ReAlloc(TAny* aPtr, TInt aSize, TInt aMode)
{
#ifdef TRACK_ALLOCATIONS
	TUint32 originalCellId;
	TBool issueNewCellId = TracePreRealloc(&aPtr, &aSize, originalCellId);
#endif
	
	TAny* p = iPool->ReAllocate( aPtr, aSize );

    // Handle aMode.
	// 0: default. Don't move on shrink, but can move if growing
    // ENeverMove: Don't move memory on realloc, return NULL if it was moved.
    // EAllowMoveOnShrink: Allow moving memory on realloc when size requested
    //    is smaller, return NULL if moved and request size is greater (equal).
	//
	// This allocator only operates in mode 0 (for now), so we only have to check the stricter ENeverMove mode
	//
    if ( (aMode == ENeverMove && p != aPtr) )
        {
        Free(p);
        return NULL;
        }
#ifdef TRACK_ALLOCATIONS
    TracePostRealloc(&p, aSize, &iNextCellId, originalCellId, issueNewCellId);
#endif
    return p;
}

TInt RSymbianDlAllocatorWrapper::AllocLen(const TAny* aCell) const
{
#ifdef TRACK_ALLOCATIONS
	if(aCell)
		aCell = GetRealPtrFromClientPtr((TAny*)aCell);
	TInt size = iPool->MemorySize( (void*)(aCell) );
	DecreaseSizeInPlace(size);
	return size;
#else
	return iPool->MemorySize( (void*)(aCell) );
#endif
}

TInt RSymbianDlAllocatorWrapper::Compress()
{
    // DO NOTHING
    return 0;
}

void RSymbianDlAllocatorWrapper::Reset()
{
    // DO NOTHING
}

TInt RSymbianDlAllocatorWrapper::AllocSize(TInt& aTotalAllocSize) const
{
    // allocated cell size
    aTotalAllocSize = 0;
    return 0;
}

TInt RSymbianDlAllocatorWrapper::Available(TInt& aBiggestBlock) const
{
    // make sure the caller is not scared of futher allocation
    aBiggestBlock = KMaxTInt;
    return KMaxTUint;
}

TInt RSymbianDlAllocatorWrapper::DebugFunction(TInt, TAny*, TAny*)
{
    return 0;
}

TInt RSymbianDlAllocatorWrapper::Extension_(TUint, TAny*&, TAny*)
{
    return 0;
}

// END OF FILE
