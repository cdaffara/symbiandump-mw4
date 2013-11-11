/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  A factory class used to create UrlHandlers.
*
*/


#ifndef URL_HANDLER_FACTORY_H
#define URL_HANDLER_FACTORY_H


// INCLUDES
#include <e32base.h>

#include "LoadObserver.h"
#include "UrlHandler.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION
class CHttpConnection;


/**
*  A factory class used to create UrlHandlers.
*
*  \b Library: FeedsEngine.lib
*
*  @since 3.0
*/
class UrlHandlerFactory
    {
    public:
        /**
        * Given the schema of the url it creates an url handler instance
        * capable of fetching the url.
        *
        * @since 3.0
        * @param aHttpConnection A shared http connection (not used for non
        *        http handlers).
        * @param aUrl The url to load
        * @param aObserver The load observer.
        * @return A UrlHandler.
        */
        static CUrlHandler* NewUrlHandlerL(CHttpConnection& aHttpConnection,
                const TDesC& aUrl);
    };

#endif      // URL_HANDLER_FACTORY_H
            
// End of File