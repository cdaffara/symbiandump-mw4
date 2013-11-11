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
* Description:  Handles uploads
*
*/


// System include files
#include <es_sock.h>

// User include files
#include "httpuploader.h"
#include "httpuploadworker.h"
#include "httptransferbase.h"
#include "httptransferobserver.h"

// Constants
const TUint KMaxParallelTransfers = 5;

// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// CHttpUploader::CHttpDownloader()
// --------------------------------------------------------------------------
//
CHttpUploader::CHttpUploader( 
                       MHttpTransferObserver& aObserver,
                       TUint aIAPId,
                       TUint aBufferSize,
                       TUint aParallelTransfers = KDefaultParallelTransfers )
    {
    iObserver = &aObserver;
    iIAPId = aIAPId;
    iBufferSize = aBufferSize;
    
    if ( aParallelTransfers <= 0 )
        {
        iParallelTransfers = KDefaultParallelTransfers;
        }
    else if ( aParallelTransfers > KMaxParallelTransfers )
        {
        iParallelTransfers = KMaxParallelTransfers;
        }
    else
        {
        iParallelTransfers = aParallelTransfers;
        }
    }


// --------------------------------------------------------------------------
// CHttpUploader::ConstructL()
// (See comments in header file)
// --------------------------------------------------------------------------
//
void CHttpUploader::ConstructL()
    {
    
    for ( TInt i=0; i < iParallelTransfers; i++ )
        {
        iWorkerArray.Append( CHttpUploadWorker::NewL( *iObserver,
                                                      iIAPId,
                                                      iBufferSize,
                                                      *this ) );
        }
    }


// --------------------------------------------------------------------------
// CHttpUploader::NewL()
// (See comments in header file)
// --------------------------------------------------------------------------
//
EXPORT_C CHttpUploader* CHttpUploader::NewL( 
                                          MHttpTransferObserver& aObserver,
                                          TUint aIAPId,
                                          TUint aBufferSize,
                                          TUint aParallelTransfers )
    {
    CHttpUploader* self = CHttpUploader::NewLC( aObserver, 
                                                aIAPId, 
                                                aBufferSize,
                                                aParallelTransfers );
    CleanupStack::Pop( self );
    return self;
    }


// --------------------------------------------------------------------------
// CHttpDownloader::NewLC()
// (See comments in header file)
// --------------------------------------------------------------------------
//
EXPORT_C CHttpUploader* CHttpUploader::NewLC( 
                                          MHttpTransferObserver& aObserver,
                                          TUint aIAPId,
                                          TUint aBufferSize,
                                          TUint aParallelTransfers )
    {
    CHttpUploader* self = new( ELeave ) CHttpUploader( aObserver, 
                                                       aIAPId,
                                                       aBufferSize,
                                                       aParallelTransfers );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// --------------------------------------------------------------------------
// CHttpUploader::~CHttpUploader()
// (See comments in header file)
// --------------------------------------------------------------------------
//
CHttpUploader::~CHttpUploader()
    {
    }


// --------------------------------------------------------------------------
// CHttpUploader::UploadFileL
// (See comments in header file)
// --------------------------------------------------------------------------
//
EXPORT_C void CHttpUploader::UploadFileL( const TDesC8& aUri, 
                                          const TDesC& aTargetPath, 
                                          TAny* aKey )
    {
    // if aKey already exists -> leave
    for ( TInt i=0; i < iFileQueue.Count(); i++ )
        {
        if ( iFileQueue[i]->Key() == aKey )
            {
            User::Leave( KErrAlreadyExists );
            }
        }

    // Add new CHttpFile into the iFileQueue
    CreateAndQueueHttpFileL( aUri, aTargetPath, aKey );
    
    // Activate new request
    ActivateNewTransferRequest();
    }


// end of file
