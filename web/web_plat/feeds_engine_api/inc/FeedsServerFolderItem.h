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
* Description:  The client side connection to a folder on the FeedsSever.
*
*/


#ifndef FEEDS_SERVER_FOLDER_ITEM_H
#define FEEDS_SERVER_FOLDER_ITEM_H

#warning The Feeds Engine API will be removed wk25. Please see http://wikis.in.nokia.com/Browser/APIMigration for more information

// INCLUDES
#include <e32base.h>
#include <e32std.h>

#include <FeedsServerRequestHandler.h>
#include "FeedsEntity.h"
#include "LeakTracker.h"

#include "Transaction.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION
class CItemMini;
class CFolderItem;
class CPackedFolder;
class RFeedsServer;
class CFolderItemRequestHandler;
class CFolderItemRequestHandler;
/**
*  The RFolderItem observer interface.
*
*  \b Library: FeedsEngine.lib
*
*  @since 3.1
*/
class MFolderItemObserver : public MConnectionObserver
    {
    public:  // New Methods
        /**
        * Notifies the observer that the request is completed.
        *
        * @since 3.1
        * @param aStatus The result code of the request.
        * @return void.
        */
        virtual void FolderItemRequestCompleted(TInt aStatus, CRequestHandler::TRequestHandlerType aRequestType=CRequestHandler::ENone) = 0;

        /**
        * Notifies the observer that the FolderItem has changed.  The client
        * should request an updated copy from the server.
        *
        * @since 3.1
        * @return void.
        */
        virtual void FolderItemChanged() = 0;
    };

/**
*  The client side interface to a server resident FolderItem.
*
*  \b Library: FeedsEngine.lib
*
*  @since 3.1
*/
class RFolderItem: public RSubSessionBase, public MRequestHandlerObserver
    {
    public:
        /**
        * C++ default constructor.
        */
        IMPORT_C RFolderItem( MFolderItemObserver& aObserver, TInt aFolderListId = 0, TBool aItemTitleNeed = EFalse );
        
        
        public:  // From MRequestHandlerObserver
        
        virtual void SendAsyncCommand(TInt /*aFunction*/, const TIpcArgs& /*aArgs*/, 
                TRequestStatus& /*aStatus*/) const { } ;

        /**
        * Notifies the observer that the request is completed.
        *
        * @since 3.1
        * @param aRequestHandler The request handler.
        * @param aStatus The result code of the request.
        * @return void.
        */
        virtual void RequestHandlerCompleted(CRequestHandler& /*aRequestHandler*/,
                TInt /*aStatus*/) { } ;

        /**
        *
        */
        virtual void CancelAllCommandsL() { } ;

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
        virtual void NetworkConnectionNeededL(TInt* aConnectionPtr, TInt& aSockSvrHandle,
                TBool& aNewConn, TApBearerType& aBearerType);

    public:  // New methods

        /**
        * Notifies the observer that the request is completed.
        *
        * @since 3.1
        * @param aRequestHandler The request handler.
        * @param aStatus The result code of the request.
        * @return void.
        */
        void RequestHandlerCompletedL(CTransaction& aTrans,
                TInt aStatus);

        /**
        * Opens a session to the root FolderItem on the server.
        *
        * @since 3.1
        * @param aFeedsServer The shared connection to the feeds server.
        * @return Error/success code
        */
        IMPORT_C TInt Open(RFeedsServer &aFeedsServer);

        /**
        * Closes the session.
        *
        * @since 3.1
        * @return void
        */
        IMPORT_C void Close();

        /**
        * Fetches the root FolderItem.  FetchRootFolderItemCompleted
        * is called upon completion.
        *
        * @since 3.1
        * @return void
        */
        IMPORT_C void FetchRootFolderItemL();

        /**
        * Request the feeds server to add a new folder item.
        *
        * @since 3.1
        * @param aName The name of the item.
        * @param aUrl The url of the item.
        * @param aIsFolder True if this is a folder.
        * @param aParent The parent folder.
        * @return void
        */
        IMPORT_C void AddFolderItemL(const TDesC& aName, const TDesC& aUrl,
                TBool aIsFolder, const CFolderItem& aParent);

        /**
        * Request the feeds server to change the folder item.  If this 
        * is a folder then KNullDesC should be passed to the aUrl argument.
        *
        * @since 3.1
        * @param aFolderItem The item to update.
        * @param aTitle The item's title..
        * @param aUrl The item's url or KNullDesC.
        * @return void
        */
        IMPORT_C void ChangeFolderItemL(const CFolderItem& aFolderItem, 
                const TDesC& aTitle, const TDesC& aUrl);

        /**
        * Request the feeds server to delete the folder items 
        *
        * @since 3.1
        * @param aFolderItem The folder item that's being deleted.
        * @return void
        */
        IMPORT_C void DeleteFolderItemsL(RPointerArray<const CFolderItem>& aFolderItems);

        /**
        * Request the feeds server to move the folder item to a different
        * parent. 
        *
        * @since 3.1
        * @param aFolderItem The folder item that's being moved.
        * @param aParent The folder item's new parent.
        * @return void
        */
        IMPORT_C void MoveFolderItemsToL(RPointerArray<const CFolderItem>& aFolderItems,
                const CFolderItem& aParent);

        /**
        * Request the feeds server to move the folder item to a different
        * index. 
        *
        * @since 3.1
        * @param aFolderItem The folder item that's being moved.
        * @param aIndex The new index of the folder item (within its parent).
        * @return void
        */
        IMPORT_C void MoveFolderItemsL(RPointerArray<const CFolderItem>& aFolderItems,
                TInt aIndex);

        /**
        * Request the feeds server to update the folder items (refresh them from the web).  
        *
        * @since 3.1
        * @param aFolderItems The folder items being updated, has to be feed, not folder
        * @return void
        */
        IMPORT_C void UpdateFolderItemsL(const RPointerArray<const CFolderItem>& aFolderItems);

        /**
        * Request the feeds server to update all the folder items (refresh them from 
        * the web) in a current folder-list.  
        *
        * @since 3.1
        * @return void
        */
        IMPORT_C void UpdateAllL();

        /**
        * Sets up a notifier to execute when the folder list changes.
        *
        * @since 3.1
        * @param aFolderListId The folder-list's id.
        * @return void
        */
        IMPORT_C void WatchFolderListL();

        /**
        * Returns ETrue if it has an available root folder.
        *
        * @since 3.1
        * @return Returns ETrue if it has an available root folder..
        */
        IMPORT_C TBool HasRootFolderItem() const;

        /**
        * Once fetched, it returns the root CFolderItem.  This method panics the
        * client if it is called before the folder item is available.
        *
        * @since 3.1
        * @return The root CFolderItem.
        */
        IMPORT_C const CFolderItem& RootFolderItem() const;

        /**
        * Import OPML file passed from the client. 
        *
        * @since 3.2
        * @param aPath The OPML file path.
        * @param aClearFolderList Flag indicating to or not to clear entries under this Folder List before load OPML.
        * @return void.
        */
        IMPORT_C void ImportOPMLL( const TDesC& aPath, TBool aClearFolderList = EFalse );


        /**
        * Request the feeds server to export the folder items 
        *
        * @since 3.2
        * @param aFolderItem The folder item that's being exported.
        * @param aExportFileName The OPML file name.
        * @return void
        */
        IMPORT_C void ExportFolderItemsL(RPointerArray<const CFolderItem>& aFolderItems, const TDesC &aExportFileName);

    public:
        friend class RFeedsServer;

    private:  // Data
        MFolderItemObserver&            iObserver;
        CFolderItem*                    iFolderItem;
        RPointerArray<CFolderItemRequestHandler>  iActiveRequests;
        TInt                            iFolderListId;
        TBool                           iItemTitleNeed;
        RFeedsServer*                   iFeedsServer;
     
    };


/**
*  FolderItem either contains a list of FolderItems or contains
*  information about a feed.
*
*  \b Library: FeedsEngine.lib
*
*  @since 3.0
*/
class CFolderItem: public CBase
    {
    public:  // Destructor
        /**
        * Destructor.
        */        
        IMPORT_C virtual ~CFolderItem();

    public: // New methods
        /**
        * Returns the folder-list-id associated with this item.
        *
        * @since 3.0
        * @return the folder-list-id associated with this item.
        */
        IMPORT_C TInt FolderListId(void) const;

        /**
        * Returns the parent folder or NULL if this is the root folder.
        *
        * @since 3.0
        * @return The parent folder.
        */
        IMPORT_C const CFolderItem* Parent(void) const;

        /**
        * Returns true if this is a folder.
        *
        * @since 3.0
        * @return true or false.
        */
        IMPORT_C TBool IsFolder(void) const;

        /**
        * Returns the name of the item.
        *
        * @since 3.0
        * @return The item's name.
        */
        IMPORT_C const TDesC& Name(void) const;

        /**
        * Returns the source url of the item.
        *
        * @since 3.0
        * @return The item's source url.
        */
        IMPORT_C const TDesC& SourceUrl(void) const;

        /**
        * Returns the timestamp of the last update.
        *
        * @since 3.1
        * @return The item's timestamp of the last update.
        */
        IMPORT_C const TTime& Timestamp(void) const;

        /**
        * Returns the number of unread topics for this folder-item.
        *
        * @since 3.1
        * @return The number of unread topics for this folder-item.
        */
        IMPORT_C TInt UnreadCount(void) const;

        /**
        * Returns the number of unread feeds for this folder-item.
        *
        * @since 5.0
        * @return The number of unread feeds (those with > 1 unread topics) for
        * this folder item and all folders below it recursively
        */
        IMPORT_C TInt UnreadFeedCountRecursive(void) const;

        /**
        * Returns the number of unread topics for this folder-item.
        *
        * @since 5.0
        * @return The number of unread topics within all feeds for
        * this folder item and all folders below it recursively
        */
        IMPORT_C TInt UnreadTopicCountRecursive(void) const;

        /**
        * Returns the unique id for this folder-item.
        *
        * @since 3.0
        * @return The item's id.
        */
        IMPORT_C TInt Id(void) const;

        /**
        * Returns the associated feed id for this folder-item.  The
        * method panics if the folder-item is a folder (rather than a feed).
        *
        * @since 3.1
        * @return The item's feed id.
        */
        IMPORT_C TInt FeedId(void) const;

        /**
        * Returns the folder-item at the given index
        *
        * @since 3.0
        * @param aIndex The index of the item.
        * @return Returns the item.
        */
        IMPORT_C const CFolderItem* FolderItemAt(TInt aIndex) const;

        /**
        * Returns the index of the given folder-item.
        *
        * @since 3.0
        * @param aItem The item to find
        * @return The item's index or KErrNotFound.
        */
        IMPORT_C TInt Index(const CFolderItem& aItem) const;

        /**
        * Returns the number of items.
        *
        * @since 3.0
        * @return The number of items.
        */
        IMPORT_C TInt FolderItemCount(void) const;

        /**
        * Searches for a FolderItem with the given name.  If "this"
        * isn't a folder it only checks whether or not it matches
        * the given name.  If "this" is a folder it also checks 
        * all embedded items.
        *
        * @since 3.0
        * @param aName The name to search for.
        * @return The matching FolderItem or NULL.
        */
        IMPORT_C const CFolderItem* Search(const TDesC& aName) const;

        /**
        * Searches for a FolderItem with the given id.  If "this"
        * isn't a folder it only checks whether or not it matches
        * the given id.  If "this" is a folder it also checks 
        * all embedded items.
        *
        * @since 3.1
        * @param aFolderItemId The id to search for.
        * @return The matching FolderItem or NULL.
        */
        IMPORT_C const CFolderItem* Search(TInt aFolderItemId) const;

        /**
        * Returns the number of mini items.
        *
        * @since 3.2
        * @return The number of mini items.
        */
        IMPORT_C TInt MiniItemsCount(void) const;

        /**
        * Return The mini item.
        *
        * @since 3.2
        * @return The array of items title.
        */
        IMPORT_C const CItemMini* MiniItemAt( TInt aIndex ) const;

        /**
        * Sets the number of unread topics for this folder-item.
        *
        * @since 3.1
        * Sets The number of unread topics for this folder-item.
        */
        IMPORT_C void SetUnreadCount(TInt aUnreadCount);

        /**
        * Sets the number of unread topics for this folder-item.
        *
        * @since 5.0
        * Returns the Status of Folder item
        */
        IMPORT_C TInt Status() const;

    private:  // New methods
        /**
        * Two-phased constructor.
        * Only called from RFolderItem.  aPackedFolder is adopted by this method.
        */
        static CFolderItem* NewL(TInt aFolderListId, CFeedsEntity* aFeedsEntity);

        /**
        * Two-phased constructor.
        */
        static CFolderItem* NewFolderL( TInt aFolderListId, const TDesC& aTitle, TInt aEntryId, TInt aStatusCode);

        /**
        * Two-phased constructor.
        */
        static CFolderItem* NewFeedL( TInt aFolderListId, const TDesC& aTitle, const TDesC& aUrl, 
                TInt aEntryId, TInt aFeedId, const TTime& aTimestamp, TInt aStatusCode, const TInt& aUnreadCount=0 );

        /**
        * C++ default constructor.  aPackedFolder is adopted by this method. 
        */
        CFolderItem(TInt aFolderListId, CFeedsEntity* aFeedsEntity);

        /**
        * C++ default constructor.
        */
        CFolderItem( TInt aFolderListId );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(const CFeedsEntity& aFeedsEntity);

        /**
        * Adds aItem as a child of this FolderItem.  aItem is adopted by this method.
        *
        * @since 3.0
        * @param aItem The item to append
        * @return void.
        */
        void AddItemL(CFolderItem* aItem);

        /**
        * 
        *
        * @since 3.2
        * Sets 
        */
        void AddMiniItem( CItemMini* aMiniItem );

        /**
        * A private function and associated enum (to be used as a flag for this function)
        * that will be called recursively to gather the count of feeds
        *
        * @since 5.0
        * @param aFolder pointer to the "Root" of the tree folder item for counting
        * @param aCountType the type of count to be performed
        */

        enum TCountType { ECountUnreadFeeds, ECountUnreadTopics };
        static TInt GetFolderUnreadCount(const CFolderItem* aFolder, TCountType aCountType);

    public:  // Friends
        friend class CFolderItemRequestHandler;
        friend class RFolderItem;

    private:
        TLeakTracker                iLeakTracker;

        CFeedsEntity*               iFeedsEntity;
        TInt                        iFolderListId;
        CPackedFolder*              iPackedFolder;
        TPtrC                       iTitle;
        TPtrC                       iSourceUrl;
        TInt                        iEntryId;
        TInt                        iFeedId;
        TTime                       iTimestamp;
        TBool                       iIsFolder;
        TInt                        iUnreadCount;  // how many unread topics
        TInt						iStatusCode;

        const CFolderItem*          iParent;
        RPointerArray<CFolderItem>  iFolderItems;   // children
        RPointerArray<CItemMini>    iMiniItems;    // minimum info of articles
    };


/**
*  Holds minimum information about a feed's item.
*
*  \b Library: FeedsEngine.lib
*
*  @since 3.0
*/
class CItemMini: public CBase
    {
    public:  // Destructor
        /**
        * Destructor.
        */        
        IMPORT_C virtual ~CItemMini();

    public: // New methods
        /**
        * Returns the item's id.
        *
        * @since 3.1
        * @return The value.
        */
        IMPORT_C TInt Id() const;

        /**
        * Returns the item's title.
        *
        * @since 3.0
        * @return The value.
        */
        IMPORT_C const TDesC& Title() const;

    private:  // New methods
        /**
        * Two-phased constructor.
        * Only called from RFeed.
        */
        static CItemMini* NewL();

        /**
        * C++ default constructor.
        */
        CItemMini();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Sets an attribute.
        *
        * @since 3.1
        * @param aAttribute The attribute.
        * @param aAttributeValue The attribute value.
        * @return void.
        */
        void SetAttribute(TUint aAttribute, const TPtrC& aAttributeValue);

    public:  // Friends
        friend class CFolderItem;

    private:  // Data
        TLeakTracker               iLeakTracker;
        TPtrC                      iTitle;
        TInt                       iId;
    };

#endif  // FEEDS_SERVER_FOLDER_ITEM_H
// End of File
