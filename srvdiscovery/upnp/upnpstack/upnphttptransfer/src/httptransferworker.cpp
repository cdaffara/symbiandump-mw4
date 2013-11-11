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
* Description:  Base class for download / upload worker classes
*
*/


// System include files
#include <commdbconnpref.h>
#include <es_sock.h>
#include <stringpool.h>
#include <f32file.h>

// User include files
#include "httptransferworker.h"
#include "httptransfertimer.h"

// Constants
const TUint K30Sec = 30;

// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// CHttpTransferWorker::ConstructL()
// (See comments in header file)
// --------------------------------------------------------------------------
//
void CHttpTransferWorker::ConstructL()
    {
    // Open HTTP session
    iSession.OpenL();

    // Store the string pool for this HTTP session
    iStringPool = iSession.StringPool();

    // Connect to file server
    User::LeaveIfError( iFsSession.Connect() );
    }

// --------------------------------------------------------------------------
// CHttpTransferWorker::~CHttpTransferBase()
// (See comments in header file)
// --------------------------------------------------------------------------
//
CHttpTransferWorker::~CHttpTransferWorker()
    {

    if ( iProcessState != EHttpTransactionIdle )
        {
        iHttpTransaction.Close();
        iFile.Close();
        }

    // Close HTTP session
    if( &iSession )
        {
        iSession.Close();
        }

    // Closes the connection
    delete iConnectionManagerProxy;

    // Close file server session
    iFsSession.Close();

    // Close connection to the socket server
    iSocketServer.Close();

    
    delete iProcessedFile;
    iProcessedFile = NULL;

    delete iTimer;
    }

// --------------------------------------------------------------------------
// CHttpTransferWorker::ConnectL()
// (See comments in header file)
// --------------------------------------------------------------------------
//
void CHttpTransferWorker::ConnectL()
    {
    User::LeaveIfError( iSocketServer.Connect() );

    iConnectionManagerProxy = CUpnpConnectionManagerProxy::NewL( iSocketServer );
    User::LeaveIfError( iConnectionManagerProxy->EnsureStart() );

    RHTTPConnectionInfo connInfo = iSession.ConnectionInfo();

    //Attach to socket server
    connInfo.SetPropertyL( iStringPool.StringF(HTTP::EHttpSocketServ,
                            RHTTPSession::GetTable() ),
                            THTTPHdrVal(iSocketServer.Handle() ) );

    //Attach to connection
    TInt connPtr = reinterpret_cast<TInt>( &iConnectionManagerProxy->ConnectionL() );
    connInfo.SetPropertyL( iStringPool.StringF(
                            HTTP::EHttpSocketConnection,
                            RHTTPSession::GetTable() ),
                            THTTPHdrVal(connPtr) );
    }

// --------------------------------------------------------------------------
// CHttpTransferWorker::SetFile()
// (See comments in header file)
// --------------------------------------------------------------------------
//
void CHttpTransferWorker::SetFileL( CHttpFile& aFile )
    {
    // if iProcessedFile is not deleted yet for some reason
    //  --> delete it
    delete iProcessedFile;

    iProcessedFile = &aFile;

    iProcessState = EHttpWaitingForStart;

    // Create and start the timer
    delete iTimer;
    iTimer = NULL;
    iTimer = CHttpTransferTimer::NewL( K30Sec, this );
    }

// --------------------------------------------------------------------------
// CHttpTransferWorker::StartProcessL()
// (See comments in header file)
// --------------------------------------------------------------------------
//
void CHttpTransferWorker::StartProcessL()
    {
    delete iTimer;
    iTimer = NULL;
    }


// --------------------------------------------------------------------------
// CHttpTransferWorker::TrackProgress()
// (See comments in header file)
// --------------------------------------------------------------------------
//
void CHttpTransferWorker::TrackProgress( TBool aValue )
    {
    if ( iProcessedFile )
        {
        iProcessedFile->TrackProgress( aValue );
        }
    }

// --------------------------------------------------------------------------
// CHttpTransferWorker::Key()
// (See comments in header file)
// --------------------------------------------------------------------------
//
TAny* CHttpTransferWorker::Key() const
    {
    TAny* retval = NULL;

    if ( iProcessedFile )
        {
        retval = iProcessedFile->Key();
        }

    return retval;
    }

// --------------------------------------------------------------------------
// CHttpTransferWorker::Uri()
// (See comments in header file)
// --------------------------------------------------------------------------
//
const HBufC8* CHttpTransferWorker::Uri() const
    {
    const HBufC8* retval = NULL;

    if ( iProcessedFile )
        {
        retval = iProcessedFile->Uri();
        }

    return retval;
    }

// --------------------------------------------------------------------------
// CHttpTransferWorker::Path()
// (See comments in header file)
// --------------------------------------------------------------------------
//
const HBufC* CHttpTransferWorker::Path() const
    {
    const HBufC* retval = NULL;

    if ( iProcessedFile )
        {
        retval = iProcessedFile->Path();
        }

    return retval;
    }

// --------------------------------------------------------------------------
// CHttpTransferWorker::ProcessOnGoing()
// (See comments in header file)
// --------------------------------------------------------------------------
//
TBool CHttpTransferWorker::ProcessOnGoing() const
    {
    TBool retval = EFalse;

    if ( iProcessState != EHttpTransactionIdle )
        {
        retval = ETrue;
        }

    return retval;
    }

// --------------------------------------------------------------------------
// CHttpDownloadWorker::IsWaiting()
// (See comments in header file)
// --------------------------------------------------------------------------
//
TBool CHttpTransferWorker::IsWaiting() const
    {
    TBool retval = EFalse;

    // if its not waiting for start -> true
    if ( iProcessState == EHttpWaitingForStart )
        {
        retval = ETrue;
        }

    return retval;
    }

// --------------------------------------------------------------------------
// CHttpDownloadWorker::SetHeaderL()
// (See comments in header file)
// --------------------------------------------------------------------------
//
void CHttpTransferWorker::SetHeaderL( RHTTPHeaders aHeaders,
                                      TInt aHdrField,
                                      const TDesC8& aHdrValue )
    {
    RStringF valStr = iStringPool.OpenFStringL( aHdrValue );
    CleanupClosePushL( valStr );
    THTTPHdrVal val( valStr );
    aHeaders.SetFieldL( iStringPool.StringF( aHdrField,
                        RHTTPSession::GetTable() ), val );
    CleanupStack::PopAndDestroy( &valStr );
    }

// --------------------------------------------------------------------------
// CHttpDownloadWorker::SetHeaderL()
// (See comments in header file)
// --------------------------------------------------------------------------
//
void CHttpTransferWorker::SetHeaderL( RHTTPHeaders aHeaders,
                                      const TDesC8& aHdrField,
                                      const TDesC8& aHdrValue )
    {
    RStringF valStr = iStringPool.OpenFStringL( aHdrValue );
    CleanupClosePushL( valStr );
    RStringF fieldStr = iStringPool.OpenFStringL( aHdrField );
    CleanupClosePushL( fieldStr );
    THTTPHdrVal val( valStr );
    aHeaders.SetFieldL( fieldStr, val );
    CleanupStack::PopAndDestroy( &fieldStr );
    CleanupStack::PopAndDestroy( &valStr );
    }

// --------------------------------------------------------------------------
// CHttpDownloadWorker::ProcessedFile()
// (See comments in header file)
// --------------------------------------------------------------------------
//
CHttpFile* CHttpTransferWorker::ProcessedFile()
    {
    return iProcessedFile;
    }


// From base class MHttpTransferTimerObserver

// --------------------------------------------------------------------------
// CHttpTransferBase::TimerCallback()
// (See comments in header file)
// --------------------------------------------------------------------------
//
void CHttpTransferWorker::TimerCallback()
    {
    // if this method is called the waiting time has exceeded the time limit
    CancelTransfer();
    delete iTimer;
    iTimer = NULL;
    }

// end of file
