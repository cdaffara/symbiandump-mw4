/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Providing open url functionality
*
*/


// INCLUDE FILES
#include "AiwBrowserProviderSaveUrl.h"

#include <AiwMenu.h>
//#include <AiwCommon.hrh>
#include <coemain.h>
#include <barsread.h>
#include <StringLoader.h>
#include <AiwBrowserProvider.rsg>
#include <data_caging_path_literals.hrh>  
#include <aknnotewrappers.h> 
#include <aknnotedialog.h>
#include <FavouritesDb.h>
#include <e32std.h>

#include "AiwBrowserCommands.hrh"
#include "logger.h"

// LOCAL CONSTANTS AND MACROS
_LIT( KResourceFileName, "AiwBrowserProvider.rsc");

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// NewL 
// Constructor of CAiwBrowserProviderSaveUrl
// Returns:
// 
// -----------------------------------------------------------------------------
//
CAiwBrowserProviderSaveUrl* CAiwBrowserProviderSaveUrl::NewL()
    {
	CAiwBrowserProviderSaveUrl* self = new(ELeave) CAiwBrowserProviderSaveUrl;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
    }

// -----------------------------------------------------------------------------
// CAiwBrowserProviderSaveUrl
// Constructor of CAiwBrowserProviderSaveUrl class
// Returns:
// 
// -----------------------------------------------------------------------------
//
CAiwBrowserProviderSaveUrl::CAiwBrowserProviderSaveUrl()
    {
    }

// -----------------------------------------------------------------------------
// ConstructL
// Constructor of CAiwBrowserProviderOpenUrl
// Returns:
// 
// -----------------------------------------------------------------------------
//
void CAiwBrowserProviderSaveUrl::ConstructL()
    {
    TFileName fName(  KNullDesC );
    iResourceOffset = -1;
    fName.Append( _L("Z:") );
    fName.Append( KDC_RESOURCE_FILES_DIR );
    fName.Append( KResourceFileName );
    iResourceOffset = CCoeEnv::Static()->AddResourceFileL( fName );
    LOG_WRITE("AIWBP: OpenUrl Resource file added.");    
    }

// -----------------------------------------------------------------------------
// ~CAiwBrowserProviderSaveUrl
// Destructor of CAiwBrowserProviderSaveUrl class
// Returns:
// 
// -----------------------------------------------------------------------------
//
CAiwBrowserProviderSaveUrl::~CAiwBrowserProviderSaveUrl()
    {
    if( iResourceOffset != -1 )
        {
    	CCoeEnv::Static()->DeleteResourceFile(iResourceOffset);
        iResourceOffset = -1;
        LOG_WRITE("AIWBP: Resource file deleted");
        }
    }


// -----------------------------------------------------------------------------
// InitialiseL 
// aiw initialization
// Returns: none
//                    
// -----------------------------------------------------------------------------
//
void CAiwBrowserProviderSaveUrl::InitialiseL(MAiwNotifyCallback& aFrameworkCallback, 
                                       const RCriteriaArray& aInterest)
    {
    iNotifyCallback = &aFrameworkCallback;
    iInterest = &aInterest;
    LOG_WRITE("AIWBP: Save Url Provider Initialised");       
    }

// -----------------------------------------------------------------------------
// HandleServiceCmdL 
// 
// Returns: 
//         
//         
// -----------------------------------------------------------------------------
//
void CAiwBrowserProviderSaveUrl::HandleServiceCmdL
        (const TInt& /*aCmdId*/, const CAiwGenericParamList& /*aInParamList*/,
         CAiwGenericParamList& /*aOutParamList*/, TUint /*aCmdOptions*/,
         const MAiwNotifyCallback* /*aCallback*/)
    {
    }

// -----------------------------------------------------------------------------
// InitializeMenuPaneL 
// initialization of menu pane 
// Returns: none
//          
// -----------------------------------------------------------------------------
//
//
void CAiwBrowserProviderSaveUrl::InitializeMenuPaneL
        (CAiwMenuPane& aMenuPane, TInt aIndex,
         TInt /*aCascadeId*/, const CAiwGenericParamList& /*aInParamList*/)
    {
    TResourceReader reader;    
    CCoeEnv::Static()->CreateResourceReaderLC
        (reader, R_AIW_BROWSER_PROVIDER_SAVE_URL_MENU);
    aMenuPane.AddMenuItemsL(reader, EBrowserAiwCmdSaveUrl, aIndex);//save url
    CleanupStack::PopAndDestroy(); // reader
    LOG_WRITE("AIWBP: Save Url Menu initialized");
    }

// -----------------------------------------------------------------------------
// HandleMenuCmdL
// handles menu commands
// Returns: none
//          
// -----------------------------------------------------------------------------
//

void CAiwBrowserProviderSaveUrl::HandleMenuCmdL
        (TInt /*aMenuCmdId*/, const CAiwGenericParamList& aInParamList,
         CAiwGenericParamList& /*aOutParamList*/, TUint /*aCmdOptions*/,
         const MAiwNotifyCallback* /*aCallback*/)
    {
    TInt index = 0;
    const TAiwGenericParam* aiwUrl = NULL;
    const TAiwGenericParam* aiwAccessPoint = NULL;
    const TAiwGenericParam* aiwPassword = NULL;
    const TAiwGenericParam* aiwUserName = NULL;
    const TAiwGenericParam* aiwBookmarkName = NULL;
    TUint32 accessPoint = 0;
    TPtrC pPassword;
    TPtrC pUserName;
    TPtrC pBookmarkName;
   
    TPtrC paramtext;
    aiwUrl = aInParamList.FindFirst( index, EGenericParamURL, EVariantTypeDesC);
    if( index == KErrNotFound )
        {
        return;
        }
    LOG_WRITE("AIWBP: aiwUrl set");
    index = 0;                    
    aiwAccessPoint = aInParamList.FindFirst( index, EGenericParamAccessPoint, EVariantTypeTInt32);
    if( index != KErrNotFound )
        {
        TInt32 aid;
        aid = aiwAccessPoint->Value().AsTInt32();
        if( aid > 0)
            {       
            accessPoint = (TUint32) aid;
            }
        }
    LOG_WRITE("AIWBP: accesspoint set");
    index = 0;            
    aiwPassword = aInParamList.FindFirst( index, EGenericParamPassword, EVariantTypeDesC);
    if( index != KErrNotFound )
        {
        pPassword.Set( aiwPassword->Value().AsDes() );
        }
    else
        {
        pPassword.Set( KNullDesC );
        }
    LOG_WRITE("AIWBP:  password set");  
    index = 0;            
    aiwUserName = aInParamList.FindFirst( index,EGenericParamUserName, EVariantTypeDesC);
    if( index != KErrNotFound )
        {
        pUserName.Set( aiwUserName->Value().AsDes() );
        }
    else
        {
        pUserName.Set( KNullDesC );
        }
    LOG_WRITE("AIWBP: username set");   
    index = 0;            
    aiwBookmarkName = aInParamList.FindFirst( index, EGenericParamBookmarkName, EVariantTypeDesC);
    if( index != KErrNotFound )
        {
        pBookmarkName.Set( aiwBookmarkName->Value().AsDes() );
        }
    else
        {
        pBookmarkName.Set( KNullDesC );
        }
    
    LOG_WRITE("AIWBP: bookmark set");
    SaveUrlL( aiwUrl->Value().AsDes(), 
             pBookmarkName, 
             pUserName, 
             pPassword,
             accessPoint );
    }

// -----------------------------------------------------------------------------
// SaveUrlL
// saves url
// Returns: none
//          
// -----------------------------------------------------------------------------
//

void CAiwBrowserProviderSaveUrl::SaveUrlL(
                const TDesC& aUrl, 
                const TDesC& aName,
                const TDesC& aUserName, 
                const TDesC& aPassword, 
                const TUint32 aApId )
{

    CFavouritesItem* item;
    TFavouritesWapAp ap;
    TInt err;
    
    RFavouritesSession sess;
    err = sess.Connect();
    if( err != KErrNone )
        {
        LOG_WRITE("AIWBP: Failed to open a favorites session.");
	    ErrorNoteL( 
					R_AIW_BROWSER_BOOKMARKS_TEXT_BM_NOT_SAVED );
        return;
        }
    CleanupClosePushL<RFavouritesSession>( sess );
    RFavouritesDb db;
    err = db.Open( sess, KBrowserBookmarks );
    if( err != KErrNone )
        {
        LOG_WRITE("AIWBP: Failed to open a favorites db.");
	    ErrorNoteL( 
					R_AIW_BROWSER_BOOKMARKS_TEXT_BM_NOT_SAVED );
        CleanupStack::PopAndDestroy( 1 );          
        return;
        }
    CleanupClosePushL<RFavouritesDb>( db );

    item = CFavouritesItem::NewLC();

    if( aUrl.Length() < 1 )
        {
        LOG_WRITE("AIWBP: Url is empty.");
	    ErrorNoteL( 
					R_AIW_BROWSER_BOOKMARKS_TEXT_BM_NOT_SAVED );
        return;
        }
    
    item->SetUrlL( aUrl.Left( KFavouritesMaxUrl ) );
    item->SetParentFolder( KFavouritesRootUid );
    
    if( aName.Length() < 1 )
        {
        item->SetNameL( aUrl.Left( KFavouritesMaxName ) );        
        LOG_WRITE("AIWBP: Name length is 0.");
        } 
    else
        {
        item->SetNameL( aName.Left( KFavouritesMaxName ) );
        }

    ap.SetApId( aApId );        
    item->SetWapAp( ap );
    LOG_WRITE("AIWBP: ap id is set.");
    if( aUserName.Length() > 0 )
        { 
        item->SetUserNameL( aUserName.Left( KFavouritesMaxUserName ));
        LOG_WRITE("AIWBP: UserName id is set.");
        }
    if( aPassword.Length() > 0 )
        {
        item->SetPasswordL( aPassword.Left( KFavouritesMaxPassword ) );
        LOG_WRITE("AIWBP: password is set.");
        }

    HBufC* prompt = StringLoader::LoadLC( R_AIW_WML_ENTER_BOOKMARK_NAME );
	TBuf<KFavouritesMaxName> retString;
	retString.Copy( item->Name() );
    LOG_WRITE("AIWBP: String loaded from resource.");

    TBool queryName = ETrue;
	while( queryName )
		{		
		err = DialogPromptReqL( 
			prompt->Des(), 
			&retString, 
            EFalse,
    		KFavouritesMaxName );
		if( err )
			{
			item->SetNameL(retString.Left(KFavouritesMaxName));
        // Add the item. The transaction is automatic.
            err = db.Add( *item, ETrue ) ;
			if ( !err )
				{
				InfoNoteL(
							R_BROWSER_OK_NOTE,
							R_AIW_BROWSER_BOOKMARKS_TEXT_BM_SAVED );
				queryName = EFalse;
                LOG_WRITE("AIWBP: saved to bookmarks.");
				}
			else if( KErrAlreadyExists == err )
				{
				InfoNoteL( 
						R_BROWSER_INFO_NOTE, R_AIW_WML_NAME_ALREADY_IN_USE );
                LOG_WRITE("AIWBP: allready exists");
                }
			else
				{
				ErrorNoteL( 
					R_AIW_BROWSER_BOOKMARKS_TEXT_BM_NOT_SAVED );
                LOG_WRITE("AIWBP: not saved error");
                }
			}
		else
            {
			queryName = EFalse;
            }
		}
	CleanupStack::PopAndDestroy( 4 );  // prompt, db, session, item
}

// -----------------------------------------------------------------------------
// ErrorNoteL 
// popups an error note
// Returns: none
//          
// -----------------------------------------------------------------------------
//

void CAiwBrowserProviderSaveUrl::ErrorNoteL( const TInt aPromptResourceId )
    {
    HBufC* prompt = StringLoader::LoadLC( aPromptResourceId );
    CAknErrorNote* note = new ( ELeave ) CAknErrorNote();
    note->ExecuteLD( *prompt );
    CleanupStack::PopAndDestroy();  // prompt
    }

// -----------------------------------------------------------------------------
// InfoNoteL 
// popups an info dialog
// Returns: none
//          
// -----------------------------------------------------------------------------
//

void CAiwBrowserProviderSaveUrl::InfoNoteL
( TInt aDialogResourceId, const TInt aPromptResourceId )
    {
    HBufC* prompt = StringLoader::LoadLC( aPromptResourceId );
    CAknNoteDialog* dialog = new (ELeave) CAknNoteDialog
								( REINTERPRET_CAST( CEikDialog**, &dialog ) );
    dialog->PrepareLC( aDialogResourceId ) ;
    dialog->SetCurrentLabelL( EGeneralNote, *prompt );
    dialog->RunDlgLD( CAknNoteDialog::ELongTimeout, CAknNoteDialog::ENoTone );
    CleanupStack::PopAndDestroy();  // prompt
    }

// -----------------------------------------------------------------------------
// DialogPromptReqL
// popups an dialog
// Returns: errorcode
//          
// -----------------------------------------------------------------------------
//

TInt CAiwBrowserProviderSaveUrl::DialogPromptReqL( const TDesC& aMsg, 
                                        TDes* aResp, 
                                        TBool aLowerCase,
                                        TInt aMaxLength
                                        )
	{
	TPtrC msg( aMsg );

	CAknTextQueryDialog* dialog = new( ELeave )CAknTextQueryDialog( *aResp, msg );
	if( aMaxLength )
	    {
		dialog->SetMaxLength(aMaxLength);
	    }

    TInt resource;
    if ( aLowerCase )
        {
        resource = R_WB_SCRIPT_PROMPT_LOWERCASE;
        }
    else
        {
        resource = R_WB_SCRIPT_PROMPT_LINES;
        }
	return dialog->ExecuteLD( resource );
	}





