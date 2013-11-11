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
*      Declaration of class CFavouritesSrvDb.
*      
*
*/


#ifndef FAVOURITES_SRV_DB_H
#define FAVOURITES_SRV_DB_H

//  INCLUDES

#include <e32base.h>
#include <d32dbms.h>
#include <f32file.h>
#include <FavouritesLimits.h>
#include "FavouritesSrvTable.h"
#include "UnicodeFile.h"
#include <FavouritesDb.h>


// FORWARD DECLARATIONS

class CFavouritesItemImpl;
class CFavouritesItemImplList;
class MFavouritesItemData;
class TFavouritesFilter;
class MStreamBuf;
class CUidMap;
class CFavouritesFolder;

// CLASS DECLARATION

/**
* Server side favourites database.
*/
NONSHARABLE_CLASS(CFavouritesSrvDb): public CBase
    {

    public:     // Constructors and destructor
    
        /**
        * Two-phased constructor. Leaves on failure.
        * @param aFs File server session handle.
        * @param aDbs DBMS session handle.
        * @param aPath Database path.
        * @param aDBName Database name.        
        * @return The constructed db.
        */
        static CFavouritesSrvDb* NewL
            ( RFs& aFs, RDbs& aDbs, const TDesC& aPath, const TDesC& aDBName );

        /**
        * Destructor.
        */
        virtual ~CFavouritesSrvDb();

    public:     // Adminstration
        
        /**
        * Check if the database has to be recovered to be
        * fully functional.
        * @return ETrue if recovery is needed; EFalse otherwise.
        */
        TBool IsDamagedL();

        /**
        * Perform synchronous recovery. Requires exclusive access
        * to the database.
        */
        void RecoverL();

        /**
        * Perform synchronous compaction. Requires exclusive access
        * to the database.
        */
        void CompactL();

        /**
        * Get available database size.
        * @return Database size.
        */
        RDbDatabase::TSize Size() const;

        /**
        * Update database statistics.
        */
        void UpdateStatsL();

        /**
        * Check transaction.
        */
        inline TBool InTransaction();
    
    public:     // Transaction support

        /**
        * Explicitly begin a transaction.
        * @param aWrite If ETrue, write-lock is placed immediately.
        */
        void BeginL( TBool aWrite = EFalse );

        /**
        * Commit the transaction.
        */
        void CommitL();

        /**
        * Roll back the transaction.
        */
        void Rollback();

    public:     // Data access / update / delete

        /**
        * Get the item having this Uid.
        * @param aUid Uid or item to get.
        * @param aItem placeholder for the returned item data. Must be of type
        * what is returned by CreateItemL().
        */
        void GetL( TInt aUid, CFavouritesItemImpl& aItem );

        /**
        * Get all items matching the supplied criteria.
        * @param aItemList placeholder for the returned item data. Existing
        * items remain (new ones appended).
        * @param aFilter Filters.
        */
        void GetAllL
            (
            CFavouritesItemImplList& aItemList,
            const TFavouritesFilter& aFilter
            );

        /**
        * Get uids of all items matching the supplied criteria.
        * @param aUids placeholder for the returned item data. Existing
        * items remain (new ones appended).
        * @param aFilter Filters.
        * @return Error code.
        */
        void GetUidsL
            ( CArrayFix<TInt>& aUids, const TFavouritesFilter& aFilter );

        /**
        * Get preferred Uid for a folder.
        * @param aFolder Folder Uid.
        * @return - Uid of preferred item in folder aFolder, or
        *         - KFavouritesNullUid if no preferred item or in case of any
        *           errors (aFolder does not exist, is not a folder etc.).
        */
        TInt PreferredUidL( TInt aFolder );

        /**
        * Delete item. If this is a folder, all descendants and the contents
        * of them are deleted. Homepage or root cannot be deleted.
        * @param aUid Uid of item to delete.
        */
        void DeleteL( TInt aUid );

        /**
        * Update item. Homapage or Last Visited Page cannot be updated using
        * this method.
        * @param aItem Contents from this item (except Uid) will be used to
        * update the item. Must be of type what is returned by CreateItemL().
        * If successful, its Uid, Last-Mod-Time (and possibly its name) is
        * updated on return.
        * @param aUid Update this item.
        * @param aAutoRename If this is ETrue, and the name already exists,
        * the item will be renamed to a non-conflicting name.
        */
        void UpdateL
            ( CFavouritesItemImpl& aItem, TInt aUid, TBool aAutoRename );

        /**
        * Add a new item to the database.
        * If successful, its Uid, Last-Mod-Time (and possibly its name) is
        * updated on return.
        * @param aItem The item to add.
        * @param aAutoRename If this is ETrue, and the name already exists,
        * the item will be renamed to a non-conflicting name.
        */
        void AddL( CFavouritesItemImpl& aItem, TBool aAutoRename );

        /**
        * Set Homepage / Last Visited Page.
        * If successful, its Uid and Last-Mod-Time is updated on return.
        * Name needs not be unique. The cursor is left on the
        * added / updated row.
        * @param aItem Item data.
        * @param aUid Uid of the item to be set. Must be
        * KFavouritesHomepageUid or KFavouritesLastVisitedUid.
        */
        void SetSpecialItemL( CFavouritesItemImpl& aItem, TInt aUid );

        /**
        * Set factory item flag on an item.
        * (Item with this flag set will be deleted if RFS is executed.)
        * @param aUid Uid of item.
        * @param aFactoryItem Flag value to set.
        */
        void SetFactoryItemL( TInt aUid, TBool aFactoryItem );

        /**
        * Set read-only flag on an item.
        * @param aUid Uid of item.
        * @param aReadOnly Flag value to set.
        */
        void SetReadOnlyL( TInt aUid, TBool aReadOnly );

        /**
        * Manual setting of Last Modification Time of an item.
        * Note that the Last Modification Time is automatically set by any
        * edit, so this method need not be used in usual circumstances. It is
        * provided for administration purposes only.
        * @param aUid Uid of item.
        * @param aModified Last Modification Time to set.
        */
        void SetModifiedL( TInt aUid, TTime aModified );

        /**
        * Set preferred Uid for a folder.
        * @param aFolder Folder Uid.
        * @param aUid Uid to be set as preferred. Not checked to exist in the
        * folder.
        */
        void SetPreferredUidL( TInt aFolder, TInt aUid );

        /**
        * Check if we have this item.
        * @param aUid The item to check.
        * @return
        *   - ETrue if we have this item;
        *   - EFalse if no such uid, or in case of any error.
        */
        TBool ItemExistsL( TInt aUid );

        /**
        * Check if we have this folder.
        * @param aFolder The folder to check.
        * @return
        *   - ETrue if we have this folder;
        *   - EFalse if no such uid; not a folder; or in case of any error.
        */
        TBool FolderExistsL( TInt aFolder );

        /**
        * Count all items matching the supplied criteria.
        * @param aFilter Filters.
        * @return Item count.
        */
        TInt CountL( const TFavouritesFilter& aFilter );

    public:     // extra data

        /**
        * Set data associated with an item. Any existing data, belonging to
        * item having aUid, is now replaced. The item itself is not changed.
        * In case of any errors, the data is not saved.
        * @param aUid The uid of the item, to which the data belongs.
        * @param aSource Data source.
        */
        void SetDataL( TInt aUid, MStreamBuf& aSource );

        /**
        * Get data associated with an item.
        * @param aUid The uid of the item, to which the data belongs.
        * @param aSink Data sink.
        */
        void GetDataL( TInt aUid, MStreamBuf& aSink );

    public:     // Browser data

        /**
        * Set Browser data associated with an item. Any existing data,
        * belonging to  item having aUid, is now replaced. The item itself is
        * not changed.
        * This data is for Browser's dedicated use, do not tamper.
        * In case of any errors, the data is not saved.
        * @param aUid The uid of the item, to which the data belongs.
        * @param aSource Data source.
        */
        void SetBrowserDataL( TInt aUid, MStreamBuf& aSource );

        /**
        * Get Browser associated with an item.
        * This data is for Browser's dedicated use, do not tamper.
        * @param aUid The uid of the item, to which the data belongs.
        * @param aSink Data sink.
        */
        void GetBrowserDataL( TInt aUid, MStreamBuf& aSink );

    public:     // unique name support

        /**
        * Check if aName is unique in aFolder; and if not, change to
        * an unique one, appending a number. In case of any errors, aName is
        * unchanged. Names of special items (Start Page etc.) are not
        * considered (can have conflicting names).
        * @param aName Descriptor containing the original name and receiving
        * the resulting unique name. Must be large enough to accomodate the
        * result. (The appended text is KFavouritesMaxPostfix characters at
        * most; the resulting length is KFavouritesMaxName at most.)
        * @param aFolder Folder to be used for uniqueness-checking.
        */
        void MakeUniqueNameL( TDes& aName, TInt aFolder );

    public:     // new methods

        /**
        * User-level Restore Factory Settings operation.
        * Delete all items that has "factory item" flag set, then add new
        * ones from reference database. In case of name conflicts, new
        * names are generated. Leaves on any error.
        * @param aReferenceDbPath Full pathname of reference database.
        * @param aApMap AP map. After AP mapping is done, SetAccessPointsL
        * is to be called.
        * Expects a transaction.
        */
        void RestoreFactorySettingsL
            ( const TDesC& aReferenceDbPath, CUidMap& aApMap );

        /**
        * Get filename for uid.
        * @param aUid Uid.
        * @param aParse Filename is returned here.
        */

        void InitSavedFilePathL(const TDesC&  aDBName );    
        
        

        void FilenameL( TInt aUid, TParse& aParse );
      
    protected:      // Constructors

        /**
        * Constructor.
        * @param aFs File server session handle.
        * @param aDbs DBMS session handle.
        */
        CFavouritesSrvDb( RFs& aFs, RDbs& aDbs );


        /**
        * Second-phase constructor.
        * @param aPath Database path.
        * @param aDBName Database name.
        */
        void ConstructL( const TDesC& aPath, const TDesC&  aDBName  );


    protected:  // types

        /**
        * What to do with conflicting names in Add or Update operations.
        */
        enum TRenameType
            {
            EDontRename,    ///< Reject update.
            EAutoRename,    ///< Rename automatically.
            ESuppressCheck  ///< Suppress name conflict checking (accept any).
            };

    protected:  // custom cleanup

        /**
        * Orphaned files cleanup: delete files that has the proper format name
        * but do not have an item associated to them.
        */
        void DeleteOrphanedFilesL();

    protected:  // new methods

        /**
        * BeginL an explicit transaction if there is none in progress.
        * @param aWrite If ETrue, write-lock is placed immediately.
        * @param aOwnTransaction Placeholder for result. If the value is
        * ETrue on return, a transaction was started. To be used as the
        * input parameter for the corresponding CommitIfNeeded(...) call.
        */
        void BeginIfNeededL( TBool aWrite, TBool& aOwnTransaction );

        /**
        * Commit the transaction if aOwnTransaction is ETrue.
        * @param aOwnTransaction If ETrue, Commit will be performed.
        * @return Error code.
        */
        void CommitIfNeededL( TBool aOwnTransaction );

    private:    // new methods

        /**
        * Create the database file, make the database, and the root
        * folder. If anything goes wrong during this, leave. This method
        * expects everything to exist and be open/ready for use.
        * @param aDbFile Parsed filename to use.
        */
        void CreateDatabaseL( TParse& aDbFile, TBool aDoImport );

        /**
        * Create the root entry. Expects the database to be
        * open and not used by anyone else.
        */
        void CreateRootFolderL();

        /**
        * Get the used database. Friend classes use this method.
        * @return Reference to the used database.
        */
        inline RDbDatabase& Database();

        /**
        * Delete the item under the cursor. If it is a folder, delete contents
        * also.
        * Expects a transaction; expects a valid row.
        * @param aDeletedUids Uids of deleted items will be appended to this
        * list.
        */
        void DeleteCurrentL( CArrayFix<TInt>& aDeletedUids );

        /**
        * Update the item under the cursor.
        * Expects a transaction; expects a valid row. Row data not gotten.
        * @param aItem Contents from this item (except Uid) will be used to
        * update the item. Must be of type what is returned by CreateItemL().
        * If successful, its Uid, Last-Mod-Time (and possibly its name) is
        * updated on return.
        * The cursor is left on the updated row.
        * @param aRenameType What to do in case of name conflict.
        */
        void UpdateCurrentL
            ( CFavouritesItemImpl& aItem, TRenameType aRenameType );

        /**
        * Add a new item to the database. Expects a transaction.
        * If successful, its Uid, Last-Mod-Time (and possibly its name) is
        * updated on return.
        * The cursor is left on the added row.
        * @param aItem The item to add.
        * @param aRenameType What to do in case of name conflict.
        * @param aFactoryItem If ETrue, "Factory item" flag will be set.
        * @param aReadOnly If ETrue, "Read-only" flag will be set.
        */
        void DoAddL
            (
            CFavouritesItemImpl& aItem,
            TRenameType aRenameType,
            TBool aFactoryItem = EFalse,
            TBool aReadOnly = EFalse
            );

        /**
        * Check that aName (which has not yet been added/modified in the
        * database) is unique name in aFolder.
        * No sanity check. Expects the folder to exist. Expects a transaction.
        * Overwrites filters.
        * @param aName Name to check. Must be OK.
        * @param aFolder Folder to check in. Must exist.
        * @param aAcceptUid This uid is accepted (in case of update, this is
        * the existing item's own uid).
        * @return ETrue if aName is unique.
        */
        TBool IsUniqueNameL
            ( const TDesC& aName, TInt aFolder, TInt aAcceptUid );

        /**
        * Make a unique name. No sanity checks. Expects a transaction.
        * Expects the name be really conflicting (with some item other than
        * aAcceptUid). Overwrites filters.
        * @param aName Descriptor containing the original name and receiving
        * the resulting unique name.
        * @param aFolder Folder to be used for uniqueness-checking. Must exist.
        * @param aAcceptUid Exclude this item from checking (used when
        * updating aAcceptUid).
        */
        void MakeUniqueNameL( TDes& aName, TInt aFolder, TInt aAcceptUid );

        /**
        * Allocate a buffer and make a unique name in it. No sanity checks.
        * Expects a transaction. Expects the name be really conflicting
        * (with some item other than aAcceptUid). Overwrites filters.
        * @param aName Descriptor containing the original name.
        * @param aFolder Folder to be used for uniqueness-checking. Must exist.
        * @param aAcceptUid Exclude this item from checking (used when
        * updating aAcceptUid).
        * @return A newly allocated buffer, containing the new name. Caller
        * must free this.
        */
        HBufC* AllocUniqueNameLC
            ( const TDesC& aName, TInt aFolder, TInt aAcceptUid );

        /**
        * Static wrapper arounf Rollback().
        * @param aDb The database (CFavouritesDb*) as TAny*.
        */
        static void StaticRollback( TAny* aDb );

    protected:  // RFS support

        /**
        * User-level Restore Factory Settings operation.
        * Delete all items that has "factory item" flag set, then add new
        * ones from reference database. In case of name conflicts, new
        * names are generated. Leaves on any error.
        * @param aRefTable Reference data table.
        * @param aApMap AP map. Filled with AP data: iUid1 = UID of item;
        * iUid2 = current AP id. These AP-s are to be mapped, then
        * SetAccessPointsL is to be called to update.
        * Expects a transaction.
        */
        void RestoreFactorySettingsL
            ( RFavouritesSrvTable& aReferenceTable, CUidMap& aApMap );

    public:     // RFS support

        /**
        * "Batch" setting of Access points - RFS part 2.
        * @param aApMap Access Point map: iUid1 = UID of item; iUid2 = AP id.
        * Expects a transaction.
        */
        void SetAccessPointsL( const CUidMap& aApMap );

    private:    // friends
        
        /// Access to iDbs is granted for this.
        friend class CFavouritesSrvDbNotifier;
        /// Access to iDbs is granted for this.
        friend class CFavouritesSrvDbIncremental;

    private:        // Data

        RFs& iFs;                       ///< File server sess. handle. Not own.
        RDbs& iDbs;                     ///< DBMS session handle. Not own.
        RDbNamedDatabase iDatabase;     ///< Database handle. Own.
        RFavouritesSrvTable iTable;     ///< Database table handle. Own.
        TBool iOpen;                    ///< ETrue if opened.
        TBool iInTransaction;           ///< ETrue in transaction.
        TInt iUpdatesSinceLastCompact;  ///< Updates since last compaction.
        HBufC* iPath;                   ///< Database path. Own.
        HBufC* iSavedFilePath;                   ///< Saved File  path. Own.        
        RBuf iSecureDbFormat;
// -----------------------------------------
// New methods from Bookmarks import utility
// -----------------------------------------
    public:     // New methods

        /**
        * Import bookmarks from Unicode file (comma-separated list).
        * @param aFileName File Name.
        */
        void ImportL( const TDesC& aFileName );

    private:    

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
        
        /**
         * Lookup IAP id by name.
         * @param aName Name of IAP
         * @aIapId contain return value of IAP id
         * @return error status.
         */ 
        TInt IAPIdByNameL( TDesC& aName, TInt& aIapId );

    private:    // data added with integration of bookmark import capability

        RFavouritesSession iSess;   ///< Favourites Engine session.
        RFavouritesDb iDb;          ///< Favourites database. Owned.
        CArrayFixFlat<CFavouritesFolder>*  iFolderNames; // pointer to struct of array of folder name and UID

        RUnicodeFile iFile;         ///< Input file. Owned.
        TUint iCurCh;               ///< Current (lookahead) character.
        TText16* iBuf;              ///< Token buffer.
        TText16* iNextCh;           ///< Next character is stored here.
        TText16* iMaxCh;            ///< Points past the buffer.
    };

#include "FavouritesSrvDb.inl"

#endif
            
// End of File
