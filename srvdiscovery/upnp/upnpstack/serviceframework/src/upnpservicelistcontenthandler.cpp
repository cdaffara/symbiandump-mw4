/** @file
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implements the CUpnpServiceListContentHandler class
 *
*/


#include "upnpservicelistcontenthandler.h"
#include "upnpcontenthandlerscontroller.h"
#include "upnpservicetagcontenthandler.h"
#include "upnpservice.h"
#include "upnpdevice.h"
#include "upnpdeviceliterals.h"

// -----------------------------------------------------------------------------
// CUpnpDeviceListContentHandler::NewL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
CUpnpServiceListContentHandler* CUpnpServiceListContentHandler::NewL(
    CUpnpContentHandlersController& aController, CUpnpDevice& aResultDevice )
    {
    CUpnpServiceListContentHandler* serviceListContentHandler = 
        CUpnpServiceListContentHandler::NewLC( aController, aResultDevice );
    CleanupStack::Pop( serviceListContentHandler );
    return serviceListContentHandler;
    }

// -----------------------------------------------------------------------------
// CUpnpServiceListContentHandler::NewLC
// Two-phased constructor. Leaves teh object on the CleanupStack
// -----------------------------------------------------------------------------
//
CUpnpServiceListContentHandler* CUpnpServiceListContentHandler::NewLC(
    CUpnpContentHandlersController& aController, CUpnpDevice& aResultDevice )
    {
    CUpnpServiceListContentHandler* serviceListContentHandler = 
        new (ELeave) CUpnpServiceListContentHandler( aController, aResultDevice );
    CleanupStack::PushL( serviceListContentHandler );
    return serviceListContentHandler;
    }

// -----------------------------------------------------------------------------
// CUpnpServiceListContentHandler::~CUpnpServiceListContentHandler
// Destructor of CUpnpServiceListContentHandler class 
// -----------------------------------------------------------------------------
//
CUpnpServiceListContentHandler::~CUpnpServiceListContentHandler( )
    {
    }

// -----------------------------------------------------------------------------
// CUpnpServiceListContentHandler::CUpnpServiceListContentHandler
// Constructor
// -----------------------------------------------------------------------------
//
CUpnpServiceListContentHandler::CUpnpServiceListContentHandler(
    CUpnpContentHandlersController& aController, CUpnpDevice& aResultDevice ) :
    CUpnpContentHandler( aController ), iResultDevice( aResultDevice )
    {
    }

// -----------------------------------------------------------------------------
// CUpnpServiceListContentHandler::OnStartElementL
// This method is a callback to indicate an element has been parsed.
// -----------------------------------------------------------------------------
//
void CUpnpServiceListContentHandler::OnStartElementL(
    const RTagInfo& aElement, const RAttributeArray& /*aAttributes*/)
    {
    if ( aElement.LocalName().DesC().Compare( KUpnpService ) == 0 )
        {
        CUpnpService* service = CUpnpService::NewL( &iResultDevice );
        CleanupStack::PushL( service );
        iResultDevice.ServiceList().AppendL( service );
        CleanupStack::Pop( service );
        iController.SetCurrentContentHandlerL( CUpnpServiceTagContentHandler::NewL(
            iController, *service ) );
        }
    else
        {
        SetIgnoreHandlerL( );
        }
    }

// -----------------------------------------------------------------------------
// CUpnpServiceListContentHandler::OnEndElementL
// This method is a callback to indicate the end of the element has been reached.
// -----------------------------------------------------------------------------
//
void CUpnpServiceListContentHandler::OnEndElementL( const RTagInfo& aElement )
    {
    ASSERT( aElement.LocalName().DesC().Compare( KUpnpServiceList ) == 0 );
    iController.SetPreviousContentHandler();
    }

// -----------------------------------------------------------------------------
// CUpnpServiceListContentHandler::OnContentL
// This method is a callback that sends the content of the element.
// -----------------------------------------------------------------------------
//
void CUpnpServiceListContentHandler::OnContentL( const TDesC8& /*aBytes*/)
    {
    //User::Leave( KErrArgument )    
    }

//  End of File
