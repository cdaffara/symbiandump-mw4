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
* Description:  Provides default implementation of MUpnpCpHttpRequestor interface.
*
*/

#ifndef CUPNPCPHTTPREQUESTORIMPL_H_
#define CUPNPCPHTTPREQUESTORIMPL_H_

#include <e32base.h>
#include "upnpcphttprequestorimplbase.h"

class CUpnpHttpClientEngine;
class MUpnpHttpClientObserver;

/**
* Default implementation of MUpnpCpHttpRequestor. It uses symbian http engine.
*/
NONSHARABLE_CLASS( CUpnpCpHttpRequestorImpl ) : public CUpnpCpHttpRequestorImplBase
    {
public:
    /**
    * Two phased constructor
    * @return an instance of CUpnpCpHttpRequestorImpl class
    */
    static CUpnpCpHttpRequestorImpl* NewL();

    /**
    * Destructor
    */
    virtual ~CUpnpCpHttpRequestorImpl();

private:
    /**
    * C++ constructor
    */
    CUpnpCpHttpRequestorImpl();

private: //from MUpnpCpHttpRequestor
    virtual void SendL( CUpnpHttpTransaction& aTransaction );

    virtual void InitialiseL( MUpnpHttpClientObserver& aObserver, TInt aActiveIap );

private:
    CUpnpHttpClientEngine* iHttpClientEngine;
    };

#endif //CUPNPCPHTTPREQUESTORIMPL_H_
