/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declares HTTP observer class
*
*/


#ifndef C_MUPNPHTTPSERVEROBSERVER_H
#define C_MUPNPHTTPSERVEROBSERVER_H

// FORWARD DECLARATIONS
class CUpnpHttpMessage;

// CLASS DESCRIPTION
/**
 * MUpnpHttpServerObserver is an interface class notifying
 * about the HTTP events, errors.
 * Interface should be used only for achieving file transfer completion status.
 * Other usages are internal to upnp stack.
 *
 * @lib dlnawebserver.lib
 * @since S60 5.1
 */
class MUpnpHttpServerObserver
    {
public: 

    /**
     * This is virtual callback function which is used for forwarding
     * messages to upper layer - to observer.
     * 
     * @since S60 5.1
     * @param aMessage the HTTP message
     */
    virtual void HttpEventLD( CUpnpHttpMessage* aMessage ) = 0;
    };

#endif // C_MUPNPHTTPSERVEROBSERVER_H

// End of File
