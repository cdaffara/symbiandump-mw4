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
* Description:  A class that fetches resources via HTTP 1.1.
*
*/


#ifndef HTTP_HANDLER_H
#define HTTP_HANDLER_H


// INCLUDES
#include <e32base.h>
#include <http/mhttptransactioncallback.h>
#include <http/rhttpheaders.h>
#include <http/rhttptransaction.h>

#include "HttpConnection.h"
#include "LeakTracker.h"
#include "UrlHandler.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CTimer;
class RHTTPSession;
class RStringPool;

// CLASS DECLARATION


/**
*  A class that fetches resources via HTTP 1.1.
*
*  \b Library: FeedsEngine.lib
*
*  @since 3.0
*/
class CHttpHandler: public CUrlHandler, public MHttpConnectionObserver,
        public MHTTPTransactionCallback
    {
    public:  // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CHttpHandler* NewL(CHttpConnection& aHttpConnection);
        
        /**
        * Destructor.
        */        
        virtual ~CHttpHandler();
        
        
    public:  // From CUrlHandler
        /**
        * Loads the given url -- asynchronously
        *
        * @since 3.0
        * @param aUrl The url to load
        * @param aObserver The load observer.
        * @return void.
        */
        virtual void LoadUrlL(const TDesC& aUrl, MLoadObserver& aObserver);
        

      public:  // From MHttpConnectionObserver
        /**
        * Notifies the observer that the connection is available.
        * 
        * @since 3.0
        * @return 
        */
        virtual void ConnectionAvailable(); 
        
        /**
        * Notifies the observer that the establishment of the connection failed.
        * 
        * @since 3.0
        * @param aStatus The reason for the failure.
        * @return 
        */
        virtual void ConnectionFailed(TInt aStatus); 


	  public: // From MHTTPTransactionCallback
        /**
        * Called when the filter's registration conditions are satisfied for events that
        * occur on a transaction.
        *
        * @since 3.0
        * @param aTransaction The transaction that the event has occurred on.
        * @param aEvent The event that has occurred.
        * @return void.
        */
        virtual void MHFRunL(RHTTPTransaction aTransaction, const THTTPEvent& aEvent);

        /**
        * Called when RunL leaves from a transaction event. This works in the same
        * way as CActve::RunError; return KErrNone if you have handled the error.
        *
        * @since 3.0
        * @param aError The leave code that RunL left with.
        * @param aTransaction The transaction that was being processed.
        * @param aEvent The Event that was being processed.
        * @return KErrNone if the error has been cancelled or the code
        *         of the continuing error otherwise.
        */
        virtual TInt MHFRunError(TInt aError, RHTTPTransaction aTransaction, 
                const THTTPEvent& aEvent);
        

    private:
        /**
        * C++ default constructor.
        */
        CHttpHandler(CHttpConnection& aHttpConnection);
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Cleanup stack callback method to release a DataSupplier.
        *
        * @since 3.0
        * @param aPtr A libxml2 document ptr.
        * @return void.
        */
        static void CleanupDataSupplier(TAny *aPtr);

        /**
        * Get the content-type and char-encoding from the response header.
        *
        * @since 3.0
        * @param aContentType The content-type.
        * @param aCharSet The char-encoding.
        * @return void.
        */       
        void GetContentTypeL(TDesC*& aContentType, TDesC*& aCharSet);

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
        void LoadCompleted(TInt aStatusCode, TDesC8* aResponseBody,
                const TDesC& aContentType, const TDesC& aCharSet);

        /**
        * The timer's callback used to abort connections that stop responding.
        *
        * @since 3.0
        * @param aPtr Holds the iTimerState pointer.
        * @return An error code.
        */       
        static TInt TimerCallback(TAny* aPtr);


    private:
        const TTimeIntervalMicroSeconds32 KTimerPeriod;
        
        TLeakTracker      iLeakTracker;

        CHttpConnection*  iHttpConnection;
        TBool             iIsConnectionObserver;
        RHTTPSession&     iSession;
        RStringPool       iStringPool;
        RHTTPTransaction  iTransaction;
        RHTTPHeaders      iRespHeaders;
        RHTTPHeaders	  iUserAgentHeader;

        MLoadObserver*    iObserver;

        HBufC8*           iResponseBuffer;

        CPeriodic*        iTimer;
        TCallBack         iTimerState;
        TTime             iLastActivity;
        TInt			  iStatusCode;
    };

#endif      // HTTP_HANDLER_H
            
// End of File