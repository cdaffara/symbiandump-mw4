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
*      Implementation of CBookmarkFileImporter.
*      
*
*/


// INCLUDE FILES

#include "BookmarkFileImporter.h"
#include <FavouritesLimits.h>
// TEMP
#include <s32file.h>
// END TEMP

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
/// "Homepage" kewyword.
_LIT( KHomepage, "Homepage" );
/// "LastVisited" kewyword.
_LIT( KLastVisited, "LastVisited" );

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CBookmarkFileImporter::NewLC
// ---------------------------------------------------------
//
CBookmarkFileImporter* CBookmarkFileImporter::NewL()
    {
    CBookmarkFileImporter* importer = new (ELeave) CBookmarkFileImporter();
    CleanupStack::PushL( importer );
    importer->ConstructL();
    CleanupStack::Pop();    // importer
    return importer;
    }

// ---------------------------------------------------------
// CBookmarkFileImporter::~CBookmarkFileImporter
// ---------------------------------------------------------
//
CBookmarkFileImporter::~CBookmarkFileImporter()
    {
    delete iBuf;
    iFile.Close();
    iFs.Close();
    delete iItem;
    iDb.Close();
    iSess.Close();
    }

// ---------------------------------------------------------
// CBookmarkFileImporter::ImportL
// ---------------------------------------------------------
//
void CBookmarkFileImporter::ImportL( const TDesC& aFileName )
    {
    User::LeaveIfError( iFile.Open( iFs, aFileName,
        EFileRead | EFileShareReadersOnly ) );
    CleanupClosePushL<RUnicodeFile>( iFile );
    User::LeaveIfError( iDb.Open( iSess, KBrowserBookmarks ) );
    // Import is done in a transaction - all or nothing.
    User::LeaveIfError( iDb.Begin( /*aWrite=*/ETrue ) );
    iDb.CleanupRollbackPushL();
    GetCharL();
    while( NextLineL() );
    User::LeaveIfError( iDb.Commit() );
    CleanupStack::Pop();            // the rollback
    CleanupStack::PopAndDestroy();  // close iFile
    }

// ---------------------------------------------------------
// CBookmarkFileImporter::CBookmarkFileImporter
// ---------------------------------------------------------
//
CBookmarkFileImporter::CBookmarkFileImporter()
    {
    }

// ---------------------------------------------------------
// CBookmarkFileImporter::ConstructL
// ---------------------------------------------------------
//
void CBookmarkFileImporter::ConstructL()
    {
    User::LeaveIfError( iSess.Connect() );
    iItem = CFavouritesItem::NewL();
    User::LeaveIfError( iFs.Connect() );
    iBuf = new (ELeave) TText16[KFavouritesMaxUrl];
    iMaxCh = iBuf + KFavouritesMaxUrl;
    }

// ---------------------------------------------------------
// CBookmarkFileImporter::GetCharL
// ---------------------------------------------------------
//
void CBookmarkFileImporter::GetCharL()
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
// CBookmarkFileImporter::NextLineL
// ---------------------------------------------------------
//
TBool CBookmarkFileImporter::NextLineL()
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
// CBookmarkFileImporter::NextTokenL
// ---------------------------------------------------------
//
TPtrC CBookmarkFileImporter::NextTokenL( const TDesC& aStopCharSet )
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
// CBookmarkFileImporter::NextIntTokenL
// ---------------------------------------------------------
//
TInt CBookmarkFileImporter::NextIntTokenL( const TDesC& aStopCharSet )
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
// CBookmarkFileImporter::NextHexTokenL
// ---------------------------------------------------------
//
TInt32 CBookmarkFileImporter::NextHexTokenL( const TDesC& aStopCharSet )
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
// CBookmarkFileImporter::SkipL
// ---------------------------------------------------------
//
void CBookmarkFileImporter::SkipL( const TDesC& aStopCharSet )
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
// CBookmarkFileImporter::AttrsL
// ---------------------------------------------------------
//
void CBookmarkFileImporter::AttrsL()
    {
    TPtrC token;
    TInt num;
    TBool readOnly( EFalse );
    TBool factoryItem( ETrue );
    TBool homePage( EFalse );
    TBool lastVisited( EFalse );
    TBool preferred( EFalse );

    // Parse the line and fill item.
    iItem->ClearL();

    // Type (including special items).
    token.Set( NextTokenL( KStopComma ) );
    if ( !token.Compare( KFolder ) )
        {
        iItem->SetType( CFavouritesItem::EFolder );
        }
    else if ( !token.Compare( KItem ) )
        {
        iItem->SetType( CFavouritesItem::EItem );
        }
    else if ( !token.Compare( KHomepage ) )
        {
        iItem->SetType( CFavouritesItem::EItem );
        homePage = ETrue;
        }
    else if ( !token.Compare( KLastVisited ) )
        {
        iItem->SetType( CFavouritesItem::EItem );
        lastVisited = ETrue;
        }
    else
        {
        // Expected "Folder", "Item", "Homepage" or "LastVisited".
        User::Leave( KErrCorrupt );
        }
    GetCharL();

    // Name.
    iItem->SetNameL( NextTokenL( KStopComma ) );
    GetCharL();

    // Parent folder.
    iItem->SetParentFolder( FolderByNameL( NextTokenL( KStopComma ) ) );
    GetCharL();

    // URL.
    iItem->SetUrlL( NextTokenL( KStopComma ) );
    GetCharL();

    // WAP AP.
    num = NextIntTokenL( KStopComma );
    if( num )
        {
        TFavouritesWapAp ap;
        ap = num;
        iItem->SetWapAp( ap );
        }
    GetCharL();

    // User name.
    iItem->SetUserNameL( NextTokenL( KStopComma ) );
    GetCharL();

    // Password.
    iItem->SetPasswordL( NextTokenL( KStopComma ) );
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
        factoryItem = EFalse;
        }
    else if ( num == 1 )
        {
        factoryItem = ETrue;
        }
    else
        {
        // Expected "0" or "1".
        User::Leave( KErrCorrupt );
        }
    GetCharL();

    // Context id.
    iItem->SetContextId( NextHexTokenL( KStopCommaLfEof ) );
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

    // Add item and set flags.
    if ( homePage )
        {
        User::LeaveIfError( iDb.SetHomepage( *iItem ) );
        }
    else if ( lastVisited )
        {
        User::LeaveIfError( iDb.SetLastVisited( *iItem ) );
        }
    else
        {
        User::LeaveIfError( iDb.Add( *iItem, /*aAutoRename=*/EFalse ) );
        }
    User::LeaveIfError( iDb.SetReadOnly( iItem->Uid(), readOnly ) );
    User::LeaveIfError( iDb.SetFactoryItem( iItem->Uid(), factoryItem ) );
    if ( preferred )
        {
        User::LeaveIfError( iDb.SetPreferredUid
            ( iItem->ParentFolder(), iItem->Uid() ) );
        }
    }

// ---------------------------------------------------------
// CBookmarkFileImporter::FolderByNameL
// ---------------------------------------------------------
//
TInt CBookmarkFileImporter::FolderByNameL( const TDesC& aName )
    {
    TInt folder = KFavouritesRootUid;
    if ( aName.Length() )
        {
        CArrayFix<TInt>* uids = new (ELeave) CArrayFixFlat<TInt>( 1 );
        CleanupStack::PushL( uids );
        User::LeaveIfError( iDb.GetUids
            (
            *uids,
            KFavouritesNullUid,
            CFavouritesItem::EFolder,
            &aName,
            KFavouritesNullContextId
            ) );
        if( uids->Count() == 0 )
            {
            User::Leave( KErrNotFound );
            }
        if( uids->Count() == 1 )
            {
            folder = uids->At( 0 );
            }
        else
            {
            // This would worth a panic - more folders with the same name?
            User::Leave( KErrCorrupt );
            }
        CleanupStack::PopAndDestroy();  // uids
        }
    return folder;
    }
