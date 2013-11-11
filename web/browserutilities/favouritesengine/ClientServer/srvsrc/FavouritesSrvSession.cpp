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
*      Implementation of class CFavouritesSession
*      
*
*/


// INCLUDE FILES

#include "FavouritesSrvDb.h"
#include "FavouritesSrvDbNotifier.h"
#include "FavouritesSrvDbIncremental.h"
#include "FavouritesSrvStream.h"
#include "FavouritesSrvFile.h"
#include "FavouritesSrvBuf.h"
#include "FavouritesBuf.h"
#include "FavouritesFilter.h"
#include "FavouritesSrvSession.h"
#include "FavouritesSrvSessionEntry.h"
#include "FavouritesDef.h"
#include "FavouritesMsg.h"
#include "FavouritesItemImplList.h"
#include "FavouritesUtil.h"
#include "UidMap.h"
#include "FavouritesLogger.h"
#include <utf.h>
// CONSTANTS

/// Object list granularity.
LOCAL_D const TInt KFavouritesSrvSessionObjectGranularity = 4;
/// Uid list granularity.
LOCAL_D const TInt KFavouritesSrvSessionUidListGranularity = 4;
/// Granularity of AP map in RFS.
LOCAL_D const TInt KApMapGranularity = 4;

// Secure policy ID of the Faveng database files.
LOCAL_D const TUint KUidFavengDbPolicy = 0x101FD685;


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CFavouritesSession::NewL
// ---------------------------------------------------------
//
CFavouritesSession* CFavouritesSession::NewL()
    {
    CFavouritesSession* sess = new (ELeave) CFavouritesSession();
    CleanupStack::PushL( sess );
    sess->ConstructL();
    CleanupStack::Pop();
    return sess;
    }

// ---------------------------------------------------------
// CFavouritesSession::~CFavouritesSession
// ---------------------------------------------------------
//
CFavouritesSession::~CFavouritesSession()
    {
    // Free all entries in reverse order:
    // Files -> Streams -> Incrementals -> Notifiers -> Databases.
    for ( TInt type = TFavouritesSessionEntry::EFile;
          type > TFavouritesSessionEntry::ENone;
          type--
        )
        {
        for ( TInt i = 0; i < iNumEntries; i++ )
            {
            if ( STATIC_CAST( TInt, iEntries[i].Type() ) == type )
                {
                iEntries[i].Release();
                }
            }
        }
    delete iEntries;
    iEntries = NULL;    // Safety code.
    iNumEntries = 0;    // Safety code.
    
    // In the case of -Secure Backup is in progress- 
    // the ActiveScheduler is not active the server does not exists at this point.
    if(CActiveScheduler::Current() != NULL)
    	{
    	FLOG(( _L("Server().SessionClosed()") ));
    	Server().SessionClosed();
    	}
    else
    	{
    	FLOG(( _L("CActiveScheduler::Current() == NULL") ));
    	}
    
    iDbs.Close();
    iFs.Close();
    }

// ---------------------------------------------------------
// CFavouritesSession::CFavouritesSession
// ---------------------------------------------------------
//
CFavouritesSession::CFavouritesSession()
: iEntries( NULL ), iNumEntries( 0 ), iChkNum( 1 )
    {
    }

// ---------------------------------------------------------
// CFavouritesSession::ConstructL
// ---------------------------------------------------------
//
void CFavouritesSession::ConstructL()
    {
    User::LeaveIfError( iFs.Connect() );
    User::LeaveIfError( iFs.SetSessionToPrivate( EDriveC ) );
    User::LeaveIfError( iDbs.Connect() );

       }

// ---------------------------------------------------------
// CFavouritesSession::CountResources
// ---------------------------------------------------------
//
TInt CFavouritesSession::CountResources()
    {
    TInt cnt( 0 );
    for ( TInt i = 0; i < iNumEntries; i++ )
        {
        if ( iEntries[i].Type() != TFavouritesSessionEntry::ENone )
            {
            cnt++;
            }
        }
    return cnt;
    }

// ---------------------------------------------------------
// CFavouritesSession::ServiceL
// ---------------------------------------------------------
//
void CFavouritesSession::ServiceL( const RMessage2& aMessage )
    {
	// Set received message in server so it can be used in cleanup when
    // server panics.
	Server().ReceivedMessage( aMessage );
    TInt ret( KErrNone );
    TFavouritesMsg msg( aMessage.Function() );
    if ( msg.IsNull() )
        {
        // Null handle -> session function.
        switch ( msg.Function() )
            {
            // Session functions.
            case EFavengResourceMarkStart:
                {
                ResourceCountMarkStart();
                break;
                }

            case EFavengResourceMarkEnd:
                {
                ResourceCountMarkEnd( aMessage );
                break;
                }

            case EFavengResourceCount:
                {
                ret = CountResources();
                break;
                }

            case EFavengDeleteDatabase:
                {
                TBuf<KFavouritesMaxName> name;
                TInt len = aMessage.GetDesLength( 0 );
                if ( len < 1 || len > KFavouritesMaxName )
                    {
                    User::Leave( KErrBadName );
                    }
                name.SetLength( len );
                aMessage.ReadL( 0, name );
                DatabaseDirL( name, iParse );
                TRAPD( err, DeleteDirL( iParse ) );
                // Map KErrPathNotFound error to KErrNotFound.
                if ( err == KErrPathNotFound )
                    {
                    err = KErrNotFound;
                    }
                User::LeaveIfError( err );
                break;
                }

            case EFavengSetHeapFailure:
                {
                User::__DbgSetAllocFail
                    (
                    RAllocator::EUser,
                    STATIC_CAST( RAllocator::TAllocFail, aMessage.Int0() ),
                    aMessage.Int1()
                    );
                break;
                }

            case EFavengOpenDatabase:
                {
                
                // Fixed for Bug id - JJUN-78RARE (FAVENGSRV crashes under IPC attack)
                // When client sends 8 bit descriptor input and favourite server always 
                // will attempt  on 16 bit RMessage2 read/write. So server will leave with 
                // KErrBadDescriptor and hence crash occurs. To avoid this crash,favourite 
                // server will attempt to  read/write on 8bit descriptor.
                
                HBufC *buf = HBufC::NewLC(KFavouritesMaxName);
                TPtr name(buf->Des());
                TInt len = aMessage.GetDesLength( 0 );
                if ( len < 1 || len > KFavouritesMaxName )
                    {
                    User::Leave( KErrBadName );
                    }
                name.SetLength( len );
                TInt err =  aMessage.Read( 0, name );
                
                if ( KErrBadDescriptor == err)   // Mismatch between 8 bit and 16 bit descriptors
	            {
            	    HBufC8 *buf8 = HBufC8::NewLC(KFavouritesMaxName);
            	    TPtr8 name8(buf8->Des());
                	name8.SetLength( len );
                	aMessage.ReadL( 0, name8 );
                	CnvUtfConverter::ConvertToUnicodeFromUtf8(name,name8);
                	CleanupStack::PopAndDestroy(/*buf8*/);
                }
                else
                {
                	User::LeaveIfError(err);
                }
                
                DatabaseDirL( name, iParse );
                TFavouritesHandle h = NewHandleL();
                Entry( h ) = *(CFavouritesSrvDb::NewL
                    ( iFs, iDbs, iParse.FullName(), name ));
                ret = h;
                CleanupStack::PopAndDestroy(/*buf*/);
                break;
                }

            case EFavengOpenStream:
                {
                ret = NewStreamL( *(HFavouritesSrvBuf::NewL()), aMessage );
                break;
                }

            default:
                {
                ret = KErrNotSupported;
                break;
                }
            }
        }
    else
        {
        // Object based functions.
        TFavouritesSessionEntry& e = EntryL( msg );

        switch( msg.Function() )
            {
            // Favourites Engine functions.
            case EFavengClose:
                {
                e.Release();
                break;
                };

            case EFavengIsDamaged:
                {
                ret = e.Database().IsDamagedL();
                break;
                }

            case EFavengRecover:
                {
                e.Database().RecoverL();
                break;
                }

            case EFavengCompact:
                {
                e.Database().CompactL();
                break;
                }

            case EFavengSize:
                {
                TPckgBuf<RDbDatabase::TSize> size;
                size() = e.Database().Size();
                aMessage.WriteL( 0, size );
                break;
                }

            case EFavengUpdateStats:
                {
                e.Database().UpdateStatsL();
                break;
                }

            case EFavengBegin:
                {
                if ( e.Database().InTransaction() )
                    {
                    Server().PanicClient( EFavouritesNestedTransaction );
                    }
                else
                    {
                    e.Database().BeginL( (TBool)aMessage.Int0() );
                    }
                break;
                }

            case EFavengCommit:
                {
                if ( e.Database().InTransaction() )
                    {
                    e.Database().CommitL();
                    }
                else
                    {
                    Server().PanicClient( EFavouritesNoTransaction );
                    }
                break;
                }

            case EFavengRollback:
                {
                if ( e.Database().InTransaction() )
                    {
                    e.Database().Rollback();
                    }
                else
                    {
                    Server().PanicClient( EFavouritesNoTransaction );
                    }
                break;
                }

            case EFavengGet:
                {
                RWriteStream stream( HFavouritesSrvBuf::NewLC() );
                CFavouritesItemImpl* item = CFavouritesItemImpl::NewLC();
                e.Database().GetL( aMessage.Int0(), *item );
                item->ExternalizeL( stream );
                stream.CommitL();
                CleanupStack::PopAndDestroy( item );
                CleanupStack::Pop();    // Stream buf; will be taken by stream.
                ret = NewStreamL( *(stream.Sink()), aMessage );
                break;
                }

            case EFavengGetAll:
                {
                TPckgBuf<TFavouritesFilter> filter;
                aMessage.ReadL( 0, filter );
                RWriteStream stream( HFavouritesSrvBuf::NewLC() );
                CFavouritesItemImplList* list =
                    new (ELeave) CFavouritesItemImplList();
                CleanupStack::PushL( list );
                e.Database().GetAllL( *list, filter() );
                list->ExternalizeL( stream );
                stream.CommitL();
                CleanupStack::PopAndDestroy( list );
                CleanupStack::Pop();    // Stream buf; will be taken by stream.
                ret = NewStreamL( *(stream.Sink()), aMessage );
                break;
                }

            case EFavengGetUids:
                {
                TPckgBuf<TFavouritesFilter> filter;
                aMessage.ReadL( 0, filter );
                RWriteStream stream( HFavouritesSrvBuf::NewLC() );
                CArrayFixFlat<TInt>* list = new (ELeave) CArrayFixFlat<TInt>
                    ( KFavouritesSrvSessionUidListGranularity );
                CleanupStack::PushL( list );
                e.Database().GetUidsL( *list, filter() );
                FavouritesUtil::ExternalizeL( *list, stream );
                stream.CommitL();
                CleanupStack::PopAndDestroy( list );
                CleanupStack::Pop();    // Stream buf; will be taken by stream.
                ret = NewStreamL( *(stream.Sink()), aMessage );
                break;
                }

            case EFavengPreferredUid:
                {
                TPckgBuf<TInt> uid;
                uid() = e.Database().PreferredUidL( aMessage.Int0() );
                aMessage.WriteL( 1, uid );
                break;
                }

            case EFavengDelete:
                {
                e.Database().DeleteL( aMessage.Int0() );
                break;
                }

            case EFavengUpdate:
                {
                MStreamBuf& buf = EntryL( TFavouritesHandle
                    ( aMessage.Int0() ) ).Stream().Host();
                RReadStream rs( &buf );
                CFavouritesItemImpl* item = CFavouritesItemImpl::NewLC();
                item->InternalizeL( rs );
                e.Database().UpdateL
                    ( *item, aMessage.Int1(), aMessage.Int2() );
                RWriteStream ws( &buf );
                item->ExternalizeUpdateDeltaL( ws );
                ws.CommitL();
                CleanupStack::PopAndDestroy( item );
                break;
                }

            case EFavengAdd:
                {
                MStreamBuf& buf = EntryL( TFavouritesHandle
                    ( aMessage.Int0() ) ).Stream().Host();
                RReadStream rs( &buf );
                CFavouritesItemImpl* item = CFavouritesItemImpl::NewLC();
                item->InternalizeL( rs );
                e.Database().AddL( *item, aMessage.Int1() );
                RWriteStream ws( &buf );
                item->ExternalizeUpdateDeltaL( ws );
                ws.CommitL();
                CleanupStack::PopAndDestroy( item );
                break;
                }

            case EFavengSetSpecialItem:
                {
                MStreamBuf& buf = EntryL( TFavouritesHandle
                    ( aMessage.Int0() ) ).Stream().Host();
                RReadStream rs( &buf );
                CFavouritesItemImpl* item = CFavouritesItemImpl::NewLC();
                item->InternalizeL( rs );
                e.Database().SetSpecialItemL( *item, aMessage.Int1() );
                RWriteStream ws( &buf );
                item->ExternalizeUpdateDeltaL( ws );
                ws.CommitL();
                CleanupStack::PopAndDestroy( item );
                break;
                }

            case EFavengSetFactoryItem:
                {
                e.Database().SetFactoryItemL
                                        ( aMessage.Int0(), aMessage.Int1() );
                break;
                }

            case EFavengSetReadOnly:
                {
                e.Database().SetReadOnlyL
                                        ( aMessage.Int0(), aMessage.Int1() );
                break;
                }

            case EFavengSetModified:
                {
                e.Database().SetModifiedL( aMessage.Int0(),
                    TTime( MAKE_TINT64( aMessage.Int1(), aMessage.Int2() ) ) );
                break;
                }

            case EFavengSetPreferredUid:
                {
                e.Database().SetPreferredUidL
                                        ( aMessage.Int0(), aMessage.Int1() );
                break;
                }

            case EFavengItemExists:
                {
                ret = e.Database().ItemExistsL( aMessage.Int0() );
                break;
                }

            case EFavengFolderExists:
                {
                ret = e.Database().FolderExistsL( aMessage.Int0() );
                break;
                }

            case EFavengCount:
                {
                TPckgBuf<TFavouritesFilter> filter;
                aMessage.ReadL( 0, filter );
                ret = e.Database().CountL( filter() );
                break;
                }

            case EFavengSetData:
                {
                MStreamBuf& buf = EntryL( TFavouritesHandle
                    ( aMessage.Int0() ) ).Stream().Host();
                e.Database().SetDataL( aMessage.Int1(), buf );
                break;
                }

            case EFavengGetData:
                {
                RWriteStream stream( HFavouritesSrvBuf::NewLC() );
                e.Database().GetDataL
                    ( aMessage.Int0(), *(stream.Sink()) );
                stream.CommitL();
                CleanupStack::Pop();    // Stream buf; will be taken by stream.
                ret = NewStreamL( *(stream.Sink()), aMessage );
                break;
                }

            case EFavengSetBrowserData:
                {
                MStreamBuf& buf = EntryL( TFavouritesHandle
                    ( aMessage.Int0() ) ).Stream().Host();
                e.Database().SetBrowserDataL( aMessage.Int1(), buf );
                break;
                }

            case EFavengGetBrowserData:
                {
                RWriteStream stream( HFavouritesSrvBuf::NewLC() );
                e.Database().GetBrowserDataL
                    ( aMessage.Int0(), *(stream.Sink()) );
                stream.CommitL();
                CleanupStack::Pop();    // Stream buf; will be taken by stream.
                ret = NewStreamL( *(stream.Sink()), aMessage );
                break;
                }

            case EFavengMakeUniqueName:
                {
                TBuf<KFavouritesMaxName> name;
                name.SetLength( aMessage.Int1() );
                aMessage.ReadL( 0, name );
                e.Database().MakeUniqueNameL( name, aMessage.Int2() );
                aMessage.WriteL( 0, name );
                ret = name.Length();
                break;
                };


            case EFavengRestoreFactorySettings:
                {
                TFileName refDb;
                TInt len = aMessage.Int1();
                if ( len > refDb.MaxLength() )
                    {
                    User::Leave( KErrBadName );
                    }
                refDb.SetLength( len );
                aMessage.ReadL( 0, refDb );
                RWriteStream stream( HFavouritesSrvBuf::NewLC() );
                CUidMap* map = new (ELeave) CUidMap( KApMapGranularity );
                CleanupStack::PushL( map );
                e.Database().RestoreFactorySettingsL( refDb, *map );
                map->ExternalizeL( stream );
                stream.CommitL();
                CleanupStack::PopAndDestroy( map );
                CleanupStack::Pop();    // Stream buf; will be taken by stream.
                ret = NewStreamL( *(stream.Sink()), aMessage );
			    FLOG(( _L("restore: case EFavengRestoreFactorySettings") ));
                break;
                }

            case EFavengSetAccessPoints:
                {
                MStreamBuf& buf = EntryL( TFavouritesHandle
                    ( aMessage.Int0() ) ).Stream().Host();
                RReadStream rs( &buf );
                CUidMap* map = new (ELeave) CUidMap( KApMapGranularity );
                CleanupStack::PushL( map );
                map->InternalizeL( rs );
                e.Database().SetAccessPointsL( *map );
                CleanupStack::PopAndDestroy( map );
                break;
                }

			case EFavengFileOpen:
                {
                TFavouritesHandle h = NewHandleL();
                e.Database().FilenameL( aMessage.Int0(), iParse );
                CFavouritesSrvFile* file = CFavouritesSrvFile::NewL();
                Entry( h ) = *file;
                file->SetName(iParse);
            
                ret = h;
                break;
                }
                
            case EFavengFileOpenRead:
                {
                CFavouritesSrvFile& file = e.File();
                
                file.OpenL();
                file.TransferToClientL( aMessage );
                
                break;
                }

            case EFavengFileOpenWrite:
                {
                CFavouritesSrvFile& file = e.File();
                
                file.ReplaceL();
                file.TransferToClientL( aMessage );
                
                break;
                }

            case EFavengFileDelete:
                {
                TFavouritesHandle h = NewHandleL();
                e.Database().FilenameL( aMessage.Int0(), iParse );
                TInt err = iFs.Delete( iParse.FullName() );
                // Map KErrPathNotFound error to KErrNotFound.
                if ( err == KErrPathNotFound )
                    {
                    err = KErrNotFound;
                    }
                User::LeaveIfError( err );
                break;
                }

            case EFavengIncrementalRecover:
                {
                TFavouritesHandle h = NewHandleL();
                CFavouritesSrvDbIncremental* inc =
                    new (ELeave) CFavouritesSrvDbIncremental();
                Entry( h ) = *inc;
                TPckgBuf<TInt> step;
                inc->RecoverL( e.Database(), step() );
                aMessage.WriteL( 0, step );
                ret = h;
                break;
                }

            case EFavengIncrementalCompact:
                {
                TFavouritesHandle h = NewHandleL();
                CFavouritesSrvDbIncremental* inc =
                    new (ELeave) CFavouritesSrvDbIncremental();
                Entry( h ) = *inc;
                TPckgBuf<TInt> step;
                inc->CompactL( e.Database(), step() );
                aMessage.WriteL( 0, step );
                ret = h;
                break;
                }

            case EFavengIncrementalNext:
                {
                TInt step =  aMessage.Int0();
                if ( step == e.Incremental().Step() && step > 0 )
                    {
                    e.Incremental().NextL( step );
                    ret = step;
                    }
                else
                    {
                    Server().PanicClient( EFavouritesBadStep );
                    }
                break;
                }

            case EFavengOpenNotifier:
                {
                TFavouritesHandle h = NewHandleL();
                Entry( h ) = *(CFavouritesSrvDbNotifier::NewL( e.Database() ));
                ret = h;
                break;
                }

            case EFavengNotifyAllEvents:
                {
                if ( e.Notifier().IsActive() )
                    {
                    Server().PanicClient( EFavouritesAlreadyOpen );
                    }
                else
                    {
                    e.Notifier().NotifyAllEvents( aMessage );
                    return; // Notifier will complete the message.
                    }
                break;
                }

            case EFavengNotifyChange:
                {
                if ( e.Notifier().IsActive() )
                    {
                    Server().PanicClient( EFavouritesAlreadyOpen );
                    }
                else
                    {
                    e.Notifier().NotifyChange( aMessage );
                    return; // Notifier will complete the message.
                    }
                break;
                }

            case EFavengNotifyCancel:
                {
                e.Notifier().Cancel();
                break;
                }

            case EFavengStreamRead:
                {
                ret = e.Stream().ReadL( aMessage );
                break;
                }

            case EFavengStreamWrite:
                {
                e.Stream().WriteL( aMessage );
                break;
                }

            default:
                {
                ret = KErrNotSupported;
                break;
                }
            }

        }

	// Fixed for Bug id - JJUN-78RARE (FAVENGSRV crashes under IPC attack)    
	if ( (EFavengFileOpenRead == msg.Function()) && msg.IsNull() )
		{
		User::Leave(ret);	
		}
		
    if ( !Server().ReceivedMessage().IsNull() &&
    	// Special cases - file handler passed -
    	// IPC message is transfered automatically. 
    	msg.Function() != EFavengFileOpenRead &&
    	msg.Function() != EFavengFileOpenWrite)
        {
        aMessage.Complete( ret );
        }
    }

// ---------------------------------------------------------
// CFavouritesSession::ServiceError
// ---------------------------------------------------------
//
void CFavouritesSession::ServiceError( const RMessage2& aMessage, TInt aError )
    {
    // A bad descriptor / bad handle error implies a badly programmed client,
    // so panic it; otherwise use the default handling (report the error to
    // the client).
    if ( aError == KErrBadDescriptor || aError == KErrBadHandle )
        {
        Server().PanicClient( aError );
        }
    CSession2::ServiceError( aMessage, aError );
    }

// ---------------------------------------------------------
// CFavouritesSession::NewHandleL
// ---------------------------------------------------------
//
TFavouritesHandle CFavouritesSession::NewHandleL()
    {
    TInt i;
    for ( i = 0; i < iNumEntries; i++ )
        {
        if ( iEntries[i].Type() == TFavouritesSessionEntry::ENone )
            {
            // Found free slot; assign check number and return it.
            iEntries[i].ChkNum() = NextChkNum();
            return TFavouritesHandle( i, iEntries[i].ChkNum() );
            }
        }
    // No free slot; expand entry list.
    TInt newNum = iNumEntries + KFavouritesSrvSessionObjectGranularity;
    if ( newNum > (1 << KIndexBits) )
        {
        // Index range exhausted.
        User::Leave( KErrNoMemory );
        }
    iEntries = STATIC_CAST( TFavouritesSessionEntry*, User::ReAllocL\
        ( iEntries, newNum * sizeof( TFavouritesSessionEntry ) ) );
    // Initialize new entries.
    for ( i = iNumEntries; i < newNum; i++ )
        {
        iEntries[i].SetNull();
        }
    i = iNumEntries;    // Index of first newly allocated entry.
    iNumEntries = newNum;
    iEntries[i].ChkNum() = NextChkNum();
    return TFavouritesHandle( i, iEntries[i].ChkNum() );
    }

// ---------------------------------------------------------
// CFavouritesSession::EntryL
// ---------------------------------------------------------
//
TFavouritesSessionEntry& CFavouritesSession::EntryL
( TFavouritesHandle aHandle )
    {
    TInt i = aHandle.Index();
    if ( i > iNumEntries || aHandle.ChkNum() != iEntries[i].ChkNum() )
        {
        User::Leave( KErrBadHandle );
        }
    return iEntries[i];
    }

// ---------------------------------------------------------
// CFavouritesSession::Entry
// ---------------------------------------------------------
//
TFavouritesSessionEntry& CFavouritesSession::Entry
( TFavouritesHandle aHandle )
    {
    TInt i = aHandle.Index();
    if ( i > iNumEntries || aHandle.ChkNum() != iEntries[i].ChkNum() )
        {
        FavouritesPanic( EFavouritesInternal );
        }
    return iEntries[i];
    }

// ---------------------------------------------------------
// CFavouritesSession::NextChkNum
// ---------------------------------------------------------
//
TInt CFavouritesSession::NextChkNum()
    {
    if ( iChkNum > (1 << KChkNumBits - 1) )
        {
        iChkNum = 1;    // Start from 1 - we never get 0 handle.
        }
    return iChkNum++;
    }

// ---------------------------------------------------------
// CFavouritesSession::NewStreamL
// ---------------------------------------------------------
//
TFavouritesHandle CFavouritesSession::NewStreamL
( MStreamBuf& aHost, const RMessage2& aMessage )
    {
    aHost.PushL();
    TInt len = Min( aHost.SizeL(), KFavouritesStreamBufSize );
    if ( len )
        {
        // We already have some data; send it.
        TPckgBuf<TFavouritesBuf> buf;
        aHost.ReadL( buf().iData, len );
        buf().iLen = len;
        aMessage.WriteL( 3, buf );
        }
    // Make a handle.
    TFavouritesHandle h = NewHandleL();
    TFavouritesSessionEntry& e = Entry( h );
    // Create the stream, owner is the entry (== the session).
    // Pass ownership to session.
    e = *(new (ELeave) HFavouritesSrvStream( aHost ));
    CleanupStack::Pop();    // aHost
    return h;
    }


// ---------------------------------------------------------
// CFavouritesSession::DatabaseDirL
// ---------------------------------------------------------
//
void CFavouritesSession::DatabaseDirL( const TDesC& aName, TParse& aParse )
    {
    TDriveName cDrive = TDriveUnit( EDriveC ).Name();
    
    _LIT(KDbExtension, ".db");
    
    User::LeaveIfError( aParse.SetNoWild( aName, &cDrive, &KDbExtension() ) );
    }

    
// ---------------------------------------------------------
// CFavouritesSession::DeleteDirL
// ---------------------------------------------------------
//
void CFavouritesSession::DeleteDirL( const TParse& aParse )
    {
    const TUid KDbPolicyUid = TUid::Uid(KUidFavengDbPolicy);

    TPtrC path( aParse.FullName() );
    TInt err = iDbs.DeleteDatabase( path, KDbPolicyUid );
    
    if ( err != KErrNone )
        {
        User::Leave( err );
        }
    }

//  End of File
