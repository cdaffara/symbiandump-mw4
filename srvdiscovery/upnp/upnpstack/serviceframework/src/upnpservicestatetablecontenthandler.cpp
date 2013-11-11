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


#include "upnpservicestatetablecontenthandler.h"
#include "upnpcontenthandlerscontroller.h"
#include "upnpignorecontenthandler.h"
#include "upnpstatevariablecontenthandler.h"
#include "upnpservice.h"
#include "upnpstatevariable.h"
#include "upnpserviceliterals.h"

// -----------------------------------------------------------------------------
// CUpnpServiceStateTableContentHandler::NewL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
CUpnpServiceStateTableContentHandler* CUpnpServiceStateTableContentHandler::NewL(
    CUpnpContentHandlersController& aController, CUpnpService& aResultService )
    {
    CUpnpServiceStateTableContentHandler* actionListContentHandler = 
        CUpnpServiceStateTableContentHandler::NewLC( aController, aResultService );
    CleanupStack::Pop( actionListContentHandler );
    return actionListContentHandler;
    }

// -----------------------------------------------------------------------------
// CUpnpServiceStateTableContentHandler::NewLC
// Two-phased constructor. Leaves teh object on the CleanupStack
// -----------------------------------------------------------------------------
//
CUpnpServiceStateTableContentHandler* CUpnpServiceStateTableContentHandler::NewLC(
    CUpnpContentHandlersController& aController, CUpnpService& aResultService )
    {
    CUpnpServiceStateTableContentHandler* actionListContentHandler =
        new (ELeave) CUpnpServiceStateTableContentHandler(aController, 
            aResultService);
    CleanupStack::PushL( actionListContentHandler );
    return actionListContentHandler;
    }

// -----------------------------------------------------------------------------
// CUpnpServiceStateTableContentHandler::~CUpnpServiceStateTableContentHandler
// Destructor of CUpnpServiceStateTableContentHandler class 
// -----------------------------------------------------------------------------
//
CUpnpServiceStateTableContentHandler::~CUpnpServiceStateTableContentHandler()
    {
    }

// -----------------------------------------------------------------------------
// CUpnpServiceStateTableContentHandler::CUpnpServiceStateTableContentHandler
// Constructor
// -----------------------------------------------------------------------------
//
CUpnpServiceStateTableContentHandler::CUpnpServiceStateTableContentHandler(
    CUpnpContentHandlersController& aController, CUpnpService& aResultService ) :
    CUpnpContentHandler(aController), iResultService(aResultService)
    {
    }

// -----------------------------------------------------------------------------
// CUpnpServiceStateTableContentHandler::OnStartElementL
// This method is a callback to indicate an element has been parsed.
// -----------------------------------------------------------------------------
//
void CUpnpServiceStateTableContentHandler::OnStartElementL(
    const RTagInfo& aElement, const RAttributeArray& aAttributes )
    {
    if ( aElement.LocalName().DesC().Compare(KUpnpStateVariable) == 0 )
        {
        iStateVariableFound = ETrue;
        CUpnpStateVariable* stateVariable = CUpnpStateVariable::NewL();
        CleanupStack::PushL( stateVariable );
        iResultService.AddStateVariableL( stateVariable );
        CleanupStack::Pop( stateVariable );
        ParseAttributesL( *stateVariable, aAttributes );
        iController.SetCurrentContentHandlerL( 
            CUpnpStateVariableContentHandler::NewL( iController, *stateVariable ) );
        }
    else
        {
        iController.SetCurrentContentHandlerL( 
            CUpnpIgnoreContentHandler::NewL( iController ) );
        }
    }

// -----------------------------------------------------------------------------
// CUpnpServiceStateTableContentHandler::OnEndElementL
// This method is a callback to indicate the end of the element has been reached.
// -----------------------------------------------------------------------------
//
void CUpnpServiceStateTableContentHandler::OnEndElementL(
    const RTagInfo& aElement )
    {
    ASSERT( aElement.LocalName().DesC().Compare(KUpnpServiceStateTable) == 0 );
    //if ( iStateVariableFound )
        //{
        iController.SetPreviousContentHandler();
        //}
    //else
        //{
        //User::Leave( KErrArgument ) //required but not checked now 
        //}
    }

// -----------------------------------------------------------------------------
// CUpnpServiceStateTableContentHandler::ParseAttributesL
// Parse attributes, and set correct eventable values to aResultStateVariable.
// -----------------------------------------------------------------------------
//
void CUpnpServiceStateTableContentHandler::ParseAttributesL(
    CUpnpStateVariable& aResultStateVariable,
    const RAttributeArray& aAttributes )
    {
    TBool sendEventsNotFound(ETrue);
    for ( TInt index=0; index<aAttributes.Count(); ++index )
        {
        if ( !aAttributes[index].Attribute().LocalName().DesC().Compare( KUpnpSendEvents ) )
            {
            aResultStateVariable.SetEventableL( aAttributes[index].Value().DesC() );
            sendEventsNotFound = EFalse;
            }
        }
    if ( sendEventsNotFound )
        {
        aResultStateVariable.SetEventableL( KYes );
        }
    }

// -----------------------------------------------------------------------------
// CUpnpServiceStateTableContentHandler::OnContentL
// This method is a callback that sends the content of the element.
// aErrorCode must be KErrNone, and that aBytes should contains complete
// content (one chunk).
// -----------------------------------------------------------------------------
//
void CUpnpServiceStateTableContentHandler::OnContentL( const TDesC8& /*aBytes*/ )
    {
    //User::Leave( KErrArgument ) 
    }

//  End of File
