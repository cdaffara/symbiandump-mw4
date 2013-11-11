/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*      Declaration of class CFavouritesItemImplList.
*      
*
*/


#ifndef FAVOURITES_ITEM_IMPL_LIST_H
#define FAVOURITES_ITEM_IMPL_LIST_H

// INCLUDE FILES

#include <e32base.h>

// FORWARD DECLARATION

class CFavouritesItemImpl;
class RWriteStream;
class RReadStream;

// CLASS DECLARATION

/**
* Array of CFavouritesItemImplImpl-s. Items are owned and deleted upon destruction.
* Note: using this array downcasted to its base can result in leaks.
* Delete method of base class is not virtual!
*/
NONSHARABLE_CLASS(CFavouritesItemImplList): public CArrayPtrFlat<CFavouritesItemImpl>
    {
    public:     // Construct / destruct

        /**
        * Constructor.
        */
        CFavouritesItemImplList();

        /**
        * Destructor. Elements are destroyed.
        */
        virtual ~CFavouritesItemImplList();

    public:     // Streaming

        /**
        * Externalize into a stream.
        * @param aStream The stream to externalize to.
        */
        void ExternalizeL( RWriteStream& aStream ) const;

        /**
        * Internalize from a stream. Existing data is kept, new ones appended.
        * @param aStream The stream to externalize from.
        */
        void InternalizeL( RReadStream& aStream );

    };

#endif

// End of file
