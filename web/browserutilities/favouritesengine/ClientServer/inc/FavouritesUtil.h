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
*      Declaration of class FavouritesUtil.
*      
*
*/


#ifndef FAVOURITES_UTIL_H
#define FAVOURITES_UTIL_H

//  INCLUDES

#include <e32base.h>

class RWriteStream;
class RReadStream;

// CLASS DECLARATION

/**
* Static utility functions.
*/
NONSHARABLE_CLASS(FavouritesUtil)
    {

    public:     // Streaming

        /**
        * Externalize integer array into a stream.
        * @param aArray Array to externalize.
        * @param aStream The stream to externalize to.
        */
        static void ExternalizeL
            ( const CArrayFix<TInt>& aArray, RWriteStream& aStream );

        /**
        * Internalize integer array from a stream. Existing data is kept,
        * new ones appended.
        * @param aArray Array to internalize.
        * @param aStream The stream to externalize from.
        */
        static void InternalizeL
            ( CArrayFix<TInt>& aArray, RReadStream& aStream );

        /**
        * Checks weather a backup operation is in progress or not.
        * @return ETrue if backup is in progress.
        */
        static TBool IsBackupInProgressL();

    };

#endif

// End of File
