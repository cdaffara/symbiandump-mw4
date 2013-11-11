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
* Description:  Contain the normalized tokens for the feed attributes.
*
*/


#ifndef FEED_ATTRIBUTES_H
#define FEED_ATTRIBUTES_H

#warning The Feeds Engine API will be removed wk25. Please see http://wikis.in.nokia.com/Browser/APIMigration for more information

// INCLUDES

// CONSTANTS

// MACROS

// DATA TYPES

enum TItemStatus
    {
    EUndefinedStatus,
    ENewItem,
    EUnreadItem,
    EReadItem
    };
    
enum TFeedAttributes
    {
    EFeedAttributeUnused         = 0,
    EFeedAttributeTitle          = 1,
    EFeedAttributeLink           = 2,
    EFeedAttributeDescription    = 3,
    EFeedAttributeTimestamp      = 4,

    EFeedAttributeImage          = 5,
    EFeedAttributeTTL            = 6,
    EFeedAttributeSkipHours      = 7,
    EFeedAttributeSkipDays       = 8,

    EFeedAttributeLanguage       = 9,
    EFeedAttributeCopyright      = 10,
    EFeedAttributeEditor         = 11,
    EFeedAttributeWebMaster      = 12,
    EFeedAttributePubDate        = 13,
    EFeedAttributeCategory       = 14,
    EFeedAttributeGenerator      = 15,
    EFeedAttributeDocs           = 16,
    EFeedAttributeCloud          = 17,
    EFeedAttributeRating         = 18,
    EFeedAttributeTextInput      = 19,

    EFeedAttributeAssociatedUrl  = 20,
    EFeedAttributeFeedUrl        = 21,
    
    EFeedAttributeFeedId         = 22,
    EFeedAttributeAutoUpdateFreq = 23
    };
    
enum TItemAttributes
    {
    EItemAttributeTitle          = 0,
    EItemAttributeLink           = 1,
    EItemAttributeDescription    = 2,
    EItemAttributeIdStr          = 3,
    EItemAttributeEnclosure      = 4,
    EItemAttributeTimestamp      = 5,
    
    EItemAttributeImage          = 6,
    EItemAttributeAuthor         = 7,
    EItemAttributeCategory       = 8,
    EItemAttributeComments       = 9,
    EItemAttributeSource         = 10,

    EItemAttributeStatus         = 11,
    EItemAttributeItemId         = 12
    };
    
enum TEnclosureAttributes
    {
    EEnclosureAttributeContentType    = 0,
    EEnclosureAttributeSize           = 1,
    EEnclosureAttributeLink           = 2,
    EEnclosureAttributeTitle          = 3    
    };
    
    
// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION


#endif      // FEED_ATTRIBUTES_H
            
// End of File
