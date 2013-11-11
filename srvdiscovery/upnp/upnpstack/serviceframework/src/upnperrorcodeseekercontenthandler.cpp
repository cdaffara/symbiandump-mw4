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
* Description:  Implements the CUpnpErrorCodeSeekerContentHandler class
 *
*/


#include "upnperrorcodeseekercontenthandler.h"
#include "upnpcontenthandlerscontroller.h"
#include "upnpsingletagcontenthandler.h"
#include "upnpsoapliterals.h"


// -----------------------------------------------------------------------------
// CUpnpErrorCodeSeekerContentHandler::NewL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
CUpnpErrorCodeSeekerContentHandler* CUpnpErrorCodeSeekerContentHandler::NewL(
    CUpnpContentHandlersController& aController, RBuf8& aErrorValue )
    {
    CUpnpErrorCodeSeekerContentHandler* seekerContentHandler =
        CUpnpErrorCodeSeekerContentHandler::NewLC( aController, aErrorValue );
    CleanupStack::Pop( seekerContentHandler );
    return seekerContentHandler;
    }

// -----------------------------------------------------------------------------
// CUpnpErrorCodeSeekerContentHandler::NewLC
// Two-phased constructor. Leaves teh object on the CleanupStack
// -----------------------------------------------------------------------------
//
CUpnpErrorCodeSeekerContentHandler* CUpnpErrorCodeSeekerContentHandler::NewLC(
    CUpnpContentHandlersController& aController, RBuf8& aErrorValue )
    {
    CUpnpErrorCodeSeekerContentHandler* seekerContentHandler =
        new (ELeave) CUpnpErrorCodeSeekerContentHandler( aController, aErrorValue );
    CleanupStack::PushL( seekerContentHandler );
    return seekerContentHandler;
    }

// -----------------------------------------------------------------------------
// CUpnpErrorCodeSeekerContentHandler::~CUpnpErrorCodeSeekerContentHandler
// Destructor of CUpnpErrorCodeSeekerContentHandler class
// -----------------------------------------------------------------------------
//
CUpnpErrorCodeSeekerContentHandler::~CUpnpErrorCodeSeekerContentHandler()
    {
    }

// -----------------------------------------------------------------------------
// CUpnpErrorCodeSeekerContentHandler::CUpnpErrorCodeSeekerContentHandler
// Constructor
// -----------------------------------------------------------------------------
//
CUpnpErrorCodeSeekerContentHandler::CUpnpErrorCodeSeekerContentHandler(
    CUpnpContentHandlersController& aController, RBuf8& aErrorValue ) :
    CUpnpContentHandler( aController ), iErrorValue( aErrorValue )
    {
    iErrorValue.Zero();
    iCurrentState.Set( KUpnpSoapFault );
    }

// -----------------------------------------------------------------------------
// CUpnpErrorCodeSeekerContentHandler::OnStartElementL
// This method is a callback to indicate an element has been parsed.
// -----------------------------------------------------------------------------
//
void CUpnpErrorCodeSeekerContentHandler::OnStartElementL(
    const RTagInfo& aElement, const RAttributeArray& /*aAttributes*/)
    {
    SetNextStateL( aElement.LocalName().DesC() );
    }

// -----------------------------------------------------------------------------
// CUpnpErrorCodeSeekerContentHandler::OnEndElementL
// This method is a callback to indicate the end of the element has been reached.
// -----------------------------------------------------------------------------
//
void CUpnpErrorCodeSeekerContentHandler::OnEndElementL( const RTagInfo& aElement )
    {
    ASSERT( aElement.LocalName().DesC().Compare( iCurrentState ) == 0 );
    SetPreviousState();
    }

// -----------------------------------------------------------------------------
// CUpnpErrorCodeSeekerContentHandler::OnContentL
// This method is a callback that sends the content of the element.
// -----------------------------------------------------------------------------
//
void CUpnpErrorCodeSeekerContentHandler::OnContentL( const TDesC8& aBytes )
    {
    if ( iCurrentState == KUpnpSoapErrorCode )
        {
        CUpnpSingleTagContentHandler::SafeCopyL( iErrorValue, aBytes );
        }
    }

// -----------------------------------------------------------------------------
// CUpnpErrorCodeSeekerContentHandler::InterestedInAllNamespaces
// We are interested in tags from all possible namespaces
// -----------------------------------------------------------------------------
//
TBool CUpnpErrorCodeSeekerContentHandler::InterestedInAllNamespaces()
    {
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CUpnpErrorCodeSeekerContentHandler::SetNextStateL
// Set next state of parsing. If we're interested in particular subtree
// state will be handeled by this class, otherwise by ignore handler
// -----------------------------------------------------------------------------
//
void CUpnpErrorCodeSeekerContentHandler::SetNextStateL(
    const TDesC8& aFoundTagName )
    {
    if ( iCurrentState == KUpnpSoapFault )
        {
        SetParticularNextStateL( KUpnpSoapDetail, aFoundTagName );
        }
    else if ( iCurrentState == KUpnpSoapDetail )
        {
        SetParticularNextStateL( KUpnpSoapError, aFoundTagName );
        }
    else if ( iCurrentState == KUpnpSoapError )
        {
        SetParticularNextStateL( KUpnpSoapErrorCode, aFoundTagName );
        }
    else
        {
        SetIgnoreHandlerL();
        }
    }

// -----------------------------------------------------------------------------
// CUpnpErrorCodeSeekerContentHandler::SetPreviousState()
// Set previous state of parsing. When particular tag is ended
// state of parsind process should be updated.
// -----------------------------------------------------------------------------
//
void CUpnpErrorCodeSeekerContentHandler::SetPreviousState()
    {
    if ( iCurrentState == KUpnpSoapFault )
        {
        iController.SetPreviousContentHandler();
        }
    else if ( iCurrentState == KUpnpSoapDetail )
        {
        iCurrentState.Set( KUpnpSoapFault );
        }
    else if ( iCurrentState == KUpnpSoapError )
        {
        iCurrentState.Set( KUpnpSoapDetail );
        }
    else if ( iCurrentState == KUpnpSoapErrorCode )
        {
        iCurrentState.Set( KUpnpSoapError );
        }
    else
        {
        ASSERT( EFalse );   //wrong state
        }
    }

// -----------------------------------------------------------------------------
// CUpnpErrorCodeSeekerContentHandler::SetParticularNextStateL()
// Set particular next state of parsing. If we're interested in particular subtree
// state will be handeled by this class, otherwise by ignore handler
// -----------------------------------------------------------------------------
//
void CUpnpErrorCodeSeekerContentHandler::SetParticularNextStateL(
    const TDesC8& aNextAcceptedState, const TDesC8& aFoundTagName )
    {
    if ( aFoundTagName == aNextAcceptedState )
        {
        iCurrentState.Set( aNextAcceptedState );
        }
    else
        {
        SetIgnoreHandlerL();
        }
    }
//  End of File
