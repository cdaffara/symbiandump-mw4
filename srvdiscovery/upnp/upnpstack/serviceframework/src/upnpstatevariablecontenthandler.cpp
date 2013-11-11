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


#include "upnpstatevariablecontenthandler.h"
#include "upnpcontenthandlerscontroller.h"
#include "upnpignorecontenthandler.h"
#include "upnpallowedvaluerangecontenthandler.h"
#include "upnpallowedvaluelistcontenthandler.h"
#include "upnpstatevariable.h"
#include "upnpserviceliterals.h"

const TUint8 KReqiuredTagsBoundary(3); //first two bits
enum TFlagsPositions
    {
    ENamePos = 0,
    EDataTypePos,
    EDefaultValuePos, //optional
    EAllowedValuePos //optional, AllowedValueList or AllowedValueRange
    };

// -----------------------------------------------------------------------------
// CUpnpStateVariableContentHandler::NewL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
CUpnpStateVariableContentHandler* CUpnpStateVariableContentHandler::NewL(
    CUpnpContentHandlersController& aController,
    CUpnpStateVariable& aResultStateVariable )
    {
    CUpnpStateVariableContentHandler* actionContentHandler = 
        CUpnpStateVariableContentHandler::NewLC( aController, aResultStateVariable );
    CleanupStack::Pop( actionContentHandler );
    return actionContentHandler;
    }

// -----------------------------------------------------------------------------
// CUpnpStateVariableContentHandler::NewLC
// Two-phased constructor. Leaves teh object on the CleanupStack
// -----------------------------------------------------------------------------
//
CUpnpStateVariableContentHandler* CUpnpStateVariableContentHandler::NewLC(
    CUpnpContentHandlersController& aController,
    CUpnpStateVariable& aResultStateVariable )
    {
    CUpnpStateVariableContentHandler* actionContentHandler = 
        new (ELeave) CUpnpStateVariableContentHandler( aController, 
            aResultStateVariable );
    CleanupStack::PushL( actionContentHandler );
    return actionContentHandler;
    }

// -----------------------------------------------------------------------------
// CUpnpStateVariableContentHandler::~CUpnpStateVariableContentHandler
// Destructor of CUpnpStateVariableContentHandler class 
// -----------------------------------------------------------------------------
//
CUpnpStateVariableContentHandler::~CUpnpStateVariableContentHandler()
    {
    }

// -----------------------------------------------------------------------------
// CUpnpStateVariableContentHandler::CUpnpStateVariableContentHandler
// Constructor
// -----------------------------------------------------------------------------
//
CUpnpStateVariableContentHandler::CUpnpStateVariableContentHandler(
    CUpnpContentHandlersController& aController,
    CUpnpStateVariable& aResultStateVariable ) :
        CUpnpContentHandler( aController ),
        iResultStateVariable( aResultStateVariable ),
        iCurrentState( EInitial )
    {
    }

// -----------------------------------------------------------------------------
// CUpnpStateVariableContentHandler::OnStartElementL
// This method is a callback to indicate an element has been parsed.
// -----------------------------------------------------------------------------
//
void CUpnpStateVariableContentHandler::OnStartElementL(
    const RTagInfo& aElement, const RAttributeArray& /*aAttributes*/ )
    {
    if ( iCurrentState == EInitial )
        {
        const TDesC8& elementName( aElement.LocalName().DesC() );
        if ( elementName.Compare( KUpnpName ) == 0 )
            {
            RepeatedTagCheckL( ENamePos, iFoundTags );
            iCurrentState = EName;
            }
        else if ( elementName.Compare( KUpnpDataType ) == 0 )
            {
            RepeatedTagCheckL( EDataTypePos, iFoundTags );
            iCurrentState = EDataType;
            }
        else if ( elementName.Compare( KUpnpDefaultValue ) == 0 )
            {
            RepeatedTagCheckL( EDefaultValuePos, iFoundTags );
            iCurrentState = EDefaultValue;
            }
        else if ( elementName.Compare( KUpnpAllowedValueList ) == 0 )
            {
            RepeatedTagCheckL( EAllowedValuePos, iFoundTags );
            iController.SetCurrentContentHandlerL( 
                CUpnpAllowedValueListContentHandler::NewL(
                    iController, iResultStateVariable ) );
            }
        else if ( elementName.Compare( KUpnpAllowedValueRange ) == 0 )
            {
            RepeatedTagCheckL( EAllowedValuePos, iFoundTags );
            iController.SetCurrentContentHandlerL( 
                CUpnpAllowedValueRangeContentHandler::NewL(
                    iController, iResultStateVariable ) );
            }
        else
            {
            iController.SetCurrentContentHandlerL( 
                CUpnpIgnoreContentHandler::NewL( iController ) );
            }
        }
    else
        {
        //User::Leave( KErrArgument ) //content="textOnly" not checked now
        iController.SetCurrentContentHandlerL( 
            CUpnpIgnoreContentHandler::NewL( iController ) );
        }
    }

// -----------------------------------------------------------------------------
// CUpnpStateVariableContentHandler::OnEndElementL
// This method is a callback to indicate the end of the element has been reached.
// -----------------------------------------------------------------------------
//
void CUpnpStateVariableContentHandler::OnEndElementL( const RTagInfo& aElement )
    {
    switch ( iCurrentState )
        {
        case EInitial:
            ASSERT( aElement.LocalName().DesC().Compare(KUpnpStateVariable)==0 );
            if ( (iFoundTags.iFlags & KReqiuredTagsBoundary) == 
                    KReqiuredTagsBoundary )
                {
                iController.SetPreviousContentHandler();
                }
            else
                {
                //User::Leave( KErrArgument ) //required tag not found                    
                iController.SetPreviousContentHandler();
                }
            break;
        default:
            iCurrentState = EInitial;
        }
    }

// -----------------------------------------------------------------------------
// CUpnpStateVariableContentHandler::OnContentL
// This method is a callback that sends the content of the element.
// aErrorCode must be KErrNone, and that aBytes should contains complete
// content (one chunk).
// -----------------------------------------------------------------------------
//
void CUpnpStateVariableContentHandler::OnContentL( const TDesC8& aBytes )
    {
    switch ( iCurrentState )
        {
        case EName:
            iResultStateVariable.SetNameL( aBytes );
            break;
        case EDataType:
            iResultStateVariable.SetTypeL( aBytes );
            break;
        case EDefaultValue:
            iResultStateVariable.SetDefaultValueL( aBytes );
            break;
        default:
            //User::Leave( KErrArgument ) //eltOnly not checked now
            break;
        }
    }

//  End of File
