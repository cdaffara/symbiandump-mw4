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
* Description:  Defines the CUpnpDevice class
*
*/



// INCLUDE FILES

#include <e32math.h>

#include "upnpfileutils.h"
#include "upnpicon.h"
#include "upnpdevice.h"
#include "upnpcommonstructs.h"
#include "upnpstring.h"
#include "upnpdispatcher.h"
#include "upnpcons.h"
#include "upnpcommonupnplits.h"
#include "upnpcustomlog.h"
#include "upnpserviceimplementation.h"
#include "upnpdevicecontenthandler.h"

#define KLogFile _L("UPnPStack.txt")

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpDevice::SetUuidL
// -----------------------------------------------------------------------------
//
void CUpnpDevice::SetUuidL( const TDesC8& aUUID )
    {
    HBufC8* tmp = aUUID.AllocL();
    delete iUUID;
    iUUID = tmp;
    }

// -----------------------------------------------------------------------------
// CUpnpDevice::AddDeviceL
// -----------------------------------------------------------------------------
//
void CUpnpDevice::AddDeviceL( CUpnpDevice* device )
    {
    device->SetRootDevice( EFalse );
    iDeviceList.AppendL( device );
    }

// -----------------------------------------------------------------------------
// CUpnpDevice::CUpnpDevice
// -----------------------------------------------------------------------------
//
CUpnpDevice::CUpnpDevice() :
    iExpired(ETrue), iNetworkType(EHomeNetwork), iIsRootDevice(ETrue)
    {
    }

// -----------------------------------------------------------------------------
// CUpnpDevice::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpDevice::ConstructL( const TUpnpDevice* aDevice,
    const RPointerArray<TUpnpService>& aServices )
    {
    Init();

    if ( aDevice )
        {
        iUUID = aDevice->iUUID.AllocL();
        iDeviceType = aDevice->iDeviceType.AllocL();
        iDescriptionURL = aDevice->iDescriptionURL.AllocL();
        iDomain = aDevice->iDomain.AllocL();
        iExpired = aDevice->iExpired;
        iAlive = aDevice->iAlive;

        if ( aDevice->iLocal )
            {
            iNetworkType = ELocalDevice;
            }
        else if ( aDevice->iRemote )
            {
            iNetworkType = ERemoteDevice;
            }
        else
            {
            iNetworkType = EHomeNetwork;
            }

        iServiceTypes = new (ELeave) CDesC8ArrayFlat( aServices.Count()
                == 0 ? 1 : aServices.Count() );

        for ( TInt i = 0; i < aServices.Count(); i++ )
            {
            iServiceTypes->AppendL( aServices[i]->iServiceType );
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpDevice::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpDevice* CUpnpDevice::NewL( const TUpnpDevice* aDevice,
        const RPointerArray<TUpnpService> & aServices
)
    {
    CUpnpDevice* self = new (ELeave) CUpnpDevice();

    CleanupStack::PushL( self );
    self->ConstructL( aDevice, aServices );
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpDevice::~CUpnpDevice
// Destructor.
// -----------------------------------------------------------------------------
//
CUpnpDevice::~CUpnpDevice()
    {
    iServiceDescriptionSessionIds.Reset();
    iServiceDescriptionSessionIds.Close();

    if ( iServiceTypes )
        {
        iServiceTypes->Reset();
        delete iServiceTypes;
        }
    if ( iServicesId )
        {
        iServicesId->Reset();
        delete iServicesId;
        }

    delete iUUID;
    delete iDeviceType;
    delete iDescriptionURL;
    delete iDomain;

    CleanupServiceArray();

    // destroy embedded devices
    iDeviceList.ResetAndDestroy();
    iDeviceList.Close();

    delete iClockSeq;

    iIcons.ResetAndDestroy();
    iIcons.Close();

    iServiceDescriptionSessionIds.Reset();
    iServiceDescriptionSessionIds.Close();
    iIconSessionIds.Reset();
    iIconSessionIds.Close();

    delete iUrlBase;

    iProperties.ResetAndDestroy();
    iProperties.Close();
    }

// -----------------------------------------------------------------------------
// CUpnpDevice::Init
// Initialisation.
// -----------------------------------------------------------------------------
//
void CUpnpDevice::Init()
    {
    iAddress = TInetAddr( INET_ADDR( 0,0,0,0 ), 0 );
    iIconReceiveState = EAllIconsAdded;
    }

// -----------------------------------------------------------------------------
// CUpnpDevice::CleanupServiceArray
// -----------------------------------------------------------------------------
//
void CUpnpDevice::CleanupServiceArray()
    {
    for ( TInt i = iServiceList.Count() - 1; i >= 0; --i )
        {
        delete iServiceList[ i ];
        iServiceList[ i ] = NULL;
        }
    iServiceList.Close();
    }

// -----------------------------------------------------------------------------
// CUpnpDevice::DescriptionUrl
// Return description URL.
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CUpnpDevice::DescriptionUrl() const
    {
    if ( iDescriptionURL )
        {
        return *iDescriptionURL;
        }
    else
        {
        return KNullDesC8();
        }
    }

// -----------------------------------------------------------------------------
// CUpnpDevice::SetDescriptionUrlL
// Return description URL.
// -----------------------------------------------------------------------------
//
void CUpnpDevice::SetDescriptionUrlL( const TDesC8& aDescriptionUrl )
    {
    HBufC8* tmp = aDescriptionUrl.AllocL();
    delete iDescriptionURL;
    iDescriptionURL = tmp;
    }

// -----------------------------------------------------------------------------
// CUpnpDevice::SetAlive
// Set alive.
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpDevice::SetAlive( TBool aAlive )
    {
    iAlive = aAlive;
    }

// -----------------------------------------------------------------------------
// CUpnpDevice::Expired
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CUpnpDevice::Expired() const
    {
    return iExpired;
    }

// -----------------------------------------------------------------------------
// CUpnpDevice::SetExpired
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpDevice::SetExpired( TBool aExpired )
    {
    iExpired = aExpired;
    }

// -----------------------------------------------------------------------------
// CUpnpDevice::DeviceType
// Return device type.
// -----------------------------------------------------------------------------
//
EXPORT_C const TPtrC8 CUpnpDevice::DeviceType()
    {
    if ( iDeviceType )
        {
        return TPtrC8( iDeviceType->Des() );
        }
    else
        {
        return KNullDesC8();
        }
    }

// -----------------------------------------------------------------------------
// CUpnpDevice::Uuid
// Return UUID.
// -----------------------------------------------------------------------------
//
EXPORT_C const TPtrC8 CUpnpDevice::Uuid()
    {
    if ( iUUID )
        {
        return TPtrC8( iUUID->Des() );
        }
    else
        {
        return KNullDesC8();
        }
    }

// -----------------------------------------------------------------------------
// CUpnpDevice::Alive
// Return iAlive.
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CUpnpDevice::Alive() const
    {
    return iAlive;
    }

// -----------------------------------------------------------------------------
// CUpnpDevice::DescriptionUrlAddressL
// Return description URL address.
// -----------------------------------------------------------------------------
//
EXPORT_C TInetAddr CUpnpDevice::DescriptionUrlAddressL() const
    {
    if ( iAddress == TInetAddr( INET_ADDR( 0,0,0,0 ), 0) )
        {

        const TDesC8& url = DescriptionUrl();

        if ( url.Length()> KHttp().Length() )
            {
            TPtrC8 addrAndPath = url.Right( url.Length() - KHttp().Length() );

            TInt index = addrAndPath.Find( KSepar() );

            if ( index == KErrNotFound )
                {
                index = addrAndPath.Find( KSlash8() );
                }

            if ( index == KErrNotFound )
                {
                return TInetAddr( INET_ADDR( 0,0,0,0 ), 0 );
                }

            TPtrC8 addr = addrAndPath.Left( index );

            TInetAddr address;

            HBufC* addrBuf = HBufC::NewLC(addr.Length());
            addrBuf->Des().Copy(addr);
            address.Input(*addrBuf);
            CleanupStack::PopAndDestroy(addrBuf);

            return address;
            }

        return TInetAddr(INET_ADDR( 0,0,0,0 ), 0 );
        }

    return iAddress;
    }

// -----------------------------------------------------------------------------
// CUpnpDevice::DescriptionUrlPath
// Return description URL path.
// -----------------------------------------------------------------------------
//
EXPORT_C const TPtrC8 CUpnpDevice::DescriptionUrlPath() const
    {

    const TDesC8& url = DescriptionUrl();

    if ( url.Length()> KHttp().Length() )
        {
        TPtrC8 addrAndPath = url.Right( url.Length() - KHttp().Length() );

        TInt index = addrAndPath.Find( KSlash8() );

        if ( index != KErrNotFound )
            {
            TPtrC8 addr = addrAndPath.Right( addrAndPath.Length() - index );
            return addr;
            }
        }
    return TPtrC8( KNullDesC8 );
    }

// -----------------------------------------------------------------------------
// CUpnpDevice::DescriptionUrlPort
// Retrun description URL port.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpDevice::DescriptionUrlPort() const
    {
    if ( iAddress != TInetAddr( INET_ADDR( 0,0,0,0 ), 0) )
        {
        return iAddress.Port();
        }

    const TDesC8& url = DescriptionUrl();

    if ( url.Length()> KHttp().Length() )
        {
        TPtrC8 addrAndPath = url.Right( url.Length() - KHttp().Length() );

        TInt separIndex = addrAndPath.Find( KSepar() );

        if ( separIndex == KErrNotFound )
            {
            return KDefaultDescriptionUrlPort;
            }

        TInt slashIndex = addrAndPath.Find( KSlash8() );

        if ( slashIndex != KErrNotFound )
            {
            TPtrC8 port = addrAndPath.Mid(
                    separIndex + 1, slashIndex - separIndex
            );

            TLex8 lex( port );
            TInt prt;
            TInt err = lex.Val( prt );
            if( KErrNone != err )
                {
                return KErrNotFound;
                }

            return prt;
            }
        else
            {
            TPtrC8 port = addrAndPath.Right(
                    addrAndPath.Length() - ( separIndex + 1 )
            );

            TLex8 lex( port );
            TInt prt;
            TInt err = lex.Val( prt );
            if( KErrNone != err )
                {
                return KErrNotFound;
                }

            return prt;
            }
        }

    return KDefaultDescriptionUrlPort;
    }

// -----------------------------------------------------------------------------
// CUpnpDevice::ServiceTypesL
// Return service types.
// -----------------------------------------------------------------------------
//
EXPORT_C CDesC8Array& CUpnpDevice::ServiceTypesL()
    {
    if ( !iServiceTypes )
        {
        iServiceTypes = new (ELeave) CDesC8ArrayFlat( 2 );
        }
    return *iServiceTypes;

    }

// -----------------------------------------------------------------------------
// CUpnpDevice::FriendlyName
// Return friendly name.
// -----------------------------------------------------------------------------
//
EXPORT_C const TPtrC8 CUpnpDevice::DescriptionProperty( const TDesC8& aPropertyName )
    {
    TInt index = 0;

    while ( index < iProperties.Count() )
        {
        CUpnpDescriptionProperty* var = iProperties[index];

        if ( aPropertyName.Compare( var->Name() ) == KErrNone )
            {
            return var->Value();
            }

        index++;
        }

    return KNullDesC8();

    }

// -----------------------------------------------------------------------------
// CUpnpDevice::ServiceId
// Return service ID.
// -----------------------------------------------------------------------------
//
EXPORT_C const TPtrC8 CUpnpDevice::ServiceId( const TDesC8 &aServiceType )
    {
    //return Content( KServiceId(), aServiceType );
    for( TInt i(0); i < iServiceTypes->Count(); i++)
        {
        TInt pos =(*iServiceTypes)[i].Find( aServiceType );
        if( pos != KErrNotFound )
        return TPtrC8( ( *iServicesId )[i] );

        }

    return TPtrC8( (TUint8*)"", 0);
    }

// -----------------------------------------------------------------------------
// CUpnpDevice::ServiceDescriptionUrl
// Return service description URL.
// -----------------------------------------------------------------------------
//
EXPORT_C const TPtrC8 CUpnpDevice::ServiceDescriptionUrl( const TDesC8 &aServiceType )
    {

    for (TInt i=0;i<iServiceList.Count();i++)
        {
        if (!iServiceList[i]->ServiceType().Compare(aServiceType))
            {
            return iServiceList[i]->ServiceDescriptionUrl();
            }
        }
    return TPtrC8( (TUint8*)"", 0 );;

    }

// -----------------------------------------------------------------------------
// CUpnpDevice::ServiceList
// Return service list.
// -----------------------------------------------------------------------------
//
EXPORT_C RPointerArray<CUpnpService>& CUpnpDevice::ServiceList()
    {
    return iServiceList;
    }

// -----------------------------------------------------------------------------
// CUpnpDevice::DeviceList
// Return service list.
// -----------------------------------------------------------------------------
//
EXPORT_C RPointerArray<CUpnpDevice>& CUpnpDevice::DeviceList()
    {
    return iDeviceList;
    }

// -----------------------------------------------------------------------------
// CUpnpDevice::DeviceList
// Return service list.
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpDevice::GetAllDevices( RPointerArray<CUpnpDevice>& aDevices )
    {

    for (TInt i = 0; i < iDeviceList.Count(); i++ )
        {
        TRAP_IGNORE(aDevices.AppendL( iDeviceList[i] ));
        iDeviceList[i]->GetAllDevices(aDevices);
        }

    }

// -----------------------------------------------------------------------------
// CUpnpDevice::AttachService
// Attach to service.
// -----------------------------------------------------------------------------
//
void CUpnpDevice::AttachServiceL( CUpnpService *aService )
    {
    if ( aService )
        {
        if ( !aService->IsAdded() )
            {
            TInt servCount = aService->Device().ServiceList().Count();
            CUpnpService* service= NULL;
            TInt i(0);

            do
                {
                service = aService->Device().ServiceList()[i];
                i++;
                } while ( service->IsAdded() );

            aService->SetServiceTypeL( service->ServiceType() );
            aService->SetPathL( service->Path() );

            aService->SetControlUrl( ConcatWithUrlBaseL( service->ControlUrl() ) );
            aService->SetSubscriptionUrl( ConcatWithUrlBaseL( service->SubscriptionUrl() ) );
            aService->SetServiceDescriptionUrl( service->ServiceDescriptionUrl().AllocL() );

            ServiceList().Remove( i-1 );
            ServiceList().Insert( aService, i-1 );
            delete service;
            }

        aService->Added();
        }
    }

// -----------------------------------------------------------------------------
// CUpnpDevice::DetachService
// -----------------------------------------------------------------------------
//
void CUpnpDevice::DetachService( CUpnpService *aService )
    {
    for ( TInt i = ServiceList().Count() - 1; i >= 0; --i )
        {
        if ( ServiceList()[ i ] == aService )
            {
            ServiceList().Remove( i );
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpDevice::WaitServiceDescriptionL
// Wait service description.
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpDevice::WaitServiceDescriptionL( TInt aSessionId )
    {
    User::LeaveIfError( iServiceDescriptionSessionIds.Append( aSessionId ) );
    }

// -----------------------------------------------------------------------------
// CUpnpDevice::WaitServiceDescriptionCount
// Wait service description count.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpDevice::WaitServiceDescriptionCount()
    {
    return iServiceDescriptionSessionIds.Count();
    }

// -----------------------------------------------------------------------------
// CUpnpDevice::WaitServiceDescriptionSessionId
// Wait service description session ID.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpDevice::WaitServiceDescriptionSessionId(TInt aIndex)
    {
    if(iServiceDescriptionSessionIds.Count()> aIndex)
        {
        return iServiceDescriptionSessionIds[aIndex];
        }
    return KErrGeneral;
    }

// -----------------------------------------------------------------------------
// CUpnpDevice::WaitServiceDescriptionRemoveSessionId
// Wait service description remove session ID.
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpDevice::WaitServiceDescriptionRemoveSessionId( TInt aIndex )
    {
    if(iServiceDescriptionSessionIds.Count()> aIndex)
        {
        iServiceDescriptionSessionIds.Remove(aIndex);
        iServiceDescriptionSessionIds.Compress();
        }
    }

// -----------------------------------------------------------------------------
// CUpnpDevice::AddServiceL
// Add service.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpDevice::TServiceAdd CUpnpDevice::AddServiceL(
        TInt aSessionId,
        CUpnpService* aService )
    {
    TInt idx = iServiceDescriptionSessionIds.Find( aSessionId );
    if ( idx != KErrNotFound )
        { // received Service Description for service of this device
        iServiceDescriptionSessionIds[idx] = KErrNotFound;

        AttachServiceL(aService);

        TInt count = iServiceDescriptionSessionIds.Count();
        TInt i;
        for ( i = 0; i < count && iServiceDescriptionSessionIds[i] == KErrNotFound; i++ )
            {
            }

        // We have retrieved all services only when i == iServices.Count().
        //      is there some reason why this was ( i == count )
        if ( i == iServiceList.Count() ) 

            { // All Service Descriptions for this device arrived  
            iServiceDescriptionSessionIds.Reset();
            iServiceDescriptionReceiveState = EAllServicesAdded;
            return EAllServicesAdded;
            }
        else
            {
            return EServiceAdded;
            }
        }
    return ENotServiceAdded;
    }

// -----------------------------------------------------------------------------
// CUpnpDevice::WaitIconL
// Wait icon.
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpDevice::WaitIconL( TInt aSessionId )
    {
    User::LeaveIfError( iIconSessionIds.Append( aSessionId ) );
    iIconReceiveState = ENotIconAdded;
    }

// -----------------------------------------------------------------------------
// CUpnpDevice::AddIcon
// Add icon.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpDevice::TIconAdd CUpnpDevice::AddIcon( TInt aSessionId )
    {
    TInt idx = iIconSessionIds.Find( aSessionId );
    if ( idx != KErrNotFound )
        { // received Icon Description for service of this device
        iIconSessionIds[idx] = -1;

        TInt count = iIconSessionIds.Count();
        TInt i;
        for (i = 0; i < count && iIconSessionIds[i] == -1; i++ )
            {
            }

        if ( i == count ) 

            { // All Icon Descriptions for this device arrived 
            iIconSessionIds.Reset();
            iIconReceiveState = EAllIconsAdded;
            return EAllIconsAdded;
            }
        else
            {
            return EIconAdded;
            }
        }
    return ENotIconAdded;
    }

// -----------------------------------------------------------------------------
// CUpnpDevice::Address
// Return IP address.
// -----------------------------------------------------------------------------
//
EXPORT_C TInetAddr CUpnpDevice::Address()
    {
    return iAddress;
    }

// -----------------------------------------------------------------------------
// CUpnpDevice::SetAddress
// Set IP address.
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpDevice::SetAddress( const TInetAddr& aAddr )
    {
    iAddress = aAddr;
    }

// -----------------------------------------------------------------------------
// CUpnpDevice::Icons
// Return iIcons.
// -----------------------------------------------------------------------------
//
EXPORT_C RPointerArray<CUpnpIcon>& CUpnpDevice::Icons()
    {
    return iIcons;
    }

// -----------------------------------------------------------------------------
// CUpnpDevice::Local
// Return iLocal.
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CUpnpDevice::Local() const
    {
    return iNetworkType == ELocalDevice;
    }

// -----------------------------------------------------------------------------
// CUpnpDevice::SetLocal
// Set local.
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpDevice::SetLocal( TBool aLocal )
    {
    if(aLocal)
        {
        iNetworkType = ELocalDevice;
        }
    }

// -----------------------------------------------------------------------------
// CUpnpDevice::Local
// Return iLocal.
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CUpnpDevice::Remote() const
    {
    return iNetworkType == ERemoteDevice;
    }

// -----------------------------------------------------------------------------
// CUpnpDevice::DeviceNetwork
// Get device network type.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpDevice::TUpnpDeviceNetwork CUpnpDevice::DeviceNetwork() const
    {
    return iNetworkType;
    }

// -----------------------------------------------------------------------------
// CUpnpDevice::SetDeviceNetwork
// Set device network type.
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpDevice::SetDeviceNetwork( CUpnpDevice::TUpnpDeviceNetwork aNetworkType )
    {
    iNetworkType = aNetworkType;
    }

// -----------------------------------------------------------------------------
// CUpnpDevice::RemoveIpAddress
// Removes IP address in front of the url leaving only path to file.
//  (12.34.56.78/folder/file.ext -> /folder/file )
// -----------------------------------------------------------------------------
//
TPtrC8 CUpnpDevice::RemoveIpAddress( const TDesC8 &aAddress )
    {
    if ( aAddress.Length() > KHttp().Length() )
        {
        if ( aAddress.Left( KHttp().Length() ) == KHttp() )
            {
            TPtrC8 newContent = aAddress.Mid( KHttp().Length() );

            TInt slashIndex = newContent.Find( UpnpString::KSlash() );

            if ( slashIndex != KErrNotFound )
                {
                return newContent.Mid( slashIndex );
                }
            }
        }
    return TPtrC8( aAddress );
    }

// -----------------------------------------------------------------------------
// CUpnpDevice::ChangeIconSessionId
// This function is used for removing old icon request session id from list
// and adding a new one.
// -----------------------------------------------------------------------------
//
TInt CUpnpDevice::ChangeIconSessionIdL( const TInt aOldSessionId,
    const TInt aNewSessionId )
    {

    // remove old sessionid from list
    TInt idx = iIconSessionIds.Find( aOldSessionId );
    if ( idx != KErrNotFound )
        {
        iIconSessionIds.Remove( idx );
        }

    // add new to list
    User::LeaveIfError( iIconSessionIds.Append( aNewSessionId ) );

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CUpnpDevice::CheckIconSessionIdExist
// Checking if given session id is listed in this device's iconSessionId list.
// -----------------------------------------------------------------------------
//
TInt CUpnpDevice::CheckIconSessionIdExist( const TInt aSessionId )
    {
    return iIconSessionIds.Find( aSessionId );
    }

// -----------------------------------------------------------------------------
// CUpnpDevice::GetUrlBase
// Return URLBase content.
// -----------------------------------------------------------------------------
//
EXPORT_C const TPtrC8 CUpnpDevice::UrlBase()
    {
    if( iUrlBase )
        {
        return iUrlBase->Des();
        }
    else
        {
        return KNullDesC8();
        }
    }

// -----------------------------------------------------------------------------
// CUpnpDevice::PresentationUrl
// Return presentation URL.
// -----------------------------------------------------------------------------
//
EXPORT_C const TPtrC8 CUpnpDevice::PresentationUrl()
    {
    TInt index = 0;

    while ( index < iProperties.Count() )
        {
        CUpnpDescriptionProperty* var = iProperties[index];

        if ( KPresentationUrl().Compare( var->Name() ) == KErrNone )
            {
            return var->Value();
            }

        index++;
        }

    return KNullDesC8();//return Content( KPresentationUrl() );
    }

// -----------------------------------------------------------------------------
// CUpnpDevice::PresentationUrl
// Return presentation URL.
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpDevice::SetUrlBaseL(const TDesC8& aUrlBase)
    {
    if( iUrlBase )
        {
        delete iUrlBase;
        iUrlBase = NULL;
        }
    iUrlBase = aUrlBase.AllocL();
    }

// -----------------------------------------------------------------------------
// CUpnpDevice::ConcatWithUrlBase
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* CUpnpDevice::ConcatWithUrlBaseL(const TDesC8& aUrl)
    {
    TPtrC8 baseUrl( UrlBase() );

    if( baseUrl.Length() == 0 ||
            aUrl.Length() == 0 ||
            aUrl.Find( KHttp ) == 0 ||
            ( aUrl[0] == KSlash8()[0] && !( baseUrl.Find( KHttp ) == 0 ) ) )

        {
        return aUrl.AllocL();
        }

    HBufC8* result( NULL );
    TBool slashInBase( baseUrl[baseUrl.Length() - 1] == KSlash8()[0] );
    TBool slashInUrl( aUrl[0] == KSlash8()[0] );
    TInt length( 0 );

    if( slashInBase && slashInUrl )
        {
        length = baseUrl.Length() + aUrl.Length() - 1;
        result = HBufC8::NewL( length );
        TPtr8 resultPtr( result->Des() );
        resultPtr.Append( baseUrl.Left( baseUrl.Length() - 1 ));
        }
    else if( slashInBase != slashInUrl )
        {
        length = baseUrl.Length() + aUrl.Length();
        result = HBufC8::NewL( length );
        TPtr8 resultPtr( result->Des() );
        resultPtr.Append( baseUrl );
        }
    else
        {
        length = baseUrl.Length() + aUrl.Length() + 1;
        result = HBufC8::NewL( length );
        TPtr8 resultPtr( result->Des() );
        resultPtr.Append( baseUrl );
        resultPtr.Append( '/' );
        }

    result->Des().Append( aUrl );
    return result;
    }

// -----------------------------------------------------------------------------
// CUpnpDevice::SetTypeL
// -----------------------------------------------------------------------------
//
void CUpnpDevice::SetTypeL( const TDesC8& aType )
    {
    HBufC8* tmp = aType.AllocL();
    delete iDeviceType;
    iDeviceType = tmp;
    }

// -----------------------------------------------------------------------------
// CUpnpDevice::AddDevicePropertyL
// -----------------------------------------------------------------------------
//
void CUpnpDevice::AddDevicePropertyL(
    CUpnpDescriptionProperty* aDeviceProperty )
    {
    iProperties.AppendL( aDeviceProperty );
    }

// -----------------------------------------------------------------------------
// CUpnpDevice::SetRootDevice
// -----------------------------------------------------------------------------
//
void CUpnpDevice::SetRootDevice( TBool aIsRoot )
    {
    iIsRootDevice = aIsRoot;
    }

// -----------------------------------------------------------------------------
// CUpnpDevice::GetProperty
// Get information from device description.
// -----------------------------------------------------------------------------
//
EXPORT_C TPtrC8 CUpnpDevice::GetProperty(const TDesC8& aProperty)
    {
    TInt index = 0;

    while ( index < iProperties.Count() )
        {
        CUpnpDescriptionProperty* var = iProperties[index];
        if ( aProperty.Compare( var->Name() ) == KErrNone )
            {
            return var->Value();
            }

        index++;
        }

    return KNullDesC8();
    //return Content( aProperty );
    }

// -----------------------------------------------------------------------------
// CUpnpDevice::AddIconL
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpDevice::AddIconL( CUpnpIcon* aIcon)
    {
    if( aIcon )
    iIcons.AppendL( aIcon);

    }

// -----------------------------------------------------------------------------
// CUpnpDevice::ServiceIdL
// Get information about service IDs.
// -----------------------------------------------------------------------------
//
EXPORT_C CDesC8Array& CUpnpDevice::ServiceIdL()
    {
    if ( !iServicesId )
        {
        iServicesId = new (ELeave) CDesC8ArrayFlat( 2 );
        }
    return *iServicesId;
    }

// -----------------------------------------------------------------------------
// CUpnpDevice::IsEmbeddDevice
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CUpnpDevice::IsEmbeddDevice()
    {
    return !(iIsRootDevice);
    }

// -----------------------------------------------------------------------------
// CUpnpDevice::CloneL
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpDevice* CUpnpDevice::CloneL()
    {
    CUpnpDevice* device = new (ELeave) CUpnpDevice();
    CleanupStack::PushL( device );
    Init();
    if(!this->iUUID || !this->iDeviceType || !this->iDescriptionURL)
        {
        CleanupStack::PopAndDestroy(device);
        User::Leave(KErrGeneral);
        }

    device->SetUuidL( *this->iUUID );
    device->SetTypeL( *this->iDeviceType);
    device->SetDescriptionUrlL( *this->iDescriptionURL );
    device->SetDeviceNetwork( this->iNetworkType );
    device->SetAddress( this->iAddress );
    device->SetAlive( this->iAlive );
    device->SetExpired( this->iExpired );

    CleanupStack::Pop( device );
    return device;
    }

// End of File
