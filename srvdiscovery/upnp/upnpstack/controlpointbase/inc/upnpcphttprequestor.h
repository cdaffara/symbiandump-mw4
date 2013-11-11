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
* Description:  Declares MUpnpCpHttpRequestor class.
*
*/

#ifndef MUPNPCPHTTPREQUESTOR_H_
#define MUPNPCPHTTPREQUESTOR_H_

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class MUpnpHttpClientObserver;
class CUpnpHttpTransaction;

/**
* Interface that allows control point to communicate by http in abstract manner
*/
class MUpnpCpHttpRequestor
    {
public:
    /**
     * Method is called by cp to initialise object, and subscribe for responses
     */
    virtual void InitialiseL( MUpnpHttpClientObserver& aObserver, TInt aActiveIap ) = 0;

    /**
     * Initialise sending http transaction
     */
    virtual void SendL( CUpnpHttpTransaction& aTransaction ) = 0;

    };

#endif /*MUPNPCPHTTPREQUESTOR_H_*/
