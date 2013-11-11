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
* Description:  CUpnpDeviceLibraryElement
*
*/


// INCLUDE FILES
#include <e32base.h>
#include "upnpdevicelibraryelement.h"
#include "upnpssdpmessage.h"
#include "upnpcons.h"
#include "upnpstring.h"

using namespace UpnpSSDP;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpDeviceLibraryElement::CUpnpDeviceLibraryElement
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpDeviceLibraryElement::CUpnpDeviceLibraryElement(MUpnpTimeoutElementParent& aParent)
    : CUpnpTimeoutElement(aParent)
    {
    iLocal = EFalse;
    iFilter = ETrue;
    iAdvertised = EFalse;
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceLibraryElement::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpDeviceLibraryElement::ConstructL()
    {
    BaseConstructL();
    iUUID = HBufC8::NewL(0);
    iDescriptionURL = HBufC8::NewL(0);
    iDomain = HBufC8::NewL(0);
    iDeviceType = HBufC8::NewL(0);

    iServiceList = new (ELeave) CDesC8ArrayFlat(4);

    iAlive = ETrue;
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceLibraryElement::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpDeviceLibraryElement* CUpnpDeviceLibraryElement::NewL( 
                                               MUpnpTimeoutElementParent& aParent )
    {
    CUpnpDeviceLibraryElement* self = new (ELeave) CUpnpDeviceLibraryElement(aParent);
    
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceLibraryElement::~CUpnpDeviceLibraryElement
// Destructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpDeviceLibraryElement::~CUpnpDeviceLibraryElement()
    {
    Cancel(); 

    delete iUUID;
    iUUID = NULL;
    delete iDescriptionURL;
    iDescriptionURL = NULL;
    delete iDomain;
    iDomain = NULL;
    delete iDeviceType;
    iDeviceType = NULL;

    if( iServiceList )    
        {
        iServiceList->Reset();
        delete iServiceList;
        }
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceLibraryElement::IsRootDevice
// Return iRootDevice.
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CUpnpDeviceLibraryElement::IsRootDevice() const
    {
    return iRootDevice;
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceLibraryElement::UUID
// Return iUUID.
// -----------------------------------------------------------------------------
//
EXPORT_C TDesC8& CUpnpDeviceLibraryElement::UUID() 
    {
    return *iUUID;
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceLibraryElement::DescriptionURL
// Return iDescriptionURL.
// -----------------------------------------------------------------------------
//
EXPORT_C TDesC8& CUpnpDeviceLibraryElement::DescriptionUrl() 
    {
    return *iDescriptionURL;
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceLibraryElement::Domain
// Return iDomain.
// -----------------------------------------------------------------------------
//
EXPORT_C TDesC8& CUpnpDeviceLibraryElement::Domain() 
    {
    return *iDomain;
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceLibraryElement::DeviceType
// Return iDeviceType.
// -----------------------------------------------------------------------------
//
EXPORT_C TDesC8& CUpnpDeviceLibraryElement::DeviceType() 
    {
    return *iDeviceType;
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceLibraryElement::ServiceList
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CDesC8Array& CUpnpDeviceLibraryElement::ServiceList() 
    {
    return *iServiceList;
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceLibraryElement::AddInfoFromNotificationTypeHeaderL
//
// Supported NT (Notification Type) messages:
// "uuid:"                         (length: 5 )
// "upnp:rootdevice"               (length: 15)
// "urn:schemas-upnp-org:device:"  (length: 28)
// "urn:schemas-upnp-org:service:" (length: 29)
// -----------------------------------------------------------------------------
//
TBool CUpnpDeviceLibraryElement::AddInfoFromNotificationTypeHeaderL(
    CUpnpSsdpMessage* aMessage )
    {
    TPtrC8 nt = aMessage->St( );

    if ( nt.Length( ) == 0 )
        {
        nt.Set( aMessage->Nt( ) );
        }

    if ( HasPrefix( nt, KUPnPUuid ) )
        {
        // an uuid advertisement
        return ETrue;
        }

    if ( iDeviceType->Length( ) == 0 )
        {
        if ( nt == KUPnPRootDevice )
            {
            // a root device advertisement
            SetDeviceTypeL( nt );
            return ETrue;
            }

        if ( HasPrefix( nt, KUPnPDeviceSchema ) )
            {
            // a device advertisement
            SetDeviceTypeL( nt );
            return ETrue;
            }
        }

    if ( HasPrefix( nt, KUPnPServiceSchema ) )
        {
        // a service advertisement
        AddServiceTypeL( nt );
        return ETrue;
        }

    return EFalse;
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceLibraryElement::AddInfoL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CUpnpDeviceLibraryElement::AddInfoL( CUpnpSsdpMessage* aMessage, 
                                                TInt& aUpdateId )
    {
    if ( !aMessage )
        {
        return EFalse;
        }

    if ( iDescriptionURL->Length() == 0 )
        {
        SetDescriptionUrlL( aMessage->Location() );
        }

    TBool updateInc = AddInfoFromNotificationTypeHeaderL( aMessage );

    //If iUUID message has not been defined, Set it as Message's UUID
    if ( iUUID->Length() == 0 )
        {
        SetUuidL( aMessage->Uuid() );
        }
    else
        {
        // Set update only when device didn't notify before, always 
        // when device notify - uuid will be set
        updateInc = EFalse;
        }

    if ( updateInc )
        {
        aUpdateId++;
        SetUpdateId(aUpdateId);
        }

    return updateInc;
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceLibraryElement::AddInfoL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpDeviceLibraryElement::AddInfoL(const TUpnpAddLocalDevice* anIndex,
                                                  const TDesC8& aBuffer,
                                                  const TInetAddr& aAddr)
    {
    if ( !anIndex )
        {
        return;
        }

    TInt index = 0;

    TPtrC8 uuid = aBuffer.Left(anIndex->iUuidLength);
    index += anIndex->iUuidLength;

    TPtrC8 deviceType = aBuffer.Mid(index, anIndex->iDeviceTypeLength);
    index += anIndex->iDeviceTypeLength;

    TPtrC8 descriptionUrl = aBuffer.Mid(index, anIndex->iDescriptionUrlLength);
    index += anIndex->iDescriptionUrlLength;

    TPtrC8 domain = aBuffer.Mid(index, anIndex->iDomainLength);
    index += anIndex->iDomainLength;

    TPtrC8 port = aBuffer.Mid(index, anIndex->iPortNumberLength);
    index += anIndex->iPortNumberLength;

    TPtrC8 root = aBuffer.Mid(index, anIndex->iRootDeviceLength);
    index += anIndex->iRootDeviceLength;

    if (uuid.Length()> 0)
        {
        SetUuidL(uuid);
        }

    if (deviceType.Length()> 0)
        {
        SetDeviceTypeL(deviceType);
        }

    if (root.Length()> 0)
        {
        SetRootDevice( ETrue );
        }
    else
        {
        SetRootDevice( EFalse );
        }

    if (descriptionUrl.Length()> 0)
        {
        if (aAddr.IsUnspecified())
            {
            SetDescriptionUrlL(descriptionUrl);
            }
        else
            {
            SetDescriptionUrlL(descriptionUrl, aAddr, port);
            }
        }

    if (domain.Length()> 0)
        {
        SetDomainL(domain);
        }

    for (TInt i=0; i < KMaxServiceCount; i++)
        {
        TPtrC8 serviceType = aBuffer.Mid(index, anIndex->iServiceLength[i]);
        index += anIndex->iServiceLength[i];

        AddServiceTypeL(serviceType);
        }
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceLibraryElement::AddInfoL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpDeviceLibraryElement::AddInfoL(const TUpnpAddLocalDevice* anIndex,
                                                  const TDesC8& aBuffer)
    {
    AddInfoL(anIndex, aBuffer, TInetAddr());
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceLibraryElement::SetUuidL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpDeviceLibraryElement::SetUuidL( const TDesC8& aUUID )
    {
    if ( aUUID.Length()> KUdnMaxLength )
        {
        User::Leave( KErrOverflow );
        }
    
    delete iUUID;
    iUUID = NULL;

    iUUID = aUUID.AllocL( );
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceLibraryElement::SetDescriptionUrlL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpDeviceLibraryElement::SetDescriptionUrlL(
    const TDesC8& aDescriptionURL )
    {
    if ( aDescriptionURL.Length( ) > KUrlMaxLength )
        {
        User::Leave( KErrOverflow );
        }

    delete iDescriptionURL;
    iDescriptionURL = NULL;

    iDescriptionURL = aDescriptionURL.AllocL( );
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceLibraryElement::SetDescriptionUrlL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpDeviceLibraryElement::SetDescriptionUrlL(
    const TDesC8& aDescriptionURL, const TInetAddr& aAddr, const TDesC8& aPort )
    {
    TBuf<UpnpSSDP::KIPAddBuffLen> addrBuf;
    aAddr.Output( addrBuf );

    HBufC8* addrBuf8 = UpnpString::FromUnicodeL( addrBuf );
    CleanupStack::PushL( addrBuf8 );

    HBufC8* url = HBufC8::NewLC( (UpnpHTTP::KHTTPUrl().Length( ))
            + aDescriptionURL.Length( ) + addrBuf8->Length( )
            + aPort.Length( )+ 2 );//+2 reserved for colon and slashes in url

    url->Des().Append( UpnpHTTP::KHTTPUrl( ) );
    url->Des().Append( *addrBuf8 );
    if ( aPort.Length( ) > 0 )
        {
        url->Des().Append( UpnpString::KColon( ) );
        url->Des().Append( aPort );
        }
    // After Ip address there should be slash and device Url so device url
    // must start with slash
    ASSERT( aDescriptionURL.Find( UpnpString::KSlash) == 0 ); 
   	
    url->Des().Append( aDescriptionURL );

    SetDescriptionUrlL( *url );

    CleanupStack::PopAndDestroy( url );
    CleanupStack::PopAndDestroy( addrBuf8 );
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceLibraryElement::SetDomainL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpDeviceLibraryElement::SetDomainL(const TDesC8& aDomain)
    {
    if ( aDomain.Length( ) > KDeviceTypeBufferLength )
        {
        User::Leave( KErrOverflow );
        }
    
    delete iDomain;
    iDomain = NULL;

    iDomain = aDomain.AllocL();
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceLibraryElement::SetDeviceTypeL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpDeviceLibraryElement::SetDeviceTypeL(const TDesC8& aDeviceType)
    {
    if ( aDeviceType.Length( ) > KDeviceTypeBufferLength )
        {
        User::Leave( KErrOverflow );
        }
    
    delete iDeviceType;
    iDeviceType = NULL;

    iDeviceType = aDeviceType.AllocL();
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceLibraryElement::AddServiceTypeL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpDeviceLibraryElement::AddServiceTypeL( const TDesC8& aServiceType )
    {
    if ( aServiceType.Length() > 0 )
        {
        if ( aServiceType.Length() > KServiceTypeBufferLength )
            {
            User::Leave( KErrOverflow );
            }
        // check if this service type is already mentioned 
        // in the service type list
        TInt foundIdx;
        
        if ( iServiceList->Find( aServiceType, foundIdx ) == 0 )
            {
            return;
            }

        // else, append the new service type to the list
        iServiceList->AppendL( aServiceType );
        }
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceLibraryElement::AsTDeviceL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TUpnpDevice* CUpnpDeviceLibraryElement::AsTDeviceL() const
    {
    TUpnpDevice* dev = new (ELeave) TUpnpDevice;
    dev->Set(const_cast<CUpnpDeviceLibraryElement*>(this));
    return dev;
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceLibraryElement::UpdateId
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpDeviceLibraryElement::UpdateId() const
    {
    return iUpdateId;
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceLibraryElement::SetUpdateId
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpDeviceLibraryElement::SetUpdateId(TInt aUpdateId)    
    {
    iUpdateId = aUpdateId;
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceLibraryElement::SetAlive
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpDeviceLibraryElement::SetAlive(TBool aAlive)
    {
    iAlive = aAlive;
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceLibraryElement::Alive
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CUpnpDeviceLibraryElement::Alive() const
    {
    return iAlive;
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceLibraryElement::SetLocal
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpDeviceLibraryElement::SetLocal(TBool aLocal)
    {
    iLocal = aLocal;
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceLibraryElement::Local
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CUpnpDeviceLibraryElement::Local() const
    {
    return iLocal;
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceLibraryElement::SetFilter
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpDeviceLibraryElement::SetFilter(TBool aFilter)
    {
    iFilter = aFilter;
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceLibraryElement::Filter
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CUpnpDeviceLibraryElement::Filter() const
    {
    return iFilter;
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceLibraryElement::Expired
//
// -----------------------------------------------------------------------------
//
TBool CUpnpDeviceLibraryElement::Expired() const
    {
    return iExpired;
    }
    
// -----------------------------------------------------------------------------
// CUpnpDeviceLibraryElement::SetExpired
//
// -----------------------------------------------------------------------------
//
void CUpnpDeviceLibraryElement::SetExpired( TBool aExpired )
    {
    iExpired = aExpired;
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceLibraryElement::SetRootDevice
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpDeviceLibraryElement::SetRootDevice(TBool aRootDevice)
    {
    iRootDevice = aRootDevice;
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceLibraryElement::Advertised
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CUpnpDeviceLibraryElement::Advertised()
    {
    return iAdvertised;
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceLibraryElement::Advertising
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpDeviceLibraryElement::Advertising()
    {
    iAdvertised = ETrue;
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceLibraryElement::AdvertisingFinished
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpDeviceLibraryElement::AdvertisingFinished()
    {
    iAdvertised = EFalse;
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceLibraryElement::HasPrefix
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CUpnpDeviceLibraryElement::HasPrefix( const TDesC8& aDescriptor,
    const TDesC8& aPrefix )
    {
    return (aDescriptor.Left( aPrefix.Length( ) ) == aPrefix );
    }

//  End of File
