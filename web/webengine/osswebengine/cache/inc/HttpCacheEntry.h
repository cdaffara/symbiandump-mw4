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
* Description:  Definition of CHttpCacheEntry
*
*/

#ifndef CHTTPCACHEENTRY_H
#define CHTTPCACHEENTRY_H

//  INCLUDES
#include <e32base.h>
#include <f32file.h>
#include "HttpCachePostponeWriteUtilities.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class RHTTPTransaction;
class MHTTPDataSupplier;
class RFileWriteStream;
class RFileReadStream;
class CHttpCacheEvictionHandler;
class CHttpCacheStreamHandler;
class CHttpCacheEntry;

// CLASS DECLARATION
class MHttpCacheEntryDeleteObserver
    {
public:
    virtual void EntryDeleted(CHttpCacheEntry *aEntry) = 0;
    };

/**
*
*  @lib
*  @since 3.1
*/
class CHttpCacheEntry : public CBase, public MHttpCacheWriteSource
    {
    public:  // Constructors and destructor

        enum TCacheEntryState
            {
            ECacheUninitialized,
            ECacheInitialized,
            ECacheRequesting,
            ECacheResponding,
            ECacheComplete,
            ECacheDestroyed
            };

        /**
        * Two-phased constructor.
        * @since 3.1
        * @param
        * @param
        * @return CHttpCacheEntry object.
        */
        static CHttpCacheEntry* NewL( const TDesC8& aUrl,
            CHttpCacheEvictionHandler& aEvictionHandler );

        /**
        * Two-phased constructor.
        * @since 3.1
        * @param
        * @param
        * @return CHttpCacheEntry object.
        */
        static CHttpCacheEntry* NewLC( const TDesC8& aUrl,
            CHttpCacheEvictionHandler& aEvictionHandler );

        /**
        * Destructor.
        */
        virtual ~CHttpCacheEntry();

    public: // new functions

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void SetState( TCacheEntryState aState );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        inline TCacheEntryState State() { return iState; }

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void SetFileNameL( const TFileName& aFileName );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        const TDesC& Filename() const { return *iFileName; }

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        inline const TDesC8& Url() const { return *iUrl; }

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        inline TInt64 LastAccessed() const { return iLastAccessed; }

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void Accessed();

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        inline TUint16 Ref() const { return iRef; }

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        inline TUint BodySize() const { return iBodySize; }

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void SetBodySize( TUint aSize );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        inline TUint16 HeaderSize() const { return iHeaderSize; }

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        inline void SetHeaderSize( TUint16 aHeaderSize ) { iHeaderSize = aHeaderSize; }

        /**
        *
        * @since 7.1
        * @param
        * @return
        */
        void CreateHeaderBufferL( TInt aHeaderBufferSize );

        /**
        *
        * @since 7.1
        * @param
        * @return
        */
        void CreateHeaderBufferL( const TDesC8& aHeaderData );

        /**
        *
        * @since 7.1
        * @param
        * @return
        */
        TDesC8& HeaderData();

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        inline TBool Protected() const { return iProtected; }

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void SetProtected();

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        TInt Internalize( RReadStream& aReadStream, const TDesC& aDirectory );

        /**
        *
        * @since 7.1
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
        TInt Externalize( RWriteStream& aWriteStream, const TDesC& aDirectory );

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
        void Accessed(TInt64 aLastAccessed, TUint16 aRef);

        /**
        *
        * @since 7.1
        * @param
        * @return
        */
        inline TBool BodyFileDeleteNeeded() { return iBodyFileDeleteNeeded; }

        /**
        *
        * @since 7.1
        * @param
        * @return
        */
        inline void SetBodyFileDeleteNeeded( TBool aBodyFileDeleteNeeded )
                    {
                    iBodyFileDeleteNeeded = aBodyFileDeleteNeeded;
                    }
        /**
         *
         * @since 7.1
         * @param
         * @return
         *
         */
         void UnsetEvictionCandidate() { iEvictionCandidate = EFalse; };    // this only exists because when EvictL removes an item from the eviction candidate list, it can't tell the entry that this has happened and so the entry then goes on to attempt to remove itself later when it's being deleted.

        /**
         *
         * @since 7.1
         * @param
         * @return
         *
         */
         void SetDeleteObserver(MHttpCacheEntryDeleteObserver* aObserver);

        /**
         *
         * @since 7.1
         * @param
         * @return
         *
         */
         void ClearDeleteObserver();
         
    public :

        // support linked list
        static const TInt iOffset;

    private:

        /**
        * Construct.
        * @since 3.1
        * @param
        * @param
        * @return CHttpCacheEntry object.
        */
        CHttpCacheEntry( CHttpCacheEvictionHandler& aEvictionHandler );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( const TDesC8& aUrl );

        /**
        *
        * @since 7.1
        * @param
        * @return
        */
        void SetCacheBufferL( TInt aCacheBufferSize );

    public:
        // from MHttpCacheStreamWriteSource
        virtual RFile& BodyFile();
        virtual void   BodyWriteInProgress();
        virtual void   BodyWriteComplete();
        virtual CSegmentedHeapBuffer& BodyData();

        enum TWriteStateBits
            {
            EFileOk = 0x01,
            EHeaderDataCached = 0x02,
            ENewHeaderData = 0x04,
            EBodyDataCached = 0x08,
            EDelayedWriteAllowed = 0x10,
            EDelayedWriteInProgress = 0x20,
            EBodyDataPartiallyWritten = 0x40
            };

        inline TBool FileOk() const { return (iWriteState & EFileOk); }
        inline TBool BodyDataCached() const { return (iWriteState & EBodyDataCached); }
        inline TBool DelayedWriteAllowed() const { return (iWriteState & EDelayedWriteAllowed); }
        inline TBool DelayedWriteInProgress() const { return (iWriteState & EDelayedWriteInProgress); }
        inline TBool BodyDataPartiallyWritten() const { return (iWriteState & EBodyDataPartiallyWritten); }

        inline void SetFileOk(TBool aBit)           { (aBit ? iWriteState |= EFileOk : iWriteState &= ~EFileOk); }
        void SetBodyDataCached(TBool aBit)          { (aBit ? iWriteState |= EBodyDataCached : iWriteState &= ~EBodyDataCached); if(!aBit){ iCacheBuffer->Reset(); } }
        void SetDelayedWriteAllowed(TBool aBit)     { (aBit ? iWriteState |= EDelayedWriteAllowed : iWriteState &= ~EDelayedWriteAllowed); }
        void SetDelayedWriteInProgress(TBool aBit)  { (aBit ? iWriteState |= EDelayedWriteInProgress : iWriteState &= ~EDelayedWriteInProgress); }
        void SetBodyDataPartiallyWritten(TBool aBit){ (aBit ? iWriteState |= EBodyDataPartiallyWritten : iWriteState &= ~EBodyDataPartiallyWritten); }

        void WriteBodyDataAsync(TRequestStatus& aStatus);
        void CancelBodyWrite();

    private:    // Data

        //
        HBufC8*                         iUrl;               // owned
        // if empty, then use url to get filename
        HBufC*                          iFileName;          // owned
        //
        TCacheEntryState                iState;
        //
        TInt64                          iLastAccessed;
        // ref counter
        TUint16                         iRef;
        // content size
        TUint                           iBodySize;
        // header size
        TUint16                         iHeaderSize;
        // protected content like css, script
        TUint8                          iProtected;
        //
        TSglQueLink                     iSqlQueLink;
        //
        CHttpCacheEvictionHandler*      iEvictionHandler;   // not owned
        // Etrue if added to the eviction table
        TUint8                          iEvictionCandidate;
        //
        TBool                           iBodyFileDeleteNeeded;
        //
        RFile                           iBodyFile;          // owned
        //
        CSegmentedHeapBuffer*           iCacheBuffer;       //owned
        //
        HBufC8*                         iHeaderBuffer;      // owned
        // TWriteStateBits
        TUint32                         iWriteState;
        //
        CHttpCacheEntryAsyncWriteHelper* iWriteHelper;      //owned
        //
        MHttpCacheEntryDeleteObserver* iDeleteObserver;    // NOT owned
       
    };

#endif      // CHTTPCACHEENTRY_H

// End of File
