/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class which allocates random UIDs from a given range
*
*/

#include <e32std.h>
#include "UidAllocator.h"

// MEMBER FUNCTION DEFINITIONS

// ============================================================================
// TUidAllocator::AllocateL()
// Allocate a UID or leave with KErrNotFound
//
// @since 3.1
// ============================================================================
//
TInt TUidAllocator::AllocateL( const RUidArray& aUsedUids, TInt aDriveLetter )
    {
    if ( 'C' == aDriveLetter )
        {
        aDriveLetter = 'c';
        }

    TInt uidStart = ( ('c' == aDriveLetter) ?
                      KWidgetUidInternalMemoryStart :
                      KWidgetUidExternalMemoryStart );
    TInt uidStop = ( ('c' == aDriveLetter) ?
                     KWidgetUidExternalMemoryStart :
                     KWidgetUidExternalMemoryStop + 1);

    TInt uid = uidStart;
    for ( ; uid < uidStop; ++uid )
        {
        if ( KErrNotFound == aUsedUids.Find( TUid::Uid( uid ) ) )
            {
            break;
            }
        }
    if ( uid == uidStop )
        {
        User::Leave( KErrNotFound );
        }
    return uid;
    }

TBool TUidAllocator::IsWidget( TUid aUid )
    {
    return ( ( aUid.iUid >= KWidgetUidLowerBound
               && aUid.iUid <= KWidgetUidUpperBound )?
             ETrue : EFalse );
    }
