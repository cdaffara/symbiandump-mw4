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
*      Implementation of class CFavouritesSrvDb
*      
*
*/


// INCLUDE FILES

#include <s32std.h>
#include <eikenv.h>
#include <rfsApMapper.h>
#include <commdb.h>
#include "FavouritesSrvDb.h"
#include "FavouritesSrvTable.h"
#include "FavouritesItemImpl.h"
#include "FavouritesItemImplList.h"
#include "FavouritesPanic.h"
#include "FavouritesLimits.h"
#include "FavouritesItemData.h"
#include "FavouritesFilter.h"
#include "UidMap.h"
#include "FavouritesLogger.h" 
#include "FavouritesFolder.h"



// CONSTANTS

/// Uid list granularity.
LOCAL_D const TInt KUidListGranularity = 4;
/// Postfix list granularity.
LOCAL_D const TInt KPostfixListGranularity = 4;
/// Number of updates needed to trigger automatic compaction.
LOCAL_D const TInt KUpdatesNeededForAutoCompact = 32;
/// Length of filename: 8 char.
LOCAL_D const TInt KFnameLength = 8;

// Secure policy ID of the Faveng database files.
LOCAL_D const TUint KUidFavengDbPolicy = 0x101FD685;


// ==================== LOCAL FUNCTIONS ====================

/**
* Given aName in the format <prefix> or <prefix><integer in parenthesis>,
* return a pointer to the leading part. That is, if there is trailing
* <integer in  parenthesis>, then that is excluded; if there is no trailing
* part, then the original decriptor is returned.
* Examples:
*   - "Foo" returns "Foo";
*   - "Foo(12)" returns "Foo";
*   - "Foo(12 34)" returns "Foo(12 34)";
*
* @param aName Name to get prefix from.
* @return Pointer to prefix part.
*/
LOCAL_C TPtrC GetPrefix( const TDesC& aName )
    {
    TPtrC prefix = aName;
    TInt length = aName.Length();
    if ( length >= 3 && aName[length - 1] == ')' )
        {
        // Any name shorter than 3 chars cannot have a postfix.
        TInt openPar = aName.LocateReverse('(');
        if ( openPar != KErrNotFound )
            {
            // aName looks like "<prefix>(<something>)".
            // See if <something> is an integer number.
            TPtrC num = aName.Mid( openPar + 1, length - openPar - 2);
            TInt val;
            TLex lex( num );
            if ( lex.Val( val ) == KErrNone )
                {
                // Yes, the trailer is a parenthesized integer.
                prefix.Set( aName.Left( openPar ) );
                }
            }
        }
    return prefix;
    }

/**
* If aName is constructed from aPrefix with a positive integer postfix,
* get the numeric value of the postfix, e.g:
*   - GetPostfix( "Foo(3)", "Foo" ) == 3
* If aName is the same as aPrefix, return 0, e.g.:
*   - GetPostfix( "Foo", "Foo" ) == 0
* If aName is not constructed from aPrefix, return -1, e.g.:
*   - GetPostfix( "Foobar", "Foo" ) == -1
*   - GetPostfix( "Foo(23 45)", "Foo" ) == -1
* If postfix is 0 or negative, return -1, e.g.:
*   - GetPostfix( "Foo(00)", "Foo" ) == -1
*   - GetPostfix( "Foo(-8)", "Foo" ) == -1
*
* Return values for the following cases cannot be differentiated (-1 for all):
* - postfix is 0;
* - postfix is negative;
* - aName not constructed from aPrefix.
*
* @param aName Name to get postfix from.
* @param aPrefix Prefix to be searched.
* @return Postfix value.
*/
LOCAL_C TInt GetPostfix( const TDesC& aName, const TDesC& aPrefix )
    {
    TInt postfix = -1;
    TInt nameLength = aName.Length();
    TInt prefixLength = aPrefix.Length();
    if ( nameLength >= prefixLength && aName.FindF( aPrefix ) == 0 )
        {
        // aName is longer or equal length, and
        // aPrefix can be found in the beginning of aName.
        if ( nameLength == prefixLength )
            {
            // They have the same length; they equal.
            postfix = 0;
            }
        else if (
                nameLength - prefixLength >= 3 &&   // Because of this...
                aName[prefixLength] == '(' &&
                aName[nameLength - 1] == ')'        // ... this is safe (*)
                )
            {
            // (*) "aName[nameLength - 1]" no need to check whether this
            // index is valid. nameLength is at least 3 characters longer
            // than prefixLength (a positive integer) -> nameLength >= 3.

            // aName looks like "aPrefix(<something>)".
            // See if <something> is an integer number.
            TPtrC num = aName.Mid
                ( prefixLength + 1, nameLength - prefixLength - 2 );    // (**)
            // (**) These are also safe because nameLength - prefixLength >= 3.
            TInt val;
            TLex lex( num );
            if ( lex.Val( val ) == KErrNone && val > 0)
                {
                // Yes, the trailer is a positive integer.
                postfix = val;
                }
            }
        }
    return postfix;
    }

/**
* Generate filename for uid: return number as 8 character hex string.
* @param aUid Uid.
* @param aName Filename returned here. Minimum length KFnameLength.
*/
LOCAL_C void FilenameForUid( TInt aUid, TDes& aName )
    {
    __ASSERT_DEBUG( aName.MaxLength() >= KFnameLength, \
        FavouritesPanic( EFavouritesInternal ) );
    aName.NumFixedWidth( STATIC_CAST( TUint, aUid ), EHex, KFnameLength );
    }

/**
* Get uid for filename; reverse of FilenameForUid.
* @param aName Filename.
* @return Uid or KFavouritesNullUid.
*/
LOCAL_C TInt UidForFilename( const TDesC& aName )
    {
    // Parse the supplied text as hex number.
    TInt uid = KFavouritesNullUid;
    if ( aName.Length() == KFnameLength )
        {
        // Length is OK, check contents.
        TUint val;
        TLex lex( aName );
        if ( !lex.Val( val, EHex ) && lex.Eos() )
            {
            // OK, this filename can be parsed as hex TUint.
            uid = STATIC_CAST( TInt, val );
            }
        }
    return uid;
    }

// ================= MEMBER FUNCTIONS =======================


// ---------------------------------------------------------
// CBookmarkSrvDb::NewL
// ---------------------------------------------------------
//
CFavouritesSrvDb* CFavouritesSrvDb::NewL
( RFs& aFs, RDbs& aDbs, const TDesC& aPath, const TDesC& aDBName )
    {
    CFavouritesSrvDb* db = new (ELeave) CFavouritesSrvDb( aFs, aDbs );
    CleanupStack::PushL( db );
    db->ConstructL( aPath , aDBName );
    CleanupStack::Pop();    // db
    return db;
    }


// ---------------------------------------------------------
// CFavouritesSrvDb::~CFavouritesSrvDb
// ---------------------------------------------------------
//
CFavouritesSrvDb::~CFavouritesSrvDb()
    {
    if ( iOpen )
        {
        if ( iInTransaction )
            {
            Rollback();
            }
        TRAP_IGNORE( DeleteOrphanedFilesL() );
        // TODO move compacting to server side, when last client exits.
        (void)iDatabase.Compact();  // Any error is silently ignored.
        iDatabase.Close();
        iOpen = EFalse;
        }
    delete iPath;   
    delete iSavedFilePath;    
    iSecureDbFormat.Close();
    }

// ---------------------------------------------------------
// CFavouritesSrvDb::IsDamagedL
// ---------------------------------------------------------
//
TBool CFavouritesSrvDb::IsDamagedL()
    {
    TBool damaged = EFalse;
    // We must open a table to make RDbNameDatabase::IsDamaged() work!.
    RFavouritesSrvTable table;
    table.OpenL( iDatabase );
    damaged = iDatabase.IsDamaged();
    table.Close();
    return damaged;
    }

// ---------------------------------------------------------
// CFavouritesSrvDb::RecoverL
// ---------------------------------------------------------
//
void CFavouritesSrvDb::RecoverL()
    {
    User::LeaveIfError( iDatabase.Recover() );
    }

// ---------------------------------------------------------
// CFavouritesSrvDb::CompactL
// ---------------------------------------------------------
//
void CFavouritesSrvDb::CompactL()
    {
    User::LeaveIfError( iDatabase.Compact() );
    iUpdatesSinceLastCompact = 0;
    }

// ---------------------------------------------------------
// CFavouritesSrvDb::Size
// ---------------------------------------------------------
//
RDbDatabase::TSize CFavouritesSrvDb::Size() const
    {
    return iDatabase.Size();
    }

// ---------------------------------------------------------
// CFavouritesSrvDb::UpdateStatsL
// ---------------------------------------------------------
//
void CFavouritesSrvDb::UpdateStatsL()
    {
    User::LeaveIfError( iDatabase.UpdateStats() );
    }

// ---------------------------------------------------------
// CFavouritesSrvDb::BeginL
// ---------------------------------------------------------
//
void CFavouritesSrvDb::BeginL( TBool aWrite /*=EFalse*/ )
    {
    __ASSERT_DEBUG( !iInTransaction, \
        FavouritesPanic( EFavouritesNestedTransaction ) );

    // Three parts: table open + transacion + (optional) write lock.
    // Temp use of cleanup items make the three atomic.
    iTable.OpenL( iDatabase );
    CleanupClosePushL<RFavouritesSrvTable>( iTable );
    User::LeaveIfError( iDatabase.Begin() );
    iInTransaction = ETrue;
    CleanupStack::PushL( TCleanupItem( StaticRollback, this ) );
    if ( aWrite )
        {
        iTable.PutWriteLockL();
        }
    CleanupStack::Pop( 2 ); // Rollback, Closing iTable
    }

// ---------------------------------------------------------
// CFavouritesSrvDb::CommitL
// ---------------------------------------------------------
//
void CFavouritesSrvDb::CommitL()
    {
    __ASSERT_DEBUG( iInTransaction, \
        FavouritesPanic( EFavouritesNoTransaction ) );

    User::LeaveIfError( iDatabase.Commit() );
    iInTransaction = EFalse;
    iTable.Close();
    if ( iUpdatesSinceLastCompact > KUpdatesNeededForAutoCompact )
        {
        // Auto-compact, to avoid overgrowth.
        TRAP_IGNORE( CompactL() );
        }
    }

// ---------------------------------------------------------
// CFavouritesSrvDb::Rollback
// ---------------------------------------------------------
//
void CFavouritesSrvDb::Rollback()
    {
    __ASSERT_DEBUG( iInTransaction, \
        FavouritesPanic( EFavouritesNoTransaction ) );

    iDatabase.Rollback();
    iInTransaction = EFalse;
    iTable.Close();
    }

// ---------------------------------------------------------
// CFavouritesSrvDb::GetL
// ---------------------------------------------------------
//
void CFavouritesSrvDb::GetL( TInt aUid, CFavouritesItemImpl& aItem )
    {
    TBool ownTransaction;
    BeginIfNeededL( /*aWrite=*/EFalse, ownTransaction );
    iTable.GotoToUidL( aUid );
    iTable.GetL();
    iTable.ReadItemDataL( aItem );
    CommitIfNeededL( ownTransaction );
    }

// ---------------------------------------------------------
// CFavouritesSrvDb::GetAllL
// ---------------------------------------------------------
//
void CFavouritesSrvDb::GetAllL
( CFavouritesItemImplList& aItemList, const TFavouritesFilter& aFilter )
    {
    TBool ownTransaction;

    BeginIfNeededL( /*aWrite=*/EFalse, ownTransaction );
    iTable.SetFiltersL( aFilter );
    CFavouritesItemImpl* item;
    iTable.BeginningL();
    while( iTable.NextL() )
        {
        item = CFavouritesItemImpl::NewLC();
        iTable.GetL();
        iTable.ReadItemDataL( *item );
        aItemList.AppendL( item );
        CleanupStack::Pop();    // item; owner is the list now.
        }
    iTable.ClearFilters();
    CommitIfNeededL( ownTransaction );
    }

// ---------------------------------------------------------
// CFavouritesSrvDb::PreferredUidL
// ---------------------------------------------------------
//
TInt CFavouritesSrvDb::PreferredUidL( TInt aFolder )
{
    TInt uid( KFavouritesNullUid );
    TBool ownTransaction;
    BeginIfNeededL( /*aWrite=*/EFalse, ownTransaction );
    if ( iTable.SeekToUidL( aFolder ) )
        {
        iTable.GetL();
        if ( iTable.Type() == CFavouritesItem::EFolder )
            {
            uid = iTable.PreferredUid();
            }
        }
    CommitIfNeededL( ownTransaction );
    return uid;
}

// ---------------------------------------------------------
// CFavouritesSrvDb::GetUidsL
// ---------------------------------------------------------
//
void CFavouritesSrvDb::GetUidsL
( CArrayFix<TInt>& aUids, const TFavouritesFilter& aFilter )
    {
    TBool ownTransaction;
    BeginIfNeededL( /*aWrite=*/EFalse, ownTransaction );
    iTable.SetFiltersL( aFilter );
    iTable.BeginningL();
    while( iTable.NextL() )
        {
        iTable.GetL();
        aUids.AppendL( iTable.Uid() );
        }
    iTable.ClearFilters();
    CommitIfNeededL( ownTransaction );
    }

// ---------------------------------------------------------
// CFavouritesSrvDb::DeleteL
// ---------------------------------------------------------
//
void CFavouritesSrvDb::DeleteL( TInt aUid )
    {
    if ( aUid == KFavouritesRootUid || aUid == KFavouritesHomepageUid )
        {
        // Cannot delete these.
        User::Leave( KErrAccessDenied );
        }
    // Make a list, to remember which items were deleted.
    CArrayFix<TInt>* deletedUids =
        new (ELeave) CArrayFixFlat<TInt>( KUidListGranularity );
    CleanupStack::PushL( deletedUids );

    TBool ownTransaction;
    BeginIfNeededL( /*aWrite=*/ETrue, ownTransaction );
    iTable.GotoToUidL( aUid );
    DeleteCurrentL( *deletedUids );
    CommitIfNeededL( ownTransaction );

    // Delete associated files.
    TInt i;
    TBuf<KFnameLength> name;
    TParse parse;
    for ( i = 0; i < deletedUids->Count(); i++ )
        {
        FilenameForUid( deletedUids->At( i ), name );
        User::LeaveIfError( parse.SetNoWild( name, iPath, NULL ) );
        (void)iFs.Delete( parse.FullName() );
        // Errors ignored; the file may be locked etc. etc.
        // Orphaned file cleanup will remove them eventually.
        }

    CleanupStack::PopAndDestroy();  // deletedUids
    }

// ---------------------------------------------------------
// CFavouritesSrvDb::UpdateL
// ---------------------------------------------------------
//
void CFavouritesSrvDb::UpdateL
( CFavouritesItemImpl& aItem, TInt aUid, TBool aAutoRename )
    {
    if ( aUid == KFavouritesRootUid ||
         aUid == KFavouritesHomepageUid ||
         aUid == KFavouritesLastVisitedUid )
        {
        // Cannot modify these.
        // (Root cannot be modified at all; the others can be modified but
        // not through this method.)
        User::Leave( KErrAccessDenied );
        }
    TBool ownTransaction;
    BeginIfNeededL( /*aWrite=*/ETrue, ownTransaction );
    iTable.GotoToUidL( aUid );
    UpdateCurrentL( aItem, aAutoRename ? EAutoRename : EDontRename );
    CommitIfNeededL( ownTransaction );
    }

// ---------------------------------------------------------
// CFavouritesSrvDb::AddL
// ---------------------------------------------------------
//
void CFavouritesSrvDb::AddL( CFavouritesItemImpl& aItem, TBool aAutoRename )
    {
    TBool ownTransaction;
    BeginIfNeededL( /*aWrite=*/ETrue, ownTransaction );
    DoAddL( aItem, aAutoRename ? EAutoRename : EDontRename );
    CommitIfNeededL( ownTransaction );
    }

// ---------------------------------------------------------
// CFavouritesSrvDb::SetSpecialItemL
// ---------------------------------------------------------
//
void CFavouritesSrvDb::SetSpecialItemL( CFavouritesItemImpl& aItem, TInt aUid )
    {
    __ASSERT_DEBUG( \
        aUid == KFavouritesHomepageUid || aUid == KFavouritesLastVisitedUid, \
        FavouritesPanic( EFavouritesInternal )
        );

    if ( aItem.ParentFolder() != KFavouritesRootUid )
        {
        // Must be in root.
        User::Leave( KErrArgument );
        }

    TBool ownTransaction;
    BeginIfNeededL( /*aWrite=*/ETrue, ownTransaction );
    if ( iTable.SeekToUidL( aUid ) )
        {
        UpdateCurrentL( aItem, ESuppressCheck );
        }
    else
        {
        DoAddL( aItem, ESuppressCheck );
        // Cursor is still on the new record. Write uid back.
        iTable.UpdateLC();
        iTable.SetUidL( aUid );
        iTable.PutL();
        aItem.SetUid( aUid );                   // Get uid back.
        aItem.SetModified( iTable.Modified() ); // Get modtime.
        }
    CommitIfNeededL( ownTransaction );
    }

// ---------------------------------------------------------
// CFavouritesSrvDb::SetFactoryItemL
// ---------------------------------------------------------
//
void CFavouritesSrvDb::SetFactoryItemL( TInt aUid, TBool aFactoryItem )
    {
    TBool ownTransaction;
    BeginIfNeededL( /*aWrite=*/ETrue, ownTransaction );
    iTable.GotoToUidL( aUid );
    iTable.UpdateLC();
    iTable.SetFactoryItemL( aFactoryItem );
    iTable.PutL();
    CommitIfNeededL( ownTransaction );
    }

// ---------------------------------------------------------
// CFavouritesSrvDb::SetReadOnlyL
// ---------------------------------------------------------
//
void CFavouritesSrvDb::SetReadOnlyL( TInt aUid, TBool aReadOnly )
    {
    TBool ownTransaction;
    BeginIfNeededL( /*aWrite=*/ETrue, ownTransaction );
    iTable.GotoToUidL( aUid );
    iTable.UpdateLC();
    iTable.SetReadOnlyL( aReadOnly );
    iTable.PutL();
    CommitIfNeededL( ownTransaction );
    }

// ---------------------------------------------------------
// CFavouritesSrvDb::SetModifiedL
// ---------------------------------------------------------
//
void CFavouritesSrvDb::SetModifiedL( TInt aUid, TTime aModified )
    {
    TBool ownTransaction;
    BeginIfNeededL( /*aWrite=*/ETrue, ownTransaction );
    iTable.GotoToUidL( aUid );
    iTable.UpdateLC();
    iTable.SetModifiedL( aModified );
    iTable.PutL( /*aTouch=*/ EFalse );
    CommitIfNeededL( ownTransaction );
    }

// ---------------------------------------------------------
// CFavouritesSrvDb::SetPreferredUidL
// ---------------------------------------------------------
//
void CFavouritesSrvDb::SetPreferredUidL( TInt aFolder, TInt aUid )
    {
    TBool ownTransaction;
    BeginIfNeededL( /*aWrite=*/ETrue, ownTransaction );
    iTable.GotoToUidL( aFolder );
    iTable.GetL();
    if ( iTable.Type() == CFavouritesItem::EFolder )
        {
        iTable.UpdateLC();
        iTable.SetPreferredUidL( aUid );
        iTable.PutL();
        }
    else
        {
        User::Leave( KErrArgument );
        }
    CommitIfNeededL( ownTransaction );
    }

// ---------------------------------------------------------
// CFavouritesSrvDb::ItemExistsL
// ---------------------------------------------------------
//
TBool CFavouritesSrvDb::ItemExistsL( TInt aUid )
    {
    TBool ownTransaction;
    BeginIfNeededL( /*aWrite=*/EFalse, ownTransaction );
    TBool res = iTable.SeekToUidL( aUid );
    CommitIfNeededL( ownTransaction );
    return res;
    }

// ---------------------------------------------------------
// CFavouritesSrvDb::FolderExistsL
// ---------------------------------------------------------
//
TBool CFavouritesSrvDb::FolderExistsL( TInt aFolder )
    {
    TBool res = EFalse;
    TBool ownTransaction;
    BeginIfNeededL( /*aWrite=*/EFalse, ownTransaction );
    if ( iTable.SeekToUidL( aFolder ) )
        {
        iTable.GetL();
        if ( iTable.Type() == CFavouritesItem::EFolder )
            {
            res = ETrue;
            }
        }
    CommitIfNeededL( ownTransaction );
    return res;
    }

// ---------------------------------------------------------
// CFavouritesSrvDb::CountL
// ---------------------------------------------------------
//
TInt CFavouritesSrvDb::CountL( const TFavouritesFilter& aFilter )
    {
    TInt count = 0;
    TBool ownTransaction;
    BeginIfNeededL( /*aWrite=*/EFalse, ownTransaction );
    iTable.SetFiltersL( aFilter );
    // Get all items in the aFolder.
    iTable.BeginningL();
    while( iTable.NextL() )
        {
        count++;
        }
    iTable.ClearFilters();
    CommitIfNeededL( ownTransaction );
    return count;
    }

// ---------------------------------------------------------
// CFavouritesSrvDb::SetDataL
// ---------------------------------------------------------
//
void CFavouritesSrvDb::SetDataL( TInt aUid, MStreamBuf& aSource )
    {
    TBool ownTransaction;
    BeginIfNeededL( /*aWrite=*/ETrue, ownTransaction );
    iTable.GotoToUidL( aUid );
    iTable.UpdateLC();
    iTable.SetExtraDataL( aSource );
    iTable.PutL();
    CommitIfNeededL( ownTransaction );
    }

// ---------------------------------------------------------
// CFavouritesSrvDb::GetDataL
// ---------------------------------------------------------
//
void CFavouritesSrvDb::GetDataL( TInt aUid, MStreamBuf& aSink )
    {
    TBool ownTransaction;
    BeginIfNeededL( /*aWrite=*/EFalse, ownTransaction );
    iTable.GotoToUidL( aUid );
    iTable.GetL();
    iTable.GetExtraDataL( aSink );
    CommitIfNeededL( ownTransaction );
    }

// ---------------------------------------------------------
// CFavouritesSrvDb::SetBrowserDataL
// ---------------------------------------------------------
//
void CFavouritesSrvDb::SetBrowserDataL( TInt aUid, MStreamBuf& aSource )
    {
    TBool ownTransaction;
    BeginIfNeededL( /*aWrite=*/ETrue, ownTransaction );
    iTable.GotoToUidL( aUid );
    iTable.UpdateLC();
    iTable.SetBrowserDataL( aSource );
    iTable.PutL();
    CommitIfNeededL( ownTransaction );
    }

// ---------------------------------------------------------
// CFavouritesSrvDb::GetBrowserDataL
// ---------------------------------------------------------
//
void CFavouritesSrvDb::GetBrowserDataL( TInt aUid, MStreamBuf& aSink )
    {
    TBool ownTransaction;
    BeginIfNeededL( /*aWrite=*/EFalse, ownTransaction );
    iTable.GotoToUidL( aUid );
    iTable.GetL();
    iTable.GetBrowserDataL( aSink );
    CommitIfNeededL( ownTransaction );
    }

// ---------------------------------------------------------
// CFavouritesSrvDb::MakeUniqueNameL
// ---------------------------------------------------------
//
void CFavouritesSrvDb::MakeUniqueNameL( TDes& aName, TInt aFolder )
    {
    // Trim aName first.
    HBufC* buf = HBufC::NewLC( aName.Length() + KFavouritesMaxPostfix );
    TPtr ptr( buf->Des() );
    CFavouritesItemImpl::MakeName( aName, ptr );

    if ( !CFavouritesItemImpl::IsValidName( ptr ) )
        {
        User::Leave( KErrBadName ); // aName is invalid.
        }

    TBool ownTransaction;
    BeginIfNeededL( /*aWrite=*/EFalse, ownTransaction );
    if ( !FolderExistsL( aFolder ) )
        {
        User::Leave( KErrArgument );    // No parent folder.
        }

    if ( !IsUniqueNameL( ptr, aFolder, KFavouritesNullUid ) )
        {
        MakeUniqueNameL( ptr, aFolder, KFavouritesNullUid );
        }
    // Always set it back, even if it was unique in the first place:
    // It may be trimmed.
    aName = ptr;

    CommitIfNeededL( ownTransaction );
    CleanupStack::PopAndDestroy();  // buf
    }

// ---------------------------------------------------------
// CFavouritesSrvDb::RestoreFactorySettingsL
// ---------------------------------------------------------
//
void CFavouritesSrvDb::RestoreFactorySettingsL
( const TDesC& aReferenceDbPath, CUidMap& aApMap )
    {
    __ASSERT_DEBUG( InTransaction(), FavouritesPanic( EFavouritesInternal ) );
    FLOG(( _L("restoer: FavoritesSvrDb.cpp CFavouritesSrvDb::RestoreFactorySettingsL") ));
        
    // Open a read-only database on the reference db (client-side access).
    RDbNamedDatabase refDb;
    
    FLOG(( _L("CFavouritesSrvDb::RestoreFactorySettingsL before Leave call") ));    

    RFs fsSession;
    User::LeaveIfError(fsSession.Connect());
    
    CleanupClosePushL(fsSession);
    User::LeaveIfError(refDb.Open(fsSession, aReferenceDbPath, TPtrC(), RDbNamedDatabase::EReadOnly));

    FLOG(( _L("CFavouritesSrvDb::RestoreFactorySettingsL after Leave call") ));
    CleanupClosePushL<RDbNamedDatabase>( refDb );
    // Open a read-only table on the database.
    RFavouritesSrvTable refTable;
    refTable.OpenL( refDb, RDbRowSet::EReadOnly );
    CleanupClosePushL<RFavouritesSrvTable>( refTable );
    // No transaction needed - the database is opened read-only
    // with exclusive access.
    RestoreFactorySettingsL( refTable, aApMap );
    CleanupStack::PopAndDestroy( 2 );   // close refTable, close refDb

    CleanupStack::PopAndDestroy(); // close fsSession
    }
    

// ---------------------------------------------------------
// CFavouritesSession::InitSavedFilePathL
// ---------------------------------------------------------
//
void CFavouritesSrvDb::InitSavedFilePathL( const TDesC&  aDBName )
    {
    TParse parse;
    TDriveName cDrive = TDriveUnit( EDriveC ).Name();
    TFileName path;
    User::LeaveIfError( iFs.PrivatePath( path ) );
    User::LeaveIfError( parse.SetNoWild( path, &cDrive, NULL ) );
    parse.AddDir(aDBName);       
    delete iSavedFilePath;
	iSavedFilePath = NULL;

    iSavedFilePath =  parse.FullName().AllocL();
    iFs.CreatePrivatePath(EDriveC);
    iFs.MkDirAll(*iSavedFilePath);    
    }
    
    
    
// ---------------------------------------------------------
// CFavouritesSrvDb::FilenameL
// ---------------------------------------------------------
//
void CFavouritesSrvDb::FilenameL( TInt aUid, TParse& aParse )
    {
    // Check if item exists.
    TBool ownTransaction;
    BeginIfNeededL( /*aWrite=*/EFalse, ownTransaction );
    iTable.GotoToUidL( aUid );
    CommitIfNeededL( ownTransaction );
    // Generate filename.
    TBuf<KFnameLength> name;
    FilenameForUid( aUid, name );
    // Append filename to session path.
    User::LeaveIfError( aParse.SetNoWild( name, iSavedFilePath, NULL ) );
    }

// ---------------------------------------------------------
// CFavouritesSrvDb::CFavouritesSrvDb
// ---------------------------------------------------------
//
CFavouritesSrvDb::CFavouritesSrvDb( RFs& aFs, RDbs& aDbs )
: iFs( aFs ),
  iDbs( aDbs ),
  iOpen( EFalse ),
  iInTransaction( EFalse ),
  iUpdatesSinceLastCompact( 0 )
    {
    }


// ---------------------------------------------------------
// CFavouritesSrvDb::ConstructL
// ---------------------------------------------------------
//
void CFavouritesSrvDb::ConstructL( const TDesC& aPath, const TDesC&  aDBName )
    {
    const TUid KDbPolicyUid = TUid::Uid(KUidFavengDbPolicy);
    
    iPath = aPath.AllocL();
    
    InitSavedFilePathL(aDBName );

	_LIT( KSecureKeyword, "SECURE" );
    iSecureDbFormat.CreateL( KSecureKeyword(), KSecureKeyword().Length() + 
    	KDbPolicyUid.Name().Length() );
	iSecureDbFormat += KDbPolicyUid.Name();
    
    // Check if we have the database.
    TBool isDbExists = EFalse;
    
    CDbDatabaseNames *dbNames = iDbs.DatabaseNamesL(EDriveC, KDbPolicyUid);
    
    CleanupStack::PushL(dbNames);
    
    TParse dbFile;
    User::LeaveIfError( dbFile.SetNoWild( aPath, NULL, NULL ) );
    
    for(int i = 0; i < dbNames->Count(); i++)
    	{
    	if( (*dbNames)[i] == dbFile.NameAndExt() )
    		{
    		isDbExists = ETrue;
    		break;
    		}
    	}
    
    CleanupStack::PopAndDestroy(dbNames);
    
    if ( !isDbExists )
        {
        // No such database. Make it now.
        TInt err = KErrNone;
        TBool doImport = ( aDBName.Compare( KBrowserBookmarks ) == 0 );
        TRAP( err, CreateDatabaseL( dbFile, doImport ) );

        // Bookmark import failed. Create an empty DB
        if ( err && doImport )
        	{
        	iDbs.DeleteDatabase( aPath, KDbPolicyUid );
        	TRAP( err, CreateDatabaseL( dbFile, EFalse ) );
        	}
        if ( err )
            {
            // Should anything go wrong during the database creation,
            // delete the database. This ensures that we never have
            // half-constructed database (e.g. database created but no
            // root folder because of leave). No data is lost by deleting
            // the file; it has just been created.
            iDbs.DeleteDatabase( aPath, KDbPolicyUid );
            User::Leave( err );
            }
        }

    // Here we have the database, successfully created. It is closed now.
    // Open it and make some repairs if needed.
	TInt error;
	error = iDatabase.Open( iDbs, dbFile.FullName(), iSecureDbFormat );
	if (error != KErrNone)
		{
		User::Leave( error );
		}
    iOpen = ETrue;
    if ( IsDamagedL() )
        {
        RecoverL();
        }
    // Verify structure and upgrade if needed.
    RFavouritesSrvTable::VerifyStructureL( iDatabase, ETrue );
    }

// ---------------------------------------------------------
// CFavouritesSrvDb::DeleteOrphanedFilesL
// ---------------------------------------------------------
//
void CFavouritesSrvDb::DeleteOrphanedFilesL()
    {
    // Get all uids and sort them by uid.
    CArrayFix<TInt>* uids =
        new (ELeave) CArrayFixFlat<TInt>( KUidListGranularity );
    CleanupStack::PushL( uids );
    GetUidsL( *uids, TFavouritesFilter() );
    TKeyArrayFix key( 0, ECmpTInt32 );
    uids->Sort( key );

    // Get a directory listing.
    RDir dir;
    User::LeaveIfError( dir.Open( iFs, *iSavedFilePath, KEntryAttNormal ) );
    CleanupClosePushL<RDir>( dir );

    TEntryArray files;
    TInt err = dir.Read( files );
    if( err != KErrEof )    // KErrEof means all files read.
        {
        User::Leave( err );
        }

    // Now check if any of the files is orphaned.
    TParse parse;
    TInt dummy;
    TInt uid;
    TInt i;
    const TInt KElementFound = 0;
    
    for ( i = 0; i < files.Count(); i++ )
        {
        uid = UidForFilename( files[i].iName );
        if ( uid == KFavouritesNullUid || 
        	 uids->FindIsq( uid, key, dummy ) != KElementFound )
            {
            // The filename is in the required format (8 char hex number),
            // but no item for this file -> an orphaned file.
            User::LeaveIfError
                ( parse.SetNoWild( files[i].iName, iSavedFilePath, NULL ) );
            (void)iFs.Delete( parse.FullName() );
            }
        }

    CleanupStack::PopAndDestroy( 2, uids ); // dir, uids
    }


// ---------------------------------------------------------
// CFavouritesSrvDb::BeginIfNeededL
// ---------------------------------------------------------
//
void CFavouritesSrvDb::BeginIfNeededL( TBool aWrite, TBool& aOwnTransaction )
    {
    aOwnTransaction = EFalse;
    if ( !iInTransaction )
        {
        // Not in transaction yet.
        BeginL( aWrite );
        aOwnTransaction = ETrue;
        CleanupStack::PushL( TCleanupItem( StaticRollback, this ) );
        }
    }

// ---------------------------------------------------------
// CFavouritesSrvDb::CommitIfNeeded
// ---------------------------------------------------------
//
void CFavouritesSrvDb::CommitIfNeededL( TBool aOwnTransaction )
    {
    if ( aOwnTransaction )
        {
        CommitL();
        CleanupStack::Pop();    // StaticRollback, pushed in BeginL
        }
    }


// ---------------------------------------------------------
// CFavouritesSrvDb::CreateDatabaseL
// ---------------------------------------------------------
//
void CFavouritesSrvDb::CreateDatabaseL( TParse& aDbFile, TBool aDoImport )
    {
    // Make the database.
    // 
    TInt errorCode = iDatabase.Create( iDbs, aDbFile.FullName(), iSecureDbFormat);
    
    User::LeaveIfError( errorCode );
    CleanupClosePushL<RDbNamedDatabase>( iDatabase );
	// Create table.
    RFavouritesSrvTable::CreateStructureL( iDatabase );
    // Create the root folder. No need to close the database after schema
    // modifications in ER5.
    CreateRootFolderL();
    // If the bookmark import file exists on the z: drive, re-import the bookmarks.
#ifndef __WINSCW__
    _LIT(KBookmarkImportFile,"z:\\data\\BookmarkImportSample.txt");            // armv5
#else
    _LIT(KBookmarkImportFile,"c:\\private\\10008d38\\BookmarkImportSample.txt"); // winscw
#endif
    if( aDoImport )
    	{
        TUint attImport;
        User::LeaveIfError( iFs.Connect() );
        TInt errImport = iFs.Att( KBookmarkImportFile(), attImport );
        if ( errImport == KErrNone )
           {
           // Bookmark import file exists.  Import to browserbookmarks.db database.
           ImportL( KBookmarkImportFile() );
           }
    	}
    CleanupStack::PopAndDestroy();  // close iDatabase
    }



// ---------------------------------------------------------
// CFavouritesSrvDb::CreateRootFolderL
// ---------------------------------------------------------
//
void CFavouritesSrvDb::CreateRootFolderL()
    {
    // Database just created; we have exclusive access.
    iTable.OpenL( iDatabase );
    CleanupClosePushL<RFavouritesSrvTable>( iTable );
    CFavouritesItemImpl* root = CFavouritesItemImpl::NewLC();
    // Add a new empty folder to the database.
    root->SetType( CFavouritesItem::EFolder );
    root->SetParentFolder( KFavouritesNullUid );
    iTable.InsertLC();
    iTable.WriteItemDataL( *root );
    // Now the cursor is positioned over the new entry; change its Uid
    // so it becomes the root folder.
    iTable.SetUidL( KFavouritesRootUid );
    iTable.PutL();
    // Now we have the root. The first entry added came with 0 Uid, so this
    // value will never be used again.
    CleanupStack::PopAndDestroy( 2 );   // root, close iTable
    }

// ---------------------------------------------------------
// CFavouritesSrvDb::DeleteCurrentL
// ---------------------------------------------------------
//
void CFavouritesSrvDb::DeleteCurrentL( CArrayFix<TInt>& aDeletedUids )
    {
    TInt uid;
    iTable.GetL();
    if( iTable.ReadOnly() )
        {
        User::Leave( KErrAccessDenied );
        }
    if ( iTable.Type() == CFavouritesItem::EFolder )
        {
        // This is a folder. Delete contents.
        // Save cursor pos, since the cursor will be moved.
        TDbBookmark originalPos = iTable.Bookmark();
        iTable.SetFiltersL( TFavouritesFilter( iTable.Uid(),
            CFavouritesItem::ENone, NULL, KFavouritesNullContextId ) );
        iTable.BeginningL();
        while ( iTable.NextL() )
            {
            iTable.GetL();
            if( iTable.ReadOnly() )
                {
                // Read-only item in a non-read-only folder.
                User::Leave( KErrAccessDenied );
                }
            else
                {
                // Get uid of item to be deleted
                uid = iTable.Uid();
                iTable.DeleteL();
                aDeletedUids.AppendL( uid );
                iUpdatesSinceLastCompact++;
                }
            }
        // Restore cursor pos.
        iTable.ClearFilters();
        iTable.GotoL( originalPos );
        iTable.GetL();
        }

    // Now delete the item itself. If it's a folder and there is no error
    // so far, it is empty by now. Error indicates read-only item in this
    // non-read-only folder, so keep the folder as well.
    uid = iTable.Uid();
    iTable.DeleteL();
    aDeletedUids.AppendL( uid );
    iUpdatesSinceLastCompact++;
    }

// ---------------------------------------------------------
// CFavouritesSrvDb::UpdateCurrentL
// ---------------------------------------------------------
//
void CFavouritesSrvDb::UpdateCurrentL
( CFavouritesItemImpl& aItem, CFavouritesSrvDb::TRenameType aRenameType )
    {
    if ( !CFavouritesItemImpl::IsValidName( aItem.Name() ) )
        {
        // CFavouritesItemImpl::IsValidL also checks this; but we want to
        // return different error code for this case, so manually check this
        // first.
        User::Leave( KErrBadName );
        }

    if ( !aItem.IsValid() )
        {
        User::Leave( KErrArgument );
        }

    HBufC* newName = NULL;
    // Save cursor pos; parent folder and name checking will move it.
    TDbBookmark originalPos = iTable.Bookmark();
    iTable.GetL();
    TInt oldParent = iTable.ParentFolder();
    TInt uid = iTable.Uid();
    if ( iTable.Type() != aItem.Type() )
        {
        // Cannot change type.
        User::Leave( KErrArgument );
        }

    if ( iTable.ReadOnly() )
        {
        // Read-only item.
        User::Leave( KErrAccessDenied );
        }

    if ( aItem.Type() == CFavouritesItem::EFolder &&
        aItem.ParentFolder() != KFavouritesRootUid )
        {
        // Folders must be in the root folder
        // (folder hierarchy is only one-level deep).
        User::Leave( KErrArgument );
        }

    if ( oldParent != aItem.ParentFolder() &&
        !FolderExistsL( aItem.ParentFolder() ) )
        {
        // No parent folder.
        // (Not checked if the parent folder has not changed.)
        User::Leave( KErrArgument );
        }

    if ( aRenameType != ESuppressCheck )
        {
        if ( !IsUniqueNameL( aItem.Name(), aItem.ParentFolder(), uid ) )
            {
            if ( aRenameType == EAutoRename )
                {
                // Name is conflicting, rename automatically.
                newName = AllocUniqueNameLC
                    ( aItem.Name(), aItem.ParentFolder(), uid );
                }
            else
                {
                User::Leave( KErrAlreadyExists );
                }
            }
        }

    // All is well so far; update is valid.
    iTable.PutWriteLockL();
    // If there is a new name, set it.
    if ( newName )
        {
        aItem.SetNameL( *newName );
        }
    // Validation may have moved the cursor; set it back.
    iTable.GotoL( originalPos );
    iTable.UpdateLC();                      // Prepare for update.
    iTable.WriteItemDataL( aItem );         // Put the data.
    iTable.SetFactoryItemL( EFalse );
    iTable.PutL();                          // Apply changes.
    aItem.SetUid( iTable.Uid() );           // Get Uid.
    aItem.SetModified( iTable.Modified() ); // Get modtime.
    // Sanity check; it must be the specified Uid.
    __ASSERT_DEBUG( aItem.Uid() == uid,
        FavouritesPanic( EFavouritesInternal ) );
    iUpdatesSinceLastCompact++;

    if ( newName )
        {
        CleanupStack::PopAndDestroy();  // newName
        }
    }

// ---------------------------------------------------------
// CFavouritesSrvDb::DoAddL
// ---------------------------------------------------------
//
void CFavouritesSrvDb::DoAddL
        (
        CFavouritesItemImpl& aItem,
        CFavouritesSrvDb::TRenameType aRenameType,
        TBool aFactoryItem /*=EFalse*/,
        TBool aReadOnly /*=EFalse*/
        )
    {
    if ( !CFavouritesItemImpl::IsValidName( aItem.Name() ) )
        {
        // CFavouritesItemImpl::IsValidL also checks this; but we want to
        // return different error code for this case, so manually check this
        // first.
        User::Leave( KErrBadName );
        }

    if ( !aItem.IsValid() )
        {
        User::Leave( KErrArgument );
        }

    HBufC* newName = NULL;
    if ( aItem.Type() == CFavouritesItem::EFolder &&
        aItem.ParentFolder() != KFavouritesRootUid )
        {
        // Folders must be in the root folder
        // (folder hierarchy is only one-level deep).
        User::Leave( KErrArgument );
        }

    if ( !FolderExistsL( aItem.ParentFolder() ) )
        {
        // No parent folder.
        User::Leave( KErrArgument );
        }

    if ( aRenameType != ESuppressCheck )
        {
        if ( !IsUniqueNameL
                ( aItem.Name(), aItem.ParentFolder(), KFavouritesNullUid ) )
            {
            if ( aRenameType == EAutoRename )
                {
                // Name is conflicting, rename automatically.
                newName = AllocUniqueNameLC
                    ( aItem.Name(), aItem.ParentFolder(), KFavouritesNullUid );
                }
            else
                {
                User::Leave( KErrAlreadyExists );
                }
            }
        }

    // All is well so far; update is valid.
    iTable.PutWriteLockL();
    // If there is a new name, set it.
    if ( newName )
        {
        aItem.SetNameL( *newName );
        }
    // Validation may have moved the cursor; set it back.
    iTable.InsertLC();                      // Prepare for update.
    iTable.WriteItemDataL( aItem );         // Put the data.
    iTable.SetFactoryItemL( aFactoryItem );
    iTable.SetReadOnlyL( aReadOnly );
    iTable.PutL();                          // Apply changes.
    aItem.SetUid( iTable.Uid() );           // Get Uid.
    aItem.SetModified( iTable.Modified() ); // Get modtime.
    // Sanity check; no item can exist with the Null uid.
    __ASSERT_DEBUG( aItem.Uid() != KFavouritesNullUid,
        FavouritesPanic( EFavouritesNullUidInDatabase ) );
    iUpdatesSinceLastCompact++;

    if ( newName )
        {
        CleanupStack::PopAndDestroy();  // newName
        }
    }

// ---------------------------------------------------------
// CFavouritesSrvDb::IsUniqueNameL
// ---------------------------------------------------------
//
TBool CFavouritesSrvDb::IsUniqueNameL
( const TDesC& aName, TInt aFolder, TInt aAcceptUid )
    {
    __ASSERT_DEBUG( CFavouritesItemImpl::IsValidName( aName ), \
        FavouritesPanic( EFavouritesInternal ) );
    TBool unique = ETrue;
    TInt uid;
    iTable.SetFiltersL( TFavouritesFilter( aFolder, CFavouritesItem::ENone,
        &aName, KFavouritesNullContextId ) );
    iTable.BeginningL();
    while ( iTable.NextL() )
        {
        // We have an item with this name; check if it is aAcceptUid or
        // one of the special items (for which names need not be unique).
        iTable.GetL();
        uid = iTable.Uid();
        if (
           uid != aAcceptUid &&
           uid != KFavouritesHomepageUid &&
           uid != KFavouritesLastVisitedUid &&
           uid != KFavouritesStartPageUid &&
           uid != KFavouritesAdaptiveItemsFolderUid
           )
            {
            // It's not aAcceptUid or special item. We have a name clash.
            unique = EFalse;
            break;
            }
        }
    iTable.ClearFilters();
    return unique;
    }

// ---------------------------------------------------------
// CFavouritesSrvDb::MakeNameUniqueL
// ---------------------------------------------------------
//
void CFavouritesSrvDb::MakeUniqueNameL
( TDes& aName, TInt aFolder, TInt aAcceptUid )
    {
    TBool forcePostfix = EFalse;
    __ASSERT_DEBUG( CFavouritesItemImpl::IsValidName( aName ), \
        FavouritesPanic( EFavouritesInternal ) );
    // Make sure the result fits.
    __ASSERT_DEBUG( aName.MaxLength() >= Min\
        ( aName.Length() + KFavouritesMaxPostfix, KFavouritesMaxName ), \
        FavouritesPanic( EFavouritesBufferTooSmall ) );

    TPtrC prefix = GetPrefix( aName );
    if ( prefix.Length() + KFavouritesMaxPostfix > KFavouritesMaxName )
        {
        // Oops, this prefix is so long that if we append the longest possible
        // postfix to it, the result will not fit into a bookmark name!
        // In this case, we truncate the prefix. The truncated prefix is most
        // likely not conflicting as is, but we do the postfix appending
        // anyway.
        prefix.Set
            ( prefix.Left( KFavouritesMaxName - KFavouritesMaxPostfix ) );
        forcePostfix = ETrue;
        }
    HBufC* buf = HBufC::NewLC( prefix.Length() + KFavouritesMaxPostfix );
    _LIT( KFormatPrefix, "%S*" );
    buf->Des().Format( KFormatPrefix, &prefix );
    iTable.SetFiltersL( TFavouritesFilter
        ( aFolder, CFavouritesItem::ENone, buf, KFavouritesNullContextId ) );
    CleanupStack::PopAndDestroy();  // buf

    TInt uid;
    TInt postfix;
    CArrayFix<TInt>* postfixes =
        new (ELeave) CArrayFixFlat<TInt>( KPostfixListGranularity );
    CleanupStack::PushL( postfixes );
    iTable.BeginningL();
    while ( iTable.NextL() )
        {
        iTable.GetL();
        uid = iTable.Uid();
        if (
           uid != aAcceptUid &&
           uid != KFavouritesHomepageUid &&
           uid != KFavouritesLastVisitedUid &&
           uid != KFavouritesStartPageUid &&
           uid != KFavouritesAdaptiveItemsFolderUid
           )
            {
            // Appending all values (incl. negative values and duplicates).
            postfixes->AppendL( GetPostfix( iTable.Name(), prefix ) );
            }
        }
    iTable.ClearFilters();

    // Sort postfixes ascending.
    TKeyArrayFix key( 0, ECmpTInt );
    User::LeaveIfError( postfixes->Sort( key ) );

    // Initial postfix candidate is 0 ("no postfix"), or 1 if forcePostfix
    // is ETrue.
    postfix = forcePostfix ? 1 : 0;
    // Find the first free postfix, above the starting candidate (0 or 1).
    // The loop skips negative values and duplicates.
    TInt i;
    for ( i = 0; i < postfixes->Count(); i++ )
        {
        if ( postfixes->At( i ) < postfix )
            {
            // This value is smaller than the current candidate;
            // candidate stays the current one (do nothing); but keep checking
            // forthcoming bigger values (continue).
            ;
            }
        else if ( postfixes->At( i ) == postfix )
            {
            // This postfix already taken. Candidate is next one.
            postfix++;
            }
        else
            {
            // postfixes->At( i ) > postfix: a "hole" is found.
            //
            // This case could be the condition in the controlling "for" (and
            // the "break" statement could be avoided); but I intentionally
            // placed it here so the logic of the "hole-search" can be seen
            // more clearly.
            break;
            }
        }
    CleanupStack::PopAndDestroy();      // postfixes

    if ( postfix == 0 )
        {
        // First unique name is the prefix without any postfix.
        // Simply set length to the prefix length.
        aName.SetLength( prefix.Length() );
        }
    else
        {
        // Append a new postfix to the prefix.
        // It seems we have to copy the prefix into a buffer.
        // The prefix points to aName, so Format would format aName
        // using the prefix from itself. But Format does some weird
        // tricks (filling the buffer with some rubbish before writing
        // into it). It's not sprintf :-(.
        buf = prefix.AllocLC();
        _LIT( KFormatSmallPostfix, "%S(0%d)" );
        _LIT( KFormatLargePostfix, "%S(%d)" );
        if ( postfix < 10 )
            {
            aName.Format( KFormatSmallPostfix, buf, postfix );
            }
        else
            {
            aName.Format( KFormatLargePostfix, buf, postfix );
            }
        CleanupStack::PopAndDestroy();  // buf
        }

    // Sanity check. The generated name should really be unique.
    __ASSERT_DEBUG \
        ( \
        IsUniqueNameL( aName, aFolder, aAcceptUid ), \
        FavouritesPanic( EFavouritesInternal ) \
        );
    }

// ---------------------------------------------------------
// CFavouritesSrvDb::AllocUniqueNameLC
// ---------------------------------------------------------
//
HBufC* CFavouritesSrvDb::AllocUniqueNameLC
( const TDesC& aName, TInt aFolder, TInt aAcceptUid )
    {
    HBufC* uniqueName = HBufC::NewLC( aName.Length() + KFavouritesMaxPostfix );
    *uniqueName = aName;
    TPtr ptr( uniqueName->Des() );
    MakeUniqueNameL( ptr, aFolder, aAcceptUid );
    return uniqueName;
    }

// ---------------------------------------------------------
// CFavouritesSrvDb::StaticRollback
// ---------------------------------------------------------
//
void CFavouritesSrvDb::StaticRollback( TAny* aDb )
    {
    STATIC_CAST( CFavouritesSrvDb*, aDb )->Rollback();
    }

// ---------------------------------------------------------
// CFavouritesSrvDb::RestoreFactorySettingsL
// ---------------------------------------------------------
//
void CFavouritesSrvDb::RestoreFactorySettingsL
( RFavouritesSrvTable& aReferenceTable, CUidMap& aApMap )
    {
    __ASSERT_DEBUG( InTransaction(), FavouritesPanic( EFavouritesInternal ) );

    // 1. Check ContextId-s of folders in reference table, and clear all those
    //    ContextId-s from our folders.
    //
    //    This part is a bit "out of line" for the Engine, as here we do deal
    //    with ContextId semantics: we ensure that we don't create more folders
    //    with a given ContextId. This ensures the expected correct behaviour
    //    of Seamless Link functionality after an RFS: the newly restored
    //    folders become the Seamless Link folders.
    //    (Note: if the reference table contains more folders with the same
    //    ContextId, they will be mechanically copied. Checking that is really
    //    not our business.)

    TInt32 contextId;
    aReferenceTable.BeginningL();
    while ( aReferenceTable.NextL() )
        {
        aReferenceTable.GetL();
        contextId = aReferenceTable.ContextId();
        if ( contextId != KFavouritesNullContextId )
            {
            // Folders from root with given context id.
            iTable.SetFiltersL( TFavouritesFilter( KFavouritesRootUid,
                CFavouritesItem::EFolder, NULL, contextId ) );
            iTable.BeginningL();
            while( iTable.NextL() )
                {
                iTable.UpdateLC();
                iTable.SetContextIdL( KFavouritesNullContextId );
                iTable.PutL( /*aTouch=*/EFalse );
                }
            iTable.ClearFilters();
            }
        }

    // 2. Delete all items that have the "factory item" flag.
    //    (These are the unmodified factory items - any modification
    //    clears the flag.) The "factory item" and read-only flag of floders are cleared.
    
	iTable.BeginningL();
    while ( iTable.NextL() )
        {
        iTable.GetL();
        if ( iTable.FactoryItem() )
            {
            switch ( iTable.Type() )
                {
                case CFavouritesItem::EItem:
                    {
					// Delete unmodified factory item 
					iTable.DeleteL();
                    break;
                    }

                case CFavouritesItem::EFolder:
                    {
                    // Clear flags for folders.
                    iTable.UpdateLC();
                    iTable.SetFactoryItemL( EFalse );
                    iTable.SetReadOnlyL( EFalse );
                    iTable.PutL( /*aTouch=*/EFalse );
                    break;
                    }

                default:
                    {
                    FavouritesPanic( EFavouritesInternal );
                    break;
                    }
                }
            }
        }

    // 3. Lookup folders by name, creating new ones if necessary. The target
    //    folder gets the attributes of the source folder (read-only and
    //    factory item flag), regardless of whether it is existing or newly
    //    created. During this process, a mapping table
    //    (source folder uid -> target folder uid) is built. 

    TTime modTime;
    SUidPair mapping;
    TPtrC name;
    CFavouritesItemImpl* item = CFavouritesItemImpl::NewLC();
    CUidMap* parentMap = new (ELeave) CUidMap( 1 );
    CleanupStack::PushL( parentMap );

    // Add the "root->root" mapping to the map.
    mapping.iUid1 = KFavouritesRootUid;
    mapping.iUid2 = KFavouritesRootUid;
    parentMap->AppendL( mapping );

    // All folder of reference table in root.
    aReferenceTable.SetFiltersL( TFavouritesFilter( KFavouritesRootUid,
        CFavouritesItem::EFolder, NULL, KFavouritesNullContextId ) );
    aReferenceTable.BeginningL();
    while( aReferenceTable.NextL() )
        {
        aReferenceTable.GetL();
        name.Set( aReferenceTable.Name() );
        __ASSERT_DEBUG( name.Length(), FavouritesPanic( EFavouritesInternal ) );
        // Folders matching the name.
        iTable.SetFiltersL( TFavouritesFilter( KFavouritesRootUid,
            CFavouritesItem::EFolder, &name, KFavouritesNullContextId ) );
        iTable.BeginningL();
        if( iTable.NextL() )
            {
            // Folder exists with this name.
            // Copy read-only and factory item flags from reference table.
            // Also copy ContextId (possibly we cleared that earlier).
            iTable.UpdateLC();
            iTable.SetReadOnlyL( aReferenceTable.ReadOnly() );
            iTable.SetFactoryItemL( aReferenceTable.FactoryItem() );
            iTable.SetContextIdL( aReferenceTable.ContextId() );
            iTable.PutL( /*aTouch=*/EFalse );
            }
        else
            {
            // No folder exists with this name.
            // Add now by copying from reference table (incl. modtime).
            aReferenceTable.ReadItemDataL( *item );
            modTime = item->Modified();
            DoAddL
                (
                *item,
                EDontRename,
                /*aFactoryItem=*/item->IsFactoryItem(),
                /*aReadOnly=*/item->IsReadOnly()
                );
            // Cursor is still on the added record. Set modtime.
            // Not using CFavouritesSrvDb::SetModifiedL() - that would mess up
            // the filters.
            iTable.UpdateLC();
            iTable.SetModifiedL( modTime );
            iTable.PutL( /*aTouch=*/ EFalse );
            }
        // In both cases (existing folder updated, or new added), the cursor
        // is now on the target folder. Add a new mapping entry.
        mapping.iUid1 = aReferenceTable.Uid();
        mapping.iUid2 = iTable.Uid();
        parentMap->AppendL( mapping );
        iTable.ClearFilters();
        }
    aReferenceTable.ClearFilters();

    // 4. Replace the unmodified factory items of our table with corresponding items from reference table and 
	// add remaining items in the reference table to our table,
	// with AP mapping and parent folder mapping applied.

    TInt i;
    TFavouritesWapAp ap;
	TInt origId;
	TInt newId;

    // Get all items.
    aReferenceTable.SetFiltersL( TFavouritesFilter ( KFavouritesNullUid,
        CFavouritesItem::EItem, NULL, KFavouritesNullContextId ) );
    aReferenceTable.BeginningL();

    while ( aReferenceTable.NextL() )
        {
        aReferenceTable.GetL();
        aReferenceTable.ReadItemDataL( *item );
        modTime = item->Modified();
		origId = item->Uid();

        // Remap parent folder.
        for( i = 0; i < parentMap->Count(); i++ )
            {
            if( item->ParentFolder() == parentMap->At( i ).iUid1 )
                {
                item->SetParentFolder( parentMap->At( i ).iUid2 );
                break;
                }
            // Parent folder mapping should always be successful.
            __ASSERT_DEBUG( i < parentMap->Count(), \
                FavouritesPanic( EFavouritesInternal ) );
            }
				
		// Add the item.
		DoAddL
			(
			*item,
			EAutoRename,
			/*aFactoryItem=*/ETrue,
			/*aReadOnly=*/item->IsReadOnly()
			);	

		// Cursor is still on the added record. Set modtime.
        // Not using CFavouritesSrvDb::SetModifiedL() - that would mess up
        // the filters.
        iTable.UpdateLC();
        iTable.SetModifiedL( modTime );
        iTable.PutL( /*aTouch=*/ EFalse );
		
		// Get the new item UID 
		newId = item->Uid();

		if( !iTable.SeekToUidL( origId ) )
			{
			iTable.GotoToUidL(newId);
			iTable.UpdateLC();
			iTable.SetUidL( origId );
			iTable.PutL( /*aTouch=*/ EFalse );
			item->SetUid( origId );
			}        

        // Check AP, enrol for mapping if needed.
        ap = item->WapAp();
        if ( !ap.IsDefault() && !ap.IsNull() )
            {
            mapping.iUid1 = item->Uid();
            mapping.iUid2 = STATIC_CAST( TInt, ap.ApId() );
            aApMap.AppendL( mapping );
            }
        }
    aReferenceTable.ClearFilters();

    CleanupStack::PopAndDestroy( 2 );   // parentMap, item
    }

// ---------------------------------------------------------
// CFavouritesSrvDb::SetAccessPointsL
// ---------------------------------------------------------
//
void CFavouritesSrvDb::SetAccessPointsL( const CUidMap& aApMap )
    {
    __ASSERT_DEBUG( InTransaction(), FavouritesPanic( EFavouritesInternal ) );
    TInt i;
    TFavouritesWapAp ap;
    for ( i = 0; i < aApMap.Count(); i++ )
        {
        ap.SetApId( STATIC_CAST( TUint32, aApMap[i].iUid2 ) );
        iTable.GotoToUidL( aApMap[i].iUid1 );
        iTable.GetL();
        iTable.UpdateLC();
        iTable.SetWapApL( ap );
        iTable.PutL( /*aTouch=*/ EFalse );
        }
    }


// ---------------------------------------------------------
// IMPORT BOOKMARKS CODE INTEGRATED FROM BOOKMARKS UTILITY
// ---------------------------------------------------------

// CONSTANTS

/// ',' character.
LOCAL_C const TUint KComma = ',';
/// '#' character.
LOCAL_C const TUint KHash = '#';
/// EOF (0) character.
LOCAL_C const TUint KEof = 0;
/// '\r' character.
LOCAL_C const TUint KCr = '\r';
/// '\n' character.
LOCAL_C const TUint KLf = '\n';

/// LF-EOF stop token set.
_LIT( KStopLfEof, "\n\0" );
/// Comma stop token set.
_LIT( KStopComma, "," );
/// Comma-LF-EOF stop token set.
_LIT( KStopCommaLfEof, ",\n\0" );

/// "Folder" kewyword.
_LIT( KFolder, "Folder" );
/// "Item" kewyword.
_LIT( KItem, "Item" );

// ---------------------------------------------------------
// CFavouritesSrvDb::ImportL
// ---------------------------------------------------------
//
void CFavouritesSrvDb::ImportL( const TDesC& aFileName )
    {
    User::LeaveIfError( iFile.Open( iFs, aFileName,
        EFileRead | EFileShareReadersOnly ) );
    CleanupClosePushL<RUnicodeFile>( iFile );

    iFolderNames = new (ELeave) CArrayFixFlat<CFavouritesFolder>( 1 ); 
    User::LeaveIfNull( iFolderNames  );                                
    CleanupStack::PushL( iFolderNames );                               
    
    iBuf = new (ELeave) TText16[KFavouritesMaxUrl];
    User::LeaveIfNull( iBuf );
    CleanupStack::PushL( iBuf );

    iMaxCh = iBuf + KFavouritesMaxUrl;

    GetCharL();

    while( NextLineL() )
       {;}

    TUint count = iFolderNames->Count();
    for (TUint i = 0; i < count; i++)
        {
        CFavouritesFolder folder = iFolderNames->At(i);
        delete folder.Name();
        }
    
    CleanupStack::Pop(iBuf); //iBuf
    delete iBuf;
    CleanupStack::Pop(iFolderNames); //iFolderNames
    delete iFolderNames;
    CleanupStack::PopAndDestroy();  // close iFile
    }

// ---------------------------------------------------------
// CFavouritesSrvDb::GetCharL
// ---------------------------------------------------------
//
void CFavouritesSrvDb::GetCharL()
    {
    iCurCh = iFile.GetCharL();
    if ( iCurCh == KCr )
        {
        // CR character found - ignore it. Not expecting CR to appear anywhere
        // else than before an LF.
        iCurCh = iFile.GetCharL();
        }
    }

// ---------------------------------------------------------
// CFavouritesSrvDb::NextLineL
// ---------------------------------------------------------
//
TBool CFavouritesSrvDb::NextLineL()
    {
    switch( iCurCh )
        {
        case KEof:
            // EOF
            return EFalse;

        case KHash:
            // Comment line; skip over.
            SkipL( KStopLfEof );
            GetCharL();
            break;

        case KCr:
        case KLf:
            // Empty line; skip over.
            GetCharL();
            break;

        default:
            // Parse bookmark attributes.
            AttrsL();
            break;
        }
    return ETrue;
    }

// ---------------------------------------------------------
// CFavouritesSrvDb::NextTokenL
// ---------------------------------------------------------
//
TPtrC CFavouritesSrvDb::NextTokenL( const TDesC& aStopCharSet )
    {
    iNextCh = iBuf; // Start storing token at start of buffer.
    while ( iNextCh < iMaxCh )
        {
        if ( aStopCharSet.Locate( iCurCh ) != KErrNotFound )
            {
            // Stop character found - return what we have stored so far. This
            // may be an empty string as well.
            return TPtrC( iBuf, iNextCh - iBuf );
            }
        *iNextCh = STATIC_CAST( TText16, iCurCh );
        iNextCh++;
        GetCharL();
        }
    // No more space in buffer to store token.
    User::Leave( KErrOverflow );
    /*NOTREACHED*/
    return TPtrC();
    }

// ---------------------------------------------------------
// CFavouritesSrvDb::NextIntTokenL
// ---------------------------------------------------------
//
TInt CFavouritesSrvDb::NextIntTokenL( const TDesC& aStopCharSet )
    {
    TInt ret = 0;
    TPtrC token( NextTokenL( aStopCharSet ) );
    if ( token.Length() )
        {
        TLex lex( token );
        User::LeaveIfError( lex.Val( ret ) );
        }
    return ret;
    }

// ---------------------------------------------------------
// CFavouritesSrvDb::NextHexTokenL
// ---------------------------------------------------------
//
TInt32 CFavouritesSrvDb::NextHexTokenL( const TDesC& aStopCharSet )
    {
    TUint32 ret = 0;
    TPtrC token( NextTokenL( aStopCharSet ) );
    if ( token.Length() )
        {
        TLex lex( token );
        User::LeaveIfError( lex.Val( ret, EHex ) );
        }
    return STATIC_CAST( TInt32, ret );
    }

// ---------------------------------------------------------
// CFavouritesSrvDb::SkipL
// ---------------------------------------------------------
//
void CFavouritesSrvDb::SkipL( const TDesC& aStopCharSet )
    {
    // Note that EOF also can be a stop character; aStopChar check has
    // precendence over EOF check. That is the 'expected EOF' case.
    while( aStopCharSet.Locate( iCurCh ) == KErrNotFound )
        {
        if ( iCurCh == KEof )
            {
            // Unexpected EOF.
            User::Leave( KErrEof );
            }
        GetCharL();
        }
    }

// ---------------------------------------------------------
// CFavouritesSrvDb::AttrsL
// ---------------------------------------------------------
//
void CFavouritesSrvDb::AttrsL()
    {
    TPtrC token;
    TInt num = 0;
    TBool readOnly( EFalse );
  //  TBool factoryItem;
    TBool preferred( EFalse );
    TBool hidden( EFalse );
    TFavouritesWapAp ap;
    
    iTable.OpenL( iDatabase );
    CleanupClosePushL<RFavouritesSrvTable>( iTable );
    iTable.BeginningL();
    iTable.InsertLC();

    // Parse the line and fill item.

    // Type (including special items).
    token.Set( NextTokenL( KStopComma ) );
    if ( !token.Compare( KFolder ) )
        {
        iTable.SetTypeL( CFavouritesItem::EFolder );
        }
    else if ( !token.Compare( KItem ) )
        {
        iTable.SetTypeL( CFavouritesItem::EItem );
        }
    else
        {
        // Expected "Folder", "Item"
        User::Leave( KErrCorrupt );
        }
    GetCharL();
    // Name.
    iTable.SetNameL( NextTokenL( KStopComma ) );
    if (iTable.Type() == CFavouritesItem::EFolder)    // Remember the folder name for UID lookup.
        {      
        HBufC* buf = HBufC::NewLC( iTable.Name().Length() );
        TPtr ptr( buf->Des() );
        ptr.Copy( iTable.Name() );
        CFavouritesFolder folder( buf, iTable.Uid() );
        iFolderNames->AppendL( folder );
        CleanupStack::Pop(); // for newlc of buf
        }
    GetCharL();

    // Parent folder.
    iTable.SetParentFolderL( FolderByNameL( NextTokenL( KStopComma ) ) );

    GetCharL();

    // URL.
    iTable.SetUrlL( NextTokenL( KStopComma ) );
    GetCharL();

    // WAP AP.
    token.Set( NextTokenL( KStopComma ) );
    if ( token.Length() )
        {
        TLex lex( token );
        if ( lex.Val( num ) != KErrNone )
           {
             IAPIdByNameL( token, num );
           }
        ap = num;
        }
    else
    	{
        ap.SetDefault();
    	}
    
    iTable.SetWapApL( ap );
    
    GetCharL();

    // User name.
    iTable.SetUsernameL( NextTokenL( KStopComma ) );
    GetCharL();

    // Password.
    iTable.SetPasswordL( NextTokenL( KStopComma ) );
    GetCharL();

    // Read-only flag.
    num = NextIntTokenL( KStopComma );
    if ( num == 0 )
        {
        readOnly = EFalse;
        }
    else if ( num == 1 )
        {
        readOnly = ETrue;
        }
    else
        {
        // Expected "0" or "1".
        User::Leave( KErrCorrupt );
        }
    GetCharL();

    // Factory item flag.
    num = NextIntTokenL( KStopComma );
    if ( num == 0 )
        {
  //      factoryItem = EFalse;
        }
    else if ( num == 1 )
        {
  //      factoryItem = ETrue;
        }
    else
        {
        // Expected "0" or "1".
        User::Leave( KErrCorrupt );
        }
    GetCharL();

    // Context id.
    iTable.SetContextIdL( NextHexTokenL( KStopCommaLfEof ) );
    
    GetCharL();
    num = NextIntTokenL( KStopCommaLfEof );
    if ( num == 0 )
        {
        hidden = EFalse;
        }
    else if ( num == 1 )
        {
        hidden = ETrue;
        }
    else
        {
        // Expected "0" or "1".
        User::Leave( KErrCorrupt );
        }

    iTable.SetHiddenL(hidden);
    
    // No GetCharL yet - PreferredUid is optional, so we need to investigate
    // lookeahed character first.

    // Preferred flag (optional).
    if ( iCurCh == KComma )
        {
        GetCharL();
        num = NextIntTokenL( KStopLfEof );
        if ( num == 0 )
            {
            preferred = EFalse;
            }
        else if ( num == 1 )
            {
            preferred = ETrue;
            }
        else
            {
            // Expected "0" or "1".
            User::Leave( KErrCorrupt );
            }
        }
    GetCharL();

    if ( preferred )
        {
        iTable.SetPreferredUidL( iTable.Uid() );
        }
    iTable.SetReadOnlyL( readOnly );
    iTable.PutL();
    // Now we have the root. The first entry added came with 0 Uid, so this
    // value will never be used again.
    iTable.Close();
    CleanupStack::Pop( );             // close iTable
    }

// ---------------------------------------------------------
// CFavouritesSrvDb::FolderByNameL
// ---------------------------------------------------------
//
TInt CFavouritesSrvDb::FolderByNameL( const TDesC& aName )
    {
    TInt folderUid = KFavouritesRootUid;

    if ( aName.Length() )
        {
        // loop through iFolderNames and if bookmark name is found, return the index/UID where found
        // else return root UID.
        for (TUint x = 0; x < iFolderNames->Count(); ++x)
            {
            CFavouritesFolder folder = iFolderNames->At(x);
            HBufC* fn = folder.Name();
            TPtr ptr( fn->Des());
            if (aName.Compare(ptr) == 0)
                {
                folderUid = folder.Uid();
                return folderUid;
                }
             }
        }

    return folderUid;
    }


// ---------------------------------------------------------
// CFavouritesSrvDb::IAPIdByNameL
// ---------------------------------------------------------
//
TInt CFavouritesSrvDb::IAPIdByNameL( TDesC& aName, TInt& aIapId )
	{
	TUint pushCount = 0;
	TInt retVal = KErrNone;
    TBool found = EFalse;
    TUint32 iapId = 0;
    TInt err = KErrNone;
    
    CCommsDatabase* db = CCommsDatabase::NewL();
	CleanupStack::PushL(db);
	pushCount++;
	CCommsDbTableView* tableView = db->OpenTableLC( TPtrC(WAP_ACCESS_POINT) );

	pushCount++;
	retVal = tableView->GotoFirstRecord();
	
	while( retVal == KErrNone && !found )
		{
	    TBuf< KCommsDbSvrMaxFieldLength > iapName;
	    tableView->ReadTextL( TPtrC( COMMDB_NAME ), iapName );
	    if (iapName.CompareF( aName ) == 0 )
	    	{
	        TRAP( err, tableView->ReadUintL( TPtrC(COMMDB_ID), iapId ) );
	        found = ETrue;
	    	}
	    retVal = tableView->GotoNextRecord();
		}
    	aIapId = iapId;
	
	CleanupStack::PopAndDestroy( pushCount );
	return retVal;
	}
//  End of File  
