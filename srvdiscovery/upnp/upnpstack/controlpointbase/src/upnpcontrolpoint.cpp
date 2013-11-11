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
* Description:  Declares ControlPoint class.
 *
*/


// INCLUDE FILES

#include <e32base.h>
#include <e32cons.h>
#include <e32std.h>
#include <in_sock.h>
#include <charconv.h>
#include <utf.h>

#include "upnpdispatcher.h"
#include "upnphttpmessage.h"
#include "upnpsoapmessage.h"
#include "upnpicon.h"
#include "upnpdevice.h"
#include "upnpstring.h"
#include "upnphttpmessagefactory.h"
#include "upnpsoapmessagefactory.h"
#include "upnpgenamessagefactory.h"
#include "upnpserviceinfo.h"
#include "upnpcontrolpoint.h"
#include "upnpcommonupnplits.h"
#include "upnpcons.h"
#include "upnpcpbhttpmessagecontroller.h"
#include "upnpcpbcurrenthttpclient.h"
#include "upnpcpbdescriptionagent.h"
#include "upnpcpbdiscoveryagent.h"
#include "upnpcpbdevicerepository.h"
#include "upnpcpstackrequestor.h"
#include "upnpcontenthandlerscontroller.h"
#include "upnpconnectionmanagernetworkeventprovider.h"
#include "upnpcpbinitialeventretryhandler.h"

#define KLogFile _L("UPnPCP.txt")
#include "upnpcustomlog.h"

using namespace UpnpHTTP;

static const TInt KDTargetDeviceTypesGranularity = 1;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpControlPoint::CUpnpControlPoint
// C++ default constructor
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpControlPoint::CUpnpControlPoint()
    : iPredefinedStackRequestor( NULL ), iPredefinedHttpRequestor( NULL )
    {
    }

// -----------------------------------------------------------------------------
// CUpnpControlPoint::CUpnpControlPoint
// C++ constructor for internal, automated test cases use
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpControlPoint::CUpnpControlPoint(
    MUpnpCpStackRequestor& aStackRequestor, MUpnpCpHttpRequestor& aHttpRequestor )
    : iPredefinedStackRequestor( &aStackRequestor ),
      iPredefinedHttpRequestor( &aHttpRequestor )
    {
    }

// -----------------------------------------------------------------------------
// CUpnpControlPoint::~CUpnpControlPoint
// C++ default destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpControlPoint::~CUpnpControlPoint()
    {
    LOG_FUNC_NAME;

    delete iCpbHttpClient;

    iSubscribedServicesInfos.ResetAndDestroy();
    iActionList.ResetAndDestroy();
    
    if ( iTargetDeviceTypes )
        {
        iTargetDeviceTypes->Reset();
        delete iTargetDeviceTypes;
        }

    delete iDescriptionAgent;
    delete iDiscoveryAgent;

    delete iNetworkEventProvider;

    delete iSaxController;
    delete iDeviceRepository;
    delete iInitialEventRetry;
    LOGS("CUpnpControlPoint::~CUpnpControlPoint - END");
    }

// -----------------------------------------------------------------------------
// CUpnpControlPoint::ConstructL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpControlPoint::ConstructL( const CDesC8Array& aTargetDeviceTypes )
    {
    iNetworkEventProvider = CUpnpConnectionManagerNetworkEventProvider::NewL( *this );

    iCpbHttpClient =
        CUpnpCpbCurrentHttpClient::NewL(
            *this, iPredefinedHttpRequestor, iPredefinedStackRequestor );
    iTargetDeviceTypes = new( ELeave ) CDesC8ArrayFlat( KDTargetDeviceTypesGranularity );

    for ( TInt i( 0 ); i < aTargetDeviceTypes.Count(); i++ )
        {
        iTargetDeviceTypes->AppendL( aTargetDeviceTypes[i]);
        }
    iDeviceRepository = CUpnpCpbDeviceRepository::NewL(*iTargetDeviceTypes);

    iSaxController = CUpnpContentHandlersController::NewL();
    iDiscoveryAgent = CUpnpCpbDiscoveryAgent::NewL( *iCpbHttpClient );
    iDescriptionAgent =
        CUpnpCpbDescriptionAgent::NewL(
            *this, *iCpbHttpClient, *iDeviceRepository );
    
    iInitialEventRetry = new( ELeave ) CUpnpCpbInitialEventRetryHandler( *this );
    
    }

// -----------------------------------------------------------------------------
// CUpnpControlPoint::ConstructL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpControlPoint::ConstructL( const TDesC8& aTargetDeviceType )
    {
    CDesC8ArrayFlat* targetDeviceTypes =
    new (ELeave) CDesC8ArrayFlat( KDTargetDeviceTypesGranularity );
    CleanupStack::PushL( targetDeviceTypes );
    targetDeviceTypes->AppendL( aTargetDeviceType );
    ConstructL( *targetDeviceTypes );
    CleanupStack::PopAndDestroy( targetDeviceTypes );
    }

// -----------------------------------------------------------------------------
// CUpnpControlPoint::SearchL
// Search intresting Device Types from network
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpControlPoint::SearchL( const TDesC8& aSearchString )
    {
    if( aSearchString.Length() == 0 )
        {
        User::Leave( KErrCorrupt );
        }
    iDiscoveryAgent->SearchL( aSearchString );
    }

// -----------------------------------------------------------------------------
// CUpnpControlPoint::SendL
// Send an action message.
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpControlPoint::SendL( CUpnpAction* aAction )
    {
    iCpbHttpClient->SendActionL( aAction );
    iActionList.AppendL( aAction );
    }

// -----------------------------------------------------------------------------
// CUpnpControlPoint::SendL
// Send HTTP message
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpControlPoint::SendL( CUpnpHttpMessage* aHttpMessage )
    {
    iCpbHttpClient->SendFileByPostL(aHttpMessage);
    }

// -----------------------------------------------------------------------------
// CUpnpControlPoint::SendResponseMessageLD
// Send HTTP message and destroy it (leave safe)
// -----------------------------------------------------------------------------
//
void CUpnpControlPoint::SendResponseMessageLD( CUpnpHttpMessage* aHttpMessage )
    {
    CleanupStack::PushL( aHttpMessage );
    iCpbHttpClient->SendL( aHttpMessage );
    CleanupStack::PopAndDestroy( aHttpMessage );
    }

// -----------------------------------------------------------------------------
// CUpnpControlPoint::DeviceList
//
// -----------------------------------------------------------------------------
//
EXPORT_C const RPointerArray<CUpnpDevice>& CUpnpControlPoint::DeviceList() const
    {
    return iDeviceRepository->DiscoveredDeviceList();
    }

// -----------------------------------------------------------------------------
// CUpnpControlPoint::SubscribeL
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpControlPoint::SubscribeL( CUpnpService* aService )
    {
    LOG_FUNC_NAME;

    if ( !aService )
        {
        return;
        }

    if( FindServiceInfoByService( aService ) )
        {
        return;
        }

    LOGS("CUpnpControlPoint::SubscribeL - Subscribing service");

    CUpnpServiceInfo* serviceInfo = CUpnpServiceInfo::NewLC( this, aService );
    iSubscribedServicesInfos.AppendL( serviceInfo );
    CleanupStack::Pop( serviceInfo );

    iCpbHttpClient->SendSubscribeL( serviceInfo );

    LOGS("CUpnpControlPoint::SubscribeL - end");

    }

// -----------------------------------------------------------------------------
// CUpnpControlPoint::UnsubscribeL
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpControlPoint::UnsubscribeL( const CUpnpService* aService )
    {
    UnsubscribeL( const_cast<CUpnpService*>( aService ) , ETrue );
    }

// -----------------------------------------------------------------------------
// CUpnpControlPoint::UnsubscribeL
//
// -----------------------------------------------------------------------------
//
void CUpnpControlPoint::UnsubscribeL( CUpnpService* aService,
    TBool aSendUnsubscribeMessage )
    {
    LOG_FUNC_NAME;

    if ( !aService )
        {
        return;
        }

    CUpnpServiceInfo* subscribedServiceInfo =
            FindServiceInfoByService( aService );
    if ( !subscribedServiceInfo )
        {
        return;
        }

    LOGS("CUpnpControlPoint::UnsubscribeL - Unsubscribing service");

    RemoveFromSubscribedList( subscribedServiceInfo );

    if ( aSendUnsubscribeMessage )
        {
        CleanupStack::PushL( subscribedServiceInfo );
        iCpbHttpClient->SendUnsubscribeL( subscribedServiceInfo );
        CleanupStack::Pop( subscribedServiceInfo );
        }

    delete subscribedServiceInfo;

    LOGS("CUpnpControlPoint::UnsubscribeL - end");
    }

// -----------------------------------------------------------------------------
// CUpnpControlPoint::ResubscribeL
//
// -----------------------------------------------------------------------------
//
void CUpnpControlPoint::ResubscribeL( CUpnpServiceInfo* aServiceInfo )
    {
    iCpbHttpClient->SendResubscribeL( aServiceInfo );
    }

// -----------------------------------------------------------------------------
// CUpnpControlPoint::RemoveFromSubscribedList
//
// -----------------------------------------------------------------------------
//
void CUpnpControlPoint::RemoveFromSubscribedList(
    CUpnpServiceInfo* aServiceInfoToRemove )
    {
    const TInt idx = iSubscribedServicesInfos.Find( aServiceInfoToRemove );
    if ( KErrNotFound != idx )
        {
        iSubscribedServicesInfos.Remove( idx );
        }
    iSubscribedServicesInfos.GranularCompress();
    }

// -----------------------------------------------------------------------------
// CUpnpControlPoint::DeviceFoundL
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpControlPoint::DeviceFoundL( CUpnpDevice& aDevice )
    {
    iDescriptionAgent->DeviceAliveNotificationL(&aDevice);
    }

// -----------------------------------------------------------------------------
// CUpnpControlPoint::DeviceLostL
// Callback function - remove device
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpControlPoint::DeviceLostL( CUpnpDevice& aDevice )
    {
    LOG_FUNC_NAME;

    //device from ssdp
    CUpnpDevice* ssdpDevice =
        iDeviceRepository->RemoveIncomingDevice( aDevice.Uuid() );
    if ( ssdpDevice )
        {
        delete ssdpDevice;
        }
    CUpnpDevice* device = NULL;

    device = iDeviceRepository->FindDevice( aDevice.Uuid() );

    if ( !device )
        {
        return;
        }

    if ( aDevice.Expired() )
        {
        device->SetExpired( ETrue );
        if ( !AllDevicesExpired( device ) )
            {
            return;
            }
        }

    // always remove whole device tree
    if ( device->IsEmbeddDevice() )
        {
        CUpnpDevice* rootDevice = iDeviceRepository->FindRoot( device->Uuid() );
        // In case of error when root device is not found during discovery
        if ( rootDevice )
            {
            device = rootDevice;
            }
        }

    RemoveRootDeviceLD( device );
    }

// -----------------------------------------------------------------------------
// CUpnpControlPoint::AllDevicesExpired
//
// -----------------------------------------------------------------------------
//
TBool CUpnpControlPoint::AllDevicesExpired( CUpnpDevice* aDevice )
    {
    // notification about expiration of cache-control time of aDevice
    if ( aDevice->IsEmbeddDevice() ) // take root device
        {
        aDevice = iDeviceRepository->FindRoot( aDevice->Uuid() );
        }
    // if root device not expired then return
    if ( !aDevice || !aDevice->Expired() )
        {
        return EFalse;
        }
    RPointerArray<CUpnpDevice> devices;
    aDevice->GetAllDevices( devices ); // take all embedded devices
    for ( TInt k = 0; k < devices.Count(); k++ )
        {
        // if not all embedded devices expired then return
        if ( !devices[k]->Expired() )
            {
            devices.Close();
            return EFalse;
            }
        }
    devices.Close();
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CUpnpControlPoint::Path
//
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CUpnpControlPoint::Path()
    {
    return KPath();
    }

// -----------------------------------------------------------------------------
// CUpnpControlPoint::MessageReceived
// Callback funtion - HTTP message received
// -----------------------------------------------------------------------------
//
void CUpnpControlPoint::MessageReceived( CUpnpHttpMessage* aHttpMessage )
    {
    HttpResponseReceived( aHttpMessage );
    }

// -----------------------------------------------------------------------------
// CUpnpControlPoint::MapHttpError
//
// -----------------------------------------------------------------------------
//
TInt CUpnpControlPoint::MapHttpError( TInt aError )
    {
    if ( aError >= EHttpBadRequest && aError < EHttpInternalServerError )
        {
        return KErrCouldNotConnect;
        }
    else if ( aError > EHttpInternalServerError )
        {
        return KErrServerBusy;
        }
    else
        {
        return KErrUnknown;
        }

    }

// -----------------------------------------------------------------------------
// CUpnpControlPoint::CheckEventKey
//
// -----------------------------------------------------------------------------
//
TInt CUpnpControlPoint::CheckEventKey( const TDesC8& aIncomingSeq,
    TUint32 aExistingSeq )
    {
    const TUint KMaxSeq = 2146583647;
    const TUint KMinSeq = 1;

    TUint IncomingSeq;
    TInt error = KErrNone;

    TLex8 lex(aIncomingSeq);
    lex.Mark();
    error = lex.Val( IncomingSeq );

    //--- check if SEQ is a number
    if ( !error )
        {
        lex.UnGetToMark();

        for ( TInt i(0); i < aIncomingSeq.Length(); i++ )
            {
            TChar character = lex.Get();
            if ( !(character.IsDigit() ) )
                {
                error = KErrGeneral;
                break;
                }
            }
        }

    //--- check SEQ incrementing/wrapping
    if ( !error )
        {
        // when initial event appears, don't check SEQ increment
        if ( (IncomingSeq == 0 ) && (aExistingSeq == 0 ) )
            return KErrNone;

        if ( aExistingSeq < KMaxSeq )
            {
            aExistingSeq++;
            }
        else
            {
            aExistingSeq = KMinSeq;
            }

        // if 'incoming SEQ' == 'incremented existing SEQ' -> OK
        if ( IncomingSeq == aExistingSeq )
            {
            error = KErrNone;
            }
        else
            {
            error = KErrCancel;
            }
        }

    return error;
    }

// -----------------------------------------------------------------------------
// CUpnpControlPoint::ThrowErrorL
//
// -----------------------------------------------------------------------------
//
void CUpnpControlPoint::SendErrorResponseL( CUpnpHttpMessage* aHttpMessage,
    THttpStatusCode aCode )
    {
    CUpnpHttpMessage* msg = RUpnpHttpMessageFactory::HttpResponseErrorL(
        aHttpMessage, aCode );
    SendResponseMessageLD( msg );
    }

// -----------------------------------------------------------------------------
// CUpnpControlPoint::SubscriptionResponseReceivedL
//
// -----------------------------------------------------------------------------
//
void CUpnpControlPoint::SubscriptionResponseReceivedL(
    CUpnpHttpMessage& aHttpMessage )
    {
    LOG_FUNC_NAME;
    CUpnpGenaMessage* genaMsg =
            reinterpret_cast <CUpnpGenaMessage*> ( &aHttpMessage );

    CUpnpServiceInfo* tmpServiceInfo = FindServiceInfoByGenaMessage( genaMsg );
    if ( !tmpServiceInfo )
        {
        return;
        }

    tmpServiceInfo->SetSidL( genaMsg->Sid() );
    tmpServiceInfo->StartTimerL( genaMsg->Timeout() );
    if(iDeviceRepository->MatchTargetDevice(_L8("*")))
        {
        aHttpMessage.SetType(ESubscription);
        aHttpMessage.SetDestinationPathL(tmpServiceInfo->Service()->SubscriptionUrl());
        TRAP_IGNORE( HttpResponseReceivedL( &aHttpMessage ) );
        }

    LOGS("CUpnpControlPoint::SubscRespRcvdL - end");
    }

// -----------------------------------------------------------------------------
// CUpnpControlPoint::HandlePostponedInitialEventL
//
// -----------------------------------------------------------------------------
//
void CUpnpControlPoint::HandlePostponedInitialEventL( CUpnpGenaMessage& aGenaMsg )
    {
    CUpnpServiceInfo* serviceInfo = FindServiceInfoBySid(
            iSubscribedServicesInfos, aGenaMsg.Sid() );
    if ( serviceInfo )
        {
        if(iDeviceRepository->MatchTargetDevice(_L8("*")))
            {
            aGenaMsg.SetDestinationPathL(serviceInfo->Service()->SubscriptionUrl());
            TRAP_IGNORE( HttpResponseReceivedL( &aGenaMsg) );
            }
        ParseEventUpdateStateVariablesL( aGenaMsg, *serviceInfo );        
        SendResponseMessageLD( RUpnpHttpMessageFactory::HttpResponseOkL(
                &aGenaMsg ) );
        }
    else
        {
        SendErrorResponseL( &aGenaMsg, EHttpPreconditionFailed );
        }
    }

// -----------------------------------------------------------------------------
// CUpnpControlPoint::UnSubscriptionResponseReceived
//
// -----------------------------------------------------------------------------
//
void CUpnpControlPoint::UnSubscriptionResponseReceived()
    {
    LOG_FUNC_NAME;
    }

// -----------------------------------------------------------------------------
// CUpnpControlPoint::NotifyReceivedL
//
// -----------------------------------------------------------------------------
//
void CUpnpControlPoint::NotifyReceivedL( CUpnpHttpMessage& aHttpMessage )
    {
    LOGS( "CUpnpControlPoint::NotifyReceivedL - start" );
    CUpnpGenaMessage* genaMsg =
            reinterpret_cast<CUpnpGenaMessage*> ( &aHttpMessage );
    CleanupStack::PushL(genaMsg);

    THttpStatusCode code = ValidateEventResolveHttpResponseCode( *genaMsg );
    if ( code != EHttp200Ok )
        {
        //--- If some of conditions are not fulfilled, send error
        SendErrorResponseL(genaMsg, code);
        CleanupStack::PopAndDestroy(genaMsg);
        LOGS( "CUpnpControlPoint::NotifyReceivedL - end" );
        return;
        }
    //--- checking if genaMsg SID is the same like the one in subscribed service.
    CUpnpServiceInfo* subscribedServiceInfo = FindServiceInfoBySid(
            iSubscribedServicesInfos, genaMsg->Sid());

    if ( subscribedServiceInfo )
        {
        //===== Checking validity of SEQ header
        TInt seqError = KErrNone;
        seqError = CheckEventKey( genaMsg->Seq(), subscribedServiceInfo->Seq() );
        if ( seqError )
            {
            if ( seqError == KErrCancel )
                {
                //------ if SEQ's order invalid
                //-> e.g 0,1,2 and then 1 -> Unsubscribe/Subscribe
                ResubscribeL( *subscribedServiceInfo );
                }
            else
                {
                LOGS( "CUpnpControlPoint::NotifyReceivedL - SEQ INAVLID" );
                SendErrorResponseL( genaMsg, EHttpBadRequest );
                User::Leave(seqError);
                }
            }
        else
            {
            if(iDeviceRepository->MatchTargetDevice(_L8("*")))
                {
                genaMsg->SetDestinationPathL(subscribedServiceInfo->Service()->SubscriptionUrl());
                TRAP_IGNORE( HttpResponseReceivedL( genaMsg) );
                }
            ParseEventUpdateStateVariablesL( *genaMsg, *subscribedServiceInfo );
            SendResponseMessageLD(RUpnpHttpMessageFactory::HttpResponseOkL( genaMsg ));
            LOGS( "CUpnpControlPoint::NotifyReceivedL - send OK" );
            }        
        CleanupStack::PopAndDestroy( genaMsg );
        
        }
    else
        {
        TInt seq;
        TLex8 lex( genaMsg->Seq() );
        TInt error = lex.Val(seq);
        
        if ( !error && seq == 0 )
            {
            CleanupStack::Pop( genaMsg );
            //ownership is passed through
            iInitialEventRetry->AddL( genaMsg );        
            }
        else
            {
            SendErrorResponseL( genaMsg, EHttpPreconditionFailed );            
            CleanupStack::PopAndDestroy( genaMsg );
            }
        }
    
    }


// -----------------------------------------------------------------------------
// CUpnpControlPoint::Device
// This function returns a pointer to device identified by UUID.
// (other items were commented in a header).es
// -----------------------------------------------------------------------------
//
CUpnpServiceInfo* CUpnpControlPoint::FindServiceInfoBySid( RPointerArray<CUpnpServiceInfo>& aInfos,
        const TDesC8& aSid )
    {
    for ( TInt i=0; i < aInfos.Count(); i++ )
        {
        CUpnpServiceInfo* subscribedServiceInfo = aInfos[i];             
        if ( (subscribedServiceInfo->Sid().Length()> 0 )
         && (subscribedServiceInfo->Sid() == aSid ) )
            {
            return subscribedServiceInfo;
            }
        }
    return NULL;
    }

// -----------------------------------------------------------------------------
// CUpnpControlPoint::ResubscribeL
//
// -----------------------------------------------------------------------------
//
void CUpnpControlPoint::ResubscribeL( CUpnpServiceInfo& aInfo )
    {
    //------ if SEQ's order invalid
    //-> e.g 0,1,2 and then 1 -> Unsubscribe/Subscribe
    CUpnpService* service =
        aInfo.Service(); // remember the service

    UnsubscribeL( service );
    // after unsubscribing -> subscribe again
    SubscribeL( service );

    }

// -----------------------------------------------------------------------------
// CUpnpControlPoint::ParseEventUpdateStateVariablesL
//
// -----------------------------------------------------------------------------
//
void CUpnpControlPoint::ParseEventUpdateStateVariablesL(
        CUpnpGenaMessage& aGenaMsg, CUpnpServiceInfo& aInfo )
    {
    TInt parsingError = KErrNone;
        TRAP( parsingError, iSaxController->AttachL(
                aGenaMsg.Body(), *( aInfo.Service() ) ) );
    if ( parsingError )
        {
        LOGS( "CUpnpControlPoint::NotifyReceivedL - "
                "parsing gena or parsing SEQ - FAILED" );
        SendErrorResponseL( &aGenaMsg, EHttpBadRequest );
        User::Leave( parsingError );
        }

    // if SEQ ok -> put the value to the service
    TInt seq;
    TLex8 lex( aGenaMsg.Seq() );
    TInt error = lex.Val( seq );
    // error ignored, it is fully checked in CheckEventKey()

    aInfo.SetSeq( seq );
    StateUpdated( aInfo.Service() );

    }

 // -----------------------------------------------------------------------------
// CUpnpControlPoint::ValidateEventResolveHttpResponseCode
//
// -----------------------------------------------------------------------------
//
THttpStatusCode CUpnpControlPoint::ValidateEventResolveHttpResponseCode(
        CUpnpGenaMessage& aGenaMsg )

    {
    THttpStatusCode code = EHttp200Ok;
    //--- NOTIFY message syntax - checking if headers: NT and NTS exists
    
    if ( aGenaMsg.Method() != UpnpGENA::KGenaNotify() 
            || aGenaMsg.Nt() == KNoHeader 
            || aGenaMsg.Nts() == KNoHeader )
        {
        // headers NT or NTS are missing
        code = EHttpBadRequest;
        LOGS( "CUpnpControlPoint::NotifyReceivedL - bad request" );
        }

    //--- NOTIFY message syntax
    //- checking if headers: NT and NTS has proper structure [name: value]
    else if (aGenaMsg.Nt() != UpnpGENA::KDefaultNt || aGenaMsg.Nts()
            != UpnpGENA::KDefaultNts)
        {
        // headers NT or NTS are invalid
        code = EHttpPreconditionFailed;
        LOGS( "CUpnpControlPoint::NotifyReceivedL - headers NT or NTS are invalid" );
        }
    return code;
    }


// -----------------------------------------------------------------------------
// CUpnpControlPoint::Device
// This function returns a pointer to device identified by UUID.
// (other items were commented in a header).es
// -----------------------------------------------------------------------------
//
EXPORT_C const CUpnpDevice* CUpnpControlPoint::Device(const TDesC8& aUuid)
    {
    RPointerArray<CUpnpDevice> devices = DeviceList();
    for( TInt i(0); i < devices.Count(); i++)
        {
        if(aUuid.Compare( devices[i]->Uuid() ) == 0)
            {
            return devices[i];
            }
        }
    return NULL;
    }

// -----------------------------------------------------------------------------
// CUpnpControlPoint::AddressChangedL
//
// -----------------------------------------------------------------------------
//
void CUpnpControlPoint::AddressChangedL()
    {
    LOG_FUNC_NAME;
    TInt i(0);
    RPointerArray<CUpnpDevice> devices = iDeviceRepository->DiscoveredDeviceList();
    RPointerArray<CUpnpDevice> rootDevices;
    CleanupClosePushL(rootDevices);
    for( i = 0; i < devices.Count(); i++)
        {
        if(!devices[i]->IsEmbeddDevice())
            {
            rootDevices.AppendL(devices[i]);
            }
        }
    devices = iDeviceRepository->UnneddedDeviceList();
    for( i = 0; i < devices.Count(); i++)
        {
        if(!devices[i]->IsEmbeddDevice())
            {
            rootDevices.AppendL(devices[i]);
            }
        }
    devices = iDeviceRepository->UninterestingDeviceList();
    for( i = 0; i < devices.Count(); i++)
        {
        if(!devices[i]->IsEmbeddDevice())
            {
            rootDevices.AppendL(devices[i]);
            }
        }
    devices = iDeviceRepository->IncompliteDeviceList();
    for( i = 0; i < devices.Count(); i++)
        {
        if(!devices[i]->IsEmbeddDevice())
            {
            rootDevices.AppendL(devices[i]);
            }
        }

    for( i = 0; i < rootDevices.Count(); i++)
        {
        RemoveRootDeviceLD( rootDevices[i] );
        }
    CleanupStack::PopAndDestroy(&rootDevices);

    }

// -----------------------------------------------------------------------------
// CUpnpControlPoint::GiveAddressL
//
// -----------------------------------------------------------------------------
//
TInetAddr CUpnpControlPoint::GiveAddressL( const TDesC8& aUrl )
    {
    TInetAddr result= INET_ADDR(0,0,0,0);
    result.SetPort( 0 );
    TInt colonPos = aUrl.Find( KColon() ) ;
    if ( colonPos != KErrNotFound )
        {
        TPtrC8 addressStr = aUrl.Left( colonPos );
        HBufC* buffer = HBufC::NewL( addressStr.Length() );
        TPtr bufferPtr(buffer->Des() );
        bufferPtr.Copy( addressStr );
        result.Input( *buffer );
        delete buffer;
        const TDesC8& portStr = aUrl.Mid( colonPos + 1 );
        TLex8 lexer(portStr);
        TInt port(0);
        lexer.Val( port );
        result.SetPort( port );
        }
    else
        {
        HBufC* buffer = HBufC::NewL( aUrl.Length() );
        TPtr bufferPtr(buffer->Des() );
        bufferPtr.Copy( aUrl );
        result.Input( *buffer );
        delete buffer;
        }
    return result;
    }

// -----------------------------------------------------------------------------
// CUpnpControlPoint::RemoveRootDeviceLD
//
// -----------------------------------------------------------------------------
//
void CUpnpControlPoint::RemoveRootDeviceLD( CUpnpDevice* aDevice )
    {
    TBool isDiscovered = EFalse;

    iCpbHttpClient->StopIgnoringL( aDevice );

    RemoveActionsL( aDevice );
    RemoveServicesL( aDevice );

    //find all children of the device to be removed
    RPointerArray<CUpnpDevice> devices;
    aDevice->GetAllDevices( devices );
    CleanupClosePushL( devices );

    for ( TInt k = 0; k < devices.Count(); k++ )
        {
        RemoveActionsL( devices[k] );
        isDiscovered
                = iDeviceRepository->IsDiscoveredDevice( devices[k]->Uuid() );
        iDeviceRepository->RemoveDevice( devices[k] );
        if ( isDiscovered )
            {
            DeviceDisappeared( devices[k] );
            }
        RemoveServicesL( devices[k] );
        }

    CleanupStack::PopAndDestroy( &devices );

    iActionList.Compress();
    isDiscovered = iDeviceRepository->IsDiscoveredDevice( aDevice->Uuid() );
    iDeviceRepository->RemoveDevice( aDevice );
    if ( isDiscovered )
        {
        DeviceDisappeared( aDevice );
        }

    delete aDevice;
    aDevice = NULL;
    }

// -----------------------------------------------------------------------------
// CUpnpControlPoint::RemoveActionsL
//
// -----------------------------------------------------------------------------
//
void CUpnpControlPoint::RemoveActionsL( CUpnpDevice* aDevice )
    {
    for ( TInt i = 0; i < iActionList.Count(); ++i )
        {
        if( ( iActionList[i]->Service().Device().Uuid()).Compare(
            aDevice->Uuid() ) == 0 )
            {
            CUpnpAction* action = iActionList[i];
            
            CUpnpSoapMessage* errorMsg =
                    RUpnpSoapMessageFactory::SoapResponseL( action,
                        EActionFailed );

            CleanupStack::PushL( errorMsg );
            iSaxController->UpdateActionWithErrorResponseL( errorMsg, action );
            ActionResponseReceived( action );
            iActionList.Remove( i );
            delete action;
            CleanupStack::PopAndDestroy( errorMsg );
            //as a action is removed from the list the
            //index is kept as same for the next loop.
            --i;
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpControlPoint::RemoveServicesL
//
// -----------------------------------------------------------------------------
//
void CUpnpControlPoint::RemoveServicesL( CUpnpDevice* aDevice )
    {
    RPointerArray<CUpnpService>& services = aDevice->ServiceList();
    for ( TInt j = 0; j < services.Count(); j++ )
        {
        UnsubscribeL( services[j], EFalse );
        }
    }

// -----------------------------------------------------------------------------
// CUpnpControlPoint::FindServiceInfoByService
//
// -----------------------------------------------------------------------------
//
CUpnpServiceInfo* CUpnpControlPoint::FindServiceInfoByService(
    CUpnpService* aService )
    {
    for ( TInt i=0; i < iSubscribedServicesInfos.Count() ; i++ )
        {
        if ( iSubscribedServicesInfos[i]->Service() == aService )
            {
            return iSubscribedServicesInfos[i];
            }
        }
    return NULL;
    }

// -----------------------------------------------------------------------------
// CUpnpControlPoint::FindServiceInfoByHttpMessage
//
// -----------------------------------------------------------------------------
//
CUpnpServiceInfo* CUpnpControlPoint::FindServiceInfoByGenaMessage(
    CUpnpGenaMessage* aGenaMsg )
    {
    for ( TInt i=0; i < iSubscribedServicesInfos.Count() ; i++ )
        {
        if ( iSubscribedServicesInfos[i]->SessionId()
                == aGenaMsg->SessionId() )
            {
            return iSubscribedServicesInfos[i];
            }
        }
    return NULL;
    }

void CUpnpControlPoint::ProcessDeviceMessageL( CUpnpHttpMessage* aHttpMessage )
    {
    iDescriptionAgent->ProcessDeviceMessageL( aHttpMessage );
    }

void CUpnpControlPoint::ProcessServiceMessageL( CUpnpHttpMessage* aHttpMessage )
    {
    iDescriptionAgent->ProcessServiceMessageL( aHttpMessage );
    }

void CUpnpControlPoint::ProcessActionMessageL( CUpnpHttpMessage* aHttpMessage )
    {
    
    TInt idx(0);
    // Match request action
    for ( ; idx < iActionList.Count() && iActionList[idx]->SessionId()
            != aHttpMessage->SessionId(); idx++ )
        {
        }
    if ( idx == iActionList.Count() )
        {
        return;
        }

    CUpnpAction* tmpAction = iActionList[idx];

     
    TInt error(KErrNone);
    switch ( aHttpMessage->Error() )
        {
        case EHttp200Ok:
            {
            TRAP( error,
                iSaxController->UpdateActionWithOKResponseL(
                            static_cast<CUpnpSoapMessage*>(aHttpMessage), tmpAction );
            );
            }
            break;
        case EHttpInternalServerError:
            {
            TRAP( error,
                iSaxController->UpdateActionWithErrorResponseL(
                            static_cast<CUpnpSoapMessage*>(aHttpMessage), tmpAction );
            );
            break;
            }
        default:
            {
            if ( aHttpMessage->InternalError() )
                {
                tmpAction->SetError( aHttpMessage->InternalError() );
                }
            else
                {
                tmpAction->SetError( MapHttpError( aHttpMessage->Error() ) );
                }
            break;
            }
        }
    if ( KErrNone != error )
        {
        tmpAction->SetError( error );
        }
    
    ActionResponseReceived( tmpAction );
    delete tmpAction;
    iActionList.Remove( idx );
    }

// -----------------------------------------------------------------------------
// CUpnpControlPoint::StateUpdated
//
// -----------------------------------------------------------------------------
//
void CUpnpControlPoint::StateUpdated( CUpnpService* aService )
    {
    TRAP_IGNORE( StateUpdatedL( aService ) );
    }

// -----------------------------------------------------------------------------
// CUpnpControlPoint::DeviceDiscovered
//
// -----------------------------------------------------------------------------
//
void CUpnpControlPoint::DeviceDiscovered( CUpnpDevice* aDevice )
    {
    TRAP_IGNORE( DeviceDiscoveredL( aDevice ) );
    }

// -----------------------------------------------------------------------------
// CUpnpControlPoint::DeviceDisappeared
//
// -----------------------------------------------------------------------------
//
void CUpnpControlPoint::DeviceDisappeared( CUpnpDevice* aDevice )
    {
    TRAP_IGNORE( DeviceDisappearedL( aDevice ) );
    }

// -----------------------------------------------------------------------------
// CUpnpControlPoint::ActionResponseReceived
//
// -----------------------------------------------------------------------------
//
void CUpnpControlPoint::ActionResponseReceived( CUpnpAction* aAction )
    {
    TRAP_IGNORE( ActionResponseReceivedL( aAction ) );
    }

// -----------------------------------------------------------------------------
// CUpnpControlPoint::HttpResponseReceived
//
// -----------------------------------------------------------------------------
//
void CUpnpControlPoint::HttpResponseReceived( CUpnpHttpMessage* aMessage )
    {
    TRAP_IGNORE( HttpResponseReceivedL( aMessage ) );
    }

// -----------------------------------------------------------------------------
// CUpnpControlPoint::EnableRadaDeviceL
//
// -----------------------------------------------------------------------------
//
#ifdef RD_UPNP_REMOTE_ACCESS
EXPORT_C void CUpnpControlPoint::EnableRadaDeviceL( TRequestStatus& aStatus )
#else
EXPORT_C void CUpnpControlPoint::EnableRadaDeviceL( TRequestStatus& /*aStatus*/ )
#endif
    {
    #ifdef RD_UPNP_REMOTE_ACCESS
    iDiscoveryAgent->EnableRadaDeviceL( aStatus );
    #endif
    }

// -----------------------------------------------------------------------------
// CUpnpControlPoint::EnableRadaDeviceL
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpControlPoint::EnableRadaDeviceL()
    {
    #ifdef RD_UPNP_REMOTE_ACCESS
    return iDiscoveryAgent->EnableRadaDeviceL();
    #else
    return KErrNotSupported;
    #endif
    }

// -----------------------------------------------------------------------------
// CUpnpControlPoint::DisableRadaDeviceL
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpControlPoint::DisableRadaDeviceL()
    {
    #ifdef RD_UPNP_REMOTE_ACCESS
    iDiscoveryAgent->DisableRadaDeviceL();
    #endif
    }

// -----------------------------------------------------------------------------
// CUpnpControlPoint::StartIPFilteringL
//
// -----------------------------------------------------------------------------
//
#ifdef RD_UPNP_REMOTE_ACCESS
EXPORT_C void CUpnpControlPoint::StartIPFilteringL( TFilteringMode aMode )
#else
EXPORT_C void CUpnpControlPoint::StartIPFilteringL( TFilteringMode /*aMode*/ )
#endif
    {
    #ifdef RD_UPNP_REMOTE_ACCESS
    switch( aMode )
        {
        case EBoth:
            {
            iCpbHttpClient->StartIPFilteringL();
            iDiscoveryAgent->StartIPFilteringL();
            break;
            }
        case ERada:
            {
            iDiscoveryAgent->StartIPFilteringL();
            break;
            }
        case EControlPoint:
            {
            iCpbHttpClient->StartIPFilteringL();
            break;
            }
        default:
            {
            User::Leave( KErrNotSupported );
            break;
            }
        }
    #endif
    }

// -----------------------------------------------------------------------------
// CUpnpControlPoint::StopIPFilteringL
//
// -----------------------------------------------------------------------------
//
#ifdef RD_UPNP_REMOTE_ACCESS
EXPORT_C void CUpnpControlPoint::StopIPFilteringL( TFilteringMode aMode )
#else
EXPORT_C void CUpnpControlPoint::StopIPFilteringL( TFilteringMode /*aMode*/ )
#endif
    {
    #ifdef RD_UPNP_REMOTE_ACCESS
    switch( aMode )
        {
        case EBoth:
            {
            iCpbHttpClient->StopIPFiltering();
            iDiscoveryAgent->StopIPFiltering();
            break;
            }
        case ERada:
            {
            iDiscoveryAgent->StopIPFiltering();
            break;
            }
        case EControlPoint:
            {
            iCpbHttpClient->StopIPFiltering();
            break;
            }
        default:
            {
            User::Leave( KErrNotSupported );
            break;
            }
        }
    #endif
    }

// -----------------------------------------------------------------------------
// CUpnpControlPoint::NetworkEvent
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpControlPoint::NetworkEvent( CUpnpNetworkEventBase* aEvent )
    {
    LOG_FUNC_NAME;

    switch ( aEvent->Type() )
        {
        case EWlanLostEvent:
            {
            iCpbHttpClient->WlanLostOccurs();
            }
            break;

        case EAddressChangeEvent:
            {         
            TRAP_IGNORE( iCpbHttpClient->AddressChangedL(); AddressChangedL() );
            }
            break;
        }
    }

//  End of File
