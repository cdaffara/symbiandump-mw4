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
* Description:  Definition of CHttpCacheLookupTable
*
*/

#ifndef CHTTPCACHELOOKUPTABLE_H
#define CHTTPCACHELOOKUPTABLE_H

//  INCLUDES
#include <e32base.h>
#include <s32file.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CHttpCacheEntry;
class RFileWriteStream;
class RFileReadStream;
class CHttpCacheEvictionHandler;
class CHttpCacheStreamHandler;

// CLASS DECLARATION
struct THttpCacheLookupTableEntryIterator
    {
    TInt iPos;
    TInt iCount;
    };
/**
*
*  @lib
*  @since 3.1
*/
class CHttpCacheLookupTable : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @since 3.1
        * @param
        * @param
        * @return CHttpCacheLookupTable object.
        */
        static CHttpCacheLookupTable* NewL( CHttpCacheEvictionHandler& aEvictionHandler, CHttpCacheStreamHandler& aStreamHandler );

        /**
        * Destructor.
        */
        virtual ~CHttpCacheLookupTable();

    public: // new functions

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        CHttpCacheEntry* InsertL( const TDesC8& aUrl );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        CHttpCacheEntry* Find( const TDesC8& aUrl );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        TInt Remove( const TDesC8& aUrl );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        TInt RemoveAll();

        /**
        *
        * @since 7.1
        * @param
        * @return
        */
        TInt CHttpCacheLookupTable::ListFiles( RPointerArray<TDesC>& aFilenameList );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        const CArrayPtrFlat<CHttpCacheEntry>& Entries() const { return *iEntries; }

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void EraseCacheEntry( const TDesC8& aUrl );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void InternalizeL( RReadStream& aReadStream, const TDesC& aDirectory );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void ExternalizeL( RWriteStream& aWriteStream, const TDesC& aDirectory );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void MergeL( CHttpCacheLookupTable* aHttpCacheLookupTable, RFs aRfs, const TDesC& aDirectory );

        /**
         *
         * @since 7.1
         * @param
         * @return
         */
        void BeginEntryIteration(THttpCacheLookupTableEntryIterator& aIter);

        /**
         *
         * @since 7.1
         * @param
         * @return
         */
        const CHttpCacheEntry* NextEntry(THttpCacheLookupTableEntryIterator& aIter);

    private:

        /**
        * Construct.
        * @since 3.1
        * @param
        * @param
        * @return CHttpCacheLookupTable object.
        */
        CHttpCacheLookupTable( CHttpCacheEvictionHandler& aEvictionHandler, CHttpCacheStreamHandler& aStreamHandler );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private: //

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        TInt InsertL( CHttpCacheEntry* aCacheEntry );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        TInt Probe( const TDesC8& aKey, TBool aInsert );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        TInt HashUrl( const TDesC8& aUrl );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void ReHashL();

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        TUint NextPrime( TUint aNum );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void Erase( TInt aPos );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        TBool Valid( TInt aPos );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        TBool Empty( TInt aPos );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        TBool Deleted( TInt aPos );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void SetDeleted( TInt aPos );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        TBool BoundaryCheck( TInt aPos );

    private:    // Data

        // hash table for cache entries
        CArrayPtrFlat<CHttpCacheEntry>* iEntries;
        // number of entries in the hashtable
        TInt                            iCount;
        //
        CHttpCacheEvictionHandler*      iEvictionHandler;         // not owned
        //
        CHttpCacheStreamHandler*        iStreamHandler;           // not owned
    };

#endif      // CHTTPCACHELOOKUPTABLE_H

    // End of File
