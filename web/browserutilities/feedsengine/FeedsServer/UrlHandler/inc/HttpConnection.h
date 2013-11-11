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
* Description:  A class that holds the shared connection to a access point.
*
*/


#ifndef HTTP_CONNECTION_H
#define HTTP_CONNECTION_H


// INCLUDES
#include <e32base.h>
#include <http/rhttpsession.h>
#include <mconnectioncallback.h>

#include "LeakTracker.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION


/**
*  In interface which is called when the access point becomes available.
*
*  \b Library: FeedsEngine.lib
*
*  @since 3.0
*/
class MHttpConnectionObserver
    {
    public:
        /**
        * Notifies the observer that the connection is available.
        * 
        * @since 3.0
        * @return 
        */
        virtual void ConnectionAvailable() = 0; 

        /**
        * Notifies the observer that the establishment of the connection failed.
        * 
        * @since 3.0
        * @param aStatus The reason for the failure.
        * @return 
        */
        virtual void ConnectionFailed(TInt aStatus) = 0; 
    };


/**
*  A base class for http connection.
*
*  \b Library: FeedsEngine.lib
*
*  @since 3.0
*/
class CHttpConnection: public CBase, public MConnectionCallback
    {
    public:  // Destructor
        /**
        * Destructor.
        */        
        virtual ~CHttpConnection();
        
        
    public: // From MConnectionCallback        
        /**
        * This function cancel outstanding transactions and notify a user 
        * with the message "Out of coverage"
        * 
        * @since 3.0
        * @param aError Timeout error etc
        * @return 
        */
        virtual void CoverageEvent(TInt aError); 

        
    public:  // New methods.
        /**
        * Sets the observer.  In practice this is only set 
        * once during the lifetime of the instance.
        *
        * @since 3.0
        * @param aObserver The observer -- NULL is a valid value.
        * @return void.
        */
        void SetObserver(MHttpConnectionObserver* aObserver);

        /**
        * Returns the session.
        *
        * @since 3.0
        * @return The session.
        */
        RHTTPSession& Session();

        /**
        * Returns whether or not the connection is active.
        *
        * @since 3.0
        * @return void.
        */
        virtual TBool IsConnected() = 0;

        /**
        * Closes the connection.
        *
        * @since 3.0
        * @return void.
        */
        virtual void Disconnect() = 0;

		public: // New method for proper object deletion
        /**
        * Calls delete when stack is unrolled
        *
        * @since 5.0
        * @return void
        */
        void AutoDelete();
        
        /**
        * Callback function to delete object
        *
        * @since 5.0
        * @return TInt (EFalse)
        */
        static TInt DelayedDelete(TAny* aPtr);

    protected:
        /**
        * C++ default constructor.
        */
        CHttpConnection();
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void BaseConstructL();


    protected:
        RHTTPSession                 iSession;
        CIdle*                       iAutoDelete;

        MHttpConnectionObserver*     iObserver;
    };

#endif      // HTTP_CONNECTION_H
            
// End of File
