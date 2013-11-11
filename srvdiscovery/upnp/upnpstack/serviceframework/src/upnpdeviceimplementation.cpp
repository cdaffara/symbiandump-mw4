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
* Description:  Defines the CUpnpDeviceImplementation class
*
*/


// INCLUDE FILES

#include <e32math.h>
#include <xml/dom/xmlengdom.h> 

#include <upnpdevicedescriptionstore.h>
#include "upnpfileutils.h"
#include "upnpcustomlog.h"
#include "upnpdeviceimplementation.h"
#include "upnpdispatcher.h"
#include "upnpcontenthandlerscontroller.h"
#include "upnpdevicedescriptionprovider.h"


static const TInt KMaxDeviceDescriptionLenght(20480);
// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpDeviceImplementation::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpDeviceImplementation* CUpnpDeviceImplementation::NewL( 
        const TDesC8& aUri, 
        CUpnpDeviceDescriptionStore& aDescriptionStore,
        MUpnpDeviceDescriptionProvider& aProvider )
    {
    LOGS("CUpnpDevice:: CUpnpDevice::NewL( const TDesC& aFilename, TInt aIapId, TInt aHandle )" );

    HBufC8* descr = UpnpFileUtil::ReadFileL( aDescriptionStore.DescriptionFile() );
    if ( descr->Length()> KMaxDeviceDescriptionLenght )
        {
        delete descr;
        User::Leave( KErrTooBig );
        }
    CleanupStack::PushL( descr );

    CUpnpContentHandlersController* controller = CUpnpContentHandlersController::NewLC();

    CUpnpDeviceImplementation* deviceImpl = controller->ParseDeviceImplL( *descr );
    CleanupStack::PushL( deviceImpl );
    deviceImpl->ConstructL( aUri, aDescriptionStore, aProvider );
    CleanupStack::Pop( deviceImpl );

    CleanupStack::PopAndDestroy( controller );
    CleanupStack::PopAndDestroy( descr );

    return deviceImpl;
    }
           
CUpnpDeviceImplementation::CUpnpDeviceImplementation()
    {
    }

EXPORT_C CUpnpDeviceImplementation::~CUpnpDeviceImplementation()
    {
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceImplementation::ConstructL
// -----------------------------------------------------------------------------
//
void CUpnpDeviceImplementation::ConstructL( const TDesC8& aDeviceDescriptioUri,
                                        CUpnpDeviceDescriptionStore& aDescriptionStore,
                                        MUpnpDeviceDescriptionProvider& aProvider )
    {
    BaseConstructL( aDeviceDescriptioUri, aDescriptionStore, aProvider  );
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceImplementation::StartL
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpDeviceImplementation::StartL( const TInt aPort )
    {   
    StartHttpServerL( aPort );
    AddLocalDeviceL();
    }
    
// -----------------------------------------------------------------------------
// CUpnpDeviceImplementation::StopL
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpDeviceImplementation::StopL()
    {
    StopL( EStopNormal );
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceImplementation::StopL
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpDeviceImplementation::StopL( TStopMode aMode )
    {
    if ( ( aMode == EStopNormal  ) || ( aMode == EStopSilent ) )
        {
        StopDeviceL( aMode );
        }
    }
    
// -----------------------------------------------------------------------------
// CUpnpDeviceImplementation::StopDeviceL
// -----------------------------------------------------------------------------
//
void CUpnpDeviceImplementation::StopDeviceL( TStopMode aMode )
    {
    SaveDescriptionStoreL();
    
    if( aMode == EStopSilent )
        {
        RemoveLocalDeviceL( Uuid(), ETrue );    
        }
    else 
        {
        RemoveLocalDeviceL( Uuid(), EFalse );    
        }
    StopHttpServer();
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceImplementation::AddLocalDeviceL
// -----------------------------------------------------------------------------
//
void CUpnpDeviceImplementation::AddLocalDeviceL()
    {
    GenerateUdnsL(this);
    SaveDescriptionStoreL();
    
    const TPtrC8 uuid = Uuid();
    const TPtrC8 device = DeviceType();
    const TDesC8& url = DescriptionUrl();

    iDispatcher->AddLocalDeviceL( uuid, device, url, ServiceTypesL(), iIsRootDevice );

    delete iPath;
    iPath = NULL;
    iPath = DescriptionUrl().AllocL();
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceImplementation::RemoveLocalDeviceL
// -----------------------------------------------------------------------------
//
void CUpnpDeviceImplementation::RemoveLocalDeviceL( const TDesC8& aUuid, TBool aSilent )
    {
    iDispatcher->RemoveLocalDeviceL( aUuid, aSilent );
    }

// End of File



