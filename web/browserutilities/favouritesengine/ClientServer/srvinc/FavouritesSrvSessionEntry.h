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
*      Declaration of class TFavouritesSessionEntry and
*                           CFavouritesSessionEntryList
*      
*
*/


#ifndef FAVOURITES_SRV_SESSION_ENTRY_H
#define FAVOURITES_SRV_SESSION_ENTRY_H

//  INCLUDES

#include <e32base.h>
#include "FavouritesDef.h"
#include "FavouritesPanic.h"

// FORWARD DECLARATION

class CFavouritesSrvDb;
class CFavouritesSrvDbNotifier;
class CFavouritesSrvDbIncremental;
class HFavouritesSrvStream;
class CFavouritesSrvFile;

// CLASS DECLARATIONS

/**
* Favourites Engine Server Session entry.
*/
NONSHARABLE_CLASS(TFavouritesSessionEntry)
    {
    public:     // types

        enum TType          ///< Entry types.
            {
            ENone,          ///< No object (empty).
            EDatabase,      ///< Database.
            ENotifier,      ///< Notifier.
            EIncremental,   ///< Incremental.
            EStream,        ///< Stream.
            EFile           ///< File.
            };

    public:     // administration

        /**
        * Set object pointer to NULL (initialize / reset).
        */
        inline void SetNull();

        /**
        * Check if NULL.
        * @return ETrue if NULL.
        */
        inline TBool IsNull();

        /**
        * Delete object in entry and set to NULL.
        */
        void Release();

    public:     // operators (setters)

        /**
        * Set from a database.
        */
        inline void operator=( CFavouritesSrvDb& aDb );

        /**
        * Set from a notifier.
        */
        inline void operator=( CFavouritesSrvDbNotifier& aNotifier );

        /**
        * Set from a incremental.
        */
        inline void operator=( CFavouritesSrvDbIncremental& aIncremental );

        /**
        * Set from a stream.
        */
        inline void operator=( HFavouritesSrvStream& aStream );

        /**
        * Set from a file.
        */
        inline void operator=( CFavouritesSrvFile& aFile );

        /**
        * Access check number.
        */
        inline TInt& ChkNum();

    public:     // getters

        /**
        * Get type.
        */
        inline TType Type() const;

        /**
        * Get database.
        * @return Database.
        */
        inline CFavouritesSrvDb& Database();

        /**
        * Get notifier.
        * @return Notifier.
        */
        inline CFavouritesSrvDbNotifier& Notifier();

        /**
        * Get incremental.
        * @return Incremental.
        */
        inline CFavouritesSrvDbIncremental& Incremental();

        /**
        * Get incremental.
        * @return Incremental.
        */
        inline HFavouritesSrvStream& Stream();

        /**
        * Get file.
        * @return File.
        */
        inline CFavouritesSrvFile& File();

        /**
        * Get check number.
        */
        inline TInt ChkNum() const;

    private:    // data

        TType iType;        ///< Type.
        TAny* iObject;      ///< The object. Owned by the session.
        TInt iChkNum;       ///< Check number.
    };

#include "FavouritesSrvSessionEntry.inl"

#endif

// End of File
