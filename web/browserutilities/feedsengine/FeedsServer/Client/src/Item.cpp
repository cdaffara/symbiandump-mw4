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
* Description:  Holds information about a feed item.
*
*/


#include "FeedAttributes.h"
#include "FeedsServerFeed.h"
#include "Logger.h"

// Constants
_LIT(KNew, "new");
_LIT(KRead, "read");
_LIT(KUnread, "unread");


// -----------------------------------------------------------------------------
// CItem::NewL
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CItem* CItem::NewL(CFeedsEntity* aFeedsEntity)
    {
    CItem* self = new (ELeave) CItem(aFeedsEntity);

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CItem::CItem
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CItem::CItem(CFeedsEntity* aFeedsEntity):iLeakTracker(CLeakTracker::EItem),
        iFeedsEntity(aFeedsEntity), iTitle(KNullDesC), 
        iDescription(KNullDesC), iUrl(KNullDesC), iItemStatus(EUnreadItem), iEnclosures(5)
    {
    }

// -----------------------------------------------------------------------------
// CItem::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CItem::ConstructL()
    {
    TPtrC   url;    
    TPtrC   description; 
    TPtrC   title;
    TInt    item;

    iFeedsEntity->GetTimeValue( EItemAttributeTimestamp, iTimestamp);

    iFeedsEntity->GetStringValue( EItemAttributeLink, url); 
    SetAttribute( EItemAttributeLink, url);

    iFeedsEntity->GetStringValue( EItemAttributeDescription, description);
    SetAttribute( EItemAttributeDescription, description);    

    iFeedsEntity->GetStringValue( EItemAttributeTitle, title); 
    SetAttribute( EItemAttributeTitle, title);

    iFeedsEntity->GetIntegerValue( EItemAttributeStatus, item);

    switch(item)
        {
        case EItemStatusNew:
             iItemStatus = ENewItem;
             break;

        case EItemStatusRead:
             iItemStatus = EReadItem;
             break;

        case EItemStatusUnread:
             iItemStatus = EUnreadItem;
             break;
        }

    iId = iFeedsEntity->GetId();
    }

// -----------------------------------------------------------------------------
// CItem::~CItem
// Deconstructor.
// -----------------------------------------------------------------------------
//
CItem::~CItem()
    {
    // Delete the enclosures.
    iEnclosures.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CItem::Id
//
// Returns the item's id.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CItem::Id() const
    {
    return iId;
    }

// -----------------------------------------------------------------------------
// CItem::Title
//
// Returns the item's title.
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CItem::Title() const
    {
    return iTitle;
    }

// -----------------------------------------------------------------------------
// CItem::Description
//
// Returns the item's description.
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CItem::Description() const
    {    
    return iDescription;
    }

// -----------------------------------------------------------------------------
// CItem::Url
//
// Returns the item's url.
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CItem::Url() const
    {              
    return iUrl;
    }

// -----------------------------------------------------------------------------
// CItem::Date
//
// Returns the item's date.
// -----------------------------------------------------------------------------
//
EXPORT_C const TTime& CItem::Timestamp() const
    {
    return iTimestamp;
    }

// -----------------------------------------------------------------------------
// CItem::ItemStatus
//
// Returns the item's status.
// -----------------------------------------------------------------------------
//
EXPORT_C TItemStatus CItem::ItemStatus() const
    {
    return iItemStatus;
    }

// -----------------------------------------------------------------------------
// CItem::EnclosureCount
//
// Returns the number of enclosures.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CItem::EnclosureCount() const
    {
    return iEnclosures.Count();
    }

// -----------------------------------------------------------------------------
// CItem::ItemAt
//
// Returns the enclosure at the given index
// -----------------------------------------------------------------------------
//
EXPORT_C const CEnclosure& CItem::EnclosureAt(TInt aIndex) const
    {
    // TODO: assert if out of range.
    return *(iEnclosures[aIndex]);
    }

// -----------------------------------------------------------------------------
// CItem::AddEnclosureL
//
// Appends the new enclosure to the Feed.  aItem is adopted by this method.
// -----------------------------------------------------------------------------
//
void CItem::AddEnclosureL(CEnclosure* aEnclosure)
    {
    User::LeaveIfError(iEnclosures.Append(aEnclosure));
    }

// -----------------------------------------------------------------------------
// CItem::SetAttribute
//
// Sets an attribute.
// -----------------------------------------------------------------------------
//
void CItem::SetAttribute(TUint aAttribute, const TPtrC& aAttributeValue)
    {
    switch (aAttribute)
        {
        case EItemAttributeTitle:
            iTitle.Set(aAttributeValue);
            break;

        case EItemAttributeDescription:
            iDescription.Set(aAttributeValue);
            break;

        case EItemAttributeLink:
            iUrl.Set(aAttributeValue);
            break;

        case EItemAttributeStatus:
            if (aAttributeValue.CompareF(KNew) == 0)
                {
                iItemStatus = ENewItem;
                }
            else if (aAttributeValue.CompareF(KRead) == 0)
                {
                iItemStatus = EReadItem;
                }
            else if (aAttributeValue.CompareF(KUnread) == 0)
                {
                iItemStatus = EUnreadItem;
                }
            break;

        case EItemAttributeTimestamp:
            {
            TLex16  lex(aAttributeValue);
            TInt64  ts;
              
            lex.Val(ts);
            iTimestamp = ts;
            }
            break;

        case EItemAttributeItemId:
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

//#ifdef _DEBUG
// -----------------------------------------------------------------------------
// CItem::Debug_Print
//
// Prints the item to the log file.
// -----------------------------------------------------------------------------
//
EXPORT_C void CItem::Debug_Print(void) const
    {
    FEED_LOG(_L("Feeds"), _L("Feeds_Dump.log"), 
        EFileLoggingModeAppend, _L("\tItem:"));

    if (iTitle.Length() > 0)
        {
        FEED_LOG1(_L("Feeds"), _L("Feeds_Dump.log"), 
            EFileLoggingModeAppend, _L("\t\tTitle: %S"), &iTitle);
        }

    if (iUrl.Length() > 0)
        {
        FEED_LOG1(_L("Feeds"), _L("Feeds_Dump.log"), 
            EFileLoggingModeAppend, _L("\t\tLink: %S"), &iUrl);
        }

    if (iDescription.Length() > 0)
        {
        FEED_LOG1(_L("Feeds"), _L("Feeds_Dump.log"), 
            EFileLoggingModeAppend, _L("\t\tDescription: %S"), &iDescription);
        }

    switch( iItemStatus )
        {
        case ENewItem:
            {            
            FEED_LOG1(_L("Feeds"), _L("Feeds_Dump.log"), 
                EFileLoggingModeAppend, _L("\t\tStatus: %S"), &KNew());
            }
            break;
        case EReadItem:
            {            
            FEED_LOG1(_L("Feeds"), _L("Feeds_Dump.log"), 
                EFileLoggingModeAppend, _L("\t\tStatus: %S"), &KRead());
            }
            break;
        case EUnreadItem:
            {
            FEED_LOG1(_L("Feeds"), _L("Feeds_Dump.log"), 
                EFileLoggingModeAppend, _L("\t\tStatus: %S"), &KUnread());
            }
            break;
        }

    // Print the enclosures.
    if (iEnclosures.Count() > 0)
        {        
        FEED_LOG(_L("Feeds"), _L("Feeds_Dump.log"), 
            EFileLoggingModeAppend, _L("\tEnclosures:"));

        for (TInt i = 0; i < iEnclosures.Count(); i++)
            {
            iEnclosures[i]->Debug_Print();
            }
        }
    }
