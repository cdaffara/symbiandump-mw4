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
* Description:  Provides default implementation of MUpnpCpHttpClientRequestor interface.
*
*/


// INCLUDE FILES
#include "upnpcphttprequestorimpl.h"
#include "upnphttpclientengine.h"

// -----------------------------------------------------------------------------
// CUpnpCpHttpRequestorImpl::CUpnpCpHttpRequestorImpl
// -----------------------------------------------------------------------------
//
CUpnpCpHttpRequestorImpl::CUpnpCpHttpRequestorImpl()
    {
    }

// -----------------------------------------------------------------------------
// CUpnpCpHttpRequestorImpl::~CUpnpCpHttpRequestorImpl
// -----------------------------------------------------------------------------
//
CUpnpCpHttpRequestorImpl::~CUpnpCpHttpRequestorImpl()
    {
    delete iHttpClientEngine;
    }

// -----------------------------------------------------------------------------
// CUpnpCpHttpRequestorImpl::NewL
// -----------------------------------------------------------------------------
//
CUpnpCpHttpRequestorImpl* CUpnpCpHttpRequestorImpl::NewL()
    {
    CUpnpCpHttpRequestorImpl* self = new (ELeave) CUpnpCpHttpRequestorImpl();
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpCpHttpRequestorImpl::InitialiseL
// -----------------------------------------------------------------------------
//
void  CUpnpCpHttpRequestorImpl::InitialiseL(
    MUpnpHttpClientObserver& aObserver, TInt aActiveIap )
    {
    delete iHttpClientEngine;
    iHttpClientEngine = NULL;
    iHttpClientEngine = CUpnpHttpClientEngine::NewL( aObserver, aActiveIap );
    }

// -----------------------------------------------------------------------------
// CUpnpCpHttpRequestorImpl::SendL
// -----------------------------------------------------------------------------
//
void CUpnpCpHttpRequestorImpl::SendL( CUpnpHttpTransaction& aTransaction )
    {
    ASSERT( iHttpClientEngine );//check if InitialiseL was invoked
    iHttpClientEngine->SendL( aTransaction );
    }

//  End of File
