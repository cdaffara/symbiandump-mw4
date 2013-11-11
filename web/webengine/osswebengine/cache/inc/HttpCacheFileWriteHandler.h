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
* Description:  Implementation of CHttpCacheFileWriteHandler
*
*/

#ifndef CHTTPCACHEFILEWRITEHANDLER_H
#define CHTTPCACHEFILEWRITEHANDLER_H

//  INCLUDES
#include <e32base.h>
#include <f32file.h>
#include "HttpCacheHandler.h"
#include "HttpCacheEntry.h"
#include "MemoryManager.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES
class CAsyncFileWriter;
class CHttpCacheWriteTimeout;

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*
*  @lib
*  @since 5.0
*/
NONSHARABLE_CLASS(CHttpCacheFileWriteHandler) : public CActive, public MMemoryCollector, public MHttpCacheEntryDeleteObserver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @since 5.0
        * @param
        * @param
        * @return CacheFileWriteHandler object.
        */
        static CHttpCacheFileWriteHandler* NewL(CHttpCacheHandler* aManager, CHttpCacheStreamHandler* aStreamHandler, RFs& aRfs, const THttpCachePostponeParameters& aParams);

        /**
        * Destructor.
        */
        virtual ~CHttpCacheFileWriteHandler();

    public: // From MMemoryCollector
        /**
        * Collect free memory, this function is called when allocation from
        * System heap fails
        * @since 3.1
        * @param amount of memory needs to be collected
        * @return amount of memory collected
        */
        virtual TUint Collect(TUint aRequired);

        /**
        * restore the entity controlled memory collector when there is enough memory
        * System heap fails
        * @since 3.1
        * @param
        * @return
        */
        virtual void Restore();

        /**
        * Priority of this collector, 0 - lowest, 10 - highest;
        * the lower the priority, the earlier this collector is executed.
        * @since 3.1
        * @param
        * @return
        */
        virtual TOOMPriority Priority();

        /**
         *
         * @since 7.1
         * @param
         * @return
         */
        TBool IsCacheEntryPostponed(const CHttpCacheEntry* aEntry);

    public: // from MHttpCacheEntryDeleteObserver
        virtual void EntryDeleted(CHttpCacheEntry *aEntry); 
        
        
    public: // new functions
        enum TAddStatus {
            EAddedOk,
            EBodySmallerThanThreshold,
            ENotEnoughFreeMemory,
            ECheckReturn
        };

        /**
         * Add a cache entry to write out when possible
         *
         */
        TInt AddEntry(TAddStatus& aAddStatus, CHttpCacheEntry *aEntry);

        /**
         * Remove a stream entry from the list
         */
        CHttpCacheEntry* RemoveEntry(CHttpCacheEntry *aEntry);

        /**
         * Remove all stream entries from the list
         */
        void RemoveAll();

        /**
         * Emergency method if memory is short - write everything
         * to disk *now* and wait for it. Or maybe just dump it all
         * and delete the headers...
         */
        void DumpAllObjects();

        /**
         * Signal called by the timer callback to indicate that we
         * should begin writing out the cached data.
         */
        void BeginWriting();

    private:

        /**
        * Construct.
        * @since 5.0
        * @param
        * @param
        * @return CacheFileWriteHandler object.
        */
        CHttpCacheFileWriteHandler(CHttpCacheHandler* aHandler, CHttpCacheStreamHandler* aStreamHandler, RFs& aRfs);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(const THttpCachePostponeParameters& aParams);

        /**
         * from CActive
         */
        void DoCancel();

        /**
         * from CActive
         */
        void RunL();

        void ContinueFlushing();

        void OutputQueueContentToDebug();

        // sort by size function for arrays of CHttpCacheEntry objects.
        static TInt CompareHttpCacheEntrySize( const CHttpCacheEntry& aFirst, const CHttpCacheEntry& aSecond );

        void CollectMemory( TUint aRequired );

        /**
         * Callback function for timer to activate writing.
         */
        static TInt WriteTimeout(TAny* aParam);

    private:    // Data
        CHttpCacheHandler*                  iCacheHandler;
        CHttpCacheStreamHandler*            iCacheStreamHandler;
        TInt                                iFreeRamThreshold;
        TInt                                iImmediateWriteThreshold;
        TBool                               iLowMemoryState;
        RFs                                 iFs;
        CHttpCacheWriteTimeout*             iWaitTimer;
        // objects in this array are not owned.
        RPointerArray<CHttpCacheEntry>      iObjectQueue;
        // contains a pointer to the object that flush was called for
        CHttpCacheEntry*                    iObjectFlushing;
    };

#endif      // CHttpCacheFileWriteHandler_H

// End of File
