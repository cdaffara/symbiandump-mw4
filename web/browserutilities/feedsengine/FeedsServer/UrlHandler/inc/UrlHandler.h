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
* Description:  UrlHandler base class.
*
*/


#ifndef URL_HANDLER_H
#define URL_HANDLER_H


// INCLUDES
#include <e32base.h>

#include "LoadObserver.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION


/**
*  UrlHandler base class.
*
*  \b Library: FeedsEngine.lib
*
*  @since 3.0
*/
class CUrlHandler: public CBase
    {
    public:
        /**
        * Loads the given url -- asynchronously
        *
        * @since 3.0
        * @param aUrl The url to load
        * @param aObserver The load observer.
        * @return void.
        */
        virtual void LoadUrlL(const TDesC& aUrl, MLoadObserver& aObserver) = 0;
    };

#endif      // URL_HANDLER_H
            
// End of File