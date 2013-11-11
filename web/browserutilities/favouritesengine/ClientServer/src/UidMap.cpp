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
*      Implementation of CUidMap.
*      
*
*/


// INCLUDE FILES

#include <s32strm.h>
#include "UidMap.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CUidMap::ExternalizeL
// ---------------------------------------------------------
//
void CUidMap::ExternalizeL( RWriteStream& aStream ) const
    {
    aStream.WriteInt32L( Count() );
    for ( TInt i = 0; i < Count(); i++ )
        {
        aStream.WriteInt32L( At( i ).iUid1 );
        aStream.WriteInt32L( At( i ).iUid2 );
        }
    }

// ---------------------------------------------------------
// CUidMap::InternalizeL
// ---------------------------------------------------------
//
void CUidMap::InternalizeL( RReadStream& aStream )
    {
    Reset();
    TInt count = aStream.ReadInt32L();
    ResizeL( count );
    for ( TInt i = 0; i < count; i++ )
        {
        At( i ).iUid1  = aStream.ReadInt32L();
        At( i ).iUid2  = aStream.ReadInt32L();
        }
    }

//  End of File  
