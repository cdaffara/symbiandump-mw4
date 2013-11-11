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
* Description:  A task to update all Feeds.
*
*/


#ifndef UPDATE_ALL_FEEDS_TASK_H
#define UPDATE_ALL_FEEDS_TASK_H


// INCLUDES
#include <e32base.h>

#include "LeakTracker.h"
#include "UpdateFeedTask.h"
#include "Task.h"
#include "f32file.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CPackedFeed;
class CUpdateFeedTask;
class CCheckDiskSpace;

// CLASS DECLARATION


/**
*  The UpdateAllFeedsTask's observer.
*
*  \b Library: FeedsEngine.lib
*
*  @since 3.1
*/
class MUpdateAllFeedsTaskObserver
    {
    public: 
        /**
        * Notifies the observer that the task is about to start a lengthy 
        * operation.  In response the observer could display somekind of 
        * status indicator.
        *
        * @since 3.1
        * @return void.
        */
        //virtual void StartWait() = 0;

        /**
        * Called to notify the obsever how many feeds remain to be updated.
        *
        * @since 3.1
        * @param aStatusCode The completion status of the request.
        * @return Void
        */
        virtual void UpdateAllFeedsProgress(TInt aMaxCount, TInt aRemaining) = 0;

        /**
        * Called upon completion of the task.
        *
        * @since 3.1
        * @param aStatusCode The completion status of the request.
        * @return Void
        */
        virtual void UpdateAllFeedsCompleted(TInt aStatusCode) = 0;

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
*  @since 3.1
*/
class CUpdateAllFeedsTask: public CTask, public MUpdateFeedTaskObserver
    {
    public:  // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CUpdateAllFeedsTask* NewL(CFeedsServer& aFeedsServer, 
                TInt aFolderListId, MUpdateAllFeedsTaskObserver& aObserver);
        
        /**
        * Two-phased constructor.
        * For a folder list, only update selected feeds
        */
        static CUpdateAllFeedsTask* NewL(CFeedsServer& aFeedsServer,
                TInt aFolderListId, const RArray<TInt>& aFeedIds, MUpdateAllFeedsTaskObserver& aObserver);
        
        /**
        * Destructor.
        */        
        virtual ~CUpdateAllFeedsTask();
        
        
    public:  // From CTask
        /**
        * Starts the task.
        *
        * @since 3.1
        * @return Void
        */
        virtual void StartTaskL();
        

    public:  // From MUpdateFeedTaskObserver
        /**
        * Notifies the observer that the task is about to start a lengthy 
        * operation.  In response the observer could display somekind of 
        * status indicator.
        *
        * @since 3.1
        * @return void.
        */
        virtual void StartWait();

        /**
        * Called upon completion of the task.
        *
        * @since 3.1
        * @param aPackedFeed The resulting feed.  aPackedFeed is adopted by the 
        *                    implementing class.
        * @param aFeedUrl The feed's url.
        * @param aFeedId feed id
        * @param aStatusCode The completion status of the request.
        * @return Void
        */
        virtual void Completed(CPackedFeed* aPackedFeed, const TDesC& aFeedUrl,TInt aFeedId, TInt aStatusCode);

        /**
        * Called upon start of the task.
        *
        * @since 3.1
        * @param .
        * @return connection
        */
        virtual CHttpConnection& HttpConnection();
        
    public:
        /**
        * Adding the FeedIDs for updation .
        *
        * @since 7.1
        * @param aFeedIds.
        * @return Integer
        */
        TInt AddFeedL( RArray<TInt> aFeedIds);

        /**
        * Called upon completion of the task.
        *
        * @since 7.1
        * @param aStatusCode The completion status of the request.
        * @return Void
        */
        void UpdateAllFeedsCompleted(TInt aStatusCode);

    private:
        /**
        * C++ default constructor.
        */
        CUpdateAllFeedsTask(CFeedsServer& aFeedsServer, TInt aFolderListId, 
                MUpdateAllFeedsTaskObserver& aObserver, TBool aSelected);
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Starts the update of the next feed.
        *
        * @since 3.1
        * @return void.
        */
        void UpdateNextFeedL();

    private:
        TLeakTracker                  iLeakTracker;
        
        MUpdateAllFeedsTaskObserver&  iObserver;
        
        TInt                          iFolderListId;
        RArray<TInt>                  iFeedIds;
        TInt                          iNextFeedIndex;
        CUpdateFeedTask*              iUpdateFeedTask;
        TBool                         iSelected;
        CCheckDiskSpace*              iCheckDiskSpace ;
    };

/**
*  Checks disk space.
*
*  \b Library: FeedsEngine.lib
*
*  @since 7.1
*/
// CLASS DECLARATION
class CCheckDiskSpace : public CActive
    {
    public:	// Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CCheckDiskSpace* NewL(CUpdateAllFeedsTask*);

        /**
        * Destructor.
        */
        ~CCheckDiskSpace();

     private://	Constructor

        /**
        * C++ default constructor.
        */ 	
        CCheckDiskSpace();

        /**
        * By default Symbian 2nd phase constructor is private.
        *
        * @param pointer to CFeedsServer class,This pointer will be used to make server sleep and to wake up server
        *
        */
        void ConstructL(CUpdateAllFeedsTask*);

    public:	//Functions from CActive Class
        /**
        * Handles an active object's request completion event.
        *
        * @since 7.1
        * @return void.
        */
        void RunL();

        /**
        * Called by CActiveScheduler when RunL() Method leaves.
        *
        * @since 7.1
        * @return void.
        */
        void RunError();

        /**
        * Called by CActiveScheduler when Cancel() Method is called.
        *
        * @since 7.1
        * @return void.
        */
        void DoCancel();

        /**
        * This method subscribe to receive event when OS level
        * when the memory in the C: drive falls below a certain level
        *
        * @since 7.1
        * @return void.
        */
        void StartListening();

    private:

        CUpdateAllFeedsTask* iUpdateAllFeedsTask;
        RFs                 iRfs;
        TBool               iIsListening;

    };

#endif      // UPDATE_ALL_FEEDS_TASK_H
            
// End of File
