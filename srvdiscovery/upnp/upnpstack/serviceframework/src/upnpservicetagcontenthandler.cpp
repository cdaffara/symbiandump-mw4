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
* Description:  Implements the CUpnpServiceTagContentHandler class
 *
*/


#include "upnpservicetagcontenthandler.h"
#include "upnpcontenthandlerscontroller.h"
#include "upnpservice.h"
#include "upnpdevice.h"
#include "upnpdeviceliterals.h"

//const TUint8 KReqiuredTagsBoundary(31) //first five bits
enum TFlagsPositions
    {
    EServiceTypePos = 0,
    EServiceIdPos,
    ESCPDURLPos,
    EControlURLPos,
    EEventSubURLPos //all required
    };

// -----------------------------------------------------------------------------
// CUpnpServiceTagContentHandler::NewL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
CUpnpServiceTagContentHandler* CUpnpServiceTagContentHandler::NewL(
    CUpnpContentHandlersController& aController, CUpnpService& aResultService )
    {
    CUpnpServiceTagContentHandler* serviceTagContentHandler = 
        CUpnpServiceTagContentHandler::NewLC( aController, aResultService );
    CleanupStack::Pop( serviceTagContentHandler );
    return serviceTagContentHandler;
    }

// -----------------------------------------------------------------------------
// CUpnpServiceTagContentHandler::NewLC
// Two-phased constructor. Leaves teh object on the CleanupStack
// -----------------------------------------------------------------------------
//
CUpnpServiceTagContentHandler* CUpnpServiceTagContentHandler::NewLC(
    CUpnpContentHandlersController& aController, CUpnpService& aResultService )
    {
    CUpnpServiceTagContentHandler* serviceTagContentHandler = 
        new (ELeave) CUpnpServiceTagContentHandler( aController, aResultService );
    CleanupStack::PushL( serviceTagContentHandler );
    return serviceTagContentHandler;
    }

// -----------------------------------------------------------------------------
// CUpnpServiceTagContentHandler::~CUpnpServiceTagContentHandler
// Destructor of CUpnpServiceTagContentHandler class 
// -----------------------------------------------------------------------------
//
CUpnpServiceTagContentHandler::~CUpnpServiceTagContentHandler()
    {
    }

// -----------------------------------------------------------------------------
// CUpnpServiceTagContentHandler::CUpnpServiceTagContentHandler
// Constructor
// -----------------------------------------------------------------------------
//
CUpnpServiceTagContentHandler::CUpnpServiceTagContentHandler(
    CUpnpContentHandlersController& aController, CUpnpService& aResultService ) :
    CUpnpContentHandler( aController ), iResultService( aResultService ),
            iCurrentState( EInitial )
    {
    }

// -----------------------------------------------------------------------------
// CUpnpServiceTagContentHandler::ProcessInternalTagL
// Do necessary operation when specyfic tag inside service tag have been found.
// -----------------------------------------------------------------------------
//
void CUpnpServiceTagContentHandler::ProcessInternalTagL( TInt aFlagPosition,
    TState aStateToSet )
    {
    if ( IsTagRepeated( aFlagPosition, iFoundTags ) )
        {
        SetIgnoreHandlerL();
        }
    else
        {
        iCurrentState = aStateToSet;
        }
    }

// -----------------------------------------------------------------------------
// CUpnpServiceTagContentHandler::OnStartElementL
// This method is a callback to indicate an element has been parsed.
// -----------------------------------------------------------------------------
//
void CUpnpServiceTagContentHandler::OnStartElementL(
    const RTagInfo& aElement, const RAttributeArray& /*aAttributes*/)
    {
    if ( EInitial == iCurrentState )
        {
        const TDesC8& elementName(aElement.LocalName().DesC() );
        if ( elementName.Compare( KUpnpServiceType ) == 0 )
            {
            ProcessInternalTagL( EServiceTypePos, EServiceType );
            }
        else if ( elementName.Compare( KUpnpServiceId ) == 0 )
            {
            ProcessInternalTagL( EServiceIdPos, EServiceId );
            }
        else if ( elementName.Compare( KUpnpSCPDURL ) == 0 )
            {
            ProcessInternalTagL( ESCPDURLPos, ESCPDURL );
            }
        else if ( elementName.Compare( KUpnpControlURL ) == 0 )
            {
            ProcessInternalTagL( EControlURLPos, EControlURL );
            }
        else if ( elementName.Compare( KUpnpEventSubURL ) == 0 )
            {
            ProcessInternalTagL( EEventSubURLPos, EEventSubURL );
            }
        else
            {
            SetIgnoreHandlerL();
            }
        }
    else
        {
        //User::Leave( KErrArgument )    //textOnly
        SetIgnoreHandlerL();
        }
    }

// -----------------------------------------------------------------------------
// CUpnpServiceTagContentHandler::OnEndElementL
// This method is a callback to indicate the end of the element has been reached.
// -----------------------------------------------------------------------------
//
void CUpnpServiceTagContentHandler::OnEndElementL( const RTagInfo& aElement )
    {
    switch ( iCurrentState )
        {
        case EInitial:
            ASSERT( aElement.LocalName().DesC().Compare( KUpnpService ) == 0 );
            //if ( (iFoundTags.iFlags & KReqiuredTagsBoundary) == KReqiuredTagsBoundary )
            //{     //nothing required for now
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
// CUpnpServiceTagContentHandler::OnContentL
// This method is a callback that sends the content of the element.
// -----------------------------------------------------------------------------
//
void CUpnpServiceTagContentHandler::OnContentL( const TDesC8& aBytes )
    {
    switch ( iCurrentState )
        {
        case EServiceType:
            ProcessServiceTypeContentL( aBytes );
            break;
        case EServiceId:
            iResultService.Device().ServiceIdL().AppendL( aBytes );
            break;
        case ESCPDURL:
            iResultService.SetServiceDescriptionUrl( aBytes.AllocL() );
            break;
        case EControlURL:
            iResultService.SetControlUrl( aBytes.AllocL() );
            break;
        case EEventSubURL:
            iResultService.SetSubscriptionUrl( aBytes.AllocL() );
            break;
        default:
            ASSERT( EInitial == iCurrentState );
            //User::Leave(KErrArgument) //eltOnly
            break;
        }
    }

// -----------------------------------------------------------------------------
// CUpnpServiceTagContentHandler::ProcessServiceTypeContentL
// Process content of serviceType tag.
// -----------------------------------------------------------------------------
//
void CUpnpServiceTagContentHandler::ProcessServiceTypeContentL(
    const TDesC8& aBytes )
    {
    

    //iResultService.SetServiceTypeL( aBytes )
    //iResultService.SetPathL( aBytes )

    TInt index = aBytes.Find( UpnpSSDP::KUPnPServiceSchema() );
    TPtrC8 serviceType( KNullDesC8() );

    // if service type conforms UPnP service schema
    if ( index != KErrNotFound )
        {
        serviceType.Set( aBytes.Mid( index + UpnpSSDP::KUPnPServiceSchema().Length() ) );
        }
    else
        {
        serviceType.Set( aBytes );
        }

    TInt typeEnd = serviceType.Find( UpnpString::KColon() );
    if ( typeEnd > KErrNotFound )
        {
        HBufC8* temp = serviceType.AllocLC();
        TPtr8 ptr = temp->Des();
        ptr.Delete( typeEnd, 1 );
        iResultService.SetPathL( ptr );
        CleanupStack::PopAndDestroy( temp );
        }
    else
        {
        iResultService.SetPathL( serviceType );
        }

    iResultService.SetServiceTypeL( serviceType );
    iResultService.Device().ServiceTypesL().AppendL( serviceType );
    }

//  End of File
