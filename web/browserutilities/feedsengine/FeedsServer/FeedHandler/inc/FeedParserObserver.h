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
* Description:  Feeds parser observer.
*
*/


#ifndef FEED_PARSER_OBSERVER_H
#define FEED_PARSER_OBSERVER_H


// INCLUDES
#include <e32std.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION


/**
*  The feed parser observer.
*
*  \b Library: FeedsEngine.lib
*
*  @since 3.1
*/
class MFeedParserObserver
    {
    public:
        /**
        * The beginning of a feed was found.
        *
        * @since 3.1
        * @return void.
        */
        virtual void FeedBeginsL() = 0;

        /**
        * The end of a feed was found.
        *
        * @since 3.1
        * @return void.
        */
        virtual void FeedEndsL() = 0;

        /**
        * The beginning of a item was found.
        *
        * @since 3.1
        * @return void.
        */
        virtual void ItemBeginsL() = 0;

        /**
        * The end of a item was found.
        *
        * @since 3.1
        * @return void.
        */
        virtual void ItemEndsL() = 0;

        /**
        * The beginning of a enclosure was found.
        *
        * @since 3.1
        * @return void.
        */
        virtual void EnclosureBeginsL() = 0;

        /**
        * The end of a enclosure was found.
        *
        * @since 3.1
        * @return void.
        */
        virtual void EnclosureEndsL() = 0;

        /**
        * A attribute was found.  This attribute should be applied to
        * the enclosing entity (Feed, Item, or Enclosure).
        *
        * @since 3.1
        * @return void.
        */
        virtual void AddAttributeL(TInt aAttribute, const TDesC& aValue) = 0;
        
        /**
        * An unimportant element was found.  The client can use this 
        * information for feed validation.
        *
        * @since 3.1
        * @return void.
        */
        virtual void OtherTitleL() = 0;

        /**
        * An unimportant element was found.  The client can use this 
        * information for feed validation.
        *
        * @since 3.1
        * @return void.
        */
        virtual void OtherDescriptionL() = 0;

        /**
        * An unimportant element was found.  The client can use this 
        * information for feed validation.
        *
        * @since 3.1
        * @return void.
        */
        virtual void OtherLinkL() = 0;
    };


#endif      // FEED_PARSER_OBSERVER_H
            
// End of File