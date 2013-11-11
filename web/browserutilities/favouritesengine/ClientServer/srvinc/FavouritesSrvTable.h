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
*      Declaration of class RFavouritesSrvTable.
*      
*
*/


#ifndef FAVOURITES_SRV_TABLE_H
#define FAVOURITES_SRV_TABLE_H

//  INCLUDES

#include <e32base.h>
#include <d32dbms.h>
#include "FavouritesItemImpl.h"

// FORWARD DECLARATION

class MFavouritesItemData;
class TFavouritesFilter;
class MStreamBuf;

// CLASS DECLARATION

/**
* Database table for favourites data.
* This class provides the way to access / manipulate database tables, which
* store favourites data in the table structure defined by the FavouritesEngine.
* Its purpose is to hide the underlying table structure.
* It does not provide data validation or sanity checks, and is not intended
* to be used by users.
*/
NONSHARABLE_CLASS(RFavouritesSrvTable): public RDbTable
    {

    public:     // opening / closing with cleanup support

        /**
        * Open the table on this database, push a Close on the
        * cleanup stack.
        * @param aDb Open table on this database.
        * @param aAccess Access mode.
        * @return Error code.
        */
        void OpenL
            (
            RDbNamedDatabase& aDb,
            RDbRowSet::TAccess aAccess = RDbRowSet::EUpdatable
            );

        /**
        * Close the table, pop the cleanup item.
        */
        void Close();

    public:     // update with cleanup support

        /**
        * Begin insertion, push a Cancel on the cleanup stack.
        */
        void InsertLC();

        /**
        * Begin update, push a Cancel on the cleanup stack.
        */
        void UpdateLC();

        /**
        * Apply changes to database, pop the cleanup item.
        * @param aTouch If ETrue (the default), last modification time is set
        * to current universal time.
        */
        void PutL( TBool aTouch = ETrue );

        /**
        * Cancel changes to database, pop the cleanup item.
        */
        void Cancel();

    public:     // Navigation, filtering

        /**
        * Set filters.
        * @param aFilter Filters.
        */
        void SetFiltersL( const TFavouritesFilter& aFilter );

        /**
        * Clear filters (all records accepted).
        */
        void ClearFilters();

        /**
        * Goto next record. If filtering is on, filters are considered.
        * Otherwise it is eqivalent to base class method.
        * @return ETrue if positioned on a record.
        */
        TBool NextL();

        /**
        * Goto record having this Uid. Filters are not considered.
        * @param aUid Uid of record to go to.
        * @return ETrue if positioned on a record.
        */
        TBool SeekToUidL( TInt aUid );

        /**
        * Goto record having this Uid. Filters are not considered.
        * Leaves with KErrNotFound if no such record exists.
        * @param aUid Uid of record to go to.
        */
        void GotoToUidL( TInt aUid );

    public:     // data retrieval

        /**
        * Read the data from the current row to aItem. Row data must have
        * been gotten; no validation or sanity check is performed.
        * @param aItem This item is filled with data from the
        * current row.
        */
        void ReadItemDataL( CFavouritesItemImpl& aItem ) const;

        /**
        * Get Uid from the current row. Row data must have been retrieved;
        * no sanity check.
        * @return The Uid in the current row.
        */
        TInt Uid() const;

        /**
        * Get Parent Folder Uid from the current row. Row data must have
        * been retrieved; no sanity check.
        * @return Parent Folder's Uid in the current row.
        */
        TInt ParentFolder() const;

        /**
        * Get type from the current row. Row data must have
        * been retrieved; no sanity check.
        * @return Type in the current row.
        */
        CFavouritesItem::TType Type() const;

        /**
        * Get name from the current row. Row data must have been retrieved;
        * no sanity check.
        * @return Pointer containing the name in the current row.
        */
        TPtrC Name() const;

        /**
        * Get URL from the current row. Row data must have been retrieved;
        * no sanity check.
        * @return Buffer containing the URL in the current row. Owner is
        * the caller.
        */
        HBufC* UrlLC() const;

        /**
        * Get Wap AP from the current row. Row data must have been retrieved;
        * no sanity check.
        * @return WAP AP in the current row.
        */
        TFavouritesWapAp WapAp() const;

        /**
        * Get username from the current row. Row data must have been retrieved;
        * no sanity check.
        * @return Pointer containing the name in the current row.
        */
        TPtrC Username() const;

        /**
        * Get password from the current row. Row data must have been retrieved;
        * no sanity check.
        * @return Pointer containing the name in the current row.
        */
        TPtrC Password() const;

        /**
        * Get extra data from the current row. Row data must have been
        * retrieved; no sanity check.
        * @param aSink Data sink.
        */
        void GetExtraDataL( MStreamBuf& aSink );

        /**
        * Get Browser data from the current row. Row data must have been
        * retrieved; no sanity check.
        * @param aSink Data sink.
        */
        void GetBrowserDataL( MStreamBuf& aSink );

        /**
        * Get "factory item" flag from the current row. Row data must have
        * been retrieved; no sanity check.
        * @return "Factory item" flag.
        */
        TBool FactoryItem() const;

        /**
        * Get "read-only" flag from the current row. Row data must have
        * been retrieved; no sanity check.
        * @return "Read-only" flag.
        */
        TBool ReadOnly() const;

        /**
        * Get context id from the current row. Row data must have been
        * retrieved; no sanity check.
        * @return The context id in the current row.
        */
        TInt32 ContextId() const;

        /**
        * Get last modification time from the current row. Row data must have
        * been retrieved; no sanity check.
        * @return The last modification time in the current row, or zero (0)
        * if not available.
        */
        TTime Modified() const;

        /**
        * Get preferred uid from the current row. Row data must have been
        * retrieved; no sanity check.
        * @return The preferred uid in the current row.
        */
        TInt PreferredUid() const;

        /**
        * Get "Hidden" flag from the current row. Row data must have
        * been retrieved; no sanity check.
        * @return "Hidden" flag.
        */
        TBool Hidden() const;
    public:     // data update

        /**
        * Write the data from aItem to the current row. The table must be
        * inserting or updating a row; no validation or sanity check is
        * performed. The current row must not be read-only.
        * @param aItem Data from this item (except Uid and read-only flag) will
        * be written to the current row.
        */
        void WriteItemDataL( const CFavouritesItemImpl& aItem );

        /**
        * Set Uid to the current row. The table must be
        * updating or inserting a row. No sanity check.
        * @param aUid Uid to be written to the current row.
        */
        void SetUidL( TInt aUid );

        /**
        * Set Parent Folder Uid to the current row. The table must be
        * updating or inserting a row. No sanity check.
        * @param aUid Parent Folder's Uid to be written to the current row.
        */
        void SetParentFolderL( TInt aUid );

        /**
        * Set type to the current row. The table must be
        * updating or inserting a row. No sanity check.
        * @param aType Type be written to the current row.
        */
        void SetTypeL( CFavouritesItem::TType aType );

        /**
        * Set name to the current row. The table must be
        * updating or inserting a row. No sanity check.
        * @param aName Name to be written to the current row.
        */
        void SetNameL( const TDesC& aName );

        /**
        * Set URL to the current row. The table must be
        * updating or inserting a row. No sanity check.
        * @param aUrl URL to be written to the current row.
        */
        void SetUrlL( const TDesC& aUrl );

        /**
        * Set WAP AP to the current row. The table must be
        * updating or inserting a row. No sanity check.
        * @param aWapAp WAP AP to be written to the current row.
        */
        void SetWapApL( const TFavouritesWapAp& aWapAp );

        /**
        * Set username to the current row. The table must be
        * updating or inserting a row. No sanity check.
        * @param aUsername Username to be written to the current row.
        */
        void SetUsernameL( const TDesC& aUsername );

        /**
        * Set password to the current row. The table must be
        * updating or inserting a row. No sanity check.
        * @param aPassword Password to be written to the current row.
        */
        void SetPasswordL( const TDesC& aPassword );

        /**
        * Set extra data to the current row. The table must be
        * updating or inserting a row. No sanity check.
        * @param aSource Data source.
        */
        void SetExtraDataL( MStreamBuf& aSource );

        /**
        * Set Browser data to the current row. The table must be
        * updating or inserting a row. No sanity check.
        * @param aSource Data source.
        */
        void SetBrowserDataL( MStreamBuf& aSource );

        /**
        * Set "factory item" flag. The table must be
        * updating or inserting a row. No sanity check.
        * @param aFactoryItem "Factory item" flag to be written to the current
        * row.
        */
        void SetFactoryItemL( TBool aFactoryItem );

        /**
        * Set "read-only" flag. The table must be
        * updating or inserting a row. No sanity check.
        * @param aReadOnly "Read-only"" flag to be written to the current
        * row.
        */
        void SetReadOnlyL( TBool aReadOnly );

        /**
        * Set context id to the current row. The table must be
        * updating or inserting a row. No sanity check.
        * @param aContextId Context id to be written to the current row.
        */
        void SetContextIdL( TInt32 aContextId );

        /**
        * Set last modification time to the current row. The table must be
        * updating or inserting a row. No sanity check.
        * Note: the database may not store this kind of data. This error is
        * silently ignored.
        * @param aModified Last modification time to be written to the current
        * row.
        */
        void SetModifiedL( TTime aModified );

        /**
        * Set preferred uid to the current row. The table must be
        * updating or inserting a row. No sanity check.
        * @param aUid Preferred uid to be written to the current row.
        */
        void SetPreferredUidL( TInt aUid );

        /**
        * Set hidden value to the current row. The table must be
        * updating or inserting a row. No sanity check.
        * @param aHidden hidden value to be written to the current row.
        */
        void SetHiddenL( TBool aHidden );
    public:     // utility

        /**
        * Place a write-lock. The cursor is positioned on an invalid row.
        */
        void PutWriteLockL();

    public:     // creating database structure

        /**
        * Create the appropriate structure (table, columns, indexes) in aDb.
        * @param aDb The database to be used. Must be open.
        */
        static void CreateStructureL( RDbNamedDatabase& aDb );

        /**
        * Verify the appropriate structure (columns, indexes) in aDb.
        * @param aDb The database to be used. Must be open, with no rowsets
        * opened on it.
        * @param aUpgrade If ETrue, old structure databases are upgraded to
        * new format (new columns added as necessary). Upgrade errors are
        * silently ignored.
        */
        static void VerifyStructureL( RDbNamedDatabase& aDb, TBool aUpgrade );

    private:    // implementation details

        /**
        * Get the ordinal numbers for database columns.
        * Expects the table be open.
        */
        void GetColumnNumbersL();

        /**
        * Cancel changes to database; called in leave processing.
        * @param aTable The table (RFavouritesTable*) as TAny*.
        */
        static void StaticCancel( TAny* aTable );

    private:        // Data

        /// Column ordinal for Uid.
        TDbColNo iColNoUid;
        /// Column ordinal for parent folder.
        TDbColNo iColNoParentFolder;
        /// Column ordinal for type.
        TDbColNo iColNoType;
        /// Column ordinal for name.
        TDbColNo iColNoName;
        /// Column ordinal for URL.
        TDbColNo iColNoUrl;
        /// Column ordinal for username.
        TDbColNo iColNoUserName;
        /// Column ordinal for password.
        TDbColNo iColNoPassword;
        /// Column ordinal for WAP AP id.
        TDbColNo iColNoWapApId;
        /// Column ordinal for WAP AP kind.
        TDbColNo iColNoWapApValueKind;
        /// Column ordinal for derived classes' data.
        TDbColNo iColNoExtraData;
        /// Column ordinal for factory item flag.
        TDbColNo iColNoFactoryItem;
        /// Column ordinal for read-ony flag.
        TDbColNo iColNoReadOnly;
        /// Column ordinal for context id.
        TDbColNo iColNoContextId;
        /// Column ordinal for last mod time (can be KDbNullColNo if no such).
        TDbColNo iColNoModified;
        /// Column ordinal for preferred uid (can be KDbNullColNo if no such).
        TDbColNo iColNoPreferredUid;
        /// Column ordinal for Browser data (can be KDbNullColNo if no such).
        TDbColNo iColNoBrowserData;
        ///
        TDbColNo iColNoHidden;
        /// ETrue if filtering is on.
        TBool iFiltering;
        /// Constraint when filtering.
        RDbRowConstraint iFilter;

    };

#endif
            
// End of File
