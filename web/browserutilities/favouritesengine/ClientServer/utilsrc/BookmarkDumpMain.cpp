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
*      Entry point and main function for Bookmark Dump utility application.
*      
*
*/


// INCLUDE FILES

#include <bacline.h>
#include <FavouritesDb.h>
#include <FavouritesLimits.h>
#include <FavouritesItemList.h>
#include "UnicodeFile.h"

// CONSTANTS

/// Formatting buffer size. Longest data is URL (plus format stuff).
LOCAL_D const TInt KFormatBufLen = KFavouritesMaxUrl + 20;

// ================= LOCAL FUNCTIONS =======================

/**
* Main function. Process command line arguments and import bookmark data.
*/
LOCAL_D void MainL()
    {
    TBuf<KFormatBufLen> buf;
    TInt num;
    CCommandLineArguments* args = CCommandLineArguments::NewLC();
    if( args->Count() != 2 )
        {
        // Expecting command line 'Appname outfile'.
        User::Leave( KErrArgument );
        }
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL<RFs>( fs );
    RUnicodeFile file;
    User::LeaveIfError( file.ReplaceL
        ( fs, args->Arg( 1 ), EFileWrite | EFileShareExclusive ) );
    CleanupClosePushL<RUnicodeFile>( file );
    RFavouritesSession sess;
    User::LeaveIfError( sess.Connect() );
    CleanupClosePushL<RFavouritesSession>( sess );
    RFavouritesDb db;
    User::LeaveIfError( db.Open( sess, KBrowserBookmarks ) );
    CleanupClosePushL<RFavouritesDb>( db );
    User::LeaveIfError( db.Begin( /*aWrite=*/EFalse ) );
    db.CleanupRollbackPushL();

    CFavouritesItemList* items = new (ELeave) CFavouritesItemList();
    CleanupStack::PushL( items );
    const CFavouritesItem* item;
    User::LeaveIfError( db.GetAll( *items ) );
    buf.Format( _L("%d entries\r\n"), items->Count() );
    User::LeaveIfError( file.Write( buf ) );
    for ( TInt i = 0; i < items->Count(); i++ )
        {
        User::LeaveIfError( file.Write( _L("\r\n") ) );
        item = items->At( i );
        // Uid.
        num = item->Uid();
        buf.Format( _L("Uid=(%d)"), num );
        User::LeaveIfError( file.Write( buf ) );
        // Fixed Uid info.
        if ( num == KFavouritesRootUid )
            {
            User::LeaveIfError( file.Write( _L(" (root folder)\r\n") ) );
            }
        else if ( num == KFavouritesHomepageUid )
            {
            User::LeaveIfError( file.Write( _L(" (homepage)\r\n") ) );
            }
        else if ( num == KFavouritesLastVisitedUid )
            {
            User::LeaveIfError( file.Write( _L(" (last visited)\r\n") ) );
            }
        else
            {
            User::LeaveIfError( file.Write( _L("\r\n") ) );
            }
        // Type.
        if ( item->Type() == CFavouritesItem::EFolder )
            {
            User::LeaveIfError( file.Write( _L("Type=Folder\r\n") ) );
            }
        else if ( item->Type() == CFavouritesItem::EItem )
            {
            User::LeaveIfError( file.Write( _L("Type=Item\r\n") ) );
            }
        else
            {
            User::LeaveIfError( file.Write( _L("Type=HUH???\r\n") ) );
            }
        TPtrC ptr;
        // Name
        ptr.Set( item->Name() );
        buf.Format( _L("Name=<%S>\r\n"), &ptr );
        User::LeaveIfError( file.Write( buf ) );
        // Parent.
        buf.Format( _L("ParentFolder=(%d)\r\n"), item->ParentFolder() );
        User::LeaveIfError( file.Write( buf ) );
        // Url.
        ptr.Set( item->Url() );
        buf.Format( _L("Url=<%S>\r\n"), &ptr );
        User::LeaveIfError( file.Write( buf ) );
        // Access Point.
        if ( item->WapAp().IsNull() )
            {
            User::LeaveIfError( file.Write( _L("WapAp=Null\r\n") ) );
            }
        else if ( item->WapAp().IsDefault() )
            {
            User::LeaveIfError( file.Write( _L("WapAp=Default\r\n") ) );
            }
        else
            {
            buf.Format( _L("WapAp=(%d)\r\n"), item->WapAp().ApId() );
            User::LeaveIfError( file.Write( buf ) );
            }
        // UserName.
        ptr.Set( item->UserName() );
        buf.Format( _L("UserName=<%S>\r\n"), &ptr );
        User::LeaveIfError( file.Write( buf ) );
        // Password.
        ptr.Set( item->Password() );
        buf.Format( _L("Password=<%S>\r\n"), &ptr );
        User::LeaveIfError( file.Write( buf ) );
        // Read-only.
        buf.Format( _L("ReadOnly=(%d)\r\n"), item->IsReadOnly() );
        User::LeaveIfError( file.Write( buf ) );
        // Factory item.
        buf.Format( _L("FactoryItem=(%d)\r\n"), item->IsFactoryItem() );
        User::LeaveIfError( file.Write( buf ) );
        // ContextId.
        buf.Format( _L("ContextId=(0x%x)\r\n"), item->ContextId() );
        User::LeaveIfError( file.Write( buf ) );
        // Modified.
        item->Modified().FormatL
            ( buf, _L("Modified=<%F%Y/%M/%D %H:%T:%S>\r\n") );
        User::LeaveIfError( file.Write( buf ) );
        // Preferred uid.
        if ( item->Type() == CFavouritesItem::EFolder )
            {
            User::LeaveIfError( db.PreferredUid( item->Uid(), num ) );
            buf.Format( _L("PreferredUid=(%d)\r\n"), num );
            User::LeaveIfError( file.Write( buf ) );
            }
        }
    CleanupStack::PopAndDestroy();      // items

    User::LeaveIfError( db.Commit() );
    CleanupStack::Pop();                // Pop the rollback

    CleanupStack::PopAndDestroy( 5 );   // sess, db, file, fs, args
    }

/**
* Entry point to the application.
* @return Error code.
*/
GLDEF_C TInt E32Main()
    {
    TInt err = KErrNone;
    CTrapCleanup* trapCleanup = CTrapCleanup::New();
    if ( trapCleanup )
        {
        TRAP( err, MainL() );
        delete trapCleanup;
        }
    else
        {
        err = KErrNoMemory;
        }
    return err;
    }
