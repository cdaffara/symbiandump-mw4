/** @file
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CUpnpDeviceLibrary
*
*/


// INCLUDE FILES
#include "upnpdevicelibrary.h"
#include "upnpssdpmessage.h"
#define KLogFile _L("DLNAWebServer.txt")
#include "upnpcustomlog.h"

static const TInt KIntervalBetweenByeAndAlive = 1;
static const TInt KIntervalBeforeFirstAlive = 1;
// prevents treating SSDP alive messages sent by local device
// as an external device messages in case of quick local device restarting
// heuristic value 
static const TInt KLocalDeviceHysteresisWindow  = 20;
// used for IPC communication 2 phase request
// heuristic value 
static const TInt KIpcCommunicationTimeout = 2;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpDeviceLibrary::CUpnpDeviceLibrary
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpDeviceLibrary::CUpnpDeviceLibrary( MUpnpDeviceLibraryObserver& aObserver,
                                TInt aHandle )
    : iObserver(aObserver),
      iHandle( aHandle )
    {
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceLibrary::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpDeviceLibrary::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceLibrary::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpDeviceLibrary* CUpnpDeviceLibrary::NewL( 
                                        MUpnpDeviceLibraryObserver& aObserver,
                                        TInt aHandle )
    {
    CUpnpDeviceLibrary* self = new (ELeave) CUpnpDeviceLibrary( aObserver, aHandle );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }
// -----------------------------------------------------------------------------
// CUpnpDeviceLibrary::~CUpnpDeviceLibrary
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpDeviceLibrary::~CUpnpDeviceLibrary()
    {
    LOGSH( iHandle, "Devicelibrary ~CUpnpDeviceLibrary");

    iElementArray.ResetAndDestroy();
    iElementArray.Close();
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceLibrary::PrepareShutdown
// Prepare devices for shutdown.
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpDeviceLibrary::PrepareShutdown()
    {
    LOGSH( iHandle, "Devicelibrary PrepareShutdown" );

    }

// -----------------------------------------------------------------------------
// CUpnpDeviceLibrary::AddInfoL
// Add device info.
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpDeviceLibrary::AddInfoL( const TUpnpAddLocalDevice* aIndex, 
                                        const TDesC8& aBuffer, 
                                        const TInetAddr& aLocalAddr )
    {
    LOGSH( iHandle, "Devicelibrary AddInfoL" );
    if ( !aIndex )
        {
        return;
        }
    
    TPtrC8 uuid = aBuffer.Left(aIndex->iUuidLength);    
    TInt index = Find( uuid );
    
    if ( index != KErrNotFound )
        {        
        CUpnpDeviceLibraryElement* elem = iElementArray[index];
        
        if ( elem->Local() )
            {            
            CUpnpTimeoutElement::TRenew state = elem->Renew();          
            elem->AddInfoL( aIndex, aBuffer, aLocalAddr );                        
            elem->SetRenew( CUpnpTimeoutElement::ERenew );
            elem->SetTimeout( KIntervalBeforeFirstAlive );        
            elem->SetUpdateId( iUpdateId );
                 
            if ( state == CUpnpTimeoutElement::ERemove ) 
                {   
                elem->SetAlive( ETrue );                
                elem->SetUpdateId( ++iUpdateId );
                iObserver.DeviceListChangedL();
                }        
            }
        else
            {
            InvalidateNonLocalDevice( *elem );
            AppendLocalDeviceL( aIndex, aBuffer, aLocalAddr );                            
            iObserver.DeviceListChangedL();            
            }                
        }
    else
        {        
        AppendLocalDeviceL( aIndex, aBuffer, aLocalAddr );                
        iObserver.DeviceListChangedL();
        }
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceLibrary::InvalidateNonLocalDevice
// -----------------------------------------------------------------------------
//
void CUpnpDeviceLibrary::InvalidateNonLocalDevice( CUpnpDeviceLibraryElement& aElem )
    {
    aElem.SetAlive( EFalse );
    aElem.SetUpdateId( ++iUpdateId );
    aElem.SetTimeout( KIpcCommunicationTimeout );                            
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceLibrary::AppendLocalDeviceL
// -----------------------------------------------------------------------------
//
void CUpnpDeviceLibrary::AppendLocalDeviceL( const TUpnpAddLocalDevice* aIndex, 
                         const TDesC8& aBuffer, 
                         const TInetAddr& aLocalAddr )
    {    
    CUpnpDeviceLibraryElement* element = CUpnpDeviceLibraryElement::NewL(*this);
    CleanupStack::PushL( element );
    element->AddInfoL( aIndex, aBuffer, aLocalAddr );
    element->SetRenew( CUpnpTimeoutElement::ERenew );
    element->SetTimeout( KIntervalBeforeFirstAlive );
    element->SetLocal( ETrue );
    element->SetUpdateId( ++iUpdateId );
    iElementArray.AppendL( element );
    CleanupStack::Pop( element );    
    }
// -----------------------------------------------------------------------------
// CUpnpDeviceLibrary::AddInfoL
// Add device info.
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpDeviceLibrary::AddInfoL( const TUpnpAddLocalDevice* aIndex, 
                                        const TDesC8& aBuffer )
    {
    LOGSH( iHandle, "Devicelibrary AddInfoL");

    if ( !aIndex )
        {
        return;
        }
    
    TPtrC8 uuid = aBuffer.Left(aIndex->iUuidLength);
    
    TInt index = Find( uuid );
    
    if ( index != KErrNotFound )
        {
        CUpnpDeviceLibraryElement* elem = iElementArray[index];
        
        CUpnpTimeoutElement::TRenew state = elem->Renew();
        
        elem->AddInfoL( aIndex, aBuffer );                        
        elem->SetRenew( CUpnpTimeoutElement::ERenew );
        elem->SetTimeout( 1 );        
        elem->SetUpdateId( iUpdateId );
             
        if (state == CUpnpTimeoutElement::ERemove) 
            {   
            iUpdateId++; 
            elem->SetAlive( ETrue );                
            elem->SetUpdateId( iUpdateId );
            iObserver.DeviceListChangedL();
            }        
        }
    else
        {
        iUpdateId++;
        
        CUpnpDeviceLibraryElement* element = CUpnpDeviceLibraryElement::NewL(*this);
        CleanupStack::PushL( element );
        element->AddInfoL( aIndex, aBuffer );
        element->SetRenew( CUpnpTimeoutElement::ERenew );
        element->SetTimeout( 1 );
        element->SetLocal( EFalse );
        element->SetUpdateId( iUpdateId );
        iElementArray.AppendL( element );
        CleanupStack::Pop( element );
        
        iObserver.DeviceListChangedL();
        }
    }
// -----------------------------------------------------------------------------
// CUpnpDeviceLibrary::AddInfoL
// Add device info.
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpDeviceLibrary::AddInfoL( CUpnpSsdpMessage* aMessage )
    {
    LOGSH( iHandle, "Devicelibrary AddInfoL");
    if ( aMessage )
        {
        if ( aMessage->IsSsdpAlive() )
            {       
            if( AddDeviceL(aMessage) )
                {  
                iObserver.DeviceListChangedL();
                }
            }
        else
            {
            RemoveDeviceL( aMessage );
            iObserver.DeviceListChangedL();
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceLibrary::Find
// Find device given by UUID.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpDeviceLibrary::Find( const TDesC8& aUuid )
    {
    LOGSH( iHandle, "Devicelibrary Find");

    for ( TInt i=0; i<iElementArray.Count(); i++ )
        {
        if ( aUuid.FindC( iElementArray[i]->UUID() ) == 0 
             && aUuid.Length() == iElementArray[i]->UUID().Length() )
            {
            return i;
            }
        }
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceLibrary::RemoveL
// Remove device by given UID. Used for local devices only.
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpDeviceLibrary::RemoveL( const TDesC8& aUuid )
    {
    LOGSH( iHandle, "Devicelibrary RemoveL");

    TInt result = Find( aUuid );
    
    if ( result != KErrNotFound )
        {
        CUpnpDeviceLibraryElement* elem = iElementArray[result];

        iObserver.AdvertiseDeviceL( EFalse, *elem );
        iObserver.AdvertiseDeviceL( EFalse, *elem );        

        elem->AdvertisingFinished();
        
        elem->SetRenew( CUpnpTimeoutElement::ERemove );
        elem->SetTimeout( KLocalDeviceHysteresisWindow );
        elem->SetUpdateId( ++iUpdateId );
        elem->SetAlive( EFalse );            

        iObserver.DeviceListChangedL();
        }
    }
// -----------------------------------------------------------------------------
// CUpnpDeviceLibrary::RemoveSilentL
// Remove device by given UID. Used for local devices only.
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpDeviceLibrary::RemoveSilentL( const TDesC8& aUuid )
    {
    //LOGS( iHandle, "Devicelibrary RemoveSilentL");
    
   LOGS("CUpnpDeviceLibrary::RemoveSilentL( const TDesC8& aUuid )");
    TInt result = Find( aUuid );
    
    if ( result != KErrNotFound )
        {
        CUpnpDeviceLibraryElement* elem = iElementArray[result];

        
        elem->SetRenew( CUpnpTimeoutElement::ERemove );
        elem->SetTimeout( KLocalDeviceHysteresisWindow );
        elem->SetUpdateId( ++iUpdateId );
        elem->SetAlive( EFalse );            

        iObserver.DeviceListChangedL();
        }
         LOGS("CUpnpDeviceLibrary::RemoveSilentL( const TDesC8& aUuid ) passed");
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceLibrary::TimeoutExpiredL
// Timeout expired for advertisement.
// -----------------------------------------------------------------------------
//
void CUpnpDeviceLibrary::TimeoutExpiredL( CUpnpTimeoutElement* anElement )
    {
    LOGSH( iHandle, "Devicelibrary TimeoutExpiredL");

    if ( !anElement )
        {
        return;
        }
    
    CUpnpDeviceLibraryElement* elem = 
        static_cast<CUpnpDeviceLibraryElement*>(anElement);
    
    switch ( elem->Renew() )
        {
        case CUpnpTimeoutElement::ERenew:
            {
            if ( !elem->Advertised() )
                {
                elem->Advertising();
                // whatever happends we don't want to have leave here
                TRAP_IGNORE( iObserver.AdvertiseDeviceL( EFalse, *elem ));
                TRAP_IGNORE( iObserver.AdvertiseDeviceL( EFalse, *elem ));
                elem->SetTimeout( KIntervalBetweenByeAndAlive );
                }
            else 
                {
                // whatever happends we don't want to have leave here
                TRAP_IGNORE( iObserver.AdvertiseDeviceL( ETrue, *elem ));
                elem->SetTimeout( KDeviceTimeout );
                }
            }
            break;
            
        case CUpnpTimeoutElement::EOnce: // after cache control expires
            {    
            // Set the timeout for the device to be removed
            iUpdateId++;
            elem->SetRenew( CUpnpTimeoutElement::ERemove );
            if( elem->Local() )
                {
                elem->SetTimeout( KLocalDeviceHysteresisWindow );    
                }
            else
                {
                elem->SetTimeout( KIpcCommunicationTimeout );
                }
            elem->SetUpdateId( iUpdateId );
            elem->SetAlive( EFalse );
            elem->SetExpired( ETrue );

            iObserver.DeviceListChangedL();
            }
            break;

        case CUpnpTimeoutElement::ERemove:
            {
            // Remove device from the element array
            for ( TInt i=0; i<iElementArray.Count(); i++ )
                {
                if ( elem == iElementArray[i] )
                    {
                    iElementArray.Remove( i );
                    iElementArray.Compress();
                    
                    delete elem;
                    break;
                    }
                }
            }
            break;

        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceLibrary::DeviceList
// Return device list.
// -----------------------------------------------------------------------------
//
EXPORT_C RPointerArray<CUpnpDeviceLibraryElement>& CUpnpDeviceLibrary::DeviceList()
    {
    LOGSH( iHandle, "Devicelibrary DeviceList");

    return iElementArray;
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceLibrary::GetUpdate
// Get update.
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpDeviceLibrary::GetUpdate( TInt& aUpdateId, 
                                         TUpnpDevice *&aDevices, 
                                         TUpnpService *&aServices,
                                         TInt& aDeviceCount,
                                         TInt& aServiceCount )
    {
    aDevices = NULL;
    aServices = NULL;
    
    TUpnpDevice* devices = NULL;
    TUpnpService* services = NULL;
    
    aDeviceCount = 0;
    aServiceCount = 0;
    
    for ( TInt i = 0; i < iElementArray.Count(); i++ )
        {
        CUpnpDeviceLibraryElement* elem = iElementArray[i];
        
        if (elem->UpdateId() > aUpdateId)
            {
            TUpnpDevice* temp = new TUpnpDevice[aDeviceCount+1];
            if ( !temp )
                {
                continue;
                }
            
            TPtr8 destPtr(reinterpret_cast<TUint8*>(temp), 
                0,
                sizeof(TUpnpDevice)*(aDeviceCount+1));
            TPtr8 srcPtr(reinterpret_cast<TUint8*>(devices),
                sizeof(TUpnpDevice)*aDeviceCount,
                sizeof(TUpnpDevice)*aDeviceCount);
            
            destPtr.Copy(srcPtr);
            
            if (devices)
                {
                delete[] devices;
                devices = NULL;
                }
            
            devices = temp;
            
            TUpnpDevice devtemp;
            destPtr.Set(reinterpret_cast<TUint8*>(&devices[aDeviceCount]),
                0,
                sizeof(TUpnpDevice));
            srcPtr.Set(reinterpret_cast<TUint8*>(&devtemp), 
                sizeof(TUpnpDevice),
                sizeof(TUpnpDevice));
            destPtr.Copy(srcPtr);
            
            devices[aDeviceCount].Set(elem);
            
            TInt servs = elem->ServiceList().Count();
            
            TUpnpService* tempServ = new TUpnpService[aServiceCount+servs];
            if ( !tempServ )
                {
                delete [] temp;
                temp = NULL;
                devices = NULL;
                continue;
                }
            
            destPtr.Set(reinterpret_cast<TUint8*>(tempServ), 
                0,
                sizeof(TUpnpService)*(aServiceCount+servs));
            srcPtr.Set(reinterpret_cast<TUint8*>(services),
                sizeof(TUpnpService)*aServiceCount,
                sizeof(TUpnpService)*aServiceCount);
            
            destPtr.Copy(srcPtr);
            
            if (services)
                {
                delete[] services;
                services = NULL;
                }
            
            services = tempServ;
            
            for (TInt j=0; j<servs; j++)
                {
                TUpnpService service;
                destPtr.Set(reinterpret_cast<TUint8*>(&services[aServiceCount]),
                    0,
                    sizeof(TUpnpService));
                srcPtr.Set(reinterpret_cast<TUint8*>(&service), 
                    sizeof(TUpnpService),
                    sizeof(TUpnpService));
                destPtr.Copy(srcPtr);
                
                services[aServiceCount].iServiceType.Zero();
                services[aServiceCount].iServiceType.Append(
                    elem->ServiceList()[j] );
                
                aServiceCount++;
                }
            
            aDeviceCount++;
            }
        }
    
    aUpdateId = iUpdateId;
    
    aDevices = devices;
    aServices = services;
    }
    
// -----------------------------------------------------------------------------
// CUpnpDeviceLibrary::RemoveAllDevicesL
// Removes all remote devices from device library.
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpDeviceLibrary::RemoveAllDevicesL()
    {
    for (TInt i = 0; i < iElementArray.Count(); i++)
        {
        CUpnpDeviceLibraryElement* elem = iElementArray[i];
        if ( elem->Alive() )
            {
            iUpdateId++;
            elem->SetAlive(EFalse);
            elem->SetUpdateId( iUpdateId );
            if( elem->Renew() == CUpnpTimeoutElement::EOnce )
                {
                elem->SetRenew( CUpnpTimeoutElement::ERemove );
                elem->SetTimeout( KIpcCommunicationTimeout );
                }
            }
        }
    iObserver.DeviceListChangedL();        
    }    

// -----------------------------------------------------------------------------
// TUpnpDevice::Set
// Set device.
// -----------------------------------------------------------------------------
//
EXPORT_C void TUpnpDevice::Set(CUpnpDeviceLibraryElement* aElem)
    {
    LOGS("Devicelibrary Set");

    if ( !aElem )
        {
        return;
        }

    iDescriptionURL = aElem->DescriptionUrl().Left( iDescriptionURL.MaxLength() );
    iUUID = aElem->UUID().Left( iUUID.MaxLength() );
    iDeviceType = aElem->DeviceType().Left( iDeviceType.MaxLength() );
    iDomain = aElem->Domain().Left( iDomain.MaxLength() );

    iServiceCount = aElem->ServiceList().Count();
    iLocal = aElem->Local();
    iRemote = EFalse;
    iAlive = aElem->Alive();
    iExpired = aElem->Expired();
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceLibrary::AddDeviceL
// Add a device.
// -----------------------------------------------------------------------------
//
TBool CUpnpDeviceLibrary::AddDeviceL(CUpnpSsdpMessage *aMessage)
    {
    TBool update = EFalse;

    LOGSH( iHandle, "Devicelibrary AddDeviceL");

    if ( !aMessage )
        {
        return update;
        }
    
    TPtr8 uuidPtr = aMessage->Uuid();
    TInt index = Find( uuidPtr );
    
    if (index != KErrNotFound)
        {
        CUpnpDeviceLibraryElement* elem = iElementArray[index];
        if ( !elem->Local() )
        {
            if( elem->Renew() == CUpnpTimeoutElement::ERemove )
                {
                iElementArray.Remove( index );
                iElementArray.Compress();
                
                delete elem;
                
                CUpnpDeviceLibraryElement* element = CUpnpDeviceLibraryElement::NewL(*this);
                CleanupStack::PushL(element);
                
                update = element->AddInfoL(aMessage, iUpdateId);
                
                element->SetRenew(CUpnpTimeoutElement::EOnce);
                element->SetTimeout(aMessage->CacheControl());
                element->SetLocal(EFalse);
                iElementArray.AppendL(element);
                
                CleanupStack::Pop(); // element
                }
            
            else 
                {
                if(elem->Filter())
                    {
                    //Alive message from 
                    if(elem->AddInfoL(aMessage, iUpdateId))
                        {
                        update=ETrue;
                        }
                    elem->SetTimeout(aMessage->CacheControl());    
                    }
                else
                    {
                    iElementArray.Remove( index );
                    iElementArray.Compress();
                
                    delete elem;
                
                    CUpnpDeviceLibraryElement* element = CUpnpDeviceLibraryElement::NewL(*this);
                    CleanupStack::PushL(element);
                
                    update = element->AddInfoL(aMessage, iUpdateId);
                
                    element->SetRenew(CUpnpTimeoutElement::EOnce);
                    element->SetTimeout(aMessage->CacheControl());
                    element->SetLocal(EFalse);
                    iElementArray.AppendL(element);
                
                    CleanupStack::Pop(); // element
                    }
                }
            }
        }
    else
        {    
        CUpnpDeviceLibraryElement* element = CUpnpDeviceLibraryElement::NewL(*this);
        CleanupStack::PushL(element);
        
        update = element->AddInfoL(aMessage, iUpdateId);
        
        element->SetRenew(CUpnpTimeoutElement::EOnce);
        element->SetTimeout(aMessage->CacheControl());
        element->SetLocal(EFalse);
        iElementArray.AppendL(element);
        
        CleanupStack::Pop(); // element
        }
    return update;
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceLibrary::RemoveDevice
// Remove a device.
// -----------------------------------------------------------------------------
//
void CUpnpDeviceLibrary::RemoveDeviceL( CUpnpSsdpMessage *aMessage )
    {
    LOGSH( iHandle, "Devicelibrary RemoveDevice" );

    if ( !aMessage )
        {
        return;
        }
    
    TPtr8 uuidPtr = aMessage->Uuid();
    TInt index = Find( uuidPtr );
    
    if ( index != KErrNotFound )
        {
        CUpnpDeviceLibraryElement* elem = iElementArray[index];
        if ( !elem->Local() )
            {
            if ( elem->Alive() )
                {
                iUpdateId++;
                elem->SetAlive(EFalse);
                elem->SetUpdateId( iUpdateId );
                if( elem->Renew() == CUpnpTimeoutElement::EOnce )
                    {
                    elem->SetRenew( CUpnpTimeoutElement::ERemove );
                    elem->SetTimeout( KIpcCommunicationTimeout );
                    }
                }
            iObserver.DeviceListChangedL();
            }
        }
    else
        {        
        CUpnpDeviceLibraryElement* element = CUpnpDeviceLibraryElement::NewL( *this );
        CleanupStack::PushL( element );

        element->AddInfoL( aMessage, iUpdateId );

        iUpdateId++;
        element->SetAlive( EFalse );
        element->SetUpdateId( iUpdateId );
        element->SetRenew( CUpnpTimeoutElement::ERemove );
        element->SetTimeout( KIpcCommunicationTimeout );
        element->SetLocal( EFalse );
        iElementArray.AppendL( element );
        
        CleanupStack::Pop( element );
        
        iObserver.DeviceListChangedL();
        }
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceLibrary::StopFilteringDeviceL
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpDeviceLibrary::StopFilteringDeviceL( const TDesC8& aUuid )
    {
    LOGSH( iHandle, "Devicelibrary RemoveDevice" );

    TInt index = Find( aUuid );
    
    if ( index != KErrNotFound )
        {
        CUpnpDeviceLibraryElement* elem = iElementArray[index];
        elem->SetFilter(EFalse);
        }
    }

//  End of File
