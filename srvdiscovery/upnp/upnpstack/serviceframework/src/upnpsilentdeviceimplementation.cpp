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
* Description:  Defines the CUpnpSilentDeviceImplementation class
*
*/


// INCLUDE FILES

#include <e32math.h>
#include <xml/dom/xmlengdom.h> 

#include <upnpdevicedescriptionstore.h>
#include "upnpfileutils.h"
#include "upnpcustomlog.h"
#include "upnpsilentdeviceimplementation.h"
#include "upnpsilentdeviceimplcontenthandler.h"
#include "upnpcontenthandlerscontroller.h"

// -----------------------------------------------------------------------------
// CUpnpSilentDeviceImplementation::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpSilentDeviceImplementation* CUpnpSilentDeviceImplementation::NewL(
                            const TDesC8& aUri, 
                            CUpnpDeviceDescriptionStore& aDescriptionStore,
                            MUpnpDeviceDescriptionProvider& aProvider )
    {
    LOGS("CUpnpSilentDeviceImplementation::NewL( const TDesC& aFilename )" );

    HBufC8* descr = UpnpFileUtil::ReadFileL( aDescriptionStore.DescriptionFile() );
    CleanupStack::PushL( descr );

    CUpnpContentHandlersController* controller =  CUpnpContentHandlersController::NewLC( );

    CUpnpSilentDeviceImplementation* deviceImpl = controller->ParseSilentDeviceImplL( *descr );

    CleanupStack::PushL( deviceImpl );
    deviceImpl->ConstructL( aUri, aDescriptionStore, aProvider );
    CleanupStack::Pop( deviceImpl );

    CleanupStack::PopAndDestroy( controller );
    CleanupStack::PopAndDestroy( descr );

    return deviceImpl;
    }
    
CUpnpSilentDeviceImplementation::CUpnpSilentDeviceImplementation()
    {
    }

EXPORT_C CUpnpSilentDeviceImplementation::~CUpnpSilentDeviceImplementation()
    {
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceImplementation::ConstructL
// -----------------------------------------------------------------------------
//
void CUpnpSilentDeviceImplementation::ConstructL(         
        const TDesC8& aUri, 
        CUpnpDeviceDescriptionStore& aDescriptionStore,
        MUpnpDeviceDescriptionProvider& aProvider )
    {
    BaseConstructL( aUri, aDescriptionStore, aProvider );
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceImplementationBase::StartL
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpSilentDeviceImplementation::StartL( const TInt aPort )
    {   
    StartHttpServerL( aPort );
    GenerateUdnsL( this );
    SaveDescriptionStoreL();
    }

// -----------------------------------------------------------------------------
// CUpnpSilentDeviceImplementation ::StopL
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpSilentDeviceImplementation::StopL()
    {
    StopDeviceL();
    }
        
// -----------------------------------------------------------------------------
// CUpnpSilentDeviceImplementation::StopDeviceL
// -----------------------------------------------------------------------------
//
void CUpnpSilentDeviceImplementation::StopDeviceL()
    {
    SaveDescriptionStoreL();
    StopHttpServer();
    }   

// End of File
