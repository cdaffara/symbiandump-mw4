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
* Description:  Implements the CUpnpRootContentHandler class
 *
*/


#include "upnprootcontenthandler.h"
#include "upnpdevice.h"
#include "upnpcontenthandlerscontroller.h"
#include "upnpdevicetagcontenthandler.h"
#include "upnpdeviceliterals.h"

const TUint8 KReqiuredTagsBoundary(1); //first bit    //specVersion ignored now
enum TFlagsPositions
    {
    EDevicePos = 0,
    //ESpecVersion, required but ignored for now...
    EURLBasePos //optional
    };

// -----------------------------------------------------------------------------
// CUpnpRootContentHandler::NewL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
CUpnpRootContentHandler* CUpnpRootContentHandler::NewL(
    CUpnpContentHandlersController& aController, CUpnpDevice& aResultDevice,
    TParseType aParseType )
    {
    CUpnpRootContentHandler* rootContentHandler = 
        CUpnpRootContentHandler::NewLC( aController, aResultDevice, aParseType );
    CleanupStack::Pop( rootContentHandler );
    return rootContentHandler;
    }

// -----------------------------------------------------------------------------
// CUpnpRootContentHandler::NewLC
// Two-phased constructor. Leaves teh object on the CleanupStack
// -----------------------------------------------------------------------------
//
CUpnpRootContentHandler* CUpnpRootContentHandler::NewLC(
    CUpnpContentHandlersController& aController, CUpnpDevice& aResultDevice,
    TParseType aParseType )
    {
    CUpnpRootContentHandler* rootContentHandler = 
        new (ELeave) CUpnpRootContentHandler( aController, aResultDevice,
                aParseType );
    CleanupStack::PushL( rootContentHandler );
    return rootContentHandler;
    }

// -----------------------------------------------------------------------------
// CUpnpRootContentHandler::~CUpnpRootContentHandler
// Destructor of CUpnpRootContentHandler class 
// -----------------------------------------------------------------------------
//
CUpnpRootContentHandler::~CUpnpRootContentHandler()
    {
    }

// -----------------------------------------------------------------------------
// CUpnpRootContentHandler::CUpnpRootContentHandler
// Constructor
// -----------------------------------------------------------------------------
//
CUpnpRootContentHandler::CUpnpRootContentHandler(
    CUpnpContentHandlersController& aController, CUpnpDevice& aResultDevice,
    TParseType aParseType ) :
    CUpnpContentHandler( aController ), iResultDevice( aResultDevice ),
            iCurrentState( EInitial ), iParseType( aParseType )
    {
    }

// -----------------------------------------------------------------------------
// CUpnpRootContentHandler::OnStartElementL
// This method is a callback to indicate an element has been parsed.
// -----------------------------------------------------------------------------
//
void CUpnpRootContentHandler::OnStartElementL( const RTagInfo& aElement,
    const RAttributeArray& /*aAttributes*/)
    {
    if ( EInitial == iCurrentState )
        {
        if ( aElement.LocalName().DesC().Compare( KUpnpDevice )==0 )
            {
            RepeatedTagCheckL( EDevicePos, iFoundTags );
            iController.SetCurrentContentHandlerL( CUpnpDeviceTagContentHandler::NewL(
                iController, iResultDevice, iParseType ) );
            }
        else
            if ( aElement.LocalName().DesC().Compare( KUpnpURLBase )==0 )
                {
                //RepeatedTagCheckL( EURLBasePos, iFoundTags )  NIY
                iCurrentState = EURLBase;
                }
            else
                {
                SetIgnoreHandlerL();
                }
        }
    else
        {
        ASSERT( EURLBase == iCurrentState );
        //User::Leave(KErrArgument)  //content="textonly"
        SetIgnoreHandlerL();
        }
    }

// -----------------------------------------------------------------------------
// CUpnpRootContentHandler::OnEndElementL
// This method is a callback to indicate the end of the element has been reached.
// -----------------------------------------------------------------------------
//
void CUpnpRootContentHandler::OnEndElementL( const RTagInfo& aElement )
    {
    switch ( iCurrentState )
        {
        case EInitial:
            ASSERT( aElement.LocalName().DesC().Compare( KUpnpDeviceRoot ) == 0 );
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
// CUpnpRootContentHandler::OnContentL
// This method is a callback that sends the content of the element.
// aErrorCode must be KErrNone, and that aBytes should contains complete
// content (one chunk).
// -----------------------------------------------------------------------------
//
void CUpnpRootContentHandler::OnContentL( const TDesC8& aBytes )
    {
    switch ( iCurrentState )
        {
        case EURLBase:
            iResultDevice.SetUrlBaseL( aBytes );
            break;
        default:
            //User::Leave(KErrArgument)  
            break;
        }
    }

//  End of File
