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

#include <s32mem.h>

#include "upnpmessagehandlersession.h"
#define KLogFile _L("UpnpMessageHandler.txt")
#include "upnpcustomlog.h"

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CUpnpMessageHandlerSession::CUpnpMessageHandlerSession
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpMessageHandlerSession::CUpnpMessageHandlerSession(
    CUpnpMessageHandler& aMessageHandler, CUpnpMessageHandlerEngine& aEngine ) :
    CSession2(), iMessageHandler(aMessageHandler), iEngine(aEngine),
            iMoreNewDevices(ETrue), iOldId(KErrNotFound), iAddedDevices( 1)
    {
    LOG_FUNC_NAME;
    }

// -----------------------------------------------------------------------------
// CUpnpMessageHandlerSession::~CUpnpMessageHandlerSession
// Destructor.
// -----------------------------------------------------------------------------
//
CUpnpMessageHandlerSession::~CUpnpMessageHandlerSession()
    {
    delete[] iAwaitingUpdateDevices;
    delete[] iAwaitingUpdateServices;

    iMessageHandler.RemoveObserver( this );
    iAddedDevices.Reset();
    }

// -----------------------------------------------------------------------------
// CUpnpMessageHandlerSession::ServiceL
// Handle client requests.
// -----------------------------------------------------------------------------
//
void CUpnpMessageHandlerSession::ServiceL( const RMessage2& aMessage )
    {

    if ( aMessage.Function() != EStartSsdp && aMessage.Function()
            != ECheckError && !iMessageHandler.IsSsdpStarted() )
        {
        aMessage.Complete( KErrServerTerminated );
        return;
        }

    switch ( aMessage.Function() )
        {
        case EStartSsdp:
            {
            aMessage.Complete( EStartSsdpComplete );
            User::LeaveIfError( EventStartSsdpL() );
            iMessageHandler.SetSsdpStarted( ETrue );
            break;
            }
        case EAddLocalDevice:
            {
            EventAddLocalDeviceL( aMessage );
            aMessage.Complete( EAddLocalDeviceComplete );
            break;
            }
        case ERemoveLocalDevice:
            {
            EventRemoveLocalDeviceL( aMessage, EFalse );
            aMessage.Complete( ERemoveLocalDeviceComplete );
            break;
            }
        case ERemoveSilentLocalDevice:
            {
            LOGS("CUpnpMessageHandlerSession:  case ERemoveSilentLocalDevice");
            EventRemoveLocalDeviceL( aMessage, ETrue );
            aMessage.Complete( ERemoveSilentLocalDeviceComplete );
            LOGS("CUpnpMessageHandlerSession:  case ERemoveSilentLocalDevice passed");
            break;
            }
        case EAddControlPointClient:
            {
            EventAddLocalControlPoint( aMessage );
            aMessage.Complete( EAddControlPointClientComplete );
            break;
            }
        case ERemoveControlPointClient:
            {
            EventRemoveLocalControlPoint( aMessage );
            aMessage.Complete( ERemoveControlPointClientComplete );
            break;
            }
        case EStopFilteringDevice:
            {
            EventStopFilteringDeviceL( aMessage );
            aMessage.Complete( EStopFilteringDeviceComplete );
            break;
            }
        case EGetDeviceListSize:
            {
            CopyMessage( (RMessage2&) aMessage );
            EventGetDeviceListSizeL( aMessage );
            break;
            }
        case EGetDeviceList:
            {
            EventGetDeviceListL( aMessage );
            aMessage.Complete( EGetDeviceListComplete );
            break;
            }
        case ESsdpSearch:
            {
            EventSsdpSearchL( aMessage );
            aMessage.Complete( ESsdpSearchComplete );
            break;
            }
        case EActiveIap:
            {
            EventActiveIapL( aMessage );
            aMessage.Complete( EActiveIapComplete );
            break;
            }
        case ECheckError:
            {
            EventCheckErrorL( aMessage );
            aMessage.Complete( ECheckErrorComplete );
            break;
            }
        case ECancelRequest:
            CompletePendingRequest();
            aMessage.Complete( KErrNone );
            break;

#ifdef RD_UPNP_REMOTE_ACCESS    
            case EAddFilterAddrByIP:
                {
                EventAddFilterAddrByIpL( aMessage );
                aMessage.Complete( KErrNone );
                break;
                }
            case EAddFilterAddrByHostname:
                {
                EventAddFilterAddrByHostnameL( aMessage );
                break;
                }
            case ERemoveFilterAddrByIP:
                {
                EventRemoveFilterAddrByIpL( aMessage );
                aMessage.Complete( KErrNone );
                break;
                }
            case ERemoveFilterAddrByHostname:
                {
                EventRemoveFilterAddrByHostnameL( aMessage );
                break;
                }
            case ERemoveAllIPsAddresses:
                {
                EventRemoveAllIPsAddressesL( aMessage );
                aMessage.Complete( KErrNone );
                break;
                }
            case EIsIPAddressAllowed:
                {
                aMessage.Complete( EventIsIPAddressAllowedL( aMessage ) );
                break;
                }
            case ESubscribeIPListChanges:
                {
                EventSubscriveIPListChanges( const_cast<RMessage2&>( aMessage ) );
                break;
                }
            case EGetIPFilterList:
                {
                EventGetIPFilterListL( aMessage );
                break;
                }
            case ECancelIPLIstSubsctiption:
                {
                EventCancelIPListSubscription( aMessage );
                aMessage.Complete( KErrNone );
                break;
                }
#endif    
        default:
            PanicClient( aMessage, EMessageHandlerBadRequest );
            break;
        }

    }

// -----------------------------------------------------------------------------
// CUpnpMessageHandlerSession::PanicClient
// Panic client.
// -----------------------------------------------------------------------------
//
void CUpnpMessageHandlerSession::PanicClient( const RMessage2 &aMessage,
    TInt aPanic ) const
    {
    LOG_FUNC_NAME;
    iMessageHandler.PanicClient( aMessage, aPanic );
    }

// -----------------------------------------------------------------------------
// CUpnpMessageHandlerSession::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpMessageHandlerSession* CUpnpMessageHandlerSession::NewL(
    CUpnpMessageHandler& aServer, CUpnpMessageHandlerEngine& aEngine )
    {
    CUpnpMessageHandlerSession* self = CUpnpMessageHandlerSession::NewLC(
        aServer, aEngine );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpMessageHandlerSession::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpMessageHandlerSession* CUpnpMessageHandlerSession::NewLC(
    CUpnpMessageHandler& aServer, CUpnpMessageHandlerEngine& aEngine )
    {
    CUpnpMessageHandlerSession* self = new (ELeave) CUpnpMessageHandlerSession(
            aServer, aEngine );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpMessageHandlerSession::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpMessageHandlerSession::ConstructL()
    {
    //Add session to MessageHandlers count and to the ObserverList
    iMessageHandler.AddObserverL( this );
    }

// -----------------------------------------------------------------------------
// CUpnpMessageHandlerSession::EventAddLocalDeviceL
// Add local device.
// -----------------------------------------------------------------------------
//
void CUpnpMessageHandlerSession::EventAddLocalDeviceL(
    const RMessage2 &aMessage )
    {
    LOG_FUNC_NAME;

    TUpnpAddLocalDevice device;
    TPtr8 ptr( reinterpret_cast<TUint8*>(&device), sizeof(device ),
        sizeof(device ));

    LOGS("\t MessageHandler, added a local device");

    aMessage.ReadL( 0, ptr );

    HBufC8* buffer = HBufC8::NewLC( device.iBufferLength );
    buffer->Des().SetLength( 0 );

    TPtr8 bufPtr = buffer->Des();

    aMessage.ReadL( 1, bufPtr );

    LOGS("Session::Add");

    iDeviceListSizeRequested = EFalse;

    TPtrC8 uuid = buffer->Left( device.iUuidLength );

    iAddedDevices.AppendL( uuid );

    iEngine.AddLocalDeviceL( device, *buffer );

    CleanupStack::PopAndDestroy( buffer );
    }

// -----------------------------------------------------------------------------
// CUpnpMessageHandlerSession::EventAddLocalControlPointL
// -----------------------------------------------------------------------------
//
void CUpnpMessageHandlerSession::EventAddLocalControlPoint( const RMessage2 &/*aMessage*/)
    {
    iEngine.AddLocalControlPoint();
    }

// -----------------------------------------------------------------------------
// CUpnpMessageHandlerEngine::RemoveLocalControlPointL
// -----------------------------------------------------------------------------
//
void CUpnpMessageHandlerSession::EventRemoveLocalControlPoint(
    const RMessage2 &/*aMessage*/)
    {
    iEngine.RemoveLocalControlPoint();
    }

// -----------------------------------------------------------------------------
// CUpnpMessageHandlerEngine::EventStopFilteringDeviceL
// -----------------------------------------------------------------------------
//
void CUpnpMessageHandlerSession::EventStopFilteringDeviceL(
    const RMessage2& aMessage )
    {
    TInt lenght = aMessage.GetDesLength( 0 );
    HBufC8* uuids = HBufC8::NewLC( lenght );
    TPtr8 uuidsPtr = uuids->Des();
    aMessage.ReadL( 0, uuidsPtr );
    TLex8 uuidLex(*uuids);
    uuidLex.Mark();
    for ( ;; )
        {
        if ( uuidLex.Peek() == ';' )
            {
            TPtrC8 uidPtr = uuidLex.MarkedToken();
            iEngine.StopFilteringDeviceL( uidPtr );
            uuidLex.Inc();
            uuidLex.Mark();
            }
        else if ( uuidLex.Eos() )
            {
            TPtrC8 uidPtr = uuidLex.MarkedToken();
            if ( uidPtr.Length() )
                {
                iEngine.StopFilteringDeviceL( uidPtr );
                }
            break;
            }
        else
            {
            uuidLex.Inc();
            }
        }
    CleanupStack::PopAndDestroy( uuids );
    }

// -----------------------------------------------------------------------------
// CUpnpMessageHandlerSession::EventGetDeviceListSizeL
// Get device list.
// -----------------------------------------------------------------------------
//
void CUpnpMessageHandlerSession::EventGetDeviceListSizeL(
    const RMessage2& aMessage )
    {
    LOGS( "CUpnpMessageHandlerSession::EventGetDeviceListSizeL");

    TPtr8 ptr( reinterpret_cast<TUint8*>(&iSize), sizeof(iSize ),
        sizeof(iSize ));

    iDeviceListSizeMessage.ReadL( 0, ptr );

    if ( iSize.iUpdateId >= iOldId && iOldId != KErrNotFound )
        {
        LOGS( "CUpnpMessageHandlerSession::GetDeviceListSize OLD=NEW");
        iDeviceListSizeRequested = ETrue;
        }

    if ( iSize.iUpdateId == 0 )
        {
        iMoreNewDevices = ETrue;
        }

    if ( !iDeviceListSizeRequested )
        {
        LOGS( "CUpnpMessageHandlerSession::GetDeviceListSize OLD>NEW");
        iDeviceListSizeRequested = ETrue;

        LOGS( "CUpnpMessageHandlerSession::New Devices Found");

        if ( iMoreNewDevices && iDeviceListSizeMessage.Function()
                == EGetDeviceListSize )
            {

            delete[] iAwaitingUpdateDevices;
            iAwaitingUpdateDevices = NULL;

            delete[] iAwaitingUpdateServices;
            iAwaitingUpdateServices = NULL;

            iEngine.DeviceLibrary()->GetUpdate( iSize.iUpdateId, iAwaitingUpdateDevices,
                iAwaitingUpdateServices, iSize.iDeviceCount,
                iSize.iServiceCount );
            if ( iSize.iDeviceCount > 0 )
                {
                iOldId = iSize.iUpdateId;

                iDeviceListSizeMessage.WriteL( 0, ptr );
                aMessage.Complete( EGetDeviceListSizeComplete );
                iDeviceListSizeRequested = EFalse;
                }
            }

        iMoreNewDevices = EFalse;
        }

    }

// -----------------------------------------------------------------------------
// CUpnpMessageHandlerSession::EventGetDeviceListL
// Get device list.
// -----------------------------------------------------------------------------
//
void CUpnpMessageHandlerSession::EventGetDeviceListL(
    const RMessage2 &aMessage )
    {
    LOGS( "CUpnpMessageHandlerSession::EventGetDeviceListL");
    if ( !iDeviceListSizeRequested )
        {
        TPtr8 devPtr( reinterpret_cast<TUint8*>(iAwaitingUpdateDevices),
            sizeof(TUpnpDevice)*iSize.iDeviceCount, sizeof(TUpnpDevice)
                    *iSize.iDeviceCount);
        TPtr8 servPtr( reinterpret_cast<TUint8*>(iAwaitingUpdateServices),
            sizeof(TUpnpService)*iSize.iServiceCount, sizeof(TUpnpService)
                    *iSize.iServiceCount);

        aMessage.WriteL( 0, devPtr );
        aMessage.WriteL( 1, servPtr );

        delete[] iAwaitingUpdateDevices;
        iAwaitingUpdateDevices = NULL;

        delete[] iAwaitingUpdateServices;
        iAwaitingUpdateServices = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CUpnpMessageHandlerSession::EventSsdpSearchL
// SSDP search.
// -----------------------------------------------------------------------------
//
void CUpnpMessageHandlerSession::EventSsdpSearchL( const RMessage2 &aMessage )
    {
    LOGS( "CUpnpMessageHandlerSession::EventSsdpSearchL");

    // getting search string from client.
    HBufC8* search= NULL;
    HBufC8* mx= NULL;

    TInt len = aMessage.GetDesLength( 0 );
    TInt len2 = aMessage.GetDesLength( 1 );

    if ( len > 0 )
        {
        search = HBufC8::NewLC( len );
        search->Des().Zero();

        mx = HBufC8::NewLC( len2 );
        mx->Des().Zero();

        TPtr8 searchPtr = search->Des();

        aMessage.ReadL( 0, searchPtr );
        if ( len2 )
            {
            TPtr8 mxPtr = mx->Des();
            aMessage.ReadL( 1, mxPtr );
            }

        iDeviceListSizeRequested = EFalse;

        iEngine.SsdpSearchL( *search, *mx );

        CleanupStack::PopAndDestroy( mx );
        CleanupStack::PopAndDestroy( search );
        }
    else
        {
        LOGS( "CUpnpMessageHandlerSession::EventSsdpSearchL");
        }
    }

// -----------------------------------------------------------------------------
// CUpnpMessageHandlerSession::EventActiveIapL
// Activate IAP.
// -----------------------------------------------------------------------------
//
void CUpnpMessageHandlerSession::EventActiveIapL( const RMessage2 &aMessage ) const
    {
    LOGS( "CUpnpMessageHandlerSession::EventActiveIapL");
    TInt activeIap = iEngine.ActiveIap();
    TPtr8 ptr( reinterpret_cast<TUint8*>(&activeIap), sizeof(TInt),
        sizeof(TInt));

    aMessage.WriteL( 0, ptr );
    }

// -----------------------------------------------------------------------------
// CUpnpMessageHandlerSession::EventRemoveLocalDeviceL
// Remove local device.
// -----------------------------------------------------------------------------
//
void CUpnpMessageHandlerSession::EventRemoveLocalDeviceL(
    const RMessage2 &aMessage, TBool aSilent )
    {
    LOGS( "CUpnpMessageHandlerSession::EventRemoveLocalDeviceL");
    TBuf8<KMaxUuidLength> uuid;

    aMessage.ReadL( 0, uuid );

    TInt pos( 0);
    TInt error( 0);

    error = iAddedDevices.Find( uuid, pos );
    if ( error == KErrNone )
        {
        iAddedDevices.Delete( pos );
        }

    iDeviceListSizeRequested = EFalse;

    iEngine.RemoveLocalDeviceL( uuid, aSilent );
    }

// -----------------------------------------------------------------------------
// CUpnpMessageHandlerSession::EventStartSsdp
// Start SSPD.
// -----------------------------------------------------------------------------
//
TInt CUpnpMessageHandlerSession::EventStartSsdpL()
    {
    LOGS( "CUpnpMessageHandlerSession::EventStartSsdp");
    return iEngine.StartSsdpL();
    }

// -----------------------------------------------------------------------------
// CUpnpMessageHandlerSession::EventIsSsdpStarted
// Check SSPD.
// -----------------------------------------------------------------------------
//
void CUpnpMessageHandlerSession::EventCheckErrorL( const RMessage2 &aMessage )
    {
    TBool isStarted;
    isStarted = iMessageHandler.IsSsdpStarted();
    TInt error = iMessageHandler.StartingError();
    TPckgBuf<TInt> ptr;

    if ( !isStarted )
        {
        ptr = error;
        aMessage.WriteL( 0, ptr );
        }
    else
        {
        ptr = 0;
        aMessage.WriteL( 0, ptr );
        }

    }

// -----------------------------------------------------------------------------
// CUpnpMessageHandlerSession::DeviceListChangeL
// Device list change.
// -----------------------------------------------------------------------------
//
void CUpnpMessageHandlerSession::DeviceListChangeL()
    {
    LOGS( "CUpnpMessageHandlerSession::DeviceListChange");
    if( iDeviceListSizeRequested && !iDeviceListSizeMessage.IsNull() )
        {
        LOGS( "CUpnpMessageHandlerSession::New Devices Found");

        TPtr8 ptr( reinterpret_cast<TUint8*>(&iSize), sizeof(iSize ),
            sizeof(iSize ));

        iDeviceListSizeMessage.ReadL( 0, ptr );

        delete[] iAwaitingUpdateDevices;
        iAwaitingUpdateDevices = NULL;

        delete[] iAwaitingUpdateServices;
        iAwaitingUpdateServices = NULL;

        iEngine.DeviceLibrary()->GetUpdate( iSize.iUpdateId, iAwaitingUpdateDevices,
            iAwaitingUpdateServices, iSize.iDeviceCount, iSize.iServiceCount );

        iOldId = iSize.iUpdateId;

        iDeviceListSizeMessage.WriteL( 0, ptr );
        iDeviceListSizeMessage.Complete( EGetDeviceListSizeComplete );

        iDeviceListSizeRequested = EFalse;
        }
    else
        {
        iMoreNewDevices = ETrue;
        if ( iOldId != KErrNotFound )
            {
            iOldId++;
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpMessageHandlerSession::CopyMessage
// Copy message.
// -----------------------------------------------------------------------------
//
void CUpnpMessageHandlerSession::CopyMessage( RMessage2& aMessage )
    {
    iDeviceListSizeMessage = aMessage;
    }

// -----------------------------------------------------------------------------
// CUpnpMessageHandlerSession::CancelPendingRequest
// Cancel pending request.
// -----------------------------------------------------------------------------
//
void CUpnpMessageHandlerSession::CompletePendingRequest()
    {
    LOGS( "CUpnpMessageHandlerSession::CancelPendingRequest");
    if ( iDeviceListSizeRequested && !iDeviceListSizeMessage.IsNull() )
        {
        iDeviceListSizeMessage.Complete( EGetDeviceListSizeComplete );

        iDeviceListSizeRequested = EFalse;
        }
    }

#ifdef RD_UPNP_REMOTE_ACCESS
// -----------------------------------------------------------------------------
// CUpnpMessageHandlerSession::EventAddFilterAddrByIpL
// Cancel pending request.
// -----------------------------------------------------------------------------
//
void CUpnpMessageHandlerSession::EventAddFilterAddrByIpL( const RMessage2& aMessage )
    {
    TInetAddr addr;
    TPtr8 ptr( reinterpret_cast<TUint8*>(&addr), sizeof( addr ),
            sizeof( addr ) );

    aMessage.ReadL( 0, ptr );

    iEngine.AddAddressL( addr );
    }

// -----------------------------------------------------------------------------
// CUpnpMessageHandlerSession::EventAddFilterAddrByHostnameL
// Cancel pending request.
// -----------------------------------------------------------------------------
//    
void CUpnpMessageHandlerSession::EventAddFilterAddrByHostnameL( const RMessage2& aMessage )
    {
    TInt lenght( aMessage.GetDesLength(0) );
    HBufC8* hostName = HBufC8::NewLC( lenght );
    TPtr8 hostNamePtr( hostName->Des() );
    aMessage.ReadL( 0, hostNamePtr );

    iEngine.AddAddressL( aMessage, *hostName );

    CleanupStack::PopAndDestroy( hostName );
    }

// -----------------------------------------------------------------------------
// CUpnpMessageHandlerSession::EventRemoveFilterAddrByIpL
// Cancel pending request.
// -----------------------------------------------------------------------------
//
void CUpnpMessageHandlerSession::EventRemoveFilterAddrByIpL( const RMessage2& aMessage )
    {
    TInetAddr addr;
    TPtr8 ptr( reinterpret_cast<TUint8*>(&addr), sizeof( addr ),
            sizeof( addr ) );

    aMessage.ReadL( 0, ptr );

    iEngine.RemoveAddressL( addr );
    }

// -----------------------------------------------------------------------------
// CUpnpMessageHandlerSession::EventRemoveFilterAddrByHostnameL
// Cancel pending request.
// -----------------------------------------------------------------------------
//
void CUpnpMessageHandlerSession::EventRemoveFilterAddrByHostnameL( const RMessage2& aMessage )
    {
    TInt lenght( aMessage.GetDesLength(0) );
    HBufC8* hostName = HBufC8::NewLC( lenght );
    TPtr8 hostNamePtr( hostName->Des() );
    aMessage.ReadL( 0, hostNamePtr );

    iEngine.RemoveAddressL( aMessage, *hostName );

    CleanupStack::PopAndDestroy( hostName );
    }

// -----------------------------------------------------------------------------
// CUpnpMessageHandlerSession::EventRemoveAllIPsAddressesL
// Cancel pending request.
// -----------------------------------------------------------------------------
//    
void CUpnpMessageHandlerSession::EventRemoveAllIPsAddressesL( const RMessage2& /*aMessage*/)
    {
    iEngine.RemoveAll();
    }

// -----------------------------------------------------------------------------
// CUpnpMessageHandlerSession::EventIsIPAddressAllowedL
// Cancel pending request.
// -----------------------------------------------------------------------------
//    
TBool CUpnpMessageHandlerSession::EventIsIPAddressAllowedL( const RMessage2& aMessage)
    {
    TInetAddr addr;
    TPtr8 ptr( reinterpret_cast<TUint8*>(&addr), sizeof( addr ),
            sizeof( addr ) );

    aMessage.ReadL( 0, ptr );

    return iEngine.IsIPAllowed( addr );
    }

// -----------------------------------------------------------------------------
// CUpnpMessageHandlerSession::EventSubscriveIPListChanges
// Cancel pending request.
// -----------------------------------------------------------------------------
//
void CUpnpMessageHandlerSession::EventSubscriveIPListChanges( RMessage2& aMessage )
    {

    TPckgBuf<TInt> clientsSeqNo;
    TPckgBuf<TInt> listSizeBuff;

    aMessage.Read( 0 , clientsSeqNo );

    if( clientsSeqNo() < iSeqNo )
        {
        clientsSeqNo = iSeqNo;
        iEngine.IPFilteringStatus( listSizeBuff() );

        aMessage.Write(0 , clientsSeqNo );
        aMessage.Write(1 , listSizeBuff );
        aMessage.Complete( KErrNone );
        }
    else
        {
        iIPListSizeMessage = aMessage;
        }
    }

// -----------------------------------------------------------------------------
// CUpnpMessageHandlerSession::EventCancelIPListSubscription
// Cancel pending request.
// -----------------------------------------------------------------------------
//
void CUpnpMessageHandlerSession::EventCancelIPListSubscription( const RMessage2& /*aMessage*/)
    {
    if( !iIPListSizeMessage.IsNull() )
        {
        iIPListSizeMessage.Complete( KErrCancel );
        }
    }

void CUpnpMessageHandlerSession::EventGetIPFilterListL( const RMessage2& aMessage )
    {
    const TPtrC8 listBuf( iEngine.GetIPFilterList() );

    if( listBuf.Length()> aMessage.GetDesMaxLength( 0 ) )
        {
        aMessage.Complete( KErrOverflow );
        return;
        }

    aMessage.WriteL( 0 , listBuf );
    aMessage.Complete( KErrNone );

    }
#endif   

// -----------------------------------------------------------------------------
// CUpnpMessageHandlerSession::IPListChange
// Cancel pending request.
// -----------------------------------------------------------------------------
//
void CUpnpMessageHandlerSession::IPListChange()
    {
#ifdef RD_UPNP_REMOTE_ACCESS
    TPckgBuf<TInt> currentSeqNo( iSeqNo++ );
    TPckgBuf<TInt> listSizeBuff;

    if( !iIPListSizeMessage.IsNull())
        {
        iEngine.IPFilteringStatus( listSizeBuff() );

        iIPListSizeMessage.Write(0 , currentSeqNo );
        iIPListSizeMessage.Write(1 , listSizeBuff );

        iIPListSizeMessage.Complete( KErrNone );
        }
#endif    
    }

// -----------------------------------------------------------------------------
// CUpnpMessageHandlerSession::CancelPendingRequests
// Cancel pending request.
// -----------------------------------------------------------------------------
//
void CUpnpMessageHandlerSession::CancelPendingRequests()
    {
    if ( !iDeviceListSizeMessage.IsNull() )
        {
        iDeviceListSizeRequested = EFalse;
        iDeviceListSizeMessage.Complete( KErrCancel );
        }
        
#ifdef RD_UPNP_REMOTE_ACCESS
    if ( !iIPListSizeMessage.IsNull() )
        {
        iIPListSizeMessage.Complete( KErrCancel );
        }
#endif    
    }

// End of File
