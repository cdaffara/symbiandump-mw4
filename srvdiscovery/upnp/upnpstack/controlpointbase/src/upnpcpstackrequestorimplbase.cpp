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
* Description:  Class used in WLan Lost case
*
*/


// INCLUDE FILES
#include "upnpcpstackrequestorimplbase.h"


// -----------------------------------------------------------------------------
// CUpnpCpStackRequestorImplBase::SearchL
// -----------------------------------------------------------------------------
//
void CUpnpCpStackRequestorImplBase::SearchL( const TDesC8& /*aSearchString*/ )
    {
    User::Leave( KErrNotReady );
    }

// -----------------------------------------------------------------------------
// CUpnpCpStackRequestorImplBase::SearchL
// -----------------------------------------------------------------------------
//
void CUpnpCpStackRequestorImplBase::SearchL( const TDesC16& /*aSearchString*/ )
    {
    User::Leave( KErrNotReady );
    }

// -----------------------------------------------------------------------------
// CUpnpCpStackRequestorImplBase::AddCustomerL
// -----------------------------------------------------------------------------
//
void CUpnpCpStackRequestorImplBase::AddCustomerL( 
                                  const MUpnpDispatcherCustomer& /*aCustomer*/ )
    {
    User::Leave( KErrNotReady );
    }

// -----------------------------------------------------------------------------
// CUpnpCpStackRequestorImplBase::RemoveCustomer
// -----------------------------------------------------------------------------
//
void CUpnpCpStackRequestorImplBase::RemoveCustomer( 
                                  const MUpnpDispatcherCustomer& /*aCustomer*/ )
    {
    }

// -----------------------------------------------------------------------------
// CUpnpCpStackRequestorImplBase::HttpServerAddress
// -----------------------------------------------------------------------------
//
TInetAddr CUpnpCpStackRequestorImplBase::HttpServerAddress()
    {
    return TInetAddr();
    }
    
// -----------------------------------------------------------------------------
// CUpnpCpStackRequestorImplBase::SendLocalRequestL
// -----------------------------------------------------------------------------
//
void CUpnpCpStackRequestorImplBase::SendLocalRequestL( 
        CUpnpHttpMessage* /*aMessage*/, MUpnpDispatcherCustomer& /*aCustomer*/ )
    {
    User::Leave( KErrNotReady );
    }

// -----------------------------------------------------------------------------
// CUpnpCpStackRequestorImplBase::SendMessageL
// -----------------------------------------------------------------------------
//
void CUpnpCpStackRequestorImplBase::SendMessageL( CUpnpHttpMessage* /*aMessage*/,
                                          MUpnpDispatcherCustomer& /*aCustomer*/,
                                          TBool /*aIsLocal*/ )
    {
    User::Leave( KErrNotReady );
    }

// -----------------------------------------------------------------------------
// CUpnpCpStackRequestorImplBase::StopFilteringL
// -----------------------------------------------------------------------------
//
void  CUpnpCpStackRequestorImplBase::StopIgnoringL( const TDesC8& /*aUuids*/ )
    {
    User::Leave( KErrNotReady );
    }

// -----------------------------------------------------------------------------
// CUpnpCpStackRequestorImplBase::StartIPFilteringL
// -----------------------------------------------------------------------------
//
void CUpnpCpStackRequestorImplBase::StartIPFilteringL()
    {
    User::Leave( KErrNotReady );
    }

// -----------------------------------------------------------------------------
// CUpnpCpHttpClientRequestorImplBase::StopIPFiltering
// -----------------------------------------------------------------------------
//
void CUpnpCpStackRequestorImplBase::StopIPFiltering()
    {
    }

// -----------------------------------------------------------------------------
// CUpnpCpHttpClientRequestorImplBase::StopHttpServer
// -----------------------------------------------------------------------------
//    
void CUpnpCpStackRequestorImplBase::StopHttpServer()
    {
    }
    
// -----------------------------------------------------------------------------
// CUpnpCpHttpClientRequestorImplBase::StartHttpServer
// -----------------------------------------------------------------------------
//    
void CUpnpCpStackRequestorImplBase::StartHttpServerL()
    {
    }    

//  End of File
