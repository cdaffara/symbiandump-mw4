/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of the CBrowserDialogsProvider class.
*
*/


// INCLUDE Files

// User includes
#include "BrowserDialogsProvider.h"	// Class header
#include "BrowserDialogsProvider.hrh"
#include "BrowserAuthenticationDialog.h"
#include "BrowserDialogsProviderConstants.h"
#include "BrowserDialogsProviderObserver.h" //obs

// Browser as a Plugin - own classes
#include "BrowserViewImagesPopup.h"		// For DialogDisplayPageImagesL
#include "BrowserScriptPromptDialog.h"	// For DialogPromptLC
#include "BrowserSelectElementDlg.h"	// For DialogFileSelectLC
#include "BrowserUploadProgressNote.h"  // For UploadProgressNoteL

// System Includes
#include <aknlists.h>					// for Object Element Dialog
#include <aknmessagequerydialog.h>		// DialogAlertL
#include <CAknFileSelectionDialog.h>	// File Selection Dialog

#ifdef RD_MULTIPLE_DRIVE
#include <driveinfo.h>
#include <CAknMemorySelectionDialogMultiDrive.h>
#include <AknCommonDialogsDynMem.h>
#else
#include <CAknMemorySelectionDialog.h>
#include <AknCommonDialogs.h>			// File Selection Dialog
#endif

#include <aknnotewrappers.h>			
#include <AknInfoPopupNoteController.h> // tooltip
#include <pathconfiguration.hrh>
#include <featmgr.h>
#include <MGFetch.h> // Media Fetch
// General
#include <StringLoader.h>				// strings
#include <f32file.h>
#include <thttpfields.h>
#include <textresolver.h>

// Resources
#include <ConeResLoader.h>
#include <BrowserDialogsProvider.rsg>

// Data Caging
#include <data_caging_path_literals.hrh>    

// CONSTANTS
const TInt KBrCtlObjectElementMaxLength = 50;
const TInt KBrCtlMBFormat = 4;
const TInt KBrCtlGBFormat = 10;
const TInt KBrCtlMegabyte = 1000;	// although 1MB=1024 kB, treat as 1000kb for user simplicity

// DLL resource file name with path
_LIT( KBrowserDialogsProviderDirAndFile, "z:BrowserDialogsProvider.rsc" );// resource

//Mime Types
_LIT( KMimeTypeImage, "image/*" );
_LIT( KMimeTypeAudio, "audio/*" );
_LIT( KMimeTypeVideo, "video/*" );

// MACROS
#define JAPANESE ( FeatureManager::FeatureSupported( KFeatureIdJapanese ) )


//-----------------------------------------------------------------------------
/*
*/
//-----------------------------------------------------------------------------
CBrowserDialogsProvider::CBrowserDialogsProvider( 
								MBrowserDialogsProviderObserver* aObserver )
:   iCoeEnv( *CCoeEnv::Static() ), 
    iObserver( aObserver ),
    iResourceLoader( iCoeEnv ),    
    iResourceOpened( EFalse )
	{
	}

//-----------------------------------------------------------------------------
//	CBrowserDialogsProvider* CBrowserDialogsProvider::NewL
//
//-----------------------------------------------------------------------------
EXPORT_C CBrowserDialogsProvider* CBrowserDialogsProvider::NewL(
								MBrowserDialogsProviderObserver* aObserver )
	{
	CBrowserDialogsProvider* self = 
							new (ELeave) CBrowserDialogsProvider( aObserver );
	CleanupStack::PushL( self );
	self->ConstructL( );
	CleanupStack::Pop( self );   // self
	return self;
	}

//-----------------------------------------------------------------------------
//  CBrowserDialogsProvider::ConstructL()
//
//-----------------------------------------------------------------------------
void CBrowserDialogsProvider::ConstructL()
	{
	// Load resource file
	AssureResourceL();
	
	// Tooltip
	iTooltip = CAknInfoPopupNoteController::NewL();
	}

//-----------------------------------------------------------------------------
//  CBrowserDialogsProvider::~CBrowserDialogsProvider()
//
//-----------------------------------------------------------------------------
CBrowserDialogsProvider::~CBrowserDialogsProvider()
	{
	iResourceLoader.Close();

	// Delete any dialogs
	CancelAll();
	
	// Clean up tooltip
	if ( iTooltip )
	    {
	    iTooltip->HideInfoPopupNote();
	    }
	delete iTooltip;
	}

//-----------------------------------------------------------------------------
//  CBrowserDialogsProvider::DialogNotifyErrorL( TInt aErrCode )
//
//-----------------------------------------------------------------------------
EXPORT_C void CBrowserDialogsProvider::DialogNotifyErrorL( TInt aErrCode )
	{
    TInt httpErr = KBrCtlHttpErrorsOffset - aErrCode;
	CTextResolver* textresolver = CTextResolver::NewLC(); 
	TPtrC msg;
		
    switch ( httpErr )
        {
        case EHttpMultipleChoices: 
        case EHttpMovedPermanently:
        case EHttpMovedTemporarily:
        case EHttpTemporaryRedirect:
            {
            // Id for r_nw_stat_too_many_redirects is -20019, browser errors start at -20000 
            msg.Set( textresolver->ResolveErrorString( KErrTooManyRedirects )); 
            DialogNoteL( msg );
            break;
            }
        default:
            {
            // Handle all others as system error dialog
            CCoeEnv::Static()->HandleError( aErrCode );
        	break;
            }
        }   // end of switch
	
   CleanupStack::PopAndDestroy(); //textresolver
       
	if ( iObserver )
    	{
    	iObserver->ReportDialogEventL( 
    	                            MBrowserDialogsProviderObserver::ENotifyError,
    	                            aErrCode );
        }
	}


//-----------------------------------------------------------------------------
//  CBrowserDialogsProvider::DialogNotifyHttpErrorL(
//								TInt aErrCode, const TDesC& /*aUri*/ )
//-----------------------------------------------------------------------------
EXPORT_C void CBrowserDialogsProvider::DialogNotifyHttpErrorL(
								TInt aErrCode, const TDesC& /*aUri*/ )
	{
    TInt httpErr = KBrCtlHttpErrorsOffset - aErrCode;
    CTextResolver* textresolver = CTextResolver::NewLC(); 
	TPtrC msg;	

    switch ( httpErr )
        {
        case EHttpMultipleChoices: 
        case EHttpMovedPermanently:
        case EHttpMovedTemporarily:
        case EHttpTemporaryRedirect:
            {
            // Handle the redirect error dialog browser errors start from -20000 ,id for r_nw_stat_malformed_url is -20019.
			msg.Set( textresolver->ResolveErrorString( KErrTooManyRedirects ));  
			DialogNoteL( msg );
            break;     
            }
        case EHttpBadGateway:
	        {
    	    // Handle the unresolved dns name error dialog
    	    HBufC* msg = NULL;
        	msg = StringLoader::LoadLC( R_BROWSER_NOT_VALID_DNS_NAME);
			DialogNoteL( *msg );			
			CleanupStack::PopAndDestroy();	// msg
			break;
        	}
        default:
            {
            // Handle all others as system error dialog
            msg.Set( textresolver->ResolveErrorString( aErrCode ));     
			DialogNoteL( msg );
            break;
            }
        }   // end of switch			 
    CleanupStack::PopAndDestroy(); //textresolver    
	
    if ( iObserver )
        {
        iObserver->ReportDialogEventL( 
                                MBrowserDialogsProviderObserver::ENotifyHttpError,
                                KBrCtlHttpErrorsOffset - aErrCode );
        }
	}

//CBrowserDialogsProvider::DialogFileSelectLC( 
//                                              const TDesC& aStartPath,
//												const TDesC& aRootPath,
//												HBufC*& aSelectedFileName )
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

EXPORT_C TBool CBrowserDialogsProvider::DialogFileSelectLC( 
                                                    const TDesC& aStartPath,
													const TDesC&/*aRootPath*/,
													HBufC*& aSelectedFileName )
	{
	// Launch the AVKON dialog that will use existing path.
	TBool selected( EFalse );

    HBufC* buf = NULL;
    buf = HBufC::NewLC( KMaxFileName );
	TPtr ptr ( buf->Des() );
	
	// If a recent path exists, use that as the starting point.
	if ( aStartPath.Length() )
		{

		TParsePtrC fileNameParser( aStartPath );
         // Copy drive into the buffer as a way to pass it to AVKON dialog.
        TPtrC ptrDrive( fileNameParser.Drive() );
        // Get phone memory root 
      	TPtrC8 tempPtrC8((const TUint8*)text_phone_memory_root_path);

		// Unicode data will be <= in length to ASCII
		HBufC* rootBuf = HBufC::NewLC(tempPtrC8.Length() + 1);
		
		rootBuf->Des().Copy(tempPtrC8);
		rootBuf->Des().ZeroTerminate();
		
		TPtr rootPtr( rootBuf->Des() );
		// Compare phone memory drive with drive from aStartPath
        if( rootPtr.Length() && (rootPtr[0] == ptrDrive[0] ))
			{
			// we are on the phone memory
    		TPtrC ptrPath( fileNameParser.DriveAndPath() );  
       		ptr.Copy(rootPtr); 
        	if ( ptrPath.Length() )
        
				{// make ptrPath directory relative to a phone root directory
				// remove "C:\\Data\\" from ptrPath
      			if(aStartPath.Match(rootBuf->Des() )) 
					{
					ptrPath.Set( ptrPath.Right( ptrPath.Length() - rootPtr.Length() ) );
					}
				} // for current configuration ptr is "C:\\Data\\", ptrPath is something like
            //  "images//" etc
        	selected = CAknFileSelectionDialog::RunDlgLD(ptr,ptrPath);   	
			}
		else
			{
			TBuf<KMaxPath> rootPath;
			rootPath.Copy( ptrDrive ); 
			ptr.Copy( rootPath );	
	
			// Remove leading backslash; required for AVKON dialog.
			TPtrC ptrPath( fileNameParser.Path() );
    
			if ( ptrPath.Length() )
				{
				if ( ptrPath[0] == '\\' ) 
					{
					ptrPath.Set( ptrPath.Right( ptrPath.Length() - 1 ) );
					}
				}	
            
     		selected = CAknFileSelectionDialog::RunDlgLD( ptr, ptrPath);
			}
     	CleanupStack:: PopAndDestroy(rootBuf);

		if( !selected )
			{
			// if mmc present launch memory selection dialog
			FeatureManager::InitializeLibL();
			TBool mmcSupported( FeatureManager::FeatureSupported( KFeatureIdMmc ) );
			FeatureManager::UnInitializeLib();

			TInt driveInfo( KErrNone );

#ifdef RD_MULTIPLE_DRIVE
            TDriveNumber selectedMemory;
            User::LeaveIfError(
                DriveInfo::GetDefaultDrive( DriveInfo::EDefaultPhoneMemory,
                                            ( TInt& )selectedMemory ) );
#else
			CAknMemorySelectionDialog::TMemory selectedMemory( 
							CAknMemorySelectionDialog::EPhoneMemory );
#endif

			if ( mmcSupported )
				{
#ifdef RD_MULTIPLE_DRIVE
                TDriveNumber defaultRMS;
                User::LeaveIfError(
                    DriveInfo::GetDefaultDrive( DriveInfo::EDefaultRemovableMassStorage,
                                                ( TInt& )defaultRMS ) );
                TUint status;
                RFs& fs = CCoeEnv::Static()->FsSession();
                driveInfo = DriveInfo::GetDriveStatus( fs, ( TInt& )defaultRMS, status );
#else
   				driveInfo = DriveReady( TDriveNumber( EDriveE ) );
#endif
				}
			// Temp variable
			HBufC* defaultFolder = HBufC::NewLC( KMaxPath );
			TPtr ptrDefaultFolder = defaultFolder->Des();
			HBufC* defaultPath = HBufC::NewLC( KMaxPath );
			TPtr ptrDefaultPath = defaultPath->Des();

			if ( ( driveInfo == KErrNone ) && mmcSupported )
				{
				// Run MEMORY SELECTION DIALOG
#ifdef RD_MULTIPLE_DRIVE
				CAknMemorySelectionDialogMultiDrive* memoryDialog = 
					CAknMemorySelectionDialogMultiDrive::NewL( ECFDDialogTypeSelect, 
													 R_MEMORY_SELECTION_DIALOG,
													 EFalse );
#else
				CAknMemorySelectionDialog* memoryDialog = 
					CAknMemorySelectionDialog::NewL( ECFDDialogTypeSelect, 
													 R_MEMORY_SELECTION_DIALOG,
													 EFalse );
#endif
				CleanupStack::PushL( memoryDialog );
				selected = memoryDialog->ExecuteL( selectedMemory,
											   &ptrDefaultPath, 
											   &ptrDefaultFolder );
				CleanupStack::PopAndDestroy(); // memoryDialog
				}
			
			if ( selected ) 
				{
				selected = CAknFileSelectionDialog::RunDlgLD( ptrDefaultPath, 
														  ptrDefaultFolder );
				if ( selected ) 
					{
					ptr.Copy( ptrDefaultPath );
					}
				}
			CleanupStack::PopAndDestroy(defaultPath); //defaultPath
			CleanupStack::PopAndDestroy(defaultFolder); //defaultFolder
			}
		}
	else
		{
        // aStartPath is NULL
        ptr.Copy( aStartPath );
#ifdef RD_MULTIPLE_DRIVE
		selected = AknCommonDialogsDynMem::RunSelectDlgLD( AknCommonDialogsDynMem::EMemoryTypePhone|
                                                           AknCommonDialogsDynMem::EMemoryTypeInternalMassStorage|
                                                           AknCommonDialogsDynMem::EMemoryTypeMMCExternal|
                                                           AknCommonDialogsDynMem::EMemoryTypeRemote, 
                                                           ptr, R_MEMORY_SELECTION_DIALOG );
#else
		selected = AknCommonDialogs::RunSelectDlgLD( ptr, 
													 R_MEMORY_SELECTION_DIALOG );
#endif
		}

	// return fully qualified file name
	if ( selected )
		{
		aSelectedFileName = buf;
		}

    if ( iObserver )
        {
        iObserver->ReportDialogEventL( 
    	                        MBrowserDialogsProviderObserver::EFileSelect,
    	                        ( TInt ) selected );    
        }       
	return selected;
	}



//-----------------------------------------------------------------------------
//  CBrowserDialogsProvider::DialogSelectOptionL(
//								const TDesC& aTitle, 
//								TBrCtlSelectOptionType aBrCtlSelectOptionType,
//								CArrayFix<TBrCtlSelectOptionData>& aOptions )
//-----------------------------------------------------------------------------
EXPORT_C TBool CBrowserDialogsProvider::DialogSelectOptionL(
								const TDesC& aTitle, 
								TBrCtlSelectOptionType aBrCtlSelectOptionType,
								CArrayFix<TBrCtlSelectOptionData>& aOptions )
	{
    CBrowserSelectElementDlg* dlg = CBrowserSelectElementDlg::NewL(	aTitle, 
												aBrCtlSelectOptionType, 
												aOptions );

	
    iDialogs.Append( dlg );     // Store a pointer to the dialog for CancelAll()

	TInt result = dlg->ExecuteLD();

    RemoveDialogFromArray();
    
    if ( iObserver )
        {
        iObserver->ReportDialogEventL( 
    	                        MBrowserDialogsProviderObserver::ESelectOption,
    	                        ( TInt ) result );    
        }
        
    return result;
	}


//-----------------------------------------------------------------------------
// CBrowserDialogsProvider::DialogUserAuthenticationLC(	
//												const TDesC& /*aUrl**/, 
//												const TDesC& /*aRealm*/, 
//												const TDesC& aDefaultUserName, 
//												HBufC*& aReturnedUserName, 
//												HBufC*& aReturnedPasswd,
//												TBool aBasicAuthentication )
//-----------------------------------------------------------------------------
EXPORT_C TBool CBrowserDialogsProvider::DialogUserAuthenticationLC(	
												const TDesC& /*aUrl**/, 
												const TDesC& /*aRealm*/, 
												const TDesC& aDefaultUserName, 
												HBufC*& aReturnedUserName, 
												HBufC*& aReturnedPasswd,
												TBool /*aBasicAuthentication*/ )
	{
	TBuf<KBrowserDialogsProviderMaxUsername> username;
    TBuf<KBrowserDialogsProviderMaxPassword> password;
    TBool bTryAgain = ETrue;
	TBool cancelled = ETrue;
    
    // Fill in default username and notify that previous authentication failed
    if ( aDefaultUserName.Length() )
        {
        username.Copy( aDefaultUserName );
        bTryAgain = ( ConfirmQueryDialogL( R_BROWSER_DLGPROV_AUTH_FAIL_RETRY_QUERY ) != 0 );
        }
    
    // Show Authentication dialog
    if( bTryAgain )
        {
        switch (GetUserPasswordL( username, password ))
            {
            case EAknSoftkeyOk:
                {
                cancelled = EFalse;
                break;
                }
            case EAknSoftkeyCancel:
            default:
                {
                cancelled = ETrue;
                }
            }
        }

    aReturnedUserName = username.AllocLC();
    aReturnedPasswd = password.AllocLC();
    
    if ( iObserver )
        {
        iObserver->ReportDialogEventL( 
                            MBrowserDialogsProviderObserver::EUserAuthentication,
                            ( TInt ) cancelled );
        }
    
    return !cancelled;
	}


//-----------------------------------------------------------------------------
//  CBrowserDialogsProvider::DialogNoteL( const TDesC& aMessage )
//
//-----------------------------------------------------------------------------
EXPORT_C void CBrowserDialogsProvider::DialogNoteL( const TDesC& aMessage )
	{
    CAknNoteDialog* dialog = new (ELeave) CAknNoteDialog
								( REINTERPRET_CAST( CEikDialog**, &dialog ) );
		
    iDialogs.Append( dialog );                  // Set pointer for CancelAll()
    dialog->PrepareLC( R_DIALOGSPROVIDER_OK_NOTE ) ;
    dialog->SetCurrentLabelL( EGeneralNote, aMessage );
    dialog->RunDlgLD( CAknNoteDialog::ELongTimeout, CAknNoteDialog::ENoTone );

    RemoveDialogFromArray();
    
    if ( iObserver )
        {
        iObserver->ReportDialogEventL( 
                            MBrowserDialogsProviderObserver::ENote,
                            NULL );    
        }    
	}


//-----------------------------------------------------------------------------
//  CBrowserDialogsProvider::DialogAlertL( const TDesC& aTitle, 
//											const TDesC& aMessage )
//-----------------------------------------------------------------------------
EXPORT_C void CBrowserDialogsProvider::DialogAlertL( const TDesC& aTitle, 
											const TDesC& aMessage )
	{
	// Taken from CBrowserAppUi::WmlScriptDlgAlertL()
    HBufC* messagebody = HBufC::NewLC( aMessage.Length()+2 );
    messagebody->Des().Append( aMessage );
    messagebody->Des().Append( _L("\n\n"));

    HBufC* header = HBufC::NewLC( aTitle.Length() );
	header->Des().Append( aTitle );

    CAknMessageQueryDialog* dialog = 
                            CAknMessageQueryDialog::NewL( *messagebody );

	// Store a pointer to the dialog for CancelAll()
    iDialogs.Append( dialog );

    dialog->PrepareLC( R_BROWSER_MESSAGE_QUERY_DIALOG );
    dialog->ButtonGroupContainer().MakeCommandVisible( 
													EAknSoftkeyCancel, EFalse );

    dialog->QueryHeading()->SetTextL( *header );

    dialog->RunLD();

    RemoveDialogFromArray();
    CleanupStack::PopAndDestroy( 2 );  // header , messagebody
    
    if ( iObserver )
        {
        iObserver->ReportDialogEventL( 
                            MBrowserDialogsProviderObserver::EAlert,
                            NULL );
        }
	}


//-----------------------------------------------------------------------------
//  CBrowserDialogsProvider::DialogConfirmL( const TDesC& aTitle,
//												const TDesC& aMessage,
//												const TDesC& aYesMessage,
//												const TDesC& aNoMessage )
//-----------------------------------------------------------------------------
EXPORT_C TBool CBrowserDialogsProvider::DialogConfirmL( const TDesC& aTitle,
												const TDesC& aMessage,
												const TDesC& aYesMessage,
												const TDesC& aNoMessage )
	{
	TBool retVal;

	// CAknMessageQueryDialog must have title text, for the message text to be shown
	if ( aTitle.Length() )
		{
		CAknMessageQueryDialog* dialog = 
								CAknMessageQueryDialog::NewL( (TDesC&) aMessage );

		
		iDialogs.Append( dialog );  // Store a pointer to the dialog for CancelAll()

		dialog->PrepareLC( R_BROWSER_MESSAGE_QUERY_DIALOG );

		dialog->QueryHeading()->SetTextL( aTitle );

		if( aYesMessage.Length() )
			{
			dialog->ButtonGroupContainer().SetCommandL( EAknSoftkeyOk, aYesMessage );
			}

		if( aNoMessage.Length() )
			{ 
			dialog->ButtonGroupContainer().SetCommandL( EAknSoftkeyCancel, aNoMessage );
			}

		if ( !dialog->RunLD() )
			{
			retVal = EFalse;
			}
		else
			{
			retVal = ETrue;
			}

		RemoveDialogFromArray();
		}
	else
		{
		// This dialog has an animation question mark.
	    CAknQueryDialog* dialog = new (ELeave) CAknQueryDialog();

		iDialogs.Append( dialog );	// Store dialog pointer for CancelAll()

		dialog->PrepareLC( R_BROWSER_CONFIRM_QUERY );

		dialog->SetPromptL( aMessage );
		
		if( aYesMessage.Length() )
			{
			dialog->ButtonGroupContainer().SetCommandL( EAknSoftkeyYes, aYesMessage );
			}

		if( aNoMessage.Length() )
			{ 
			dialog->ButtonGroupContainer().SetCommandL( EAknSoftkeyNo, aNoMessage );
			}

		if ( !dialog->RunLD() )
			{
			retVal = EFalse;
			}
		else
			{
			retVal = ETrue;
			}
		RemoveDialogFromArray();		
		}
		
	if ( iObserver )	
    	{
    	iObserver->ReportDialogEventL( 
                            MBrowserDialogsProviderObserver::EConfirm,
                            ( TInt ) retVal );
    	}
	return retVal;
	}

//-----------------------------------------------------------------------------
// CBrowserDialogsProvider::DialogPromptLC( const TDesC& aTitle,
//												const TDesC& aMessage,
//												const TDesC& aDefaultInput,
//												HBufC*& aReturnedInput )
//-----------------------------------------------------------------------------
EXPORT_C TBool CBrowserDialogsProvider::DialogPromptLC( const TDesC& aTitle,
												const TDesC& aMessage,
												const TDesC& aDefaultInput,
												HBufC*& aReturnedInput )
	{
    TBool retVal;

	// defInput is not modified by the dialog.
    TPtr defInput( (TUint16*) aDefaultInput.Ptr(), aDefaultInput.Length(), 
													    KMaxAltTextLength );

    CBrowserScriptPromptDialog* dialog = 
		new (ELeave) CBrowserScriptPromptDialog( defInput, aReturnedInput );

	iDialogs.Append( dialog );  // Store a pointer to the dialog for CancelAll()

    dialog->PrepareLC( R_DATA_QUERY_WITH_HEADING );
	dialog->SetHeaderTextL( aTitle );
    dialog->SetPromptL( aMessage );

	if ( !dialog->RunLD() )
		{
		retVal = EFalse;
		}
	else
		{
		retVal = ETrue;
		}

    RemoveDialogFromArray();
    CleanupStack::PushL( aReturnedInput );

    if ( iObserver )
        {
        iObserver->ReportDialogEventL( 
                            MBrowserDialogsProviderObserver::EPrompt,
                            (TInt) retVal );
    	}
    	
    return retVal;
	}

//-----------------------------------------------------------------------------
//  CBrowserDialogsProvider::DialogDownloadObjectL(
//										CBrCtlObjectInfo* aBrCtlObjectInfo )
//
//-----------------------------------------------------------------------------
EXPORT_C TBool CBrowserDialogsProvider::DialogDownloadObjectL(
										CBrCtlObjectInfo* aBrCtlObjectInfo )
	{
	HBufC* header;	// holds the header text
	HBufC* message;	// a buffer holding the text for each list item
	HBufC* buf;		// a buffer for strings
	TInt ret;		// general variable for return values
	
	//-------------------------------------------
	// Query object info
	//
	CEikFormattedCellListBox* listBox = new ( ELeave ) 
												CAknDoublePopupMenuStyleListBox;
	CleanupStack::PushL( listBox );

	// If supported show OK & Cancel buttons, otherwise just CANCEL
	CAknPopupList* popupList;
	if ( aBrCtlObjectInfo->AppSupported() || 
						    aBrCtlObjectInfo->PluginSupported() )
		{
		popupList = CAknPopupList::NewL(listBox, R_AVKON_SOFTKEYS_OK_CANCEL__OK, 
												AknPopupLayouts::EMenuDoubleWindow);
		}
	else
		{
		popupList = CAknPopupList::NewL(listBox, R_AVKON_SOFTKEYS_CANCEL, 
												AknPopupLayouts::EMenuDoubleWindow);
		}

    CleanupStack::PushL( popupList );

	TInt flags = CEikListBox::EDisableHighlight;
	listBox->ConstructL( popupList, flags );
	listBox->CreateScrollBarFrameL( ETrue );
	listBox->ScrollBarFrame()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff, 
															CEikScrollBarFrame::EAuto );

	MDesCArray* itemList = listBox->Model()->ItemTextArray();
	CDesCArray* itemArray = ( CDesCArray* ) itemList;
	
	//-------------------------------------------
	// Header
	//
	header = StringLoader::LoadLC( R_BROWSER_DLGPROV_OBJ_DOWNLOAD_TITLE );
	popupList->SetTitleL( *header );
	CleanupStack::PopAndDestroy();	// header
	header = NULL;

	//-------------------------------------------
	// Supported?
	//
	if ( !( aBrCtlObjectInfo->AppSupported() 
			|| aBrCtlObjectInfo->PluginSupported()) )	// Object is "Not Supported"
		{
		HBufC* supported = StringLoader::LoadLC( R_BROWSER_DLGPROV_OBJ_DOWNLOAD_NOT_SUPPORTED );		
		itemArray->AppendL( *supported );
		CleanupStack::PopAndDestroy();	// supported
		}

	//-------------------------------------------
	// Application Size
	//
	message = HBufC::NewLC( KBrCtlObjectElementMaxLength );
	buf = StringLoader::LoadLC( R_BROWSER_DLGPROV_OBJ_DOWNLOAD_SIZE );	// first line - bold text
	message->Des().Append( *buf );
	message->Des().Append( TChar( '\t' ) );
		
	if ( aBrCtlObjectInfo->Size().Length() )	// second line - normal text
		{
		HBufC* size;
		const TPtrC sizeString = aBrCtlObjectInfo->Size();
		// convert size to int
		TLex sizeLex( sizeString );		
		TInt sizeInt = 0;
				
		ret = sizeLex.Val( sizeInt );
        __ASSERT_DEBUG( !ret , User::Panic( _L("BrowserDialogsProvider"), EInvalidSizeValue ) ) ;
        
		// KB
		if ( sizeInt < KBrCtlMegabyte )
			{
			// %N KB
			size = StringLoader::LoadLC( R_BROWSER_DLGPROV_UNIT_KB, sizeInt );
			}
		// MB - up to 999MB
		else if ( sizeInt < ( KBrCtlMegabyte * 1000 ) )
			{			
			HBufC* mbString = HBufC::NewLC( KBrCtlMBFormat );
			HBufC* decimalSeparator = StringLoader::LoadLC( R_WML_DECIMAL_SEPARATOR );
			TInt mb = sizeInt / KBrCtlMegabyte;
			TInt remainder = sizeInt % KBrCtlMegabyte;

			// Size shown in 3 numbers
			mbString->Des().AppendNum( mb );
			if ( mb < 10 )
				{
				// show 2 decimal places
				mbString->Des().Append( *decimalSeparator );

                // Add leading zeroes if necessary                                
                if ( remainder < ( KBrCtlMegabyte / 10 ) )
                    {
                    // One leading zero needed
                    mbString->Des().AppendNum( 0 );
                    }

				mbString->Des().AppendNum(  ( remainder * 100 ) / KBrCtlMegabyte  );
				}
			else if ( mb < 100 )
				{
				// show 1 decimal place
				mbString->Des().Append( *decimalSeparator );				
				mbString->Des().AppendNum( ( remainder * 10 ) / KBrCtlMegabyte );
				}

			CleanupStack::PopAndDestroy(); // decimalSeparator
			
			// %U MB
			size = StringLoader::LoadLC( R_BROWSER_DLGPROV_UNIT_MB, *mbString );
			CleanupStack::Pop();//size
			CleanupStack::PopAndDestroy(); // mbString
			CleanupStack::PushL( size );
			}
		// GB
		else
			{
			HBufC* gb = HBufC::NewLC( KBrCtlGBFormat );
			gb->Des().AppendNum( sizeInt / KBrCtlMegabyte );
			size = StringLoader::LoadLC( R_BROWSER_DLGPROV_UNIT_MB, *gb );
			CleanupStack::Pop();//size
			CleanupStack::PopAndDestroy(); // gb
			CleanupStack::PushL( size );
			}
		message->Des().Append( *size );
		CleanupStack::PopAndDestroy();	// size
		}
	else
		{
		HBufC* sizeString = StringLoader::LoadLC( R_BROWSER_DLGPROV_OBJ_DOWNLOAD_UNKNOWN );
		message->Des().Append( *sizeString );
		CleanupStack::PopAndDestroy(); // sizeString
		}
	itemArray->AppendL( *message );
	CleanupStack::PopAndDestroy(2);	// buf , message
	buf = NULL;
	message = NULL;

	//-------------------------------------------
	// Application Name	- only show if Supported
	//	
	if ( aBrCtlObjectInfo->AppSupported() 
						|| aBrCtlObjectInfo->PluginSupported() )
		{
		message = HBufC::NewLC( KBrCtlObjectElementMaxLength );
		HBufC* appName = StringLoader::LoadLC( R_BROWSER_DLGPROV_OBJ_DOWNLOAD_OPENWITH );
		message->Des().Append( *appName );
		message->Des().Append( TChar( '\t' ) );
		message->Des().Append( aBrCtlObjectInfo->AppName() );
		itemArray->AppendL( *message );
		CleanupStack::PopAndDestroy(2);	// appName , message
		appName = NULL;
		message = NULL;
		}

	//-------------------------------------------
	// (MIME) File Type
	//
	message = HBufC::NewLC( KBrCtlObjectElementMaxLength );
	buf = StringLoader::LoadLC( R_BROWSER_DLGPROV_OBJ_DOWNLOAD_FILETYPE );
	message->Des().Append( *buf );
	message->Des().Append( TChar( '\t' ) );

	if ( aBrCtlObjectInfo->FileType().Length() )
		{
		message->Des().Append( aBrCtlObjectInfo->FileType() );
		}
	else
		{		
		HBufC* fileName = StringLoader::LoadLC( R_BROWSER_DLGPROV_OBJ_DOWNLOAD_UNKNOWN );
		message->Des().Append( *fileName );
		CleanupStack::PopAndDestroy(); // fileName
		fileName = NULL;
		}
	itemArray->AppendL( *message );
	CleanupStack::PopAndDestroy(2); // buf , message
	buf = NULL;
	message = NULL;

	CleanupStack::Pop();	// popupList 
    
    iDialogs.Append( popupList );   // store ptr to dlg

	// Show Dialog
	if ( !popupList->ExecuteLD() )
		{
		ret = EFalse;
		}
	else
		{
		ret = ETrue;
		}
    
	// cleanup
    RemoveDialogFromArray();
	CleanupStack::PopAndDestroy();	// listBox 
	listBox = NULL;

    if ( iObserver )
        {
        iObserver->ReportDialogEventL( 
                            MBrowserDialogsProviderObserver::EDownloadObject,
                            (TInt) ret );
        }

	return ret;
	}

//-----------------------------------------------------------------------------
//CBrowserDialogsProvider::DialogDisplayPageImagesL( 
//							CArrayFixFlat<TBrCtlImageCarrier>& aPageImages )
//-----------------------------------------------------------------------------
EXPORT_C void CBrowserDialogsProvider::DialogDisplayPageImagesL( 
							CArrayFixFlat<TBrCtlImageCarrier>& aPageImages )
	{
    if (aPageImages.Count() > 1)
        {
        CBrowserViewImagesPopup* dialog = new (ELeave) CBrowserViewImagesPopup();
        CleanupStack::PushL( dialog );
        dialog->ConstructL( aPageImages );    

        iDialogs.Append( dialog );                  // add dialog ptr to array
        CleanupStack::Pop();
        dialog->ExecuteLD();
        RemoveDialogFromArray();
        }
    else if (aPageImages.Count() == 1)
        {
        CBrowserViewImagesPopup::PhotoOperationL(aPageImages[0]);
        }
    if ( iObserver )
    	{
            iObserver->ReportDialogEventL( 
                       	MBrowserDialogsProviderObserver::EDisplayPageImages,NULL );
    	}     
	}


//-----------------------------------------------------------------------------
//  CBrowserDialogsProvider::CancelAll()
//
//-----------------------------------------------------------------------------
//
EXPORT_C void CBrowserDialogsProvider::CancelAll()
	{
    // Empty the array
    iDialogs.ResetAndDestroy();
	}

//-----------------------------------------------------------------------------
//  CBrowserDialogsProvider::RemoveDialogFromArray()
//
//-----------------------------------------------------------------------------
//
void CBrowserDialogsProvider::RemoveDialogFromArray()
    {
    // Do not try to remove a dialog if CancelAll() has been called
    if ( iDialogs.Count() )
        {
        iDialogs.Remove( iDialogs.Count() - 1 );    // remove dialog ptr to array
        }
    }

//-----------------------------------------------------------------------------
// New functions
//

// ----------------------------------------------------------------------------
// CBrowserDialogsProvider::ShowTooltip
// ----------------------------------------------------------------------------
//
EXPORT_C void CBrowserDialogsProvider::ShowTooltipL( const TDesC& aText, 
                                                     TInt aDuration,
                                                     TInt aDelay )
    {
    iTooltip->SetTextL( aText );
    iTooltip->SetTimeDelayBeforeShow( aDelay );
    iTooltip->SetTimePopupInView( aDuration );
    iTooltip->ShowInfoPopupNote();
    }


// ----------------------------------------------------------------------------
// CBrowserDialogsProvider::UploadProgressNoteL
// ----------------------------------------------------------------------------
//
EXPORT_C void CBrowserDialogsProvider::UploadProgressNoteL(  
                                TInt32 aTotalSize,
                                TInt32 aChunkSize,
                                TBool aIsLastChunk,
                                MBrowserDialogsProviderObserver* aObserver )
    {
    // If the download is finished.
    if( aIsLastChunk )
        {
        if( iUploadProgressNote )
            {
            // Delete progress note.
            delete iUploadProgressNote;
            iUploadProgressNote = NULL;
            }
        }
    else
        {
        // If the download just started.
        if ( !iUploadProgressNote )
            {
            // Create progress note.
            iUploadProgressNote = CBrowserUploadProgressNote::NewL( 
                    aTotalSize, 
                    aObserver,
                    REINTERPRET_CAST ( CEikDialog**, &iUploadProgressNote ) );
                    
            iUploadProgressNote->UpdateL( aChunkSize );
            }
        // If the download is in progress, and needs to update.
        else
            {
            // Update progress note.
            iUploadProgressNote->UpdateL( aChunkSize );
            }
        }
    }
    
//-----------------------------------------------------------------------------
//CBrowserDialogsProvider::DialogMimeFileSelectLC( 
//                                              HBufC*& aSelectedFileName,
//												const TDesC& aMimeType )
//-----------------------------------------------------------------------------

EXPORT_C TBool CBrowserDialogsProvider::DialogMimeFileSelectLC( 
													HBufC*& aSelectedFileName,
													const TDesC& aMimeType )
	{
	TBool selected( EFalse );
	
	TMediaFileType mediaFileType(ENoMediaFile);

    if (aMimeType.FindF(KMimeTypeImage) != KErrNotFound)
        {
        mediaFileType = EImageFile;
        }
    else if (aMimeType.FindF(KMimeTypeAudio) != KErrNotFound)
        {
        mediaFileType = EAudioFile;
        }
    else if (aMimeType.FindF(KMimeTypeVideo) != KErrNotFound)
        {
        mediaFileType = EVideoFile;
        }

    // if mime type not supported then use default file selector
    if (mediaFileType == ENoMediaFile)
        {
        return DialogFileSelectLC(KNullDesC(), KNullDesC(), aSelectedFileName);
        }

	HBufC* buf = NULL;
    buf = HBufC::NewLC( KMaxFileName );
	TPtr ptr ( buf->Des() );

    CDesCArrayFlat *selectedFiles = new (ELeave) CDesCArrayFlat(1);
    CleanupStack::PushL(selectedFiles);

	selected = MGFetch::RunL( *selectedFiles,
                                mediaFileType,
                                EFalse,
                                this );
	
	if ( selected && selectedFiles->Count()>0) 
		{
		ptr.Copy( (*selectedFiles)[0] );
		}
	selectedFiles->Reset();
	CleanupStack::PopAndDestroy();//selectedFiles

	
	// return fully qualified file name
	if ( selected )
		{
		aSelectedFileName = buf;
		}

    if ( iObserver )
        {
        iObserver->ReportDialogEventL( 
    	                        MBrowserDialogsProviderObserver::EFileSelect,
    	                        ( TInt ) selected );    
        }       
	return selected;
	}

TBool CBrowserDialogsProvider::VerifySelectionL
        (const MDesCArray* aSelectedFiles)
    {
    return (aSelectedFiles && aSelectedFiles->MdcaCount() > 0)? ETrue: EFalse;
    }

// ----------------------------------------------------------------------------
// CBrowserDialogsProvider::ConfirmQueryDialogL
// ----------------------------------------------------------------------------
//
TInt CBrowserDialogsProvider::ConfirmQueryDialogL(	const TInt aPromptResourceId,
													TInt aResId, 
													TInt aAnimation )
    {
    HBufC* prompt = StringLoader::LoadLC( aPromptResourceId );
    TInt ret = ConfirmQueryDialogL( *prompt, aResId, aAnimation );
    CleanupStack::PopAndDestroy();  // prompt
    return ret;
    }

// ----------------------------------------------------------------------------
// CBrowserDialogsProvider::ConfirmQueryDialogL
// ----------------------------------------------------------------------------
//
TInt CBrowserDialogsProvider::ConfirmQueryDialogL(	const TDesC& aPrompt, 
													TInt aResId, 
													TInt aAnimation )
    {
    TInt ret;
    CAknQueryDialog* dialog = new (ELeave) CAknQueryDialog();

	// Store dialog pointer for CancelAll()
	iDialogs.Append( dialog );

    dialog->PrepareLC( R_BROWSER_CONFIRM_QUERY );
    // passing prompt text in constructor is deprecated according to 
    // AknQueryDialog.h
    dialog->SetPromptL( aPrompt );
    if( aResId )
        {
        dialog->ButtonGroupContainer().SetCommandSetL( aResId );
        }

    if( aAnimation )
        {
        CAknQueryControl* control = STATIC_CAST( CAknQueryControl*, dialog->Control(EGeneralQuery) );
        control->SetAnimationL( aAnimation );
        }
    ret = dialog->RunLD();
    RemoveDialogFromArray();
    return ret;
    }


// ---------------------------------------------------------
// CBrowserDialogsProvider::GetUserPasswordL
// ---------------------------------------------------------
//
TInt CBrowserDialogsProvider::GetUserPasswordL( TDes& aUsername, TDes& aPassword )
	{
    TInt ret;
	CBrowserAuthenticationDialog* dialog = 
                CBrowserAuthenticationDialog::NewL( aUsername, aPassword );

	iDialogs.Append( dialog );                  // Store pointer for CancelAll()
	if (JAPANESE)
    	{
    	ret = dialog->ExecuteLD( R_WB_SCRIPT_USERPASSWD_LINES_JAPAN );
    	}
	else
    	{
    	ret = dialog->ExecuteLD( R_WB_SCRIPT_USERPASSWD_LINES_NON_JAPAN );
    	}
    RemoveDialogFromArray();
    return ret;
	}


// -----------------------------------------------------------------------------
// CBrowserDialogsProvider::AssureResourceL
// -----------------------------------------------------------------------------
//
void CBrowserDialogsProvider::AssureResourceL()
    {
    if ( !iResourceOpened )
        {
        // Add resource file.
        TParse* fp = new(ELeave) TParse(); 

        TInt err = fp->Set( KBrowserDialogsProviderDirAndFile, 
                            &KDC_RESOURCE_FILES_DIR, 
                            NULL ); 
        if ( err != KErrNone)
            {
            User::Leave( err );
            }   
    
        TFileName resourceFileName = fp->FullName();
        delete fp;

        TRAPD( error, iResourceLoader.OpenL( resourceFileName ) );
        if ( !error )
            {
            iResourceOpened = ETrue;
            }
        else
            {
            User::Leave( error );
            }
        }
    }


// -----------------------------------------------------------------------------
// CBrowserDialogsProvider::DriveReady
// -----------------------------------------------------------------------------
//
TInt CBrowserDialogsProvider::DriveReady( TDriveNumber aDrive )
	{
	RFs& fs = CCoeEnv::Static()->FsSession();
	_LIT( KFat, "Fat" );
    // Check if the drive is already mounted
	TFullName name;
	TInt error( fs.FileSystemName( name, aDrive ) );
	if ( error )
		{
		return KErrNotReady;
		}
	else
		{
		// check if MMC already mounted	
		if ( name.Length() == 0 )
			{
			// MMC drive isnt mounted at present, so try it now....
			error = fs.MountFileSystem( KFat, aDrive );

			// If it's a locked MMC and the password is already known it'll be
			// unlocked automatically when it's mounted., otherwise the mount will
			// return with KErrLocked.....
			if ( error == KErrLocked )
				{
				return KErrLocked;
				}
			}
		}
	return KErrNone;
	}

//  END OF FILE
