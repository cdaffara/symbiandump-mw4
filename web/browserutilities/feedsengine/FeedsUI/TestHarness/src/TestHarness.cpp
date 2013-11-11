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
* Description:  The Feeds project test harness.
*
*/


#include "LeakTracker.h"
#include "Logger.h"
#include "StatusPane.h"
#include "TestHarness.h"
#include <InternetConnectionManager.h>
#include "FeedsWaitDialog.h"

#include <FeedsApp.Rsg>

//const TInt NTFolderListId = 270501633;
// default id: browser
const TInt NTFolderListId = 0;
// The pair is used testing feed with same name, url, different folder list Id should have 2 entries in database
// The default.xml should contain the same feed with folder list Id as 0
_LIT(KNewTitle, "Slashdot");
_LIT(KUrl, "http://rss.slashdot.org/Slashdot/slashdot");
//
//_LIT(KNewTitle, "New");
_LIT(KRenamedTitle, "Renamed");
//_LIT(KUrl, "http://xml.newsisfree.com/feeds/39/1439.xml");
_LIT(KChangedUrl, "http://www.theshiftedlibrarian.com/rss.xml");
// http://www.microsite.reuters.com/rss/topNews
// http://dictionary.reference.com/wordoftheday/wotd.rss

// -----------------------------------------------------------------------------
// CTestHarness::NewL
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CTestHarness* CTestHarness::NewL(MTestHarnessObserver& aObserver)
    {
    CTestHarness* self = new (ELeave) CTestHarness(aObserver);
    
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

        
// -----------------------------------------------------------------------------
// CTestHarness::CTestHarness
//
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CTestHarness::CTestHarness(MTestHarnessObserver& aObserver):
        iObserver(aObserver), iFeedsServer(*this, NTFolderListId), iRootFolder(*this, NTFolderListId, ETrue), iFeed(*this), 
        iIsConnected(EFalse), iWaitDialog(NULL)
    {
    LEAK_TRACKER_INCREMENT(CLeakTracker::ETestHarness);
    }
        

// -----------------------------------------------------------------------------
// CTestHarness::ConstructL
//
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CTestHarness::ConstructL()
    {
    _LIT(KInfoPath, "\\logs\\feeds\\info.txt");
    _LIT(KStatusPath, "\\logs\\feeds\\status.txt");
    TInt  err;

    // Connect to the file server.
    User::LeaveIfError(iFs.Connect());

    // If need be create the log dir.
    err = iFs.MkDirAll(KInfoPath);
    if (err != KErrAlreadyExists)
        {
        User::LeaveIfError(err);
        }

    // Create the log file.
    User::LeaveIfError(iLogFile.Replace(iFs, KInfoPath, EFileWrite));
    User::LeaveIfError(iStatusFile.Replace(iFs, KStatusPath, EFileWrite));

    iConMgr = CInternetConnectionManager::NewL(ETrue);

    // Create the WaitDialog
    if (iWaitDialog == NULL)
        {        
        iWaitDialog = CFeedsWaitDialog::NewL( *this );
        }
    
    // Connect to the server.
    ConnectToServerL();
    }        


// -----------------------------------------------------------------------------
// CTestHarness::~CTestHarness
//
// Deconstructor.
// -----------------------------------------------------------------------------
//
CTestHarness::~CTestHarness()
    {
    LEAK_TRACKER_DECREMENT(CLeakTracker::ETestHarness);

    delete iIdle;
    
    delete iDirPath;
    delete iCurrentFeedUrl;

	//++pK: Check for NULL
	if( iConMgr != NULL )
	{
		iConMgr->Disconnect();	
	}
    delete iConMgr;

    delete iWaitDialog;   

    DisconnectFromServer();

    iLogFile.Close();
    iStatusFile.Close();
    iFs.Close();
    }


// -----------------------------------------------------------------------------
// CTestHarness::Start
//
// Start the test harness.
// -----------------------------------------------------------------------------
//
void CTestHarness::Start(TTestSuite aSuite, TInt aStartAt)
    {
#if !defined( __WINS__ ) && !defined( __WINSCW__ )
#define REMOTE
#endif

#ifdef REMOTE
    _LIT(KCanned, "http://waplabdc.nokia-boston.com/browser/users/rss/feeds_canned_rss/");
    _LIT(KCannedAtom, "http://waplabdc.nokia-boston.com/browser/users/rss/feeds_canned_atom/");
    //_LIT(KLive, "http://waplabdc.nokia-boston.com/browser/users/rss/live.opml");
#else
    _LIT(KCanned, "file://c:/feeds_canned_rss/");
    _LIT(KCannedAtom, "file://c:/feeds_canned_atom/");
    //_LIT(KLive, "file://c:/live.opml");
#endif

    TInt err = KErrNone;

    iOperation = EParser;

    switch (aSuite)
        {
        case ECanned:
            TRAP(err, TestFeedsL(KCanned, 0, 2530));
            break;

        case ECannedShort:
            TRAP(err, TestFeedsL(KCanned, 0, 100));
            break;

        case ECannedStartAt:
            TRAP(err, TestFeedsL(KCanned, aStartAt, aStartAt));
            break;
            
        case ECannedAtom:
            TRAP(err, TestFeedsL(KCannedAtom, 0, 350));
            break;
            
        case ELive:
            //TRAP(err, TestFolderL(KLive));
            break;
        }

    if (err != KErrNone)
        {
        FEED_LOG1(_L("Feeds"), _L("Feeds_Errors.log"), 
                EFileLoggingModeAppend, _L("Warning!!! CTestHarness::Start failed, error[%d]."), err);
        }
    }


// -----------------------------------------------------------------------------
// CTestHarness::TestFeedL
//
// Tests a single Feed.
// -----------------------------------------------------------------------------
//
void CTestHarness::TestFeedL(const TDesC& aPath)
    {
    // Update the status pane to show some feedback to the user.
    TPtr  ptr(const_cast<TUint16*>(aPath.Ptr()), aPath.Length(), aPath.Length());
    
    if (ptr.Length() > 20)
        {
        ptr.Set(const_cast<TUint16*>(aPath.Ptr()) + 
                (aPath.Length() - 20), 20, 20);
        }
    StatusPane::SetTitleL(ptr);

    // Save the feed's url.
    delete iCurrentFeedUrl;
    iCurrentFeedUrl = NULL;
    
    iCurrentFeedUrl = aPath.AllocL();

    // Schedule the test to run after the call stack unrolls.
    iIdle = CIdle::NewL(CActive::EPriorityIdle);
    iIdle->Start(TCallBack(RunTest, this));
    }


// -----------------------------------------------------------------------------
// CTestHarness::TestFeedsL
//
// Tests all feeds in the given folder, where the feeds must be named XX.xml where
// XX are consecutive integers (ie 1.xml, 2.xml, etc.).
// -----------------------------------------------------------------------------
//
void CTestHarness::TestFeedsL(const TDesC& aDirPath, TInt aStartIndex, TInt aEndIndex)
    {
    iDirPath = aDirPath.AllocL();
    iFeedIndex = aStartIndex;
    iEndFileIndex = aEndIndex;
    iTestingFeedSeries = TRUE;

    TestNextFeedInSeriesL();
    }


// -----------------------------------------------------------------------------
// CTestHarness::TestNextFeedInSeriesL
//
// Tests a single Feed given the current value of iFeedIndex. See TestFeedsL.
// -----------------------------------------------------------------------------
//
void CTestHarness::TestNextFeedInSeriesL()
    {
    TBuf16<100> path;

    // Test the next Feed.
    if (iFeedIndex <= iEndFileIndex)
        {
        path.Format(_L("%S%d.xml"), iDirPath, iFeedIndex);
        iFeedIndex++;

        TestFeedL(path);
        }
    // Otherwise we're all done.
    else
        {
        _LIT(KDone, "Done!");

        iTestingFeedSeries = FALSE;
        
        // All done...
        iObserver.Completed();
        StatusPane::SetTitleL(KDone());
        }
    }


// -----------------------------------------------------------------------------
// CTestHarness::FeedRequestCompleted
//
// Called when the asynchronous request is complete.
// -----------------------------------------------------------------------------
//
// TODO: Trap the leaving methods.
void CTestHarness::FeedRequestCompleted(TInt aStatus)
    {
    // Close the wait dialog.
    iWaitDialog->Close();

    if (aStatus == KErrNone)
        {
        switch( iOperation )
            {
            case EFetchFeed:
                {
                iFeed.Feed().Debug_PrintInfo( iFeed.Feed().Url(), iLogFile );
                iFeed.Feed().Debug_Print();

                // Extract out the intial item status
                RArray<TInt>  itemIds(20);
                RArray<TItemStatus>  itemStatus(20);
        
                itemIds.Reset();
                itemStatus.Reset();
        
                TRAP_IGNORE(iFeed.Feed().ItemStatusL( itemIds, itemStatus ));
        
                // update folder item for unread count
                TInt     unreadCount = 0;
                for (TInt i = 0; i < itemStatus.Count(); i++)
                    {
                    if ( itemStatus[i] == EUnreadItem || itemStatus[i] == ENewItem )
                        {
                        unreadCount++;
                        }
                    }

                // unread count from folder item should be same as from feed
                FEED_LOG(_L("Feeds"), _L("Feeds_Test.log"), 
                    EFileLoggingModeAppend, _L(""));				
                FEED_LOG(_L("Feeds"), _L("Feeds_Test.log"), 
                    EFileLoggingModeAppend,
                    _L("============================================================================================================="));				

                if( !CheckRootFolderItem() )
                    {
                    return;
                    }

                CFolderItem* item = const_cast<CFolderItem*> (iRootFolder.RootFolderItem().Search( iFeed.Feed().Title() ));

                FEED_LOG2(_L("Feeds"), _L("Feeds_Test.log"), 
                    EFileLoggingModeAppend, 
                    _L("Folder Item: %d originally has unread count: %d"), item->Id(), item->UnreadCount());
                FEED_LOG2(_L("Feeds"), _L("Feeds_Test.log"), 
                    EFileLoggingModeAppend, _L("Feed: %d has unread count: %d"), iFeed.Feed().Id(), unreadCount);

                item->SetUnreadCount( unreadCount );               
                }
                break;

            case EParser:
                {
                iFeed.Feed().Debug_PrintInfo(*iCurrentFeedUrl, iLogFile);
                iFeed.Feed().Debug_Print();

                delete iCurrentFeedUrl;
                iCurrentFeedUrl = NULL;
    
                // Start the next local-feed test.
                if (iTestingFeedSeries == TRUE)
                    {
                    TRAPD(err, TestNextFeedInSeriesL());
                    if (err != KErrNone)
                        {
                        FEED_LOG(_L("Feeds"), _L("Feeds_Errors.log"), 
                                EFileLoggingModeAppend, _L("Warning!!! Failed to start the next test!"));
                        }
                    }
        
                #if 0
                    // Start the next folder-feed test.
                    else if (iTestingFolder == TRUE)
                        {
                        TRAP(err, TestNextFeedInFolderL());
                        if (err != KErrNone)
                            {
                            FEED_LOG(_L("Feeds"), _L("Feeds_Errors.log"), 
                                    EFileLoggingModeAppend, _L("Warning!!! Failed to start the next test!"));
                            }
                        }
                #endif
                }
                break;

            default:
                break;
            }
        }    
    // Otherwise show an error.
    else
        {
        // If the server terminated, reconnect to it.
        if (aStatus == KErrServerTerminated)
            {
            DisconnectFromServer();
            TRAP_IGNORE(ConnectToServerL());
            
            // TODO: Show the FolderView.
            }
            
        ShowServerError(aStatus);

        if( iOperation == EParser )
            {
            TUint8* asciiBuf = new TUint8[iCurrentFeedUrl->Length() + 50];

            if (asciiBuf != NULL)
                {
                TPtr8 asciiPtr(asciiBuf, 0, iCurrentFeedUrl->Length() + 50);
                TPtrC16 ucs2Ptr(*iCurrentFeedUrl);

                asciiPtr.Copy(_L8("Feed Completed["));
                asciiPtr.AppendNum(aStatus);
                asciiPtr.Append(_L8("]: "));
                asciiPtr.Append(ucs2Ptr);
                asciiPtr.Append(_L8("\r\n"));
                asciiPtr.ZeroTerminate();

                iStatusFile.Write(asciiPtr);

                delete asciiBuf;
                }
            }
        }
    }


// -----------------------------------------------------------------------------
// CFeedsViewMgr::NetworkConnectionNeededL
//
// Request to create a network connection.
// -----------------------------------------------------------------------------
//
void CTestHarness::NetworkConnectionNeededL(TInt* aConnectionPtr,
        TInt& aSockSvrHandle, TBool& aNewConn, TApBearerType& aBearerType) 
    {
    iConMgr->SetRequestedAP( 0 );
        
    // Open a connection.
    iConMgr->StartConnectionL(ETrue);                

    // Set the connection, server handler, and bearer type.
    aNewConn = ETrue;
    *aConnectionPtr = (TInt) &iConMgr->Connection();
    aSockSvrHandle = (TInt) iConMgr->SocketServer().Handle();
    aBearerType = iConMgr->CurrentBearerTypeL();
    }


// -----------------------------------------------------------------------------
// CTestHarness::RunTest
// 
// Runs the next test.
// -----------------------------------------------------------------------------
//
TInt CTestHarness::RunTest(TAny* aPtr)
    {
    CTestHarness*  self = static_cast<CTestHarness*>(aPtr);    

	// Start fetching the feed: 2nd param is the folder list ID, 0 is the default folder list ID
    TRAP_IGNORE(self->iFeed.FetchFeedL(*(self->iCurrentFeedUrl), NTFolderListId, EFalse, ETrue));	

    return EFalse;
    }


#if 0
// -----------------------------------------------------------------------------
// CTestHarness::Completed
//
// Called upon completion of the get folder task.
// -----------------------------------------------------------------------------
//
void CTestHarness::Completed(TInt /*aStatusCode*/, CFolderItem* aFolder)
    {
    // Delete the task.
    if (iGetFolderTask != NULL)
        {        
        iGetFolderTask->AutoDelete();
        iGetFolderTask = NULL;
        }        

    if (aFolder != NULL)
        {
        iFolder = aFolder;

        if (iTestingFolder == TRUE)
            {
            TRAPD(err, TestNextFeedInFolderL());
            if (err != KErrNone)
                {
                FEED_LOG(_L("Feeds"), _L("Feeds_Errors.log"), 
                        EFileLoggingModeAppend, _L("Warning!!! Failed to start the next test!"));
                }
            }
        }
    }


// -----------------------------------------------------------------------------
// CTestHarness::TestFolderL
//
// Tests all feeds in the OPML file.
// -----------------------------------------------------------------------------
//
void CTestHarness::TestFolderL(const TDesC& aPath)
    {
    // Start the task
    iGetFolderTask = CGetFolderTask::NewL(aPath, *this);
    iGetFolderTask->StartTaskL();
    iGetFolderTask->EnableAutoDeleteL();

    iTestingFolder = TRUE;
    }


// -----------------------------------------------------------------------------
// CTestHarness::TestNextFeedInFolderL
//
// Tests a single Feed given the current value of iFeedIndex. See TestFolderL.
// -----------------------------------------------------------------------------
//
void CTestHarness::TestNextFeedInFolderL()
    {
    const CFolderItem*  nextItem = NULL;
    const TDesC* url = NULL;

    while (iFeedIndex < iFolder->FolderItemCount())
        {
        // Get the next Feed.
        nextItem = iFolder->FolderItem(iFeedIndex);
        iFeedIndex++;
        
        // Ignore Folders
        if (nextItem->IsFolder() == TRUE)
            {
            continue;
            }

        // Get the Feed's url.
        url = nextItem->SourceUrl();

        // Ignore Items without urls
        if (url == NULL)
            {
            continue;
            }

        // Test the next Feed.
        TestFeedL(*url);
        return;
        }

    // Otherwise we're all done.
    if (iFeedIndex == iFolder->FolderItemCount())
        {
        _LIT(KDone, "Done!");

        iTestingFolder = FALSE;
            
        // All done...
        iObserver.Completed();
        StatusPane::SetTitleL(KDone());
        }
    }
#endif


// -----------------------------------------------------------------------------
// CTestHarness::DebugPrintTablesL
//
// Debug print database tables.
// -----------------------------------------------------------------------------
//
void CTestHarness::DebugPrintTablesL()
    {
    // Import the feeds.
    iFeedsServer.DebugPrintTablesL();

    iOperation = EPrintDBTables;
    }


// -----------------------------------------------------------------------------
// CTestHarness::FetchRootFolderItemL
// -----------------------------------------------------------------------------
//
void CTestHarness::FetchRootFolderItemL()
    {
    // TODO: Start the background indicator.
    iWaitDialog->ShowWaitDialogL( R_FEEDS_OPENING_FEED );

    // Fetch the feeds.
    iRootFolder.FetchRootFolderItemL();

    iOperation = EFetchRootFolderItem;
    }


// -----------------------------------------------------------------------------
// CTestHarness::WatchFolderListL
// -----------------------------------------------------------------------------
//
void CTestHarness::WatchFolderListL()
    {
    iRootFolder.WatchFolderListL();

    iOperation = EWatchFolderList;
    }


// -----------------------------------------------------------------------------
// CTestHarness::ImportOPMLL
//
// Import OPML file.
// -----------------------------------------------------------------------------
//
void CTestHarness::ImportOPMLL( const TDesC& aPath, TBool aClearFolderList )
    {
    // TODO: Start the background indicator.
    iWaitDialog->ShowWaitDialogL( R_FEEDS_OPENING_FEED );

    // Import the feeds.
    iRootFolder.ImportOPMLL( aPath, aClearFolderList );

    iOperation = EImportOPML;
    }


// -----------------------------------------------------------------------------
// CTestHarness::UpdateAllL
// -----------------------------------------------------------------------------
//
void CTestHarness::UpdateAllL()
    {
    // TODO: Start the background indicator.
    iWaitDialog->ShowWaitDialogL( R_FEEDS_UPDATING_ALL_WAIT_DIALOG );

    iRootFolder.UpdateAllL();

    iOperation = EUpdateAll;
    }


// -----------------------------------------------------------------------------
// CTestHarness::AddFolderItemL
// -----------------------------------------------------------------------------
//
void CTestHarness::AddFolderItemL( TBool aIsFolder )
    {
    // TODO: Start the background indicator.
    iWaitDialog->ShowWaitDialogL( R_FEEDS_OPENING_FEED );

    if( !CheckRootFolderItem() )
        {
        return;
        }
        
    if( aIsFolder )
        {
        iRootFolder.AddFolderItemL( KNewTitle(), KNullDesC, ETrue, iRootFolder.RootFolderItem() );
        }
    else
        {
        iRootFolder.AddFolderItemL( KNewTitle(), KUrl(), EFalse, iRootFolder.RootFolderItem() );
        }

    iOperation = EAddFolderItem;
    }


// -----------------------------------------------------------------------------
// CTestHarness::ChangeFolderItemL
// -----------------------------------------------------------------------------
//
void CTestHarness::ChangeFolderItemL()
    {
    const CFolderItem* folderItem = NULL;

    if( !CheckRootFolderItem() )
        {
        return;
        }

    // TODO: Start the background indicator.
    iWaitDialog->ShowWaitDialogL( R_FEEDS_OPENING_FEED );

    TInt count = iRootFolder.RootFolderItem().FolderItemCount();

    if( count != 0 )
        {
        // Copy the folder items under root into a temp array.
        folderItem = iRootFolder.RootFolderItem().FolderItemAt( 0 );
        if( folderItem->IsFolder() )
            {
            iRootFolder.ChangeFolderItemL( *folderItem, KRenamedTitle(), KNullDesC );
            }
        else
            {
            iRootFolder.ChangeFolderItemL( *folderItem, KRenamedTitle(), KChangedUrl );
            }

        iOperation = EChangeFolderItem;
        }
    else
        {
        FolderItemRequestCompleted( KErrNotFound );
        }
    }


// -----------------------------------------------------------------------------
// CTestHarness::DeleteFolderItemsL
// -----------------------------------------------------------------------------
//
void CTestHarness::DeleteFolderItemsL()
    {
    const CFolderItem* folderItem = NULL;

    if( !CheckRootFolderItem() )
        {
        return;
        }

    // TODO: Start the background indicator.
    iWaitDialog->ShowWaitDialogL( R_FEEDS_OPENING_FEED );

    TInt count = iRootFolder.RootFolderItem().FolderItemCount();

    if( count != 0 )
        {
        // Copy the folder items under root into a temp array.
        folderItem = iRootFolder.RootFolderItem().FolderItemAt( 0 );
        iMovedItems.Reset();
        User::LeaveIfError( iMovedItems.Append( folderItem ) );
        iRootFolder.DeleteFolderItemsL( iMovedItems );

        iOperation = EDeleteFolderItems;
        }
    else
        {
        FolderItemRequestCompleted( KErrNotFound );
        }
    }


// -----------------------------------------------------------------------------
// CTestHarness::DeleteAllFolderItemsL
// -----------------------------------------------------------------------------
//
void CTestHarness::DeleteAllFolderItemsL()
    {
    const CFolderItem* folderItem = NULL;

    if( !CheckRootFolderItem() )
        {
        return;
        }

    // TODO: Start the background indicator.
    iWaitDialog->ShowWaitDialogL( R_FEEDS_OPENING_FEED );

    TInt count = iRootFolder.RootFolderItem().FolderItemCount();

    if( count != 0 )
        {
        iMovedItems.Reset();
        // Copy the folder items under root into a temp array.
        for( int i=0; i<count; i++ )
            {
            folderItem = iRootFolder.RootFolderItem().FolderItemAt( i );
            User::LeaveIfError( iMovedItems.Append( folderItem ) );
            }

        iRootFolder.DeleteFolderItemsL( iMovedItems );

        iOperation = EDeleteFolderItems;
        }
    }


// -----------------------------------------------------------------------------
// CTestHarness::MoveFolderItemsToL
//
// Moves the marked items into the target-folder.
// -----------------------------------------------------------------------------
//
void CTestHarness::MoveFolderItemsToL()
    {
    const CFolderItem* folderItem = NULL;
    const CFolderItem* dest = NULL;

    if( !CheckRootFolderItem() )
        {
        return;
        }
        
    TInt count = iRootFolder.RootFolderItem().FolderItemCount();

    iMovedItems.Reset();
    // Copy the folder items under root into a temp array.
    for( int i=0; i<count; i++ )
        {
        folderItem = iRootFolder.RootFolderItem().FolderItemAt( i );
        if( folderItem->IsFolder()  )
            {
            dest = folderItem;
            }
        else
            {
            User::LeaveIfError( iMovedItems.Append( folderItem ) );
            }
        }

    if( dest != NULL && count != 0 )
        {
        // Move the items to the "New Folder".
        iRootFolder.MoveFolderItemsToL( iMovedItems, *dest );

        iOperation = EMoveFolderItemsTo;
        }
    else
        {
        FolderItemRequestCompleted( KErrNotFound );
        }
    }


// -----------------------------------------------------------------------------
// CTestHarness::MoveFolderItemsL
//
// Moves the items to a different position in its parent folder.
// -----------------------------------------------------------------------------
//
void CTestHarness::MoveFolderItemsL()
    {
    const CFolderItem* folderItem = NULL;
    TInt targetIndex;

    if( !CheckRootFolderItem() )
        {
        return;
        }
        
    // Copy the marked items into a temp array.
    TInt count = iRootFolder.RootFolderItem().FolderItemCount();

    // Get the target index.
    targetIndex = count;

    iMovedItems.Reset();
    TInt min = 2 < count ? 2 : count;
    // Copy the 1st two folder items under root into a temp array.
    for( int i=0; i<min; i++ )
        {
        folderItem = iRootFolder.RootFolderItem().FolderItemAt( i );
        User::LeaveIfError( iMovedItems.Append( folderItem ) );

        TInt index = iRootFolder.RootFolderItem().Index( *folderItem );
        if( index != KErrNone )
            {
            FEED_LOG2(_L("Feeds"), _L("Feeds_Test.log"), 
                EFileLoggingModeAppend, _L("Folder item: %d has new index: %d in RootFolder"), iMovedItems[i]->Id(), index);
            }
        else
            {
            FEED_LOG1(_L("Feeds"), _L("Feeds_Test.log"), 
                EFileLoggingModeAppend, _L("Error: Folder item: %d is not found in RootFolder"), iMovedItems[i]->Id());
            }
        }

    // Move the items to the the taget index.
    iRootFolder.MoveFolderItemsL( iMovedItems, targetIndex );

    iOperation = EMoveFolderItems;
    }


// -----------------------------------------------------------------------------
// CTestHarness::UpdateFolderItemsL
// -----------------------------------------------------------------------------
//
void CTestHarness::UpdateFolderItemsL()
    {
    const CFolderItem* folderItem = NULL;

    if( !CheckRootFolderItem() )
        {
        return;
        }
        
    // TODO: Start the background indicator.
    iWaitDialog->ShowWaitDialogL( R_FEEDS_OPENING_FEED );

    iMovedItems.Reset();

    // Copy the feed into a temp array.
    TInt count = iRootFolder.RootFolderItem().FolderItemCount();
    for( int i = 0; i < count; i++ )
        {
        folderItem = iRootFolder.RootFolderItem().FolderItemAt( i );
        // has to be feed
        if( !folderItem->IsFolder() )
            {
            User::LeaveIfError( iMovedItems.Append( folderItem ) );
            }
        }

    if( iMovedItems.Count() != 0 )
        {
        iRootFolder.UpdateFolderItemsL( iMovedItems );

        iOperation = EUpdateFolderItems;
        }
    else
        {
        FolderItemRequestCompleted( KErrNotFound );
        }
    }


// -----------------------------------------------------------------------------
// CTestHarness::FetchFeedL
// -----------------------------------------------------------------------------
//
void CTestHarness::FetchFeedL()
    {
    const CFolderItem* folderItem = NULL;
    TBool found = EFalse;

    if( !CheckRootFolderItem() )
        {
        return;
        }
        
    // TODO: Start the background indicator.
    iWaitDialog->ShowWaitDialogL( R_FEEDS_OPENING_FEED );

    TInt count = iRootFolder.RootFolderItem().FolderItemCount();

    // Copy the folder items under root into a temp array.
    for( int i=0; i<count; i++ )
        {
        folderItem = iRootFolder.RootFolderItem().FolderItemAt( i );

        if( !folderItem->IsFolder() )
            {
            // aForceUpdate, aNoCache is EFalse by default
            iFeed.FetchFeedL( folderItem->FeedId() );

            iOperation = EFetchFeed;
            found = ETrue;
            break;
            }
        }

    if( !found )
        {
        FeedRequestCompleted( KErrNotFound );
        }
    }


// -----------------------------------------------------------------------------
// CTestHarness::UpdateFeedItemStatusL
// -----------------------------------------------------------------------------
//
void CTestHarness::UpdateFeedItemStatusL()
    {
    if( iFeed.HasCurrentFeed() )
        {
        RArray<TInt>         ids(20);
        RArray<TItemStatus>  status(20);
        RArray<TInt>  itemIds(20);
        RArray<TItemStatus>  itemStatus(20);
        TInt     unreadCount = 0;

        ids.Reset();
        status.Reset();
        itemIds.Reset();
        itemStatus.Reset();

        CleanupClosePushL(ids);
        CleanupClosePushL(status);
        CleanupClosePushL(itemIds);
        CleanupClosePushL(itemStatus);

        // Extract out the intial item status
        iFeed.Feed().ItemStatusL( itemIds, itemStatus );
    
        if (itemStatus.Count() != 0)
            {
            FEED_LOG(_L("Feeds"), _L("Feeds_Test.log"), 
                EFileLoggingModeAppend, _L(""));				
            FEED_LOG(_L("Feeds"), _L("Feeds_Test.log"), 
                EFileLoggingModeAppend,
                _L("============================================================================================================="));				

            for (TInt i = 0; i < itemStatus.Count(); i++)
                {
                if ( itemStatus[i] == EUnreadItem || itemStatus[i] == ENewItem )
                    {
                    unreadCount++;
                    }
                }
    
            // Build new status arrays that only contains items that changed.
            TInt counter = 0;
            for (TInt i = 0; i < itemStatus.Count(); i++)
                {
                if( itemStatus[i] == ENewItem || itemStatus[i] == EUnreadItem )
                    {
                    itemStatus[i] = EReadItem;

                    ids.AppendL(itemIds[i]);
                    status.AppendL(itemStatus[i]);

                    FEED_LOG1(_L("Feeds"), _L("Feeds_Test.log"), 
                        EFileLoggingModeAppend, _L("Article: %d changed status to ERead"), itemIds[i]);

                    // In current UI, status can only change from New -> Read, or Unread -> Read
                    if ( itemStatus[i] == EReadItem )
                        {
                        unreadCount--;
                        }

                    // only change 3 items
                    if( counter == 2 )
                        {
                        break;
                        }
                    counter++;
                    }
                }    

            // Write the item status out to the server.
            iFeed.UpdateFeedItemStatusL(ids, status, unreadCount);

            iOperation = EFetchFeed;

            FEED_LOG2(_L("Feeds"), _L("Feeds_Test.log"), 
                EFileLoggingModeAppend, _L("Feed: %d has updated unread count: %d"), iFeed.Feed().Id(), unreadCount);
            }        

        CleanupStack::PopAndDestroy(/*itemStatus*/);
        CleanupStack::PopAndDestroy(/*itemIds*/);
        CleanupStack::PopAndDestroy(/*status*/);
        CleanupStack::PopAndDestroy(/*ids*/);
        }
    else
        {
        FeedRequestCompleted( KErrNotFound );
        }
    }


// -----------------------------------------------------------------------------
// CTestHarness::UpdateSettingsL()
// -----------------------------------------------------------------------------
//
void CTestHarness::UpdateSettingsL( TBool aOn )
    {
    TFeedsServerSettings settings;
    TInt freq = 0;
    TUint32 autoUpdatingAP( 0 );
    TBool isAutoUpdating = EFalse;

    // turn on
	if( aOn )
		{
		freq = 2;
		isAutoUpdating = ETrue;
        autoUpdatingAP = 6;
		}
	// else: turn off

    settings.SetAutoUpdate( isAutoUpdating );
    settings.SetAutoUpdateFreq( freq );
    settings.SetAutoUpdateAP( autoUpdatingAP );

    // Pass the updated settings to the server.
    iFeedsServer.UpdateSettingsL( settings );

    FEED_LOG3(_L("Feeds"), _L("Feeds_Test.log"), 
        EFileLoggingModeAppend, _L("Set: autoUpdate: %d, freq: %d, AP: %d"), isAutoUpdating, freq, autoUpdatingAP);
    }


// -----------------------------------------------------------------------------
// CTestHarness::FetchServerSettingsSyncL
//
// Returns Feeds Server related settings synchronously.
// -----------------------------------------------------------------------------
//
void CTestHarness::FetchServerSettingsSyncL()
    {
    const TFeedsServerSettings settings = iFeedsServer.FetchServerSettingsSyncL();
    TBool isAutoUpdating = settings.AutoUpdate();
    TInt freq = settings.AutoUpdateFreq();
    TInt autoUpdatingAP = settings.AutoUpdateAP();

    FEED_LOG3(_L("Feeds"), _L("Feeds_Test.log"), 
        EFileLoggingModeAppend, _L("FetchServerSettingsSyncL: autoUpdate: %d, freq: %d, AP: %d"), isAutoUpdating, freq, autoUpdatingAP);
    }


// -----------------------------------------------------------------------------
// CTestHarness::FetchServerSettingsL
//
// Returns Feeds Server related settings asynchronously.
// -----------------------------------------------------------------------------
//
void CTestHarness::FetchServerSettingsL()
    {
    iFeedsServer.FetchServerSettingsL();

    FEED_LOG(_L("Feeds"), _L("Feeds_Test.log"), 
        EFileLoggingModeAppend, _L("Called FetchServerSettingsL."));
    }


// -----------------------------------------------------------------------------
// CTestHarness::SettingsChanged
//
// Notifies from the feeds server that the settings have changed.
// -----------------------------------------------------------------------------
//
void CTestHarness::SettingsChanged( TInt aStatus )
    {
    if( aStatus == KErrNone )
        {
        const TFeedsServerSettings settings = iFeedsServer.Settings();
        TBool isAutoUpdating = settings.AutoUpdate();
        TInt freq = settings.AutoUpdateFreq();
        TInt autoUpdatingAP = settings.AutoUpdateAP();

        FEED_LOG3(_L("Feeds"), _L("Feeds_Test.log"), 
            EFileLoggingModeAppend, _L("SettingsChanged: autoUpdate: %d, freq: %d, AP: %d"), isAutoUpdating, freq, autoUpdatingAP);
        }
    else
        {
        FEED_LOG1(_L("Feeds"), _L("Feeds_Test.log"), 
            EFileLoggingModeAppend, _L("SettingsChanged Error: %d"), aStatus);
        }
    }
    

// -----------------------------------------------------------------------------
// CTestHarness::DisconnectL
//
// Disconnect mannual update connection.
// -----------------------------------------------------------------------------
//
void CTestHarness::DisconnectL()
    {
	iFeedsServer.DisconnectManualUpdateConnectionL();

    FEED_LOG(_L("Feeds"), _L("Feeds_Test.log"), EFileLoggingModeAppend, _L("Disconnected"));
	}


// -----------------------------------------------------------------------------
// CTestHarness::ConnectToServerL
//
// Connect to the server.
// -----------------------------------------------------------------------------
//
void CTestHarness::ConnectToServerL()
    {
    if (!iIsConnected)
        {
        User::LeaveIfError( iFeedsServer.Connect() );
        User::LeaveIfError( iRootFolder.Open(iFeedsServer) );
        User::LeaveIfError( iFeed.Open(iFeedsServer) );
           
        iIsConnected = ETrue;
        }
    }


// -----------------------------------------------------------------------------
// CTestHarness::DisconnectFromServer
//
// Disconnect from the server.
// -----------------------------------------------------------------------------
//
void CTestHarness::DisconnectFromServer()
    {
    iFeed.Close();
    iRootFolder.Close();
    iFeedsServer.Close();

    iIsConnected = EFalse;
    }


// -----------------------------------------------------------------------------
// CTestHarness::FolderItemRequestCompleted
//
// Called when the asynchronous request is complete.
// -----------------------------------------------------------------------------
//
// TODO: Trap the leaving methods.
void CTestHarness::FolderItemRequestCompleted(TInt aStatus)
    {
    // Close the wait dialog.
    iWaitDialog->Close();

    // Show the view if everything went ok.
    if (aStatus == KErrNone)
        {
        // Set the updated folder
        TInt count = iRootFolder.RootFolderItem().FolderItemCount();

        FEED_LOG(_L("Feeds"), _L("Feeds_Test.log"), 
            EFileLoggingModeAppend, _L(""));				
        FEED_LOG(_L("Feeds"), _L("Feeds_Test.log"), 
            EFileLoggingModeAppend,
            _L("============================================================================================================="));				
        FEED_LOG1(_L("Feeds"), _L("Feeds_Test.log"), 
            EFileLoggingModeAppend, _L("\tFolderItems: %d"), count);
        FEED_LOG(_L("Feeds"), _L("Feeds_Test.log"), 
            EFileLoggingModeAppend,
            _L("============================================================================================================="));				

        for( int i=0; i<count; i++ )
            {
            const CFolderItem* folderItem = iRootFolder.RootFolderItem().FolderItemAt( i );
            Debug_Print( *folderItem );
            }

        switch( iOperation )
            {
            // the following case need to check the folder list table print to verify the result
            // case EMoveFolderItemsTo:
            // look for new parent ID in the table
            // case EMoveFolderItems:
            // look for new sibling ID in the table
                
            default:
                break;
            }
        }
    
    // Otherwise show an error.
    else
        {
        // If the server terminated, reconnect to it.
        if (aStatus == KErrServerTerminated)
            {
            DisconnectFromServer();
            TRAP_IGNORE(ConnectToServerL());
            
            // TODO: Show an empty folder list and display a dialog to ask 
            //       to refresh the folder list.
            }
            
        ShowServerError(aStatus);
        }        
    }


// -----------------------------------------------------------------------------
// CTestHarness::ShowServerError
//
// Show a server error.
// -----------------------------------------------------------------------------
//
void CTestHarness::ShowServerError(TInt aStatus)
    {
    // Determine which label to use.
    switch(aStatus)
        {
        case KErrNoMemory:
            FEED_LOG(_L("Feeds"), _L("Feeds_Errors.log"), 
                    EFileLoggingModeAppend, _L("CTestHarness KErrNoMemory"));
            break;

        case KErrCorrupt:
            FEED_LOG(_L("Feeds"), _L("Feeds_Errors.log"), 
                    EFileLoggingModeAppend, _L("CTestHarness KErrCorrupt"));
            break;

		case KErrNotSupported:
            FEED_LOG(_L("Feeds"), _L("Feeds_Errors.log"), 
                    EFileLoggingModeAppend, _L("CTestHarness KErrNotSupported"));
            break;

		case KErrTimedOut:
            FEED_LOG(_L("Feeds"), _L("Feeds_Errors.log"), 
                    EFileLoggingModeAppend, _L("CTestHarness KErrTimedOut"));
			break;
			
        case -KErrNotSupported:
            // Show the special http not supported on WINSCW error
            FEED_LOG(_L("Feeds"), _L("Feeds_Errors.log"), 
                    EFileLoggingModeAppend, _L("CTestHarness KErrNotSupported"));
            return;

		case KErrAlreadyExists:
            FEED_LOG(_L("Feeds"), _L("Feeds_Errors.log"), 
                    EFileLoggingModeAppend, _L("CTestHarness KErrAlreadyExists"));
			break;			

		case KErrNotFound:
            FEED_LOG(_L("Feeds"), _L("Feeds_Errors.log"), 
                    EFileLoggingModeAppend, _L("CTestHarness KErrNotFound"));
			break;			
            
        default:
            FEED_LOG1(_L("Feeds"), _L("Feeds_Errors.log"), 
                    EFileLoggingModeAppend, _L("CTestHarness err [%d]"), aStatus);
            break;
        }
    }


// -----------------------------------------------------------------------------
// CTestHarness::Debug_Print
//
// Prints the folder item to the log file.
// -----------------------------------------------------------------------------
//
void CTestHarness::Debug_Print( const CFolderItem& aFolderItem ) const
    {
    // folder or feed
    if( aFolderItem.IsFolder() )
        {
        FEED_LOG(_L("Feeds"), _L("Feeds_Test.log"), 
                EFileLoggingModeAppend, _L("folder"));
        }
    else
        {
        FEED_LOG(_L("Feeds"), _L("Feeds_Test.log"), 
                EFileLoggingModeAppend, _L("feed"));
        }

    // folder list ID
    FEED_LOG1(_L("Feeds"), _L("Feeds_Test.log"), 
        EFileLoggingModeAppend, _L("\t\tFolderListId: %d"), aFolderItem.FolderListId());

    // folder item ID
    FEED_LOG1(_L("Feeds"), _L("Feeds_Test.log"), 
        EFileLoggingModeAppend, _L("\t\tFolderItemId: %d"), aFolderItem.Id());

    // parent
    FEED_LOG1(_L("Feeds"), _L("Feeds_Test.log"), 
        EFileLoggingModeAppend, _L("\t\tParent: %d"), aFolderItem.Parent());

    // title
    if (aFolderItem.Name().Length() > 0)
        {
        FEED_LOG1(_L("Feeds"), _L("Feeds_Test.log"), 
            EFileLoggingModeAppend, _L("\t\tTitle: %S"), &aFolderItem.Name());
        }

    if( !aFolderItem.IsFolder() )
        {
        // url
        if (aFolderItem.SourceUrl().Length() > 0)
            {
            FEED_LOG1(_L("Feeds"), _L("Feeds_Test.log"), 
                EFileLoggingModeAppend, _L("\t\tLink: %S"), &aFolderItem.SourceUrl());
            }

        // unread count
        FEED_LOG1(_L("Feeds"), _L("Feeds_Test.log"), 
            EFileLoggingModeAppend, _L("\t\tUnreadCount: %d"), aFolderItem.UnreadCount());

        // article titles
        TInt count = aFolderItem.MiniItemsCount();
        if (count > 0)
            {
            for( int i=0; i<count; i++ )
                {
                const CItemMini* miniItem = aFolderItem.MiniItemAt( i );
                TPtrC title( KNullDesC );
                title.Set( miniItem->Title() );
                FEED_LOG1( _L("Feeds"), _L("Feeds_Test.log"), 
                    EFileLoggingModeAppend, _L("\t\tArticleTitle: %S"), &title );
                FEED_LOG1( _L("Feeds"), _L("Feeds_Test.log"), 
                    EFileLoggingModeAppend, _L("\t\tArticleId: %d"), miniItem->Id() );
                }
            }
        }
    }


// -----------------------------------------------------------------------------
// CTestHarness::DialogDismissedL
//
// Called when the user presses the cancel button.
// -----------------------------------------------------------------------------
//
void CTestHarness::DialogDismissedL()
    {
    // Cancel all activities that can be cancelled.
    iFeedsServer.CancelAllL();
    }


// -----------------------------------------------------------------------------
// CTestHarness::CheckRootFolderItem
// -----------------------------------------------------------------------------
//
TBool CTestHarness::CheckRootFolderItem()
    {
    if( iRootFolder.HasRootFolderItem() )
        {
        return ETrue;
        }
    else
        {
        FolderItemRequestCompleted( KErrNotFound );

        return EFalse;
        }
    }
