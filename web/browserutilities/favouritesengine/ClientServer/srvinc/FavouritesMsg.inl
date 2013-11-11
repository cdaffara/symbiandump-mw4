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
*      Inline method definitions of class TFavouritesMsg.   
*      
*
*/


#ifndef FAVOURITES_MSG_INL
#define FAVOURITES_MSG_INL

// CONSTANTS

// Note: a handle / message is always positive. This way, an integer value can
// be return both a handle or an error: if it's positive, it's a handle. If it
// is negative, it is an error.

LOCAL_D const TInt KFuncBits = 8;       ///< Function bits.
LOCAL_D const TInt KIndexBits = 8;      ///< Index bits.
LOCAL_D const TInt KChkNumBits = 15;    ///< Check number bits.

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// TFavouritesHandle::TFavouritesHandle()
// ---------------------------------------------------------
//
TFavouritesHandle::TFavouritesHandle( TInt aIndex, TInt aChkNum ) :
    iData(((aChkNum << KIndexBits) | aIndex) << KFuncBits)
    {
    __ASSERT_DEBUG( aIndex < (1 << KIndexBits), \
                FavouritesPanic( EFavouritesInternal ) );
    __ASSERT_DEBUG( aChkNum < (1 << KChkNumBits), \
        FavouritesPanic( EFavouritesInternal ) );
    }

// ---------------------------------------------------------
// TFavouritesHandle::TFavouritesHandle()
// ---------------------------------------------------------
//
TFavouritesHandle::TFavouritesHandle( TInt aHandle ) :
    iData(aHandle)
    {
    __ASSERT_DEBUG( (aHandle & ((1 << KFuncBits) - 1)) == 0, \
        FavouritesPanic( EFavouritesInternal ) );
    }

// ---------------------------------------------------------
// TFavouritesHandle::operator TInt()
// ---------------------------------------------------------
//
TFavouritesHandle::operator TInt() const
    {
    return iData & (((1 << (KChkNumBits + KIndexBits)) - 1) << KFuncBits );
    }

// ---------------------------------------------------------
// TFavouritesHandle::Index()
// ---------------------------------------------------------
//
TInt TFavouritesHandle::Index() const
    {
    return (iData >> KFuncBits) & ((1 << KIndexBits) - 1);
    }

// ---------------------------------------------------------
// TFavouritesHandle::ChkNum()
// ---------------------------------------------------------
//
TInt TFavouritesHandle::ChkNum() const
    {
    return ((iData >> (KIndexBits + KFuncBits))) & ((1 << KChkNumBits) - 1);
    }

// ---------------------------------------------------------
// TFavouritesHandle::IsNull()
// ---------------------------------------------------------
//
TBool TFavouritesHandle::IsNull() const
    {
    return !((iData >> KFuncBits) & ((1 << (KChkNumBits + KIndexBits)) - 1 ));
    }

// ---------------------------------------------------------
// TFavouritesMsg::TFavouritesMsg()
// ---------------------------------------------------------
//
TFavouritesMsg::TFavouritesMsg( TInt aHandle, TInt aFunction )
: TFavouritesHandle( aHandle )
    {
    __ASSERT_DEBUG( aFunction < EFavengMaxFunction, \
        FavouritesPanic( EFavouritesInternal ) );
    __ASSERT_DEBUG( aFunction < (1 << KFuncBits), \
        FavouritesPanic( EFavouritesInternal ) );
    iData |= aFunction;
    }

// ---------------------------------------------------------
// TFavouritesMsg::TFavouritesMsg()
// ---------------------------------------------------------
//
TFavouritesMsg::TFavouritesMsg( TInt aMsg ): TFavouritesHandle( 0 )
    {
    __ASSERT_DEBUG( (aMsg & ((1 << KFuncBits) - 1)) < EFavengMaxFunction, \
        FavouritesPanic( EFavouritesInternal ) );
    iData = aMsg;
    }

// ---------------------------------------------------------
// TFavouritesMsg::operator TInt()
// ---------------------------------------------------------
//
TFavouritesMsg::operator TInt() const
    {
    return iData;
    }

// ---------------------------------------------------------
// TFavouritesMsg::Function()
// ---------------------------------------------------------
//
TFavouritesFunction TFavouritesMsg::Function() const
    {
    return STATIC_CAST \
        ( TFavouritesFunction, (iData & ((1 << KFuncBits) - 1 )) );
    }

#endif

// End of File
