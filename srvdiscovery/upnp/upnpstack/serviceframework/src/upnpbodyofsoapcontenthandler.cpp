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
* Description:  Implements the CUpnpBodyOfSoapContentHandler class
 *
*/


#include "upnpbodyofsoapcontenthandler.h"
#include "upnpcontenthandlerscontroller.h"
#include "upnpsoapliterals.h"
#include "upnpsingletagcontenthandler.h"
#include "upnperrorcodeseekercontenthandler.h"
#include "upnpdescriptionproperty.h"

_LIT8( KUpnpActionName, "ActionName" );

// -----------------------------------------------------------------------------
// CUpnpBodyOfSoapContentHandler::NewL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
CUpnpBodyOfSoapContentHandler* CUpnpBodyOfSoapContentHandler::NewL(
    CUpnpContentHandlersController& aController,
    RPointerArray<CUpnpDescriptionProperty>& aResultSetOfArguments )
    {
    CUpnpBodyOfSoapContentHandler* soapContentHandler =
        CUpnpBodyOfSoapContentHandler::NewLC( aController, aResultSetOfArguments );
    CleanupStack::Pop( soapContentHandler );
    return soapContentHandler;
    }

// -----------------------------------------------------------------------------
// CUpnpBodyOfSoapContentHandler::NewLC
// Two-phased constructor. Leaves teh object on the CleanupStack
// -----------------------------------------------------------------------------
//
CUpnpBodyOfSoapContentHandler* CUpnpBodyOfSoapContentHandler::NewLC(
    CUpnpContentHandlersController& aController,
    RPointerArray<CUpnpDescriptionProperty>& aResultSetOfArguments)
    {
    CUpnpBodyOfSoapContentHandler* soapContentHandler =
        new (ELeave) CUpnpBodyOfSoapContentHandler( aController, aResultSetOfArguments );
    CleanupStack::PushL( soapContentHandler );
    soapContentHandler->ConstructL();
    return soapContentHandler;
    }

// -----------------------------------------------------------------------------
// CUpnpBodyOfSoapContentHandler::~CUpnpBodyOfSoapContentHandler
// Destructor of CUpnpBodyOfSoapContentHandler class
// -----------------------------------------------------------------------------
//
CUpnpBodyOfSoapContentHandler::~CUpnpBodyOfSoapContentHandler()
    {
    }

// -----------------------------------------------------------------------------
// CUpnpBodyOfSoapContentHandler::CUpnpBodyOfSoapContentHandler
// Constructor
// -----------------------------------------------------------------------------
//
CUpnpBodyOfSoapContentHandler::CUpnpBodyOfSoapContentHandler(
    CUpnpContentHandlersController& aController,
                RPointerArray<CUpnpDescriptionProperty>& aResultSetOfArguments) :
    CUpnpPropertysetContentHandler( aController, aResultSetOfArguments )
    {
    }

// -----------------------------------------------------------------------------
// CUpnpBodyOfSoapContentHandler::ConstructL
// Second phase constructor
// -----------------------------------------------------------------------------
//
void CUpnpBodyOfSoapContentHandler::ConstructL()
    {
    CUpnpPropertysetContentHandler::ConstructL();
    }

// -----------------------------------------------------------------------------
// CUpnpBodyOfSoapContentHandler::OnStartElementL
// This method is a callback to indicate an element has been parsed.
// -----------------------------------------------------------------------------
//
void CUpnpBodyOfSoapContentHandler::OnStartElementL(
    const RTagInfo& aElement, const RAttributeArray& /*aAttributes*/)
    {
    const TDesC8& elementName( aElement.LocalName().DesC() );

    if ( iIsInsideOfAction )
        {
        if ( InsideOfArgument() )   
            {
            SetIgnoreHandlerL();
            }
        else
            {
            CUpnpSingleTagContentHandler::SafeCopyL( iCurrentPropertyKey,
                elementName);
            }
        }
    else
        {
        if ( elementName == KUpnpSoapFault ) 
            {
            iResultPropertyset.ResetAndDestroy();
            iCurrentPropertyKey = KUpnpSoapFault;
            iController.SetCurrentContentHandlerL(
                CUpnpErrorCodeSeekerContentHandler::NewL( iController, iCurrentPropertyValue )
            );
            }
        else
            {
            //we're assuming that now is 'actionName'/'actionName'Response tag
            //we're also ignoring namespaces
            //these both things could be changed if we want to have stricter parser
            StoreCurrentPropertyL();
            iCurrentPropertyKey = KUpnpActionName;
            CUpnpSingleTagContentHandler::SafeCopyL( iCurrentPropertyValue, elementName );
            StoreCurrentPropertyL();
            iIsInsideOfAction = ETrue;
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpBodyOfSoapContentHandler::OnEndElementL
// This method is a callback to indicate the end of the element has been reached.
// -----------------------------------------------------------------------------
//
void CUpnpBodyOfSoapContentHandler::OnEndElementL( const RTagInfo& /*aElement*/ )
    {
    if ( iIsInsideOfAction )
        {
        if ( !InsideOfArgument() )
            {
            iIsInsideOfAction = EFalse;
            }
        StoreCurrentPropertyL();
        }
    else
        {
        StoreCurrentPropertyL();
        iController.SetPreviousContentHandler();
        }
    }

// -----------------------------------------------------------------------------
// CUpnpBodyOfSoapContentHandler::OnContentL
// This method is a callback that sends the content of the element.
// -----------------------------------------------------------------------------
//
void CUpnpBodyOfSoapContentHandler::OnContentL( const TDesC8& aBytes)
    {
    if ( iIsInsideOfAction && InsideOfArgument() )
        {
        CUpnpSingleTagContentHandler::SafeCopyL( iCurrentPropertyValue, aBytes );
        }
    }

// -----------------------------------------------------------------------------
// CUpnpBodyOfSoapContentHandler::InterestedInAllNamespaces
// We are interested in tags from all possible namespaces
// Specific namespace we can check in event callback functions (On...Element)
// -----------------------------------------------------------------------------
//
TBool CUpnpBodyOfSoapContentHandler::InterestedInAllNamespaces()
    {
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CUpnpBodyOfSoapContentHandler::InsideOfArgument
// Returns true if content handler is in inside argument state
// -----------------------------------------------------------------------------
//
TBool CUpnpBodyOfSoapContentHandler::InsideOfArgument()
    {
    return (iCurrentPropertyKey.Length() > 0);
    }

//  End of File
