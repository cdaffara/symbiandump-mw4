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
* Description:  Represent a session on server side.
*
*/


#ifndef FEEDS_SERVER_SESSION_H
#define FEEDS_SERVER_SESSION_H

// INCLUDES
#include <e32base.h>

#include "LeakTracker.h"
#include "UpdateFeedTask.h"
#include "UpdateAllFeedsTask.h"
#include "ImportFeedsTask.h"
#include "PackedFolder.h"

// CONSTANTS
// Fixed for Bug id - JJUN-78VES7 (FeedsServer crashes under IPC attack)
#define KMaxTitleLength 256
// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION
class CFeedsServer;
class CPacked;
class CPackedFeed;
class CPackedFolder;
class CSessionHttpConnection;


/**
*  Represents a session on server side.
*
*  \b Library: FeedsEngine.lib
*
*  @since 3.1
*/
NONSHARABLE_CLASS(CFeedsServerSession): public CSession2, public MUpdateFeedTaskObserver,
        public MUpdateAllFeedsTaskObserver, public MImportFeedsTaskObserver
    {
    public:  // Constructors and destructor

        /**
        * Constructor.
        */
        static CFeedsServerSession* NewL(CFeedsServer& aFeedsServer);

        /**
        * Destructor.
        */
        ~CFeedsServerSession();


    private: // From CSession2
        /**
        * Client-server framework calls this function to handle messages 
        * from client.  This calls DispatchMessageL under the trap harness 
        * in order to propagate any errors back to the client (via 
        * RMessage2::Complete) rather than leaving.
        *
        * @since 3.1
        * @param aMessage A received message.
        * @return Void.
        */
        virtual void ServiceL(const RMessage2& aMessage);


    public:  // From MUpdateFeedTaskObserver
        /**
        * Notifies the observer that the task is about to start a lengthy 
        * operation.  In response the observer could display somekind of 
        * status indicator.
        *
        * @since 3.0
        * @return void.
        */
        virtual void StartWait();

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
        virtual void Completed(CPackedFeed* aPackedFeed, const TDesC& aFeedUrl, TInt aFeedId, TInt aStatusCode);


    public:  // From MUpdateAllFeedsTaskObserver
        /**
        * Called to notify the obsever how many feeds remain to be updated.
        *
        * @since 3.1
        * @param aStatusCode The completion status of the request.
        * @return Void
        */
        virtual void UpdateAllFeedsProgress(TInt aMaxCount, TInt aRemaining);

        /**
        * Called upon completion of the task.
        *
        * @since 3.1
        * @param aStatusCode The completion status of the request.
        * @return Void
        */
        virtual void UpdateAllFeedsCompleted(TInt aStatusCode);

        /**
        * Returns the server's http-connection instance;
        *
        * @since 3.1
        * @return The database.
        */
        inline virtual CHttpConnection& HttpConnection() {return (CHttpConnection&) *iHttpConnection;}
        
    public:  // From MImportFeedsTaskObserver
        /**
        * Called to notify the obsever to indicate the start of OPML import
        *
        * @since 3.2
        * @return Void
        */    
        virtual void StartImportWait();

        /**
        * Called to notify the obsever to indicate the completion OPML import
        *
        * @since 3.2
        * @param aStatusCode The completion status of the request.
        * @return Void
        */    
        virtual void ImportCompleted(TInt aStatusCode);

    public:  // New methods
        /**
        * 
        *
        * @since 3.1
        * @param aMessage The message to write to.
        * @return void.
        */
        void NetworkConnectionNeededL();


    private:  // New methods
        /**
        * C++ default constructor.
        */
        CFeedsServerSession(CFeedsServer& aFeedsServer);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Handle messages from the client.
        *
        * @since 3.1
        * @param aMessage A received message.
        * @return Void.
        */
        void DispatchMessageL(const RMessage2& aMessage);

        /**
        * Returns the root folder item -- fetching it if need be.
        *
        * @since 3.1
        * @param aMessage A received message.
        * @return void.
        */
        void GetRootFolderL(const RMessage2& aMessage);
   
        /**
        * Returns the requested feed -- fetching it if need be.
        *
        * @since 3.1
        * @param aMessage A received message.
        * @return void.
        */
        void GetFeedL(const RMessage2& aMessage);
   
        /**
        * Cancel all activities that can be cancelled.
        *
        * @since 3.1
        * @param aMessage A received message.
        * @return void.
        */
        void CancelAllRequestsL(const RMessage2& aMessage);
   
        /**
        * Cancel all feed related activities that can be cancelled.
        *
        * @since 3.1
        * @return void.
        */
        void CancelFeedRequestsL();
   
        /**
        * Cancel all folder related activities that can be cancelled.
        *
        * @since 3.1
        * @return void.
        */
        void CancelFolderRequestsL();
   
        /**
        * Updates the given feed's item status.
        *
        * @since 3.1
        * @param aMessage A received message.
        * @return void.
        */
        void ChangeFeedItemStatusL(const RMessage2& aMessage);
   
        /**
        * Updates the database to reflect the addition of the folder item.
        * A update root folder item is sent out to all active
        * folder item sessions.
        *
        * @since 3.1
        * @param aMessage A received message.
        * @return void.
        */
        void AddFolderItemL(const RMessage2& aMessage);
   
        /**
        * Updates the database to reflect the edited folder item.
        * A update root folder item is sent out to all active
        * folder item sessions.
        *
        * @since 3.1
        * @param aMessage A received message.
        * @return void.
        */
        void ChangeFolderItemL(const RMessage2& aMessage);
   
        /**
        * Updates the database to reflect the deletion of the folder item.
        * A update root folder item is sent out to all active
        * folder item sessions.
        *
        * @since 3.1
        * @param aMessage A received message.
        * @return void.
        */
        void DeleteFolderItemsL(const RMessage2& aMessage);
   
        /**
        * Updates the database to reflect the moved folder items.
        * A update root folder item is sent out to all active
        * folder item sessions.
        *
        * @since 3.1
        * @param aMessage A received message.
        * @return void.
        */
        void MoveFolderItemsL(const RMessage2& aMessage);
   
        /**
        * Updates the database to reflect the new parentage of the 
        * folder items.  A update root folder item is sent out to 
        * all active folder item sessions.
        *
        * @since 3.1
        * @param aMessage A received message.
        * @return void.
        */
        void MoveFolderItemsToL(const RMessage2& aMessage);
   
        /**
        * Update the given folder items or all feeds associated with the folder list id.
        *
        * @since 3.1
        * @param aMessage A received message.
        * @return void.
        */
        void UpdateFolderItemsL(const RMessage2& aMessage);

        /**
        * Returns the settings.
        *
        * @since 3.1
        * @param aMessage A received message.
        * @return void.
        */
        void GetSettingsL(const RMessage2& aMessage);
   
        /**
        * Updates the database to reflect the new settings.
        * A update settings is sent out to all active
        * server sessions.
        *
        * @since 3.1
        * @param aMessage A received message.
        * @return void.
        */
        void ChangeSettingsL(const RMessage2& aMessage);      

        /**
        * Sets up a notifier to execute when the settings changes.
        *
        * @since 3.2
        * @param aMessage A received message.
        * @return void.
        */
        void WatchSettingsL(const RMessage2& aMessage);
   
        /**
        * Disconnect the session http connection provided by client app.
        *
        * @since 3.1
        * @return void.
        */
        void DisconnectManualUpdateConnection(const RMessage2& aMessage);      

        /**
        * Sets up a notifier to execute when the folder list changes.
        *
        * @since 3.1
        * @param aMessage A received message.
        * @return void.
        */
        void WatchFolderListL(const RMessage2& aMessage);

        /**
        * Notify the session that the given folder-list changed.
        *
        * @since 3.1
        * @return void.
        */
        void NotifyFolderListChanged(TInt aFolderListId);

        /**
        * Notify the session that the settings of given folder-list changed.
        *
        * @since 3.2
        * @return void.
        */
        void NotifySettingsChanged(TInt aFolderListId);

        /**
        * Called upon completion of the task -- helper method for Completed.
        *
        * @since 3.1
        * @param aPackedFeed The resulting feed.  Adopted by this method.
        * @param aFeedUrl The feed's url.
        * @param aStatusCode The completion status of the request.
        * @return Void
        */
        void CompletedHelperL(CPackedFeed*& aPackedFeed, const TDesC& aFeedUrl,TInt aFeedId);

        /**
        * Send the next chunk of the token-array to the client.
        *
        * @since 3.1
        * @param aPacked The source of the payload to send.
        * @param aMessage The message to write to.
        * @return void.
        */
        void SendTokenChunkL(CPacked& aPacked, const RMessagePtr2& aMessage);

        /**
        * Send the next chunk of the string-table to the client.
        *
        * @since 3.1
        * @param aPacked The source of the payload to send.
        * @param aMessage The message to write to.
        * @return void.
        */
        void SendStringTableChunkL(CPacked& aPacked, const RMessage2& aMessage);

        /**
        * Set the connection to use.
        *
        * @since 3.1
        * @param aMessage The message to write to.
        * @return void.
        */
        void SetConnectionL(const RMessage2& aMessage);
   
        /**
        * Import OPML file passed from the client.
        *
        * @since 3.2
        * @param aMessage The message passed by client
        * @return void.
        */
        void ImportOPMLL( const RMessage2& aMessage );      
        
        /**
        * Import OPML file passed from the client asyncronously.
        *
        * @since 3.2
        * @param aMessage The message passed by client
        * @return void.
        */
		void ImportOPMLAsyncL( const RMessage2& aMessage );

        /**
        * Export feeds as an OPML file passed from the client.
        *
        * @since 3.2
        * @return void.
        */
        void ExportOPMLL( const RMessage2& aMessage );      

        /**
        * Completes any pending (asynchronous) message from the client
        * with the specified error code.
        */
        void CompleteClientPendingMessage( TInt aCompletionCode = KErrNone );
        
        /**
        * Set up an asynchronous message from the client so that we can
        * complete it at a later date.
        */
        void SetClientPendingMessage( RMessagePtr2 aMessage );
        
        /**
        * Extracts the message sent by the client
        *
        * @since 3.2
        * @return void.
        */
        void ExtractClientImportMessageL();

        /**
        * Callback function for calling SendTokenChunkL from CompletedHelperL() function
        * This ensures if GetFeedL call was cancelled then no data should be sent to client.
        *
        * @since ?
        * @ param aPtr Pointer to CFeedServerSession class
        * @return Integer
        */
        static TInt LazyCallBack(TAny* aPtr);

        /**
        * This function checks whether disk space has not gone below critical level
        *
        * @since 7.1
        * @return ETrue if the there is sufficient space.
        */
        TBool IsSpaceAvailableL();

        /**
        * Called upon completion of the task.
        *
        * @since 7.1
        * @param aStatusCode The completion status of the request.
        * @return Void
        */
        void UpdateAllFeedsCompletedL(TInt aStatusCode);


    private:  // Friends
        friend class CFeedsServer;
        
    
    private:   
        TLeakTracker           iLeakTracker;
        
        CFeedsServer&                 iFeedsServer;
        CSessionHttpConnection*       iHttpConnection;
        
        CUpdateAllFeedsTask*  iUpdateAllFeedsTask;
        TInt                  iUpdateAllFolderListId;
        TInt                  iFolderListId;
        CUpdateFeedTask*      iUpdateFeedTask;
        RMessagePtr2          iPendingMessage;
        
        // TODO: These variables aren't safe to use if multiple requests
        //       are being made in the same RFeedsServer session.  This would be 
        //       an issue if the client had two active RFeeds or two active 
        //       RFolderItems or even one active RFeed and one active RFolderItem 
        //       (which may happen if a client is fetching a feed and a different 
        //       client changes their shared folder list).
        CPackedFeed*          iPackedFeed;
        TBool                 iStoreFeeds;
        
        CPackedFolder*        iPackedFolder;

        TInt                  iWatchFolderListId;
        RMessage2             iWatchFolderListMessage;
        RMessagePtr2          iWatchSettingsMessage;
        
        // iResponseTokensSent and iResponseOffset are used by GetFeed and GetRootFolder
        // There may be multiple GetFeed or GetRootFolder request coming from client for one feed or
        // folder list request,These two variable keep track of what part of CPacked should be sent
        // to the client.
        // for GetRootFolder iCurrentRequest = 0
        // for GetFeed       iCurrentRequest = 1;
        TInt                  iCurrentRequest;
        TBool                 iResponseTokensSent[2];
        TInt                  iResponseOffset[2];

        CImportFeedsTask*     iOPMLImportTask;

        TInt                  iCurrOp;
        TInt                  iPrevOp;
        HBufC*                iExportOPMLFileName;
        CIdle*                iLazyCaller; //To call SetTokenChunkL function;
        TInt                  iPendingStatus;
        TInt                  iPendingMessageHandle;
        TBool                 iGetFeedCalled;


    };


#endif      // FEEDS_SERVER_SESSION_H

// End of File
