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


#include "upnpallowedvaluelistcontenthandler.h"
#include "upnpcontenthandlerscontroller.h"
#include "upnpignorecontenthandler.h"
#include "upnpstatevariable.h"
#include "upnpserviceliterals.h"

// -----------------------------------------------------------------------------
// CUpnpAllowedValueListContentHandler::NewL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
CUpnpAllowedValueListContentHandler* CUpnpAllowedValueListContentHandler::NewL(
    CUpnpContentHandlersController& aController,
    CUpnpStateVariable& aResultStateVariable )
    {
    CUpnpAllowedValueListContentHandler* actionContentHandler = 
        CUpnpAllowedValueListContentHandler::NewLC( aController, aResultStateVariable );
    CleanupStack::Pop( actionContentHandler );
    return actionContentHandler;
    }

// -----------------------------------------------------------------------------
// CUpnpAllowedValueListContentHandler::NewLC
// Two-phased constructor. Leaves teh object on the CleanupStack
// -----------------------------------------------------------------------------
//
CUpnpAllowedValueListContentHandler* CUpnpAllowedValueListContentHandler::NewLC(
    CUpnpContentHandlersController& aController,
    CUpnpStateVariable& aResultStateVariable )
    {
    CUpnpAllowedValueListContentHandler* actionContentHandler = 
        new (ELeave) CUpnpAllowedValueListContentHandler(aController, 
            aResultStateVariable);
    CleanupStack::PushL( actionContentHandler );
    return actionContentHandler;
    }

// -----------------------------------------------------------------------------
// CUpnpAllowedValueListContentHandler::~CUpnpAllowedValueListContentHandler
// Destructor of CUpnpAllowedValueListContentHandler class 
// -----------------------------------------------------------------------------
//
CUpnpAllowedValueListContentHandler::~CUpnpAllowedValueListContentHandler()
    {
    }

// -----------------------------------------------------------------------------
// CUpnpAllowedValueListContentHandler::CUpnpAllowedValueListContentHandler
// Constructor
// -----------------------------------------------------------------------------
//
CUpnpAllowedValueListContentHandler::CUpnpAllowedValueListContentHandler(
    CUpnpContentHandlersController& aController,
    CUpnpStateVariable& aResultStateVariable ) :
    CUpnpContentHandler( aController ),
            iResultStateVariable( aResultStateVariable ),
            iIsInsideAllowedValue( EFalse )
    {
    }

// -----------------------------------------------------------------------------
// CUpnpAllowedValueListContentHandler::OnStartElementL
// This method is a callback to indicate an element has been parsed.
// -----------------------------------------------------------------------------
//
void CUpnpAllowedValueListContentHandler::OnStartElementL(
    const RTagInfo& aElement, const RAttributeArray& /*aAttributes*/ )
    {
    if ( !iIsInsideAllowedValue )
        {
        const TDesC8& elementName( aElement.LocalName().DesC() );
        if ( elementName.Compare( KUpnpAllowedValue ) == 0 )
            {
            iIsInsideAllowedValue = ETrue;
            }
        else
            {
            iController.SetCurrentContentHandlerL( 
                CUpnpIgnoreContentHandler::NewL( iController ) );
            }
        }
    else
        {
        iController.SetCurrentContentHandlerL( 
            CUpnpIgnoreContentHandler::NewL( iController ) );
        //User::Leave( KErrArgument ) //content="textOnly" not checked now
        }
    }

// -----------------------------------------------------------------------------
// CUpnpAllowedValueListContentHandler::OnEndElementL
// This method is a callback to indicate the end of the element has been reached.
// -----------------------------------------------------------------------------
//
void CUpnpAllowedValueListContentHandler::OnEndElementL( const RTagInfo& aElement )
    {
    if ( iIsInsideAllowedValue )
        {
        ASSERT( aElement.LocalName().DesC().Compare(KUpnpAllowedValue) == 0 );
        iIsInsideAllowedValue = EFalse;
        }
    else
        {
        ASSERT(aElement.LocalName().DesC().Compare(KUpnpAllowedValueList)==0);
        iController.SetPreviousContentHandler();

        }
    }

// -----------------------------------------------------------------------------
// CUpnpAllowedValueListContentHandler::OnContentL
// This method is a callback that sends the content of the element.
// aErrorCode must be KErrNone, and that aBytes should contains complete
// content (one chunk).
// -----------------------------------------------------------------------------
//
void CUpnpAllowedValueListContentHandler::OnContentL( const TDesC8& aBytes )
    {
    if ( iIsInsideAllowedValue )
        {
        iResultStateVariable.AddAllowedValueL( aBytes );
        }
    else
        {
        //User::Leave( KErrArgument )
        }
    }

//  End of File
