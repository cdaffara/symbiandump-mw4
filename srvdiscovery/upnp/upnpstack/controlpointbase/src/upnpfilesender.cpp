/** @file
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  CUpnpFileSender implementation
 *
 */

#include "upnpfilesender.h"
#include "httpuploader.h"
#include "upnphttpmessage.h"
#include "upnpstring.h"
#include "upnpcons.h"
#include "upnphttpmessagefactory.h"
#include "upnpcontrolpoint.h"

// --------------------------------------------------------------------------
// CUpnpFileSender::CUpnpFileSender
// --------------------------------------------------------------------------
//
CUpnpFileSender::CUpnpFileSender( CUpnpControlPoint& aCPToNotifyOfSendingResult )
    : iControlPoint( aCPToNotifyOfSendingResult )
    {
    // No implementation required
    }

// --------------------------------------------------------------------------
// CUpnpFileSender::~CUpnpFileSender
// --------------------------------------------------------------------------
//
CUpnpFileSender::~CUpnpFileSender()
    {
    delete iUploader;
    }

// --------------------------------------------------------------------------
// CUpnpFileSender::NewLC
// --------------------------------------------------------------------------
//
CUpnpFileSender* CUpnpFileSender::NewLC( CUpnpControlPoint& aCPToNotifyOfSendingResult )
    {
    CUpnpFileSender* self = new (ELeave) CUpnpFileSender( aCPToNotifyOfSendingResult );
    CleanupStack::PushL( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUpnpFileSender::NewL
// --------------------------------------------------------------------------
//
CUpnpFileSender* CUpnpFileSender::NewL( CUpnpControlPoint& aCPToNotifyOfSendingResult )
    {
    CUpnpFileSender* self = CUpnpFileSender::NewLC( aCPToNotifyOfSendingResult );
    CleanupStack::Pop(self);
    return self;
    }

// --------------------------------------------------------------------------
// DestinationUriL
// Allocate destination uri from message
// --------------------------------------------------------------------------
//
static HBufC8* DestinationUriL( CUpnpHttpMessage& aMessage )
    {
    TInetAddr add( aMessage.Receiver() );
    HBufC8* address = UpnpString::InetToStringL( add );
    CleanupStack::PushL( address );
    TPtrC8 path( aMessage.SenderPathFromHeader() ) ;
    HBufC8* uriBuf = HBufC8::NewL(
        UpnpHTTP::KHTTPUrl().Length() + address->Length() + path.Length() );
    TPtr8 uri( uriBuf->Des() );
    uri.Append( UpnpHTTP::KHTTPUrl );
    uri.Append( *address );
    uri.Append( path );
    CleanupStack::PopAndDestroy( address );
    return uriBuf;
    }

// --------------------------------------------------------------------------
// CUpnpFileSender::SendL
// --------------------------------------------------------------------------
//
void CUpnpFileSender::SendL( CUpnpHttpMessage& aMessageToSend )
    {
    ASSERT( aMessageToSend.Method() == KHttpPost && aMessageToSend.OutFilename().Length() > 0 );
    if ( !iUploader )   //late initialisation
        {
        const TInt KUnused(0);
        const TUint KBufferSize( 10 * 1024 );
        const TUint KParallelTransfersNumber( 1 );
        iUploader = CHttpUploader::NewL(*this,KUnused,KBufferSize,KParallelTransfersNumber);
        }

    RBuf fileName;
    User::LeaveIfError( fileName.Create( aMessageToSend.OutFilename().Length() ) );
    CleanupClosePushL( fileName );
    fileName.Copy( aMessageToSend.OutFilename() );
    HBufC8* destUri = DestinationUriL( aMessageToSend );
    CleanupStack::PushL( destUri );

    iUploader->UploadFileL( *destUri, fileName, (TAny*)aMessageToSend.SessionId() );

    CleanupStack::PopAndDestroy( destUri );
    CleanupStack::PopAndDestroy( &fileName );
    }

// --------------------------------------------------------------------------
// CUpnpFileSender::TransferProgress
// --------------------------------------------------------------------------
//
void CUpnpFileSender::TransferProgress( TAny* /*aKey*/,
                               TInt /*aBytes*/,
                               TInt /*aTotalBytes*/ )
     {
     // no implementation needed
     }

// --------------------------------------------------------------------------
// CUpnpFileSender::ReadyForTransferL
// --------------------------------------------------------------------------
//
void CUpnpFileSender::ReadyForTransferL( TAny* aKey )
    {
    _LIT8( KTextHtmlMimeType, "text/html" );
    iUploader->SetHeaderL( aKey, UpnpHTTP::KHdrContentType, KTextHtmlMimeType );
    iUploader->SetHeaderL( aKey, UpnpHTTP::KConnection, UpnpHTTP::KClose );
    iUploader->StartTransferL( aKey );
    }

// --------------------------------------------------------------------------
// CUpnpFileSender::TransferCompleted
// --------------------------------------------------------------------------
//
void CUpnpFileSender::TransferCompleted( TAny* /*aKey*/, TInt aStatus )
    {
    CUpnpHttpMessage* rsp = NULL;
    TRAP_IGNORE(
        rsp = RUpnpHttpMessageFactory::HttpResponseErrorL( TInetAddr(), aStatus ) );
    iControlPoint.HttpResponseReceived( rsp );
    delete rsp;
    }
