/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*      Implementation of class TFavouritesWapAp
*      
*
*/


// INCLUDE FILES

#include "FavouritesWapAp.h"
#include "FavouritesPanic.h"

// CONSTANTS

/**
* This value is set to iApId, when the AP is "Default" or "Null"; not really
* necessary, but clearer.
*/
LOCAL_D const TUint32 KMeaninglessNeverAccessed = 0xFFFFFFFF;

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// TFavouritesWapAp::TFavouritesWapAp
// ---------------------------------------------------------
//
EXPORT_C TFavouritesWapAp::TFavouritesWapAp() :
    iApId( KMeaninglessNeverAccessed ),
    iValueKind( EDefault )
    {
    }

// ---------------------------------------------------------
// TFavouritesWapAp::operator=
// ---------------------------------------------------------
//
EXPORT_C TFavouritesWapAp&
TFavouritesWapAp::operator= ( const TFavouritesWapAp& aAp )
    {
    if ( &aAp != this )
        {
        iApId = aAp.iApId;
        iValueKind = aAp.iValueKind;
        }
    return *this;
    }

// ---------------------------------------------------------
// TFavouritesWapAp::operator=
// ---------------------------------------------------------
//
EXPORT_C TFavouritesWapAp& TFavouritesWapAp::operator= ( TUint32 aApId )
    {
    iApId = aApId;
    iValueKind = ENormal;
    return *this;
    }

// ---------------------------------------------------------
// TFavouritesWapAp::SetNull
// ---------------------------------------------------------
//
EXPORT_C void TFavouritesWapAp::SetNull()
    {
    iApId = KMeaninglessNeverAccessed;  // Not really necessary.
    iValueKind = ENull;
    }

// ---------------------------------------------------------
// TFavouritesWapAp::SetDefault
// ---------------------------------------------------------
//
EXPORT_C void TFavouritesWapAp::SetDefault()
    {
    iApId = KMeaninglessNeverAccessed;  // Not really necessary.
    iValueKind = EDefault;
    }

// ---------------------------------------------------------
// TFavouritesWapAp::SetApId
// ---------------------------------------------------------
//
EXPORT_C void TFavouritesWapAp::SetApId( TUint32 aApId )
    {
    iApId = aApId;
    iValueKind = ENormal;
    }

// ---------------------------------------------------------
// TFavouritesWapAp::IsNull
// ---------------------------------------------------------
//
EXPORT_C TBool TFavouritesWapAp::IsNull() const
    {
    return iValueKind == ENull;
    }

// ---------------------------------------------------------
// TFavouritesWapAp::IsDefault
// ---------------------------------------------------------
//
EXPORT_C TBool TFavouritesWapAp::IsDefault() const
    {
    return iValueKind == EDefault;
    }

// ---------------------------------------------------------
// TFavouritesWapAp::ApId
// ---------------------------------------------------------
//
EXPORT_C TUint32 TFavouritesWapAp::ApId() const
    {
    __ASSERT_DEBUG( iValueKind == ENormal, \
        FavouritesPanic ( EFavouritesNoWapApValue ) );
    return iApId;
    }

// ---------------------------------------------------------
// TFavouritesWapAp::ExternalizeL
// ---------------------------------------------------------
//
void TFavouritesWapAp::ExternalizeL( RWriteStream& aStream ) const
    {
    aStream.WriteInt32L( iApId );
    aStream.WriteInt32L( STATIC_CAST( TInt, iValueKind ) );
    }

// ---------------------------------------------------------
// TFavouritesWapAp::InternalizeL
// ---------------------------------------------------------
//
void TFavouritesWapAp::InternalizeL( RReadStream& aStream )
    {
    iApId = aStream.ReadInt32L();
    iValueKind = STATIC_CAST( TValueKind, aStream.ReadInt32L() );
    }

//  End of File  
