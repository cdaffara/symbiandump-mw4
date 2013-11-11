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

#ifndef UPNPCPSTACKREQUESTORIMPL_H_
#define UPNPCPSTACKREQUESTORIMPL_H_

#include <e32base.h>
#include "upnpcpstackrequestorimplbase.h"

class CUpnpDispatcher;
class CUpnpControlPoint;
class MUpnpDiscoveryObserver;

/**
* @brief Class provides default implementation
* of MUpnpCpStackRequestor interface
* It simply delegates requests to the CUpnpDispatcher.
*
* @since Series60 3.2
*/
NONSHARABLE_CLASS( CUpnpCpStackRequestorImpl ) : public CUpnpCpStackRequestorImplBase
    {
public:
    /**
    * Two phased constructor
    * @since Series60 3.2
    * @return an instance of CUpnpCpStackRequestorImpl class
    */
    static CUpnpCpStackRequestorImpl* NewL( MUpnpDiscoveryObserver& aDiscoveryObserver );

    /**
    * Destructor
    * @since Series60 3.2
    */
    virtual ~CUpnpCpStackRequestorImpl();

public: //from MUpnpCpStackRequestor

    virtual void SearchL( const TDesC8& aSearchString );
    virtual void SearchL( const TDesC16& aSearchString );
    virtual void AddCustomerL( const MUpnpDispatcherCustomer& aCustomer );
    virtual void RemoveCustomer( const MUpnpDispatcherCustomer& aCustomer );
    virtual TInetAddr HttpServerAddress();
    
    
    virtual void SendLocalRequestL( CUpnpHttpMessage* aMessage,
                                    MUpnpDispatcherCustomer& aCustomer );

    virtual void SendMessageL( CUpnpHttpMessage* aMessage,
                               MUpnpDispatcherCustomer& aCustomer,
                               TBool aIsLocal );

    virtual void StopIgnoringL( const TDesC8& aUuids );

    virtual void StartIPFilteringL();
    virtual void StopIPFiltering();
    
    /**
    * Stops http server    
    */
    virtual void StopHttpServer();
    
    /**
    * Starts http server    
    */
    virtual void StartHttpServerL();
    
private:

    /**
    * C++ constructor
    */
    CUpnpCpStackRequestorImpl();

    /**
    * 2nd phase constructor
    */
    void ConstructL( MUpnpDiscoveryObserver& aDiscoveryObserver );

private:

    // pointer to dispatcher
    CUpnpDispatcher* iDispatcher;
    };

#endif //UPNPCPSTACKREQUESTORIMPL_H_
