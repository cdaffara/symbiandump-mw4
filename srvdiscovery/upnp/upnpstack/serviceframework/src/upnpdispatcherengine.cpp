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
* Description:  Message dispacher engine
*
*/


// INCLUDE FILES

#include "upnpdispatcherengine.h"
#include "upnphttpmessagefactory.h"
#include "upnphttpfileaccess.h"
#include "upnpdispatcherrequest.h"
#include "upnpdispatcherengineobserver.h"
#include "upnpcustomlog.h"
#include "upnphttpserversession.h"
#include "upnphttpserverruntime.h"
#include "upnpsettings.h"

#ifdef _DEBUG
#define KLogFile _L("UPnPStack.txt")
#endif

// CONSTANTS
const TInt KPortBufferLength = 10;
_LIT8(KMSearchConfig,"1,1,5,5,10");

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpDispatcherEngine::CUpnpDispatcherEngine
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpDispatcherEngine::CUpnpDispatcherEngine(
                                      MDispatcherEngineObserver& aObserver )
    : CActive( EPriorityStandard ),
      iAddLocalPtr( NULL, 0 ),
      iObserver( aObserver ),
      iGetDevPtr( NULL, 0 ),
      iGetServPtr( NULL, 0 ),
      iRemoveUuidPtr( NULL, 0 ),
      iIsSessionOpen( EFalse )
    {
    CActiveScheduler::Add( this );
    iDeviceGetState = ENoOperation;
    iAddLocalBuffer = NULL;
    iRemoveUuid = NULL;
    }

// -----------------------------------------------------------------------------
// CUpnpDispatcherEngine::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpDispatcherEngine::ConstructL()
    {
#ifdef _DEBUG
    iReqsInStart = RThread().RequestCount();
    RThread().HandleCount( iProcessHandlesInStart, iThreadHandlesInStart );
#endif // _DEBUG

    User::LeaveIfError( iSession.Connect() );
    iIsSessionOpen = ETrue;
    iSession.RequestStartSsdpL();

    iActiveRequest = NULL;
    iSearch = NULL;

    ConstructHttpL();

    iDeviceGetState = ENoOperation;
    }

// -----------------------------------------------------------------------------
// CUpnpDispatcherEngine::ConstructHttpL
// -----------------------------------------------------------------------------
//
void CUpnpDispatcherEngine::ConstructHttpL()
    {
    TInt iapId = CUpnpSettings::GetIapL();
    iHttpServerSession = CUpnpHttpServerSession::NewL( iapId, *this );    
    }
    
// -----------------------------------------------------------------------------
// CUpnpDispatcherEngine::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpDispatcherEngine* CUpnpDispatcherEngine::NewL(
        MDispatcherEngineObserver& aObserver )
    {
    CUpnpDispatcherEngine* self = NewLC( aObserver );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpDispatcherEngine::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpDispatcherEngine* CUpnpDispatcherEngine::NewLC(
        MDispatcherEngineObserver& aObserver )
    {
    CUpnpDispatcherEngine* self =
        new ( ELeave ) CUpnpDispatcherEngine( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpDispatcherEngine::~CUpnpDispatcherEngine
// -----------------------------------------------------------------------------
//
CUpnpDispatcherEngine::~CUpnpDispatcherEngine()
    {
    // If active, calls DoCancel
    Cancel();

    delete iRemoveUuid;
    iRemoveUuid = NULL;

    delete iAddLocalBuffer;
    iAddLocalBuffer = NULL;

    delete iSearch;
    iSearch = NULL;

    delete iUuids;
    iUuids = NULL;

    delete [] iDevs;
    delete [] iServs;
    
    iSession.Close();
    iIsSessionOpen = EFalse;
    iCustomers.Reset();

    if ( iHttpServerSession )
        {
        iHttpServerSession->Stop();
        delete iHttpServerSession;
        }

    iPendingRequests.ResetAndDestroy();

    iNewDevices.ResetAndDestroy();

    RemoveHandledRequest();

    delete iMXString;
    }

// -----------------------------------------------------------------------------
// CUpnpDispatcherEngine::AddCustomer
// -----------------------------------------------------------------------------
//
void CUpnpDispatcherEngine::AddCustomer( const MUpnpDispatcherCustomer& aCustomer )
    {
    // adds a new customer for CUpnpDispatcherEngine
    if ( iCustomers.Find( &aCustomer ) == KErrNotFound )
        {
        TRAP_IGNORE(iCustomers.AppendL( &aCustomer ));
        }
    }
// -----------------------------------------------------------------------------
// CUpnpDispatcherEngine::RemoveCustomer
// -----------------------------------------------------------------------------
//
void CUpnpDispatcherEngine::RemoveCustomer( const MUpnpDispatcherCustomer& aCustomer )
    {
    TInt pos = iCustomers.Find( &aCustomer );
    if ( pos != KErrNotFound )
        {
        LOGS2("CUpnpDispatcherEngine::RemoveCustomer, removing customer 0x%x from position %d", &aCustomer, pos);
        iCustomers.Remove( pos );
        iCustomers.Compress();
        LOGS("CUpnpDispatcherEngine::RemoveCustomer done");
        }
    else
        LOGS1("CUpnpDispatcherEngine::RemoveCustomer, customer 0x%x not found", &aCustomer);
    }

// -----------------------------------------------------------------------------
// CUpnpDispatcherEngine::SendMessageL
// -----------------------------------------------------------------------------
//
void CUpnpDispatcherEngine::SendMessageL( CUpnpHttpMessage* aMessage )
    {
    // Sends a Http message
    iHttpServerSession->SendMessageL( aMessage );
    }

// -----------------------------------------------------------------------------
// CUpnpDispatcherEngine::UpdateDevicesL
// -----------------------------------------------------------------------------
//
void CUpnpDispatcherEngine::UpdateDevicesL( TInt aUpdateId )
    {
    // checking if new list size is called.
    if ( iDeviceGetState != EGetListSize )
        {
        if ( IsActive() )
            {
            return;
            }

        // setting information for message handler
        // iSize contains information about last known
        // update id. On return, it will contain also
        // number of new devices and servises.

        if( iSize.iUpdateId > iOldUpdateId )
            {
            iDeviceGetState = EGetList;
            SetActive();
            iStatus = KRequestPending;
            TRequestStatus* stat = &iStatus;
            User::RequestComplete( stat, KErrNone );
            }
        else
            {
            iSize.iUpdateId = aUpdateId;
            iOldUpdateId = aUpdateId;

            iStatus = KRequestPending;
            // connection restoring
            if(!iIsSessionOpen)
                {
                User::LeaveIfError( iSession.Connect() );
                iIsSessionOpen = ETrue;
                }

            iSession.RequestGetDeviceListSize( iStatus, iSize );
            if( !IsActive() )
                {
                SetActive();
                }
                iDeviceGetState = EGetList;
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpDispatcherEngine::GetDevicesL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CUpnpDispatcherEngine::TDispatcherRequest CUpnpDispatcherEngine::GetDevicesL()
    {
    LOG_FUNC_NAME;

    TDispatcherRequest status( ENewDevices );
    if ( iDevs )
        {
        delete [] iDevs;
        iDevs = NULL;
        }

    if ( iServs )
        {
        delete [] iServs;
        iServs = NULL;
        }

    // checking if new devices or services has been found.
    if ( iSize.iUpdateId > iOldUpdateId )
        {
        if( IsActive() )
            {
            Cancel();
            }

        // constructing buffers for getting the information
        // for new devices and services.
        // iDevs is an array, that will have number of
        // devices as we have just received from Message Handler
        // Same with iServs for services.

        iDevs = new (ELeave) TUpnpDevice[iSize.iDeviceCount];
        iServs = new (ELeave) TUpnpService[iSize.iServiceCount];

        TPtr8 devPtr( reinterpret_cast<TUint8*>( iDevs ),
            iSize.iDeviceCount*sizeof( TUpnpDevice ),
            iSize.iDeviceCount*sizeof( TUpnpDevice ) );
        TPtr8 servPtr( reinterpret_cast<TUint8*>( iServs ),
            iSize.iServiceCount*sizeof( TUpnpService ),
            iSize.iServiceCount*sizeof( TUpnpService ) );

        // setting member pointers to point to strictures that will be
        // filled with device and service information as call returns.
        iGetDevPtr.Set(devPtr);
        iGetServPtr.Set(servPtr);

        // connection restoring
        if(!iIsSessionOpen)
            {
            User::LeaveIfError( iSession.Connect() );
            iIsSessionOpen = ETrue;
            }

        iStatus = KRequestPending;
        iSession.RequestGetDeviceList( iStatus, iGetDevPtr, iGetServPtr );
        if( !IsActive() )
            {
            SetActive();
            }

        status = ENewDevices;
        }
    else
        {
        // if no new devices, return the request for Message Handler,
        // so it can complete message when there will be new devices.
        UpdateDevicesL(iSize.iUpdateId);

        status = EDeviceList;
        }

    return status;
    }

// -----------------------------------------------------------------------------
// CUpnpDispatcherEngine::DevicesReceivedL
// -----------------------------------------------------------------------------
//
TInt CUpnpDispatcherEngine::DevicesReceivedL()
    {
    if( iDevs && iServs)
        {
        TInt servIndex = 0;

        // parsing data to devices and services.
        for ( TInt i = 0; i < iSize.iDeviceCount; i++ )
            {
            RPointerArray<TUpnpService> currServs;
            CleanupClosePushL( currServs );
            for ( TInt j = 0; j < iDevs[i].iServiceCount; j++ )
                {
                currServs.AppendL( (const TUpnpService*)&iServs[servIndex+j] );
                }
            CleanupStack::Check(&currServs);
            servIndex += iDevs[i].iServiceCount;

            // creating CUpnpDevice objects from this information.

            CUpnpDevice* device = CUpnpDevice::NewL( &iDevs[i], currServs );
            CleanupStack::Check(&currServs);
            device->SetAlive( iDevs[i].iAlive );
            CleanupStack::Check(&currServs);
            currServs.Reset();
            CleanupStack::Pop( &currServs );

            iNewDevices.AppendL( device );
            }

        delete[] iDevs;
        iDevs = NULL;

        delete[] iServs;
        iServs = NULL;

        iOldUpdateId = iSize.iUpdateId;

        // inform CUpnpDispatcher that new devices and services has been found.
        iObserver.DeviceListReceivedL(iOldUpdateId);
        }
    //The UpdateId should be passed back to the DispatcherEngine
    //(Through Observer)
    return iOldUpdateId;
    }

// -----------------------------------------------------------------------------
// CUpnpDispatcherEngine::NewDevices
// -----------------------------------------------------------------------------
//
const RPointerArray<CUpnpDevice>& CUpnpDispatcherEngine::NewDevices() const
    {
    return iNewDevices;
    }

// -----------------------------------------------------------------------------
// CUpnpDispatcherEngine::RemoveDevices
// -----------------------------------------------------------------------------
//
void CUpnpDispatcherEngine::RemoveDevices()
    {
    iNewDevices.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CUpnpDispatcherEngine::AddLocalDeviceL
// -----------------------------------------------------------------------------
//
void CUpnpDispatcherEngine::AddLocalDeviceL( const TDesC8& aUuid,
                                             const TDesC8& aDeviceType,
                                             const TDesC8& aDescriptionPath,
                                             const CDesC8Array& aServices,
                                             const TBool aIsRootDevice )
    {
    // checking if client is busy.
    if ( iDeviceGetState != EOtherOperation &&
        iDeviceGetState != EListReceived)
        {
        if ( IsActive() )
            {
            Cancel();
            }

        TBuf8<KPortBufferLength> port;
        port.Num( HttpServerAddress().Port() );

        _LIT8( KRootDevice, "r" );

        TInt bufLen = aUuid.Length() +
            aDeviceType.Length() +
            aDescriptionPath.Length() +
            port.Length() + KRootDevice().Length();

        TInt i=0;
        for ( i = 0; i < aServices.Count(); i++ )
            {
            bufLen += aServices[i].Length();
            }

        if(iAddLocalBuffer)
            {
            delete iAddLocalBuffer;
            iAddLocalBuffer = NULL;
            }

        // creating a buffer that will hold information about device
        // that is to be added.

        iAddLocalBuffer = HBufC8::NewL( bufLen );
        iAddLocalPtr.Set( iAddLocalBuffer->Des() );

        // iAddLocalDevice is a struct, that will hold length of each
        // data in the buffer. When we have length of each data and
        // the data buffer, we can parse the data back to separate
        // buffers on server side.

        iAddLocalDevice.iUuidLength = aUuid.Length();
        iAddLocalDevice.iDeviceTypeLength = aDeviceType.Length();
        iAddLocalDevice.iDescriptionUrlLength = aDescriptionPath.Length();
        iAddLocalDevice.iDomainLength = 0;
        iAddLocalDevice.iPortNumberLength = port.Length();
        iAddLocalDevice.iRootDeviceLength = 0;

        iAddLocalPtr.Append( aUuid );
        iAddLocalPtr.Append( aDeviceType );
        iAddLocalPtr.Append( aDescriptionPath );
        iAddLocalPtr.Append( port );

                if ( aIsRootDevice )
                    {
                    iAddLocalPtr.Append( KRootDevice() );
                    iAddLocalDevice.iRootDeviceLength = KRootDevice().Length();
                    }

        for ( i = 0; i < aServices.Count(); i++ )
            {
            iAddLocalDevice.iServiceLength[i] = aServices[i].Length();
            iAddLocalPtr.Append( aServices[i] );
            }

        for ( ; i < KMaxServiceCount; i++ )
            {
            iAddLocalDevice.iServiceLength[i] = 0;
            }

        iAddLocalDevice.iBufferLength = iAddLocalPtr.Length();

        // connection restoring
        if(!iIsSessionOpen)
            {
            User::LeaveIfError( iSession.Connect() );
            iIsSessionOpen = ETrue;
            }

        iStatus = KRequestPending;
        iSession.RequestAddLocalDevice( iStatus, iAddLocalDevice,
            iAddLocalPtr );
        if( !IsActive() )
            {
            SetActive();
            }

        iDeviceGetState = EOtherOperation;
        }
    else
        {
        // if client is busy, making a request to request queue.
        CUpnpDispatcherRequest* request = CUpnpDispatcherRequest::NewLC(
            EAddLocalDevice );

        request->AddArgumentL( aUuid );
        request->AddArgumentL( aDeviceType );
        request->AddArgumentL( aDescriptionPath );

        _LIT8( KRootDevice, "r" );

        if ( aIsRootDevice )
            {
            request->AddArgumentL( KRootDevice );
            }
        else
            {
            request->AddArgumentL( KNullDesC8 );
            }

        TInt granularity = aServices.Count();
        if( 0 == granularity )
            {
            granularity = 1;
            }
        CDesC8ArrayFlat* array = new (ELeave) CDesC8ArrayFlat( granularity );

        CleanupStack::PushL(array);

        for ( TInt k = 0; k < aServices.Count(); k++ )
            {
            array->AppendL( aServices[k] );
            }

        CleanupStack::Pop(array);
        request->AddServices(array);

        CleanupStack::Pop(request);
        iPendingRequests.AppendL(request);
        }
    }

// -----------------------------------------------------------------------------
// CUpnpDispatcherEngine::AddControlPointClientL
// -----------------------------------------------------------------------------
//
void CUpnpDispatcherEngine::AddControlPointClientL()
    {
    // checking if client is busy.
    if ( iDeviceGetState != EOtherOperation &&
        iDeviceGetState != EListReceived )
        {
        if ( IsActive() )
            {
            Cancel();
            }

        // connection restoring
        if ( !iIsSessionOpen )
            {
            User::LeaveIfError( iSession.Connect() );
            iIsSessionOpen = ETrue;
            }

        iStatus = KRequestPending;
        iSession.RequestAddControlPoint( iStatus );

        if ( !IsActive() )
            {
            SetActive();
            }
        iDeviceGetState = EOtherOperation;
        }
    else
        {
        // client is busy, making a request and adding it to queue.
        CUpnpDispatcherRequest* request =
            CUpnpDispatcherRequest::NewLC( EAddControlPointClient );
        iPendingRequests.AppendL( request );
        CleanupStack::Pop( request );
        }
    }

void CUpnpDispatcherEngine::RemoveControlPointClientL()
    {
    // checking if client is busy.
    if ( iDeviceGetState != EOtherOperation &&
        iDeviceGetState != EListReceived )
        {
        if ( IsActive() )
            {
            Cancel();
            }

        // connection restoring
        if ( !iIsSessionOpen )
            {
            User::LeaveIfError( iSession.Connect() );
            iIsSessionOpen = ETrue;
            }

        iStatus = KRequestPending;
        iSession.RequestRemoveControlPoint( iStatus );

        if ( !IsActive() )
            {
            SetActive();
            }
        iDeviceGetState = EOtherOperation;
        }
    else
        {
        // client is busy, making a request and adding it to queue.
        CUpnpDispatcherRequest* request =
                CUpnpDispatcherRequest::NewLC( ERemoveControlPointClient );
        iPendingRequests.AppendL( request );
        CleanupStack::Pop( request );
        }
    }

// -----------------------------------------------------------------------------
// CUpnpDispatcherEngine::SsdpSearchL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpDispatcherEngine::SsdpSearchL( const TDesC8& aString )
    {
    if(!iMXString)
        {
        TRAPD(err,iMXString = CUpnpSettings::GetMSearchConfigurationL());
        if(err == KErrNotFound)
            {
            iMXString = KMSearchConfig().AllocL();
            }
        else
            {
            User::LeaveIfError(err);
            }
        }

    if( aString.Length() <= 0 )
        {
        return;
        }

    // checking if client is busy.
    if ( iDeviceGetState != EOtherOperation &&
        iDeviceGetState != EListReceived )
        {
        if ( IsActive() )
            {
            Cancel();
            }

        if( iSearch )
            {
            delete iSearch;
            iSearch = NULL;
            }

        iSearch = HBufC8::NewL( aString.Length() );
        iSearch->Des().Zero();
        iSearch->Des().Append( aString );
        // connection restoring
        if(!iIsSessionOpen)
            {
            User::LeaveIfError( iSession.Connect() );
            iIsSessionOpen = ETrue;
            }

        iStatus = KRequestPending;
        iSession.RequestSsdpSearch( iStatus, *iSearch, *iMXString );

        if( !IsActive() )
            {
            SetActive();
            }

        iDeviceGetState = EOtherOperation;
        }
    else
        {
        // client is busy, making a request and adding it to queue.
        CUpnpDispatcherRequest* request = CUpnpDispatcherRequest::NewLC( ESsdpSearch );

        request->AddArgumentL( aString );

        CleanupStack::Pop(request);
        iPendingRequests.AppendL(request);
        }
    }

// -----------------------------------------------------------------------------
// CUpnpDispatcherEngine::SsdpStopFilteringL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpDispatcherEngine::SsdpStopFilteringL( const TDesC8& aString )
    {
    if( aString.Length() <= 0 )
        {
        return;
        }
    if( iUuids )
        {
        delete iUuids;
        iUuids = NULL;
        }
    iUuids = HBufC8::NewL( aString.Length() );
    iUuids->Des().Zero();
    iUuids->Des().Append( aString );
    // connection restoring
    if(!iIsSessionOpen)
        {
        User::LeaveIfError( iSession.Connect() );
        iIsSessionOpen = ETrue;
        }
    iSession.RequestStopFiltering( *iUuids );
    }

// -----------------------------------------------------------------------------
// CUpnpDispatcherEngine::HttpEventLD
// -----------------------------------------------------------------------------
//
void CUpnpDispatcherEngine::HttpEventLD( CUpnpHttpMessage* aMessage )
    {
    if( !aMessage )
        {
        return;
        }
    CleanupStack::PushL( aMessage );

    HBufC8* customerPath = aMessage->ServicePath().AllocLC();
    for ( TInt i = 0; i < iCustomers.Count(); i++ )
        {
        MUpnpDispatcherCustomer* customer = iCustomers[i];
        if ( customerPath->CompareF( customer->Path() ) == 0 )
            {
            CleanupStack::PopAndDestroy(customerPath);
            CleanupStack::Pop( aMessage );
            customer->MessageReceivedLD( aMessage );
            return;
            }
        }
    CleanupStack::PopAndDestroy(customerPath);
    
    if ( aMessage->Type() <= ETransferStart )
        {
        CUpnpHttpMessage* reply = RUpnpHttpMessageFactory::HttpResponseErrorL(
                                                        aMessage->Sender(), EHttpBadRequest );
        CleanupStack::PushL( reply );
        reply->SetSessionId( aMessage->SessionId() );
        SendMessageL( reply );
        CleanupStack::PopAndDestroy( reply );
        }
    CleanupStack::PopAndDestroy( aMessage );
    }

// -----------------------------------------------------------------------------
// CUpnpDispatcherEngine::HttpServerAddressL
// -----------------------------------------------------------------------------
//
TInetAddr CUpnpDispatcherEngine::HttpServerAddress()
    {
    TInetAddr addr;
    iHttpServerSession->GetAddress( addr );
    return addr;
    }

// CUpnpDispatcherEngine::RemoveLocalDeviceL
// -----------------------------------------------------------------------------
//
void CUpnpDispatcherEngine::RemoveLocalDeviceL( const TDesC8& aUuid, TBool aSilent )
    {
    // checking if client is busy.
    if ( iDeviceGetState != EOtherOperation &&
         iDeviceGetState != EListReceived )
        {
        if ( IsActive() )
            {
            Cancel();
            }

        if(iRemoveUuid)
            {
            delete iRemoveUuid;
            iRemoveUuid = NULL;
            }

        iRemoveUuid = aUuid.AllocL();
        iRemoveUuidPtr.Set(iRemoveUuid->Des());

        // connection restoring
        if(!iIsSessionOpen)
            {
            User::LeaveIfError( iSession.Connect() );
            iIsSessionOpen = ETrue;
            }

        iStatus = KRequestPending;

        if (!aSilent)
            {
            iSession.RequestRemoveLocalDevice( iStatus, iRemoveUuidPtr );
            }
        else
            {
            iSession.RequestRemoveSilentLocalDevice( iStatus, iRemoveUuidPtr );
            }
        iDeviceGetState = EOtherOperation;

        if( !IsActive() )
            {
            SetActive();
            }
        }
    else
        {
        // client is busy, making a request and adding it to queue.
        CUpnpDispatcherRequest* request = CUpnpDispatcherRequest::NewLC(
            ERemoveLocalDevice );

        request->AddArgumentL( aUuid );

        // we need to store information if the removal should be silent
        if ( aSilent )
            {
            request->AddArgumentL( KNullDesC8 );
            }

        CleanupStack::Pop(request);
        iPendingRequests.AppendL(request);
        }
    }

// -----------------------------------------------------------------------------
// CUpnpDispatcherEngine::MacAddressL
// -----------------------------------------------------------------------------
//
const HBufC8* CUpnpDispatcherEngine::MacAddressL()
    {
    TSockAddr* addr;
    addr = iHttpServerSession->HWAddressL();
    CleanupStack::PushL( addr);
    TInt i = 0;
    HBufC8* hwAddr = NULL;
    hwAddr = HBufC8::NewL( 2 * addr->GetUserLen() );
    hwAddr->Des().Zero();

    for ( i = sizeof(SSockAddr); i < sizeof(SSockAddr) + addr->GetUserLen(); i++ )
        {
        hwAddr->Des().AppendNumFixedWidth((*addr)[i], EHex, 2);
        }

    CleanupStack::PopAndDestroy( addr);

    hwAddr->Des().UpperCase();

    return hwAddr;
    }

// -----------------------------------------------------------------------------
// CUpnpDispatcherEngine::RunL
// -----------------------------------------------------------------------------
//
void CUpnpDispatcherEngine::RunL()
    {
    if ( iStatus.Int() == KErrServerTerminated || iStatus.Int() == KErrCancel )
        {
        LOGS("CUpnpDispatcherEngine::RunL, Server terminated or canceled, trying to restart");
        iSession.Close();
        iIsSessionOpen = EFalse;
        return;
        }

    switch( iDeviceGetState )
        {
        case EOtherOperation:
            if ( iPendingRequests.Count() > 0 )
                {
                HandlePendingRequestL();
                }
            else
                {
                UpdateDevicesL(iOldUpdateId);
                }
            break;
        case EGetListSize:
            UpdateDevicesL(iOldUpdateId);
            break;
        case EGetList:
            if( GetDevicesL() == EDeviceList)
                {
                iDeviceGetState = EGetList;
                }
            else
                {
                iDeviceGetState = EListReceived;
                }

            break;
        case EListReceived:
            if( iSize.iUpdateId >= iOldUpdateId )
                {
                DevicesReceivedL();
                }
            if( iPendingRequests.Count() > 0 && !IsActive())
                {
                iDeviceGetState = EOtherOperation;
                //Re-execute RunL to launch HandlePendingRequest
                SetActive();
                iStatus = KRequestPending;
                TRequestStatus* stat = &iStatus;
                User::RequestComplete( stat, KErrNone );
                break;
                }
            UpdateDevicesL(iOldUpdateId);

            break;
        default: //NoOperation
            break;
        }
    }

// -----------------------------------------------------------------------------
// CUpnpDispatcherEngine::RunError
// RunError in case RunL leaves.
// -----------------------------------------------------------------------------
//
TInt CUpnpDispatcherEngine::RunError( TInt /*aError*/ )
    {
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CUpnpDispatcherEngine::DoCancel
// -----------------------------------------------------------------------------
//
void CUpnpDispatcherEngine::DoCancel()
    {
    TRAP_IGNORE( StopRequestL() );

    if(iDevs)
        {
        delete [] iDevs;
        iDevs = NULL;
        }
    if(iServs)
        {
        delete [] iServs;
        iServs = NULL;
        }

    iDeviceGetState = ENoOperation;
    }

// -----------------------------------------------------------------------------
// CUpnpDispatcherEngine::HandlePendingRequestL
// -----------------------------------------------------------------------------
//
void CUpnpDispatcherEngine::HandlePendingRequestL()
    {
    ASSERT( iPendingRequests.Count() > 0 );
    RemoveHandledRequest();
    iActiveRequest = iPendingRequests[0];
    CDesC8ArrayFlat& arguments = iActiveRequest->Arguments();
    switch( iActiveRequest->Request() )
        {
        case EAddLocalDevice:
            {
            if( iDeviceGetState == EOtherOperation )
                {
                iDeviceGetState = ENoOperation;
                }

            if ( arguments[3].Length() > 0 )
                {
                AddLocalDeviceL(
                                arguments[0],
                                arguments[1],
                                arguments[2],
                                iActiveRequest->Services(),
                        ETrue );
                }
            else
                {
                AddLocalDeviceL(
                                arguments[0],
                                arguments[1],
                                arguments[2],
                                iActiveRequest->Services(),
                                EFalse );
                }

            }
            break;

        case ERemoveLocalDevice:
            {

            if( iDeviceGetState == EOtherOperation )
                {
                iDeviceGetState = ENoOperation;
                }

            // the presence of 2nd argument means the removal should be silent
            TBool silent = EFalse;
            if ( arguments.Count() == 2 )
                {
                silent = ETrue;
                }
            RemoveLocalDeviceL( arguments[0], silent );
            }
            break;

        case EAddControlPointClient:
            {
            if( iDeviceGetState == EOtherOperation )
                {
                iDeviceGetState = ENoOperation;
                }
                AddControlPointClientL();
            }
            break;

        case ERemoveControlPointClient:
            {
            if( iDeviceGetState == EOtherOperation )
                {
                iDeviceGetState = ENoOperation;
                }

            RemoveControlPointClientL();
            }
            break;

        case ESsdpSearch:
            {

            if( iDeviceGetState == EOtherOperation )
                {
                iDeviceGetState = ENoOperation;
                }

            SsdpSearchL( arguments[0] );

            }
        default:
            break;
        }
    iPendingRequests.Remove( 0 );
    iPendingRequests.Compress();
    iDeviceGetState = EOtherOperation;
    }

// -----------------------------------------------------------------------------
// CUpnpDispatcherEngine::RemoveHandledRequest
// -----------------------------------------------------------------------------
//
TInt CUpnpDispatcherEngine::RemoveHandledRequest()
    {
    if(iActiveRequest)
        {
        // If last message was ssdp search, now it is safe
        // to delete iSearch also.
        if( iActiveRequest->Request() == ESsdpSearch )
            {
            if( iSearch )
                {
                delete iSearch;
                iSearch = NULL;
                }
            }
        delete iActiveRequest;
        iActiveRequest = NULL;
        return KErrNone;
        }
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CUpnpDispatcherEngine::StopRequestL
// -----------------------------------------------------------------------------
//
void CUpnpDispatcherEngine::StopRequestL()
    {
    //LOGSH( iHandle, "Disp Request Cancelled");

    if( iDeviceGetState == EGetList )
        {
        // connection restoring
        if(!iIsSessionOpen)
            {
            User::LeaveIfError( iSession.Connect() );
            iIsSessionOpen = ETrue;
            }

        iDeviceGetState = ENoOperation;
        iSession.RequestStop();
        }
    }

// -----------------------------------------------------------------------------
// CUpnpDispatcherEngine::SetTransactionCreator
// -----------------------------------------------------------------------------
//
void CUpnpDispatcherEngine::SetTransactionCreator( 
          MUpnpHttpServerTransactionCreator* aTransactionCreator )
    {
    iHttpServerSession->DefaultRuntime().SetCreator( *aTransactionCreator );
    }

// -----------------------------------------------------------------------------
// CUpnpDispatcher::StartHttpServerL
// -----------------------------------------------------------------------------
//
void CUpnpDispatcherEngine::StartHttpServerL( TBool aRegister , const TInt aPort )
    {
    iHttpServerSession->StartL( aPort );    
    CheckErrorL( aRegister );
    }

// -----------------------------------------------------------------------------
// CUpnpDispatcher::CheckErrorL
// -----------------------------------------------------------------------------
//
void CUpnpDispatcherEngine::CheckErrorL( TBool /*aRegister*/ )
    {
    TInt error = KErrNone;
    iSession.RequestCheckError(error);
    User::LeaveIfError(error);
    }

// -----------------------------------------------------------------------------
// CUpnpDispatcher::StopHttpServer
// -----------------------------------------------------------------------------
//
void CUpnpDispatcherEngine::StopHttpServer()
    {
    iHttpServerSession->Stop();
    }

//  End of File
