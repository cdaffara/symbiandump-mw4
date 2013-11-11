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
* Description:  Handles downloads
*
*/


// System include files
#include <es_sock.h>

// User include files
#include "httpdownloader.h"
#include "httpdownloadworker.h"
#include "httptransferbase.h"
#include "httptransferobserver.h"
#include "httpworkerobserver.h"

// Constants
const TUint KMaxParallelTransfers = 5;

// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// CHttpDownloader::CHttpDownloader()
// --------------------------------------------------------------------------
//
CHttpDownloader::CHttpDownloader( 
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
// CHttpDownloader::ConstructL()
// (See comments in header file)
// --------------------------------------------------------------------------
//
void CHttpDownloader::ConstructL()
    {
    
    for ( TInt i=0; i < iParallelTransfers; i++ )
        {
        iWorkerArray.Append( CHttpDownloadWorker::NewL( *iObserver,
                                                        iIAPId,
                                                        iBufferSize,
                                                        *this ) );
        }

    }

// --------------------------------------------------------------------------
// CHttpDownloader::NewL()
// (See comments in header file)
// --------------------------------------------------------------------------
//
EXPORT_C CHttpDownloader* CHttpDownloader::NewL( 
                                        MHttpTransferObserver& aObserver,
                                        TUint aIAPId,
                                        TUint aBufferSize,
                                        TUint aParallelTransfers )
    {
    CHttpDownloader* self = CHttpDownloader::NewLC( aObserver, 
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
EXPORT_C CHttpDownloader* CHttpDownloader::NewLC( 
                                         MHttpTransferObserver& aObserver,
                                         TUint aIAPId,
                                         TUint aBufferSize,
                                         TUint aParallelTransfers )

    {
    CHttpDownloader* self = 
                new( ELeave ) CHttpDownloader( aObserver, 
                                               aIAPId,
                                               aBufferSize,
                                               aParallelTransfers );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// --------------------------------------------------------------------------
// CHttpDownloader::~CHttpDownloader()
// (See comments in header file)
// --------------------------------------------------------------------------
//
CHttpDownloader::~CHttpDownloader()
    {
    }

// --------------------------------------------------------------------------
// CHttpDownloader::DownloadFileL
// (See comments in header file)
// --------------------------------------------------------------------------
//
EXPORT_C void CHttpDownloader::DownloadFileL( const TDesC8& aUri, 
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

// --------------------------------------------------------------------------
// CHttpDownloader::DownloadFileL
// (See comments in header file)
// --------------------------------------------------------------------------
//
EXPORT_C void CHttpDownloader::DownloadFileL( const TDesC8& aUri, 
                                              const RFile& aFileHandle, 
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
    CreateAndQueueHttpFileL( aUri, aFileHandle, aKey );

    // Activate new request
    ActivateNewTransferRequest();
    }

// end of file
