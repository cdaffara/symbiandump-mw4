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
*      Implementation of CFavouritesItemList.
*      
*
*/


// INCLUDE FILES

#include <s32strm.h>
#include "FavouritesItemList.h"
#include "FavouritesItem.h"

// CONSTANTS

/// Granularity of the item list.
LOCAL_D const TInt KGranularity = 8;

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CFavouritesItemList::CFavouritesItemList
// ---------------------------------------------------------
//
EXPORT_C CFavouritesItemList::CFavouritesItemList()
: CArrayPtrFlat<CFavouritesItem>( KGranularity )
    {
    }

// ---------------------------------------------------------
// CFavouritesItemList::~CFavouritesItemList
// ---------------------------------------------------------
//
EXPORT_C CFavouritesItemList::~CFavouritesItemList()
    {
    ResetAndDestroy();
    }

// ---------------------------------------------------------
// CFavouritesItemList::Delete
// ---------------------------------------------------------
//
EXPORT_C void CFavouritesItemList::Delete( TInt aIndex )
    {
    delete At( aIndex );    // At will panic as usual, if out of bounds.
    CArrayPtrFlat<CFavouritesItem>::Delete( aIndex );
    }

// ---------------------------------------------------------
// CFavouritesItemList::Delete
// ---------------------------------------------------------
//
EXPORT_C void CFavouritesItemList::Delete( TInt aIndex, TInt aCount )
    {
    for ( ; aCount > 0; aCount-- )
        {
        // After deleting an item, the remaining ones "fall down".
        Delete( aIndex );
        }
    }

// ---------------------------------------------------------
// CFavouritesItemList::SortL
// ---------------------------------------------------------
//
EXPORT_C void CFavouritesItemList::SortL( ComparisonFuncL aCompareItemsL )
    {
    CFavouritesItem* left;
    CFavouritesItem* right;
    CFavouritesItem* tmp;
    TInt i;
    TInt j;
    TInt count = Count();
    if ( count )
        {
        for ( i = count - 1; i >= 0; i-- )
            {
            for ( j = 0; j < i; j++ )
                {
                left = At( j );
                right = At( j + 1 );
                if ( (*aCompareItemsL)( *left, *right ) > 0 )
                    {
                    tmp = left;
                    At( j ) = right;
                    At( j + 1 ) = tmp;
                    }
                }
            }
        }
    }

// ---------------------------------------------------------
// CFavouritesItemList::UidToIndex
// ---------------------------------------------------------
//
EXPORT_C TInt CFavouritesItemList::UidToIndex( TInt aUid ) const
    {
    TInt i;
    TInt count = Count();
    for ( i = 0; i < count; i++ )
        {
        if ( At( i )->Uid() == aUid )
            {
            return i;
            }
        }
    return KErrNotFound;
    }

// ---------------------------------------------------------
// CFavouritesItemList::IndexToUid
// ---------------------------------------------------------
//
EXPORT_C TInt CFavouritesItemList::IndexToUid( TInt aIndex ) const
    {
    if ( aIndex >= 0 && aIndex < Count() )
        {
        return At( aIndex )->Uid();
        }
    return KFavouritesNullUid;
    }

// ---------------------------------------------------------
// CFavouritesItemList::ItemByUid
// ---------------------------------------------------------
//
EXPORT_C const CFavouritesItem* CFavouritesItemList::ItemByUid
( TInt aUid ) const
    {
    TInt i;
    TInt count = Count();
    for ( i = 0; i < count; i++ )
        {
        if ( At( i )->Uid() == aUid )
            {
            return At( i );
            }
        }
    return NULL;
    }

// ---------------------------------------------------------
// CFavouritesItemList::ExternalizeL
// ---------------------------------------------------------
//
void CFavouritesItemList::ExternalizeL( RWriteStream& aStream ) const
    {
    aStream.WriteInt32L( Count() );
    for ( TInt i = 0; i < Count(); i++ )
        {
        At( i )->ExternalizeL( aStream );
        }
    }

// ---------------------------------------------------------
// CFavouritesItemList::InternalizeL
// ---------------------------------------------------------
//
void CFavouritesItemList::InternalizeL( RReadStream& aStream )
    {
    TInt count = aStream.ReadInt32L();
    for ( TInt i = 0; i < count; i++ )
        {
        CFavouritesItem* item = CFavouritesItem::NewLC();
        item->InternalizeL( aStream );
        AppendL( item );
        CleanupStack::Pop( item );
        }
    }

//  End of File  
