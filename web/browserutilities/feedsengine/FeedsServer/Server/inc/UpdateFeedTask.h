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
* Description:  A task to update a Feed.
*
*/


#ifndef UPDATE_FEED_TASK_H
#define UPDATE_FEED_TASK_H


// INCLUDES
#include <e32base.h>

#include "FeedParserObserver.h"
#include "LeakTracker.h"
#include "LoadObserver.h"
#include "Task.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CIdle;
class CFeedHandler;
class CHttpConnection;
class CPackedFeed;
class CUrlHandler;

// CLASS DECLARATION


/**
*  The UpdateFeedTask's observer.
*
*  \b Library: FeedsEngine.lib
*
*  @since 3.0
*/
class MUpdateFeedTaskObserver
    {
    public: 
        /**
        * Notifies the observer that the task is about to start a lengthy 
        * operation.  In response the observer could display somekind of 
        * status indicator.
        *
        * @since 3.0
        * @return void.
        */
        virtual void StartWait() = 0;

        /**
        * Called upon completion of the task.
        *
        * @since 3.0
        * @param aPackedFeed The resulting feed.  aPackedFeed is adopted by the 
        *                    implementing class.
        * @param aFeedUrl The feed's url.
        * @param aStatusCode The completion status of the request.
        * @return Void
        */
        virtual void Completed(CPackedFeed* aPackedFeed, const TDesC& aFeedUrl,TInt aFeedId, TInt aStatusCode) = 0;

        /**
        * Called upon start of the task.
        *
        * @since 3.1
        * @param .
        * @return connection
        */
        virtual CHttpConnection& HttpConnection() = 0;
    };


/**
*  A task to update a Feed.
*
*  \b Library: FeedsEngine.lib
*
*  @since 3.0
*/
class CUpdateFeedTask: public CTask, public MLoadObserver, public MFeedParserObserver
    {
    public:  // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CUpdateFeedTask* NewL(CFeedsServer& aFeedsServer, 
                const TDesC& aUrl,TInt aFeedId, MUpdateFeedTaskObserver& aObserver);
        
        /**
        * Two-phased constructor.
        */
        static CUpdateFeedTask* NewL(CFeedsServer& aFeedsServer, const TDesC& aUrl,TInt aFeedId, 
                TDesC8* aFeedBuffer, MUpdateFeedTaskObserver& aObserver);
        
        /**
        * Destructor.
        */        
        virtual ~CUpdateFeedTask();
        
        
    public:  // From CTask
        /**
        * Starts the task.
        *
        * @since 3.0
        * @return Void
        */
        virtual void StartTaskL();
        

    public:  // From MLoadObserver
        /**
        * Notifies the observer that the UrlHandler is about to start a lengthy 
        * operation.  In response the observer could display somekind of 
        * status indicator.
        *
        * @since 3.0
        * @return void.
        */
        virtual void StartWait();

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
                const TDesC& aContentType, const TDesC& aCharSet);


    public:  // From MFeedParserObserver
        /**
        * The beginning of a feed was found.
        *
        * @since 3.1
        * @return void.
        */
        virtual void FeedBeginsL();

        /**
        * The end of a feed was found.
        *
        * @since 3.1
        * @return void.
        */
        virtual void FeedEndsL();

        /**
        * The beginning of a item was found.
        *
        * @since 3.1
        * @return void.
        */
        virtual void ItemBeginsL();

        /**
        * The end of a item was found.
        *
        * @since 3.1
        * @return void.
        */
        virtual void ItemEndsL();

        /**
        * The beginning of a enclosure was found.
        *
        * @since 3.1
        * @return void.
        */
        virtual void EnclosureBeginsL();

        /**
        * The end of a enclosure was found.
        *
        * @since 3.1
        * @return void.
        */
        virtual void EnclosureEndsL();

        /**
        * A attribute was found.  This attribute should be applied to
        * the enclosing entity (Feed, Item, or Enclosure).
        *
        * @since 3.1
        * @param aAttribute The attribute.
        * @param aValue The attribute's value.
        * @return void.
        */
        virtual void AddAttributeL(TInt aAttribute, const TDesC& aValue);

        /**
        * An unimportant element was found.  The client can use this 
        * information for feed validation.
        *
        * @since 3.1
        * @return void.
        */
        virtual void OtherTitleL();

        /**
        * An unimportant element was found.  The client can use this 
        * information for feed validation.
        *
        * @since 3.1
        * @return void.
        */
        virtual void OtherDescriptionL();

        /**
        * An unimportant element was found.  The client can use this 
        * information for feed validation.
        *
        * @since 3.1
        * @return void.
        */
        virtual void OtherLinkL();


    private:
        /**
        * C++ default constructor.
        */
        CUpdateFeedTask(CFeedsServer& aFeedsServer, MUpdateFeedTaskObserver& aObserver);
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(const TDesC& aUrl,TInt aFeedId);

        /**
        * Parses the buffer to the observer now that the callstack has unrolled.
        *
        * @since 3.0
        * @param aPtr The this pointer.
        * @return Always returns EFalse.
        */
        static TInt DelayedParse(TAny* aPtr);
                

    private:
        TLeakTracker              iLeakTracker;
        
        MUpdateFeedTaskObserver&  iObserver;
        CIdle*                    iIdle;

        CUrlHandler*              iUrlHandler;
        TDesC*                    iUrl;
        TDesC8*                   iFileBuffer;
        
        CPackedFeed*              iPackedFeed;
        TInt 					  iFeedId;
    };
    
    
#endif      // UPDATE_FEED_TASK_H
            
// End of File