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

#include "common.h"

#ifdef TRACE_CHUNK_USAGE
void TraceChunkUsage(TInt aChunkHandle, TUint8* aBase, TInt aChunkSize)
	{
	RDebug::Print(_L("MEM: c,%d,%d,%d"), aChunkHandle, aBase, aChunkSize);
	}
#else
#define TraceChunkUsage(a,b,c)
#endif

#ifdef __NEW_ALLOCATOR__

#include "MemoryLogger.h"
#include "SymbianDlHeap.h"

_LIT(KDLHeapPanicCategory, "DL Heap");
#define	GET_PAGE_SIZE(x)			UserHal::PageSizeInBytes(x)
#define	__CHECK_CELL(p)
#define __POWER_OF_2(x)				((TUint32)((x)^((x)-1))>=(TUint32)(x))

#define gm  (&iGlobalMallocState)

void Panic(TCdtPanic aPanic)
	{
	User::Panic(KDLHeapPanicCategory, aPanic);
	}

#undef UEXPORT_C
#define UEXPORT_C 


/* Purpose:     Map chunk memory pages from system RAM
 * Arguments:   tp - tchunkptr in which memmory should be mapped
 *              psize - incoming tchunk size
 * Return:      KErrNone if successful, else KErrNoMemory  
 * Note:        
 */
TInt RSymbianDLHeap::map_chunk_pages(tchunkptr tp, size_t psize)
{
    if(page_not_in_memory(tp, psize)) {
        char *a_addr = tchunk_page_align(tp);
        size_t npages = tp->npages;
    
#ifdef OOM_LOGGING
        // check that npages matches the psize
        size_t offset = address_offset(a_addr,tp);        
        if(offset < psize && (psize - offset) >= mparams.page_size )
        {               
            size_t tpages = ( psize - offset) >> pageshift;            
            if(tpages != tp->npages) //assert condition                
                MEM_LOG("CHUNK_PAGE_ERROR:map_chunk_pages, error in npages");                        
        }
        else
            MEM_LOG("CHUNK_PAGE_ERROR::map_chunk_pages: - Incorrect page-in-memmory flag");
#endif        
    
        if(map(a_addr, npages*mparams.page_size)) {        
            TRACE_DL_CHUNK_MAP(tp, psize, a_addr, npages*mparams.page_size);
            ASSERT_RCHUNK_SIZE();
            TRACE_UNMAPPED_CHUNK(-1*npages*mparams.page_size);
            return KErrNone;
        }
        else { 
            MEM_LOGF(_L8("CHUNK_PAGE_ERROR:: map_chunk_pages - Failed to Commit RAM, page_addr=%x, npages=%d, chunk_size=%d"), a_addr, npages, psize);
            MEM_DUMP_OOM_LOGS(psize, "RSymbianDLHeap::map_chunk_pages - Failed to Commit RAM");
            return KErrNoMemory;
        }
    }
    return KErrNone;
}

/* Purpose:     Map partial chunk memory pages from system RAM
 * Arguments:   tp - tchunkptr in which memmory should be mapped
 *              psize - incoming tchunk size
 *              r - remainder chunk pointer
 *              rsize - remainder chunk size  
 * Return:      Number of unmapped pages from remainder chunk if successful (0 or more), else KErrNoMemory
 * Note:        Remainder chunk should be large enough to be mapped out (checked before invoking this function)    
 *              pageout headers will be set from insert_large_chunk(), not here.
 */
TInt RSymbianDLHeap::map_chunk_pages_partial(tchunkptr tp, size_t psize, tchunkptr r, size_t rsize)
{
    if(page_not_in_memory(tp, psize)) {
        size_t npages = tp->npages; // total no of pages unmapped in this chunk        
        char *page_addr_map = tchunk_page_align(tp); // address to begin page map
        char *page_addr_rem = tchunk_page_align(r);  // address in remainder chunk to remain unmapped
        assert(address_offset(page_addr_rem, r) < rsize);
        size_t npages_map = address_offset(page_addr_rem, page_addr_map) >> pageshift; // no of pages to be mapped
        if(npages_map > 0) {
            if(map(page_addr_map, npages_map*mparams.page_size)) {            
                TRACE_DL_CHUNK_MAP(tp, psize, page_addr_map, npages_map*mparams.page_size);
                ASSERT_RCHUNK_SIZE();
                TRACE_UNMAPPED_CHUNK(-1*npages_map*mparams.page_size);    
                return (npages - npages_map);
            }
            else { 
                MEM_LOGF(_L8("CHUNK_PAGE_ERROR:: map_chunk_pages_partial - Failed to Commit RAM, page_addr=%x, npages=%d, chunk_size=%d"), page_addr_map, npages_map, psize);
                MEM_DUMP_OOM_LOGS(psize, "RSymbianDLHeap::map_chunk_pages_partial - Failed to Commit RAM");
                return KErrNoMemory;
            }              
        }
        else {
             // map not needed, first page is already mapped
             return npages;
        }
    }
    
    return 0;
}


/* Purpose:     Release (unmap) chunk memory pages to system RAM
 * Arguments:   tp - tchunkptr from which memmory may be released
 *              psize - incoming tchunk size
 *              prev_npages - number of pages that has been already unmapped from this chunk
 * Return:      total number of pages that has been unmapped from this chunk (new unmapped pages + prev_npages)
 * Note:        pageout headers will be set from insert_large_chunk(), not here.    
 */
TInt RSymbianDLHeap::unmap_chunk_pages(tchunkptr tp, size_t psize, size_t prev_npages)
{
    size_t npages = 0;
    char *a_addr = tchunk_page_align(tp);
    size_t offset = address_offset(a_addr,tp);    
    if(offset < psize && (psize - offset) >= mparams.page_size)
    { /* check for new pages to decommit */
        npages = ( psize - offset) >> pageshift;
        if(npages > prev_npages) {
            unmap(a_addr, npages*mparams.page_size);    // assuming kernel takes care of already unmapped pages
            TRACE_DL_CHUNK_UNMAP(tp, psize, a_addr, npages*mparams.page_size);
            iChunkSize += prev_npages*mparams.page_size; //adjust actual chunk size
            ASSERT_RCHUNK_SIZE();
            TRACE_UNMAPPED_CHUNK((npages-prev_npages)*mparams.page_size);
            assert((a_addr + npages*mparams.page_size - 1) < (char*)next_chunk(tp));
        }        
    }

#ifdef OOM_LOGGING        
    if(npages && (npages < prev_npages))
        MEM_LOG("CHUNK_PAGE_ERROR:unmap_chunk_pages, error in npages");    
    if(npages > prev_npages) {
        /* check that end of decommited address lie within this chunk */      
        if((a_addr + npages*mparams.page_size - 1) >= (char*)next_chunk(tp))
            MEM_LOG("CHUNK_PAGE_ERROR:unmap_chunk_pages, error chunk boundary");
    }
#endif
#ifdef DL_CHUNK_MEM_DEBUG 
    mchunkptr next = next_chunk(tp);
    do_check_any_chunk_access(next, chunksize(next));
    if(!npages)  do_check_any_chunk_access((mchunkptr)tp, psize);
#endif
   
    return (npages);
}

/* Purpose:     Unmap all pages between previously unmapped and end of top chunk 
                and reset top to beginning of prev chunk
 * Arguments:   fm - global malloc state
 *              prev - previous chunk which has unmapped pages
 *              psize - size of previous chunk
 *              prev_npages - number of unmapped pages from previous chunk
 * Return:      nonzero if sucessful, else 0
 * Note:                    
 */
TInt RSymbianDLHeap::sys_trim_partial(mstate m, mchunkptr prev, size_t psize, size_t prev_npages)
{
    size_t released = 0;
    size_t extra = 0;
    if (is_initialized(m)) {
      psize += m->topsize;
      char *a_addr = tchunk_page_align(prev); // includes space for TOP footer 
      size_t addr_offset = address_offset(a_addr, prev);
      assert(addr_offset > TOP_FOOT_SIZE); //always assert?
      assert((char*)iTop >= a_addr); //always assert?
      if((char*)iTop > a_addr)
          extra = address_offset(iTop, a_addr);

#ifdef OOM_LOGGING      
      if ((char*)iTop < a_addr)
          MEM_LOGF(_L8("RSymbianDLHeap::sys_trim_partial - incorrect iTop value, top=%x, iTop=%x"), m->top, iTop);
#endif            
        msegmentptr sp = segment_holding(m, (TUint8*)prev);
        if (!is_extern_segment(sp)) {
          if (is_mmapped_segment(sp)) {
            if (HAVE_MMAP &&  sp->size >= extra && !has_segment_link(m, sp)) { /* can't shrink if pinned */
              size_t newsize = sp->size - extra;
              /* Prefer mremap, fall back to munmap */
              if ((CALL_MREMAP(sp->base, sp->size, newsize, 0) != MFAIL) ||
                  (CALL_MUNMAP(sp->base + newsize, extra) == 0)) {
                released = extra;
              }
            }
          }
          else if (HAVE_MORECORE) {
            if (extra >= HALF_MAX_SIZE_T) /* Avoid wrapping negative */
                extra = (HALF_MAX_SIZE_T) + SIZE_T_ONE - mparams.granularity;
            ACQUIRE_MORECORE_LOCK(m);
            {
              /* Make sure end of memory is where we last set it. */
              TUint8* old_br = (TUint8*)(CALL_MORECORE(0));
              if (old_br == sp->base + sp->size) {
                TUint8* rel_br = (TUint8*)(CALL_MORECORE(-extra));
                TUint8* new_br = (TUint8*)(CALL_MORECORE(0));
                if (rel_br != CMFAIL && new_br < old_br)
                  released = old_br - new_br;
              }
            }
            RELEASE_MORECORE_LOCK(m);
          }
        }
        
        if (released != 0) {
          TRACE_DL_CHUNK_UNMAP(prev, psize, a_addr, released);
          iChunkSize += prev_npages*mparams.page_size; // prev_unmapped was already unmapped
          TRACE_UNMAPPED_CHUNK(-1*prev_npages*mparams.page_size);
          ASSERT_RCHUNK_SIZE();
          sp->size -= released;
          m->footprint -= released;
        }
        
        /* reset top to prev chunk */
        init_top(m, prev, addr_offset - TOP_FOOT_SIZE);
        check_top_chunk(m, m->top);
    }

    // DL region not initalized, do not reset top here
    return (released != 0)? 1 : 0;
}


UEXPORT_C RSymbianDLHeap::RSymbianDLHeap(TInt aMaxLength, TInt aAlign, TBool aSingleThread)
// constructor for a fixed heap. Just use DL allocator
	:iMinLength(aMaxLength), iMaxLength(aMaxLength), iOffset(0), iGrowBy(0), iChunkHandle(0),
	iNestingLevel(0), iAllocCount(0), iFailType(ENone), iTestData(NULL), iChunkSize(aMaxLength)
	{

	// bodge so GKIServ (hudson generic low level layer) starts up ok - it uses an aAlign of 0 which panics, so if see 0 then force to 4
	if ((TUint32)aAlign>=sizeof(TAny*) && __POWER_OF_2(iAlign))
		{
		iAlign = aAlign;
		}
	else
		{
		iAlign = 4;
		}
	iPageSize = 0;
	iFlags = aSingleThread ? (ESingleThreaded|EFixedSize) : EFixedSize;

	Init(0, 0, 0);
	}

UEXPORT_C RSymbianDLHeap::RSymbianDLHeap(TInt aChunkHandle, TInt aOffset, TInt aMinLength, TInt aMaxLength, TInt aGrowBy,
			TInt aAlign, TBool aSingleThread)
		: iMinLength(aMinLength), iMaxLength(aMaxLength), iOffset(aOffset), iChunkHandle(aChunkHandle), iNestingLevel(0), iAllocCount(0),
			iAlign(aAlign),iFailType(ENone), iTestData(NULL), iChunkSize(aMinLength)
	{
	// TODO: Locked the page size to 4 KB - change this to pick up from the OS
	GET_PAGE_SIZE(iPageSize);
	__ASSERT_ALWAYS(aOffset >=0, User::Panic(KDLHeapPanicCategory, ETHeapNewBadOffset));
	iGrowBy = _ALIGN_UP(aGrowBy, iPageSize);
	iFlags = aSingleThread ? ESingleThreaded : 0;

	// Initialise
	// if the heap is created with aMinLength==aMaxLength then it cannot allocate slab or page memory
	// so these sub-allocators should be disabled. Otherwise initialise with default values
	if (aMinLength == aMaxLength)
		Init(0, 0, 0);
	else
		Init(0x3fff, 15, 0x10000);	// all slabs, page {32KB}, trim {64KB}
//		Init(0xabe, 16, iPageSize*4);	// slabs {48, 40, 32, 24, 20, 16, 12}, page {64KB}, trim {16KB}
	}

UEXPORT_C TAny* RSymbianDLHeap::operator new(TUint aSize, TAny* aBase) __NO_THROW
	{
	__ASSERT_ALWAYS(aSize>=sizeof(RSymbianDLHeap), HEAP_PANIC(ETHeapNewBadSize));
	RSymbianDLHeap* h = (RSymbianDLHeap*)aBase;
	h->iAlign = 0x80000000;	// garbage value
	h->iBase = ((TUint8*)aBase) + aSize;
	return aBase;
	}

void RSymbianDLHeap::Init(TInt aBitmapSlab, TInt aPagePower, size_t aTrimThreshold)
	{
	__ASSERT_ALWAYS((TUint32)iAlign>=sizeof(TAny*) && __POWER_OF_2(iAlign), HEAP_PANIC(ETHeapNewBadAlignment));

	/*Moved code which does iunitilization */
	iTop = (TUint8*)this + iMinLength;
	spare_page = 0;
	memset(&mparams,0,sizeof(mparams));

	Init_Dlmalloc(iTop - iBase, 0, aTrimThreshold);

	slab_init(aBitmapSlab);

	/*10-1K,11-2K,12-4k,13-8K,14-16K,15-32K,16-64K*/
	paged_init(aPagePower);

#ifdef OOM_LOGGING    
    iUnmappedChunkSize = 0;
#endif    
	}

UEXPORT_C RSymbianDLHeap::SCell* RSymbianDLHeap::GetAddress(const TAny* aCell) const
//
// As much as possible, check a cell address and backspace it
// to point at the cell header.
//
	{

	TLinAddr m = TLinAddr(iAlign - 1);
	__ASSERT_ALWAYS(!(TLinAddr(aCell)&m), HEAP_PANIC(ETHeapBadCellAddress));

	SCell* pC = (SCell*)(((TUint8*)aCell)-EAllocCellSize);
	__CHECK_CELL(pC);

	return pC;
	}

UEXPORT_C TInt RSymbianDLHeap::AllocLen(const TAny* aCell) const
{
	if (ptrdiff(aCell, this) >= 0)
	{
		mchunkptr m = mem2chunk(aCell);
		return chunksize(m) - CHUNK_OVERHEAD;
	}
	if (lowbits(aCell, pagesize) > cellalign)
		return header_size(slab::slabfor(aCell)->header);
	if (lowbits(aCell, pagesize) == cellalign)
		return *(unsigned*)(offset(aCell,-int(cellalign)))-cellalign;
	return paged_descriptor(aCell)->size;
}

UEXPORT_C TAny* RSymbianDLHeap::Alloc(TInt aSize)
{
	__ASSERT_ALWAYS((TUint)aSize<(KMaxTInt/2),HEAP_PANIC(ETHeapBadAllocatedCellSize));

	TAny* addr;

	Lock();
	if (aSize < slab_threshold)
	{
		TInt ix = sizemap[(aSize+3)>>2];
		ASSERT(ix != 0xff);
		addr = slab_allocate(slaballoc[ix]);
	}else if((aSize >> page_threshold)==0)
		{
		addr = dlmalloc(aSize);
		}
	else
		{
		addr = paged_allocate(aSize);
		if(!addr) { // paged_allocator failed, try in dlmalloc 
            addr = dlmalloc(aSize);
		}
		}

	Unlock();

	return addr;
}

UEXPORT_C TInt RSymbianDLHeap::Compress()
	{
	if (iFlags & EFixedSize)
		return 0;

	Lock();
	dlmalloc_trim(0);
	if (spare_page)
	{
		unmap(spare_page, pagesize);
		spare_page = 0;
	}
	Unlock();
	return 0;
	}

UEXPORT_C void RSymbianDLHeap::Free(TAny* aPtr)
{

	Lock();

	if (!aPtr)
		;
	else if (ptrdiff(aPtr, this) >= 0)
		{
		dlfree( aPtr);
		}
	else if (lowbits(aPtr, pagesize) <= cellalign)
		{
		paged_free(aPtr);
		}
	else
		{
		slab_free(aPtr);
		}
	Unlock();
}


UEXPORT_C void RSymbianDLHeap::Reset()
	{
	// TODO free everything
	}

UEXPORT_C TAny* RSymbianDLHeap::ReAlloc(TAny* aPtr, TInt aSize, TInt /*aMode = 0*/)
//
// Support aMode==0 properly: do not move the cell if the request is smaller than old size
//
	{
	if(ptrdiff(aPtr,this)>=0)
	{
		// original cell is in DL zone
		if((aSize>>page_threshold)==0 || aSize <= chunksize(mem2chunk(aPtr)) - CHUNK_OVERHEAD)
			{
			// new one is below page limit or smaller than old one (so can't be moved)
			Lock();
			TAny* addr = dlrealloc(aPtr,aSize);
			Unlock();
			return addr;
			}
	}
	else if(lowbits(aPtr,pagesize)<=cellalign)
	{
		// original cell is either NULL or in paged zone
		if (!aPtr)
			return Alloc(aSize);

		// either the new size is larger (in which case it will still be in paged zone)
		// or it is smaller, but we will never move a shrinking cell so in paged zone
		// must handle [rare] case that aSize == 0, as paged_[re]allocate() will panic
		if (aSize == 0)
			aSize = 1;
		Lock();
		TAny* addr = paged_reallocate(aPtr,aSize);
		Unlock();
		return addr;
	}
	else
	{
		// original cell is in slab zone
		// return original if new one smaller
		if(aSize <= header_size(slab::slabfor(aPtr)->header))
			return aPtr;
	}
	// can't do better than allocate/copy/free
	TAny* newp = Alloc(aSize);
	if(newp)
	{
		TInt oldsize = AllocLen(aPtr);
		memcpy(newp,aPtr,oldsize<aSize?oldsize:aSize);
		Free(aPtr);
	}
	return newp;
	}

UEXPORT_C TInt RSymbianDLHeap::Available(TInt& aBiggestBlock) const
{
	aBiggestBlock = 0;
	return 1000;
	/*Need to see how to implement this*/
	// TODO: return iHeap.Available(aBiggestBlock);
}
UEXPORT_C TInt RSymbianDLHeap::AllocSize(TInt& aTotalAllocSize) const
{
	aTotalAllocSize = 0;
	return 0;
}

UEXPORT_C TInt RSymbianDLHeap::DebugFunction(TInt /*aFunc*/, TAny* /*a1*/, TAny* /*a2*/)
	{
	return 0;
	}
UEXPORT_C TInt RSymbianDLHeap::Extension_(TUint /* aExtensionId */, TAny*& /* a0 */, TAny* /* a1 */)
	{
	return KErrNotSupported;
	}

long    sysconf		(int size )
	{
	if (UserHal::PageSizeInBytes(size)!=KErrNone)
		size = 0x1000;
	return size;
	}


///////////////////////////////////////////////////////////////////////////////
// imported from dla.cpp
///////////////////////////////////////////////////////////////////////////////

//#include <unistd.h>
//#define DEBUG_REALLOC
#ifdef DEBUG_REALLOC
#include <e32debug.h>
#endif
inline int RSymbianDLHeap::init_mparams(size_t aTrimThreshold /*= DEFAULT_TRIM_THRESHOLD*/)
{
	if (mparams.page_size == 0)
	{
		size_t s;
		mparams.mmap_threshold = DEFAULT_MMAP_THRESHOLD;
		mparams.trim_threshold = aTrimThreshold;
		#if MORECORE_CONTIGUOUS
			mparams.default_mflags = USE_LOCK_BIT|USE_MMAP_BIT;
		#else  /* MORECORE_CONTIGUOUS */
			mparams.default_mflags = USE_LOCK_BIT|USE_MMAP_BIT|USE_NONCONTIGUOUS_BIT;
		#endif /* MORECORE_CONTIGUOUS */

			s = (size_t)0x58585858U;
		ACQUIRE_MAGIC_INIT_LOCK(&mparams);
		if (mparams.magic == 0) {
		  mparams.magic = s;
		  /* Set up lock for main malloc area */
		  INITIAL_LOCK(&gm->mutex);
		  gm->mflags = mparams.default_mflags;
		}
		RELEASE_MAGIC_INIT_LOCK(&mparams);

		// DAN replaced
		// mparams.page_size = malloc_getpagesize;
		int temp = 0;
		GET_PAGE_SIZE(temp);
		mparams.page_size = temp;

		mparams.granularity = ((DEFAULT_GRANULARITY != 0)?
							   DEFAULT_GRANULARITY : mparams.page_size);

		/* Sanity-check configuration:
		   size_t must be unsigned and as wide as pointer type.
		   ints must be at least 4 bytes.
		   alignment must be at least 8.
		   Alignment, min chunk size, and page size must all be powers of 2.
		*/

		if ((sizeof(size_t) != sizeof(TUint8*)) ||
			(MAX_SIZE_T < MIN_CHUNK_SIZE)  ||
			(sizeof(int) < 4)  ||
			(MALLOC_ALIGNMENT < (size_t)8U) ||
			((MALLOC_ALIGNMENT    & (MALLOC_ALIGNMENT-SIZE_T_ONE))    != 0) ||
			((MCHUNK_SIZE         & (MCHUNK_SIZE-SIZE_T_ONE))         != 0) ||
			((mparams.granularity & (mparams.granularity-SIZE_T_ONE)) != 0) ||
			((mparams.page_size   & (mparams.page_size-SIZE_T_ONE))   != 0))
		  ABORT;
	}
	return 0;
}

inline void RSymbianDLHeap::init_bins(mstate m) {
  /* Establish circular links for smallbins */
  bindex_t i;
  for (i = 0; i < NSMALLBINS; ++i) {
    sbinptr bin = smallbin_at(m,i);
    bin->fd = bin->bk = bin;
  }
}
/* ---------------------------- malloc support --------------------------- */

/* allocate a large request from the best fitting chunk in a treebin */
void* RSymbianDLHeap::tmalloc_large(mstate m, size_t nb) {
  tchunkptr v = 0;
  size_t rsize = -nb; /* Unsigned negation */
  tchunkptr t;
  bindex_t idx;
  compute_tree_index(nb, idx);

  if ((t = *treebin_at(m, idx)) != 0) {
    /* Traverse tree for this bin looking for node with size == nb */
    size_t sizebits =
    nb <<
    leftshift_for_tree_index(idx);
    tchunkptr rst = 0;  /* The deepest untaken right subtree */
    for (;;) {
      tchunkptr rt;
      size_t trem = chunksize(t) - nb;
      if (trem < rsize) {
        v = t;
        if ((rsize = trem) == 0)
          break;
      }
      rt = t->child[1];
      t = t->child[(sizebits >> (SIZE_T_BITSIZE-SIZE_T_ONE)) & 1];
      if (rt != 0 && rt != t)
        rst = rt;
      if (t == 0) {
        t = rst; /* set t to least subtree holding sizes > nb */
        break;
      }
      sizebits <<= 1;
    }
  }
  if (t == 0 && v == 0) { /* set t to root of next non-empty treebin */
    binmap_t leftbits = left_bits(idx2bit(idx)) & m->treemap;
    if (leftbits != 0) {
      bindex_t i;
      binmap_t leastbit = least_bit(leftbits);
      compute_bit2idx(leastbit, i);
      t = *treebin_at(m, i);
    }
  }
  while (t != 0) { /* find smallest of tree or subtree */
    size_t trem = chunksize(t) - nb;
    if (trem < rsize) {
      rsize = trem;
      v = t;
    }
    t = leftmost_child(t);
  }
  /*  If dv is a better fit, return 0 so malloc will use it */
    if (v != 0) {
    if (RTCHECK(ok_address(m, v))) { /* split */
      mchunkptr r = chunk_plus_offset(v, nb);
      assert(chunksize(v) == rsize + nb);
      
      /* check for chunk memory page-in */
      size_t npages_out = 0;
      if(page_not_in_memory(v, chunksize(v))) {
          if(!is_small(rsize) && rsize>=CHUNK_PAGEOUT_THESHOLD) {
              // partial chunk page mapping
              TInt result = map_chunk_pages_partial(v, chunksize(v), (tchunkptr)r, rsize);
              if (result < 0) return 0; // Failed to Commit RAM
              else npages_out = (size_t)result;              
          }          
          else {
              // full chunk page map needed
              TInt err = map_chunk_pages(v, chunksize(v));
              if(err != KErrNone)  return 0; // Failed to Commit RAM
          }
      }

      if (RTCHECK(ok_next(v, r))) {
        unlink_large_chunk(m, v);
        if (rsize < free_chunk_threshold) // exaust if less than slab threshold
          set_inuse_and_pinuse(m, v, (rsize + nb));
        else {
          set_size_and_pinuse_of_inuse_chunk(m, v, nb);
          set_size_and_pinuse_of_free_chunk(r, rsize);
          insert_chunk(m, r, rsize, npages_out);
        }
        return chunk2mem(v);
      }
    }
    CORRUPTION_ERROR_ACTION(m);
  }
  return 0;
}

/* allocate a small request from the best fitting chunk in a treebin */
void* RSymbianDLHeap::tmalloc_small(mstate m, size_t nb) {
  tchunkptr t, v;
  size_t rsize;
  bindex_t i;
  binmap_t leastbit = least_bit(m->treemap);
  compute_bit2idx(leastbit, i);

  v = t = *treebin_at(m, i);
  rsize = chunksize(t) - nb;

  while ((t = leftmost_child(t)) != 0) {
    size_t trem = chunksize(t) - nb;
    if (trem < rsize) {
      rsize = trem;
      v = t;
    }
  }

  if (RTCHECK(ok_address(m, v))) {
    mchunkptr r = chunk_plus_offset(v, nb);
    assert(chunksize(v) == rsize + nb);
    
    /* check for chunk memory page-in */
      if(page_not_in_memory(v, chunksize(v))) {
          TInt err = map_chunk_pages(v, chunksize(v));
          if(err != KErrNone)  return 0; // Failed to Commit RAM
      }
      
    if (RTCHECK(ok_next(v, r))) {
      unlink_large_chunk(m, v);
      if (rsize < free_chunk_threshold) // exaust if less than slab threshold
        set_inuse_and_pinuse(m, v, (rsize + nb));
      else {
        set_size_and_pinuse_of_inuse_chunk(m, v, nb);
        set_size_and_pinuse_of_free_chunk(r, rsize);
        insert_chunk(m, r, rsize, 0);      
      }
      return chunk2mem(v);
    }
  }
  CORRUPTION_ERROR_ACTION(m);
  return 0;
}

inline void RSymbianDLHeap::init_top(mstate m, mchunkptr p, size_t psize)
{
	/* Ensure alignment */
	size_t offset = align_offset(chunk2mem(p));
	p = (mchunkptr)((TUint8*)p + offset);
	psize -= offset;
	m->top = p;
	m->topsize = psize;
	p->head = psize | PINUSE_BIT;
	/* set size of fake trailing chunk holding overhead space only once */
	mchunkptr chunkPlusOff = chunk_plus_offset(p, psize);
	chunkPlusOff->head = TOP_FOOT_SIZE;
	m->trim_check = mparams.trim_threshold; /* reset on each update */
}

void* RSymbianDLHeap::internal_realloc(mstate m, void* oldmem, size_t bytes)
{
  if (bytes >= MAX_REQUEST) {
    MALLOC_FAILURE_ACTION;
    return 0;
  }
  if (!PREACTION(m)) {
    mchunkptr oldp = mem2chunk(oldmem);
    size_t oldsize = chunksize(oldp);
    mchunkptr next = chunk_plus_offset(oldp, oldsize);
    mchunkptr newp = 0;
    void* extra = 0;

    /* Try to either shrink or extend into top. Else malloc-copy-free */

    if (RTCHECK(ok_address(m, oldp) && ok_cinuse(oldp) &&
                ok_next(oldp, next) && ok_pinuse(next))) {
      size_t nb = request2size(bytes);
      if (is_mmapped(oldp))
        newp = mmap_resize(m, oldp, nb);
      else
	  if (oldsize >= nb) { /* already big enough */
        size_t rsize = oldsize - nb;
        newp = oldp;
        if (rsize >= free_chunk_threshold) {
          mchunkptr remainder = chunk_plus_offset(newp, nb);
          set_inuse(m, newp, nb);
          set_inuse(m, remainder, rsize);
          extra = chunk2mem(remainder);
        }
      }
		/*AMOD: Modified to optimized*/
		else if (next == m->top && oldsize + m->topsize > nb)
		{
			/* Expand into top */
			if(oldsize + m->topsize > nb)
			{
		        size_t newsize = oldsize + m->topsize;
		        size_t newtopsize = newsize - nb;
		        mchunkptr newtop = chunk_plus_offset(oldp, nb);
		        set_inuse(m, oldp, nb);
		        newtop->head = newtopsize |PINUSE_BIT;
		        m->top = newtop;
		        m->topsize = newtopsize;
		        newp = oldp;
			}
      }
    }
    else {
      USAGE_ERROR_ACTION(m, oldmem);
      POSTACTION(m);
      return 0;
    }

    POSTACTION(m);

    if (newp != 0) {
      if (extra != 0) {
        internal_free(m, extra);
      }
      check_inuse_chunk(m, newp);
      return chunk2mem(newp);
    }
    else {
      void* newmem = internal_malloc(m, bytes);
      if (newmem != 0) {
        size_t oc = oldsize - overhead_for(oldp);
        memcpy(newmem, oldmem, (oc < bytes)? oc : bytes);
        internal_free(m, oldmem);
      }
      return newmem;
    }
  }
  return 0;
}
/* ----------------------------- statistics ------------------------------ */
mallinfo RSymbianDLHeap::internal_mallinfo(mstate m) {
  struct mallinfo nm = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  TInt chunkCnt = 0;
  if (!PREACTION(m)) {
    check_malloc_state(m);
    if (is_initialized(m)) {
      size_t nfree = SIZE_T_ONE; /* top always free */
      size_t mfree = m->topsize + TOP_FOOT_SIZE;
      size_t sum = mfree;
      msegmentptr s = &m->seg;
      TInt tmp = (TUint8*)m->top - (TUint8*)s->base;
      while (s != 0) {
        mchunkptr q = align_as_chunk(s->base);
        chunkCnt++;
        while (segment_holds(s, q) &&
               q != m->top && q->head != FENCEPOST_HEAD) {
          size_t sz = chunksize(q);
          sum += sz;
          if (!cinuse(q)) {
            mfree += sz;
            ++nfree;
          }
          q = next_chunk(q);
        }
        s = s->next;
      }
      nm.arena    = sum;
      nm.ordblks  = nfree;
      nm.hblkhd   = m->footprint - sum;
      nm.usmblks  = m->max_footprint;
      nm.uordblks = m->footprint - mfree;
      nm.fordblks = mfree;
      nm.keepcost = m->topsize;
      nm.cellCount= chunkCnt;/*number of chunks allocated*/
    }
    POSTACTION(m);
  }
  return nm;
}

void  RSymbianDLHeap::internal_malloc_stats(mstate m) {
if (!PREACTION(m)) {
  size_t maxfp = 0;
  size_t fp = 0;
  size_t used = 0;
  check_malloc_state(m);
  if (is_initialized(m)) {
    msegmentptr s = &m->seg;
    maxfp = m->max_footprint;
    fp = m->footprint;
    used = fp - (m->topsize + TOP_FOOT_SIZE);

    while (s != 0) {
      mchunkptr q = align_as_chunk(s->base);
      while (segment_holds(s, q) &&
             q != m->top && q->head != FENCEPOST_HEAD) {
        if (!cinuse(q))
          used -= chunksize(q);
        q = next_chunk(q);
      }
      s = s->next;
    }
  }
  POSTACTION(m);
}
}
/* support for mallopt */
int RSymbianDLHeap::change_mparam(int param_number, int value) {
  size_t val = (size_t)value;
  init_mparams(DEFAULT_TRIM_THRESHOLD);
  switch(param_number) {
  case M_TRIM_THRESHOLD:
    mparams.trim_threshold = val;
    return 1;
  case M_GRANULARITY:
    if (val >= mparams.page_size && ((val & (val-1)) == 0)) {
      mparams.granularity = val;
      return 1;
    }
    else
      return 0;
  case M_MMAP_THRESHOLD:
    mparams.mmap_threshold = val;
    return 1;
  default:
    return 0;
  }
}
/* Get memory from system using MORECORE or MMAP */
void* RSymbianDLHeap::sys_alloc(mstate m, size_t nb)
{
	TUint8* tbase = CMFAIL;
	size_t tsize = 0;
	flag_t mmap_flag = 0;
	//init_mparams();/*No need to do init_params here*/
	/* Directly map large chunks */
	if (use_mmap(m) && nb >= mparams.mmap_threshold)
	{
		void* mem = mmap_alloc(m, nb);
		if (mem != 0)
			return mem;
	}
  /*
    Try getting memory in any of three ways (in most-preferred to
    least-preferred order):
    1. A call to MORECORE that can normally contiguously extend memory.
       (disabled if not MORECORE_CONTIGUOUS or not HAVE_MORECORE or
       or main space is mmapped or a previous contiguous call failed)
    2. A call to MMAP new space (disabled if not HAVE_MMAP).
       Note that under the default settings, if MORECORE is unable to
       fulfill a request, and HAVE_MMAP is true, then mmap is
       used as a noncontiguous system allocator. This is a useful backup
       strategy for systems with holes in address spaces -- in this case
       sbrk cannot contiguously expand the heap, but mmap may be able to
       find space.
    3. A call to MORECORE that cannot usually contiguously extend memory.
       (disabled if not HAVE_MORECORE)
  */
  /*Trying to allocate the memory*/
	if(MORECORE_CONTIGUOUS && !use_noncontiguous(m))
	{
	TUint8* br = CMFAIL;
    msegmentptr ss = (m->top == 0)? 0 : segment_holding(m, (TUint8*)m->top);
    size_t asize = 0;
    ACQUIRE_MORECORE_LOCK(m);
    if (ss == 0)
	{  /* First time through or recovery */
		TUint8* base = (TUint8*)CALL_MORECORE(0);
		if (base != CMFAIL)
		{
			asize = granularity_align(nb + TOP_FOOT_SIZE + SIZE_T_ONE);
			/* Adjust to end on a page boundary */
			if (!is_page_aligned(base))
				asize += (page_align((size_t)base) - (size_t)base);
			/* Can't call MORECORE if size is negative when treated as signed */
			if (asize < HALF_MAX_SIZE_T &&(br = (TUint8*)(CALL_MORECORE(asize))) == base)
			{
				tbase = base;
				tsize = asize;
			}
		}
    }
    else
	{
      /* Subtract out existing available top space from MORECORE request. */
		asize = granularity_align(nb - m->topsize + TOP_FOOT_SIZE + SIZE_T_ONE);
    /* Use mem here only if it did continuously extend old space */
      if (asize < HALF_MAX_SIZE_T &&
          (br = (TUint8*)(CALL_MORECORE(asize))) == ss->base+ss->size) {
        tbase = br;
        tsize = asize;
      }
    }
    if (tbase == CMFAIL) {    /* Cope with partial failure */
      if (br != CMFAIL) {    /* Try to use/extend the space we did get */
        if (asize < HALF_MAX_SIZE_T &&
            asize < nb + TOP_FOOT_SIZE + SIZE_T_ONE) {
          size_t esize = granularity_align(nb + TOP_FOOT_SIZE + SIZE_T_ONE - asize);
          if (esize < HALF_MAX_SIZE_T) {
            TUint8* end = (TUint8*)CALL_MORECORE(esize);
            if (end != CMFAIL)
              asize += esize;
            else {            /* Can't use; try to release */
              CALL_MORECORE(-asize);
              br = CMFAIL;
            }
          }
        }
      }
      if (br != CMFAIL) {    /* Use the space we did get */
        tbase = br;
        tsize = asize;
      }
      else
        disable_contiguous(m); /* Don't try contiguous path in the future */
    }
    RELEASE_MORECORE_LOCK(m);
  }
  if (HAVE_MMAP && tbase == CMFAIL) {  /* Try MMAP */
    size_t req = nb + TOP_FOOT_SIZE + SIZE_T_ONE;
    size_t rsize = granularity_align(req);
    if (rsize > nb) { /* Fail if wraps around zero */
      TUint8* mp = (TUint8*)(CALL_MMAP(rsize));
      if (mp != CMFAIL) {
        tbase = mp;
        tsize = rsize;
        mmap_flag = IS_MMAPPED_BIT;
      }
    }
  }
  if (HAVE_MORECORE && tbase == CMFAIL) { /* Try noncontiguous MORECORE */
    size_t asize = granularity_align(nb + TOP_FOOT_SIZE + SIZE_T_ONE);
    if (asize < HALF_MAX_SIZE_T) {
      TUint8* br = CMFAIL;
      TUint8* end = CMFAIL;
      ACQUIRE_MORECORE_LOCK(m);
      br = (TUint8*)(CALL_MORECORE(asize));
      end = (TUint8*)(CALL_MORECORE(0));
      RELEASE_MORECORE_LOCK(m);
      if (br != CMFAIL && end != CMFAIL && br < end) {
        size_t ssize = end - br;
        if (ssize > nb + TOP_FOOT_SIZE) {
          tbase = br;
          tsize = ssize;
        }
      }
    }
  }
  if (tbase != CMFAIL) {
    if ((m->footprint += tsize) > m->max_footprint)
      m->max_footprint = m->footprint;
    if (!is_initialized(m)) { /* first-time initialization */
      m->seg.base = m->least_addr = tbase;
      m->seg.size = tsize;
      m->seg.sflags = mmap_flag;
      m->magic = mparams.magic;
      init_bins(m);
      if (is_global(m))
        init_top(m, (mchunkptr)tbase, tsize - TOP_FOOT_SIZE);
      else {
        /* Offset top by embedded malloc_state */
        mchunkptr mn = next_chunk(mem2chunk(m));
        init_top(m, mn, (size_t)((tbase + tsize) - (TUint8*)mn) -TOP_FOOT_SIZE);
      }
    }else {
      /* Try to merge with an existing segment */
      msegmentptr sp = &m->seg;
      while (sp != 0 && tbase != sp->base + sp->size)
        sp = sp->next;
      if (sp != 0 && !is_extern_segment(sp) &&
          (sp->sflags & IS_MMAPPED_BIT) == mmap_flag &&
          segment_holds(sp, m->top))
    	  { /* append */
        sp->size += tsize;
        init_top(m, m->top, m->topsize + tsize);
      }
      else {
        if (tbase < m->least_addr)
          m->least_addr = tbase;
        sp = &m->seg;
        while (sp != 0 && sp->base != tbase + tsize)
          sp = sp->next;
        if (sp != 0 &&
            !is_extern_segment(sp) &&
            (sp->sflags & IS_MMAPPED_BIT) == mmap_flag) {
          TUint8* oldbase = sp->base;
          sp->base = tbase;
          sp->size += tsize;
          return prepend_alloc(m, tbase, oldbase, nb);
        }
        else
          add_segment(m, tbase, tsize, mmap_flag);
      }
    }
    if (nb < m->topsize) { /* Allocate from new or extended top space */
      size_t rsize = m->topsize -= nb;
      mchunkptr p = m->top;
      mchunkptr r = m->top = chunk_plus_offset(p, nb);
      r->head = rsize | PINUSE_BIT;
      set_size_and_pinuse_of_inuse_chunk(m, p, nb);
      check_top_chunk(m, m->top);
      check_malloced_chunk(m, chunk2mem(p), nb);
      return chunk2mem(p);
    }
  }
  /*need to check this*/
  MEM_DUMP_OOM_LOGS(nb, "sys_alloc:: FAILED to get more memory");
  
  //errno = -1;
  return 0;
}
msegmentptr RSymbianDLHeap::segment_holding(mstate m, TUint8* addr) {
  msegmentptr sp = &m->seg;
  for (;;) {
    if (addr >= sp->base && addr < sp->base + sp->size)
      return sp;
    if ((sp = sp->next) == 0)
      return 0;
  }
}
/* Unlink the first chunk from a smallbin */
inline void RSymbianDLHeap::unlink_first_small_chunk(mstate M,mchunkptr B,mchunkptr P,bindex_t& I)
{
  mchunkptr F = P->fd;
  assert(P != B);
  assert(P != F);
  assert(chunksize(P) == small_index2size(I));
  if (B == F)
    clear_smallmap(M, I);
  else if (RTCHECK(ok_address(M, F))) {
    B->fd = F;
    F->bk = B;
  }
  else {
    CORRUPTION_ERROR_ACTION(M);
  }
}
/* Link a free chunk into a smallbin  */
inline void RSymbianDLHeap::insert_small_chunk(mstate M,mchunkptr P, size_t S)
{
  bindex_t I  = small_index(S);
  mchunkptr B = smallbin_at(M, I);
  mchunkptr F = B;
  assert(S >= MIN_CHUNK_SIZE);
  if (!smallmap_is_marked(M, I))
    mark_smallmap(M, I);
  else if (RTCHECK(ok_address(M, B->fd)))
    F = B->fd;
  else {
    CORRUPTION_ERROR_ACTION(M);
  }
  B->fd = P;
  F->bk = P;
  P->fd = F;
  P->bk = B;
}


inline void RSymbianDLHeap::insert_chunk(mstate M,mchunkptr P,size_t S,size_t NPAGES)
{
	if (is_small(S))
		insert_small_chunk(M, P, S);
	else{
		tchunkptr TP = (tchunkptr)(P); insert_large_chunk(M, TP, S, NPAGES);
	 }
}

inline void RSymbianDLHeap::unlink_large_chunk(mstate M,tchunkptr X)
{
  tchunkptr XP = X->parent;
  tchunkptr R;
  reset_tchunk_mem_pageout(X); // clear chunk pageout flag
  if (X->bk != X) {
    tchunkptr F = X->fd;
    R = X->bk;
    if (RTCHECK(ok_address(M, F))) {
      F->bk = R;
      R->fd = F;
    }
    else {
      CORRUPTION_ERROR_ACTION(M);
    }
  }
  else {
    tchunkptr* RP;
    if (((R = *(RP = &(X->child[1]))) != 0) ||
        ((R = *(RP = &(X->child[0]))) != 0)) {
      tchunkptr* CP;
      while ((*(CP = &(R->child[1])) != 0) ||
             (*(CP = &(R->child[0])) != 0)) {
        R = *(RP = CP);
      }
      if (RTCHECK(ok_address(M, RP)))
        *RP = 0;
      else {
        CORRUPTION_ERROR_ACTION(M);
      }
    }
  }
  if (XP != 0) {
    tbinptr* H = treebin_at(M, X->index);
    if (X == *H) {
      if ((*H = R) == 0)
        clear_treemap(M, X->index);
    }
    else if (RTCHECK(ok_address(M, XP))) {
      if (XP->child[0] == X)
        XP->child[0] = R;
      else
        XP->child[1] = R;
    }
    else
      CORRUPTION_ERROR_ACTION(M);
    if (R != 0) {
      if (RTCHECK(ok_address(M, R))) {
        tchunkptr C0, C1;
        R->parent = XP;
        if ((C0 = X->child[0]) != 0) {
          if (RTCHECK(ok_address(M, C0))) {
            R->child[0] = C0;
            C0->parent = R;
          }
          else
            CORRUPTION_ERROR_ACTION(M);
        }
        if ((C1 = X->child[1]) != 0) {
          if (RTCHECK(ok_address(M, C1))) {
            R->child[1] = C1;
            C1->parent = R;
          }
          else
            CORRUPTION_ERROR_ACTION(M);
        }
      }
      else
        CORRUPTION_ERROR_ACTION(M);
    }
  }
}

/* Unlink a chunk from a smallbin  */
inline void RSymbianDLHeap::unlink_small_chunk(mstate M, mchunkptr P,size_t S)
{
  mchunkptr F = P->fd;
  mchunkptr B = P->bk;
  bindex_t I = small_index(S);
  assert(P != B);
  assert(P != F);
  assert(chunksize(P) == small_index2size(I));
  if (F == B)
    clear_smallmap(M, I);
  else if (RTCHECK((F == smallbin_at(M,I) || ok_address(M, F)) &&
                   (B == smallbin_at(M,I) || ok_address(M, B)))) {
    F->bk = B;
    B->fd = F;
  }
  else {
    CORRUPTION_ERROR_ACTION(M);
  }
}

inline void RSymbianDLHeap::unlink_chunk(mstate M, mchunkptr P, size_t S)
{
  if (is_small(S))
	unlink_small_chunk(M, P, S);
  else
  {
	  tchunkptr TP = (tchunkptr)(P); unlink_large_chunk(M, TP);
  }
}

inline void RSymbianDLHeap::compute_tree_index(size_t S, bindex_t& I)
{
  size_t X = S >> TREEBIN_SHIFT;
  if (X == 0)
    I = 0;
  else if (X > 0xFFFF)
    I = NTREEBINS-1;
  else {
    unsigned int Y = (unsigned int)X;
    unsigned int N = ((Y - 0x100) >> 16) & 8;
    unsigned int K = (((Y <<= N) - 0x1000) >> 16) & 4;
    N += K;
    N += K = (((Y <<= K) - 0x4000) >> 16) & 2;
    K = 14 - N + ((Y <<= K) >> 15);
    I = (K << 1) + ((S >> (K + (TREEBIN_SHIFT-1)) & 1));
  }
}

/* ------------------------- Operations on trees ------------------------- */

/* Insert chunk into tree */
inline void RSymbianDLHeap::insert_large_chunk(mstate M,tchunkptr X,size_t S,size_t NPAGES)
{
  tbinptr* H;
  bindex_t I;
  compute_tree_index(S, I);
  H = treebin_at(M, I);
  X->index = I;
  X->child[0] = X->child[1] = 0;

  if(NPAGES) { set_tchunk_mem_pageout(X, NPAGES) }
  else  { reset_tchunk_mem_pageout(X) }
  
  if (!treemap_is_marked(M, I)) {
    mark_treemap(M, I);
    *H = X;
    X->parent = (tchunkptr)H;
    X->fd = X->bk = X;
  }
  else {
    tchunkptr T = *H;
    size_t K = S << leftshift_for_tree_index(I);
    for (;;) {
      if (chunksize(T) != S) {
        tchunkptr* C = &(T->child[(K >> (SIZE_T_BITSIZE-SIZE_T_ONE)) & 1]);
        K <<= 1;
        if (*C != 0)
          T = *C;
        else if (RTCHECK(ok_address(M, C))) {
          *C = X;
          X->parent = T;
          X->fd = X->bk = X;
          break;
        }
        else {
          CORRUPTION_ERROR_ACTION(M);
          break;
        }
      }
      else {
        tchunkptr F = T->fd;
        if (RTCHECK(ok_address(M, T) && ok_address(M, F))) {
          T->fd = F->bk = X;
          X->fd = F;
          X->bk = T;
          X->parent = 0;
          break;
        }
        else {
          CORRUPTION_ERROR_ACTION(M);
          break;
        }
      }
    }
  }
}

/*
  Unlink steps:

  1. If x is a chained node, unlink it from its same-sized fd/bk links
     and choose its bk node as its replacement.
  2. If x was the last node of its size, but not a leaf node, it must
     be replaced with a leaf node (not merely one with an open left or
     right), to make sure that lefts and rights of descendents
     correspond properly to bit masks.  We use the rightmost descendent
     of x.  We could use any other leaf, but this is easy to locate and
     tends to counteract removal of leftmosts elsewhere, and so keeps
     paths shorter than minimally guaranteed.  This doesn't loop much
     because on average a node in a tree is near the bottom.
  3. If x is the base of a chain (i.e., has parent links) relink
     x's parent and children to x's replacement (or null if none).
*/

/* Replace dv node, binning the old one */
/* Used only when dvsize known to be small */
inline void RSymbianDLHeap::replace_dv(mstate M, mchunkptr P, size_t S)
{
  size_t DVS = M->dvsize;
  if (DVS != 0) {
    mchunkptr DV = M->dv;
    assert(is_small(DVS));
    insert_small_chunk(M, DV, DVS);
  }
  M->dvsize = S;
  M->dv = P;
}

inline void RSymbianDLHeap::compute_bit2idx(binmap_t X,bindex_t& I)
{
	unsigned int Y = X - 1;
	unsigned int K = Y >> (16-4) & 16;
	unsigned int N = K;        Y >>= K;
	N += K = Y >> (8-3) &  8;  Y >>= K;
	N += K = Y >> (4-2) &  4;  Y >>= K;
	N += K = Y >> (2-1) &  2;  Y >>= K;
	N += K = Y >> (1-0) &  1;  Y >>= K;
	I = (bindex_t)(N + Y);
}

void RSymbianDLHeap::add_segment(mstate m, TUint8* tbase, size_t tsize, flag_t mmapped) {
  /* Determine locations and sizes of segment, fenceposts, old top */
  TUint8* old_top = (TUint8*)m->top;
  msegmentptr oldsp = segment_holding(m, old_top);
  TUint8* old_end = oldsp->base + oldsp->size;
  size_t ssize = pad_request(sizeof(struct malloc_segment));
  TUint8* rawsp = old_end - (ssize + FOUR_SIZE_T_SIZES + CHUNK_ALIGN_MASK);
  size_t offset = align_offset(chunk2mem(rawsp));
  TUint8* asp = rawsp + offset;
  TUint8* csp = (asp < (old_top + MIN_CHUNK_SIZE))? old_top : asp;
  mchunkptr sp = (mchunkptr)csp;
  msegmentptr ss = (msegmentptr)(chunk2mem(sp));
  mchunkptr tnext = chunk_plus_offset(sp, ssize);
  mchunkptr p = tnext;
  int nfences = 0;

  /* reset top to new space */
  init_top(m, (mchunkptr)tbase, tsize - TOP_FOOT_SIZE);

  /* Set up segment record */
  assert(is_aligned(ss));
  set_size_and_pinuse_of_inuse_chunk(m, sp, ssize);
  *ss = m->seg; /* Push current record */
  m->seg.base = tbase;
  m->seg.size = tsize;
  m->seg.sflags = mmapped;
  m->seg.next = ss;

  /* Insert trailing fenceposts */
  for (;;) {
    mchunkptr nextp = chunk_plus_offset(p, SIZE_T_SIZE);
    p->head = FENCEPOST_HEAD;
    ++nfences;
    if ((TUint8*)(&(nextp->head)) < old_end)
      p = nextp;
    else
      break;
  }
  assert(nfences >= 2);

  /* Insert the rest of old top into a bin as an ordinary free chunk */
  if (csp != old_top) {
    mchunkptr q = (mchunkptr)old_top;
    size_t psize = csp - old_top;
    mchunkptr tn = chunk_plus_offset(q, psize);
    set_free_with_pinuse(q, psize, tn);
    insert_chunk(m, q, psize, 0);
  }

  check_top_chunk(m, m->top);
}


void* RSymbianDLHeap::prepend_alloc(mstate m, TUint8* newbase, TUint8* oldbase,
                           size_t nb) {
  mchunkptr p = align_as_chunk(newbase);
  mchunkptr oldfirst = align_as_chunk(oldbase);
  size_t psize = (TUint8*)oldfirst - (TUint8*)p;
  mchunkptr q = chunk_plus_offset(p, nb);
  size_t qsize = psize - nb;
  set_size_and_pinuse_of_inuse_chunk(m, p, nb);

  assert((TUint8*)oldfirst > (TUint8*)q);
  assert(pinuse(oldfirst));
  assert(qsize >= MIN_CHUNK_SIZE);

  /* consolidate remainder with first chunk of old base */
  if (oldfirst == m->top) {
    size_t tsize = m->topsize += qsize;
    m->top = q;
    q->head = tsize | PINUSE_BIT;
    check_top_chunk(m, q);
  }
  else {
    if (!cinuse(oldfirst)) {
      size_t nsize = chunksize(oldfirst);
      
      /* check for chunk memory page-in */
      if(page_not_in_memory(oldfirst, nsize))
        map_chunk_pages((tchunkptr)oldfirst, nsize);       //Err Ignored, branch not reachable.
      
      unlink_chunk(m, oldfirst, nsize);
      oldfirst = chunk_plus_offset(oldfirst, nsize);
      qsize += nsize;
    }
    set_free_with_pinuse(q, qsize, oldfirst);
    insert_chunk(m, q, qsize, 0);
    check_free_chunk(m, q);
  }

  check_malloced_chunk(m, chunk2mem(p), nb);
  return chunk2mem(p);
}

void* RSymbianDLHeap::mmap_alloc(mstate m, size_t nb) {
  size_t mmsize = granularity_align(nb + SIX_SIZE_T_SIZES + CHUNK_ALIGN_MASK);
  if (mmsize > nb) {     /* Check for wrap around 0 */
    TUint8* mm = (TUint8*)(DIRECT_MMAP(mmsize));
    if (mm != CMFAIL) {
      size_t offset = align_offset(chunk2mem(mm));
      size_t psize = mmsize - offset - MMAP_FOOT_PAD;
      mchunkptr p = (mchunkptr)(mm + offset);
      p->prev_foot = offset | IS_MMAPPED_BIT;
      (p)->head = (psize|CINUSE_BIT);
      mark_inuse_foot(m, p, psize);
      chunk_plus_offset(p, psize)->head = FENCEPOST_HEAD;
      chunk_plus_offset(p, psize+SIZE_T_SIZE)->head = 0;

      if (mm < m->least_addr)
        m->least_addr = mm;
      if ((m->footprint += mmsize) > m->max_footprint)
        m->max_footprint = m->footprint;
      assert(is_aligned(chunk2mem(p)));
      check_mmapped_chunk(m, p);
      return chunk2mem(p);
    }
  }
  return 0;
}

	int RSymbianDLHeap::sys_trim(mstate m, size_t pad)
	{
	  size_t released = 0;
	  if (pad < MAX_REQUEST && is_initialized(m)) {
	    pad += TOP_FOOT_SIZE; /* ensure enough room for segment overhead */

	    if (m->topsize > pad) {
	      /* Shrink top space in granularity-size units, keeping at least one */
	      size_t unit = mparams.granularity;
				size_t extra = ((m->topsize - pad + (unit - SIZE_T_ONE)) / unit - SIZE_T_ONE) * unit;
	      msegmentptr sp = segment_holding(m, (TUint8*)m->top);

	      if (!is_extern_segment(sp)) {
	        if (is_mmapped_segment(sp)) {
	          if (HAVE_MMAP &&
	              sp->size >= extra &&
	              !has_segment_link(m, sp)) { /* can't shrink if pinned */
	            size_t newsize = sp->size - extra;
	            /* Prefer mremap, fall back to munmap */
	            if ((CALL_MREMAP(sp->base, sp->size, newsize, 0) != MFAIL) ||
	                (CALL_MUNMAP(sp->base + newsize, extra) == 0)) {
	              released = extra;
	            }
	          }
	        }
	        else if (HAVE_MORECORE) {
	          if (extra >= HALF_MAX_SIZE_T) /* Avoid wrapping negative */
	            extra = (HALF_MAX_SIZE_T) + SIZE_T_ONE - unit;
	          ACQUIRE_MORECORE_LOCK(m);
	          {
	            /* Make sure end of memory is where we last set it. */
	            TUint8* old_br = (TUint8*)(CALL_MORECORE(0));
	            if (old_br == sp->base + sp->size) {
	              TUint8* rel_br = (TUint8*)(CALL_MORECORE(-extra));
	              TUint8* new_br = (TUint8*)(CALL_MORECORE(0));
	              if (rel_br != CMFAIL && new_br < old_br)
	                released = old_br - new_br;
	            }
	          }
	          RELEASE_MORECORE_LOCK(m);
	        }
	      }

	      if (released != 0) {
	        sp->size -= released;
	        m->footprint -= released;
	        init_top(m, m->top, m->topsize - released);
	        check_top_chunk(m, m->top);
	      }
	    }

	    /* Unmap any unused mmapped segments */
	    if (HAVE_MMAP)
	      released += release_unused_segments(m);

	    /* On failure, disable autotrim to avoid repeated failed future calls */
	    if (released == 0)
	      m->trim_check = MAX_SIZE_T;
	  }

	  return (released != 0)? 1 : 0;
	}

	inline int RSymbianDLHeap::has_segment_link(mstate m, msegmentptr ss)
	{
	  msegmentptr sp = &m->seg;
	  for (;;) {
	    if ((TUint8*)sp >= ss->base && (TUint8*)sp < ss->base + ss->size)
	      return 1;
	    if ((sp = sp->next) == 0)
	      return 0;
	  }
	}

	/* Unmap and unlink any mmapped segments that don't contain used chunks */
	size_t RSymbianDLHeap::release_unused_segments(mstate m)
	{
	  size_t released = 0;
	  msegmentptr pred = &m->seg;
	  msegmentptr sp = pred->next;
	  while (sp != 0) {
	    TUint8* base = sp->base;
	    size_t size = sp->size;
	    msegmentptr next = sp->next;
	    if (is_mmapped_segment(sp) && !is_extern_segment(sp)) {
	      mchunkptr p = align_as_chunk(base);
	      size_t psize = chunksize(p);
	      /* Can unmap if first chunk holds entire segment and not pinned */
	      if (!cinuse(p) && (TUint8*)p + psize >= base + size - TOP_FOOT_SIZE) {
	        tchunkptr tp = (tchunkptr)p;
	        size_t npages_out = tp->npages;
	        assert(segment_holds(sp, (TUint8*)sp));
            unlink_large_chunk(m, tp);
	        if (CALL_MUNMAP(base, size) == 0) {
	          released += size;
	          m->footprint -= size;
	          /* unlink obsoleted record */
	          sp = pred;
	          sp->next = next;
	        }
	        else { /* back out if cannot unmap */
	          insert_large_chunk(m, tp, psize, npages_out);
	        }
	      }
	    }
	    pred = sp;
	    sp = next;
	  }/*End of while*/
	  return released;
	}
	/* Realloc using mmap */
	inline	mchunkptr RSymbianDLHeap::mmap_resize(mstate m, mchunkptr oldp, size_t nb)
	{
	  size_t oldsize = chunksize(oldp);
	  if (is_small(nb)) /* Can't shrink mmap regions below small size */
	    return 0;
	  /* Keep old chunk if big enough but not too big */
	  if (oldsize >= nb + SIZE_T_SIZE &&
	      (oldsize - nb) <= (mparams.granularity << 1))
	    return oldp;
	  else {
	    size_t offset = oldp->prev_foot & ~IS_MMAPPED_BIT;
	    size_t oldmmsize = oldsize + offset + MMAP_FOOT_PAD;
	    size_t newmmsize = granularity_align(nb + SIX_SIZE_T_SIZES +
	                                         CHUNK_ALIGN_MASK);
	    TUint8* cp = (TUint8*)CALL_MREMAP((char*)oldp - offset,
	                                  oldmmsize, newmmsize, 1);
	    if (cp != CMFAIL) {
	      mchunkptr newp = (mchunkptr)(cp + offset);
	      size_t psize = newmmsize - offset - MMAP_FOOT_PAD;
	      newp->head = (psize|CINUSE_BIT);
	      mark_inuse_foot(m, newp, psize);
	      chunk_plus_offset(newp, psize)->head = FENCEPOST_HEAD;
	      chunk_plus_offset(newp, psize+SIZE_T_SIZE)->head = 0;

	      if (cp < m->least_addr)
	        m->least_addr = cp;
	      if ((m->footprint += newmmsize - oldmmsize) > m->max_footprint)
	        m->max_footprint = m->footprint;
	      check_mmapped_chunk(m, newp);
	      return newp;
	    }
	  }
	  return 0;
	}


void RSymbianDLHeap::Init_Dlmalloc(size_t capacity, int locked, size_t aTrimThreshold)
	{
		memset(gm,0,sizeof(malloc_state));
		init_mparams(aTrimThreshold); /* Ensure pagesize etc initialized */
		// The maximum amount that can be allocated can be calculated as:-
		// 2^sizeof(size_t) - sizeof(malloc_state) - TOP_FOOT_SIZE - page size (all accordingly padded)
		// If the capacity exceeds this, no allocation will be done.
		gm->seg.base = gm->least_addr = iBase;
		gm->seg.size = capacity;
		gm->seg.sflags = !IS_MMAPPED_BIT;
		set_lock(gm, locked);
		gm->magic = mparams.magic;
		init_bins(gm);
		init_top(gm, (mchunkptr)iBase, capacity - TOP_FOOT_SIZE);
	}

void* RSymbianDLHeap::dlmalloc(size_t bytes) {
  /*
     Basic algorithm:
     If a small request (< 256 bytes minus per-chunk overhead):
       1. If one exists, use a remainderless chunk in associated smallbin.
          (Remainderless means that there are too few excess bytes to represent as a chunk.)
       2. If one exists, split the smallest available chunk in a bin, saving remainder in bin.
       4. If it is big enough, use the top chunk.
       5. If available, get memory from system and use it
     Otherwise, for a large request:
       1. Find the smallest available binned chunk that fits, splitting if necessary.
       3. If it is big enough, use the top chunk.
       4. If request size >= mmap threshold, try to directly mmap this chunk.
       5. If available, get memory from system and use it

     The ugly goto's here ensure that postaction occurs along all paths.
  */
  if (!PREACTION(gm)) {
    void* mem;
    size_t nb;
    if (bytes <= MAX_SMALL_REQUEST) {
      bindex_t idx;
      binmap_t smallbits;
      nb = (bytes < MIN_REQUEST)? MIN_CHUNK_SIZE : pad_request(bytes);
      idx = small_index(nb);
      smallbits = gm->smallmap >> idx;

      if ((smallbits & 0x3U) != 0) { /* Remainderless fit to a smallbin. */
        mchunkptr b, p;
        idx += ~smallbits & 1;       /* Uses next bin if idx empty */
        b = smallbin_at(gm, idx);
        p = b->fd;
        assert(chunksize(p) == small_index2size(idx));
        unlink_first_small_chunk(gm, b, p, idx);
        set_inuse_and_pinuse(gm, p, small_index2size(idx));
        mem = chunk2mem(p);
        check_malloced_chunk(gm, mem, nb);
        goto postaction;
      } else {
        if (smallbits != 0) { /* Use chunk in next nonempty smallbin */
          mchunkptr b, p, r;
          size_t rsize;
          bindex_t i;
          binmap_t leftbits = (smallbits << idx) & left_bits(idx2bit(idx));
          binmap_t leastbit = least_bit(leftbits);
          compute_bit2idx(leastbit, i);
          b = smallbin_at(gm, i);
          p = b->fd;
          assert(chunksize(p) == small_index2size(i));
          unlink_first_small_chunk(gm, b, p, i);
          rsize = small_index2size(i) - nb;
          /* Fit here cannot be remainderless if 4byte sizes */
          if (rsize < free_chunk_threshold)
            set_inuse_and_pinuse(gm, p, small_index2size(i));
          else {
            set_size_and_pinuse_of_inuse_chunk(gm, p, nb);
            r = chunk_plus_offset(p, nb);
            set_size_and_pinuse_of_free_chunk(r, rsize);
            insert_chunk(gm, r, rsize, 0);
          }
          mem = chunk2mem(p);
          check_malloced_chunk(gm, mem, nb);
          goto postaction;
        }
        else if (gm->treemap != 0 && (mem = tmalloc_small(gm, nb)) != 0) {
          check_malloced_chunk(gm, mem, nb);
          goto postaction;
        }
      }
    } /* else - large alloc request */ 
    else if (bytes >= MAX_REQUEST)
      nb = MAX_SIZE_T; /* Too big to allocate. Force failure (in sys alloc) */
    else {
      nb = pad_request(bytes);
      if (gm->treemap != 0 && (mem = tmalloc_large(gm, nb)) != 0) {
        check_malloced_chunk(gm, mem, nb);
        goto postaction;
      }
    }

    if (nb < gm->topsize) { /* Split top */
      size_t rsize = gm->topsize -= nb;
      mchunkptr p = gm->top;
      mchunkptr r = gm->top = chunk_plus_offset(p, nb);
      r->head = rsize | PINUSE_BIT;
      set_size_and_pinuse_of_inuse_chunk(gm, p, nb);
      mem = chunk2mem(p);
      check_top_chunk(gm, gm->top);
      check_malloced_chunk(gm, mem, nb);
      goto postaction;
    }

    mem = sys_alloc(gm, nb);

  postaction:
    POSTACTION(gm);
#ifdef DL_CHUNK_MEM_DEBUG    
    if(mem) {
        mchunkptr pp = mem2chunk(mem);
        do_check_any_chunk_access(pp, chunksize(pp));
    }
#endif   

    return mem;
  }

  return 0;
}

void RSymbianDLHeap::dlfree(void* mem) {
  /*
     Consolidate freed chunks with preceeding or succeeding bordering
     free chunks, if they exist, and then place in a bin.  Intermixed
     with special cases for top, dv, mmapped chunks, and usage errors.
  */

	if (mem != 0)
	{
	    size_t unmapped_pages = 0;
	    int prev_chunk_unmapped = 0;
		mchunkptr p  = mem2chunk(mem);
#if FOOTERS
		mstate fm = get_mstate_for(p);
		if (!ok_magic(fm))
		{
			USAGE_ERROR_ACTION(fm, p);
			return;
		}
#else /* FOOTERS */
#define fm gm
#endif /* FOOTERS */

		if (!PREACTION(fm))
		{
			check_inuse_chunk(fm, p);
			if (RTCHECK(ok_address(fm, p) && ok_cinuse(p)))
			{
				size_t psize = chunksize(p);
				mchunkptr next = chunk_plus_offset(p, psize);
				if (!pinuse(p))
				{
					size_t prevsize = p->prev_foot;
					if ((prevsize & IS_MMAPPED_BIT) != 0)
					{
						prevsize &= ~IS_MMAPPED_BIT;
						psize += prevsize + MMAP_FOOT_PAD;
							if (CALL_MUNMAP((char*)p - prevsize, psize) == 0)
								fm->footprint -= psize;
							goto postaction;
					}
					else
					{
						mchunkptr prev = chunk_minus_offset(p, prevsize);
						if(page_not_in_memory(prev, prevsize)) {
                            prev_chunk_unmapped = 1;
                            unmapped_pages = ((tchunkptr)prev)->npages;
						}
						
						psize += prevsize;
						p = prev;
						if (RTCHECK(ok_address(fm, prev)))
						{ /* consolidate backward */
						    unlink_chunk(fm, p, prevsize);							
						}
						else
							goto erroraction;
					}
				}

				if (RTCHECK(ok_next(p, next) && ok_pinuse(next)))
				{
					if (!cinuse(next))
					{  /* consolidate forward */
						if (next == fm->top)
						{							
							if(prev_chunk_unmapped) { // previous chunk is unmapped
                            /* unmap all pages between previously unmapped and end of top chunk 
                               and reset top to beginning of prev chunk - done in sys_trim_partial() */
                                sys_trim_partial(fm, p, psize, unmapped_pages);
                                do_check_any_chunk_access(fm->top, fm->topsize);
                                goto postaction;
							}
							else { // forward merge to top
                                size_t tsize = fm->topsize += psize;
                                fm->top = p;
                                p->head = tsize | PINUSE_BIT;
                                if (should_trim(fm, tsize))
                                    sys_trim(fm, 0);
                                do_check_any_chunk_access(fm->top, fm->topsize);
                                goto postaction;							
							}							    							    
						}
						else
						{
                            size_t nsize = chunksize(next);
                            int next_chunk_unmapped = 0;
                            if( page_not_in_memory(next, nsize) ) {
                                next_chunk_unmapped = 1;
                                unmapped_pages += ((tchunkptr)next)->npages;
                            }
                            
							psize += nsize;
							unlink_chunk(fm, next, nsize);
							set_size_and_pinuse_of_free_chunk(p, psize);
						}
					}
					else
						set_free_with_pinuse(p, psize, next);
					
 		            /* check if chunk memmory can be released */
				    size_t npages_out = 0;
		            if(!is_small(psize) && psize>=CHUNK_PAGEOUT_THESHOLD)   
		                npages_out = unmap_chunk_pages((tchunkptr)p, psize, unmapped_pages);

					insert_chunk(fm, p, psize, npages_out);
					check_free_chunk(fm, p);
					do_chunk_page_release_check(p, psize, fm, npages_out);
					goto postaction;
				}
			}
erroraction:
    	USAGE_ERROR_ACTION(fm, p);
postaction:
    	POSTACTION(fm);
		}
	}
#if !FOOTERS
#undef fm
#endif /* FOOTERS */
}

void* RSymbianDLHeap::dlrealloc(void* oldmem, size_t bytes) {
  if (oldmem == 0)
    return dlmalloc(bytes);
#ifdef REALLOC_ZERO_BYTES_FREES
  if (bytes == 0) {
    dlfree(oldmem);
    return 0;
  }
#endif /* REALLOC_ZERO_BYTES_FREES */
  else {
#if ! FOOTERS
    mstate m = gm;
#else /* FOOTERS */
    mstate m = get_mstate_for(mem2chunk(oldmem));
    if (!ok_magic(m)) {
      USAGE_ERROR_ACTION(m, oldmem);
      return 0;
    }
#endif /* FOOTERS */
    return internal_realloc(m, oldmem, bytes);
  }
}


int RSymbianDLHeap::dlmalloc_trim(size_t pad) {
  int result = 0;
  if (!PREACTION(gm)) {
    result = sys_trim(gm, pad);
    POSTACTION(gm);
  }
  return result;
}

size_t RSymbianDLHeap::dlmalloc_footprint(void) {
  return gm->footprint;
}

size_t RSymbianDLHeap::dlmalloc_max_footprint(void) {
  return gm->max_footprint;
}

#if !NO_MALLINFO
struct mallinfo RSymbianDLHeap::dlmallinfo(void) {
  return internal_mallinfo(gm);
}
#endif /* NO_MALLINFO */

void RSymbianDLHeap::dlmalloc_stats() {
  internal_malloc_stats(gm);
}

int RSymbianDLHeap::dlmallopt(int param_number, int value) {
  return change_mparam(param_number, value);
}

//inline slab* slab::slabfor(void* p)
slab* slab::slabfor( const void* p)
{
return (slab*)(floor(p, slabsize));
}


void RSymbianDLHeap::tree_remove(slab* s)
{
	slab** r = s->parent;
	slab* c1 = s->child1;
	slab* c2 = s->child2;
	for (;;)
	{
		if (!c2)
		{
			*r = c1;
			if (c1)
				c1->parent = r;
			return;
		}
		if (!c1)
		{
			*r = c2;
			c2->parent = r;
			return;
		}
		if (c1 > c2)
		{
			slab* c3 = c1;
			c1 = c2;
			c2 = c3;
		}
		slab* newc2 = c1->child2;
		*r = c1;
		c1->parent = r;
		c1->child2 = c2;
		c2->parent = &c1->child2;
		s = c1;
		c1 = s->child1;
		c2 = newc2;
		r = &s->child1;
	}
}	
void RSymbianDLHeap::tree_insert(slab* s,slab** r)
	{
		slab* n = *r;
		for (;;)
		{
			if (!n)
			{	// tree empty
				*r = s;
				s->parent = r;
				s->child1 = s->child2 = 0;
				break;
			}
			if (s < n)
			{	// insert between parent and n
				*r = s;
				s->parent = r;
				s->child1 = n;
				s->child2 = 0;
				n->parent = &s->child1;
				break;
			}
			slab* c1 = n->child1;
			slab* c2 = n->child2;
			if ((c1 - 1) > (c2 - 1))
			{
				r = &n->child1;
				n = c1;
			}
			else
			{
				r = &n->child2;
				n = c2;
			}
		}
	}
void* RSymbianDLHeap::allocnewslab(slabset& allocator)
//
// Acquire and initialise a new slab, returning a cell from the slab
// The strategy is:
// 1. Use the lowest address free slab, if available. This is done by using the lowest slab
//    in the page at the root of the partial_page heap (which is address ordered). If the
//    is now fully used, remove it from the partial_page heap.
// 2. Allocate a new page for slabs if no empty slabs are available
//
{
	page* p = page::pagefor(partial_page);
	if (!p)
		return allocnewpage(allocator);

	unsigned h = p->slabs[0].header;
	unsigned pagemap = header_pagemap(h);
	ASSERT(&p->slabs[hibit(pagemap)] == partial_page);

	unsigned slabix = lowbit(pagemap);
	p->slabs[0].header = h &~ (0x100<<slabix);
	if (!(pagemap &~ (1<<slabix)))
	{
		tree_remove(partial_page);	// last free slab in page
	}
	return allocator.initslab(&p->slabs[slabix]);
}

/**Defination of this functionis not there in proto code***/
#if 0
void RSymbianDLHeap::partial_insert(slab* s)
	{
		// slab has had first cell freed and needs to be linked back into partial tree
		slabset& ss = slaballoc[sizemap[s->clz]];

		ASSERT(s->used == slabfull);
		s->used = ss.fulluse - s->clz;		// full-1 loading
		tree_insert(s,&ss.partial);
		checktree(ss.partial);
	}
/**Defination of this functionis not there in proto code***/
#endif

void* RSymbianDLHeap::allocnewpage(slabset& allocator)
//
// Acquire and initialise a new page, returning a cell from a new slab
// The partial_page tree is empty (otherwise we'd have used a slab from there)
// The partial_page link is put in the highest addressed slab in the page, and the
// lowest addressed slab is used to fulfill the allocation request
//
{
	page* p	 = spare_page;
	if (p)
		spare_page = 0;
	else
	{
		p = static_cast<page*>(map(0,pagesize));
		if (!p)
			return 0;
	}
	
	ASSERT(p == floor(p,pagesize));
	p->slabs[0].header = ((1<<3) + (1<<2) + (1<<1))<<8;		// set pagemap
	p->slabs[3].parent = &partial_page;
	p->slabs[3].child1 = p->slabs[3].child2 = 0;
	partial_page = &p->slabs[3];
	return allocator.initslab(&p->slabs[0]);
}

void RSymbianDLHeap::freepage(page* p)
//
// Release an unused page to the OS
// A single page is cached for reuse to reduce thrashing
// the OS allocator.
//
{
	ASSERT(ceiling(p,pagesize) == p);
	if (!spare_page)
	{
		spare_page = p;
		return;
	}
	unmap(p,pagesize);
}

void RSymbianDLHeap::freeslab(slab* s)
//
// Release an empty slab to the slab manager
// The strategy is:
// 1. The page containing the slab is checked to see the state of the other slabs in the page by
//    inspecting the pagemap field in the header of the first slab in the page.
// 2. The pagemap is updated to indicate the new unused slab
// 3. If this is the only unused slab in the page then the slab header is used to add the page to
//    the partial_page tree/heap
// 4. If all the slabs in the page are now unused the page is release back to the OS
// 5. If this slab has a higher address than the one currently used to track this page in
//    the partial_page heap, the linkage is moved to the new unused slab
//
{
	tree_remove(s);
	checktree(*s->parent);
	ASSERT(header_usedm4(s->header) == header_size(s->header)-4);
	CHECK(s->header |= 0xFF00000);			// illegal value for debug purposes
	page* p = page::pagefor(s);
	unsigned h = p->slabs[0].header;
	int slabix = s - &p->slabs[0];
	unsigned pagemap = header_pagemap(h);
	p->slabs[0].header = h | (0x100<<slabix);
	if (pagemap == 0)
	{	// page was full before, use this slab as link in empty heap
		tree_insert(s, &partial_page);
	}
	else
	{	// find the current empty-link slab
		slab* sl = &p->slabs[hibit(pagemap)];
		pagemap ^= (1<<slabix);
		if (pagemap == 0xf)
		{	// page is now empty so recycle page to os
			tree_remove(sl);
			freepage(p);
			return;
		}
		// ensure the free list link is in highest address slab in page
		if (s > sl)
		{	// replace current link with new one. Address-order tree so position stays the same
			slab** r = sl->parent;
			slab* c1 = sl->child1;
			slab* c2 = sl->child2;
			s->parent = r;
			s->child1 = c1;
			s->child2 = c2;
			*r = s;
			if (c1)
				c1->parent = &s->child1;
			if (c2)
				c2->parent = &s->child2;
		}
		CHECK(if (s < sl) s=sl);
	}
	ASSERT(header_pagemap(p->slabs[0].header) != 0);
	ASSERT(hibit(header_pagemap(p->slabs[0].header)) == unsigned(s - &p->slabs[0]));
}

void RSymbianDLHeap::slab_init(unsigned slabbitmap)
{
	ASSERT((slabbitmap & ~okbits) == 0);
	ASSERT(maxslabsize <= 60);

	slab_threshold=0;
	partial_page = 0;
	unsigned char ix = 0xff;
	unsigned bit = 1<<((maxslabsize>>2)-1);
	for (int sz = maxslabsize; sz >= 0; sz -= 4, bit >>= 1)
	{
		if (slabbitmap & bit)
		{
			if (++ix == 0)
				slab_threshold=sz+1;
			slabset& c = slaballoc[ix];
			c.size = sz;
			c.partial = 0;
		}
		sizemap[sz>>2] = ix;
	}
	
    free_chunk_threshold = pad_request(slab_threshold);
}

void* RSymbianDLHeap::slab_allocate(slabset& ss)
//
// Allocate a cell from the given slabset
// Strategy:
// 1. Take the partially full slab at the top of the heap (lowest address).
// 2. If there is no such slab, allocate from a new slab
// 3. If the slab has a non-empty freelist, pop the cell from the front of the list and update the slab
// 4. Otherwise, if the slab is not full, return the cell at the end of the currently used region of
//    the slab, updating the slab
// 5. Otherwise, release the slab from the partial tree/heap, marking it as 'floating' and go back to
//    step 1
//
{
	for (;;)
	{
		slab *s = ss.partial;
		if (!s)
			break;
		unsigned h = s->header;
		unsigned free = h & 0xff;		// extract free cell positiong
		if (free)
		{
			ASSERT(((free<<2)-sizeof(slabhdr))%header_size(h) == 0);
			void* p = offset(s,free<<2);
			free = *(unsigned char*)p;	// get next pos in free list
			h += (h&0x3C000)<<6;		// update usedm4
			h &= ~0xff;
			h |= free;					// update freelist
			s->header = h;
			ASSERT(header_free(h) == 0 || ((header_free(h)<<2)-sizeof(slabhdr))%header_size(h) == 0);
			ASSERT(header_usedm4(h) <= 0x3F8u);
			ASSERT((header_usedm4(h)+4)%header_size(h) == 0);
			return p;
		}
		unsigned h2 = h + ((h&0x3C000)<<6);
		if (h2 < 0xfc00000)
		{
			ASSERT((header_usedm4(h2)+4)%header_size(h2) == 0);
			s->header = h2;
			return offset(s,(h>>18) + sizeof(unsigned) + sizeof(slabhdr));
		}
		h |= 0x80000000;				// mark the slab as full-floating
		s->header = h;
		tree_remove(s);
		checktree(ss.partial);
		// go back and try the next slab...
	}
	// no partial slabs found, so allocate from a new slab
	return allocnewslab(ss);
}
void RSymbianDLHeap::slab_free(void* p)
//
// Free a cell from the slab allocator
// Strategy:
// 1. Find the containing slab (round down to nearest 1KB boundary)
// 2. Push the cell into the slab's freelist, and update the slab usage count
// 3. If this is the last allocated cell, free the slab to the main slab manager
// 4. If the slab was full-floating then insert the slab in it's respective partial tree
//
{
	ASSERT(lowbits(p,3)==0);
	slab* s = slab::slabfor(p);

	unsigned pos = lowbits(p, slabsize);
	unsigned h = s->header;
	ASSERT(header_usedm4(h) != 0x3fC);		// slab is empty already
	ASSERT((pos-sizeof(slabhdr))%header_size(h) == 0);
	*(unsigned char*)p = (unsigned char)h;
	h &= ~0xFF;
	h |= (pos>>2);
	unsigned size = h & 0x3C000;
	if (int(h) >= 0)
	{
		h -= size<<6;
		if (int(h)>=0)
		{
			s->header = h;
			return;
		}
		freeslab(s);
		return;
	}
	h -= size<<6;
	h &= ~0x80000000;
	s->header = h;
	slabset& ss = slaballoc[sizemap[size>>14]];
	tree_insert(s,&ss.partial);
	checktree(ss.partial);
}

void* slabset::initslab(slab* s)
//
// initialise an empty slab for this allocator and return the fist cell
// pre-condition: the slabset has no partial slabs for allocation
//
{
	ASSERT(partial==0);
	unsigned h = s->header & 0xF00;	// preserve pagemap only
	h |= (size<<12);					// set size
	h |= (size-4)<<18;					// set usedminus4 to one object minus 4
	s->header = h;
	partial = s;
	s->parent = &partial;
	s->child1 = s->child2 = 0;
	return offset(s,sizeof(slabhdr));
}

TAny* RSymbianDLHeap::SetBrk(TInt32 aDelta)
{
	if (iFlags & EFixedSize)
		return MFAIL;

	if (aDelta < 0)
		{
		unmap(offset(iTop, aDelta), -aDelta);
		}
	else if (aDelta > 0)
		{
		if (!map(iTop, aDelta))
			return MFAIL;
		}
	void * p =iTop;
	iTop = offset(iTop, aDelta);
	
	return p;
}

void* RSymbianDLHeap::map(void* p,unsigned sz)
//
// allocate pages in the chunk
// if p is NULL, find an allocate the required number of pages (which must lie in the lower half)
// otherwise commit the pages specified
//
{
ASSERT(p == floor(p, pagesize));
ASSERT(sz == ceiling(sz, pagesize));
ASSERT(sz > 0);

	if (iChunkSize + sz > iMaxLength)
		return 0;

	RChunk chunk;
	chunk.SetHandle(iChunkHandle);
	if (p)
	{
		TInt r = chunk.Commit(iOffset + ptrdiff(p, this),sz);
		if (r < 0)
			return 0;
		iChunkSize += sz;	
		return p;
	}

	TInt r = chunk.Allocate(sz);
	if (r < 0)
		return 0;
	if (r > iOffset)
	{
		// can't allow page allocations in DL zone
		chunk.Decommit(r, sz);
		return 0;
	}
	iChunkSize += sz;
	
	TraceChunkUsage(iChunkHandle, iBase, iChunkSize);
	
	return offset(this, r - iOffset);
}

void* RSymbianDLHeap::remap(void* p,unsigned oldsz,unsigned sz)
{
	if (oldsz > sz)
		{	// shrink
		unmap(offset(p,sz), oldsz-sz);
		}
	else if (oldsz < sz)
		{	// grow, try and do this in place first
		if (!map(offset(p, oldsz), sz-oldsz))
			{
			// need to allocate-copy-free
			void* newp = map(0, sz);
			memcpy(newp, p, oldsz);
			unmap(p,oldsz);
			return newp;
			}
		}
	return p;
}

void RSymbianDLHeap::unmap(void* p,unsigned sz)
{
	ASSERT(p == floor(p, pagesize));
	ASSERT(sz == ceiling(sz, pagesize));
	ASSERT(sz > 0);

	RChunk chunk;
	chunk.SetHandle(iChunkHandle);
	TInt r = chunk.Decommit(ptrdiff(p, offset(this,-iOffset)), sz);
	//TInt offset = (TUint8*)p-(TUint8*)chunk.Base();
	//TInt r = chunk.Decommit(offset,sz);

	ASSERT(r >= 0);
	iChunkSize -= sz;
	
	TraceChunkUsage(iChunkHandle, iBase, iChunkSize);
}

void RSymbianDLHeap::paged_init(unsigned pagepower)
	{
		if (pagepower == 0)
			pagepower = 31;
		else if (pagepower < minpagepower)
			pagepower = minpagepower;
		page_threshold = pagepower;
		for (int i=0;i<npagecells;++i)
		{
			pagelist[i].page = 0;
			pagelist[i].size = 0;
		}
	}

void* RSymbianDLHeap::paged_allocate(unsigned size)
{
	unsigned nbytes = ceiling(size, pagesize);
	if (nbytes < size + cellalign)
	{	// not enough extra space for header and alignment, try and use cell list
		for (pagecell *c = pagelist,*e = c + npagecells;c < e;++c)
			if (c->page == 0)
			{
				void* p = map(0, nbytes);
				if (!p)
					return 0;
				c->page = p;
				c->size = nbytes;
				return p;
			}
	}
	// use a cell header
	nbytes = ceiling(size + cellalign, pagesize);
	void* p = map(0, nbytes);
	if (!p)
		return 0;
	*static_cast<unsigned*>(p) = nbytes;
	return offset(p, cellalign);
}

void* RSymbianDLHeap::paged_reallocate(void* p, unsigned size)
{
	if (lowbits(p, pagesize) == 0)
	{	// continue using descriptor
		pagecell* c = paged_descriptor(p);
		unsigned nbytes = ceiling(size, pagesize);
		void* newp = remap(p, c->size, nbytes);
		if (!newp)
			return 0;
		c->page = newp;
		c->size = nbytes;
		return newp;
	}
	else
	{	// use a cell header
		ASSERT(lowbits(p,pagesize) == cellalign);
		p = offset(p,-int(cellalign));
		unsigned nbytes = ceiling(size + cellalign, pagesize);
		unsigned obytes = *static_cast<unsigned*>(p);
		void* newp = remap(p, obytes, nbytes);
		if (!newp)
			return 0;
		*static_cast<unsigned*>(newp) = nbytes;
		return offset(newp, cellalign);
	}
}

void RSymbianDLHeap::paged_free(void* p)
{
	if (lowbits(p,pagesize) == 0)
	{	// check pagelist
		pagecell* c = paged_descriptor(p);
		unmap(p, c->size);
		c->page = 0;
		c->size = 0;
	}
	else
	{	// check page header
		unsigned* page = static_cast<unsigned*>(offset(p,-int(cellalign)));
		unsigned size = *page;
		unmap(page,size);
	}
}

pagecell* RSymbianDLHeap::paged_descriptor(const void* p) const
{
	ASSERT(lowbits(p,pagesize) == 0);
	// Double casting to keep the compiler happy. Seems to think we can trying to
	// change a non-const member (pagelist) in a const function
	pagecell* c = (pagecell*)((void*)pagelist);
	pagecell* e = c + npagecells;
	for (;;)
	{
		ASSERT(c!=e);
		if (c->page == p)
			return c;
		++c;
	}
}

/* Only for debugging purpose - start*/
#ifdef DL_CHUNK_MEM_DEBUG
void RSymbianDLHeap::debug_check_small_chunk_access(mchunkptr p, size_t psize)
{
    size_t sz = chunksize(p);
    char ch = *((char*)chunk_plus_offset(p, psize-1));
}

void RSymbianDLHeap::debug_check_any_chunk_access(mchunkptr p, size_t psize)
{
    if(p==0 || psize==0) return;
    
    mchunkptr next = chunk_plus_offset(p, psize);
    char* t = (char*)chunk_plus_offset(p, mparams.page_size);
    char ch = *((char*)p);
    while((size_t)t<(size_t)next)
    {
        ch = *t;
        t = (char*)chunk_plus_offset(t, mparams.page_size);
    };
}

void RSymbianDLHeap::debug_check_large_chunk_access(tchunkptr p, size_t psize)
{
    mchunkptr next = chunk_plus_offset(p, psize);
    char* t = (char*)chunk_plus_offset(p, mparams.page_size);
    char ch = *((char*)p);
    while((size_t)t<(size_t)next)
    {
        ch = *t;
        t = (char*)chunk_plus_offset(t, mparams.page_size);
    };
}

void RSymbianDLHeap::debug_chunk_page_release_check(mchunkptr p, size_t psize, mstate fm, int mem_released)
{
    if(mem_released)
    {        
        if(!page_not_in_memory(p, psize) )
            MEM_LOG("CHUNK_PAGE_ERROR::dlfree, error - page_in_mem flag is corrupt");          
        if(chunk_plus_offset(p, psize) > fm->top)
            MEM_LOG("CHUNK_PAGE_ERROR: error Top chunk address invalid");
        if(fm->dv >= p && fm->dv < chunk_plus_offset(p, psize))
            MEM_LOG("CHUNK_PAGE_ERROR: error DV chunk address invalid");    
    }
}
#endif

#ifdef OOM_LOGGING
#include <hal.h>
void RSymbianDLHeap::dump_large_chunk(mstate m, tchunkptr t) {
    tchunkptr u = t;
    bindex_t tindex = t->index;
    size_t tsize = chunksize(t);
    bindex_t idx;
    compute_tree_index(tsize, idx);

    size_t free = 0;
    int nfree = 0;
    do
        {   /* traverse through chain of same-sized nodes */
        if (u->child[0] != 0)
            {
            dump_large_chunk(m, u->child[0]);
            }

        if (u->child[1] != 0)
            {
            dump_large_chunk(m, u->child[1]);
            }

        free += chunksize(u);
        nfree++;
        u = u->fd;
        }
    while (u != t);
    C_LOGF(_L8("LARGE_BIN,%d,%d,%d"), tsize, free, nfree);
}

void RSymbianDLHeap::dump_dl_free_chunks()
{ 
    C_LOG("");    
    C_LOG("------------ dump_dl_free_chunks start -------------");
    C_LOG("BinType,BinSize,FreeSize,FreeCount");
    
    // dump small bins
    for (int i = 0; i < NSMALLBINS; ++i)
        {
        sbinptr b = smallbin_at(gm, i);
        unsigned int empty = (gm->smallmap & (1 << i)) == 0;
        int nfree = 0;
        if (!empty)
            {
            int nfree = 0;
            size_t free = 0;
            mchunkptr p = b->bk;
            size_t size = chunksize(p);
            for (; p != b; p = p->bk)
                {
                free += chunksize(p);
                nfree++;
                }

            C_LOGF(_L8("SMALL_BIN,%d,%d,%d"), size, free, nfree);
            }
        }

    // dump large bins
    for (int i = 0; i < NTREEBINS; ++i)
        {
        tbinptr* tb = treebin_at(gm, i);
        tchunkptr t = *tb;
        int empty = (gm->treemap & (1 << i)) == 0;
        if (!empty)
            dump_large_chunk(gm, t);
        }

    C_LOG("------------ dump_dl_free_chunks end -------------");
    C_LOG("");
    }

void RSymbianDLHeap::dump_heap_logs(size_t fail_size)
{
    MEM_LOG("");
    if (fail_size) {
        MEM_LOG("MEMDEBUG::RSymbianDLHeap OOM Log dump *************** start");
        MEM_LOGF(_L8("Failing to alloc size: %d"), fail_size);
    }
    else
        MEM_LOG("MEMDEBUG::RSymbianDLHeap Log dump *************** start");
    
    TInt dl_chunk_size = ptrdiff(iTop,iBase);
    TInt slabp_chunk_size = iChunkSize + iUnmappedChunkSize - dl_chunk_size;
    TInt freeMem = 0;    
    HAL::Get(HALData::EMemoryRAMFree, freeMem);
    MEM_LOGF(_L8("System Free RAM Size: %d"), freeMem);
    MEM_LOGF(_L8("Allocator Commited Chunk Size: %d"), iChunkSize);
    MEM_LOGF(_L8("DLHeap Arena Size=%d"), dl_chunk_size);
    MEM_LOGF(_L8("DLHeap unmapped chunk size: %d"), iUnmappedChunkSize);
    MEM_LOGF(_L8("Slab-Page Allocator Chunk Size=%d"), slabp_chunk_size);
    
    mallinfo info = dlmallinfo();   
    TUint heapAlloc = info.uordblks;
    TUint heapFree = info.fordblks;
    MEM_LOGF(_L8("DLHeap allocated size: %d"), heapAlloc);
    MEM_LOGF(_L8("DLHeap free size: %d"), heapFree);
        
    if (fail_size) {
        MEM_LOG("MEMDEBUG::RSymbianDLHeap OOM Log dump *************** end");
    }else {
        MEM_LOG("MEMDEBUG::RSymbianDLHeap Log dump *************** end");
    }
    MEM_LOG("");
}

#endif
/* Only for debugging purpose - end*/

#endif
