/** @file
 * Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
 * Description:  CUpnpHttpServerTransactionHandler implementation.
 *
 */
// INCLUDES
#include <bautils.h> 

#include "upnphttpservertransactionhandler.h"
#include "upnpstring.h"
#include "upnphttpmessage.h"
#include "upnpicon.h"

#include "upnphttpservertransactiondescription.h"
#include "upnpdevicedescriptionrequest.h"
#include "upnpdevicedescriptionprovider.h"
#include "upnpdeviceimplementationbase.h"




// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpHttpServerTransactionHandler::CUpnpHttpServerTransactionHandler
// C++ default constructor.
// -----------------------------------------------------------------------------
//
CUpnpHttpServerTransactionHandler::CUpnpHttpServerTransactionHandler(
        CUpnpDeviceImplementationBase& aDevice ) :
    iDevice( aDevice )

    {
    }

// -----------------------------------------------------------------------------
// CUpnpHttpServerTransactionHandler::~CUpnpHttpServerTransactionHandler
// C++ default destructor.
// -----------------------------------------------------------------------------
//
CUpnpHttpServerTransactionHandler::~CUpnpHttpServerTransactionHandler()
    {
    }

// -----------------------------------------------------------------------------
// CUpnpHttpServerTransactionHandler::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpHttpServerTransactionHandler* CUpnpHttpServerTransactionHandler::NewLC(
        CUpnpDeviceImplementationBase& aDevice )
    {
    CUpnpHttpServerTransactionHandler* self =
            new ( ELeave ) CUpnpHttpServerTransactionHandler( aDevice );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpServerTransactionHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpHttpServerTransactionHandler* CUpnpHttpServerTransactionHandler::NewL(
        CUpnpDeviceImplementationBase& aDevice )
    {
    CUpnpHttpServerTransactionHandler* self =
            CUpnpHttpServerTransactionHandler::NewLC( aDevice );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpServerTransactionHandler::ConstructL
// EPOC default constructor for performing 2nd stage construction.
// -----------------------------------------------------------------------------
//
void CUpnpHttpServerTransactionHandler::ConstructL()
    {   
    }


// -----------------------------------------------------------------------------
//  CUpnpHttpServerTransactionHandler::NewTransactionL
//
// -----------------------------------------------------------------------------
//
void CUpnpHttpServerTransactionHandler::NewTransactionL( 
    const TDesC8& aMethod, const TDesC8& aUri, 
    const TInetAddr& aSender, CUpnpHttpServerTransaction*& aResultTrans )
    {
    _LIT8( KGet, "GET" );
    _LIT8( KHead, "HEAD" );
    if ( (aMethod.CompareF(KGet)==0) || (aMethod.CompareF(KHead)==0) )
        {
        aResultTrans = CUpnpHttpServerTransactionDescription::NewL( *this, aSender, aUri );
        }
    else
        {
        User::Leave( KErrGeneral );//bad request
        }
    }


// -----------------------------------------------------------------------------
//  CUpnpHttpServerTransactionHandler::GetFileL
//
// -----------------------------------------------------------------------------
//
TInt CUpnpHttpServerTransactionHandler::GetFileL( RFile& aFile,
        const TDesC8& aUri, const TInetAddr& aInetAdress )
    {
    
    CUpnpDeviceDescriptionRequest* request = CUpnpDeviceDescriptionRequest::NewL( aUri, aInetAdress );    
    TInt error = KErrNone;
    
    switch ( MatchType( aUri ) )
        {
        case EDevice:
            error = iDevice.DescritptionProvider().OnDeviceDescription( *request );
            break;        
        case EService:
            error = iDevice.DescritptionProvider().OnServiceDescription( *request );
            break;        
        case EIcon:
            error = iDevice.DescritptionProvider().OnIcon( *request );
            break;        
        default:
            error = -EHttpNotFound;            
            break;
        }
    delete request;
    request = NULL;
    
    if( KErrNone == error )
        {
        if ( KErrNone != iDevice.DescritptionProvider().GetFile( aUri, aFile ) )
            {
            error = -EHttpNotFound;
            }
        }
    else
        {
        error = -EHttpNotFound;
        }
    
    return error;
    }

// -----------------------------------------------------------------------------
//  CUpnpHttpServerTransactionHandler::MatchType
//
// -----------------------------------------------------------------------------
//
CUpnpHttpServerTransactionHandler::TDescriptionType 
              CUpnpHttpServerTransactionHandler::MatchType( const TDesC8& aUri )
    {
    if ( aUri.CompareF( iDevice.DescriptionUrl() ) == 0 )
        {
        return EDevice;
        }

    //1st check if url is service url
    RPointerArray<CUpnpService> servList =
                 iDevice.ServiceList();
    TInt count = servList.Count();
    for( TInt index = 0; index < count; index++ )
        {
        if( servList[ index ]->ServiceDescriptionUrl().CompareF( aUri ) == 0 )
            {
            return EService;
            }
        }
    //next check icon list
    RPointerArray<CUpnpIcon> iconList =
                 iDevice.Icons();
    count = iconList.Count();
    for( TInt index2 = 0; index2 < count; index2++ )
        {
        if( iconList[ index2 ]->Url().CompareF( aUri ) == 0 )
            {
            return EIcon;
            }
        }
    return ENotFound;
    }


//  End of File
