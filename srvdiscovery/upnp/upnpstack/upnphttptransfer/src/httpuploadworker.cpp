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
* Description:  Handles upload of one file
*
*/


// User include files
#include "httpuploadworker.h"

// Constants
_LIT8( KModuleName,         "HttpTransfer" );
_LIT8( KAllowedData,        "*/*"          );
_LIT8( KContentTypeName,    "Content-Type" );
_LIT8( KDefaultContentType, "text/plain"   );
_LIT8( KExpect,             "Expect" );
_LIT8( K100Continue,        "100-continue" );

const TUint KDefaultBufferSize = 64*KKilo;
const TUint KMaxBufferSize     = 128*KKilo;

// Time to wait for response after sending 100 continue post request.
// Value in microseconds.
const TInt K100ContinueResponseWaitTime = 3000000; // 3 seconds

// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// CHttpUploadWorker::CHttpUploadWorker()
// (See comments in header file)
// --------------------------------------------------------------------------
//
CHttpUploadWorker::CHttpUploadWorker( MHttpTransferObserver& aObserver,
                                      TUint /*aIAPId*/,
                                      TUint aBufferSize,
                                      MHttpWorkerObserver& aCallback )
    {
    iObserver = &aObserver;
    iCallback = &aCallback;

    iProcessState = EHttpTransactionIdle;

    // Check that size of the buffer is between boundaries
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

    iSendDataCount = 0;
    iBodyFileOffset = 0;
    iHttpStatus = 0;
    iMoreToCome = EFalse;
    iAlreadyDone = EFalse;
    }

// --------------------------------------------------------------------------
// CHttpUploadWorker::ConstructL()
// (See comments in header file)
// --------------------------------------------------------------------------
//
void CHttpUploadWorker::ConstructL()
    {
    CHttpTransferWorker::ConstructL();
    ConnectL();
    iSessionTimer = CHttpNotifyTimer::NewL( this );
    }

// --------------------------------------------------------------------------
// CHttpUploadWorker::NewL()
// (See comments in header file)
// --------------------------------------------------------------------------
//
CHttpUploadWorker* CHttpUploadWorker::NewL(
                                          MHttpTransferObserver& aObserver,
                                          TUint aIAPId,
                                          TUint aBufferSize,
                                          MHttpWorkerObserver& aCallback )
    {
    CHttpUploadWorker* self = CHttpUploadWorker::NewLC( aObserver,
                                                        aIAPId,
                                                        aBufferSize,
                                                        aCallback );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CHttpUploadWorker::NewLC()
// (See comments in header file)
// --------------------------------------------------------------------------
//
CHttpUploadWorker* CHttpUploadWorker::NewLC(
                                          MHttpTransferObserver& aObserver,
                                          TUint aIAPId,
                                          TUint aBufferSize,
                                          MHttpWorkerObserver& aCallback )
    {
    CHttpUploadWorker* self = new( ELeave ) CHttpUploadWorker( aObserver,
                                                               aIAPId,
                                                               aBufferSize,
                                                               aCallback );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// --------------------------------------------------------------------------
// CHttpUploadWorker::~CHttpUploadWorker()
// (See comments in header file)
// --------------------------------------------------------------------------
//
CHttpUploadWorker::~CHttpUploadWorker()
    {
    delete iRequestBodyBuffer;
    delete iSessionTimer;
    
    // Destroy the 100 continue timer if it exists.
    Destroy100ContinueTimer();
    }

// --------------------------------------------------------------------------
// CHttpUploadWorker::CancelTransfer()
// (See comments in header file)
// --------------------------------------------------------------------------
//
void CHttpUploadWorker::CancelTransfer()
    {
    iSessionTimer->Cancel();

    // Destroy the 100 continue response wait timer if it exists.
    Destroy100ContinueTimer();
    
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

        delete iRequestBodyBuffer;
        iRequestBodyBuffer = NULL;

        iProcessState = EHttpTransactionIdle;
        iHttpTransaction.Close();

        // call reset so that this worker can be used for the next download
        Reset(); 

        delete iProcessedFile;
        iProcessedFile = NULL;

        iOverallDataSize = 0;
        iFile.Close();

        // inform the worker observer
        iCallback->WorkerCompleted();
        }
    }

// --------------------------------------------------------------------------
// CHttpUploadWorker::MapPostTransactionError()
// (See comments in header file)
// --------------------------------------------------------------------------
//
TInt CHttpUploadWorker::MapPostTransactionError( TInt aError ) const
    {
    TInt error = KErrNone;
    switch (aError)
        {
        case HTTPStatus::EForbidden:
            {
            error = KErrAccessDenied;
            break;
            }

        case HTTPStatus::EMethodNotAllowed:
            {
            error = KErrAccessDenied;
            break;
            }

        case HTTPStatus::EConflict:
            {
            // resource already exists .
            error = KErrAlreadyExists;
            break;
            }

        case HTTPStatus::EUnsupportedMediaType:
            {
            error = KErrNotSupported;
            break;
            }

        case HTTPStatus::EInternalServerError:
            {
            error = KErrNotSupported;
            break;
            }

        case HTTPStatus::EMovedPermanently:
            {
            // fall through
            }

        case HTTPStatus::ETemporaryRedirect:
            {
            // It is not allowed to write a file with the same name
            // as an existing directory.
            error = KErrGeneral;    
            break;
            }

        default:
            {
            if ( HTTPStatus::IsServerError( aError ) )
                {
                error = KErrNotSupported;
                }
            else if ( HTTPStatus::IsClientError( aError ) )
                {
                error = KErrAccessDenied;
                }
            else if ( HTTPStatus::IsRedirection( aError ) )
                {
                error = KErrNotFound;
                }
            else
                {
                if ( aError > 0 )
                    {
                    // An arbitrary choice for error codes that 
                    // should not occur
                    error = KErrAccessDenied;
                    }
                }
            break;
            }
        }

    return error;
    }

// --------------------------------------------------------------------------
// CHttpUploadWorker::CompleteAndNotify()
// (See comments in header file)
// --------------------------------------------------------------------------
//
void CHttpUploadWorker::CompleteAndNotify( TInt aError )
    {
    iSessionTimer->Cancel();
        
    iProcessState = EHttpTransactionIdle;
    iHttpTransaction.Cancel();
    iHttpTransaction.Close();

    delete iRequestBodyBuffer;
    iRequestBodyBuffer = NULL;

    // call reset so that this worker can be used for the next download
    Reset(); 
    if ( iProcessedFile )
        {
        iObserver->TransferCompleted( iProcessedFile->Key(), aError );
        }

    delete iProcessedFile;
    iProcessedFile = NULL;

    iOverallDataSize = 0;
    iFile.Close();

    // inform the worker observer
    iCallback->WorkerCompleted();
    }

// --------------------------------------------------------------------------
// CHttpUploadWorker::StartProcessL()
// (See comments in header file)
// --------------------------------------------------------------------------
//
void CHttpUploadWorker::StartProcessL()
    {
    CHttpTransferWorker::StartProcessL();
    
    SendL();
    }

// --------------------------------------------------------------------------
// CHttpUploadWorker::SendL()
// (See comments in header file)
// --------------------------------------------------------------------------
//
void CHttpUploadWorker::SendL()
    {
    // iAlreadyDone need to be reset
    iSendDataCount = 0;
    iBodyFileOffset = 0;
    iHttpStatus = 0;
    iMoreToCome = EFalse;
    iAlreadyDone = EFalse;
    // Open the file in the file system
    User::LeaveIfError( iFile.Open( iFsSession,
                                    *(iProcessedFile->Path()),
                                    EFileShareReadersOnly | EFileRead ) );

    User::LeaveIfError( iFile.Size( iOverallDataSize ) );

    // get read buffer size from the Central Repository
    iRequestBodyBuffer = HBufC8::NewL( iBufferSize );

    InvokeHttpMethodL();

    // Change state of the state machine
    iProcessState = EHttpPostSent;
    }

// --------------------------------------------------------------------------
// CHttpUploadWorker::InvokeHttpMethodL()
// (See comments in header file)
// --------------------------------------------------------------------------
//
void CHttpUploadWorker::InvokeHttpMethodL()
    {
    // This actually creates the transaction, sets the headers and body 
    // and then starts the transaction.
    TBool contentTypeExists = EFalse;

    // open HTTP transaction
    TUriParser8 uri; 
    User::LeaveIfError( uri.Parse( *( iProcessedFile->Uri() ) ) );

    // POST HTTP method
    RStringF method = iStringPool.StringF( HTTP::EPOST,
                                           RHTTPSession::GetTable() );
    CleanupClosePushL( method );

    iHttpTransaction = iSession.OpenTransactionL( uri, *this, method );

    RHTTPHeaders hdr = iHttpTransaction.Request().GetHeaderCollection();

    SetHeaderL( hdr, HTTP::EUserAgent, KModuleName );
    SetHeaderL( hdr, HTTP::EAccept, KAllowedData );

    //Set the property of upnphttptransfer to ENotifyOnDisconnect
    //Set the property of HTTP Transaction to EEnableDisconnectNotification
    //The MHFRunL can get the really http error.
    iHttpTransaction.PropertySet().SetPropertyL(
        iSession.StringPool().StringF( 
            HTTP::ENotifyOnDisconnect,RHTTPSession::GetTable() ),
            iSession.StringPool().StringF( 
                HTTP::EEnableDisconnectNotification,
                RHTTPSession::GetTable() ) );
    // Add arbitrary Http headers
    RPointerArray<CHttpHeader> headerArray = iProcessedFile->Headers();
    TBool expect100ContinueFound = EFalse;
    for ( TInt i = 0; i < headerArray.Count(); i++ )
        {
        const TDesC8& headerName = headerArray[i]->FieldName();
        const TDesC8& headerValue = headerArray[i]->FieldValue();
        
        SetHeaderL( hdr,
                    headerName,
                    headerValue );
        if ( headerName == KContentTypeName() )
            {
            contentTypeExists = ETrue;
            }
        if ( headerName == KExpect() )
            {
            if ( headerValue == K100Continue() ) 
                {
                // Wait for 100 continue before sending the body in case of 
                // "Expect: 100-Continue" header.
                RHTTPTransactionPropertySet propSet = 
                    iHttpTransaction.PropertySet();
                RStringF notifyContinue = 
                    iStringPool.StringF( 
                        HTTP::ENotify100Continue, 
                        RHTTPSession::GetTable() );
                RStringF enableNotification = 
                    iStringPool.StringF( 
                        HTTP::EEnableNotification, 
                        RHTTPSession::GetTable() );
                THTTPHdrVal val( enableNotification );
                propSet.SetPropertyL( notifyContinue, val );
                      
                // we found "Expect: 100-continue"-header
                expect100ContinueFound = ETrue;
                
                // Need to start a timer when sending the headers that waits 
                // for some time and if no response during that time, sends 
                // this event to continue with the body.
                i100ContinueTimer = CPeriodic::NewL( CActive::EPriorityStandard );
                i100ContinueTimer->Start(
                    K100ContinueResponseWaitTime, // time to cancel waiting
                    K100ContinueResponseWaitTime, // this is not actually used
                    TCallBack( Cancel100ContinueWaitL, this ) );
                }
            }
        }

    if ( contentTypeExists == EFalse )
        {
        SetHeaderL( hdr, HTTP::EContentType, KDefaultContentType );
        }
        
    if ( !expect100ContinueFound ) 
        {
        // "Expect: 100-continue" header not found. Do not wait for 
        // 100-continue. This might not be needed but it is added here to 
        // avoid possible regression by not changing the previous behaviour
        // too much. 
        RHTTPTransactionPropertySet propSet = iHttpTransaction.PropertySet();
        RStringF notifyContinue = 
            iStringPool.StringF( 
                HTTP::ENotify100Continue, 
                RHTTPSession::GetTable() );
        RStringF disableNotification = 
            iStringPool.StringF( 
                HTTP::EDisableNotification, 
                RHTTPSession::GetTable() );
        THTTPHdrVal val( disableNotification );
        propSet.SetPropertyL( notifyContinue, val );
        }

    // Set Body data
    MHTTPDataSupplier* dataSupplier = this;
    iHttpTransaction.Request().SetBody( *dataSupplier );

    // submit the request
    iHttpTransaction.SubmitL();

    //set timer
    iSessionTimer->Cancel();
    iSessionTimer->AfterSeconds( KSessionTimeout );
    CleanupStack::PopAndDestroy( &method );
    }

// --------------------------------------------------------------------------
// CHttpUploadWorker::MHFRunL()
// (See comments in header file)
// --------------------------------------------------------------------------
void CHttpUploadWorker::MHFRunL( RHTTPTransaction aTransaction,
                                 const THTTPEvent& aEvent )
    {
    iSessionTimer->Cancel();
    
    // HTTP status code
    RHTTPResponse response = aTransaction.Response();
    iHttpStatus = response.StatusCode();
         
    // If HTTP status is not what we expected, better to quit
    // status 0 will be handled in switch case default
    
    if ( !( iHttpStatus == HTTPStatus::EOk ||
    	    iHttpStatus == HTTPStatus::ECreated ||
            iHttpStatus == HTTPStatus::ENoContent ||
            iHttpStatus == HTTPStatus::EContinue ||
            iHttpStatus == 0 ) )
        {
        CompleteAndNotify( 
                       MapPostTransactionError( iHttpStatus ) );
        }
    else
        {                

        switch ( aEvent.iStatus ) 
            {
            // process the headers
            case THTTPEvent::EGotResponseHeaders:
                {
                // POST response headers
                // according to the RFC, location header is here if
                // the entity was created
                if( !iMoreToCome )
                    {
                    // Everything has been sent, no need to wait ESucceeded
                    
                    if ( ( iHttpStatus == HTTPStatus::EOk ) ||
                    	 ( iHttpStatus == HTTPStatus::ECreated ) ||
                         ( iHttpStatus == HTTPStatus::ENoContent ) )
                        {
                        CompleteAndNotify( KErrNone );
                        }
                     else
                        {
                        CompleteAndNotify( 
                           MapPostTransactionError( iHttpStatus ) );
                        }                            
                    }
                break;
                }
            case THTTPEvent::EGotResponseBodyData:
                {
                // discard the body data    
                MHTTPDataSupplier* responseBody = 
                                    aTransaction.Response().Body();
                ASSERT( responseBody );
                // Done with that bit of body data
                responseBody->ReleaseData();
                break;
                }
            case THTTPEvent::EResponseComplete:
                {
                // The transaction's response is complete
                break;
                }
            case THTTPEvent::ESucceeded:
                {
                // check the status code and decide whether we
                // actually succeeded or not

                // inform observers
                // according to the RFC we expect 200 OK,
                //                                204 NO CONTENT or
                //                                201 CREATED    
                if ( ( iHttpStatus == HTTPStatus::EOk ) ||
                	 ( iHttpStatus == HTTPStatus::ECreated ) ||
                     ( iHttpStatus == HTTPStatus::ENoContent ) )
                    {
                    CompleteAndNotify( KErrNone );
                    }
                 else
                    {
                    CompleteAndNotify( 
                       MapPostTransactionError( iHttpStatus ) );
                    }
            	break;
                }
            case THTTPEvent::EFailed:
            	{
            	// This is not supposed to do if transaction is finished
            	// happens when cancel is done
            	if ( iProcessState != EHttpTransactionIdle ) 
            	    {
            	    CompleteAndNotify( 
            	            MapPostTransactionError( iHttpStatus ) );
            	    }

                break;
                }
            case THTTPEvent::EReceived100Continue: 
                {
                // The server responded with a 100-Continue status code. 
                // HTTP FW continues with sending the body.
                Destroy100ContinueTimer();
                break;
                } 
            default:
                {
                if ( aEvent.iStatus < 0 )
                    {
                    CompleteAndNotify( aEvent.iStatus ); 
                    }
                else
                    {
                    CompleteAndNotify( KErrGeneral );
                    }
                break;
                }
            }
        }
    }
// --------------------------------------------------------------------------
// CHttpUploadWorker::MHFRunError()
// (See comments in header file)
// --------------------------------------------------------------------------
TInt CHttpUploadWorker::MHFRunError( TInt /*aError*/,
                                     RHTTPTransaction /*aTransaction*/,
                                     const THTTPEvent& /* aEvent*/ )
    {
    // Just notify the client about the failure and return KErrNone to 
    // the stack indicating that we handled this error.
    if ( iProcessState != EHttpTransactionIdle )
        {
        CompleteAndNotify( MapPostTransactionError( iHttpStatus ) );
        }

    return KErrNone;
    }

// --------------------------------------------------------------------------
// CHttpUploadWorker::GetNextDataPart()
// (See comments in header file)
// --------------------------------------------------------------------------
//
TBool CHttpUploadWorker::GetNextDataPart( TPtrC8& aDataPart )
    {
    if ( !iAlreadyDone )// Note: this is zero only when new part is requested
        {
        if ( iSendDataCount == 0 )
            {
            // first run
            TInt pos = iBodyFileOffset;
            iFile.Seek( ESeekStart, pos );
            }

        // We read data that will be given to the stack next time,
        // or we will find out that there is no more data...
        TInt readLength;
        if ( ( iOverallDataSize - iSendDataCount ) >= iBufferSize )
            {
            readLength = iBufferSize;
            }
        else
            {
            readLength = iOverallDataSize - iSendDataCount;
            }

        TPtr8 requestBodyBufferPtr = iRequestBodyBuffer->Des();

        TInt err = iFile.Read( requestBodyBufferPtr, readLength );
        iSendDataCount = iSendDataCount + iRequestBodyBuffer->Length();
        if ( err == KErrNone )
            {
            if ( ( iSendDataCount < iOverallDataSize ) &&
                 ( iRequestBodyBuffer->Length() > 0  ) )
                {  
                iMoreToCome = ETrue;
                }
            }
        iAlreadyDone = ETrue;
        } // on first call, allocate the current part

    aDataPart.Set( *iRequestBodyBuffer ); // .. otherwise, just re-use it

    //set timer
    iSessionTimer->Cancel();
    iSessionTimer->AfterSeconds( KSessionTimeout );

    return !iMoreToCome;
    }

// --------------------------------------------------------------------------
// CHttpUploadWorker::ReleaseData()
// (See comments in header file)
// --------------------------------------------------------------------------
//
void CHttpUploadWorker::ReleaseData()
    {
    // HTTP client is ready with the current data
    // notify observer if notifications are required
    // and tell stack whether we have more data coming
    // when done, close file etc.

    // notify iObserver if iTrackProgress flag is on
    if ( iProcessedFile->TrackingOn() )
        {
        iObserver->TransferProgress( iProcessedFile->Key(),
                                     iSendDataCount,
                                     iOverallDataSize );
        }

    if ( iMoreToCome )
        {
        TRAP_IGNORE( iHttpTransaction.NotifyNewRequestBodyPartL() );
        iMoreToCome = EFalse;
        }

    iAlreadyDone = EFalse;
    }

// --------------------------------------------------------------------------
// CHttpUploadWorker::OverallDataSize()
// (See comments in header file)
// --------------------------------------------------------------------------
//
TInt CHttpUploadWorker::OverallDataSize()
    {
    return iOverallDataSize;
    }

// --------------------------------------------------------------------------
// CHttpUploadWorker::Reset()
// (See comments in header file)
// --------------------------------------------------------------------------
//
TInt CHttpUploadWorker::Reset()
    {
    // Reset send data count and buffer. This will cause GetNextDataPart() 
    // to start from the beginning.
    iSendDataCount = 0;
    
    // Destroy the 100 continue response wait timer if it exists.
    Destroy100ContinueTimer();
    
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CHttpUploadWorker::Destroy100ContinueTimer()
// (See comments in header file)
// --------------------------------------------------------------------------
// 
void CHttpUploadWorker::Destroy100ContinueTimer() 
    {
    if ( i100ContinueTimer ) 
        {
        i100ContinueTimer->Cancel();
        delete i100ContinueTimer;
        i100ContinueTimer = NULL;
        }
    }

// --------------------------------------------------------------------------
// CHttpUploadWorker::Cancel100ContinueWaitL()
// (See comments in header file)
// --------------------------------------------------------------------------
// 
TInt CHttpUploadWorker::Cancel100ContinueWaitL( TAny* aParam ) 
    {
    CHttpUploadWorker* self =
        (static_cast<CHttpUploadWorker*>( aParam ));

    // Destroy the timer. We don't need subsequent events.
    self->Destroy100ContinueTimer(); 

    // Cancel waiting for 100 continue. As the server did not respond,
    // continue with sending the body.
    self->iHttpTransaction.SendEventL(
       THTTPEvent::ECancelWaitFor100Continue,
       THTTPEvent::EOutgoing,
       THTTPFilterHandle( THTTPFilterHandle::EClient) );

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CHttpUploadWorker::TimerEventL
// Disconnect connection
// -----------------------------------------------------------------------------
//        
void CHttpUploadWorker::TimerEventL( CHttpNotifyTimer* /*aTimer*/ )
    {
    CompleteAndNotify( KErrTimedOut );
    }

// End of File
