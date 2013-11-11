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
* Description:  Implements the CUpnpScpdContentHandler class
 *
*/


#include "upnpscpdcontenthandler.h"
#include "upnpcontenthandlerscontroller.h"
#include "upnpignorecontenthandler.h"
#include "upnpactionlistcontenthandler.h"
#include "upnpservicestatetablecontenthandler.h"
#include "upnpservice.h"
#include "upnpserviceliterals.h"

//const TUint8 KReqiuredTagsBoundary(1) //first bit (no missing specVersion checking)
const TUint8 KReqiuredTagsBoundary(0);  //no required tag checking for now
enum TFlagsPositions
    {
    EServiceStateTable = 0,
    //ESpecVersion,           //no missing/repeated specVersion checking
    EActionList //optional
    };

// -----------------------------------------------------------------------------
// CUpnpScpdContentHandler::NewL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
CUpnpScpdContentHandler* CUpnpScpdContentHandler::NewL(
    CUpnpContentHandlersController& aController, CUpnpService& aResultService )
    {
    CUpnpScpdContentHandler* scpdContentHandler = 
        CUpnpScpdContentHandler::NewLC( aController, aResultService );
    CleanupStack::Pop( scpdContentHandler );
    return scpdContentHandler;
    }

// -----------------------------------------------------------------------------
// CUpnpScpdContentHandler::NewLC
// Two-phased constructor. Leaves teh object on the CleanupStack
// -----------------------------------------------------------------------------
//
CUpnpScpdContentHandler* CUpnpScpdContentHandler::NewLC(
    CUpnpContentHandlersController& aController, CUpnpService& aResultService )
    {
    CUpnpScpdContentHandler* scpdContentHandler = 
        new (ELeave) CUpnpScpdContentHandler( aController, aResultService );
    CleanupStack::PushL( scpdContentHandler );
    return scpdContentHandler;
    }

// -----------------------------------------------------------------------------
// CUpnpScpdContentHandler::~CUpnpScpdContentHandler
// Destructor of CUpnpScpdContentHandler class 
// -----------------------------------------------------------------------------
//
CUpnpScpdContentHandler::~CUpnpScpdContentHandler()
    {
    }

// -----------------------------------------------------------------------------
// CUpnpScpdContentHandler::CUpnpScpdContentHandler
// Constructor
// -----------------------------------------------------------------------------
//
CUpnpScpdContentHandler::CUpnpScpdContentHandler(
    CUpnpContentHandlersController& aController, CUpnpService& aResultService ) :
    CUpnpContentHandler( aController ), iResultService( aResultService )
    {
    }

// -----------------------------------------------------------------------------
// CUpnpScpdContentHandler::OnStartElementL
// This method is a callback to indicate an element has been parsed.
// -----------------------------------------------------------------------------
//
void CUpnpScpdContentHandler::OnStartElementL( const RTagInfo& aElement,
    const RAttributeArray& /*aAttributes*/ )
    {
    TPtrC8 elementName(aElement.LocalName().DesC() );
    if ( elementName.Compare( KUpnpActionList ) == 0 )
        {
        RepeatedTagCheckL( EActionList, iFoundTags );
        iController.SetCurrentContentHandlerL( CUpnpActionListContentHandler::NewL(
            iController, iResultService ) );
        }
    else
        if ( elementName.Compare( KUpnpServiceStateTable ) == 0 )
            {
        RepeatedTagCheckL( EServiceStateTable, iFoundTags );
            iController.SetCurrentContentHandlerL( 
                CUpnpServiceStateTableContentHandler::NewL(
                    iController, iResultService ) );
            }
        //not checked by current version
        //    else if ( elementName.Compare(KUpnpSpecVersion) == 0 )
        //        {
        //        RepeatedTagCheckL( ESpecVersion, iFoundTags )
        //        iController.SetCurrentContentHandlerL( 
        //                CUpnpSpecVersionContentHandler::NewL( iController ) 
        //                                    )
        //        }
        else
            {
            iController.SetCurrentContentHandlerL( 
                CUpnpIgnoreContentHandler::NewL( iController ) );
            }
    }

// -----------------------------------------------------------------------------
// CUpnpScpdContentHandler::OnEndElementL
// This method is a callback to indicate the end of the element has been reached.
// 
// -----------------------------------------------------------------------------
//
void CUpnpScpdContentHandler::OnEndElementL( const RTagInfo& aElement )
    {
    ASSERT( aElement.LocalName().DesC().Compare(KUpnpScpd) == 0 );
    if ( ( iFoundTags.iFlags & KReqiuredTagsBoundary ) == KReqiuredTagsBoundary )
        {
        iResultService.SetArgumentTypes();
        iController.SetPreviousContentHandler();
        }
    else
        {
        User::Leave( KErrArgument ); //required tag not found
        }
    }

// -----------------------------------------------------------------------------
// CUpnpScpdContentHandler::OnContentL
// This method is a callback that sends the content of the element.
// aErrorCode must be KErrNone, and that aBytes should contains complete
// content (one chunk).
// -----------------------------------------------------------------------------
//
void CUpnpScpdContentHandler::OnContentL( const TDesC8& /*aBytes*/ )
    {
    //User::Leave( KErrArgument ) 
    }

//  End of File
