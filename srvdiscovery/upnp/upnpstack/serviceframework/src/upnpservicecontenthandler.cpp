/** @file
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implements the CUpnpBaseContentHandler class
*
*/


#include "upnpservicecontenthandler.h"
#include "upnpservice.h"
#include "upnpcontenthandlerscontroller.h"
#include "upnpscpdcontenthandler.h"
#include "upnpserviceliterals.h"

// -----------------------------------------------------------------------------
// CUpnpServiceContentHandler::NewL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
CUpnpServiceContentHandler* CUpnpServiceContentHandler::NewL(
        CUpnpContentHandlersController& aController, CUpnpDevice* aParentDevice )
    {
    CUpnpServiceContentHandler* serviceContentHandler =
        CUpnpServiceContentHandler::NewLC( aController, aParentDevice );
    CleanupStack::Pop( serviceContentHandler );
    return serviceContentHandler;
    }

// -----------------------------------------------------------------------------
// CUpnpServiceContentHandler::NewLC
// Two-phased constructor. Leaves teh object on the CleanupStack
// -----------------------------------------------------------------------------
CUpnpServiceContentHandler* CUpnpServiceContentHandler::NewLC(
        CUpnpContentHandlersController& aController, CUpnpDevice* aParentDevice )
    {
    CUpnpServiceContentHandler* serviceContentHandler =
        new (ELeave) CUpnpServiceContentHandler( aController, aParentDevice );
    CleanupStack::PushL( serviceContentHandler );
    return serviceContentHandler;
    }

// -----------------------------------------------------------------------------
// CUpnpServiceContentHandler::NewL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
CUpnpServiceContentHandler* CUpnpServiceContentHandler::NewL(
        CUpnpContentHandlersController& aController,
        CUpnpService& aNotOwnedResultService )
    {
    CUpnpServiceContentHandler* serviceContentHandler =
        CUpnpServiceContentHandler::NewLC( aController, aNotOwnedResultService );
    CleanupStack::Pop( serviceContentHandler );
    return serviceContentHandler;
    }

// -----------------------------------------------------------------------------
// CUpnpServiceContentHandler::NewLC
// Two-phased constructor. Leaves teh object on the CleanupStack
// -----------------------------------------------------------------------------
//
CUpnpServiceContentHandler* CUpnpServiceContentHandler::NewLC(
        CUpnpContentHandlersController& aController,
        CUpnpService& aNotOwnedResultService )
    {
    CUpnpServiceContentHandler* serviceContentHandler = 
        new (ELeave) CUpnpServiceContentHandler(aController, 
            aNotOwnedResultService);
    CleanupStack::PushL( serviceContentHandler );
    return serviceContentHandler;
    }

// -----------------------------------------------------------------------------
// CUpnpServiceContentHandler::~CUpnpServiceContentHandler
// Destructor of CUpnpServiceContentHandler class 
// -----------------------------------------------------------------------------
//
CUpnpServiceContentHandler::~CUpnpServiceContentHandler()
    {
    if ( iIsServiceOwned )
        {
        delete iResultService;
        }
    }

// -----------------------------------------------------------------------------
// CUpnpServiceContentHandler::CUpnpServiceContentHandler
// Constructor
// -----------------------------------------------------------------------------
//
CUpnpServiceContentHandler::CUpnpServiceContentHandler(
        CUpnpContentHandlersController& aController, CUpnpDevice* aParentDevice ) :
        CUpnpContentHandler(aController), iParentDevice(aParentDevice),
        iIsServiceOwned(ETrue)
    {
    }

// -----------------------------------------------------------------------------
// CUpnpServiceContentHandler::CUpnpServiceContentHandler
// Constructor
// -----------------------------------------------------------------------------
//
CUpnpServiceContentHandler::CUpnpServiceContentHandler(
        CUpnpContentHandlersController& aController,
        CUpnpService& aNotOwnedResultService ) :
        CUpnpContentHandler(aController),
        iResultService( &aNotOwnedResultService ), iIsServiceOwned( EFalse )
    {
    }

// -----------------------------------------------------------------------------
// CUpnpServiceContentHandler::OnStartElementL
// This method is a callback to indicate an element has been parsed.
// -------------------------------------------------------- ---------------------
//
void CUpnpServiceContentHandler::OnStartElementL( const RTagInfo& aElement,
        const RAttributeArray& /*aAttributes*/ )
    {
    if ( aElement.LocalName().DesC().Compare( KUpnpScpd ) == 0 )
        {
        if ( iIsServiceOwned )
            {
            delete iResultService;
            iResultService = NULL;
            iResultService = CUpnpService::NewL( iParentDevice );
            iParentDevice = NULL;
            }
        iController.SetCurrentContentHandlerL( CUpnpScpdContentHandler::NewL(
                        iController, *iResultService ) );
        }
    else
        {
        User::Leave( KErrArgument ); //wrong root node
        }
    }

// -----------------------------------------------------------------------------
// CUpnpServiceContentHandler::OnEndElementL
// This method is a callback to indicate the end of the element has been reached.
// -----------------------------------------------------------------------------
//
void CUpnpServiceContentHandler::OnEndElementL( 
    const RTagInfo& /*aElement*/ )
    {
    }
    
// -----------------------------------------------------------------------------
// CUpnpServiceContentHandler::OnContentL
// This method is a callback that sends the content of the element.
// -----------------------------------------------------------------------------
//
void CUpnpServiceContentHandler::OnContentL( const TDesC8& /*aBytes*/ )
    {
    }
    
// -----------------------------------------------------------------------------
// CUpnpServiceContentHandler::ResultService()
// Returns parsed CUpnpService object, and pass ownership to the caller.
// One shouldn't call this method if CUpnpServiceContentHandler dosn't have
// ownership CUpnpService object.
// -----------------------------------------------------------------------------
//
CUpnpService* CUpnpServiceContentHandler::ResultService()
    {
    ASSERT( iIsServiceOwned );
    CUpnpService* result = iResultService;
    iIsServiceOwned= EFalse;
    iResultService = NULL;
    return result;
    }

//  End of File
