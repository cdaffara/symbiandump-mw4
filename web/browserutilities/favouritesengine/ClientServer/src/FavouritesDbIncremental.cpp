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
*      Implementation of class CFavouritesDbIncremental
*      
*
*/


// INCLUDE FILES

#include "FavouritesDbIncremental.h"
#include "FavouritesDb.h"
#include "FavouritesMsg.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// RFavouritesDbIncremental::Recover
// ---------------------------------------------------------
//
EXPORT_C TInt RFavouritesDbIncremental::Recover
( RFavouritesDb& aDb, TInt& aStep )
    {
    TPckg<TInt> step( aStep );
    return Open( aDb, EFavengIncrementalRecover, TIpcArgs( &step ) );
    }

// ---------------------------------------------------------
// RFavouritesDbIncremental::Compact
// ---------------------------------------------------------
//
EXPORT_C TInt RFavouritesDbIncremental::Compact
( RFavouritesDb& aDb, TInt& aStep )
    {
    TPckg<TInt> step( aStep );
    return Open( aDb, EFavengIncrementalCompact, TIpcArgs( &step ) );
    }

// ---------------------------------------------------------
// RFavouritesDbIncremental::Next
// ---------------------------------------------------------
//
EXPORT_C TInt RFavouritesDbIncremental::Next( TInt& aStep )
    {
    TInt ret = SendReceive( EFavengIncrementalNext, TIpcArgs( aStep ) );
    if ( ret >= 0 )
        {
        aStep = ret;
        ret = KErrNone;
        }
    return ret;
    }

//  End of File  
