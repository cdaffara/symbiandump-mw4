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
*      Implementation of class CFavouritesSrvDbIncremental
*      
*
*/


// INCLUDE FILES

#include "FavouritesSrvDbIncremental.h"
#include "FavouritesSrvDb.h"
#include "FavouritesPanic.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CFavouritesSrvDbIncremental::RecoverL
// ---------------------------------------------------------
//
void CFavouritesSrvDbIncremental::RecoverL
( CFavouritesSrvDb& aDb, TInt& aStep )
    {
    __ASSERT_DEBUG( !iStep, FavouritesPanic( EFavouritesAlreadyOpen ) );
    User::LeaveIfError( iIncremental.Recover( aDb.Database(), iStep ) );
    aStep = iStep;
    }

// ---------------------------------------------------------
// CFavouritesSrvDbIncremental::CompactL
// ---------------------------------------------------------
//
void CFavouritesSrvDbIncremental::CompactL
( CFavouritesSrvDb& aDb, TInt& aStep )
    {
    __ASSERT_DEBUG( !iStep, FavouritesPanic( EFavouritesAlreadyOpen ) );
    User::LeaveIfError( iIncremental.Compact( aDb.Database(), iStep ) );
    aStep = iStep;
    }

// ---------------------------------------------------------
// CFavouritesSrvDbIncremental::NextL
// ---------------------------------------------------------
//
void CFavouritesSrvDbIncremental::NextL( TInt& aStep )
    {
    __ASSERT_DEBUG( iStep == aStep && aStep > 0, \
        FavouritesPanic( EFavouritesInternal ) );
    User::LeaveIfError( iIncremental.Next( iStep ) );
    aStep = iStep;
    }

//  End of File  
