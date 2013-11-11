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
* Description:  Implements the CUpnpIconListContentHandler class
 *
*/


#include "upnpiconlistcontenthandler.h"
#include "upnpcontenthandlerscontroller.h"
#include "upnpiconcontenthandler.h"
#include "upnpicon.h"
#include "upnpdevice.h"
#include "upnpdeviceliterals.h"

// -----------------------------------------------------------------------------
// CUpnpIconListContentHandler::NewL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
CUpnpIconListContentHandler* CUpnpIconListContentHandler::NewL(
    CUpnpContentHandlersController& aController, CUpnpDevice& aResultDevice )
    {
    CUpnpIconListContentHandler* iconListContentHandler = 
        CUpnpIconListContentHandler::NewLC( aController, aResultDevice );
    CleanupStack::Pop( iconListContentHandler );
    return iconListContentHandler;
    }

// -----------------------------------------------------------------------------
// CUpnpIconListContentHandler::NewLC
// Two-phased constructor. Leaves teh object on the CleanupStack
// -----------------------------------------------------------------------------
//
CUpnpIconListContentHandler* CUpnpIconListContentHandler::NewLC(
    CUpnpContentHandlersController& aController, CUpnpDevice& aResultDevice )
    {
    CUpnpIconListContentHandler* iconListContentHandler = 
        new (ELeave) CUpnpIconListContentHandler( aController, aResultDevice );
    CleanupStack::PushL( iconListContentHandler );
    return iconListContentHandler;
    }

// -----------------------------------------------------------------------------
// CUpnpIconListContentHandler::~CUpnpIconListContentHandler
// Destructor of CUpnpIconListContentHandler class 
// -----------------------------------------------------------------------------
//
CUpnpIconListContentHandler::~CUpnpIconListContentHandler()
    {
    }

// -----------------------------------------------------------------------------
// CUpnpIconListContentHandler::CUpnpIconListContentHandler
// Constructor
// -----------------------------------------------------------------------------
//
CUpnpIconListContentHandler::CUpnpIconListContentHandler(
    CUpnpContentHandlersController& aController, CUpnpDevice& aResultDevice ) :
    CUpnpContentHandler( aController ), iResultDevice( aResultDevice )
    {
    }

// -----------------------------------------------------------------------------
// CUpnpIconListContentHandler::OnStartElementL
// This method is a callback to indicate an element has been parsed.
// -----------------------------------------------------------------------------
//
void CUpnpIconListContentHandler::OnStartElementL( const RTagInfo& aElement,
    const RAttributeArray& /*aAttributes*/)
    {
    if ( aElement.LocalName().DesC().Compare( KUpnpIcon ) == 0 )
        {
        CUpnpIcon* icon = CUpnpIcon::NewL();
        CleanupStack::PushL( icon );
        iResultDevice.AddIconL( icon );
        CleanupStack::Pop( icon );

        iController.SetCurrentContentHandlerL( CUpnpIconContentHandler::NewL(
            iController, *icon ) );
        }
    else
        {
        SetIgnoreHandlerL();
        }
    }

// -----------------------------------------------------------------------------
// CUpnpIconListContentHandler::OnEndElementL
// This method is a callback to indicate the end of the element has been reached.
// -----------------------------------------------------------------------------
//
void CUpnpIconListContentHandler::OnEndElementL( const RTagInfo& aElement )
    {
    ASSERT( aElement.LocalName().DesC().Compare( KUpnpIconList ) == 0 );
    iController.SetPreviousContentHandler();
    }

// -----------------------------------------------------------------------------
// CUpnpIconListContentHandler::OnContentL
// This method is a callback that sends the content of the element.
// aErrorCode must be KErrNone, and that aBytes should contains complete
// content (one chunk).
// -----------------------------------------------------------------------------
//
void CUpnpIconListContentHandler::OnContentL( const TDesC8& /*aBytes*/)
    {
    //User::Leave( KErrArgument )    
    }

//  End of File
