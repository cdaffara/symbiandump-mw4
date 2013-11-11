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
 * Description:  Message handler engine
 *
 */

// INCLUDE FILES
#include <commdb.h>
#include <commdbconnpref.h>
#include "upnpcons.h"
#include "upnpmessagehandlerengine.h"
#include "upnpssdpserver.h"

#ifdef RD_UPNP_REMOTE_ACCESS
#include "upnpipfiltering.h"
#include "upnpipfilteringdnsquery.h"
#endif

#include "upnpssdphandlerup.h"
#include "upnpssdphandlerbase.h"
#include "upnpssdphandlerdown.h"
#include "upnpconnectionmanagerproxy.h"

#define KLogFile _L("UpnpMessageHandler.txt")
#include "upnpcustomlog.h"


// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CUpnpMessageHandlerEngine::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpMessageHandlerEngine* CUpnpMessageHandlerEngine::NewL(
    MMessageHandlerEngineObserver& aObserver )
    {
    // call the default constructor
    CUpnpMessageHandlerEngine* self = new (ELeave) CUpnpMessageHandlerEngine( aObserver );

    // call the default L constructor when in cleanup stack
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    // returns new instance
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpMessageHandlerEngine::CUpnpMessageHandlerEngine
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpMessageHandlerEngine::CUpnpMessageHandlerEngine(
    MMessageHandlerEngineObserver& aObserver ) :
    iObserver(aObserver)
    {
    }

// -----------------------------------------------------------------------------
// CUpnpMessageHandlerEngine::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpMessageHandlerEngine::ConstructL()
    {
    LOG_FUNC_NAME;

    // create a socket server connection
    User::LeaveIfError( iSocketServ.Connect() );

    CreateSessionIdPropertyL();

    iSsdpHandlerUp = CUpnpSsdpHandlerUp::NewL( *this );
    iSsdpHandlerDown = CUpnpSsdpHandlerDown::NewL( *this );
    iCurrentSsdpHandler = iSsdpHandlerUp;

    // device library stores information about remote and local devices
    iDeviceLibrary = CUpnpDeviceLibrary::NewL( *this, 0 );

    iConnectionManagerProxy = CUpnpConnectionManagerProxy::NewL( iSocketServ );
    iConnectionManagerProxy->SubscribeForNetworkEventsL( this );
    }

// -----------------------------------------------------------------------------
// CUpnpMessageHandlerEngine::CreateSessionIdPropertyL
// Create session id property and signal id semaphore
// -----------------------------------------------------------------------------
//
void CUpnpMessageHandlerEngine::CreateSessionIdPropertyL()
    {
    // create RProperty used as session id counter
    TInt err = RProperty::Define( KUPnPUtilsCat, EUPnPUtilsCounter,
        RProperty::EInt );
    if ( err != KErrAlreadyExists )
        {
        User::LeaveIfError( err );
        }

    // there are four global widely-used semaphores in the system
    // first two semaphores are for getting a running session id
    // first semaphore is an access semaphore
    // if it's signaled, we can have access to id semaphore
    // id semaphore holds the current session id
    //
    // the second place where these semaphores are used is CUpnpHttpMessage::NewSessionIdL()
    // 
    // basically, we want to tell the CUpnpHttpMessage class that the
    // EUPnPUtilsCounter RProperty is defined and ready to use

    TInt result =
            iAccessSemaphore.OpenGlobal( KSessionIdAccessSemaphoreName() );
    if ( result == KErrNotFound )
        {
        result = iAccessSemaphore.CreateGlobal(
            KSessionIdAccessSemaphoreName(), 0 );
            
        if ( result != KErrNone )
            {
            LOGS( "MESSAGEHANDLER *** Session id access semaphore creation failed" );
            }
        }
    else if ( result != KErrNone )
        {
        LOGS( "MESSAGEHANDLER *** Session id access semaphore creation failed" );
        }
    User::LeaveIfError( result );

    iAccessSemaphore.Signal();
    }

// -----------------------------------------------------------------------------
// CUpnpMessageHandlerEngine::~CUpnpMessageHandlerEngine
// Destructor.
// -----------------------------------------------------------------------------
//
CUpnpMessageHandlerEngine::~CUpnpMessageHandlerEngine()
    {
    __ASSERT_DEBUG( iCurrentSsdpHandler == iSsdpHandlerUp
            || iCurrentSsdpHandler == iSsdpHandlerDown,
            User::Panic( KMessageHandler, EMessageHandlerBadState ) );
    delete iDeviceLibrary;

    delete iSsdpHandlerUp;
    delete iSsdpHandlerDown;

    delete iConnectionManagerProxy;

#ifdef RD_UPNP_REMOTE_ACCESS    
    /*************   IPFiltering    *****************/
    delete iIPFilterRepository;
    /************************************************/
#endif

    // close socket server connection
    iSocketServ.Close();
    iAccessSemaphore.Close();
    }

// -----------------------------------------------------------------------------
// CUpnpMessageHandlerEngine::AdvertiseDeviceL
// Advertise device
// -----------------------------------------------------------------------------
//
void CUpnpMessageHandlerEngine::AdvertiseDeviceL( TInt aLive,
    CUpnpDeviceLibraryElement& aElement )
    {
    iCurrentSsdpHandler->AdvertiseDeviceL( aLive, aElement );
    }

// -----------------------------------------------------------------------------
// CUpnpMessageHandlerEngine::DeviceListChangedL
// Inform observer that device list changed.
// -----------------------------------------------------------------------------
//
void CUpnpMessageHandlerEngine::DeviceListChangedL()
    {
    LOG_FUNC_NAME;

    //Check waiting
    if ( iCurrentSsdpHandler == iSsdpHandlerUp )
        {        
        iObserver.DeviceListUpdateL();
        }
    }

// -----------------------------------------------------------------------------
// CUpnpMessageHandlerEngine::DeviceList
// Get device list.
// -----------------------------------------------------------------------------
//
RPointerArray<CUpnpDeviceLibraryElement>& CUpnpMessageHandlerEngine::DeviceList()
    {
    LOG_FUNC_NAME;
    // return device list
    return iDeviceLibrary->DeviceList();
    }

// -----------------------------------------------------------------------------
// CUpnpMessageHandlerEngine::AddLocalDeviceL
// Add local device to device library.
// -----------------------------------------------------------------------------
//
TInt CUpnpMessageHandlerEngine::AddLocalDeviceL(
    TUpnpAddLocalDevice &aDevice, TDesC8& aBuffer )
    {
    LOG_FUNC_NAME;
    
    iDeviceLibrary->AddInfoL( &aDevice, aBuffer, iConnectionManagerProxy->LocalAddress() );

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CUpnpMessageHandlerEngine::AddLocalControlPointL
// -----------------------------------------------------------------------------
//
TInt CUpnpMessageHandlerEngine::AddLocalControlPoint()
    {
    return iSsdpHandlerUp->AddLocalControlPoint();
    }

// -----------------------------------------------------------------------------
// CUpnpMessageHandlerEngine::RemoveLocalControlPointL
// -----------------------------------------------------------------------------
//
TInt CUpnpMessageHandlerEngine::RemoveLocalControlPoint()
    {
    return iSsdpHandlerUp->RemoveLocalControlPoint();
    }

// -----------------------------------------------------------------------------
// CUpnpMessageHandlerEngine::StopFilteringDeviceL
// -----------------------------------------------------------------------------
//
void CUpnpMessageHandlerEngine::StopFilteringDeviceL( const TDesC8& aUuid )
    {
    iDeviceLibrary->StopFilteringDeviceL( aUuid );
    }

// -----------------------------------------------------------------------------
// CUpnpMessageHandlerEngine::DeviceLibrary
// Get device library.
// -----------------------------------------------------------------------------
//
CUpnpDeviceLibrary* CUpnpMessageHandlerEngine::DeviceLibrary()
    {
    LOG_FUNC_NAME;
    return iDeviceLibrary;
    }

// -----------------------------------------------------------------------------
// CUpnpMessageHandlerEngine::SsdpEventL
// Callback function for received ssdp messages.
// -----------------------------------------------------------------------------
//
void CUpnpMessageHandlerEngine::SsdpEventL( CUpnpSsdpMessage *aMessage )
    {
    LOG_FUNC_NAME;

    // ignore messages with M-SEARCH there are serverd by SSDP Server internally
    if ( aMessage->IsSsdpMSearch() )
        {
        return;
        }
    // forward message to device library
    iDeviceLibrary->AddInfoL( aMessage );
    }

// -----------------------------------------------------------------------------
// CUpnpMessageHandlerEngine::SsdpSearchL
// Do ssdp searches.
// -----------------------------------------------------------------------------
//
void CUpnpMessageHandlerEngine::SsdpSearchL( TDesC8& aSearchString,
    TDesC8& aMX )
    {
    LOG_FUNC_NAME;
    iCurrentSsdpHandler->SsdpSearchL( aSearchString, aMX );
    }

// -----------------------------------------------------------------------------
// CUpnpMessageHandlerEngine::ActiveIap
// Returns the iap id (which is got in function above).
// -----------------------------------------------------------------------------
//
TInt CUpnpMessageHandlerEngine::ActiveIap() const
    {
    LOG_FUNC_NAME;
    return iConnectionManagerProxy->ActiveIap();
    }

// -----------------------------------------------------------------------------
// CUpnpMessageHandlerEngine::RemoveLocalDevice
// Removes local device from device library.
// -----------------------------------------------------------------------------
//
void CUpnpMessageHandlerEngine::RemoveLocalDeviceL( const TDesC8 &aUuid,
    TBool aSilent )
    {
    LOG_FUNC_NAME;

    // forward the request to device library
    if ( iCurrentSsdpHandler == iSsdpHandlerUp)
        {
        if ( !aSilent )
            {
            iDeviceLibrary->RemoveL( aUuid );
            }
        else
            {
            iDeviceLibrary->RemoveSilentL( aUuid );
            }
        }

    LOGS( "CUpnpMessageHandlerEngine::RemoveLocalDevice passed");
    }

// -----------------------------------------------------------------------------
// CUpnpMessageHandlerEngine::StartSsdp
// Start SSPD.
// -----------------------------------------------------------------------------
//
TInt CUpnpMessageHandlerEngine::StartSsdpL()
    {
    LOG_FUNC_NAME;
    if ( !iCurrentSsdpHandler->IsStarted() )
        {
        LOGS( "MESSAGEHANDLER *** Connecting to same access point...");        

        iCurrentSsdpHandler->StartL( iSocketServ );

#ifdef RD_UPNP_REMOTE_ACCESS    
        /**********IPFiltering*******************/
        iIPFilterRepository = CUpnpIPFiltering::NewL( iSocketServ,
                ActiveIap(),
                iObserver );
        /****************************************/
#endif

        return KErrNone;
        }
    else
        {
        // Ensure all two servers are started
        iCurrentSsdpHandler->StartL( iSocketServ );
        return KErrNone;
        }
    }


// -----------------------------------------------------------------------------
// CUpnpMessageHandlerEngine::InterfaceDown
// -----------------------------------------------------------------------------
//
void CUpnpMessageHandlerEngine::InterfaceDown()
    {
    LOG_FUNC_NAME;
       
    iSsdpHandlerUp->Stop();
    iCurrentSsdpHandler = iSsdpHandlerDown;
    }

#ifdef RD_UPNP_REMOTE_ACCESS
// -----------------------------------------------------------------------------
// CUpnpServiceInfo::AddAddressL
// -----------------------------------------------------------------------------
//
void CUpnpMessageHandlerEngine::AddAddressL( const TInetAddr& aAddress )
    {
    User::LeaveIfError( iIPFilterRepository->AddAddressL( aAddress ) );
    iObserver.IPListChange();
    }

// -----------------------------------------------------------------------------
// CUpnpServiceInfo::AddAddressL
// -----------------------------------------------------------------------------
//
void CUpnpMessageHandlerEngine::AddAddressL( const RMessage2& aMessage,
        const TDesC8& aRemoteName )
    {
    CUpnpIPFilteringDNSQuery* query = CUpnpIPFilteringDNSQuery::NewLC( aMessage,
            aRemoteName );
    iIPFilterRepository->AddAddressL( query );
    CleanupStack::Pop( query );
    }

// -----------------------------------------------------------------------------
// CUpnpServiceInfo::RemoveAddressL
// -----------------------------------------------------------------------------
//
void CUpnpMessageHandlerEngine::RemoveAddressL( const TInetAddr& aAddress )
    {
    if ( iIPFilterRepository->RemoveAddressL( aAddress ) >=0 )
        {
        iObserver.IPListChange();
        }
    }

// -----------------------------------------------------------------------------
// CUpnpServiceInfo::RemoveAddressL
// -----------------------------------------------------------------------------
//
void CUpnpMessageHandlerEngine::RemoveAddressL( const RMessage2& aMessage,
        const TDesC8& aRemoteName )
    {
    CUpnpIPFilteringDNSQuery* query = CUpnpIPFilteringDNSQuery::NewLC( aMessage,
            aRemoteName );
    iIPFilterRepository->RemoveAddressL( query );
    CleanupStack::Pop( query );
    }

// -----------------------------------------------------------------------------
// CUpnpServiceInfo::RemoveAll
// -----------------------------------------------------------------------------
//
void CUpnpMessageHandlerEngine::RemoveAll()
    {
    if ( iIPFilterRepository->RemoveAll() >=0 )
        {
        iObserver.IPListChange();
        }
    }

// -----------------------------------------------------------------------------
// CUpnpServiceInfo::IsIPAllowed
// -----------------------------------------------------------------------------
//
TBool CUpnpMessageHandlerEngine::IsIPAllowed( const TInetAddr& aAddress ) const
    {
    return iIPFilterRepository->IsAllowed( aAddress );
    }

// -----------------------------------------------------------------------------
// CUpnpServiceInfo::IPFilteringStatus
// -----------------------------------------------------------------------------
//
void CUpnpMessageHandlerEngine::IPFilteringStatus( TInt& aListSize ) const
    {
    aListSize = iIPFilterRepository->GetIPFilterList().Size();
    }

// -----------------------------------------------------------------------------
// CUpnpServiceInfo::GetIPFilterList
// -----------------------------------------------------------------------------
//
const TDesC8& CUpnpMessageHandlerEngine::GetIPFilterList()
    {
    return iIPFilterRepository->GetIPFilterList();
    }
#endif

// -----------------------------------------------------------------------------
// CUpnpMessageHandlerEngine::NetworkEventWlanLost
// -----------------------------------------------------------------------------
//
void CUpnpMessageHandlerEngine::NetworkEvent( CUpnpNetworkEventBase* aEvent )
    {
    LOG_FUNC_NAME;

    if ( aEvent->SubscriberError() < KErrNone && aEvent->Type() == EUnknownEvent )
        {
        LOGS( "MESSAGEHANDLER *** NetworkEvent error" );
        return;
        }

    switch ( aEvent->Type() )
        {
        case EWlanLostEvent:
            {
            InterfaceDown();
  
            TDblQueIter<CSession2>& sessionIter = iObserver.Sessions();
        
            sessionIter.SetToFirst();
            CUpnpMessageHandlerSession *session = 
                    reinterpret_cast<CUpnpMessageHandlerSession*>( sessionIter++ );
            while ( session )
                {
                session->CancelPendingRequests();
                session = reinterpret_cast<CUpnpMessageHandlerSession*>( sessionIter++ );
                }
            }
            break;

        case EAddressChangeEvent:
            {
            CUpnpNetworkEventAddressChange* networkEvent = 
                                static_cast< CUpnpNetworkEventAddressChange* >( aEvent );
  
            TInetAddr addr = networkEvent->Address();

            TRAPD( error, iCurrentSsdpHandler->AddressChangeL( addr ) );
            TRAP( error, iDeviceLibrary->RemoveAllDevicesL() );

            if ( error )
                {
                // only case this should happen
                ASSERT( error == KErrNoMemory );
                }
            }
            break;
        }
    }

// End of File
