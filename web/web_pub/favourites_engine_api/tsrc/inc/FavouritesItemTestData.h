/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declaration of class TFavouritesItemTestData.
*
*/


#ifndef FAVOURITES_ITEM_TEST_DATA_H
#define FAVOURITES_ITEM_TEST_DATA_H

//  INCLUDES

#include <e32base.h>
#include <favouritesitemdata.h>

// CLASS DECLARATION

/**
* Mixin class for representing extra data for a favourites item.
*/
class TFavouritesItemTestData: public MFavouritesItemData
    {

    public:     // from MFavouritesItemData

        /**
        * Externalize into a stream.
        * @param aStream The stream to externalize to.
        */
        virtual void ExternalizeL( RWriteStream& aStream ) const;

        /**
        * Internalize from a stream.
        * @param aStream The stream to externalize from.
        */
        virtual void InternalizeL( RReadStream& aStream );

    public:     // data

        TInt iTestData;     ///< Some test data to write / read.

    };

#endif
            
// End of File
