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
* Description:  Implements the CUpnpIconContentHandler class
 *
*/


#include "upnpiconcontenthandler.h"
#include "upnpcontenthandlerscontroller.h"
#include "upnpicon.h"
#include "upnpdeviceliterals.h"

//const TUint8 KReqiuredTagsBoundary(31) //first five bits
enum TFlagsPositions
    {
    EMimetypePos = 0,
    EWidthPos,
    EHeightPos,
    EDepthPos,
    EUrlPos //all required
    };

// -----------------------------------------------------------------------------
// CUpnpIconContentHandler::NewL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
CUpnpIconContentHandler* CUpnpIconContentHandler::NewL(
    CUpnpContentHandlersController& aController, CUpnpIcon& aResultIcon )
    {
    CUpnpIconContentHandler* iconContentHandler = 
        CUpnpIconContentHandler::NewLC( aController, aResultIcon );
    CleanupStack::Pop( iconContentHandler );
    return iconContentHandler;
    }

// -----------------------------------------------------------------------------
// CUpnpIconContentHandler::NewLC
// Two-phased constructor. Leaves teh object on the CleanupStack
// -----------------------------------------------------------------------------
//
CUpnpIconContentHandler* CUpnpIconContentHandler::NewLC(
    CUpnpContentHandlersController& aController, CUpnpIcon& aResultIcon )
    {
    CUpnpIconContentHandler* iconContentHandler = 
        new (ELeave) CUpnpIconContentHandler( aController, aResultIcon );
    CleanupStack::PushL( iconContentHandler );
    return iconContentHandler;
    }

// -----------------------------------------------------------------------------
// CUpnpIconContentHandler::~CUpnpIconContentHandler
// Destructor of CUpnpIconContentHandler class 
// -----------------------------------------------------------------------------
//
CUpnpIconContentHandler::~CUpnpIconContentHandler()
    {
    }

// -----------------------------------------------------------------------------
// CUpnpIconContentHandler::CUpnpIconContentHandler
// Constructor
// -----------------------------------------------------------------------------
//
CUpnpIconContentHandler::CUpnpIconContentHandler(
    CUpnpContentHandlersController& aController, CUpnpIcon& aResultIcon ) :
    CUpnpContentHandler( aController ), iResultIcon( aResultIcon ),
            iCurrentState( EInitial )
    {
    }

// -----------------------------------------------------------------------------
// CUpnpIconContentHandler::OnStartElementL
// This method is a callback to indicate an element has been parsed.
// -----------------------------------------------------------------------------
//
void CUpnpIconContentHandler::OnStartElementL( const RTagInfo& aElement,
    const RAttributeArray& /*aAttributes*/)
    {
    if ( EInitial == iCurrentState  )
        {
        const TDesC8& elementName(aElement.LocalName().DesC() );
        if ( elementName.Compare( KUpnpMimetype ) == 0 )
            {
            //            RepeatedTagCheckL( EMimetypePos, iFoundTags ) NIY
            iCurrentState = EMimeType;
            }
        else if ( elementName.Compare( KUpnpWidth ) == 0 )
            {
            //            RepeatedTagCheckL( EWidthPos, iFoundTags )    NIY
            iCurrentState = EWidth;
            }
        else if ( elementName.Compare( KUpnpHeight ) == 0 )
            {
            //            RepeatedTagCheckL( EHeightPos, iFoundTags )   NIY
            iCurrentState = EHeight;
            }
        else if ( elementName.Compare( KUpnpDepth ) == 0 )
            {
            //            RepeatedTagCheckL( EDepthPos, iFoundTags )    NIY
            iCurrentState = EDepth;
            }
        else if ( elementName.Compare( KUpnpUrl ) == 0 )
            {
            //            RepeatedTagCheckL( EUrlPos, iFoundTags )      NIY
            iCurrentState = EUrl;
            }
        else
            {
            SetIgnoreHandlerL();
            }
        }
    else
        {
        //User::Leave( KErrArgument )    
        SetIgnoreHandlerL();
        }
    }

// -----------------------------------------------------------------------------
// CUpnpIconContentHandler::OnEndElementL
// This method is a callback to indicate the end of the element has been reached.
// -----------------------------------------------------------------------------
//
void CUpnpIconContentHandler::OnEndElementL( const RTagInfo& aElement )
    {
    switch ( iCurrentState )
        {
        case EInitial:
            ASSERT( aElement.LocalName().DesC().Compare( KUpnpIcon )==0 );
            //if ( (iFoundTags.iFlags & KReqiuredTagsBoundary) == KReqiuredTagsBoundary )
            //{
            iController.SetPreviousContentHandler();
            //}
            //else
            //{
            //User::Leave( KErrArgument )    //required tag not found
            //}
            break;
        default:
            iCurrentState = EInitial;
        }
    }

// -----------------------------------------------------------------------------
// CUpnpIconContentHandler::OnContentL
// This method is a callback that sends the content of the element.
// -----------------------------------------------------------------------------
//
void CUpnpIconContentHandler::OnContentL( const TDesC8& aBytes )
    {
    switch ( iCurrentState )
        {
        case EMimeType:
            iResultIcon.SetMimeTypeL( aBytes );
            break;
        case EWidth:
            iResultIcon.SetWidthL( ParseIntL( aBytes ) );
            break;
        case EHeight:
            iResultIcon.SetHeightL( ParseIntL( aBytes ) );
            break;
        case EDepth:
            iResultIcon.SetDepthL( ParseIntL( aBytes ) );
            break;
        case EUrl:
            iResultIcon.SetUrlL( aBytes );
            break;
        default:
            ASSERT( EInitial == iCurrentState );
            //User::Leave(KErrArgument) 
            break;
        }
    }

// -----------------------------------------------------------------------------
// CUpnpIconContentHandler::ParseIntL
// Parses and returns int value stored in descriptor, or leaves with KErrArgument
// if parsing failes.
// -----------------------------------------------------------------------------
//
TInt CUpnpIconContentHandler::ParseIntL( const TDesC8& aToParse )
    {
    TLex8 val(aToParse);
    TInt ret(0);
    TInt err = val.Val( ret );
    //    if ( err )
    //        {
    //        User::Leave( KErrArgument ) no error checking for now
    //        }
    return ret;
    }

//  End of File
