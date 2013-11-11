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
* Description:  Implements the CUpnpSingleTagContentHandler class
 *
*/


#include "upnpsingletagcontenthandler.h"
#include "upnpcontenthandlerscontroller.h"

// -----------------------------------------------------------------------------
// CUpnpSingleTagContentHandler::NewL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
CUpnpSingleTagContentHandler* CUpnpSingleTagContentHandler::NewL(
    CUpnpContentHandlersController& aController,
    RBuf8& aTagName, RBuf8& aValue )
    {
    CUpnpSingleTagContentHandler* singleTagContentHandler =
        CUpnpSingleTagContentHandler::NewLC( aController, aTagName, aValue );
    CleanupStack::Pop( singleTagContentHandler );
    return singleTagContentHandler;
    }

// -----------------------------------------------------------------------------
// CUpnpSingleTagContentHandler::NewLC
// Two-phased constructor. Leaves teh object on the CleanupStack
// -----------------------------------------------------------------------------
//
CUpnpSingleTagContentHandler* CUpnpSingleTagContentHandler::NewLC(
    CUpnpContentHandlersController& aController,
    RBuf8& aTagName, RBuf8& aValue )
    {
    CUpnpSingleTagContentHandler* singleTagContentHandler =
        new (ELeave) CUpnpSingleTagContentHandler( aController, aTagName, aValue );
    CleanupStack::PushL( singleTagContentHandler );
    return singleTagContentHandler;
    }

// -----------------------------------------------------------------------------
// CUpnpSingleTagContentHandler::~CUpnpSingleTagContentHandler
// Destructor of CUpnpSingleTagContentHandler class
// -----------------------------------------------------------------------------
//
CUpnpSingleTagContentHandler::~CUpnpSingleTagContentHandler()
    {
    }

// -----------------------------------------------------------------------------
// CUpnpSingleTagContentHandler::CUpnpSingleTagContentHandler
// Constructor
// -----------------------------------------------------------------------------
//
CUpnpSingleTagContentHandler::CUpnpSingleTagContentHandler(
    CUpnpContentHandlersController& aController,
    RBuf8& aTagName, RBuf8& aValue ) :
    CUpnpContentHandler( aController ), iTagName( aTagName ), iValue( aValue )
    {
    iTagName.Zero();
    iValue.Zero();
    }

// -----------------------------------------------------------------------------
// CUpnpSingleTagContentHandler::OnStartElementL
// This method is a callback to indicate an element has been parsed.
// -----------------------------------------------------------------------------
//
void CUpnpSingleTagContentHandler::OnStartElementL(
    const RTagInfo& aElement, const RAttributeArray& /*aAttributes*/)
    {
    if ( iIsInsideOfTag )
        {
        SetIgnoreHandlerL();
        }
    else
        {
        SafeCopyL( iTagName, aElement.LocalName().DesC() );
        iIsInsideOfTag = ETrue;
        }
    }

// -----------------------------------------------------------------------------
// CUpnpSingleTagContentHandler::OnEndElementL
// This method is a callback to indicate the end of the element has been reached.
// -----------------------------------------------------------------------------
//
void CUpnpSingleTagContentHandler::OnEndElementL( const RTagInfo& aElement )
    {
    if ( iIsInsideOfTag )
        {
        ASSERT( aElement.LocalName().DesC().Compare( iTagName ) == 0 );
        iIsInsideOfTag = EFalse;
        }
    else
        {
        iController.SetPreviousContentHandler();
        }
    }

// -----------------------------------------------------------------------------
// CUpnpSingleTagContentHandler::OnContentL
// This method is a callback that sends the content of the element.
// -----------------------------------------------------------------------------
//
void CUpnpSingleTagContentHandler::OnContentL( const TDesC8& aBytes )
    {
    if ( iIsInsideOfTag )
        {
        SafeCopyL( iValue, aBytes );
        }
    }

// -----------------------------------------------------------------------------
// CUpnpSingleTagContentHandler::InterestedInAllNamespaces
// We are interested in tags from all possible namespaces
// -----------------------------------------------------------------------------
//
TBool CUpnpSingleTagContentHandler::InterestedInAllNamespaces()
    {
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CUpnpSingleTagContentHandler::SafeCopyL
// Copy bytes from aSrc to aDest, and realloc aDest if its length is too less.
// -----------------------------------------------------------------------------
//
void CUpnpSingleTagContentHandler::SafeCopyL( RBuf8& aDest, const TDesC8& aSrc )
    {
    if ( aDest.MaxLength() < aSrc.Length() )
        {
        aDest.ReAllocL( aSrc.Length() );
        }
    aDest.Copy( aSrc );
    }

//  End of File
