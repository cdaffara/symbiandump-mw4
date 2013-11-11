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
* Description:  Implementation of CHttpCacheLookupTable
*
*/

// INCLUDE FILES
#include "HttpCacheLookupTable.h"
#include "HttpCacheEntry.h"
#include "HttpCacheUtil.h"
#include "HttpCacheEvictionHandler.h"
#include "HttpCacheStreamHandler.h"
#include <e32cmn.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
// Golden ratio - arbitrary start value to avoid mapping all 0's to all 0's
// or anything like that.
const TUint KHashPHI = 0x9e3779b9U;
//
const TUint KHashStringLength = 16;
// kbyte
//
const TUint KHttpCacheLookupTableSize = 217;

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHttpCacheLookupTable::CHttpCacheLookupTable
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CHttpCacheLookupTable::CHttpCacheLookupTable(
    CHttpCacheEvictionHandler& aEvictionHandler,
    CHttpCacheStreamHandler& aStreamHandler )
    : iEvictionHandler( &aEvictionHandler ),
      iStreamHandler( &aStreamHandler )
    {
    }

// -----------------------------------------------------------------------------
// CHttpCacheLookupTable::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHttpCacheLookupTable::ConstructL()
    {
    iEntries = new( ELeave )CArrayPtrFlat<CHttpCacheEntry>( KHttpCacheLookupTableSize );
    for ( TInt i = 0; i < KHttpCacheLookupTableSize; i++ )
        {
        iEntries->AppendL( NULL );
        }
    }

// -----------------------------------------------------------------------------
// CHttpCacheLookupTable::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHttpCacheLookupTable* CHttpCacheLookupTable::NewL(
    CHttpCacheEvictionHandler& aEvictionHandler,
    CHttpCacheStreamHandler& aStreamHandler )
    {
    CHttpCacheLookupTable* self = new( ELeave ) CHttpCacheLookupTable( aEvictionHandler, aStreamHandler );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
CHttpCacheLookupTable::~CHttpCacheLookupTable()
    {
    // do not call ResetAndDestroy on iEntries
    // as delete item are not set to NULL
    if (iEntries)
    {
      for( TInt i = 0; i < iEntries->Count(); i++ )
          {
          if( Valid( i ) )
              {
              CHttpCacheEntry* entry = iEntries->At( i );
              delete entry;
              }
          }
      iEntries->Reset();
    }

    delete iEntries;
    }

// -----------------------------------------------------------------------------
// CHttpCacheLookupTable::InsertL
//
// -----------------------------------------------------------------------------
//
CHttpCacheEntry* CHttpCacheLookupTable::InsertL(
    const TDesC8& aUrl )
    {
    // create and insert the item
    CHttpCacheEntry* entry = CHttpCacheEntry::NewLC( aUrl, *iEvictionHandler );

    if( InsertL( entry ) == KErrCorrupt )
        {
        // cleanup
        CleanupStack::PopAndDestroy(); // entry
        entry = NULL;
        }
    else
        {
#ifdef __CACHELOG__
        HttpCacheUtil::WriteFormatLog(0, _L("  Added entry %08x to list."), entry);
#endif
        entry->Accessed();
        iStreamHandler->InitialiseCacheEntryL(*entry);

        // lookuptable takes ownership
        CleanupStack::Pop(); // entry
        }
    return entry;
    }

// -----------------------------------------------------------------------------
// CHttpCacheLookupTable::Find
//
// -----------------------------------------------------------------------------
//
CHttpCacheEntry* CHttpCacheLookupTable::Find( const TDesC8& aUrl )
    {
    CHttpCacheEntry* entry = NULL;
    TInt pos( Probe( aUrl, EFalse ) );
#ifdef __CACHELOG__
    HttpCacheUtil::WriteUrlToLog(0, _L("CHttpCacheLookupTable::Find"), aUrl);
    HttpCacheUtil::WriteFormatLog(0, _L("  Probe returned position %d"), pos);
#endif
    if ( Valid( pos ) )
        {
#ifdef __CACHELOG__
        HttpCacheUtil::WriteFormatLog(0, _L("  Entry %d valid."), pos);
#endif
        entry = iEntries->At( pos );

        if ( entry )
            {
#ifdef __CACHELOG__
            HttpCacheUtil::WriteFormatLog(0, _L("  BodySize is %d\n  State is %d"), entry->BodySize(), entry->State());
#endif
            if ( entry->BodySize() == 0 &&
                 entry->State() == CHttpCacheEntry::ECacheComplete )
                {

#ifdef __CACHELOG__
                HttpCacheUtil::WriteLogFilenameAndUrl( 0,
                                           _L("CHttpCacheLookupTable::Find - Found ZERO size (can't reuse)"),
                                           entry->Filename(),
                                           entry->Url(),
                                           pos,
                                           ELogLookupTablePos );
#endif
                return NULL;
                }
            }
        }
#ifdef __CACHELOG__
    HttpCacheUtil::WriteFormatLog(0, _L("  returning entry pointer 0x%08x"), entry);
#endif

    return entry;
    }

// -----------------------------------------------------------------------------
// CHttpCacheLookupTable::Remove
//
// -----------------------------------------------------------------------------
//
TInt CHttpCacheLookupTable::Remove( const TDesC8& aUrl )
    {
    TInt status( KErrNotFound );
    TInt pos( Probe( aUrl, EFalse ) );
#ifdef __CACHELOG__
    HttpCacheUtil::WriteUrlToLog(0, _L("CHttpCacheLookupTable::Remove"), aUrl);
    HttpCacheUtil::WriteFormatLog(0, _L("  Probe returned position %d"), pos);
#endif
    if( Valid( pos ) )
        {
        // remove only nonactive entry
        CHttpCacheEntry* entry = iEntries->At( pos );
        if ( entry->State() == CHttpCacheEntry::ECacheComplete )
            {
#ifdef __CACHELOG__
            HttpCacheUtil::WriteLogFilenameAndUrl( 0,
                                           _L("CHttpCacheLookupTable::Remove - ERASING item"),
                                           entry->Filename(),
                                           entry->Url(),
                                           pos,
                                           ELogLookupTablePos );
#endif
            Erase( pos );
            status = KErrNone;
            }
#ifndef __CACHELOG__
        }
#else //__CACHELOG__
        else
            {
            HttpCacheUtil::WriteLogFilenameAndUrl( 0,
                                           _L("CHttpCacheLookupTable::Remove - can NOT remove item, is active state"),
                                           entry->Filename(),
                                           entry->Url(),
                                           pos,
                                           ELogLookupTablePos );
            }
        }
    else
        {
        HttpCacheUtil::WriteFormatLog( 0, _L( "CHttpCacheLookupTable::Remove - item %d is not valid. cannot be removed" ), pos );
        }
#endif // __CACHELOG__

    return status;
    }

// -----------------------------------------------------------------------------
// CHttpCacheLookupTable::EraseCacheEntry
//
// -----------------------------------------------------------------------------
//
void CHttpCacheLookupTable::EraseCacheEntry( const TDesC8& aUrl )
    {
    TInt pos( Probe( aUrl, EFalse ) );

    if ( Valid( pos ) )
        {
        Erase( pos );
        }
#ifdef __CACHELOG__
    else
        {
        // there must be a valid position for this entry
        __ASSERT_DEBUG( EFalse, User::Panic( _L("cacheHandler Panic"), KErrCorrupt ) );
        }
#endif // __CACHELOG__
    }

// -----------------------------------------------------------------------------
// CHttpCacheLookupTable::RemoveAll
//
// -----------------------------------------------------------------------------
//
TInt CHttpCacheLookupTable::RemoveAll()
    {
    TInt numberOfBytes( 0 );

#ifdef __CACHELOG__
    HttpCacheUtil::WriteLog( 0, _L( "CHttpCacheLookupTable::RemoveAll - remove 'em all" ) );
#endif

    for ( TInt i = 0; i < iEntries->Count(); i++ )
        {
        CHttpCacheEntry* entry = iEntries->At( i );
        // Remove all nonactive entries
        if ( Valid( i ) && entry->State() == CHttpCacheEntry::ECacheComplete )
            {
            numberOfBytes += ( entry->HeaderSize() + entry->BodySize() );
            Erase( i );
            }
        }

#ifdef __CACHELOG__
    HttpCacheUtil::WriteLog( 0, _L( "number of items left:" ), iCount );
    // check if there are pending items -should not be though
    __ASSERT_DEBUG( iCount == 0, User::Panic( _L("cacheHandler Panic"), KErrCorrupt ) );
#endif // __CACHELOG__

    return numberOfBytes;
    }


// -----------------------------------------------------------------------------
// CHttpCacheLookupTable::ListFiles
// Adds all filenames known to this lookup table to aFilenameList
// No ownership transfer occurs, only pointers are added
// -----------------------------------------------------------------------------
//
TInt CHttpCacheLookupTable::ListFiles(RPointerArray<TDesC>& aFilenameList)
    {
    TInt count( 0 );
    TInt error( KErrNone );

    //1. Tally up
    for (TInt i = 0; i < iEntries->Count(); i++)
        {
        if (Valid(i)) count++;
        }

    //2. Preallocation.
    TInt existing( aFilenameList.Count() );
    error = aFilenameList.Reserve( existing + count );

    if (error == KErrNone)
        {
        //3. Iterate once more and add pointers to filename strings
        for (TInt i = 0; i < iEntries->Count(); i++)
            {
            if (Valid(i))
                {
                //add filename pointer to the array.
                const TDesC* ptr = &(iEntries->At(i)->Filename());
                aFilenameList.Append( ptr ); // no ownership transfer happens here
                }
            }
        }

    return error;

    }

// -----------------------------------------------------------------------------
// CHttpCacheLookupTable::InternalizeL
//
// -----------------------------------------------------------------------------
//
void CHttpCacheLookupTable::InternalizeL(
    RReadStream& aReadStream,
    const TDesC& aDirectory )
    {
    // get number of entries
    TInt version = 0;
    version = aReadStream.ReadInt32L();
    if ( version == KCacheVersionNumber )
        {
        // read directory stub and validate it
        TInt len = aReadStream.ReadInt32L();
        if(!len)
            return;
            
        HBufC* dirstub = HBufC::NewLC(len);
        TPtr dirstubptr = dirstub->Des();
        aReadStream.ReadL( dirstubptr, len );
        ASSERT( aDirectory.CompareF( dirstubptr ) == 0 );
        CleanupStack::PopAndDestroy( dirstub );

        TInt count( aReadStream.ReadInt32L() );
        TInt contentSize( 0 );
        TInt err;
        for( TInt i = 0; i < count; i++ )
            {
            // create empty object
            CHttpCacheEntry* entry = CHttpCacheEntry::NewLC( KNullDesC8, *iEvictionHandler );
            // read it
            err = entry->Internalize( aReadStream, aDirectory );
            // leave only on no memory
            if( err == KErrNone )
                {
                // insert to the table
                InsertL( entry );
                contentSize += entry->HeaderSize();
                contentSize += entry->BodySize();
                }
            else if ( err == KErrNoMemory )
                {
                User::Leave( KErrNoMemory );
                }
            else
                {
                // suggestions
                }

            // takes ownership
            CleanupStack::Pop(); // entry
            }

        // set startup cache size
#ifdef __CACHELOG__
        HttpCacheUtil::WriteLog( 0, _L( "startup content size" ), contentSize );
#endif
        iStreamHandler->SetSavedContentSize( contentSize );
        }
    else
        {
        // cleanup index.dat
        }
    }

// -----------------------------------------------------------------------------
// CHttpCacheLookupTable::Externalize
//
// -----------------------------------------------------------------------------
//
void CHttpCacheLookupTable::ExternalizeL(
    RWriteStream& aWriteStream, const TDesC& aDirectory )
    {
    // write version number and the number of entries
    TRAP_IGNORE( aWriteStream.WriteInt32L( KCacheVersionNumber ) );

    // directory stub length
    aWriteStream.WriteInt32L( aDirectory.Length() ) ;
    // directory stub
    aWriteStream.WriteL( aDirectory );

    // entry count - don't write entries with zero body length, so precalculate this.
    TInt goingToWrite = 0;
    for( TInt i = 0; i < iEntries->Count(); i++ )
        {
        CHttpCacheEntry* entry = iEntries->At( i );
        // save complete entries only
        if( Valid( i ) && entry->BodySize() > 0 )
            {
            goingToWrite++;
            }
        }
    aWriteStream.WriteInt32L( goingToWrite );

    for( TInt i = 0; i < iEntries->Count(); i++ )
        {
        CHttpCacheEntry* entry = iEntries->At( i );
        // save complete entries only
        if( Valid( i ) && entry->BodySize() > 0 )
            {
            // save entry
            TInt err;
            err = entry->Externalize( aWriteStream, aDirectory );
            // leave only on no memory
            if ( err == KErrNoMemory )
                {
                User::Leave( KErrNoMemory );
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CHttpCacheLookupTable::InsertL
//
// -----------------------------------------------------------------------------
//
TInt CHttpCacheLookupTable::InsertL(
    CHttpCacheEntry* aCacheEntry )
    {
    __ASSERT_DEBUG( aCacheEntry != NULL,
        User::Panic( _L("cacheHandler Panic"), KErrCorrupt ) );

    TInt pos( -1 );

    if ( aCacheEntry )
        {
        pos = Probe( aCacheEntry->Url(), ETrue );
#ifdef __CACHELOG__
        HttpCacheUtil::WriteUrlToLog(0, _L("CHttpCacheLookupTable::InsertL"), aCacheEntry->Url());
        HttpCacheUtil::WriteFormatLog(0, _L("  Probe returned position %d"), pos);
#endif
        // double check
        if( Valid( pos ) )
            {
#ifdef __CACHELOG__
            HttpCacheUtil::WriteLog(0, _L("  Valid entry already exists according to Valid(). Rehash table."));
#endif
            // try to rehash the table if probe failed
            ReHashL();
            pos = Probe( aCacheEntry->Url(), ETrue );
#ifdef __CACHELOG__
            HttpCacheUtil::WriteFormatLog(0, _L("  ReProbe after rehash returned new position %d"), pos);
#endif
            if( pos == -1 || Valid( pos ) )
                {
#ifdef __CACHELOG__
                HttpCacheUtil::WriteLog(0, _L("  Pos already filled (or == -1) - Couldn't find an empty slot to store it."));
#endif
                // completly failed
                pos = -1;
                }
            }

        // insert
        if( pos != -1 )
            {
            iEntries->At( pos ) = aCacheEntry;
            iCount++;
#ifdef __CACHELOG__
            HttpCacheUtil::WriteFormatLog( 0, _L( "insert new item at %d in the lookuptable" ), pos );
#endif
            // check if the hashtable is full
            if ( iCount > ( iEntries->Count() >> 1 ) )
                {
#ifdef __CACHELOG__
                HttpCacheUtil::WriteLog(0, _L("  But! hashtable is full - rehash."));
#endif
                ReHashL();
                }
            }
        else
            {
            // lose entry???
            __ASSERT_DEBUG( EFalse, User::Panic( _L("cacheHandler Panic"), KErrCorrupt ) );
            }
        }
    return pos == -1 ? KErrCorrupt : KErrNone;
    }

// -----------------------------------------------------------------------------
// CHttpCacheLookupTable::Probe
//
// -----------------------------------------------------------------------------
//
TInt CHttpCacheLookupTable::Probe(
    const TDesC8& aKey,
    TBool aInsert )
    {
    //
  TInt collision( 0 );
  TInt pos( HashUrl( aKey ) );

  if( aInsert )
        {
        // insert
        while( Valid( pos ) && ( collision < ( iEntries->Count() / 2 ) ) )
            {
            pos += ++collision * 2 - 1;
            pos = pos % iEntries->Count();
            }
        }
    else
        {
        // find
      while( !Empty( pos ) )
            {
            CHttpCacheEntry* entry = iEntries->At( pos );
            if( Valid( pos ) && entry && entry->Url().CompareF( aKey ) == 0 )
                {
                break;
                }
            if( collision > iEntries->Count() )
                {
                return -1;
                }
            pos += ++collision * 2 - 1;
            pos = pos % iEntries->Count();
            }
        }
#ifdef __CACHELOG__
    if( collision > 0 )
        {
        HttpCacheUtil::WriteLog( 1, _L( "collision" ), collision );
        }
#endif // __CACHELOG__
  return pos;
    }

// -----------------------------------------------------------------------------
// CHttpCacheLookupTable::HashUrl
//
// -----------------------------------------------------------------------------
//
TInt CHttpCacheLookupTable::HashUrl(
    const TDesC8& aUrl )
    {
    // This hash algorithm comes from:
    // http://burtleburtle.net/bob/hash/hashfaq.html
    // http://burtleburtle.net/bob/hash/doobs.html
    TUint len( aUrl.Length() );
    TUint h( KHashPHI );

    h += len;
    h += (h << 10);
    h ^= (h << 6);
    //
    if( len )
        {
        // hash backward to avoid collisions
        // as the first part of the url is
        // always the same http://www.
        // take 16 characters by default
        TUint hashStringLength( len < KHashStringLength ? len : KHashStringLength );

        for( TUint i = ( len - 1 ); i > len - hashStringLength; i-- )
            {
        h += aUrl[ i ];
        h += (h << 10);
        h ^= (h << 6);
          }
        }
    h += (h << 3);
    h ^= (h >> 11);
    h += (h << 15);

    //
    h = h % iEntries->Count();
#ifdef __CACHELOG__
    HttpCacheUtil::WriteUrlToLog( 1, aUrl, h );
#endif // __CACHELOG__
    return h;
    }

// -----------------------------------------------------------------------------
// CHttpCacheLookupTable::ReHashL
//
// -----------------------------------------------------------------------------
//
void CHttpCacheLookupTable::ReHashL()
    {
#ifdef __CACHELOG__
    HttpCacheUtil::WriteLog( 1, _L( "Resize lookuptable!" ) );
    HttpCacheUtil::WriteLog( 1, _L( "count=" ), iCount );
    HttpCacheUtil::WriteLog( 1, _L( "Table size=" ), iEntries->Count() );
#endif

    TUint newSize( NextPrime( iEntries->Count() * 2 ) );
    CArrayPtrFlat<CHttpCacheEntry>* newEntries =
        new( ELeave )CArrayPtrFlat<CHttpCacheEntry>( newSize );
    // hash must operate on the new table
    CArrayPtrFlat<CHttpCacheEntry>* oldEntries = iEntries;
    iEntries = newEntries;
    CleanupStack::PushL( oldEntries );
    // fill list with 0
    for( TUint i = 0; i < newSize; i++ )
        {
        iEntries->AppendL( NULL );
        }
    //
  for( TUint i = 0; i < oldEntries->Count(); i++ )
      {
    CHttpCacheEntry* entry = oldEntries->At( i );
        // transfer valid entries only
    if( entry && entry != (CHttpCacheEntry*)0xffffffff )
            {
            TInt pos( Probe( entry->Url(), ETrue ) );
            //
      if( pos != -1 && !Valid( pos ) )
                {
                iEntries->At( pos ) = entry;
                // remove ownership
                oldEntries->At( i ) = NULL;
                }
            // else lose the entry??
            else
                {
                // assert
                __ASSERT_DEBUG( EFalse, User::Panic( _L("cacheHandler Panic"), KErrCorrupt ) );
                }
        }
      }
#ifdef __CACHELOG__
  for( TUint i = 0; i < iEntries->Count(); i++ )
        {
    CHttpCacheEntry* entry = iEntries->At( i );
        if( entry && Valid( i ) )
            {
            HttpCacheUtil::WriteUrlToLog( 1, entry->Url(), i );
            }
        }
#endif // __CACHELOG__
    CleanupStack::PopAndDestroy(); // oldEntries
    }

// -----------------------------------------------------------------------------
// CHttpCacheLookupTable::NextPrime
//
// -----------------------------------------------------------------------------
//
TUint CHttpCacheLookupTable::NextPrime(
    TUint aNum )
    {
  if( aNum % 2 == 0 )
        {
        aNum++;
        }

  for( ;; )
        {
    for( TUint i = 3; i * i <= aNum; i += 2 )
      if( aNum % i == 0 )
                {
        aNum += 2;
        i = 1;
        continue;
      }
    return aNum;
      }
    }

// -----------------------------------------------------------------------------
// CHttpCacheLookupTable::Erase
//
// -----------------------------------------------------------------------------
//
void CHttpCacheLookupTable::Erase( TInt aPos )
    {
    // must be a valid pos
    __ASSERT_DEBUG( Valid( aPos ),  User::Panic( _L("cacheHandler Panic"), KErrCorrupt ) );

    CHttpCacheEntry* entry = iEntries->At( aPos );

    if( entry )
        {
#ifdef __CACHELOG__
        HttpCacheUtil::WriteLogFilenameAndUrl( 0,
                                           _L("CHttpCacheLookupTable::Erase"),
                                           entry->Filename(),
                                           entry->Url(),
                                           aPos,
                                           ELogLookupTablePos );
#endif
        iStreamHandler->Erase( *entry );
        SetDeleted( aPos );
        delete entry;
        iCount--;
        }
    }

// -----------------------------------------------------------------------------
// CHttpCacheLookupTable::Valid
//
// -----------------------------------------------------------------------------
//
TBool CHttpCacheLookupTable::Valid( TInt aPos )
    {
    return( BoundaryCheck( aPos ) && !Empty( aPos ) && !Deleted( aPos ) );
    }

// -----------------------------------------------------------------------------
// CHttpCacheLookupTable::Empty
//
// -----------------------------------------------------------------------------
//
TBool CHttpCacheLookupTable::Empty( TInt aPos )
    {
    return( BoundaryCheck( aPos ) && iEntries->At( aPos ) == NULL );
    }

// -----------------------------------------------------------------------------
// CHttpCacheLookupTable::Deleted
//
// -----------------------------------------------------------------------------
//
TBool CHttpCacheLookupTable::Deleted(
    TInt aPos )
    {
    return( BoundaryCheck( aPos ) && iEntries->At( aPos ) == (CHttpCacheEntry*)0xffffffff );
    }

// -----------------------------------------------------------------------------
// CHttpCacheLookupTable::SetDeleted
//
// -----------------------------------------------------------------------------
//
void CHttpCacheLookupTable::SetDeleted(
    TInt aPos )
    {
    if( BoundaryCheck( aPos ) )
        {
        iEntries->At( aPos ) = (CHttpCacheEntry*)0xffffffff;
        }
    }

// -----------------------------------------------------------------------------
// CHttpCacheLookupTable::BoundaryCheck
//
// -----------------------------------------------------------------------------
//
TBool CHttpCacheLookupTable::BoundaryCheck(
    TInt aPos )
    {
    return ( aPos >= 0 && aPos < iEntries->Count() );
    }

// -----------------------------------------------------------------------------
// CHttpCacheLookupTable::MergeL
//
// -----------------------------------------------------------------------------
//
void CHttpCacheLookupTable::MergeL( CHttpCacheLookupTable* aHttpCacheLookupTable, RFs aRfs, const TDesC& aDirectory )
    {
    CHttpCacheFileHash *onDiskFilesMap = NULL;
    
    // creating this object will use some memory, so this may fail.
    TRAPD(err, HttpCacheUtil::GenerateCacheContentHashMapL( onDiskFilesMap, aRfs, aDirectory , 0 ));
    // if it does fail, we fall back to examining the disk.
    if(err != KErrNone)
        {
        onDiskFilesMap = NULL;
        }
    else
        {
        CleanupStack::PushL( onDiskFilesMap );
        }
    
    TInt myCount = iEntries->Count();
    TInt i = 0;
    for (i = myCount - 1; i >= 0; i--)
        {
        if ( Valid( i ) )
            {
            CHttpCacheEntry* entry = iEntries->At(i);
            CHttpCacheEntry* newEntry = aHttpCacheLookupTable->Find(entry->Url());
            if (newEntry)
                {
                // Entry is in the new index file
                if (newEntry->LastAccessed() > entry->LastAccessed())
                    {
                    entry->Accessed(newEntry->LastAccessed(), newEntry->Ref());
                    }
                TInt pos = aHttpCacheLookupTable->Probe(newEntry->Url(), EFalse);
                if (aHttpCacheLookupTable->Valid(pos))
                    {
                    aHttpCacheLookupTable->SetDeleted(pos);
                    delete newEntry;
                    aHttpCacheLookupTable->iCount--;
                    }
                }
            else // (newEntry)
                {
                // Entry is not in the new index file
                // Entry is in RAM index, may not be on disk yet due to postpone operation.
                TUint att;
                if ( !entry->BodyDataCached() && (onDiskFilesMap ? onDiskFilesMap->HashMap().Find( entry->Filename() ) == NULL : (aRfs.Att(entry->Filename(), att) != KErrNone)) )
                    {
                    TInt thePos = Probe(entry->Url(), EFalse);
                    if (Valid(thePos))
                        {
                        Erase(thePos);
                        }
                    }
                }
            }
        }

    TInt newCount = aHttpCacheLookupTable->iEntries->Count();
    for (i = newCount - 1; i >= 0; i--)
        {
        if ( aHttpCacheLookupTable->Valid( i ) )
            {
            CHttpCacheEntry* newEntry = aHttpCacheLookupTable->iEntries->At(i);
            TInt pos = aHttpCacheLookupTable->Probe(newEntry->Url(), EFalse);
            TUint att;
            if ( onDiskFilesMap ? (onDiskFilesMap->HashMap().Find(newEntry->Filename()) != NULL ) : (aRfs.Att(newEntry->Filename(), att) == KErrNone) )
                {
                CHttpCacheEntry* myEntry = InsertL(newEntry->Url());
                myEntry->SetState( CHttpCacheEntry::ECacheComplete );
                myEntry->Accessed(newEntry->LastAccessed(), newEntry->Ref());
                }
            aHttpCacheLookupTable->SetDeleted(pos);
            delete newEntry;
            aHttpCacheLookupTable->iCount--;
            }
        }
    
    if( onDiskFilesMap )
        CleanupStack::PopAndDestroy( onDiskFilesMap );
    }

// -----------------------------------------------------------------------------
// CHttpCacheLookupTable::BeginEntryIteration
//
// -----------------------------------------------------------------------------
//
void CHttpCacheLookupTable::BeginEntryIteration(THttpCacheLookupTableEntryIterator& aIter)
    {
    aIter.iPos = 0;
    aIter.iCount = iCount;
    }

// -----------------------------------------------------------------------------
// CHttpCacheLookupTable::NextEntry
//
// -----------------------------------------------------------------------------
//
const CHttpCacheEntry* CHttpCacheLookupTable::NextEntry(THttpCacheLookupTableEntryIterator& aIter)
    {
    const CHttpCacheEntry *entry = NULL;

    while ( !entry && aIter.iPos < iEntries->Count() )
        {
        entry = Valid(aIter.iPos) ? iEntries->At(aIter.iPos) : NULL;
        aIter.iPos++;
        }

    return entry;
    }

//  End of File
