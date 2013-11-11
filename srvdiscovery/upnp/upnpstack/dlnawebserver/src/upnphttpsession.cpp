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
#include <in_sock.h>
#include <e32std.h>
#include <apgcli.h> // For RApaLsSession
#include <apmstd.h>
#include <escapeutils.h>
#include <bautils.h>
#include "upnphttpserver.h"
#include "upnpstring.h"
#include "upnpcons.h"
#include "upnphttpsession.h"
#include "upnphttpbuffer.h"
#include "upnphttpfiletransferreader.h"
#include "upnphttpfiletransferwriter.h"
#include "upnphttpmessagefactory.h"
#include "upnphttpheader.h"
#include "upnperrors.h"
#include "upnpcommonupnplits.h"
#include "upnpfileutils.h"
#define KLogFile _L("DLNAWebServer.txt")
#include "upnpcustomlog.h"
#include "upnphttpfileaccess.h"
#include "httperr.h"
#include "upnphttpservertransactioncreator.h"
#include "upnprangeheaderparser.h"
#include "upnphttpservertransaction.h"

// CONSTANTS
_LIT8( KTransferPending801, "HTTP/1.1 801 Transfer Pending");



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpHttpSession::NewL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
CUpnpHttpSession* CUpnpHttpSession::NewL( RSocket aSocket,
    CUpnpHttpServer* aServer, TInt aSessionId, TThreadPriority aPriority )
    {
    LOGS1(  "%i, CUpnpHttpSession::NewL - creating session for incoming connection",
            aSessionId );

    CUpnpHttpSession* self = new (ELeave) CUpnpHttpSession( aSocket, aServer,
        aSessionId, aPriority );
    CleanupStack::PushL( self );
    self->TcpConstructL( aSocket, KRcvBufSizeDefault,
        aServer->FileReadBufferSize() );
    self->ConstructL();
    CleanupStack::Pop( self );

    LOGS1( "%i, CUpnpHttpSession::NewL - HTTP *** Created new session.",
            self->Id( ) );

    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpSession::CUpnpHttpSession
// C++ default constructor
// -----------------------------------------------------------------------------
//
CUpnpHttpSession::CUpnpHttpSession( RSocket aSocket,
    CUpnpHttpServer* aServer, TInt aSessionId, TThreadPriority aPriority ) :
    CUpnpTcpSession( aSocket, aPriority )
    {
    iId = aSessionId;
    iServer = aServer;
    iRequestedTransferNotifySent = EFalse;
    // Assume that session should be kept alive unless the request
    // with HTTP header "Connection: close" comes.
    iSessionKeepAlive = ETrue;
    iSessionIsDeletedAfterResponse = EFalse;
    iUsesConnectionClose = EFalse;
    iSaveToFile = EFalse;
    iOverwriteExisting = EFalse;
    iOffset = 0;
    iLength = 0;
    iSaveAtOffset = EFalse;
    iMaximumSizeChecked = EFalse;
    iHeadersCompleted = EFalse;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpSession::ConstructL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
void CUpnpHttpSession::ConstructL()
    {
    LOGS1( "%i, CUpnpHttpSession::ConstructL", iId );
    iFileServe = NULL;

    iDestinationPath = HBufC8::NewL( 0 );
    iSenderPath = HBufC8::NewL( 0 );

    iInFilename = HBufC8::NewL( 0 );
    iOutFilename = HBufC8::NewL( 0 );

    iPendingRequests = new (ELeave) RPointerArray<CUpnpHttpMessage> ( 1 );
    }

// -----------------------------------------------------------------------------
// CUpnpHttpSession::~CUpnpHttpSession
// C++ default destructor
// -----------------------------------------------------------------------------
//
CUpnpHttpSession::~CUpnpHttpSession()
    {
    LOGS1( "%i, CUpnpHttpSession::~CUpnpHttpSession()", iId );

    delete iFileServe;
    delete iInFilename;
    delete iOutFilename;
    delete iDestinationPath;
    delete iSenderPath;

    if ( iPendingRequests )
        {
        iPendingRequests->ResetAndDestroy();
        iPendingRequests->Close();
        delete iPendingRequests;
        }

    // no deleting of iPendingRequest, it's not owned
    iPendingRequest = NULL;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpSession::SendL
// Send HTTP message.
// -----------------------------------------------------------------------------
//
void CUpnpHttpSession::SendL( CUpnpHttpMessage* aMessage )
    {
    LOGS1( "%i, CUpnpHttpSession::SendL(CUpnpHttpMessage*)", iId );

    if ( !IsConnected() )
        {
        return;
        }

    iSessionNeedsErrorReply = EFalse;

    // Set senders and destination path
    delete iSenderPath;
    iSenderPath = NULL;
    delete iDestinationPath;
    iDestinationPath = NULL;

    if ( aMessage->Method().Find( KHttp11WithoutSpace ) == 0 )
        {
        iSenderPath = HBufC8::NewL( 0 );
        aMessage->AddPairL( UpnpHTTP::KHdrServer(),
            iServer->ServerDescription() );
        }
    else
        {
        iSenderPath = aMessage->SenderPathFromHeader().AllocL();
        }

    iDestinationPath = aMessage->SenderPath().AllocL();

    // Set other properties
    iRequestType = aMessage->Type();
    iRetryCounter = aMessage->RetryCounter();

    if ( aMessage->PendingRequest() )
        {
        iPendingRequest = aMessage->PendingRequest();
        }

    //we assume that converting to UTF8 is already done by upper layers
    HBufC8* buf = aMessage->ToStringL();
    CleanupStack::PushL( buf );
    WriteL( *buf );
    CleanupStack::PopAndDestroy( buf );
    if ( !iIsPersistent )
        {
        iSessionKeepAlive = EFalse;
        }
    }

// -----------------------------------------------------------------------------
// CUpnpHttpSession::DeleteThisSessionL
// Delete the session.
// -----------------------------------------------------------------------------
//
void CUpnpHttpSession::DeleteThisSessionL( CUpnpTcpSession* /*aSession*/)
    {
    LOGS1( "%i, CUpnpHttpSession::DeleteThisSessionL", iId );

    if ( iReceivedMessage )
        {
        if ( iReceivedMessage->BytesInBuffer() > 0
                && iReceivedMessage->IsReadyL() )
            {
            TPtrC8 content = iReceivedMessage->Content();
            CUpnpHttpMessage* msg = CUpnpHttpMessage::NewL( content,
                RemoteHost(), iId );
            CleanupStack::PushL( msg );

            msg->SetType( iRequestType );

            msg->SetRetryCounter( iRetryCounter );

            THttpInvalidMessage aVal;
            aVal = IsValidMessage( msg );

            switch ( aVal )
                {

                case EMessageOk:
                    //HandleQuery will take ownership of msg object
                    CleanupStack::Pop( msg );
                    // Pass the message forward
                    HandleQueryL( msg );
                    break;

                default:
                    CleanupStack::PopAndDestroy( msg );
                    break;
                }
            }
        }

    ShutdownStop();

    }

// -----------------------------------------------------------------------------
// CUpnpHttpSession::DeleteFileIfExists
//
// -----------------------------------------------------------------------------
//
void CUpnpHttpSession::DeleteFileIfExists( TDesC16& aFilename )
    {
    iServer->FileSession().Delete( aFilename );
    }

// -----------------------------------------------------------------------------
// CUpnpHttpSession::MatchHttpError
//
// -----------------------------------------------------------------------------
//
TInt CUpnpHttpSession::MatchHttpError( TInt error )
    {
    TInt httpError = KErrNone;

    if ( error == KErrDiskFull )
        httpError = EHttpInsufficientStorage;
    else if ( error == KErrOverflow ) //one of the reason can be if chunk value is > 2^32
        httpError = EHttpInsufficientStorage;
    else if ( error == KErrAbort ) //POST HTTP/1.1
        httpError = EHttpBadRequest;
    else if ( error == KErrGeneral || error == KErrHttpUnknownParseState ) // this error may occur e.g when ContentLengthL() leaves
        httpError = EHttpBadRequest;
    else if ( -error >= EHttpBadRequest ) // HTTP errors
        httpError = -error;
    else if ( error == KErrNoMemory )
        httpError = EHttpEntityTooLarge; // OOM handling
    else if ( error == KErrNotSupported )
        httpError = EHttpNotFound; // no transaction
    else
        httpError = EHttpInternalServerError;

    return httpError;

    }
// -----------------------------------------------------------------------------
// CUpnpHttpSession::BufferReceivedL
//
// -----------------------------------------------------------------------------
//
void CUpnpHttpSession::BufferReceivedL( TDesC8& aBuffer )
    {

    TInt index = 0;
    TPtrC8 ptrIn( aBuffer );
    iPendingPostRequests = ETrue;
    //handling requests on the server-side
    if ( ptrIn.Length() > KHttpPost().Length() )
        {
        TPtrC8 ptrInOffset( ptrIn.Mid( KHttpPost().Length() ) );

        //this loop splits buffer if it includes more than one POST message
        while ( (index = ptrInOffset.Find( KHttpPost() )) >= 0 )
            {
            TPtrC8 ptrL = ptrIn.Left( index + KHttpPost().Length() );
            SinglePostBufferReceivedL( ptrL );
            ptrIn.Set( ptrIn.Mid( index + KHttpPost().Length() ) );
            ptrInOffset.Set( ptrIn.Mid( KHttpPost().Length() ) );
            }
        }
    SinglePostBufferReceivedL( ptrIn );
    iPendingPostRequests = EFalse;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpSession::SinglePostBufferReceivedL
//
// -----------------------------------------------------------------------------
//
void CUpnpHttpSession::SinglePostBufferReceivedL( TDesC8& aBuffer )
    {
    TInt error = KErrNone;
        TRAP( error, BufferReceivedProcessL( aBuffer ) )
    LOGS2( "%i, CUpnpHttpSession::BufferReceivedL finished with error %i", iId, error );
    if ( error != KErrNone )
        {
        //if session left - no continuation of file transfer
        if ( iFTReader && iFTReader->Activated() )
            {
            iFTReader->SetActivated( EFalse );
            if ( iFileServe )
                iFileServe->DeleteFile();
            }

        TInt httpError = MatchHttpError( error );
        if ( httpError != KErrNone )
            {// If we act as a server side
            ResponseErrorL( httpError );
            if ( httpError == EHttpEntityTooLarge )
                {
                NotifyErrorL( httpError, error );
                }
            ShutdownStop();
            return;
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpHttpSession::BufferReceivedCheckingHeadersL
//
// -----------------------------------------------------------------------------
//
void CUpnpHttpSession::BufferReceivedCheckingHeadersL( TDesC8& aBuffer )
    {
    // If iInFilename is defined, data should be saved in to a file,
    // this is the case of incoming GET response and for following parts of POST request.
    if ( iInFilename && iInFilename->Length() > 0 )
        {
        iSaveToFile = ETrue;

        // If this flag was set it means that even previously message was not ment
        // to be saved to file, now when the body is downloaded it will be saved to file.
        if ( iContinuedMessageDownload && !iReceivedMessage->IsToFile() )
            {
            iReceivedMessage->SetToFile( ETrue );
            }

        // Check Maximum Size requirement, but do this only once
        if ( !iMaximumSizeChecked )
            {
            //Make temp buffer to check content of incoming data,
            //its possible that content-Length is too huge (>2^32)
            CUpnpHttpBuffer* checkBuf = CUpnpHttpBuffer::NewL( this, EFalse );
            CleanupStack::PushL( checkBuf );
            //leaves in case of problems with string operation, no HTTP error
            checkBuf->AppendHeadersL( aBuffer, iSaveToFile );

            if ( checkBuf->IsHeaderReady() )
                {
                // Now headers were checked
                iHeadersCompleted = ETrue;
                TRAPD( error, checkBuf->ContentLengthL( ) );
                if ( error )
                    {
                    iSaveToFile = EFalse;
                    }
                iMaximumSizeChecked = ETrue;

                //mark chunked encoding
                iIsChunked = checkBuf->IsChunked();
                //mark if persistent connection
                if ( IsPersistentConnection() )
                    iIsPersistent = !checkBuf->UsesConnectionClose();
                }
            else
                {
                iSaveToFile = EFalse;
                }

            CleanupStack::PopAndDestroy( checkBuf );
            }
        }
    // Checking that if file is a POST that needs to be saved to a file. Only check this
    // for the first time (iFileServe does not exist only on receiving the first buffer of data)
    else if ( !iFileServe )
        {
        // making temp buffer to check content of incoming data
        CUpnpHttpBuffer* checkBuf = CUpnpHttpBuffer::NewL( this, EFalse );
        CleanupStack::PushL( checkBuf );
        //leaves in case of problems with string operation, no HTTP error
        checkBuf->AppendHeadersL( aBuffer, iSaveToFile );

        if ( checkBuf->IsHeaderReady() )
            {
            // Now headers were checked
            iHeadersCompleted = ETrue;

            TPtrC8 headers = checkBuf->Headers();
            CUpnpHttpMessage* tempMsg = NULL;
            TRAPD( err, tempMsg = CUpnpHttpMessage::NewL( (TDesC8&) headers,
                            RemoteHost( ), iId ) );

            //POST HTTP/1.1
            if ( err == KErrAbort )
                {
                if ( checkBuf->Method().Find( KHttpPost ) == 0 )
                    //leaves in case of HTTP error for a POST
                    User::Leave( KErrAbort );
                }
            else if ( err < KErrNone )
                {
                User::Leave( err );
                }
            else if ( err == KErrNone )
                {
                CleanupStack::PushL( tempMsg );

                // Check method and make decision about handling message.
                // It's a POST message
                if ( tempMsg->Method().Find( KHttpPost ) == 0 )
                    {
                    // 100-Continue check should be done for all types of POST reqests, even for GENAs and SOAPs

                    // First thing to do in case of upload is to chceck if there
                    // is sufficient disk space to save incoming file.
                    TUint fileSize = 0;
                    TBool hasCL;
                    TLex8 cntLength( tempMsg->IsHeader(
                        UpnpHTTP::KHdrContentLength, hasCL ) );
                    if ( hasCL )
                        {
                        TInt errorLength = cntLength.Val( fileSize );
                        if ( errorLength == KErrOverflow )
                            {
                            iSaveToFile = EFalse;
                            //leaves in case of HTTP error
                            User::Leave( -EHttpInsufficientStorage );
                            }
                        else if ( errorLength != KErrNone )
                            {
                            iSaveToFile = EFalse;
                            //leaves in case of HTTP error
                            User::Leave( errorLength );
                            }
                        }
                    //First check URI, because there is no sense in sending 100 continue for invalid URI
                    if (!(tempMsg->IsGena()) && !(tempMsg->IsSoap()))
                        {
                        //response is sent inside function

                        delete iInFilename;
                        iInFilename = NULL;
                        
                        if ( !iServer->TransactionCreator() )
                            {
                            User::Leave( KErrNotSupported );
                            }
                        if ( UpnpFileUtil::ParseUri( tempMsg->SenderPathFromHeader() ) 
                             != KErrNone )
                            {
                            iInFilename = KNullDesC8().AllocL();
                            iSaveToFile = EFalse;
                            User::Leave( -EHttpBadRequest );
                            }
                        TPtrC8 relativeUri = PrepareRelativeUriL( tempMsg->SenderPathFromHeader() );
                        CUpnpHttpServerTransaction* transactionOut;
                        iServer->TransactionCreator()->NewTransactionL(
                                tempMsg->Method(), relativeUri,
                                tempMsg->Sender(), transactionOut );
                        CleanupStack::PushL( transactionOut );

                        transactionOut->SetRequest(tempMsg);
                        transactionOut->OnCallbackL( CUpnpHttpServerTransaction::EOnRequestStart );
                        
                        iSaveToFile = EFalse;
                        User::LeaveIfError( transactionOut->Error() );
                        RBuf fileName;
                        fileName.CreateL( KMaxFileName );
                        CleanupClosePushL( fileName );
                        User::LeaveIfError( transactionOut->DataSink().FullName( fileName ) );
                        iInFilename = UpnpString::FromUnicodeL( fileName );
                        iSaveToFile = ETrue;
                        CleanupStack::PopAndDestroy(&fileName);
                        
                        transactionOut->OnCallbackL( CUpnpHttpServerTransaction::EOnComplete );
                        CleanupStack::PopAndDestroy( transactionOut );
                        }

                    
                    //Maximum size was checked already so don't do this again
                    iMaximumSizeChecked = ETrue;

                    // If client sends Expect: 100-continue header then it must be checked
                    // if file can be saved and if so, server should responds with 100.
                    if ( tempMsg->GetHeaderValue( UpnpHTTP::KHdrExpect ).FindC(
                        UpnpHTTP::K100Continue ) == 0 )
                        {

                        // Check disk space and ContentLength limit (2^32) and if it's not sufficient send 507 response and
                        // close connection - sugested behaviour from DLNA 1.5 spec.

                        HBufC16* pathTmp = NULL;
                        pathTmp = UpnpString::ToUnicodeL( iInFilename->Des() );
                        CleanupStack::PushL( pathTmp );

                        TBool noSpace = UpnpFileUtil::CheckDiskSpaceShortL(
                            pathTmp->Des(), fileSize, iServer->FileSession() );
                        CleanupStack::PopAndDestroy( pathTmp );

                        if ( noSpace )
                            {
                            iSaveToFile = EFalse;
                            //leaves in case of HTTP error
                            User::Leave( -EHttpInsufficientStorage );
                            }
                        else
                            {
                            // Set the flag informing that current message's body will
                            // should be downloaded in some time
                            iContinuedMessageDownload = ETrue;

                            // If there is enough space to save incoming file send response 100,
                            // which means that client can continue with sending the body.
                            CUpnpHttpMessage* reply =
                                    RUpnpHttpMessageFactory::Http11ResponseL(
                                        tempMsg, EHttpContinue );
                            CleanupStack::PushL( reply );
                            HBufC8* sendBuf = reply->ToStringL();
                            CleanupStack::PushL( sendBuf );
                            WriteL( *sendBuf );
                            CleanupStack::PopAndDestroy( sendBuf );
                            CleanupStack::PopAndDestroy( reply );
                            }

                        }
                    }
                if ( iSaveToFile )
                    {
                    CleanupStack::Pop( tempMsg );
                    tempMsg->SetType( ETransferStart );
                    iServer->ToReceiveStackD( tempMsg );
                    }
                else
                    {
                    CleanupStack::PopAndDestroy( tempMsg );
                    }
                }
            }
        //mark if chunked encoding
        iIsChunked = checkBuf->IsChunked();
        //mark if persistent connection
        if ( IsPersistentConnection() )
            iIsPersistent = !checkBuf->UsesConnectionClose();
        CleanupStack::PopAndDestroy( checkBuf );
        }
    else
        {
        iSaveToFile = EFalse;
        }
    }

// -----------------------------------------------------------------------------
// CUpnpHttpSession::BufferReceivedProcessL
//
// -----------------------------------------------------------------------------
//
void CUpnpHttpSession::BufferReceivedProcessL( TDesC8& aBuffer )
    {
    TInt httpError;
    LOGS1( "%i, CUpnpHttpSession::BufferReceivedProcessL", iId );

    if ( !iHeadersCompleted && iReceivedMessage )
        {
        // If all headers aren't completed after receiving a few data chunks,
        // enlarge the buffer and try again.
        HBufC8* headers = HBufC8::NewLC( iReceivedMessage->Content().Length()
                + aBuffer.Length() );

        headers->Des().Zero();
        headers->Des().Append( iReceivedMessage->Content() );
        headers->Des().Append( aBuffer );
        //returns HTTP error only in case of a POST
        BufferReceivedCheckingHeadersL( *headers );
        CleanupStack::PopAndDestroy( headers );
        }
    else if ( !iHeadersCompleted && !iReceivedMessage )
        {
        // The first data chunk, both variables are not set. returns HTTP error only in case of a POST
        BufferReceivedCheckingHeadersL( aBuffer );
        }

    // If message content should be saved to file then create a file access object
    // that takes care of this operation.
    if ( iSaveToFile && !iFileServe )
        {
        HBufC16* name = NULL;
        name = UpnpString::ToUnicodeL( *iInFilename );
        CleanupStack::PushL( name );
        iFileServe = CUpnpHttpFileAccess::NewL( this, *name );
        CleanupStack::PopAndDestroy( name );

        // If it's response for Range reqest then setup range saving parameters in iFileServe
        if ( iSaveAtOffset )
            {
            iFileServe->SetPosOfFile( iOffset );
            iFileServe->SetEndPosOfFile( iOffset + Length() - 1 );
            }

        delete iInFilename;
        iInFilename = NULL;

        iInFilename = UpnpString::FromUnicodeL( iFileServe->FileToServe() );
        }
    //initiate filetransfer component
    if ( iSaveToFile && iFileServe )
        {
        //file transfer reader
        iFileServe->SetEncodingMode( iIsChunked );
        FileTransferReaderConstructL();
        }

    if ( !iReceivedMessage )
        {
        iReceivedMessage = CUpnpHttpBuffer::NewL( this, iSaveToFile );
        }
    //leaves in case of problems with string operation, no HTTP error
    User::LeaveIfError( iReceivedMessage->AppendL( aBuffer, iSaveToFile ) );

    // This 'if statement' is required for proper deleting partial file
    // when 'stop transfer' was invoked during exporting the resource from other MS.
    if ( iFileServe && iReceivedMessage )
        iFileServe->SetTotalSize( iReceivedMessage->ContentLengthL() );

    
    if ( FileTransferReader() && FileTransferReader()->Activated() )
        return;
    // Finish receiving message and handle query contained in message.
    // Processing goes on and on till the incoming buffer is empty -
    // this is the way the pipelining is handled.
    while ( iReceivedMessage->IsReadyL() )
        {

        TPtrC8 buffer = iReceivedMessage->Content();

        CUpnpHttpMessage* msg = NULL;
        TInt err( KErrNone );
            TRAP( err, msg = CUpnpHttpMessage::NewL( buffer, RemoteHost( ), iId ) );
        CleanupStack::PushL( msg );

        if ( err < KErrNone )
            {
            LOGS1(
                    "%i, CUpnpHttpSession::BufferReceivedProcessL - failed", iId );

            // sending response to remote host: bad request
            httpError = MatchHttpError( -EHttpBadRequest );
            if ( httpError != KErrNone )
                {// If we act as a server side
                ResponseErrorL( httpError );
                }
            CleanupStack::PopAndDestroy( msg );
            }
        else
            {
            iSessionKeepAlive = ETrue;
            if ( IsPersistentConnection() )
                if ( !iFileServe && iPendingRequests->Count() == 0 )
                    if ( msg->GetHeaderValue( UpnpHTTP::KConnection() ).FindC(
                        UpnpHTTP::KClose() ) >= 0
                            || msg->RequestHTTPVersion() == KHttp10() )
                        {
                        iIsPersistent = EFalse;
                        }

            if ( !IsPersistentConnection() && !msg->IsSoap()
                    && !msg->IsGena() )
                {
                iSessionKeepAlive = EFalse;
                }

            msg->SetType( iRequestType );

            msg->SetSessionId( iId );

            msg->SetRetryCounter( iRetryCounter );

            if ( (iSaveToFile) && (iInFilename) )
                {
                msg->SetInFilenameL( iInFilename->Des() );
                }

            THttpInvalidMessage aVal;
            aVal = IsValidMessage( msg );

            switch ( aVal )
                {
                case EMessageOk:
                    //HandleQueryL will take ownership of msg object
                    CleanupStack::Pop( msg );
                    TRAP( err, HandleQueryL( msg ) );
                    if ( err < KErrNone ) 
                        {
                        httpError = MatchHttpError( err );
                        if ( httpError != KErrNone )
                            {// If we act as a server side
                            ResponseErrorL( httpError );
                            }
                        }

                    break;

                case EUnknownMethod:
                    {
                    LOGS1H(
                            iHandle,
                            "%i, CUpnpHttpSession::BufferReceivedL - Unknown method",
                            iId );

                    httpError = MatchHttpError( -EHttpMethodNotAllowed );
                    if ( httpError != KErrNone )
                        {// If we act as a server side
                        ResponseErrorL( httpError );
                        }
                    CleanupStack::PopAndDestroy( msg );
                    }
                    break;

                default:
                    LOGS1(
                            "%i, CUpnpHttpSession::BufferReceivedL - failed", iId );
                    CleanupStack::PopAndDestroy( msg );
                    break;
                };

            }

        // No continued message download is expected because
        // the message processing is finished
        iContinuedMessageDownload = EFalse;
        iReceivedMessage->RemoveL( buffer.Length() );

        iHeadersCompleted = EFalse;

        delete iInFilename;
        iInFilename = NULL;
        iInFilename = HBufC8::NewL( 0 );

        if ( iSessionIsDeletedAfterResponse )
            {
            StartDisconnect();
            }
        }

    if ( iReceivedMessage->BytesInBuffer() <= 0 )
        {
        delete iReceivedMessage;
        iReceivedMessage = NULL;
        if ( iSessionIsDeletedAfterResponse )
            {
            StartDisconnect();
            }
        else
            {
            StartTimeoutTimer( iSessionNeedsErrorReply );
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpHttpSession::ResponseErrorL
//
// -----------------------------------------------------------------------------
//
void CUpnpHttpSession::ResponseErrorL( TInt aStatus )
    {
    LOGS2( "%i, CUpnpHttpSession::ResponseErrorL - error code %i",
            iId, aStatus );

    CUpnpHttpMessage* reply = NULL;
    reply = RUpnpHttpMessageFactory::HttpResponseErrorL( RemoteHost(),
        aStatus );
    CleanupStack::PushL( reply );

    reply->AddPairL( UpnpHTTP::KHdrServer(), iServer->ServerDescription() );
    reply->SetSessionId( Id() );
    reply->SetDestinationPathL( *iDestinationPath );
    reply->SetSenderPathL( *iSenderPath );

    HBufC8* sendBuf = reply->ToStringL();
    CleanupStack::PushL( sendBuf );
    WriteL( *sendBuf );
    CleanupStack::PopAndDestroy( sendBuf );
    CleanupStack::PopAndDestroy( reply );
    }
// -----------------------------------------------------------------------------
// CUpnpHttpSession::HandleQueryL
//
// -----------------------------------------------------------------------------
//
void CUpnpHttpSession::HandleQueryL( CUpnpHttpMessage* aMsg )
    {
    LOGS1( "%i, CUpnpHttpSession::HandleQueryL", iId );
    CleanupStack::PushL( aMsg );
    
    // Handle GET and HEAD requests
    if ( aMsg->Method().Find( KHttpGet ) == 0 || aMsg->Method().Find(
        KHttpHead ) == 0 )
        {
        // method starts with GET or HEAD
        if ( FileServeExists() || (FileTransferWriter()
                && FileTransferWriter()->IsActive()) )
            {
            User::LeaveIfError( iPendingRequests->Append( aMsg ) );
            CleanupStack::Pop( aMsg );
            }
        else
            {
            User::LeaveIfError( PrepareToServeFileL( aMsg ) );
            CleanupStack::PopAndDestroy( aMsg );
            aMsg = NULL;
            }
        }

    // Handle GENA, SOAP, responses, other messages etc.
    else
        {
        aMsg->SetInFilenameL( *iInFilename );
        delete iInFilename;
        iInFilename = NULL;
        iInFilename = HBufC8::NewL( 0 );
        if ( !aMsg->Is1xx() )
            {
            CleanupStack::Pop( aMsg );
            NotifyUpperLayerLD( aMsg );
            iPendingRequest = NULL;
            }
        else
            {
            CleanupStack::PopAndDestroy( aMsg );
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpHttpSession::PrepareToServeFileL
//
// -----------------------------------------------------------------------------
//
TInt CUpnpHttpSession::PrepareToServeFileL( CUpnpHttpMessage* aMsg )
    {

    LOGS1( "%i, CUpnpHttpSession::PrepareToServeFileL", iId );
    if ( !iServer->TransactionCreator() )
         {
         User::Leave( KErrNotSupported ); 
         }
    
    CheckSessionPersistence( aMsg );
    TInt parseUrlError = UpnpFileUtil::ParseUri( aMsg->SenderPathFromHeader() );
    if ( parseUrlError != KErrNone )
        {
        return -EHttpBadRequest;
        }
    
    TPtrC8 relativeUri = PrepareRelativeUriL( aMsg->SenderPathFromHeader() );
    
    CUpnpHttpServerTransaction* transactionOut; 
    iServer->TransactionCreator()->NewTransactionL( aMsg->Method() ,
            relativeUri, aMsg->Sender(), transactionOut );    
    CleanupStack::PushL( transactionOut );
    
    transactionOut->SetRequest( aMsg );    
    transactionOut->OnCallbackL( CUpnpHttpServerTransaction::EOnRequestStart );
    TInt error = transactionOut->Error();
    if( error != KErrNone )
        {
        CleanupStack::PopAndDestroy( transactionOut ); 
        return error;
        }
                 
    // 2. Try to open requested file
    RFile& file = transactionOut->DataSource();
    
    TInt fileSize = -1;    
    if( file.Size( fileSize ) != KErrNone  )
        {
        fileSize = -1;
        }

    TBool servingOutFilename = ( aMsg->OutFilename().Length() > 0 );

    TInt result( KErrNone );
    // 3. If file was opened it means it exists and might be served
    if ( fileSize >= 0 )
        {
        TInt rangeStatus;
        TInt startPos;
        TInt endPos;
        TInt errorValue;
        errorValue = PrepareRangeHeaderL( 
            aMsg, servingOutFilename, fileSize, *transactionOut,
            rangeStatus, startPos, endPos );
        
        if ( errorValue == KErrNone )
            {
            if ( EHttpNoContent == rangeStatus )
                {
                transactionOut->AddResponseHeaderL( UpnpHTTP::KHdrServer(),
                                                    iServer->ServerDescription() );             
             
                WriteL( transactionOut->QueryResponseHeader() );
                }
            else
                {
                transactionOut->OnCallbackL( CUpnpHttpServerTransaction::EOnResponseStart );
                //last header fiished by double CRLF so we must add last CRLF

                errorValue = transactionOut->Error();
                if ( errorValue != KErrNone  ) 
                    {
                        CleanupStack::PopAndDestroy( transactionOut );
                        return errorValue;
                    }
                if ( transactionOut->QueryResponseHeader().Length() > 0 )
                    {
                    transactionOut->AddResponseHeaderL( UpnpHTTP::KHdrServer(),
                                                        iServer->ServerDescription() );             
                    RBuf fileName;
                    fileName.CreateL( KMaxFileName );
                    CleanupClosePushL( fileName );
                    User::LeaveIfError( file.FullName( fileName ) );
                    WriteMsgAndHeaderL( aMsg, transactionOut->QueryResponseHeader(),
                        fileName, fileSize, rangeStatus, startPos, endPos );
                    CleanupStack::PopAndDestroy( &fileName );
                    }
                }
            }
        result = errorValue;
        }
    else // 4. File was not found, so just respose with HTTP 404 error.
        {

        if ( !iSaveToFile )
            {
            //error. No such file or folder found. Send error message.
            result = -EHttpNotFound;
            }
        }
    if ( KErrNone == result )
        {
        result = transactionOut->Error();
        }    

    
    transactionOut->OnCallbackL( CUpnpHttpServerTransaction::EOnComplete );
    CleanupStack::PopAndDestroy( transactionOut );
    return result;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpSession::WriteMsgAndHeaderL
//
// -----------------------------------------------------------------------------
//
void CUpnpHttpSession::WriteMsgAndHeaderL( CUpnpHttpMessage* aMsg,
    const TDesC8& aHeaderToWrite, const TDesC& aPathWithNewMethod,
    TInt aFileSize, TInt aRangeStatus, const TInt aStartPos,
    const TInt aEndPos )
    {
    if ( aMsg->Method().Find( KHttpHead ) == 0 ) // HEAD - just header
        {
        WriteL( aHeaderToWrite );
        DeleteServeFileL();
        }
    else // GET response and also sending POST message
        {
        if ( iFileServe )
            {
            delete iFileServe;
            iFileServe = NULL;
            }

        iFileServe = CUpnpHttpFileAccess::NewL( this, aHeaderToWrite,
            aPathWithNewMethod, aFileSize );

        if ( aStartPos != KErrNotFound )
            iFileServe->SetPosOfFile( aStartPos );

        if ( aEndPos != KErrNotFound )
            iFileServe->SetEndPosOfFile( aEndPos );

        if ( !FileTransferWriter() )
            {
            TInt requestedBytes = aRangeStatus == EHttpPartialContent
                ? aEndPos - aStartPos + 1 : aFileSize;
            if ( requestedBytes <= HttpServer()->FileWriteBufferSize() )
                {
                FileTransferWriterConstructL( requestedBytes
                        + aHeaderToWrite.Length() );
                }
            else
                {
                FileTransferWriterConstructL(
                    HttpServer()->FileWriteBufferSize() );
                }
            }
        FileTransferWriter()->Reset();

        if ( iOutFilename->Length() > 0 && iDestinationPath->Length() > 0 )
            {
            FileTransferWriter()->SetPostNotify();
            }
        FileTransferWriter()->StartL();
        }
    }

// -----------------------------------------------------------------------------
// CUpnpHttpSession::FileServeExists
//
// -----------------------------------------------------------------------------
//
TBool CUpnpHttpSession::FileServeExists()
    {
    return iFileServe ? ETrue : EFalse;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpSession::DeleteServeFileL
//
// -----------------------------------------------------------------------------
//
void CUpnpHttpSession::DeleteServeFileL()
    {
    LOGS1( "%i, CUpnpHttpSession::DeleteServeFileL()", iId );

    CUpnpHttpMessage* msg = RUpnpHttpMessageFactory::HttpResponseErrorL(
        iAddr, EHttp200Ok );
    CleanupStack::PushL( msg );
    msg->SetType( EExportComplete );

    msg->SetSessionId( Id() );

    CleanupStack::Pop( msg );
    iServer->ToReceiveStackD( msg );

    delete iFileServe;
    iFileServe = NULL;

    if ( iPendingRequests->Count() > 0 )
        {
        CUpnpHttpMessage* request = (*iPendingRequests)[0];
        iPendingRequests->Remove( 0 );
        CleanupStack::PushL( request );
        User::LeaveIfError( PrepareToServeFileL( request ) );
        CleanupStack::PopAndDestroy( request );
        }
    }

// -----------------------------------------------------------------------------
// CUpnpHttpSession::NotifyErrorL
//
// -----------------------------------------------------------------------------
//
void CUpnpHttpSession::NotifyErrorL( const TInt aError, TInt aInternalError )
    {
    LOGS2( "%i, CUpnpHttpSession::NotifyErrorL error = %i", iId,
            aError );

    CUpnpHttpMessage* msg = NULL;
    msg = RUpnpHttpMessageFactory::HttpResponseErrorL( RemoteHost(), aError );
    msg->SetInternalError( aInternalError );
    NotifyUpperLayerLD( msg );
    }

// -----------------------------------------------------------------------------
// CUpnpHttpSession::NotifyDisconnectL
//
// -----------------------------------------------------------------------------
//
void CUpnpHttpSession::NotifyDisconnectL( TInetAddr aAddr )
    {
    LOGS1( "%i, CUpnpHttpSession::NotifyDisconnectL", iId );

    if ( iSenderPath->Length() > 0 && iDestinationPath->Length() > 0 )
        {
        CUpnpHttpMessage* msg = RUpnpHttpMessageFactory::HttpResponseErrorL(
            aAddr, EHttpRequestTimeout );
        CleanupStack::PushL( msg );
        msg->SetInFilenameL( *iInFilename );
        msg->SetInternalError( iSessionError );

        CleanupStack::Pop( msg );
        NotifyUpperLayerLD( msg );

        if ( iReceivedMessage && iFileServe )
            {
            iFileServe->SetTotalSize( iReceivedMessage->ContentLengthL() );
            }
        }
    else if ( iReceivedMessage && iReceivedMessage->Method() == KHttpPost )
        {
        if ( !iErrorNotified )
            {
            // incoming post failed
            CUpnpHttpMessage* msg =
                    RUpnpHttpMessageFactory::HttpResponseErrorL( iAddr,
                        EHttpRequestTimeout );
            CleanupStack::PushL( msg );
            msg->SetType( ETransferError );
            msg->SetSessionId( Id() );
            // get destination path
            msg->SetDestinationPathL( iReceivedMessage->Target() );

            CleanupStack::Pop( msg );
            iServer->ToReceiveStackD( msg );
            iErrorNotified = ETrue;
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpHttpSession::NotifyTimeoutL
//
// -----------------------------------------------------------------------------
//
void CUpnpHttpSession::NotifyTimeoutL()
    {
    if ( iReceivedMessage )
        {
        if ( iReceivedMessage->Method() == KHttpPost )
            {
            // incoming post failed
            CUpnpHttpMessage* msg =
                    RUpnpHttpMessageFactory::HttpResponseErrorL( iAddr,
                        EHttpRequestTimeout );
            CleanupStack::PushL( msg );
            msg->SetType( ETransferError );
            msg->SetSessionId( Id() );

            // get destination path
            msg->SetDestinationPathL( iReceivedMessage->Target() );

            CleanupStack::Pop( msg );
            iServer->ToReceiveStackD( msg );
            iErrorNotified = ETrue;
            if ( iFileServe )
                iFileServe->DeleteFile();
            }
        else if ( iReceivedMessage->Method().Find( KHttp8 ) != KErrNotFound
                && iReceivedMessage->Target() == KHttpOkCode8 )
            { // incoming response failed
            CUpnpHttpMessage* msg =
                    RUpnpHttpMessageFactory::HttpResponseErrorL( iAddr,
                        EHttpRequestTimeout );
            CleanupStack::PushL( msg );
            msg->SetType( ETransferError );

            // get destination path
            msg->SetDestinationPathL( KNullDesC8 );
            msg->SetSessionId( Id() );

            CleanupStack::Pop( msg );
            iServer->ToReceiveStackD( msg );
            iErrorNotified = ETrue;
            }
        else
            { // incoming response failed
            CUpnpHttpMessage* msg =
                    RUpnpHttpMessageFactory::HttpResponseErrorL( iAddr,
                        EHttpRequestTimeout );
            CleanupStack::PushL( msg );
            msg->SetType( ETransferError );

            // get destination path
            msg->SetDestinationPathL( iReceivedMessage->Target() );
            msg->SetSessionId( Id() );
            if ( iInFilename )
                {
                msg->SetInFilenameL( *iInFilename );

                }
            CleanupStack::Pop( msg );
            iServer->ToReceiveStackD( msg );
            iErrorNotified = ETrue;
            }

        }
    else
        {
        CUpnpHttpMessage* msg = RUpnpHttpMessageFactory::HttpResponseErrorL(
            iAddr, EHttpRequestTimeout );
        CleanupStack::PushL( msg );
        msg->SetType( ETransferError );

        // get destination path
        msg->SetDestinationPathL( KNullDesC8 );
        msg->SetSessionId( Id() );

        CleanupStack::Pop( msg );
        iServer->ToReceiveStackD( msg );
        iErrorNotified = ETrue;
        }
    }

// -----------------------------------------------------------------------------
// CUpnpHttpSession::SocketServerBusyL
//
// -----------------------------------------------------------------------------
//
void CUpnpHttpSession::SocketServerBusyL( const TInetAddr& aAddr )
    {
    CUpnpHttpMessage* msg = RUpnpHttpMessageFactory::HttpResponseErrorL(
        aAddr, EHttpExpectationFailed );
    CleanupStack::PushL( msg );
    msg->SetInFilenameL( *iInFilename );

    CleanupStack::Pop( msg );
    NotifyUpperLayerLD( msg );

    if ( iReceivedMessage && iFileServe )
        {
        iFileServe->SetTotalSize( iReceivedMessage->ContentLengthL() );
        }
    }

// -----------------------------------------------------------------------------
// CUpnpHttpSession::NotifyUpperLayerL
//
// -----------------------------------------------------------------------------
//
void CUpnpHttpSession::NotifyUpperLayerLD( CUpnpHttpMessage* aMessage )
    {
    LOGS2( "%i, CUpnpHttpSession::NotifyUpperLayerL %i", iId,
            aMessage->Error( ) );
    CleanupStack::PushL( aMessage );

    aMessage->SetSessionId( iId );

    if ( iPendingRequest )
        {
        aMessage->SetPendingRequest( iPendingRequest );
        }
    aMessage->SetSenderPathL( *iSenderPath );

    if ( aMessage->Method().FindC( KHttp11WithoutSpace ) == 0
            || aMessage->Method().FindC( KHttp10 ) == 0 )
        {
        aMessage->SetDestinationPathL( *iDestinationPath );
        }
    else
        {
        TPtrC8 path = aMessage->SenderPathFromHeader();
        aMessage->SetDestinationPathL( path );
        }

    aMessage->SetType( iRequestType );
    aMessage->SetRetryCounter( iRetryCounter );

    iDisconnectNotification = EFalse;
    aMessage->SetOutFilenameL( *iOutFilename );
    CleanupStack::Pop( aMessage );
    iServer->ToReceiveStackD( aMessage );
    }

// -----------------------------------------------------------------------------
// CUpnpHttpSession::IsValidMessage
//
// -----------------------------------------------------------------------------
//
CUpnpHttpSession::THttpInvalidMessage CUpnpHttpSession::IsValidMessage(
    CUpnpHttpMessage* aMsg )
    {
    // Checking method. METHOD NAMES ARE CASE SENSITIVE,
    // so checking that method is OK.
    // http://www.w3.org/Protocols/HTTP/Methods.html
    if ( aMsg->Method().Find( KHttpGet() ) != 0 && aMsg->Method().Find(
        KHttpPost() ) != 0 && aMsg->Method().Find( KHttpHead() ) != 0
            && aMsg->Method().Find( KHttpOptions() ) != 0
            && aMsg->Method().Find( KHttpPut() ) != 0 && aMsg->Method().Find(
        KHttpDelete() ) != 0 && aMsg->Method().Find( KHttpTrace() ) != 0
            && aMsg->Method().Find( KHttpConnect() ) != 0
            && aMsg->Method().Find( KHttp11WithoutSpace() ) != 0
            && aMsg->Method().Find( KHttp10() ) != 0 && aMsg->Method().Find(
        UpnpGENA::KGenaSubscribe() ) != 0 && aMsg->Method().Find(
        UpnpGENA::KGenaUnSubscribe() ) != 0 && aMsg->Method().Find(
        UpnpGENA::KGenaNotify() ) != 0 )
        {
        return EUnknownMethod;
        }
    return EMessageOk;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpSession::FileAccess
//
// -----------------------------------------------------------------------------
//
CUpnpHttpFileAccess* CUpnpHttpSession::FileAccess()
    {
    return iFileServe;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpSession::OverwriteExisting
//
// -----------------------------------------------------------------------------
//
TBool CUpnpHttpSession::OverwriteExisting()
    {
    return iOverwriteExisting;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpSession::Offset
//
// -----------------------------------------------------------------------------
//
TInt CUpnpHttpSession::Offset()
    {
    return iOffset;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpSession::Length
//
// -----------------------------------------------------------------------------
//
TInt CUpnpHttpSession::Length()
    {
    return iLength;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpSession::SaveAtOffset
//
// -----------------------------------------------------------------------------
//
TBool CUpnpHttpSession::SaveAtOffset()
    {
    return iSaveAtOffset;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpSession::ServerPort
//
// -----------------------------------------------------------------------------
//
TInt CUpnpHttpSession::ServerPort()
    {
    return iServer->ServerPort();
    }

// -----------------------------------------------------------------------------
// CUpnpHttpSession::CheckConnectionClose
//
// -----------------------------------------------------------------------------
//
void CUpnpHttpSession::CheckConnectionCloseL()
    {
    LOGS1( "%i, CUpnpHttpSession::CheckConnectionCloseL", iId );

    if ( iReceivedMessage )
        {
        if ( iReceivedMessage->IsHeaderReady()
                && iReceivedMessage->UsesConnectionClose() )
            {
            TPtrC8 buffer = iReceivedMessage->Content();

            CUpnpHttpMessage* msg = NULL;
            TInt err;
                TRAP( err, msg = CUpnpHttpMessage::NewL( buffer, RemoteHost( ),
                                iId ) );
            CleanupStack::PushL( msg );
            // If parsing error occurs, or 'stop transfer' occurs during export from other MS to this MS
            // and the resource [file] isn't completely downloaded -> don't notify upper layers
            if ( err < KErrNone || (err == KErrNone
                    && (iReceivedMessage->Method().Find( KHttpPost ) == 0)
                    && !iReceivedMessage->IsReadyL()) )
                {
                LOGS1H(
                        iHandle,
                        "%i, CUpnpHttpSession::CheckConnectionCloseL - parsing error",
                        iId );

                iReceivedMessage->RemoveL( buffer.Length() );
                StartDisconnect();
                CleanupStack::PopAndDestroy( msg );
                return;
                }

            msg->SetInFilenameL( *iInFilename );
            if ( !iReceivedMessage->IsToFile()
                    || (iReceivedMessage->IsToFile() && iFileServe
                            && iFileServe->FileExist()) )
                {
                CleanupStack::Pop( msg );
                NotifyUpperLayerLD( msg );
                }
            else
                {
                CleanupStack::PopAndDestroy( msg );
                }
            iPendingRequest = NULL;
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpHttpSession::NotifyUpperLayersOnPostL
//
// -----------------------------------------------------------------------------
//
void CUpnpHttpSession::NotifyUpperLayersOnPostL()
    {
    CUpnpHttpMessage* msg = RUpnpHttpMessageFactory::HttpResponseOkL(
        RemoteHost() );

    CleanupStack::PushL( msg );

    CUpnpHttpHeaderList* List = NULL;
    List = msg->HeaderList();

    CUpnpHttpHeader* hdr = List->First();

    if(hdr != NULL)
	{
		HBufC8* newHeader = NULL;
		newHeader = HBufC8::NewLC( KTransferPending801().Length() );
		newHeader->Des().Zero();
		newHeader->Des().Append( KTransferPending801() );

		hdr->SetNameL( *newHeader );

		CleanupStack::PopAndDestroy( newHeader );
	}

    msg->SetOutFilenameL( *iOutFilename );
    CleanupStack::Pop( msg );
    NotifyUpperLayerLD( msg );
    }

// -----------------------------------------------------------------------------
// CUpnpHttpSession::PostingFileCompleteL
//
// -----------------------------------------------------------------------------
//
void CUpnpHttpSession::PostingFileCompleteL()
    {
    CUpnpHttpMessage* msg = RUpnpHttpMessageFactory::HttpResponseOkL(
        RemoteHost() );
    CleanupStack::PushL( msg );
    msg->SetOutFilenameL( *iOutFilename );

    CleanupStack::Pop( msg );
    NotifyUpperLayerLD( msg );
    }


// -----------------------------------------------------------------------------
//  CUpnpHttpSession::IsMsgReady
//
// -----------------------------------------------------------------------------
//
TBool CUpnpHttpSession::IsMsgReady()
    {
    TBool ret = EFalse;

    if ( iReceivedMessage )
        {
        TRAPD( err, ret = iReceivedMessage->IsReadyL( ) );

        if ( err != KErrNone )
            return EFalse;

        return ret;
        }
    else
        return EFalse;
    }


// -----------------------------------------------------------------------------
//  CUpnpHttpSession::HttpServer()
// Returns pointer to the CUpnpHttpServer object
// -----------------------------------------------------------------------------
//
CUpnpHttpServer* CUpnpHttpSession::HttpServer()
    {
    return iServer;
    }


// -----------------------------------------------------------------------------
//  CUpnpHttpSession::FileTransferReaderDoneL()
// Callback from file transfer reader
// -----------------------------------------------------------------------------
//
void CUpnpHttpSession::FileTransferReaderDoneL()
    {
    LOGS1( "%i, CUpnpHttpSession::FileTransferReaderDoneL", iId );
    //notify import completed to upper layers
    CUpnpHttpMessage* msg = RUpnpHttpMessageFactory::HttpResponseErrorL(
        iAddr, EHttp200Ok );
    CleanupStack::PushL( msg );
    msg->SetType( EImportComplete );
    msg->SetSessionId( Id() );
    //setting it only for a POST request
    msg->SetDestinationPathL( KNullDesC8 );
    CleanupStack::Pop( msg );
    iServer->ToReceiveStackD( msg );

    TPtrC8 headers = iReceivedMessage->Headers();
    //original message received from the remote MS
    CUpnpHttpMessage* original = CUpnpHttpMessage::NewL( (TDesC8&) headers,
        RemoteHost(), iId );
    CleanupStack::PushL( original );
    original->SetInFilenameL( *iInFilename );
    delete iFileServe;
    iFileServe = NULL;
    //notify POST is finished
    //prepare reply 200 Ok for the remote MS
    CUpnpHttpMessage* reply = RUpnpHttpMessageFactory::HttpResponseOkL(
        original );
    CleanupStack::PushL( reply );
    reply->AddPairL( UpnpHTTP::KHdrServer(), iServer->ServerDescription() );
    reply->AddPairL( UpnpHTTP::KConnection(), UpnpHTTP::KClose() );
    HBufC8* sendBuf = reply->ToStringL();
    CleanupStack::PopAndDestroy( reply );
    CleanupStack::Pop( original );
    //inform the upper layer
    iServer->ToReceiveStackD( original );
    CleanupStack::PushL( sendBuf );
    //send reply to the client
    WriteL( *sendBuf );
    CleanupStack::PopAndDestroy( sendBuf );

    ResetSettingsL();

    }

// -----------------------------------------------------------------------------
//  CUpnpHttpSession::FileTransferReaderDoneL()
// Callback from file transfer reader
// -----------------------------------------------------------------------------
//
void CUpnpHttpSession::FileTransferReaderErrorL( TInt aError )
    {
    LOGS1( "CUpnpHttpSession::FileTransferReaderErrorL %d", aError );
    if ( iFileServe )
        iFileServe->DeleteFile();
    NotifyErrorL( aError );
    if ( aError > 0 )
        aError = -aError;
    TInt httpError = MatchHttpError( aError );
    if ( httpError != KErrNone )
        {// If we act as a server side
        ResponseErrorL( httpError );
        ShutdownStop();
        return;
        }
    ResetSettingsL();
    }

// -----------------------------------------------------------------------------
//  CUpnpHttpSession::ResetSettingsL()
// Callback from file transfer reader
// -----------------------------------------------------------------------------
//
void CUpnpHttpSession::ResetSettingsL()

    {
    LOGS1( "%d, CUpnpHttpSession::ResetSettingsL", iId );
    iContinuedMessageDownload = EFalse;
    iHeadersCompleted = EFalse;

    delete iFileServe;
    iFileServe = NULL;

    delete iReceivedMessage;
    iReceivedMessage = NULL;

    delete iInFilename;
    iInFilename = NULL;
    iInFilename = HBufC8::NewMaxL( 0 );

    }

// -----------------------------------------------------------------------------
// CUpnpHttpSession::FileTransferWriterDoneL
//
// -----------------------------------------------------------------------------
//
void CUpnpHttpSession::FileTransferWriterDoneL()
    {

    LOGS1( "%i, CUpnpHttpSession::FileTransferWriterDoneL()", iId );

    CUpnpHttpMessage* msg = RUpnpHttpMessageFactory::HttpResponseErrorL(
        iAddr, EHttp200Ok );
    CleanupStack::PushL( msg );
    msg->SetType( EExportComplete );

    msg->SetSessionId( Id() );

    CleanupStack::Pop( msg );
    iServer->ToReceiveStackD( msg );

    delete iFileServe;
    iFileServe = NULL;

    iSessionNeedsErrorReply = EFalse;
    }

// -----------------------------------------------------------------------------
//  CUpnpHttpSession::FileTransferWriterHandleMoreL()
// Callback from file transfer writer
// -----------------------------------------------------------------------------
//
void CUpnpHttpSession::FileTransferWriterHandleMoreL()

    {
    LOGS1( "%i, CUpnpHttpSession::FileTransferWriterHandleMoreL()",
            iId );
    if ( FileTransferWriter() )
        FileTransferWriter()->Reset();

    if ( !IsPersistentConnection() )
        {
        StartClosingSession();
        return;
        }

    while ( iPendingRequests->Count() > 0 )
        {
        iSessionNeedsErrorReply = ETrue;

        CUpnpHttpMessage* request = (*iPendingRequests)[0];
        iPendingRequests->Remove( 0 );
        CleanupStack::PushL( request );
        TInt error = PrepareToServeFileL( request );
        CleanupStack::PopAndDestroy( request );

        if ( error )
            {
            NotifyErrorL( error );
            if ( error > 0 )
                error = -error;
            TInt httpError = MatchHttpError( error );
            if ( httpError != KErrNone )
                {// If we act as a server side
                if ( iSessionNeedsErrorReply )
                    {
                    ResponseErrorL( httpError );
                    }
                }

            if ( !IsPersistentConnection() )
                {
                StartClosingSession();
                }
            }
        else
            {
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
//  CUpnpHttpSession::CheckSessionPersistence
// Checks if message contains CONNECTION:CLOSE
// -----------------------------------------------------------------------------
//
void CUpnpHttpSession::CheckSessionPersistence( CUpnpHttpMessage* aMsg )
    {
    if ( aMsg->GetHeaderValue( UpnpHTTP::KConnection() ).FindC(
        UpnpHTTP::KClose() ) >= 0 || aMsg->RequestHTTPVersion() == KHttp10() )
        {
        iIsPersistent = EFalse;
        }
    else
        {
        iIsPersistent = ETrue;
        }

    if ( !IsPersistentConnection() )
        {
        iSessionKeepAlive = EFalse;
        }
    }

// -----------------------------------------------------------------------------
//  CUpnpHttpSession::DestroyIt()
//
// -----------------------------------------------------------------------------
//
void CUpnpHttpSession::DestroyIt( CUpnpTcpSession* aSession )
    {
    iServer->DeleteSession( aSession );
    }

// -----------------------------------------------------------------------------
//  CUpnpHttpSession::PrepareRangeHeaderL()
//
// -----------------------------------------------------------------------------
//
TInt CUpnpHttpSession::PrepareRangeHeaderL( CUpnpHttpMessage* aMsg,
    TBool aServingOutFileName, TInt aFileSize,
    CUpnpHttpServerTransaction &aTransaction, TInt& aRangeStatus, TInt& aStartPos, TInt& aEndPos )
    {
    // Generating headers
    aStartPos = KErrNotFound;
    aEndPos = KErrNotFound;

    // Check if there is a Range header in request and if it's correct
    CUpnpRangeHeaderParser* rangeParser = CUpnpRangeHeaderParser::NewLC(
        aTransaction, aStartPos, aEndPos );
    aRangeStatus = rangeParser->ParseRangeL( aMsg, aFileSize );
    CleanupStack::PopAndDestroy( rangeParser );

    if ( aRangeStatus != EHttpOk )
        {
        if ( aRangeStatus <= -EHttpBadRequest )
            {
            return aRangeStatus;
            }
        else if ( aRangeStatus == EHttpNoContent )
            {
            return KErrNone;
            }
        }
    else
        {
        // Serve whole file
        if ( !aServingOutFileName )
            {
            // If OutFilename() was set in msg
            TBuf8<2 * KMaxIntegerLength> sizeBuffer;
            sizeBuffer.Num( aFileSize );
            aTransaction.AddResponseHeaderL( KNullDesC8(), UpnpHTTP::KHTTPOk() );
            aTransaction.AddResponseHeaderL( UpnpHTTP::KHdrContentLength(), sizeBuffer );
            }
        else
            {
            // If file path was extracted from header
            HBufC8* tempBuffer = aMsg->HeadersToStringL();
            CleanupStack::PushL( tempBuffer );
            aTransaction.AddResponseHeaderL( KNullDesC8(), *tempBuffer );
            CleanupStack::PopAndDestroy( tempBuffer );
            }            
        }

    return  KErrNone;
    }

// -----------------------------------------------------------------------------
//  CUpnpHttpSession::PrepareRelativeUriL
//
// -----------------------------------------------------------------------------
//
TPtrC8 CUpnpHttpSession::PrepareRelativeUriL( const TDesC8& aUri )
    {
    TInetAddr resourceServerAddress;
    iServer->ServerAddress( resourceServerAddress );
    TInt portNo = iServer->ServerPort();
    
    TBuf<KMaxName> addrTmp;
    resourceServerAddress.Output( addrTmp );
    HBufC8* addr = UpnpString::FromUnicodeL( addrTmp );
    CleanupStack::PushL( addr );
    
    TBuf8<KMaxName> port;
    port.Num( portNo );
    
    TInt addrLength = UpnpHTTP::KHTTPUrl().Length() +  addr->Length( ) + UpnpString::KColon().Length( ) + port.Length( );
    HBufC8* temp = HBufC8::NewLC( addrLength );
    TPtr8 tempPtr( temp->Des() );
    tempPtr.Append(  UpnpHTTP::KHTTPUrl );
    tempPtr.Append( *addr );
    tempPtr.Append( UpnpString::KColon );
    tempPtr.Append( port );

    CleanupStack::Pop( temp );
    CleanupStack::PopAndDestroy( addr );
    
    if ( aUri.Find( *temp ) == 0  )
        {
        delete temp;
        return aUri.Mid( addrLength );
        }
    
    delete temp;
    return aUri; 
    }

// End Of File
