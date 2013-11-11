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
* Description:  The Feeds project test harness.
*
*/


#ifndef TEST_HARNESS_H
#define TEST_HARNESS_H


// INCLUDES
#include <e32base.h>
#include <e32std.h>
#include <f32file.h>

#include <FeedsServerClient.h>
#include <FeedsServerFeed.h>
#include <FeedsServerFolderItem.h>
#include <FeedsServerSettings.h>
#include "FeedsWaitDialog.h"

// CONSTANTS

// MACROS

// DATA TYPES
enum TOperation
    {
    EParser,
    EFetchRootFolderItem,
    EWatchFolderList,
    EImportOPML,
    EUpdateAll,
    EAddFolderItem,
    EChangeFolderItem,
    EDeleteFolderItems,
    EMoveFolderItemsTo,
    EMoveFolderItems,
    EUpdateFolderItems,
    EFetchFeed,
    EUpdateFeedItemStatus,
    EPrintDBTables
    };

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CInternetConnectionManager;
class CFeedsWaitDialog;

// CLASS DECLARATION


/**
*  The TestHarness's observer.
*
*  \b Library: FeedsEngine.lib
*
*  @since 3.0
*/
class MTestHarnessObserver
    {
    public: 
        /**
        * Notifies the observer that the test harness is completed.
        *
        * @since 3.0
        * @return void.
        */
        virtual void Completed() = 0;
    };
    
    
/**
*  The Feeds project test harness.
*
*  \b Library: FeedsEngine.lib
*
*  @since 3.0
*/
class CTestHarness: public CBase, 
                    public MFeedsServerObserver, 
                    public MFeedObserver, 
                    public MFolderItemObserver,
                    public MFeedsWaitDialogListener
    {
    public:  // Data types
        enum TTestSuite
            {
            ECanned,
            ECannedShort,
            ECannedStartAt,
            ECannedAtom,
            ELive
            };


    public:  // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CTestHarness* NewL(MTestHarnessObserver& aObserver);
        
        /**
        * Destructor.
        */        
        virtual ~CTestHarness();
   

    public:  // From MFeedsServerObserver
        /**
        * Reports the progress of the UpdateAllFeedsL request.  
        *
        * @since 3.1
        * @param aTotalCount The total count of feeds to be updated.
        * @param aRemaining The current number of feeds remaining to be updated.
        * @return void.
        */
        virtual void UpdateAllProgress(TInt /*aTotalCount*/, TInt /*aRemaining*/) {};

        /**
        * Notifies the observer that the feeds server settings have changed.  
        * This is only called when the value was changed by a differnt client.
        *
        * @since 3.1
        * @return void.
        */
        virtual void SettingsChanged( TInt aStatus );


    public:  // From MFeedObserver
        /**
        * Called when the asynchronous request is complete.
        *
        * @since 3.1
        * @param aStatusCode The completion status code.
        * @return void.
        */
        virtual void FeedRequestCompleted(TInt aStatusCode);

        /**
        * Notifies the observer that the feed has changed.  This method is called
        * when the feed is updated by the feeds server.
        *
        * @since 3.1
        * @return void.
        */
        virtual void FeedChanged() {};

        /**
        * Notifies the observer that the feed's item status has changed.  
        * This method is called when __another__ client updates the feed's 
        * item status (via UpdateFeedItemStatusL).
        *
        * @since 3.1
        * @return void.
        */
        virtual void FeedItemStatusChanged() {};


    public:  // From MFolderItemObserver
        /**
        * Notifies the observer that the root FolderItem is ready or an error occurred.
        *
        * @since 3.1
        * @param aStatus The result code of the FetchRootFolderItemL method.
        * @return void.
        */
        void FolderItemRequestCompleted(TInt aStatus);

        /**
        * Notifies the observer that the FolderItem has changed.  The client
        * should request an updated copy from the server.
        *
        * @since 3.1
        * @return void.
        */
        void FolderItemChanged() {};


    public:  // From MConnectionObserver
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
        virtual void NetworkConnectionNeededL(TInt* /*aConnectionPtr*/, TInt& /*aSockSvrHandle*/,
            TBool& /*aNewConn*/, TApBearerType& /*aBearerType*/);


    public:  // From MWaitDialogListener
        /**
        * Called when the user presses the cancel button.
        *
        * @since 3.0
        * @return void.
        */
        void DialogDismissedL();


    public:  // New Methods.
        /**
        * Starts the test harness
        *
        * @since 3.0
        * @param aSuite The suite to run.
        * @param aStartAt The starting index.
        * @return void.
        */
        void Start(TTestSuite aSuite, TInt aStartAt = 0);
        
        /**
        * @since 3.2
        */
        void FetchRootFolderItemL();
        
        /**
        * @since 3.2
        */
        void WatchFolderListL();
        
        /**
        * @since 3.2
        */
        void ImportOPMLL( const TDesC& aPath, TBool aClearFolderList = EFalse );
        
        /**
        * @since 3.2
        */
        void UpdateAllL();

        /**
        * @since 3.2
        */
        void AddFolderItemL( TInt aIsFolder );

        /**
        * @since 3.2
        */
        void ChangeFolderItemL();

        /**
        * @since 3.2
        */
        void DeleteFolderItemsL();

        /**
        * @since 3.2
        */
        void DeleteAllFolderItemsL();

        /**
        * @since 3.2
        */
        void MoveFolderItemsToL();

        /**
        * @since 3.2
        */
        void MoveFolderItemsL();

        /**
        * @since 3.2
        */
        void UpdateFolderItemsL();

        /**
        * @since 3.2
        */
        void FetchFeedL();

        /**
        * @since 3.2
        */
        void UpdateFeedItemStatusL();

        /**
        * @since 3.2
        */
        void UpdateSettingsL( TBool aOn );

        /**
        * Returns Feeds Server related settings synchronously.
        *
        * @since 3.2
        */
        void FetchServerSettingsSyncL();

        /**
        * Returns Feeds Server related settings asynchronously.
        *
        * @since 3.2
        */
        void FetchServerSettingsL();

        /**
        * @since 3.2
        */
        void DisconnectL();

        /**
        * Debug print database tables.
        * @since 3.2
        */
        void DebugPrintTablesL();


    private:
        /**
        * C++ default constructor.
        */
        CTestHarness(MTestHarnessObserver& aObserver);
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Tests a single Feed.
        *
        * @since 3.0
        * @param aPath The path to the feed file.
        * @return void.
        */
        void TestFeedL(const TDesC& aPath);

        /**
        * Tests all feeds in the given folder, where the feeds must be named XX.xml where
        * XX are consecutive integers (ie 1.xml, 2.xml, etc.).
        *
        * @since 3.0
        * @param aDirPath The path to the feeds folder.
        * @param aStartIndex The starting file-name index.
        * @param aStartIndex The ending file-name index.
        * @return void.
        */
        void TestFeedsL(const TDesC& aDirPath, TInt aStartIndex, TInt aEndIndex);

        /**
        * Tests a single Feed given the current value of iFeedIndex. See TestFeedsL.
        *
        * @since 3.0
        * @return void.
        */
        void TestNextFeedInSeriesL();

#if 0
        /**
        * Tests all feeds in the OPML file.
        *
        * @since 3.0
        * @param aPath The url to the OPML file.
        * @return void.
        */
        void TestFolderL(const TDesC& aPath);

        /**
        * Tests a single Feed given the current value of iFeedIndex. See TestFolderL.
        *
        * @since 3.0
        * @return void.
        */
        void TestNextFeedInFolderL();
#endif

        /**
        * Runs the next test.
        *
        * @since 3.0
        * @param aPtr The this pointer.
        * @return Always returns EFalse.
        */
        static TInt RunTest(TAny* aPtr);

        /**
        * @since 3.2
        */
        void ConnectToServerL();

        /**
        * @since 3.2
        */
        void DisconnectFromServer();

        /**
        * @since 3.2
        */
        void ShowServerError(TInt aStatus);

        /**
        * @since 3.2
        */
        void Debug_Print( const CFolderItem& aFolderItem ) const;

        /**
        * @since 3.2
        */
        TBool CheckRootFolderItem();


    private:
        MTestHarnessObserver&   iObserver;
        
        RFeedsServer            iFeedsServer;
        RFolderItem  			iRootFolder;
        RFeed                   iFeed;
        TBool					iIsConnected;

        CFeedsWaitDialog*		iWaitDialog;

        TDesC*                  iCurrentFeedUrl;

        TBool                   iTestingFeedSeries;
        TBool                   iTestingFolder;
        
        TInt                    iFeedIndex;
        TInt                    iEndFileIndex;
        TDesC*                  iDirPath;

        RFs                     iFs;
        RFile                   iLogFile;
        RFile                   iStatusFile;
        
        CIdle*                  iIdle;

        CInternetConnectionManager*  iConMgr;

        RPointerArray<const CFolderItem>  iMovedItems;
        TInt                    iOperation;
    };

#endif      // TEST_HARNESS_H
            
// End of File