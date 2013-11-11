/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  .
*
*/


#ifndef CONNECTION_OBSERVER_H
#define CONNECTION_OBSERVER_H

#warning The Feeds Engine API will be removed wk25. Please see http://wikis.in.nokia.com/Browser/APIMigration for more information

// INCLUDES
#include <e32base.h>
#include <e32std.h>

#include <ApEngineconsts.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  The Connection observer interface.
*
*  \b Library: FeedsEngine.lib
*
*  @since 3.1
*/
class MConnectionObserver
	{
    public:  // New Methods

        /**
        * Request to create a network connection.
        *
        * @since 3.1
        * @param aConnectionPtr A pointer to the new connection. If NULL, the 
        *                       proxy filter will automatically create a network connection.
        * @param aSockSvrHandle A handle to the socket server.
        * @param aNewConn A flag if a new connection was created. If the 
        *                 connection is not new, proxy filter optimization will not 
        *                 read the proxy again from CommsBd.
        * @param aBearerType The bearer type of the new connection
        * @return void
        */
        virtual void NetworkConnectionNeededL(TInt* aConnectionPtr, TInt& aSockSvrHandle,
                TBool& aNewConn, TApBearerType& aBearerType) = 0;
	};


#endif  // CONNECTION_OBSERVER_H
// End of File
