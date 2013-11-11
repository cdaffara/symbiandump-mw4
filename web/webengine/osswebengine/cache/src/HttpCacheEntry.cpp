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
* Description:  Implementation of CHttpCacheEntry
*
*/

// INCLUDE FILES
#include "HttpCacheEntry.h"
#include "HttpCacheEvictionHandler.h"
#include "HttpCacheStreamHandler.h"
#include "HttpCacheUtil.h"
#include <s32file.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
const TInt CHttpCacheEntry::iOffset = _FOFF( CHttpCacheEntry, iSqlQueLink );
const TInt KBufferSize32k = 32768;
const TInt KBufferGranularity4k = 4096;

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHttpCacheEntry::CHttpCacheEntry
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CHttpCacheEntry::CHttpCacheEntry( CHttpCacheEvictionHandler& aEvictionHandler )
    : iState( ECacheUninitialized ),
      iEvictionHandler( &aEvictionHandler )
    {
    }

// -----------------------------------------------------------------------------
// CHttpCacheEntry::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHttpCacheEntry::ConstructL( const TDesC8& aUrl )
    {
    iUrl = aUrl.AllocL();
    iHeaderBuffer = KNullDesC8().AllocL();
    iCacheBuffer = CSegmentedHeapBuffer::NewL( KBufferSize32k, KBufferGranularity4k );
    }

// -----------------------------------------------------------------------------
// CHttpCacheEntry::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHttpCacheEntry* CHttpCacheEntry::NewL(
    const TDesC8& aUrl,
    CHttpCacheEvictionHandler& aEvictionHandler )
    {
    CHttpCacheEntry* self = new( ELeave ) CHttpCacheEntry( aEvictionHandler );

    CleanupStack::PushL( self );
    self->ConstructL( aUrl );
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CHttpCacheEntry::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHttpCacheEntry* CHttpCacheEntry::NewLC(
    const TDesC8& aUrl,
    CHttpCacheEvictionHandler& aEvictionHandler )
    {
    CHttpCacheEntry* self = CHttpCacheEntry::NewL( aUrl, aEvictionHandler );
    CleanupStack::PushL( self );
    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CHttpCacheEntry::~CHttpCacheEntry()
    {
#ifdef __CACHELOG__
    HttpCacheUtil::WriteFormatLog(0, _L("Deleting CHttpCacheEntry %08x"), this);
#endif
    // Clean up eviction handler
    if ( iEvictionCandidate && iEvictionHandler )
        {
#ifdef __CACHELOG__
        HttpCacheUtil::WriteLog(0, _L("Removing from eviction candidate list"));
#endif
        iEvictionHandler->Remove( *this );
        }

    if( iDeleteObserver )
        {
#ifdef __CACHELOG__
        HttpCacheUtil::WriteFormatLog(0, _L("Notifying delete observer %08x"), this);
#endif
        iDeleteObserver->EntryDeleted(this);
        }
    
    // Close files, this will commit changes
    iBodyFile.Close();

    // Clean up our memory
    delete iUrl;
    delete iFileName;
    delete iHeaderBuffer;
    delete iCacheBuffer;
    delete iWriteHelper;
    }

// -----------------------------------------------------------------------------
// CHttpCacheEntry::SetState
//
// -----------------------------------------------------------------------------
//
void CHttpCacheEntry::SetState( TCacheEntryState aState )
    {
    // Add entry to the eviction table once it gets completed
    if ( aState == ECacheComplete && !iEvictionCandidate )
        {
        // don't add it twice
        iEvictionHandler->Insert( *this );
        iEvictionCandidate = ETrue;
        }

    iState = aState;
    }

// -----------------------------------------------------------------------------
// CHttpCacheEntry::SetFileNameL
//
// -----------------------------------------------------------------------------
//
void CHttpCacheEntry::SetFileNameL( const TFileName& aFileName )
    {
    delete iFileName;
    iFileName = NULL;

    iFileName = aFileName.AllocL();
    }


// -----------------------------------------------------------------------------
// CHttpCacheEntry::Accessed
//
// -----------------------------------------------------------------------------
//
void CHttpCacheEntry::Accessed()
    {
    TTime now;
    now.HomeTime();
    iLastAccessed = now.Int64();

    iRef++;

    if ( iEvictionCandidate )
        {
        iEvictionHandler->Accessed( *this );
        }

#ifdef __CACHELOG__
    _LIT( KAccessFormat, "entry accessed: %d" );
    TBuf<100> buf;
    buf.Format( KAccessFormat, iRef );
    HttpCacheUtil::WriteUrlToLog( 0, buf, iUrl->Des() );
#endif // __CACHELOG__

    }

// -----------------------------------------------------------------------------
// CHttpCacheEntry::SetBodySize
//
// -----------------------------------------------------------------------------
//
void CHttpCacheEntry::SetBodySize( TUint aBodySize )
    {
    if ( iBodySize && !aBodySize )
        {
        // Remove from the eviction table, this is no longer a candidate
        if ( iEvictionCandidate )
            {
            iEvictionHandler->Remove( *this );
            iEvictionCandidate = EFalse;
            }
        }
    iBodySize = aBodySize;
    }

// -----------------------------------------------------------------------------
// CHttpCacheEntry::SetProtected
//
// -----------------------------------------------------------------------------
//
void CHttpCacheEntry::SetProtected()
    {
    iProtected = ETrue;
    iRef = 50;

#ifdef __CACHELOG__
    HttpCacheUtil::WriteLog( 0, _L( "CHttpCacheEntry::SetProtected - protected item" ) );
#endif
    }

// -----------------------------------------------------------------------------
// CHttpCacheEntry::Internalize
//
// -----------------------------------------------------------------------------
//
TInt CHttpCacheEntry::Internalize( RReadStream& aReadStream, const TDesC& aDirectory )
    {
    TRAPD( err, InternalizeL( aReadStream, aDirectory ) );
    return err;
    }

// -----------------------------------------------------------------------------
// CHttpCacheEntry::InternalizeL
//
// -----------------------------------------------------------------------------
//
void CHttpCacheEntry::InternalizeL( RReadStream& aReadStream, const TDesC& aDirectory )
    {
    // url length
    TInt len;
    len = aReadStream.ReadInt32L();
    delete iUrl;
    iUrl=NULL;
    iUrl = HBufC8::NewL( len );
    TPtr8 ptr8( iUrl->Des() );
    // url
    aReadStream.ReadL( ptr8, len );

    // calculate full path and filename length
    // aDirectory/ + "x/xxxxxxxx" : note aDirectory has trailing '/'
    len = aDirectory.Length() + KSubdirNameLength + KFilenameLength;
    HBufC* filename = HBufC::NewLC( len );
    TPtr ptr( filename->Des() );

    // Read max char length of filename.
    // NOTE: The filename and filename length is calculated by the code in
    // HttpCacheUtil::GenerateNameLC. The sub directory is the same as the
    // last char of the filename, e.g. ..\A\0123DCBA
    TBuf<KFilenameLength> uniqueFilename;
    aReadStream.ReadL( uniqueFilename , KFilenameLength );
    TPtrC uniqueSubDir = uniqueFilename.Right(1);

    // assemble path and filename
    ptr.Format(_L("%S%S\\%S"), &aDirectory, &uniqueSubDir, &uniqueFilename);
    //
    SetFileNameL( filename->Des() );
    //
    CleanupStack::PopAndDestroy(); // filename
    // la
    TReal64 la;
    la = aReadStream.ReadReal64L();
    iLastAccessed = la;
    // ref
    iRef = aReadStream.ReadUint32L();
    // size
    iBodySize = aReadStream.ReadUint32L( );
    // size
    iHeaderSize = aReadStream.ReadUint32L( );
    // protected
    iProtected = aReadStream.ReadInt32L();
    // header data
    delete iHeaderBuffer;
    iHeaderBuffer = NULL;
    len = aReadStream.ReadInt32L();
    iHeaderBuffer = HBufC8::NewL(len);
    TPtr8 header_ptr( iHeaderBuffer->Des() );
    aReadStream.ReadL( header_ptr, len );
    //
    SetState( ECacheComplete );
    }

// -----------------------------------------------------------------------------
// CHttpCacheEntry::Externalize
//
// -----------------------------------------------------------------------------
//
TInt CHttpCacheEntry::Externalize( RWriteStream& aWriteStream, const TDesC& aDirectory )
    {
    TRAPD( err, ExternalizeL( aWriteStream, aDirectory ) );
    return err;
    }

// -----------------------------------------------------------------------------
// CHttpCacheEntry::Externalize
//
// -----------------------------------------------------------------------------
//
void CHttpCacheEntry::ExternalizeL( RWriteStream& aWriteStream, const TDesC& aDirectory )
    {
    // check directory matches filename
    ASSERT(aDirectory.CompareF(iFileName->Left(aDirectory.Length())) == 0);

    // url length
    aWriteStream.WriteInt32L( iUrl->Length() );
    // url
    aWriteStream.WriteL( iUrl->Des() );
    // filename
    // know that filenames are 8 chars and no extension. Can reconstruct on
    // import from directory and last char. See HttpCacheUtil::GenerateNameLC.
    aWriteStream.WriteL( iFileName->Des().Right( KFilenameLength ) );
    // la
    aWriteStream.WriteReal64L( iLastAccessed );
    // ref
    aWriteStream.WriteUint32L( iRef );
    // size
    aWriteStream.WriteUint32L( iBodySize );
    // size
    aWriteStream.WriteUint32L( iHeaderSize );
    // protected
    aWriteStream.WriteInt32L( iProtected );
    // header data length
    aWriteStream.WriteInt32L( iHeaderBuffer->Length() );
    // header data
    aWriteStream.WriteL( iHeaderBuffer->Des() );
    }

// -----------------------------------------------------------------------------
// CHttpCacheEntry::Accessed
//
// -----------------------------------------------------------------------------
//
void CHttpCacheEntry::Accessed( TInt64 aLastAccessed, TUint16 aRef )
    {
    iLastAccessed = aLastAccessed;
    iRef = aRef;
    if ( iEvictionCandidate )
        {
        iEvictionHandler->Accessed( *this );
        }

#ifdef __CACHELOG__
    _LIT( KAccessFormat, "entry accessed: %d" );
    TBuf<100> buf;
    buf.Format( KAccessFormat, iRef );
    HttpCacheUtil::WriteUrlToLog( 0, buf, iUrl->Des() );
#endif // __CACHELOG__

    }

// -----------------------------------------------------------------------------
// CHttpCacheEntry::SetCacheBufferL
// NOTE: Cache buffer is created on:
// 1. Normal content entrypoint into CacheManager
//    CacheManager::ReceivedResponseHeadersL -> CacheHandler::ReceivedResponseHeadersL ->
//    CacheHandler::HandleResponseOkL (calls method - SetCacheBuffer, needed to
//    accumulate body content on multiple CacheHandler::ReceivedBodyDataL calls)
// 2. Multipart content entrypoint into CacheManager
//    CacheManager::SaveL -> CacheHandler::SaveL -> CacheHandler::SaveBuffer ->
//    CacheStreamHandler::SaveBodyData (calls this method - SetCacheBufferL, needed
//    because cacheBuffer=null and single call made, no accumulation of body data)
// -----------------------------------------------------------------------------
//
void CHttpCacheEntry::SetCacheBufferL( TInt aCacheBufferSize )
    {
    // Delete cacheBuffer and null, a way to zero buffer and handle if NewL leave
    delete iCacheBuffer;
    iCacheBuffer = NULL;

    if ( aCacheBufferSize > 0 )
        {
        iCacheBuffer = CSegmentedHeapBuffer::NewL( aCacheBufferSize, KBufferGranularity4k );
        }
    }

// -----------------------------------------------------------------------------
// CHttpCacheEntry::CreateHeaderBufferL
//
// -----------------------------------------------------------------------------
//
void CHttpCacheEntry::CreateHeaderBufferL( TInt aHeaderBufferSize )
    {
    // Delete cacheBuffer and null, a way to zero buffer and handle if NewL leave
    delete iHeaderBuffer;
    iHeaderBuffer = NULL;

    if ( aHeaderBufferSize > 0 )
        {
        iHeaderBuffer = HBufC8::NewL( aHeaderBufferSize );
        }
    SetHeaderSize( aHeaderBufferSize );
    }

// -----------------------------------------------------------------------------
// CHttpCacheEntry::CreateHeaderBufferL
//
// -----------------------------------------------------------------------------
//
void CHttpCacheEntry::CreateHeaderBufferL( const TDesC8& aHeaderData )
    {
    // Delete cacheBuffer and null, a way to zero buffer and handle if NewL leave
    delete iHeaderBuffer;
    iHeaderBuffer = NULL;

    TInt aHeaderBufferSize = aHeaderData.Length();
    if ( aHeaderBufferSize > 0 )
        {
        iHeaderBuffer = aHeaderData.AllocL();
        }
    SetHeaderSize( aHeaderBufferSize );
    }

// -----------------------------------------------------------------------------
// CHttpCacheEntry::BodyData
//
// -----------------------------------------------------------------------------
//
CSegmentedHeapBuffer& CHttpCacheEntry::BodyData()
    {
    return *iCacheBuffer;
    }

// -----------------------------------------------------------------------------
// CHttpCacheEntry::BodyFile
//
// -----------------------------------------------------------------------------
//
RFile& CHttpCacheEntry::BodyFile()
    {
    return iBodyFile;
    }

// -----------------------------------------------------------------------------
// CHttpCacheEntry::HeaderData
//
// -----------------------------------------------------------------------------
//
TDesC8& CHttpCacheEntry::HeaderData()
    {
    return *iHeaderBuffer;
    }

// -----------------------------------------------------------------------------
// CHttpCacheEntry::BodyWriteInProgress
//
// -----------------------------------------------------------------------------
//
void CHttpCacheEntry::BodyWriteInProgress()
    {
    SetBodyDataPartiallyWritten( ETrue );
    }

// -----------------------------------------------------------------------------
// CHttpCacheEntry::BodyWriteComplete
//
// -----------------------------------------------------------------------------
//
void CHttpCacheEntry::BodyWriteComplete()
    {
    iCacheBuffer->Reset();
    SetBodyDataPartiallyWritten( EFalse );
    SetBodyDataCached( EFalse );
    }

// -----------------------------------------------------------------------------
// CHttpCacheEntry::WriteBodyDataAsync
//
// -----------------------------------------------------------------------------
//
void CHttpCacheEntry::WriteBodyDataAsync(TRequestStatus& aStatus)
    {
    delete iWriteHelper;
    iWriteHelper = NULL;
    TRAP_IGNORE( iWriteHelper = CHttpCacheEntryAsyncWriteHelper::NewL( this, aStatus ) );
    }

// -----------------------------------------------------------------------------
// CHttpCacheEntry::CancelBodyWrite
//
// -----------------------------------------------------------------------------
//
void CHttpCacheEntry::CancelBodyWrite()
    {
    if ( BodyDataPartiallyWritten() && iWriteHelper )
        {
        iWriteHelper->Cancel();
        }
    }

void CHttpCacheEntry::SetDeleteObserver(MHttpCacheEntryDeleteObserver* aObserver)
    {
    iDeleteObserver = aObserver;
    }

void CHttpCacheEntry::ClearDeleteObserver()
    {
    iDeleteObserver = NULL;
    }
//  End of File
