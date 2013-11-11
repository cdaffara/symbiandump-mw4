/** @file
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Definition of the CUpnpHttpMessageSender class
*
*/


#include <upnphttpmessage.h>
#include <upnplist.h>
#include <upnpcons.h>
#include <upnpstring.h>

#include "upnphttpmessagesender.h"
#include "upnphttpmessagesenderowner.h"
#include "upnphttptransaction.h"

#ifdef _DEBUG
#define KLogFile _L("HttpClientEngine.txt")
#endif
#include "upnpcustomlog.h"

// ----------------------------------------------------------------------------
// CUpnpHttpMessageSender::NewL
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CUpnpHttpMessageSender* CUpnpHttpMessageSender::NewL( CUpnpHttpTransaction& aUpnpTransaction,
    RHTTPSession aSession, MUpnpHttpMessageSenderOwner& aOwner )
    {
    CUpnpHttpMessageSender* self = CUpnpHttpMessageSender::NewLC( aUpnpTransaction,
                                                                  aSession, aOwner );
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CUpnpHttpMessageSender::NewLC
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CUpnpHttpMessageSender* CUpnpHttpMessageSender::NewLC( CUpnpHttpTransaction& aUpnpTransaction,
    RHTTPSession aSession, MUpnpHttpMessageSenderOwner& aOwner )
    {
    CUpnpHttpMessageSender* self = new (ELeave) CUpnpHttpMessageSender( aUpnpTransaction,
                                                                        aSession, aOwner );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// CUpnpHttpMessageSender::CUpnpHttpMessageSender
// Constructor.
// ----------------------------------------------------------------------------
//
CUpnpHttpMessageSender::CUpnpHttpMessageSender( CUpnpHttpTransaction& aUpnpTransaction,
    RHTTPSession aSession, MUpnpHttpMessageSenderOwner& aOwner ) :
    iUpnpTransaction( aUpnpTransaction ),
    iSession( aSession ),
    iOwner( aOwner )
    {
    }

// ----------------------------------------------------------------------------
// CUpnpHttpMessageSender::ConstructL
// Constructor of CUpnpHttpMessageSender
// ----------------------------------------------------------------------------
//
void CUpnpHttpMessageSender::ConstructL()
    {
    iTimer = CUpnpNotifyTimer::NewL( this );
    }

// ----------------------------------------------------------------------------
// CUpnpHttpMessageSender::~CUpnpHttpMessageSender
// Destructor.
// ----------------------------------------------------------------------------
//
CUpnpHttpMessageSender::~CUpnpHttpMessageSender()
    {
    delete iBody;
    delete iTimer;
    iTransaction.Close();
    }

// ----------------------------------------------------------------------------
// CUpnpHttpMessageSender::UpnpTransaction
// Returns upnp transaction object that is maintained by the object.
// ----------------------------------------------------------------------------
//
CUpnpHttpTransaction& CUpnpHttpMessageSender::UpnpTransaction()
    {
    return iUpnpTransaction;
    }

// ----------------------------------------------------------------------------
// CUpnpHttpMessageSender::StartTransactionL
// Start a new HTTP transaction maintained by the object. It will start
// asynchronous sending of request message.
// ----------------------------------------------------------------------------
//
void CUpnpHttpMessageSender::StartTransactionL()
    {
    ASSERT( !iTimer->IsActive() && NULL == iBody ); //check if this method is called only once
    LOG_FUNC_NAME;

    PrepareRequestTransactionL();
    PrepareRequestHeadersL();
    PrepareRequestBody();
    StartRequestTimer();
    iTransaction.SubmitL();

    ASSERT( iTimer->IsActive() );   //timer is set
    }

// ----------------------------------------------------------------------------
// CUpnpHttpMessageSender::PrepareRequestTransactionL
// Opens RHTTPTransaction with request method and uri, subscribe
// for transaction's events
// ----------------------------------------------------------------------------
//
void CUpnpHttpMessageSender::PrepareRequestTransactionL()
    {
    HBufC8* uri = DestinationUriL( iUpnpTransaction.Request() );
    CleanupStack::PushL( uri );
    TUriParser8 uriParser;
    uriParser.Parse( *uri );
    RStringF method = iSession.StringPool().OpenFStringL( iUpnpTransaction.Request()->Method() );
    CleanupClosePushL( method );
    iTransaction = iSession.OpenTransactionL( uriParser, *this, method );
    CleanupStack::PopAndDestroy( &method );
    //synchronize ids of both transactions
    iUpnpTransaction.Request()->SetSessionId( iTransaction.Id() );
    CleanupStack::PopAndDestroy( uri );
    }

// ----------------------------------------------------------------------------
// CUpnpHttpMessageSender::PrepareRequestHeadersL
// Sets http headers from UpnpHttpTransaction to RHTTPTransaction
// ----------------------------------------------------------------------------
//
void CUpnpHttpMessageSender::PrepareRequestHeadersL()
    {
    RHTTPHeaders hdr = iTransaction.Request().GetHeaderCollection();
    hdr.RemoveAllFields();
    CUpnpHttpHeaderList& headerList =  *(iUpnpTransaction.Request()->HeaderList());
    CUpnpHttpHeader* upnphdr = headerList.First();
    upnphdr = headerList.Next(upnphdr);    // first header is a method
    while ( upnphdr )
        {
        RStringF valStr = iSession.StringPool().OpenFStringL( upnphdr->Value() );
        CleanupClosePushL( valStr );
        RStringF namStr = iSession.StringPool().OpenFStringL( upnphdr->Name() );
        CleanupClosePushL( namStr );

        hdr.SetFieldL( namStr, THTTPHdrVal( valStr ) );
        upnphdr = headerList.Next(upnphdr);

        CleanupStack::PopAndDestroy( &namStr );
        CleanupStack::PopAndDestroy( &valStr );
        }
    }

// ----------------------------------------------------------------------------
// CUpnpHttpMessageSender::CopyResponseHeadersL
// Copies http headers from RHTTPTransaction to UpnpHttpTransaction
// ----------------------------------------------------------------------------
//
void CUpnpHttpMessageSender::CopyResponseHeadersL()
    {
    CUpnpHttpMessage* msg = iUpnpTransaction.Response();
    RStringPool strPool = iSession.StringPool();
    RHTTPHeaders headers = iTransaction.Response().GetHeaderCollection();

    THTTPHdrFieldIter iter = headers.Fields();
    while ( !iter.AtEnd() )
        {
        RStringTokenF fieldName = iter();
        RStringF fieldNameStr = strPool.StringF( fieldName );
        const TDesC8& fieldNameDesC = fieldNameStr.DesC();

        TPtrC8 rawFieldData;
        if ( headers.GetRawField( fieldNameStr, rawFieldData ) == KErrNone )
            {
            msg->AddPairL( fieldNameDesC, rawFieldData );
            }

        ++iter;
        }
    }

// ----------------------------------------------------------------------------
// CUpnpHttpMessageSender::PrepareRequestBodyL
// Sets http message body from UpnpHttpTransaction to RHTTPTransaction
// ----------------------------------------------------------------------------
//
void CUpnpHttpMessageSender::PrepareRequestBody()
    {
    if ( iUpnpTransaction.Request()->Method().Compare( KHttpPost ) == 0 
       || ( iUpnpTransaction.Request()->Method().Compare( UpnpGENA::KGenaNotify ) == 0 ) )
        {
        iTransaction.Request().SetBody( *this );
        }
    }

// ----------------------------------------------------------------------------
// CUpnpHttpMessageSender::StartRequestTimer
// Starts timer of request sending with TcpTimeout value.
// ----------------------------------------------------------------------------
//
void CUpnpHttpMessageSender::StartRequestTimer()
    {
    iTimer->After( iUpnpTransaction.Request()->TcpTimeout(), EFalse );
    }

// ----------------------------------------------------------------------------
// CUpnpHttpMessageSender::TimerEventL
// From MNotifyTimerObserver function which indicate that request timeout
// expired, so transaction failed.
// ----------------------------------------------------------------------------
//
void CUpnpHttpMessageSender::TimerEventL( CUpnpNotifyTimer* /*aTimer*/ )
    {
    LOG_FUNC_NAME;
    TransactionFailed( EHttpRequestTimeout );
    }

// ----------------------------------------------------------------------------
// CUpnpHttpMessageSender::MHFRunL
// Called by Symbian OS HTTP client framework to notify about transaction events.
// ----------------------------------------------------------------------------
//
void CUpnpHttpMessageSender::MHFRunL( RHTTPTransaction aTransaction, const THTTPEvent& aEvent )
    {
    ASSERT( aTransaction == iTransaction );
    switch ( aEvent.iStatus )
        {
        case THTTPEvent::EGotResponseHeaders:
            {
            LOGS1( "THTTPEvent::EGotResponseHeaders trans id: %d",
                aTransaction.Id() );
            }
            break;
        case THTTPEvent::EGotResponseBodyData:
            {
            GotResponseBodyDataL( *(aTransaction.Response().Body()) );
            }
            break;
        case THTTPEvent::EResponseComplete:
            {
            LOGS( "THTTPEvent::EResponseComplete");
            // Indicates that header & body of response is completely received.
            }
            break;
        //note: there is a guarantee that THTTPEvent::ESucceeded XOR THTTPEvent::EFailed
        //event will be provided, and it will be the last event for a transaction
        //so ESucceeded and EFailed are the only legal states
        //in which we can call TransactionSucceeded/TransactionFailed
        case THTTPEvent::ESucceeded:
            {
            LOGS( "THTTPEvent::ESucceeded");
            TransactionSucceeded();
            }
            break;
        case THTTPEvent::EFailed:
            {
            LOGS( "THTTPEvent::EFailed");
            if ( KErrNone != iCurrentErrorNumber )
                {
                TransactionFailed( EHttpRequestTimeout, iCurrentErrorNumber );
                }
            else
                {
                TransactionFailed( aTransaction.Response().StatusCode() );
                }
            }
            break;
        default:
            // There are more events in THTTPEvent, but they are not usually
            // needed. However, event status smaller than zero should be handled
            // correctly since it's error.
            {
            LOGS1( "Other THTTPEvent::%d", aEvent.iStatus );
            if ( aEvent.iStatus < 0 )
                {
                iCurrentErrorNumber = aEvent.iStatus;
                }
            }
            break;
        }
    }

// ----------------------------------------------------------------------------
// CUpnpHttpMessageSender::MHFRunError
// Called by Symbian OS HTTP client framework when *leave* occurs in handling
// of transaction event
// ----------------------------------------------------------------------------
//
TInt CUpnpHttpMessageSender::MHFRunError( TInt aError,
                          RHTTPTransaction /*aTransaction*/,
                          const THTTPEvent& /*aEvent*/ )
    {
    LOGS1( "Run error %d", aError );
    //in case of serious errors we are receiving here we have to notify
    //error immediatly (we probably won't receive THTTPEvent::EFailed)
    TransactionFailed( EHttpRequestTimeout, aError );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CUpnpHTTPMessageSender::TransactionSucceededL
// Called when transaction succeeded.
// Closes RHTTPTransaction, and forward result message to observer.
// -----------------------------------------------------------------------------
void CUpnpHttpMessageSender::TransactionSucceeded()
    {
    //there is nothing we can do about error during notification upper layer
    //except retry (risky), or ignore
    TRAP_IGNORE( DoTransactionSucceededL() );
    }

// -----------------------------------------------------------------------------
// CUpnpHttpMessageSender::TransactionFailedL
// Called in case of every fail of transaction.
// Closes RHTTPTransaction, creates error message with given status, and forward it
// to observer.
// -----------------------------------------------------------------------------
void CUpnpHttpMessageSender::TransactionFailed( TInt aStatus, TInt aError )
    {
    //see comment in TransactionSucceeded
    TRAP_IGNORE( DoTransactionFailedL( aStatus, aError ) );
    }

// -----------------------------------------------------------------------------
// CUpnpHTTPMessageSender::DoTransactionSucceededL
// Internal, leaving code to notify that transaction succeeded
// Do NOT use this method directly, but via TransactionSucceded
// -----------------------------------------------------------------------------
void CUpnpHttpMessageSender::DoTransactionSucceededL()
    {
    LOG_FUNC_NAME;
    iTimer->Cancel();
    iUpnpTransaction.CreateOkResponseL( iBody ? *iBody : KNullDesC8() );
    CopyResponseHeadersL(); //to copy SID from service subscription http response 
    iTransaction.Close();
    delete iBody;
    iBody = NULL;
    iOwner.SenderFinishedLD( this );    
    }

// -----------------------------------------------------------------------------
// CUpnpHTTPMessageSender::DoTransactionFailedL
// Internal, leaving code to notify that transaction failed
// Do NOT use this method directly, but via TransactionFailed
// -----------------------------------------------------------------------------
void CUpnpHttpMessageSender::DoTransactionFailedL( TInt aStatus, TInt aError )
    {
    LOG_FUNC_NAME;
    iTimer->Cancel();
    iTransaction.Close();
    if ( KErrNone != aError ) //in case of internal symbian error colected body isn't meaningful
        {
        delete iBody;
        iBody = NULL;
        }    
    iUpnpTransaction.CreateFaultResponseL( iBody ? *iBody : KNullDesC8(),
                                               aStatus, 
                                               aError );
    delete iBody;
    iBody = NULL;
    iOwner.SenderFinishedLD( this );    
    }

// -----------------------------------------------------------------------------
// CUpnpHttpMessageSender::GotResponseBodyDataL
// Called when transaction got another part of body data.
// Data is concatenated to iBody and when the last part of body is received
// request message is created witin transaction.
// -----------------------------------------------------------------------------
void CUpnpHttpMessageSender::GotResponseBodyDataL(
    MHTTPDataSupplier& aResponseBodySupplier )
    {
    LOG_FUNC_NAME;

    TPtrC8 dataChunk;
    aResponseBodySupplier.GetNextDataPart( dataChunk );
    
    if (!iBody)
        {
        iBody = dataChunk.AllocL();
        }
    else
        {
        iBody = iBody->ReAllocL( iBody->Length() + dataChunk.Length() );
        iBody->Des().Append( dataChunk );
        }

    aResponseBodySupplier.ReleaseData();    
    }

// -----------------------------------------------------------------------------
// CUpnpHTTPMessageSender::DestinationUriL
// Helper method that allocate descriptor object with destination uri of message
// passed as a parameter.
// -----------------------------------------------------------------------------
HBufC8* CUpnpHttpMessageSender::DestinationUriL( CUpnpHttpMessage* aMessage )
    {
    TInetAddr add( aMessage->Receiver() );
    HBufC8* address = UpnpString::InetToStringL( add );
    CleanupStack::PushL( address );
    TPtrC8 path( aMessage->SenderPathFromHeader() ) ;
    HBufC8* uriBuf = HBufC8::NewL(
        UpnpHTTP::KHTTPUrl().Length() + address->Length() + path.Length() );
    TPtr8 uri( uriBuf->Des() );
    uri.Append( UpnpHTTP::KHTTPUrl );
    uri.Append( *address );
    uri.Append( path );
    CleanupStack::PopAndDestroy( address );
    return uriBuf;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpMessageSender::GetNextDataPart
// Method from MHTTPDataSupplier used to supply body of request from
// our UpnpMessage to Symian RHTTPRequest
// -----------------------------------------------------------------------------
TBool CUpnpHttpMessageSender::GetNextDataPart( TPtrC8& aDataChunk )
    {
    aDataChunk.Set( iUpnpTransaction.Request()->Body() );
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpMessageSender::ReleaseData()
// Method from from MHTTPDataSupplier to supply body of request from
// our UpnpMessage to Symian RHTTPRequest
// -----------------------------------------------------------------------------
void CUpnpHttpMessageSender::ReleaseData()
    {
    }

// -----------------------------------------------------------------------------
// CUpnpHttpMessageSender::OverallDataSize
// Method from MHTTPDataSupplier used to supply body of request from
// our UpnpMessage to Symian RHTTPRequest
// -----------------------------------------------------------------------------
TInt CUpnpHttpMessageSender::OverallDataSize()
    {
    return iUpnpTransaction.Request()->Body().Length();
    }

// -----------------------------------------------------------------------------
// CUpnpHttpMessageSender::Reset
// Method from MHTTPDataSupplier used to supply body of request from
// our UpnpMessage to Symian RHTTPRequest
// -----------------------------------------------------------------------------
TInt CUpnpHttpMessageSender::Reset()
    {
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CUpnpHttpMessageSender::CancelTransaction
// Cancels transaction
// ----------------------------------------------------------------------------
//
void CUpnpHttpMessageSender::CancelTransaction()
    {
    LOG_FUNC_NAME;
    iTransaction.Cancel();
    iTimer->Cancel();
    TransactionFailed( EHttpRequestTimeout );//no need to wait, notify timeout
    }

//end of file
