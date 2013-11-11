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
* Description:  Implements the CUpnpIgnoreContentHandler class
 *
*/


#include "upnpignorecontenthandler.h"
#include "upnpcontenthandlerscontroller.h"

// -----------------------------------------------------------------------------
// CUpnpIgnoreContentHandler::NewL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
CUpnpIgnoreContentHandler* CUpnpIgnoreContentHandler::NewL(
    CUpnpContentHandlersController& aController )
    {
    CUpnpIgnoreContentHandler* dummyContentHandler = 
        CUpnpIgnoreContentHandler::NewLC( aController );
    CleanupStack::Pop( dummyContentHandler );
    return dummyContentHandler;
    }

// -----------------------------------------------------------------------------
// CUpnpIgnoreContentHandler::NewLC
// Two-phased constructor. Leaves the object on the CleanupStack
// -----------------------------------------------------------------------------
//
CUpnpIgnoreContentHandler* CUpnpIgnoreContentHandler::NewLC(
    CUpnpContentHandlersController& aController )
    {
    CUpnpIgnoreContentHandler* dummyContentHandler = 
        new (ELeave) CUpnpIgnoreContentHandler( aController );
    CleanupStack::PushL( dummyContentHandler );
    return dummyContentHandler;
    }

// -----------------------------------------------------------------------------
// CUpnpContentHandler::OnStartElementL
// This method is a callback to indicate an element has been parsed.
// -----------------------------------------------------------------------------
//
void CUpnpIgnoreContentHandler::OnStartElementL( const RTagInfo& /*aElement*/, 
    const RAttributeArray& /*aAttributes*/ )
    {
    ++iOpenedTagCounter;
    }

// -----------------------------------------------------------------------------
// CUpnpIgnoreContentHandler::OnEndElementL
// This method is a callback to indicate the end of the element has been reached.
// -----------------------------------------------------------------------------
//
void CUpnpIgnoreContentHandler::OnEndElementL( const RTagInfo& /*aElement*/ )
    {
    ASSERT( iOpenedTagCounter > 0 );
    --iOpenedTagCounter;
    if ( iOpenedTagCounter == 0 )
        {
        iController.SetPreviousContentHandler();
        }
    }

// -----------------------------------------------------------------------------
// CUpnpIgnoreContentHandler::OnContentL
// This method is a callback that sends the content of the element.
// -----------------------------------------------------------------------------
//
void CUpnpIgnoreContentHandler::OnContentL( const TDesC8& /*aBytes*/ )
    {
    }
    
// -----------------------------------------------------------------------------
// CUpnpIgnoreContentHandler::CUpnpIgnoreContentHandler
// Constructor
// -----------------------------------------------------------------------------
//
CUpnpIgnoreContentHandler::CUpnpIgnoreContentHandler(
    CUpnpContentHandlersController& aController ) :
    CUpnpContentHandler( aController ), iOpenedTagCounter( 1 )
    {
    }

//  End of File
