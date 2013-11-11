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
#ifndef __ALLOCATOR_H__
#define __ALLOCATOR_H__

//  INCLUDES

#include <e32base.h>
#include <e32std.h>
#include "MemoryManager.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FORWARD DECLARATIONS

class CStopScheduler;

struct TFreeMem
{
    TUint iPool;
    TUint iHeap;
    TUint iHal;
};

// CLASS DECLARATION

/**
*
*  @lib memman.lib
*  @since 3.1
*/
class CMemoryPool
  {
  public:
      virtual ~CMemoryPool();
      
    /**
      * create the allocator
      * @since 3.1
    * @param aSize bytes of memory to be allocated
      * @return pointer to the allocated memory
      */
    virtual TBool Create();
      
    /**
      * allocate a memory buffer
      * @since 3.1
      * @param aSize bytes of memory to be allocated
      * @return pointer to the allocated memory
      */
    virtual TAny* Allocate( TUint aSize ) = 0;
    
    /**
      * realloc a memory buffer
      * @since 3.1
    * @param aSize bytes of memory to be allocated
      * @return pointer to the allocated memory
      */
    virtual TAny* ReAllocate( TAny* aPtr, TUint aSize ) = 0;

    /**
      * free the memory
      * @since 3.1
    * @param aPtr pointer to the memory to be freed
    * @param 
      * @return 
      */
    virtual void Free( TAny* aPtr ) = 0;

    /**
      * get the size of a memory buffer
      * @since 3.1
    * @param aPtr pointer to the memory buffer
    * @param 
      * @return size of memory buffer
      */
    virtual TUint MemorySize( TAny* aPtr ) = 0;
    /**
      * set the size of rescue buffer
      * @since 3.1
      * @param aPtr pointer to the memory buffer
      * @param 
      * @return size of memory buffer
      */
    virtual void SetRescueBufferSize( TInt aSize ) = 0;
        
    /**
      * register a memory collector, memory manager doesn't own this collector
      * @since 3.1
    * @param aCollector the collector to be registered
    * @param 
      * @return 
      */
    void AddCollector( MMemoryCollector* aCollector );

    /**
      * unregister a memory collector
      * @since 3.1
    * @param aCollector the collector to be unregistered
    * @param 
      * @return 
      */
    void RemoveCollector( MMemoryCollector* aCollector );
    
    /**
      * collect free memory from memory collectors
      * @since 3.1
    * @param
      * @return 
      */    
    void CollectMemory(TUint aSize=-1);
    
    /**
      * restore the states of memory collectors
      * @since 3.1
    * @param
      * @return 
      */        
    void RestoreCollectors( TOOMPriority aPriority );
    
    /**
      * register a stopper, memory manager doesn't own this stopper.
      * @since 3.1
    * @param aStopper the stopper to be registered
    * @param 
      * @return 
      */
    void AddStopper( MOOMStopper* aStopper );

    /**
      * unregister a stopper
      * @since 3.1
    * @param aStopper the stopper to be unregistered
    * @param 
      * @return 
      */
    void RemoveStopper( MOOMStopper* aStopper );

    /**
      * toggle the stopping status of memory manager
      * @since 3.1
    * @param aStopping 
    * @param 
      * @return 
      */
    void SetStopping( TBool aStopping );
    
    /**
      * get the array of stoppers, used by stopscheduler
      * @since 3.1
    * @param 
    * @param 
      * @return an array of stoppers 
      */    
    RPointerArray<MOOMStopper>& Stoppers()        { return iStoppers; }
  
    /**
      * set the OOM notifier, memory manager doesn't own this notifier
      * @since 3.1
      * @param aNotifier the notifier
      * @param 
      * @return 
      */
    void SetNotifier( MOOMNotifier* aNotifier );
    
    /**
      * check if the memory manager is able to reserve enough memory for the coming operation.
      * @since 3.1
      * @param aTotalSize total amount of memory
      * @param aMaxBufSizse the biggest contiguous memory buffer
      * @param aChecker the name of operation
      * @return result of prechecking, ETrue = successful
      */
    virtual TBool PreCheck( TUint aTotalSize, TUint aMaxBufSize, const TDesC8& aChecker ) = 0;
    
    /**
      * An additional check after an operation is completed. 
      * @since 3.1
    * @param 
    * @param
      * @return the status of memory during this operation.
      */
    virtual TUint PostCheck() = 0;
      
    /**
      * Trigger an OOM event, this function is only for debugging purpose
      * @since 3.1
    * @param aType defines where memory allocation fails
    * @param
      * @return
      */
    void TriggerOOM( TOOMType aType );
          
    /**
      * get status of memory manager
      * @since 3.1
    * @param 
    * @param
      * @return the status
      */
    TUint Status()                    { return iMemStatus; }
    
    /**
    * 
    * @since 3.1 
    * @param 
    * @return 
    */   
    void SetStatus( TOOMCheckResult aStatus );

    /**
    * 
    * @since 3.1 
    * @param 
    * @return 
    */   
    TAny* AllocFromPool( TUint sz );    
    
    /**
    * 
    * @since 3.1 
    * @param 
    * @return 
    */   
    TUint DebugOOM()                  { return iOOMType; }

    /**
    * 
    * @since 3.1 
    * @param 
    * @return 
    */   
    virtual TUint FreeMemory( TFreeMem& aFree ) = 0;

    virtual void RestoreRescueBuffer() = 0;
    
  protected:
    CMemoryPool() : iNotifier( 0 )   {}
  
    /**
      * utility function, notify the user and stop ongoing operations
      * @since 3.1
      * @return 
      */
    void NotifyAndStop();
  
    /**
      * allocate a buffer
      * @since 3.1
    * @param aPtr the buffer to be allocated
    * @param
      * @return ETrue successfully freed
      */
    virtual TAny* DoAlloc( TUint aSize ) = 0;
    
    MOOMNotifier*           iNotifier; 
    TInt                    iNestedChecks;
    TBool                   iIsStopping;
    RPointerArray<MOOMStopper>  iStoppers;
    CStopScheduler*       iStopScheduler;
    
    TBool             iIsCollecting;
    RPointerArray<MMemoryCollector> iCollectors;
    TUint           iMemStatus;
    
    // for debugging
    TUint           iOOMType;
    };

/**
*  The memory pool using dlmalloc for fast allocation
*  @lib memman.lib
*  @since 3.1
*/
class CFastMemoryPool : public CMemoryPool
    {
    public:
        TAny* Allocate( TUint aSize )   { return AllocFromPool( aSize ); }
        TAny* ReAllocate( TAny* aPtr, TUint aSize );
        void Free( TAny* aPtr );
        TUint MemorySize( TAny* aPtr );
        void SetRescueBufferSize( TInt aSize );
        TAny* DoAlloc( TUint aSize );
        TBool PreCheck( TUint aTotalSize, TUint aMaxBufSize, const TDesC8& aChecker );
        TUint PostCheck();
        TUint FreeMemory( TFreeMem& aFree );
        void RestoreRescueBuffer();
    };

/**
*  The default memory pool only using User static allocating functions
*  @lib memman.lib
*  @since 3.1
*/
class CDefaultMemoryPool : public CMemoryPool
    {
    public:
        CDefaultMemoryPool() : iRescueBufferSize( 0 )       {}
        ~CDefaultMemoryPool();

        TAny* Allocate( TUint aSize )   { return AllocFromPool( aSize ); }
        TAny* ReAllocate( TAny* aPtr, TUint aSize );
        void Free( TAny* aPtr );
        TUint MemorySize( TAny* aPtr );
        void SetRescueBufferSize( TInt aSize );
        TAny* DoAlloc( TUint aSize );
        TBool PreCheck( TUint aTotalSize, TUint aMaxBufSize, const TDesC8& aChecker );
        TUint PostCheck();
        TUint FreeMemory( TFreeMem& aFree );
        void RestoreRescueBuffer();

    private:
        TUint iRescueBufferSize;
    };

class RSymbianDLHeap;

/**
*  The memory pool using new symbian DLMalloc/Slab hybrid allocator for fast allocation
*  @lib memman.lib
*  @since 3.1
*/
NONSHARABLE_CLASS(CNewSymbianHeapPool) : public CMemoryPool
    {
    public:
    	CNewSymbianHeapPool();
    	~CNewSymbianHeapPool();
    	
        TBool Create();
    	
        TAny* Allocate( TUint aSize )   { return AllocFromPool( aSize ); }
        TAny* ReAllocate( TAny* aPtr, TUint aSize );
        void Free( TAny* aPtr );
        TUint MemorySize( TAny* aPtr );
        void SetRescueBufferSize( TInt aSize );
        TAny* DoAlloc( TUint aSize );
        TBool PreCheck( TUint aTotalSize, TUint aMaxBufSize, const TDesC8& aChecker );
        TUint PostCheck();
        TUint FreeMemory( TFreeMem& aFree );
        void RestoreRescueBuffer();
#ifdef OOM_LOGGING         
        void DumpHeapLogs();
#endif        
    private:
    	void InitLocal();
    	
    	RSymbianDLHeap *iAlloc;
    };

#endif
