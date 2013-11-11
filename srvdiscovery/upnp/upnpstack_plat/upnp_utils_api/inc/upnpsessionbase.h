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
* Description:  Declaration of RUpnpSessionBase
*
*/

#ifndef RUPNPSESSIONBASE_H
#define RUPNPSESSIONBASE_H

#include <e32base.h>

/**
 * Base class for all client side session classes used in upnp stack project
 * It provides you connecting and starting server
 *
 * Note that it should be used with server part implementation that use
 * CUpnpSymbianServerBase class.
 *
 *  @lib upnpipserversutils
 */
class RUpnpSessionBase : public RSessionBase
    {
protected:

    /**
     * Connect to the server and create a session
     *
     * @param aServerName               name of server to connect to
     * @param aServerVersion            version of server that is supported
     * @param aServerMessageSlots       number of message slots
     * @param aServerStartRetryCount    number of possible start retries to be made
     * @param aServerFileName           name of server exe file to run
     * @param aServerUid3               uid3 of server exe to run
     * @return error code
     */
    IMPORT_C TInt Connect( const TDesC& aServerName, TVersion aServerVersion,
        TInt aServerMessageSlots, TInt aServerStartRetryCount,
        const TDesC& aServerFileName, TUid aServerUid3 );

    };

#endif // RUPNPSESSIONBASE_H
