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
* Description:  Implementation of class TFavouritesItemTestData
*
*/


// INCLUDE FILES

#include <s32strm.h>
#include "favouritesitemtestdata.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// TFavouritesItemTestData::ExternalizeL
// ---------------------------------------------------------
//
void TFavouritesItemTestData::ExternalizeL( RWriteStream& aStream ) const
    {
    aStream.WriteInt32L( iTestData );
    }

// ---------------------------------------------------------
// TFavouritesItemTestData::InternalizeL
// ---------------------------------------------------------
//
void TFavouritesItemTestData::InternalizeL( RReadStream& aStream )
    {
    iTestData = aStream.ReadInt32L();
    }
