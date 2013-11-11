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
* Description:  The client side connection to the root-folder on the FeedsSever.
*
*/


#include "FeedsServerClient.h"
#include "FeedsServerFolderItem.h"
#include "FolderItemRequestHandlers.h"
#include "FeedRequestHandlers.h"
#include "ClientRequestHandlers.h"

#include "Logger.h"


// -----------------------------------------------------------------------------
// RFolderItem::RFolderItem
//
// C++ default constructor can NOT contain any code that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C RFolderItem::RFolderItem( MFolderItemObserver& aObserver, TInt aFolderListId, TBool aItemTitleNeed ):
        iObserver(aObserver), iFolderItem(NULL), iFolderListId(aFolderListId), iItemTitleNeed(aItemTitleNeed), 
        iFeedsServer( NULL )
    {
    CFolderItemRequestHandler *folderReqHandler = NULL;
    TRAP_IGNORE(
    {
    folderReqHandler =   CFolderItemRequestHandler::NewL();
    CleanupStack::PushL(folderReqHandler);
    iActiveRequests.AppendL(folderReqHandler);
    CleanupStack::Pop(folderReqHandler);
    });
    }

// -----------------------------------------------------------------------------
// RFolderItem::Open
//
// Opens a session to the given folder on the server.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RFolderItem::Open(RFeedsServer &aFeedsServer)
    {

    iFeedsServer = &aFeedsServer;
    RFeedsInterface *feedsInterface = iFeedsServer->iActiveRequests[0]->GetFeedInterface();
    iActiveRequests[0]->SetFeedInterface(feedsInterface);
    iFeedsServer->SetServerFolderItem(*this);
    return KErrNone;   
    }

// -----------------------------------------------------------------------------
// RFolderItem::Close
//
// Closes the connection to the server.
// -----------------------------------------------------------------------------
//
EXPORT_C void RFolderItem::Close()
    {    
    iFeedsServer = NULL;
    iActiveRequests.ResetAndDestroy();    
    delete iFolderItem;
    iFolderItem = NULL;
    }


// -----------------------------------------------------------------------------
// RFolderItem::RequestHandlerCompletedL
//
// Notifies the observer that the request is completed.
// -----------------------------------------------------------------------------
//
void RFolderItem::RequestHandlerCompletedL(CTransaction& aTrans, 
        TInt aStatus)
    {
    switch (aTrans.Type())
        {
        case CTransaction::EFetchRootFolderItem:
            if(aStatus == KErrNone)
               {
               delete iFolderItem;
               iFolderItem = NULL;

               iFolderItem = CFolderItem::NewL(iFolderListId, iActiveRequests[0]->GetFeedInterface()->RootFolder());
               // Notify the observer that the request has completed.
               iObserver.FolderItemRequestCompleted(aStatus, (CRequestHandler::TRequestHandlerType)aTrans.Type());
               }
            break;

        case CTransaction::EWatchForChanges:
            // Only notify the user if an error occurred.
            if (aStatus != KErrNone)
                {
                iObserver.FolderItemRequestCompleted(aStatus, (CRequestHandler::TRequestHandlerType)aTrans.Type());
                }
            break;

        // no need to notify observer now, server will notify us later
        // assuming FetchRootFolder or WatchFolderList has been called before
        case CTransaction::EAddFolderItem:
        case CTransaction::EDeleteFolderItem:
        case CTransaction::EChangeFolderItem:
        case CTransaction::EMoveFolderItem:
        case CTransaction::EMoveFolderItemTo:
        case CTransaction::EUpdateFolderItem:
        break;

        case CTransaction::EImportOPML:
        case CTransaction::EExportOPML:
            iObserver.FolderItemRequestCompleted(aStatus, (CRequestHandler::TRequestHandlerType)aTrans.Type());
            break;

        default:
            // Notify the observer that the request has completed.
            iObserver.FolderItemRequestCompleted(aStatus, (CRequestHandler::TRequestHandlerType)aTrans.Type());
            break;
        }
    }

// -----------------------------------------------------------------------------
// RFolderItem::NetworkConnectionNeededL()
// -----------------------------------------------------------------------------
//
void RFolderItem::NetworkConnectionNeededL( TInt* aConnectionPtr, TInt& aSockSvrHandle,
                                            TBool& aNewConn, TApBearerType& aBearerType )
    {
    iActiveRequests[0]->SetOperationRequested( EFalse);//reset the flag
    iObserver.NetworkConnectionNeededL( aConnectionPtr, aSockSvrHandle, aNewConn, aBearerType );
    }

// -----------------------------------------------------------------------------
// RFolderItem::FetchRootFolderItemL
//
// Fetches the root FolderItem.  FetchRootFolderItemCompleted is called upon completion.
// -----------------------------------------------------------------------------
//
EXPORT_C void RFolderItem::FetchRootFolderItemL()
    {
    iActiveRequests[0]->GetFeedInterface()->GetRootFolderL(); 
    }

// -----------------------------------------------------------------------------
// RFolderItem::AddFolderItemL
//
// Request the feeds server to add a new folder item.
// -----------------------------------------------------------------------------
//
EXPORT_C void RFolderItem::AddFolderItemL(const TDesC& aName, const TDesC& aUrl,
        TBool aIsFolder, const CFolderItem& aParent)
    {
    CFeedsMap* map = CFeedsMap::NewL();
    CleanupStack::PushL(map);

    map->SetStringAttribute(EFeedAttributeTitle, aName);
    map->SetStringAttribute(EFeedAttributeLink, aUrl);

    if(aIsFolder)
        {
        iActiveRequests[0]->GetFeedInterface()->AddL(EFolder, *map, *(aParent.iFeedsEntity));	
        }
    else
        {
        iActiveRequests[0]->GetFeedInterface()->AddL(EFeed, *map, *(aParent.iFeedsEntity)); 
        }
    CleanupStack::PopAndDestroy(map);
    }

// -----------------------------------------------------------------------------
// RFolderItem::ChangeFolderItemL
//
// Request the feeds server to change the folder item.  If this is a folder then
// KNullDesC should be passed to the aUrl argument.
// -----------------------------------------------------------------------------
//
EXPORT_C void RFolderItem::ChangeFolderItemL(const CFolderItem& aFolderItem, 
        const TDesC& aTitle, const TDesC& aUrl)
    {
    CFeedsMap* map = CFeedsMap::NewL();

    map->SetStringAttribute(EFeedAttributeTitle,aTitle);
    map->SetStringAttribute(EFeedAttributeLink,aUrl);

    iActiveRequests[0]->GetFeedInterface()->SetAttributeL(*(aFolderItem.iFeedsEntity), *map);

    delete map;
    }

// -----------------------------------------------------------------------------
// RFolderItem::DeleteFolderItemsL
//
// Request the feeds server to delete the folder items.
// -----------------------------------------------------------------------------
//
EXPORT_C void RFolderItem::DeleteFolderItemsL(RPointerArray<const CFolderItem>& 
        aFolderItems)
    {
    RPointerArray<const CFeedsEntity> feedsEntities;
    CleanupClosePushL(feedsEntities);

    for(TInt index = 0; index <aFolderItems.Count(); index ++)
        {
        feedsEntities.Append(aFolderItems[index]->iFeedsEntity);
        }
    
    iActiveRequests[0]->GetFeedInterface()->DeleteL(feedsEntities);
    // Clean up
    CleanupStack::PopAndDestroy(/*feedsEntities*/);
    }

// -----------------------------------------------------------------------------
// RFolderItem::ExportFolderItemsL
//
// Request the feeds server to export the folder items.
// -----------------------------------------------------------------------------
//
EXPORT_C void RFolderItem::ExportFolderItemsL(RPointerArray<const CFolderItem>& 
        aFolderItems, const TDesC &aExportFileName)
    {
    if(aExportFileName.Length() <=0)
        {
        User::Leave(KErrArgument);
        }

    if(aFolderItems.Count() <=0)
        {
        User::Leave(KErrArgument);
        }

    RPointerArray<const CFeedsEntity> feedsEntities;
    CleanupClosePushL(feedsEntities);

    for(TInt index = 0; index <aFolderItems.Count(); index ++)
        {
        feedsEntities.Append(aFolderItems[index]->iFeedsEntity);
        }
    
    iActiveRequests[0]->GetFeedInterface()->ExportFoldersL(feedsEntities, aExportFileName);
    // Clean up
    CleanupStack::PopAndDestroy(/*feedsEntities*/);
    }

// -----------------------------------------------------------------------------
// RFolderItem::MoveFolderItemsToL
//
// Request the feeds server to move the folder items to a different parent. 
// -----------------------------------------------------------------------------
//
EXPORT_C void RFolderItem::MoveFolderItemsToL(RPointerArray<const CFolderItem>& 
        aFolderItems, const CFolderItem& aParent)
    {

    if (aFolderItems.Count() > 0)
        {        
        RPointerArray<const CFeedsEntity> feedsEntities;
        CleanupClosePushL(feedsEntities);

        for(TInt index = 0; index <aFolderItems.Count(); index ++)
           {
    	   feedsEntities.Append(aFolderItems[index]->iFeedsEntity);
           }
               
        iActiveRequests[0]->GetFeedInterface()->MoveToFolderL(feedsEntities, *(aParent.iFeedsEntity));
        // Clean up
        CleanupStack::PopAndDestroy(/*feedsEntities*/);
        }
    }

// -----------------------------------------------------------------------------
// RFolderItem::MoveFolderItemsL
//
// Request the feeds server to move the folder items to a different index. 
// -----------------------------------------------------------------------------
//
EXPORT_C void RFolderItem::MoveFolderItemsL(RPointerArray<const CFolderItem>& 
        aFolderItems, TInt aIndex)
    {
    if (aFolderItems.Count() > 0)
        {        
        RPointerArray<const CFeedsEntity> feedsEntities;
        CleanupClosePushL(feedsEntities);

        for(TInt index = 0; index <aFolderItems.Count(); index ++)
           {
    	   feedsEntities.Append(aFolderItems[index]->iFeedsEntity);
           }

        iActiveRequests[0]->GetFeedInterface()->MoveL(feedsEntities, aIndex);
        // Clean up
        CleanupStack::PopAndDestroy(/*feedsEntities*/);
       }
    }

// -----------------------------------------------------------------------------
// RFolderItem::UpdateFolderItemsL
//
// Request the feeds server to update the folder items (refresh them from the web). 
// -----------------------------------------------------------------------------
//
EXPORT_C void RFolderItem::UpdateFolderItemsL(const RPointerArray<const CFolderItem>& 
        aFolderItems)
    {
        if (aFolderItems.Count() > 0)
        {  
                
        iActiveRequests[0]->SetOperationRequested( ETrue);
        RPointerArray<const CFeedsEntity> feedsEntities;

        CleanupClosePushL(feedsEntities);

        for(TInt index = 0; index <aFolderItems.Count(); index ++)
           {
    	   feedsEntities.Append(aFolderItems[index]->iFeedsEntity);
           }
        iActiveRequests[0]->GetFeedInterface()->UpdateL(feedsEntities);

        // Clean up
        CleanupStack::PopAndDestroy(/*feedsEntities*/);
        }
    }

// -----------------------------------------------------------------------------
// RFolderItem::UpdateAllL
//
// Request the feeds server to update all of the folder items (refresh them 
// from the web) associated with current given folder list.
// -----------------------------------------------------------------------------
//
EXPORT_C void RFolderItem::UpdateAllL()
    {
    RPointerArray<const CFeedsEntity> feedsEntities;

    CleanupClosePushL(feedsEntities);

    iActiveRequests[0]->SetOperationRequested( ETrue);
    feedsEntities.Append((iActiveRequests[0]->GetFeedInterface()->RootFolder()));
    iActiveRequests[0]->GetFeedInterface()->UpdateL(feedsEntities);

    // Clean up
    CleanupStack::PopAndDestroy(/*feedsEntities*/);
    }

// -----------------------------------------------------------------------------
// RFolderItem::WatchFolderListL
//
// Sets up a notifier to execute when the folder list changes.
// -----------------------------------------------------------------------------
//
EXPORT_C void RFolderItem::WatchFolderListL()
    {
    iActiveRequests[0]->GetFeedInterface()->WatchFolderListL();
    }


// -----------------------------------------------------------------------------
// RFolderItem::HasRootFolderItem
//
// Returns ETrue if it has an available root folder.
// -----------------------------------------------------------------------------
//
EXPORT_C TBool RFolderItem::HasRootFolderItem() const
    {
    return (iFolderItem != NULL);
    }


// -----------------------------------------------------------------------------
// RFolderItem::RootFolderItem
//
// Once fetched, it returns the CFolderItem.  This method panics the client if it is 
// called before the folder item is available.
// -----------------------------------------------------------------------------
//
EXPORT_C const CFolderItem& RFolderItem::RootFolderItem() const
    {
    return *iFolderItem;
    }


// -----------------------------------------------------------------------------
// RFolderItem::ImportOPMLL
//
// Import OPML file passed from the client.
// -----------------------------------------------------------------------------
//
EXPORT_C void RFolderItem::ImportOPMLL( const TDesC& aPath, TBool /*aClearFolderList=EFalse*/ )
    {
    iActiveRequests[0]->GetFeedInterface()->ImportOPMLL(aPath);
    }
