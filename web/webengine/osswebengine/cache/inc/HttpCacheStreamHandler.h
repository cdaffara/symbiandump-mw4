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
* Description:  Definition of CHttpCacheStreamHandler
*
*/

#ifndef CHTTPCACHESTREAMHANDLER_H
#define CHTTPCACHESTREAMHANDLER_H

//  INCLUDES

#include <e32base.h>
#include <f32file.h>
#include <s32file.h>
#include "HttpCacheHandler.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CHttpCacheEntry;
class RFileWriteStream;
class RFileReadStream;
class CHttpCacheFileWriteHandler;
class CSegmentedHeapBuffer;

// CLASS DECLARATION

/**
*
*  @lib
*  @since 3.1
*/
class CHttpCacheStreamHandler : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @since 3.1
        * @param
        * @param
        * @return CHttpCacheStreamHandler object.
        */
        static CHttpCacheStreamHandler* NewL( const TDesC& aDirectory, TInt aCriticalLevel, RFs& aRFs );

        /**
        * Destructor.
        */
        virtual ~CHttpCacheStreamHandler();

    public: // new functions

        void InitialiseCacheEntryL(CHttpCacheEntry& aCacheEntry);

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        inline TInt SavedContentSize() const { return iContentSize; }

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        inline void SetSavedContentSize( TInt aContentSize ) { iContentSize = aContentSize; }

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void Erase( CHttpCacheEntry& aCacheEntry );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        HBufC8* HeadersL( CHttpCacheEntry& aCacheEntry );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        HBufC8* NextChunkL( CHttpCacheEntry& aCacheEntry, TBool& aLastChunk );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        TBool SaveHeaders( CHttpCacheEntry& aCacheEntry, const TDesC8& aHeaderStr );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void RemoveHeaders( CHttpCacheEntry& aCacheEntry );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        TBool SaveBodyData( CHttpCacheEntry& aCacheEntry, const TDesC8& aBodyStr );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void RemoveBodyData( CHttpCacheEntry& aCacheEntry );

        /**
        * Flush
        * @since 3.1
        * @param
        * @return
        */
        TBool Flush( CHttpCacheEntry& aCacheEntry );

        /**
        * FlushL
        * @since 7.1
        * @param
        * @return
        */
        TBool FlushL( CHttpCacheEntry& aCacheEntry );

        /**
        * FlushAsync
        * @since 7.1
        * @param
        * @return
        */
        TBool FlushAsync( CHttpCacheEntry& aCacheEntry , TRequestStatus& aStatus);

        /**
        *
        * @since 7.1
        * @param
        * @return
        */
        TBool OpenBodyFile( CHttpCacheEntry& aCacheEntry );

        /**
        *
        * @since 7.1
        * @param
        * @return
        */
        TBool CreateNewBodyFile( CHttpCacheEntry& aCacheEntry, TBool aUseDirectIO = EFalse );

    private:

        /**
        * Construct.
        * @since 3.1
        * @param
        * @param
        * @return CacheHandler object.
        */
        CHttpCacheStreamHandler(RFs& aRFs);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( const TDesC& aDirectory, TInt aCriticalLevel );

    private:

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        TBool IsDiskSpaceAvailable( TInt aContentSize );

        /**
        *
        * @since 7.1
        * @param
        * @return
        */
        void FindCacheEntryIndex( const CHttpCacheEntry& aCacheEntry, TInt* aIndex );

    private:    // Data

        //
        RFs                                 iRfs;
        // attached entries
        CArrayPtrFlat<CHttpCacheEntry>*     iActiveEntries;     // owned
        //
        TInt                                iContentSize;
        //
        TDriveUnit                          iDrive;
        //
        TInt64                              iCriticalLevel;
    };

#endif      // CHTTPCACHESTREAMHANDLER_H

// End of File
