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
* Description:  Implementation of CHttpCacheStreamHandler
*
*/

// INCLUDE FILES
#include <f32file.h>
#include "HttpCacheStreamHandler.h"
#include "HttpCacheEntry.h"
#include "HttpCacheUtil.h"
#include "HttpCacheHandler.h"
#include <centralrepository.h>
#include <sysutildomaincrkeys.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
const TInt KHttpCacheActiveCount = 20;
#if 0
const TInt KHttpCacheChunkSize = 2048;
#endif // 0
// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHttpCacheStreamHandler::CHttpCacheStreamHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CHttpCacheStreamHandler::CHttpCacheStreamHandler(RFs& aRFs)
    : iRfs( aRFs )
    {
    }

// -----------------------------------------------------------------------------
// CHttpCacheStreamHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHttpCacheStreamHandler::ConstructL(
    const TDesC& aDirectory,
    TInt aCriticalLevel)
    {
    iActiveEntries = new( ELeave )CArrayPtrFlat<CHttpCacheEntry>( KHttpCacheActiveCount );
    // get drive letter for sysutil
    TParsePtrC pathParser( aDirectory );
    iDrive = pathParser.Drive();
    iCriticalLevel = aCriticalLevel;
    }

// -----------------------------------------------------------------------------
// CHttpCacheStreamHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHttpCacheStreamHandler* CHttpCacheStreamHandler::NewL(
    const TDesC& aDirectory ,
    TInt aCriticalLevel,
    RFs& aRFs)
    {
    CHttpCacheStreamHandler* self = new( ELeave ) CHttpCacheStreamHandler(aRFs);

    CleanupStack::PushL( self );
    self->ConstructL( aDirectory , aCriticalLevel);
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CHttpCacheStreamHandler::~CHttpCacheStreamHandler()
    {
    if ( iActiveEntries )
        {
        iActiveEntries->ResetAndDestroy();
        }
    delete iActiveEntries;
    }

// -----------------------------------------------------------------------------
// CHttpCacheStreamHandler::InitialiseCacheEntryL
// -----------------------------------------------------------------------------
//
void CHttpCacheStreamHandler::InitialiseCacheEntryL(CHttpCacheEntry& aCacheEntry)
    {
    // create a filename for the cache entry.
    TPath sessionPath;
    User::LeaveIfError( iRfs.SessionPath( sessionPath ) );

    // Given the full URL, generates a fully qualified path for saving the HTTP response
    HBufC* bodyFileName = HttpCacheUtil::GenerateNameLC( aCacheEntry.Url(), sessionPath );
    TPtrC bodyFileNamePtr( *bodyFileName );
    aCacheEntry.SetFileNameL(bodyFileNamePtr);

    CleanupStack::PopAndDestroy(bodyFileName);

    // any more one-time initialisation to go here.
    aCacheEntry.SetState( CHttpCacheEntry::ECacheInitialized );

    // since this only happens one time, we can check if we have files left over with no index entry
    // we're too late to reuse any information stored in there, so lets just delete them.  This might prevent any
    // problems later down the line..
    iRfs.Delete(aCacheEntry.Filename());
    // header filename
    TFileName headerFileName;
    HttpCacheUtil::GetHeaderFileName( aCacheEntry.Filename(), headerFileName );
    iRfs.Delete(headerFileName);
    }

// -----------------------------------------------------------------------------
// CHttpCacheStreamHandler::Erase
//
// -----------------------------------------------------------------------------
//
void CHttpCacheStreamHandler::Erase( CHttpCacheEntry& aCacheEntry )
    {
    HttpCacheUtil::WriteUrlToLog( 0, _L( "CHttpCacheStreamHandler::Erase - erase files associated with" ), aCacheEntry.Url() );

    // just in case it's busy being written out...
    aCacheEntry.CancelBodyWrite();

    // Delete body file
#ifndef __CACHELOG__
    iRfs.Delete( aCacheEntry.Filename() );
#else
    TInt statusBody( KErrNotFound );
    statusBody = iRfs.Delete( aCacheEntry.Filename() );
#endif
    // Adjust the size
    iContentSize -= aCacheEntry.BodySize();
    iContentSize -= aCacheEntry.HeaderSize();

#ifdef __CACHELOG__
    if ( statusBody == KErrNone )
        {
        HttpCacheUtil::WriteLogFilenameAndUrl( 0,
                                           _L("CHttpCacheStreamHandler::Erase - SUCCESS bodyFile delete"),
                                           aCacheEntry.Filename(),
                                           aCacheEntry.Url(),
                                           aCacheEntry.BodySize(),
                                           ELogEntrySize );
        }
    else if ( statusBody == KErrNotFound )
        {
        HttpCacheUtil::WriteLogFilenameAndUrl( 0,
                                           _L("CHttpCacheStreamHandler::Erase - CHECK bodyFile not found."),
                                           aCacheEntry.Filename(),
                                           aCacheEntry.Url(),
                                           statusBody,
                                           ELogFileErrorCode );
        }
    else
        {
    HttpCacheUtil::WriteLogFilenameAndUrl( 0,
                                       _L("CHttpCacheStreamHandler::Erase - ERROR bodyFile delete"),
                                       aCacheEntry.Filename(),
                                       aCacheEntry.Url(),
                                       statusBody,
                                       ELogFileErrorCode );
        }
#endif //__CACHELOG__
    }

// -----------------------------------------------------------------------------
// CHttpCacheStreamHandler::HeadersL
//
// -----------------------------------------------------------------------------
//
HBufC8* CHttpCacheStreamHandler::HeadersL( CHttpCacheEntry& aCacheEntry )
    {
    return aCacheEntry.HeaderData().AllocL();
    }

// -----------------------------------------------------------------------------
// CHttpCacheStreamHandler::NextChunkL
//
// -----------------------------------------------------------------------------
//
HBufC8* CHttpCacheStreamHandler::NextChunkL(
    CHttpCacheEntry& aCacheEntry,
    TBool& aLastChunk )
    {
    HBufC8 *bodyStr = NULL;

    if ( !aCacheEntry.BodyDataCached() && OpenBodyFile(aCacheEntry) )
        {
        CleanupClosePushL( aCacheEntry.BodyFile() );

        // read body
        TInt size;
        TInt err( aCacheEntry.BodyFile().Size( size ) );
        if ( err == KErrNone && size > 0 )
            {
            bodyStr = HBufC8::NewL( size );
            TPtr8 ptr( bodyStr->Des() );
            //
            err = aCacheEntry.BodyFile().Read( ptr, size );
#ifdef __CACHELOG__
            if ( err != KErrNone ) {
                HttpCacheUtil::WriteLogFilenameAndUrl( 0,
                                                   _L("CCHttpCacheStreamEntry::NextChunkL - bodyFile.read"),
                                                   aCacheEntry.Filename(),
                                                   aCacheEntry.Url(),
                                                   err,
                                                   ELogFileErrorCode );
                }
            else {
                HttpCacheUtil::WriteLogFilenameAndUrl( 0,
                                                   _L("CCHttpCacheStreamEntry::NextChunkL - bodyFile.read"),
                                                   aCacheEntry.Filename(),
                                                   aCacheEntry.Url(),
                                                   ptr.Length(),
                                                   ELogEntrySize );
                }
#endif  // __CACHELOG__
            }
        // Close body file
        CleanupStack::PopAndDestroy(1);
        }
    else
        {
        // reuse stored data if we have any.
        CSegmentedHeapBuffer& buffer = aCacheEntry.BodyData();
        TInt size = buffer.Length();
        bodyStr = HBufC8::NewL( size );
        TPtr8 ptr(bodyStr->Des());

        TInt readSegment = 0;
        TInt count = buffer.Count();
        while(readSegment < count)
            {
            TPtrC8 source = buffer.GetSegmentData(readSegment);
            ptr.Append(source);
            }
        }
    aLastChunk = ETrue;
    return bodyStr;
    }

// -----------------------------------------------------------------------------
// CHttpCacheStreamHandler::SaveHeaders
//
// -----------------------------------------------------------------------------
//
TBool CHttpCacheStreamHandler::SaveHeaders(
    CHttpCacheEntry& aCacheEntry,
    const TDesC8& aHeaderStr )
    {
    TBool headerSaved( EFalse );
    TInt headerLen = aHeaderStr.Length();

    if ( headerLen && IsDiskSpaceAvailable( headerLen ) )
        {
        TRAPD(err, aCacheEntry.CreateHeaderBufferL(aHeaderStr));
        if ( err == KErrNone )
            {
            iContentSize += aCacheEntry.HeaderSize();
            headerSaved = ETrue;
            }
        }
    return headerSaved;
    }

// -----------------------------------------------------------------------------
// CHttpCacheStreamHandler::RemoveHeaders
//
// -----------------------------------------------------------------------------
//
void CHttpCacheStreamHandler::RemoveHeaders( CHttpCacheEntry& aCacheEntry )
    {
    iContentSize -= aCacheEntry.HeaderSize();

    TRAP_IGNORE( aCacheEntry.CreateHeaderBufferL( 0 ) );
    }

// -----------------------------------------------------------------------------
// CHttpCacheStreamHandler::SaveBodyData
//
// -----------------------------------------------------------------------------
//
TBool CHttpCacheStreamHandler::SaveBodyData(
    CHttpCacheEntry& aCacheEntry,
    const TDesC8& aBodyStr )
    {
    TInt bodySaved( KErrNone );
    TInt newBodyLength( aBodyStr.Length() );

    if ( newBodyLength )
        {
        TInt remainder = 0;
        CSegmentedHeapBuffer& cacheBuffer = aCacheEntry.BodyData();

        // Add data to the buffer
        TRAPD( err, cacheBuffer.AppendL(remainder, aBodyStr) );
        if ( err == KErrNone )
            {
            aCacheEntry.SetBodyDataCached(ETrue);
            }
        else
            {
            // We failed to allocate memory to store the new data in the current buffer.
            // Check to see if it's possible to write it to disk instead.
            TBool bodyFileCreated = CreateNewBodyFile( aCacheEntry );
            if ( !bodyFileCreated )
                {
                return EFalse;
                }

            TBool enoughSpace;
            enoughSpace = IsDiskSpaceAvailable( cacheBuffer.Length() + remainder );
            if ( enoughSpace )
                {
                // In this case, we have not been able to store all the data.
                // if there is enough space on disk to write everything we know
                // about now, we will flush the current buffer out synchronously.
                TInt block=0;
                TInt count = cacheBuffer.Count();
                while ( bodySaved == KErrNone && count > block )
                    {
                    TPtrC8 buf = cacheBuffer.GetSegmentData(block);
                    bodySaved = aCacheEntry.BodyFile().Write(buf);
                    }
                }
            else
                {
                // disk too full, drop the cache data
                bodySaved = KErrDiskFull;
                // reset buffers
                cacheBuffer.Reset();
                }

            if ( bodySaved == KErrNone )
                {
                // We have completed writing out the cached data, now
                // try to save the new data if we haven't run out of disk space.
                bodySaved = aCacheEntry.BodyFile().Write(aBodyStr.Right(remainder));
                }
            aCacheEntry.SetBodyDataCached(EFalse);
            aCacheEntry.BodyFile().Close();
            }
        // update size information
        aCacheEntry.SetBodySize( aCacheEntry.BodySize() + newBodyLength );
        iContentSize += aBodyStr.Length();
        }
    return ( bodySaved == KErrNone );
    }

// -----------------------------------------------------------------------------
// CHttpCacheStreamHandler::RemoveBodyData
//
// -----------------------------------------------------------------------------
//
void CHttpCacheStreamHandler::RemoveBodyData( CHttpCacheEntry& aCacheEntry )
    {
#ifdef __CACHELOG__
     HttpCacheUtil::WriteLogFilenameAndUrl( 0,
                                           _L("CHttpCacheStreamHandler::RemoveBodyData - CLEAR iCacheBuffer, entrySize, and iBodyFile"),
                                           aCacheEntry.Filename(),
                                           aCacheEntry.Url(),
                                           aCacheEntry.BodySize(),
                                           ELogEntrySize );
#endif

    // Remove data
    iContentSize -= aCacheEntry.BodySize();
    aCacheEntry.SetBodySize( 0 );
    aCacheEntry.BodyData().Reset();
    if ( OpenBodyFile(aCacheEntry) )
        {
        aCacheEntry.BodyFile().SetSize( 0 );
        aCacheEntry.BodyFile().Close();
        }
    }

// -----------------------------------------------------------------------------
// CHttpCacheStreamHandler::Flush
//
// -----------------------------------------------------------------------------
//
TBool CHttpCacheStreamHandler::Flush( CHttpCacheEntry& aCacheEntry )
    {
    TInt err( KErrNone );
    TBool bFlushed( EFalse );

    TRAP( err, bFlushed = FlushL( aCacheEntry ) );
    if ( err || !bFlushed )
        {
        return EFalse;
        }

    return ETrue;
    }

// -----------------------------------------------------------------------------
// CHttpCacheStreamHandler::FlushL
//
// -----------------------------------------------------------------------------
//
TBool CHttpCacheStreamHandler::FlushL( CHttpCacheEntry& aCacheEntry )
    {
    TInt saveOk( KErrNone );

#ifdef __CACHELOG__
    HttpCacheUtil::WriteFormatLog(0, _L("CACHEPOSTPONE:  >>StreamHander::Flush object %08x"), &aCacheEntry );
#endif
    CSegmentedHeapBuffer& cacheBuffer = aCacheEntry.BodyData();

    TInt dataLength = cacheBuffer.Length() + aCacheEntry.HeaderData().Length();

    if ( !IsDiskSpaceAvailable( dataLength ) || !CreateNewBodyFile( aCacheEntry ) )
        {
#ifdef __CACHELOG__
        HttpCacheUtil::WriteLog(0, _L("CACHEPOSTPONE:    Flush failed: not enough space or cannot create file."));
#endif
        // can't flush if not enough space, or cannot create new files
        return EFalse;
        }

    // files are open, push them onto cleanup stack.
    CleanupClosePushL( aCacheEntry.BodyFile() );

    if ( aCacheEntry.BodyDataCached() )
        {
#ifdef __CACHELOG__
        HttpCacheUtil::WriteLog(0, _L("CACHEPOSTPONE:    Body Data is Cached"));
#endif
        // append body
        TInt segment=0;
        TInt count=cacheBuffer.Count();
#ifdef __CACHELOG__
        HttpCacheUtil::WriteFormatLog(0, _L("CACHEPOSTPONE:    %d segments stored"), count);
#endif
        while ( saveOk == KErrNone && count > segment )
            {
            TPtrC8 segBuf = cacheBuffer.GetSegmentData(segment);
            saveOk = aCacheEntry.BodyFile().Write( segBuf );
#ifdef __CACHELOG__
            HttpCacheUtil::WriteFormatLog(0, _L("CACHEPOSTPONE:    segment %d write returned %d"), (segment-1), saveOk );
#endif
            }

        cacheBuffer.Reset();
        aCacheEntry.SetBodyDataCached(EFalse);
        }
    // close files
    CleanupStack::PopAndDestroy(1);

    return ( saveOk == KErrNone );
    }

// -----------------------------------------------------------------------------
// CHttpCacheStreamHandler::FlushAsync
//
// -----------------------------------------------------------------------------
//
TInt CHttpCacheStreamHandler::FlushAsync(CHttpCacheEntry& aEntry, TRequestStatus& aStatus)
    {
    TInt saveOk( KErrNone );

    TInt datalen = aEntry.BodyData().Length() + aEntry.HeaderData().Length();

#ifdef __CACHELOG__
    HttpCacheUtil::WriteFormatLog(0, _L("CACHEPOSTPONE: CHttpCacheStreamEntry::FlushAsync called on object %08x. Cached data %d bytes"), &aEntry, datalen );
#endif

    if ( datalen && aEntry.BodyData().Length() ) // don't bother writing files which have no body data
        {
        if ( IsDiskSpaceAvailable( datalen ) && CreateNewBodyFile( aEntry, ETrue ) )
            {
#ifdef __CACHELOG__
            HttpCacheUtil::WriteFormatLog(0, _L("CACHEPOSTPONE:   Triggering Async write for object 0x%08x."), &aEntry);
#endif
            // trim any spare space available.
            aEntry.BodyData().Compress();
            aEntry.WriteBodyDataAsync(aStatus);
            aEntry.SetDelayedWriteInProgress(ETrue);
            }
        else
            {
#ifdef __CACHELOG__
            HttpCacheUtil::WriteLog(0, _L("CACHEPOSTPONE:   FAILED FlushAsync."));
#endif
            // !enoughSpace
            saveOk = KErrDiskFull;
            aEntry.BodyData().Reset();
            }
        }
    else
        {
#ifdef __CACHELOG__
        HttpCacheUtil::WriteFormatLog(0, _L("CACHEPOSTPONE:  Not writing file %S for entry %08x since it has no data."), &(aEntry.Filename()), &aEntry );
#endif
        TRequestStatus* stat = &aStatus;
        User::RequestComplete(stat, KErrNone);
        }

    return ( saveOk == KErrNone );
    }

// -----------------------------------------------------------------------------
// CHttpCacheStreamHandler::OpenBodyFile
// -----------------------------------------------------------------------------
//
TBool CHttpCacheStreamHandler::OpenBodyFile( CHttpCacheEntry& aCacheEntry )
    {
    TInt statusBody( KErrNotFound );

    // get body filename
    TFileName bodyFileName = aCacheEntry.Filename();

    statusBody = aCacheEntry.BodyFile().Open( iRfs, bodyFileName, EFileShareExclusive | EFileWrite );

    return ( statusBody == KErrNone );
    }

#if 0
// -----------------------------------------------------------------------------
// CHttpCacheStreamHandler::CreateNewFilesL
//
// -----------------------------------------------------------------------------
//
TBool CHttpCacheStreamHandler::CreateNewFilesL( CHttpCacheEntry& aCacheEntry )
    {
    TInt statusHeader( KErrNotFound );
    TInt statusBody( KErrNotFound );

    // Create header file name from body file name
    TFileName headerFileName;
    HttpCacheUtil::GetHeaderFileName( aCacheEntry.Filename(), headerFileName );

    // Create the body file or replace it, if it exists.
    statusBody = aCacheEntry.BodyFile().Replace( iRfs, aCacheEntry.Filename(), EFileShareExclusive | EFileWrite );
    if ( statusBody == KErrNone )
        {
        // Header file should not fail
        statusHeader = aCacheEntry.HeaderFile().Replace( iRfs, headerFileName, EFileShareExclusive | EFileWrite );
        }

#ifdef __CACHELOG__
    HttpCacheUtil::WriteUrlToLog( 0, bodyFileNamePtr, aCacheEntry.Url() );
#endif

    TBool fileOk( statusHeader == KErrNone && statusBody == KErrNone );
    if ( !fileOk )
        {
        // Only the body file created, no header file, delete body file
        iRfs.Delete( aCacheEntry.Filename() );
        iRfs.Delete( headerFileName );

        aCacheEntry.SetBodySize( 0 );
        aCacheEntry.SetHeaderSize( 0 );

#ifdef __CACHELOG__
        HttpCacheUtil::WriteLog( 0, _L( "CHttpCacheEntry::CreateNewFilesL - DELETE body file, header file failed" ) );
#endif
//        __ASSERT_DEBUG( EFalse, User::Panic( _L("CHttpCacheHandler::CreateNewFilesL Panic"), KErrCorrupt )  );
        }

    return fileOk;
    }
#endif

// -----------------------------------------------------------------------------
// CHttpCacheStreamHandler::CreateNewBodyFile
//
// -----------------------------------------------------------------------------
//
TBool CHttpCacheStreamHandler::CreateNewBodyFile( CHttpCacheEntry& aCacheEntry, TBool aUseDirectIO )
    {
    TInt statusBody( KErrNotFound );

    // Create the body file or replace it, if it exists.
#ifdef BRDO_RFILE_WRITE_DIRECT_IO_FF
    statusBody = aCacheEntry.BodyFile().Replace( iRfs, aCacheEntry.Filename(), EFileShareExclusive | EFileWrite | (aUseDirectIO ? EFileWriteDirectIO : 0) );
#else
    statusBody = aCacheEntry.BodyFile().Replace( iRfs, aCacheEntry.Filename(), EFileShareExclusive | EFileWrite );
#endif

#ifdef __CACHELOG__
    HttpCacheUtil::WriteUrlToLog( 0, aCacheEntry.Filename(), aCacheEntry.Url() );
#endif

    if ( statusBody != KErrNone )
        {
        aCacheEntry.SetBodySize( 0 );
        aCacheEntry.BodyData().Reset();
#ifdef __CACHELOG__
        HttpCacheUtil::WriteLog( 0, _L( "CHttpCacheEntry::CreateNewBodyFileL - create body file failed!" ) );
#endif
        }
    return ( statusBody == KErrNone );
    }

// -----------------------------------------------------------------------------
// CHttpCacheStreamHandler::FindCacheEntryIndex
//
// -----------------------------------------------------------------------------
//
void CHttpCacheStreamHandler::FindCacheEntryIndex(
    const CHttpCacheEntry& aCacheEntry,
    TInt* aIndex )
    {
    for ( TInt i = 0; i < iActiveEntries->Count(); i++ )
        {
        CHttpCacheEntry* entry = iActiveEntries->At( i );

        if ( entry == &aCacheEntry )
            {
            if ( aIndex )
                {
                *aIndex = i;
                }
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CHttpCacheStreamHandler::IsDiskSpaceAvailable
//
// -----------------------------------------------------------------------------
//
TBool CHttpCacheStreamHandler::IsDiskSpaceAvailable( TInt aContentSize )
    {
    TBool diskSpaceAvailable( EFalse );
    TVolumeInfo vinfo;
    TInt errorCode = iRfs.Volume( vinfo, iDrive );

    if ( errorCode == KErrNone && ( vinfo.iFree - aContentSize ) > iCriticalLevel )
        {
        // We have space on the disk for the content
        diskSpaceAvailable = ETrue;
        }

    return diskSpaceAvailable;
    }

//  End of File
