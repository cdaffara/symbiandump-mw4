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
* Description:  Implements the CUpnpArgumentContentHandler class
 *
*/


#include "upnpargumentcontenthandler.h"
#include "upnpcontenthandlerscontroller.h"
#include "upnpignorecontenthandler.h"
#include "upnpargument.h"
#include "upnpserviceliterals.h"

//const TUint8 KReqiuredTagsBoundary(7) //first three bits when
//relatedStateVariable, and direction is required
const TUint8 KReqiuredTagsBoundary(1);
enum TFlagsPositions
    {
    ENamePos = 0,
    EDirectionPos,
    ERelatedStateVariablePos    //reqired, but optional for now
//    ERetvalPos 
    };

// -----------------------------------------------------------------------------
// CUpnpArgumentContentHandler::NewL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
CUpnpArgumentContentHandler* CUpnpArgumentContentHandler::NewL(
    CUpnpContentHandlersController& aController, CUpnpArgument& aResultArgument )
    {
	CUpnpArgumentContentHandler* argumentContentHandler = 
        CUpnpArgumentContentHandler::NewLC( aController, aResultArgument );
    CleanupStack::Pop( argumentContentHandler );
	return argumentContentHandler;
    }

// -----------------------------------------------------------------------------
// CUpnpArgumentContentHandler::NewLC
// Two-phased constructor. Leaves teh object on the CleanupStack
// -----------------------------------------------------------------------------
//
CUpnpArgumentContentHandler* CUpnpArgumentContentHandler::NewLC(
    CUpnpContentHandlersController& aController, CUpnpArgument& aResultArgument )
    {
    CUpnpArgumentContentHandler* argumentContentHandler = 
        new (ELeave) CUpnpArgumentContentHandler( aController, aResultArgument );
	CleanupStack::PushL( argumentContentHandler );	
	return argumentContentHandler;
    }

// -----------------------------------------------------------------------------
// CUpnpArgumentContentHandler::~CUpnpArgumentContentHandler
// Destructor of CUpnpArgumentContentHandler class 
// -----------------------------------------------------------------------------
//
CUpnpArgumentContentHandler::~CUpnpArgumentContentHandler()
    {
    }

// -----------------------------------------------------------------------------
// CUpnpArgumentContentHandler::CUpnpArgumentContentHandler
// Constructor
// -----------------------------------------------------------------------------
//
CUpnpArgumentContentHandler::CUpnpArgumentContentHandler(
    CUpnpContentHandlersController& aController, CUpnpArgument& aResultArgument ) :
    CUpnpContentHandler( aController ), iResultArgument( aResultArgument ),
            iCurrentState( EInitial )
    {
    }

// -----------------------------------------------------------------------------
// CUpnpArgumentContentHandler::OnStartElementL
// This method is a callback to indicate an element has been parsed.
// -----------------------------------------------------------------------------
//
void CUpnpArgumentContentHandler::OnStartElementL( const RTagInfo& aElement,
    const RAttributeArray& /*aAttributes*/ )
    {
    if ( iCurrentState == EInitial )
        {
        const TDesC8& elementName( aElement.LocalName().DesC() );
        if ( elementName.Compare( KUpnpName ) == 0 )
            {
            RepeatedTagCheckL( ENamePos, iFoundTags );
            iCurrentState = EName;
            }
        else if ( elementName.Compare(KUpnpDirection) == 0 )
            {
            RepeatedTagCheckL( EDirectionPos, iFoundTags );
            iCurrentState = EDirection;
            }
        else if ( elementName.Compare(KUpnpRelatedStateVariable) == 0 )
            {
            RepeatedTagCheckL( ERelatedStateVariablePos, iFoundTags );
            iCurrentState = ERelatedStateVariable;
            }
//        else if ( elementName.Compare(KUpnpRetval) == 0 ) 
//            {
//            RepeatedTagCheckL( ERetvalPos, iFoundTags )
//            iController.SetCurrentContentHandlerL(    
//                    CUpnpRetvalContentHandler::NewL( iController ) 
//                                        )
//            }        
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
        //User::Leave( KErrArgument ) 
        }
    }

// -----------------------------------------------------------------------------
// CUpnpArgumentContentHandler::OnEndElementL
// This method is a callback to indicate the end of the element has been reached.
// -----------------------------------------------------------------------------
//
void CUpnpArgumentContentHandler::OnEndElementL( const RTagInfo& aElement )
    {
    switch ( iCurrentState )
        {
        case EInitial:
            ASSERT( aElement.LocalName().DesC().Compare(KUpnpArgument) == 0 );
            if ( ( iFoundTags.iFlags & KReqiuredTagsBoundary ) == 
                                                        KReqiuredTagsBoundary )
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
// CUpnpArgumentContentHandler::OnContentL
// This method is a callback that sends the content of the element.
// aErrorCode must be KErrNone, and that aBytes should contains complete
// content (one chunk).
// -----------------------------------------------------------------------------
//
void CUpnpArgumentContentHandler::OnContentL( const TDesC8& aBytes )
    {
    switch ( iCurrentState )
        {
        case EName:
            iResultArgument.SetNameL( aBytes );
            break;
        case EDirection:
            if ( aBytes.Compare( KUpnpIn ) == 0 )
                {
                iResultArgument.SetDirectionL( EIn );
                }
            else //if ( aBytes.Compare( KUpnpOut ) == 0 )
                {
                iResultArgument.SetDirectionL( EOut );
                }
//            else
//                {
//                User::Leave( KErrArgument )  
//                }
                
            break;
        case ERelatedStateVariable:
            iResultArgument.SetRelatedStateVarL( aBytes );
            break;
        default:
            ASSERT( EInitial == iCurrentState );
            //User::Leave( KErrArgument ) 
            break;
        }
    }

//  End of File
