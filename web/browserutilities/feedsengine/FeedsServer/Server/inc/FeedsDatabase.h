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
* Description:  The feeds server database.
*
*/


#ifndef FEEDS_DATABASE_H
#define FEEDS_DATABASE_H

//  INCLUDES
#include <d32dbms.h>
#include <e32base.h>
#include <f32file.h>

#include "LeakTracker.h"

// CONSTANTS
const TInt KUnassignedId = -1;
const TInt KMinFreebytes = 1024000;

// DATA TYPES

// FORWARD DECLARATIONS
class CDbKey;
class CDesC16ArraySeg;
class CPackedFeed;
class CPackedFolder;
class CFeedsServer;
struct TAttribute;

// CLASS DECLARATION


/**
*  The feeds server database.
*
*  \b Library: FeedsEngine.lib
*
*  @since 3.1
*/
NONSHARABLE_CLASS(CFeedsDatabase): public CBase
    {
    public:  // Constructors and destructor
        /**
        * Two-phased constructor.
        *
        * @since 3.1
        * @param aDatabaseCreated To to ETrue by this method if the database
        *        was newly created.
        * @return A new instance.
        */
        static CFeedsDatabase* NewL(CFeedsServer* aFeedsServer, TBool &aDatabaseCreated);

        /**
        * Destructor.
        */
        ~CFeedsDatabase();


    public:  // New methods
        /**
        * Returns the feed if of the feed with the given url.
        *
        * @since 3.1
        * @param aFeedUrl The feed's url
        * @param aFeedId The resulting FeedsTable feed id.
        * @return ETrue if the feed was resolved.
        */
        TBool FeedIdFromUrlL(const TDesC& aFeedUrl, TInt aFolderListId, TInt& aFeedId);

        /**
        * Returns the feed id of the feed with the given entry id.
        *
        * @since 7.1
        * @param aEntryId The feed's folder item id
        * @param aFolderListId The folder list ID of the feed.
        * @param aFeedId The resulting FolderListTable feed id.
        * @return ETrue if the feed was resolved.
        */

        TBool CFeedsDatabase::FeedIdFromEntryIdL(const TInt& aEntryId, TInt aFolderListId, TInt& aFeedId);

        /**
        * Returns the feed id of the entry id with the given feed.
        *
        * @since 7.1
        * @param aFeedId The resulting FolderListTable feed id.
        * @param aFolderListId The folder list ID of the feed.
        * @param aEntryId The feed's folder item id
        * @return ETrue if the feed was resolved.
        */
        TBool CFeedsDatabase::EntryIdFromFeedIdL(const TInt& aFeedId, TInt aFolderListId, TInt& aEntryId);

        /**
        * Return the folder list ID of the feed with the given feed-id.
        *
        * @since 3.2
        * @param aFeedId The FeedsTable feed's id 
        * @param aFolderListId The folder list ID of the feed
        * @return void.
        */
        void FolderListIdFromFeedIdL( TInt aFeedId, TInt& aFolderListId );

        /**
        * Return the feed-id of the given folder-item-id.
        *
        * @since 3.1
        * @param aFolderItemId The FolderListTable folder item's id 
        * @param aFeedId The feed's id
        * @return ETrue if the feed was resolved.
        */
        TBool FeedIdFromFolderItemIdL(TInt aFolderItemId, TInt& aFeedId);

        /**
        * Gets the folder item properties like IsFolder, Title and FeedURL
        *
        * @since 3.2
        * @param aFolderItemId The Folder item's id
        * @param aTitle Title of the folder item will be stored in this
        * @param aFeedUrl Feed URL of the folder item will be stored in this
        * @return void
        */
		void FolderItemInfoL(TInt aFolderItemId, TBool &aIsFolder, HBufC*& aTitle, HBufC*& aFeedUrl);
		
        /**
        * Return the url of the feed with the given feed-id.
        *
        * @since 3.1
        * @param aFeedId The FeedsTable feed's id 
        * @param aFeedUrl The feed's url
        * @return ETrue if the feed was resolved.
        */
        TBool UrlFromFeedIdL(TInt aFeedId, HBufC*& aFeedUrl);

        /**
        * Finds the item with the given item-id-str and returns its id.
        *
        * @since 3.1
        * @param aFeedId The FeedsTable feed id.
        * @param aItemIdStr The item's id string.
        * @param aItemId The resulting ItemsTable item id.
        * @return ETrue if the feed was resolved.
        */
        TBool FindItemL(TInt aFeedId, const TDesC& aItemIdStr, TInt& aItemId);

        /**
        * Finds the folder item with the given name.
        *
        * @since 3.1
        * @param aFolderListId The folder list id.
        * @param aName The name.
        * @param aFolderItemId The resulting folder-item-id, if found.
        * @return ETrue if a folder item with this name was found.
        */
        TBool CFeedsDatabase::FindFolderItemL(TInt& aFolderListId, const TDesC& aName, 
                TInt& aFolderItemId);

        /**
        * Extract all of the feed-ids.
        *
        * @since 3.1
        * @param aFeedIds The resulting feed ids.
        * @return Void.
        */
        void AllFeedIdsL( RArray<TInt>& aFeedIds, TInt aFolderListId = -1 );

        /**
        * Determines if the given feed is newly created (not ok to read from the database).
        *
        * @since 3.1
        * @param aFeedId The resulting feed id.
        * @return ETrue if the feed is newly created.
        */
        TBool IsNewlyCreatedL(TInt aFeedId);
        
        /**
        * Extracts the feed from the database.
        *
        * @since 3.1
        * @param aFeedId The feed's id.  Call ResolveFeedUrl to get the id.
        * @param aFeed The resulting packed feed.
        * @return void
        */
        void ExtractFeedL(TInt aFeedId, CPackedFeed& aFeed);
            
        /**
        * Extracts the folder from the database.
        *
        * @since 3.1
        * @param aFolderListId The folder list's id.
        * @param aPackedFolder The resulting packed folder.
        * @param aItemTitleNeed The flag indicates if the titles of items with new status will be returned.
        * @return void
        */
        void ExtractRootFolderL( TInt aFolderListId, CPackedFolder& aPackedFolder, TBool aItemTitleNeed = EFalse );

        /**
        * Update the database given the packed feed.
        *
        * @since 3.1
        * @param aFeed A packed feed.
        * @param aFeedUrl The feed's url.
        * @param aPurgeOldItems If ETrue all items in the db that are not in the 
        *                       packed feed are purged from the database.
        * @return Void.
        */
        void UpdateFeedL(const CPackedFeed& aPackedFeed, const TDesC& aFeedUrl,TInt aFeedId, TInt aFolderListId,
                TBool aPurgeOldItems = ETrue);

        /**
        * Update the database given the packed folder.
        *
        * @since 3.1
        * @param aFolderListId The folder list's id.
        * @param aFeed A packed folder.
        * @return Void.
        */
        void ImportFolderL(TInt aFolderListId, const CPackedFolder& aPackedFolder);

        /**
        * Add a new entry.
        *
        * @since 3.1
        * @param aFolderListId The folder-list id.
        * @param aTitle The title.
        * @param aUrl The url.
        * @param aIsFolder ETrue if this is a folder.
        * @param aParentEntryId The entry id of the parent.
        * @param aFreq auto update frequency
        * @return The entry-id of the new entry.
        */
        TInt FolderItemAddL(TInt aFolderListId, const TDesC& aTitle,
                const TDesC& aUrl, TBool aIsFolder, TInt aParentEntryId, TInt aFreq);

        /**
        * Update an entry.
        *
        * @since 3.1
        * @param aFolderItemId The folder-item's id.
        * @param aTitle The title.
        * @param aUrl The url.
        * @param aFreq auto update frequency
        * @return void
        */
        void FolderItemUpdateL(TInt aFolderItemId, const TDesC& aTitle, 
                const TDesC& aUrl, TInt aFreq);

        /**
        * Delete the given folder items.
        *
        * @since 3.1
        * @param aFolderItemIds The id of the folder items.
        * @param iSessionCalled This should be true if FeedServerSession calls this function
        * @return void
        */
        void FolderItemDeleteL(const RArray<TInt>& aFolderItemIds, TBool iSessionCalled = EFalse);

        /**
        * Delete anything under the folder list.
        *
        * @since 3.2
        * @param aFolderListId The folder list ID.
        * @return void
        */
        void FolderListDeleteL( TInt aFolderListId );

        /**
        * Move the folder items within their parent.
        *
        * @since 3.1
        * @param aFolderItemIds The id of the folder items.
        * @param aNewIndex The target index.
        * @return void.
        */
        void FolderItemMoveL(const RArray<TInt>& aFolderItemIds, 
                TInt aNewIndex);

        /**
        * Move the entries to another parent.
        *
        * @since 3.1
        * @param aFolderItemIds The id of the folder items.
        * @param aNewParent The id of the new parent.
        * @return void.
        */
        void FolderItemMoveToL(const RArray<TInt>& aFolderItemIds,             
                TInt aNewParent);

        /**
        * Update the status of each of the items in the given feed.
        *
        * @since 3.1
        * @param aFeedId The feed's id.
        * @param aItemIds The Id's of the items to update.
        * @param aItemStatus The corresponding status of items defined in aItemIdStr.
        * @return void
        */
        void FeedUpdateItemStatusL(TInt aFeedId, const RArray<TInt>& aItemIds,
                const RArray<TInt>& aItemStatus, TInt aUnreadCount);

        /**
        * Deletes the feed if the feedId isn't referenced in the folder-list table.
        *
        * @since 3.1
        * @param aFeedId The feed's id.
        * @return void.
        */
        void PurgeFeedIfNotReferencedL(TInt aFeedId);
        
        /**
        * Removes the feed's items also deletes the feed if aDeleteFeed is ETrue.
        * It is only safe to call this method with aDeleteFeed set to ETrue if
        * aFeedId is not found in the FolderListTable.
        *
        * @since 3.1
        * @param aFeedId The feed's id.
        * @param aDeleteFeed If ETrue the feed is also delete.
        * @return void.
        */
        void FeedPurgeL(TInt aFeedId, TBool aDeleteFeed = EFalse);
            
        /**
        * Purge all old items in the given feed.
        *
        * @since 3.1
        * @param aFeedId The feed's id.
        * @param aTimestamp The timestamp used to remove old items.
        * @return void.
        */
        void PurgeOldItemsL(TInt aFeedId, const TTime& aTimestamp);
            
        /**
        * Purge the associated enclosures
        *
        * @since 3.1
        * @param aFeedId The feed's id.
        * @param aItemId The item's id.
        * @return void.
        */
        void PurgeEnclosuresL(TInt aFeedId, TInt aItemId);

        /**
        * Purge all items not found in the given array.
        *
        * @since 3.1
        * @param aFeedId The feed's id.
        * @param aItemIds An array that holds the itemIds of the items to keep.
        * @return purged unread count.
        */
        TInt PurgeOtherItemsL(TInt aFeedId, const RArray<TInt>& aItemIds);

        /**
        * Get the settings from the database.
        *
        * @since 3.2
        * @param aFolderListIds The array holding folder list Ids in the settings table.
        * @return void.
        */
        void ExtractFolderListIdInSettingsL( RArray<TInt>& aFolderListIds );

        /**
        * Get the settings from the database.
        *
        * @since 3.2
        * @param aFolderListId The client to whom the settings belong to.
        * @param aAutoUpdate ETrue if auto update is enabled.
        * @param aAutoUpdateFreq The freq of auto updates.
        * @param aAutoUpdateAP The access point.
        * @param aAutoUpdateWhileRoaming ETrue if auto update is enabled while roaming.
	    * @return void.
        */
		void ExtractAutoUpdateSettingsL( TInt aFolderListId, TBool& aAutoUpdate, TInt& aAutoUpdateFreq,
        TUint32& aAutoUpdateAP, TBool &autoUpdateWhileRoaming  );        

        /**
        * Commit the settings to the database
        *
        * @since 3.2
        * @param aFolderListId The client to whom the settings belong to.
        * @param aAutoUpdate ETrue if auto update is enabled.
        * @param aAutoUpdateFreq The freq of auto updates.
        * @param aAutoUpdateAP The access point.
        * @param aAutoUpdateWhileRoaming ETrue if auto update is enabled.
        * @return Void.
        */
		void CommitAutoUpdateSettingsL( TInt aFolderListId, TBool aAutoUpdate, TInt aAutoUpdateFreq,
        TUint32 aAutoUpdateAP, TBool aAutoUpdateWhileRoaming );
                
        /**
        * Get the last auto update settings from the database.
        *
        * @since 3.2
        * @param aFolderListId The client to whom the settings belong to.
        * @param aLastAutoUpdate The timestamp of the last auto update.
        * @return void.
        */
        void ExtractLastAutoUpdateSettingsL(TInt aFolderListId, TTime& aLastAutoUpdate);

        /**
        * Commit the last auto update settings to the database
        *
        * @since 3.2
        * @param aFolderListId The client to whom the settings belong to.
        * @param aLastAutoUpdate The timestamp of the last auto update.
        * @return Void.
        */
        void CommitLastAutoUpdateSettingsL(TInt aFolderListId, TTime aLastAutoUpdate);

        /**
        * Compacts the database.
        *
        * @since 3.1
        * @return Void.
        */
        void Compact();

        /**
        * Print the tables from the database.
        *
        * @since 3.2
        * @return Void.
        */
        void DebugPrintTablesL();

        /**
        * Print the folder list table from the database.
        *
        * @since 3.2
        * @return Void.
        */
        void DebugPrintFolderListTableL();

        /**
        * Print the item table from the database.
        *
        * @since 3.2
        * @return Void.
        */
        void DebugPrintItemTableL();
            
        /**
        * Extract the folder-item-ids of the children of the given parent.
        *
        * @since 3.2
        * @param aFolderItemId The id of the parent folder item.
        * @param aChildren The ids of the children.
        * @return void.
        */
        void FolderItemGetChildrenL(TInt aFolderItemId, RArray<TInt>& aChildren);
        
        /**
        * Check for the duplicate folder names and feed URLs and suggest the next title.
        *
        * @since 3.2
        * @param aFolderListId The folder list id.
        * @param aParentEntryId The parent id
        * @param aTitle Folder/Feed title to be checked for duplicates
        * @param aIsFolder Flag which indicates folder/feed
        * @param aNewFeedTitle The new feed/folder title generated
        * @return TBool - ETrue if new folder/feed title is generated successfully and ready to add to the DB.
        */
		TBool ValidateFeedFolderTitleL(const TInt &aFolderListId, const TInt &aParentEntryId,
			 const TDesC& aTitle, const TBool aIsFolder, TDes& aNewFeedTitle);

        /**
         * Update Feed Status
         *
         * @since 5.0
         * @param aFeedId Feed Id.
         * @param aFeedStatus Feed status
         * @return void
         */
		void UpdateFeedStatusL(TInt aFeedId, TInt aFeedStatus);

        /**
         * Adds a new column AutoUpdateFreq to FeedTable in the case of versions less than 7.1.
         *
         * @since 7.1
         * @return void
         */
        void AlterFeedTableWithAutoFequencyL();

        /**
         * Fetchs Freqs for all feeds.
         *
         * @since 7.1
         * @return bool
         */        
        TBool FreqFromFeedIdL(TInt aFeedId, TInt& aFreq);

    private:  // New methods
        /**
        * C++ default constructor.
        */
        CFeedsDatabase(CFeedsServer* aFeedsServer);

        /**
        * Two-Phase constructor
        */
        void ConstructL(TBool &aDatabaseCreated);

        /**
        * Reset the database.
        *
        * @since 3.1
        * @return void.
        */
        void ResetDatabaseL();

        /**
        * Creates the version table.
        *
        * @since 3.1
        * @return void.
        */
        void CreateVersionTableL();

        /**
        * Creates the folder-list table.
        *
        * @since 3.1
        * @return void.
        */
        void CreateFolderListTableL();

        /**
        * Creates the feed table.
        *
        * @since 3.1
        * @return void.
        */
        void CreateFeedTableL();

        /**
        * Creates the feed-item table.
        *
        * @since 3.1
        * @return void.
        */
        void CreateFeedItemTableL();

        /**
        * Creates the item-enclosure table.
        *
        * @since 3.1
        * @return void.
        */
        void CreateItemEnclosureTableL();

        /**
        * Creates the settings table.
        *
        * @since 3.1
        * @return void.
        */
        void CreateSettingsTableL();

        /**
        * Returns an available folder-item id
        *
        * @since 3.1
        * @return void.
        */
        TInt NextFolderItemIdL();

        /**
        * Returns an available feed id
        *
        * @since 3.1
        * @return void.
        */
        TInt NextFeedIdL();

        /**
        * Returns an available item id
        *
        * @since 3.1
        * @return void.
        */
        TInt NextItemIdL();

        /**
        * Add a new entry.
        *
        * @since 3.1
        * @param aFolderListId The folder-list id.
        * @param aTitle The title.
        * @param aUrl The url.
        * @param aIsFolder ETrue if this is a folder.
        * @param aSiblingIndex The sibling index within the given parent.
        * @param aParentEntryId The entry id of the parent.
        * @param aOutFolderId The new id generated for the folder added
        * @param aOutFeedId The new id generated for the feed added
        * @param aFreq auto update frequency
        * @return void.
        */
        void FolderItemAddHelperL(TInt aFolderListId, const TDesC& aTitle,
                const TDesC& aUrl, TBool aIsFolder, TInt aSiblingIndex, TInt aParentEntryId,
                TInt& aOutFolderId, TInt& aOutFeedId, TInt aFreq);

        /**
        * Delete the given folder items and store the feedIds in aFeedIds.  aFeedIds 
        * can then be used to delete any newly unreferenced feeds.
        *
        * @since 3.1
        * @param aFolderItemIds The ids of the folder items to delete.
        * @param aFeedIds The feed ids of the folder items deleted.
        * @return void.
        */
        void FolderItemDeleteHelperL(const RArray<TInt>& aFolderItemIds,
                RArray<TInt>& aFeedIds);
        
        /**
        * Reorders the sibling indexes.
        *
        * @since 3.1
        * @param aParentId The entry id of the parent.
        * @return void.
        */
        void AdjustSiblingIndexesL(TInt aParentId);

        /**
        * Creates a hole in the sibling index.
        *
        * @since 3.1
        * @param aParentId The entry id of the parent.
        * @param aNewIndex Where the hole will be made.
        * @param aHoleSize The size of the hole.
        * @return void.
        */
        void CreateSiblingIndexHoleL(TInt aParentId, TInt aNewIndex, TInt aHoleSize);
                
        /**
        * Commit the folder entry to the database.
        *
        * @since 3.1
        * @param aFolderListId The folder-list-id that this entry is added to.  This id
        *        allows multiple lists of feeds and folders to be tracked by the database. 
        * @param aFolderItemId The folder item's id.
        * @param aParentId The entry's parent folder.
        * @param aSiblingIndex The sibling order of this entry.
        * @param aIsFolder ETrue when the entry is an folder.
        * @param aFeedId The feed's id or KUnassignedId if this is folder.
        * @param aTitle The title.
        * @return Void.
        */
        void CommitFolderListL(TInt aFolderListId, TInt aFolderItemId, 
                TInt aParentId, TInt aSiblingIndex, TBool aIsFolder, TInt aFeedId, 
                const TDesC& aTitle);

    private:
        /**
        * Commit the feed to the database.
        *
        * @since 3.1
        * @param aFolderListId The folder list's id.
        * @param aIsNewFeed If ETrue a new entry is added to the database, otherwise
        *        the existing entry is updated.
        * @param aFeedId The feed's id.
        * @param aAttributes The associated attributes.
        * @param aDefaultTime The default timestamp if the feed doesn't provide a value.
        * @param aUnreadCount number of unread articles
        * @param aFreq auto update frequency
        * @return Void.
        */
        void CommitFeedL(TInt aFolderListId, TBool aIsNewFeed, TInt aFeedId, 
                const RArray<TAttribute>& aAttributes, const TTime& aDefaultTime, TInt aUnreadCount, TInt aFreq);

        /**
        * Commit the feed to the database.
        *
        * @since 3.1
        * @param aFolderListId The folder list's id.
        * @param aIsNewFeed If ETrue a new entry is added to the database, otherwise
        *        the existing entry is updated.
        * @param aFeedId The feed's id.
        * @param aAttributes The associated attributes.
        * @param aDefaultTime The default timestamp if the feed doesn't provide a value.
        * @param aUnreadCount number of unread articles
        * @return Void.
        */
        void CommitFeedL(TInt aFolderListId, TBool aIsNewFeed, TInt aFeedId, 
                const RArray<TAttribute>& aAttributes, const TTime& aDefaultTime, TInt aUnreadCount);

        /**
        * Commit a new feed to the database.
        *
        * @since 3.1
        * @param aFolderListId The folder list's id.
        * @param aTitle The title.
        * @param aUrl The url.
        * @param aFreq auto update frequency
        * @return The feed's id.
        */
        TInt CommitFeedL(TInt aFolderListId, const TDesC& aTitle, const TDesC& aUrl, TInt aFreq);

        /**
        * Commit the item to the database.  The itemIdStr is also appended to aItemIdStrs.
        *
        * @since 3.1
        * @param aItemId The item's id.
        * @param aFeedId The feed's id.
        * @param aAttributes The associated attributes.
        * @param aItemIds An array that holds the itemId of the items in this feed.
        * @return ETrue if this item was added to the database (i.e. it wasn't already there).
        */
        TBool CommitItemL(TInt aItemId, TInt aFeedId, 
                const RArray<TAttribute>& aAttributes, RArray<TInt>& aItemIds);

        /**
        * Commit the enclosure to the database
        *
        * @since 3.1
        * @param aEnclosureId The enclosure's id.
        * @param aItemId The item's id.
        * @param aFeedId The feed's id.
        * @param aAttributes The associated attributes.
        * @return Void.
        */
        void CommitEnclosureL(TInt aEnclosureId, TInt aItemId, TInt aFeedId, 
                const RArray<TAttribute>& aAttributes);
            
        /**
        * Extracts and pack the folder.
        *
        * @since 3.1
        * @param aFolderListId The folder list's id.
        * @param aFolderId The id of the folder to be packed.
        * @param aPackedFolder The resulting packed folder.
        * @param aItemTitleNeed Need items title or not.
        * @return void
        */
        void PackFolderL(TInt aFolderListId, TInt aFolderId, CPackedFolder& aPackedFolder,
                TBool aItemTitleNeed );
            
        /**
        * Extracts and pack the feed.
        *
        * @since 3.1
        * @param aFeedId The feed's id
        * @param aFeed The destination feed.
        * @return Void.
        */
        void PackFeedL(TInt aFeedId, CPackedFeed& aFeed);

        /**
        * Extracts and pack the items.
        *
        * @since 3.1
        * @param aFeedId The feed's id
        * @param aFeed The destination feed.
        * @return Void.
        */
        void PackItemsL(TInt aFeedId, CPackedFeed& aFeed);
        
        /**
        * Extracts and pack the enclosures.
        *
        * @since 3.1
        * @param aFeedId The feed's id
        * @param aItemId The item's id
        * @param aFeed The destination feed.
        * @return Void.
        */
        void PackEnclosuresL(TInt aFeedId, TInt aItemId, CPackedFeed& aFeed);

        /**
        * Extracts and pack the enclosures.
        *
        * @since 3.2
        * @param aFeedId The feed's id
        * @param aStatus The desired status of items
        * @param aPackedFolder The resulting packed folder.
        * @return Void.
        */
        void SelectMiniItemsL( TInt aFeedId, TInt aStatus, CPackedFolder& aFolder );

        /**
        * Writes "long" text to the database.
        *
        * @since 3.1
        * @param aTable The table.
        * @param aColumnIndex The column index to write to.
        * @param aValue The value.
        * @return Void.
        */
        void WriteLongTextL(RDbRowSet& aTable, TInt aColumnIndex, 
                const TDesC& aValue);

        /**
        * Reads "long" text from the database.
        *
        * @since 3.1
        * @param aTable The table.
        * @param aColumnIndex The column index to read from.
        * @param aValue The resulting value.
        * @return Void.
        */
        void ReadLongTextL(RDbRowSet& aTable, TInt aColumnIndex, 
                HBufC*& aValue);

        /**
        * Pushes the release table function onto cleanup stack 
		* and opens the FolderList table for use.
        *
        * @since 3.1
        * @param aAccess The access specification for the rowset.
        * @return Void.
        */
        void UseFolderListTableLC(RDbRowSet::TAccess aAccess);
        
        /**
        * Opens the Folder List table for use.
        *
        * @since 3.2
        * @param aAccess The access specification for the rowset.
        * @return Void.
        */        
		void OpenFolderListTableL(RDbRowSet::TAccess aAccess);
		
		/**
        * Closes the Folder List table.
        *
        * @since 3.1
        * @param aPtr Pointer to self.
        * @return Void.
        */
        static void ReleaseFolderListTable(TAny *aPtr);

        /**
        * Pushes the release table function onto cleanup stack 
		* and opens the Feed table for use.
        *
        * @since 3.1
        * @param aAccess The access specification for the rowset.
        * @return Void.
        */
        void UseFeedTableLC(RDbRowSet::TAccess aAccess);

        /**
        * Opens the feed table for use.
        *
        * @since 3.2
        * @param aAccess The access specification for the rowset.
        * @return Void.
        */
		void OpenFeedTableL(RDbRowSet::TAccess aAccess);
		
        /**
        * Closes the feed table.
        *
        * @since 3.1
        * @return Void.
        */
        static void ReleaseFeedTable(TAny *aPtr);

        /**
        * Opens the OPML import related tables (FeedTable and FolderListTable) and starts a transaction
        *
        * @since 3.2
        * @return Void.
        */		
		void PrepareImportTransationsL();
		
        /**
        * Closes the OPML import related tables (FeedTable and FolderListTable)
        *
        * @since 3.2
        * @return Void.
        */		
		void ReleaseImportTables();
		
        /**
        * Commits current database transaction
        *
        * @since 3.2
        * @return Void.
        */		
		void CommitImportTransaction();
		
        /**
        * Cancels/Rollbacks current database transaction
        *
        * @since 3.2
        * @return Void.
        */		
		void CancelImportTransaction();

        /**
        * Deletes records from a FeedTable
        *
        * @since 3.2
        * @param aFeedIds The array of FeedIds to be deleted
        * @return Void.
        */		
		void DeleteFeedTableRecordsL(RArray<TInt>& aFeedIds);
		
        /**
        * Deletes records from a FolderListTable
        *
        * @since 3.2
        * @param aFolderItemIds The array of FolderItemIds to be deleted
        * @return Void.
        */		
		void DeleteFolderListTableRecordsL(RArray<TInt>& aFolderItemIds);

        /**
        * Deletes records from a given table
        *
        * @since 3.2
        * @param aTableName The table from which records to be deleted
        * @param aColumnName The column name whose values have been specified to be deleted
        * @param aIds The Id valus for the columns to be deleted
        * @return Void.
        */		
		void DeleteRecordsFromTableL(const TDesC& aTableName, const TDesC& aColumnName, RArray<TInt>& aIds);

        /**
        * Opens the item table for use.
        *
        * @since 3.1
        * @param aAccess The access specification for the rowset.
        * @return Void.
        */
        void UseItemTableLC(RDbRowSet::TAccess aAccess);

        /**
        * Closes the Item table.
        *
        * @since 3.1
        * @param aPtr Pointer to self.
        * @return Void.
        */
        static void ReleaseItemTable(TAny *aPtr);

        /**
        * Opens the Enclosure table for use.
        *
        * @since 3.1
        * @param aAccess The access specification for the rowset.
        * @return Void.
        */
        void UseEnclosureTableLC(RDbRowSet::TAccess aAccess);

        /**
        * Closes the Enclosure table.
        *
        * @since 3.1
        * @param aPtr Pointer to self.
        * @return Void.
        */
        static void ReleaseEnclosureTable(TAny *aPtr);

        /**
        * Opens the Settings table for use.
        *
        * @since 3.1
        * @param aAccess The access specification for the rowset.
        * @return Void.
        */
        void UseSettingsTableLC(RDbRowSet::TAccess aAccess);

        /**
        * Closes the Settings table.
        *
        * @since 3.1
        * @param aPtr Pointer to self.
        * @return Void.
        */
        static void ReleaseSettingsTable(TAny *aPtr);

        /**
        * Opens the Version table for use.
        *
        * @since 3.1
        * @param aAccess The access specification for the rowset.
        * @return Void.
        */
        void UseVersionTableLC(RDbRowSet::TAccess aAccess);

        /**
        * Closes the Version table.
        *
        * @since 3.1
        * @param aPtr Pointer to self.
        * @return Void.
        */
        static void ReleaseVersionTable(TAny *aPtr);

        /**
        * Parses the Folder/Feed name to get the counter for duplicated names (ex Folder, Folder (2), Folder (3) ...)
        *
        * @since 3.2
        * @param aFolderTitle is the input Folder/Feed title
        * @param aCounterVal is the output postfix integer value
        * @return TBool ETrue if its a valid duplicated folder name else EFalse
        */
        TBool GetDuplicateFolderCounter(const TDesC& aFolderToBeAdded, const TDesC& aFolderTitle, TInt &aCounterVal);

        /**
        * Checks for the FeedURL's existance
        *
        * @since 3.2
        * @param aFolderListId is the input folder list id
        * @param aUrl is the feed-url needs to be checked
        * @return TBool ETrue if the specified URL exists else EFalse
        */
        TBool ValidateFeedURLL(const TInt &aFolderListId,  const TDesC& aUrl);
        
        /**
        * This will check for the duplicate folder names and suggest a new folder title if duplicated
        *
        * @since 3.2
        * @param aFolderListId is the input folder list id
        * @param aParentEntryId is the parent id
        * @param aTitle The Folder/Feed title to be checked for duplicate
        * @param aNewFeedTitle The new Folder/Feed title generated
        * @return void
        */
        void GenerateNewFeedFolderTitleL(
                                        const TInt &aFolderListId,
                                        const TInt &aParentEntryId,
                                        const TDesC& aTitle,
                                        TDes& aNewFeedTitle
                                        );

        inline void SetIsFolderTableUpdateNeeded (TBool aIsFolderTableUpdateNeeded)
        {
           iIsFolderTableUpdateNeeded = aIsFolderTableUpdateNeeded;
        }

    private:    // Data
        TLeakTracker        iLeakTracker;
        
        RDbs                iDBs;                       ///< DBMS session handle. Own
        RDbNamedDatabase    iDatabase;                  ///< Database handle. Own.
        TBuf<KMaxFileName>  iDatabasePath;
            
        RDbTable            iFolderListTable;
        CDbColSet*          iFolderListColSet;
        TInt                iFolderListTableRefCount;
        
        RDbTable            iFeedTable;
        CDbColSet*          iFeedColSet;
        TInt                iFeedTableRefCount;

        RDbTable            iItemTable;
        CDbColSet*          iItemColSet;
        TInt                iItemTableRefCount;

        RDbTable            iEnclosureTable;
        CDbColSet*          iEnclosureColSet;
        TInt                iEnclosureTableRefCount;
        
        RDbTable            iVersionTable;
        CDbColSet*          iVersionColSet;
        TInt                iVersionTableRefCount;

        RDbTable            iSettingsTable;
        CDbColSet*          iSettingsColSet;
        TInt                iSettingsTableRefCount;

        TInt                iNextAvailableFeedId;

        RArray<TInt>        iDeleteFolderArray; // This array will be populated when session calls delete
        CFeedsServer*       iFeedsServer;

        TBool               iIsFolderTableUpdateNeeded;


        friend class CImportFeedsTask;
        friend class CFeedsServerSession;
    };

#endif      // FEEDS_DATABASE_H

// End of File
