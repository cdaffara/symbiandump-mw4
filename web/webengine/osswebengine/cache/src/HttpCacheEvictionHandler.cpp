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
* Description:  Implementation of CHttpCacheEvictionHandler
*
*/

// INCLUDE FILES
#include "HttpCacheEvictionHandler.h"
#include "HttpCacheEntry.h"
#include "HttpCacheUtil.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// from KHTML loader.cpp
#define FAST_LOG2(_log2,_n)   \
      unsigned int j_ = (unsigned int)(_n);   \
      (_log2) = 0;                    \
      if ((j_) & ((j_)-1))            \
      (_log2) += 1;               \
      if ((j_) >> 16)                 \
      (_log2) += 16, (j_) >>= 16; \
      if ((j_) >> 8)                  \
      (_log2) += 8, (j_) >>= 8;   \
      if ((j_) >> 4)                  \
      (_log2) += 4, (j_) >>= 4;   \
      if ((j_) >> 2)                  \
      (_log2) += 2, (j_) >>= 2;   \
      if ((j_) >> 1)                  \
      (_log2) += 1;

const TUint KHttpCacheBucketNum = 5;

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHttpCacheEvictionHandler::CHttpCacheEvictionHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CHttpCacheEvictionHandler::CHttpCacheEvictionHandler()
    {
    }

// -----------------------------------------------------------------------------
// CHttpCacheEvictionHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHttpCacheEvictionHandler::ConstructL()
    {
    // 5 buckets
    // LRU, size/nref = 1 to 1
    // LRU, size/nref = 2 to 3
    // LRU, size/nref = 4 to 7
    // LRU, size/nref = 8 to 15
    // LRU, size/nref > 15
    iBuckets = new(ELeave)CArrayPtrFlat<TBucket>( KHttpCacheBucketNum );
    for( TInt i = 0; i < KHttpCacheBucketNum; i++ )
        {
        TBucket* bucket = new(ELeave)TBucket( CHttpCacheEntry::iOffset );
        iBuckets->AppendL( bucket );
        TBucket* bucketi = iBuckets->At( i );
        }
    }

// -----------------------------------------------------------------------------
// CHttpCacheEvictionHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHttpCacheEvictionHandler* CHttpCacheEvictionHandler::NewL()
    {
    CHttpCacheEvictionHandler* self = new( ELeave ) CHttpCacheEvictionHandler();

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// Destructor
CHttpCacheEvictionHandler::~CHttpCacheEvictionHandler()
    {
    if (iBuckets)
        {
        // delete the entries
        for( TInt i=0; i<iBuckets->Count(); ++i )
            delete iBuckets->At( i );
        }
        delete iBuckets;
    }

// -----------------------------------------------------------------------------
// CHttpCacheEvictionHandler::InsertL
//
// -----------------------------------------------------------------------------
//
void CHttpCacheEvictionHandler::Insert( CHttpCacheEntry& aCacheEntry )
    {
#ifdef __CACHELOG__
    HttpCacheUtil::WriteUrlToLog( 0, _L( "Eviction table: item is inserted:" ), aCacheEntry.Url() );
    // check double insert
    CHttpCacheEntry* entry;
    for ( TInt i = 0; i < KHttpCacheBucketNum; i++ )
        {
        TBucketIter bucketIter( *iBuckets->At( i ) );
        //
        bucketIter.SetToFirst();
        while ( ( entry = bucketIter++ ) != NULL )
            {
            __ASSERT_DEBUG( ( entry != &aCacheEntry ) , User::Panic( _L("cacheHandler Panic"), KErrCorrupt ) );
            }
        }

    HttpCacheUtil::WriteLog( 0, _L( "insert entry to eviction table:" ), BucketIndex( FastLog2( aCacheEntry.BodySize() ) ) );
#endif // __CACHELOG__

    // Objects are classified into a limited number of groups according to log2(Si/refi)
    // calculate to which bucket it goes. Buckets are orderd by LRU
    Bucket( BucketIndex( FastLog2( aCacheEntry.BodySize() / aCacheEntry.Ref() ) ) )->AddLast( aCacheEntry );
    }

// -----------------------------------------------------------------------------
// CHttpCacheEvictionHandler::Accessed
//
// -----------------------------------------------------------------------------
//
void CHttpCacheEvictionHandler::Accessed( CHttpCacheEntry& aCacheEntry )
    {
    HttpCacheUtil::WriteUrlToLog( 0, _L( "CHttpCacheEvictionHandler::Accessed - Eviction table: item is accessed:" ), aCacheEntry.Url() );
    // first check where it is now
    TInt oldRef( aCacheEntry.Ref() - 1 );
    if( oldRef > 0 )
        {
        //
        // buckets are orderd by LRU
        // see if we need to move it to a new bucket
        TInt currentBucketIndex( BucketIndex( FastLog2( aCacheEntry.BodySize() / oldRef ) ) );
        TInt newBucketIndex( BucketIndex( FastLog2( aCacheEntry.BodySize() / aCacheEntry.Ref() ) ) );
        // check if the item is really there
        TBool itemOk( ItemIsInBucket( currentBucketIndex, aCacheEntry ) );

        if ( itemOk )
            {
            if ( currentBucketIndex == newBucketIndex )
                {
#ifdef __CACHELOG__
                HttpCacheUtil::WriteLogFilenameAndUrl( 0,
                                               _L("CHttpCacheEvictionHandler::Accessed - MOVED item to end of bucket"),
                                               aCacheEntry.Filename(),
                                               aCacheEntry.Url(),
                                               newBucketIndex,
                                               ELogBucketIndex );
#endif
                TBucket* bucket( Bucket( currentBucketIndex ) );
                // move it to the end
                if ( !bucket->IsLast( &aCacheEntry ) )
                    {
                    bucket->Remove( aCacheEntry );
                    bucket->AddLast( aCacheEntry );
                    }
                }
            else
                {
#ifdef __CACHELOG__
                HttpCacheUtil::WriteLogFilenameAndUrl( 0,
                                               _L("CHttpCacheEvictionHandler::Accessed - MOVED item to new bucket"),
                                               aCacheEntry.Filename(),
                                               aCacheEntry.Url(),
                                               newBucketIndex,
                                               ELogBucketIndex );
#endif
                // new bucket
                TBucket* currBucket( Bucket( currentBucketIndex ) );
                TBucket* newBucket( Bucket( newBucketIndex ) );
                // remove from the current and add it to the
                // new as last item

                // make sure the item is there, move it to the end
                currBucket->Remove( aCacheEntry );
                newBucket->AddLast( aCacheEntry );
                }
            }
        else
            {
            // assert
            __ASSERT_DEBUG( EFalse , User::Panic( _L("cacheHandler Panic"), KErrCorrupt ) );
            }
        }
    else if( oldRef == 0 )
        {
        // first -original access. ignore it
        // as it is already placed at insert()
        }
    else
        {
        // entry with 0 starting refcount??? doh.
        __ASSERT_DEBUG( EFalse , User::Panic( _L("cacheHandler Panic"), KErrCorrupt ) );
        }
    }

// -----------------------------------------------------------------------------
// CHttpCacheEvictionHandler::Remove
//
// -----------------------------------------------------------------------------
//
void CHttpCacheEvictionHandler::Remove( CHttpCacheEntry& aCacheEntry )
    {
#ifdef __CACHELOG__
    HttpCacheUtil::WriteLogFilenameAndUrl( 0,
                                           _L("CHttpCacheEvictionHandler::Remove - trying to removing entry"),
                                           aCacheEntry.Filename(),
                                           aCacheEntry.Url(),
                                           aCacheEntry.BodySize(),
                                           ELogEntrySize );
#endif

    TInt sizeFreqVal( FastLog2( aCacheEntry.BodySize() / aCacheEntry.Ref() ) );
    // check if the item is in the right bucket
    TInt bucketIndex( BucketIndex( sizeFreqVal ) );
    TBucket* bucket( Bucket( bucketIndex ) );
    TBool itemOk( ItemIsInBucket( bucketIndex, aCacheEntry ) );

    if ( itemOk )
        {
#ifdef __CACHELOG__
        HttpCacheUtil::WriteLogFilenameAndUrl( 0,
                                           _L("CHttpCacheEvictionHandler::Remove - removing entry from"),
                                           aCacheEntry.Filename(),
                                           aCacheEntry.Url(),
                                           bucketIndex,
                                           ELogBucketIndex );
#endif
        bucket->Remove( aCacheEntry );
        }
#ifdef __CACHELOG__
    else
        {
         HttpCacheUtil::WriteLogFilenameAndUrl( 0,
                                           _L("CHttpCacheEvictionHandler::Remove - item NOT in"),
                                           aCacheEntry.Filename(),
                                           aCacheEntry.Url(),
                                           bucketIndex,
                                           ELogBucketIndex );
        }
#endif // __CACHELOG__
    }

// -----------------------------------------------------------------------------
// CHttpCacheEvictionHandler::EvictL
//
// -----------------------------------------------------------------------------
//
CArrayPtrFlat<CHttpCacheEntry>* CHttpCacheEvictionHandler::EvictL(
    TInt aSpaceNeeded )
    {
#ifdef __CACHELOG__
    HttpCacheUtil::WriteLog( 0, _L( "CHttpCacheEvictionHandler::EvictL - aSpaceNeeded = " ), aSpaceNeeded );
    LogBuckets();
#endif // __CACHELOG__

    CArrayPtrFlat<CHttpCacheEntry>* evictedEntries = new(ELeave)CArrayPtrFlat<CHttpCacheEntry>( 10 );
    CleanupStack::PushL( evictedEntries );

    // Each group is managed using a LRU
    // policy. The extended cost-to-size model is applied to the
    // eviction candidates from all nonempty groups, purging the
    // object with largest ( DeltaT * Size / Ref ) ->delataT == curr time - last accessed
    TTime currTime;
    currTime.HomeTime();
    //
    while( aSpaceNeeded > 0 )
        {
        // take one item from each bucket and
        // check who has to go
        TInt bucketInd( -1 );
        TInt maxVal( -1 );
        TTimeIntervalSeconds secInt;
        CHttpCacheEntry* candidate = NULL;
        for( TInt i = 0; i < KHttpCacheBucketNum; i++ )
            {
            CHttpCacheEntry* entry = NULL;
            TBucket* bucket = iBuckets->At( i );
            // get the first (LRU) item from each bucket
            if( !bucket->IsEmpty() )
                {
                entry = bucket->First();
                }
#ifdef __CACHELOG__
            else {
                HttpCacheUtil::WriteLog( 0, _L("CHttpCacheEvictionHandler::EvictL : NO entries found in bucket ="), i);
            }
#endif
            // evacuate nonactive entries only
            if( entry && entry->State() == CHttpCacheEntry::ECacheComplete && !entry->BodyDataPartiallyWritten())
                {
                // watch out for 32 bit. it might overflow secInt.
                currTime.SecondsFrom( entry->LastAccessed(), secInt );

                TInt val( secInt.Int() * entry->BodySize() / entry->Ref() );
                // get the object with largest ( DeltaT * Size / Ref )
                if( val > maxVal )
                    {
                    maxVal = val;
                    bucketInd = i;
                    candidate = entry;
                    }
                }
            }

        // remove from the bucket, add it to the evicted list,
        // reduce space needed size
        if ( candidate )
            {
#ifdef __CACHELOG__
            // no protected entries should be evacuated
            if( candidate->Protected() )
                {
                HttpCacheUtil::WriteUrlToLog( 0, _L( "CHttpCacheEvictionHandler::EvictL - PROTECTED entry is about to be removed" ), candidate->Url() );
                }

            HttpCacheUtil::WriteLogFilenameAndUrl( 0,
                                           _L("CHttpCacheEvictionHandler::EvictL - removing entry "),
                                           candidate->Filename(),
                                           candidate->Url(),
                                           candidate->BodySize(),
                                           ELogEntrySize );
#endif //__CACHELOG__

            iBuckets->At( bucketInd )->Remove( *candidate );
            // Reduce size needed
            aSpaceNeeded -= candidate->BodySize();
            aSpaceNeeded -= candidate->HeaderSize();

            evictedEntries->AppendL( candidate );

#ifdef __CACHELOG__
            if ( aSpaceNeeded > 0 ) {
                HttpCacheUtil::WriteLog( 0, _L( "CHttpCacheEvictionHandler::EvictL - more space needed aSpaceNeeded = " ), aSpaceNeeded );
                }
#endif
            }
        else
            {
            // no candidate no free
#ifdef __CACHELOG__
            HttpCacheUtil::WriteLog( 0, _L( "CHttpCacheEvictionHandler::EvictL - NO Candidates to remove" ) );
#endif
            break;
            }
        }   // end if while

    CleanupStack::Pop(); // evictedEntries

    return evictedEntries;
    }

// -----------------------------------------------------------------------------
// CHttpCacheEvictionHandler::RemoveAll
//
// -----------------------------------------------------------------------------
//
void CHttpCacheEvictionHandler::RemoveAll()
    {
#ifdef __CACHELOG__
    HttpCacheUtil::WriteLog( 0, _L( "Eviction table: remove all item:" ) );
#endif

    for ( TInt i = 0; i < KHttpCacheBucketNum; i++ )
        {
        iBuckets->At( i )->Reset();
        //
        }
    }

// -----------------------------------------------------------------------------
// CHttpCacheEvictionHandler::Bucket
//
// -----------------------------------------------------------------------------
//
TBucket* CHttpCacheEvictionHandler::Bucket(
    TInt aIndex )
    {
    return iBuckets->At( aIndex );
    }

// -----------------------------------------------------------------------------
// CHttpCacheEvictionHandler::BucketIndex
//
// -----------------------------------------------------------------------------
//
TInt CHttpCacheEvictionHandler::BucketIndex(
    TInt aSizeFrequencyValue )
    {
    // last bucket by default
    TInt index( 4 );
    // supports only 5 buckets at the moment
    if( aSizeFrequencyValue == 1 )
        {
        index = 0;
        }
    else if( aSizeFrequencyValue < 4 )
        {
        index = 1;
        }
    else if( aSizeFrequencyValue < 8 )
        {
        index = 2;
        }
    else if( aSizeFrequencyValue < 16 )
        {
        index = 3;
        }
    return index;
    }

// -----------------------------------------------------------------------------
// CHttpCacheEvictionHandler::ItemIsInBucket
//
// -----------------------------------------------------------------------------
//
TBool CHttpCacheEvictionHandler::ItemIsInBucket(
    TInt aBucketIndex,
    const CHttpCacheEntry& aCacheEntry )
    {
    TBool found( EFalse );
    CHttpCacheEntry* entry;

    TBucketIter bucketIter( *(iBuckets->At( aBucketIndex )) );
    //
    bucketIter.SetToFirst();
    while( ( entry = bucketIter++ ) != NULL )
        {
        if( entry == &aCacheEntry )
            {
            found = ETrue;
            break;
            }
        }   // end of while

#ifdef __CACHELOG__
    if( !found )
        {
        HttpCacheUtil::WriteLogFilenameAndUrl( 0,
                                           _L("CHttpCacheEvictionHandler::ItemIsInBucket - entry NOT found"),
                                           aCacheEntry.Filename(),
                                           aCacheEntry.Url(),
                                           aCacheEntry.BodySize(),
                                           ELogEntrySize );
        }
#endif // __CACHELOG__

    return found;
    }

// -----------------------------------------------------------------------------
// CHttpCacheEvictionHandler::FastLog2
//
// -----------------------------------------------------------------------------
//
TInt CHttpCacheEvictionHandler::FastLog2(
    TUint aNum )
    {
    TInt log2;
    FAST_LOG2( log2, aNum );
    return log2;
    }

#ifdef __CACHELOG__
// -----------------------------------------------------------------------------
// CHttpCacheEvictionHandler::LogBuckets
//
// -----------------------------------------------------------------------------
//
void CHttpCacheEvictionHandler::LogBuckets()
    {
    CHttpCacheEntry* entry;
    for( TInt i = 0; i < KHttpCacheBucketNum; i++ )
        {
        HttpCacheUtil::WriteLog( 0, _L( "------------------------------" ), i );

        TBucketIter bucketIter( *(iBuckets->At( i )) );
        //
        bucketIter.SetToFirst();
        while ( ( entry = bucketIter++ ) != NULL )
            {
            _LIT( KDateString,"%D%M%Y%/0%1%/1%2%/2%3%/3 %-B%:0%J%:1%T%:2%S%.%*C4%:3%+B");
            _LIT( KRefSizeString,"CHttpCacheEvictionHandler::LogBuckets - size: %d refcount:%d");

            // note ref string is 60 chars before we format it
            TBuf<80> refStr;
            TBuf<50> lastAccessedStr;

            TTime lastAccessed( entry->LastAccessed() );
            TRAP_IGNORE( lastAccessed.FormatL( lastAccessedStr, KDateString ) );
            // add size/refcount
            refStr.Format( KRefSizeString, entry->BodySize(), entry->Ref() );
            // copy to 8bit string
            HttpCacheUtil::WriteLog( 0, lastAccessedStr );
            HttpCacheUtil::WriteLogFilenameAndUrl( 0,
                                               _L("CHttpCacheEvictionHandler::LogBuckets - "),
                                               entry->Filename(),
                                               entry->Url(),
                                               i,
                                               ELogBucketIndex );
            }
        }
    }
#endif // __CACHELOG__

//  End of File
