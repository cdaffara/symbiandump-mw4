/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
*      Declaration of RUnicodeFile.
*      
*
*/


#ifndef BOOKMARK_FILE_IMPORTER_H
#define BOOKMARK_FILE_IMPORTER_H

// INCLUDE FILES

#include <e32base.h>
#include <f32file.h>
#include <FavouritesDb.h>
#include "UnicodeFile.h"

// FORWARD DECLARATIONS

class CFavouritesItem;

// CLASS DECLARATION

/**
* Import bookmark data from unicode text file (comma-separated list).
*/
class CBookmarkFileImporter: public CBase
    {
    public:     // Constructors and destructor

        /**
        * Two-phased constructor. Leaves on failure.
        * @return The constructed importer.
        */
        static CBookmarkFileImporter* NewL();

        /**
        * Destructor.
        */
        virtual ~CBookmarkFileImporter();

    public:     // New methods

        /**
        * Import bookmarks from Unicode file (comma-separated list).
        * @param aFileName File Name.
        */
        void ImportL( const TDesC& aFileName );

    protected:  // Constructors and destructor

        /**
        * Constructor.
        */
        CBookmarkFileImporter();

        /**
        * Second phase constructor. Leaves on failure.
        */
        void ConstructL();

    private:    // New methods

        /**
        * Get next character and store it in iCurCh.
        */
        inline void GetCharL();

        /**
        * Process next line.
        * @return ETrue if more lines to go, EFalse on EOF.
        */
        TBool NextLineL();

        /**
        * Parse next token. Next token spans from current character up to (but
        * excluding) to the next stop character.
        * @param aStopCharSet Stop characters (terminate the token).
        * @return Pointer to token. This may be empty string. Note that the
        * returned pointer is valid until next call (consecutive calls reuse
        * the same buffer).
        */
        TPtrC NextTokenL( const TDesC& aStopCharSet );

        /**
        * Parse next token, which expected to be an integer.
        * @param aStopCharSet Stop characters (terminate the token).
        * @return Numeric value of token (0 for empty token).
        */
        TInt NextIntTokenL( const TDesC& aStopCharSet );

        /**
        * Parse next token, which expected to be a hexadecimal number.
        * @param aStopCharSet Stop characters (terminate the token).
        * @return Numeric value of token (0 for empty token).
        */
        TInt32 NextHexTokenL( const TDesC& aStopCharSet );

        /**
        * @param aStopCharSet Stop characters (terminate the token).
        * @param aStopChar Stop character.
        */
        void SkipL( const TDesC& aStopCharSet );

        /**
        * Parse comma separated list of bookmark values; create and add a
        * bookmark to the bookmark database.
        */
        void AttrsL();

        /**
        * Lookup uid of folder by name.
        * @param aName Name of folder to be found.
        * @return Uid of folder with name aName.
        */
        TInt FolderByNameL( const TDesC& aName );

    private:    // data

        RFavouritesSession iSess;   ///< Favourites Engine session.
        RFavouritesDb iDb;          ///< Favourites database. Owned.
        CFavouritesItem* iItem;     ///< Favourites item. Owned.
        RFs iFs;                    ///< File Server session. Owned.
        RUnicodeFile iFile;         ///< Input file. Owned.
        TUint iCurCh;               ///< Current (lookahead) character.
        TText16* iBuf;              ///< Token buffer.
        TText16* iNextCh;           ///< Next character is stored here.
        TText16* iMaxCh;            ///< Points past the buffer.
    };

#endif
            
// End of File
