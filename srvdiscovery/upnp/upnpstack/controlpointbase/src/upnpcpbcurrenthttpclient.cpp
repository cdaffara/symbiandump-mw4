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
* Description:  CUpnpCpbCurrentHttpClient
 *
*/

#include <e32base.h>
#include <e32cons.h>
#include <e32std.h>
#include <upnpsettings.h>
#include <charconv.h>
#include <utf.h>

#include "upnpcpstackrequestor.h"
#include "upnphttpmessage.h"
#include "upnpsoapmessage.h"
#include "upnpicon.h"
#include "upnpdevice.h"
#include "upnpstring.h"
#include "upnphttpmessagefactory.h"
#include "upnpsoapmessagefactory.h"
#include "upnpgenamessagefactory.h"
#include "upnpserviceinfo.h"
#include "upnpcpbhttpmessagecontroller.h"
#include "upnpcpbcurrenthttpclient.h"
#include "upnpcontrolpoint.h"
#include "upnpcommonupnplits.h"
#include "upnpcpbdevicedescription.h"
#include "upnpdevicedescriptionhttptransaction.h"
#include "upnpservicedescriptionhttptransaction.h"
#include "upnpsoapactionhttptransaction.h"
#include "upnpservicesubscriptionhttptransaction.h"
#include "upnpserviceunsubscriptionhttptransaction.h"
#include "upnpcphttprequestorimpl.h"
#include "upnpcpstackrequestorimpl.h"
#include "upnpfilesender.h"

#define KLogFile _L("UPnPControlPoint.txt")
#include <upnpcustomlog.h>

using namespace UpnpHTTP;

static const TInt KMessageTimeout( 10000000 );
static const TInt KActionTimeout( 35000000 );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpCpbCurrentHttpClient::CUpnpCurrentHttpClient
// C++ default constructor
// -----------------------------------------------------------------------------
//
CUpnpCpbCurrentHttpClient::CUpnpCpbCurrentHttpClient( CUpnpControlPoint& aControlPoint,
        MUpnpCpHttpRequestor* aHttpRequestor, MUpnpCpStackRequestor* aStackRequestor )
    : iControlPoint( aControlPoint ),
    iHttpRequestor( aHttpRequestor ), iStackRequestor( aStackRequestor )
    {
    }

// -----------------------------------------------------------------------------
// CUpnpCpbCurrentHttpClient::~CUpnpCpbCurrentHttpClient
// Destructor
// -----------------------------------------------------------------------------
//
CUpnpCpbCurrentHttpClient::~CUpnpCpbCurrentHttpClient()
    {
    if ( iStackRequestor )
        {
        iStackRequestor->RemoveCustomer( *this );
        }
    iPendingHttpMessages.ResetAndDestroy();
    iPendingTransactions.ResetAndDestroy();
    delete iUserAgent;
    delete iHttpRequestorDefaultImpl;
    delete iStackRequestorDefaultImpl;
    delete iFileSender;
    }

// -----------------------------------------------------------------------------
// CUpnpCpbCurrentHttpClient::NewL
//
// -----------------------------------------------------------------------------
//
CUpnpCpbCurrentHttpClient* CUpnpCpbCurrentHttpClient::NewL(
    CUpnpControlPoint& aControlPoint,
    MUpnpCpHttpRequestor* aHttpRequestor, MUpnpCpStackRequestor* aStackRequestor)
    {
    LOG_FUNC_NAME;
    CUpnpCpbCurrentHttpClient* self =
        new (ELeave) CUpnpCpbCurrentHttpClient( aControlPoint, aHttpRequestor, aStackRequestor );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpCpbCurrentHttpClient::ConstructL
//
// -----------------------------------------------------------------------------
//
void CUpnpCpbCurrentHttpClient::ConstructL()
    {
    iFileSender = CUpnpFileSender::NewL( iControlPoint );
    
    if ( !iHttpRequestor )
        {
        iHttpRequestorDefaultImpl = CUpnpCpHttpRequestorImpl::NewL();
        iHttpRequestor = iHttpRequestorDefaultImpl;
        }
    iHttpRequestor->InitialiseL( *this, CUpnpSettings::GetIapL() );

    if ( !iStackRequestor )
        {
        iStackRequestorDefaultImpl = CUpnpCpStackRequestorImpl::NewL( iControlPoint );
        iStackRequestor = iStackRequestorDefaultImpl;
        }
    iStackRequestor->AddCustomerL( *this );
    ASSERT( iHttpRequestor && iStackRequestor ); //ensure correct initialisation
    }

// -----------------------------------------------------------------------------
// CUpnpCpbCurrentHttpClient::SendActionL
//
// -----------------------------------------------------------------------------
//
void CUpnpCpbCurrentHttpClient::SendActionL( CUpnpAction* aAction )
    {
    if( !aAction )
        {
        User::Leave( KErrArgument );
        }

    CUpnpHttpMessage* msg = RUpnpSoapMessageFactory::SoapRequestL( aAction );
    CleanupStack::PushL( msg );

    // 7.2.32.1 UPnP action requests (sent by a control point) must include a
    // DLNA-CP-version in a USER-AGENT HTTP header value.
    if ( !iUserAgent )
        {
        HBufC8* userAgent = CUpnpSettings::GetUserAgentL();
        CleanupStack::PushL( userAgent );

        iUserAgent = HBufC8::NewL( userAgent->Length() );
        iUserAgent->Des().Append( *userAgent );

        CleanupStack::PopAndDestroy( userAgent );
        }

    msg->AddPairL( UpnpHTTP::KHdrUserAgent, *iUserAgent );

    msg->SetTcpTimeout( KActionTimeout );
    
    CleanupStack::Pop( msg );//ownership passed to Transaction and is safe
    CUpnpHttpTransaction* soapTransaction = 
         CUpnpSoapActionHttpTransaction::NewLC( msg, aAction->SessionId(), iControlPoint );
                    
    iPendingTransactions.AppendL( soapTransaction );
    CleanupStack::Pop( soapTransaction );
    iHttpRequestor->SendL( *soapTransaction );
    
    }

// -----------------------------------------------------------------------------
// CUpnpCpbCurrentHttpClient::SendSubscribeL
//
// -----------------------------------------------------------------------------
//
void CUpnpCpbCurrentHttpClient::SendSubscribeL( CUpnpServiceInfo* aServiceInfo )
    {
    const TDesC8& path = Path();

    TInetAddr addr = iStackRequestor->HttpServerAddress();
    HBufC8* longPath = aServiceInfo->ConstructSubscribtionPathLC( path , addr ); 
    CUpnpGenaMessage* gena = aServiceInfo->CreateSubscriptionMessageL(
                                   *longPath,
                                   ESubscription );
    
    gena->SetTcpTimeout( KMessageTimeout );
    CUpnpServiceSubscriptionHttpTransaction* transaction =
        CUpnpServiceSubscriptionHttpTransaction::NewLC( gena, iControlPoint );
    iPendingTransactions.AppendL( transaction );
    CleanupStack::Pop( transaction );
    iHttpRequestor->SendL( *transaction );
    CleanupStack::PopAndDestroy( longPath );
    }

// -----------------------------------------------------------------------------
// CUpnpCpbCurrentHttpClient::SendUnsubscribeL
//
// -----------------------------------------------------------------------------
//
void CUpnpCpbCurrentHttpClient::SendUnsubscribeL( CUpnpServiceInfo* aServiceInfo )
    {
    if ( aServiceInfo->Sid() != KNullDesC8 ) 
        {
        CUpnpGenaMessage* gena = aServiceInfo->CreateSubscriptionMessageL( EUnSubscription );
        gena->SetTcpTimeout( KMessageTimeout );
        CUpnpServiceUnsubscriptionHttpTransaction* transaction = 
            CUpnpServiceUnsubscriptionHttpTransaction::NewLC( gena, iControlPoint) ;
        iPendingTransactions.AppendL( transaction );
        CleanupStack::Pop( transaction );
        iHttpRequestor->SendL( *transaction );
        }
    }

// -----------------------------------------------------------------------------
// CUpnpCpbCurrentHttpClient::SendResubscribeL
//
// -----------------------------------------------------------------------------
//
void CUpnpCpbCurrentHttpClient::SendResubscribeL( CUpnpServiceInfo* aServiceInfo )
    {
    CUpnpGenaMessage* gena = aServiceInfo->CreateSubscriptionMessageL( EResubscription );
    gena->SetTcpTimeout( KMessageTimeout );
    CUpnpServiceSubscriptionHttpTransaction* transaction =
         CUpnpServiceSubscriptionHttpTransaction::NewLC( gena, iControlPoint );
    iPendingTransactions.AppendL( transaction );
    CleanupStack::Pop( transaction );
    iHttpRequestor->SendL( *transaction );
    }

// -----------------------------------------------------------------------------
// CUpnpCpbCurrentHttpClient::GetServiceDescriptionL
//
// -----------------------------------------------------------------------------
//
TInt CUpnpCpbCurrentHttpClient::GetServiceDescriptionL( CUpnpDevice* aDevice,
    const TDesC8& aUrl )
    {
    CUpnpHttpMessage* result( NULL );
    TPtrC8 urlBase( aDevice->UrlBase() );

    if ( aUrl.Find( KHttp ) == 0 )
        {
        result = RUpnpHttpMessageFactory::HttpGetL( aUrl );
        }
    else
        {
        TInetAddr addr = aDevice->Address();
        if ( urlBase.Length() > 0 )
            {
            HBufC8* url = aDevice->ConcatWithUrlBaseL( aUrl );
            CleanupStack::PushL( url );

            if ( url->Find( KHttp ) )
                {
                result = RUpnpHttpMessageFactory::HttpGetL( addr, *url,
                    KNullDesC8 );
                }
            else
                {
                result = RUpnpHttpMessageFactory::HttpGetL( *url );
                }

            CleanupStack::PopAndDestroy( url );
            }
        else
            {
            result = RUpnpHttpMessageFactory::HttpGetL( addr,
                aDevice->DescriptionUrlPath(), aUrl );
            }
        }
    result->SetTcpTimeout( KMessageTimeout );

    CUpnpServiceDescriptionHttpTransaction* transaction =
        CUpnpServiceDescriptionHttpTransaction::NewLC( result, iControlPoint );

    iPendingTransactions.AppendL( transaction );
    CleanupStack::Pop( transaction );
    iHttpRequestor->SendL( *transaction );
    return result->SessionId();
    }

// -----------------------------------------------------------------------------
// CUpnpCpbCurrentHttpClient::GetDeviceDescriptionL
//
// -----------------------------------------------------------------------------
//
TInt CUpnpCpbCurrentHttpClient::GetDeviceDescriptionL( CUpnpDevice* aDevice )
    {
    TInetAddr addr = aDevice->DescriptionUrlAddressL();
    addr.SetPort( aDevice->DescriptionUrlPort() );

    CUpnpHttpMessage* message = RUpnpHttpMessageFactory::HttpGetL( addr,
        aDevice->DescriptionUrlPath() );
    message->SetTcpTimeout( KMessageTimeout );

    CUpnpDeviceDescriptionHttpTransaction* transaction =
        CUpnpDeviceDescriptionHttpTransaction::NewLC( message, iControlPoint );

    iPendingTransactions.AppendL( transaction );
    CleanupStack::Pop( transaction );
    iHttpRequestor->SendL( *transaction );

    return message->SessionId();
    }

// -----------------------------------------------------------------------------
// CUpnpCpbCurrentHttpClient::MessageReceivedLD
//
// -----------------------------------------------------------------------------
//
void CUpnpCpbCurrentHttpClient::MessageReceivedLD( CUpnpHttpMessage* aHttpMessage )
    {
    if ( !aHttpMessage )
        {
        User::Leave( KErrArgument );
        }
    // match arrived response to pending request

    TIdentityRelation<CUpnpHttpMessage> matcher( CUpnpHttpMessage::SessionIdMatch );
    TInt idx = iPendingHttpMessages.Find ( aHttpMessage, matcher );

    CleanupStack::PushL( aHttpMessage );

    if ( aHttpMessage->IsGena() )
        {
        CleanupStack::Pop( aHttpMessage );
        iControlPoint.NotifyReceivedL( *aHttpMessage );
        }
    else
        {
        iControlPoint.MessageReceived( aHttpMessage );
        iPendingHttpMessages.Compress();
        CleanupStack::PopAndDestroy( aHttpMessage );
        }
    }

// -----------------------------------------------------------------------------
// CUpnpCpbCurrentHttpClient::Path
//
// -----------------------------------------------------------------------------
//
const TDesC8& CUpnpCpbCurrentHttpClient::Path()
    {
    return KPath();
    }

// ----------------------------------------------------------------------------
// CUpnpCpbCurrentHttpClient::StopIgnoringL
//
// -----------------------------------------------------------------------------
//
void CUpnpCpbCurrentHttpClient::StopIgnoringL( CUpnpDevice* aDevice )
    {
    iStackRequestor->StopIgnoringL( aDevice->Uuid() );
    StopIgnoringEmbeddedL( aDevice );
    }

// -----------------------------------------------------------------------------
// CUpnpCpbCurrentHttpClient::StopIgnoringEmbeddedL
//
// -----------------------------------------------------------------------------
//
void CUpnpCpbCurrentHttpClient::StopIgnoringEmbeddedL( CUpnpDevice* aRoot )
    {
    RPointerArray<CUpnpDevice> devicesAll;
    aRoot->GetAllDevices( devicesAll );
    CleanupClosePushL( devicesAll );
    for ( TInt k = 0; k < devicesAll.Count(); k++ )
        {
        iStackRequestor->StopIgnoringL( devicesAll[k]->Uuid() );
        }
    CleanupStack::PopAndDestroy( &devicesAll );
    }

// -----------------------------------------------------------------------------
// CUpnpCpbCurrentHttpClient::StopIgnoringUuidL
// Send stop ignoring message to MH
// -----------------------------------------------------------------------------
//
void CUpnpCpbCurrentHttpClient::StopIgnoringUuidL( const TDesC8& aUuids )
    {
    iStackRequestor->StopIgnoringL( aUuids );
    }

// -----------------------------------------------------------------------------
// CUpnpCpbCurrentHttpClient::StartIPFilteringL
//
// -----------------------------------------------------------------------------
//
void CUpnpCpbCurrentHttpClient::StartIPFilteringL()
    {
    iStackRequestor->StartIPFilteringL();
    }

// -----------------------------------------------------------------------------
// CUpnpCpbCurrentHttpClient::StopIPFiltering
//
// -----------------------------------------------------------------------------
//
void CUpnpCpbCurrentHttpClient::StopIPFiltering()
    {
    iStackRequestor->StopIPFiltering();
    }

// -----------------------------------------------------------------------------
// CUpnpCpbCurrentHttpClient::SendL
// Send response HTTP message
// -----------------------------------------------------------------------------
//
void CUpnpCpbCurrentHttpClient::SendL( CUpnpHttpMessage* aHttpMessage )
    {
    const TInt KUnused(0);
    iStackRequestor->SendMessageL( aHttpMessage, *this, KUnused );
    }

// -----------------------------------------------------------------------------
// CUpnpCpbCurrentHttpClient::SendFileByPostL
// Send HTTP message
// -----------------------------------------------------------------------------
//
void CUpnpCpbCurrentHttpClient::SendFileByPostL( CUpnpHttpMessage* aHttpMessage )
    {
    if ( !aHttpMessage )
        {
        User::Leave( KErrArgument );
        }
    iFileSender->SendL( *aHttpMessage );
    }

// -----------------------------------------------------------------------------
// CUpnpCpbCurrentHttpClient::SearchL
// Sends a SSDP Search with 8-bit search string.
// -----------------------------------------------------------------------------
//
void CUpnpCpbCurrentHttpClient::SearchL( const TDesC8& aSearchString )
    {
    iStackRequestor->SearchL( aSearchString );
    }

// -----------------------------------------------------------------------------
// CUpnpCpbCurrentHttpClient::ClientResponseRecivedLD
// Callback indicating that response was received within transaction.
// -----------------------------------------------------------------------------
//
void CUpnpCpbCurrentHttpClient::ClientResponseRecivedLD(
    CUpnpHttpTransaction& aCompletedTrasaction )
    {
    TInt idx( iPendingTransactions.Find( &aCompletedTrasaction ) );
    //it should never happen that we received transaction that wasn't sent by us
    ASSERT( idx >= 0 );
    aCompletedTrasaction.ProcessResponseL();
    iPendingTransactions.Remove( idx );
    delete ( &aCompletedTrasaction );
    }

// -----------------------------------------------------------------------------
// CUpnpCpbCurrentHttpClient::WlanLostOccurs
//
// -----------------------------------------------------------------------------
//
void CUpnpCpbCurrentHttpClient::WlanLostOccurs()
    {
    iStackRequestor->StopHttpServer();
    TRAP_IGNORE( SetNullRequestorsL() );
    }
    
// -----------------------------------------------------------------------------
// CUpnpCpbCurrentHttpClient::AddressChanged
//
// -----------------------------------------------------------------------------
//
void CUpnpCpbCurrentHttpClient::AddressChangedL()
    {
    iStackRequestor->StopHttpServer();
    iStackRequestor->StartHttpServerL();
    }    

// -----------------------------------------------------------------------------
// CUpnpCpbCurrentHttpClient::SetNullRequestorsL
//
// -----------------------------------------------------------------------------
//
void CUpnpCpbCurrentHttpClient::SetNullRequestorsL()
    {
    CUpnpCpHttpRequestorImplBase* httpReq = new ( ELeave ) CUpnpCpHttpRequestorImplBase();
    delete iHttpRequestorDefaultImpl;
    iHttpRequestorDefaultImpl = NULL;
    iHttpRequestorDefaultImpl = httpReq;
    iHttpRequestor = iHttpRequestorDefaultImpl;

    CUpnpCpStackRequestorImplBase* stackReq = new ( ELeave ) CUpnpCpStackRequestorImplBase();
    delete iStackRequestorDefaultImpl;
    iStackRequestorDefaultImpl = NULL;
    iStackRequestorDefaultImpl = stackReq;
    iStackRequestor = iStackRequestorDefaultImpl;
    }

//  End of File

