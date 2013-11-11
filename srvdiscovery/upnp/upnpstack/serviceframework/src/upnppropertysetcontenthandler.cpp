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
* Description:  Implements the CUpnpPropertysetContentHandler class
 *
*/


#include "upnppropertysetcontenthandler.h"
#include "upnpcontenthandlerscontroller.h"
#include "upnpdescriptionproperty.h"
#include "upnpsingletagcontenthandler.h"

const TInt KDefaultPropertyLength( 32 );

// -----------------------------------------------------------------------------
// CUpnpPropertysetContentHandler::NewL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
CUpnpPropertysetContentHandler* CUpnpPropertysetContentHandler::NewL(
    CUpnpContentHandlersController& aController,
    RPointerArray<CUpnpDescriptionProperty>& aResultPropertyset )
    {
    CUpnpPropertysetContentHandler* propertysetContentHandler =
        CUpnpPropertysetContentHandler::NewLC( aController, aResultPropertyset );
    CleanupStack::Pop( propertysetContentHandler );
    return propertysetContentHandler;
    }

// -----------------------------------------------------------------------------
// CUpnpPropertysetContentHandler::NewLC
// Two-phased constructor. Leaves teh object on the CleanupStack
// -----------------------------------------------------------------------------
//
CUpnpPropertysetContentHandler* CUpnpPropertysetContentHandler::NewLC(
    CUpnpContentHandlersController& aController,
    RPointerArray<CUpnpDescriptionProperty>& aResultPropertyset )
    {
    CUpnpPropertysetContentHandler* propertysetContentHandler =
        new (ELeave) CUpnpPropertysetContentHandler( aController, aResultPropertyset );
    CleanupStack::PushL( propertysetContentHandler );
    propertysetContentHandler->ConstructL();
    return propertysetContentHandler;
    }

// -----------------------------------------------------------------------------
// CUpnpPropertysetContentHandler::~CUpnpPropertysetContentHandler
// Destructor of CUpnpPropertysetContentHandler class
// -----------------------------------------------------------------------------
//
CUpnpPropertysetContentHandler::~CUpnpPropertysetContentHandler()
    {
    iCurrentPropertyKey.Close();
    iCurrentPropertyValue.Close();
    }

// -----------------------------------------------------------------------------
// CUpnpPropertysetContentHandler::CUpnpPropertysetContentHandler
// Constructor
// -----------------------------------------------------------------------------
//
CUpnpPropertysetContentHandler::CUpnpPropertysetContentHandler(
    CUpnpContentHandlersController& aController,
    RPointerArray<CUpnpDescriptionProperty>& aResultPropertyset ) :
    CUpnpContentHandler( aController ), iResultPropertyset( aResultPropertyset )
    {
    }

// -----------------------------------------------------------------------------
// CUpnpPropertysetContentHandler::ConstructL
// Second phase constructor
// -----------------------------------------------------------------------------
//
void CUpnpPropertysetContentHandler::ConstructL()
    {
    iCurrentPropertyKey.CreateL( KDefaultPropertyLength );
    iCurrentPropertyValue.CreateL( KDefaultPropertyLength );
    }

// -----------------------------------------------------------------------------
// CUpnpPropertysetContentHandler::OnStartElementL
// This method is a callback to indicate an element has been parsed.
// -----------------------------------------------------------------------------
//
void CUpnpPropertysetContentHandler::OnStartElementL(
    const RTagInfo& aElement, const RAttributeArray& /*aAttributes*/)
    {
    if ( iIsInsidePropertyset )
        {
        if ( aElement.LocalName().DesC().Compare( KUpnpProperty ) == 0 )
            {
            StoreCurrentPropertyL();
            iController.SetCurrentContentHandlerL( CUpnpSingleTagContentHandler::NewL(
                iController, iCurrentPropertyKey, iCurrentPropertyValue ) );
            }
        else
            {
            SetIgnoreHandlerL();
            }
        }
    else
        {
        if ( aElement.LocalName().DesC().Compare( KUpnpPropertyset ) == 0 )
            {
            iIsInsidePropertyset = ETrue;
            }
        else
            {
            User::Leave( KErrArgument ); //wrong root node
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpPropertysetContentHandler::OnEndElementL
// This method is a callback to indicate the end of the element has been reached.
// -----------------------------------------------------------------------------
//
void CUpnpPropertysetContentHandler::OnEndElementL( const RTagInfo& aElement )
    {
    if ( iIsInsidePropertyset )
        {
        ASSERT( aElement.LocalName().DesC().Compare(KUpnpPropertyset) == 0 );
        iIsInsidePropertyset = EFalse;
        StoreCurrentPropertyL();
        }
    else
        {
        ASSERT( EFalse );   
        }
    }

// -----------------------------------------------------------------------------
// CUpnpPropertysetContentHandler::OnContentL
// This method is a callback that sends the content of the element.
// -----------------------------------------------------------------------------
//
void CUpnpPropertysetContentHandler::OnContentL( const TDesC8& /*aBytes*/)
    {
    }

// -----------------------------------------------------------------------------
// CUpnpPropertysetContentHandler::StoreCurrentPropertyL
// Store parsed current property values in result set, and reset its values.
// -----------------------------------------------------------------------------
//
void CUpnpPropertysetContentHandler::StoreCurrentPropertyL()
    {
    if ( iCurrentPropertyKey.Length() > 0 )
        {
        CUpnpDescriptionProperty* currentProperty =
            CUpnpDescriptionProperty::NewL( iCurrentPropertyKey, iCurrentPropertyValue );
        CleanupStack::PushL( currentProperty );
        iResultPropertyset.AppendL( currentProperty );
        CleanupStack::Pop( currentProperty );
        iCurrentPropertyKey.Zero();
        iCurrentPropertyValue.Zero();
        }
    }
//  End of File
