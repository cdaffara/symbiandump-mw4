/** @file
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Definition of the CUpnpHttpTransaction class
*
*/


#include <upnphttpmessage.h>
#include <upnpcons.h>
#include <upnphttpmessagefactory.h>

#include "upnphttptransaction.h"

// ----------------------------------------------------------------------------
// CUpnpHttpTransaction::BaseConstructL
// Second phase constructor intended to use by child classes.
// ----------------------------------------------------------------------------
//
EXPORT_C void CUpnpHttpTransaction::BaseConstructL()
    {
    iHttpRequest->AddPairL( UpnpHTTP::KConnection, UpnpHTTP::KClose );
    }

// ----------------------------------------------------------------------------
// CUpnpHttpTransaction::CUpnpHttpTransaction
// Constructs object of transaction and takes ownership of request object
// ----------------------------------------------------------------------------
//
EXPORT_C CUpnpHttpTransaction::CUpnpHttpTransaction( CUpnpHttpMessage* aRequest ):
    iHttpRequest( aRequest )
    {
    ASSERT( NULL != aRequest );
    }

// ----------------------------------------------------------------------------
// CUpnpHttpTransaction::~CUpnpHttpTransaction
// Destructor.
// ----------------------------------------------------------------------------
//
EXPORT_C CUpnpHttpTransaction::~CUpnpHttpTransaction()
    {
    delete iHttpRequest;
    delete iHttpResponse;
    }

// ----------------------------------------------------------------------------
// CUpnpHttpTransaction::Request
// Returns request message of transaction
// ----------------------------------------------------------------------------
//
EXPORT_C CUpnpHttpMessage* CUpnpHttpTransaction::Request()
    {
    return iHttpRequest;
    }

// ----------------------------------------------------------------------------
// CUpnpHttpTransaction::Response
// Returns response message of transaction
// ----------------------------------------------------------------------------
//
EXPORT_C CUpnpHttpMessage* CUpnpHttpTransaction::Response()
    {
    return iHttpResponse;
    }

// ----------------------------------------------------------------------------
// CUpnpHttpTransaction::CreateOkResponseL
// Creates ok response for the request
// ----------------------------------------------------------------------------
//
void CUpnpHttpTransaction::CreateOkResponseL( const TDesC8& aResponseBody )
    {
    ASSERT( NULL == iHttpResponse );    //only one ok response is allowed
    iHttpResponse = RUpnpHttpMessageFactory::HttpResponseOkL( iHttpRequest );
    iHttpResponse->SetBodyL( aResponseBody );
    }

// ----------------------------------------------------------------------------
// CUpnpHttpTransaction::CreateFaultResponseL
// Creates timeout response for the request
// ----------------------------------------------------------------------------
//
void CUpnpHttpTransaction::CreateFaultResponseL( const TDesC8& aResponseBody, 
                                                 TInt aStatus, 
                                                 TInt aError )
    {
    delete iHttpResponse;
    iHttpResponse = NULL;
    iHttpResponse = RUpnpHttpMessageFactory::HttpResponseErrorL(
        iHttpRequest, aStatus );
    iHttpResponse->SetInternalError( aError );
    iHttpResponse->SetBodyL( aResponseBody );
    }


//end of file

