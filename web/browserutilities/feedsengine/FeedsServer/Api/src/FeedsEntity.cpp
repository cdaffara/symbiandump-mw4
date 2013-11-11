/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  The client side connection to the FeedsSever.
 *
*/


#include "FeedsEntity.h"
#include "FeedsMap.h"
#include "PackedAttributes.h"
#include "FeedAttributes.h"
#include <FolderAttributes.h>
#include "Packed.h"
#include "PackedFeed.h"
#include "PackedFolder.h"
#include "FeedsInterface.h"

// Constant definitions
_LIT(KNew, "new");
_LIT(KRead, "read");
_LIT(KUnread, "unread");

// -----------------------------------------------------------------------------
// CFeedsEntity::ChangeValueL
//
// Changes attribute/Property values based on passed Map
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CFeedsEntity::ChangeValueL(const CFeedsMap& aNewValues)
    {
    return iFeedInterface->SetAttributeL(*this, aNewValues);
    } 
// -----------------------------------------------------------------------------
// CFeedsEntity::GetStringValue
//
// Returns String value based on passed attribute.
// -----------------------------------------------------------------------------
//

EXPORT_C TInt CFeedsEntity::GetStringValue(TUint anAttribute, TPtrC& aValue) const
    {
    return iAttrProp->GetStringAttribute(anAttribute, aValue);
    }
// -----------------------------------------------------------------------------
// CFeedsEntity::GetIntegerValue
//
// Initialises aValue with integer value of passed anAttribute.
// -----------------------------------------------------------------------------
//

EXPORT_C TInt CFeedsEntity::GetIntegerValue(TUint anAttribute, TInt& aValue) const
    {
    if(EFeedAttributeFeedId == anAttribute)
        {
        aValue = iFeedId;
        return KErrNone;    
        }
    else
        return iAttrProp->GetIntegerAttribute(anAttribute, aValue);
    }
// -----------------------------------------------------------------------------
// CFeedsEntity::GetTimeValue
//
// Initialises aValue with time value of passed anAttribute.
// -----------------------------------------------------------------------------
//

EXPORT_C TInt CFeedsEntity::GetTimeValue(TUint anAttribute, TTime& aValue) const
    {
    return iAttrProp->GetTimeAttribute(anAttribute, aValue);
    }

// -----------------------------------------------------------------------------
// CFeedsEntity::NewL
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CFeedsEntity* CFeedsEntity::NewL(RFeedsInterface* aFeedsInterface)
    {
    CFeedsEntity* self = new (ELeave) CFeedsEntity();

    CleanupStack::PushL (self);
    self->ConstructL ();
    self->SetFeedInterface (aFeedsInterface);
    CleanupStack::Pop ();

    return self;
    }

// -----------------------------------------------------------------------------
// CFeedsEntity::NewL
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CFeedsEntity* CFeedsEntity::NewL(CPackedFolder* aPackedFolder,
        RFeedsInterface* aFeedsInterface)
    {
    CFeedsEntity* self = new (ELeave) CFeedsEntity(aPackedFolder);

    CleanupStack::PushL (self);
    self->SetFeedInterface (aFeedsInterface);
    self->ConstructL (*aPackedFolder);
    CleanupStack::Pop ();

    return self;
    }

// -----------------------------------------------------------------------------
// CFeedsEntity::NewL
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CFeedsEntity* CFeedsEntity::NewL(CPackedFeed* aPackedFeed,
        RFeedsInterface* aFeedsInterface)
    {
    CFeedsEntity* self = new (ELeave) CFeedsEntity(aPackedFeed);

    CleanupStack::PushL (self);
    self->SetFeedInterface (aFeedsInterface);
    self->ConstructL (*aPackedFeed);
    CleanupStack::Pop ();

    return self;
    }

// -----------------------------------------------------------------------------
// CFeedsEntity::NewFolderL
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CFeedsEntity* CFeedsEntity::NewFolderL(const TDesC& aTitle, TInt aEntryId, TInt aStatusCode)
    {
    CFeedsEntity* self = new (ELeave) CFeedsEntity();

    CleanupStack::PushL (self);
    self->ConstructL ();
    CleanupStack::Pop ();

    self->iId = aEntryId;
    self->iType = EFolder;
    self->iAttrProp->SetStringAttribute (EFolderAttributeTitle, aTitle);
    self->iAttrProp->SetIntegerAttribute(EFolderAttributeStatus,aStatusCode);

    return self;
    }

// -----------------------------------------------------------------------------
// CFeedsEntity::NewFeedL
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CFeedsEntity* CFeedsEntity::NewFeedL( const TDesC& aTitle, const TDesC& aUrl,
        TInt aEntryId, TInt aFeedId, const TTime& aTimestamp, TInt aStatusCode, TInt aFreq,
        const TInt& aUnreadCount)
    {
    CFeedsEntity* self = new (ELeave) CFeedsEntity();

    CleanupStack::PushL (self);
    self->ConstructL ();
    CleanupStack::Pop ();

    self->iId = aEntryId;
    self->iType = EFeed;
    self->iAttrProp->SetStringAttribute (EFeedAttributeTitle, aTitle);
    self->iAttrProp->SetStringAttribute (EFeedAttributeLink, aUrl);
    self->iAttrProp->SetTimeAttribute (EFeedAttributeTimestamp, aTimestamp);
    self->iAttrProp->SetIntegerAttribute (EFeedAttributeAutoUpdateFreq, aFreq);
    self->iAttrProp->SetIntegerAttribute (EFolderAttributeUnreadCount,
    		aUnreadCount);
    self->iFeedId = aFeedId;
    self->iAttrProp->SetIntegerAttribute(EFolderAttributeStatus,aStatusCode);
    return self;
    }
	
// -----------------------------------------------------------------------------
// CFeedsEntity::NewMiniItemL
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CFeedsEntity* CFeedsEntity::NewMiniItemL()
    {
    CFeedsEntity* self = new (ELeave) CFeedsEntity();

    CleanupStack::PushL (self);
    self->ConstructL ();
    CleanupStack::Pop ();

    self->iType = EMiniItem;

    return self;
    }

// -----------------------------------------------------------------------------
// CFeedsEntity::CFeedsEntity
// C++ default constructor can NOT contain any code, that
// might leave.    aPackedFolder is adopted 
// -----------------------------------------------------------------------------
//
CFeedsEntity::CFeedsEntity(CPackedFolder* aPackedFolder) :
    iPacked((CPacked*)aPackedFolder)
    {
    }

// -----------------------------------------------------------------------------
// CFeedsEntity::CFeedsEntity
// C++ default constructor can NOT contain any code, that
// might leave.    aPackedFolder is adopted 
// -----------------------------------------------------------------------------
//
CFeedsEntity::CFeedsEntity(CPackedFeed* aPackedFeed) :
    iPacked((CPacked*)aPackedFeed)
    {
    }

// -----------------------------------------------------------------------------
// CFeedsEntity::CFeedsEntity
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CFeedsEntity::CFeedsEntity()
    {
    }

// -----------------------------------------------------------------------------
// CFeedsEntity::ConstructL
// Symbian 2nd phase constructor can leave.
// by this method.
// -----------------------------------------------------------------------------
//
void CFeedsEntity::ConstructL()
    {
    iAttrProp = CFeedsMap::NewL ();
    }

CFeedsEntity::~CFeedsEntity()
    {
    delete iAttrProp;
    iAttrProp = NULL;
    iChildren.ResetAndDestroy ();
    }

// -----------------------------------------------------------------------------
// CFeedsEntity::ConstructL
// Symbian 2nd phase constructor can leave.
// by this method.
// -----------------------------------------------------------------------------
//
void CFeedsEntity::ConstructL(const CPackedFolder& aPackedFolder)
    {
    CFeedsEntity* rootFolder = NULL;
    CFeedsEntity* currentFolder = NULL;
    CFeedsEntity* folderItem = NULL;
    CFeedsEntity* miniItem = NULL;
    TPtrC title;
    TPtrC url;
    TInt entryId;
    TInt feedId;
    TTime timestamp;
    TInt freq;
    TInt unreadCount;
    TInt status; 
    TBool isMiniItem = EFalse;
    TUint attribute;
    //	CItemMini* miniItem = NULL;
    TPtrC attributeValue;

    ConstructL ();

    // Unpack the packed folder.
    while (aPackedFolder.HasNextToken ())
        {
        TUint token;

        token = aPackedFolder.NextToken ();

        switch (token)
            {
            case EFolderTokenFolderBegin:
                // Get the folder item's attributes.
                aPackedFolder.ExtractAttributes (title, url, entryId, feedId,
                        timestamp, unreadCount, status, freq);

                // CPackedFolders contain a single root folder.  Because of this
                // the first folder should be treated as the root.  "this" 
                // becomes the root CFeedsEntity.
                if ( rootFolder == NULL)
                    {
                    iId = entryId;
                    iType = EFolder;
                    iParent = NULL;
                    iAttrProp->SetStringAttribute (EFolderAttributeTitle, title);
                    iAttrProp->SetIntegerAttribute(EFolderAttributeStatus,status);
                    rootFolder = this;
                    currentFolder = this;
                    }

            	// Otherwise create a subfolder and set its parent.
            	else
                    {
                    folderItem = CFeedsEntity::NewFolderL (title, entryId, status);
                    folderItem->SetFeedInterface (iFeedInterface);
                    // Add the folder to the parent.
                    User::LeaveIfError (currentFolder->iChildren.Append (folderItem));
                    folderItem->iParent = currentFolder;

                    // Set this folder as the active folder.
                    currentFolder = folderItem;
                    }
                break;

            case EFolderTokenFolderEnd:
                // Set the current folder to its parent.
                currentFolder = const_cast<CFeedsEntity*>(currentFolder->iParent);
                break;

            case EFolderTokenFeed:
                // Get the folder item's attributes.
                aPackedFolder.ExtractAttributes (title, url, entryId, feedId,
                        timestamp, unreadCount, status, freq);

                // Create the new feed and add it to the current folder.
                // folderListId is shared by all feeds/folders
                folderItem = CFeedsEntity::NewFeedL (title, url, entryId,
                        feedId, timestamp, status, freq, unreadCount );
                folderItem->SetFeedInterface (iFeedInterface);
                // Add the folder to the parent.
                User::LeaveIfError (currentFolder->iChildren.Append (folderItem));
                folderItem->iParent = currentFolder;
                break;

            case EFolderTokenItemBegin:
                miniItem = CFeedsEntity::NewMiniItemL();
            //	miniItem->iType = EMiniItem;
                CleanupStack::PushL(miniItem);
                isMiniItem = ETrue;
                break;

            case EFolderTokenItemEnd:
                isMiniItem = EFalse;         
                miniItem->SetFeedInterface (iFeedInterface);       
            //    folderItem->AddMiniItem(miniItem);
                User::LeaveIfError (currentFolder->iChildren.Append (miniItem));
               // CleanupStack::Pop(miniItem);
            	break;

            case EPackedTokenAttribute:
                aPackedFolder.ExtractAttributeValue(attribute, attributeValue);
                    
                if (isMiniItem)
                    {
                    miniItem->SetAttribute(attribute, attributeValue);
                    }
                else
                    {
                    // TODO: Panic.
                    }
                break;			

            	
            }
        }
    }

// -----------------------------------------------------------------------------
// CFeedsEntity::ConstructL
// Symbian 2nd phase constructor can leave.
// by this method.
// -----------------------------------------------------------------------------
//
void CFeedsEntity::ConstructL(const CPackedFeed& aPackedFeed)
    {
    TBool isFeed = EFalse;
    TBool isItem = EFalse;
    TBool isEnclosure = EFalse;

    CFeedsEntity* item = NULL;
    CFeedsEntity* enclosure = NULL;
    TUint attribute;
    TPtrC attributeValue;
    ConstructL ();

    // Unpack the packed feed.
    while (aPackedFeed.HasNextToken ())
        {
        TUint token;

        token = aPackedFeed.NextToken ();

        switch (token)
            {
            case EFeedTokenFeedBegin:
                isFeed = ETrue;
                break;

            case EFeedTokenItemBegin:
                item = CFeedsEntity::NewL (iFeedInterface);
                item->iType = EFeedsItem;
                item->iParent = this;
                CleanupStack::PushL (item);
                isItem = ETrue;
                break;

            case EFeedTokenEnclosureBegin:
                enclosure = CFeedsEntity::NewL (iFeedInterface);
                enclosure->iType = EFeedsEnclosure;
                enclosure->iParent = item;
                CleanupStack::PushL (enclosure);
                isEnclosure = ETrue;
                break;

            case EFeedTokenFeedEnd:
                isFeed = EFalse;
                break;

            case EFeedTokenItemEnd:
                isItem = EFalse;
                User::LeaveIfError (iChildren.Append (item));
                CleanupStack::Pop (item);
                break;

            case EFeedTokenEnclosureEnd:
                isEnclosure = EFalse;
                User::LeaveIfError (item->iChildren.Append (enclosure));
                CleanupStack::Pop (enclosure);
                break;

            case EPackedTokenAttribute:
                aPackedFeed.ExtractAttributeValue (attribute, attributeValue);
                if ( isEnclosure)
                    {
                    enclosure->SetAttribute (attribute, attributeValue);
                    }
                else
                    if ( isItem)
                        {
                        item->SetAttribute (attribute, attributeValue);
                        }
                    else
                        if ( isFeed)
                            {
                            SetAttribute (attribute, attributeValue);
                            }
                        else
                            {
                            // TODO: Panic.
                            }
                break;

            case EFeedTokenOtherTitle:
                //iOtherTitles++;
                break;

            case EFeedTokenOtherDescription:
                //iOtherDescriptions++;
                break;

            case EFeedTokenOtherLink:
                //iOtherLinks++;
                break;

            default:
                // Ignore
                break;
            };
        }
    }

// -----------------------------------------------------------------------------
// CFeedsEntity::SetAttribute
//
// adds given attribute in iAttrProp Map
// -----------------------------------------------------------------------------
//
void CFeedsEntity::SetAttribute(TUint aAttribute, const TPtrC& aAttributeValue)
    {
    switch(iType)
    {
        case EFeed:
            {
            switch(aAttribute)
                {
                case EFeedAttributeUnused:
                case EFeedAttributeTitle:
                case EFeedAttributeLink:
                case EFeedAttributeDescription:
                case EFeedAttributeTimestamp:
                case EFeedAttributeImage:
                case EFeedAttributeTTL:
                case EFeedAttributeSkipHours:
                case EFeedAttributeSkipDays:
                case EFeedAttributeLanguage:
                case EFeedAttributeCopyright:
                case EFeedAttributeEditor:
                case EFeedAttributeWebMaster:
                case EFeedAttributePubDate:
                case EFeedAttributeCategory:
                case EFeedAttributeGenerator:
                case EFeedAttributeDocs:
                case EFeedAttributeCloud:
                case EFeedAttributeRating:
                case EFeedAttributeTextInput:
                case EFeedAttributeAssociatedUrl:
                case EFeedAttributeFeedUrl:
                    iAttrProp->SetStringAttribute (aAttribute, aAttributeValue);
                    break; 
                case EFeedAttributeFeedId:
                    {
                    TLex lex(aAttributeValue);
                    lex.Val (iFeedId);
                    break;
                    }                    
                case EFeedAttributeAutoUpdateFreq:
                    {
                    iAttrProp->SetStringAttribute (aAttribute, aAttributeValue);
                    break;
                    }
                }
                break;
            }
        case EFeedsItem:
            {
            switch(aAttribute)
                {
                case EItemAttributeTitle:
                case EItemAttributeLink:
                case EItemAttributeDescription:
                case EItemAttributeIdStr:
                case EItemAttributeEnclosure:
                case EItemAttributeTimestamp:
                case EItemAttributeImage:
                case EItemAttributeAuthor:
                case EItemAttributeCategory:
                case EItemAttributeComments:
                case EItemAttributeSource:
                    iAttrProp->SetStringAttribute (aAttribute, aAttributeValue);
                    break;

                case EItemAttributeStatus:
                    {
                    TInt itemStatus(EItemStatusUndefined);
                    if ( aAttributeValue.CompareF (KNew)== 0)
                        {
                        itemStatus = EItemStatusNew;
                        }
                    else
                        if ( aAttributeValue.CompareF (KRead)== 0)
                            {
                            itemStatus = EItemStatusRead;
                            }
                        else
                            if ( aAttributeValue.CompareF (KUnread)== 0)
                                {
                                itemStatus = EItemStatusUnread;
                                }
                    iAttrProp->SetIntegerAttribute (aAttribute, itemStatus);
                    }
                    break;

                case EItemAttributeItemId:
                    {
                    TLex lex(aAttributeValue);
                    lex.Val (iId);
                    break;
                    }
               }
               break;        
            }
        case EFeedsEnclosure:
            {
            switch(aAttribute)
                {
                case EEnclosureAttributeContentType:
                case EEnclosureAttributeSize:
                case EEnclosureAttributeLink:
                case EEnclosureAttributeTitle:
                     iAttrProp->SetStringAttribute (aAttribute, aAttributeValue);
                     break;
                }
                break;
            }
        case EMiniItem:
            {
            switch(aAttribute)
                {
                case EFolderAttributeMiniItemTitle:
                    iAttrProp->SetStringAttribute (aAttribute, aAttributeValue);
                    break;
                case EFolderAttributeMiniItemId: 
                    {
                    TLex16  lex(aAttributeValue);
                    lex.Val(iId);
                    break;                
                    }
                }
              break;
            }
        case EFolder:
        default:
            {
            switch(aAttribute)
                {
                case EFolderAttributeUnreadCount:
                case EFolderAttributeTitle:
                    iAttrProp->SetStringAttribute (aAttribute, aAttributeValue);
                    break;
                }
              break;                
            }
    }
   }
   

// -----------------------------------------------------------------------------
// CFeedsEntity::GetType
//@return TFeedsEntityType enum
// Gets type of CFeedsEntity (Where this represents folder/feeds/items/enclosures)
// -----------------------------------------------------------------------------
//   
EXPORT_C TFeedsEntityType CFeedsEntity::GetType() const
    {
    return iType;
    }

// -----------------------------------------------------------------------------
// CFeedsEntity::GetType
//
//Gets ID of feeds entity (This ID is actually unique integer value)
// -----------------------------------------------------------------------------
//      
EXPORT_C TInt CFeedsEntity::GetId() const
  	{
  	return iId;
  	}
  
// -----------------------------------------------------------------------------
// CFeedsEntity::GetParent
//
// Returns Parent of this feeds entity (NULL if it is root folder)
// -----------------------------------------------------------------------------
//    
EXPORT_C CFeedsEntity* CFeedsEntity::GetParent() const
    {
    return iParent;
    }

// -----------------------------------------------------------------------------
// CFeedsEntity::GetChildren
//
// Returns RPointerArray containing all children of this CFeedsEntity
// -----------------------------------------------------------------------------
// 
EXPORT_C const RPointerArray<CFeedsEntity>& CFeedsEntity::GetChildren() const
    {
    return iChildren;
    }
