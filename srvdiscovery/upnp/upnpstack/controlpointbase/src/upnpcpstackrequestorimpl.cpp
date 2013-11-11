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
#include "upnpcpstackrequestorimpl.h"
#include "upnpdispatcher.h"
#include "upnpcontrolpoint.h"
#include "upnpdeviceimplementationbase.h"

// -----------------------------------------------------------------------------
// CUpnpCpStackRequestorImpl::CUpnpCpStackRequestorImpl
// -----------------------------------------------------------------------------
//
CUpnpCpStackRequestorImpl::CUpnpCpStackRequestorImpl()
    {
    }

// -----------------------------------------------------------------------------
// CUpnpCpStackRequestorImpl::~CUpnpCpStackRequestorImpl
// -----------------------------------------------------------------------------
//
CUpnpCpStackRequestorImpl::~CUpnpCpStackRequestorImpl()
    {
    delete iDispatcher;
    }

// -----------------------------------------------------------------------------
// CUpnpCpStackRequestorImpl::NewL
// -----------------------------------------------------------------------------
//
CUpnpCpStackRequestorImpl* CUpnpCpStackRequestorImpl::NewL(
    MUpnpDiscoveryObserver& aDiscoveryObserver )
    {
    CUpnpCpStackRequestorImpl* self = new (ELeave) CUpnpCpStackRequestorImpl();
    CleanupStack::PushL( self );
    self->ConstructL( aDiscoveryObserver );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpCpStackRequestorImpl::SearchL
// -----------------------------------------------------------------------------
//
void  CUpnpCpStackRequestorImpl::SearchL( const TDesC8& aSearchString )
    {
    iDispatcher->SearchL( aSearchString );
    }

// -----------------------------------------------------------------------------
// CUpnpCpStackRequestorImpl::SearchL
// -----------------------------------------------------------------------------
//
void  CUpnpCpStackRequestorImpl::SearchL( const TDesC16& aSearchString )
    {
    iDispatcher->SearchL( aSearchString );
    }

// -----------------------------------------------------------------------------
// CUpnpCpStackRequestorImpl::AddCustomerL
// -----------------------------------------------------------------------------
//
void  CUpnpCpStackRequestorImpl::AddCustomerL( const MUpnpDispatcherCustomer& aCustomer )
    {
    iDispatcher->AddCustomerL( aCustomer, ETrue );
    }

// -----------------------------------------------------------------------------
// CUpnpCpStackRequestorImpl::RemoveCustomer
// -----------------------------------------------------------------------------
//
void  CUpnpCpStackRequestorImpl::RemoveCustomer( const MUpnpDispatcherCustomer& aCustomer )
    {
    iDispatcher->RemoveCustomer( aCustomer, ETrue );
    }

// -----------------------------------------------------------------------------
// CUpnpCpStackRequestorImpl::HttpServerAddress
// -----------------------------------------------------------------------------
//
TInetAddr CUpnpCpStackRequestorImpl::HttpServerAddress()
    {
    return iDispatcher->HttpServerAddress();
    }
    
// -----------------------------------------------------------------------------
// CUpnpCpStackRequestorImpl::ConstructL
// -----------------------------------------------------------------------------
//
void  CUpnpCpStackRequestorImpl::ConstructL( MUpnpDiscoveryObserver& aDiscoveryObserver )
    {
    iDispatcher = CUpnpDispatcher::NewL( &aDiscoveryObserver );
    }

// -----------------------------------------------------------------------------
// CUpnpCpStackRequestorImpl::SendLocalRequestL
// -----------------------------------------------------------------------------
//
void CUpnpCpStackRequestorImpl::SendLocalRequestL( CUpnpHttpMessage* aMessage,
                             MUpnpDispatcherCustomer &aCustomer )
    {
    iDispatcher->SendMessageL( aMessage, aCustomer, ETrue );
    }

// -----------------------------------------------------------------------------
// CUpnpCpStackRequestorImpl::SendMessageL
// -----------------------------------------------------------------------------
//
void CUpnpCpStackRequestorImpl::SendMessageL( CUpnpHttpMessage* aMessage,
                                MUpnpDispatcherCustomer &aCustomer,
                                TBool aIsLocal)
    {
    iDispatcher->SendMessageL( aMessage, aCustomer, aIsLocal );
    }

// -----------------------------------------------------------------------------
// CUpnpCpStackRequestorImpl::StopFilteringL
// -----------------------------------------------------------------------------
//
void  CUpnpCpStackRequestorImpl::StopIgnoringL( const TDesC8& aUuids )
    {
    iDispatcher->StopFilteringL( aUuids );
    }

// -----------------------------------------------------------------------------
// CUpnpCpStackRequestorImpl::StartIPFilteringL
// -----------------------------------------------------------------------------
//
void CUpnpCpStackRequestorImpl::StartIPFilteringL()
    {
    iDispatcher->StartIPFilteringL();
    }

// -----------------------------------------------------------------------------
// CUpnpCpHttpClientRequestorImpl::StopIPFiltering
// -----------------------------------------------------------------------------
//
void CUpnpCpStackRequestorImpl::StopIPFiltering()
    {
    iDispatcher->StopIPFiltering();
    }

// -----------------------------------------------------------------------------
// CUpnpCpHttpClientRequestorImpl::StopHttpServer
// -----------------------------------------------------------------------------
//    
void CUpnpCpStackRequestorImpl::StopHttpServer()
    {
    iDispatcher->StopHttpServer();
    }
    
// -----------------------------------------------------------------------------
// CUpnpCpHttpClientRequestorImpl::StartHttpServerL
// -----------------------------------------------------------------------------
//    
void CUpnpCpStackRequestorImpl::StartHttpServerL()
    {
    iDispatcher->StartHttpServerL( ETrue, KRandomPort );
    }    

//  End of File 
