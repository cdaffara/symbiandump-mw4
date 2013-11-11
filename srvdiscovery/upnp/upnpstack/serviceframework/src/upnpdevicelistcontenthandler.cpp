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
* Description:  Implements the CUpnpDeviceListContentHandler class
 *
*/


#include "upnpdevicelistcontenthandler.h"
#include "upnpcontenthandlerscontroller.h"
#include "upnpdevicetagcontenthandler.h"
#include "upnpdevice.h"
#include "upnpdeviceimplementation.h"
#include "upnpsilentdeviceimplementation.h"
#include "upnpdeviceliterals.h"

// -----------------------------------------------------------------------------
// CUpnpDeviceListContentHandler::NewL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
CUpnpDeviceListContentHandler* CUpnpDeviceListContentHandler::NewL(
    CUpnpContentHandlersController& aController, CUpnpDevice& aResultDevice,
    TParseType aParseType )
    {
    CUpnpDeviceListContentHandler* deviceListContentHandler = 
        CUpnpDeviceListContentHandler::NewLC( aController, aResultDevice,
        		                              aParseType );
    CleanupStack::Pop( deviceListContentHandler );        		                              
    return deviceListContentHandler;
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceListContentHandler::NewLC
// Two-phased constructor. Leaves teh object on the CleanupStack
// -----------------------------------------------------------------------------
//
CUpnpDeviceListContentHandler* CUpnpDeviceListContentHandler::NewLC(
    CUpnpContentHandlersController& aController, CUpnpDevice& aResultDevice,
    TParseType aParseType )
    {
    CUpnpDeviceListContentHandler* deviceListContentHandler = 
        new (ELeave) CUpnpDeviceListContentHandler( aController, aResultDevice,
        		aParseType );
    CleanupStack::PushL( deviceListContentHandler );
    return deviceListContentHandler;
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceListContentHandler::~CUpnpDeviceListContentHandler
// Destructor of CUpnpDeviceListContentHandler class 
// -----------------------------------------------------------------------------
//
CUpnpDeviceListContentHandler::~CUpnpDeviceListContentHandler()
    {
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceListContentHandler::CUpnpDeviceListContentHandler
// Constructor
// -----------------------------------------------------------------------------
//
CUpnpDeviceListContentHandler::CUpnpDeviceListContentHandler(
    CUpnpContentHandlersController& aController, CUpnpDevice& aResultDevice,
    TParseType aParseType ) :
    CUpnpContentHandler( aController ), iResultDevice( aResultDevice ),
    iParseType( aParseType )
    {
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceListContentHandler::OnStartElementL
// This method is a callback to indicate an element has been parsed.
// -----------------------------------------------------------------------------
//
void CUpnpDeviceListContentHandler::OnStartElementL(
    const RTagInfo& aElement, const RAttributeArray& /*aAttributes*/)
    {
    if ( aElement.LocalName().DesC().Compare( KUpnpDevice ) == 0 )
        {
        CUpnpDevice* device( NULL );
        
        switch ( iParseType )
            {
            case EDeviceImpl:
                device = new (ELeave) CUpnpDeviceImplementation();
                break;
            case ESilentDeviceImpl:
                device = new (ELeave) CUpnpSilentDeviceImplementation();
                break;
            case EDevice:
        	    device = new (ELeave) CUpnpDevice();
                break;
            default:
				_LIT( KPanicString, "CUpnpDeviceListContentHandler::OnStartElementL" );
                User::Panic( KPanicString, 1 );  
                }
                
        CleanupStack::PushL( device );
        iResultDevice.AddDeviceL( device );
        CleanupStack::Pop( device );
        iController.SetCurrentContentHandlerL( CUpnpDeviceTagContentHandler::NewL(
            iController, *device, iParseType ) );
        }
    else
        {
        SetIgnoreHandlerL();
        }
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceListContentHandler::OnEndElementL
// This method is a callback to indicate the end of the element has been reached.
// -----------------------------------------------------------------------------
//
void CUpnpDeviceListContentHandler::OnEndElementL( const RTagInfo& aElement )
    {
    ASSERT( aElement.LocalName().DesC().Compare(KUpnpDeviceList) == 0 );
    iController.SetPreviousContentHandler();
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceListContentHandler::OnContentL
// This method is a callback that sends the content of the element.
// -----------------------------------------------------------------------------
//
void CUpnpDeviceListContentHandler::OnContentL( const TDesC8& /*aBytes*/ )
    {
    //User::Leave( KErrArgument )    
    }

//  End of File
