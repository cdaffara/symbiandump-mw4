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
* Description:  Declares virtual MUpnpMessageObserver callback class
*                and the implemented active object class CUpnpNotifyTimer
*
*/


#ifndef C_MUPNPMESSAGEOBSERVER_H
#define C_MUPNPMESSAGEOBSERVER_H

// INCLUDES
#include <e32base.h>

// CONSTANTS

// CLASS DECLARATION
class CUpnpHttpMessage;

/**
* brief An interface from SsdpMessage to its customer (service)
*
*  @since Series60 2.0
*/
class MUpnpMessageObserver
    {
    public:

        /**
        * MessageExpiredL
        * @since Series60 2.0
        * @param aMessage A pointer to expired message.
        * @return None
        */
	    virtual void MessageExpiredL( CUpnpHttpMessage* aMessage ) = 0;
    };

#endif // C_MUPNPMESSAGEOBSERVER_H

// End Of File