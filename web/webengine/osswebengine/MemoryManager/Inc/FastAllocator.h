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
*/

#ifndef __FASTALLOCATOR_H__
#define __FASTALLOCATOR_H__

#include <e32base.h>
#include <e32cmn.h>

#ifdef TRACK_ALLOCATIONS
typedef struct {
	TInt requestedSize;
	TUint32 cellId; 
}THeapTrace;

TAny* GetRealPtrFromClientPtr(TAny* aClientPtr);
void  DecreaseSizeInPlace(TInt& aSize);

class CMemoryPool;
TAny* TraceAlloc(CMemoryPool* aPool, TInt* aSize, TUint32* aCellId);
void TraceFree(TAny **aPtr);
TBool TracePreRealloc(TAny** aPtr, TInt *aSize, TUint32& aOriginalCellId);
void TracePostRealloc(TAny** p, TInt aRequestedSize, TUint32* aNewCellId, TUint32 aOriginalCellId, TBool aIssueNewCellId);

#endif

class CFastMemoryPool;

class RFastAllocator : public RAllocator
{
public:
	IMPORT_C RFastAllocator(CFastMemoryPool*);
    IMPORT_C ~RFastAllocator();

	IMPORT_C TAny* Alloc(TInt aSize);
	IMPORT_C void Free(TAny* aPtr);
	IMPORT_C TAny* ReAlloc(TAny* aPtr, TInt aSize, TInt aMode=0);
	IMPORT_C TInt AllocLen(const TAny* aCell) const;
	IMPORT_C TInt Compress();
	IMPORT_C void Reset();
	IMPORT_C TInt AllocSize(TInt& aTotalAllocSize) const;
	IMPORT_C TInt Available(TInt& aBiggestBlock) const;
	IMPORT_C TInt DebugFunction(TInt aFunc, TAny* a1=NULL, TAny* a2=NULL);
	IMPORT_C TInt Extension_(TUint aExtensionId, TAny*& a0, TAny* a1);

private:
    TBool IsLocatedInHeap( TAny* aPtr ) const;

    RHeap& iHeap;
    TUint32 iHeapBase;
    CFastMemoryPool* iPool;
#ifdef TRACK_ALLOCATIONS
    TUint32 iNextCellId;
#endif
  };

class CNewSymbianHeapPool;

NONSHARABLE_CLASS(RSymbianDlAllocatorWrapper) : public RAllocator
{
public:
	RSymbianDlAllocatorWrapper(CNewSymbianHeapPool*);
    ~RSymbianDlAllocatorWrapper();

	TAny* Alloc(TInt aSize);
	void Free(TAny* aPtr);
	TAny* ReAlloc(TAny* aPtr, TInt aSize, TInt aMode=0);
	TInt AllocLen(const TAny* aCell) const;
	TInt Compress();
	void Reset();
	TInt AllocSize(TInt& aTotalAllocSize) const;
	TInt Available(TInt& aBiggestBlock) const;
	TInt DebugFunction(TInt aFunc, TAny* a1=NULL, TAny* a2=NULL);
	TInt Extension_(TUint aExtensionId, TAny*& a0, TAny* a1);

private:
    CNewSymbianHeapPool* iPool;
#ifdef TRACK_ALLOCATIONS
    TUint32 iNextCellId;
#endif
  };

#endif //!__FASTALLOCATOR_H__
