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
* Description:  Is a serialized version of CFeed suitable for transfer between
*                client and server.
*
*/


#ifndef PACKED_FEED_H
#define PACKED_FEED_H


// INCLUDES
#include <e32base.h>

#include "LeakTracker.h"
#include "Packed.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION


/**
*  Is a serialized version of CFeed suitable for transfer between client and server.
*
*  \b Library: FeedsEngine.lib
*
*  @since 3.1
*/
class CPackedFeed: public CPacked
    {
    public: // Construct/Destructor
        /**
        * Two-phased constructor.
        */
        static CPackedFeed* NewL();

        /**
        * Destructor.
        */        
        virtual ~CPackedFeed();
        
        
    public: // New Methods -- server side methods.
        /**
        * Insert the begining of a feed.
        *
        * @since 3.1
        * @return Void
        */
        void FeedBeginsL();
        
        /**
        * Insert the ending of a feed.
        *
        * @since 3.1
        * @return Void
        */
        void FeedEndsL();
        
        /**
        * Insert the begining of a item.
        *
        * @since 3.1
        * @return Void
        */
        void ItemBeginsL();
        
        /**
        * Insert the ending of a item.
        *
        * @since 3.1
        * @return Void
        */
        void ItemEndsL();
        
        /**
        * Insert the begining of a enclosure.
        *
        * @since 3.1
        * @return Void
        */
        void EnclosureBeginsL();
        
        /**
        * Insert the ending of a enclosure.
        *
        * @since 3.1
        * @return Void
        */
        void EnclosureEndsL();
        
        /**
        * Insert an unimportant token.  The client can use this 
        * information for feed validation.
        *
        * @since 3.1
        * @return void.
        */
        void OtherTitleL();

        /**
        * Insert an unimportant token.  The client can use this 
        * information for feed validation.
        *
        * @since 3.1
        * @return void.
        */
        void OtherDescriptionL();

        /**
        * Insert an unimportant token.  The client can use this 
        * information for feed validation.
        *
        * @since 3.1
        * @return void.
        */
        void OtherLinkL();


    protected:  // New Methods
        /**
        * C++ default constructor.
        */
        CPackedFeed(TInt aTokenArrayInc, TInt aStringTableInc);
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
    private:
        TLeakTracker  iLeakTracker;
    };


#endif      // PACKED_FEED_H
            
// End of File