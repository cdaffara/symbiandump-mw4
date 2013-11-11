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
*      Declaration of class TFavouritesFilter.
*      
*
*/


#ifndef FAVOURITES_FILTER_H
#define FAVOURITES_FILTER_H

//  INCLUDES

#include <e32base.h>
#include <FavouritesItem.h>

// CLASS DECLARATION

/**
* Filtering constraints for data retrieval.
*/
NONSHARABLE_CLASS(TFavouritesFilter)
    {

    public:     // Constructors

        /**
        * Default constructor.
        */
        inline TFavouritesFilter();

        /**
        * Constructor.
        * @param aParentFolder Uid value to filter. KFavouritesNullUid
        * clears (all accepted).
        * @param aType EItem or EFolder to use filter; ENone to clear
        * filter (all accepted).
        * @param aName wildcard pattern to be used for name matching.
        * NULL clears (all accepted).
        * @param aContextId ContextId value to filter.
        * KFavouritesNullContextId clears (all accepted).
        */
        inline TFavouritesFilter
            (
            TInt aParentFolder,
            CFavouritesItem::TType aType,
            const TDesC* aName,
            TInt32 aContextId
            );

    public:     // Data

        TInt iParentFolder;             ///< Parent folder filter.
        CFavouritesItem::TType iType;   ///< Type filter.
        const TDesC* iName;             ///< Name filter. Not owned.
        TInt32 iContextId;              ///< Context id filter.
    };

#include "FavouritesFilter.inl"

#endif

// End of File
