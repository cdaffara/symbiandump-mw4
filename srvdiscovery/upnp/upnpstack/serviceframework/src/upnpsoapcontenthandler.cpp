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
* Description:  Implements the CUpnpSoapContentHandler class
 *
*/


#include "upnpsoapcontenthandler.h"
#include "upnpcontenthandlerscontroller.h"
#include "upnpbodyofsoapcontenthandler.h"

// -----------------------------------------------------------------------------
// CUpnpSoapContentHandler::NewL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
CUpnpSoapContentHandler* CUpnpSoapContentHandler::NewL(
    CUpnpContentHandlersController& aController,
    RPointerArray<CUpnpDescriptionProperty>& aParsedValues )
    {
    CUpnpSoapContentHandler* soapContentHandler =
        CUpnpSoapContentHandler::NewLC( aController, aParsedValues );
    CleanupStack::Pop( soapContentHandler );
    return soapContentHandler;
    }

// -----------------------------------------------------------------------------
// CUpnpSoapContentHandler::NewLC
// Two-phased constructor. Leaves teh object on the CleanupStack
// -----------------------------------------------------------------------------
//
CUpnpSoapContentHandler* CUpnpSoapContentHandler::NewLC(
    CUpnpContentHandlersController& aController,
    RPointerArray<CUpnpDescriptionProperty>& aParsedValues )
    {
    CUpnpSoapContentHandler* soapContentHandler =
        new (ELeave) CUpnpSoapContentHandler( aController, aParsedValues );
    CleanupStack::PushL( soapContentHandler );
    return soapContentHandler;
    }

// -----------------------------------------------------------------------------
// CUpnpSoapContentHandler::~CUpnpSoapContentHandler
// Destructor of CUpnpSoapContentHandler class
// -----------------------------------------------------------------------------
//
CUpnpSoapContentHandler::~CUpnpSoapContentHandler()
    {
    }

// -----------------------------------------------------------------------------
// CUpnpSoapContentHandler::CUpnpSoapContentHandler
// Constructor
// -----------------------------------------------------------------------------
//
CUpnpSoapContentHandler::CUpnpSoapContentHandler(
    CUpnpContentHandlersController& aController,
    RPointerArray<CUpnpDescriptionProperty>& aParsedValues ) :
    CUpnpContentHandler( aController ), iParsedValues( aParsedValues )
    {
    }

// -----------------------------------------------------------------------------
// CUpnpSoapContentHandler::OnStartElementL
// This method is a callback to indicate an element has been parsed.
// -----------------------------------------------------------------------------
//
void CUpnpSoapContentHandler::OnStartElementL(
    const RTagInfo& aElement, const RAttributeArray& /*aAttributes*/)
    {
    if ( iIsInsideEnvelope )
        {
        if ( aElement.LocalName().DesC().Compare( KUpnpSoapBody ) == 0 )
            {
            iController.SetCurrentContentHandlerL( CUpnpBodyOfSoapContentHandler::NewL(
                iController, iParsedValues ) );
            }
        else
            {
            SetIgnoreHandlerL();
            }
        }
    else
        {
        if ( aElement.LocalName().DesC().Compare( KUpnpSoapEnvelope ) == 0 )
            {
            iIsInsideEnvelope = ETrue;
            }
        else
            {
            User::Leave( KErrArgument ); //wrong root node
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpSoapContentHandler::OnEndElementL
// This method is a callback to indicate the end of the element has been reached.
// -----------------------------------------------------------------------------
//
void CUpnpSoapContentHandler::OnEndElementL( const RTagInfo& aElement )
    {
    if ( iIsInsideEnvelope )
        {
        ASSERT( aElement.LocalName().DesC().Compare(KUpnpSoapEnvelope) == 0 );
        iIsInsideEnvelope = EFalse;
        }
    else
        {
        ASSERT( EFalse );   //we should never be here
        }
    }

// -----------------------------------------------------------------------------
// CUpnpSoapContentHandler::OnContentL
// This method is a callback that sends the content of the element.
// -----------------------------------------------------------------------------
//
void CUpnpSoapContentHandler::OnContentL( const TDesC8& /*aBytes*/)
    {
    }

// -----------------------------------------------------------------------------
// CUpnpSoapContentHandler::ClearEnvelopeFlag
// Clears envleope flag
// -----------------------------------------------------------------------------
//
void CUpnpSoapContentHandler::ResetState()
    {
    iIsInsideEnvelope = EFalse;
    }
//  End of File
