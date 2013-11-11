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
*      Panic function and codes for the Favourites Engine.
*      
*
*/


#ifndef FAVOURITES_PANIC_H
#define FAVOURITES_PANIC_H

// TYPES

/**
* Panic reasons for Favourites Engine.
*/
enum TFavouritesPanicReason
    {
    /// Attempt to begin operation on closed db.
    EFavouritesNotReady = -300,
    /// Nested Begin() calls.
    EFavouritesNestedTransaction,
    /// KFavouritesNullUid found in database.
    EFavouritesNullUidInDatabase,
    /// Item is neither EItem nor EFolder.
    EFavouritesBadType,
    /// No numeric AP value for "Default" or "Null".
    EFavouritesNoWapApValue,
    /// Buffer too small (unique name generation).
    EFavouritesBufferTooSmall,
    /// Unsupported operation.
    EFavouritesUnsupported,
    /// Internal error.
    EFavouritesInternal,
    /// Object already open.
    EFavouritesAlreadyOpen,
    /// Attempt to Commit when there is no transaction.
    EFavouritesNoTransaction,
    /// Bad step value passed to RFavouritesDbIncremental::Next().
    EFavouritesBadStep,
    /// Unexpected file found.
    EFavouritesBadFile
    };

// FUNCTION DECLARATIONS

/**
* Panic the Favourites Engine.
* @param aReason Reason for the panic.
*/
void FavouritesPanic( TFavouritesPanicReason aReason );

#endif
            
// End of File
