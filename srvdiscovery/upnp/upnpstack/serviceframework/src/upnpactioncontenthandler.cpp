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
* Description:  Implements the CUpnpActionContentHandler class
 *
*/


#include "upnpactioncontenthandler.h"
#include "upnpcontenthandlerscontroller.h"
#include "upnpignorecontenthandler.h"
#include "upnpactionnamecontenthandler.h"
#include "upnpargumentlistcontenthandler.h"
#include "upnpaction.h"
#include "upnpserviceliterals.h"

enum TFlagsPositions
    {
    EName = 0,
    EArgumentList 
    };

// -----------------------------------------------------------------------------
// CUpnpActionContentHandler::NewL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
CUpnpActionContentHandler* CUpnpActionContentHandler::NewL(
    CUpnpContentHandlersController& aController, CUpnpAction& aResultAction )
    {
    CUpnpActionContentHandler* actionContentHandler = 
        CUpnpActionContentHandler::NewLC( aController, aResultAction );
    CleanupStack::Pop( actionContentHandler );
    return actionContentHandler;
    }

// -----------------------------------------------------------------------------
// CUpnpActionContentHandler::NewLC
// Two-phased constructor. Leaves teh object on the CleanupStack
// -----------------------------------------------------------------------------
//
CUpnpActionContentHandler* CUpnpActionContentHandler::NewLC(
    CUpnpContentHandlersController& aController, CUpnpAction& aResultAction )
    {
    CUpnpActionContentHandler* actionContentHandler = 
        new (ELeave) CUpnpActionContentHandler( aController, aResultAction );
    CleanupStack::PushL( actionContentHandler );
    return actionContentHandler;
    }

// -----------------------------------------------------------------------------
// CUpnpActionContentHandler::~CUpnpActionContentHandler
// Destructor of CUpnpActionContentHandler class 
// -----------------------------------------------------------------------------
//
CUpnpActionContentHandler::~CUpnpActionContentHandler()
    {
    }

// -----------------------------------------------------------------------------
// CUpnpActionContentHandler::CUpnpActionContentHandler
// Constructor
// -----------------------------------------------------------------------------
//
CUpnpActionContentHandler::CUpnpActionContentHandler(
    CUpnpContentHandlersController& aController, CUpnpAction& aResultAction ) :
    CUpnpContentHandler( aController ), iResultAction( aResultAction )
    {
    }

// -----------------------------------------------------------------------------
// CUpnpActionContentHandler::OnStartElementL
// This method is a callback to indicate an element has been parsed.
// -----------------------------------------------------------------------------
//
void CUpnpActionContentHandler::OnStartElementL( const RTagInfo& aElement,
    const RAttributeArray& /*aAttributes*/ )
    {
    const TDesC8& elementName( aElement.LocalName().DesC() );
    if ( elementName.Compare( KUpnpArgumentList ) == 0 )
        {
        RepeatedTagCheckL( EArgumentList, iFoundTags );
        iController.SetCurrentContentHandlerL( 
            CUpnpArgumentListContentHandler::NewL(
                iController, iResultAction.Service(), iResultAction ) );
        }
    else
        if ( elementName.Compare( KUpnpName ) == 0 )
            {
            RepeatedTagCheckL( EName, iFoundTags );
            iController.SetCurrentContentHandlerL( 
                CUpnpActionNameContentHandler::NewL( iController, iResultAction ) );
            }
        else
            {
            iController.SetCurrentContentHandlerL( 
                CUpnpIgnoreContentHandler::NewL( iController ) );
            }
    }

// -----------------------------------------------------------------------------
// CUpnpActionContentHandler::OnEndElementL
// This method is a callback to indicate the end of the element has been reached.
// -----------------------------------------------------------------------------
//
void CUpnpActionContentHandler::OnEndElementL( const RTagInfo& aElement )
    {
    ASSERT( aElement.LocalName().DesC().Compare( KUpnpAction ) == 0 );
    if ( iFoundTags.IsClear( EName ) )
        {
        User::Leave( KErrArgument ); //required
        }
    iController.SetPreviousContentHandler();
    }

// -----------------------------------------------------------------------------
// CUpnpActionContentHandler::OnContentL
// This method is a callback that sends the content of the element.
// aErrorCode must be KErrNone, and that aBytes should contains complete
// content (one chunk).
// -----------------------------------------------------------------------------
//
void CUpnpActionContentHandler::OnContentL( const TDesC8& /*aBytes*/ )
    {
    //User::Leave( KErrArgument ) 
    }

//  End of File
