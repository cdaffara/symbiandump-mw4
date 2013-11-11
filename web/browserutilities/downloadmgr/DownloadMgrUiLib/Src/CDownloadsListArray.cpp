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
* Description:  Associate list box text array with array of downloads.
*
*/



// INCLUDE FILES
//#include <platform/mw/Browser_platform_variant.hrh>
#include    <Browser_platform_variant.hrh>
#include    "CDownloadsListArray.h"
#include    "ProgressInfoCreator.h"
#include    "DMgrUiLibPanic.h"
#include    "UiLibLogger.h"
#include    <DownloadMgrUiLib.rsg>
#include    <DownloadMgrUiLib.mbg>
#include    <gulicon.h>
#include    <coemain.h>
#include    <apgcli.h>
#include    <apgicnfl.h>
#include    <data_caging_path_literals.hrh>
#include    <f32file.h>
#include    <AknsUtils.h>
#include    <DocumentHandler.h>

// following line is temporary: AVKON dependency removal
#undef BRDO_APP_GALLERY_SUPPORTED_FF

#ifdef BRDO_APP_GALLERY_SUPPORTED_FF
#include    <MediaGalleryUid.h>
#endif

#include    "CDownloadUtils.h"
#include    <HttpDownloadMgrCommon.h>
#include    "CUserInteractionsUtils.h"


// LOCAL CONSTANTS AND MACROS
LOCAL_D const TInt KArrayGranularity = 16;
_LIT( KCharTab, "\t" );
_LIT( KDownloadMgrUiLibMbmFileNameAndDriveZ, "Z:DownloadMgrUiLib.mbm");

_LIT( KIndexString, "%d/%d  " );
const TInt KMaxIndexStringLength = 16;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TDownloadUiData::TDownloadUiData
// -----------------------------------------------------------------------------
//
TDownloadUiData::TDownloadUiData()
:   iName( KNullDesC ),
	iDestFileName( KNullDesC ),
    iContentType( KNullDesC8 ),
    iDownloadState( KErrNotFound ),
    iFullSize( KErrNotFound ),
    iDownloadedSize( KErrNotFound ),
    iIconIndex( KErrNotFound ),
    iPausable( EFalse ),
    iIsOnExternalMemory( EFalse ),
    iProgressState( KErrNotFound ),
    iNumMediaObjects( KErrNotFound )
    {
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDownloadsListArray::CDownloadsListArray
// -----------------------------------------------------------------------------
//
CDownloadsListArray::CDownloadsListArray()
:   iCoeEnv( *CCoeEnv::Static() ),
    iOwnedExt( EFalse ),
    iPlatformSupportsGallery ( EFalse )
    {
    }

// -----------------------------------------------------------------------------
// CDownloadsListArray::ConstructL
// -----------------------------------------------------------------------------
//
void CDownloadsListArray::ConstructL()
    {
    CLOG_ENTERFN("CDownloadsListArray::ConstructL");

    iDownloadArray = new (ELeave) CDownloadArray( KArrayGranularity );
    CLOG_WRITE(" iDownloadArray OK");
    iUiDataArray = new (ELeave) CDownloadUiDataArray( KArrayGranularity );
    CLOG_WRITE(" iUiDataArray OK");
    iListBoxTextArray = new (ELeave) CDesCArrayFlat( KArrayGranularity );
    CLOG_WRITE(" iListBoxTextArray OK");
    iListBoxIconArray = new (ELeave) CArrayPtrFlat<CGulIcon>( KArrayGranularity );
    CLOG_WRITE(" iListBoxIconArray OK");
    iProgressInfoCreator = CProgressInfoCreator::NewL();
    CLOG_WRITE(" iProgressInfoCreator OK");
    iCompletedString = iCoeEnv.AllocReadResourceL( R_DMUL_DOWNLOAD_COMPLETE );
    CLOG_WRITE(" iCompletedString OK");
    iSavedToGalleryString = iCoeEnv.AllocReadResourceL( R_DMUL_DOWNLOAD_CONTENT_SAVED_TO_GALLERY );
    CLOG_WRITE(" iSavedToGalleryString OK");
	iSavedToDownloadsFolderString=  iCoeEnv.AllocReadResourceL( R_DMUL_DOWNLOAD_CONTENT_SAVED_TO_DOWNLOADS_FOLDER);
	CLOG_WRITE(" iSavedToGalleryDownload OK");

	#ifndef BRDO_APP_GALLERY_SUPPORTED_FF
	    iFileSavedString = iCoeEnv.AllocReadResourceL( R_DMUL_DOWNLOAD_FILE_SAVED );
	    iFilesSavedString = iCoeEnv.AllocReadResourceL( R_DMUL_DOWNLOAD_FILES_SAVED );
	#endif


    iNullDesC = KNullDesC().AllocL();
    CLOG_WRITE(" iNullDesC OK");
    User::LeaveIfError( iApaLsSess.Connect() );
    CLOG_WRITE(" iApaLsSess OK");


    //whether the platform supports gallery app or not; defined in browser_platfrom_variant.hrh
    #ifdef BRDO_APP_GALLERY_SUPPORTED_FF
    iPlatformSupportsGallery = ETrue;
    #endif


    // Initialize the UiLib' mbm file name:
    TParse* fileParser = new (ELeave) TParse;
    CleanupStack::PushL( fileParser );

    fileParser->Set( KDownloadMgrUiLibMbmFileNameAndDriveZ, &KDC_APP_BITMAP_DIR, NULL );
    TUint attValue(0);
    TInt fileErr = iCoeEnv.FsSession().Att( fileParser->FullName(), attValue );
    CLOG_WRITE_FORMAT(" fileErr Z: %d",fileErr);
    User::LeaveIfError( fileErr );
    iMbmResourceFileName = HBufC::NewMaxL( fileParser->FullName().Length() );
    iMbmResourceFileName->Des().Copy( fileParser->FullName() );
    CLOG_WRITE_FORMAT(" iMbmResourceFileName: %S",iMbmResourceFileName);

    CleanupStack::PopAndDestroy( fileParser ); // fileParser
    fileParser = NULL;

    AddDefaultIconsL();

    CLOG_LEAVEFN("CDownloadsListArray::ConstructL");
    }

// -----------------------------------------------------------------------------
// CDownloadsListArray::NewL
// -----------------------------------------------------------------------------
//
CDownloadsListArray* CDownloadsListArray::NewL()
    {
    CDownloadsListArray* self = new ( ELeave ) CDownloadsListArray();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// Destructor
CDownloadsListArray::~CDownloadsListArray()
    {
    CLOG_ENTERFN("CDownloadsListArray::~CDownloadsListArray");

    delete iApaMaskedBitmap;
    iApaMaskedBitmap = 0;
    delete iProgressInfoCreator;
    iProgressInfoCreator = 0;
    delete iListBoxTextArray;
    iListBoxTextArray = 0;
    if ( (iOwnedExt == EFalse ) && (iListBoxIconArray != NULL) )
    {
        iListBoxIconArray->ResetAndDestroy();
        delete iListBoxIconArray;
        iListBoxIconArray = 0;
    }


    delete iUiDataArray;
    iUiDataArray = 0;
    delete iDownloadArray;
    iDownloadArray = 0;
    delete iCompletedString;
    iCompletedString = 0;
	delete iSavedToGalleryString;
	iSavedToGalleryString = 0;
	delete iSavedToDownloadsFolderString;
	iSavedToDownloadsFolderString=0;

	#ifndef BRDO_APP_GALLERY_SUPPORTED_FF
	    delete iFileSavedString;
	    iFileSavedString = 0;
	    delete iFilesSavedString;
	    iFilesSavedString = 0;
	#endif

    delete iNullDesC;
    iNullDesC = 0;
    iApaLsSess.Close();
    CLOG_WRITE(" iApaLsSess.Close");
    delete iMbmResourceFileName;
    iMbmResourceFileName = 0;

    CLOG_LEAVEFN("CDownloadsListArray::~CDownloadsListArray");
    }

// -----------------------------------------------------------------------------
// CDownloadsListArray::AppendL
// -----------------------------------------------------------------------------
//
TInt CDownloadsListArray::AppendL( CGulIcon* aIcon )
    {
    // TODO Performance: append only if it does not exist in list already.
    iListBoxIconArray->AppendL( aIcon );
    return (iListBoxIconArray->Count()-1);//todo
    }

// -----------------------------------------------------------------------------
// CDownloadsListArray::AppendL
// -----------------------------------------------------------------------------
//
TInt CDownloadsListArray::AppendL
    ( const TDownloadUiData& aDownloadUiData, const RHttpDownload& aDownload )
    {
    TInt itemIndex( KErrNotFound );

    __ASSERT_DEBUG( Find( aDownload, itemIndex ) == KErrNotFound,
                    Panic( EUiLibPanAlreadyExistWhenAppend ) );

    #ifdef _DEBUG
    TInt findErr = Find( aDownload, itemIndex ) ;
    CLOG_WRITE_FORMAT(" findErr: %d",findErr);
    CLOG_WRITE_FORMAT(" itemIndex: %d",itemIndex);
    #endif // _DEBUG

    // Append at the end:
    // Reserve space in order that appending to all the arrays be atomic.
    // Use iListBoxTextArray->Count() for SetReserveL, because thus we can
    // eliminate that subsequent SetReserveL reserve unnecessary
    // place if iListBoxTextArray->AppendL() LEAVEs.
    iDownloadArray->SetReserveL( iListBoxTextArray->Count() + 1 );
    iUiDataArray->SetReserveL( iListBoxTextArray->Count() + 1 );
    iListBoxTextArray->AppendL( LBTextFromUiData( aDownloadUiData ) );
    // iListBoxTextArray->AppendL did not leave and the following
    // will not leave, because of SetReserveL()'s:
    iDownloadArray->AppendL( (RHttpDownload*)&aDownload );
    iUiDataArray->AppendL( aDownloadUiData );
    itemIndex = Count() - 1;

    return itemIndex;
    }

// -----------------------------------------------------------------------------
// CDownloadsListArray::UpdateL
// -----------------------------------------------------------------------------
//
void CDownloadsListArray::UpdateL( TInt aIndex, const TDownloadUiData& aDownloadUiData )
    {
    // Two steps: update UI data and the list item text.
    // Because updating the list text array can leave and the two
    // array must be consistent, this operation must be atomic, this is
    // why we apply the operations in the following order:
    // Insert the new text, and when it succeeds, remove the old (which is shifted).
    // No need to modify iDownloadArray, because that does not change.
    iListBoxTextArray->InsertL( aIndex, LBTextFromUiData( aDownloadUiData ) );
    iListBoxTextArray->Delete( (aIndex+1) ); // Old item was shifted.
    // Yes, update the UI data, which cannot leave.
    // No index boundary check - using operator []!
    (*iUiDataArray)[aIndex] = aDownloadUiData;
    }

// -----------------------------------------------------------------------------
// CDownloadsListArray::UpdateIfNeededL
// -----------------------------------------------------------------------------
//
void CDownloadsListArray::UpdateIfNeededL
    ( TInt aIndex, const TDownloadUiData& aDownloadUiData, TBool& aUpdated )
    {
    // Regenerate item text
    TPtrC regenerated = LBTextFromUiData( aDownloadUiData );
    if ( regenerated.Compare( Text( aIndex ) ) != KErrNone )
        {
        // The are different - update needed
        UpdateL( aIndex, aDownloadUiData );
        aUpdated = ETrue;
        }
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// CDownloadsListArray::Delete
// -----------------------------------------------------------------------------
//
void CDownloadsListArray::Delete( TInt aIndex )
    {
    // Delete the indicated record from all arrays:

    iDownloadArray->Delete( aIndex );
    iUiDataArray->Delete( aIndex );
    iListBoxTextArray->Delete( aIndex );

    iDownloadArray->Compress();
    iUiDataArray->Compress();
    iListBoxTextArray->Compress();
    }

// -----------------------------------------------------------------------------
// CDownloadsListArray::SetIconArrayOwnedExternally
// -----------------------------------------------------------------------------
//
void CDownloadsListArray::SetIconArrayOwnedExternally( TBool aOwnedExt )
    {
    iOwnedExt = aOwnedExt;
    }

// -----------------------------------------------------------------------------
// CDownloadsListArray::DownloadsCount
// -----------------------------------------------------------------------------
//
TInt CDownloadsListArray::DownloadsCount( TUint16 aDownloadStates )
    {
    TInt retCount( 0 );
    TInt count = Count();
    for ( TInt i = 0; i < count; ++i )
        {
        TDownloadUiData& dlData = DlUiData(i);
        TInt32 state( dlData.iDownloadState );

        if ( aDownloadStates & MASKED_DL_STATE( state ) )
            {
            ++retCount;
            }
        }
    return retCount;
    }

// -----------------------------------------------------------------------------
// CDownloadsListArray::LBTextFromUiData
// Be careful when modifying this as it is not leavable!
// This function must be consistent with LBTextLength()!
// -----------------------------------------------------------------------------
//
TPtrC CDownloadsListArray::LBTextFromUiData( const TDownloadUiData& aDownloadUiData )
    {
    // Compute the space required for the current progress string:
    TInt spaceNeeded = LBTextLength( aDownloadUiData );

    // Realloc a bigger buffer if needed
    if ( iNewItemText && ( spaceNeeded <= iNewItemText->Length() ) )
        {
        // No need to reallocate a bigger buffer.
        // The existing iNewItemText will be good.
        }
    else
        {
        // Allocate a new (bigger) buffer (delete the old)
        delete iNewItemText;
        iNewItemText = 0;
        iNewItemText = HBufC::New( spaceNeeded );
        }

    // Non-leaving function - couldn't call NewL!
    if ( iNewItemText )
        {
        TPtr newItemTextPtr = iNewItemText->Des();
        newItemTextPtr.SetLength( 0 );
        //
        iTempBuf.Zero();
        iTempBuf.Num( aDownloadUiData.iIconIndex );
        newItemTextPtr.Append( iTempBuf ); // Add icon array index
        newItemTextPtr.Append( KCharTab );
        //
        if ( aDownloadUiData.iName.Length() )
            {
            newItemTextPtr.Append( aDownloadUiData.iName );
            }
        else
            {
            newItemTextPtr.Append( KNullDesC );
            }
        newItemTextPtr.Append( KCharTab );

        // If its album download, format display string cur track/total tracks

        if ((aDownloadUiData.iNumMediaObjects > 1) &&
            (EHttpDlMultipleMOCompleted != aDownloadUiData.iDownloadState))
            {
            TBuf<KMaxIndexStringLength> indexStr;
            indexStr.Format( KIndexString, aDownloadUiData.iActiveMoIndex, aDownloadUiData.iNumMediaObjects );
            newItemTextPtr.Append( indexStr );
            }

        iProgressInfoRes.Zero();

		if ( aDownloadUiData.iDownloadState == EHttpDlMultipleMOCompleted )
		    {
 		    if ( aDownloadUiData.iProgressState == EHttpProgContentFileMoved || aDownloadUiData.iProgressState == EHttpProgContentFileMovedAndDestFNChanged )
			    {
			    if ( iPlatformSupportsGallery )
			        {
			    	TBool supported( EFalse );
                    TRAP_IGNORE(supported = IsSupportedL(aDownloadUiData));
 				    if( supported )
				        {
				        iProgressInfoRes.Copy
					                    ( iSavedToGalleryString->Left( KMaxDownloadItemTextPartLength ) );
				        }
				    else
                        {
                	    iProgressInfoRes.Copy
			                            ( iSavedToDownloadsFolderString->Left( KMaxDownloadItemTextPartLength ) );
                        }
			        }
			    else
			        {
			        if (aDownloadUiData.iNumMediaObjects > 1)
			            {
			        	iProgressInfoRes.Copy
			                            ( iFilesSavedString->Left( KMaxDownloadItemTextPartLength ) );
			            }
			        else
			            {
			        	iProgressInfoRes.Copy
			                            ( iFileSavedString->Left( KMaxDownloadItemTextPartLength ) );
			            }
                    }
			    }
	        else
    	        {
                iProgressInfoRes.Copy
		    	        ( iCompletedString->Left( KMaxDownloadItemTextPartLength ) );
				}
			}
        else
            {
            if ( aDownloadUiData.iFullSize <= 0 )
                {
                iProgressInfoCreator->ProgressInfo( aDownloadUiData.iDownloadedSize,
                                                    iProgressInfoRes );
                }
            else
                {
                iProgressInfoCreator->ProgressInfo( aDownloadUiData.iDownloadedSize,
                                                    aDownloadUiData.iFullSize,
                                                    iProgressInfoRes );
                }
            }
        //
        newItemTextPtr.Append( iProgressInfoRes );
        newItemTextPtr.Append( KCharTab );
        //
        if ( aDownloadUiData.iDownloadState == EHttpDlCreated || // User must be able to start it via Resume.
             aDownloadUiData.iDownloadState == EHttpDlPaused ||
             aDownloadUiData.iDownloadState == EHttpDlMultipleMOFailed ) // User must be able to restart it.
            {
            // Add a "Paused" icon.
            iTempBuf.Zero();
            iTempBuf.Num( iPausedIconIndex );
            newItemTextPtr.Append( iTempBuf ); // iPausedIconIndex in literal form
            }
        newItemTextPtr.Append( KCharTab );
        //
        if ( aDownloadUiData.iIsOnExternalMemory )
            {
            // Add an "External memory" icon.
            iTempBuf.Zero();
            iTempBuf.Num( iExternalMemoryIconIndex );
            newItemTextPtr.Append( iTempBuf ); // iExternalMemoryIconIndex in literal form
            }
        }
    return iNewItemText ? (TPtrC)*iNewItemText : (TPtrC)*iNullDesC;
    }

// -----------------------------------------------------------------------------
// CDownloadsListArray::LBTextLength
// This function must be consistent with LBTextFromUiData()!
// -----------------------------------------------------------------------------
//
TInt CDownloadsListArray::LBTextLength( const TDownloadUiData& aDownloadUiData )
    {
    TInt ret(0);

    iTempBuf.Zero();
    iTempBuf.Num( aDownloadUiData.iIconIndex );
    ret += iTempBuf.Length();
    ret += KCharTab().Length();
    ret += aDownloadUiData.iName.Length();
    ret += KCharTab().Length();

    if( aDownloadUiData.iNumMediaObjects > 1 )
        {
        TBuf<KMaxIndexStringLength> indexStr;
        indexStr.Format( KIndexString, aDownloadUiData.iActiveMoIndex, aDownloadUiData.iNumMediaObjects );
        ret += indexStr.Length();
        }

    iProgressInfoRes.Zero();
	if ( aDownloadUiData.iDownloadState == EHttpDlMultipleMOCompleted )
	    {
		if ( aDownloadUiData.iProgressState == EHttpProgContentFileMoved || aDownloadUiData.iProgressState == EHttpProgContentFileMovedAndDestFNChanged )
		    {
		    if ( iPlatformSupportsGallery )
		        {
		        TBool supported( EFalse );
                TRAP_IGNORE(supported=IsSupportedL(aDownloadUiData));
		        if( supported )
		            {
		            iProgressInfoRes.Copy
				                    ( iSavedToGalleryString->Left( KMaxDownloadItemTextPartLength ) );
		            }
		        else
		            {
            	    iProgressInfoRes.Copy
			                        ( iSavedToDownloadsFolderString->Left( KMaxDownloadItemTextPartLength ) );
		            }
		        }
		    else
		        {
		        if (aDownloadUiData.iNumMediaObjects > 1)
			        {
			        iProgressInfoRes.Copy
			                        ( iFilesSavedString->Left( KMaxDownloadItemTextPartLength ) );
			        }
			    else
			        {
			       	iProgressInfoRes.Copy
			                        ( iFileSavedString->Left( KMaxDownloadItemTextPartLength ) );
			        }
		        }
			}
		else
		    {
            iProgressInfoRes.Copy
			                 ( iCompletedString->Left( KMaxDownloadItemTextPartLength ) );
		    }
        }
     //finished  here
    else
        {
        if ( aDownloadUiData.iFullSize <= 0 )
            {
            iProgressInfoCreator->ProgressInfo( aDownloadUiData.iDownloadedSize,
                                                iProgressInfoRes );
            }
        else
            {
            iProgressInfoCreator->ProgressInfo( aDownloadUiData.iDownloadedSize,
                                                aDownloadUiData.iFullSize,
                                                iProgressInfoRes );
            }
        }
    ret += iProgressInfoRes.Length();
    ret += KCharTab().Length();
    //
    if ( aDownloadUiData.iDownloadState == EHttpDlCreated || // User must be able to start it via Resume.
         aDownloadUiData.iDownloadState == EHttpDlPaused ||
         aDownloadUiData.iDownloadState == EHttpDlMultipleMOFailed ) // User must be able to restart it.
        {
        iTempBuf.Zero();
        iTempBuf.Num( iPausedIconIndex );
        ret += iTempBuf.Length();
        }
    //
    ret += KCharTab().Length();
    //
    if ( aDownloadUiData.iIsOnExternalMemory )
        {
        iTempBuf.Zero();
        iTempBuf.Num( iExternalMemoryIconIndex );
        ret += iTempBuf.Length();
        }


    return ret;
    }

// -----------------------------------------------------------------------------
// CDownloadsListArray::AddDefaultIconsL
// -----------------------------------------------------------------------------
//
void CDownloadsListArray::AddDefaultIconsL()
    {
    CLOG_ENTERFN("CDownloadsListArray::AddDefaultIconsL");

    // iPausedIconIndex
    MAknsSkinInstance* skins = AknsUtils::SkinInstance();
    TAknsItemID id = KAknsIIDQgnIndiPaused;
    CGulIcon* gulIcon = AknsUtils::CreateGulIconL( skins, id,
                                   *iMbmResourceFileName,
                                   EMbmDownloadmgruilibQgn_indi_paused,
                                   EMbmDownloadmgruilibQgn_indi_paused_mask );
    CleanupStack::PushL( gulIcon );

    iPausedIconIndex = AppendL( gulIcon );
    CLOG_WRITE_FORMAT(" iPausedIconIndex: %d",iPausedIconIndex);

    CleanupStack::Pop( gulIcon ); // gulIcon

    // iExternalMemoryIconIndex
    TAknsItemID id2 = KAknsIIDQgnIndiBrowserMmcAdd;
    CGulIcon* gulIcon2 = AknsUtils::CreateGulIconL( skins, id2,
                                   *iMbmResourceFileName,
                                   EMbmDownloadmgruilibQgn_indi_browser_mmc_add,
                                   EMbmDownloadmgruilibQgn_indi_browser_mmc_add_mask );
    CleanupStack::PushL( gulIcon2 );

    iExternalMemoryIconIndex = AppendL( gulIcon2 );
    CLOG_WRITE_FORMAT(" iExternalMemoryIconIndex: %d",iExternalMemoryIconIndex);

    CleanupStack::Pop( gulIcon2 ); // gulIcon2

    CLOG_LEAVEFN("CDownloadsListArray::AddDefaultIconsL");
    }

// -----------------------------------------------------------------------------
// CDownloadsListArray::AddHandlerAppIconL
// -----------------------------------------------------------------------------
//
TInt CDownloadsListArray::AddHandlerAppIconL( const TDesC8& aContentType )
    {
    CLOG_ENTERFN("CDownloadsListArray::AddHandlerAppIconL");

    // Find the appropriate application that we associate with the content type
    TUid appUid( TUid::Uid(0) );
    TBool appFound = FindHandlerApp( aContentType, appUid );
    CLOG_WRITE_FORMAT(" appFound: %d",appFound);

    // Use Media Gallery's icon instead of Image Viewer, because IV's icon
    // does not support skins, but MG's icon does support it.


#ifdef BRDO_APP_GALLERY_SUPPORTED_FF

    if ( appFound && appUid == TUid::Uid(KImageViewerHandler) )
        {
        appUid = TUid::Uid(KMediaGalleryUID3);
        }
#endif

    // Load the proper icon.
    CFbsBitmap* bitmap = 0;
    CFbsBitmap* mask = 0;
    LoadAppIconLC( (appFound ? appUid : TUid::Uid(0)), bitmap, mask );
    // Do append.
    CGulIcon* gulIcon = CGulIcon::NewL( bitmap, mask );
    gulIcon->SetBitmapsOwnedExternally( EFalse ); // gulIcon will own
    CleanupStack::Pop( 2 ); // mask, bitmap
    CleanupStack::PushL( gulIcon );

    TInt index = AppendL( gulIcon );

    CleanupStack::Pop( gulIcon ); // gulIcon

    CLOG_WRITE_FORMAT(" index: %d",index);
    CLOG_LEAVEFN("CDownloadsListArray::AddHandlerAppIconL");
    return (index);
    }

// -----------------------------------------------------------------------------
// CDownloadsListArray::FindHandlerApp
// -----------------------------------------------------------------------------
//
TBool CDownloadsListArray::FindHandlerApp( const TDesC8& aContentType, TUid& aAppUid ) const
    {
    if ( aContentType.Length() == 0 )
        {
        return EFalse;
        }
    else
        {
        TDataType dataType( aContentType );
        TInt appFoundErr = iApaLsSess.AppForDataType( dataType, aAppUid );
        return ( appFoundErr == KErrNone );
        }
    }

// -----------------------------------------------------------------------------
// CDownloadsListArray::LoadAppIconLC
// -----------------------------------------------------------------------------
//
void CDownloadsListArray::LoadAppIconLC( TUid aAppUid,
                                         CFbsBitmap*& aBitmap,
                                         CFbsBitmap*& aMask )
    {
    CLOG_ENTERFN("CDownloadsListArray::LoadAppIconLC");

    MAknsSkinInstance* skins = AknsUtils::SkinInstance();
    TAknsItemID id;

    if ( aAppUid == TUid::Uid(0) )
        {
        // Indicate that the content is unsupported, that is no
        // viewer application that could handle the content.
        id = KAknsIIDQgnMenuUnknownLst;
        AknsUtils::CreateIconLC( skins, id, aBitmap, aMask,
                                 *iMbmResourceFileName,
                                 EMbmDownloadmgruilibQgn_menu_unknown_lst,
                                 EMbmDownloadmgruilibQgn_menu_unknown_lst_mask );
        }
    else
        {
        // Load the application's icon
        AknsUtils::CreateAppIconLC( skins, aAppUid,
                                    EAknsAppIconTypeList,
                                    aBitmap, aMask );
        }

    CLOG_LEAVEFN("CDownloadsListArray::LoadAppIconLC");
    }



TBool  CDownloadsListArray::IsSupportedL(const TDownloadUiData& aDownloadUiData)
    {

    TBool supported;
    HBufC* fullName = HBufC::NewLC(KMaxFileName); //TODO: change len
    TPtr fileName( fullName->Des() );
    fileName.Append(aDownloadUiData.iName);
    HBufC8* contentType = HBufC8::NewLC(KMaxPath);
    TPtr8 mimeTypePtr = contentType->Des();
    mimeTypePtr.Copy(aDownloadUiData.iContentType);
    if(!mimeTypePtr.Compare(KSisFileMimeType))
        {
            mimeTypePtr.Copy(KOctetStreamMimeType);
        }
    else if(!mimeTypePtr.Compare(KDrmMessageMimeType))
        {
            mimeTypePtr.Copy(KDrmMessageMimeTypeDrmDcf);
        }
    TDataType mimeSupported(mimeTypePtr);
    CDocumentHandler *docHandler=CDocumentHandler::NewLC();
    docHandler->CheckFileNameExtension(fileName, mimeSupported );
    TRAPD(err,supported = docHandler->CanOpenL(mimeSupported));
    if(err == KMimeNotSupported || !(CDownloadUtils::IsGallerySupported(aDownloadUiData.iContentType)))
        {
            supported = EFalse;

        }
    else
        {
            supported = ETrue;
        }
    CleanupStack::PopAndDestroy(docHandler);
    CleanupStack::PopAndDestroy( contentType);
    CleanupStack::PopAndDestroy(fullName);
    return supported;

    }


// End of file.

