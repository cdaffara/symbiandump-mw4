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
*      Declaration of class TFavouritesHandle and TFavouritesMsg.
*      
*
*/


#ifndef FAVOURITES_MSG_H
#define FAVOURITES_MSG_H

//  INCLUDES

#include <e32std.h>
#include "FavouritesPanic.h"

/// Favourites Engine functions.
enum TFavouritesFunction
    {
    EFavengNullFunction,
    
    // Functions with test capability
    EFavengTestCapabilityStart,
    
    EFavengResourceMarkStart,
    EFavengResourceMarkEnd,
    EFavengResourceCount,
    EFavengSetHeapFailure,
    
    EFavengTestCapabilityEnd,
    
    // Functions with read user data capability
    EFavengReadCapabilityStart,
    
    EFavengOpenDatabase,
    EFavengOpenNotifier,
    EFavengOpenStream,
    EFavengIsDamaged,
    EFavengSize,
    EFavengClose,
    EFavengGet,
    EFavengGetAll,
    EFavengGetUids,
    EFavengPreferredUid,
    EFavengItemExists,
    EFavengFolderExists,
    EFavengCount,
    EFavengGetData,
    EFavengGetBrowserData,
    EFavengFileOpen,
    EFavengFileOpenRead,
    EFavengNotifyAllEvents,
    EFavengNotifyChange,
    EFavengNotifyCancel,
    EFavengStreamRead,
    
    EFavengReadCapabilityEnd,
    
    // Functions with write user data capability
    EFavengWriteCapabilityStart,
    
    EFavengDeleteDatabase,
    EFavengRecover,
    EFavengCompact,
    EFavengUpdateStats,
    EFavengBegin,
    EFavengCommit,
    EFavengRollback,
    EFavengDelete,
    EFavengUpdate,
    EFavengAdd,
    EFavengSetSpecialItem,
    EFavengSetFactoryItem,
    EFavengSetReadOnly,
    EFavengSetModified,
    EFavengSetPreferredUid,
    EFavengSetData,
    EFavengSetBrowserData,
    EFavengMakeUniqueName,
    EFavengRestoreFactorySettings,
    EFavengSetAccessPoints,
    EFavengFileOpenWrite,
    EFavengFileDelete,
    EFavengIncrementalRecover,
    EFavengIncrementalCompact,
    EFavengIncrementalNext,
    EFavengStreamWrite,
    
    EFavengWriteCapabilityEnd,
    
    EFavengMaxFunction                  // End marker for sanity check.
    };

// CLASS DECLARATION

/**
* Favourites Engine handle.
*/
NONSHARABLE_CLASS(TFavouritesHandle)
    {
    public:     // Constructor

        /**
        * Construct from index and check number.
        * @param aIndex Index.
        * @param aChkNum Check number.
        */
        inline TFavouritesHandle( TInt aIndex, TInt aChkNum );

        /**
        * Construct from handle number.
        * @param aIndex Index.
        * @param aChkNum Check number.
        */
        inline TFavouritesHandle( TInt aHandle );

    public:     // Operators

        /**
        * Convert to integer.
        */
        inline operator TInt() const;

    public:     // Getters

        /**
        * Get index.
        * @return Index.
        */
        inline TInt Index() const;

        /**
        * Get check number.
        * @return Check number.
        */
        inline TInt ChkNum() const;

        /**
        * Check if null handle.
        * @return Check number.
        */
        inline TBool IsNull() const;

    protected:  // Data

        TInt iData; ///< Data.

    };

/**
* Favourites Engine Message (a handle and a function packaged together).
* For object-less functions, set the handle to 0.
*/
NONSHARABLE_CLASS(TFavouritesMsg): public TFavouritesHandle
    {
    public:     // Constructors

        /**
        * Create from handle number and function.
        * @param aHandle Handle.
        * @param aFunction Function.
        */
        inline TFavouritesMsg( TInt aHandle, TInt aFunction );

        /**
        * Create from message.
        * @param aMsg Message.
        */
        inline TFavouritesMsg( TInt aMsg );

    public:     // Operators

        /**
        * Convert to integer.
        */
        inline operator TInt() const;

    public:     // Getters.

        /**
        * Get function.
        * @return Function.
        */
        inline TFavouritesFunction Function() const;

    };

#include "FavouritesMsg.inl"

#endif

// End of File
