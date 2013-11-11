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
* Description:  Implementation of class RFavouritesHandle
*
*/


// INCLUDE FILES

#include "FavouritesHandle.h"
#include "FavouritesMsg.h"
#include "FavouritesPanic.h"
#include "FavouritesLogger.h" 

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// RFavouritesHandle::Close
// ---------------------------------------------------------
//
EXPORT_C void RFavouritesHandle::Close()
    {
    if ( iHandle )
        {
        (void) SendReceive( EFavengClose );
        }
    iSess.SetHandle( 0 );
    iHandle = 0;
    }

// ---------------------------------------------------------
// RFavouritesHandle::Open
// ---------------------------------------------------------
//
TInt RFavouritesHandle::Open
( RFavouritesSession& aSess, TInt aFunction, const TIpcArgs& aArgs )
    {
    __ASSERT_DEBUG( !iHandle, FavouritesPanic( EFavouritesAlreadyOpen ) );
    TInt ret = aSess.SendReceive( aFunction, aArgs );
    __ASSERT_DEBUG( ret, FavouritesPanic( EFavouritesInternal ) );
    // Positive value -> handle returned.
    // Negative value -> error returned.
    if ( ret > 0 )
        {
        iSess = aSess;
        iHandle = ret;
        ret = KErrNone;
        }
    return ret;
    }

// ---------------------------------------------------------
// RFavouritesHandle::Open
// ---------------------------------------------------------
//
TInt RFavouritesHandle::Open
( RFavouritesHandle& aHandle, TInt aFunction, const TIpcArgs& aArgs )
    {
    __ASSERT_DEBUG( !iHandle, FavouritesPanic( EFavouritesAlreadyOpen ) );
//    FLOG(( _L("RFavouritesHandle::Open before SendReceive") ));
    TInt ret = aHandle.SendReceive( aFunction, aArgs );
//    FLOG(( _L("RFavouritesHandle::Open after SendReceive") ));
    __ASSERT_DEBUG( ret, FavouritesPanic( EFavouritesInternal ) );
    // Positive value -> handle returned.
    // Negative value -> error returned.
    if ( ret > 0 )
        {
        FLOG(( _L("RFavouritesHandle::Open() ret>0") ));
        iSess = aHandle.iSess;
        iHandle = ret;
        ret = KErrNone;
        }
    return ret;
    }

// ---------------------------------------------------------
// RFavouritesHandle::SendReceive
// ---------------------------------------------------------
//
void RFavouritesHandle::SendReceive
( TInt aFunction, const TIpcArgs& aArgs, TRequestStatus& aStatus ) const
    {
    __ASSERT_DEBUG( iHandle, FavouritesPanic( EFavouritesNotReady ) );
    iSess.SendReceive( TFavouritesMsg( iHandle, aFunction ), aArgs, aStatus );
    }

// ---------------------------------------------------------
// RFavouritesHandle::SendReceive
// ---------------------------------------------------------
//
TInt RFavouritesHandle::SendReceive
( TInt aFunction, const TIpcArgs& aArgs ) const
    {
    __ASSERT_DEBUG( iHandle, FavouritesPanic( EFavouritesNotReady ) );
    return iSess.SendReceive( TFavouritesMsg( iHandle, aFunction ), aArgs );
    }

// ---------------------------------------------------------
// RFavouritesHandle::SendReceive
// ---------------------------------------------------------
//
TInt RFavouritesHandle::SendReceive( TInt aFunction ) const
    {
    __ASSERT_DEBUG( iHandle, FavouritesPanic( EFavouritesNotReady ) );
    return iSess.SendReceive( TFavouritesMsg( iHandle, aFunction ) );
    }

// End of file
