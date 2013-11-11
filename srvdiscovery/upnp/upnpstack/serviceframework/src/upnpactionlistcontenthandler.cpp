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
* Description:  Implements the CUpnpActionListContentHandler class
 *
*/


#include "upnpactionlistcontenthandler.h"
#include "upnpcontenthandlerscontroller.h"
#include "upnpignorecontenthandler.h"
#include "upnpactioncontenthandler.h"
#include "upnpaction.h"
#include "upnpservice.h"
#include "upnpserviceliterals.h"

// -----------------------------------------------------------------------------
// CUpnpActionListContentHandler::NewL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
CUpnpActionListContentHandler* CUpnpActionListContentHandler::NewL(
    CUpnpContentHandlersController& aController, CUpnpService& aResultService )
    {
    CUpnpActionListContentHandler* actionListContentHandler = 
        CUpnpActionListContentHandler::NewLC( aController, aResultService );
    CleanupStack::Pop( actionListContentHandler );
    return actionListContentHandler;
    }

// -----------------------------------------------------------------------------
// CUpnpActionListContentHandler::NewLC
// Two-phased constructor. Leaves teh object on the CleanupStack
// -----------------------------------------------------------------------------
//
CUpnpActionListContentHandler* CUpnpActionListContentHandler::NewLC(
    CUpnpContentHandlersController& aController, CUpnpService& aResultService )
    {
    CUpnpActionListContentHandler* actionListContentHandler = 
        new (ELeave) CUpnpActionListContentHandler( aController, aResultService );
    CleanupStack::PushL( actionListContentHandler );
    return actionListContentHandler;
    }

// -----------------------------------------------------------------------------
// CUpnpActionListContentHandler::~CUpnpActionListContentHandler
// Destructor of CUpnpActionListContentHandler class 
// -----------------------------------------------------------------------------
//
CUpnpActionListContentHandler::~CUpnpActionListContentHandler()
    {
    }

// -----------------------------------------------------------------------------
// CUpnpActionListContentHandler::CUpnpActionListContentHandler
// Constructor
// -----------------------------------------------------------------------------
//
CUpnpActionListContentHandler::CUpnpActionListContentHandler(
    CUpnpContentHandlersController& aController, CUpnpService& aResultService ) :
    CUpnpContentHandler( aController ), iResultService( aResultService )
    {
    }

// -----------------------------------------------------------------------------
// CUpnpActionListContentHandler::OnStartElementL
// This method is a callback to indicate an element has been parsed.
// -----------------------------------------------------------------------------
//
void CUpnpActionListContentHandler::OnStartElementL(
    const RTagInfo& aElement, const RAttributeArray& /*aAttributes*/ )
    {
    const TDesC8& elementName( aElement.LocalName().DesC() );
    if ( elementName.Compare( KUpnpAction ) == 0 )
        {
        CUpnpAction* action = CUpnpAction::NewL( iResultService );
        CleanupStack::PushL( action );
        iResultService.AddActionL( *action );
        CleanupStack::Pop( action );
        iController.SetCurrentContentHandlerL( 
            CUpnpActionContentHandler::NewL( iController, *action ) );
        }
    else
        {
        iController.SetCurrentContentHandlerL( 
            CUpnpIgnoreContentHandler::NewL( iController ) );
        }
    }

// -----------------------------------------------------------------------------
// CUpnpActionListContentHandler::OnEndElementL
// This method is a callback to indicate the end of the element has been reached.
// -----------------------------------------------------------------------------
//
void CUpnpActionListContentHandler::OnEndElementL( const RTagInfo& aElement )
    {
    ASSERT( aElement.LocalName().DesC().Compare( KUpnpActionList ) == 0 );
    iController.SetPreviousContentHandler();
    }

// -----------------------------------------------------------------------------
// CUpnpActionListContentHandler::OnContentL
// This method is a callback that sends the content of the element.
// aErrorCode must be KErrNone, and that aBytes should contains complete
// content (one chunk).
// -----------------------------------------------------------------------------
//
void CUpnpActionListContentHandler::OnContentL( const TDesC8& /*aBytes*/ )
    {
    //User::Leave( KErrArgument ) 
    }

//  End of File
