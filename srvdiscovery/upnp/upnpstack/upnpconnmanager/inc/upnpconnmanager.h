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
* Description:  CUpnpConnectionManager
*
*/

#ifndef C_CUPNPCONNMANAGER_H
#define C_CUPNPCONNMANAGER_H

#include <e32base.h>
#include "upnpconnmanagercommon.h"
#include <upnpsymbianserverbase.h>

class CUpnpConnManagerEngine;

/**
 * Connection Manager Server
 * @since S60 5.0
 */
class CUpnpConnectionManager : public CUpnpSymbianServerBase
    {
public : // New methods
    /**
      * Create a CUpnpConnectionManager object using two phase construction,
      * and return a pointer to the created object, leaving a pointer to the
      * object on the cleanup stack
      * @result pointer to created CUpnpConnectionManager object
      */
    static CUpnpSymbianServerBase* NewLC();

    /**
     * Destroy the object and release all memory objects
     */
    ~CUpnpConnectionManager();

private:    // from CUpnpSymbianServerBase

    const TDesC& ServerName() const;
   
private: // New methods

    /**
     * Perform the first phase of two phase construction
     */
    CUpnpConnectionManager();

    /**
     * Perform the second phase construction of a CUpnpConnectionManager object
     */
    void ConstructL();

private: // From CUpnpSymbianServer

    /**
     * Create a time server session, and return a pointer to the created object
     * @result pointer to new session
     */
    CSession2* NewSessionL( const RMessage2& aMessage ) const;

    /**
     * Returns version that is supported by this server.
     */
    virtual TVersion SupportedVersion() const;
    
    /**
     * Process any errors
     * @param aError the leave code reported.
     * @result return KErrNone if leave is handled
     */
    TInt RunError( TInt aError );

private:

    /* @var iEngine Engine of the server */
    CUpnpConnManagerEngine* iEngine;

    };


#endif // C_CUPNPCONNMANAGER_H
