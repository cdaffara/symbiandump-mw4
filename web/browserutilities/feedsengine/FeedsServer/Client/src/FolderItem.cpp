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
* Description:  FolderItem either contains a list of FolderItems or contains
*                information about a feed. 
*
*/


#include "FeedsServerFolderItem.h"
#include "FolderAttributes.h"
#include "FeedAttributes.h"
#include "PackedFolder.h"

// -----------------------------------------------------------------------------
// CFolderItem::NewL
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CFolderItem* CFolderItem::NewL(TInt aFolderListId, CFeedsEntity* aFeedsEntity)
    {
    CFolderItem* self = new (ELeave) CFolderItem(aFolderListId, aFeedsEntity);

    CleanupStack::PushL(self);
    self->ConstructL(*aFeedsEntity);
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CFolderItem::NewFolderL
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CFolderItem* CFolderItem::NewFolderL( TInt aFolderListId, const TDesC& aTitle, TInt aEntryId, TInt aStatusCode)
    {
    CFolderItem* self = new (ELeave) CFolderItem( aFolderListId );

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();

    self->iTitle.Set(aTitle);
    self->iIsFolder = ETrue;
    self->iEntryId = aEntryId;

    // Set the timestamp to the current time as they don't apply to folders.
    self->iTimestamp.UniversalTime();
    self->iStatusCode = aStatusCode;
    return self;
    }

// -----------------------------------------------------------------------------
// CFolderItem::NewFeedL
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CFolderItem* CFolderItem::NewFeedL( TInt aFolderListId, const TDesC& aTitle, const TDesC& aUrl, 
        TInt aEntryId, TInt aFeedId, const TTime& aTimestamp, TInt aStatusCode, const TInt& aUnreadCount)
    {
    CFolderItem* self = new (ELeave) CFolderItem( aFolderListId );

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();

    self->iTitle.Set(aTitle);
    self->iSourceUrl.Set(aUrl);
    self->iEntryId = aEntryId;
    self->iFeedId = aFeedId;
    self->iTimestamp = aTimestamp;
    self->iUnreadCount = aUnreadCount;
    self->iStatusCode = aStatusCode;

    return self;
    }

// -----------------------------------------------------------------------------
// CFolderItem::CFolderItem
// C++ default constructor can NOT contain any code, that
// might leave.    aPackedFolder is adopted 
// -----------------------------------------------------------------------------
//
CFolderItem::CFolderItem(TInt aFolderListId, CFeedsEntity* aFeedsEntity):
        iLeakTracker(CLeakTracker::EFolderItem), iFeedsEntity(aFeedsEntity),
		iFolderListId(aFolderListId)
    {
    }

// -----------------------------------------------------------------------------
// CFolderItem::CFolderItem
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CFolderItem::CFolderItem( TInt aFolderListId ):
        iLeakTracker(CLeakTracker::EFolderItem), iFolderListId(aFolderListId)
    {
    }

// -----------------------------------------------------------------------------
// CFolderItem::ConstructL
// Symbian 2nd phase constructor can leave.
// by this method.
// -----------------------------------------------------------------------------
//
void CFolderItem::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CFolderItem::ConstructL
// Symbian 2nd phase constructor can leave.
// by this method.
// -----------------------------------------------------------------------------
//
void CFolderItem::ConstructL(const CFeedsEntity& aFeedsEntity)
    {
    TPtrC         title;
    TPtrC         url;
    TInt          feedId;
    TTime         timestamp;
    TInt          unreadCount;
    CItemMini*    miniItem = NULL;
    TPtrC         attributeValue;
    TInt 		  statusCode;

    ConstructL();    

    for(TInt i=0 ; i< aFeedsEntity.GetChildren().Count(); i++ )
        {
        CFolderItem *newItem = CFolderItem::NewL( iFolderListId, aFeedsEntity.GetChildren()[i]);    
        newItem->iParent = this;           
        iFolderItems.Append( newItem);
        }               

        iEntryId = iFeedsEntity->GetId();      

        iFeedsEntity->GetIntegerValue( EFolderAttributeStatus, statusCode);  
        iStatusCode = statusCode;

        if(iFeedsEntity->GetType()  == EFolder)
            {
            iIsFolder = ETrue;    

            iFeedsEntity->GetStringValue( EFolderAttributeTitle, title);
            iTitle.Set(title);

            // Set the timestamp to the current time as they don't apply to folders.
            iTimestamp.UniversalTime();       

            }
        else if( iFeedsEntity->GetType()  == EFeed)
            {
            iIsFolder = EFalse;

            iFeedsEntity->GetIntegerValue( EFeedAttributeFeedId, feedId);
            iFeedId = feedId;

            iFeedsEntity->GetStringValue( EFeedAttributeTitle, title);
            iTitle.Set(title);

            iFeedsEntity->GetStringValue( EFeedAttributeLink, url);
            iSourceUrl.Set(url);

            iFeedsEntity->GetTimeValue( EFeedAttributeTimestamp, iTimestamp);  

            iFeedsEntity->GetIntegerValue( EFolderAttributeUnreadCount, unreadCount);
            iUnreadCount = unreadCount; 

            }
        else if( iFeedsEntity->GetType()  == EMiniItem)
            {            
            iIsFolder = EFalse;

            miniItem = CItemMini::NewL();

            iMiniItems.Append( miniItem);   

            miniItem->iId = iEntryId = iFeedsEntity->GetId();

            iFeedsEntity->GetStringValue( EFolderAttributeMiniItemTitle, title);
            miniItem->iTitle.Set( url); 

            }    
      }

// -----------------------------------------------------------------------------
// CFolderItem::~CFolderItem
// Deconstructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CFolderItem::~CFolderItem()
    {
    // Delete the items values.
    iFolderItems.ResetAndDestroy();

    iMiniItems.ResetAndDestroy();

    delete iPackedFolder;
    }

// -----------------------------------------------------------------------------
// CFolderItem::FolderListId
//
// Returns the folder-list-id associated with this item.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CFolderItem::FolderListId(void) const
    {
    return iFolderListId;
    }

// -----------------------------------------------------------------------------
// CFolderItem::Parent
//
// Returns the parent folder or NULL if this is the root folder.
// -----------------------------------------------------------------------------
//
EXPORT_C const CFolderItem* CFolderItem::Parent(void) const
    {
    return iParent;
    }

// -----------------------------------------------------------------------------
// CFolderItem::IsFolder
//
// Returns true if this is a folder.
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CFolderItem::IsFolder(void) const
    {
    return iIsFolder;
    }

// -----------------------------------------------------------------------------
// CFolderItem::Name
//
// Returns the name of the item.
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CFolderItem::Name(void) const
    {
    return iTitle;
    }

// -----------------------------------------------------------------------------
// CFolderItem::SourceUrl
//
// Returns the source url of the item.
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CFolderItem::SourceUrl(void) const
    {
    if (!iIsFolder)
        {
        return iSourceUrl;
        }
    else
        {
        return KNullDesC;
        }
    }

// -----------------------------------------------------------------------------
// CFolderItem::Name
//
// Returns the name of the item.
// -----------------------------------------------------------------------------
//
EXPORT_C const TTime& CFolderItem::Timestamp(void) const
    {
    return iTimestamp;
    }

// -----------------------------------------------------------------------------
// CFolderItem::UnreadCount
//
// Returns the number of unread topics for this folder-item.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CFolderItem::UnreadCount(void) const
    {
    return iUnreadCount;
    }

// -----------------------------------------------------------------------------
// CFolderItem::GetFolderUnreadCount
//
// Recursively marches through folder/feed hierarchy and counts the number of
// unread according to the style in aCountType
//
// -----------------------------------------------------------------------------
//

TInt CFolderItem::GetFolderUnreadCount(const CFolderItem* aFolder, TCountType aCountType)
    {
    TInt folderItemCount = 0;
    TInt unreadCount = 0;

    if(NULL == aFolder)
    {
    return 0;
    }

    folderItemCount = aFolder->FolderItemCount();
    for(TInt i=0; i < folderItemCount; i++)
        {
        const CFolderItem* itemInFolder = aFolder->FolderItemAt( i );
        if(itemInFolder->IsFolder())
            {
            // recurse through this folder
            unreadCount += GetFolderUnreadCount(itemInFolder,aCountType);
            }
        else
            {
            // update according to desired count
            // UnreadTopcs counts all unread articles within a feed
            // UnreadFeeds just counts the feed as unread if at least one
            // article remains unread			
            if(aCountType == ECountUnreadTopics)
                {
                unreadCount += itemInFolder->UnreadCount();
                }
            else if(aCountType == ECountUnreadFeeds)
                {
                if(itemInFolder->UnreadCount() > 0)
                    {
                    unreadCount++;
                    }
                }			
            }
        }
    return unreadCount;
    }

// -----------------------------------------------------------------------------
// CFolderItem::UnreadFeedCountRecursive
//
// Returns the number of unread feeds (those with > 1 undread topics) for this
// folder-item and all folders below it recursively
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CFolderItem::UnreadFeedCountRecursive(void) const
    {
    return GetFolderUnreadCount(this, ECountUnreadFeeds);
    }

// -----------------------------------------------------------------------------
// CFolderItem::UnreadTopicCountRecursive
//
// Returns the number of unread topics (i.e., all articles within feed) for this
// folder-item and all folders below it recursively
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CFolderItem::UnreadTopicCountRecursive(void) const
    {
    return GetFolderUnreadCount(this, ECountUnreadTopics);
    }

// -----------------------------------------------------------------------------
// CFolderItem::Id
//
// Returns the unique id for this folder-item.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CFolderItem::Id(void) const
    {
    return iEntryId;
    }
    

// -----------------------------------------------------------------------------
// CFolderItem::FeedId
//
// Returns the associated feed id for this folder-item.  The
// method panics if the folder-item is a folder (rather than a feed).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CFolderItem::FeedId(void) const
    {
    return iFeedId;
    }


// -----------------------------------------------------------------------------
// CFolderItem::FolderItem
//
// Returns the folder-item at the given index
// -----------------------------------------------------------------------------
//
EXPORT_C const CFolderItem* CFolderItem::FolderItemAt(TInt aIndex) const
    {
    if (iIsFolder && (iFolderItems.Count() > 0) && (aIndex >= 0) && (aIndex < iFolderItems.Count()))
        {
        return iFolderItems[aIndex];
        }
    else
        {
        return NULL;
        }
    }

// -----------------------------------------------------------------------------
// CFolderItem::Index
//
// Returns the index of the given folder-item.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CFolderItem::Index(const CFolderItem& aItem) const
    {
    return iFolderItems.Find(&aItem);
    }

// -----------------------------------------------------------------------------
// CFolderItem::FolderItemCount
//
// Returns the number of items.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CFolderItem::FolderItemCount(void) const
    {
    if (iIsFolder)
        {
        return iFolderItems.Count();
        }
    else
        {
        return 0;
        }
    }

// -----------------------------------------------------------------------------
// CFolderItem::Search
//
// Searches for a FolderItem with the given name.  If "this"
// isn't a folder it only checks whether or not it matches
// the given name.  If "this" is a folder it also checks 
// all embedded items.
// -----------------------------------------------------------------------------
//
EXPORT_C const CFolderItem* CFolderItem::Search(const TDesC& aName) const
    {
    // If "this" matches then return it.
    if (iTitle.CompareF(aName) == 0)
        {
        return this;
        }

    // Otherwise if this is a folder, search it's children too.
    if (IsFolder())
        {
        for (TInt i = 0; i < FolderItemCount(); i++)
            {
            const CFolderItem*  child;
            const CFolderItem*  item;

            child = FolderItemAt(i);
            if ((item = child->Search(aName)) != NULL)
                {
                return item;
                }
            }
        }

    // Otherwise no match was found.
    return NULL;
    }

// -----------------------------------------------------------------------------
// CFolderItem::Search
//
// Searches for a FolderItem with the given id.  If "this"
// isn't a folder it only checks whether or not it matches
// the given id.  If "this" is a folder it also checks 
// all embedded items.
// -----------------------------------------------------------------------------
//
EXPORT_C const CFolderItem* CFolderItem::Search(TInt aFolderItemId) const
    {
    // If "this" matches then return it.
    if (iEntryId == aFolderItemId)
        {
        return this;
        }

    // Otherwise if this is a folder, search it's children too.
    if (IsFolder())
        {
        for (TInt i = 0; i < FolderItemCount(); i++)
            {
            const CFolderItem*  child;
            const CFolderItem*  item;

            child = FolderItemAt(i);
            if ((item = child->Search(aFolderItemId)) != NULL)
                {
                return item;
                }
            }
        }

    // Otherwise no match was found.
    return NULL;
    }

// -----------------------------------------------------------------------------
// CFolderItem::MiniItemsCount
//
// Returns the number of mini items.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CFolderItem::MiniItemsCount(void) const
    {
    return iMiniItems.Count();
    }

// -----------------------------------------------------------------------------
// CFolderItem::MiniItemAt
//
// Return The mini item.
// -----------------------------------------------------------------------------
//
EXPORT_C const CItemMini* CFolderItem::MiniItemAt(TInt aIndex) const
    {
    if ( (iMiniItems.Count() > 0) && (aIndex >= 0) && (aIndex < iMiniItems.Count()) )
        {
        return iMiniItems[ aIndex ];
        }
    else
        {
        return NULL;
        }
    }


// -----------------------------------------------------------------------------
// CFolderItem::AddMiniItem
//
// Add a mini item to the array of mini items.
// -----------------------------------------------------------------------------
//
EXPORT_C void CFolderItem::AddMiniItem( CItemMini* aMiniItem )
    {
    iMiniItems.Append( aMiniItem );
    }


// -----------------------------------------------------------------------------
// CFolderItem::AddItemL
//
// Adds aItem as a child of this FolderItem.  aItem is adopted by this method.
// -----------------------------------------------------------------------------
//
void CFolderItem::AddItemL(CFolderItem* aItem)
    {
    // Insert the item.
    if (iIsFolder)
        {
        User::LeaveIfError(iFolderItems.Append(aItem));

        // Set the parentage.
        aItem->iParent = this;
        }
    else
        {
        User::Leave(KErrNotSupported);
        }
    }


// -----------------------------------------------------------------------------
// CFolderItem::SetUnreadCount
//
// Sets the number of unread topics for this folder-item. 
// -----------------------------------------------------------------------------
//
EXPORT_C void CFolderItem::SetUnreadCount(TInt aUnreadCount)
    {
    iUnreadCount = aUnreadCount;
    }

// -----------------------------------------------------------------------------
// CFolderItem::Status
//
// Returns status of Folder Item
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CFolderItem::Status() const
    {
    return iStatusCode;
    }

// -----------------------------------------------------------------------------
// CItemMini::NewL
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CItemMini* CItemMini::NewL()
    {
    CItemMini* self = new (ELeave) CItemMini();

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CItemMini::CItemMini
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CItemMini::CItemMini():
        iLeakTracker(CLeakTracker::EMiniItem), iTitle(KNullDesC)
    {
    }

// -----------------------------------------------------------------------------
// CItem::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CItemMini::ConstructL()
    {
    }
 
// -----------------------------------------------------------------------------
// CItemMini::~CItemMini
// Deconstructor.
// -----------------------------------------------------------------------------
//
CItemMini::~CItemMini()
    {
    }

// -----------------------------------------------------------------------------
// CItemMini::Id
//
// Returns the item's id.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CItemMini::Id() const
    {
    return iId;
    }

// -----------------------------------------------------------------------------
// CItemMini::Title
//
// Returns the item's title.
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CItemMini::Title() const
    {
    return iTitle;
    }

// -----------------------------------------------------------------------------
// CItemMini::SetAttribute
//
// Sets an attribute.
// -----------------------------------------------------------------------------
//
void CItemMini::SetAttribute(TUint aAttribute, const TPtrC& aAttributeValue)
    {
    switch (aAttribute)
        {
        case EFolderAttributeMiniItemTitle:
            iTitle.Set(aAttributeValue);
            break;

        case EFolderAttributeMiniItemId:
            {
            TLex16  lex(aAttributeValue);
            TInt64  id;
              
            lex.Val(id);
            iId = id;
            }
            break;

        default:
            break;
        }
    }

