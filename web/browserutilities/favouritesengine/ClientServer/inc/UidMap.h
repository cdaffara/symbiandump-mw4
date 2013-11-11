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
*      Declaration of class SUidPair and CUidMap.
*      
*
*/


#ifndef UID_MAP_H
#define UID_MAP_H

// INCLUDE FILES

#include <e32base.h>

// CONSTANTS

// FORWARD DECLARATION

class RWriteStream;
class RReadStream;

// CLASS DECLARATION

/**
* Uid mapping.
*/
NONSHARABLE_STRUCT(SUidPair)
    {
    public:     // data
        TInt iUid1; ///< Uid 1.
        TInt iUid2; ///< Uid 2.
    };

/**
* Array of SUidPair-s.
*/
NONSHARABLE_CLASS(CUidMap): public CArrayFixFlat<SUidPair>
    {

    public:     // Constructors and destructor

        /**
        * Constructor.
        * @param aGranularity Granularity.
        */
        inline CUidMap( TInt aGranularity );

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

#include "UidMap.inl"

// End of file
