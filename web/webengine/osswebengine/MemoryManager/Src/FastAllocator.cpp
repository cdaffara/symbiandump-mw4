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


#ifdef TRACK_ALLOCATIONS
TAny* GetClientPtrFromRealPtr(TAny* aRealPtr)
	{
	return (TAny*)(((TUint8*)aRealPtr) + sizeof(THeapTrace));
	}
TAny* GetRealPtrFromClientPtr(TAny* aClientPtr)
	{
	return (TAny*)(((TUint8*)aClientPtr) - sizeof(THeapTrace));
	}

void IncreaseSizeInPlace(TInt& aSize)
	{
	aSize += sizeof(THeapTrace);
	}
void DecreaseSizeInPlace(TInt& aSize)
	{
	aSize -= sizeof(THeapTrace);
	}

TInt GetSizeFromRealPtr(TAny* aRealPtr)
	{
	return ((THeapTrace*)aRealPtr)->requestedSize;
	}

void SetSizeWithRealPtr(TAny *aRealPtr, TInt aSize)
	{
	((THeapTrace*)aRealPtr)->requestedSize = aSize;
	}

TUint32 GetCellIdWithRealPtr(TAny* aRealPtr)
	{
	return((THeapTrace*)aRealPtr)->cellId;
	}

void SetCellIdWithRealPtr(TAny* aRealPtr, TUint32 aCellId)
	{
	((THeapTrace*)aRealPtr)->cellId = aCellId;
	}
void IncrementAndSetCellIdWithRealPtr(TAny* aRealPtr, TUint32* aCellId)
	{
	((THeapTrace*)aRealPtr)->cellId = (*aCellId)++;
	}


TAny* TraceAlloc(CMemoryPool* aPool, TInt* aSize, TUint32* aCellId)
	{
	// add space at the start of the data area for a TInt tracking requested size
	TInt requestedSize = *aSize;
	IncreaseSizeInPlace(*aSize);
	TAny *ptr = aPool->Allocate( *aSize );
    SetSizeWithRealPtr(ptr, requestedSize);
    IncrementAndSetCellIdWithRealPtr(ptr, aCellId);
    
    RDebug::Print(_L("MEM: a,%d,,%d"), GetCellIdWithRealPtr(ptr), requestedSize);
	
    // somewhere to add a debugger to catch every 1024 allocations
    if((*aCellId & 0x0FFF) == 0)
    	{
    	aCellId = aCellId;
    	}
    
    return GetClientPtrFromRealPtr(ptr);
	}

void TraceFree(TAny **aPtr)
	{
	if(aPtr && *aPtr)
		{
		*aPtr = GetRealPtrFromClientPtr(*aPtr);
	    RDebug::Print(_L("MEM: f,%d,%d,"), GetCellIdWithRealPtr(*aPtr), GetSizeFromRealPtr(*aPtr));
		}
	}

TBool TracePreRealloc(TAny** aPtr, TInt *aSize, TUint32& aOriginalCellId)
	{
	TInt requestedSize = *aSize;
	TBool issueNewCellId = EFalse;
	aOriginalCellId=0;
	if(aPtr && *aPtr)
		{
		*aPtr = GetRealPtrFromClientPtr(*aPtr);
		aOriginalCellId = GetCellIdWithRealPtr(*aPtr);
	    RDebug::Print(_L("MEM: r,%d,%d,%d"), GetCellIdWithRealPtr(*aPtr), GetSizeFromRealPtr(*aPtr), requestedSize);
		}
	else
		{
		issueNewCellId = ETrue;
		}
	IncreaseSizeInPlace(*aSize);
	
	return issueNewCellId;
	}

void TracePostRealloc(TAny** p, TInt aRequestedSize, TUint32* aNewCellId, TUint32 aOriginalCellId, TBool aIssueNewCellId)
	{
    if(p && *p)
    	{
    	SetSizeWithRealPtr(*p, aRequestedSize);
    	if(aIssueNewCellId)
    		IncrementAndSetCellIdWithRealPtr(*p, aNewCellId);
    	else
    		SetCellIdWithRealPtr(*p, aOriginalCellId);
    	*p = GetClientPtrFromRealPtr(*p);
    	}
	}
#endif
EXPORT_C RFastAllocator::RFastAllocator(CFastMemoryPool* aPool) : iHeap( User::Heap() ), iPool( aPool )
{
    iHeapBase = (TUint32)iHeap.Base() & 0xffff0000;
#ifdef TRACK_ALLOCATIONS
    iNextCellId = 0;
#endif
}

EXPORT_C RFastAllocator::~RFastAllocator()
{
}


EXPORT_C TAny* RFastAllocator::Alloc(TInt aSize)
{
#ifdef TRACK_ALLOCATIONS
	return TraceAlloc(iPool, &aSize, &iNextCellId);
#else
    return iPool->Allocate( aSize );
#endif
}

EXPORT_C void RFastAllocator::Free(TAny* aPtr)
{
#ifdef TRACK_ALLOCATIONS
	TraceFree(&aPtr);
#endif	
    // make sure we go to the right memory pool
    if( IsLocatedInHeap( aPtr ) )
        iHeap.Free( aPtr );
    else
        iPool->Free( aPtr );
}

EXPORT_C TAny* RFastAllocator::ReAlloc(TAny* aPtr, TInt aSize, TInt aMode)
{
#ifdef TRACK_ALLOCATIONS
	TUint32 originalCellId;
	TBool issueNewCellId = TracePreRealloc(&aPtr, &aSize, originalCellId);
#endif
    TAny* p(NULL);
    TInt originalSize(0);
    
    // check the right memory pool
    if( IsLocatedInHeap( aPtr ) )
        {
        // allocate in memory manager's pool
        p = iPool->Allocate( aSize );
        TInt cpSize = iHeap.AllocLen( aPtr );
        originalSize = cpSize;
        cpSize = cpSize < aSize ? cpSize : aSize;
        memcpy( p, aPtr, cpSize );
        iHeap.Free( aPtr );
        }
    else
        {
        originalSize = iPool->MemorySize(aPtr);
        p = iPool->ReAllocate( aPtr, aSize );
        }

    // Handle aMode.
    // ENeverMove: Don't move memory on realloc, return NULL if it was moved.
    // EAllowMoveOnShrink: Allow moving memory on realloc when size requested
    //    is smaller, return NULL if moved and request size is greater (equal).
    if ( (aMode == ENeverMove && p != aPtr) ||
         (aMode == EAllowMoveOnShrink && originalSize >= aSize && p != aPtr) )
        {
        Free(p);
        return NULL;
        }
#ifdef TRACK_ALLOCATIONS
    TracePostRealloc(&p, aSize, &iNextCellId, originalCellId, issueNewCellId);
#endif
    return p;
}

EXPORT_C TInt RFastAllocator::AllocLen(const TAny* aCell) const
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

EXPORT_C TInt RFastAllocator::Compress()
{
    // DO NOTHING
    return 0;
}

EXPORT_C void RFastAllocator::Reset()
{
    // DO NOTHING
}

EXPORT_C TInt RFastAllocator::AllocSize(TInt& aTotalAllocSize) const
{
    // allocated cell size
    aTotalAllocSize = 0;
    return 0;
}

EXPORT_C TInt RFastAllocator::Available(TInt& aBiggestBlock) const
{
    // make sure the caller is not scared of futher allocation
    aBiggestBlock = KMaxTInt;
    return KMaxTUint;
}

EXPORT_C TInt RFastAllocator::DebugFunction(TInt, TAny*, TAny*)
{
    return 0;
}

EXPORT_C TInt RFastAllocator::Extension_(TUint, TAny*&, TAny*)
{
    return 0;
}

TBool RFastAllocator::IsLocatedInHeap( TAny* aPtr ) const
{
    // Quick check first, the initial heap size is usually defined as
    // 0x5000. For memory cell allocated in this heap, it's address 
    // share the same upper 16 bits with heap base address.
    if( ((TUint32)aPtr & 0xffff0000) != iHeapBase ) return EFalse;

    // very rare case, check it anyway
    TUint32 top = iHeapBase + iHeap.Size();
    return ( (TUint32)aPtr >= iHeapBase && (TUint32)aPtr <= top );
}

// END OF FILE
