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
* Description:  Implements the CUpnpDeviceContentHandler class
 *
*/


#include "upnpdevicecontenthandler.h"
#include "upnpdevice.h"
#include "upnpdeviceimplementation.h"
#include "upnpsilentdeviceimplementation.h"
#include "upnpcontenthandlerscontroller.h"
#include "upnprootcontenthandler.h"
#include "upnpdeviceliterals.h"

// -----------------------------------------------------------------------------
// CUpnpDeviceContentHandler::NewL
// Two-phased constructor that will parse CUpnpDevice object. If aResultDevice 
// is NULL the result object will be created during the parsing, and will be 
// owned by this CUpnpDeviceContentHandler.
// Otherwise result will be parsed to existing aResultDevice object
// -----------------------------------------------------------------------------
//
CUpnpDeviceContentHandler* CUpnpDeviceContentHandler::NewL(
    CUpnpContentHandlersController& aController, CUpnpDevice* aResultDevice )
    {
    CUpnpDeviceContentHandler* deviceContentHandler = 
        CUpnpDeviceContentHandler::NewLC( aController, aResultDevice );
    CleanupStack::Pop( deviceContentHandler );
    return deviceContentHandler;
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceContentHandler::NewLC
// Two-phased constructor that will parse CUpnpDevice object. If aResultDevice 
// is NULL the result object will be created during the parsing, and will be 
// owned by this CUpnpDeviceContentHandler.
// Otherwise result will be parsed to existing aResultDevice object.
// Leaves the object on the CleanupStack
// -----------------------------------------------------------------------------
//
CUpnpDeviceContentHandler* CUpnpDeviceContentHandler::NewLC(
    CUpnpContentHandlersController& aController, CUpnpDevice* aResultDevice )
    {
    CUpnpDeviceContentHandler* deviceContentHandler = 
        new (ELeave) CUpnpDeviceContentHandler( aController, aResultDevice );
    CleanupStack::PushL( deviceContentHandler );
    return deviceContentHandler;
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceContentHandler::NewL
// Two-phased constructor that will parse CUpnpDeviceImplementation object. If 
// aResultDeviceImpl is NULL the result object will be created during the parsing, 
// and will be owned by this CUpnpDeviceContentHandler.
// Otherwise result will be parsed to existing aResultDevice object.
// -----------------------------------------------------------------------------
//
CUpnpDeviceContentHandler* CUpnpDeviceContentHandler::NewL(
    CUpnpContentHandlersController& aController,
    CUpnpDeviceImplementation* aResultDeviceImpl )
    {
    CUpnpDeviceContentHandler* deviceContentHandler = 
        CUpnpDeviceContentHandler::NewLC( aController, aResultDeviceImpl );
    CleanupStack::Pop( deviceContentHandler );
    return deviceContentHandler;
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceContentHandler::NewLC
// Two-phased constructor that will parse CUpnpDeviceImplementation object. If 
// aResultDeviceImpl is NULL the result object will be created during the parsing, 
// and will be owned by this CUpnpDeviceContentHandler.
// Otherwise result will be parsed to existing aResultDevice object.
// Leaves the object on the CleanupStack
// -----------------------------------------------------------------------------
//
CUpnpDeviceContentHandler* CUpnpDeviceContentHandler::NewLC(
    CUpnpContentHandlersController& aController,
    CUpnpDeviceImplementation* aResultDeviceImpl )
    {
    CUpnpDeviceContentHandler* deviceContentHandler = 
        new (ELeave) CUpnpDeviceContentHandler( aController, aResultDeviceImpl );
    CleanupStack::PushL( deviceContentHandler );
    return deviceContentHandler;
    }


// -----------------------------------------------------------------------------
// CUpnpDeviceContentHandler::NewL
// Two-phased constructor that will parse CUpnpSilentDeviceImplementation object. If 
// aResultSilentDeviceImpl is NULL the result object will be created during the parsing, 
// and will be owned by this CUpnpDeviceContentHandler.
// Otherwise result will be parsed to existing aResultDevice object.
// -----------------------------------------------------------------------------
//
CUpnpDeviceContentHandler* CUpnpDeviceContentHandler::NewL(
    CUpnpContentHandlersController& aController,
    CUpnpSilentDeviceImplementation* aResultSilentDeviceImpl )
    {
    CUpnpDeviceContentHandler* deviceContentHandler = 
        CUpnpDeviceContentHandler::NewLC( aController, aResultSilentDeviceImpl );
    CleanupStack::Pop( deviceContentHandler );
    return deviceContentHandler;
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceContentHandler::NewLC
// Two-phased constructor that will parse CUpnpSilentDeviceImplementation object. If 
// aResultDeviceImpl is NULL the result object will be created during the parsing, 
// and will be owned by this CUpnpDeviceContentHandler.
// Otherwise result will be parsed to existing aResultDevice object.
// Leaves the object on the CleanupStack
// -----------------------------------------------------------------------------
//
CUpnpDeviceContentHandler* CUpnpDeviceContentHandler::NewLC(
    CUpnpContentHandlersController& aController,
    CUpnpSilentDeviceImplementation* aResultSilentDeviceImpl )
    {
    CUpnpDeviceContentHandler* deviceContentHandler = 
        new (ELeave) CUpnpDeviceContentHandler( aController, aResultSilentDeviceImpl );
    CleanupStack::PushL( deviceContentHandler );
    return deviceContentHandler;
    }
    
// -----------------------------------------------------------------------------
// CUpnpDeviceContentHandler::~CUpnpDeviceContentHandler
// Destructor of CUpnpDeviceContentHandler class 
// -----------------------------------------------------------------------------
//
CUpnpDeviceContentHandler::~CUpnpDeviceContentHandler()
    {
    if ( iIsDeviceOwned )
        {
        ASSERT( ( EDevice == iParseType && NULL == iResultDeviceImpl && NULL == iResultSilentDeviceImpl ) ||
                ( EDeviceImpl == iParseType && NULL == iResultDevice && NULL == iResultSilentDeviceImpl ) ||  
               ( ESilentDeviceImpl == iParseType && NULL == iResultDevice && NULL == iResultDeviceImpl ) );
        //check if object is in legal state (class invariants)
        delete iResultDevice;
        delete iResultDeviceImpl;
        delete iResultSilentDeviceImpl;
        }
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceContentHandler::CUpnpDeviceContentHandler
// Constructor that will parse CUpnpDevice object. If aResultDevice is NULL
// the result object will be created during the parsing, and will be owned by
// this CUpnpDeviceContentHandler.
// Otherwise result will be parsed to existing aResultDevice object
// -----------------------------------------------------------------------------
//
CUpnpDeviceContentHandler::CUpnpDeviceContentHandler(
    CUpnpContentHandlersController& aController, CUpnpDevice* aResultDevice ) :
    CUpnpContentHandler( aController ), iResultDevice( aResultDevice ),
            iIsDeviceOwned( NULL == aResultDevice), iParseType( EDevice )
    {
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceContentHandler::CUpnpDeviceContentHandler
// Constructor that will parse CUpnpDeviceImplementation object. 
// If aResultDeviceImpl is NULL the result object will be created during the 
// parsing, and will be owned by this CUpnpDeviceContentHandler.
// Otherwise result will be parsed to existing aResultDeviceImpl object
// -----------------------------------------------------------------------------
//
CUpnpDeviceContentHandler::CUpnpDeviceContentHandler(
    CUpnpContentHandlersController& aController,
    CUpnpDeviceImplementation* aResultDeviceImpl ) :
    CUpnpContentHandler( aController ), iResultDeviceImpl( aResultDeviceImpl ),
            iIsDeviceOwned( NULL == aResultDeviceImpl),
            iParseType( EDeviceImpl )
    {
    }
    
// -----------------------------------------------------------------------------
// CUpnpDeviceContentHandler::CUpnpDeviceContentHandler
// Constructor that will parse CUpnpSilentDeviceImplementation object. 
// If aResultSilentDeviceImpl is NULL the result object will be created during the 
// parsing, and will be owned by this CUpnpDeviceContentHandler.
// Otherwise result will be parsed to existing aResultDeviceImpl object
// -----------------------------------------------------------------------------
//
CUpnpDeviceContentHandler::CUpnpDeviceContentHandler(
    CUpnpContentHandlersController& aController,
    CUpnpSilentDeviceImplementation* aResultSilentDeviceImpl ) :
    CUpnpContentHandler( aController ), iResultSilentDeviceImpl( aResultSilentDeviceImpl ),
            iIsDeviceOwned( NULL == aResultSilentDeviceImpl ),
            iParseType( ESilentDeviceImpl )
    {
    }    

// -----------------------------------------------------------------------------
// CUpnpDeviceContentHandler::OnStartElementL
// This method is a callback to indicate an element has been parsed.
// -----------------------------------------------------------------------------
//
void CUpnpDeviceContentHandler::OnStartElementL( const RTagInfo& aElement,
    const RAttributeArray& /*aAttributes*/)
    {
    if ( aElement.LocalName().DesC().Compare( KUpnpDeviceRoot )==0 )
        {
        CUpnpRootContentHandler* rootCH = NULL;
        if ( iIsDeviceOwned )
            {
            switch ( iParseType )
                {
                case EDeviceImpl:
                    delete iResultDeviceImpl;
                    iResultDeviceImpl = NULL;
                    iResultDeviceImpl = new (ELeave) CUpnpDeviceImplementation();
                    break;
                case ESilentDeviceImpl:
                    delete iResultSilentDeviceImpl;
                    iResultSilentDeviceImpl = NULL;
                    iResultSilentDeviceImpl = new (ELeave) CUpnpSilentDeviceImplementation();
                    break;
                case EDevice:
                    delete iResultDevice;
                    iResultDevice = NULL;
                    iResultDevice = new (ELeave) CUpnpDevice();
                    break;
                default:
                    _LIT( KPanicString, "CUpnpDeviceContentHandler::OnStartElementL" );
                    User::Panic( KPanicString, 1 );  
                }
            }
        if ( iParseType == EDeviceImpl )
            {
            rootCH = CUpnpRootContentHandler::NewL( iController,
                *iResultDeviceImpl, EDeviceImpl );
            }
        else if ( iParseType == ESilentDeviceImpl )    
            {
            rootCH = CUpnpRootContentHandler::NewL( iController,
                *iResultSilentDeviceImpl, ESilentDeviceImpl );
            }
        else
            {
            rootCH = CUpnpRootContentHandler::NewL( iController,
                *iResultDevice, EDevice );
            }
        iController.SetCurrentContentHandlerL( rootCH );
        }
    else
        {
        User::Leave( KErrArgument ); //wrong root node
        }
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceContentHandler::OnEndElementL
// This method is a callback to indicate the end of the element has been reached.
// -----------------------------------------------------------------------------
//
void CUpnpDeviceContentHandler::OnEndElementL( const RTagInfo& /*aElement*/)
    {
    }

// -----------------------------------------------------------------------------
// CUpnpServiceContentHandler::OnContentL
// This method is a callback that sends the content of the element.
// -----------------------------------------------------------------------------
//
void CUpnpDeviceContentHandler::OnContentL( const TDesC8& /*aBytes*/)
    {
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceContentHandler::GetResultDevice
// Returns parsed CUpnpDevice object, and pass ownership to the caller.
// One shouldn't call this method if CUpnpDeviceContentHandler doesn't have
// ownership CUpnpDevice object.
// @see GetResultDevice( CUpnpDeviceImplementation*& aResultDeviceImpl )
// -----------------------------------------------------------------------------
//
void CUpnpDeviceContentHandler::GetResultDevice( CUpnpDevice*& aResultDevice )
    {
    ASSERT( iIsDeviceOwned && iParseType==EDevice );
    aResultDevice = iResultDevice;
    iIsDeviceOwned = EFalse;
    iResultDevice = NULL;
    }

// -----------------------------------------------------------------------------
// CUpnpDeviceContentHandler::GetResultDevice
// Returns parsed CUpnpDeviceImplementation object, and pass ownership to the caller.
// One shouldn't call this method if CUpnpDeviceContentHandler doesn't have
// ownership CUpnpDeviceImplementation object.
// @see GetResultDevice( CUpnpDevice*& aResultDeviceImpl )
// -----------------------------------------------------------------------------
//
void CUpnpDeviceContentHandler::GetResultDevice(
    CUpnpDeviceImplementation*& aResultDeviceImpl )
    {
    ASSERT( iIsDeviceOwned && iParseType==EDeviceImpl );
    aResultDeviceImpl = iResultDeviceImpl;
    iIsDeviceOwned = EFalse;
    iResultDeviceImpl = NULL;
    }
    
// -----------------------------------------------------------------------------
// CUpnpDeviceContentHandler::GetResultDevice
// Returns parsed CUpnpSilentDeviceImplementation object, and pass ownership to the caller.
// One shouldn't call this method if CUpnpDeviceContentHandler doesn't have
// ownership CUpnpSilentDeviceImplementation object.
// @see GetResultDevice( CUpnpDevice*& aResultSilentDeviceImpl )
// -----------------------------------------------------------------------------
//
void CUpnpDeviceContentHandler::GetResultDevice(
    CUpnpSilentDeviceImplementation*& aResultSilentDeviceImpl )
    {
    ASSERT( iIsDeviceOwned && iParseType==ESilentDeviceImpl );
    aResultSilentDeviceImpl = iResultSilentDeviceImpl;
    iIsDeviceOwned = EFalse;
    iResultSilentDeviceImpl = NULL;
    }

//  End of File
