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
* Description:  User Interactions event handler
*
*/



// INCLUDE FILES
//#include <platform/mw/Browser_platform_variant.hrh>
#include    <Browser_platform_variant.hrh>
#include    "UserInteractionsEventHandler.h"
#include    "CUserInteractionsUtils.h"
#include    "ProgressInfoCreator.h"
#include    "CDownloadMgrUiLibRegistry.h"
#include    "CDownloadMgrUiDownloadsList.h"
#include    "CDownloadMgrUiUserInteractions.h"
#include    "UiLibLogger.h"
#include    "DMgrUiLibPanic.h"
#include    <bldvariant.hrh>
#include    <DownloadMgrUiLib.rsg>
#include    <aknlists.h>
#include    <aknPopup.h>
#include    <StringLoader.h>
#include    <apgcli.h>
#include    <eikbtgpc.h>
#include    <sysutil.h>
#include    <exterror.h>
#include    <etelpckt.h>
#include    <aknnotedialog.h>
#include    <uriutils.h>


// following line is temporary: AVKON dependency removal
#undef BRDO_APP_GALLERY_SUPPORTED_FF

#ifdef BRDO_APP_GALLERY_SUPPORTED_FF
#include    <MGXFileManagerFactory.h>
#include    <CMGXFileManager.h>
#endif

#include    <pathinfo.h>
#include    <driveinfo.h>

#include    <apmrec.h>
#include    <apmstd.h>
#include    <s32std.h>
#include    <e32des8.h>

#include    <AiwGenericParam.h>
#include    <DocumentHandler.h>


// LOCAL CONSTANTS AND MACROS
_LIT( KListBoxSeparator, "\t" );
_LIT( KDownloadPath,"download");


const TInt KDownloadConfSizeLimit = 100000;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUserInteractionsEventHandler::CUserInteractionsEventHandler
// -----------------------------------------------------------------------------
//
CUserInteractionsEventHandler::CUserInteractionsEventHandler
    ( RHttpDownload& aDownload, THttpDownloadEvent aEvent,
      MDownloadMgrUiLibRegModel& aRegistryModel,
      CAsyncEventHandlerArray& aOwner,
      CUserInteractionsUtils& aUiUtils )
:   CAsyncEventHandlerBase( aDownload, aEvent, aRegistryModel, aOwner ),
    iUiUtils( aUiUtils ),
    iDownloadConfirmationShown( EFalse )
    {
    CLOG_ENTERFN("CUserInteractionsEventHandler::CUserInteractionsEventHandler");
    CLOG_WRITE_FORMAT(" %x", this);
    CLOG_LEAVEFN("CUserInteractionsEventHandler::CUserInteractionsEventHandler");
    }

// Destructor
CUserInteractionsEventHandler::~CUserInteractionsEventHandler()
    {
    CLOG_ENTERFN("CUserInteractionsEventHandler::~CUserInteractionsEventHandler");
    CLOG_WRITE_FORMAT(" %x", this);
    if ( iPopupList )
        {
        iPopupList->CancelPopup();
        iPopupList = NULL;
        }
    if ( iDeletedPtr )
        {
        *iDeletedPtr = ETrue;
        }
    if ( iDownloadConfirmationDeletedPtr )
        {
        *iDownloadConfirmationDeletedPtr = ETrue;
        }
    Cancel();
    CLOG_LEAVEFN("CUserInteractionsEventHandler::~CUserInteractionsEventHandler");
    }

// -----------------------------------------------------------------------------
// CUserInteractionsEventHandler::DiskSpaceBelowCriticalLevelL
// -----------------------------------------------------------------------------
//
#ifdef RD_MULTIPLE_DRIVE
TBool CUserInteractionsEventHandler::DiskSpaceBelowCriticalLevelL( RHttpDownload& aDownload, TInt aSize ) const
#else
TBool CUserInteractionsEventHandler::DiskSpaceBelowCriticalLevelL( TInt aSize ) const
#endif
    {
    CLOG_ENTERFN("CUserInteractionsEventHandler::DiskSpaceBelowCriticalLevelL");

   	TBool isSpaceBelowCL( EFalse );

    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL( fs );

    TInt bytesToWrite = aSize;
    if (bytesToWrite < 0)
        bytesToWrite = 0;

#ifdef RD_MULTIPLE_DRIVE
    // Destination is FFS in default
    TInt selectedDrive;
    User::LeaveIfError(
        DriveInfo::GetDefaultDrive( DriveInfo::EDefaultPhoneMemory,
                                    selectedDrive ) );
    TInt err( KErrNone );

    HBufC* fileName = HBufC::NewLC( KMaxPath );
    TPtr fileNamePtr = fileName->Des();
    User::LeaveIfError
        ( aDownload.GetStringAttribute( EDlAttrDestFilename, fileNamePtr ) );
    CLOG_WRITE_FORMAT(" EDlAttrDestFilename: %S",&fileNamePtr);

    // only that drive needs to be checked
    CLOG_WRITE( "cfds1" );
    if( (err = fs.CharToDrive( (*fileName)[0], selectedDrive )) != KErrNone )
        {
        CLOG_WRITE( "Bad drive letter" );
        User::Leave( err );
        }
    else if( !fs.IsValidDrive( selectedDrive ) )
        // invalid drive letter in filename
        {
        CLOG_WRITE( "Invalid drive" );
        User::Leave( KErrBadName );
        }

    CleanupStack::PopAndDestroy( fileName ); // fileName
    fileName = NULL;

    // Check if there's enough memory in the phone
	TRAP_IGNORE( isSpaceBelowCL = SysUtil::DiskSpaceBelowCriticalLevelL(
                                            &fs,
                                            bytesToWrite,
                                            selectedDrive ) );

    // If there is error, then isSpaceBelowCL is untouched and
    // contains the right value. Otherwise it also contains
    // the right value.
    CLOG_WRITE_FORMAT(" DiskSpace: isSpaceBelowCL: %d",isSpaceBelowCL);

#else
	isSpaceBelowCL = SysUtil::DiskSpaceBelowCriticalLevelL( &fs,
			                                bytesToWrite,
                                            EDriveC );
    CLOG_WRITE_FORMAT(" C: isSpaceBelowCL: %d",isSpaceBelowCL);

    if ( isSpaceBelowCL )
        {
        // Check MMC
        TRAP_IGNORE( isSpaceBelowCL =
                     SysUtil::MMCSpaceBelowCriticalLevelL( &fs, bytesToWrite ) );
        // If there is error, then isSpaceBelowCL is untouched and
        // contains the right value. Otherwise it also contains
        // the right value.
        CLOG_WRITE_FORMAT(" MMC: isSpaceBelowCL: %d",isSpaceBelowCL);
        }
#endif

    CleanupStack::PopAndDestroy( &fs ); // fs

    CLOG_LEAVEFN("CUserInteractionsEventHandler::DiskSpaceBelowCriticalLevelL");
    return isSpaceBelowCL;
    }

// -----------------------------------------------------------------------------
// CUserInteractionsEventHandler::ShowDownloadConfirmationL
// Confirmation is not shown, if:
// - the downloaded size = total size
// - or the total size is less than 100 kB
// The confirmation query is shown, if aForce is ETrue!
// -----------------------------------------------------------------------------
//
TBool CUserInteractionsEventHandler::ShowDownloadConfirmationL
    ( RHttpDownload& aDownload, TInt aCbaResource, TBool aForce )
    {
    CLOG_ENTERFN("CUserInteractionsEventHandler::ShowDownloadConfirmationL aForce");

    TBool ret;
    if ( aForce )
        {
        ret = ShowDownloadConfirmationL( aDownload, aCbaResource );
        }
    else
        {
        TInt32 totalLength( KErrNotFound );
        TInt32 downloadedSize( KErrNotFound );

        /*
        * Get full size of media content and downloaded size
        */

        User::LeaveIfError( aDownload.GetIntAttribute
                          ( EDlAttrMultipleMOLength, totalLength ) );
        CLOG_WRITE_FORMAT(" EDlAttrMultipleMOLength: %d",totalLength);

        User::LeaveIfError( aDownload.GetIntAttribute
                          ( EDlAttrMultipleMODownloadedSize, downloadedSize ) );
        CLOG_WRITE_FORMAT(" EDlAttrMultipleMODownloadedSizes: %d",downloadedSize);

        HBufC8* contentType = iUiUtils.ContentTypeL( aDownload, ETrue );
        CleanupStack::PushL( contentType );

        // Don't show the confirmation query if the downloading
        // already finished
        if ( downloadedSize == totalLength )
            {
            ret = ETrue;
            }
        else if ( 0 <= totalLength && totalLength < KDownloadConfSizeLimit )
            {
            ret = ETrue;
            }
        // GGUO-775RXR for widgets we don't need to have download confirmation either
        else if ( contentType->Compare( KWidgetMimeType) == 0 )
            {
            ret = ETrue;
            }
        else
            {
            ret = ShowDownloadConfirmationL( aDownload, aCbaResource );
            }
        CleanupStack::PopAndDestroy( contentType );
        }

    CLOG_LEAVEFN("CUserInteractionsEventHandler::ShowDownloadConfirmationL aForce");
    return ret;
    }
// -----------------------------------------------------------------------------
// CUserInteractionsEventHandler::ShowDownloadConfirmationL
// -----------------------------------------------------------------------------
//
// Cod we used the following order
// 		name/size/type/price(optional)/description(no)/vendor(no)

TBool CUserInteractionsEventHandler::ShowDownloadConfirmationL
    ( RHttpDownload& aDownload, TInt aCbaResource )
    {
    CLOG_ENTERFN("CUserInteractionsEventHandler::ShowDownloadConfirmationL");

    HBufC* itemText = NULL; // a buffer holding the text for each list item

    CEikFormattedCellListBox* listBox = new ( ELeave ) CAknDoublePopupMenuStyleListBox;
    CleanupStack::PushL( listBox );

    // If supported show OK & Cancel buttons, otherwise just CANCEL
    iPopupList = NULL; // reset pointer.
    CAknPopupList* popupList = CAknPopupList::NewL
            ( listBox, aCbaResource, AknPopupLayouts::EMenuDoubleWindow );
    CleanupStack::PushL( popupList );

    TInt flags = CEikListBox::EDisableHighlight;
    listBox->ConstructL( popupList, flags );
    listBox->CreateScrollBarFrameL( ETrue );
    listBox->ScrollBarFrame()->SetScrollBarVisibilityL
        ( CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );

    MDesCArray* itemList = listBox->Model()->ItemTextArray();
    CDesCArray* itemArray = (CDesCArray*)itemList;

    //-------------------------------------------
    // Header
    //
    HBufC* header = StringLoader::LoadLC( R_DMUL_DOWNLOADCONF_TITLE ); //"Download"
    popupList->SetTitleL( *header );
    CleanupStack::PopAndDestroy( header );  // header
    header = NULL;

    //-------------------------------------------
    // Check the available free space
    //
    TInt32 contentSize( 0 );
    TInt err = aDownload.GetIntAttribute( EDlAttrMultipleMOLength, contentSize );
    if ( err != KErrNone && err != KErrNotFound )
        {
        User::LeaveIfError( err );
        }

    // if size is not known it is set to -1. 0 size content size is ok here.
    TBool isContentSizeKnown = ( err == KErrNone && contentSize > -1 );

#ifdef RD_MULTIPLE_DRIVE
    if ( DiskSpaceBelowCriticalLevelL( aDownload, (TInt)contentSize ) )
#else
    if ( DiskSpaceBelowCriticalLevelL( (TInt)contentSize ) )
#endif
        {
        // Not enough free disk space!
        HBufC* notEnoughSpace =
            StringLoader::LoadLC( R_DMUL_DOWNLOADCONF_NOT_ENOUGH_MEM );
        itemArray->AppendL( *notEnoughSpace );
        CleanupStack::PopAndDestroy( notEnoughSpace );  // notEnoughSpace
        // and disable OK button
        popupList->ButtonGroupContainer()->MakeCommandVisible( EAknSoftkeyOk, EFalse );
        }

    //-------------------------------------------
    // Supported?  no need - this was confirmed earlier
    //

    //-------------------------------------------
    // Add name here
    // name/size/type/price(optional)/description(no)/vendor(no)
   HBufC* nameHeader = StringLoader::LoadLC( R_DMUL_DOWNLOADCONF_NAME );
   HBufC* dlName = HBufC::NewLC( KMaxPath );
   TPtr dlNamePtr = dlName->Des();
   User::LeaveIfError( aDownload.GetStringAttribute( EDlAttrName, dlNamePtr ) );

   itemText = FormatListBoxItemLC( *nameHeader, *dlName );
   itemArray->AppendL( *itemText );
   CleanupStack::PopAndDestroy( 3, nameHeader ); // itemText, sizeHeader
   dlName = NULL;
   itemText = NULL;
   nameHeader = NULL;

    //-------------------------------------------
    // Content Size
    //
    HBufC* sizeHeader = StringLoader::LoadLC( R_DMUL_DOWNLOADCONF_SIZE );
    if ( !isContentSizeKnown )
        {
        HBufC* unknownString = StringLoader::LoadL( R_DMUL_DOWNLOADCONF_UNKNOWN );
        iSizeInfoRes.Copy( unknownString->Left(KMaxDownloadSizeTextLength) );
        delete unknownString;
        }
    else
        {
        CProgressInfoCreator* progInfoCreator = CProgressInfoCreator::NewL();
        progInfoCreator->ProgressInfo( contentSize, iSizeInfoRes );
        delete progInfoCreator;
        progInfoCreator = NULL;
        }

    itemText = FormatListBoxItemLC( *sizeHeader, iSizeInfoRes );
    itemArray->AppendL( *itemText );
    CleanupStack::PopAndDestroy( 2, sizeHeader ); // itemText, sizeHeader
    itemText = NULL;
    sizeHeader = NULL;

    //-------------------------------------------
    // Application Name - only show if Supported
    //
    HBufC* handlerHeader = StringLoader::LoadLC( R_DMUL_DOWNLOADCONF_OPENWITH );

    RApaLsSession apaLs;
    User::LeaveIfError( apaLs.Connect() );
    CleanupClosePushL( apaLs );
    TUid appUid;
    HBufC8* contentType = iUiUtils.ContentTypeL( aDownload, ETrue );
    CleanupStack::PushL( contentType );

    #ifdef _DEBUG
    HBufC* contType = HBufC::NewL( contentType->Length() );
    TPtr ptr = contType->Des();
    ptr.Copy( *contentType );
    CLOG_WRITE_FORMAT(" contentType: %S",&ptr);
    delete contType;
    contType = NULL;
    #endif // _DEBUG

    User::LeaveIfError( apaLs.AppForDataType( TDataType(*contentType), appUid ) );
    HBufC* handlerName = ConstructHandlerAppNameLC( appUid, apaLs );

    itemText = FormatListBoxItemLC( *handlerHeader, *handlerName );
    CleanupStack::PopAndDestroy( itemText );

    CleanupStack::PopAndDestroy( handlerName );
    CleanupStack::Pop( contentType ); // Still needed - push later again
    CleanupStack::PopAndDestroy( &apaLs );
    CleanupStack::PopAndDestroy( handlerHeader );
    itemText = NULL;
    handlerName = NULL;
    handlerHeader = NULL;
    CleanupStack::PushL( contentType );


    //-------------------------------------------
    // MIME Content Type
    //
    HBufC* typeHeader = StringLoader::LoadLC( R_DMUL_DOWNLOADCONF_FILETYPE );

    HBufC* typeString = NULL;
    if ( contentType->Length() != 0 )
        {
        // 8 bit -> 16 bit conversion
        typeString = HBufC::NewLC( contentType->Length() );
        typeString->Des().Copy( *contentType );
        }
    else
        {
        typeString = StringLoader::LoadLC( R_DMUL_DOWNLOADCONF_UNKNOWN );
        }

    itemText = FormatListBoxItemLC( *typeHeader, *typeString );
    itemArray->AppendL( *itemText );
    CleanupStack::PopAndDestroy( 3, typeHeader ); // itemText, typeString , typeHeader
    CleanupStack::PopAndDestroy( contentType );
    itemText = NULL;
    typeString = NULL;
    typeHeader = NULL;
    contentType = NULL;

    //-------------------------------------------
    // Price: since dm has no info, we don't need this at all here
    //

    //-------------------------------------------
    // Description: since dm has no info, we don't need this at all here
    //

    //-------------------------------------------
    // Vendor: since dm has no info, we don't need this at all here
    //

    // Show popup list - it calls CActiveScheduler::Start(), so be careful
    // when using data members after ExecuteLD!
    iDownloadConfirmationShown = ETrue;
    iPopupList = popupList;
    TBool deleted( EFalse );
    iDownloadConfirmationDeletedPtr = &deleted;
    TInt pressedOk = popupList->ExecuteLD();
    CleanupStack::Pop( popupList );    // popupList
    if ( !deleted )
        {
        // We can use members only in this case!
        iDownloadConfirmationShown = EFalse;
        iPopupList = NULL;
        }

    // cleanup
    CleanupStack::PopAndDestroy( /*listBox*/ ); // listBox

    CLOG_WRITE_FORMAT(" pressedOk: %d",pressedOk);
    CLOG_LEAVEFN("CUserInteractionsEventHandler::ShowDownloadConfirmationL");
    return (TBool)pressedOk;
    }

// -----------------------------------------------------------------------------
// CUserInteractionsEventHandler::ConstructHandlerAppNameLC
// -----------------------------------------------------------------------------
//
HBufC* CUserInteractionsEventHandler::ConstructHandlerAppNameLC
    ( const TUid& aAppUid, RApaLsSession& aApaLs )
    {
    CLOG_ENTERFN("CUserInteractionsEventHandler::ConstructHandlerAppNameLC");

    HBufC* handlerName = NULL;
    if ( aAppUid == TUid::Null() )
        {
        // No handler found
        handlerName = StringLoader::LoadLC( R_DMUL_DOWNLOADCONF_UNKNOWN );
        }
    else
        {
        TApaAppInfo appInfo;
        User::LeaveIfError( aApaLs.GetAppInfo( appInfo, aAppUid ) );
        CLOG_WRITE_FORMAT(" appInfo.iFullName: %S",&appInfo.iFullName);
        CLOG_WRITE_FORMAT(" appInfo.iCaption: %S",&appInfo.iCaption);
        CLOG_WRITE_FORMAT(" appInfo.iShortCaption: %S",&appInfo.iShortCaption);
        // In many cases there is long caption. We should use short caption only
        // if it is empty. They say that sometimes the long caption is not empty,
        // but it contains only a space (0x20). In this case we must use the short.
        // For safety reasons, we will check the whole long caption, if it
        // contains only space characters.
        const TDesC& longCap = appInfo.iCaption;
        TBool longCapIsEmpty = (longCap.Length() == 0);
        if ( longCapIsEmpty == EFalse )
            {
            TInt longCapLength = longCap.Length();
            TBool containsOnlySpaces = ETrue; // We'll change it, if it's not true.
            for (TInt i = 0; i < longCapLength; ++i)
                {
                if ( TChar(longCap[i]).IsSpace() == EFalse )
                    {
                    containsOnlySpaces = EFalse;
                    break;
                    }
                }
            if ( containsOnlySpaces )
                {
                longCapIsEmpty = ETrue;
                }
            }

        if ( longCapIsEmpty )
            {
            // Using short caption
            handlerName = appInfo.iShortCaption.AllocLC();
            }
        else
            {
            // Using long caption
            handlerName = appInfo.iCaption.AllocLC();
            }
        }

    CLOG_LEAVEFN("CUserInteractionsEventHandler::ConstructHandlerAppNameLC");
    return handlerName;
    }

// -----------------------------------------------------------------------------
// CUserInteractionsEventHandler::FormatListBoxItemLC
// -----------------------------------------------------------------------------
//
HBufC* CUserInteractionsEventHandler::FormatListBoxItemLC
    ( const TDesC& aFirst, const TDesC& aSecond )
    {
    HBufC* res = HBufC::NewLC( aFirst.Length() +
                               aSecond.Length() +
                               KListBoxSeparator().Length() );
    res->Des().Copy( aFirst );
    res->Des().Append( KListBoxSeparator );
    res->Des().Append( aSecond );
    return res;
    }

// -----------------------------------------------------------------------------
// CUserInteractionsEventHandler::HandleInProgressStateL
// -----------------------------------------------------------------------------
//
void CUserInteractionsEventHandler::HandleInProgressStateL( TBool& aThisDeleted )
    {
    CLOG_ENTERFN("CUserInteractionsEventHandler::HandleInProgressStateL");

    // GET - download is in progress - user can cancel
    if( iEvent.iProgressState == EHttpProgCodDownloadStarted && iEvent.iDownloadState == EHttpDlInprogress )
        {
        iRegistryModel.DownloadsList().CancelDisplayingDownloadsList();
        }
    if ( iEvent.iProgressState == EHttpContTypeRecognitionAvail )
        {
        // Don't show confirmation query for COD downloads
        TBool isCodDownload( EFalse );
		TBool userConfirms( EFalse );
        User::LeaveIfError( iDownload.GetBoolAttribute
                          ( EDlAttrCodDownload, isCodDownload ) );
        if ( isCodDownload )
            {
            // Do nothing.
            }
        else
            {
            // Check if multipart
            HBufC8* contentType = HBufC8::NewLC( KMaxContentTypeLength );
            TPtr8 contentTypePtr = contentType->Des();
            User::LeaveIfError
                ( iDownload.GetStringAttribute( EDlAttrContentType, contentTypePtr ) );
            if( 0 == contentType->Compare( KMultiPartMimeType ) )
                {
                // Don't show confirmation query yet, but
                // observe EHttpProgSupportedMultiPart.
                }
            else
                {
                iUiUtils.IsContentTypeSupportedL( iDownload );
                TBool suppressDownloadConfirmation( EFalse );
                iRegistryModel.UserInteractions().GetBoolAttributeL
                    ( CDownloadMgrUiUserInteractions::EAttrSuppressDownloadConfirmation,
                      suppressDownloadConfirmation );
                CLOG_WRITE_FORMAT(" suppressDownloadConfirmation: %d",
                                    suppressDownloadConfirmation);

                if ( suppressDownloadConfirmation )
                    {
                    // Nothing to do.
                    }
                else
                    {
                    TInt32 statusCode(0);
                    User::LeaveIfError( iDownload.GetIntAttribute
                                      ( EDlAttrStatusCode, statusCode ) );
                    if ( statusCode == 206 ) // http status code
                        {
                        // Display information note (only if the DL is not visible)
                        if ( iRegistryModel.DownloadsListInstalled() )
                            {
                            if ( !iRegistryModel.DownloadsList().IsVisible() )
                                {
                                CUserInteractionsUtils::InfoNoteL
                                    ( R_DMUL_OK_NOTE, R_DMUL_NOTE_DOWNLOADING );
                                }
                            }
                        }
                    else
                        {
                        userConfirms = ShowDownloadConfirmationL
                            ( iDownload, R_AVKON_SOFTKEYS_OK_CANCEL__OK, EFalse );

                        // executes a wait dialog! Check if this is deleted meanwhile
                        if ( aThisDeleted )
                            {
                            CLOG_WRITE_FORMAT(" this deleted: %x", this);
                            CLOG_LEAVEFN("CUserInteractionsEventHandler::HandleInProgressStateL");
                            return;
                            }

                        if ( userConfirms )
                            {
                            TBool isLaunchType( EFalse );
                            if( iUiUtils.DrmDownloadL( iDownload ) )
                                {
                                // get content type of media file in drm content
                                HBufC8* ctype = iUiUtils.ContentTypeL( iDownload, ETrue );
                                if( KDrmInnerContentTypesToLaunch().Find( *ctype )
                                    != KErrNotFound )
                                    {
                                    isLaunchType = ETrue;
                                    }
                                delete ctype;
                                }
                            if( isLaunchType )
                                {
                                User::LeaveIfError( iDownload.SetIntAttribute(
                                    EDlAttrAction, ELaunch ) );
                                }
                            else
                                {
                                //part of error PNIO-73GEM3.Sis file progress should be visible to user
                                if( (KSisxApplication().Find( contentTypePtr)!= KErrNotFound) ||
                                    (KPipApplication().Find( contentTypePtr)!= KErrNotFound) ||
                                    (KWidgetMimeType().Find( contentTypePtr)!= KErrNotFound) ||
                                    (KSharingConfig().Find( contentTypePtr)!= KErrNotFound) )
                                    {
                                    isLaunchType = ETrue;
                                    User::LeaveIfError( iDownload.SetIntAttribute(
                                        EDlAttrAction, ELaunch ) );
                                    }
                                // Open the download list if there is no more
                                // download confirmation shown:
                                if ( iRegistryModel.UserInteractions().
                                    DownloadConfirmationsShown() == 0 )
                                    {
                                    if ( iRegistryModel.DownloadsListInstalled() )
                                        {
                                        iRegistryModel.DownloadsList().
                                            DisplayDownloadsListL( iDownload );
                                        }
                                    }
                                }
                            }
                        else
                            {
                            // The download must be removed from Downloads List
                            User::LeaveIfError( iDownload.Delete() );
                            CLOG_WRITE(" Delete OK");
                            }
                        }
                     }
                    if (userConfirms)
					{

	#ifdef __DMGR_PD_TESTHARNESS
                        iUiUtils.TestLaunchPdAppL(iDownload);
	#else
 		//#ifdef RD_BROWSER_PROGRESSIVE_DOWNLOAD

                        // check if it's PdLaunch action
		                TInt32 action(0);
                        User::LeaveIfError( iDownload.GetIntAttribute( EDlAttrAction, action ) );
                        if( (action & EPdLaunch) &&
                            iUiUtils.CanLaunchAsProgDownload( iDownload, iRegistryModel.DownloadsList(),EFalse ))
                            {
      						CLOG_WRITE( " launchPdAppL(iDownload)" );
						    iUiUtils.LaunchPdAppL(iDownload, ETrue);
						    }
		//#endif	//RD_BROWSER_PROGRESSIVE_DOWNLOAD
	#endif	//__DMGR_PD_TESTHARNESS
					}
                }
            CleanupStack::PopAndDestroy( contentType ); // contentType
            }
        }
    else if ( iEvent.iProgressState == EHttpProgCodPdAvailable )
        {
        TBool isCodPdAvailable( EFalse );
        User::LeaveIfError( iDownload.GetBoolAttribute
                          ( EDlAttrCodPdAvailable, isCodPdAvailable ) );
        if ( isCodPdAvailable )
            {

			#ifdef __DMGR_PD_TESTHARNESS
				iUiUtils.TestLaunchPdAppL(iDownload);
			#else
	            // check if it's PdLaunch action
	            TInt32 action(0);
                User::LeaveIfError( iDownload.GetIntAttribute( EDlAttrAction, action ) );
				if ( (action & EPdLaunch) &&
                    iUiUtils.CanLaunchAsProgDownload( iDownload, iRegistryModel.DownloadsList(),ETrue ))
					{
					iUiUtils.LaunchPdAppL(iDownload, ETrue);
					}
			#endif	//__DMGR_PD_TESTHARNESS
			}
        }
    else if ( iEvent.iProgressState == EHttpProgCodDescriptorAccepted )
        {
        // COD load accepted. Download is turned to visible, and
        // downloads list is shown.
        User::LeaveIfError( iDownload.SetBoolAttribute
                          ( EDlAttrHidden, EFalse ) );
        CLOG_WRITE(" SetBoolAttribute EDlAttrHidden OK");
        // If this download's ServiceFlow was running, it's not more
        // a ServiceFlow, thus the next postponed download can be
        // scheduled.
        if ( iRegistryModel.UserInteractions().IsCodServiceFlowRunning( iDownload ) )
            {
            iRegistryModel.UserInteractions().SchedulePostponedDownloadL();
            }
        // Display downloads list
        if ( iRegistryModel.DownloadsListInstalled() )
            {
            iRegistryModel.DownloadsList().
                DisplayDownloadsListL( iDownload );
            }
        }
    else if ( iEvent.iProgressState == EHttpProgCodLoadEnd )
        {
        // UI state must be checked before we set download to hidden!
        TBool isUiBusyNow = iRegistryModel.UserInteractions().IsUiBusy();
        // COD load has ended. Download is turned to hidden, removed from
        // downloads list, and restarted (ServiceFlow).
        CLOG_WRITE(" SetBoolAttribute EDlAttrHidden OK");

        // Restart if possible - start ServiceFlow.
        // (UserInteractions must be installed...)
        if ( isUiBusyNow )
            {
            // Not possible to do it. Postpone the Service Flow and
            // invoke when no more handler is running.
            // The download is already hidden!
            iRegistryModel.UserInteractions().PostponeCodHandlingL( iDownload );
             //Download will be handled when the UI becomes free.
            }
        else
            {
            CLOG_WRITE(" (Re)Starting...");
            User::LeaveIfError( iDownload.Start() );
            CLOG_WRITE(" (Re)Start OK");
            }
        }
    else if ( iEvent.iProgressState == EHttpProgSupportedMultiPart )
        {
        // Show download confirmation for multipart download, if appropriate.
        // Don't show confirmation query for COD downloads
        TBool isCodDownload( EFalse );
        User::LeaveIfError( iDownload.GetBoolAttribute
                          ( EDlAttrCodDownload, isCodDownload ) );
        if ( isCodDownload )
            {
            // Do nothing.
            }
        else
            {
            TBool suppressDownloadConfirmation( EFalse );
            iRegistryModel.UserInteractions().GetBoolAttributeL
                ( CDownloadMgrUiUserInteractions::EAttrSuppressDownloadConfirmation,
                  suppressDownloadConfirmation );
            CLOG_WRITE_FORMAT(" suppressDownloadConfirmation: %d",
                                suppressDownloadConfirmation);

            if ( suppressDownloadConfirmation )
                {
                // Nothing to do.
                }
            else
                {
                TBool userConfirms = ShowDownloadConfirmationL
                    ( iDownload, R_AVKON_SOFTKEYS_OK_CANCEL__OK, EFalse );

                // executes a wait dialog! Check if this is deleted meanwhile
                if ( aThisDeleted )
                    {
                    CLOG_WRITE_FORMAT(" this deleted: %x", this);
                    CLOG_LEAVEFN("CUserInteractionsEventHandler::HandleInProgressStateL");
                    return;
                    }

                if ( userConfirms )
                    {
                    // Open the download list if there is no more
                    // download confirmation shown:
                    if ( iRegistryModel.UserInteractions().
                                        DownloadConfirmationsShown() == 0 )
                        {
                        if ( iRegistryModel.DownloadsListInstalled() )
                            {
                            iRegistryModel.DownloadsList().
                                DisplayDownloadsListL( iDownload );
                            }
                        }
                    }
                else
                    {
                    // The download must be removed from Downloads List
                    User::LeaveIfError( iDownload.Delete() );
                    CLOG_WRITE(" Delete OK");
                    }
                }
            }
        }

    CLOG_LEAVEFN("CUserInteractionsEventHandler::HandleInProgressStateL");
    }


// -----------------------------------------------------------------------------
// CUserInteractionsEventHandler::HandlePausedStateL
// -----------------------------------------------------------------------------
//
void CUserInteractionsEventHandler::HandlePausedStateL( TBool& aThisDeleted )
    {
    CLOG_ENTERFN("CUserInteractionsEventHandler::HandlePausedStateL");

    // HEAD - download is paused - started if user confirms
    if ( iEvent.iProgressState == EHttpContentTypeReceived )
        {
        // Don't show confirmation query for COD downloads
        TBool isCodDownload( EFalse );
        User::LeaveIfError( iDownload.GetBoolAttribute
                          ( EDlAttrCodDownload, isCodDownload ) );
        if ( isCodDownload )
            {
            // COD downloads cannot be paused!
            }
        else
            {
            TBool suppressDownloadConfirmation( EFalse );
            iRegistryModel.UserInteractions().GetBoolAttributeL
                ( CDownloadMgrUiUserInteractions::EAttrSuppressDownloadConfirmation,
                  suppressDownloadConfirmation );
            CLOG_WRITE_FORMAT(" suppressDownloadConfirmation: %d",
                                suppressDownloadConfirmation);

            if ( suppressDownloadConfirmation )
                {
                // Nothing to do, just resume the download.
                User::LeaveIfError( iDownload.Start() );
                CLOG_WRITE(" Restart OK");
                }
            else
                {
                aThisDeleted = EFalse;
                TBool userConfirms = ShowDownloadConfirmationL
                                     ( iDownload, R_AVKON_SOFTKEYS_OK_CANCEL__OK, EFalse );

                // executes a wait dialog! Check if this is deleted meanwhile
                if ( aThisDeleted )
                    {
                    CLOG_WRITE_FORMAT(" this deleted: %x", this);
                    CLOG_LEAVEFN("CUserInteractionsEventHandler::HandlePausedStateL");
                    return;
                    }

                // Open Downloads List if user confirms:
                if ( userConfirms )
                    {
                    // Resume the download:
                    User::LeaveIfError( iDownload.Start() );
                    // and open the download list if there is no more
                    // download confirmation shown:
                    if ( iRegistryModel.UserInteractions().
                                        DownloadConfirmationsShown() == 0 )
                        {
                        if ( iRegistryModel.DownloadsListInstalled() )
                            {
                            iRegistryModel.DownloadsList().
                                DisplayDownloadsListL( iDownload );
                            }
                        }
                    }
                else
                    {
                    // The download must be removed from Downloads List
                    User::LeaveIfError( iDownload.Delete() );
                    CLOG_WRITE(" Delete OK");
                    }
                }
            }
        }

    CLOG_LEAVEFN("CUserInteractionsEventHandler::HandlePausedStateL");
    }

// -----------------------------------------------------------------------------
// CUserInteractionsEventHandler::HandleCompletedStateL
// -----------------------------------------------------------------------------
//
void CUserInteractionsEventHandler::HandleCompletedStateL()
    {
    CLOG_ENTERFN("CUserInteractionsEventHandler::HandleCompletedStateL");

    if ( iEvent.iProgressState == EHttpProgMovingContentFile )
        {
        // An asynchronous move has started. Do nothing.
        CLOG_WRITE(" Move has started");
        }
    else if ( iEvent.iProgressState == EHttpProgContentFileMoved || iEvent.iProgressState == EHttpProgContentFileMovedAndDestFNChanged )
        {
        // An asynchronous move has ended.

        CLOG_WRITE(" Move has ended");
        // After successful moving, display updated download list
        iRegistryModel.DownloadsList().DisplayDownloadsListL( iDownload );
        // Notify Media Gallery about new media file
        // first get path to gallery where file was moved
        HBufC* fileName = HBufC::NewLC( KMaxPath );
        TPtr fileNamePtr = fileName->Des();
        TInt attErr = iDownload.GetStringAttribute(
            EDlAttrDestFilename, fileNamePtr );
        CLOG_WRITE_FORMAT(" EDlAttrDestFilename: %S",&fileNamePtr);
        TBool havePath = (attErr == KErrNone) && (fileNamePtr.Length() > 0);

        #ifdef BRDO_APP_GALLERY_SUPPORTED_FF
        //
        CMGXFileManager* mgFileManager = MGXFileManagerFactory::NewFileManagerL(
            CEikonEnv::Static()->FsSession() );
        if( havePath )
            {
            TRAP_IGNORE( mgFileManager->UpdateL( fileNamePtr ) );
            TRAP_IGNORE( iUiUtils.UpdateDCFRepositoryL( fileNamePtr ) );
            }
        else
            {
            TRAP_IGNORE( mgFileManager->UpdateL() );
            }

        delete mgFileManager;
        mgFileManager = NULL;

        #else
         if( havePath )
            {
            TRAP_IGNORE( iUiUtils.UpdateDCFRepositoryL( fileNamePtr ) );
            }
        #endif

        CleanupStack::PopAndDestroy( fileName );

        }
    else
        {
        // Normal completed state. Handling depends on EDlAttrAction
        TInt32 action(0);
        User::LeaveIfError( iDownload.GetIntAttribute
                            ( EDlAttrAction, action ) );
        CLOG_WRITE_FORMAT(" EDlAttrAction: %d",action);
        if ( action == EDoNothing )
            {
            // Do nothing
            }
        else if ( action & EMove )
            {
            // check if COD download
            TBool isCodDownload( EFalse );
            User::LeaveIfError( iDownload.GetBoolAttribute
                              ( EDlAttrCodDownload, isCodDownload ) );

            // check if progressive download
            TBool isProgressive = EFalse;
            TInt err = iDownload.GetBoolAttribute
                ( EDlAttrProgressive, isProgressive );
            CLOG_WRITE_FORMAT(" EDlAttrProgressive err: %d",err);
            // 'err' is ignored.
            CLOG_WRITE_FORMAT(" EDlAttrProgressive: %d", isProgressive);

            // check if move is already issued
            TBool isMoveIssued = EFalse;
            TInt32 progState(0);
            User::LeaveIfError( iDownload.GetIntAttribute
                            ( EDlAttrProgressState, progState ) );

            if(progState == EHttpProgMovingContentFile) isMoveIssued = ETrue;

            if( isCodDownload )
                {
                HBufC8* contentType = HBufC8::NewLC( KMaxContentTypeLength );
                TPtr8 contentTypePtr = contentType->Des();
                User::LeaveIfError
                    ( iDownload.GetStringAttribute( EDlAttrContentType, contentTypePtr ) );
                if( !contentType->Compare(KRoapMimeType))
                    {
                    User::LeaveIfError
                    ( iDownload.Delete());
                    }
                CleanupStack::PopAndDestroy( contentType );
                }
             else if( isProgressive || isMoveIssued )
                {
                //do not move

                }
             else
                {
                // Move content to the location defined by KDownloadPath

                // first, get current EDlAttrDestFilename
			    HBufC* fileName = HBufC::NewLC( KMaxPath );
			    TPtr fileNamePtr = fileName->Des();
			    User::LeaveIfError
                    ( iDownload.GetStringAttribute( EDlAttrDestFilename, fileNamePtr ) );

                RFs& fs = CEikonEnv::Static()->FsSession();

#ifdef RD_MULTIPLE_DRIVE
                TDriveUnit currentDrive(fileNamePtr);

                // using PathInfo::RootPath to set correct destination folder
                // depending on memory used

                // Getting RootPath for selected Drive
                TFileName rootPath;
                User::LeaveIfError( PathInfo::GetRootPath( rootPath, currentDrive ) );

                // remove path from EDlAttrDestFilename
                if( fileNamePtr.Length() > 0 )
                    {
                    TInt lastSlashPos = fileNamePtr.LocateReverse( '\\' );
                    if( lastSlashPos != KErrNotFound )
                        {
                        fileNamePtr.Delete( 0, lastSlashPos );
                        }
                    }

                // Setting RootPath for new Destination file
                fileNamePtr.Insert( 0, rootPath );
                // Setting KDownloadPath
                fileNamePtr.Insert( rootPath.Length(), KDownloadPath );
#else

                TParse p;
                p.SetNoWild(fileNamePtr, NULL, NULL);
                TDriveUnit currentDrive(fileNamePtr);
                TFileName rootPath;
                User::LeaveIfError( PathInfo::GetRootPath( rootPath, currentDrive ) );

                TPtrC namePtr = p.NameAndExt();
                fileNamePtr.Format( _L("%S%S\\%S"), &rootPath,
                                                    &KDownloadPath,
                                                    &namePtr );
#endif
                // next, creating directory
                TInt error = 0;

                // next, get the directory path from new EDlAttrDestFilename
                if( fileNamePtr.Length() > 0 )
                    {
                    TInt lastSlashPos = fileNamePtr.LocateReverse( '\\' );
                    if( lastSlashPos != KErrNotFound )
                        {
                        error = fs.MkDirAll( fileNamePtr.LeftTPtr( lastSlashPos + 1 ) );
                        if ( error != KErrNone && error != KErrAlreadyExists )
                            // leave if makedir failed in some way
                            // don't leave if already exists
                            {
                            CLOG_WRITE_FORMAT( "MkDirAll: %d", error );
                            User::Leave( error );
                            }
                        }
                    }


                // update EDlAttrDestFilename with new path
                User::LeaveIfError
		                ( iDownload.SetStringAttribute( EDlAttrDestFilename, *fileName ) );
                // move file
                User::LeaveIfError( iDownload.Move() );
          	    CleanupStack::PopAndDestroy( fileName ); // fileName
                }
            }
        else if ( action & EPdLaunch )
        {
        	// do nothing since PdLaunch was already launched during the progress
        }
        else
            {
            // Defaulting to Launch
            iRegistryModel.UserInteractions().HandleDownloadL( iDownload );
            }
        }

    CLOG_LEAVEFN("CUserInteractionsEventHandler::HandleCompletedStateL");

    }

// -----------------------------------------------------------------------------
// CUserInteractionsEventHandler::HandleFailedStateL
// -----------------------------------------------------------------------------
//
void CUserInteractionsEventHandler::HandleFailedStateL()
    {
    CLOG_ENTERFN("CUserInteractionsEventHandler::HandleFailedStateL");

    // check if it is COD download
    TBool isCodDownload( EFalse );
    User::LeaveIfError( iDownload.GetBoolAttribute
                      ( EDlAttrCodDownload, isCodDownload ) );
    CLOG_WRITE_FORMAT("EDlAttrCodDownload: %d",isCodDownload);
    if ( isCodDownload )
        {
        // If the ServiceFlow of the download was running, but failed,
        // schedule the next download for running.
        // UserInteractions is assumed to be installed.
        // Do not delete the download yet (because IsCodServiceFlowRunning needs it)!
        if ( iRegistryModel.UserInteractions().IsCodServiceFlowRunning( iDownload ) )
            {
            iRegistryModel.UserInteractions().SchedulePostponedDownloadL();
            }
        // Nothing to do with COD - just delete it.
        HandleUnrecoverableFailL();
        }
    else // NORMAL download
        {
        // What is the error?
        TInt32 errorId( KErrNone ); // DMgr specific error ID
        User::LeaveIfError( iDownload.GetIntAttribute( EDlAttrErrorId, errorId ) );
        CLOG_WRITE_FORMAT(" Fail reason: %d", errorId);
        //
        if ( errorId == EHttpAuthenticationFailed )
            {
            if ( iUiUtils.GetAndSetHttpAuthCredentialsL( iDownload ) )
                {
                // and continue download
                User::LeaveIfError( iDownload.Start() );
                }
            }
        else if ( errorId == EProxyAuthenticationFailed )
            {
            if ( iUiUtils.GetAndSetProxyAuthCredentialsL( iDownload ) )
                {
                // and continue download
                User::LeaveIfError( iDownload.Start() );
                }
            }
        else if ( errorId == EHttpRestartFailed )
            {
            // Do not handle it.
            }
        else
            {
            TBool errorHandled( EFalse );

            if ( !errorHandled )
                {
                iUiUtils.ShowErrorNoteL( iDownload, errorId );
                // This component is responsible for deleting
                // not recoverable downloads
                if ( iUiUtils.IsRecoverableFailL( iDownload, errorId ) )
                    {
                    // Do nothing.
                    }
                else
                    {
                    // Delete the download
                    HandleUnrecoverableFailL();
                    }
                }
            }
        }

    CLOG_LEAVEFN("CUserInteractionsEventHandler::HandleFailedStateL");
    }

// -----------------------------------------------------------------------------
// CUserInteractionsEventHandler::HandleUnrecoverableFailL
// -----------------------------------------------------------------------------
//
void CUserInteractionsEventHandler::HandleUnrecoverableFailL()
    {
    CLOG_ENTERFN("CUserInteractionsEventHandler::HandleUnrecoverableFailL");

    HBufC* dlName = HBufC::NewLC( KMaxPath );
    TPtr dlNamePtr = dlName->Des();
    User::LeaveIfError( iDownload.GetStringAttribute( EDlAttrName, dlNamePtr ) );
    CLOG_WRITE_FORMAT(" EDlAttrName: %S",&dlNamePtr);

    // If download name is empty, use the file name got from URI
    if ( dlName->Length() == 0 )
        {
        HBufC* url = HBufC::NewLC( KMaxUrlLength );
        TPtr urlPtr = url->Des();
        TInt err = iDownload.GetStringAttribute( EDlAttrCurrentUrl, urlPtr );
        CLOG_WRITE_FORMAT(" EDlAttrCurrentUrl err: %d",err);
        if ( err != KErrNone && err != KErrNotFound )
            {
            User::LeaveIfError( err );
            }
        CLOG_WRITE_FORMAT(" EDlAttrCurrentUrl: %S",url);
        if ( url->Length() > 0 )
            {
            // Extract file name, if possible
            CUri8* converted = UriUtils::CreateUriL( *url );
            CleanupStack::PushL( converted );
            HBufC* extracted = converted->Uri().GetFileNameL( EUriFileNameTail );
            CleanupStack::PushL( extracted );
            // Use this for download name
            // (Left() is used to avoid overflow.)
            dlNamePtr.Copy( extracted->Left( dlNamePtr.MaxLength() ) );
            CleanupStack::PopAndDestroy( extracted );
            CleanupStack::PopAndDestroy( converted );
            }
        CleanupStack::PopAndDestroy( url );
        }

    // iDownload can now been deleted
    User::LeaveIfError( iDownload.Delete() );
    CLOG_WRITE(" Delete OK");

    HBufC* infoPrompt = StringLoader::LoadLC( R_DMUL_ERROR_CANCELLED, *dlName );
    CLOG_WRITE(" StringLoader OK");

    CAknNoteDialog* dlg = new (ELeave) CAknNoteDialog();
    dlg->PrepareLC( R_DMUL_DOWNLOAD_OK_INFO );
    dlg->SetTextL( *infoPrompt );
    dlg->RunLD();
    CLOG_WRITE(" RunLD OK");

    CleanupStack::PopAndDestroy( infoPrompt );
    CleanupStack::PopAndDestroy( dlName );

    CLOG_LEAVEFN("CUserInteractionsEventHandler::HandleUnrecoverableFailL");
    }

// -----------------------------------------------------------------------------
// CUserInteractionsEventHandler::DoCancel
// -----------------------------------------------------------------------------
//
void CUserInteractionsEventHandler::DoCancel()
    {
    }

// -----------------------------------------------------------------------------
// CUserInteractionsEventHandler::RunL
// -----------------------------------------------------------------------------
//
void CUserInteractionsEventHandler::RunL()
    {
    CLOG_ENTERFN("CUserInteractionsEventHandler::RunL");
    CLOG_WRITE_FORMAT(" %x", this);
    CLOG_WRITE_EVENT("UsrIntEve",&iDownload,iEvent);

    // This RunL may show wait dialogs. Use an indicator for if this is deleted
    TBool deleted( EFalse );
    iDeletedPtr = &deleted;

    // Incase of completed and moved downloads, subsession will be closed and whole download info will be cached client side.
    // No need to check for subsession close
    if(!(iEvent.iDownloadState == EHttpDlMultipleMOCompleted && (iEvent.iProgressState == EHttpProgContentFileMoved || iEvent.iProgressState == EHttpProgContentFileMovedAndDestFNChanged )))
        {
        // Due to the postponed event handling, it may happen that somebody already
        // closed the download meanwhile. Check if it is still alive:
        if ( iDownload.SubSessionHandle() == 0 )
            {
            // No, it's already closed!! Do nothing.
            // Necessary task done in RunL. This object is no more necessary.
            CLOG_WRITE(" Zero handle!");
            delete this;
            CLOG_LEAVEFN("CUserInteractionsEventHandler::RunL");
            return;
            }
        }

    switch ( iEvent.iDownloadState )
        {
        //---------------------------------------------
        case EHttpDlCreated:
        //---------------------------------------------
            {
            break;
            }
        //---------------------------------------------
        case EHttpDlInprogress:
        //---------------------------------------------
            {
            HandleInProgressStateL( deleted );
            break;
            }
        //---------------------------------------------
        case EHttpDlPaused:
        //---------------------------------------------
            {
            HandlePausedStateL( deleted );
            break;
            }
        //---------------------------------------------
        case EHttpDlMultipleMOCompleted:
        //---------------------------------------------
            {
            HandleCompletedStateL();
            break;
            }
        //---------------------------------------------
        case EHttpDlMultipleMOFailed:
        //---------------------------------------------
            {
            HandleFailedStateL();
            break;
            }
        //---------------------------------------------
        case EHttpDlMoved:
        //---------------------------------------------
            {
            // Do nothing. Downloads List handles it.
            break;
            }
        //---------------------------------------------
        case EHttpDlMediaRemoved:
        //---------------------------------------------
            {
            iUiUtils.ShowMediaRemovedNoteL();
            break;
            }
        //---------------------------------------------
        default:
        //---------------------------------------------
            {
            break;
            }
        }

    CLOG_WRITE_FORMAT(" deleted: %d",deleted);
    if ( !deleted )
        {
        // Necessary task done in RunL. This object is no more necessary.
        delete this;
        }

    CLOG_LEAVEFN("CUserInteractionsEventHandler::RunL");
    }

// -----------------------------------------------------------------------------
// CUserInteractionsEventHandler::RunError
// -----------------------------------------------------------------------------
//
TInt CUserInteractionsEventHandler::RunError( TInt aError )
    {
    if ( aError )
        {
        TRAP_IGNORE( iUiUtils.ShowErrorNoteL( aError ) );
        }
    return KErrNone;
    }

// End of file.
