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
* Description:  Message dispatcher
*
*/


// INCLUDE FILES
#include <e32svr.h>
#include "upnpdispatcher.pan"
#include "upnpdispatcher.h"
#include "upnpdiscoveryobserver.h"
#include "upnpdispatcherengine.h"
#include "upnpstring.h"
#include "upnpsettings.h"
#include "upnpcustomlog.h"

#ifdef _DEBUG
#define KLogFile _L("UPnPStack.txt")
#endif

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpDispatcher::CUpnpDispatcher
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpDispatcher::CUpnpDispatcher( MUpnpDiscoveryObserver* aParent)
    : iParent( aParent )
{
}

// -----------------------------------------------------------------------------
// CUpnpDispatcher::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpDispatcher::ConstructL( )
    {
    //  LOGSH(iHandle, "Dispatcher::ConstructL");

    iEngine = CUpnpDispatcherEngine::NewL( *this );
    iUpdateId = 0;

    iEngine->UpdateDevicesL(iUpdateId);
    }

// -----------------------------------------------------------------------------
// CUpnpDispatcher::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpDispatcher* CUpnpDispatcher::NewL( MUpnpDiscoveryObserver* aParent )
{
    CUpnpDispatcher* self = NewLC( aParent );
    CleanupStack::Pop( self );
    return( self );
}

// -----------------------------------------------------------------------------
// CUpnpDispatcher::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpDispatcher* CUpnpDispatcher::NewLC( MUpnpDiscoveryObserver* aParent )
{
    CUpnpDispatcher* self = new (ELeave) CUpnpDispatcher( aParent );
    CleanupStack::PushL( self );
    self->ConstructL();

    if( aParent )
        {
        self->StartHttpServerL( EFalse , KRandomPort );
        }
    return self;
}

// -----------------------------------------------------------------------------
// CUpnpDispatcher::~CUpnpDispatcher
// Destructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpDispatcher::~CUpnpDispatcher()
{
    delete iEngine;
}

// -----------------------------------------------------------------------------
// CUpnpDispatcher::DeviceListReceivedL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpDispatcher::DeviceListReceivedL(TInt aUpdateId)
{
    //LOGSH( iHandle, "Dispatcher DeviceListReceived");
    UpdateDevicesL(aUpdateId);
}

// -----------------------------------------------------------------------------
// CUpnpDispatcher::AddCustomer
// Adds a customer to this Dispatcher.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpDispatcher::AddCustomer( const MUpnpDispatcherCustomer& aCustomer )
{
    //LOGSH( iHandle, "Dispatcher AddCustomer");
    TRAP_IGNORE(AddCustomerL(aCustomer, ETrue ));
}

// -----------------------------------------------------------------------------
// CUpnpDispatcher::RemoveCustomer
// Removes a customer from Dispatcher.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpDispatcher::RemoveCustomer( const MUpnpDispatcherCustomer& aCustomer )
{
    RemoveCustomer(aCustomer, ETrue);
}

// -----------------------------------------------------------------------------
// CUpnpDispatcher::AddCustomer
// Adds a customer to this Dispatcher.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpDispatcher::AddCustomerL( const MUpnpDispatcherCustomer& aCustomer, TBool aIsCP)
{
    //LOGSH( iHandle, "Dispatcher AddCustomer");
    iEngine->AddCustomer( aCustomer );
    if (aIsCP)
        {
        iEngine->AddControlPointClientL( );
        }
}

// -----------------------------------------------------------------------------
// CUpnpDispatcher::RemoveCustomer
// Removes a customer from Dispatcher.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpDispatcher::RemoveCustomer( const MUpnpDispatcherCustomer& aCustomer,
                                               TBool aIsCP)
{
    LOGS1("CUpnpDispatcher::RemoveCustomer 0x%x", &aCustomer);
    iEngine->RemoveCustomer( aCustomer );
    if ( aIsCP )
        {
        TRAP_IGNORE(iEngine->RemoveControlPointClientL( ));
        }
}

// -----------------------------------------------------------------------------
// CUpnpDispatcher::SendMessageL
// Sends a message using Engine's HTTP server,
// if device is local (third param), used local sending feature, type of message has to be EAction
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpDispatcher::SendMessageL( CUpnpHttpMessage* aMessage,
                                             MUpnpDispatcherCustomer& aCustomer ,
                                             TBool /*aIsLocal*/ )
{
    //LOGSH( iHandle, "Dispatcher SendMessageL");

    TPtrC8 path(0,0);
    if (iParent != NULL)
        {
        path.Set(iParent->Path());
        }


    HBufC8* wholePath = HBufC8::NewLC( path.Length()
                                       + 1
                                       + aCustomer.Path().Length() );
    TPtr8 ptr = wholePath->Des();
    ptr.Append( path );
    ptr.Append( UpnpString::KSlash() );
    ptr.Append( aCustomer.Path() );

    if ( aMessage )
    {
        aMessage->SetSenderPathL( *wholePath );
        iEngine->SendMessageL( aMessage );
    }

    CleanupStack::PopAndDestroy(wholePath);
}

// -----------------------------------------------------------------------------
// CUpnpDispatcher::UpdateDevicesL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpDispatcher::UpdateDevicesL( TInt aNewId )
    {

    //LOGSH( iHandle, "Dispatcher UpdateDevicesL");
    if ( iUpdateId != aNewId )
        {
        // Go through the new devices and add them inform
        // the Control Point of the new devices?
        const RPointerArray<CUpnpDevice>& newDevices = iEngine->NewDevices();
        for ( TInt i = 0; i < newDevices.Count(); i++ )
            {
            CUpnpDevice* dev = newDevices[i];

            if ( iParent != NULL )
                {

                if ( dev->Alive() )
                    {
                    iParent->DeviceFoundL( *dev );
                    }
                else
                    {
                    iParent->DeviceLostL( *dev );
                    }
                }

            }
        iEngine->RemoveDevices();

        iUpdateId = aNewId;
        }
    }

// -----------------------------------------------------------------------------
// CUpnpDispatcher::AddLocalDeviceL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpDispatcher::AddLocalDeviceL( const TDesC8& aUuid,
                                                const TDesC8& aDeviceType,
                                                const TDesC8& aDescriptionPath,
                                                const CDesC8Array& aServices,
                                                const TBool aIsRootDevice )
{
    //LOGSH( iHandle, "Dispatcher AddLocalDeviceL");
    iEngine->AddLocalDeviceL( aUuid, aDeviceType, aDescriptionPath, aServices, aIsRootDevice );
}

// -----------------------------------------------------------------------------
// CUpnpDispatcher::RemoveLocalDeviceL
// -----------------------------------------------------------------------------
//
void CUpnpDispatcher::RemoveLocalDeviceL( const TDesC8& aUuid, TBool aSilent )
{
    //LOGSH( iHandle, "Dispatcher RemoveLocalDeviceL");
    iEngine->RemoveLocalDeviceL( aUuid, aSilent );
}

// -----------------------------------------------------------------------------
// CUpnpDispatcher::SearchL
// Sends a SSDP search with given string.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpDispatcher::SearchL( const TDesC8& aSearchString )
{
    iEngine->SsdpSearchL( aSearchString );
}

// -----------------------------------------------------------------------------
// CUpnpDispatcher::StopFilteringL
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpDispatcher::StopFilteringL( const TDesC8& aUuids )
{
    iEngine->SsdpStopFilteringL( aUuids );
}

// -----------------------------------------------------------------------------
// CUpnpDispatcher::SearchL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpDispatcher::SearchL( const TDesC16& aSearchString )
{
    //LOGSH( iHandle, "Dispatcher SearchL");
    HBufC8* buf = UpnpString::FromUnicodeL( aSearchString );
    SearchL( *buf );
    delete buf;
}

// -----------------------------------------------------------------------------
// CUpnpDispatcher::HttpServerAddressL
// Returns the HTTP servers IP address.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInetAddr CUpnpDispatcher::HttpServerAddress()
    {
    return iEngine->HttpServerAddress();
    }

// -----------------------------------------------------------------------------
// CUpnpDispatcher::MacAddressL
// Returns the MAC address of HTTP server.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const HBufC8* CUpnpDispatcher::MacAddressL()
{
    //LOGSH( iHandle, "Dispatcher MacAddressL");
    return iEngine->MacAddressL();
}

// -----------------------------------------------------------------------------
// CUpnpDispatcher::SetTransactionCreator
// -----------------------------------------------------------------------------
//
void CUpnpDispatcher::SetTransactionCreator( 
          MUpnpHttpServerTransactionCreator* aTransactionCreator )
    {
    iEngine->SetTransactionCreator( aTransactionCreator );
    }

// -----------------------------------------------------------------------------
// CUpnpDispatcher::StartHttpServerL
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpDispatcher::StartHttpServerL( TBool aRegister , const TInt aPort )
    {
    iEngine->StartHttpServerL( aRegister , aPort );
    }

// -----------------------------------------------------------------------------
// CUpnpDispatcher::StopHttpServerL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpDispatcher::StopHttpServer()
    {
    iEngine->StopHttpServer();
    }

EXPORT_C void CUpnpDispatcher::StartIPFilteringL( )
    {
    #ifdef RD_UPNP_REMOTE_ACCESS
    iEngine->HttpServer()->StartIPFilteringL();
    #endif
    }

EXPORT_C void CUpnpDispatcher::StopIPFiltering( )
    {
    #ifdef RD_UPNP_REMOTE_ACCESS
    iEngine->HttpServer()->StopIPFiltering( );
    #endif
    }

// End of File
