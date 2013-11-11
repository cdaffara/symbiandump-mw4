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


// INCLUDES

#include <e32base.h>
#include <e32std.h>
#include <e32hal.h>
#include "MemoryPool.h"
#include "StopScheduler.h"
#include "fast_malloc.h"
#include "SymbianDlHeap.h"
#include <oommonitorsession.h>
#include <hal.h>

// CONSTANTS

// CLASS DECLARATIONS

//-----------------------------------------------------------------------------
// CMemoryPool::AddCollector
//-----------------------------------------------------------------------------
void CMemoryPool::AddCollector( MMemoryCollector* aCollector )
    {
    // sort the collectors according to their priorities
    TInt i;
    for( i=0; i<iCollectors.Count(); ++i )
      {
      if( aCollector->Priority() <= iCollectors[i]->Priority() ) break;
      }

    iCollectors.Insert( aCollector, i );
    }

//-----------------------------------------------------------------------------
// CMemoryPool::RemoveCollector
//-----------------------------------------------------------------------------
void CMemoryPool::RemoveCollector( MMemoryCollector* aCollector )
    {
    TInt idx = iCollectors.Find( aCollector );
    if( idx != KErrNotFound )
        iCollectors.Remove( idx );
    }

//-----------------------------------------------------------------------------
// CMemoryPool::AddStopper
//-----------------------------------------------------------------------------
void CMemoryPool::AddStopper( MOOMStopper* aStopper )
    {
    if( !iStopScheduler )
        iStopScheduler = new CStopScheduler(*this);
    iStoppers.Append( aStopper );
    }

//-----------------------------------------------------------------------------
// CMemoryPool::RemoveStopper
//-----------------------------------------------------------------------------
void CMemoryPool::RemoveStopper( MOOMStopper* aStopper )
    {
	if( !iStopScheduler ) return;
    TInt idx = iStoppers.Find( aStopper );
    if( idx != KErrNotFound )
        iStoppers.Remove( idx );
    }

//-----------------------------------------------------------------------------
// CMemoryPool::SetNotifier
//-----------------------------------------------------------------------------
void CMemoryPool::SetNotifier( MOOMNotifier* aNotifier )
    {
    iNotifier = aNotifier;
    }

//-----------------------------------------------------------------------------
// CMemoryPool::Create()
//-----------------------------------------------------------------------------
TBool CMemoryPool::Create()
    {
    iNestedChecks = 0;

    iCollectors.Reset();
    iIsStopping = EFalse;
    iIsCollecting = EFalse;
    iMemStatus = ENoOOM;

    // stop scheduler
    iStopScheduler = 0;
    iNotifier = 0;

    return ETrue;
    }


//-----------------------------------------------------------------------------
// CMemoryPool::SetStopping
//-----------------------------------------------------------------------------
void CMemoryPool::SetStopping( TBool aStopping )
    {
    iIsStopping = aStopping;
    // all operations are stopped, must have some memory available
    if( !iIsStopping ) iMemStatus = ENoOOM;

    // notify the client when stopping is done
    if( !aStopping && iNotifier )
        {
        iNotifier->Notify();
        }
    }

//-----------------------------------------------------------------------------
// CMemoryPool::CollectMemory
//-----------------------------------------------------------------------------
void CMemoryPool::CollectMemory(TUint aSize)
    {
    if( iIsCollecting ) return;

    iIsCollecting = ETrue;
    for( TInt i=0; i<iCollectors.Count(); ++i )
      {
        iCollectors[i]->Collect(aSize);
      }

    User::CompressAllHeaps();
    iIsCollecting = EFalse;
	
	if (iStopScheduler)
		iStopScheduler->Start( CStopScheduler::ECheckMemory, aSize );
    }

//-----------------------------------------------------------------------------
// CMemoryPool::RestoreCollectors
//-----------------------------------------------------------------------------
void CMemoryPool::RestoreCollectors( TOOMPriority aPriority )
  {
  if( iIsCollecting ) return;

    for( TInt i=0; i<iCollectors.Count(); ++i )
      {
        if( iCollectors[i]->Priority() >= aPriority )
            iCollectors[i]->Restore();
      }
  }

//-----------------------------------------------------------------------------
// CMemoryPool::~CMemoryPool
//-----------------------------------------------------------------------------
CMemoryPool::~CMemoryPool()
    {
    iCollectors.Reset();
    iStoppers.Reset();
    delete iStopScheduler;

    // NOTE: remove this when UI spec has OOM notifier defined
    delete iNotifier;
    }

//-----------------------------------------------------------------------------
// CMemoryPool::AllocFromPool
//-----------------------------------------------------------------------------
TAny* CMemoryPool::AllocFromPool( TUint aSize )
    {
    // reset the status for next allocation
    iMemStatus &= ~ERescueOOM;

    TAny *p = DoAlloc( aSize );

    // check memory manager status
    if( !p || iMemStatus & ERescueOOM )
        {
        if( !iIsCollecting )
            {
            CollectMemory();
            }

        if( !p )
            p = DoAlloc( aSize );

        NotifyAndStop();
        }

    return p;
    }

//-----------------------------------------------------------------------------
// CMemoryPool::SetStatus
//-----------------------------------------------------------------------------
void CMemoryPool::SetStatus( TOOMCheckResult aType )
    {
    iMemStatus |= aType;
    }

//-----------------------------------------------------------------------------
// CMemoryPool::NotifyAndStop()
//-----------------------------------------------------------------------------
void CMemoryPool::NotifyAndStop()
  {
  if( !iIsStopping )
    {
        iIsStopping = ETrue;
		if (iStopScheduler)
			iStopScheduler->Start( CStopScheduler::EStopLoading, 0 );
    }
  }

//-----------------------------------------------------------------------------
// CPlainAllocator::DoAlloc
//-----------------------------------------------------------------------------
TUint CFastMemoryPool::FreeMemory(TFreeMem& aFree )
    {
    return free_memory( aFree.iPool, aFree.iHeap, aFree.iHal );
    }

//-----------------------------------------------------------------------------
// CFastMemoryPool::DoAlloc
//-----------------------------------------------------------------------------
TAny* CFastMemoryPool::DoAlloc( TUint aSize )
    {
    return fast_malloc( aSize );
    }

//-----------------------------------------------------------------------------
// CFastMemoryPool::ReAllocate
//-----------------------------------------------------------------------------
TAny* CFastMemoryPool::ReAllocate( TAny* aPtr, TUint aSize )
    {
    // reset the status for next allocation
    iMemStatus &= ~ERescueOOM;

    TAny* p = fast_realloc( aPtr, aSize );

    // check memory manager status
    if( !p || iMemStatus & ERescueOOM )
        {
        if( !iIsCollecting )
            {
            CollectMemory();
            }

        if( !p )
            p = fast_realloc( aPtr, aSize );

        NotifyAndStop();
        }

    return p;
    }

//-----------------------------------------------------------------------------
// CFastMemoryPool::PreCheck
//-----------------------------------------------------------------------------
TBool CFastMemoryPool::PreCheck( TUint aTotalSize, TUint aMaxBufSize, const TDesC8& /*aCheckerName*/ )
    {
    // avoid small checkings
    if( aTotalSize < 1024 ) return ETrue;

    if( !fast_pre_check( aTotalSize, aMaxBufSize ) )
        {
        CollectMemory(aTotalSize);
        ROomMonitorSession oomMs;
        if ( oomMs.Connect() == KErrNone ) {
            oomMs.RequestFreeMemory( aTotalSize );
            oomMs.Close();
        }
        if( !fast_pre_check( aTotalSize, aMaxBufSize ) )
            {
            iMemStatus |= ECheckOOM;
            NotifyAndStop();
            return EFalse;
            }
        }

    return ETrue;
    }

//-----------------------------------------------------------------------------
// CMemoryPool::PostCheck
//-----------------------------------------------------------------------------
TUint CFastMemoryPool::PostCheck()
    {
    fast_post_check();
    return iMemStatus;
    }

//-----------------------------------------------------------------------------
// CPlainAllocator::Free
//-----------------------------------------------------------------------------
void CFastMemoryPool::Free( TAny* aPtr )
    {
    return fast_free( aPtr );
    }

//-----------------------------------------------------------------------------
// CPlainAllocator::MemorySize
//-----------------------------------------------------------------------------
TUint CFastMemoryPool::MemorySize( TAny* aPtr )
    {
    return fast_malloc_size( aPtr );
    }

//-----------------------------------------------------------------------------
// CPlainAllocator::SetRescueBufferSize
//-----------------------------------------------------------------------------
void CFastMemoryPool::SetRescueBufferSize( TInt aSize )
    {
    fast_set_rescue_buffer_size( aSize );
    }

void CFastMemoryPool::RestoreRescueBuffer()
    {
    alloc_rescue_buffer();
    }

//-----------------------------------------------------------------------------
// CDefaultMemoryPool::~CDefaultMemoryPool
//-----------------------------------------------------------------------------
CDefaultMemoryPool::~CDefaultMemoryPool()
    {
    }

//-----------------------------------------------------------------------------
// CDefaultMemoryPool::DoAlloc
//-----------------------------------------------------------------------------
TAny* CDefaultMemoryPool::DoAlloc(TUint aSize)
    {
    return User::Alloc( aSize );
    }

//-----------------------------------------------------------------------------
// CDefaultMemoryPool::ReAllocate
//-----------------------------------------------------------------------------
TAny* CDefaultMemoryPool::ReAllocate( TAny* aPtr, TUint aSize )
    {   
    return User::ReAlloc( aPtr, aSize );
    }

//-----------------------------------------------------------------------------
// CDefaultMemoryPool::FreeMemory
//-----------------------------------------------------------------------------
TUint CDefaultMemoryPool::FreeMemory(TFreeMem& /*aFree*/)
    {
    // free memory in Hal
    TMemoryInfoV1Buf info;
    UserHal::MemoryInfo( info );
    return info().iFreeRamInBytes;
    }

//-----------------------------------------------------------------------------
// CDefaultMemoryPool::Free
//-----------------------------------------------------------------------------
void CDefaultMemoryPool::Free(TAny* aPtr)
    {
    User::Free( aPtr );
    }

//-----------------------------------------------------------------------------
// CDefaultMemoryPool::MemorySize
//-----------------------------------------------------------------------------
TUint CDefaultMemoryPool::MemorySize(TAny* aPtr)
    {
    return User::AllocLen( aPtr );
    }

//-----------------------------------------------------------------------------
// CDefaultMemoryPool::SetRescueBufferSize
//-----------------------------------------------------------------------------
void CDefaultMemoryPool::SetRescueBufferSize( TInt aSize )
    {
    iRescueBufferSize = aSize;
    }

//-----------------------------------------------------------------------------
// CDefaultMemoryPool::PreCheck
//-----------------------------------------------------------------------------
TBool CDefaultMemoryPool::PreCheck( TUint aTotalSize, TUint /*aMaxBufSize*/, const TDesC8& /*aCheckerName*/ )
    {
    // avoid small checkings
    if( aTotalSize < 1024 ) return ETrue;

	// first check the application heap's max free block
	TInt maxFreeBlock = 0;
	User::Allocator().Available(maxFreeBlock);
	if (aTotalSize < maxFreeBlock)
		return ETrue;
	

    // free memory in Hal
    TMemoryInfoV1Buf info;
    UserHal::MemoryInfo( info );
    TInt sizeNeeded = aTotalSize + iRescueBufferSize;

	// check if there is enough space for the heap to grow
	bool needSysCheck = EFalse;
	if (info().iFreeRamInBytes > sizeNeeded) {
		RHeap& heap = User::Heap();
		if (heap.MaxLength() - heap.Size() > sizeNeeded)
			return ETrue;

		needSysCheck = ETrue;
	}

	if(sizeNeeded > info().iFreeRamInBytes || needSysCheck)
        {
        CollectMemory(sizeNeeded);
        
        // check memory again
        UserHal::MemoryInfo( info );

        if(sizeNeeded > info().iFreeRamInBytes ) 
            {
            NotifyAndStop();
            iMemStatus |= ECheckOOM;
            return EFalse;
            }
        }

    return ETrue;
    }

//-----------------------------------------------------------------------------
// CDefaultMemoryPool::PostCheck
//-----------------------------------------------------------------------------
TUint CDefaultMemoryPool::PostCheck()
    {
    return iMemStatus;
    }

void CDefaultMemoryPool::RestoreRescueBuffer()
    {
    // do nothing here.
    }

#ifdef __NEW_ALLOCATOR__
//-----------------------------------------------------------------------------
// CNewSymbianHeapPool::FreeMemory
//-----------------------------------------------------------------------------
TUint CNewSymbianHeapPool::FreeMemory(TFreeMem& /*aFree*/ )
    {
    // TODO: implement free_memory
    return KMaxTUint;
//    return free_memory( aFree.iPool, aFree.iHeap, aFree.iHal );
    }

//-----------------------------------------------------------------------------
// CNewSymbianHeapPool::DoAlloc
//-----------------------------------------------------------------------------
TAny* CNewSymbianHeapPool::DoAlloc( TUint aSize )
    {
    return iAlloc->Alloc( aSize );
    }

//-----------------------------------------------------------------------------
// CNewSymbianHeapPool::ReAllocate
//-----------------------------------------------------------------------------
TAny* CNewSymbianHeapPool::ReAllocate( TAny* aPtr, TUint aSize )
    {
    // reset the status for next allocation
    iMemStatus &= ~ERescueOOM;

    TAny* p = iAlloc->ReAlloc( aPtr, aSize );
    
    // check memory manager status
    if( !p || iMemStatus & ERescueOOM )
        {
        if( !iIsCollecting )
            {
            CollectMemory();
            }

        if( !p )
			p = iAlloc->ReAlloc( aPtr, aSize );

        NotifyAndStop();
        }

    return p;
    }

//-----------------------------------------------------------------------------
// CNewSymbianHeapPool::PreCheck
//-----------------------------------------------------------------------------
TBool CNewSymbianHeapPool::PreCheck(TUint aTotalSize, TUint aMaxBufSize,
		const TDesC8& /*aCheckerName*/)
	{
	/* aTotalSize - total amount desired
	 * aMaxBufSize - largest single allocation desired
	 */

	// avoid small checkings
	if (aTotalSize < 1024)
		return ETrue;

	/*
	 * Browser fast_malloc implementation merges together total/max allocation requests and says YES if system free memory is >6MB
	 */
	// mirror existing size calculation
	aTotalSize = aTotalSize > (aMaxBufSize * 2) ? aTotalSize
			: (aMaxBufSize * 2);

	// if we have more than 6 MB of free mem, we 
	// should skip malloc_info call.
	// free memory in Hal
    TInt systemFreeMemory = 0;
	if( HAL::Get(HALData::EMemoryRAMFree, systemFreeMemory) == KErrNone )
		{
		const TUint checkThreshold = 6*1024*1024;
		if(systemFreeMemory> checkThreshold && systemFreeMemory> aTotalSize)
		return ETrue;
		}

	/* For a proper implementation of RHeap, the stats available are:
	 * AllocSize	Total number of allocated bytes
	 * Size			Total number of bytes held from system
	 * Available	Max block size and amount of bytes held but not allocated
	 */
	/* We don't have those bits of the heap implemented yet
	 */ 
	TInt freeSpace = 0; // be conservative, use 0 intead of 64<<10;
	TInt req = freeSpace + systemFreeMemory - aTotalSize;	
	
	if(req > 0)
		return ETrue;
	
	// First, ask system for memory from other applications.
	ROomMonitorSession oomMs;
	if (oomMs.Connect() == KErrNone)
		{
		oomMs.RequestFreeMemory(aTotalSize);
		oomMs.Close();
		}

	// We haven't altered our heap yet, so ask the system how much is free now...
	HAL::Get(HALData::EMemoryRAMFree, systemFreeMemory);
	req = freeSpace + systemFreeMemory - aTotalSize;	
	
	if(req > 0)
		return ETrue;
	
	// We haven't got the required amount free yet, try the browser heap.
	CollectMemory(aTotalSize);
	// ask the system how much is free now...
	HAL::Get(HALData::EMemoryRAMFree, systemFreeMemory);
	req = freeSpace + systemFreeMemory - aTotalSize;	
	
	// if we still haven't got enough RAM, we should stop the browser from going any further just yet.
	if (req < 0)
		{
		iMemStatus |= ECheckOOM;
		NotifyAndStop();
		return EFalse;
		}
	
	return ETrue;
	}

//-----------------------------------------------------------------------------
// CNewSymbianHeapPool::PostCheck
//-----------------------------------------------------------------------------
TUint CNewSymbianHeapPool::PostCheck()
    {
    return iMemStatus;
    }

//-----------------------------------------------------------------------------
// CNewSymbianHeapPool::Free
//-----------------------------------------------------------------------------
void CNewSymbianHeapPool::Free( TAny* aPtr )
    {
    return iAlloc->Free( aPtr );
    }

//-----------------------------------------------------------------------------
// CNewSymbianHeapPool::MemorySize
//-----------------------------------------------------------------------------
TUint CNewSymbianHeapPool::MemorySize( TAny* aPtr )
    {
    return iAlloc->AllocLen( aPtr );
    }

//-----------------------------------------------------------------------------
// CNewSymbianHeapPool::SetRescueBufferSize
//-----------------------------------------------------------------------------
void CNewSymbianHeapPool::SetRescueBufferSize( TInt /*aSize*/ )
    {
    //fast_set_rescue_buffer_size( aSize );
    }

//-----------------------------------------------------------------------------
// CNewSymbianHeapPool::RestoreRescueBuffer
//-----------------------------------------------------------------------------
void CNewSymbianHeapPool::RestoreRescueBuffer()
    {
    //alloc_rescue_buffer();
    }

CNewSymbianHeapPool::CNewSymbianHeapPool() : CMemoryPool()
	{
	}

CNewSymbianHeapPool::~CNewSymbianHeapPool()
	{
//	iAlloc->Close();	// TODO: Need to clean up here, but it's not implemented in the allocator yet.
						// Not fatal anyway because all the handles are process local and we should only
						// be deleting this object when we're closing the allocator - which we never do
						// except at process end.
	}

#ifdef __WINSCW__
const TInt KMaxHeapSize = 0x2000000; // 32MB, on emulator
#else
const TInt KMaxHeapSize = 0x4000000; // 64MB, on hardware
#endif

const TInt KHeapGrowSize = 0x10000;  // 64KB

TBool CNewSymbianHeapPool::Create()
	{
	// need to know system page size
	TInt page_size;
	UserHal::PageSizeInBytes(page_size);
	
    TInt maxmem = 0;
    HAL::Get(HALData::EMemoryRAM, maxmem);
    TInt maxHeapSize = Min(maxmem, KMaxHeapSize);
        
	/* Create the thread's heap chunk.
	 * The chunk needs reserve enough address space for twice the maximum allocation
	 * The heap object is instantiated exactly half way up the chunk, and initially is provided just 1 page of memory
	 * This memory can be committed as part of the call to creat the chunk
	 */
	TInt maxChunkSize = 2 * maxHeapSize;
	TInt offset = maxHeapSize;
	TInt minLength = page_size;
	RChunk c;
	TInt r = c.CreateDisconnectedLocal(offset, offset+minLength, maxChunkSize, EOwnerProcess);
	if (r!=KErrNone)
		return EFalse;
	
	iAlloc = new (c.Base() + offset) RSymbianDLHeap(c.Handle(), offset, minLength, maxHeapSize, KHeapGrowSize, 8, EFalse /* not single threaded! */);
	iAlloc->iHandles = &iAlloc->iChunkHandle;
	iAlloc->iHandleCount = 2;
	// chunk handle now 'owned' by iAlloc
	r = iAlloc->iLock.CreateLocal(EOwnerProcess);

	if(r != KErrNone)
		return EFalse;
	
	return CMemoryPool::Create();
	}

#ifdef OOM_LOGGING   
void CNewSymbianHeapPool::DumpHeapLogs()
    {
    iAlloc->dump_heap_logs(0);
    iAlloc->dump_dl_free_chunks();
    }
#endif
#endif
// END OF FILE
