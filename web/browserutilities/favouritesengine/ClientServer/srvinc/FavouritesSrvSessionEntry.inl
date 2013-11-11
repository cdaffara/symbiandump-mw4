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
*      Inline method definitions of class TFavouritesSessionEntry and
*                                         CFavouritesSessionEntryList.   
*      
*
*/


#ifndef FAVOURITES_SRV_SESSION_ENTRY_INL
#define FAVOURITES_SRV_SESSION_ENTRY_INL

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// TFavouritesSessionEntry::SetNull()
// ---------------------------------------------------------
//
void TFavouritesSessionEntry::SetNull()
    {
    iType = ENone;
    iObject = NULL; // Clarity code.
    iChkNum = 0;    // Clarity code.
    }

// ---------------------------------------------------------
// TFavouritesSessionEntry::IsNull()
// ---------------------------------------------------------
//
TBool TFavouritesSessionEntry::IsNull()
    {
    return iType == ENone;
    }

// ---------------------------------------------------------
// TFavouritesSessionEntry::operator=()
// ---------------------------------------------------------
//
void TFavouritesSessionEntry::operator=( CFavouritesSrvDb& aDb )
    {
    __ASSERT_DEBUG( IsNull(), FavouritesPanic( EFavouritesInternal ) );
    iType = EDatabase;
    iObject = STATIC_CAST( TAny*, &aDb );
    }

// ---------------------------------------------------------
// TFavouritesSessionEntry::operator=()
// ---------------------------------------------------------
//
void TFavouritesSessionEntry::operator=( CFavouritesSrvDbNotifier& aNotifier )
    {
    __ASSERT_DEBUG( IsNull(), FavouritesPanic( EFavouritesInternal ) );
    iType = ENotifier;
    iObject = STATIC_CAST( TAny*, &aNotifier );
    }

// ---------------------------------------------------------
// TFavouritesSessionEntry::operator=()
// ---------------------------------------------------------
//
void TFavouritesSessionEntry::operator=
( CFavouritesSrvDbIncremental& aIncremental )
    {
    __ASSERT_DEBUG( IsNull(), FavouritesPanic( EFavouritesInternal ) );
    iType = EIncremental;
    iObject = STATIC_CAST( TAny*, &aIncremental );
    }

// ---------------------------------------------------------
// TFavouritesSessionEntry::operator=()
// ---------------------------------------------------------
//
void TFavouritesSessionEntry::operator=( HFavouritesSrvStream& aStream )
    {
    __ASSERT_DEBUG( IsNull(), FavouritesPanic( EFavouritesInternal ) );
    iType = EStream;
    iObject = STATIC_CAST( TAny*, &aStream );
    }

// ---------------------------------------------------------
// TFavouritesSessionEntry::operator=()
// ---------------------------------------------------------
//
void TFavouritesSessionEntry::operator=
( CFavouritesSrvFile& aFile )
    {
    __ASSERT_DEBUG( IsNull(), FavouritesPanic( EFavouritesInternal ) );
    iType = EFile;
    iObject = STATIC_CAST( TAny*, &aFile );
    }

// ---------------------------------------------------------
// TFavouritesSessionEntry::ChkNum()
// ---------------------------------------------------------
//
TInt& TFavouritesSessionEntry::ChkNum()
    {
    return iChkNum;
    }

// ---------------------------------------------------------
// TFavouritesSessionEntry::Type()
// ---------------------------------------------------------
//
TFavouritesSessionEntry::TType TFavouritesSessionEntry::Type() const
    {
    return iType;
    }

// ---------------------------------------------------------
// TFavouritesSessionEntry::Database()
// ---------------------------------------------------------
//
CFavouritesSrvDb& TFavouritesSessionEntry::Database()
    {
    __ASSERT_DEBUG( !IsNull(), FavouritesPanic( EFavouritesInternal ) );
    __ASSERT_DEBUG( iType == EDatabase, \
                                FavouritesPanic( EFavouritesInternal ) );
    return *(STATIC_CAST( CFavouritesSrvDb*, iObject ));
    }

// ---------------------------------------------------------
// TFavouritesSessionEntry::Notifier()
// ---------------------------------------------------------
//
CFavouritesSrvDbNotifier& TFavouritesSessionEntry::Notifier()
    {
    __ASSERT_DEBUG( !IsNull(), FavouritesPanic( EFavouritesInternal ) );
    __ASSERT_DEBUG( iType == ENotifier, \
                                FavouritesPanic( EFavouritesInternal ) );
    return *(STATIC_CAST( CFavouritesSrvDbNotifier*, iObject ));
    }

// ---------------------------------------------------------
// TFavouritesSessionEntry::Incremental()
// ---------------------------------------------------------
//
CFavouritesSrvDbIncremental& TFavouritesSessionEntry::Incremental()
    {
    __ASSERT_DEBUG( !IsNull(), FavouritesPanic( EFavouritesInternal ) );
    __ASSERT_DEBUG( iType == EIncremental, \
                                FavouritesPanic( EFavouritesInternal ) );
    return *(STATIC_CAST( CFavouritesSrvDbIncremental*, iObject ));
    }

// ---------------------------------------------------------
// TFavouritesSessionEntry::Stream()
// ---------------------------------------------------------
//
HFavouritesSrvStream& TFavouritesSessionEntry::Stream()
    {
    __ASSERT_DEBUG( !IsNull(), FavouritesPanic( EFavouritesInternal ) );
    __ASSERT_DEBUG( iType == EStream, \
                                FavouritesPanic( EFavouritesInternal ) );
    return *(STATIC_CAST( HFavouritesSrvStream*, iObject ));
    }

// ---------------------------------------------------------
// TFavouritesSessionEntry::File()
// ---------------------------------------------------------
//
CFavouritesSrvFile& TFavouritesSessionEntry::File()
    {
    __ASSERT_DEBUG( !IsNull(), FavouritesPanic( EFavouritesInternal ) );
    __ASSERT_DEBUG( iType == EFile, \
                                FavouritesPanic( EFavouritesInternal ) );
    return *(STATIC_CAST( CFavouritesSrvFile*, iObject ));
    }

// ---------------------------------------------------------
// TFavouritesSessionEntry::ChkNum()
// ---------------------------------------------------------
//
TInt TFavouritesSessionEntry::ChkNum() const
    {
    return iChkNum;
    }

#endif

// End of File
