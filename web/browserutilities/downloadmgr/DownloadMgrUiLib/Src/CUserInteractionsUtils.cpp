/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Supports user interaction dialogs for downloads
*
*/



// INCLUDE FILES
//#include <platform/mw/Browser_platform_variant.hrh>
#include    <Browser_platform_variant.hrh>
#include    "CUserInteractionsUtils.h"
#include    "MDownloadHandlerObserver.h"
#include    "CDownloadMgrUiBase.h"
#include    "CDownloadMgrUiLibRegistry.h"
#include    "CDownloadMgrUiDownloadsList.h"
#include    "CDownloadMgrUiUserInteractions.h"
#include    "UiLibLogger.h"
#include    "DMgrUiLibPanic.h"
#include    <DownloadMgrUiLib.rsg>
#include    <DownloadMgrClient.h>
#include    <AiwGenericParam.h>
#include    <DocumentHandler.h>
#include    <coemain.h>
#include    <ErrorUI.h>
#include    <eikenv.h>
#include    <apmstd.h>
#include    <CAknFileSelectionDialog.h>
#include    <CAknMemorySelectionDialog.h>
#include    <AknQueryDialog.h>
#include    <StringLoader.h>
#include    <AknSoftNotificationParameters.h>
#include    <AknSoftNotifier.h>
#include    <aknnotewrappers.h>
#include    <BrowserDialogsProvider.h>
#include    <HttpDownloadMgrCommon.h>
#include    <pathinfo.h>
#include    <AknGlobalNote.h>
#include    <Oma2Agent.h>
#include    <DRMCommon.h>
#include	<apmrec.h>
#include	<apgcli.h>
#include    <s32mem.h>
#include    "bautils.h"
#include	<etelmm.h>
#include	<mmtsy_names.h>
#include    <rconnmon.h>
#include    <DcfEntry.h>
#include    <DcfRep.h>

// following line is temporary: AVKON dependency removal
#undef BRDO_APP_GALLERY_SUPPORTED_FF

#ifdef BRDO_APP_GALLERY_SUPPORTED_FF
#include    <MGXFileManagerFactory.h>
#include    <CMGXFileManager.h>
#endif

// LOCAL CONSTANTS AND MACROS
const TInt KErrorUiHttpStatusBase = -25000;
const TInt KMinimumSoftNotePriority = 1000;
#define GLOBAL_HTTP_ERROR( err ) ( KErrorUiHttpStatusBase - err )

const TInt FilenameSuffixMaxLength = 16; // Extra space for localization
const TInt FilenameSuffixMaxValue = 9999;// Maximum file number

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUserInteractionsUtils::CUserInteractionsUtils
// -----------------------------------------------------------------------------
//
CUserInteractionsUtils::CUserInteractionsUtils( CDownloadMgrUiBase& aDMgrUiBase,
                                                MDownloadMgrUiLibRegModel& aRegistryModel )
:   iCoeEnv( *CCoeEnv::Static() ),
    iDMgrUiBase( aDMgrUiBase ),
    iRegistryModel( aRegistryModel ),
    iDocHandlerUsesTheUi( EFalse )
    {
    }

// -----------------------------------------------------------------------------
// CUserInteractionsUtils::ConstructL
// -----------------------------------------------------------------------------
//
void CUserInteractionsUtils::ConstructL()
    {
    CLOG_ENTERFN("CUserInteractionsUtils::ConstructL");
    iDialogsProv = CBrowserDialogsProvider::NewL( 0 );
    CLOG_WRITE(" iDialogsProv OK");
    iSoftNotifier = CAknSoftNotifier::NewL();
    CLOG_WRITE(" iSoftNotifier OK");
    iErrorUi = CErrorUI::NewL();

    iUnderTenFormatter = StringLoader::LoadL( R_QTN_IV_NAME_SUFFIX_UNDER_TEN_FORMATTER );
    iOverTenFormatter = StringLoader::LoadL( R_QTN_IV_NAME_SUFFIX_OVER_TEN_FORMATTER );
    iSearchChar = (*iUnderTenFormatter)[0];
    iCodDownload = EFalse;

    CLOG_LEAVEFN("CUserInteractionsUtils::ConstructL");
    }

// -----------------------------------------------------------------------------
// CUserInteractionsUtils::NewL
// -----------------------------------------------------------------------------
//
CUserInteractionsUtils* CUserInteractionsUtils::NewL
( CDownloadMgrUiBase& aDMgrUiBase, MDownloadMgrUiLibRegModel& aRegistryModel )
    {
    CUserInteractionsUtils* self =
        new ( ELeave ) CUserInteractionsUtils( aDMgrUiBase, aRegistryModel );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// Destructor
CUserInteractionsUtils::~CUserInteractionsUtils()
    {
    CLOG_ENTERFN("CUserInteractionsUtils::~CUserInteractionsUtils");
    // Cancel all executing dialogs/notes/controls
    CancelAllDialogs();
    CLOG_WRITE(" CancelAllDialogs OK");
    delete iDocHandler;
    CLOG_WRITE(" iDocHandler OK");
    delete iSoftNotifier;
    CLOG_WRITE(" iSoftNotifier OK");
    delete iErrorUi;
    CLOG_WRITE(" iErrorUi OK");
    delete iUnderTenFormatter;
    CLOG_WRITE(" iUnderTenFormatter OK");
    delete iOverTenFormatter;
    CLOG_WRITE(" iOverTenFormatter OK");
    CLOG_LEAVEFN("CUserInteractionsUtils::~CUserInteractionsUtils");
    }

// -----------------------------------------------------------------------------
// CUserInteractionsUtils::ShowErrorNoteL
// -----------------------------------------------------------------------------
//
void CUserInteractionsUtils::ShowErrorNoteL( RHttpDownload& aDownload,
                                             TInt32 aErrorId )
    {
    TInt32 sysErrorId( KErrNone ); // Global (system-wide) error ID

    // DMgr specific error code. See THttpDownloadMgrError.
    // Convert it to Symbian OS system-wide error code.
    switch ( aErrorId )
        {
        case EGeneral:
            {
            // EDlAttrGlobalErrorId gives the system-wide code.
            User::LeaveIfError( aDownload.GetIntAttribute
                              ( EDlAttrGlobalErrorId, sysErrorId ) );
            break;
            }
        case EInternal:
        	{
        	sysErrorId = KErrGeneral;
        	break;
        	}
        case EContentFileIntegrity:
        	{
        	sysErrorId = KErrCorrupt;
        	break;
        	}
        case EDiskFull:
            {
            sysErrorId = KErrDiskFull;
            break;
            }
        case EConnectionFailed:
        case ETransactionFailed:
        case EMoveFailed:
            {
            // EDlAttrGlobalErrorId gives the system-wide code.
            User::LeaveIfError( aDownload.GetIntAttribute
                              ( EDlAttrGlobalErrorId, sysErrorId ) );
            break;
            }
        case EDestFileWriteFailed:
        	{
        	sysErrorId = KErrWrite;
        	break;
        	}
        case EMMCRemoved:
        	{
        	sysErrorId = KErrNotFound;
        	break;
        	}
        case EBadUrl:
        	{
        	sysErrorId = GLOBAL_HTTP_ERROR(414);
        	break;
        	}
        case EWrongDestFilename:
        	{
        	sysErrorId = KErrArgument;
        	break;
        	}
        case EDestFileInUse:
        	{
        	sysErrorId = KErrAccessDenied;
        	break;
        	}
        case EHttpUnhandled:
        	{
            // EDlAttrGlobalErrorId gives the system-wide code.
            User::LeaveIfError( aDownload.GetIntAttribute
                              ( EDlAttrGlobalErrorId, sysErrorId ) );
        	break;
        	}
        // Don't show error notification in the following cases:
        case EHttpAuthenticationFailed:
        case EProxyAuthenticationFailed:
        	{
            sysErrorId = KErrNone;
            break;
        	}
        case EObjectNotFound:
            {
            // Convert to global http error code.
            sysErrorId = GLOBAL_HTTP_ERROR(404);
            break;
            }
        case EPartialContentModified:
        	{
            sysErrorId = GLOBAL_HTTP_ERROR(412);
            break;
        	}
        case EContentExpired:
            {
            sysErrorId = GLOBAL_HTTP_ERROR(205);
            break;
            }
        case EHttpRestartFailed:
            {
            // Show error note by ourself
            HBufC* errorString =
                   iCoeEnv.AllocReadResourceLC( R_DMUL_ERROR_START_FROM_BEGINNING );
            CAknGlobalNote* globalNote = CAknGlobalNote::NewL();
            CleanupStack::PushL( globalNote );
            globalNote->ShowNoteL( EAknGlobalErrorNote, *errorString );
            CleanupStack::PopAndDestroy( 2, errorString );  // globalNote, errorString
            sysErrorId = KErrNone; // Do not use errorui
            break;
            }
        default:
            {
            sysErrorId = KErrGeneral;
            break;
            }
        }

    if ( sysErrorId != KErrNone )
        {
        if ( sysErrorId == KErrHttpPartialResponseReceived )
            { // FMLK-72653Y : as server closed connection unexpectedly,
              // and there is no good error note, use KErrDisconnected instead.
            iErrorUi->ShowGlobalErrorNoteL( KErrDisconnected, CTextResolver::ECtxAutomatic );
            }
        else
            {
            iErrorUi->ShowGlobalErrorNoteL( sysErrorId, CTextResolver::ECtxAutomatic );
            }
        }
    }

// -----------------------------------------------------------------------------
// CUserInteractionsUtils::ShowErrorNoteL
// -----------------------------------------------------------------------------
//
void CUserInteractionsUtils::ShowErrorNoteL( TInt aErrorId )
    {
    iErrorUi->ShowGlobalErrorNoteL( aErrorId, CTextResolver::ECtxAutomatic );
    }

// -----------------------------------------------------------------------------
// CUserInteractionsUtils::IsRecoverableFailL
// -----------------------------------------------------------------------------
//
TBool CUserInteractionsUtils::IsRecoverableFailL( RHttpDownload& aDownload,
                                                  TInt32 aErrorId ) const
    {
    TBool recoverable( ETrue );

    // What is the error?
    TInt32 errorId( aErrorId ); // DMgr specific error ID
    CLOG_WRITE_FORMAT("Fail reason: %d", errorId);
    //
    // EDlAttrGlobalErrorId gives the system-wide code.
    TInt32 sysErrorId( 0 );
    User::LeaveIfError( aDownload.GetIntAttribute
                      ( EDlAttrGlobalErrorId, sysErrorId ) );

    TBool pausable( ETrue );
    User::LeaveIfError( aDownload.GetBoolAttribute( EDlAttrPausable, pausable ) );
    if ( errorId == EObjectNotFound )
         // Other error situations can be added to the list. //
        {
        recoverable = EFalse;
        }
    else if ( errorId == ETransactionFailed )
        {
        if ( sysErrorId == KBadMimeType || sysErrorId == -20000 || sysErrorId == KErrHttpPartialResponseReceived || !pausable )
        // KBadMimeType and -20000 is from DRMFilter
        // Transaction must be cancelled and download must be deleted
            {
            recoverable = EFalse;
            }
        else
            {
            recoverable = ETrue;
            }
        }
    else if ( errorId == EGeneral )
        {
        if ( sysErrorId == KErrNoMemory || sysErrorId == KErrDiskFull )
        //Memory full or Disk Full can be Recoverable error if Download is Pausable
            {
            recoverable = pausable;
            }
        else
            {
            recoverable = EFalse;
            }
        }
    else
        {
        recoverable = EFalse;
        }

    CLOG_WRITE_FORMAT("IsRecoverableFailL: %d", (TInt)recoverable);
    return recoverable;
    }

// -----------------------------------------------------------------------------
// CUserInteractionsUtils::HandleContentL
// -----------------------------------------------------------------------------
//
void CUserInteractionsUtils::HandleContentL
( RHttpDownload& aDownload, MDownloadHandlerObserver& aHandlerObserver )
    {
    CLOG_ENTERFN("CUserInteractionsUtils::HandleContentL");

    __ASSERT_DEBUG( !iRegistryModel.UserInteractions().IsUiBusy(),
                    Panic( EUiLibPanDocHandlerAlreadyRunning ) );

    if ( iRegistryModel.UserInteractions().IsUiBusy() )
        {
        CLOG_WRITE(" IsUiBusy() true");
        }
    else
        {
        iHandledDownload = &aDownload;
        iHandlerObserver = &aHandlerObserver;

        // BEGIN: Get attributes for generic params
        //=========================
        //   EDlAttrCurrentUrl
        //=========================
        HBufC* url = HBufC::NewLC( KMaxUrlLength );
        TPtr urlPtr = url->Des();
        User::LeaveIfError
            ( aDownload.GetStringAttribute( EDlAttrCurrentUrl, urlPtr ) );
        CLOG_WRITE_FORMAT(" EDlAttrCurrentUrl: %S",url);
        //=========================
        //   EDlAttrDestFilename
        //=========================
        HBufC* fileName = HBufC::NewLC( KMaxPath );
        TPtr fileNamePtr = fileName->Des();
        TInt32 numMediaObjects = 0;
        User::LeaveIfError( aDownload.GetIntAttribute( EDlAttrNumMediaObjects, numMediaObjects ) );
        if (numMediaObjects > KFirstMoIndex)
            {
            User::LeaveIfError
                ( aDownload.GetStringAttribute( EDlAttrDestFilename, KFirstMoIndex, fileNamePtr ) );
            }
        else
            {
            User::LeaveIfError
                ( aDownload.GetStringAttribute( EDlAttrDestFilename, fileNamePtr ) );
            }
        CLOG_WRITE_FORMAT(" EDlAttrDestFilename: %S",&fileNamePtr);

        // check if file exists, if does not , don't continue
        if ( !IsDuplicateL( fileNamePtr ) )
            {
            HBufC* infoPrompt = StringLoader::LoadLC( R_DMUL_ERROR_FILE_NOT_FOUND);
            CAknInformationNote* note = new(ELeave)  CAknInformationNote();
            note->ExecuteLD(*infoPrompt);
            CleanupStack::PopAndDestroy(infoPrompt);

            CleanupStack::PopAndDestroy( fileName ); // fileName
            CleanupStack::PopAndDestroy( url ); // url
            return;
            }

        //=========================
        //   EDlAttrContentType
        //=========================
        HBufC8* contentType = ContentTypeL(aDownload, ETrue, KFirstMoIndex);

        //=========================
        //   EDlMgrIap
        //=========================
        TInt32 iap;
        User::LeaveIfError
            ( iRegistryModel.DownloadMgr().GetIntAttribute( EDlMgrIap, iap ) );
        CLOG_WRITE_FORMAT(" EDlMgrIap: %d",iap);
        //=========================
        //   EDlAttrCharSet
        //=========================
        HBufC* charset = HBufC::NewLC( KMaxDefAttrLength );
        TPtr charsetPtr = charset->Des();
        TInt retVal = aDownload.GetStringAttribute( EDlAttrCharSet, charsetPtr );
        CLOG_WRITE_FORMAT(" EDlAttrCharSet retVal: %d",retVal);
        if( ( KErrNotFound == retVal ) || ( KErrNone == retVal ) )
            {
            // The Char Set is either found or not found but we don't care!!!
            CLOG_WRITE_FORMAT(" EDlAttrCharSet: %S",charset);
            }
        else
            {
            User::Leave( retVal );
            }
        //=========================
        //   EDlAttrRequestReferer
        //=========================
        HBufC* refererUri = HBufC::NewLC( KMaxUrlLength );
        TPtr refererUriPtr = refererUri->Des();
        retVal = aDownload.GetStringAttribute( EDlAttrRequestReferer, refererUriPtr );
        CLOG_WRITE_FORMAT(" EDlAttrRequestReferer retVal: %d",retVal);
        if( ( KErrNotFound == retVal ) || ( KErrNone == retVal ) )
            {
            // The Char Set is either found or not found but we don't care!!!
            CLOG_WRITE_FORMAT(" refererUri: %S",refererUri);
            }
        else
            {
            User::Leave( retVal );
            }
        // END: Get attributes for generic params

        //Creating generic param list
        CAiwGenericParamList* genericParamList = CAiwGenericParamList::NewLC();

        TAiwVariant iapVariant( iap );
        TAiwGenericParam genericParamIap( EGenericParamAccessPoint, iapVariant );
        genericParamList->AppendL( genericParamIap );

        if( url->Length() )
            {
            TAiwVariant urlVariant( *url );
            TAiwGenericParam genericParamUrl( EGenericParamURL, urlVariant );
            genericParamList->AppendL( genericParamUrl );
            }
        if( charset->Length() )
            {
            TAiwVariant charSetVariant( *charset );
            TAiwGenericParam genericParamCharSet
                ( EGenericParamCharSet, charSetVariant );
            genericParamList->AppendL( genericParamCharSet );
            }
        if( refererUri->Length() )
            {
            TAiwVariant refUriVariant( *refererUri );
            TAiwGenericParam genericParamRefUri
                ( EGenericParamReferringURI, refUriVariant );
            genericParamList->AppendL( genericParamRefUri );
            }
        if( fileName->Length() )
            {
            TAiwVariant fileNameVariant( *fileName );
            TAiwGenericParam genericParamFileName
                ( EGenericParamFile, fileNameVariant );
            genericParamList->AppendL( genericParamFileName );
            }

        CLOG_WRITE_FORMAT(" iDocHandler: %x",iDocHandler);
    	delete iDocHandler;
    	iDocHandler = 0;
        iDocHandler = CDocumentHandler::NewL();
        iDocHandler->SetExitObserver( this );

        RFile file;
        iDocHandler->OpenTempFileL( *fileName, file );
        CleanupClosePushL( file );

        TDataType dataType( *contentType );
        TInt docErr( KErrNone );
        TInt trappedError( KErrNone );
        iDocHandlerUsesTheUi = ETrue;
        TRAP( trappedError,
              docErr = iDocHandler->OpenFileEmbeddedL( file,
                                                       dataType,
                                                       *genericParamList ) );
        CLOG_WRITE_FORMAT(" trappedError: %d",trappedError);
        CLOG_WRITE_FORMAT(" docErr: %d",docErr);
        CleanupStack::PopAndDestroy( &file ); // file
        CLOG_WRITE(" PopAndDestroy file OK");

        if ( trappedError || docErr )
            {
            delete iDocHandler;
    	    iDocHandler = 0;
    	    iDocHandlerUsesTheUi = EFalse;
    	    // Since we know this is an open file error, we should handle it by calling
    	    // ShowErrorNotesL instead of propagating the error
            if ( trappedError )
                {
                ShowErrorNoteL (aDownload, trappedError );
                }
            else
                {
                ShowErrorNoteL (aDownload, docErr );
                }
            }

        CleanupStack::PopAndDestroy( genericParamList ); // genericParamList
        CleanupStack::PopAndDestroy( refererUri ); // refererUri
        CleanupStack::PopAndDestroy( charset ); // charset
        CleanupStack::PopAndDestroy( fileName ); // fileName
        CleanupStack::PopAndDestroy( url ); // url
        }

    CLOG_LEAVEFN("CUserInteractionsUtils::HandleContentL");
    }

// -----------------------------------------------------------------------------
// CUserInteractionsUtils::SaveContentL
// -----------------------------------------------------------------------------
//
TBool CUserInteractionsUtils::SaveContentL( RHttpDownload& aDownload )
    {
    CLOG_ENTERFN("CUserInteractionsUtils::SaveContentL");

    __ASSERT_DEBUG( iMemSelectionDialog == 0, Panic( EUiLibPanNull ) ); // Only one can run

    TBool boolRet( EFalse );

    if ( !DrmDownloadL( aDownload ) )
        {
        boolRet = SaveContentWithPathSelectionL( aDownload );
        }
    else
        {
        // DRM download, but...
        if ( !IsContentTypeSupportedL( aDownload ) )
            {
            boolRet = SaveContentWithPathSelectionL( aDownload );
            }
        else
            {
            // We can use DocHandler's MoveL
            HBufC* fileName = HBufC::NewLC( KMaxPath );
            TPtr fileNamePtr = fileName->Des();
            User::LeaveIfError
                ( aDownload.GetStringAttribute( EDlAttrDestFilename, fileNamePtr ) );
            CLOG_WRITE_FORMAT(" EDlAttrDestFilename: %S",&fileNamePtr);

            HBufC* dlName = HBufC::NewLC( KMaxPath );
            TPtr dlNamePtr = dlName->Des();
            User::LeaveIfError
                ( aDownload.GetStringAttribute( EDlAttrName, dlNamePtr ) );
            CLOG_WRITE_FORMAT(" EDlAttrName: %S",&dlNamePtr);

            HBufC8* contentType = HBufC8::NewLC( KMaxContentTypeLength );
            TPtr8 contentTypePtr = contentType->Des();
            User::LeaveIfError
                ( aDownload.GetStringAttribute( EDlAttrContentType, contentTypePtr ) );
            CLOG_WRITE(" EDlAttrContentType OK");
            TDataType dataType( *contentType );

            CLOG_WRITE_FORMAT(" iDocHandler: %x",iDocHandler);
        	delete iDocHandler;
        	iDocHandler = 0;
            iDocHandler = CDocumentHandler::NewL();
            CLOG_WRITE(" DocHandler constructed");

            TInt moveRet(0);
            iDocHandlerUsesTheUi = ETrue;
            // MoveL is synchronous
            TRAPD( moveErr, moveRet = iDocHandler->MoveL( *fileName,
                                                *dlName,
                                                dataType,
                                                KEntryAttNormal ) );
            CLOG_WRITE_FORMAT(" moveErr: %d",moveErr);
            CLOG_WRITE_FORMAT(" moveRet: %d",moveRet);

            // Delete DocHandler, otherwise IsUiBusy returns ETrue.
            delete iDocHandler;
            iDocHandler = 0;
            iDocHandlerUsesTheUi = EFalse;
            CleanupStack::PopAndDestroy( contentType ); // contentType
            contentType = 0;
            CleanupStack::PopAndDestroy( dlName ); // dlName
            dlName = 0;
            CleanupStack::PopAndDestroy( fileName ); // fileName
            fileName = 0;

            if( moveRet == KUserCancel )
                {
                // The user cancelled the operation.
                }
            else if( moveErr == KErrNone )
                {
                boolRet = ETrue;
                // The download can be deleted
                User::LeaveIfError( aDownload.Delete() );
                CLOG_WRITE(" Delete OK");
                }
            else if( moveErr == KDRMErrPreviewRights )
                {
                // Launch from the download directory
                }
            else
                {
                // Leave with the error
                User::Leave( moveErr );
                }
            }
        }

    CLOG_LEAVEFN("CUserInteractionsUtils::SaveContentL");
    return boolRet;
    }

// -----------------------------------------------------------------------------
// CUserInteractionsUtils::SaveContentWithPathSelectionL
// -----------------------------------------------------------------------------
//
TBool CUserInteractionsUtils::SaveContentWithPathSelectionL
    ( RHttpDownload& aDownload )
    {
    CLOG_ENTERFN("CUserInteractionsUtils::SaveContentWithPathSelectionL");

    TBool boolRet( EFalse );

    // Select memory
    iMemSelectionDialog = CAknMemorySelectionDialog::NewL
        ( ECFDDialogTypeSave, R_DMUL_MEMORY_SELECTION_DLG,
          /*aShowUnavailableDrives*/EFalse );
    CLOG_WRITE(" CAknMemorySelectionDialog::NewL OK");
    CAknMemorySelectionDialog::TMemory mem( CAknMemorySelectionDialog::EPhoneMemory );
    boolRet = iMemSelectionDialog->ExecuteL( mem );
    CLOG_WRITE_FORMAT(" MEM boolRet: %d",boolRet);
    delete iMemSelectionDialog;
    iMemSelectionDialog = 0;

    if ( boolRet )
        {
        CLOG_WRITE_FORMAT(" mem: %d",(TInt)mem);
        TBool useMmc = (mem == CAknMemorySelectionDialog::EMemoryCard);

        __ASSERT_DEBUG( iFileSelectionDialog == 0, Panic( EUiLibPanNull ) ); // Only one can run

        HBufC* fileName = HBufC::NewLC( KMaxPath );
        TPtr fileNamePtr = fileName->Des();
        User::LeaveIfError
            ( aDownload.GetStringAttribute( EDlAttrName, fileNamePtr ) );
        CLOG_WRITE(" EDlAttrName OK");

        // Select folder
        TInt resourceId = useMmc ? R_DMUL_MMC_FILE_SELECTION_DLG :
                                   R_DMUL_FILE_SELECTION_DLG;
        iFileSelectionDialog = CAknFileSelectionDialog::NewL
                             ( ECFDDialogTypeSave, resourceId );
        CLOG_WRITE(" CAknFileSelectionDialog::NewL OK");
        TPath selectedPath( KNullDesC );
        // Execute dialog - it calls CActiveScheduler::Start(), so be careful
        // when using data members after ExecuteLD!
        // TODO introduce deleted, as it uses data after executeld.
        boolRet = iFileSelectionDialog->ExecuteL( selectedPath );
        CLOG_WRITE_FORMAT(" FILE boolRet: %d",boolRet);
        delete iFileSelectionDialog;
        iFileSelectionDialog = 0;

        if ( boolRet )
            {
            // Folder selected. Move file there.
            fileNamePtr.Insert( 0, selectedPath );
            CLOG_WRITE_FORMAT(" fileName before EnsureUniqueFileNameL: %S", fileName);
            TPtr fname = fileName->Des();
            EnsureUniqueFileNameL( fname );
            CLOG_WRITE_FORMAT(" fileName after EnsureUniqueFileNameL: %S", fileName);
            User::LeaveIfError
                ( aDownload.SetStringAttribute( EDlAttrDestFilename, *fileName ) );
            CLOG_WRITE(" EDlAttrDestFilename OK");
            User::LeaveIfError( aDownload.Move() );
            CLOG_WRITE(" Move OK");
            }

        CleanupStack::PopAndDestroy( fileName ); // fileName
        }

    CLOG_LEAVEFN("CUserInteractionsUtils::SaveContentWithPathSelectionL");
    return boolRet;
    }

// -----------------------------------------------------------------------------
// CUserInteractionsUtils::EnsureUniqueFileNameL
// -----------------------------------------------------------------------------
//
void CUserInteractionsUtils::EnsureUniqueFileNameL( TPtr& aFileName )
    {
    TInt counter( 0 );
	while ( IsDuplicateL( aFileName ) )
	    {
        if ( !GenerateNewNameL( aFileName, counter ) )
            {
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CUserInteractionsUtils::GenerateNewNameL
// -----------------------------------------------------------------------------
//
TBool CUserInteractionsUtils::GenerateNewNameL( TPtr& aFileName, TInt& aCounter )
    {
    HBufC* original = aFileName.AllocLC();
    TParsePtrC parse( *original );
    HBufC* nameBuf = HBufC::NewLC( KMaxFileName );
    TPtr name = nameBuf->Des();
    name = parse.Name();
    TInt counter( aCounter );
    TBool found( EFalse );
    TInt len = name.Length();
    TInt i = len - 1;
    TBool ret( ETrue );

    // Search starting from the end
    for ( ; i >= 0; --i )
        {
        if ( name[i] == iSearchChar )
            {
            found = ETrue;
            break;
            }
        }
    if ( aCounter == 0 && found && i > 0
            && i <= len - iUnderTenFormatter->Length() )
        {
        // Existing counter, parse the count
        TPtrC oldCounter = name.Right( len - i );
        TLex lex( oldCounter );
        if ( lex.Val( counter ) )
            {
            counter = 0;
            }
        aCounter = counter;
        }
    else if ( !found )
        {
        // No counter found
        i = len;
        }
    ++counter;
    ++aCounter;
    if ( counter > FilenameSuffixMaxValue )
        {
        // Count too high, can't generate new name
        ret = EFalse;
        }
    else
        {
        TBuf<FilenameSuffixMaxLength> number;
        HBufC* formatter = iUnderTenFormatter;
        if ( counter >= 10 )
            {
            formatter = iOverTenFormatter;
            }
        StringLoader::Format( number, *formatter, -1, counter );
        // Check for length, the full path must not get too long
        TPtrC driveAndPath = parse.DriveAndPath();
        TPtrC ext = parse.Ext();
        TInt pathLen = driveAndPath.Length() + i + number.Length()
                        + ext.Length();
        TInt maxLen = aFileName.MaxLength();
        if ( pathLen > maxLen )
            {
            i -= pathLen - maxLen;
            }
        if ( i < 1 )
            {
            // At least one character from the old name must be included
            ret = EFalse;
            }
        else
            {
            name.SetLength( i );
            name.Append( number );

            aFileName.Zero();
            aFileName.Append( driveAndPath );
            aFileName.Append( name );
            aFileName.Append( ext );
            }
        }
    CleanupStack::PopAndDestroy( 2, original ); // nameBuf, original
    return ret;
    }

// -----------------------------------------------------------------------------
// CUserInteractionsUtils::IsDuplicateL
// -----------------------------------------------------------------------------
//
TBool CUserInteractionsUtils::IsDuplicateL( const TPtrC& aFileName )
    {
	TBool result( EFalse );
	RFs rfs;
    User::LeaveIfError( rfs.Connect() );
    CleanupClosePushL<RFs>( rfs );
	result = BaflUtils::FileExists( rfs, aFileName );
    CleanupStack::PopAndDestroy( &rfs );
	return result;
    }

// -----------------------------------------------------------------------------
// CUserInteractionsUtils::CancelMemorySelectionDialog
// -----------------------------------------------------------------------------
//
void CUserInteractionsUtils::CancelMemorySelectionDialog()
    {
    CLOG_ENTERFN("CUserInteractionsUtils::CancelMemorySelectionDialog");
    if ( iMemSelectionDialog )
        {
        delete iMemSelectionDialog;
        iMemSelectionDialog = 0;
        }
    CLOG_LEAVEFN("CUserInteractionsUtils::CancelMemorySelectionDialog");
    }

// -----------------------------------------------------------------------------
// CUserInteractionsUtils::CancelFileSelectionDialog
// -----------------------------------------------------------------------------
//
void CUserInteractionsUtils::CancelFileSelectionDialog()
    {
    CLOG_ENTERFN("CUserInteractionsUtils::CancelFileSelectionDialog");
    if ( iFileSelectionDialog )
        {
        delete iFileSelectionDialog;
        iFileSelectionDialog = 0;
        }
    CLOG_LEAVEFN("CUserInteractionsUtils::CancelFileSelectionDialog");
    }

// -----------------------------------------------------------------------------
// CUserInteractionsUtils::DeleteWithUserConfirmL
// -----------------------------------------------------------------------------
//
TBool CUserInteractionsUtils::DeleteWithUserConfirmL( RHttpDownload& aDownload )
    {
    CLOG_ENTERFN("CUserInteractionsUtils::DeleteWithUserConfirmL");

    TInt32 numMediaObjects(0);
    aDownload.GetIntAttribute( EDlAttrNumMediaObjects, numMediaObjects );

    HBufC* fileName = HBufC::NewLC( KMaxPath );
    TPtr fileNamePtr = fileName->Des();

    // In case of album, show the album name. Otherwise show file name.
    TInt err(KErrNone);
    if (numMediaObjects > 1)
        {
        err = aDownload.GetStringAttribute( EDlAttrAlbumName, fileNamePtr );
        }
    else
        {
        err = aDownload.GetStringAttribute( EDlAttrName, fileNamePtr );
        }

    if ( err != KErrNone && err != KErrNotFound )
        {
        User::LeaveIfError( err );
        }
    TBool fileNameSet = ( err == KErrNone );
    if ( !fileNameSet )
        {
        fileNamePtr.Copy( _L("Filename unknown") );//TODO what to write out?
        }

    HBufC* prompt = StringLoader::LoadLC( R_DMUL_DOWNLOAD_DEL_CONF, *fileName );
    CAknQueryDialog* dlg = CAknQueryDialog::NewL();
    delete iDeleteConfirmDlg;
    iDeleteConfirmDlg = dlg;
    // Execute dialog - it calls CActiveScheduler::Start(), so be careful
    // when using data members after ExecuteLD!
    TInt resp = iDeleteConfirmDlg->ExecuteLD
                    ( R_DMUL_DOWNLOAD_YESNO_CONF_Q, *prompt );
    iDeleteConfirmDlg = 0;
    CleanupStack::PopAndDestroy( 2, fileName ); // prompt, fileName

    if ( resp ) // We have to check only that the value is non-zero
        {

        aDownload.GetBoolAttribute( EDlAttrCodDownload,iCodDownload );


        if(iCodDownload)
			{

			#ifdef BRDO_APP_GALLERY_SUPPORTED_FF
			CMGXFileManager* mgFileManager = MGXFileManagerFactory::NewFileManagerL(
	        CEikonEnv::Static()->FsSession() );

	        #endif

	        //Assume that all files to be deleted are valid.
	        TBool fileNotFound = EFalse;

			for(TInt i = 1; i <= numMediaObjects; i++)
		    	{
                HBufC* fileName = HBufC::NewLC( KMaxPath );
                TPtr fileNamePtr = fileName->Des();
                User::LeaveIfError( aDownload.GetStringAttribute( EDlAttrDestFilename,i,fileNamePtr ));
                RFs rfs;
                User::LeaveIfError( rfs.Connect() );
                CleanupClosePushL<RFs>( rfs );
                TFindFile file(rfs);
                TPtrC ptr(KNullDesC);
                TInt found = file.FindByPath(fileNamePtr,&ptr); //when second parameter to the API is Null then the it searches for the file in the Dir specified in the first parameter
                if(found == KErrNotFound)
                    {
                    //Found out that at least one file is not found
                    fileNotFound =ETrue;
                    }

                rfs.Delete( fileNamePtr );

                #ifdef BRDO_APP_GALLERY_SUPPORTED_FF
                // Notify Media Gallery about new media file
                if( fileNamePtr.Length() > 0 )
                   {
                   TRAP_IGNORE( mgFileManager->UpdateL( fileNamePtr ) );
                   TRAP_IGNORE( UpdateDCFRepositoryL( fileNamePtr ) );
                   }
                else
                   {
                   TRAP_IGNORE( mgFileManager->UpdateL() );
                   }

                 #else
                    if( fileNamePtr.Length() > 0 )
                   {
                   TRAP_IGNORE( UpdateDCFRepositoryL( fileNamePtr ) );
                   }

                 #endif

                CleanupStack::PopAndDestroy( &rfs );
                CleanupStack::PopAndDestroy( fileName );
                }

                #ifdef BRDO_APP_GALLERY_SUPPORTED_FF
                delete mgFileManager;
                mgFileManager = NULL;
                #endif

                //Inform the user that atleast one file not found.
	   		    if(fileNotFound)
	   		        {
                    HBufC* infoPrompt = StringLoader::LoadLC( R_DMUL_ERROR_FILE_NOT_FOUND);
                    CAknInformationNote* note = new(ELeave)  CAknInformationNote();
                    note->ExecuteLD(*infoPrompt);
                    CleanupStack::PopAndDestroy(infoPrompt);
	   		        }
			}

        User::LeaveIfError( aDownload.Delete() );
        CLOG_LEAVEFN("CUserInteractionsUtils::DeleteWithUserConfirmL");
        return ETrue; // Deleted
        }
    else
        {
        // Do nothing.
        CLOG_LEAVEFN("CUserInteractionsUtils::DeleteWithUserConfirmL");
        return EFalse;
        }
    }

// -----------------------------------------------------------------------------
// CUserInteractionsUtils::CancelDeleteConfirmationDialog
// -----------------------------------------------------------------------------
//
void CUserInteractionsUtils::CancelDeleteConfirmationDialog()
    {
    CLOG_ENTERFN("CUserInteractionsUtils::CancelDeleteConfirmationDialog");
    delete iDeleteConfirmDlg;
    iDeleteConfirmDlg = 0;
    CLOG_LEAVEFN("CUserInteractionsUtils::CancelDeleteConfirmationDialog");
    }

// -----------------------------------------------------------------------------
// CUserInteractionsUtils::CancelWithUserConfirmL
// -----------------------------------------------------------------------------
//
TBool CUserInteractionsUtils::CancelWithUserConfirmL( RHttpDownload& aDownload )
    {
    CLOG_ENTERFN("CUserInteractionsUtils::CancelWithUserConfirmL");

    HBufC* fileName = HBufC::NewLC( KMaxPath );
    TPtr fileNamePtr = fileName->Des();
    TInt err = aDownload.GetStringAttribute( EDlAttrName, fileNamePtr );
    if ( err != KErrNone && err != KErrNotFound )
        {
        User::LeaveIfError( err );
        }
    TBool fileNameSet = ( err == KErrNone );
    if ( !fileNameSet )
        {
        fileNamePtr.Copy( _L("Filename unknown") );//TODO what to write out?
        }

    HBufC* prompt = StringLoader::LoadLC( R_DMUL_DOWNLOAD_CAN_CONF, *fileName );
    CAknQueryDialog* dlg = CAknQueryDialog::NewL();
    delete iCancelConfirmDlg;
    iCancelConfirmDlg = dlg;
    // Execute dialog - it calls CActiveScheduler::Start(), so be careful
    // when using data members after ExecuteLD!
    TInt resp = iCancelConfirmDlg->ExecuteLD
                    ( R_DMUL_DOWNLOAD_YESNO_CONF_Q, *prompt );
    iCancelConfirmDlg = 0;
    CleanupStack::PopAndDestroy( 2, fileName ); // prompt, fileName

    if ( resp ) // We have to check only that the value is non-zero
        {
        User::LeaveIfError( aDownload.Delete() );
        CLOG_LEAVEFN("CUserInteractionsUtils::CancelWithUserConfirmL");
        return ETrue; // Cancelled
        }
    else
        {
        // Do nothing.
        CLOG_LEAVEFN("CUserInteractionsUtils::CancelWithUserConfirmL");
        return EFalse;
        }
    }

// -----------------------------------------------------------------------------
// CUserInteractionsUtils::CancelAllWithUserConfirmL
// -----------------------------------------------------------------------------
//
TBool CUserInteractionsUtils::CancelAllWithUserConfirmL( RHttpDownload& /* aDownload */)
    {
    CLOG_ENTERFN("CUserInteractionsUtils::CancelAllWithUserConfirmL");

    HBufC* prompt = StringLoader::LoadLC( R_DMUL_EXIT_CONF_PLUR );
    CAknQueryDialog* dlg = CAknQueryDialog::NewL();
    delete iCancelConfirmDlg;
    iCancelConfirmDlg = dlg;
    // Execute dialog - it calls CActiveScheduler::Start(), so be careful
    // when using data members after ExecuteLD!
    TInt resp = iCancelConfirmDlg->ExecuteLD
                    ( R_DMUL_DOWNLOAD_YESNO_CONF_Q, *prompt );
    iCancelConfirmDlg = 0;
    CleanupStack::PopAndDestroy(); // prompt

    if ( resp ) // We have to check only that the value is non-zero
        {
        TInt count = iRegistryModel.DownloadMgr().CurrentDownloads().Count();
        TInt32 dlState;
        TInt i = 0;
        while((i < count)&& (count>0))
        	{
        	(iRegistryModel.DownloadMgr().CurrentDownloads().At(i))->GetIntAttribute(EDlAttrState, dlState);
        	i++;
        	//add fix for the bug  JERI-7P8CF2, if checking against EHttpDlMultipleMOCompleted EHttpDlMultipleMOFailed
             //Changes for the bug JERI-7P8CF2
            //Changes made in the server side to fix for the video center receiving unexpected events
            //Reassigning these events back to the changes done in server side
            if(dlState  == EHttpDlCompleted )
		       {
                dlState  = EHttpDlMultipleMOCompleted;
		       }
            else if(dlState  == EHttpDlFailed )
		       {
		       dlState  = EHttpDlMultipleMOFailed;
		       }

            if(dlState == EHttpDlInprogress || dlState == EHttpDlPaused )
        	    {
        	    i--;
        	    count--;
        	    User::LeaveIfError( (iRegistryModel.DownloadMgr().CurrentDownloads().At(i))->Delete());
        	    }
        	}
        CLOG_LEAVEFN("CUserInteractionsUtils::CancelAllWithUserConfirmL");
        return ETrue; // Cancelled
        }
    else
        {
        // Do nothing.
        CLOG_LEAVEFN("CUserInteractionsUtils::CancelAllWithUserConfirmL");
        return EFalse;
        }
    }

// -----------------------------------------------------------------------------
// CUserInteractionsUtils::CancelCancelConfirmationDialog
// -----------------------------------------------------------------------------
//
void CUserInteractionsUtils::CancelCancelConfirmationDialog()
    {
    CLOG_ENTERFN("CUserInteractionsUtils::CancelCancelConfirmationDialog");
    delete iCancelConfirmDlg;
    iCancelConfirmDlg = 0;
    CLOG_LEAVEFN("CUserInteractionsUtils::CancelCancelConfirmationDialog");
    }

// -----------------------------------------------------------------------------
// CUserInteractionsUtils::ShowMediaRemovedNoteL
// -----------------------------------------------------------------------------
//
void CUserInteractionsUtils::ShowMediaRemovedNoteL()
    {
    CLOG_ENTERFN("CUserInteractionsUtils::ShowMediaRemovedNoteL");
    if ( iMediaRemovedNote == 0 )
        {
        HBufC* value = iCoeEnv.AllocReadResourceLC
                        ( R_DMUL_ERROR_EXT_MEM_REMOVED );
        iMediaRemovedNote =
            new (ELeave) CAknInformationNote( &iMediaRemovedNote );
        iMediaRemovedNote->ExecuteLD( *value );
        CleanupStack::PopAndDestroy( value ); // value
        }
    CLOG_LEAVEFN("CUserInteractionsUtils::ShowMediaRemovedNoteL");
    }

// -----------------------------------------------------------------------------
// CUserInteractionsUtils::CancelMediaRemovedNote
// -----------------------------------------------------------------------------
//
void CUserInteractionsUtils::CancelMediaRemovedNote()
    {
    CLOG_ENTERFN("CUserInteractionsUtils::CancelMediaRemovedNote");
    if ( iMediaRemovedNote != 0 )
        {
        delete iMediaRemovedNote;
        iMediaRemovedNote = 0;
        }
    CLOG_LEAVEFN("CUserInteractionsUtils::CancelMediaRemovedNote");
    }

// -----------------------------------------------------------------------------
// CUserInteractionsUtils::GetAndSetHttpAuthCredentialsL
// -----------------------------------------------------------------------------
//
TBool CUserInteractionsUtils::GetAndSetHttpAuthCredentialsL
    ( RHttpDownload& aDownload )
    {
    CLOG_ENTERFN("CUserInteractionsUtils::GetAndSetHttpAuthCredentialsL");

    HBufC* username = 0;
    HBufC* password = 0;

    // Get the authentication scheme
    TInt32 authScheme(0);
    User::LeaveIfError( aDownload.GetIntAttribute
                        ( EDlAttrAuthScheme, authScheme ) );
    CLOG_WRITE_FORMAT(" EDlAttrAuthScheme: %d",authScheme);

    // Get the default (already set) username and other stuff
    HBufC* userName = HBufC::NewLC( KMaxDefAttrLength );
    TPtr userNamePtr = userName->Des();
    TInt getErr = aDownload.GetStringAttribute
                    ( EDlAttrUsername, userNamePtr );
    if ( getErr != KErrNone && getErr != KErrNotFound )
        { // KErrNotFound is also allowed. In this case it has not been set.
        User::LeaveIfError( getErr );
        }
    HBufC* url = HBufC::NewLC( KMaxUrlLength );
    TPtr urlPtr = url->Des();
    User::LeaveIfError( aDownload.GetStringAttribute
                        ( EDlAttrReqUrl, urlPtr ) );
    // URL must be set!
    HBufC* realm = HBufC::NewLC( KMaxRealmLength );
    TPtr realmPtr = realm->Des();
    getErr = aDownload.GetStringAttribute( EDlAttrRealm, realmPtr );
    if ( getErr != KErrNone && getErr != KErrNotFound )
        { // KErrNotFound is also allowed. In this case it has not been set.
        User::LeaveIfError( getErr );
        }

    // Execute a wait dialog
    TBool resp = iDialogsProv->DialogUserAuthenticationLC
            ( *url, *realm, *userName, username, password,
              authScheme == EAuthBasic );

    if ( resp )
        {
        if ( authScheme == EAuthBasic && BrowserSettingSecWarningL() )
            {
            resp = DisplaySecurityWarningL();
            }
        if ( resp )
            {
            // Set credentials...
            User::LeaveIfError( aDownload.SetStringAttribute
                        ( EDlAttrUsername, *username ) );
            User::LeaveIfError( aDownload.SetStringAttribute
                        ( EDlAttrPassword, *password ) );
            }
        }

    CleanupStack::PopAndDestroy( 2 ); // username, password
    CleanupStack::PopAndDestroy( realm ); // realm
    CleanupStack::PopAndDestroy( url ); // url
    CleanupStack::PopAndDestroy( userName ); // userName

    CLOG_LEAVEFN("CUserInteractionsUtils::GetAndSetHttpAuthCredentialsL");
    return ( resp );
    }

// -----------------------------------------------------------------------------
// CUserInteractionsUtils::GetAndSetProxyAuthCredentialsL
// -----------------------------------------------------------------------------
//
TBool CUserInteractionsUtils::GetAndSetProxyAuthCredentialsL
    ( RHttpDownload& aDownload )
    {
    CLOG_ENTERFN("CUserInteractionsUtils::GetAndSetProxyAuthCredentialsL");

    HBufC* username = 0;
    HBufC* password = 0;

    // Get the authentication scheme
    TInt32 authScheme(0);
    User::LeaveIfError( aDownload.GetIntAttribute
                        ( EDlAttrAuthScheme, authScheme ) );
    CLOG_WRITE_FORMAT(" EDlAttrAuthScheme: %d",authScheme);

    // Get the default (already set) username and other stuff
    HBufC* userName = HBufC::NewLC( KMaxDefAttrLength );
    TPtr userNamePtr = userName->Des();
    User::LeaveIfError( aDownload.GetStringAttribute
                ( EDlAttrProxyUsername, userNamePtr ) );
    HBufC* url = HBufC::NewLC( KMaxUrlLength );
    TPtr urlPtr = url->Des();
    User::LeaveIfError( aDownload.GetStringAttribute
                ( EDlAttrReqUrl, urlPtr ) );
    HBufC* realm = HBufC::NewLC( KMaxRealmLength );
    TPtr realmPtr = realm->Des();
    User::LeaveIfError( aDownload.GetStringAttribute
                ( EDlAttrProxyRealm, realmPtr ) );

    // Execute a wait dialog
    TBool resp = iDialogsProv->DialogUserAuthenticationLC
            ( *url, *realm, *userName, username, password,
              authScheme == EAuthBasic );

    if ( resp )
        {
        if ( authScheme == EAuthBasic && BrowserSettingSecWarningL() )
            {
            resp = DisplaySecurityWarningL();
            }
        if ( resp )
            {
            // Set credentials...
            User::LeaveIfError( aDownload.SetStringAttribute
                        ( EDlAttrProxyUsername, *username ) );
            User::LeaveIfError( aDownload.SetStringAttribute
                        ( EDlAttrProxyPassword, *password ) );
            }
        }

    CleanupStack::PopAndDestroy( 2 ); // username, password
    CleanupStack::PopAndDestroy( realm ); // realm
    CleanupStack::PopAndDestroy( url ); // url
    CleanupStack::PopAndDestroy( userName ); // userName

    CLOG_LEAVEFN("CUserInteractionsUtils::GetAndSetProxyAuthCredentialsL");
    return ( resp );
    }

// -----------------------------------------------------------------------------
// CUserInteractionsUtils::CancelCredentialsQueries
// -----------------------------------------------------------------------------
//
void CUserInteractionsUtils::CancelCredentialsQueries()
    {
    if ( iDialogsProv )
        {
        iDialogsProv->CancelAll();
        }
    }

// -----------------------------------------------------------------------------
// CUserInteractionsUtils::DisplaySecurityWarningL
// -----------------------------------------------------------------------------
//
TBool CUserInteractionsUtils::DisplaySecurityWarningL()
    {
    TBool resp = EFalse;

    // Set up the message text
    HBufC* message = StringLoader::LoadLC
                     ( R_DMUL_QUERY_BASIC_AUTHENTICATION );
    HBufC* okMsg = StringLoader::LoadLC( R_DMUL_OK_BUTTON );
    HBufC* cancelMsg = StringLoader::LoadLC( R_DMUL_CANCEL_BUTTON );

    resp = iDialogsProv->DialogConfirmL
           ( KNullDesC, *message, *okMsg, *cancelMsg );

    // Clean up the basic authentication dialog memory
    CleanupStack::PopAndDestroy( 3, message );
        // message, okMsg, cancelMsg

    return resp;
    }

// -----------------------------------------------------------------------------
// CUserInteractionsUtils::CancelSecurityWarning
// -----------------------------------------------------------------------------
//
void CUserInteractionsUtils::CancelSecurityWarning()
    {
    if ( iDialogsProv )
        {
        iDialogsProv->CancelAll();
        }
    }

// -----------------------------------------------------------------------------
// CUserInteractionsUtils::BrowserSettingSecWarningL
// This method must behave the same as
// CBrowserSettings::GetHttpSecurityWarnings().
// See CBrowserSettings !
// -----------------------------------------------------------------------------
//
TBool CUserInteractionsUtils::BrowserSettingSecWarningL()
    {
    //todo
    return 1;
    }

// -----------------------------------------------------------------------------
// CUserInteractionsUtils::InitializeSoftNotifStndL
// -----------------------------------------------------------------------------
//
void CUserInteractionsUtils::InitializeSoftNotifStndL( TVwsViewId aViewId,
                                                       TUid aCustomMessageId,
                                                       const TDesC8& aViewActivationMsg ) const
    {
    CLOG_ENTERFN("CUserInteractionsUtils::InitializeSoftNotifStndL");

    CAknSoftNotificationParameters* params =
        CAknSoftNotificationParameters::NewL
        (
        iDMgrUiBase.ResourceFileName(), R_DMUL_GSN_STND, KMinimumSoftNotePriority,
        R_AVKON_SOFTKEYS_YES_NO__YES,
        CAknNoteDialog::EConfirmationTone, aViewId, aCustomMessageId,
        EAknSoftkeyYes, aViewActivationMsg
        );
    CLOG_WRITE(" params OK");
    CleanupStack::PushL( params );

    iSoftNotifier->AddCustomNotificationL( *params );

    CleanupStack::PopAndDestroy( params ); // params

    CLOG_LEAVEFN("CUserInteractionsUtils::InitializeSoftNotifStndL");
    }

// -----------------------------------------------------------------------------
// CUserInteractionsUtils::InitializeSoftNotifEmbL
// -----------------------------------------------------------------------------
//
void CUserInteractionsUtils::InitializeSoftNotifEmbL( TVwsViewId aViewId,
                                                      TUid aCustomMessageId,
                                                      const TDesC8& aViewActivationMsg ) const
    {
    CLOG_ENTERFN("CUserInteractionsUtils::InitializeSoftNotifEmbL");

    CAknSoftNotificationParameters* params =
        CAknSoftNotificationParameters::NewL
        (
        iDMgrUiBase.ResourceFileName(), R_DMUL_GSN_EMB, KMinimumSoftNotePriority,
        R_AVKON_SOFTKEYS_YES_NO__YES,
        CAknNoteDialog::EConfirmationTone, aViewId, aCustomMessageId,
        EAknSoftkeyYes, aViewActivationMsg
        );
    CLOG_WRITE(" params OK");
    CleanupStack::PushL( params );

    iSoftNotifier->AddCustomNotificationL( *params );

    CleanupStack::PopAndDestroy( params ); // params

    CLOG_LEAVEFN("CUserInteractionsUtils::InitializeSoftNotifEmbL");
    }

// -----------------------------------------------------------------------------
// CUserInteractionsUtils::CancelSoftNotifStndL
// -----------------------------------------------------------------------------
//
void CUserInteractionsUtils::CancelSoftNotifStndL( TVwsViewId aViewId,
                                                  TUid aCustomMessageId,
                                                  const TDesC8& aViewActivationMsg ) const
    {
    CLOG_ENTERFN("CUserInteractionsUtils::CancelSoftNotifStndL");

    CAknSoftNotificationParameters* params =
        CAknSoftNotificationParameters::NewL
        (
        iDMgrUiBase.ResourceFileName(), R_DMUL_GSN_STND, KMinimumSoftNotePriority,
        R_AVKON_SOFTKEYS_YES_NO__YES,
        CAknNoteDialog::EConfirmationTone, aViewId, aCustomMessageId,
        EAknSoftkeyYes, aViewActivationMsg
        );
    CLOG_WRITE(" params OK");
    CleanupStack::PushL( params );

    //TODO From avkon sources it seems that it identifies the soft notification
    // with the AVKON_NOTE resource ID, which is the same in case of
    // all UI Lib clients!!
    iSoftNotifier->CancelCustomSoftNotificationL( *params );

    CleanupStack::PopAndDestroy( params ); // params

    CLOG_LEAVEFN("CUserInteractionsUtils::CancelSoftNotifStndL");
    }

// -----------------------------------------------------------------------------
// CUserInteractionsUtils::CancelSoftNotifEmbL
// -----------------------------------------------------------------------------
//
void CUserInteractionsUtils::CancelSoftNotifEmbL( TVwsViewId aViewId,
                                                 TUid aCustomMessageId,
                                                 const TDesC8& aViewActivationMsg ) const
    {
    CLOG_ENTERFN("CUserInteractionsUtils::CancelSoftNotifEmbL");

    CAknSoftNotificationParameters* params =
        CAknSoftNotificationParameters::NewL
        (
        iDMgrUiBase.ResourceFileName(), R_DMUL_GSN_EMB, KMinimumSoftNotePriority,
        R_AVKON_SOFTKEYS_YES_NO__YES,
        CAknNoteDialog::EConfirmationTone, aViewId, aCustomMessageId,
        EAknSoftkeyYes, aViewActivationMsg
        );
    CLOG_WRITE(" params OK");
    CleanupStack::PushL( params );

    iSoftNotifier->CancelCustomSoftNotificationL( *params );

    CleanupStack::PopAndDestroy( params ); // params

    CLOG_LEAVEFN("CUserInteractionsUtils::CancelSoftNotifEmbL");
    }

// -----------------------------------------------------------------------------
// CUserInteractionsUtils::CancelAllDialogs
// -----------------------------------------------------------------------------
//
void CUserInteractionsUtils::CancelAllDialogs()
    {
    if ( iDialogsProv )
        {
        iDialogsProv->CancelAll();
        delete iDialogsProv;
        iDialogsProv = 0;
        }
    CancelMemorySelectionDialog();
    CancelFileSelectionDialog();
    CancelDeleteConfirmationDialog();
    CancelCancelConfirmationDialog();
    CancelMediaRemovedNote();
    CancelCredentialsQueries();
    CancelSecurityWarning();
    // The soft notifications should not be cancelled here,
    // because they are used in a different context.
    }

// -----------------------------------------------------------------------------
// CUserInteractionsUtils::InfoNoteL
// -----------------------------------------------------------------------------
//
void CUserInteractionsUtils::InfoNoteL( TInt aDialogResourceId, const TInt aPromptResourceId )
    {
    HBufC* prompt = StringLoader::LoadLC( aPromptResourceId );
    InfoNoteL( aDialogResourceId, *prompt );
    CleanupStack::PopAndDestroy();  // prompt
    }

// -----------------------------------------------------------------------------
// CUserInteractionsUtils::InfoNoteL
// -----------------------------------------------------------------------------
//
void CUserInteractionsUtils::InfoNoteL( TInt aDialogResourceId, const TDesC& aPrompt )
    {
    CLOG_ENTERFN("CUserInteractionsUtils::InfoNoteL");

    CAknNoteDialog* dialog = new (ELeave) CAknNoteDialog
								( REINTERPRET_CAST( CEikDialog**, &dialog ) );
	CLOG_WRITE(" new");
    dialog->PrepareLC( aDialogResourceId ) ;
    CLOG_WRITE(" PrepareLC");
    dialog->SetCurrentLabelL( EGeneralNote, aPrompt );
    CLOG_WRITE(" SetCurrentLabelL");
    dialog->RunDlgLD( CAknNoteDialog::ELongTimeout, CAknNoteDialog::ENoTone );

    CLOG_LEAVEFN("CUserInteractionsUtils::InfoNoteL");
    }

// -----------------------------------------------------------------------------
// CUserInteractionsUtils::IsUiBusy
// -----------------------------------------------------------------------------
//
TBool CUserInteractionsUtils::IsUiBusy() const
    {
    CLOG_ENTERFN("CUserInteractionsUtils::IsUiBusy");
    CLOG_WRITE_FORMAT(" MS: %x", iMemSelectionDialog);
    CLOG_WRITE_FORMAT(" FS: %x", iFileSelectionDialog);
    CLOG_LEAVEFN("CUserInteractionsUtils::IsUiBusy");
    return (IsDocHandlerRunning() || iMemSelectionDialog!=0 || iFileSelectionDialog!=0);
    }

// -----------------------------------------------------------------------------
// CUserInteractionsUtils::IsDocHandlerRunning
// -----------------------------------------------------------------------------
//
TBool CUserInteractionsUtils::IsDocHandlerRunning() const
    {
    CLOG_ENTERFN("CUserInteractionsUtils::IsDocHandlerRunning");
    CLOG_WRITE_FORMAT(" %x", iDocHandler);
    CLOG_WRITE_FORMAT(" %d", iDocHandlerUsesTheUi);
    CLOG_LEAVEFN("CUserInteractionsUtils::IsDocHandlerRunning");
    return iDocHandlerUsesTheUi;
    }

// -----------------------------------------------------------------------------
// CUserInteractionsUtils::IsCorruptedDcfL
// -----------------------------------------------------------------------------
//
TBool CUserInteractionsUtils::IsCorruptedDcfL
    ( RHttpDownload& aDownload ) const
    {
    CLOG_ENTERFN("CUserInteractionsUtils::IsCorruptedDcfL");
    TBool isCorruptedDcf( EFalse );

    DRMCommon* drmCommon = DRMCommon::NewL();
    CLOG_WRITE(" DRMCommon::NewL OK");
    CleanupStack::PushL( drmCommon );
    User::LeaveIfError( drmCommon->Connect() );
    CLOG_WRITE(" Connect OK");

    HBufC* fileName = HBufC::NewLC( KMaxPath );
    TPtr fileNamePtr = fileName->Des();
    User::LeaveIfError
        ( aDownload.GetStringAttribute( EDlAttrDestFilename, fileNamePtr ) );
    CLOG_WRITE_FORMAT(" EDlAttrDestFilename: %S",&fileNamePtr);

    DRMCommon::TContentProtection prot;
    HBufC8* mimeType = 0;
    HBufC8* contentURI = 0;
    TUint dataLength = 0;
    TInt err = drmCommon->GetFileInfo( *fileName,
        prot, mimeType, contentURI, dataLength );

    if ( err == DRMCommon::EVersionNotSupported ||
         ( err == DRMCommon::EOk && ( !mimeType || !contentURI ) ) )
        {
        isCorruptedDcf = ETrue;
        }

    delete mimeType;
    delete contentURI;
    CleanupStack::PopAndDestroy( fileName );
    CleanupStack::PopAndDestroy( drmCommon );

    CLOG_WRITE_FORMAT(" ret: %d",isCorruptedDcf);
    CLOG_LEAVEFN("CUserInteractionsUtils::IsCorruptedDcfL");
    return isCorruptedDcf;
    }

// -----------------------------------------------------------------------------
// CUserInteractionsUtils::IsBadMimeInDcfL
// -----------------------------------------------------------------------------
//
TBool CUserInteractionsUtils::IsBadMimeInDcfL
    ( RHttpDownload& aDownload ) const
    {
    CLOG_ENTERFN("CUserInteractionsUtils::IsBadMimeInDcfL");
    TBool isBadMimeInDcfL( EFalse );

    DRMCommon* drmCommon = DRMCommon::NewL();
    CLOG_WRITE(" DRMCommon::NewL OK");
    CleanupStack::PushL( drmCommon );
    User::LeaveIfError( drmCommon->Connect() );
    CLOG_WRITE(" Connect OK");

    HBufC* fileName = HBufC::NewLC( KMaxPath );
    TPtr fileNamePtr = fileName->Des();
    User::LeaveIfError
        ( aDownload.GetStringAttribute( EDlAttrDestFilename, fileNamePtr ) );
    CLOG_WRITE_FORMAT(" EDlAttrDestFilename: %S",&fileNamePtr);

    DRMCommon::TContentProtection prot;
    HBufC8* mimeType = 0;
    HBufC8* contentURI = 0;
    TUint dataLength = 0;
    TInt err = drmCommon->GetFileInfo( *fileName,
        prot, mimeType, contentURI, dataLength );

    if ( err == DRMCommon::EOk )
        {
        if ( !mimeType || !contentURI )
            {
            isBadMimeInDcfL = ETrue;
            }
        else if ( mimeType->CompareF( KOma1DcfContentType ) == KErrNone )
            {
            // The MIME type in the DCF is also DCF, which is wrong.
            // It should contain the real mime type.
            isBadMimeInDcfL = ETrue;
            }
        }

    delete mimeType;
    delete contentURI;
    CleanupStack::PopAndDestroy( fileName );
    CleanupStack::PopAndDestroy( drmCommon );

    CLOG_WRITE_FORMAT(" ret: %d",isBadMimeInDcfL);
    CLOG_LEAVEFN("CUserInteractionsUtils::IsBadMimeInDcfL");
    return isBadMimeInDcfL;
    }

// -----------------------------------------------------------------------------
// CUserInteractionsUtils::DownloadHasBeenDeleted
// -----------------------------------------------------------------------------
//
void CUserInteractionsUtils::DownloadHasBeenDeleted( RHttpDownload* aDownload )
    {
    CLOG_ENTERFN("CUserInteractionsUtils::DownloadHasBeenDeleted");
    if ( iHandledDownload == aDownload )
        {
        iHandledDownload = 0;
        }
    CLOG_LEAVEFN("CUserInteractionsUtils::DownloadHasBeenDeleted");
    }


// -----------------------------------------------------------------------------
// CUserInteractionsUtils::LaunchPdAppL
// -----------------------------------------------------------------------------
//
void CUserInteractionsUtils::LaunchPdAppL( RHttpDownload& aDownload, const TBool aProgressively )
    {
    CLOG_ENTERFN("CUserInteractionsEventHandler::LaunchPdAppL");

    // Get the PD player application's UID for the download
    TUid pdPlayerUid = { 0 };
    HBufC8* contentType = ContentTypeL( aDownload, ETrue, KFirstMoIndex );
    TDataType dataType( *contentType );
    delete contentType;
    contentType = NULL;

    CDocumentHandler* docHandler = CDocumentHandler::NewLC();
    TBool pdSupported = docHandler->CanHandleProgressivelyL( dataType, pdPlayerUid );
    CleanupStack::PopAndDestroy( docHandler ); // docHandler

    if ( pdSupported )
        {

        TInt downloadCnt = iRegistryModel.DownloadCount();
        TBool isProg(EFalse);

        const CDownloadArray& downloads = iRegistryModel.DownloadMgr().CurrentDownloads();

        for ( TInt i = 0; i < downloadCnt ;i++ )
			{
        	RHttpDownload* dl = downloads.At(i); //current download
        	dl->GetBoolAttribute( EDlAttrProgressive, isProg );
        	if (isProg )
				{
                dl->SetBoolAttribute( EDlAttrProgressive, EFalse );
                }
			}

        TInt32 numMediaObjects = 0;
        User::LeaveIfError( aDownload.GetIntAttribute( EDlAttrNumMediaObjects, numMediaObjects ) );

        // Pack download ID into EGenericParamDownloadId.
        TInt32 downloadID( -1 );
        TInt32 activeDownloadID(-1);
        if ( aProgressively )
            {
            User::LeaveIfError( aDownload.GetIntAttribute( EDlAttrId, downloadID ) );
            User::LeaveIfError( aDownload.GetIntAttribute( EDlAttrActiveDownload, activeDownloadID ) );
            }


        CAiwGenericParamList* genParList = CAiwGenericParamList::NewLC();
        TAiwVariant downloadIdVariant( downloadID );
        TAiwGenericParam genericParam( EGenericParamDownloadId, downloadIdVariant );
        genParList->AppendL( genericParam );

		// Pack fileName into EGenericParamFile
		HBufC* fileName = HBufC::NewLC( KMaxPath );
		TPtr fileNamePtr = fileName->Des();
		    // Get first track's name in case of album
		if (numMediaObjects > KFirstMoIndex)
		    User::LeaveIfError( aDownload.GetStringAttribute( EDlAttrDestFilename, KFirstMoIndex, fileNamePtr ) );
		else
		    User::LeaveIfError( aDownload.GetStringAttribute( EDlAttrDestFilename, fileNamePtr ) );

        // check if file exists, if does not , don't continue
        if ( !IsDuplicateL( fileNamePtr ) )
            {
            HBufC* infoPrompt = StringLoader::LoadLC( R_DMUL_ERROR_FILE_NOT_FOUND);
            CAknInformationNote* note = new(ELeave)  CAknInformationNote();
            note->ExecuteLD(*infoPrompt);
            CleanupStack::PopAndDestroy(infoPrompt);

    		CleanupStack::PopAndDestroy( fileName );
            CleanupStack::PopAndDestroy( genParList );

            return;
            }


		TAiwVariant fileNameVariant( fileNamePtr );
		TAiwGenericParam fileNameParam( EGenericParamFile, fileNameVariant );
		genParList->AppendL( fileNameParam );


    	// Since we don't have the EGenericParamId for track index we are using EGenericParamUnspecified.
    	// Once we get the generic parameter for the id in future,this will be changed.
        TAiwVariant activeIndexVariant( activeDownloadID );
        TAiwGenericParam activeIndexParam( EGenericParamUnspecified, activeIndexVariant );
        genParList->AppendL( activeIndexParam );

        HBufC8* param8 = HBufC8::NewLC( KMinLength ); // Must be large enough!!
        TPtr8 paramPtr8 = param8->Des();
        RDesWriteStream writeStream( paramPtr8 );

        genParList->ExternalizeL( writeStream );

        writeStream.CommitL();
        writeStream.Close();

		//Instead of Selecting topmost task matching the UID from all the tasks, opening the standalone application
        CAknTaskList *taskList = CAknTaskList::NewL(CEikonEnv::Static()->WsSession());
        TApaTask task = taskList->FindRootApp(pdPlayerUid);
        TInt result = aDownload.SetBoolAttribute( EDlAttrProgressive, ETrue );

        if ( task.Exists() )
            {
            task.BringToForeground();
            // 8-bit buffer is required.
            task.SendMessage( TUid::Uid( 0 ), *param8 ); // Uid is not used
            }
        else
            {
            HBufC* param = HBufC::NewLC( param8->Length() );
            param->Des().Copy( *param8 );

            RApaLsSession appArcSession;
            User::LeaveIfError( appArcSession.Connect() );
            CleanupClosePushL( appArcSession );
            TThreadId id;
            User::LeaveIfError
                (
                    appArcSession.StartDocument( *param, pdPlayerUid, id )
                );

            CleanupStack::PopAndDestroy( &appArcSession );
            CleanupStack::PopAndDestroy( param );
            }

        //Store the information whether progressive play is launched or not
        iRegistryModel.UserInteractions().SetProgressiveDownloadLaunched( ETrue );
        if( aProgressively )
            {
            User::LeaveIfError( aDownload.SetIntAttribute( EDlAttrActivePlayedDownload, activeDownloadID ) );
            }

        CleanupStack::PopAndDestroy( param8 );
		CleanupStack::PopAndDestroy( fileName );
        CleanupStack::PopAndDestroy( genParList );
        }

    CLOG_LEAVEFN("CUserInteractionsEventHandler::LaunchPdAppL");
    }


#ifdef __DMGR_PD_TESTHARNESS
// -----------------------------------------------------------------------------
// CUserInteractionsEventHandler::TestLaunchPdAppL
// -----------------------------------------------------------------------------
//
void CUserInteractionsUtils::TestLaunchPdAppL( RHttpDownload& aDownload )
    {
    CLOG_ENTERFN("CUserInteractionsEventHandler::TestLaunchPdAppL");

    const TUid KTestPdPlayerUid = { 0x01FFDAAC };

    // Pack download ID into EGenericParamDownloadId
    TInt32 downloadID( 0 );
    User::LeaveIfError( aDownload.GetIntAttribute( EDlAttrId, downloadID ) );

    CAiwGenericParamList* genParList = CAiwGenericParamList::NewLC();
    TAiwVariant downloadIdVariant( downloadID );
    TAiwGenericParam downloadIdParam( EGenericParamDownloadId, downloadIdVariant );
    genParList->AppendL( downloadIdParam );

    // Pack fileName into EGenericParamFile
    HBufC* fileName = HBufC::NewLC( KMaxPath );
    TPtr fileNamePtr = fileName->Des();
    User::LeaveIfError( aDownload.GetStringAttribute( EDlAttrDestFilename, fileNamePtr ) );

    TAiwVariant fileNameVariant( fileNamePtr );
    TAiwGenericParam fileNameParam( EGenericParamFile, fileNameVariant );
    genParList->AppendL( fileNameParam );

    HBufC8* param8 = HBufC8::NewLC( KMinLength );
    TPtr8 paramPtr8 = param8->Des();
    RDesWriteStream writeStream( paramPtr8 );

    genParList->ExternalizeL( writeStream );

    writeStream.CommitL();
    writeStream.Close();

	TApaTaskList taskList( CEikonEnv::Static()->WsSession() );
    TApaTask task = taskList.FindApp( KTestPdPlayerUid );
    TInt result = aDownload.SetBoolAttribute( EDlAttrProgressive, ETrue );

    if ( task.Exists() )
        {
        // 8-bit buffer is required.
        task.SendMessage( TUid::Uid( 0 ), *param8 ); // Uid is not used
        task.BringToForeground();
        }
    else
        {
        HBufC* param = HBufC::NewLC( param8->Length() );
        param->Des().Copy( *param8 );

        RApaLsSession appArcSession;
        User::LeaveIfError( appArcSession.Connect() );
        CleanupClosePushL( appArcSession );
        TThreadId id;

        User::LeaveIfError(appArcSession.StartDocument( *param, KTestPdPlayerUid, id ));

        CleanupStack::PopAndDestroy( &appArcSession );
        CleanupStack::PopAndDestroy( param );
        }

    CleanupStack::PopAndDestroy( param8 );
    CleanupStack::PopAndDestroy( fileName );
    CleanupStack::PopAndDestroy( genParList );

    CLOG_LEAVEFN("CUserInteractionsEventHandler::TestLaunchPdAppL");
    }

#endif // __DMGR_PD_TESTHARNESS


// -----------------------------------------------------------------------------
// CUserInteractionsUtils::HandleServerAppExit
// -----------------------------------------------------------------------------
//
void CUserInteractionsUtils::HandleServerAppExit( TInt aReason )
    {
    CLOG_ENTERFN("CUserInteractionsUtils::HandleServerAppExit");
    CLOG_WRITE_FORMAT(" aReason: %d", aReason);

    // DocHandler has finished its task.
    iDocHandlerUsesTheUi = EFalse;
    // Schedule the next download:
#ifdef _DEBUG
    TRAPD( scheduleErr, iRegistryModel.UserInteractions().SchedulePostponedDownloadL() );
    CLOG_WRITE_FORMAT(" scheduleErr: %d", scheduleErr);
#else
    TRAP_IGNORE( iRegistryModel.UserInteractions().SchedulePostponedDownloadL() );
#endif // _DEBUG

    if ( iHandlerObserver )
        {
        iHandlerObserver->NotifyHandlerExit( iHandledDownload, aReason );
        }

    CLOG_LEAVEFN("CUserInteractionsUtils::HandleServerAppExit");
    }

// -----------------------------------------------------------------------------
// CUserInteractionsUtils::IsNetworkPdCompatibleL()
// -----------------------------------------------------------------------------
//
TBool CUserInteractionsUtils::IsNetworkPdCompatibleL() const
    {
    CLOG_ENTERFN("CUserInteractionsEventHandler::IsNetworkPdCompatibleL");
    // This method must not be called if 2G/3G distinction is not supported;
    // the RMobilePhone-related classes are not open in that case.

    TBool isNetworkPdCompatible( EFalse );

#if !defined( __WINS__ )
	// this initialization needed for identifying 3G networks
    RTelServer  telServer;
    RMobilePhone    mobilePhone;

    RMobilePhone::TMobilePhoneNetworkMode networkMode;
    User::LeaveIfError( telServer.Connect() );
    User::LeaveIfError( telServer.LoadPhoneModule( KMmTsyModuleName ) );

    TInt numPhones;
    User::LeaveIfError( telServer.EnumeratePhones( numPhones ) );
    if( numPhones <= 0 )
        {
        User::Leave( KErrCorrupt );
        }

    RTelServer::TPhoneInfo phoneInfo;
    User::LeaveIfError( telServer.GetPhoneInfo( 0, phoneInfo ) );
    User::LeaveIfError( mobilePhone.Open( telServer, phoneInfo.iName ) );
    User::LeaveIfError( mobilePhone.Initialise() );

    User::LeaveIfError( mobilePhone.GetCurrentMode( networkMode ) );

    TBool isWlan = GetWlanAvailabilityL();
    if( (networkMode != RMobilePhone::ENetworkModeGsm) || (networkMode == RMobilePhone::ENetworkModeGsm && isWlan))
        {
        isNetworkPdCompatible = ETrue;
        }

    mobilePhone.Close();
    telServer.Close();

	isNetworkPdCompatible = ETrue;
#else
	isNetworkPdCompatible = ETrue;
#endif
    CLOG_LEAVEFN("CUserInteractionsEventHandler::IsNetworkPdCompatibleL");
	return isNetworkPdCompatible;
    }


// -----------------------------------------------------------------------------
// CUserInteractionsUtils::SendMsgTerminateToPdAppsL
// -----------------------------------------------------------------------------
//

void CUserInteractionsUtils::SendMsgTerminateToPdAppsL(TBool aProgressiveDownloadLaunched )
    {
        CLOG_ENTERFN("CUserInteractionsEventHandler::SendMsgTerminateToPdAppsL");
        // Pack EGenericParamTerminate.
        TInt32 browserTerminate( 1 );
        CAiwGenericParamList* genParList = CAiwGenericParamList::NewLC();
        TAiwVariant terminateVariant( browserTerminate );
        TAiwGenericParam genericParamTerm( EGenericParamTerminate, terminateVariant );
        genParList->AppendL( genericParamTerm );

        HBufC8* param8 = HBufC8::NewLC( KMinLength ); // Must be large enough!!
        TPtr8 paramPtr8 = param8->Des();
        RDesWriteStream writeStream( paramPtr8 );

        genParList->ExternalizeL( writeStream );

        writeStream.CommitL();
        writeStream.Close();

		TApaTaskList taskList( CEikonEnv::Static()->WsSession() );

	    TBool isProgressive (EFalse);
 	    TInt downloadCnt = iRegistryModel.DownloadCount();
 	    const CDownloadArray& downloads = iRegistryModel.DownloadMgr().CurrentDownloads();
 	    TInt j ;
 	    for( j = 0; j < downloadCnt; ++j )
 	        {
 	        RHttpDownload* dl = downloads.At(j); // current download
 	        dl->GetBoolAttribute( EDlAttrProgressive, isProgressive );
 	        if(isProgressive)
 	            {
 	            break;
 	            }
 	        }
	    for( TInt i = 0; i < KDocPDAppUidCount; i++ )
	    {
			TUid KTestPdPlayerUid = {KDocPDAppUidList[ i ]};
			TApaTask task = taskList.FindApp(KTestPdPlayerUid );		// task for MP app
			if ( task.Exists() && ( isProgressive || aProgressiveDownloadLaunched ) )
				{
				if ( isProgressive )
				    {
				    RHttpDownload* dl = downloads.At(j); // current download
                    //This Atribute will tell if MP called Delete
				    //on exit of Browser
 	                dl->SetBoolAttribute( EDlAttrProgressive, EFalse );
				    }
				// 8-bit buffer is required.
				task.SendMessage( TUid::Uid( 0 ), *param8 ); // Uid is not used
				task.BringToForeground();
				}
		}
        CleanupStack::PopAndDestroy( param8 );
        CleanupStack::PopAndDestroy( genParList );

        CLOG_LEAVEFN("CUserInteractionsEventHandler::SendMsgTerminateToPdAppsL");

    }


// -----------------------------------------------------------------------------
// CUserInteractionsUtils::GetWlanAvailabilityL
// -----------------------------------------------------------------------------
//
TBool CUserInteractionsUtils::GetWlanAvailabilityL() const
    {
    TRequestStatus status( KRequestPending );

    // Connection monitor
    RConnectionMonitor connMon;
    CleanupClosePushL( connMon );
    User::LeaveIfError( connMon.ConnectL() );

    TConnMonIapInfoBuf infoBuf;
    infoBuf().iCount = 0;

    // Get WLAN IAPs to infoBuf
    connMon.GetPckgAttribute( EBearerIdWLAN ,0 , KIapAvailability, infoBuf, status );
    User::WaitForRequest( status );

    CleanupStack::PopAndDestroy( &connMon );

    if ( !status.Int() && infoBuf().iCount )
        {
        return ETrue;
        }

    return EFalse;
    }

// ------------------------------------------------------------
// CUserInteractionsUtils::CanLaunchAsProgDownload()
// Check if several conditions are met for Progressive Playback
// ------------------------------------------------------------
//
TBool CUserInteractionsUtils::CanLaunchAsProgDownload(
    RHttpDownload& aDownload,
    CDownloadMgrUiDownloadsList& dlList,
    TBool isOma2Dl )
    {
    CLOG_ENTERFN( "CUserInteractionsUtils::CanLaunchAsProgDownload" );
    TBool progDlOk( !dlList.IsOneProgressive() && IsNetworkPdCompatibleL() );
    // if OMA 2 download, no need to check for DRM rights on the phone, as license was already acquired.
    if( progDlOk && !isOma2Dl  )
        {
        TBool isDrmDl = DrmDownloadL( aDownload );
        CLOG_WRITE_FORMAT( " :isDrmDl=%d", isDrmDl );
        if( isDrmDl )
            {
            TBool prevRights( EFalse );
            progDlOk = DrmRightsOnThePhoneL( aDownload, prevRights );
            CLOG_WRITE_FORMAT( " :prevRights=%d", prevRights );
            }
        }
    TInt32 pdTrack(0);
    aDownload.GetIntAttribute(EDlAttrActivePlayedDownload,pdTrack);
    if( pdTrack )
        {
        //SomeTrack is already being played
        progDlOk = EFalse;
        }
    CLOG_WRITE_FORMAT( " :progDlOk=%d", progDlOk );
    CLOG_LEAVEFN( "CUserInteractionsUtils::CanLaunchAsProgDownload" );
    return progDlOk;
    }

// ---------------------------------------------------------
// CUserInteractionsUtils::UpdateDCFRepositoryL()
// Update saved file to DCFRepository
// ---------------------------------------------------------
//
void CUserInteractionsUtils::UpdateDCFRepositoryL(
    const TDesC& aFileName )
    {
    CLOG_ENTERFN( "CUserInteractionsUtils::UpdateDCFRepositoryL" );
    CLOG_WRITE_FORMAT( " :aFileName=%S", &aFileName );
    CDcfEntry* dcfEntry = NULL;
    dcfEntry = CDcfEntry::NewL();
    CleanupStack::PushL( dcfEntry );

    CDcfRep* dcfRep = NULL;
    dcfRep = CDcfRep::NewL();
    CleanupStack::PushL( dcfRep );

    dcfEntry->SetLocationL( aFileName, 0 );
    CLOG_WRITE(" : SetLocationL OK");
    dcfRep->UpdateL( dcfEntry );
    CLOG_WRITE(" :UpdateL OK");
    CleanupStack::PopAndDestroy(2); // dcfEntry, dcfRep
    CLOG_LEAVEFN("CUserInteractionsUtils::UpdateDCFRepositoryL");
    }

// ---------------------------------------------------------
// CUserInteractionsUtils::IsCodDownload()
// return true if its a COD DL
// ---------------------------------------------------------
//
TBool CUserInteractionsUtils::IsCodDownload()
    {
    return iCodDownload;
    }

// End of file.
