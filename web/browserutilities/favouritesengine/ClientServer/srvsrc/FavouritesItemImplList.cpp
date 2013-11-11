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
*      Implementation of CFavouritesItemImplList.
*      
*
*/


// INCLUDE FILES

#include <s32strm.h>
#include "FavouritesItemImplList.h"
#include "FavouritesItemImpl.h"

// CONSTANTS

/// Granularity of the item list.
LOCAL_D const TInt KGranularity = 8;

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CFavouritesItemImplList::CFavouritesItemImplList
// ---------------------------------------------------------
//
CFavouritesItemImplList::CFavouritesItemImplList()
: CArrayPtrFlat<CFavouritesItemImpl>( KGranularity )
    {
    }

// ---------------------------------------------------------
// CFavouritesItemImplList::~CFavouritesItemImplList
// ---------------------------------------------------------
//
CFavouritesItemImplList::~CFavouritesItemImplList()
    {
    ResetAndDestroy();
    }

// ---------------------------------------------------------
// CFavouritesItemImplList::ExternalizeL
// ---------------------------------------------------------
//
void CFavouritesItemImplList::ExternalizeL( RWriteStream& aStream ) const
    {
    aStream.WriteInt32L( Count() );
    for ( TInt i = 0; i < Count(); i++ )
        {
        At( i )->ExternalizeL( aStream );
        }
    }

// ---------------------------------------------------------
// CFavouritesItemImplList::InternalizeL
// ---------------------------------------------------------
//
void CFavouritesItemImplList::InternalizeL( RReadStream& aStream )
    {
    TInt count = aStream.ReadInt32L();
    for ( TInt i = 0; i < count; i++ )
        {
        CFavouritesItemImpl* item = CFavouritesItemImpl::NewLC();
        item->InternalizeL( aStream );
        AppendL( item );
        CleanupStack::Pop( item );
        }
    }

//  End of File  
