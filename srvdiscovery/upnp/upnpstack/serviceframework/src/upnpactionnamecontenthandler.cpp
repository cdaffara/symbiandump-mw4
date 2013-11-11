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
* Description:  Implements the CUpnpActionNameContentHandler class
 *
*/


#include "upnpactionnamecontenthandler.h"
#include "upnpcontenthandlerscontroller.h"
#include "upnpignorecontenthandler.h"
#include "upnpaction.h"
#include "upnpserviceliterals.h"

// -----------------------------------------------------------------------------
// CUpnpActionNameContentHandler::NewL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
CUpnpActionNameContentHandler* CUpnpActionNameContentHandler::NewL(
    CUpnpContentHandlersController& aController, CUpnpAction& aResultAction )
    {
    CUpnpActionNameContentHandler* actionContentHandler = 
        CUpnpActionNameContentHandler::NewLC( aController, aResultAction );
    CleanupStack::Pop( actionContentHandler );
    return actionContentHandler;
    }

// -----------------------------------------------------------------------------
// CUpnpActionNameContentHandler::NewLC
// Two-phased constructor. Leaves teh object on the CleanupStack
// -----------------------------------------------------------------------------
//
CUpnpActionNameContentHandler* CUpnpActionNameContentHandler::NewLC(
    CUpnpContentHandlersController& aController, CUpnpAction& aResultAction )
    {
    CUpnpActionNameContentHandler* actionContentHandler = 
        new (ELeave) CUpnpActionNameContentHandler( aController, aResultAction );
    CleanupStack::PushL( actionContentHandler );
    return actionContentHandler;
    }

// -----------------------------------------------------------------------------
// CUpnpActionNameContentHandler::~CUpnpActionNameContentHandler
// Destructor of CUpnpActionNameContentHandler class 
// -----------------------------------------------------------------------------
//
CUpnpActionNameContentHandler::~CUpnpActionNameContentHandler()
    {
    }

// -----------------------------------------------------------------------------
// CUpnpActionNameContentHandler::CUpnpActionNameContentHandler
// Constructor
// -----------------------------------------------------------------------------
//
CUpnpActionNameContentHandler::CUpnpActionNameContentHandler(
    CUpnpContentHandlersController& aController, CUpnpAction& aResultAction ) :
    CUpnpContentHandler( aController ), iResultAction( aResultAction )
    {
    }

// -----------------------------------------------------------------------------
// CUpnpActionNameContentHandler::OnStartElementL
// This method is a callback to indicate an element has been parsed.
// -----------------------------------------------------------------------------
//
void CUpnpActionNameContentHandler::OnStartElementL(
    const RTagInfo& /*aElement*/, const RAttributeArray& /*aAttributes*/ )
    {
    //User::Leave( KErrArgument )  not checked now
    iController.SetCurrentContentHandlerL( 
        CUpnpIgnoreContentHandler::NewL( iController ) );
    //<ElementType name="name" dt:type="string" content="textOnly" />
    }

// -----------------------------------------------------------------------------
// CUpnpActionNameContentHandler::OnEndElementL
// This method is a callback to indicate the end of the element has been reached.
// -----------------------------------------------------------------------------
//
void CUpnpActionNameContentHandler::OnEndElementL( const RTagInfo& aElement )
    {
    ASSERT( aElement.LocalName().DesC().Compare( KUpnpName ) == 0 );
    iController.SetPreviousContentHandler();
    }

// -----------------------------------------------------------------------------
// CUpnpActionNameContentHandler::OnContentL
// This method is a callback that sends the content of the element.
// Not all the content may be returned in one go. The data may be sent in chunks.
// When an OnEndElementL is received this means there is no more content to be sent. 
// -----------------------------------------------------------------------------
//
void CUpnpActionNameContentHandler::OnContentL( const TDesC8& aBytes )
    {
    iResultAction.SetNameL( aBytes );
    }

//  End of File
