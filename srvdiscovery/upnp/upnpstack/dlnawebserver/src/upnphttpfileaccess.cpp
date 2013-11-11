/** @file
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available 
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Declares HttpServer class.
 *
*/


// INCLUDE FILES
#include "upnphttpsession.h"
#include "upnphttpfileaccess.h"
#include "upnpcommonupnplits.h"
#include "upnpfileutils.h"
#include "upnpcons.h"
#define KLogFile _L("DLNAWebServer.txt")
#include "upnpcustomlog.h"

#include <e32math.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpHttpFileAccess::NewL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
CUpnpHttpFileAccess* CUpnpHttpFileAccess::NewL( CUpnpHttpSession* aSession,
    const TDesC8& aHeaderBuffer, const TDesC16& aFilename, TInt aFileSize )
    {
    LOGSH( aHandle, "CUpnpHttpFileAccess::NewL" );
    CUpnpHttpFileAccess* self = new (ELeave) CUpnpHttpFileAccess(
            aFileSize );
    CleanupStack::PushL( self );
    self->ConstructL( aSession, aHeaderBuffer, aFilename );
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpFileAccess::NewL
// C++ default constructor, called when saving download
// -----------------------------------------------------------------------------
//
CUpnpHttpFileAccess* CUpnpHttpFileAccess::NewL( CUpnpHttpSession* aSession,
    const TDesC16& aFilename )
    {
    LOG_FUNC_NAME;
    CUpnpHttpFileAccess* self = new (ELeave) CUpnpHttpFileAccess();
    CleanupStack::PushL( self );
    self->ConstructL( aSession, aFilename );
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpFileAccess::~CUpnpHttpBuffer
// C++ default destructor
// -----------------------------------------------------------------------------
//
CUpnpHttpFileAccess::~CUpnpHttpFileAccess()
    {
    LOGS1( "%i, CUpnpHttpFileAccess::~CUpnpHttpFileAccess", this );
    if ( iIsDownload )
        {
        if ( !iIsDeleted )
            {
            iFile.Close( );
            if ( (iTotalSizeOfDownload - iBytesWritten )!= 0
                    || ( !iIsChunkCompleted && iIsChunked ) )
                iFsSession.Delete( *iFileToServe );
            }

        }
    if ( iOpen )
        {
        iTargetFile.Close( );
        }

    iFsSession.Close( );

    delete iHeaderBuffer;

    delete iFileToServe;
    delete iOpenedFile;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpFileAccess::CUpnpHttpFileAccess
// C++ default constructor
// -----------------------------------------------------------------------------
//
CUpnpHttpFileAccess::CUpnpHttpFileAccess( TInt aFileSize )
    {
    LOGS1H( aHandle, "%i, CUpnpHttpFileAccess::CUpnpHttpFileAccess", this );
    SetPosOfFile( 0 );
    SetEndPosOfFile( KErrNotFound );
    SetPosOfHeader( 0 );
    iSizeOfFile = aFileSize;
    iHeadersSent = EFalse;
    iTotalSizeOfDownload = KErrNotFound;
    iIsDownload = EFalse;
    iIsDeleted = EFalse;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpFileAccess::CUpnpHttpFileAccess
// C++ default constructor, called when saving download
// -----------------------------------------------------------------------------
//
CUpnpHttpFileAccess::CUpnpHttpFileAccess()
    {
    LOGS1H( aHandle, "%i, CUpnpHttpFileAccess::CUpnpHttpFileAccess", this );
    SetPosOfFile( 0 );
    SetEndPosOfFile( KErrNotFound );
    iHeadersSent = ETrue;
    iTotalSizeOfDownload = KErrNotFound;
    iSizeOfFile = KErrNotFound;
    iIsDownload = ETrue;
    iBytesWritten = 0;
    iIsDeleted = EFalse;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpFileAccess::CUpnpHttpFileAccess
// Two-phased constructor, called when saving download
// -----------------------------------------------------------------------------
//


void CUpnpHttpFileAccess::ConstructL( CUpnpHttpSession* aSession,
    const TDesC16& aFilename )
    {
    LOGS1(
            "%i, CUpnpHttpFileAccess::ConstructL(CUpnpHttpSession*, TDesC16&)",
            this );

    User::LeaveIfError(iFsSession.Connect());
    iFileToServe = aFilename.AllocL(); 

    TInt error =  0; 
    iPosInFile = 0;
    iHeaderLength = 0;
    iSession = aSession;
    if (iSession->OverwriteExisting() && !iSession->SaveAtOffset())
        {
        error = iFile.Replace(iFsSession, *iFileToServe, EFileWrite
                | EFileShareAny);
        User::LeaveIfError(error);
        }
    else if (iSession->OverwriteExisting() && iSession->SaveAtOffset())
        {
        error = iFile.Open(iFsSession, *iFileToServe, EFileWrite
                | EFileShareAny);
        if (error != KErrNotFound)
            {
            TInt size = 0;
            error = iFile.Size(size);
            if (size >= (iSession->Offset()))
                {
                iPosInFile = iSession->Offset();
                }
            else
                {
                iFile.Close();
                error = iFile.Replace(iFsSession, *iFileToServe, EFileWrite
                        | EFileShareAny);
                User::LeaveIfError(error);
                }
            }
        else
            {
            error = iFile.Create(iFsSession, *iFileToServe, EFileWrite
                    | EFileShareAny);
            User::LeaveIfError(error);
            }
        }
    else
        {
        error = iFile.Open(iFsSession, *iFileToServe, EFileWrite
                | EFileShareAny);
        if (error == KErrNotFound)
            {
            error = iFile.Create(iFsSession, *iFileToServe, EFileWrite
                    | EFileShareAny);
            User::LeaveIfError(error);
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpHttpFileAccess::ConstructL
// Two-phased constructor, called when serving file
// -----------------------------------------------------------------------------
//
void CUpnpHttpFileAccess::ConstructL( CUpnpHttpSession* aSession,
    const TDesC8& aHeaderBuffer, const TDesC16& aFilename )
    {
    LOGS1H(
        iHandle,
        "%i, CUpnpHttpFileAccess::ConstructL(CUpnpHttpSession*, TDesC8&, TDesC16&)",
        this );
    iHeaderBuffer = HBufC8::NewL( aHeaderBuffer.Length( ) );
    iHeaderBuffer->Des().Zero( );
    iHeaderBuffer->Des().Append( aHeaderBuffer );

    iFileToServe = HBufC16::NewL( aFilename.Length( ) );
    iFileToServe->Des().Zero( );
    iFileToServe->Des().Append( aFilename );

    iHeaderLength = aHeaderBuffer.Length( );
    User::LeaveIfError( iFsSession.Connect( ) );
    iSession = aSession;
    iOpenedFile = KNullDesC().AllocL( );
    }

// -----------------------------------------------------------------------------
// CUpnpHttpFileAccess::SetPosOfFile
// 
// -----------------------------------------------------------------------------
//
void CUpnpHttpFileAccess::SetPosOfFile( TUint aNewPos )
    {
    iPosInFile = aNewPos;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpFileAccess::PosOfFile
// 
// -----------------------------------------------------------------------------
//
TUint CUpnpHttpFileAccess::PosOfFile()
    {
    return iPosInFile;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpFileAccess::SetPosOfFile
// 
// -----------------------------------------------------------------------------
//
void CUpnpHttpFileAccess::SetEndPosOfFile( TInt aNewEndPos )
    {
    iEndPosOfFile = aNewEndPos;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpFileAccess::PosOfFile
// 
// -----------------------------------------------------------------------------
//
TInt CUpnpHttpFileAccess::EndPosOfFile()
    {
    return iEndPosOfFile;
    }
// -----------------------------------------------------------------------------
// CUpnpHttpFileAccess::GetHeaders
// 
// -----------------------------------------------------------------------------
//
const TDesC8& CUpnpHttpFileAccess::GetHeaders()
    {
    return (iHeaderBuffer) ? *iHeaderBuffer : KNullDesC8( );
    }

// -----------------------------------------------------------------------------
// CUpnpHttpFileAccess::SetPosOfHeader
// 
// -----------------------------------------------------------------------------
//	
void CUpnpHttpFileAccess::SetPosOfHeader( TUint aNewPos )
    {
    iPosToReadHeader = aNewPos;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpFileAccess::SetTotalSize
// 
// -----------------------------------------------------------------------------
//
void CUpnpHttpFileAccess::SetTotalSize( TInt aSizeToSet )
    {
    iTotalSizeOfDownload = aSizeToSet;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpFileAccess::SaveL
// 
// -----------------------------------------------------------------------------
//
TInt CUpnpHttpFileAccess::SaveL( TDesC8& aBuffer )
    {
    LOGS1( "%i, CUpnpHttpFileAccess::SaveL()", this );
    if ( iIsDeleted )
        {
        LOGS( "file closed" );
        return KErrGeneral;
        }
    LOGS( "file not closed" );
    TInt toWrite = (EncodingMode( )|| (TransferTotal( ) == KErrNotFound ) )
        ? aBuffer.Length( ) : (TransferTotal( )- iBytesWritten);

    if ( aBuffer.Size( ) < toWrite )
        {
        toWrite = aBuffer.Size( );
        }

    if ( UpnpFileUtil::CheckDiskSpaceShortL( iFileToServe->Des( ), toWrite,
        iFsSession ) )
        {
        DeleteFile( );
        return EHttpInsufficientStorage;
        }

    TInt error = KErrNone;

    // At first time iPosInFile == 0 or range offset, see ConstructL, and the next time 
    // saving will continue at stopped point. 	

    error = iFile.Write( iPosInFile, aBuffer.Right( toWrite ) );
    if ( error != KErrNone )
        {
        return error;
        }
    iPosInFile += toWrite;
    iBytesWritten += toWrite;
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CUpnpHttpFileAccess::TransferredBytes
// 
// -----------------------------------------------------------------------------
//
TInt CUpnpHttpFileAccess::TransferredBytes()
    {
    LOGS1( "%i, CUpnpHttpFileAccess::TransferredBytes()", this);
    TInt sent = 0;
    if ( !iHeadersSent )
        {
        sent = iPosToReadHeader;
        }
    else
        {
        sent = PosOfFile();
        }
    return sent;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpFileAccess::TransferTotal
// 
// -----------------------------------------------------------------------------
//
TInt CUpnpHttpFileAccess::TransferTotal()
    {
    LOGS1( "%i, CUpnpHttpFileAccess::TransferTotal()", this);
    if ( iTotalSizeOfDownload == KErrNotFound )
        {
        return iSizeOfFile;
        }
    else
        {
        return iTotalSizeOfDownload;
        }
    }

// -----------------------------------------------------------------------------
// CUpnpHttpFileAccess::FileToServe
// 
// -----------------------------------------------------------------------------
//
TDesC16& CUpnpHttpFileAccess::FileToServe()
    {
    return *iFileToServe;
    }
   
// -----------------------------------------------------------------------------
// CUpnpHttpFileAccess::GetL
// 
// -----------------------------------------------------------------------------
//
TBool CUpnpHttpFileAccess::GetL( TPtr8& aPointer, TInt aBytesToSend )
    {
    LOGS1H( aHandle, "%i, CUpnpHttpFileAccess::GetL", this );
    if ( iFileToServe->Compare( *iOpenedFile ) ) // ignore if the same file
        {
        if ( iOpen )
            {
            iTargetFile.Close( );
            }
        iOpen = EFalse;
        }

    if ( !iOpen )
        {
        User::LeaveIfError( iTargetFile.Open( iFsSession, *iFileToServe,
            EFileRead|EFileShareReadersOnly ) );
        HBufC* tmp = iFileToServe->AllocL( );
        delete iOpenedFile;
        iOpenedFile = tmp;
        iOpen = ETrue;
        }

    User::LeaveIfError( iTargetFile.Seek( ESeekStart, iPosInFile ) );

    TInt BytesToRead = aBytesToSend;

    // Get min value from iEndPosInFile and buffer length 
    // iEndPosInFile + - iPosInFile 1 because indicated byte should also be included
    if ( EndPosOfFile( ) != KErrNotFound )
        BytesToRead = (EndPosOfFile( ) - iPosInFile + 1) < BytesToRead
            ? (EndPosOfFile( ) - iPosInFile + 1) : BytesToRead;

    LOGS1( "BytesToRead: %d", BytesToRead );

    User::LeaveIfError( iTargetFile.Read( aPointer, BytesToRead ) );
    iPosInFile = iPosInFile + aPointer.Length( );

    return IsEndOfFile();
    }
// -----------------------------------------------------------------------------
//CUpnpHttpFileAccess::DeleteServedFile
// 
// -----------------------------------------------------------------------------
//
void CUpnpHttpFileAccess::DeleteFile()
    {
    LOGS1( "%i, CUpnpHttpFileAccess::DeleteFile()", this );
    if ( !iIsDeleted )
        {
        iFile.Close( );
        iFsSession.Delete( *iFileToServe );
        iIsDeleted = ETrue;
        }

    }

// -----------------------------------------------------------------------------
// CUpnpHttpFileAccess::Download
// 
// -----------------------------------------------------------------------------
//
TBool CUpnpHttpFileAccess::Download()
    {
    return iIsDownload;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpFileAccess::FileExist
// 
// -----------------------------------------------------------------------------
//
TBool CUpnpHttpFileAccess::FileExist()
    {
    return !iIsDeleted;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpFileAccess::BytesWritten
// 
// -----------------------------------------------------------------------------
//
TBool CUpnpHttpFileAccess::BytesWritten()
    {
    return (iIsDownload ? iBytesWritten : 0);   
    }

// -----------------------------------------------------------------------------
// CUpnpHttpFileAccess::SetEncodingMode(TBool aIsChunked)
// 
// -----------------------------------------------------------------------------
//
void CUpnpHttpFileAccess::SetEncodingMode( TBool aIsChunked )
    {
    iIsChunked = aIsChunked;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpFileAccess::EncodingMode()
// 
// -----------------------------------------------------------------------------
//
TBool CUpnpHttpFileAccess::EncodingMode()
    {
    return iIsChunked;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpFileAccess::SetTransferTotal(aLength)
// 
// -----------------------------------------------------------------------------
//
void CUpnpHttpFileAccess::SetTransferTotal( TInt aLength )
    {
    iTotalSizeOfDownload = aLength;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpFileAccess::HeadersSent
// 
// -----------------------------------------------------------------------------
//
void CUpnpHttpFileAccess::SetHeadersSent()
    {
    delete iHeaderBuffer;
    iHeaderBuffer = NULL;
    iPosToReadHeader = NULL;
    iHeadersSent = ETrue;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpFileAccess::HeadersSent()
// 
// -----------------------------------------------------------------------------
//
TBool CUpnpHttpFileAccess::HeadersSent()
    {
    return iHeadersSent;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpFileAccess::SetChunkComplete( TBool aCompleted )
// 
// -----------------------------------------------------------------------------
//
void CUpnpHttpFileAccess::SetChunkCompleted( TBool aCompleted )
    {
    iIsChunkCompleted = aCompleted;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpFileAccess::IsEndOfFile
// 
// -----------------------------------------------------------------------------
//
TBool CUpnpHttpFileAccess::IsEndOfFile()
    {
    TInt limit = iSizeOfFile;
    if ( EndPosOfFile() != KErrNotFound )
        {
        limit = ( EndPosOfFile() + 1 ) < iSizeOfFile ? ( EndPosOfFile() + 1 ) : iSizeOfFile;
        }
        
    if ( iPosInFile >= limit )
        {
        iTargetFile.Close();
        iOpen = EFalse;
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }
    
// End Of File

