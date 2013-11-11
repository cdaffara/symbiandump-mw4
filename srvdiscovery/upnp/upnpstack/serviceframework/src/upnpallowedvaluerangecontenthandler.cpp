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


#include "upnpallowedvaluerangecontenthandler.h"
#include "upnpcontenthandlerscontroller.h"
#include "upnpignorecontenthandler.h"
#include "upnpstatevariable.h"
#include "upnpserviceliterals.h"

//const TUint8 KReqiuredTagsBoundary( 3 ) 
const TUint8 KReqiuredTagsBoundary( 0 );    
enum TFlagsPositions
    {
    EMinimumPos = 0,
    EMaximumPos,
    EStepPos //optional
    };

// -----------------------------------------------------------------------------
// CUpnpAllowedValueRangeContentHandler::NewL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
CUpnpAllowedValueRangeContentHandler* CUpnpAllowedValueRangeContentHandler::NewL(
    CUpnpContentHandlersController& aController,
    CUpnpStateVariable& aResultStateVariable )
    {
    CUpnpAllowedValueRangeContentHandler* actionContentHandler = 
        CUpnpAllowedValueRangeContentHandler::NewLC( aController, aResultStateVariable );
    CleanupStack::Pop( actionContentHandler );
    return actionContentHandler;
    }

// -----------------------------------------------------------------------------
// CUpnpAllowedValueRangeContentHandler::NewLC
// Two-phased constructor. Leaves teh object on the CleanupStack
// -----------------------------------------------------------------------------
//
CUpnpAllowedValueRangeContentHandler* CUpnpAllowedValueRangeContentHandler::NewLC(
    CUpnpContentHandlersController& aController,
    CUpnpStateVariable& aResultStateVariable )
    {
    CUpnpAllowedValueRangeContentHandler* actionContentHandler = 
        new (ELeave) CUpnpAllowedValueRangeContentHandler( aController, 
            aResultStateVariable );
    CleanupStack::PushL( actionContentHandler );
    return actionContentHandler;
    }

// -----------------------------------------------------------------------------
// CUpnpAllowedValueRangeContentHandler::~CUpnpAllowedValueRangeContentHandler
// Destructor of CUpnpAllowedValueRangeContentHandler class 
// -----------------------------------------------------------------------------
//
CUpnpAllowedValueRangeContentHandler::~CUpnpAllowedValueRangeContentHandler()
    {
    }

// -----------------------------------------------------------------------------
// CUpnpAllowedValueRangeContentHandler::CUpnpAllowedValueRangeContentHandler
// Constructor
// -----------------------------------------------------------------------------
//
CUpnpAllowedValueRangeContentHandler::CUpnpAllowedValueRangeContentHandler(
    CUpnpContentHandlersController& aController,
    CUpnpStateVariable& aResultStateVariable ) :
        CUpnpContentHandler( aController ),
        iResultStateVariable( aResultStateVariable ),
        iCurrentState( EInitial )
    {
    }

// -----------------------------------------------------------------------------
// CUpnpAllowedValueRangeContentHandler::OnStartElementL
// This method is a callback to indicate an element has been parsed.
// -----------------------------------------------------------------------------
//
void CUpnpAllowedValueRangeContentHandler::OnStartElementL(
    const RTagInfo& aElement, const RAttributeArray& /*aAttributes*/ )
    {
    if ( iCurrentState == EInitial )
        {
        const TDesC8& elementName( aElement.LocalName().DesC() );
        if ( elementName.Compare(KUpnpMinimum) == 0 )
            {
            RepeatedTagCheckL( EMinimumPos, iFoundTags );
            iCurrentState = EMinimum;
            }
        else if ( elementName.Compare(KUpnpMaximum) == 0 )
            {
            RepeatedTagCheckL( EMaximumPos, iFoundTags );
            iCurrentState = EMaximum;
            }
        else if ( elementName.Compare(KUpnpStep) == 0 )
            {
            RepeatedTagCheckL( EStepPos, iFoundTags );
            iCurrentState = EStep;
            }
        else
            {
            iController.SetCurrentContentHandlerL( 
                    CUpnpIgnoreContentHandler::NewL( iController ) 
                                        );
            }
        }
    else
        {
        iController.SetCurrentContentHandlerL( 
            CUpnpIgnoreContentHandler::NewL( iController ) );
        //User::Leave( KErrArgument ) //content="textOnly"  not checked now
        }
    }

// -----------------------------------------------------------------------------
// CUpnpAllowedValueRangeContentHandler::OnEndElementL
// This method is a callback to indicate the end of the element has been reached.
// -----------------------------------------------------------------------------
//
void CUpnpAllowedValueRangeContentHandler::OnEndElementL(
    const RTagInfo& aElement )
    {
    switch ( iCurrentState )
        {
        case EInitial:
            ASSERT(aElement.LocalName().DesC().Compare(KUpnpAllowedValueRange)==0);
            if ( (iFoundTags.iFlags & KReqiuredTagsBoundary)
                    == KReqiuredTagsBoundary )
                {
                iController.SetPreviousContentHandler();
                }
            else
                {
                User::Leave( KErrArgument ); //required tag not found
                }
            break;
        default:
            iCurrentState = EInitial;
        }
    }

// -----------------------------------------------------------------------------
// CUpnpAllowedValueRangeContentHandler::OnContentL
// This method is a callback that sends the content of the element.
// aErrorCode must be KErrNone, and that aBytes should contains complete
// content (one chunk).
// -----------------------------------------------------------------------------
//
void CUpnpAllowedValueRangeContentHandler::OnContentL( const TDesC8& aBytes )
    {
    switch ( iCurrentState )
        {
        case EMinimum:
            iResultStateVariable.SetRangeMinL( aBytes );
            break;
        case EMaximum:
            iResultStateVariable.SetRangeMaxL( aBytes );
            break;
        case EStep:
            iResultStateVariable.SetRangeStepL( aBytes );
            break;
        default:
            //User::Leave( KErrArgument ) 
            break;
        }
    }

//  End of File
