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
* Description:  Implements the CUpnpDeviceTagContentHandler class
 *
*/


#include "upnpdevicetagcontenthandler.h"
#include "upnpcontenthandlerscontroller.h"
#include "upnpiconlistcontenthandler.h"
#include "upnpdevicelistcontenthandler.h"
#include "upnpservicelistcontenthandler.h"
#include "upnpdevice.h"
#include "upnpdescriptionproperty.h"
#include "upnpdeviceliterals.h"

//const TUint16 KReqiuredTagsBoundary(31)  //first five bits
const TUint16 KReqiuredTagsBoundary(3); //we take first two bits as reqired
enum TFlagsPositions
    {
    EDeviceTypePos = 0,
    EUDNPos,
    EFriendlyNamePos,
    EManufacturerPos,
    EModelNamePos,
    EManufacturerURLPos, //optional
    EModelDescriptionPos, //optional
    EModelNumberPos, //optional
    EModelURLPos, //optional
    ESerialNumberPos, //optional
    EUPCPos, //optional
    EIconListPos, //optional
    EServiceListPos, //optional
    EDeviceListPos, //optional
    EPresentationURLPos //optional
    };

// -----------------------------------------------------------------------------
// CUpnpDeviceTagContentHandler::NewL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
CUpnpDeviceTagContentHandler* CUpnpDeviceTagContentHandler::NewL(
    CUpnpContentHandlersController& aController, CUpnpDevice& aResultDevice,
    TParseType aParseType )
    {
    CUpnpDeviceTagContentHandler* deviceTagContentHandler = 
        CUpnpDeviceTagContentHandler::NewLC( aController, aResultDevice, 
                                             aParseType );
    CleanupStack::Pop( deviceTagContentHandler );
    return deviceTagContentHandler;
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceTagContentHandler::NewLC
// Two-phased constructor. Leaves teh object on the CleanupStack
// -----------------------------------------------------------------------------
//
CUpnpDeviceTagContentHandler* CUpnpDeviceTagContentHandler::NewLC(
    CUpnpContentHandlersController& aController, CUpnpDevice& aResultDevice,
    TParseType aParseType )
    {
    CUpnpDeviceTagContentHandler* deviceTagContentHandler = 
        new (ELeave) CUpnpDeviceTagContentHandler( aController, aResultDevice,
                aParseType );
    CleanupStack::PushL( deviceTagContentHandler );
    return deviceTagContentHandler;
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceTagContentHandler::~CUpnpDeviceTagContentHandler
// Destructor of CUpnpDeviceTagContentHandler class 
// -----------------------------------------------------------------------------
//
CUpnpDeviceTagContentHandler::~CUpnpDeviceTagContentHandler()
    {
    delete iNameWithPrefix;
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceTagContentHandler::CUpnpDeviceTagContentHandler
// Constructor
// -----------------------------------------------------------------------------
//
CUpnpDeviceTagContentHandler::CUpnpDeviceTagContentHandler(
    CUpnpContentHandlersController& aController, CUpnpDevice& aResultDevice,
    TParseType aParseType ) :
    CUpnpContentHandler( aController ), iResultDevice( aResultDevice ),
            iCurrentState( EInitial ), iParseType( aParseType )
    {
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceTagContentHandler::ProcessDevicePropertyL
// Helper function that do necessary operation when device property is found.
// -----------------------------------------------------------------------------
//
void CUpnpDeviceTagContentHandler::ProcessDevicePropertyL(
    TInt aFlagPosition, const TDesC8& aPropertyName )
    {
    if ( IsTagRepeated( aFlagPosition, iFoundTags ) )
        {
        SetIgnoreHandlerL();
        }
    else
        {
        iCurrentState = EDeviceProperty;
        iCurrentDevicePropertyName.Set( aPropertyName );
        }
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceTagContentHandler::OnStartElementL
// This method is a callback to indicate an element has been parsed.
// -----------------------------------------------------------------------------
//
void CUpnpDeviceTagContentHandler::OnStartElementL( const RTagInfo& aElement,
    const RAttributeArray& /*aAttributes*/)
    {
    if ( EInitial == iCurrentState )
        {
        const TDesC8& elementName( aElement.LocalName().DesC() );
        if ( elementName.Compare( KUpnpDeviceType ) == 0 )
            {
            if ( IsTagRepeated( EDeviceTypePos, iFoundTags ) )
                {
                SetIgnoreHandlerL();
                }
            else
                {
                iCurrentState = EDeviceType;
                }
            }
        else if ( elementName.Compare( KUpnpFriendlyName ) == 0 )
            {
            ProcessDevicePropertyL( EFriendlyNamePos, KUpnpFriendlyName );
            }
        else if ( elementName.Compare( KUpnpManufacturer ) == 0 )
            {
            ProcessDevicePropertyL( EManufacturerPos, KUpnpManufacturer );
            }
        else if ( elementName.Compare( KUpnpManufacturerURL ) == 0 )
            {
            ProcessDevicePropertyL( EManufacturerURLPos, KUpnpManufacturerURL );
            }
        else if ( elementName.Compare( KUpnpModelDescription ) == 0 )
            {
            ProcessDevicePropertyL( EModelDescriptionPos, KUpnpModelDescription );
            }
        else if ( elementName.Compare( KUpnpModelName ) == 0 )
            {
            ProcessDevicePropertyL( EModelNamePos, KUpnpModelName );
            }
        else if ( elementName.Compare( KUpnpModelNumber ) == 0 )
            {
            ProcessDevicePropertyL( EModelNumberPos, KUpnpModelNumber );
            }
        else if ( elementName.Compare( KUpnpModelURL ) == 0 )
            {
            ProcessDevicePropertyL( EModelURLPos, KUpnpModelURL );
            }
        else if ( elementName.Compare( KUpnpSerialNumber ) == 0 )
            {
            ProcessDevicePropertyL( ESerialNumberPos, KUpnpSerialNumber );
            }
        else if ( elementName.Compare( KUpnpUDN ) == 0 )
            {
            RepeatedTagCheckL( EUDNPos, iFoundTags );
            iCurrentState = EUDN;
            }
        else if ( elementName.Compare( KUpnpUPC ) == 0 )
            {
            ProcessDevicePropertyL( EUPCPos, KUpnpUPC );
            }
        else if ( elementName.Compare( KUpnpPresentationURL ) == 0 )
            {
            ProcessDevicePropertyL( EPresentationURLPos, KUpnpPresentationURL );
            }
        else if ( elementName.Compare( KUpnpIconList ) == 0 )
            {
            if ( IsTagRepeated( EIconListPos, iFoundTags ) )
                {
                SetIgnoreHandlerL();
                }
            else
                {
                iController.SetCurrentContentHandlerL( 
                    CUpnpIconListContentHandler::NewL( iController, iResultDevice ) );
                }
            }
        else if ( elementName.Compare( KUpnpServiceList ) == 0 )
            {
            if ( IsTagRepeated( EServiceListPos, iFoundTags ) )
                {
                SetIgnoreHandlerL();
                }
            else
                {
                iController.SetCurrentContentHandlerL( 
                    CUpnpServiceListContentHandler::NewL( iController, iResultDevice ) );
                }
            }
        else if ( elementName.Compare( KUpnpDeviceList ) == 0 )
            {
            if ( IsTagRepeated( EDeviceListPos, iFoundTags ) )
                {
                SetIgnoreHandlerL();
                }
            else
                {
                iController.SetCurrentContentHandlerL( 
                    CUpnpDeviceListContentHandler::NewL( iController, iResultDevice,
                    		iParseType ) );
                }
            }
        else
            {
            _LIT8(KColon, ":");
            iCurrentState = EDeviceProperty; //for f.e. dlna:tags
            delete iNameWithPrefix;
            iNameWithPrefix = NULL;
            iNameWithPrefix = HBufC8::NewL( aElement.Prefix().DesC().Length()
                            + KColon().Length()
                            + aElement.LocalName().DesC().Length() );
            iNameWithPrefix->Des().Copy( aElement.Prefix().DesC() );
            iNameWithPrefix->Des().Append( KColon );
            iNameWithPrefix->Des().Append( aElement.LocalName().DesC() );
            iCurrentDevicePropertyName.Set( *iNameWithPrefix );
            }
        }
    else
        {
        SetIgnoreHandlerL();
        //User::Leave( KErrArgument )            //content="textOnly"
        }
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceTagContentHandler::OnEndElementL
// This method is a callback to indicate the end of the element has been reached.
// -----------------------------------------------------------------------------
//
void CUpnpDeviceTagContentHandler::OnEndElementL( const RTagInfo& aElement )
    {
    switch ( iCurrentState )
        {
        case EInitial:
            ASSERT( aElement.LocalName().DesC().Compare(KUpnpDevice)==0 );
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
// CUpnpDeviceTagContentHandler::OnContentL
// This method is a callback that sends the content of the element.
// aErrorCode must be KErrNone, and that aBytes should contains complete
// content (one chunk).
// -----------------------------------------------------------------------------
//
void CUpnpDeviceTagContentHandler::OnContentL( const TDesC8& aBytes )
    {
    switch ( iCurrentState )
        {
        case EDeviceProperty:
            iResultDevice.AddDevicePropertyL( CUpnpDescriptionProperty::NewL(
                iCurrentDevicePropertyName, aBytes ) );
            break;
        case EDeviceType:
            iResultDevice.SetTypeL( aBytes );
            break;
        case EUDN:
            {
            _LIT8( KUUID, "uuid:" );
            if ( aBytes.Find( KUUID() ) == 0 ) //starts with uuid:
                {
                iResultDevice.SetUuidL( aBytes.Mid( KUUID().Length() ) );
                }
            else
                {
                iResultDevice.SetUuidL( aBytes );
                //User::Leave( KErrArgument )    
                }
            break;
            }
        default:
            //User::Leave( KErrArgument )    
            break;
        }
    }

TBool CUpnpDeviceTagContentHandler::InterestedInAllNamespaces()
    {
    return ETrue;
    }

//  End of File
