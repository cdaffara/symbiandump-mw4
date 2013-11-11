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
* Description:  Implements the CUpnpContentHandler class
 *
*/


#include "upnpcontenthandler.h"
#include "upnpcontenthandlerscontroller.h"
#include "upnpignorecontenthandler.h"

// -----------------------------------------------------------------------------
// CUpnpContentHandler::CUpnpContentHandler
// Constructor
// -----------------------------------------------------------------------------
//
CUpnpContentHandler::CUpnpContentHandler(
    CUpnpContentHandlersController& aController ) :
    iController( aController )
    {
    }

// -----------------------------------------------------------------------------
// CUpnpContentHandler::InterestedInAllNamespaces
// Returns value if class is interested in tags all namespaces
// -----------------------------------------------------------------------------
//
TBool CUpnpContentHandler::InterestedInAllNamespaces()
    {
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CUpnpContentHandler::SetIgnoreHandlerL
// Set controller's current content handler to IgnoreContentHandler object
// -----------------------------------------------------------------------------
//
void CUpnpContentHandler::SetIgnoreHandlerL()
    {
    iController.SetCurrentContentHandlerL( 
        CUpnpIgnoreContentHandler::NewL( iController ) );
    }

// -----------------------------------------------------------------------------
// CUpnpContentHandler::ResetState
// Resets internal state
// -----------------------------------------------------------------------------
//
void CUpnpContentHandler::ResetState()
    {
    // no implementation required in base class    
    }
//  End of File
