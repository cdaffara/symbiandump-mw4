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
* Description:  Base class for downloader / uploader classes
*
*/


// System include files
#include <escapeutils.h>

// User include files
#include "httptransferbase.h"
#include "httptransferobserver.h"
#include "httpfile.h"
#include "httptransferworker.h"
#include "httpworkerobserver.h"

// Definitions
enum TTransferStates 
    {
    ETransferNextFile
    };

// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// CHttpTransferBase::CHttpTransferBase()
// (See comments in header file)
// --------------------------------------------------------------------------
//
CHttpTransferBase::CHttpTransferBase() : CActive( EPriorityStandard ) 
    {
    CActiveScheduler::Add( this );
    }

// --------------------------------------------------------------------------
// CHttpTransferBase::~CHttpTransferBase()
// (See comments in header file)
// --------------------------------------------------------------------------
//
CHttpTransferBase::~CHttpTransferBase()
    {
    if( IsActive() )
        {
        CActive::Cancel();
        }

    // Reset array and destroy all the items in it
    iFileQueue.ResetAndDestroy();
    iWorkerArray.ResetAndDestroy();
    iWaitQueue.ResetAndDestroy();
    }

// --------------------------------------------------------------------------
// CHttpDownloader::CancelAll
// (See comments in header file)
// --------------------------------------------------------------------------
//
EXPORT_C void CHttpTransferBase::CancelAll()
    {
    // Reset arrays and destroy all the items in them
    iFileQueue.ResetAndDestroy();
    iWaitQueue.ResetAndDestroy();

    for( TInt i=0; i < iWorkerArray.Count(); i++ )
        {
        if ( iWorkerArray[i]->ProcessOnGoing() )
            {
            iWorkerArray[i]->CancelTransfer();
            }
        }
    }

// --------------------------------------------------------------------------
// CHttpTransferBase::Cancel
// (See comments in header file)
// --------------------------------------------------------------------------
//
EXPORT_C TInt CHttpTransferBase::CancelTransfer( TAny* aKey )
    {
    TInt retval = KErrNotFound;

    // Find the item corresponding aKey and delete it and
    // remove from the wait queue
    for ( TInt i = 0; i < iWaitQueue.Count(); i++ )
        {
        if ( iWaitQueue[i]->Key() == aKey )
            {
            retval = KErrNone;
            delete iWaitQueue[i];
            iWaitQueue.Remove(i);
            iWaitQueue.Compress();
            }
        }

    // Find the item corresponding aKey and delete it and
    // remove from the file queue
    for ( TInt i = 0; i < iFileQueue.Count(); i++ )
        {
        if ( iFileQueue[i]->Key() == aKey )
            {
            retval = KErrNone;
            delete iFileQueue[i];
            iFileQueue.Remove(i);
            iFileQueue.Compress();
            }
        }

    // Check if some of the ongoing processes has to be canceled
    for ( TInt i = 0; i < iWorkerArray.Count(); i++ )
        {
        if ( iWorkerArray[i]->ProcessOnGoing() )
            {
            if ( iWorkerArray[i]->Key() == aKey )
                {
                retval = KErrNone;
                iWorkerArray[i]->CancelTransfer();
                }
            }
        }

    return retval;
    }

// --------------------------------------------------------------------------
// CHttpTransferBase::TrackProgress
// (See comments in header file)
// --------------------------------------------------------------------------
//
EXPORT_C TInt CHttpTransferBase::TrackProgress( TAny* aKey, TBool aValue )
    {
    TInt retval = KErrNotFound;

    // Find the corresponding file from iWaitQueue
    for ( TInt i = 0; i < iWaitQueue.Count(); i++ )
        {
        if ( iWaitQueue[i]->Key() == aKey )
            {
            retval = KErrNone;
            iWaitQueue[i]->TrackProgress( aValue );
            }
        }

    // Find the corresponding instance of CHttpFile from iFileQueue and
    // switch the tracking of the progress on
    for ( TInt i = 0; i < iFileQueue.Count(); i++)
        {
        if ( iFileQueue[i]->Key() == aKey )
            {
            retval = KErrNone;
            iFileQueue[i]->TrackProgress( aValue );
            }
        }

    // Check if processed files have to be tracked
    for ( TInt i = 0; i < iWorkerArray.Count(); i++ )
        {
        if ( iWorkerArray[i]->ProcessOnGoing() && 
             iWorkerArray[i]->Key() == aKey ) 
            {
            retval = KErrNone;
            iWorkerArray[i]->TrackProgress( aValue );    
            }
        }

    return retval;
    }

// --------------------------------------------------------------------------
// CHttpTransferBase::SetHeaderL
// (See comments in header file)
// --------------------------------------------------------------------------
//    
EXPORT_C void CHttpTransferBase::SetHeaderL( TAny* aKey,
                                             const TDesC8& aFieldName, 
                                             const TDesC8& aFieldValue )
    {
    TBool keyExists = EFalse;

    // Go through iWaitQueue and if there is file mathich the key set headers
    for ( TInt i = 0; i < iWaitQueue.Count(); i++ )
        {
        if ( iWaitQueue[i]->Key() == aKey )
            {
            keyExists = ETrue;
            iWaitQueue[i]->SetHeaderL( aFieldName, aFieldValue );
            }
        }

    // Go through workers and set the headers to the downloaded file which
    // has matching key
    for ( TInt i = 0; i < iWorkerArray.Count(); i++ )
        {
        if ( iWorkerArray[i]->Key() == aKey )
            {
            // if the worker is not waiting -> leave
            if ( !iWorkerArray[i]->IsWaiting() )
                {
                User::Leave( KErrGeneral );
                }

            ( iWorkerArray[i]->ProcessedFile() )->
                           SetHeaderL( aFieldName, aFieldValue );
            keyExists = ETrue;
            }
        }

    if ( !keyExists )
        {
        User::Leave( KErrNotFound );
        }
    }

// --------------------------------------------------------------------------
// CHttpTransferBase::CreateAndQueueHttpFile()
// (See comments in header file)
// --------------------------------------------------------------------------
//
void CHttpTransferBase::CreateAndQueueHttpFileL( const TDesC8& aUri,
                                                 const TDesC& aTargetPath,
                                                 TAny* aKey )
    {
    CHttpFile* httpfile = CHttpFile::NewL( aKey, aUri, aTargetPath );
    iFileQueue.Append( httpfile );
    }

// --------------------------------------------------------------------------
// CHttpTransferBase::CreateAndQueueHttpFile()
// (See comments in header file)
// --------------------------------------------------------------------------
//
void CHttpTransferBase::CreateAndQueueHttpFileL( const TDesC8& aUri,
                                                 const RFile& aFileHandle,
                                                 TAny* aKey )
    {
    CHttpFile* httpfile = CHttpFile::NewL( aKey, aUri, aFileHandle );
    iFileQueue.Append( httpfile );
    }

// --------------------------------------------------------------------------
// CHttpTransferBase::NextFile
// (See comments in header file)
// --------------------------------------------------------------------------
//
CHttpFile* CHttpTransferBase::NextFile()
    {
    CHttpFile* newFile = NULL;

    // If iFileQueue has files
    if ( iFileQueue.Count() > 0 )
        {
        // Set the new processed file
        newFile = iFileQueue[0];

        // Set the queue
        iFileQueue.Remove(0);
        iFileQueue.Compress();
        }

    return newFile;
    }

// --------------------------------------------------------------------------
// CHttpTransferBase::StartTransferL()
// (See comments in header file)
// --------------------------------------------------------------------------
//
EXPORT_C void CHttpTransferBase::StartTransferL( TAny* aKey )
    {
    TBool keyExists = EFalse;

    // go through workerarray and if there is worker which has 
    // corresponding key -> start the transfer
    for(TInt i=0; i < iWorkerArray.Count(); i++ )
        {
        if ( iWorkerArray[i]->Key() == aKey )
            {
            iWorkerArray[i]->StartProcessL();
            keyExists = ETrue;
            }
        }

    // Leave if the key does not exist
    if ( !keyExists )
        {
        User::Leave( KErrNotFound );
        }
    }

// --------------------------------------------------------------------------
// CHttpTransferBase::SetProperty()
// (See comments in header file)
// --------------------------------------------------------------------------
//
EXPORT_C void CHttpTransferBase::SetPropertyL( TAny* aKey,
                                               THttpPropertyType aProperty, 
                                               const TDesC& aValue )
    {

    switch ( aProperty )
        {
        // This case is used to set targetUri. This is used in upnp upload
        case ETargetURI:
            {
            TBool keyExists = EFalse;
            HBufC8* uri = EscapeUtils::ConvertFromUnicodeToUtf8L( aValue );
            CleanupStack::PushL( uri );

            for ( TInt i=0; i < iWorkerArray.Count(); i++ )
                {
                if ( iWorkerArray[i]->Key() == aKey )
                    {
                    ( iWorkerArray[i]->ProcessedFile() )->
                                                    SetUriL( uri->Des() );
                    keyExists = ETrue;
                    }
                }

            CleanupStack::PopAndDestroy( uri );

            // the key was not found
            if ( !keyExists )
                {
                User::Leave( KErrNotFound );
                }

            break;
            }

        default:
            {
            
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CHttpTransferBase::GetPropertyL()
// (See comments in header file)
// --------------------------------------------------------------------------
//
EXPORT_C const HBufC* CHttpTransferBase::GetPropertyL( 
                                                TAny* aKey,
                                                THttpPropertyType aProperty )
    {
    const HBufC* retval = NULL;

    switch ( aProperty )
        {
        // Client can receive the path of the transferred file using this
        case ETargetPath:
            {
            TBool keyExists = EFalse;

            // Goes through workerarray and returns the path of the file
            // which matches the key
            for ( TInt i = 0; i < iWorkerArray.Count(); i++ )
                {
                if ( iWorkerArray[i]->Key() == aKey )
                    {
                    retval = iWorkerArray[i]->Path();
                    keyExists = ETrue;
                    }
                }

            // the key was not found
            if ( !keyExists )
                {
                User::Leave( KErrNotFound );
                }

            break;
            }

        default:
            {
            
            break;
            }
        }

    return retval;
    }

// --------------------------------------------------------------------------
// CHttpTransferBase::InsertFileIntoWaitQueueL()
// (See comments in header file)
// --------------------------------------------------------------------------
//
EXPORT_C void CHttpTransferBase::InsertFileIntoWaitQueueL(
                                                    TAny* aKey,
                                                    const TDesC& aTargetPath,
                                                    const TDesC8& aUri )
    {
    // Checks first if the key already exists in iWaitQueue or in iFileQueue
    // if does -> leave
    for ( TInt i = 0; i < iWaitQueue.Count(); i++ )
        {
        if ( iWaitQueue[i]->Key() == aKey )
            {
            User::Leave( KErrAlreadyExists );
            }
        }

    for ( TInt i = 0; i < iFileQueue.Count(); i++ )
        {
        if ( iFileQueue[i]->Key() == aKey )
            {
            User::Leave( KErrAlreadyExists );
            }
        }

    // if key is valid create CHttpFile and insert it to iWaitQueue
    CHttpFile* httpfile = CHttpFile::NewL( aKey, aUri, aTargetPath );
    iWaitQueue.Append( httpfile );
    }

// --------------------------------------------------------------------------
// CHttpTransferBase::InsertFileIntoWaitQueueL()
// (See comments in header file)
// --------------------------------------------------------------------------
//
EXPORT_C void CHttpTransferBase::InsertFileIntoWaitQueueL(
                                                    TAny* aKey,
                                                    const RFile& aFileHandle,
                                                    const TDesC8& aUri )
    {
    // Checks first if the key already exists in iWaitQueue or in iFileQueue
    // if does -> leave
    for ( TInt i = 0; i < iWaitQueue.Count(); i++ )
        {
        if ( iWaitQueue[i]->Key() == aKey )
            {
            User::Leave( KErrAlreadyExists );
            }
        }

    for ( TInt i = 0; i < iFileQueue.Count(); i++ )
        {
        if ( iFileQueue[i]->Key() == aKey )
            {
            User::Leave( KErrAlreadyExists );
            }
        }

    // if key is valid create CHttpFile and insert it to iWaitQueue
    CHttpFile* httpfile = CHttpFile::NewL( aKey, aUri, aFileHandle );
    iWaitQueue.Append( httpfile );
    }

// --------------------------------------------------------------------------
// CHttpTransferBase::MoveToFileQueue()
// (See comments in header file)
// --------------------------------------------------------------------------
//
EXPORT_C void CHttpTransferBase::MoveToTransferQueueL( TAny* aKey )
    {
    TBool keyExists = EFalse;
    CHttpFile* waitFile = NULL;

    for ( TInt i = 0; i < iWaitQueue.Count(); i++ )
        {
        if ( iWaitQueue[i]->Key() == aKey )
            {
            keyExists = ETrue;

            // Removes the file from iWaitQueue
            waitFile = iWaitQueue[i];

            // Set the queue
            iWaitQueue.Remove(i);
            iWaitQueue.Compress();
            }
        }

    // If the key does not exist -> leaves
    if ( !keyExists )
        {
        User::Leave( KErrNotFound );
        }

    // Appends the file to iFileQueue
    iFileQueue.Append( waitFile );

    // start the transfer
    ActivateNewTransferRequest();
    }

// --------------------------------------------------------------------------
// CHttpTransferBase::ActivateNewTransferRequest()
// (See comments in header file)
// --------------------------------------------------------------------------
//
void CHttpTransferBase::ActivateNewTransferRequest()
    {
    if ( !IsActive() )
        {
        SetActive();
        TRequestStatus* pStatus = &iStatus;
        User::RequestComplete( pStatus, ETransferNextFile );
        }
    }

// --------------------------------------------------------------------------
// CHttpTransferBase::ProcessNextFileL()
// (See comments in header file)
// --------------------------------------------------------------------------
//
void CHttpTransferBase::ProcessNextFileL()
    {
    // Check that there is workers available
    for ( TInt i = 0; i < iWorkerArray.Count(); i++ )
        {
        if ( !iWorkerArray[i]->ProcessOnGoing() )
            {
            CHttpFile* nextFile = NULL;

            nextFile = NextFile();

            if ( nextFile )
                {
                // sets the file and sets the state to EWaitingForStart
                iWorkerArray[i]->SetFileL( *nextFile );

                // Set worker which is handled to pointer so that 
                // if it leaves the worker can be terminated
                iActiveWorker = iWorkerArray[i];

                iObserver->ReadyForTransferL( nextFile->Key() );

                iActiveWorker = NULL;
                }
            }
        }
    }


// From base class CActive

// --------------------------------------------------------------------------
// CHttpTransferBase::DoCancel()
// (See comments in header file)
// --------------------------------------------------------------------------
//
void CHttpTransferBase::DoCancel()
    {
    CancelAll();
    }

// --------------------------------------------------------------------------
// CHttpTransferBase::RunL()
// (See comments in header file)
// --------------------------------------------------------------------------
//
void CHttpTransferBase::RunL()
    {
    ProcessNextFileL();
    }

// --------------------------------------------------------------------------
// CHttpTransferBase::RunError()
// (See comments in header file)
// --------------------------------------------------------------------------
//
TInt CHttpTransferBase::RunError( TInt aError )
    {
    // Terminate worker which leaved
    if ( iActiveWorker )
        {
        // Cancels the transfer and 
        // informs the client through TransferCompleted()
        iObserver->TransferCompleted( iActiveWorker->Key(), aError );
        iActiveWorker->CancelTransfer();
        iActiveWorker = NULL;
        }

    return KErrNone;
    }


// From base class MHttpWorkerObserver

// --------------------------------------------------------------------------
// CHttpTransferBase::ProcessNextFileL()
// (See comments in header file)
// --------------------------------------------------------------------------
//
void CHttpTransferBase::WorkerCompleted()
    {
    ActivateNewTransferRequest();
    }

// end of file
