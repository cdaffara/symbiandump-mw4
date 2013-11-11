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
* Description:  Implements the CUpnpArgumentListContentHandler class
 *
*/


#include "upnpargumentlistcontenthandler.h"
#include "upnpcontenthandlerscontroller.h"
#include "upnpignorecontenthandler.h"
#include "upnpargumentcontenthandler.h"
#include "upnpservice.h"
#include "upnpaction.h"
#include "upnpargument.h"
#include "upnpserviceliterals.h"

// -----------------------------------------------------------------------------
// CUpnpArgumentListContentHandler::NewL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
CUpnpArgumentListContentHandler* CUpnpArgumentListContentHandler::NewL(
    CUpnpContentHandlersController& aController, CUpnpService& aResultService,
    CUpnpAction& aResultAction )
    {
    CUpnpArgumentListContentHandler* argumentListContentHandler = 
        CUpnpArgumentListContentHandler::NewLC( aController, aResultService, 
                                               aResultAction );
    CleanupStack::Pop( argumentListContentHandler );                                               
    return argumentListContentHandler;
    }

// -----------------------------------------------------------------------------
// CUpnpArgumentListContentHandler::NewLC
// Two-phased constructor. Leaves teh object on the CleanupStack
// -----------------------------------------------------------------------------
//
CUpnpArgumentListContentHandler* CUpnpArgumentListContentHandler::NewLC(
    CUpnpContentHandlersController& aController, CUpnpService& aResultService,
    CUpnpAction& aResultAction )
    {
    CUpnpArgumentListContentHandler* argumentListContentHandler = 
        new (ELeave) CUpnpArgumentListContentHandler(aController, aResultService, 
            aResultAction);
    CleanupStack::PushL( argumentListContentHandler );
    return argumentListContentHandler;
    }

// -----------------------------------------------------------------------------
// CUpnpArgumentListContentHandler::~CUpnpArgumentListContentHandler
// Destructor of CUpnpArgumentListContentHandler class 
// -----------------------------------------------------------------------------
//
CUpnpArgumentListContentHandler::~CUpnpArgumentListContentHandler()
    {
    }

// -----------------------------------------------------------------------------
// CUpnpArgumentListContentHandler::CUpnpArgumentListContentHandler
// Constructor
// -----------------------------------------------------------------------------
//
CUpnpArgumentListContentHandler::CUpnpArgumentListContentHandler(
    CUpnpContentHandlersController& aController, CUpnpService& aResultService,
    CUpnpAction& aResultAction ) :
    CUpnpContentHandler( aController ), iResultService( aResultService ),
            iResultAction( aResultAction )
    {
    }

// -----------------------------------------------------------------------------
// CUpnpArgumentListContentHandler::OnStartElementL
// This method is a callback to indicate an element has been parsed.
// -----------------------------------------------------------------------------
//
void CUpnpArgumentListContentHandler::OnStartElementL(
    const RTagInfo& aElement, const RAttributeArray& /*aAttributes*/ )
    {
    TPtrC8 elementName(aElement.LocalName().DesC() );
    if ( elementName.Compare( KUpnpArgument ) == 0 )
        {
        CUpnpArgument* argument = CUpnpArgument::NewL( iResultService );
        CleanupStack::PushL( argument );
        iResultAction.AddArgumentL( *argument );
        CleanupStack::Pop( argument );
        iController.SetCurrentContentHandlerL( 
            CUpnpArgumentContentHandler::NewL( iController, *argument ) );
        }
    else
        {
        iController.SetCurrentContentHandlerL( 
            CUpnpIgnoreContentHandler::NewL( iController ) );
        }
    }

// -----------------------------------------------------------------------------
// CUpnpArgumentListContentHandler::OnEndElementL
// This method is a callback to indicate the end of the element has been reached.
// -----------------------------------------------------------------------------
//
void CUpnpArgumentListContentHandler::OnEndElementL( const RTagInfo& aElement )
    {
    ASSERT( aElement.LocalName().DesC().Compare( KUpnpArgumentList ) == 0 );
    iController.SetPreviousContentHandler();
    }

// -----------------------------------------------------------------------------
// CUpnpArgumentListContentHandler::OnContentL
// This method is a callback that sends the content of the element.
// aErrorCode must be KErrNone, and that aBytes should contains complete
// content (one chunk).
// -----------------------------------------------------------------------------
//
void CUpnpArgumentListContentHandler::OnContentL( const TDesC8& /*aBytes*/ )
    {
    //User::Leave( KErrArgument ) 
    }

//  End of File
