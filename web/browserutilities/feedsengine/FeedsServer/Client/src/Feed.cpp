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
* Description:  Holds information about a feed.
*
*/


//#ifdef _DEBUG
#include <f32file.h>
//#endif

#include "FeedAttributes.h"
#include "FeedsServerFeed.h"
#include "Logger.h"

// -----------------------------------------------------------------------------
// CFeed::NewL
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CFeed* CFeed::NewL(CFeedsEntity* aFeedsEntity)
    {
    CFeed* self = new (ELeave) CFeed(aFeedsEntity);

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CFeed::CFeed
//
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CFeed::CFeed(CFeedsEntity* aFeedsEntity):
        iLeakTracker(CLeakTracker::EFeed), iFeedsEntity(aFeedsEntity), iTitle(KNullDesC), 
        iDescription(KNullDesC), iUrl(KNullDesC), iDate(0), iItems(5)
    {
    }

// -----------------------------------------------------------------------------
// CFeed::ConstructL
//
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CFeed::ConstructL()
    {
    CEnclosure*  enclosure = NULL;
    TPtrC        title;
    TPtrC        description;
    TPtrC        url;   
    TInt         feedId;    
    TInt         itemCount  = iFeedsEntity->GetChildren().Count();


    for(TInt i=0 ; i< itemCount ; i++)
        {
        CFeedsEntity *feedEntity = iFeedsEntity->GetChildren()[i];
        CItem *newItem = CItem::NewL( feedEntity);
        CleanupStack::PushL( newItem);
        TInt   enclosureCount = feedEntity->GetChildren().Count();
 
        for(TInt j=0 ; j< enclosureCount ; j++)
             {
             enclosure = CEnclosure::NewL( feedEntity->GetChildren()[j]);
             CleanupStack::PushL( enclosure);
             newItem->AddEnclosureL( enclosure);
             CleanupStack::Pop( enclosure);
             }                 

        iItems.Append( newItem);
        CleanupStack::Pop( newItem);
        }

    iFeedsEntity->GetIntegerValue( EFeedAttributeFeedId, feedId);
    iFeedId = feedId;

    iFeedsEntity->GetStringValue( EFeedAttributeTitle, title);
    SetAttribute( EFeedAttributeTitle, title);

    iFeedsEntity->GetStringValue( EFeedAttributeDescription, description);
    SetAttribute( EFeedAttributeDescription, description);

    iFeedsEntity->GetStringValue( EFeedAttributeLink, url);
    SetAttribute( EFeedAttributeLink,url);           

    iFeedsEntity->GetTimeValue( EFeedAttributeTimestamp, iDate);   

    }


// -----------------------------------------------------------------------------
// CFeed::~CFeed
//
// Deconstructor.
// -----------------------------------------------------------------------------
//
CFeed::~CFeed()
    {
    // Delete the items.
    iItems.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CFeed::Id
//
// Returns the feed's unique id.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CFeed::Id() const
    {
    return iFeedId;
    }

// -----------------------------------------------------------------------------
// CFeed::Title
//
// Returns the feed's title.
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CFeed::Title() const
    {
    return iTitle;
    }

// -----------------------------------------------------------------------------
// CFeed::Description
//
// Returns the feed's description.
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CFeed::Description() const
    {
    return iDescription;
    }

// -----------------------------------------------------------------------------
// CFeed::Url
//
// Returns the feed's url.
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CFeed::Url() const
    {
    return iUrl;
    }

// -----------------------------------------------------------------------------
// CFeed::Date
//
// Returns the feed's date.
// -----------------------------------------------------------------------------
//
EXPORT_C const TTime& CFeed::Date() const
    {
    return iDate;
    }

// -----------------------------------------------------------------------------
// CFeed::ItemCount
//
// Returns the number of items.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CFeed::ItemCount() const
    {
    return iItems.Count();
    }

// -----------------------------------------------------------------------------
// CFeed::ItemAt
//
// Returns the item at the given index
// -----------------------------------------------------------------------------
//
EXPORT_C const CItem& CFeed::ItemAt(TInt aIndex) const
    {
    return *(iItems[aIndex]);
    }

// -----------------------------------------------------------------------------
// CFeed::ItemStatusL
//
// Returns the INITIAL status of each of the items in the feed.
// The caller can then modify the values and call RFeed::UpdateFeedItemStatusL
// to request the feeds server to update the feed's item status.
// -----------------------------------------------------------------------------
//
EXPORT_C void CFeed::ItemStatusL(RArray<TInt>& aItemIds, 
        RArray<TItemStatus>& aItemStatus) const
    {
    aItemIds.Reset();
    aItemStatus.Reset();

    // Add the item attributes.
    for (TInt i = 0; i < iItems.Count(); i++)
        {
        User::LeaveIfError(aItemIds.Append(iItems[i]->Id()));
        User::LeaveIfError(aItemStatus.Append(iItems[i]->ItemStatus()));
        }
    }

// -----------------------------------------------------------------------------
// CFeed::AddItemL
//
// Appends the new Item to the Feed.  aItem is adopted by this method.
// -----------------------------------------------------------------------------
//
void CFeed::AddItemL(CItem* aItem)
    {
    User::LeaveIfError(iItems.Append(aItem));
    }

// -----------------------------------------------------------------------------
// CFeed::SetAttribute
//
// Sets an attribute.
// -----------------------------------------------------------------------------
//
void CFeed::SetAttribute(TUint aAttribute, const TPtrC& aAttributeValue)
    {
    switch (aAttribute)
        {
        case EFeedAttributeFeedId:
            {
            TLex16  lex(aAttributeValue); 

            lex.Val(iFeedId);
            }
            break;

        case EFeedAttributeTitle:
            iTitle.Set(aAttributeValue);
            break;

        case EFeedAttributeDescription:
            iDescription.Set(aAttributeValue);
            break;

        case EFeedAttributeLink:
            iUrl.Set(aAttributeValue);
            break;

        case EFeedAttributeTimestamp:
            {
            TLex16  lex(aAttributeValue); 
            TInt64  val;
            lex.Val(val);                
            iDate = val;
            }
            break;

        default:
            break;
        }
    }

//#ifdef _DEBUG
// -----------------------------------------------------------------------------
// CFeed::Debug_Print
//
// Prints the item to the log file.
// -----------------------------------------------------------------------------
//
EXPORT_C void CFeed::Debug_Print(void) const
    {
    FEED_LOG(_L("Feeds"), _L("Feeds_Dump.log"), 
        EFileLoggingModeAppend, _L("\tFeed:"));

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

    // Print the items.
    FEED_LOG(_L("Feeds"), _L("Feeds_Dump.log"), 
        EFileLoggingModeAppend, _L("\tItems:"));

    for (TInt i = 0; i < iItems.Count(); i++)
        {
        iItems[i]->Debug_Print();
        }

    FEED_LOG(_L("Feeds"), _L("Feeds_Dump.log"), 
        EFileLoggingModeAppend, _L(""));
    FEED_LOG(_L("Feeds"), _L("Feeds_Dump.log"), 
        EFileLoggingModeAppend, _L(""));
    }


// -----------------------------------------------------------------------------
// CFeed::Debug_PrintInfo
//
// Prints info about the feed.
// -----------------------------------------------------------------------------
//
EXPORT_C void CFeed::Debug_PrintInfo(const TDesC& aUrl, RFile& aFile) const
    {
    TInt            channels = 1;
    TInt            titles = 0;
    TInt            descriptions = 0;
    TInt            items = 0;
    TInt            links = 0;
     
    // Count the "unimportant" attributes.
    titles = iOtherTitles;
    links = iOtherLinks;
    descriptions = iOtherDescriptions;
    
    // Add the Channel attributes
    if (iTitle.Length() > 0)
        {
        titles++;
        }
    if (iDescription.Length() > 0)
        {
        descriptions++;
        }
    if (iUrl.Length() > 0)
        {
        links++;
        }

    // Add the item attributes.
    for (TInt i = 0; i < iItems.Count(); i++)
        {
        CItem*  item;

        item = iItems[i];
        items++;

        if (item->Title().Length() > 0)
            {
            titles++;
            }
        if (item->Description().Length() > 0)
            {
            descriptions++;
            }
        if (item->Url().Length() > 0)
            {
            links++;
            }
        }

    // Write out the stats.
    TBuf8<100>    buff;

    aFile.Write(_L8("Feed Overview:\n"));

    // Write out the url.
    TUint8* asciiBuf = new TUint8[aUrl.Length() + 1];

    if (asciiBuf != NULL)
        {
        TPtr8 asciiPtr(asciiBuf, 0, aUrl.Length() + 1);
        TPtrC16 ucs2Ptr(aUrl);

        asciiPtr.Copy(ucs2Ptr);
        asciiPtr.ZeroTerminate();

        aFile.Write(_L8("\tfile: "));
        aFile.Write(asciiPtr);
        aFile.Write(_L8("\n"));

        delete asciiBuf;
        }

    buff.Format(_L8("\tchannel: %d\n"), channels);
    aFile.Write(buff);

    buff.Format(_L8("\tdescription: %d\n"), descriptions);
    aFile.Write(buff);

    buff.Format(_L8("\titem: %d\n"), items);
    aFile.Write(buff);

    buff.Format(_L8("\tlink: %d\n"), links);
    aFile.Write(buff);

    buff.Format(_L8("\ttitle: %d\n"), titles);
    aFile.Write(buff);

    buff.Format(_L8("\r\n"));
    aFile.Write(buff);
    }
//#endif
