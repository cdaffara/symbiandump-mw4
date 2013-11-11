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
* Description:  Definition of CHttpCacheEvictionHandler
*
*/

#ifndef CHTTPCACHEEVICTIONHANDLER_H
#define CHTTPCACHEEVICTIONHANDLER_H

//  INCLUDES
#include <e32base.h>
#include "HttpCacheUtil.h"

// CONSTANTS

// MACROS

// FORWARD DECLARATIONS
class CHttpCacheEntry;

// DATA TYPES
typedef  TSglQue<CHttpCacheEntry> TBucket;
typedef  TSglQueIter<CHttpCacheEntry> TBucketIter;

// FUNCTION PROTOTYPES

// CLASS DECLARATION

/**
*
*  @lib
*  @since 3.1
*/
class CHttpCacheEvictionHandler : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @since 3.1
        * @param
        * @param
        * @return CHttpCacheEvictionHandler object.
        */
        static CHttpCacheEvictionHandler* NewL();

        /**
        * Destructor.
        */
        virtual ~CHttpCacheEvictionHandler();

    public: // new functions

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void Insert( CHttpCacheEntry& aCacheEntry );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void Accessed( CHttpCacheEntry& aCacheEntry );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void Remove( CHttpCacheEntry& aCacheEntry );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void RemoveAll();

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        CArrayPtrFlat<CHttpCacheEntry>* EvictL( TInt aSpaceNeeded );

    private:

        /**
        * Construct.
        * @since 3.1
        * @param
        * @param
        * @return CHttpCacheEvictionHandler object.
        */
        CHttpCacheEvictionHandler();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        TBucket* Bucket( TInt aBucketIndex );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        TInt BucketIndex( TInt aSizeFrequencyValue );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        TBool ItemIsInBucket( TInt aBucketIndex, const CHttpCacheEntry& aCacheEntry );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        TInt FastLog2( TUint aNum );

        /**
        * Used for debugging
        * @since 3.1
        * @param
        * @return
        */
        void LogBuckets();

    private:    // Data

        // array of 5 buckets each bucket is a linked list of
        // cached items
        CArrayPtrFlat<TBucket>*            iBuckets;          // owned
    };

#endif      // CHTTPCACHEEVICTIONHANDLER_H

// End of File
