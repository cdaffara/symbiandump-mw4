/** @file
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Handles download of one file at a time
*
*/


// System include files
#include <sysutil.h>
#ifndef __SERIES60_31__
#include <driveinfo.h>
#endif

// User include files
#include "httpfile.h"
#include "httpdownloadworker.h"
#include "httpheader.h"
#include "httpworkerobserver.h"
#include "upnpcompvariant.hrh"

// Constants
_LIT8( KModuleName, "HttpTransfer");
const TUint KDefaultBufferSize = 64000;
const TUint32 KMaxBufferSize   = 0x100000; //1MB


// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// CHttpDownloadWorker::NewL()
// (See comments in header file)
// --------------------------------------------------------------------------
//
CHttpDownloadWorker* CHttpDownloadWorker::NewL(
                                           MHttpTransferObserver& aObserver,
                                           TUint aIAPId,
                                           TUint aBufferSize,
                                           MHttpWorkerObserver& aCallback )
    {
    CHttpDownloadWorker* self = CHttpDownloadWorker::NewLC( aObserver,
                                                            aIAPId,
                                                            aBufferSize,
                                                            aCallback );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CHttpDownloadWorker::NewLC()
// (See comments in header file)
// --------------------------------------------------------------------------
//
CHttpDownloadWorker* CHttpDownloadWorker::NewLC(
                                           MHttpTransferObserver& aObserver,
                                           TUint aIAPId,
                                           TUint aBufferSize,
                                           MHttpWorkerObserver& aCallback )
    {
    CHttpDownloadWorker* self = new( ELeave ) CHttpDownloadWorker(
                                                               aIAPId,
                                                               aBufferSize,
                                                               aObserver,
                                                               aCallback );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// --------------------------------------------------------------------------
// CHttpDownloadWorker::CHttpDownloadWorker()
// (See comments in header file)
// --------------------------------------------------------------------------
//
CHttpDownloadWorker::CHttpDownloadWorker( TUint /*aIAPId*/,
                                          TUint aBufferSize,
                                          MHttpTransferObserver& aObserver,
                                          MHttpWorkerObserver& aCallback )
    {
    iProcessState = EHttpTransactionIdle;
    iFileSizeFromHeader = 0;
    iBytesTransferred = 0;
    iObserver = &aObserver;
    iCallback = &aCallback;

    if ( aBufferSize <= 0 )
        {
        iBufferSize = KDefaultBufferSize;
        }
    else if ( aBufferSize > KMaxBufferSize )
        {
        iBufferSize = KMaxBufferSize;
        }
    else
        {
        iBufferSize = aBufferSize;
        }
    }

// --------------------------------------------------------------------------
// CHttpDownloadWorker::ConstructL()
// (See comments in header file)
// --------------------------------------------------------------------------
//
void CHttpDownloadWorker::ConstructL()
    {
    CHttpTransferWorker::ConstructL();
    ConnectL();
    iSessionTimer = CHttpNotifyTimer::NewL( this );
    }

// --------------------------------------------------------------------------
// CHttpDownloadWorker::~CHttpDownloadWorker()
// (See comments in header file)
// --------------------------------------------------------------------------
//
CHttpDownloadWorker::~CHttpDownloadWorker()
    {
    delete iResponseBody;
    delete iSessionTimer;
    }

// --------------------------------------------------------------------------
// CHttpDownloadWorker::CheckDiskSpaceL()
// (See comments in header file)
// --------------------------------------------------------------------------
//
void CHttpDownloadWorker::CheckDiskSpaceL( TInt aSize )
    {
    TInt driveNum;
    TDriveInfo aninfo;
    if ( iProcessedFile->Path() )
        {
        TParse p;
        p.Set( *(iProcessedFile->Path() ), NULL, NULL );
        TPtrC pointer = p.Drive();
        TLex lineParser( pointer.Ptr() );
        TChar ch = lineParser.Get();

        //sets the default drive or the fulnd value
        User::LeaveIfError( iFsSession.CharToDrive( ch, driveNum ) );
        }
    else
        {
        TDriveInfo driveInfo;
        User::LeaveIfError( iFile.Drive( driveNum, driveInfo ) );
        }

    User::LeaveIfError( iFsSession.Drive( aninfo, driveNum ) );

    // get the drive status from DriveInfo in order to distinguish memory card
    // and hard disk from each other in multiple drive environment.
#ifndef __SERIES60_31__
    TUint driveStatus = 0;
    User::LeaveIfError( DriveInfo::GetDriveStatus(
        iFsSession, driveNum, driveStatus ) );
#endif

    // The case for default device memory (C-drive).
    if( aninfo.iType == EMediaNANDFlash )
        {
        if( SysUtil::FFSSpaceBelowCriticalLevelL
                                        ( &iFsSession, aSize ) )
             {
             User::Leave( KErrDiskFull );
             }
        }
#ifndef __SERIES60_31__
    // Internal FLASH memory
    // Multiple drive checking rules are applied here
    // note that in case of S60 3.1 this case is skipped
    else if( aninfo.iType == EMediaHardDisk &&
             aninfo.iDriveAtt & KDriveAttRemovable &&
             driveStatus & DriveInfo::EDriveInternal )
        {
        if( SysUtil::MMCSpaceBelowCriticalLevelL( &iFsSession, aSize ) )
            {
            User::Leave( KErrDiskFull );
            }
        }
#endif
    // Memory card (or practically any other memory type)
    // This covers also emulator's EMediaRam.
    else
        {
        if( SysUtil::DiskSpaceBelowCriticalLevelL
                                        ( &iFsSession, aSize, driveNum ) )
            {
            User::Leave( KErrDiskFull );
            }
        }
    }

// --------------------------------------------------------------------------
// CHttpDownloadWorker::WriteTailIntoFileL()
// (See comments in header file)
// --------------------------------------------------------------------------
//
void CHttpDownloadWorker::WriteTailIntoFileL()
    {
    if( iResponseBody )
        {
        if( iResponseBody->Length() > 0 )
            {
            CheckDiskSpaceL( iResponseBody->Length() );
            TPtr8 ptrResponseBody = iResponseBody->Des();
#ifndef RD_UPNP_REMOTE_ACCESS
            TInt size( 0 );
            iFile.Size( size );
            if( size >= 0 )
                {
                //transfer succeed, total size can be set to file size
                //it concerns chunked encoding or HTTP1.0 
                if ( iFileSizeFromHeader == 0 )
                    {
                    iFileSizeFromHeader = size;
                    }
                
#endif //RD_UPNP_REMOTE_ACCESS
                // Write the data on the disk
                User::LeaveIfError( iFile.Write( ptrResponseBody ) );
                
                delete iResponseBody;
                iResponseBody = NULL;

                // if the tracking is on send one more progress info
#ifdef RD_UPNP_REMOTE_ACCESS
                if ( iProcessedFile->TrackingOn() )
#else // RD_UPNP_REMOTE_ACCESS
                if ( iProcessedFile->TrackingOn()  )
#endif // RD_UPNP_REMOTE_ACCESS
                    {
                    iObserver->TransferProgress( iProcessedFile->Key(),
                                                 iBytesTransferred,
                                                 iFileSizeFromHeader );
                    }
#ifndef RD_UPNP_REMOTE_ACCESS
                }
#endif //RD_UPNP_REMOTE_ACCESS
            }
        }
    }

// --------------------------------------------------------------------------
// CHttpDownloadWorker::HandleResponseDataL()
// (See comments in header file)
// --------------------------------------------------------------------------
//
void CHttpDownloadWorker::HandleResponseDataL(
                                              RHTTPTransaction aTransaction )
    {
    MHTTPDataSupplier* pRespBody = aTransaction.Response().Body();

    if( pRespBody )
        {
        TPtrC8 dataChunk;
        pRespBody->GetNextDataPart( dataChunk );

        iSessionTimer->Cancel();
        iSessionTimer->AfterSeconds( KSessionTimeout );

        // increase the progress counter
        iBytesTransferred += dataChunk.Length();

        // Allocate memory for the data
        if( !iResponseBody )
            {
            // this is the first call so create the buffer to retain
            // response data
            iResponseBody = HBufC8::NewL( iBufferSize );
            }

        if ( iResponseBody->Length() + dataChunk.Length() > iBufferSize  )
            {
            iResponseBody = iResponseBody->ReAllocL(
                                                iResponseBody->Length() +
                                                dataChunk.Length() );
            }

        // Append the new data to the buffer
        TPtr8 ptrResponseBody = iResponseBody->Des();
        ptrResponseBody.Append( dataChunk );
        pRespBody->ReleaseData();
        // If buffer is full write data to memory card ( to file )
        if( iResponseBody->Length() > iBufferSize )
            {
#ifndef RD_UPNP_REMOTE_ACCESS
            TInt size( 0 );
            // if the file handle was not valid
            User::LeaveIfError( iFile.Size( size ) );
            if( size >= 0 )
                {
#endif // RD_UPNP_REMOTE_ACCESS
                CheckDiskSpaceL( ptrResponseBody.Length() );
                User::LeaveIfError( iFile.Write( ptrResponseBody ) );
                delete iResponseBody;
                iResponseBody = NULL;
#ifndef RD_UPNP_REMOTE_ACCESS
                }                      
#endif // RD_UPNP_REMOTE_ACCESS

            // Inform observer about the progress if progress tracking is on
#ifdef RD_UPNP_REMOTE_ACCESS
            if ( iProcessedFile->TrackingOn() )
#else // RD_UPNP_REMOTE_ACCESS
            if ( iProcessedFile->TrackingOn()  )
#endif // RD_UPNP_REMOTE_ACCESS
                {
                iObserver->TransferProgress( iProcessedFile->Key(),
                                             iBytesTransferred,
                                             iFileSizeFromHeader );
                }
            }
        }

    else
        {
        aTransaction.Close();

        // Remove remains of the file from the file system
        delete iProcessedFile;
        iProcessedFile = NULL;
        }
    }

// --------------------------------------------------------------------------
// CHttpDownloadWorker::HTTPGetL()
// (See comments in header file)
// --------------------------------------------------------------------------
//
void CHttpDownloadWorker::HttpGetL()
    {
    // if URI is null -> leave
    if ( !iProcessedFile->Uri() )
        {
        User::Leave( KErrArgument );
        }

    TUriParser8 uri;
    User::LeaveIfError( uri.Parse( *( iProcessedFile->Uri() ) ) );

    RStringF method;
    method = iStringPool.StringF( HTTP::EGET, RHTTPSession::GetTable() );

    iHttpTransaction = iSession.OpenTransactionL( uri, *this, method );
    RHTTPHeaders hdr = iHttpTransaction.Request().GetHeaderCollection();

    SetHeaderL( hdr, HTTP::EUserAgent, KModuleName );

    //Set the property of upnphttptransfer to ENotifyOnDisconnect
    //Set the property of HTTP Transaction to EEnableDisconnectNotification
    //The MHFRunL can get the really http error.
    iHttpTransaction.PropertySet().SetPropertyL(
        iSession.StringPool().StringF(
            HTTP::ENotifyOnDisconnect,RHTTPSession::GetTable()),
            iSession.StringPool().StringF(
                HTTP::EEnableDisconnectNotification,
                RHTTPSession::GetTable() ));

    // Sets extra headers
    RPointerArray<CHttpHeader> headerArray = iProcessedFile->Headers();
    for ( TInt i = 0; i < headerArray.Count(); i++ )
        {
        SetHeaderL( hdr,
                    headerArray[i]->FieldName(),
                    headerArray[i]->FieldValue() );
        }

    // Now will get events in MHFRunL
    iHttpTransaction.SubmitL();

    //set timer
    iSessionTimer->Cancel();
    iSessionTimer->AfterSeconds( KSessionTimeout );

    method.Close();

    // Change state of the state machine
    iProcessState = EHttpGetSent;
    }

// --------------------------------------------------------------------------
// CHttpDownloadWorker::FinishDownload()
// (See comments in header file)
// --------------------------------------------------------------------------
//
void CHttpDownloadWorker::FinishDownload( RHTTPTransaction aTransaction )
    {
    // close transaction
    aTransaction.Close();

    delete iResponseBody;
    iResponseBody = NULL;
    
    // change state
    iProcessState = EHttpTransactionIdle;
    iProcessedFile->CloseFile(); 
    iFile.Close();

    // inform observers
    iObserver->TransferCompleted( iProcessedFile->Key(), KErrNone );

    delete iProcessedFile;
    iProcessedFile = NULL;
    
    // inform the worker observer
    iCallback->WorkerCompleted();
    }

// --------------------------------------------------------------------------
// CHttpDownloadWorker::CompleteAndNotify()
// (See comments in header file)
// --------------------------------------------------------------------------
//
void CHttpDownloadWorker::CompleteAndNotifyL( TInt aError )
    {
    iSessionTimer->Cancel();
    iHttpTransaction.Cancel();
    iHttpTransaction.Close();
    iObserver->TransferCompleted( iProcessedFile->Key(), aError );

    delete iResponseBody;
    iResponseBody = NULL;
    iProcessState = EHttpTransactionIdle;
    iCallback->WorkerCompleted();

    iFile.Close();
    iProcessedFile->DeleteFileFromFileSystemL();
    delete iProcessedFile;
    iProcessedFile = NULL;
    }


// from base class CHttpTransferWorker

// --------------------------------------------------------------------------
// CHttpDownloadWorker::CancelTransfer()
// (See comments in header file)
// --------------------------------------------------------------------------
//
void CHttpDownloadWorker::CancelTransfer()
    {
    iSessionTimer->Cancel();
    // if transfer has not been started yet
    if ( iProcessState == EHttpWaitingForStart )
        {
        delete iProcessedFile;
        iProcessedFile = NULL;

        iProcessState = EHttpTransactionIdle;
        iCallback->WorkerCompleted();
        }
    // if process is finished do nothing
    else if ( iProcessState != EHttpTransactionIdle )
        {
        iHttpTransaction.Cancel();
        iHttpTransaction.Close();

        delete iResponseBody;
        iResponseBody = NULL;
        iProcessState = EHttpTransactionIdle;
        iCallback->WorkerCompleted();

        iFile.Close();
        TRAP_IGNORE( iProcessedFile->DeleteFileFromFileSystemL() )
        delete iProcessedFile;
        iProcessedFile = NULL;
        }
    }

// --------------------------------------------------------------------------
// CHttpDownloadWorker::StartProcessL()
// (See comments in header file)
// --------------------------------------------------------------------------
//
void CHttpDownloadWorker::StartProcessL()
    {
    CHttpTransferWorker::StartProcessL();

    iProcessedFile->CreateFileInFileSystemL();

    // File creation succeeded -> handle available
    User::LeaveIfError( iFile.Duplicate( iProcessedFile->FileHandle() ) );

    // Perform HTTP GET
    HttpGetL();
    }

// --------------------------------------------------------------------------
// CHttpDownloadWorker::MHFRunL()
// (See comments in header file)
// --------------------------------------------------------------------------
//
void CHttpDownloadWorker::MHFRunL( RHTTPTransaction aTransaction,
                                   const THTTPEvent& aEvent )
    {
    //set timer
    iSessionTimer->Cancel();

    // event handler
    switch (aEvent.iStatus)
        {
        // Response headers received
        case THTTPEvent::EGotResponseHeaders:
            {
            // accept all 'Successful' range of codes 2xx
            TInt status = aTransaction.Response().StatusCode();
            if ( !( ( status >= HTTPStatus::EOk ) &&
                    ( status <  HTTPStatus::EMultipleChoices ) ) )
                {
                // calls MHFRunError
                User::Leave( aTransaction.Response().StatusCode() );
                }

            else if ( iProcessState == EHttpGetSent )
                {
                RHTTPHeaders header =
                           aTransaction.Response().GetHeaderCollection();
                RStringF field;
                field = iStringPool.StringF( HTTP::EContentLength,
                                             RHTTPSession::GetTable() );
                THTTPHdrVal value;

                // Get value of the content-length field.
                // Value is represented in bytes.
                if ( header.GetField( field, 0, value ) == KErrNone )
                    {
                    // Set the value to filesize
                    iFileSizeFromHeader = value.Int();
                    }
                else
                    {
                    // If the Content-Length is not defined
                    // in case chunked encoding or HTTP1.0 
                    // Set the iFileSizeFromHeader to zero
                    iFileSizeFromHeader = 0;
                    }

                iBytesTransferred = 0;

                iProcessState = EHttpGetResponseReceived;
                }
            iSessionTimer->AfterSeconds( KSessionTimeout );
            break;
            }

        // Response body data received
        // This is only entered when state is EHttpGetResponseReceived
        //   --> no need to check state
        case THTTPEvent::EGotResponseBodyData:
            {
            // handle body data
            // Informs the observer as well
            HandleResponseDataL( aTransaction );

            break;
            }

        // Response completed
        case THTTPEvent::EResponseComplete:
            {
            // Get response was completed
            if ( iProcessState == EHttpGetResponseReceived )
                {
                // Write last part of the buffer to file ( if exists )
                WriteTailIntoFileL();
                }

            break;
            }

        // Transaction succeeded
        case THTTPEvent::ESucceeded:
            {
            if ( iProcessState == EHttpGetResponseReceived )
                {
                // Finish download
                FinishDownload( aTransaction );
                }

            break;
            }

        // Transaction failed
        case THTTPEvent::EFailed:
            {
            CompleteAndNotifyL( KErrGeneral );
            break;
            }

        default:
            {
            if ( aEvent.iStatus < 0 )
                {
                CompleteAndNotifyL( aEvent.iStatus );
                }
            else
                {
                CompleteAndNotifyL( KErrGeneral );
                }
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CHttpDownloadWorker::MHFRunError()
// (See comments in header file)
// --------------------------------------------------------------------------
//
TInt CHttpDownloadWorker::MHFRunError( TInt aError,
                                       RHTTPTransaction /*aTransaction*/,
                                       const THTTPEvent& /*aEvent*/ )
    {
    // if the HTTP server responds with HTTP error code the aError variable
    // contains the code which is above zero, changed to KErrGeneral
    if ( aError > 0 )
        {
        aError = KErrGeneral;
        }

    TRAP_IGNORE( CompleteAndNotifyL( aError ) )

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CHttpDownloadWorker::TimerEventL
// Disconnect connection
// -----------------------------------------------------------------------------
//
void CHttpDownloadWorker::TimerEventL( CHttpNotifyTimer* /*aTimer*/ )
    {
    CompleteAndNotifyL( KErrTimedOut );
    }
// end of file
