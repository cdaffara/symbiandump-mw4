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

#ifndef UPNPCPSTACKREQUESTORIMPLBASE_H_
#define UPNPCPSTACKREQUESTORIMPLBASE_H_

#include "upnpcpstackrequestor.h"
#include <e32base.h>



/**
 * Base implementation. Class used in WLan Lost case.
 */
NONSHARABLE_CLASS( CUpnpCpStackRequestorImplBase ) : public CBase, 
                                                     public MUpnpCpStackRequestor
    {
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
    virtual void StopHttpServer();
    virtual void StartHttpServerL();
    

    };

#endif //UPNPCPSTACKREQUESTORIMPLBASE_H_
