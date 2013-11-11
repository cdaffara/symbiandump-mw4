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

#ifndef CUPNPCPHTTPREQUESTORIMPLBASE_H_
#define CUPNPCPHTTPREQUESTORIMPLBASE_H_

#include "upnpcphttprequestor.h"
#include <e32base.h>


/**
 * Base implementation. Class used in WLan Lost case.
 */
NONSHARABLE_CLASS( CUpnpCpHttpRequestorImplBase ) : public CBase,
                                                    public MUpnpCpHttpRequestor
    {
private: //from MUpnpCpHttpRequestor
    virtual void SendL( CUpnpHttpTransaction& aTransaction );

    virtual void InitialiseL( MUpnpHttpClientObserver& aObserver, TInt aActiveIap );

    };

#endif //CUPNPCPHTTPREQUESTORIMPLBASE_H_
