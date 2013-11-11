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
* Description:  An interface used to provide feed back from a UrlHandler.
*
*/


#ifndef LOAD_OBSERVER_H
#define LOAD_OBSERVER_H


// INCLUDES
#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION


/**
*  An interface used to provide feed back from a UrlHandler.
*
*  \b Library: FeedsEngine.lib
*
*  @since 3.0
*/
class MLoadObserver
    {
    public:
        /**
        * Notifies the observer that the UrlHandler is about to start a lengthy 
        * operation.  In response the observer could display somekind of 
        * status indicator.
        *
        * @since 3.0
        * @return void.
        */
        virtual void StartWait() = 0;

        /**
        * Passes the status code and responseBody to the observer.  The observer
        * adopts aResponseBody.
        *
        * @since 3.0
        * @param aStatusCode The status code - normalized to the http response codes.
        * @param aResponseBody The response body or NULL.
        * @param aContentType The content-type.
        * @param aCharSet The char-encoding.
        * @return void.
        */
        virtual void LoadCompleted(TInt aStatusCode, TDesC8* aResponseBody,
                const TDesC& aContentType, const TDesC& aCharSet) = 0;
    };

#endif      // LOAD_OBSERVER_H
            
// End of File