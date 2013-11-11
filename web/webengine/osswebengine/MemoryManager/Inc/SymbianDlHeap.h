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

#ifndef SYMBIANDLHEAP_H_
#define SYMBIANDLHEAP_H_

/**
@publishedAll
@released

Represents the default implementation for a heap.

The default implementation uses an address-ordered first fit type algorithm.

The heap itself is contained in a chunk and may be the only occupant of the 
chunk or may share the chunk with the program stack.

The class contains member functions for allocating, adjusting, freeing individual 
cells and generally managing the heap.

The class is not a handle in the same sense that RChunk is a handle; i.e. 
there is no Kernel object which corresponds to the heap.
*/
#include "DLA.h"

#undef UIMPORT_C
#define UIMPORT_C 

NONSHARABLE_CLASS(RSymbianDLHeap) : public RAllocator
	{
public:
	enum{EAllocCellSize = 8};
#ifndef __MANUALLY_INCLUDED_RSymbianDLHeap_CPP__
	UIMPORT_C virtual TAny* Alloc(TInt aSize);
	UIMPORT_C virtual void Free(TAny* aPtr);
	UIMPORT_C virtual TAny* ReAlloc(TAny* aPtr, TInt aSize, TInt aMode=0);
	UIMPORT_C virtual TInt AllocLen(const TAny* aCell) const;
	UIMPORT_C virtual TInt Compress();
	UIMPORT_C virtual void Reset();
	UIMPORT_C virtual TInt AllocSize(TInt& aTotalAllocSize) const;
	UIMPORT_C virtual TInt Available(TInt& aBiggestBlock) const;
	UIMPORT_C virtual TInt DebugFunction(TInt aFunc, TAny* a1=NULL, TAny* a2=NULL);
	
protected:
	UIMPORT_C virtual TInt Extension_(TUint aExtensionId, TAny*& a0, TAny* a1);
#else
	 virtual TAny* Alloc(TInt aSize);
	 virtual void Free(TAny* aPtr);
	 virtual TAny* ReAlloc(TAny* aPtr, TInt aSize, TInt aMode=0);
	 virtual TInt AllocLen(const TAny* aCell) const;
	 virtual TInt Compress();
	 virtual void Reset();
	 virtual TInt AllocSize(TInt& aTotalAllocSize) const;
	 virtual TInt Available(TInt& aBiggestBlock) const;
	 virtual TInt DebugFunction(TInt aFunc, TAny* a1=NULL, TAny* a2=NULL);
protected:
	 virtual TInt Extension_(TUint aExtensionId, TAny*& a0, TAny* a1);
#endif
	
public:
	TInt Size() const
	{ return iChunkSize; }
	
	inline TInt MaxLength() const;
	inline TUint8* Base() const;
	inline TInt Align(TInt a) const;
	inline const TAny* Align(const TAny* a) const;
	//inline TBool IsLastCell(const SCell* aCell) const;
	inline void Lock() const;
	inline void Unlock() const;
	inline TInt ChunkHandle() const;

    /**
    @internalComponent
    */
	struct _s_align {char c; double d;};

    /**
    The structure of a heap cell header for a heap cell on the free list.
    */
	struct SCell {
	             /**
	             The length of the cell, which includes the length of
	             this header.
	             */
	             TInt len; 
	             
	             
	             /**
	             A pointer to the next cell in the free list.
	             */
	             SCell* next;
	             };

	/** 
    The default cell alignment.
    */
	enum {ECellAlignment = sizeof(_s_align)-sizeof(double)};
			
	/**
	Size of a free cell header.
	*/
	enum {EFreeCellSize = sizeof(SCell)};

    /**
    @internalComponent
    */
    enum TDefaultShrinkRatios {EShrinkRatio1=256, EShrinkRatioDflt=512};

public:
	UIMPORT_C RSymbianDLHeap(TInt aMaxLength, TInt aAlign=0, TBool aSingleThread=ETrue);
	UIMPORT_C RSymbianDLHeap(TInt aChunkHandle, TInt aOffset, TInt aMinLength, TInt aMaxLength, TInt aGrowBy, TInt aAlign=0, TBool aSingleThread=EFalse);
	inline RSymbianDLHeap();

	TAny* operator new(TUint aSize, TAny* aBase) __NO_THROW;
	inline void operator delete(TAny*, TAny*);
	
protected:
	UIMPORT_C SCell* GetAddress(const TAny* aCell) const;

public:
	TInt iMinLength;
	TInt iMaxLength;			// maximum bytes used by the allocator in total
	TInt iOffset;					// offset of RSymbianDLHeap object from chunk base
	TInt iGrowBy;

	TInt iChunkHandle;			// handle of chunk
	RFastLock iLock;
	TUint8* iBase;				// bottom of DL memory, i.e. this+sizeof(RSymbianDLHeap)
	TUint8* iTop;					// top of DL memory (page aligned)
	TInt iAlign;
	TInt iMinCell;
	TInt iPageSize;
	SCell iFree;
protected:
	TInt iNestingLevel;
	TInt iAllocCount;
	TAllocFail iFailType;
	TInt iFailRate;
	TBool iFailed;
	TInt iFailAllocCount;
	TInt iRand;
	TAny* iTestData;
protected:
	TInt iChunkSize;				// currently allocated bytes in the chunk (== chunk.Size())
	malloc_state iGlobalMallocState;
	malloc_params mparams;
private:
	void Init(TInt aBitmapSlab, TInt aPagePower, size_t aTrimThreshold);/*Init internal data structures*/
	inline int init_mparams(size_t aTrimThreshold /*= DEFAULT_TRIM_THRESHOLD*/);		
	inline void init_bins(mstate m);
	inline void init_top(mstate m, mchunkptr p, size_t psize);
	void* sys_alloc(mstate m, size_t nb);
	msegmentptr segment_holding(mstate m, TUint8* addr);
	void add_segment(mstate m, TUint8* tbase, size_t tsize, flag_t mmapped);
	int sys_trim(mstate m, size_t pad);
	int has_segment_link(mstate m, msegmentptr ss);
	size_t release_unused_segments(mstate m);		
	void* mmap_alloc(mstate m, size_t nb);/*Need to check this function*/		
	void* prepend_alloc(mstate m, TUint8* newbase, TUint8* oldbase, size_t nb);				
	void* tmalloc_large(mstate m, size_t nb);
	void* tmalloc_small(mstate m, size_t nb);
	/*MACROS converted functions*/
	static inline void unlink_first_small_chunk(mstate M,mchunkptr B,mchunkptr P,bindex_t& I);
	static inline void insert_small_chunk(mstate M,mchunkptr P, size_t S);
	static inline void insert_chunk(mstate M,mchunkptr P,size_t S,size_t NPAGES);
	static inline void unlink_large_chunk(mstate M,tchunkptr X);
	static inline void unlink_small_chunk(mstate M, mchunkptr P,size_t S);
	static inline void unlink_chunk(mstate M, mchunkptr P, size_t S);
	static inline void compute_tree_index(size_t S, bindex_t& I);
	static inline void insert_large_chunk(mstate M,tchunkptr X,size_t S,size_t NPAGES);
	static inline void replace_dv(mstate M, mchunkptr P, size_t S);
	static inline void compute_bit2idx(binmap_t X,bindex_t& I);
	/*MACROS converted functions*/
	TAny* SetBrk(TInt32 aDelta);
	void* internal_realloc(mstate m, void* oldmem, size_t bytes);
	void  internal_malloc_stats(mstate m);
	int change_mparam(int param_number, int value);
#if !NO_MALLINFO
		mallinfo internal_mallinfo(mstate m);
#endif
	void Init_Dlmalloc(size_t capacity, int locked, size_t aTrimThreshold);
	void* dlmalloc(size_t);
	void  dlfree(void*);
	void* dlrealloc(void*, size_t);
	int dlmallopt(int, int);
	size_t dlmalloc_footprint(void);
	size_t dlmalloc_max_footprint(void);
	#if !NO_MALLINFO
		struct mallinfo dlmallinfo(void);
	#endif
	int  dlmalloc_trim(size_t);
	size_t dlmalloc_usable_size(void*);
	void  dlmalloc_stats(void);		
	inline	mchunkptr mmap_resize(mstate m, mchunkptr oldp, size_t nb);

		/****************************Code Added For DL heap**********************/			
	friend class CNewSymbianHeapPool;
private:
	unsigned short slab_threshold;
	unsigned short page_threshold;		// 2^n is smallest cell size allocated in paged allocator
	slab* partial_page;// partial-use page tree
	page* spare_page;					// single empty page cached
	unsigned char sizemap[(maxslabsize>>2)+1];	// index of slabset based on size class
private:
	static void tree_remove(slab* s);
	static void tree_insert(slab* s,slab** r);
public:
	enum {okbits = (1<<(maxslabsize>>2))-1};
	void slab_init(unsigned slabbitmap);
	void* slab_allocate(slabset& allocator);
	void slab_free(void* p);
	void* allocnewslab(slabset& allocator);
	void* allocnewpage(slabset& allocator);
	void freeslab(slab* s);
	void freepage(page* p);
	void* map(void* p,unsigned sz);
	void* remap(void* p,unsigned oldsz,unsigned sz);
	void unmap(void* p,unsigned sz);
	/**I think we need to move this functions to slab allocator class***/
	static inline unsigned header_free(unsigned h) 
	{return (h&0x000000ff);}
	static inline unsigned header_pagemap(unsigned h) 
	{return (h&0x00000f00)>>8;}
	static inline unsigned header_size(unsigned h) 
	{return (h&0x0003f000)>>12;}
	static inline unsigned header_usedm4(unsigned h) 
	{return (h&0x0ffc0000)>>18;}
	/***paged allocator code***/
	void paged_init(unsigned pagepower);	
	void* paged_allocate(unsigned size);
	void paged_free(void* p);	
	void* paged_reallocate(void* p, unsigned size);
	pagecell* paged_descriptor(const void* p) const ;

	/* Dl heap log dump functions*/
#ifdef OOM_LOGGING    
    void dump_heap_logs(size_t fail_size);
    void dump_dl_free_chunks();
    void dump_large_chunk(mstate m, tchunkptr t);
    size_t iUnmappedChunkSize;
#endif
private:
     /* Dubug checks for chunk page support*/
#ifdef DL_CHUNK_MEM_DEBUG
#define do_chunk_page_release_check(p, psize, fm, mem_released) debug_chunk_page_release_check(p, psize, fm, mem_released)
#define do_check_large_chunk_access(p, psize) debug_check_large_chunk_access(p, psize)
#define do_check_small_chunk_access(p, psize) debug_check_small_chunk_access(p, psize)
#define do_check_any_chunk_access(p, psize) debug_check_any_chunk_access(p, psize)    
    void debug_check_large_chunk_access(tchunkptr p, size_t psize);
    void debug_check_small_chunk_access(mchunkptr p, size_t psize);
    void debug_check_any_chunk_access(mchunkptr p, size_t psize);
    void debug_chunk_page_release_check(mchunkptr p, size_t psize, mstate fm, int mem_released);    
#else
#define do_chunk_page_release_check(p, psize, fm, mem_released)
#define do_check_large_chunk_access(p, psize)
#define do_check_small_chunk_access(p, psize)
#define do_check_any_chunk_access(p, psize)    
#endif
    
    /* Chunk page release mechanism support */
    TInt map_chunk_pages(tchunkptr p, size_t psize);
    TInt unmap_chunk_pages(tchunkptr p, size_t psize, size_t prev_npages);
    TInt map_chunk_pages_partial(tchunkptr tp, size_t psize, tchunkptr r, size_t rsize);
    TInt sys_trim_partial(mstate m, mchunkptr prev, size_t psize, size_t prev_npages);    
    size_t free_chunk_threshold;
    
	// paged allocator structures
	enum {npagecells=4};
	pagecell pagelist[npagecells];		// descriptors for page-aligned large allocations
	/**
	 *always keep this variable at the bottom of the class as its used as
	 * array of more than 1 objest though it declared a single.
	 *  TODO: dynamic sizing of heap object based on slab configuration.
	 *             Just allocate maximum number of slabsets for now
	 * */
	slabset slaballoc[15];
	};


inline void RSymbianDLHeap::Lock() const
/**
@internalComponent
*/
	{((RFastLock&)iLock).Wait();}




inline void RSymbianDLHeap::Unlock() const
/**
@internalComponent
*/
	{((RFastLock&)iLock).Signal();}


inline TInt RSymbianDLHeap::ChunkHandle() const
/**
@internalComponent
*/
	{
	return iChunkHandle;
	}

inline void RSymbianDLHeap::operator delete(TAny*, TAny*) 
/**
Called if constructor issued by operator new(TUint aSize, TAny* aBase) throws exception.
This is dummy as corresponding new operator does not allocate memory.
*/
	{}

#endif /* SYMBIANDLHEAP_H_ */
