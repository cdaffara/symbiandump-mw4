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
*      Implementation of class RFavouritesDb
*      
*
*/


// INCLUDE FILES

#include <s32std.h>
#include "FavouritesDb.h"
#include "FavouritesMsg.h"
#include "FavouritesBuf.h"
#include "FavouritesFilter.h"
#include "FavouritesItemImpl.h"
#include "FavouritesItemList.h"
#include "FavouritesItemData.h"
#include "FavouritesUtil.h"
#include "UidMap.h"
#include "FavouritesLogger.h" 
#include <rfsApMapper.h>

// CONSTANTS

/// Granularity of AP map in RFS.
LOCAL_D const TInt KApMapGranularity = 4;

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// RFavouritesDb::Open
// ---------------------------------------------------------
//
EXPORT_C TInt RFavouritesDb::Open
( RFavouritesSession& aSess, const TDesC& aName )
    {
    __ASSERT_DEBUG( !iHandle, FavouritesPanic( EFavouritesAlreadyOpen ) );
    TInt ret = aSess.SendReceive( EFavengOpenDatabase, TIpcArgs( &aName ) );
    if ( ret > 0 )
        {
        iSess = aSess;
        iHandle = ret;
        ret = KErrNone;
        }
    return ret;
    }

// ---------------------------------------------------------
// RFavouritesDb::Version
// ---------------------------------------------------------
//
EXPORT_C TVersion RFavouritesDb::Version() const
    {
    return RFavouritesSession::Version();
    }

// ---------------------------------------------------------
// RFavouritesDb::IsDamaged
// ---------------------------------------------------------
//
EXPORT_C TInt RFavouritesDb::IsDamaged( TBool& aIsDamaged )
    {
    TInt ret = SendReceive( EFavengIsDamaged );
    if ( ret >= 0 )
        {
        aIsDamaged = ret;
        ret = KErrNone;
        }
    return ret;
    }

// ---------------------------------------------------------
// RFavouritesDb::Recover
// ---------------------------------------------------------
//
EXPORT_C TInt RFavouritesDb::Recover()
    {
    return SendReceive( EFavengRecover );
    }

// ---------------------------------------------------------
// RFavouritesDb::Compact
// ---------------------------------------------------------
//
EXPORT_C TInt RFavouritesDb::Compact()
    {
    return SendReceive( EFavengCompact );
    }

// ---------------------------------------------------------
// RFavouritesDb::Size
// ---------------------------------------------------------
//
EXPORT_C TInt RFavouritesDb::Size( RDbDatabase::TSize& aSize ) const
    {
    TPckg<RDbDatabase::TSize> size( aSize );
    return SendReceive( EFavengSize, TIpcArgs( &size ) );
    }

// ---------------------------------------------------------
// RFavouritesDb::UpdateStats
// ---------------------------------------------------------
//
EXPORT_C TInt RFavouritesDb::UpdateStats()
    {
    return SendReceive( EFavengUpdateStats );
    }

// ---------------------------------------------------------
// RFavouritesDb::Begin
// ---------------------------------------------------------
//
EXPORT_C TInt RFavouritesDb::Begin( TBool aWrite /*=EFalse*/ )
    {
    return SendReceive( EFavengBegin, TIpcArgs( aWrite ) );
    }

// ---------------------------------------------------------
// RFavouritesDb::Commit
// ---------------------------------------------------------
//
EXPORT_C TInt RFavouritesDb::Commit()
    {
    return SendReceive( EFavengCommit );
    }

// ---------------------------------------------------------
// RFavouritesDb::Rollback
// ---------------------------------------------------------
//
EXPORT_C void RFavouritesDb::Rollback()
    {
    (void) SendReceive( EFavengRollback );
    }

// ---------------------------------------------------------
// RFavouritesDb::CleanupRollbackPushL
// ---------------------------------------------------------
//
EXPORT_C void RFavouritesDb::CleanupRollbackPushL()
    {
    CleanupStack::PushL( TCleanupItem( StaticRollback, this ) );
    }

// ---------------------------------------------------------
// RFavouritesDb::Get
// ---------------------------------------------------------
//
EXPORT_C TInt RFavouritesDb::Get( TInt aUid, CFavouritesItem& aItem )
    {
    TInt err;
    
    // Special items which are creted in memory dynamically, 
    // are filled up here at the client side.
    if( aUid <= KFavouritesStartPageUid )
    	{
    	switch( aUid )
    		{
    		case KFavouritesStartPageUid:
    			{
    			aItem.iImpl->SetUid( KFavouritesStartPageUid );
    			return KErrNone;
    			}
    		case KFavouritesAdaptiveItemsFolderUid:
    			{
    			aItem.iImpl->SetUid( KFavouritesAdaptiveItemsFolderUid );
    			aItem.SetType( CFavouritesItem::EFolder );
    			aItem.SetParentFolder( KFavouritesRootUid );
    			return KErrNone;	
    			}
    		case KFavouritesRSSItemUid:
    			{
    			aItem.iImpl->SetUid( KFavouritesRSSItemUid );
    			aItem.SetParentFolder( KFavouritesRootUid );
    			return KErrNone;	
    			}
    		case KFavouritesSavedDeckItemUid:
    			{
    			aItem.iImpl->SetUid( KFavouritesSavedDeckItemUid );
    			aItem.SetType( CFavouritesItem::EFolder );
    			aItem.SetParentFolder( KFavouritesRootUid );
    			return KErrNone;	
    			}
    		default:
    			{
    			break;
    			}
    		}
    	}
    	
    // Other UIDs are handled at the server side.
    RFavouritesBuf buf;
    TIpcArgs args( aUid );
    err = buf.Open( *this, EFavengGet, args );
    if( !err )
        {
        RReadStream stream( &buf );
        TRAP( err, aItem.InternalizeL( stream ) );
        buf.Close();
        }
    return err;
    }

// ---------------------------------------------------------
// RFavouritesDb::GetAll
// ---------------------------------------------------------
//
EXPORT_C TInt RFavouritesDb::GetAll
        (
        CFavouritesItemList& aItemList,
        TInt aParentFolderFilter /*=KFavouritesNullUid*/,
        CFavouritesItem::TType aTypeFilter /*=CFavouritesItem::ENone*/,
        const TDesC* aNameFilter /*=NULL*/,
        TInt32 aContextIdFilter /*=KFavouritesNullContextId*/
        )
    {
    RFavouritesBuf buf;
    TFavouritesFilter filter
        ( aParentFolderFilter, aTypeFilter, aNameFilter, aContextIdFilter );
    TPckg<TFavouritesFilter> filterPckg( filter );
    TIpcArgs args( &filterPckg );
    TInt err = buf.Open( *this, EFavengGetAll, args );
    if( !err )
        {
        RReadStream stream( &buf );
        TRAP( err, aItemList.InternalizeL( stream ) );
        buf.Close();
        }
    return err;
    }

// ---------------------------------------------------------
// RFavouritesDb::PreferredUid
// ---------------------------------------------------------
//
EXPORT_C TInt RFavouritesDb::PreferredUid( TInt aFolder, TInt& aPreferredUid )
{
    TPckg<TInt> prefUid( aPreferredUid );
    return SendReceive( EFavengPreferredUid, TIpcArgs( aFolder, &prefUid ) );
}

// ---------------------------------------------------------
// RFavouritesDb::GetUids
// ---------------------------------------------------------
//
EXPORT_C TInt RFavouritesDb::GetUids
        (
        CArrayFix<TInt>& aUids,
        TInt aParentFolderFilter /*=KFavouritesNullUid*/,
        CFavouritesItem::TType aTypeFilter /*=CFavouritesItem::ENone*/,
        const TDesC* aNameFilter /*=NULL*/,
        TInt32 aContextIdFilter /*=KFavouritesNullContextId*/
        )
    {
    RFavouritesBuf buf;
    TFavouritesFilter filter
        ( aParentFolderFilter, aTypeFilter, aNameFilter, aContextIdFilter );
    TPckg<TFavouritesFilter> filterPckg( filter );
    TIpcArgs args( &filterPckg );
    TInt err = buf.Open( *this, EFavengGetUids, args );
    if( !err )
        {
        RReadStream stream( &buf );
        TRAP( err, FavouritesUtil::InternalizeL( aUids, stream ) );
        buf.Close();
        }
    return err;
    }

// ---------------------------------------------------------
// RFavouritesDb::Delete
// ---------------------------------------------------------
//
EXPORT_C TInt RFavouritesDb::Delete( TInt aUid )
    {
    return SendReceive( EFavengDelete, TIpcArgs( aUid ) );
    }

// ---------------------------------------------------------
// RFavouritesDb::Update
// ---------------------------------------------------------
//
EXPORT_C TInt RFavouritesDb::Update
( CFavouritesItem& aItem, TInt aUid, TBool aAutoRename )
    {
    RFavouritesBuf buf;
    TInt err = buf.Open( iSess );
    if ( !err )
        {
        RWriteStream stream( &buf );
        TRAP( err, aItem.ExternalizeL( stream ) );
        if ( !err )
            {
            TRAP( err, stream.CommitL() );
            if ( !err )
                {
                err = SendReceive( EFavengUpdate, TIpcArgs
                    ( buf.Handle(), aUid, aAutoRename ) );
                if ( !err )
                    {
                    RReadStream readStream( &buf );
                    TRAP( err, aItem.iImpl->InternalizeUpdateDeltaL
                        ( readStream ) );
                    }
                }
            }
        buf.Close();
        }
    return err;
    }

// ---------------------------------------------------------
// RFavouritesDb::Add
// ---------------------------------------------------------
//
EXPORT_C TInt RFavouritesDb::Add( CFavouritesItem& aItem, TBool aAutoRename )
    {
    RFavouritesBuf buf;
    TInt err = buf.Open( iSess );
    if ( !err )
        {
        RWriteStream stream( &buf );
        TRAP( err, aItem.ExternalizeL( stream ) );
        if ( !err )
            {
            TRAP( err, stream.CommitL() );
            if ( !err )
                {
                err = SendReceive( EFavengAdd, TIpcArgs
                    ( buf.Handle(), aAutoRename ) );
                if ( !err )
                    {
                    RReadStream readStream( &buf );
                    TRAP( err, aItem.iImpl->InternalizeUpdateDeltaL
                        ( readStream ) );
                    }
                }
            }
        buf.Close();
        }
    return err;
    }

// ---------------------------------------------------------
// RFavouritesDb::SetHomepage
// ---------------------------------------------------------
//
EXPORT_C TInt RFavouritesDb::SetHomepage( CFavouritesItem& aItem )
    {
    return SetSpecialItem( aItem, KFavouritesHomepageUid );
    }

// ---------------------------------------------------------
// RFavouritesDb::SetLastVisited
// ---------------------------------------------------------
//
EXPORT_C TInt RFavouritesDb::SetLastVisited( CFavouritesItem& aItem )
    {
    return SetSpecialItem( aItem, KFavouritesLastVisitedUid );
    }

// ---------------------------------------------------------
// RFavouritesDb::SetFactoryItem
// ---------------------------------------------------------
//
EXPORT_C TInt RFavouritesDb::SetFactoryItem( TInt aUid, TBool aFactoryItem )
    {
    return SendReceive
        ( EFavengSetFactoryItem, TIpcArgs( aUid, aFactoryItem ) );
    }

// ---------------------------------------------------------
// RFavouritesDb::SetReadOnly
// ---------------------------------------------------------
//
EXPORT_C TInt RFavouritesDb::SetReadOnly( TInt aUid, TBool aReadOnly )
    {
    return SendReceive( EFavengSetReadOnly, TIpcArgs( aUid, aReadOnly ) );
    }

// ---------------------------------------------------------
// RFavouritesDb::SetModified
// ---------------------------------------------------------
//
EXPORT_C TInt RFavouritesDb::SetModified( TInt aUid, TTime aModified )
    {
    return SendReceive( EFavengSetModified, TIpcArgs
        ( aUid, I64HIGH( aModified.Int64() ), I64LOW( aModified.Int64() ) ) );
    }

// ---------------------------------------------------------
// RFavouritesDb::SetPreferredUid
// ---------------------------------------------------------
//
EXPORT_C TInt RFavouritesDb::SetPreferredUid( TInt aFolder, TInt aUid )
    {
    return SendReceive( EFavengSetPreferredUid, TIpcArgs( aFolder, aUid ) );
    }

// ---------------------------------------------------------
// RFavouritesDb::ItemExists
// ---------------------------------------------------------
//
EXPORT_C TInt RFavouritesDb::ItemExists( TInt aUid, TBool& aItemExists )
    {
    TInt ret = SendReceive( EFavengItemExists, TIpcArgs( aUid ) );
    if ( ret >= 0 )
        {
        aItemExists = ret;
        ret = KErrNone;
        }
    return ret;
    }

// ---------------------------------------------------------
// RFavouritesDb::FolderExists
// ---------------------------------------------------------
//
EXPORT_C TInt RFavouritesDb::FolderExists( TInt aFolder, TBool& aFolderExists )
    {
    TInt ret = SendReceive( EFavengFolderExists, TIpcArgs( aFolder ) );
    if ( ret >= 0 )
        {
        aFolderExists = ret;
        ret = KErrNone;
        }
    return ret;
    }

// ---------------------------------------------------------
// RFavouritesDb::Count
// ---------------------------------------------------------
//
EXPORT_C TInt RFavouritesDb::Count
        (
        TInt& aCount,
        TInt aParentFolderFilter /*=KFavouritesNullUid*/,
        CFavouritesItem::TType aTypeFilter /*=CFavouritesItem::ENone*/,
        const TDesC* aNameFilter /*=NULL*/,
        TInt32 aContextIdFilter /*=KFavouritesNullContextId*/
        )
    {
    TFavouritesFilter filter
        ( aParentFolderFilter, aTypeFilter, aNameFilter, aContextIdFilter );
    TPckg<TFavouritesFilter> filterPckg( filter );
    TInt ret = SendReceive( EFavengCount, TIpcArgs( &filterPckg ) );
    if ( ret >= 0 )
        {
        aCount = ret;
        ret = KErrNone;
        }
    return ret;
    }

// ---------------------------------------------------------
// RFavouritesDb::SetData
// ---------------------------------------------------------
//
EXPORT_C TInt RFavouritesDb::SetData
( TInt aUid, const MFavouritesItemData& aData )
    {
    return SetData( EFavengSetData, aUid, aData );
    }

// ---------------------------------------------------------
// RFavouritesDb::GetData
// ---------------------------------------------------------
//
EXPORT_C TInt RFavouritesDb::GetData( TInt aUid, MFavouritesItemData& aData )
    {
    return GetData( EFavengGetData, aUid, aData );
    }

// ---------------------------------------------------------
// RFavouritesDb::SetBrowserData
// ---------------------------------------------------------
//
EXPORT_C TInt RFavouritesDb::SetBrowserData
( TInt aUid, const MFavouritesItemData& aData )
    {
    return SetData( EFavengSetBrowserData, aUid, aData );
    }

// ---------------------------------------------------------
// RFavouritesDb::GetBrowserData
// ---------------------------------------------------------
//
EXPORT_C TInt RFavouritesDb::GetBrowserData
( TInt aUid, MFavouritesItemData& aData )
    {
    return GetData( EFavengGetBrowserData, aUid, aData );
    }

// ---------------------------------------------------------
// RFavouritesDb::MakeUniqueName
// ---------------------------------------------------------
//
EXPORT_C TInt RFavouritesDb::MakeUniqueName( TDes& aName, TInt aFolder )
    {
    __ASSERT_ALWAYS( aName.MaxLength() >= Min\
        ( aName.Length() + KFavouritesMaxPostfix, KFavouritesMaxName ), \
        FavouritesPanic( EFavouritesBufferTooSmall ) );
    TInt ret = SendReceive( EFavengMakeUniqueName,
        TIpcArgs( &aName, aName.Length(), aFolder ) );
    if ( ret >= 0 )
        {
        __ASSERT_DEBUG( aName.MaxLength() >= ret, \
            FavouritesPanic( EFavouritesInternal ) );
        aName.SetLength( ret );
        ret = KErrNone;
        }
    return ret;
    }

// ---------------------------------------------------------
// RFavouritesDb::MakeUniqueName
// ---------------------------------------------------------
//
EXPORT_C TInt RFavouritesDb::MakeUniqueName( CFavouritesItem& aItem )
    {
    TBuf<KFavouritesMaxName> name = aItem.Name();
    TInt err = MakeUniqueName( name, aItem.ParentFolder() );
    if ( !err )
        {
        TRAP( err, aItem.SetNameL( name ) );
        }
    return err;
    }

// ---------------------------------------------------------
// RFavouritesDb::CreateStartPageItemL
// ---------------------------------------------------------
//
EXPORT_C CFavouritesItem* RFavouritesDb::CreateStartPageItemL()
    {
    CFavouritesItem* item = CFavouritesItem::NewL();
    item->iImpl->SetUid( KFavouritesStartPageUid );
    return item;
    }

// ---------------------------------------------------------
// RFavouritesDb::CreateAdaptiveItemsFolder
// ---------------------------------------------------------
//
EXPORT_C CFavouritesItem* RFavouritesDb::CreateAdaptiveItemsFolderL()
    {
    CFavouritesItem* item = CFavouritesItem::NewL();
    item->iImpl->SetUid( KFavouritesAdaptiveItemsFolderUid );
    item->SetType( CFavouritesItem::EFolder );
    item->SetParentFolder( KFavouritesRootUid );
    return item;
    }

// ---------------------------------------------------------
// RFavouritesDb::DeleteFile
// ---------------------------------------------------------
//
EXPORT_C TInt RFavouritesDb::DeleteFile( TInt aUid )
    {
    return SendReceive( EFavengFileDelete, TIpcArgs( aUid ) );
    }



// ---------------------------------------------------------
// RFavouritesDb::RestoreFactorySettingsL
// ---------------------------------------------------------
//
EXPORT_C void RFavouritesDb::RestoreFactorySettingsL
( const TDesC& aName, const TDesC& aReferenceDbPath, MRfsApMapper& aApMapper )
    {
    FLOG(( _L("rstore:FavouritesDb.cpp RFavouritesDb::RestoreFactorySettingsL with param") ));
    
    RFavouritesSession sess;
    RFavouritesDb db;
    User::LeaveIfError( sess.Connect() );
    CleanupClosePushL<RFavouritesSession>( sess );
    User::LeaveIfError( db.Open( sess, aName ) );
    CleanupClosePushL<RFavouritesDb>( db );
    db.DoRestoreFactorySettingsL( aReferenceDbPath, aApMapper );
    CleanupStack::PopAndDestroy( 2 );   // Close db, close session
    }

// ---------------------------------------------------------
// RFavouritesDb::SetSpecialItem
// ---------------------------------------------------------
//
TInt RFavouritesDb::SetSpecialItem( CFavouritesItem& aItem, TInt aUid )
    {
    RFavouritesBuf buf;
    TInt err = buf.Open( iSess );
    if ( !err )
        {
        RWriteStream stream( &buf );
        TRAP( err, aItem.ExternalizeL( stream ) );
        if ( !err )
            {
            TRAP( err, stream.CommitL() );
            if ( !err )
                {
                err = SendReceive( EFavengSetSpecialItem, TIpcArgs
                    ( buf.Handle(), aUid ) );
                if ( !err )
                    {
                    RReadStream readStream( &buf );
                    TRAP( err, aItem.iImpl->InternalizeUpdateDeltaL
                        ( readStream ) );
                    }
                }
            }
        buf.Close();
        }
    return err;
    }

// ---------------------------------------------------------
// RFavouritesDb::SetData
// ---------------------------------------------------------
//
TInt RFavouritesDb::SetData
( TInt aFunction, TInt aUid, const MFavouritesItemData& aData )
    {
    RFavouritesBuf buf;
    TInt err = buf.Open( iSess );
    if ( !err )
        {
        RWriteStream stream( &buf );
        TRAP( err, aData.ExternalizeL( stream ) );
        if ( !err )
            {
            TRAP( err, stream.CommitL() );
            if ( !err )
                {
                err = SendReceive( aFunction, TIpcArgs
                    ( buf.Handle(), aUid ) );
                }
            }
        buf.Close();
        }
    return err;
    }

// ---------------------------------------------------------
// RFavouritesDb::GetData
// ---------------------------------------------------------
//
TInt RFavouritesDb::GetData
( TInt aFunction, TInt aUid, MFavouritesItemData& aData )
    {
    RFavouritesBuf buf;
    TIpcArgs args( aUid );
    TInt err = buf.Open( *this, aFunction, args );
    if( !err )
        {
        RReadStream stream( &buf );
        TRAP( err, aData.InternalizeL( stream ) );
        buf.Close();
        }
    return err;
    }

// ---------------------------------------------------------
// RFavouritesDb::DoRestoreFactorySettingsL
// ---------------------------------------------------------
//
void RFavouritesDb::DoRestoreFactorySettingsL
( const TDesC& aReferenceDbPath, MRfsApMapper& aApMapper )
    {
    // Start transaction.
    // As AP mapping functionality is an M class, it cannot be sent over to
    // the server. Because of this, RFS is split into two phases (RFS of
    // items and mapping AP-s). Therefore, the transaction has to be made here
    // on client side.
    User::LeaveIfError( Begin( /*aWrite=*/ ETrue ) );
    CleanupRollbackPushL();

    // Step 1: do the restore and collect AP-s to be mapped.
    RFavouritesBuf buf;
    TIpcArgs args( &aReferenceDbPath, aReferenceDbPath.Length() );
    User::LeaveIfError
        ( buf.Open( *this, EFavengRestoreFactorySettings, args ) );
    CleanupClosePushL<RFavouritesBuf>( buf );
    CUidMap* apMap = new (ELeave) CUidMap( KApMapGranularity );
    CleanupStack::PushL( apMap );
    RReadStream rs( &buf );
    apMap->InternalizeL( rs );

    // Step 2: map AP-s. (Omitted if there are no AP-s to map).
    if ( apMap->Count() )
        {
        // Do the mapping.
        TUint srcAp;
        TUint tgtAp;
        for ( TInt i = 0; i < apMap->Count(); i++ )
            {
            srcAp = STATIC_CAST( TUint, apMap->At( i ).iUid2 );
            if ( aApMapper.MapAccessPoint( srcAp, tgtAp ) )
                {
                apMap->At( i ).iUid2 = STATIC_CAST( TInt, tgtAp );
                }
            }

        // Send back mapped AP-s. Reusing existing buffer.
        RWriteStream ws( &buf );
        apMap->ExternalizeL( ws );
        ws.CommitL();
        User::LeaveIfError( SendReceive
            ( EFavengSetAccessPoints, TIpcArgs( buf.Handle() ) ) );
        }
    CleanupStack::PopAndDestroy( 2 );   // apMap, close buf.

    // Commit changes.
    User::LeaveIfError( Commit() );
    CleanupStack::Pop();    // Pop the rollback.
    }

// ---------------------------------------------------------
// RFavouritesDb::StaticRollback
// ---------------------------------------------------------
//
void RFavouritesDb::StaticRollback( TAny* aPtr )
    {
    STATIC_CAST( RFavouritesDb*, aPtr )->Rollback();
    }

//  End of File  
