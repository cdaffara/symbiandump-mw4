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
*      Implementation of class RFavouritesSrvTable
*      
*
*/


// INCLUDE FILES

#include <s32strm.h>
#include "FavouritesSrvTable.h"
#include "FavouritesPanic.h"
#include "FavouritesItemData.h"
#include "FavouritesFilter.h"

// CONSTANTS

/// Buffer size for formatting SQL query in SetFiltersL().
const TInt KFavouritesMaxSql = 255;

// Database table names.

/// Favourites table name.
_LIT( KFavouritesTableName, "Favourites" );

// Database column names.                                       // First in:

/// "Uid" column name.
_LIT( KFavouritesDbUidColName, "Uid" );                         // 6.6
/// "Parent folder" column name.
_LIT( KFavouritesDbParentColName, "Parent" );                   // 6.6
/// "Type" column name.
_LIT( KFavouritesDbTypeColName, "Type" );                       // 6.6
/// "Name" column name.
_LIT( KFavouritesDbNameColName, "Name" );                       // 6.6
/// "URL" column name.
_LIT( KFavouritesDbUrlColName, "Url" );                         // 6.6
/// "WAP AP ApId" column name.
_LIT( KFavouritesDbApIdColName, "WapApId" );                    // 6.6
/// "WAP AP Value Kind" column name.
_LIT( KFavouritesDbApValueKindColName, "WapApValueKind" );      // 6.6
/// "Username" column name.
_LIT( KFavouritesDbUserNameColName, "Username" );               // 6.6
/// "Password" column name.
_LIT( KFavouritesDbPasswordColName, "Password" );               // 6.6
/// "Derived classes' extra data" column name.
_LIT( KFavouritesDbExtraDataColName, "ExtraData" );             // 6.6
/// "Factory item" column name.
_LIT( KFavouritesDbFactoryItemColName, "FactoryItem" );         // 6.6
/// "Read-only" column name.
_LIT( KFavouritesDbReadOnlyColName, "ReadOnly" );               // 7.0
/// "ContextId" column name.
_LIT( KFavouritesDbContextIdColName, "ContextId" );             // 7.0
/// "Modified" column name.
_LIT( KFavouritesDbModifiedColName, "Modified" );               // 8.0
/// "Preferred Uid" column name.
_LIT( KFavouritesDbPrefUidColName, "PrefUid" );                 // 9.0
/// "Derived classes' extra data" column name.
_LIT( KFavouritesDbBrowserDataColName, "BrowserData" );         // 10.0

_LIT( KFavouritesDbHiddenColName, "Hidden" );                   // 11.0
// Index column names.

/// "Index by Uid" column name.
_LIT( KFavouritesDbUidIdxName, "UidIdx" );

// ================= LOCAL FUNCTIONS =======================

/**
* Append escaped aLiteral to the end of aBuffer. This means appenging aLiteral,
* with all '-s changed to ''-s.
* @param aBuffer Buffer to append to.
* @param aLiteral Literal to append escaped.
*/
LOCAL_C void AppendEscaped( TDes& aBuffer, const TDesC& aLiteral )
    {
    TInt i;
    TUint quote('\'');  // TChar gives warnings in THUMB & ARMI
    for ( i = 0; i < aLiteral.Length(); i++ )
        {
        aBuffer.Append( aLiteral[i] );
        if ( aLiteral[i] == quote )
            {
            // Duplicate quote.
            aBuffer.Append( quote );
            }
        }
    }

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// RFavouritesSrvTable::OpenL
// ---------------------------------------------------------
//
void RFavouritesSrvTable::OpenL
        (
        RDbNamedDatabase& aDb,
        RDbRowSet::TAccess aAccess /*=RDbRowSet::EUpdatable*/
        )
    {
    iFiltering = EFalse;
    User::LeaveIfError( RDbTable::Open( aDb, KFavouritesTableName, aAccess ) );
    CleanupClosePushL<RDbTable>( *this );
    GetColumnNumbersL();
    CleanupStack::Pop();    // closing this
    SetIndex( KFavouritesDbUidIdxName );
    }

// ---------------------------------------------------------
// RFavouritesSrvTable::Close
// ---------------------------------------------------------
//
void RFavouritesSrvTable::Close()
    {
    ClearFilters();
    RDbTable::Close();
    }

// ---------------------------------------------------------
// RFavouritesSrvTable::InsertLC
// ---------------------------------------------------------
//
void RFavouritesSrvTable::InsertLC()
    {
    RDbTable::InsertL();
    CleanupStack::PushL( TCleanupItem( StaticCancel, this ) );
    }

// ---------------------------------------------------------
// RFavouritesSrvTable::UpdateLC
// ---------------------------------------------------------
//
void RFavouritesSrvTable::UpdateLC()
    {
    RDbTable::UpdateL();
    CleanupStack::PushL( TCleanupItem( StaticCancel, this ) );
    }

// ---------------------------------------------------------
// RFavouritesSrvTable::PutL
// ---------------------------------------------------------
//
void RFavouritesSrvTable::PutL( TBool aTouch /*=ETrue*/ )
    {
    if ( aTouch )
        {
        TTime now;
        now.UniversalTime();
        SetModifiedL( now );
        }
    RDbTable::PutL();
    CleanupStack::Pop();    // StaticCancel, pushed by InsertLC or UpdateLC.
    }

// ---------------------------------------------------------
// RFavouritesSrvTable::Cancel
// ---------------------------------------------------------
//
void RFavouritesSrvTable::Cancel()
    {
    // Any updates in this method must also be applied StaticCancel!
    RDbTable::Cancel();
    CleanupStack::Pop();    // StaticCancel, pushed by InsertLC or UpdateLC.
    }

// ---------------------------------------------------------
// RFavouritesSrvTable::SetFiltersL
// ---------------------------------------------------------
//
void RFavouritesSrvTable::SetFiltersL( const TFavouritesFilter& aFilter )
    {
    HBufC* buf = HBufC::NewLC( KFavouritesMaxSql );
    TPtr sql = buf->Des();
    HBufC* tmpBuf = HBufC::NewLC( KFavouritesMaxSql );
    TPtr tmpPtr = tmpBuf->Des();
    _LIT( KNone, "" );
    _LIT( KAnd, "and" );
    TPtrC sConn;

    sConn.Set( KNone );

    if ( aFilter.iName )
        {
        // "name like 'foo*'"
        _LIT( KFormat1, "%S like '" );
        sql.Format( KFormat1, &KFavouritesDbNameColName );
        AppendEscaped( sql, *aFilter.iName );
        sql.Append( TChar('\'') );
        sConn.Set( KAnd );
        }

    if ( aFilter.iType != CFavouritesItem::ENone )
        {
        // "and type = 0"
        _LIT( KFormat2, " %S %S = %d" );
        tmpPtr.Format
            ( KFormat2, &sConn, &KFavouritesDbTypeColName, aFilter.iType );
        sConn.Set( KAnd );
        sql.Append( tmpPtr );
        }

    if ( aFilter.iParentFolder != KFavouritesNullUid )
        {
        // "and parent = 7"
        _LIT( KFormat3, " %S %S = %d" );
        tmpPtr.Format( KFormat3,
            &sConn, &KFavouritesDbParentColName, aFilter.iParentFolder );
        sConn.Set( KAnd );
        sql.Append( tmpPtr );
        }

    if ( aFilter.iContextId != KFavouritesNullContextId )
        {
        // "and contextid = 7"
        _LIT( KFormat3, " %S %S = %d" );
        tmpPtr.Format( KFormat3,
            &sConn, &KFavouritesDbContextIdColName, aFilter.iContextId );
        sConn.Set( KAnd );
        sql.Append( tmpPtr );
        }

    // Clear old one, if any.
    ClearFilters();

    if ( sql.Length() > 0 )
        {
        // Make new constraint, if there is any filter set.
        // Otherwise there will be no constraint.
        User::LeaveIfError
            ( iFilter.Open( *this, TDbQuery( sql, EDbCompareFolded ) ) );
        iFiltering = ETrue;
        }

    CleanupStack::PopAndDestroy( 2 );  // tmpBuf, buf
    }

// ---------------------------------------------------------
// RFavouritesSrvTable::ClearFilters
// ---------------------------------------------------------
//
void RFavouritesSrvTable::ClearFilters()
    {
    if ( iFiltering )
        {
        iFilter.Close();
        iFiltering = EFalse;
        }
    }

// ---------------------------------------------------------
// RFavouritesSrvTable::NextL
// ---------------------------------------------------------
//
TBool RFavouritesSrvTable::NextL()
    {
    while ( RDbTable::NextL() )
        {
        // Use the filters, if any.
        if ( !iFiltering || MatchL( iFilter ) )
            {
            // We have a matching row.
            return ETrue;
            }
        }
    // No matching rows were found.
    return EFalse;
    }

// ---------------------------------------------------------
// RFavouritesSrvTable::SeekToUidL
// ---------------------------------------------------------
//
TBool RFavouritesSrvTable::SeekToUidL( TInt aUid )
    {
    return SeekL( TDbSeekKey( aUid ) );
    }

// ---------------------------------------------------------
// RFavouritesSrvTable::GotoToUidL
// ---------------------------------------------------------
//
void RFavouritesSrvTable::GotoToUidL( TInt aUid )
    {
    if ( !SeekToUidL( aUid ) )
        {
        User::Leave( KErrNotFound );
        }
    }

// ---------------------------------------------------------
// RFavouritesSrvTable::ReadItemDataL
// ---------------------------------------------------------
//
void RFavouritesSrvTable::ReadItemDataL( CFavouritesItemImpl& aItem ) const
    {
    // Fill standard attributes.
    aItem.SetUid( Uid() );
    // No bookmark can exist with the Null Uid.
    __ASSERT_DEBUG( aItem.Uid() != KFavouritesNullUid,
        FavouritesPanic( EFavouritesNullUidInDatabase ) );
    aItem.SetParentFolder( ParentFolder() );
    aItem.SetType( Type() );
    aItem.SetNameL( Name() );
    HBufC* url = UrlLC();
    aItem.SetUrlL( *url );
    CleanupStack::PopAndDestroy();  // url
    aItem.SetUserNameL( Username() );
    aItem.SetPasswordL( Password() );
    aItem.SetWapAp( WapAp() );
    aItem.SetFactoryItem( FactoryItem() );
    aItem.SetReadOnly( ReadOnly() );
    aItem.SetContextId( ContextId() );
    aItem.SetModified( Modified() );
    aItem.SetHidden( Hidden() );
    }

// ---------------------------------------------------------
// RFavouritesSrvTable::Uid
// ---------------------------------------------------------
//
TInt RFavouritesSrvTable::Uid() const
    {
    return ColInt( iColNoUid );
    }

// ---------------------------------------------------------
// RFavouritesSrvTable::ParentFolder
// ---------------------------------------------------------
//
TInt RFavouritesSrvTable::ParentFolder() const
    {
    return ColInt( iColNoParentFolder );
    }

// ---------------------------------------------------------
// RFavouritesSrvTable::Type
// ---------------------------------------------------------
//
CFavouritesItem::TType RFavouritesSrvTable::Type() const
    {
    return STATIC_CAST( CFavouritesItem::TType, ColInt32( iColNoType ) );
    }

// ---------------------------------------------------------
// RFavouritesSrvTable::Name
// ---------------------------------------------------------
//
TPtrC RFavouritesSrvTable::Name() const
    {
    return ColDes( iColNoName );
    }

// ---------------------------------------------------------
// RFavouritesSrvTable::UrlLC
// ---------------------------------------------------------
//
HBufC* RFavouritesSrvTable::UrlLC() const
    {
    // URL is long column.
    RDbColReadStream stream;
    TInt len = ColLength( iColNoUrl );
    HBufC* buf = HBufC::NewLC( len );
    if ( len )
        {
        stream.OpenLC( *this, iColNoUrl );
        TPtr ptr = buf->Des();
        stream.ReadL( ptr, len );
        CleanupStack::PopAndDestroy();  // Close stream
        }
    return buf;
    }

// ---------------------------------------------------------
// RFavouritesSrvTable::WapAp
// ---------------------------------------------------------
//
TFavouritesWapAp RFavouritesSrvTable::WapAp() const
    {
    TFavouritesWapAp wapAp;
    wapAp.iApId = ColUint32 ( iColNoWapApId );
    wapAp.iValueKind = STATIC_CAST
        ( TFavouritesWapAp::TValueKind, ColInt ( iColNoWapApValueKind ) );
    return wapAp;
    }

// ---------------------------------------------------------
// RFavouritesSrvTable::Username
// ---------------------------------------------------------
//
TPtrC RFavouritesSrvTable::Username() const
    {
    return ColDes( iColNoUserName );
    }

// ---------------------------------------------------------
// RFavouritesSrvTable::Password
// ---------------------------------------------------------
//
TPtrC RFavouritesSrvTable::Password() const
    {
    return ColDes( iColNoPassword );
    }

// ---------------------------------------------------------
// RFavouritesSrvTable::GetExtraDataL
// ---------------------------------------------------------
//
void RFavouritesSrvTable::GetExtraDataL( MStreamBuf& aSink )
    {
    RDbColReadStream rs;
    rs.OpenLC( *this, iColNoExtraData );
    RWriteStream ws( &aSink );
    rs.ReadL( ws );
    CleanupStack::PopAndDestroy();  // Close rs
    }

// ---------------------------------------------------------
// RFavouritesSrvTable::GetBrowserDataL
// ---------------------------------------------------------
//
void RFavouritesSrvTable::GetBrowserDataL( MStreamBuf& aSink )
    {
    if ( iColNoBrowserData != KDbNullColNo )
        {
        RDbColReadStream rs;
        rs.OpenLC( *this, iColNoBrowserData );
        RWriteStream ws( &aSink );
        rs.ReadL( ws );
        CleanupStack::PopAndDestroy();  // Close rs
        }
    }

// ---------------------------------------------------------
// RFavouritesSrvTable::FactoryItem
// ---------------------------------------------------------
//
TBool RFavouritesSrvTable::FactoryItem() const
    {
    return ColInt( iColNoFactoryItem );
    }

// ---------------------------------------------------------
// RFavouritesSrvTable::ReadOnly
// ---------------------------------------------------------
//
TBool RFavouritesSrvTable::ReadOnly() const
    {
    return ColInt( iColNoReadOnly );
    }

// ---------------------------------------------------------
// RFavouritesSrvTable::ContextId
// ---------------------------------------------------------
//
TInt32 RFavouritesSrvTable::ContextId() const
    {
    return ColInt32( iColNoContextId );
    }

// ---------------------------------------------------------
// RFavouritesSrvTable::Modified
// ---------------------------------------------------------
//
TTime RFavouritesSrvTable::Modified() const
    {
    TTime modified( 0 );
    if ( iColNoModified != KDbNullColNo )
        {
        modified = ColTime( iColNoModified );
        }
    return modified;
    }

// ---------------------------------------------------------
// RFavouritesSrvTable::PreferredUid
// ---------------------------------------------------------
//
TInt RFavouritesSrvTable::PreferredUid() const
    {
    TInt preferredUid( KFavouritesNullUid );
    if ( iColNoPreferredUid != KDbNullColNo )
        {
        preferredUid = ColInt( iColNoPreferredUid );
        }
    return preferredUid;
    }


// ---------------------------------------------------------
// RFavouritesSrvTable::Hidden
// ---------------------------------------------------------
//
TBool RFavouritesSrvTable::Hidden() const
    {
    return ColInt( iColNoHidden );
    }

// ---------------------------------------------------------
// RFavouritesSrvTable::WriteItemDataL
// ---------------------------------------------------------
//
void RFavouritesSrvTable::WriteItemDataL( const CFavouritesItemImpl& aItem )
    {
    // Fill standard attributes.
    __ASSERT_DEBUG( !ReadOnly(), FavouritesPanic( EFavouritesInternal ) );
    // Uid is *NOT* set. We update the current row.
    SetParentFolderL( aItem.ParentFolder() );
    SetTypeL( aItem.Type() );
    SetNameL( aItem.Name() );
    SetUrlL( aItem.Url() );
    SetUsernameL( aItem.UserName() );
    SetPasswordL( aItem.Password() );
    SetWapApL( aItem.WapAp() );
    // Note: read-only flag is *NOT* set. Setting that is accessible via
    // SetReadOnlyL.
    // Note: factory item flag is *NOT* set. Setting that is accessible via
    // SetFactoryItemL.
    SetContextIdL( aItem.ContextId() );
    // Note: last modification time is *NOT* set; PutL will do it.
    // Manual setting (override) is accessible via SetModifiedL.
    SetHiddenL( aItem.IsHidden() );
    }

// ---------------------------------------------------------
// RFavouritesSrvTable::SetUidL
// ---------------------------------------------------------
//
void RFavouritesSrvTable::SetUidL( TInt aUid )
    {
    SetColL( iColNoUid, aUid );
    }

// ---------------------------------------------------------
// RFavouritesSrvTable::SetParentFolderL
// ---------------------------------------------------------
//
void RFavouritesSrvTable::SetParentFolderL( TInt aUid )
    {
    SetColL( iColNoParentFolder, aUid );
    }

// ---------------------------------------------------------
// RFavouritesSrvTable::SetTypeL
// ---------------------------------------------------------
//
void RFavouritesSrvTable::SetTypeL( CFavouritesItem::TType aType )
    {
    SetColL( iColNoType, STATIC_CAST( TInt, aType ) );
    }

// ---------------------------------------------------------
// RFavouritesSrvTable::SetNameL
// ---------------------------------------------------------
//
void RFavouritesSrvTable::SetNameL( const TDesC& aName )
    {
    SetColL( iColNoName, aName );
    }

// ---------------------------------------------------------
// RFavouritesSrvTable::SetUrlL
// ---------------------------------------------------------
//
void RFavouritesSrvTable::SetUrlL( const TDesC& aUrl )
    {
    RDbColWriteStream stream;
    stream.OpenLC( *this, iColNoUrl );
    stream.WriteL( aUrl, aUrl.Length() );
    stream.CommitL();
    CleanupStack::PopAndDestroy();  // stream;
    }

// ---------------------------------------------------------
// RFavouritesSrvTable::SetWapApL
// ---------------------------------------------------------
//
void RFavouritesSrvTable::SetWapApL( const TFavouritesWapAp& aWapAp )
    {
    SetColL( iColNoWapApId, aWapAp.iApId );
    SetColL( iColNoWapApValueKind,
        STATIC_CAST( TInt, aWapAp.iValueKind ) );
    }

// ---------------------------------------------------------
// RFavouritesSrvTable::SetUsernameL
// ---------------------------------------------------------
//
void RFavouritesSrvTable::SetUsernameL( const TDesC& aUsername )
    {
    SetColL( iColNoUserName, aUsername );
    }

// ---------------------------------------------------------
// RFavouritesSrvTable::SetPasswordL
// ---------------------------------------------------------
//
void RFavouritesSrvTable::SetPasswordL( const TDesC& aPassword )
    {
    SetColL( iColNoPassword, aPassword );
    }

// ---------------------------------------------------------
// RFavouritesSrvTable::SetExtraDataL
// ---------------------------------------------------------
//
void RFavouritesSrvTable::SetExtraDataL( MStreamBuf& aSource )
    {
    RDbColWriteStream ws;
    ws.OpenLC( *this, iColNoExtraData );
    RReadStream rs( &aSource );
    ws.WriteL( rs );
    ws.CommitL();
    CleanupStack::PopAndDestroy();  // Close ws
    }

// ---------------------------------------------------------
// RFavouritesSrvTable::SetBrowserDataL
// ---------------------------------------------------------
//
void RFavouritesSrvTable::SetBrowserDataL( MStreamBuf& aSource )
    {
    if ( iColNoBrowserData != KDbNullColNo )
        {
        RDbColWriteStream ws;
        ws.OpenLC( *this, iColNoBrowserData );
        RReadStream rs( &aSource );
        ws.WriteL( rs );
        ws.CommitL();
        CleanupStack::PopAndDestroy();  // Close ws
        }
    }

// ---------------------------------------------------------
// RFavouritesSrvTable::SetFactoryItemL
// ---------------------------------------------------------
//
void RFavouritesSrvTable::SetFactoryItemL( TBool aFactoryItem )
    {
    SetColL( iColNoFactoryItem, aFactoryItem );
    }

// ---------------------------------------------------------
// RFavouritesSrvTable::SetReadOnlyL
// ---------------------------------------------------------
//
void RFavouritesSrvTable::SetReadOnlyL( TBool aReadOnly )
    {
    SetColL( iColNoReadOnly, aReadOnly );
    }

// ---------------------------------------------------------
// RFavouritesSrvTable::SetContextIdL
// ---------------------------------------------------------
//
void RFavouritesSrvTable::SetContextIdL( TInt32 aContextId )
    {
    SetColL( iColNoContextId, aContextId );
    }

// ---------------------------------------------------------
// RFavouritesSrvTable::SetModifiedL
// ---------------------------------------------------------
//
void RFavouritesSrvTable::SetModifiedL( TTime aModified )
    {
    if ( iColNoModified != KDbNullColNo )
        {
        SetColL( iColNoModified, aModified );
        }
    }

// ---------------------------------------------------------
// RFavouritesSrvTable::SetPreferredUidL
// ---------------------------------------------------------
//
void RFavouritesSrvTable::SetPreferredUidL( TInt aUid )
    {
    if ( iColNoPreferredUid != KDbNullColNo )
        {
        SetColL( iColNoPreferredUid, aUid );
        }
    }

// ---------------------------------------------------------
// RFavouritesSrvTable::SetHiddenL
// ---------------------------------------------------------
//
void RFavouritesSrvTable::SetHiddenL( TBool aHidden )
    {
    SetColL( iColNoHidden, aHidden );
    }

// ---------------------------------------------------------
// RFavouritesSrvTable::PutWriteLockL
// ---------------------------------------------------------
//
void RFavouritesSrvTable::PutWriteLockL()
    {
    // Insert will get write-lock if possible, or leave if not.
    InsertLC();
    Cancel();
    }

// ---------------------------------------------------------
// RFavouritesSrvTable::CreateStructureL
// ---------------------------------------------------------
//
void RFavouritesSrvTable::CreateStructureL( RDbNamedDatabase& aDb )
    {
    // Create columns.
    CDbColSet* colset = CDbColSet::NewLC();

    TDbCol col( KFavouritesDbUidColName, EDbColInt32 );
    col.iAttributes = TDbCol::EAutoIncrement;
    colset->AddL( col );
    colset->AddL( TDbCol( KFavouritesDbParentColName, EDbColInt32 ) );
    colset->AddL( TDbCol( KFavouritesDbTypeColName, EDbColInt32 ) );
    colset->AddL( TDbCol
        ( KFavouritesDbNameColName, EDbColText, KFavouritesMaxName ) );
    colset->AddL( TDbCol
        ( KFavouritesDbUrlColName, EDbColLongText, KFavouritesMaxUrl ) );
    colset->AddL( TDbCol( KFavouritesDbApIdColName, EDbColUint32 ) );
    colset->AddL( TDbCol( KFavouritesDbApValueKindColName, EDbColInt32 ) );
    colset->AddL( TDbCol
        ( KFavouritesDbUserNameColName, EDbColText, KFavouritesMaxUserName ) );
    colset->AddL( TDbCol
        ( KFavouritesDbPasswordColName, EDbColText, KFavouritesMaxPassword ) );
    colset->AddL( TDbCol( KFavouritesDbExtraDataColName, EDbColLongBinary ) );
    colset->AddL( TDbCol( KFavouritesDbFactoryItemColName, EDbColBit ) );
    colset->AddL( TDbCol( KFavouritesDbReadOnlyColName, EDbColBit ) );
    colset->AddL( TDbCol( KFavouritesDbContextIdColName, EDbColInt32 ) );
    colset->AddL( TDbCol( KFavouritesDbModifiedColName, EDbColDateTime ) );
    colset->AddL( TDbCol( KFavouritesDbPrefUidColName, EDbColInt32 ) );
    colset->AddL
        ( TDbCol( KFavouritesDbBrowserDataColName, EDbColLongBinary ) );
    colset->AddL( TDbCol( KFavouritesDbHiddenColName, EDbColBit ) );
    User::LeaveIfError
        ( aDb.CreateTable( KFavouritesTableName, *colset ) );
    CleanupStack::PopAndDestroy();  // colset

    // Create index by uid.
    CDbKey* key = CDbKey::NewLC();
    // Create key on Uid column, ascending order.
    key->AddL( TDbKeyCol ( KFavouritesDbUidColName ) );
    User::LeaveIfError( aDb.CreateIndex
        ( KFavouritesDbUidIdxName, KFavouritesTableName, *key ) );
    CleanupStack::PopAndDestroy();  // key
    }

// ---------------------------------------------------------
// RFavouritesSrvTable::VerifyStructureL
// ---------------------------------------------------------
//
void RFavouritesSrvTable::VerifyStructureL
( RDbNamedDatabase& aDb, TBool aUpgrade )
    {
    CDbColSet* colset = aDb.ColSetL( KFavouritesTableName );
    CleanupStack::PushL( colset );

    if (
        colset->ColNo( KFavouritesDbUidColName ) == KDbNullColNo ||
        colset->ColNo( KFavouritesDbParentColName ) == KDbNullColNo ||
        colset->ColNo( KFavouritesDbTypeColName ) == KDbNullColNo ||
        colset->ColNo( KFavouritesDbNameColName ) == KDbNullColNo ||
        colset->ColNo( KFavouritesDbUrlColName ) == KDbNullColNo ||
        colset->ColNo( KFavouritesDbUserNameColName ) == KDbNullColNo ||
        colset->ColNo( KFavouritesDbPasswordColName ) == KDbNullColNo ||
        colset->ColNo( KFavouritesDbApIdColName ) == KDbNullColNo ||
        colset->ColNo( KFavouritesDbApValueKindColName ) == KDbNullColNo ||
        colset->ColNo( KFavouritesDbExtraDataColName ) == KDbNullColNo ||
        colset->ColNo( KFavouritesDbFactoryItemColName ) == KDbNullColNo ||
        colset->ColNo( KFavouritesDbReadOnlyColName ) == KDbNullColNo ||
        colset->ColNo( KFavouritesDbContextIdColName ) == KDbNullColNo
       )
        {
        // Mandatory column is missing, this database is corrupt.
        User::Leave( KErrCorrupt );
        }

    if ( aUpgrade )
        {
        // Check optional columns, upgrade if necessary.
        // If upgrade is not possible, we don't even check those; as the
        // database is still functional without them.

        TInt missingCols( EFalse );

        if ( colset->ColNo( KFavouritesDbModifiedColName ) == KDbNullColNo )
            {
            // Last modification column is missing.
            colset->AddL( TDbCol
                ( KFavouritesDbModifiedColName, EDbColDateTime ) );
            missingCols = ETrue;
            }

        if ( colset->ColNo( KFavouritesDbPrefUidColName ) == KDbNullColNo )
            {
            // Preferred uid column is missing.
            colset->AddL( TDbCol( KFavouritesDbPrefUidColName, EDbColInt32 ) );
            missingCols = ETrue;
            }

        if ( colset->ColNo( KFavouritesDbBrowserDataColName ) == KDbNullColNo )
            {
            // Preferred uid column is missing.
            colset->AddL
                ( TDbCol( KFavouritesDbBrowserDataColName, EDbColLongBinary ) );
            missingCols = ETrue;
            }
        if( colset->ColNo( KFavouritesDbHiddenColName)  == KDbNullColNo )
            {
            // Preferred uid column is missing.
            colset->AddL
                ( TDbCol( KFavouritesDbHiddenColName, EDbColBit ) );
            missingCols = ETrue;
            }

        if ( missingCols )
            {
            // Some columns are missing, try to add them now.
            // Upgrade error is ignored, database is still functional (except
            // of course the missing columns).
            (void)aDb.AlterTable( KFavouritesTableName, *colset );
            }
        }

    CleanupStack::PopAndDestroy();  // colset
    }

// ---------------------------------------------------------
// RFavouritesSrvTable::GetColumnNumbersL
// ---------------------------------------------------------
//
void RFavouritesSrvTable::GetColumnNumbersL()
    {
    CDbColSet* colset = ColSetL();
    CleanupStack::PushL( colset );

    iColNoUid = colset->ColNo( KFavouritesDbUidColName );
    iColNoParentFolder = colset->ColNo( KFavouritesDbParentColName );
    iColNoType = colset->ColNo( KFavouritesDbTypeColName );
    iColNoName = colset->ColNo( KFavouritesDbNameColName );
    iColNoUrl = colset->ColNo( KFavouritesDbUrlColName );
    iColNoUserName = colset->ColNo( KFavouritesDbUserNameColName );
    iColNoPassword = colset->ColNo( KFavouritesDbPasswordColName );
    iColNoWapApId = colset->ColNo( KFavouritesDbApIdColName );
    iColNoWapApValueKind = colset->ColNo( KFavouritesDbApValueKindColName );
    iColNoExtraData = colset->ColNo( KFavouritesDbExtraDataColName );
    iColNoFactoryItem = colset->ColNo( KFavouritesDbFactoryItemColName );
    iColNoReadOnly = colset->ColNo( KFavouritesDbReadOnlyColName );
    iColNoContextId = colset->ColNo( KFavouritesDbContextIdColName );
    iColNoModified = colset->ColNo( KFavouritesDbModifiedColName );
    iColNoPreferredUid = colset->ColNo( KFavouritesDbPrefUidColName );
    iColNoBrowserData = colset->ColNo( KFavouritesDbBrowserDataColName );
    iColNoHidden = colset->ColNo( KFavouritesDbHiddenColName );

    CleanupStack::PopAndDestroy();  // colset
    }

// ---------------------------------------------------------
// RFavouritesSrvTable::StaticCancel
// ---------------------------------------------------------
//
void RFavouritesSrvTable::StaticCancel( TAny* aTable )
    {
    // Same as Cancel; except it does not pop the cleanup item; because
    // it is called as part of leave processing.
    // Any updates in this method must also be applied Cancel!
    RFavouritesSrvTable* table = STATIC_CAST( RFavouritesSrvTable*, aTable );
    table->RDbTable::Cancel();
    }

//  End of File  
