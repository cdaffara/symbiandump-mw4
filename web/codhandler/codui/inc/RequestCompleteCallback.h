/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declaration of class CRequestCompleteCallback.h   
*
*/


#ifndef REQUEST_COMPLETE_CALLBACK_H
#define REQUEST_COMPLETE_CALLBACK_H

// INCLUDES

#include <e32base.h>

// CLASS DECLARATION

/**
* Active object to transfer a request completion to a callback.
* NOTE: This object handles only completion, but not cancellation. It does not
* make ('own') the request, therefore it cannot possibly cancel.
* Users of this object must make sure that the request is already completed
* or cancelled before this object is cancelled/deleted. DoCancel() of this
* object panics (to avoid the hang).
*/
NONSHARABLE_CLASS( CRequestCompleteCallback ): public CActive
    {

    public:     // Constructors and destructor

        /**
        * Constructor.
        */      
        CRequestCompleteCallback();
        
        /**
        * Destructor.
        */      
        virtual ~CRequestCompleteCallback();

    public:     // new methods

        /**
        * Start waiting for completion. A request (on iStatus of this object)
        * must be outstanding.
        * @param aCallback Callback to call when request completes.
        * Note that the callback is called only once per CallbackOnCompletion()
        * invocation (like in CAsyncOneShot etc.), and repeated callbacks are
        * not supported. Therefore, the callback function must return EFalse.
        */
        void CallbackOnCompletion( const TCallBack& aCallback );

    public:     // from CActive

        /**
        * Cancel protocol implementation: panic
        * This object cannot cancel the request.
        */
        void DoCancel();

        /**
        * Request completed: call the callback.
        */
        void RunL();

    private:    // data

        TCallBack iCallback;    ///< The callback.

    };

#endif /* def REQUEST_COMPLETE_CALLBACK_H */
