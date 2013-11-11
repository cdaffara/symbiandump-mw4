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
* Description:  ?Description
*
*/



// INCLUDE FILES
//#include <platform/mw/Browser_platform_variant.hrh>
#include    <Browser_platform_variant.hrh>
#include    "HeaderField.h"
#include    "HttpDownloadManagerServerEngine.h"
#include    "HttpClientApp.h"
#include    "FileExt.h"
#include    "HttpDownload.h"
#include    "HttpStorage.h"
#include    "HttpDownloadMgrLogger.h"
#include    "bautils.h"

#include    <commdbconnpref.h>
#include    <uri16.h>
#include    <f32file.h>
#include    <escapeutils.h>
#include    <httpfiltercommonstringsext.h>
#include    <tinternetdate.h>
#include    <sysutil.h>

// following line is temporary: AVKON dependency removal
#undef BRDO_APP_GALLERY_SUPPORTED_FF

#ifdef BRDO_APP_GALLERY_SUPPORTED_FF
#include    <MGXFileManagerFactory.h>
#include    <CMGXFileManager.h>
#endif

#include    <DcfEntry.h>
#include    <DcfRep.h>

#include    <pathinfo.h>

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
const TInt32 KDMgrInfoFileId = 0x2002;      // id to check if it's really an info file
const TInt32 KDmgrVersionNumber = 0x0301;   // version number of the info file
const TInt   KUrlFixChar = '_';
const TInt KMaxHeaderOfMultipart = 32000;
const TInt KRespSizeForRecognition = 1024;  //for THttpProgressState EHttpContTypeRecognitionAvail
const TInt KMinDataSizeToSend = (32*1024);  //for Browser Control to call NewDownloadL
const TInt KErrCodHttpDownloadPaused = -20045;  //Error code set by the CodHandler if the download is paused by the client
const TInt KHttp903LossOfService = 903; //Error code set by the CodHandler if connection is lost
const TInt KHttp954LoaderError = 954; //Error code set by the CodHandler if Loader error is found
_LIT8( KHttpScheme, "http" );
_LIT8( KHttpsScheme, "https" );
_LIT8( KDefDestFilename, "content.bin" );
_LIT8( KSchemeAddon, "://" );
_LIT8( KBoundary, "boundary=" );
_LIT8( KDoubleEOL, "\r\n\r\n" );
_LIT8( KContentType, "Content-Type: " );

#ifdef __SYNCML_DM_FOTA
_LIT8( KFotaMimeType, "application/vnd.nokia.swupd.dp2");
#endif
_LIT8( KDefaultAcceptHeader, "multipart/mixed, application/java-archive, application/java, application/x-java-archive, text/vnd.sun.j2me.app-descriptor, application/vnd.oma.drm.message, application/vnd.oma.drm.content, application/vnd.wap.mms-message, text/x-co/desc, application/vnd.oma.dd+xml, text/javascript, text/javascript, application/x-javascript, text/ecmascript, */*" );
_LIT8( KDRMOldContentType, "x-drm-old-content-type");           // old content type header to be added

_LIT( KIndexString, "(%d)" );
const TInt KMaxIndexStringLength = 16;   // max size of index string "(4294967296)"

_LIT8( KCookieUsage, "CookiesEnabled" );

const TInt KStringAttribMaxLengths[][2] = {
    {EDlAttrReqUrl, KMaxUrlLength},
    {EDlAttrRedirUlr, KMaxUrlLength},
    {EDlAttrCurrentUrl, KMaxUrlLength},
    {EDlAttrName, KMaxPath},
    {EDlAttrRealm,KMaxRealmLength},
    {EDlAttrUsername, KMaxDefAttrLength},
    {EDlAttrPassword, KMaxDefAttrLength},
    {EDlAttrProxyRealm, KMaxRealmLength},
    {EDlAttrProxyUsername, KMaxDefAttrLength},
    {EDlAttrProxyPassword, KMaxDefAttrLength},
    {EDlAttrDestFilename, KMaxPath},
    {EDlAttrContentType, KMaxContentTypeLength},
    {EDlAttrMediaType, KMaxContentTypeLength},
    {0,0}
    };

const TInt KDownloadInfoIncrSize = 2*4096;     // used for DownloadInfo

_LIT( KDownloadPath, "download");

// MACROS
//#define ?macro ?macro_def

#define _OMADLOTA2_MULTI_DOWNLOAD	(iCodDlData && iCodDlData->Count()>1)

// LOCAL CONSTANTS AND MACROS
const TInt KDMHttpErrorBase = -25000;
#define GLOBAL_HTTP_ERROR( err ) ( KDMHttpErrorBase - err )
const TInt  KErrMultipeObjectDownloadFailed = -20046;

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHttpDownload::CHttpDownload
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CHttpDownload::CHttpDownload( CHttpClientApp *aClientApp,
                              TInt32 aId,
                              CHttpClientAppInstance* aClAppInstance )
    : CActive( EPriorityStandard )
    , iId( aId )
    , iClientApp( aClientApp )
    , iClAppInstance( aClAppInstance )
    , iPort( KDefaultPort )
    , iDisconnectOnPause( ETrue )
    , iDisconnectOnReset( ETrue )
    , iCodDownload( 0 )
    , iFotaPckgId( KDefaultFotaPckgId )
    , iDate( 0, EJanuary, 1, 0, 0, 0, 0 )
    , iExpires( 0, EJanuary, 1, 0, 0, 0, 0 )
    , iDlState( EHttpDlMultipleMOStarted )
    , iAction( ELaunch )
    , iRestartAction( ERestartIfExpired )
    , iPausable( ETrue )
    , iPausableDRM( ETrue )
    , iDrmContentLengthValid( ETrue )
    , iMultiPart( EFalse )
    , iDlNameChanged(EFalse)
    {
    __ASSERT_DEBUG( iClientApp, DMPanic( KErrArgument ) );
    CLOG_CREATE;
    }

// -----------------------------------------------------------------------------
// CHttpDownload::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHttpDownload::ConstructL( const TDesC8& aUrl )
    {
    CLOG_NAME_2( _L("cl%x_d%x"), iClientApp->AppUid(), iId );
    LOGGER_ENTERFN( "ConstructL" );
    CLOG_WRITE8_1( "URL: %S", &aUrl );

    CActiveScheduler::Add( this );

    iResponseHeaders = new (ELeave) CArrayPtrFlat<CHeaderField>(2);
    iRequestHeaders = new (ELeave) CArrayPtrFlat<CHeaderField>(2);
    iEntityHeaders = new (ELeave) CArrayPtrFlat<CHeaderField>(2);
    iGeneralHeaders = new (ELeave) CArrayPtrFlat<CHeaderField>(2);
    iStorage = CHttpStorage::NewL( this );
    iActiveDownload = 1 ;
    iMoLength = 0;
    iMoDownloadCompleted = EFalse;
    CLOG_ATTACH( iStorage, this );

    iUrl = aUrl.AllocL();
    if( iUrl->Length() )
        // this is a new download
        {
        ParseRequestedUrlL();   // iCurrentUrl is initialized there
        ParseDownloadNameL();
        StoreDownloadInfoL();
        SetDownloadStatus( EHttpProgNone, EHttpDlMultipleMOStarted );
        }
    else
        {
        LoadDownloadInfoL();

        if( iDlState == EHttpDlInprogress )
            // previous crash left this download here
            // reseting download can solve the problem
            {
            ++iDontFireEvent;

            TRAPD(err, PauseL() );

            --iDontFireEvent;

            User::LeaveIfError( err );
            }
        }

    if( !iStorage->CheckContentFileIntegrityL() )
        {
        if (iStorage->ProgressiveDownload() &&
             iStorage->Length() == iStorage->DownloadedSize() )
            // If it was a progressive download
            // and downloaded size = content length,
            // it means that download was not properly closed
            // in the previous session
            {
            ReInitializeDownload();
            }
        else
            {
            OnError( KErrUnknown, EContentFileIntegrity );
            }
        }

        iMoveInProgress = EFalse;
    }

// -----------------------------------------------------------------------------
// CHttpDownload::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHttpDownload* CHttpDownload::NewL( const TDesC8& aUrl,
                                    CHttpClientApp *aClientApp,
                                    TInt32 aId,
                                    CHttpClientAppInstance* aClAppInstance )
    {
    __ASSERT_DEBUG( aId, DMPanic( KErrArgument ) );
    CHttpDownload* self = new( ELeave ) CHttpDownload( aClientApp,
                                                       aId,
                                                       aClAppInstance );

    CleanupStack::PushL( self );
    self->ConstructL( aUrl );
    CleanupStack::Pop();

    return self;
    }


// Destructor
CHttpDownload::~CHttpDownload()
    {
    ++iDontFireEvent;

    Cancel();

    delete iUrl;
    delete iRedirUrl;
    delete iCurrentUrl;
    delete iHttpRealm;
    delete iHttpNonce;
    delete iHttpUsername;
    delete iHttpPassword;
    delete iHttpProxyRealm;
    delete iHttpProxyUsername;
    delete iHttpProxyPassword;
    delete iContentType;
    delete iDDType;
    delete iMediaType;
    delete iDlName;
	delete iDispositionType;
    delete iFileMan;


    delete iHashedMsgBody;
    delete iDownloadInfo;
    delete iAttachmentFileName;

    if( iTransValid )
        {
        iTrans.Close();
        }

    if( iResponseHeaders )
        {
        iResponseHeaders->ResetAndDestroy();
        delete iResponseHeaders;
        }

    if( iRequestHeaders )
        {
        iRequestHeaders->ResetAndDestroy();
        delete iRequestHeaders;
        }

    if( iEntityHeaders )
        {
        iEntityHeaders->ResetAndDestroy();
        delete iEntityHeaders;
        }

    if( iGeneralHeaders )
        {
        iGeneralHeaders->ResetAndDestroy();
        delete iGeneralHeaders;
        }

    delete iStorage;
    iStorage = NULL;
    delete iHeaderOfMultipart;

    iMoveInProgress = EFalse;

    CLOG_CLOSE;
    }

// -----------------------------------------------------------------------------
// CHttpDownload::Attach
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CHttpDownload::Attach( CHttpClientAppInstance* aClientAppInstance )
    {
    LOGGER_ENTERFN( "CHttpDownload::Attach" );

    __ASSERT_DEBUG( !iPDClAppInstance, DMPanic( KErrInUse) );
    iPDClAppInstance = aClientAppInstance;
    }

// -----------------------------------------------------------------------------
// CHttpDownload::StartL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CHttpDownload::StartL()
    {
    LOGGER_ENTERFN( "StartL" );

    __ASSERT_DEBUG( !iNoMedia, DMPanic( KErrAbort ) );
    if( iNoMedia )
        // nomedia download cannot be started
        {
        SetDownloadStatus( EHttpProgNone, EHttpDlFailed, EInternal );

        return;
        }

    if( iContinueDownload )
        // Don't do anything until user accepts the given content
        // to be downloaded.
        {
        CLOG_WRITE( "Continue download" );


        if( !iConnHandler )
            // assign download to the connhandler
            // connection is surely up because it was created from client side
            // iClAppInstance also must be valid because it's a continue download
            {
            iConnHandler = iClAppInstance->ConnHandler();
            }

        if( iDlState == EHttpDlMultipleMOStarted )
            // do it only if EHttpDlCreated, anyway ignore Start request
            {
            iStorage->CreateDestinationFileL();

            SetDownloadStatus( EHttpProgResponseHeaderReceived );
            }
        else if( iDlState == EHttpDlInprogress )
            {
            TriggerEvent( EHttpDlAlreadyRunning );
            }

        // This is the place where client already set every continue
        // download attribute
        StoreDownloadInfoL();

        return;
        }
    if( iCodDownload )
        {
        if(iDlState == EHttpDlPaused)
            {
            if( !iConnHandler )
                {
                iConnHandler = iClAppInstance->ConnHandler();
                }
            SetDownloadStatus( EHttpStarted );
            //TriggerEvent( EHttpDlInprogress, EHttpProgCodDownloadShouldResume );
            }
        else
            {
            TriggerEvent( EHttpDlInprogress, EHttpProgCodDownloadStarted );
            }
        return;
        }
    switch( iDlState )
        {
        case EHttpDlMultipleMOStarted:
            // normal start from the beginning
            break;

        case EHttpDlInprogress:

            if( iUpdatedDDUriSet )
                {
                ReInitializeDownload();

                iStorage->UpdateDestinationFilenameL( KNullDesC, EFalse );

                ParseRequestedUrlL();   // iCurrentUrl is initialized there
                ParseDownloadNameL();
                StoreDownloadInfoL();
                SetDownloadStatus( EHttpStarted );

               if( !iConnHandler )
                    // assign download to the connhandler
                    // connection is surely up because it was created from client side
                    // iClAppInstance also must be valid because it's a continue download
                    {
                    iConnHandler = iClAppInstance->ConnHandler();
                    }

                }
            else
                {
                TriggerEvent( EHttpDlAlreadyRunning );
                }
            return;

        case EHttpDlPaused:
            {
            if( IsExpired() || !iPausable )
                // cannot be resume:
                // expired
                // non-pauseable
                {
                ForcedRestartL();
                }
            }
            break;

        case EHttpDlMultipleMOCompleted:
            {
            if( iRestartAction == ERestartNoIfCompleted )
                // nothing to do because content is downloaded
                // and update check is not interested
                {
                TriggerEvent( EHttpDlMultipleMOCompleted );
                return;
                }
            else if( iRestartAction == ERestartForced )
                // doesn't matter what we have.
                // Download must be restarted from the very beginning
                {
                ForcedRestartL();
                }
            else if( iRestartAction == ERestartIfExpired )
                // First check that content is expired
                // If so, check for update on the server
                {
                if( !IsExpired() )
                    // content is still valid -> no need to download again
                    // Client has to use ERestartForced or Reset() to
                    // be able to download it again
                    {
                    TriggerEvent( EHttpDlMultipleMOCompleted );
                    return;
                    }
                }
            }
            break;

        case EHttpDlFailed:
            {
            if( NoMedia() )
                // we can't restart because original media is not present.
                // need a reset.
                {
                TriggerEvent( EHttpDlMediaRemoved );
                }
            else if( iRestartAction == ERestartNoIfCompleted )
                {
                if( iLastError == EContentExpired )
                    // Can't download content because it's expired.
                    // Use ERestartIfExpired, ERestartForced or Reset()
                    {
                    OnError( KErrUnknown, EContentExpired );
                    return;
                    }
                }
            }
            break;

        default:
            break;
        }

    if( iRedirect )
        // Requested url has to be restored, because of a previous redirection
        {
        ReallocateStringL( iCurrentUrl, *iRedirUrl, KMaxUrlLength );
        iRedirect = EFalse;
        }

    __ASSERT_DEBUG( iClAppInstance || iPDClAppInstance, DMPanic( KErrCorrupt ) );
    if (iPDClAppInstance && !iClAppInstance )
    //this is the case when browser is the main client and we  exit browser
    //in this case the PD client has to be made the main client.
        {
        iClAppInstance = iPDClAppInstance;
        iPDClAppInstance = NULL;
        }
    if( iClAppInstance )
        {
        if( !iConnHandler )
            {
            iConnHandler = iClAppInstance->ConnHandler();
            }

        iUseCookies = iClAppInstance->Cookies();
        }

    iDlStartedByClient = ETrue;

    SetDownloadStatus( EHttpStarted );

    }

// -----------------------------------------------------------------------------
// CHttpDownload::PauseL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CHttpDownload::PauseL( TBool aStore )
    {
    LOGGER_ENTERFN( "PauseL" );

    if( iDlState == EHttpDlPaused )
        // already paused
        {
        TriggerEvent( EHttpDlPaused );
        return;
        }

    //If the download is failed or completed, pause doesnt has any effect.
    if( iDlState == EHttpDlMultipleMOCompleted || iDlState == EHttpDlMultipleMOFailed )
        {
        return;
        }

    InternalPauseL( iDisconnectOnPause );

    //Pause the download only if it is pausable
    if( iDlState == EHttpDlInprogress )
        {
        if( Pausable() )
            // Do not change state when it's
            // created, pause, completed or failed.
            {
            SetDownloadStatus( EHttpProgNone, EHttpDlPaused );

            if( iCodDownload )
                {
                TriggerEvent( EHttpDlPaused, EHttpProgCodDownloadPause );
                }
            }
        else
            {
            //Else report error
            OnError( iGlobalErrorId, (THttpDownloadMgrError) iLastError );
            }
        }

    if( aStore && Pausable() )
        {
        StoreDownloadInfoL();
        }
    }

// -----------------------------------------------------------------------------
// CHttpDownload::Reset
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CHttpDownload::Reset()
    {
    LOGGER_ENTERFN( "Reset" );

    DoReset();
    }

// -----------------------------------------------------------------------------
// CHttpDownload::Delete
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CHttpDownload::Delete( CHttpClientAppInstance* aClAppInstance )
    {
    CLOG_WRITE( "Delete()" );
    if( !ClientApp()->Engine()->IsEngineClosing() )
        {
        if( aClAppInstance )
            {
            if( iPDClAppInstance == aClAppInstance )
                {
                iPDClAppInstance = NULL;
                if( !iStorage->ProgressiveMode() )
                    {
                    return;//This Attribute tells if Delete is called from MP when Exiting from Browser
                    }
                if( iClAppInstance )
                    {
                    SetDownloadStatus( EHttpProgNone, EHttpDlDeleting );
                    return;
                    }
                }
            else if( iClAppInstance == aClAppInstance )
                {
                iClAppInstance = NULL;
                if( iPDClAppInstance )
                    {
                    SetDownloadStatus( EHttpProgNone, EHttpDlDeleting );
                    return;
                    }
                }
            }
        }

    // Do not inform user about that we are in deleteing state
    // This won't be reenabled at the end of this function so
    // don't call delete only from outside of this class.
    ++iDontFireEvent;

    TPath folder;
    TFileName file;

    // delete info file
    iClientApp->Engine()->DownloadInfoFolder( iClientApp, folder );
    file.Format( _L("%S%d"), &folder, iId );
    TInt err = iClientApp->Engine()->Fs().Delete( file );
    CLOG_WRITE_2( "Delete info file: %S, err : %d", &file, err );

    // delete cod info file
    iClientApp->Engine()->CODDownloadInfoFolder( iClientApp, folder );
    file.Format( _L("%S%d"), &folder, iId );
    //delete main info file
    err = iClientApp->Engine()->Fs().Delete( file );
    CLOG_WRITE_2( "Delete info file: %S, err : %d", &file, err );

    //delete subinfo file
    CFileMan* fileMan = CFileMan::NewL(iClientApp->Engine()->Fs() );
    file.Format( _L("%S%d_*"), &folder, iId );
    fileMan->Delete( file );
    delete fileMan;


    // to make sure the CHttpStorage releases the content file
    // and deletes it.
    CHttpStorage::TFileCloseOperation closeOp = CHttpStorage::EDeleteFile;

    if( iStorage->DestFNameSet() && iDlState == EHttpDlMultipleMOCompleted )
        // Do NOT delete the file: if the destination filename was
        // set by the client and download completed.
	        {
	        CLOG_WRITE("Keep file");
	        closeOp = CHttpStorage::EKeepFile;
	        }

	if( iStorage->RFileSetByClient())
	    {
	    TBool pausable;
        GetBoolAttributeL( EDlAttrPausable, pausable );
        if( iDlState == EHttpDlMultipleMOCompleted || ( iDlState == EHttpDlInprogress && pausable ) )
            {
            CLOG_WRITE("Keep file");
	        closeOp = CHttpStorage::EKeepFile;
            }

	    }
    if( iCodDlData )
	    {
	    // For User Delete when all the Downloads are moved to gallery
	    if( EHttpDlMultipleMOCompleted == iDlState && closeOp == CHttpStorage::EDeleteFile )
		    {
		    for(TInt i = 1; i <= iCodDlData->Count(); i++)
		    	{
		    	TPtrC fullName = ((*iCodDlData)[i])->DestFilename();
		    	ClientApp()->Engine()->Fs().Delete( fullName );
		    	}
		    }
		// For User Cancel to Cancel Multiple download, Current and Queued DLs will be delete, completed ones will be kept
		else if( EHttpDlInprogress == iDlState || EHttpDlPaused == iDlState || EHttpDlDeleting == iDlState)
			{
			for(TInt i = 1; i <= iCodDlData->Count(); i++)
		    	{
		    	if( EInProgress == ((*iCodDlData)[i])->State() || EFailed == ((*iCodDlData)[i])->State() )	// OnGoing and Queued will be deleted
			    	{
			    	TPtrC fullName = ((*iCodDlData)[i])->DestFilename();
			    	if(fullName.Length())
			    	    {
			    	    ClientApp()->Engine()->Fs().Delete( fullName );
			    	    }
			    	}
		    	//else if( ESucceeded == ((*iCodDlData)[i])->State() )
		    	// Completed ones will be kept and moved to Gallery
		    	else if( ESucceeded == ((*iCodDlData)[i])->State() )
			    	{
			    	MoveInDelete(i);
			    	}
		    	}
			}
	    }
    else
    	{
    	iStorage->CloseDestinationFile( closeOp );
    	}
    // when delete there's no store -> no leave
    //  aClAppInstance->ClientApp()->UnregisterDownload( this );
    }


// -----------------------------------------------------------------------------
// CHttpDownload::DeleteInfoFile
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CHttpDownload::DeleteInfoFile( CHttpClientAppInstance* aClAppInstance )
    {
    CLOG_WRITE( "DeleteInfoFile()" );

    // This should be called only after the completion of download
	if( ! (EHttpDlMultipleMOCompleted == iDlState &&
    		(
    		EHttpProgContentFileMoved == iProgState ||
    		EHttpProgContentFileMovedAndDestFNChanged == iProgState
    		)
    	))
		{
   		return;
		}

    // Do not inform user about that we are in deleteing state
    // This won't be reenabled at the end of this function so
    // don't call delete only from outside of this class.
    ++iDontFireEvent;

    TPath folder;
    TFileName file;

    // delete info file
    iClientApp->Engine()->DownloadInfoFolder( iClientApp, folder );
    file.Format( _L("%S%d"), &folder, iId );
    CLOG_WRITE_1( "Delete info file: %S", &file );
    iClientApp->Engine()->Fs().Delete( file );

    // delete cod info file
    iClientApp->Engine()->CODDownloadInfoFolder( iClientApp, folder );
    file.Format( _L("%S%d"), &folder, iId );
    CLOG_WRITE_1( "Delete info file: %S", &file );
    iClientApp->Engine()->Fs().Delete( file );

    //delete subinfo file
    CFileMan* fileMan = CFileMan::NewL(iClientApp->Engine()->Fs() );
    file.Format( _L("%S%d_*"), &folder, iId );
    fileMan->Delete( file );
    delete fileMan;


    // to make sure the CHttpStorage releases the content file
    // and deletes it.
    CHttpStorage::TFileCloseOperation closeOp = CHttpStorage::EDeleteFile;

    if( iStorage->DestFNameSet() && iDlState == EHttpDlMultipleMOCompleted )
        // Do NOT delete the file: if the destination filename was
        // set by the client and download completed.
        {
        CLOG_WRITE("Keep file");
        closeOp = CHttpStorage::EKeepFile;
        }

    iStorage->CloseDestinationFile( closeOp );

    // when delete there's no store -> no leave
    aClAppInstance->ClientApp()->UnregisterDownload( this );
    }


// -----------------------------------------------------------------------------
// CHttpDownload::MoveL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CHttpDownload::MoveL()
    {
    LOGGER_ENTERFN( "MoveL" );


    if(iMoveInProgress)
    {
    	CLOG_WRITE(" return MoveL  ");
    	return;
    }

    iMoveInProgress = ETrue;

    if( !iCodDownload && iDlState != EHttpDlMultipleMOCompleted )
        {
        User::Leave( KErrNotReady );
        }
    //Incase of COD downloads, Move should happen only after COD Load End
   if( iCodDownload && iProgState != EHttpProgCodLoadEnd )
		{
        User::Leave( KErrNotReady );
		}

    if( iProgState == EHttpProgMovingContentFile )
        {
        User::Leave( KErrInUse );
        }

        //File is already moved. Ignore the move
    if( iProgState == EHttpProgContentFileMovedAndDestFNChanged ||

      	iProgState ==  EHttpProgContentFileMoved )
        {
        return;
        }

    __ASSERT_DEBUG( !IsActive(), DMPanic( KErrInUse ) );
    //__ASSERT_DEBUG( !iFileMan, DMPanic( KErrInUse ) );

    if(_OMADLOTA2_MULTI_DOWNLOAD)
        {
		iMOMoved = 1;
		SetDownloadStatus( EHttpProgMovingContentFile, iDlState );
		MoveDownloadedMediaObjectL(iMOMoved);
		return;
        }


	RFs &rFs = iClientApp->Engine()->Fs();
	if(!iFileMan)
		{
		iFileMan = CFileMan::NewL(rFs);
		}

    CLOG_WRITE_1( "Src: %S", iStorage->LocalFilename() );
    CLOG_WRITE_1( "Dest: %S", iStorage->DestFilename() );
    UpdateDestFileNameL();

    HBufC* fileName = HBufC::NewLC( KMaxPath );
    TPtr fileNamePtr = fileName->Des();
    fileNamePtr = *iStorage->DestFilename();
    HBufC* uniqueName = NULL;
    TInt index( 0 );
    TBool bFound( EFalse );
    do
        {
        CreateIndexedNameL(uniqueName , fileNamePtr , index);
        if( !BaflUtils::FileExists( rFs , *uniqueName ) )
        //Check if file name exist in Destination path
        //Generate Unique name if exist
            {
            bFound =ETrue;
            break;
            }
        iDlNameChanged = ETrue;

        }while( !bFound );
    CleanupStack::PopAndDestroy( fileName );

  	if( iDlNameChanged )
  	    {
  	    HBufC16* destFileName ;
  	    destFileName = iStorage->DestFilename();
  	    TPtr destFileNamePtr = destFileName->Des();
  	    destFileNamePtr.Replace(0, destFileNamePtr.Length() , *uniqueName);
  	    TInt lastSlashPos = destFileNamePtr.LocateReverse( '\\' );
  	    TPtr dlName = destFileNamePtr.MidTPtr(lastSlashPos +1 );
        TInt dotPos = iDlName->LocateReverse( '.' );
        if( dotPos == KErrNotFound )
            {
            //Remove Extension from dlName as it was not present in old iDlName
            dotPos = dlName.LocateReverse( '.' );
            if(dotPos != KErrNotFound )
                {
                dlName.Copy( dlName.Left(dotPos));
                }
            }
  	    //download name has changed
  	    ReallocateStringL( iDlName, dlName, KDownloadNameMaxSize );
  	    }

  	TInt err =iFileMan->Move( *iStorage->LocalFilename(),
                                        *iStorage->DestFilename(),
                                        CFileMan::EOverWrite,
                                        iStatus );
    if(err != KErrNone)
    	{
    	iMoveInProgress = EFalse;
    	CLOG_WRITE("setting iMoveInProgress false when move fails");
    	}

    User::LeaveIfError( err );
    // waiting for move to complete
    SetActive();
    SetDownloadStatus( EHttpProgMovingContentFile, iDlState );
    }

// -----------------------------------------------------------------------------
// CHttpDownload::GetIntAttributeL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CHttpDownload::GetIntAttributeL( THttpDownloadAttrib aAttribute,
							                   TInt32& aValue )
    {
    LOGGER_ENTERFN( "GetIntAttributeL" );
    CLOG_WRITE_1( "Attr(%d)", aAttribute );

    iNoRealError = ETrue;

    switch( aAttribute )
        {
        case EDlAttrState:
            {
            if(iDlState == EHttpDlMultipleMOCompleted )
                {
                 aValue = EHttpDlCompleted;
                }
            else if(iDlState == EHttpDlMultipleMOFailed )
                {
                aValue = EHttpDlFailed;
                }
            else
                {
                aValue = iDlState;
                }
            }
            break;

        case EDlAttrProgressState:
            {
            aValue = iProgState;
            }
            break;

        case EDlAttrUserData:
            {
            aValue = iUserData;
            }
            break;

        case EDlAttrId:
            {
            aValue = iId;
            }
            break;

        case EDlAttrMultipleMOLength:
            {
            if ( iCodDownload )
                {
                aValue = iMoLength;
                }
            else
                {
                aValue = iStorage->Length();
                }
            }
            break;

        case EDlAttrMultipleMODownloadedSize:
            {
            if( iCodDownload )
                {
                aValue = iStorage->MoDownloadedSize();
                }
            else
                {
                aValue = iStorage->DownloadedSize();
                }
            }
            break;

        case EDlAttrLength:
            {
            aValue = iStorage->Length();
            }
            break;

        case EDlAttrDownloadedSize:
            {
            aValue = iStorage->DownloadedSize();
            }
            break;

        case EDlAttrStatusCode:
            {
            aValue = iStatusCode;
            }
            break;

        case EDlAttrAction:
            {
            aValue = iAction;
            }
            break;

        case EDlAttrPort:
            {
            aValue = iPort;
            }
            break;

        case EDlAttrRestartAction:
            {
            aValue = iRestartAction;
            }
            break;

        case EDlAttrErrorId:
            {
            aValue = iLastError;
            }
            break;

        case EDlAttrGlobalErrorId:
            {
            aValue = iGlobalErrorId;
            }
            break;

        case EDlAttrTargetApp:
            {
            aValue = iTargetApp;
            }
            break;

        case EDlAttrAuthScheme:
            {
            aValue = iAuthScheme;
            }
            break;

        case EDlAttrRequestHeaderAddonLength:
            {
            aValue = 0;

            for( TInt i = 0; i < iRequestHeaders->Count(); ++i )
                {
                aValue += (*iRequestHeaders)[i]->FieldName()->Length() +
                          (*iRequestHeaders)[i]->FieldRawData()->Length() +
                          2;    // +2 = KColon + KHttpFieldSeparator
                }
            }
            break;

        case EDlAttrMethod:
            {
            aValue = iMethod;
            }
            break;

        case EDlAttrFotaPckgId:
            {
            aValue = iFotaPckgId;
            }
            break;

        case EDlAttrNumMediaObjects:
            {
            aValue = 1;
            if(iCodDlData)
                {
                aValue = iCodDlData->Count();
                }
            }
            break;
        case EDlAttrActiveDownload:
            {
            aValue = iActiveDownload;
            }
            break;
        case EDlAttrActivePlayedDownload:
            {
            aValue = iActivePlayedDownload;
            }
            break;

        default:
            {
            CLOG_WRITE_1( "Unknown int attrib: %d", aAttribute );
#ifdef __WINS__
            DMPanic( KErrArgument );
#else
            User::Leave( KErrArgument );
#endif
            }
            break;
        }

    iNoRealError = EFalse;
    }

// -----------------------------------------------------------------------------
// CHttpDownload::GetIntAttributeL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CHttpDownload::GetIntAttributeL( THttpDownloadAttrib aAttribute,
							                   TInt32& aMoIndex,
							                   TInt32& aValue )
    {
    LOGGER_ENTERFN( "GetIntAttributeL" );
    CLOG_WRITE_1( "Attr(%d)", aAttribute );

    if (!iCodDlData)
    	{
    	aValue = 0;
    	return;
    	}
    if ((aMoIndex <= KNonMoIndex) || (aMoIndex > iCodDlData->Count()))
    	{
    	User::Leave( KErrArgument );
    	}

    CMediaDataBase* mediaData = (*iCodDlData)[aMoIndex];
    iNoRealError = ETrue;

    switch( aAttribute )
        {
        case EDlAttrMultipleMOLength:
            {
			aValue = iMoLength;
            }
            break;

        case EDlAttrMultipleMODownloadedSize:
            {
            // Calculate downloaded size of current media object from
            // album downloaded size.
            TInt dlSize = iStorage->DownloadedSize();
            for (TInt index = 0; index < iCodDlData->Count(); ++index)
            	{
            	CMediaDataBase* moData = (*iCodDlData)[index+1];
            	if ((moData->State() == ESucceeded) || (moData->State() == EInProgress))
            		dlSize -= moData->DownloadedSize();
            	}

            aValue = dlSize;
            }
            break;

        case EDlAttrLength:
	        {
	        aValue = iStorage->Length();
	        }
	        break;

        case EDlAttrDownloadedSize:
            {
            aValue = iStorage->DownloadedSize();
            }
            break;

        case EDlAttrErrorId:
            {
            aValue = mediaData->LastErrorId();
            }
            break;

        case EDlAttrGlobalErrorId:
            {
            aValue = mediaData->GlobalErrorId();
            }
            break;

        case EDlAttrMethod:
            {
            aValue = mediaData->Method();
            }
            break;

        default:
            {
            CLOG_WRITE_1( "Unknown int attrib: %d", aAttribute );
#ifdef __WINS__
            DMPanic( KErrArgument );
#else
            User::Leave( KErrArgument );
#endif
            }
            break;
        }

    iNoRealError = EFalse;
    }

// -----------------------------------------------------------------------------
// CHttpDownload::GetBoolAttributeL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CHttpDownload::GetBoolAttributeL( THttpDownloadAttrib aAttribute,
								                TBool& aValue )
    {
    LOGGER_ENTERFN( "GetBoolAttributeL" );
    CLOG_WRITE_1( "Attr(%d): %d", aAttribute );

    iNoRealError = ETrue;

    switch( aAttribute )
        {
        case EDlAttrNoContentTypeCheck:
            {
            aValue = iNoContentTypeCheck;
            }
            break;

        case EDlAttrDisconnectOnReset:
            {
            aValue = iDisconnectOnReset;
            }
            break;

        case EDlAttrDisconnectOnPause:
            {
            aValue = iDisconnectOnPause;
            }
            break;

        case EDlAttrNoMedia:
            {
            aValue = iNoMedia;
            }
            break;

        case EDlAttrRedirected:
            {
            aValue = iRedirect;
            }
            break;

        case EDlAttrPausable:
            {
            aValue = iPausable;
            }
            break;

        case EDlAttrHidden:
            {
            aValue = iHidden;
            }
            break;

        case EDlAttrSilent:
            {
            aValue = iSilentMode;
            }
            break;

        case EDlAttrContinue:
            {
            aValue = iContinueDownload;
            }
            break;

        case EDlAttrCodDownload:
            {
            aValue = iCodDownload;
            }
            break;

        case EDlAttrProgressive:
            {
            aValue = iStorage->ProgressiveMode();
            }
            break;

        case EDlAttrDestRemovable:
            {
            aValue = iStorage->RemovableDest();
            }
            break;

        case EDlAttrCodPdAvailable:
            {
            aValue = iCodPdAvailable;
            }
            break;
        default:
            {
            CLOG_WRITE_1( "Unknown bool attrib: %d", aAttribute );

#ifdef __WINS__
            DMPanic( KErrArgument );
#else
            User::Leave( KErrArgument );
#endif
            }
            break;
        }

    iNoRealError = EFalse;
    }

// -----------------------------------------------------------------------------
// CHttpDownload::GetBoolAttributeL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CHttpDownload::GetBoolAttributeL( THttpDownloadAttrib aAttribute,
								                TInt32& aMoIndex,
								                TBool& aValue )
    {
    LOGGER_ENTERFN( "GetBoolAttributeL" );
    CLOG_WRITE_1( "Attr(%d): %d", aAttribute );

    if (!iCodDlData)
    	{
    	aValue = 0;
    	return;
    	}
    if ((aMoIndex <= KNonMoIndex) || (aMoIndex > iCodDlData->Count()))
    	{
    	User::Leave( KErrArgument );
    	}

    CMediaDataBase* mediaData = (*iCodDlData)[aMoIndex];
    iNoRealError = ETrue;

    switch( aAttribute )
        {
        case EDlAttrRedirected:
            {
            aValue = mediaData->Redirected();
            }
            break;

        case EDlAttrPausable:
            {
            aValue = mediaData->Pausable();
            }
            break;

        case EDlAttrProgressive:
            {
            aValue = mediaData->ProgressiveDownload();
            }
            break;

        case EDlAttrDestRemovable:
            {
            aValue = mediaData->DesRemovable();
            }
            break;

        default:
            {
            CLOG_WRITE_1( "Unknown bool attrib: %d", aAttribute );

#ifdef __WINS__
            DMPanic( KErrArgument );
#else
            User::Leave( KErrArgument );
#endif
            }
            break;
        }

    iNoRealError = EFalse;
    }

// -----------------------------------------------------------------------------
// CHttpDownload::GetStringAttributeL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC* CHttpDownload::GetStringAttributeL( THttpDownloadAttrib aAttribute,
                                                    TBool& aDelete )
    {
    LOGGER_ENTERFN( "GetStringAttributeL" );
    CLOG_WRITE_1( "Attr(%d): %d", aAttribute );

    iNoRealError = ETrue;
    HBufC* attr = NULL;
    HBufC8* attr8 = NULL;

    aDelete = EFalse;

    switch( aAttribute )
        {
        case EDlAttrReqUrl:
            {
            attr8 = iUrl;
            }
            break;

        case EDlAttrRedirUlr:
            {
            attr8 = iRedirUrl;
            }
            break;

        case EDlAttrCurrentUrl:
            {
            attr8 = iCurrentUrl;
            }
            break;

        case EDlAttrContentType:
            {
            attr8 = iContentType;
            }
            break;

        case EDlAttrDDType:
            {
            attr8 = iDDType;
            }
            break;

        case EDlAttrRealm:
            {
            attr8 = iHttpRealm;
            }
            break;

        case EDlAttrUsername:
            {
            attr8 = iHttpUsername;
            }
            break;

        case EDlAttrProxyUsername:
            {
            attr8 = iHttpProxyUsername;
            }
            break;

        case EDlAttrRequestHeaderAddon:
            {
            TChar colon( KColon );
            TInt32 length( 0 );

            GetIntAttributeL( EDlAttrRequestHeaderAddonLength, length );
            if( length )
                {
                attr8 = HBufC8::NewLC( length );

                aDelete = ETrue;

                for( TInt i = 0; i < iRequestHeaders->Count(); ++i )
                    {
                    TPtrC8 fieldName( *(*iRequestHeaders)[i]->FieldName() );
                    TPtrC8 fieldData( *(*iRequestHeaders)[i]->FieldRawData() );

                    attr8->Des().Append( fieldName );
                    attr8->Des().Append( colon );
                    attr8->Des().Append( fieldData );
                    attr8->Des().Append( KHttpFieldSeparator() );
                    }

                CleanupStack::Pop();
                }
            }
            break;

        case EDlAttrDestFilename:
            {
            attr = iStorage->DestFilename();
            }
            break;

        case EDlAttrLocalFileName:
            {
            attr = iStorage->LocalFilename();
            }
            break;

        case EDlAttrDdFileName:
        	{
        	attr = iStorage->DdFileName();
        	}
        	break;

        case EDlAttrName:
            {
            attr = iDlName;
            }
            break;

        // Response headers
//        case EDlAttrCharSet:
        case EDlAttrResponseCharSet:
        case EDlAttrResponseAge:
        case EDlAttrResponseETag:
        case EDlAttrResponseLocation:
        case EDlAttrResponseRetryAfter:
        case EDlAttrResponseServer:
        case EDlAttrResponseVary:
            {
            for( TInt i = 0; KResponseHeaderConvTable[i][0]; ++i )
                {
                if( KResponseHeaderConvTable[i][0] == aAttribute )
                    {
                    attr8 = HeaderFieldL( iResponseHeaders, KResponseHeaderConvTable[i][1] );
                    break;
                    }
                }
            }
            break;

        // Request headers
        case EDlAttrRequestAccept:
        case EDlAttrRequestAcceptCharSet:
        case EDlAttrRequestAcceptLanguage:
        case EDlAttrRequestExpect:
        case EDlAttrRequestFrom:
        case EDlAttrRequestHost:
        case EDlAttrRequestMaxForwards:
        case EDlAttrRequestPragma:
        case EDlAttrRequestReferer:
        case EDlAttrRequestUserAgent:
        case EDlAttrRequestVary:
            {
            for( TInt i = 0; KRequestHeaderConvTable[i][0]; ++i )
                {
                if( KRequestHeaderConvTable[i][0] == aAttribute )
                    {
                    attr8 = HeaderFieldL( iRequestHeaders, KRequestHeaderConvTable[i][1] );
                    break;
                    }
                }
            }
            break;

        // Entity header fields
        case EDlAttrEntityAllow:
        case EDlAttrEntityContentEncoding:
        case EDlAttrEntityContentLanguage:
        case EDlAttrEntityContentLocation:
        case EDlAttrEntityExpires:
        case EDlAttrEntityLastModified:
            {
            for( TInt i = 0; KEntityHeaderConvTable[i][0]; ++i )
                {
                if( KEntityHeaderConvTable[i][0] == aAttribute )
                    {
                    attr8 = HeaderFieldL( iEntityHeaders, KEntityHeaderConvTable[i][1] );
                    break;
                    }
                }
            for( TInt i = 0; KResponseHeaderConvTable[i][0]; ++i )
                {
                if( KResponseHeaderConvTable[i][0] == aAttribute )
                    {
                    attr8 = HeaderFieldL( iResponseHeaders, KResponseHeaderConvTable[i][1] );
                    break;
                    }
                }
            }
            break;

        // General header fields
        case EDlAttrGeneralCacheControl:
        case EDlAttrGeneralDate:
        case EDlAttrGeneralPragma:
        case EDlAttrGeneralVia:
        case EDlAttrGeneralWarning:
            {
            for( TInt i = 0; KGeneralHeaderConvTable[i][0]; ++i )
                {
                if( KGeneralHeaderConvTable[i][0] == aAttribute )
                    {
                    attr8 = HeaderFieldL( iGeneralHeaders, KGeneralHeaderConvTable[i][1] );
                    break;
                    }
                }
            for( TInt i = 0; KResponseHeaderConvTable[i][0]; ++i )
                {
                if( KResponseHeaderConvTable[i][0] == aAttribute )
                    {
                    attr8 = HeaderFieldL( iResponseHeaders, KResponseHeaderConvTable[i][1] );
                    break;
                    }
                }
            }
            break;

        case EDlAttrAlbumName:
            {
            if (iCodDlData && (iCodDlData->Name().Compare(KNullDesC)))
                {
                attr = iCodDlData->Name().AllocL();
                aDelete = ETrue;
                }
            else
                {
                attr = iDlName;
                }
            }
            break;

        default:
            {
                {
                CLOG_WRITE_1( "Unknown string8 attrib: %d", aAttribute );
#ifdef __WINS__
            DMPanic( KErrArgument );
#else
            User::Leave( KErrArgument );
#endif
                }
            }
            break;
        }

    if( !attr && !attr8 )
        {
        CLOG_WRITE_1( "NULL attrib: %d", aAttribute );
        User::Leave( KErrNotFound );
        }

    if( attr8 )
        // it is an 8-bit attribute
        // need to be converted to 16-bits
        {
        if( aDelete )
            {
            CleanupStack::PushL( attr8 );
            }

        ReallocateStringL( attr, *attr8 );
        if( aDelete )
            {
            CleanupStack::PopAndDestroy( attr8 );
            }

        // it is always true because of the conversion.
        aDelete = ETrue;
        }

    iNoRealError = EFalse;

    return attr;
    }

// -----------------------------------------------------------------------------
// CHttpDownload::GetStringAttributeL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC* CHttpDownload::GetStringAttributeL( THttpDownloadAttrib aAttribute,
                                                    TInt32& aMoIndex,
                                                    TBool& aDelete )
    {
    LOGGER_ENTERFN( "GetStringAttributeL" );
    CLOG_WRITE_1( "Attr(%d): %d", aAttribute );

	if (!iCodDlData)
    	{
    	return (KNullDesC().AllocL());
    	}
    if ((aMoIndex <= KNonMoIndex) || (aMoIndex > iCodDlData->Count()))
    	{
    	User::Leave( KErrArgument );
    	}

    CMediaDataBase* mediaData = (*iCodDlData)[aMoIndex];
    iNoRealError = ETrue;
    HBufC* attr = NULL;
    HBufC8* attr8 = NULL;

    aDelete = EFalse;

    switch( aAttribute )
        {
        case EDlAttrReqUrl:
            {
            attr8 = mediaData->Url().AllocL();
            aDelete = ETrue;
            }
            break;

        case EDlAttrRedirUlr:
            {
            attr8 = mediaData->RedirUrl().AllocL();
            aDelete = ETrue;
            }
            break;

        case EDlAttrContentType:
            {
            attr8 = mediaData->Type().AllocL();
            aDelete = ETrue;
            }
            break;

        case EDlAttrDestFilename:
            {
            attr = mediaData->DestFilename().AllocL();
            aDelete = ETrue;
            }
            break;

        case EDlAttrName:
            {
            if (aMoIndex == iActiveDownload)
            	{
            	attr = iDlName;
            	}
            else
	            {
	            attr = mediaData->Name().AllocL();
            	aDelete = ETrue;
    	        }
            }
            break;

        default:
            {
                {
                CLOG_WRITE_1( "Unknown string8 attrib: %d", aAttribute );
#ifdef __WINS__
            DMPanic( KErrArgument );
#else
            User::Leave( KErrArgument );
#endif
                }
            }
            break;
        }

    if( !attr && !attr8 )
        {
        CLOG_WRITE_1( "NULL attrib: %d", aAttribute );
        User::Leave( KErrNotFound );
        }

    if( attr8 )
        // it is an 8-bit attribute
        // need to be converted to 16-bits
        {
        if( aDelete )
            {
            CleanupStack::PushL( attr8 );
            }

        ReallocateStringL( attr, *attr8 );
        if( aDelete )
            {
            CleanupStack::PopAndDestroy( attr8 );
            }

        // it is always true because of the conversion.
        aDelete = ETrue;
        }

    iNoRealError = EFalse;

    return attr;
    }

// -----------------------------------------------------------------------------
// CHttpDownload::GetString8AttributeL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* CHttpDownload::GetString8AttributeL( THttpDownloadAttrib aAttribute,
                                                      TBool& aDelete )
    {
    LOGGER_ENTERFN( "GetString8AttributeL" );
    CLOG_WRITE_1( "Attr(%d): %d", aAttribute );

    iNoRealError = ETrue;
    HBufC8* attr = NULL;
    HBufC* attr16 = NULL;

    aDelete = EFalse;

    switch( aAttribute )
        {
        case EDlAttrReqUrl:
            {
            attr = iUrl;
            }
            break;

        case EDlAttrRedirUlr:
            {
            attr = iRedirUrl;
            }
            break;

        case EDlAttrCurrentUrl:
            {
            attr = iCurrentUrl;
            }
            break;

        case EDlAttrRealm:
            {
            attr = iHttpRealm;
            }
            break;

        case EDlAttrUsername:
            {
            attr = iHttpUsername;
            }
            break;

        case EDlAttrProxyUsername:
            {
            attr = iHttpProxyUsername;
            }
            break;

        case EDlAttrContentType:
            {
            attr = iContentType;
            }
            break;

        case EDlAttrDDType:
            {
            attr = iDDType;
            }
            break;

        case EDlAttrRequestHeaderAddon:
            {
            TChar colon( KColon );
            TInt32 length( 0 );

            GetIntAttributeL( EDlAttrRequestHeaderAddonLength, length );
            if( length )
                {
                attr = HBufC8::NewLC( length );

                for( TInt i = 0; i < iRequestHeaders->Count(); ++i )
                    {
                    TPtrC8 fieldName( *(*iRequestHeaders)[i]->FieldName() );
                    TPtrC8 fieldData( *(*iRequestHeaders)[i]->FieldRawData() );

                    attr->Des().Append( fieldName );
                    attr->Des().Append( colon );
                    attr->Des().Append( fieldData );
                    attr->Des().Append( KHttpFieldSeparator() );
                    }

                CleanupStack::Pop();
                aDelete = ETrue;
                }
            }
            break;

        case EDlAttrResponseCharSet:
        // Response headers
        case EDlAttrResponseAge:
        case EDlAttrResponseETag:
        case EDlAttrResponseLocation:
        case EDlAttrResponseRetryAfter:
        case EDlAttrResponseServer:
        case EDlAttrResponseVary:
            {
            for( TInt i = 0; KResponseHeaderConvTable[i][0]; ++i )
                {
                if( KResponseHeaderConvTable[i][0] == aAttribute )
                    {
                    attr = HeaderFieldL( iResponseHeaders, KResponseHeaderConvTable[i][1] );
                    }
                }
            }
            break;

        // Request headers
        case EDlAttrRequestAccept:
        case EDlAttrRequestAcceptCharSet:
        case EDlAttrRequestAcceptLanguage:
        case EDlAttrRequestExpect:
        case EDlAttrRequestFrom:
        case EDlAttrRequestHost:
        case EDlAttrRequestMaxForwards:
        case EDlAttrRequestPragma:
        case EDlAttrRequestReferer:
        case EDlAttrRequestUserAgent:
        case EDlAttrRequestVary:
            {
            for( TInt i = 0; KRequestHeaderConvTable[i][0]; ++i )
                {
                if( KRequestHeaderConvTable[i][0] == aAttribute )
                    {
                    attr = HeaderFieldL( iRequestHeaders, KRequestHeaderConvTable[i][1] );
                    break;
                    }
                }
            }
            break;

        // Entity header fields
        case EDlAttrEntityAllow:
        case EDlAttrEntityContentEncoding:
        case EDlAttrEntityContentLanguage:
        case EDlAttrEntityContentLocation:
        case EDlAttrEntityExpires:
        case EDlAttrEntityLastModified:
            {
            for( TInt i = 0; KEntityHeaderConvTable[i][0]; ++i )
                {
                if( KEntityHeaderConvTable[i][0] == aAttribute )
                    {
                    attr = HeaderFieldL( iEntityHeaders, KEntityHeaderConvTable[i][1] );
                    break;
                    }
                }
            for( TInt i = 0; KResponseHeaderConvTable[i][0]; ++i )
                {
                if( KResponseHeaderConvTable[i][0] == aAttribute )
                    {
                    attr = HeaderFieldL( iResponseHeaders, KResponseHeaderConvTable[i][1] );
                    }
                }
            }
            break;

        // General header fields
        case EDlAttrGeneralCacheControl:
        case EDlAttrGeneralDate:
        case EDlAttrGeneralPragma:
        case EDlAttrGeneralVia:
        case EDlAttrGeneralWarning:
            {
            for( TInt i = 0; KGeneralHeaderConvTable[i][0]; ++i )
                {
                if( KGeneralHeaderConvTable[i][0] == aAttribute )
                    {
                    attr = HeaderFieldL( iGeneralHeaders, KGeneralHeaderConvTable[i][1] );
                    break;
                    }
                }
            for( TInt i = 0; KResponseHeaderConvTable[i][0]; ++i )
                {
                if( KResponseHeaderConvTable[i][0] == aAttribute )
                    {
                    attr = HeaderFieldL( iResponseHeaders, KResponseHeaderConvTable[i][1] );
                    }
                }
            }
            break;

        case EDlAttrHashedMsgBody:
            {
            attr = iHashedMsgBody;
            }
            break;

        case EDlAttrMediaType:
            {
            attr = iMediaType;
            }
            break;

        case EDlAttrMediaTypeBoundary:
            {
            attr = GetParamFromMediaTypeL( KBoundary() ).AllocL();
            aDelete = ETrue;
            }
            break;

        default:
            {
                {
                CLOG_WRITE_1( "Unknown string8 attrib: %d", aAttribute );
#ifdef __WINS__
            DMPanic( KErrArgument );
#else
            User::Leave( KErrArgument );
#endif
                }
            }
            break;
        }

    if( !attr && !attr16 )
        {
        CLOG_WRITE_1( "NULL attrib: %d", aAttribute );
        User::Leave( KErrNotFound );
        }

    if( attr16 )
        // it is an 8-bit attribute
        // need to be converted to 16-bits
        {
        if( aDelete )
            {
            CleanupStack::PushL( attr16 );
            }
        ReallocateStringL( attr, *attr16 );
        if( aDelete )
            {
            CleanupStack::PopAndDestroy( attr16 );
            }

        // it is always true because of the conversion.
        aDelete = ETrue;
        }

    iNoRealError = EFalse;

    return attr;
    }

// -----------------------------------------------------------------------------
// CHttpDownload::GetString8AttributeL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* CHttpDownload::GetString8AttributeL( THttpDownloadAttrib aAttribute,
                                                      TInt32& aMoIndex,
                                                      TBool& aDelete )
    {
    LOGGER_ENTERFN( "GetString8AttributeL" );
    CLOG_WRITE_1( "Attr(%d): %d", aAttribute );

	if (!iCodDlData)
    	{
    	return (KNullDesC8().AllocL());
    	}
    if ((aMoIndex <= KNonMoIndex) || (aMoIndex > iCodDlData->Count()))
    	{
    	User::Leave( KErrArgument );
    	}

    CMediaDataBase* mediaData = (*iCodDlData)[aMoIndex];
    iNoRealError = ETrue;
    HBufC8* attr = NULL;
    HBufC* attr16 = NULL;

    aDelete = EFalse;

    switch( aAttribute )
        {
        case EDlAttrReqUrl:
            {
            attr = mediaData->Url().AllocL();
            aDelete = ETrue;
            }
            break;

        case EDlAttrRedirUlr:
            {
            attr = mediaData->RedirUrl().AllocL();
            aDelete = ETrue;
            }
            break;

        case EDlAttrContentType:
            {
            attr = mediaData->Type().AllocL();
            aDelete = ETrue;
            }
            break;

        default:
            {
                {
                CLOG_WRITE_1( "Unknown string8 attrib: %d", aAttribute );
#ifdef __WINS__
            DMPanic( KErrArgument );
#else
            User::Leave( KErrArgument );
#endif
                }
            }
            break;
        }

    if( !attr && !attr16 )
        {
        CLOG_WRITE_1( "NULL attrib: %d", aAttribute );
        User::Leave( KErrNotFound );
        }

    if( attr16 )
        // it is an 8-bit attribute
        // need to be converted to 16-bits
        {
        if( aDelete )
            {
            CleanupStack::PushL( attr16 );
            }
        ReallocateStringL( attr, *attr16 );
        if( aDelete )
            {
            CleanupStack::PopAndDestroy( attr16 );
            }

        // it is always true because of the conversion.
        aDelete = ETrue;
        }

    iNoRealError = EFalse;

    return attr;
    }

// -----------------------------------------------------------------------------
// CHttpDownload::GetFileHandleAttributeL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C RFile* CHttpDownload::GetFileHandleAttributeL()
    {
    LOGGER_ENTERFN( "GetFileHandleAttributeL" );

    if( !iStorage->File() )
        // destination file is not opened
        {
        User::Leave( KErrArgument );
        }

    return iStorage->File();
    }

// -----------------------------------------------------------------------------
// CHttpDownload::SetIntAttributeL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CHttpDownload::SetIntAttributeL( THttpDownloadAttrib aAttribute,
                                               const TInt32 aValue )
    {
    LOGGER_ENTERFN( "SetIntAttributeL" );
    CLOG_WRITE_2( "Attr(%d): %d", aAttribute, aValue );
    TBool store( ETrue );

    iNoRealError = ETrue;

    switch( aAttribute )
        {
        case EDlAttrUserData:
            {
            iUserData = aValue;
            }
            break;

        case EDlAttrAction:
        	{
            TBool PdSupported(EFalse);

            iAction = (THttpDownloadMgrAction)aValue;

            if(iClAppInstance)
            	{
            	TRAP_IGNORE(iClAppInstance->GetBoolAttributeL(EDlMgrProgressiveDownload, PdSupported));
            	}

            if(!PdSupported)
                {
                iAction = (THttpDownloadMgrAction)(iAction & ~(EPdLaunch));
                }
        	}
            break;

        case EDlAttrPort:
            iPort = aValue;
            break;

        case EDlAttrRestartAction:
            iRestartAction = (THttpRestartActions)aValue;
            break;

        case EDlAttrAuthScheme:
            {
            if( iContinueDownload && iDlState == EHttpDlMultipleMOStarted )
                {
                iAuthScheme = aValue;
                }
            }
            break;

        case EDlAttrLength:
            {
            if( (iContinueDownload && iDlState == EHttpDlMultipleMOStarted) ||
                ( iCodDownload && iDlState == EHttpDlInprogress ) )
                {
                iStorage->SetLength( aValue );
                }
            else
                {
                store = EFalse;
                }
            }
            break;

        case EDlAttrMultipleMOLength:
            {
            iMoLength = aValue;
            }
            break;

        case EDlAttrDownloadedSize:
            {
            if( (iCodDownload || iContinueDownload) && iDlState == EHttpDlInprogress )
                {
                iStorage->SetDownloadedSize( aValue );
                // Calculate downloaded size of current media object from
                // album downloaded size.
                TInt dlSize = iStorage->DownloadedSize();
                if( iCodDlData )
                    {
                    for (TInt index = 0; index < iCodDlData->Count() ; ++index)
                    	{
                    	if( iActiveDownload - 1 == index )
                    	    {
                    	    //the size of active download has already been taken into account
                    	    continue;
                    	    }
                    	CMediaDataBase* moData = (*iCodDlData)[index+1];
                    	if ((moData->State() == ESucceeded) || (moData->State() == EInProgress))
                    		dlSize += moData->DownloadedSize();
                    	}
                    iStorage->SetMoDownloadedSize(dlSize);
                    }
                if( iStorage->Length() != KDefaultContentLength &&
                    iStorage->DownloadedSize() > iStorage->Length() )
                    // we don't know actually how many bytes will come down.
                    {
                    CLOG_WRITE( "Length modified" );
                    iStorage->SetLength( KDefaultContentLength );
                    StoreDownloadInfoL();
                    }
                TriggerEvent( EHttpDlInprogress, EHttpProgResponseBodyReceived );
                }

            store = EFalse;
            }
            break;

        case EDlAttrSucceeded:
            {
            if(!aValue)
                DownloadSucceededL();
            else
                iMoDownloadCompleted = ETrue;//Download of MO has been Completed so change State ,But PD is on
                                         //So MOVE will be issued by PD Client

            store = EFalse;
            }
            break;

        case EDlAttrFailed:
            {
            // This is an exeption!
            iNoRealError = EFalse;
            OnError( aValue, ETransactionFailed );
            store = EFalse;
            }
            break;

        case EDlAttrDefaultEvent:
            {
            }
            break;

        case EDlAttrMethod:
            {
            if( iContinueDownload && iDlState == EHttpDlMultipleMOStarted &&
                ( aValue == EMethodGET ||
                  aValue == EMethodPOST ||
                  aValue == EMethodHEAD ) )
                {
                iMethod = (THttpMethod)aValue;
                }
            else
                {
                User::Leave( KErrArgument );
                }
            }
            break;

        case EDlAttrFotaPckgId:
            {
            iFotaPckgId = aValue;
            store = EFalse;
            }
            break;
        case EDlAttrActiveDownload:
            {
            iActiveDownload = aValue;

            // Active MO changed. Notify clients.
           	TriggerEvent( EHttpDlCreated, EHttpProgNone );
            }
            break;

        case EDlAttrActivePlayedDownload:
            {
            iActivePlayedDownload = aValue;
            }

            break;
        default:
            {
#ifdef __WINS__
            DMPanic( KErrArgument );
#else
            User::Leave( KErrArgument );
#endif
            }
            break;
        }

    UpdatePausable();

    iNoRealError = EFalse;

    if( store )
        {
        StoreDownloadInfoL();
        }
    }

// -----------------------------------------------------------------------------
// CHttpDownload::SetBoolAttributeL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CHttpDownload::SetBoolAttributeL( THttpDownloadAttrib aAttribute,
								                const TBool aValue )
    {
    LOGGER_ENTERFN( "SetBoolAttributeL" );
    CLOG_WRITE_2( "Attr(%d): %d", aAttribute, aValue );
    TBool store( ETrue );

    iNoRealError = ETrue;

    switch( aAttribute )
        {
        case EDlAttrNoContentTypeCheck:
            {
            iNoContentTypeCheck = aValue;
            }
            break;

        case EDlAttrDisconnectOnReset:
            {
            iDisconnectOnReset = aValue;
            }
            break;

        case EDlAttrDisconnectOnPause:
            {
            iDisconnectOnPause = aValue;
            }
            break;

        case EDlAttrContinue:
            {
            if( iDlState == EHttpDlMultipleMOStarted )
                {
                iContinueDownload = aValue;
                }
            else
                {
                User::Leave( KErrInUse );
                }

            store = EFalse;
            }
            break;

        case EDlAttrHidden:
            {
            iHidden = aValue;
            }
            break;

        case EDlAttrSilent:
            {
            iSilentMode = aValue;
            }
            break;

        case EDlAttrCodDownload:
            {
            SetCodFlag( aValue );
            }
            break;

        case EDlAttrProgressive:
            {
            TBool PdSupported(EFalse);

            if(iClAppInstance)
            	{
            	TRAP_IGNORE(iClAppInstance->GetBoolAttributeL(EDlMgrProgressiveDownload, PdSupported));
            	}

            if(PdSupported)
                {
                iStorage->SetProgressiveMode( aValue );

                if(!aValue)
                    {
                    iActivePlayedDownload = 0;
                    }

                if (iCodDlData)
                	{
                	// Update for Active media object.
	                TInt active = iActiveDownload;
	                CMediaDataBase* mediaData = (*iCodDlData)[active];
	                mediaData->SetProgressiveDownload( aValue );
                	}

                if( iProgState != EHttpProgMovingContentFile )
                    {
                    if (aValue && iDlState == EHttpDlMultipleMOCompleted )
                        {
                        /*
                        if the file size is small, by the time the music player is launched, DMgr already triggered EHttpDlCompleted.
                        So if the download state is EHttpDlMultipleMOCompleted, we need to trigger EHttpDlCompleted so that music player understands download is completed already
                        This is needed to ensure backward compatibility
                        */
                        TriggerEvent( EHttpDlCompleted, EHttpDlProgProgressive);
                        }
                    else
                        {
                        TriggerEvent( iDlState, aValue ? EHttpDlProgProgressive : EHttpDlProgNonProgressive);
                        }
                    }
                }
            else
                {
				TriggerEvent( iDlState, EHttpDlProgNonProgressive);
				}
            }
            break;

        case EDlAttrCodDescriptorAccepted:
            {
            TriggerEvent( EHttpDlInprogress, EHttpProgCodDescriptorAccepted );
            }
            break;

        case EDlAttrCodLoadEnd:
            {
            SetDownloadStatus(EHttpProgCodLoadEnd,EHttpDlInprogress);
            }
            break;

        case EDlAttrCodPdAvailable:
            {
            iCodPdAvailable = aValue;
            TriggerEvent( EHttpDlInprogress, EHttpProgCodPdAvailable );
            }
            break;

        case EDlAttrDestRemovable:
            {
            if( iCodDownload )
                {
                iStorage->SetRemovableDest( aValue );

                if (iCodDlData)
                	{
	                // Update for Active media object.
	                TInt active = iActiveDownload;
	                CMediaDataBase* mediaData = (*iCodDlData)[active];
	                mediaData->SetDesRemovable( aValue );
                	}
                }
            }
            break;

        case EDlAttrDownloadUpdatedDDUri:
            {
            iUpdatedDDUriSet = aValue;
			}
            break;


        case EDlAttrCodPausable:
            {
            iPausable = aValue;

			// Update for Active media object.
            TInt active = iActiveDownload;
            CMediaDataBase* mediaData = (*iCodDlData)[active];
            mediaData->SetPausable( aValue );

            // inform client about change
            TriggerEvent( iPausable ? EHttpDlPausable : EHttpDlNonPausable );
			}
            break;


        default:
#ifdef __WINS__
            DMPanic( KErrArgument );
#else
            User::Leave( KErrArgument );
#endif
            break;
        }

    UpdatePausable();

    iNoRealError = EFalse;

    if( store )
        {
        StoreDownloadInfoL();
        }
    }



// -----------------------------------------------------------------------------
// CHttpDownload::SetStringAttributeL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CHttpDownload::SetStringAttributeL( THttpDownloadAttrib aAttribute, const TInt32 aMOIndex,
								                  const TDesC16& aValue )
    {
    LOGGER_ENTERFN( "SetStringAttributeL" );
    CLOG_WRITE_2( "Attr(%d - %d)", aAttribute, aValue.Length () );

    iNoRealError = ETrue;

    CheckAttribMaxLengthL( aAttribute, aValue );

    switch( aAttribute )
        {
        case EDlAttrDestFilename:
            {
            TBool updateFName = EFalse;

            if( iDlState == EHttpDlMultipleMOStarted ||
                iDlState == EHttpDlMultipleMOCompleted ||
                iCodDownload )
                // cannot set this attribute while
                // download is in progress
                {
                if( !aValue.Length() )
                    {
                    updateFName = ETrue;
                    }
                else if( iClientApp->Engine()->Fs().IsValidName( aValue ) )
                    {
                    TUint attrib;
                    TInt attErr = iClientApp->Engine()->Fs().Att( aValue, attrib );

                    if( (attErr == KErrNone && !(attrib & KEntryAttDir)) ||
                        attErr == KErrNotFound )
                        // File exists and not directory or not exists
                        {
                        updateFName = ETrue;
                        }
                    else
                        {
                        User::Leave( KErrArgument );
                        }
                    }
                else
                    // Invalid filename passed
                    {
                    User::Leave( KErrArgument );
                    }
                }
            else
                {
                User::Leave( KErrInUse );
                }

            if (updateFName)
            	{
                iStorage->UpdateDestinationFilenameL( aValue, ETrue );
                if (iCodDlData)
                	{
	            	CMediaDataBase* mediaData = (*iCodDlData)[aMOIndex];
	            	mediaData->SetDestFilenameL( aValue );
	            	}
            	}
            }
            break;
        }
    }


// -----------------------------------------------------------------------------
// CHttpDownload::SetStringAttributeL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CHttpDownload::SetStringAttributeL( THttpDownloadAttrib aAttribute,
								                  const TDesC16& aValue )
    {
    LOGGER_ENTERFN( "SetStringAttributeL" );
    CLOG_WRITE_2( "Attr(%d - %d)", aAttribute, aValue.Length () );
    TBool store( ETrue );

    iNoRealError = ETrue;

    CheckAttribMaxLengthL( aAttribute, aValue );

    switch( aAttribute )
        {
        case EDlAttrReqUrl:
            // this is only for internal testing usage!!!
            {
            if( iDlState == EHttpDlMultipleMOStarted )
                {
                ReallocateStringL( iUrl, aValue, KMaxUrlLength );
                ParseRequestedUrlL();   // iCurrentUrl is initialized there
                ParseDownloadNameL();
                }
            }
            break;

        case EDlAttrContentType:
            {
            if( iDlState == EHttpDlInprogress )
                {
                CLOG_WRITE( "EDlAttrContentType updated" );
                ReallocateStringL( iContentType, aValue, KMaxContentTypeLength );
                TriggerEvent( EHttpDlInprogress, EHttpProgContentTypeChanged );

                // Update for Active media object.
                HBufC8* type8 = HBufC8::NewLC( aValue.Length() );
                type8->Des().Copy( aValue );

                if (iCodDlData)
                	{
	                TInt active = iActiveDownload;
	                CMediaDataBase* mediaData = (*iCodDlData)[active];
	                mediaData->SetTypeL( type8->Des() );
	                CleanupStack::PopAndDestroy( type8 );
                	}
            	}
            }
            break;

        case EDlAttrRealm:
            {
            if( iContinueDownload && iDlState == EHttpDlMultipleMOStarted )
                {
                ReallocateStringL( iHttpRealm, aValue, KMaxRealmLength );
                }

            store = EFalse;
            }
            break;

        case EDlAttrUsername:
            {
            ReallocateStringL( iHttpUsername, aValue, KMaxDefAttrLength );
            store = EFalse;
            }
            break;

        case EDlAttrPassword:
            {
            ReallocateStringL( iHttpPassword, aValue, KMaxDefAttrLength );
            store = EFalse;
            }
            break;

        case EDlAttrProxyRealm:
            {
            if( iContinueDownload && iDlState == EHttpDlMultipleMOStarted )
                {
                ReallocateStringL( iHttpProxyRealm, aValue, KMaxRealmLength );
                }
            store = EFalse;
            }
            break;

        case EDlAttrProxyUsername:
            {
            ReallocateStringL( iHttpProxyUsername, aValue, KMaxDefAttrLength );
            store = EFalse;
            }
            break;

        case EDlAttrProxyPassword:
            {
            ReallocateStringL( iHttpProxyPassword, aValue, KMaxDefAttrLength );
            store = EFalse;
            }
            break;

        case EDlAttrDestFilename:
            {
            TBool updateFName = EFalse;

            if( iDlState == EHttpDlMultipleMOStarted ||
                iDlState == EHttpDlMultipleMOCompleted ||
                iCodDownload )
                // cannot set this attribute while
                // download is in progress
                {
                if( !aValue.Length() )
                    {
                    updateFName = ETrue;
                    }
                else if( iClientApp->Engine()->Fs().IsValidName( aValue ) )
                    {
                    TUint attrib;
                    TInt attErr = iClientApp->Engine()->Fs().Att( aValue, attrib );

                    if( (attErr == KErrNone && !(attrib & KEntryAttDir)) ||
                        attErr == KErrNotFound )
                        // File exists and not directory or not exists
                        {
                        updateFName = ETrue;
                        }
                    else
                        {
                        User::Leave( KErrArgument );
                        }
                    }
                else
                    // Invalid filename passed
                    {
                    User::Leave( KErrArgument );
                    }
                }
            else
                {
                User::Leave( KErrInUse );
                }

            if (updateFName)
            	{
            	iStorage->UpdateDestinationFilenameL( aValue, ETrue );

                if (iCodDlData)
                	{
                    TInt moIndex(0);
                    if( iActiveDownload != iActivePlayedDownload && iStorage->ProgressiveDownload() )
                        {
                        //MP has set the Destination File Name
                        moIndex = iActivePlayedDownload;
                        }
                    else
                        {
                        // Update for Active media object.
                    	moIndex = iActiveDownload;
                        }
	            	CMediaDataBase* mediaData = (*iCodDlData)[moIndex];
	            	mediaData->SetDestFilenameL( aValue );
	            	}
            	}
            }
            break;

        case EDlAttrLocalFileName:
			{
			iStorage->SetLocalFilenameL(aValue);
			}
			break;

        case EDlAttrRequestHeaderAddon:
            {
            if( aValue.Length() )
                {
                HBufC8* value = HBufC8::NewLC( aValue.Length() );

                value->Des().Copy( aValue );
                ParseRequestHeaderAddOnL( *value );

                CleanupStack::PopAndDestroy( value );
                }
            }
            break;

        case EDlAttrResponseHeader:
            {
            if( iContinueDownload && iDlState == EHttpDlMultipleMOStarted )
                {
                HBufC8* temp = HBufC8::NewLC( aValue.Length() );
                temp->Des().Copy( aValue );

                ContinueDownloadStoreResponseHeaderL( *temp );

                CleanupStack::PopAndDestroy( temp );
                }
            }
            break;

        case EDlAttrName:
            {
            if( iCodDownload )
                {
                SetDownloadNameL( aValue );

                if (iCodDlData)
	                {
	                // Update for Active media object.
	            	TInt active = iActiveDownload;
	            	CMediaDataBase* mediaData = (*iCodDlData)[active];
	            	mediaData->SetNameL( aValue );
	                }
                }
            }
            break;

        // Request headers
        case EDlAttrRequestAccept:
        case EDlAttrRequestAcceptCharSet:
        case EDlAttrRequestAcceptLanguage:
        case EDlAttrRequestExpect:
        case EDlAttrRequestFrom:
        case EDlAttrRequestHost:
        case EDlAttrRequestMaxForwards:
        case EDlAttrRequestPragma:
        case EDlAttrRequestReferer:
        case EDlAttrRequestUserAgent:
        case EDlAttrRequestVary:
            {
            HBufC8* value = HBufC8::NewLC( aValue.Length() );

            AddHeaderL( aAttribute, *value, KRequestHeaderConvTable, iRequestHeaders );

            CleanupStack::PopAndDestroy( value );
            }
            break;

        // General header fields
        case EDlAttrGeneralCacheControl:
        case EDlAttrGeneralPragma:
            {
            HBufC8* value = HBufC8::NewLC( aValue.Length() );

            AddHeaderL( aAttribute, *value, KGeneralHeaderConvTable, iGeneralHeaders );

            CleanupStack::PopAndDestroy( value );
            }
            break;

        default:
#ifdef __WINS__
            DMPanic( KErrArgument );
#else
            User::Leave( KErrArgument );
#endif
            break;
        }

    UpdatePausable();

    iNoRealError = EFalse;

    if( store )
        {
        StoreDownloadInfoL();
        }
    }

// -----------------------------------------------------------------------------
// CHttpDownload::SetStringAttributeL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CHttpDownload::SetStringAttributeL( THttpDownloadAttrib aAttribute,
								                  const TDesC8& aValue )
    {
    LOGGER_ENTERFN( "SetStringAttributeL(8)" );
    CLOG_WRITE_2( "Attr(%d - %d)", aAttribute, aValue.Length () );
    TBool store( ETrue );

    iNoRealError = ETrue;

    CheckAttribMaxLengthL( aAttribute, aValue );

    switch( aAttribute )
        {
        case EDlAttrReqUrl:
            // this is only for internal testing usage!!!
            {
            if( iDlState == EHttpDlMultipleMOStarted )
                {
                ReallocateStringL( iUrl, aValue, KMaxUrlLength );
                ParseRequestedUrlL();   // iCurrentUrl is initialized there
                ParseDownloadNameL();
                }
            }
            break;

        case EDlAttrContentType:
            {
            if( iDlState == EHttpDlInprogress )
                {
                ReallocateStringL( iContentType, aValue, KMaxContentTypeLength );
                TriggerEvent( EHttpDlInprogress, EHttpProgContentTypeChanged );
                }
            else if (iDlState == EHttpDlMultipleMOCompleted )//Allow to change content type even if state is completed
                {
                ReallocateStringL( iContentType, aValue, KMaxContentTypeLength );
                //No need to trigger any event
                }
            }
            break;

        case EDlAttrRealm:
            {
            if( iContinueDownload && iDlState == EHttpDlMultipleMOStarted )
                {
                ReallocateStringL( iHttpRealm, aValue, KMaxRealmLength );
                }
            store = EFalse;
            }
            break;

        case EDlAttrUsername:
            {
            ReallocateStringL( iHttpUsername, aValue, KMaxDefAttrLength );
            store = EFalse;
            }
            break;

        case EDlAttrPassword:
            {
            ReallocateStringL( iHttpPassword, aValue, KMaxDefAttrLength );
            store = EFalse;
            }
            break;

        case EDlAttrProxyRealm:
            {
            if( iContinueDownload && iDlState == EHttpDlMultipleMOStarted )
                {
                ReallocateStringL( iHttpProxyRealm, aValue, KMaxRealmLength );
                }
            store = EFalse;
            }
            break;

        case EDlAttrProxyUsername:
            {
            ReallocateStringL( iHttpProxyUsername, aValue, KMaxDefAttrLength );
            store = EFalse;
            }
            break;

        case EDlAttrProxyPassword:
            {
            ReallocateStringL( iHttpProxyPassword, aValue, KMaxDefAttrLength );
            store = EFalse;
            }
            break;

        case EDlAttrRequestHeaderAddon:
            {
            if( aValue.Length() )
                {
                ParseRequestHeaderAddOnL( aValue );
                }
            }
            break;

        case EDlAttrResponseHeader:
            {
            if( iContinueDownload && iDlState == EHttpDlMultipleMOStarted )
                {
                ContinueDownloadStoreResponseHeaderL( aValue );
                }
            }
            break;

        // Request headers
        case EDlAttrRequestAccept:
        case EDlAttrRequestAcceptCharSet:
        case EDlAttrRequestAcceptLanguage:
        case EDlAttrRequestExpect:
        case EDlAttrRequestFrom:
        case EDlAttrRequestHost:
        case EDlAttrRequestMaxForwards:
        case EDlAttrRequestPragma:
        case EDlAttrRequestReferer:
        case EDlAttrRequestUserAgent:
        case EDlAttrRequestVary:
            {
            AddHeaderL( aAttribute, aValue, KRequestHeaderConvTable, iRequestHeaders );
            }
            break;

        // General header fields
        case EDlAttrGeneralCacheControl:
        case EDlAttrGeneralPragma:
            {
            AddHeaderL( aAttribute, aValue, KGeneralHeaderConvTable, iGeneralHeaders );
            }
            break;

        case EDlAttrHashedMsgBody:
            {
            ReallocateStringL( iHashedMsgBody, aValue, KHashLength );
            }
            break;

        case EDlAttrRedirectedPermanently:
            {
            RedirectedPermanentlyL( aValue );

            if (iCodDlData)
	            {
	            // Update for Active media object.
	            TInt active = iActiveDownload;
	            CMediaDataBase* mediaData = (*iCodDlData)[active];
	            if (!mediaData->Redirected())
	            	mediaData->SetRedirUrlL( aValue );
	            }
            }
            break;

        case EDlAttrRedirectedTemporary:
            {
            RedirectedTemporaryL( aValue );

            if (iCodDlData)
	            {
	            // Update for Active media object.
	            TInt active = iActiveDownload;
	            CMediaDataBase* mediaData = (*iCodDlData)[active];
	            mediaData->SetRedirected( ETrue );
	            }
            }
            break;

        case EDlAttrContinueBody:
            {
            iNoRealError = EFalse;
            ResponseBodyReceivedL( aValue );
            store = EFalse;
            }
            break;

        case EDlAttrUpdatedDDUri:
            {
            ReallocateStringL( iUrl, aValue, KMaxUrlLength );
            }
            break;

        default:
#ifdef __WINS__
            DMPanic( KErrArgument );
#else
            User::Leave( KErrArgument );
#endif
            break;
        }

    UpdatePausable();

    iNoRealError = EFalse;

    if( store )
        {
        StoreDownloadInfoL();
        }
    }

// -----------------------------------------------------------------------------
// CHttpDownload::SetStringAttributeL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CHttpDownload::SetFileHandleAttributeL( RFile* aFile )
    {
    LOGGER_ENTERFN( "SetFileHandleAttributeL" );

    iStorage->AdoptFileHandleL( aFile );
    }

// -----------------------------------------------------------------------------
// CHttpDownload::SetDownloadDataAttributeL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CHttpDownload::SetDownloadDataAttributeL( HBufC8* dlData )
	{
    LOGGER_ENTERFN( "SetDownloadDataAttributeL" );

    if (iCodDlData)
        {
    	delete iCodDlData;
        iCodDlData = NULL;
        }

    iCodDlData = CDownloadDataServ::NewL(*dlData);
    TInt downloadedSize = 0 ;
    TBool activeDownloadChanged = EFalse;

    iMoLength = 0 ;
    for ( TInt i = 1; i <= iCodDlData->Count() ; ++i )
        {
        downloadedSize = downloadedSize + (*iCodDlData)[i]->DownloadedSize();
        iMoLength += (*iCodDlData)[i]->Size();

        // Below code is executed only when download is paused.
        // Set the ActiveDownload, DownloadedSize and Length.
        if (((*iCodDlData)[i]->State() == EInProgress) && !activeDownloadChanged)
            {
            iActiveDownload = i;
            iStorage->SetDownloadedSize( (*iCodDlData)[i]->DownloadedSize() );
            iStorage->SetLength( (*iCodDlData)[i]->Size() );
            activeDownloadChanged = ETrue;
            }
        }
    iStorage->SetMoDownloadedSize( downloadedSize );
    }
// -----------------------------------------------------------------------------
// CHttpDownload::SetTrackDataAttributeL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CHttpDownload::SetTrackDataAttributeL( TInt aIndex, HBufC8* dlData )
    {
    LOGGER_ENTERFN( "SetTrackDataAttributeL" );

	if (iCodDlData)
    	{
    	CMediaDataServ* updatedMediaData = CMediaDataServ::NewL(*dlData);
    	CMediaDataBase* mediaData = (*iCodDlData)[aIndex];
    	mediaData->SetNameL(updatedMediaData->Name());
    	mediaData->SetUrlL(updatedMediaData->Url());
    	mediaData->SetSize(updatedMediaData->Size());
    	mediaData->SetIconL( updatedMediaData->Icon() );
	    mediaData->SetSourceUriL( updatedMediaData->SourceUri() );
	    mediaData->SetProgressiveDownload( updatedMediaData->ProgressiveDownload() );
	    mediaData->SetState( updatedMediaData->State() );
	    mediaData->SetResult( updatedMediaData->Result() );
	    mediaData->SetDownloadedSize( updatedMediaData->DownloadedSize() );
	    mediaData->SetDestFilenameL( updatedMediaData->DestFilename() );
	    mediaData->SetTempFilenameL( updatedMediaData->DestFilename() );

    	mediaData->ResetTypes();
    	for (TInt type = 0; type < updatedMediaData->TypesCount(); ++type)
        	mediaData->AddTypeL( updatedMediaData->Types().MdcaPoint(type) );
        mediaData->SetStatusCode( updatedMediaData->StatusCode() );
        TInt result = updatedMediaData->Result();
        TInt statusCode = updatedMediaData->StatusCode();
        TBool pausable = updatedMediaData->Pausable();
        //New track download Begins.Hence make track size as 0
        //iStorage->SetDownloadedSize ( 0 );
        // Active MO completed OR failed. Notify clients.
        if (mediaData->Result() == 0)
            {
            iGlobalErrorId = 0;
	        iLastError = 0;
        	TriggerEvent( EHttpDlCompleted, EHttpProgNone );
            }
        else
            {
            // Set the error codes for single track albums
            if(iCodDlData->Count() == 1)
                {
                if ( result <= -1000 )
	        	    {
	        	    iGlobalErrorId = KErrGeneral;
	        	    iLastError = (THttpDownloadMgrError)result;
	           	    }
	            else
	        	    {
	        	    iGlobalErrorId = result;
	        	    iLastError = EGeneral;
	           	    }
                }

            //if the client pauses the download, Codhandler sets error code to KErrCodHttpDownloadPaused. In that case, no need to trigger EHttpDlFailed Event
            if ( !( result == KErrCodHttpDownloadPaused || ( pausable && statusCode == KHttp903LossOfService ) || ( pausable && statusCode == KHttp954LoaderError ) ) )
                {
    	        TriggerEvent( EHttpDlFailed, EHttpProgNone );
                }
            }
    	}
    }
// -----------------------------------------------------------------------------
// CHttpDownload::ClientAppInstance
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CHttpClientAppInstance* CHttpDownload::ClientAppInstance() const
    {
    return iClAppInstance;
    }

// -----------------------------------------------------------------------------
// CHttpDownload::OnError
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CHttpDownload::OnError( TInt aError,
                                      THttpDownloadMgrError aDlError )
    {
    LOGGER_ENTERFN( "OnError" );
    CLOG_WRITE_2( "OnError1: %d - %d", aError, aDlError );

    if( iNoRealError )
        // Leave occured in Get/Set function. This leave must NOT stop
        // download process.
        {
        CLOG_WRITE( "No real error" );
        iNoRealError = EFalse;

        return;
        }

    if( iCodDlData && iCodDlData->Count() > 1 )
        {
        if( aDlError == KErrMultipeObjectDownloadFailed )
            {
            //Permanently failed error in CodHandler
            //Delete the download
            SetDownloadStatus( EHttpProgNone,
                   EHttpDlMultipleMOFailed,
                   aDlError,
                   aError );
            return;
            }
        else
            {
            SetDownloadStatus( EHttpProgNone,
                               EHttpDlPaused,
                               aDlError,
                               aError );
            return;

            }
        }

    if( iDlState < EHttpDlCreated )
        // error occured during download object construction -> nothing to do here
        {
        return;
        }


    //For COD Downloads, the error code may be set from COD handler
    //In that case if the download is paused just set the global and download error

    if( iCodDownload && iDlState == EHttpDlPaused )
        {
        iLastError = aDlError;
        iGlobalErrorId = aError;
        TRAP_IGNORE( StoreDownloadInfoL() );
        return;
        }


    CancelTransaction();
    Cancel();
    Disconnect();
    // forget downloaded but not persisted content in case of OOM
    CHttpStorage::TFileCloseOperation closeOp = CHttpStorage::EKeepFile;
    if ( !Pausable() && ( iCodDlData && iCodDlData->Count() == 1 ) )
        {
        closeOp = CHttpStorage::EReplaceFile;
        }

    iStorage->CloseDestinationFile( closeOp );
    CLOG_WRITE_1( "OnError1 : iDlState : %d ", iDlState );

    if( iDlState != EHttpDlMultipleMOFailed )
        {

        // HTTP error code range (-7200 to -7399)
        if((aError <= KHttpErrorBase && aError >= KHttpErrorBase - 199)||
            aError == KErrConnectionTerminated ||
            aError == KErrCannotFindProtocol )
        	{
            if(!iPausable)
                {
                TriggerEvent( EHttpDlNonPausableNetworkLoss );
                }
            }

        if( aError == KErrHttpPartialResponseReceived )
            {
            //Partial response has been received and connection has been disconnected. This error will be
            //propagated to the client only, if the HTTP:ENotifyOnDisconnect property is set with a value
            //HTTP::EEnableDisconnectNotification

            //This error code was cancelling the pausable download. This error shud be ignored to keep the
            //paused download.
            //TSW Err ID : SXUU-77SRWL

            SetDownloadStatus( EHttpProgNone,
                               EHttpDlPaused,
                               aDlError,
                               aError );

            }
        else if( aDlError == EConnectionFailed && iPausable)
            {
            //Whenever connection error occurs and download can be paused.
            //Keep the download in paused state.
            SetDownloadStatus( EHttpProgNone,
                               EHttpDlPaused,
                               aDlError,
                               aError );
            }
        else if ( aDlError == EMMCRemoved )
            {
            // MMC removed. Pause the download.
            SetDownloadStatus( EHttpProgNone,
                               EHttpDlPaused,
                               aDlError,
                               aError );
            }
        else
            {

            SetDownloadStatus( EHttpProgNone,
                               EHttpDlMultipleMOFailed,
                               aDlError,
                               aError );
            if(!iCodDlData)
                {
              	TriggerEvent( EHttpDlFailed, EHttpProgNone );
              	}
            }
        }
    else if( ( EHttpDlMultipleMOFailed == iDlState ) && iCodDownload )
        {
        TriggerEvent( EHttpDlMultipleMOFailed );
        }

    TRAP_IGNORE( StoreDownloadInfoL() );
    }


// -----------------------------------------------------------------------------
// CHttpDownload::DetachClientInstance
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CHttpDownload::DetachClientInstance( CHttpClientAppInstance* aInstance )
    {
    if( iClAppInstance == aInstance )
        {
        iClAppInstance = NULL;
        }
    else if( iPDClAppInstance == aInstance )
        {
        iPDClAppInstance = NULL;
        }

    return (iPDClAppInstance || iClAppInstance);
    }


// -----------------------------------------------------------------------------
// CHttpDownload::ClientApp
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CHttpClientApp* CHttpDownload::ClientApp() const
    {
    return iClientApp;
    }

// -----------------------------------------------------------------------------
// CHttpDownload::PDClientAppInstance
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CHttpClientAppInstance* CHttpDownload::PDClientAppInstance() const
    {
    return iPDClAppInstance;
    }

// -----------------------------------------------------------------------------
// CHttpDownload::Id
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CHttpDownload::Id() const
    {
    return iId;
    }

// -----------------------------------------------------------------------------
// CHttpDownload::State
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
THttpDownloadState CHttpDownload::State() const
    {
    return iDlState;
    }

// -----------------------------------------------------------------------------
// CHttpDownload::ProgState
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
THttpProgressState CHttpDownload::ProgState() const
    {
    return iProgState;
    }

// -----------------------------------------------------------------------------
// CHttpDownload::ConnHandler
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CHttpConnHandler* CHttpDownload::ConnHandler() const
    {
    return iConnHandler;
    }

// -----------------------------------------------------------------------------
// CHttpDownload::NoMedia
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CHttpDownload::NoMedia() const
    {
    return iNoMedia;
    }

// -----------------------------------------------------------------------------
// CHttpDownload::MediaRemoved
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpDownload::MediaRemoved( TUint aUid, TBool aDontCheckMediaUid )
    {
    LOGGER_ENTERFN( "MediaRemoved" );
    CLOG_WRITE_3( "Uid: %d, dontCheck: %d, iNoMedia: %d",
                        aUid,
                        aDontCheckMediaUid,
                        iNoMedia );

    if( iNoMedia )
        // it is already known for us.
        {
        CLOG_WRITE("already know it");
        return;
        }

    if( !aDontCheckMediaUid && iStorage->GetDestinationDriveId() != aUid )
        // destination drive is not media card
        {
        CLOG_WRITE("who cares");
        return;
        }

    iNoMedia = ETrue;

    TRAP_IGNORE( InternalPauseL() );

    TriggerEvent( EHttpDlMediaRemoved );

    if( iDlState == EHttpDlInprogress )
        // it's happened during the download process
        // in case of e.g completed download it's not error
        {
        OnError( KErrGeneral, EMMCRemoved );
        }

    TRAP_IGNORE( StoreDownloadInfoL() );
    }

// -----------------------------------------------------------------------------
// CHttpDownload::MediaInserted
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpDownload::MediaInserted( TUint aUid )
    {
    LOGGER_ENTERFN( "MediaInserted" );

    if( iStorage->GetDestinationDriveId() != aUid )
        // this is not our media card
        {
        return;
        }

    iNoMedia = EFalse;
    TriggerEvent( EHttpDlMediaInserted );
    }

// -----------------------------------------------------------------------------
// CHttpDownload::GetDestinationDriveID
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CHttpDownload::GetDestinationDriveID() const
    {
    return (TInt)iStorage->GetDestinationDriveId();
    }

// -----------------------------------------------------------------------------
// CHttpDownload::SetClientInstance
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpDownload::SetClientInstance( CHttpClientAppInstance* aInstance,
                                       TBool aNoMoveEvent )
    {
    iClAppInstance = aInstance;

    if( iClAppInstance )
        {
        __ASSERT_DEBUG( iClAppInstance->Observer(), DMPanic( KErrCorrupt ) );
        if( iClAppInstance->Observer() && !aNoMoveEvent )
            // for this short moment download state becomes EHttpDlMoved
            // to inform client app about that it grabbed this download
            {
            TriggerEvent( EHttpDlMoved, iProgState );
            }
        }

    else
        // No reason to keep it PD.
        {
          if (iStorage)
		  {
          iStorage->SetProgressiveMode( EFalse );
		  }
        }
    }


// -----------------------------------------------------------------------------
// CHttpDownload::SetConnHandler
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpDownload::SetConnHandler( CHttpConnHandler* aConnHandler )
    {
    iConnHandler = aConnHandler;
    }

// -----------------------------------------------------------------------------
// CHttpDownload::Pausable
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CHttpDownload::Pausable() const
    {
    return iPausable;
    }

// -----------------------------------------------------------------------------
// CHttpDownload::SetDownloadNameL
// -----------------------------------------------------------------------------
//
void CHttpDownload::SetDownloadNameL( const TDesC& aNewName )
    {
    CLOG_WRITE_1( "New download name: [%S]", &aNewName );

    ReallocateStringL( iDlName, aNewName, KDownloadNameMaxSize );

    FixDownloadNameL();

    TriggerEvent( EHttpDlInprogress, EHttpProgDlNameChanged );
    }

// -----------------------------------------------------------------------------
// CHttpDownload::MHFRunL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpDownload::MHFRunL( RHTTPTransaction /*aTransaction*/,
                             const THTTPEvent& aEvent )
    {
    switch ( aEvent.iStatus )
        {
        case THTTPEvent::EGotResponseHeaders:
            {
            CLOG_WRITE( "Response header received" );
            ResponseHeaderReceivedL();
            }
            break;

        case THTTPEvent::EGotResponseBodyData:
            {
            MHTTPDataSupplier* respBody = iTrans.Response().Body();
            TPtrC8 buf;

            respBody->GetNextDataPart( buf );

            ResponseBodyReceivedL( buf );

            respBody->ReleaseData();
            }
            break;

        case THTTPEvent::EResponseComplete:
            {
            CLOG_WRITE( "Response complete" );
            }
            break;

        case THTTPEvent::ESucceeded:
            {
            CLOG_WRITE( "Transaction succeeded" );

            DownloadSucceededL();
            }
            break;

        case THTTPEvent::EFailed:
        case THTTPEvent::EMoreDataReceivedThanExpected:
        case THTTPEvent::EUnrecoverableError:
        case THTTPEvent::ETooMuchRequestData:
            {
            CLOG_WRITE_1( "Failed event: %d", aEvent.iStatus );
            if( iTransValid )
                {
                iTrans.Close();
                iTransValid = EFalse;
                }
            iDlStartedByClient = EFalse;
            }
            break;

        case THTTPEvent::ERedirectedPermanently:
            {
            RedirectedPermanentlyL( iTrans.Request().URI().UriDes() );
            }
            break;

        case THTTPEvent::ERedirectedTemporarily:
            {
            RedirectedTemporaryL( iTrans.Request().URI().UriDes() );
            }
            break;

        default:
            {
            CLOG_WRITE_1( "Event: %d", aEvent.iStatus );
            if( aEvent.iStatus < 0 )
                // error occured -> leave will be handled in OnError()
                {
                User::Leave( aEvent.iStatus );
                }
            }
            break;
        }
    }

// -----------------------------------------------------------------------------
// CHttpDownload::MHFRunError
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CHttpDownload::MHFRunError( TInt aError,
                                 RHTTPTransaction /*aTransaction*/,
                                 const THTTPEvent& /*aEvent*/ )
    {
    CLOG_WRITE_1( "MHFRunError: %d", aError );

    OnError( aError, ETransactionFailed );

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CHttpDownload::DoCancel
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpDownload::DoCancel()
    {
    LOGGER_ENTERFN( "DoCancel" );

    if( iProgState == EHttpProgMovingContentFile )
        {
        delete iFileMan;
        iFileMan = NULL;

        if( iStatus == KRequestPending )
            // No cancel function in CFileMan
            {
            SelfComplete( KErrCancel );
            }
        }
    }

// -----------------------------------------------------------------------------
// CHttpDownload::RunL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpDownload::RunL()
    {
    LOGGER_ENTERFN( "RunL" );

    switch( iProgState )
        // Keep progress states consistent with SetDownloadStatus()
        {
        case EHttpStarted:
            {
            iUpdatedDDUriSet = EFalse;
            Connect();
            }
            break;

        case EHttpProgConnected:
            {
            if( iStatus == KErrNone )
                {
                if( iCodDownload )
                    {
                    TriggerEvent( EHttpDlInprogress, EHttpProgCodDownloadShouldResume );
                    }
                else
                    {
                    RequestContentL();
                    }
                }

            }
            break;

        case EHttpProgMovingContentFile:
            // Move completed
            // State remains in completed
            // and move result stored in error attribs
            {
            CLOG_WRITE_1( "Move result: %d", iStatus.Int() );
            // In case of PDL for OMA2 the MP/VP moves the file after the playback is done.
			// So, the download status should be changed after that
			//but we again send the progress state as EHttpProgContentFileMoved because we need to display
			//where file is saved(saved to gallery)
			//Change Dl State to Download Completed if not already

		    if(_OMADLOTA2_MULTI_DOWNLOAD)
				{
				TPtrC fileNamePtr = ((*iCodDlData)[iMOMoved])->DestFilename();
				NotifyMediaGalleryL(fileNamePtr);

				// Initiate async move for the next media object
				iMOMoved++;
				if(iMOMoved <= iCodDlData->Count())
					{
					MoveDownloadedMediaObjectL(iMOMoved);
					break;
					}
				}

            SetDownloadStatus( iDlNameChanged ? EHttpProgContentFileMovedAndDestFNChanged : EHttpProgContentFileMoved,
                               iDlState = EHttpDlMultipleMOCompleted,
                               iStatus == KErrNone ? ENoError : EMoveFailed,
                               iStatus.Int() );

            if(iStatus == KErrNone)
            	{
            	iMoveInProgress = EFalse;
            	CLOG_WRITE("setting iMoveInProgress false when move is completed");
            	}


            delete iFileMan; iFileMan = NULL;

			if( !_OMADLOTA2_MULTI_DOWNLOAD)
				{
				TPtr fileNamePtr(iStorage->DestFilename()->Des());
				NotifyMediaGalleryL(fileNamePtr);
				}
            }
            break;

        default:
            {
            DMPanic( KErrUnknown );
            }
        }
    }

// -----------------------------------------------------------------------------
// CHttpDownload::RunError
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CHttpDownload::RunError(TInt aError)
    {
    // Untrapped leave occured in RunL() -> status must be update and
    // client app has to be informed.
    OnError( aError );

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CHttpDownload::Connected
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpDownload::Connected()
    {
    LOGGER_ENTERFN( "Connected" );

    if( iProgState == EHttpProgCreatingConnection )
        // In any other progress state we're not interested in this event
        {
        SetDownloadStatus( EHttpProgConnected, EHttpDlInprogress );
        }
    }

// -----------------------------------------------------------------------------
// CHttpDownload::Suspended
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpDownload::Suspended()
    {
    LOGGER_ENTERFN( "Suspended" );

    if( iDlState != EHttpDlInprogress )
        // we are not interested in this event,
        // only if the download is in progress.
        {
        return;
        }

    SetDownloadStatus( EHttpProgConnectionSuspended );
    }

// -----------------------------------------------------------------------------
// CHttpDownload::Disconnected
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpDownload::Disconnected()
    {
    LOGGER_ENTERFN( "Disconnected" );

    if( iDlState != EHttpDlInprogress )
        // we are not interested in this event,
        // only if the download is in progress.
        {
        return;
        }

    //Set the errors
    TRAP_IGNORE( PauseL( ETrue ) );

    iLastError = EConnectionFailed;
    iGlobalErrorId = KErrCommsLineFail;
    }


// -----------------------------------------------------------------------------
// CHttpDownload::ConnectionFailed
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpDownload::ConnectionFailed( TInt aError )
    {
    OnError( aError, EConnectionFailed );
    }

// -----------------------------------------------------------------------------
// CHttpDownload::DoReset
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpDownload::DoReset( TBool aOnDelete )
    {
    LOGGER_ENTERFN( "DoReset" );

    // Do not inform user about what we do while reseting
    ++iDontFireEvent;

    Cancel();

    CancelTransaction();

    if( iDisconnectOnReset )
        {
        Disconnect();
        }

    delete iFileMan; iFileMan = NULL;

    ReInitializeDownload();

    // these values are not initialized in ReInitializeDownload()
    delete iContentType; iContentType = NULL;
    delete iDDType; iDDType = NULL;
    delete iHttpRealm; iHttpRealm = NULL;
    delete iHttpNonce; iHttpNonce = NULL;
    TRAP_IGNORE( ReallocateStringL( iRedirUrl, *iUrl, KMaxUrlLength ) );
    TRAP_IGNORE( ReallocateStringL( iCurrentUrl, *iUrl, KMaxUrlLength ) );
    iRedirect = EFalse;
    iDlStartedByClient = EFalse;
    iPausableDRM = ETrue;
    iDrmContentLengthValid = ETrue;

    // content type is unknown -> download might be pausable again
    UpdatePausable();

    if( !aOnDelete )
        {
        TRAP_IGNORE( StoreDownloadInfoL() );
        }

    --iDontFireEvent;

    SetDownloadStatus( EHttpProgNone, EHttpDlMultipleMOStarted );
    }

// -----------------------------------------------------------------------------
// CHttpDownload::StoreDownloadInfoL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpDownload::StoreDownloadInfoL()
    {
    LOGGER_ENTERFN( "StoreDownloadInfoL" );
    TInt bufSz = KDownloadInfoIncrSize +
                 (iDownloadInfo ? iDownloadInfo->Length(): 0);
    HBufC8* newInfo = HBufC8::NewLC( bufSz );
    TPtr8 newInfoPtr = newInfo->Des();

    APPEND_BUF_INT( newInfoPtr, KDMgrInfoFileId );
    APPEND_BUF_INT( newInfoPtr, KDmgrVersionNumber );
    APPEND_BUF_INT( newInfoPtr, iDlState );
    APPEND_BUF_INT( newInfoPtr, iProgState );
    APPEND_BUF_INT( newInfoPtr, iLastError );
    APPEND_BUF_INT( newInfoPtr, iGlobalErrorId );
    APPEND_BUF_INT( newInfoPtr, iAction );
    APPEND_BUF_INT( newInfoPtr, iRestartAction );
    APPEND_BUF_INT( newInfoPtr, iNoContentTypeCheck );

    CLOG_WRITE("2");
    AppendBufL( newInfoPtr, iUrl );
    AppendBufL( newInfoPtr, iRedirUrl );
    APPEND_BUF_INT( newInfoPtr, iPort );
    AppendBufL( newInfoPtr, iCurrentUrl );
    APPEND_BUF_INT( newInfoPtr, iRedirect );
    APPEND_BUF_INT( newInfoPtr, iTargetApp );
    iStorage->AppendStorageInfoL( newInfoPtr );
    AppendBufL( newInfoPtr, iDlName );
    AppendBufL( newInfoPtr, iHashedMsgBody );
    APPEND_BUF_INT( newInfoPtr, iCodDownload );
    APPEND_BUF_INT( newInfoPtr, iNoMedia );

    CLOG_WRITE("3");
    APPEND_BUF_INT( newInfoPtr, iPreferencies );
    APPEND_BUF_INT( newInfoPtr, iDisconnectOnPause );
    APPEND_BUF_INT( newInfoPtr, iDisconnectOnReset );

    CLOG_WRITE("4");
    AppendBufL( newInfoPtr, iContentType );
    AppendBufL( newInfoPtr, iDDType );
    APPEND_BUF_INT( newInfoPtr, iDate );
    APPEND_BUF_INT( newInfoPtr, iExpires );
    APPEND_BUF_INT( newInfoPtr, iMaxAge );

    CLOG_WRITE("5");


    TInt size = GetHttpHeadersSize(iResponseHeaders)+ GetHttpHeadersSize(iRequestHeaders)+
                GetHttpHeadersSize(iEntityHeaders)+ GetHttpHeadersSize(iGeneralHeaders) + newInfoPtr.Size();


    if(size >= bufSz)
        {
    	User::LeaveIfError( KErrArgument );
        }

    AppendHeadersL( newInfoPtr, iResponseHeaders );
    AppendHeadersL( newInfoPtr, iRequestHeaders );
    AppendHeadersL( newInfoPtr, iEntityHeaders );
    AppendHeadersL( newInfoPtr, iGeneralHeaders );

    CLOG_WRITE("6");

    APPEND_BUF_INT( newInfoPtr, iFotaPckgId );

    CLOG_WRITE("7");

    // check if download info is unchanged from previous update
    if( iDownloadInfo && ( iDownloadInfo->Compare(*newInfo) == 0 ))
        {
        // no change
        CLOG_WRITE("DownloadInfo unchanged - no need to update");
        CleanupStack::PopAndDestroy( newInfo );
        }
    else
        {
        CLOG_WRITE("DownloadInfo changed - replace info file");
        delete iDownloadInfo;
        iDownloadInfo = newInfo;
        CleanupStack::Pop( newInfo );

        HBufC* folderBuf = HBufC::NewLC( KMaxPath );
        TPtr folder = folderBuf->Des();
        HBufC* fileNameBuf = HBufC::NewLC( KMaxPath );
        TPtr fileName = fileNameBuf->Des();
        iClientApp->Engine()->DownloadInfoFolder( iClientApp, folder );
        fileName.Format( _L("%S%d"), &folder, iId );
        CLOG_WRITE_1( "info: %S", &fileName );

        RFile outFile;
        CleanupClosePushL<RFile>( outFile );
        User::LeaveIfError( outFile.Replace( iClientApp->Engine()->Fs(),
                                             fileName,
                                             EFileShareExclusive |
                                             EFileStream |
                                             EFileWrite ) );

        outFile.Write( newInfoPtr );
        User::LeaveIfError( outFile.Flush() );
        CleanupStack::PopAndDestroy( 2, fileNameBuf );
        CleanupStack::PopAndDestroy(); // outFile
        }
    CLOG_WRITE("CHttpDownload::StoreDownloadInfoL() - exit");
    }

// -----------------------------------------------------------------------------
// CHttpDownload::LoadDownloadInfoL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpDownload::LoadDownloadInfoL()
    {
    LOGGER_ENTERFN( "LoadDownloadInfoL" );

    HBufC* folderBuf = HBufC::NewLC( KMaxPath );
    TPtr folder = folderBuf->Des();
    HBufC* fileNameBuf = HBufC::NewLC( KMaxPath );
    TPtr fileName = fileNameBuf->Des();

    iClientApp->Engine()->DownloadInfoFolder( iClientApp, folder );

    fileName.Format( _L("%S%d"), &folder, iId );
    CLOG_WRITE_1( "info: %S", &fileName );

    RFile inFile;
    User::LeaveIfError( inFile.Open( iClientApp->Engine()->Fs(),
                                     fileName,
                                     EFileShareReadersOnly |
                                     EFileRead ) );

    CLOG_WRITE("1");
    CleanupClosePushL<RFile>( inFile );

    TInt32 id;
    READ_INT_L( inFile, id );

    if( id == KDMgrInfoFileId )
        {
        TInt version;
        READ_INT_L( inFile, version );

        if( version != KDmgrVersionNumber )
            {
            User::Leave( KErrNotSupported );
            }

        READ_INT_L( inFile, iDlState );
        }
    else
        {
        iDlState = (THttpDownloadState)id;
        }

    READ_INT_L( inFile, iProgState );
    READ_INT_L( inFile, iLastError );
    READ_INT_L( inFile, iGlobalErrorId );
    READ_INT_L( inFile, iAction );
    READ_INT_L( inFile, iRestartAction );
    READ_INT_L( inFile, iNoContentTypeCheck );

    CLOG_WRITE("2");
    ReadHBufCL( inFile, iUrl );
    ReadHBufCL( inFile, iRedirUrl );
    READ_INT_L( inFile, iPort );
    ReadHBufCL( inFile, iCurrentUrl );
    READ_INT_L( inFile, iRedirect );
    READ_INT_L( inFile, iTargetApp );
    iStorage->LoadStorageInfoL( inFile );
    ReadHBufCL( inFile, iDlName );
	if ( *iDlName == KNullDesC )
	   {
	    User::Leave( KErrNotSupported );
	   }
    ReadHBufCL( inFile, iHashedMsgBody );
    READ_INT_L( inFile, iCodDownload );
    READ_INT_L( inFile, iNoMedia );


    CLOG_WRITE("3");
    READ_INT_L( inFile, iPreferencies );
    READ_INT_L( inFile, iDisconnectOnPause );
    READ_INT_L( inFile, iDisconnectOnReset );

    CLOG_WRITE("4");
    ReadHBufCL( inFile, iContentType );
    ReadHBufCL( inFile, iDDType );
    READ_CUST_L( inFile, iDate );
    READ_CUST_L( inFile, iExpires );
    READ_CUST_L( inFile, iMaxAge );

    CLOG_WRITE("5");

    LoadHeadersL( inFile, iResponseHeaders );
    LoadHeadersL( inFile, iRequestHeaders );
    LoadHeadersL( inFile, iEntityHeaders );
    LoadHeadersL( inFile, iGeneralHeaders );

    CLOG_WRITE("6");

    READ_INT_L( inFile, iFotaPckgId );

    CleanupStack::PopAndDestroy(); // inFile
    CleanupStack::PopAndDestroy( 2, folderBuf ); // also fileNameBuf

    CLOG_WRITE("9");

    UpdatePausable();
    }

// -----------------------------------------------------------------------------
// CHttpDownload::IsExpired
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CHttpDownload::IsExpired()
    {
    TBool expired = EFalse;
    TTime now;

    // Set UTC
    now.UniversalTime();

    if( iMaxAge.Int() )
        {
        TTime date( iDate );

        if( date + iMaxAge < now )
            {
            expired = ETrue;
            }
        }
    else if( iExpires.Year() )
        {
        TTime expires( iExpires );

        if( expires < now )
            {
            expired = ETrue;
            }
        }

    return expired;
    }

// -----------------------------------------------------------------------------
// CHttpDownload::IsContentFileStorageType
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

TBool CHttpDownload::IsContentFileStorageType()
    {


     if( iCodDownload
         && iContentType->Compare( KRoapMimeType )
         && iContentType->Compare( KRoapPduMimeType )
         && iContentType->Compare( KFotaPackageDataType)  )
         {
     	 return ETrue;
         }

     return EFalse;

    }

// -----------------------------------------------------------------------------
// CHttpDownload::OnCompletedL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpDownload::OnCompletedL()
    {
    LOGGER_ENTERFN( "OnCompletedL" );

    if( 1 == iCodDownload )
        {
        iCodDownload++;
        TriggerEvent( EHttpDlInprogress, EHttpProgCodDescriptorDownloaded );
        TriggerEvent( EHttpDlInprogress, EHttpProgCodDownloadStarted );
        }
    else
        {
        if( !iContTypeRecognitionAvailSent )
            {
            CLOG_WRITE( "CHttpDownload::OnCompletedL(): triggering event EHttpResponseForContentType" );
            TriggerEvent( EHttpDlInprogress, EHttpContTypeRecognitionAvail );
            iContTypeRecognitionAvailSent = ETrue;
            }
        //for correct display of string in download list for COD download we set progress state to moved
        if(IsContentFileStorageType())
            {
           // Retrieve the file name from the whole paths
            HBufC* fileName = iStorage->DestFilename();
            TInt lastSlashPos = fileName->LocateReverse( '\\' );
            if( lastSlashPos == KErrNotFound )
                lastSlashPos = 0;

            HBufC* name = HBufC::NewLC(KMaxFileName);
            TPtr namePtr( name->Des() );
            namePtr.Copy( fileName->Right(fileName->Length()-lastSlashPos-1) );

            TInt findDot = iDlName->LocateReverse( '.' );
            if( findDot == KErrNotFound )
                {
                //if Name displayed does not have Extension then
                // Remove extention from retrieved name
                TInt dotInd = namePtr.LocateReverse( '.' );
                 if( dotInd == KErrNotFound )
                    dotInd = namePtr.Length();
                namePtr.Copy( namePtr.Left( dotInd ) );

                }

            //we never get file moved and Download complete for Cod download becuase move is inherent
            //to Install() state i.e  Download is in progress so display in download list is incorrect.
            //related to bug  HCHA-753D6G

            if(namePtr.Compare(*iDlName))
                {
                ReallocateStringL( iDlName, namePtr, KDownloadNameMaxSize );
                SetDownloadStatus( EHttpProgContentFileMovedAndDestFNChanged, EHttpDlMultipleMOCompleted );
                }
           else
                {
                SetDownloadStatus( EHttpProgContentFileMoved, EHttpDlMultipleMOCompleted );
                }
           CleanupStack::PopAndDestroy( name );

            }
        else
            {
            TriggerEvent( EHttpDlCompleted, EHttpProgNone );
            SetDownloadStatus( EHttpProgNone, EHttpDlMultipleMOCompleted );
            }
       }

    ++iDontFireEvent;

    Cancel();
    Disconnect();

    iStorage->OnComplete();

    // this is a special case because transaction don't need to be
    // canceled, only deleted
    iContinueDownload = EFalse;
    if( iTransValid )
        {
        iTrans.Close();
        iTransValid = EFalse;
        }

    --iDontFireEvent;

    TRAP_IGNORE( StoreDownloadInfoL() );
    }

// -----------------------------------------------------------------------------
// CHttpDownload::GetParamFromMediaTypeL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TPtrC8 CHttpDownload::GetParamFromMediaTypeL( const TDesC8& aAttribute )
    {
    if( !iMediaType )
        {
        User::Leave( KErrNotFound );
        }
    TInt length = iMediaType->Length();
    TInt start = iMediaType->Des().FindF( aAttribute );
    start = ( ( start + aAttribute.Length() ) >= length ) ? KErrNotFound : start;
    User::LeaveIfError( start );
    start += aAttribute.Length();
    TInt end = iMediaType->Des().Mid( start, length-start ).Locate( KSemiColon );
    TInt boundaryLength = ( KErrNotFound == end ) ? length - start : end;
    TPtrC8 ptr = iMediaType->Des().Mid( start, boundaryLength );
    return ptr;
    }

// -----------------------------------------------------------------------------
// CHttpDownload::ReInitializeDownload
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpDownload::ReInitializeDownload()
    {
    LOGGER_ENTERFN( "ReInitializeDownload" );

    iResponseHeaders->ResetAndDestroy();
    iEntityHeaders->ResetAndDestroy();

    iStorage->ResetAndDestroy();

    iLastError = ENoError;
    iMaxAge = 0;
    iExpires.Set( 0, EJanuary, 1, 0, 0, 0, 0 );
    iDate.Set( 0, EJanuary, 1, 0, 0, 0, 0 );

    iNoMedia = EFalse;
    }

// -----------------------------------------------------------------------------
// CHttpDownload::CancelTransaction
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpDownload::CancelTransaction()
    {
    LOGGER_ENTERFN( "CancelTransaction" );
    if( iTransValid )
        {
        iTrans.Close();
        iTransValid = EFalse;
        }

    if( iContinueDownload )
        {
        TriggerEvent( EHttpDlCancelTransaction );
        }

    iContinueDownload = EFalse;

    iDlStartedByClient = EFalse;
    }

// -----------------------------------------------------------------------------
// CHttpDownload::InternalPauseL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpDownload::InternalPauseL( TBool aDisconnect )
    {
    LOGGER_ENTERFN( "InternalPauseL" );

    CHttpStorage::TFileCloseOperation closeOp = CHttpStorage::EKeepFile;
    if ( !Pausable() && ( iCodDlData && iCodDlData->Count() == 1 ) )
        {
        closeOp = CHttpStorage::EReplaceFile;
        }
    iStorage->CloseDestinationFile( closeOp );
    CancelTransaction();

    if( aDisconnect )
        {
		// Indicate all clients about network loss if the downlolad is not pausable
		if(!iPausable)
			{
			TriggerEvent(EHttpDlNonPausableNetworkLoss);
			}
		Disconnect();
		}
	}

// -----------------------------------------------------------------------------
// CHttpDownload::SetDownloadStatus
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpDownload::SetDownloadStatus( THttpProgressState aProgState,
                                       THttpDownloadState aStatus,
                                       THttpDownloadMgrError aErrorId,
                                       TInt aGlobalErrorId )
    {
#ifdef __DOWNLOADMGR_LOG__  // to filter out EHttpProgResponseBodyReceived
    if( aProgState != EHttpProgResponseBodyReceived )
        {
        CLOG_WRITE_2( "SetDownloadStatus: %d - %d", aStatus, aProgState );
        }
#endif

    TBool eventTriggered( EFalse );

    switch( aStatus )
        {
        case EHttpDlMultipleMOStarted:
		case EHttpDlDeleting:
            {
            iDlState = aStatus;
            iProgState = aProgState;
            }
            break;

        case EHttpDlFailed:
        case EHttpDlMultipleMOCompleted:
        case EHttpDlMultipleMOFailed:
            {
            iDlState = aStatus;
            iProgState = aProgState;
            iLastError = aErrorId;
            iGlobalErrorId = aGlobalErrorId;
            }
            break;

        case EHttpDlInprogress:
            {
            iDlState = aStatus;
            iProgState = aProgState;

            if( aProgState == EHttpStarted ||
                aProgState == EHttpProgConnected )
                // in the other progress state there's no need
                // to go into RunL()
                // Keep it consistente with Runl()
                {
                SelfComplete();
                }
            }
            break;

        case EHttpDlPaused:
            {
            iDlState = aStatus;
            iProgState = aProgState;
            iLastError = aErrorId;
            iGlobalErrorId = aGlobalErrorId;
            Cancel();
            }
            break;

        case EHttpDlMoved:
        case EHttpDlMediaRemoved:
        case EHttpDlMediaInserted:
            {
            TriggerEvent( EHttpDlMoved, EHttpProgNone );
            eventTriggered = ETrue;
            }
            break;

        default:
            {
            DMPanic( KErrArgument );
            }
        }

    TRAP_IGNORE( StoreDownloadInfoL() ); //saving the state

    if( !eventTriggered )
        {
        TriggerEvent( iDlState, iProgState );
        }
    }

// -----------------------------------------------------------------------------
// CHttpDownload::ResponseHeaderReceivedL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpDownload::ResponseHeaderReceivedL()
    {
    LOGGER_ENTERFN( "ResponseHeaderReceivedL" );

    RHTTPResponse resp = iTrans.Response();
    iStatusCode = resp.StatusCode();

    CLOG_WRITE_1( "iStatusCode: %d", iStatusCode );

    if( iProgState == EHttpContentTypeRequested )
        {
        // HEAD method
        switch( iStatusCode )
            {
            case 200:   // OK
            case 201:   // Created
            case 202:   // Accepted
            case 203:   // Non-Authoritative Information
            case 204:   // No Content
            case 205:   // Reset Content
                {
                if( iStorage->ProgressiveDownload() && iStorage->DownloadedSize() )
                    // progressive download cannot be resumed this way.
                    // it's always an error case.
                    {
                    OnError( KErrUnknown, EHttpRestartFailed );
                    }
                else
                    {
                    // store response header only after first request
                    StoreResponseHeaderL();
                    ContentTypeReceivedL();
                    StoreDownloadInfoL();
                    }
                }
                break;

            case 401:   // Unauthorized
                {
                HttpResponse401L();
                }
                break;

            case 407:   // Proxy Authentication Required
                {
                HttpResponse407L();
                }
                break;

            case 404:   // Not Found
                {
                OnError( KErrUnknown, EObjectNotFound );
                }
                break;

            default:
                {
                SetDownloadStatus( EHttpProgNone,
                                   EHttpDlFailed,
                                   EHttpUnhandled,
                                   GLOBAL_HTTP_ERROR( iStatusCode ) );
                }
            }
        }
    else
        // GET method
        {
        TBool respOk = EFalse;

        switch( iStatusCode )
            {
            case 200:   // OK
            case 201:   // Created
            case 202:   // Accepted
            case 203:   // Non-Authoritative Information
            case 204:   // No Content
            case 205:   // Reset Content
                // full content download
                // If ERestartNoIfCompleted comes with 200 and
                // iDownloadedSize != 0 it means http server
                // doesn't support conditional range download
                {
                ReInitializeDownload();
                // store response header only after first request
                StoreResponseHeaderL();
                iStorage->CreateDestinationFileL();
                StoreDownloadInfoL();
                respOk = ETrue;
                }
                break;

            case 206:   // Partial Content
                // partial content download
                {
                StoreResponseHeaderL();
                iStorage->CreateDestinationFileL();

                respOk = ETrue;
                }
                break;

            case 304:   // Not Modified
                // content is not modified since download paused
                {
                iStorage->CreateDestinationFileL();
                OnCompletedL();
                }
                break;

            case 412:   // Precondition Failed
            case 416:   // content range is wrong. Most probably expired, or modified.
                {
                if( iRestartAction == ERestartIfExpired ||
                    iRestartAction == ERestartForced )
                    {
                    // Forget a previous redirect
                    ReallocateStringL( iRedirUrl, *iUrl, KMaxUrlLength );
                    ReallocateStringL( iCurrentUrl, *iRedirUrl, KMaxUrlLength );
                    iRedirect = EFalse;

                    CancelTransaction();

                    ReInitializeDownload();

                    SetDownloadStatus( EHttpStarted );
                    }
                else
                    // ERestartNoIfCompleted => content modified since last partial download
                    {
                    OnError( KErrUnknown, EPartialContentModified );
                    }
                }
                break;

            case 401:   // Unauthorized
                // HTTP authentication failed
                {
                HttpResponse401L();
                }
                break;

            case 407:   // Proxy Authentication Required
                // Proxy authentication failed
                {
                HttpResponse407L();
                }
                break;

            case 404:   // Not Found
                // Object not found
                {
                OnError( KErrUnknown, EObjectNotFound );
                }

            default:
                {
                OnError( GLOBAL_HTTP_ERROR( iStatusCode ), EHttpUnhandled );
                }
            }

        if( respOk )
            {
            SetDownloadStatus( EHttpProgResponseHeaderReceived );
            }
        else
            {
            CancelTransaction();
            }
        }
    }

// -----------------------------------------------------------------------------
// CHttpDownload::ResponseBodyReceivedL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpDownload::ResponseBodyReceivedL( const TDesC8& aBuf )
    {
    if( iMultiPart && !iCodDownload )
        {
        TBool isSupportedMultiPart( EFalse );
        // If it leaves isSupportedMultiPart remains false -> does not supported!
        TRAP_IGNORE( isSupportedMultiPart = IsMultipartSupportedL( aBuf ) );
        if( isSupportedMultiPart )
            {
            SetCodFlag( ETrue );
            TriggerEvent( EHttpDlInprogress, EHttpProgSupportedMultiPart );
            }
        }

    TBool ret(ETrue);

    if( iStorage->BufferingEnabled() )
    	{
    	// Buffering is enabled, just pass on the data
    	ret = iStorage->WriteOutNextBodyDataL(aBuf);
    	}
    else
    	{
    	// Buffering not yet enabled, see how much data we still have to write without buffering
    	TInt bytesToWrite = aBuf.Length();
    	TInt downloadedSize = iStorage->DownloadedSize();

    	if(bytesToWrite + downloadedSize < KMinDataSizeToSend)
    		{
    		// Just dump non-buffered write
    		ret = iStorage->WriteOutNextBodyDataL( aBuf );
    		}
    	else
    		{
    		// Necessary to switch to buffered writing

    		TInt leftPartSize = KMinDataSizeToSend - downloadedSize;
    		TInt rightPartSize = bytesToWrite - leftPartSize;

    		TBool ret1 = ETrue;
    		TBool ret2 = ETrue;

    		if(leftPartSize > 0)
    			{
    			// Write left side of the block to get alignment matched
    			ret1 = iStorage->WriteOutNextBodyDataL( aBuf.Left(leftPartSize) );
    			}

    		// Enable buffering
    		iStorage->EnableBufferingL();

    		// And push the rest of this data block
    		if(rightPartSize > 0)
    			{
    			ret2 = iStorage->WriteOutNextBodyDataL( aBuf.Right(rightPartSize) );
    			}

    		if(!ret1 || !ret2)
    			{
    			ret = EFalse;
    			}
    		}
	    }

    if( !ret )
        {
        StoreDownloadInfoL();
        }

    if( !iContTypeRecognitionAvailSent && (iStorage->DownloadedSize() >= KRespSizeForRecognition) )
        {
        TriggerEvent( EHttpDlInprogress, EHttpContTypeRecognitionAvail );
        iContTypeRecognitionAvailSent = ETrue;
        }

    TriggerEvent( EHttpDlInprogress, EHttpProgResponseBodyReceived );
    }

// -----------------------------------------------------------------------------
// CHttpDownload::IsMultipartSupportedL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CHttpDownload::IsMultipartSupportedL( const TDesC8& aBuf )
    {
    TBool ret( EFalse );

    if( !iHeaderOfMultipart )
        {
        iHeaderOfMultipart = HBufC8::NewL( KMaxHeaderOfMultipart );
        }
    TInt full_length = iHeaderOfMultipart->Length() + aBuf.Length();
    if( KMaxHeaderOfMultipart >= full_length )
        {
        iHeaderOfMultipart->Des().Append( aBuf );
        }
    else
        {
        ret = EFalse;
        return ret;
        }

    TInt pos = iHeaderOfMultipart->Des().Find( KContentType() );

    User::LeaveIfError( pos );

    pos = pos + KContentType().Length();

    TPtrC8 p = iHeaderOfMultipart->Des().Mid( pos );
    TInt temp = p.Find( KDoubleEOL() );

    TInt posEol = pos + temp;

    TPtrC8 ptr = iHeaderOfMultipart->Des().Mid( pos, ( posEol - pos ) );
    if( 0 == ptr.Find( KDdMimeType() ) ||
        0 == ptr.Find( KDd2MimeType() ) ||
        0 == ptr.Find( KCodMimeType() ))
        {
        ret = ETrue;
        delete iHeaderOfMultipart;
        iHeaderOfMultipart = NULL;
        return ret;
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CHttpDownload::Connect
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpDownload::Connect()
    {
    LOGGER_ENTERFN( "ConnectL" );

    SetDownloadStatus( EHttpProgCreatingConnection );

    if( iConnHandler->IsConnected() )
        // we already have connection. Just use it.
        {
        Connected();
        }
    else
        // No connection
        {
        if( iClAppInstance && iClAppInstance->AutoConnect() )
            // Connection w/o question
            {
            TRAPD( err, iConnHandler->ConnectL() );

            if( err )
                {
                OnError( err, EConnectionFailed );
                }
            }
        else
            // ask before connect
            {
            TriggerEvent( EHttpDlInprogress, EHttpProgConnectionNeeded );
            }
        }
    }

// -----------------------------------------------------------------------------
// CHttpDownload::Disconnect
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpDownload::Disconnect()
    {
    LOGGER_ENTERFN( "Disconnect" );

    if( iConnHandler )
        {
        TriggerEvent( iDlState, EHttpProgDisconnected );
        // iConnHandler is set NULL from Disconnect()
        iConnHandler->Disconnect( EFalse, this );

        iConnHandler = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CHttpDownload::SetRequestHeaderL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpDownload::SetRequestHeaderL( RStringPool& aStringPool,
                                       RHTTPHeaders& aHeaders,
                                       TBool aHeadMethod )
    {
    LOGGER_ENTERFN( "SetRequestHeaderL" );

    // Set default Accept header
    aHeaders.SetRawFieldL( aStringPool.StringF(HTTP::EAccept, RHTTPSession::GetTable()),
                           KDefaultAcceptHeader(),
                           KHttpFieldSeparator );

    SetRequestHeaderAddOnL( aStringPool, aHeaders );
    SetCredentialsInfoL( aStringPool );

    // Find ETag in response header
    RStringF etag = aStringPool.StringF(HTTP::EETag,
                                        RHTTPSession::GetTable());
    TInt fieldInd = FindHeaderField( iResponseHeaders, etag.DesC() );
    if( fieldInd != KErrNotFound )
        // ETag is known. ETag identifies the content. Set If-Match to see
        // that if it's changed, or a redirection goes to another url.
        // Server will respond with 412 on error.
        {
        RStringF ifMatch = aStringPool.StringF(HTTP::EIfMatch,
                                        RHTTPSession::GetTable());
        aHeaders.RemoveField( ifMatch );
        aHeaders.SetRawFieldL( ifMatch,
                               *(*iResponseHeaders)[fieldInd]->FieldRawData(),
                               KHttpFieldSeparator );
        CLOG_WRITE8_1( "ETag: %S", (*iResponseHeaders)[fieldInd]->FieldRawData() );
        }

    if( aHeadMethod )
        // HEAD method doesn't require any other field to be set
        {
        return;
        }

    CLOG_WRITE_1( "Down: %d", iStorage->DownloadedSize() );
    CLOG_WRITE_1( "Length: %d", iStorage->Length() );

    if( iRestartAction == ERestartNoIfCompleted )
        {
        CLOG_WRITE( "ERestartNoIfCompleted" );

        SetRangeFieldL( aStringPool, aHeaders );

        RStringF field;

        if( iStorage->DownloadedSize() != iStorage->Length() )
            // download from previous point only if the content is unmodified
            {
            field = aStringPool.StringF(HTTP::EIfUnmodifiedSince,
                                        RHTTPSession::GetTable());

            SetExpireToFieldL( field, aStringPool, aHeaders );
            }
        }
    else if( iRestartAction == ERestartIfExpired )
        // Expire and max-age is already checked in StartL
        // nothing to do if response is 304
        {
        CLOG_WRITE( "ERestartIfExpired" );

        RStringF field;

        if( iStorage->DownloadedSize() != iStorage->Length() )
            // we don't have the entire content
            // start from where we stopped before
            {
            SetRangeFieldL( aStringPool, aHeaders );
            // download entire content if expired
            field = aStringPool.StringF(HTTP::EIfRange, RHTTPSession::GetTable());
            }
        else
            // we have the entire content. Check that content is modified
            // since it's downloaded, or wait for response 304.
            {
            field = aStringPool.StringF(HTTP::EIfModifiedSince, RHTTPSession::GetTable());
            }

        SetExpireToFieldL( field, aStringPool, aHeaders );
        }
    }

// -----------------------------------------------------------------------------
// CHttpDownload::DisablePipeliningL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpDownload::DisablePipeliningL( RStringPool& aStringPool )
    {
    LOGGER_ENTERFN( "DisablePipeliningL" );

    RStringF pipeline = aStringPool.StringF(HTTP::EHttpPipelining, RHTTPSession::GetTable());
    RStringF disable = aStringPool.StringF(HTTP::EDisablePipelining, RHTTPSession::GetTable());

    iTrans.PropertySet().SetPropertyL(pipeline, disable);
    }

// -----------------------------------------------------------------------------
// CHttpFilterAuthentication::SetPropertyL
// Set property of the transaction
// -----------------------------------------------------------------------------
//
void CHttpDownload::SetPropertyL( RStringPool& aStringPool,
                                  RStringF aPropertyName,
                                  const TDesC8& aToken )
    {
    RString tokenStr = aStringPool.OpenStringL( aToken );
    THTTPHdrVal tokenVal = tokenStr;
    CleanupClosePushL( tokenStr );
    iTrans.PropertySet().RemoveProperty( aPropertyName );
    iTrans.PropertySet().SetPropertyL( aPropertyName, tokenVal );

    CLOG_WRITE8_2( "SetPropertyL: %S - %S", &aPropertyName.DesC(), &aToken );

    CleanupStack::PopAndDestroy(); // tokenStr
    }

// -----------------------------------------------------------------------------
// CHttpFilterAuthentication::SetPropertyL
// Set property of the transaction
// -----------------------------------------------------------------------------
//
void CHttpDownload::SetPropertyL( RStringF aPropertyName,
                                  const TInt aValue )
    {
    THTTPHdrVal tokenVal = aValue;

    iTrans.PropertySet().RemoveProperty( aPropertyName );
    iTrans.PropertySet().SetPropertyL( aPropertyName, tokenVal );

    CLOG_WRITE8_2( "SetPropertyL: %S - %d", &aPropertyName.DesC(), aValue );
    }

// -----------------------------------------------------------------------------
// CHttpFilterAuthentication::SetRequestHeaderAddOnL
// Set property of the transaction
// -----------------------------------------------------------------------------
//
void CHttpDownload::SetRequestHeaderAddOnL( RStringPool& aStringPool,
                                            RHTTPHeaders& aHeaders )
    {
    if( !iRequestHeaders->Count() )
        {
        return;
        }

    TPtrC8 fieldName;
    TPtrC8 fieldValue;
    RStringF fieldNameStr;
    THTTPHdrVal value;

    for( TInt i = 0; i < iRequestHeaders->Count(); ++i )
        {
        CLOG_WRITE8_3( "Req(%d): %S: %S", i, (*iRequestHeaders)[i]->FieldName(), (*iRequestHeaders)[i]->FieldRawData() );

        fieldNameStr = aStringPool.OpenFStringL( *(*iRequestHeaders)[i]->FieldName() );
        CleanupClosePushL( fieldNameStr );

//        aHeaders.RemoveField( fieldNameStr );
        if (fieldNameStr == aStringPool.StringF(HTTP::EIfModifiedSince, RHTTPSession::GetTable()) ||
            fieldNameStr == aStringPool.StringF(HTTP::EIfUnmodifiedSince, RHTTPSession::GetTable()))
            {
            TInternetDate date;
            date.SetDateL(*(*iRequestHeaders)[i]->FieldRawData());
            TDateTime modifyTime(date.DateTime());
            aHeaders.SetFieldL(fieldNameStr, modifyTime);
            CleanupStack::PopAndDestroy(); // fieldNameStr
            }
        else
            {
            RStringF valueStr = aStringPool.OpenFStringL( *(*iRequestHeaders)[i]->FieldRawData() );
            value.SetStrF( valueStr );
            CleanupClosePushL( valueStr );

            aHeaders.SetFieldL( fieldNameStr, valueStr );

            CleanupStack::PopAndDestroy( 2 ); // valueStr, fieldNameStr
            }
        }
    }

// -----------------------------------------------------------------------------
// CHttpDownload::SetCredentialsInfoL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpDownload::SetCredentialsInfoL( RStringPool& aStringPool )
    {
    LOGGER_ENTERFN( "SetCredentialsInfoL" );

    if( iHttpUsername )
        {
        RStringF username = aStringPool.StringF( HTTP::EUsername,
                                                    RHTTPSession::GetTable() );

        SetPropertyL( aStringPool, username, *iHttpUsername );
        }

    if( iHttpPassword )
        {
        RStringF password = aStringPool.StringF( HTTP::EPassword,
                                                    RHTTPSession::GetTable() );

        SetPropertyL( aStringPool, password, *iHttpPassword );
        }

    if( iHttpRealm )
        {
        RStringF realm = aStringPool.StringF( HTTP::ERealm,
                                                    RHTTPSession::GetTable() );
        SetPropertyL( aStringPool, realm, *iHttpRealm );
        }

    if( iHttpProxyRealm )
        {
        RStringF proxyRealmStr = aStringPool.StringF(
                                        HttpFilterCommonStringsExt::EProxyRealm,
                                        HttpFilterCommonStringsExt::GetTable() );
        SetPropertyL( aStringPool, proxyRealmStr, *iHttpProxyRealm );
        }

    if( iHttpProxyUsername )
        {
        RStringF proxyUsernameStr = aStringPool.StringF(
                                    HttpFilterCommonStringsExt::EProxyUsername,
                                    HttpFilterCommonStringsExt::GetTable() );
        SetPropertyL( aStringPool, proxyUsernameStr, *iHttpProxyUsername );
        }

    if( iHttpProxyPassword )
        {
        RStringF proxyPasswordStr = aStringPool.StringF(
                                        HttpFilterCommonStringsExt::EProxyPassword,
                                        HttpFilterCommonStringsExt::GetTable() );
        SetPropertyL( aStringPool, proxyPasswordStr, *iHttpProxyPassword );
        }

    if( iHttpNonce )
        {
        RStringF nonce = aStringPool.StringF( HTTP::ENonce,
                                                    RHTTPSession::GetTable() );

        SetPropertyL( aStringPool, nonce, *iHttpNonce );
        }
    }

// -----------------------------------------------------------------------------
// CHttpDownload::SetRangeFieldL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpDownload::SetRangeFieldL( RStringPool& aStringPool,
                                    RHTTPHeaders& aHeaders )
    {
    LOGGER_ENTERFN( "SetRangeFieldL" );

    if( iStorage->DownloadedSize() == 0 )
        // no bytes have been downloaded yet
        {
        return;
        }

    RStringF range = aStringPool.StringF(HTTP::ERange, RHTTPSession::GetTable());

    aHeaders.RemoveField( range );

    TBuf8<48> rawData;

    rawData.Format( _L8("bytes=%d-"), iStorage->DownloadedSize() );

    CLOG_WRITE8_1( "Range: %S", &rawData );
    aHeaders.SetRawFieldL( range, rawData, KHttpFieldSeparator );
    }

// -----------------------------------------------------------------------------
// CHttpDownload::SetExpireToFieldL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpDownload::SetExpireToFieldL( RStringF& aField,
                                       RStringPool& aStringPool,
                                       RHTTPHeaders& aHeaders )
    {
    LOGGER_ENTERFN( "SetExpireToFieldL" );

    RStringF expires = aStringPool.StringF(HTTP::EExpires, RHTTPSession::GetTable());
    RStringF lastModified = aStringPool.StringF(HTTP::ELastModified, RHTTPSession::GetTable());

    TInt expInd( KErrNotFound );
    TInt modInd( KErrNotFound );

    for( TInt i = 0; i < iResponseHeaders->Count(); ++i )
        // FindHeaderField, because this is double search for fieldnames
        {
        if( *(*iResponseHeaders)[i]->FieldName() == expires.DesC() )
            {
            expInd = i;
            continue;
            }
        if( *(*iResponseHeaders)[i]->FieldName() == lastModified.DesC() )
            {
            modInd = i;
            continue;
            }
        }

    if( expInd != KErrNotFound )
        {
        CLOG_WRITE8_1( "Expire: %S", (*iResponseHeaders)[expInd]->FieldRawData() );

        aHeaders.RemoveField( aField );
        aHeaders.SetRawFieldL( aField,
                               *(*iResponseHeaders)[expInd]->FieldRawData(),
                               KHttpFieldSeparator );
        }
    else if( modInd != KErrNotFound )
        {
        CLOG_WRITE8_1( "LastMod: %S", (*iResponseHeaders)[modInd]->FieldRawData() );

        aHeaders.RemoveField( aField );
        aHeaders.SetRawFieldL( aField,
                               *(*iResponseHeaders)[modInd]->FieldRawData(),
                               KHttpFieldSeparator );
        }
    }

// -----------------------------------------------------------------------------
// CHttpDownload::ParseRequestedUrlL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpDownload::ParseRequestedUrlL()
    {
    LOGGER_ENTERFN( "ParseRequestedUrlL" );

    __ASSERT_DEBUG( iUrl->Length(), DMPanic( KErrArgument ) );

    TUriParser8 uri;
    uri.Parse( *iUrl );

    TPtrC8 scheme( uri.Extract( EUriScheme ) );

    if( scheme != KHttpScheme &&
        scheme != KHttpsScheme )
        // unsupported or no scheme in url.
        // Insert 'http://' to the beginning of it.
        {
        HBufC8* tempBuf = HBufC8::NewL( KHttpScheme().Length() +
                                        KSchemeAddon().Length() +
                                        iUrl->Length() );

        tempBuf->Des().Append( KHttpScheme );
        tempBuf->Des().Append( KSchemeAddon );
        tempBuf->Des().Append( *iUrl );

        delete iUrl; iUrl = NULL;

        iUrl = tempBuf;

        uri.Parse( *iUrl );
        }

    CUri8* url = CUri8::NewLC();

    url->SetComponentL( uri.Extract( EUriScheme ), EUriScheme );
    url->SetComponentL( uri.Extract( EUriHost ), EUriHost );
    url->SetComponentL( uri.Extract( EUriPath ), EUriPath );
    url->SetComponentL( uri.Extract( EUriUserinfo ), EUriUserinfo );
    url->SetComponentL( uri.Extract( EUriQuery ), EUriQuery );
    url->SetComponentL( uri.Extract( EUriFragment ), EUriFragment );

    if( uri.IsPresent( EUriPort ) )
        {
        url->SetComponentL( uri.Extract( EUriPort ), EUriPort );
        }
    else
        {
        if( iPort != KDefaultPort )
            {
            TBuf8<10> port;

            port.Format( _L8("%d"), iPort );

            url->SetComponentL( port, EUriPort );
            }
        }

    CLOG_WRITE8_1( "Parsed URL: %S", &url->Uri().UriDes() );

    // Initially they are the same
    ReallocateStringL( iRedirUrl, url->Uri().UriDes(), KMaxUrlLength );
    ReallocateStringL( iCurrentUrl, url->Uri().UriDes() , KMaxUrlLength);

    CleanupStack::PopAndDestroy( url );
    }

// -----------------------------------------------------------------------------
// CHttpDownload::ParseDownloadNameL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpDownload::ParseDownloadNameL()
    {
    LOGGER_ENTERFN( "ParseDownloadNameL" );

    // Calculate the download name from the requested URL
    HBufC8* parsedUrl = EscapeUtils::EscapeDecodeL( *iCurrentUrl );
    CleanupStack::PushL( parsedUrl );

    TUriParser8 uri;
    uri.Parse( *parsedUrl );

    TPtrC8 path;

    path.Set( uri.Extract( EUriPath ) );

    HBufC* newDlName = HBufC::NewLC( KDownloadNameMaxSize );
    TPtr newDlNamePtr = newDlName->Des();

    // Here we don't have to deal with that iDlName is longer than KMaxPath
    // ConvertDownloadNameUniqueL() will make the correction
    if( path.Length() )
        // there's path in the url
        {
        TInt slash = path.LocateReverse( '/' );

        if( slash == KErrNotFound )
            {
            slash = path.LocateReverse( '\\' );
            }

        if( slash != KErrNotFound && slash != path.Length() )
            // from the last slash this is the filename
            {
            TPtrC8 temp( path.Right( path.Length() - slash - 1 ) );

            if( temp.Length() )
                {
				HBufC *tempstr = EscapeUtils::ConvertToUnicodeFromUtf8L(temp.Left( KDownloadNameMaxSize ));
				newDlNamePtr.Copy(*tempstr);
				delete tempstr;
                }
            else
                // empty path -> default filename is the host
                // filename is KDefDestFilename
                {
                newDlNamePtr.Copy( KDefDestFilename );
                }
            }
        else
            // no slash -> entire path is the filename
            {
			HBufC *tempstr = EscapeUtils::ConvertToUnicodeFromUtf8L(path.Left( KDownloadNameMaxSize ));
			newDlNamePtr.Copy(*tempstr);
			delete tempstr;
            }
        }
    else
        // no path -> default filename is the host
        // filename is KDefDestFilename
        {
        newDlNamePtr.Copy( KDefDestFilename );
        }

    SetDownloadNameL( newDlNamePtr );
    CleanupStack::PopAndDestroy( newDlName );
    CleanupStack::PopAndDestroy( parsedUrl );

    CLOG_WRITE_1( "Name: %S", iDlName );
    }

// -----------------------------------------------------------------------------
// CHttpDownload::StoreResponseHeaderL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpDownload::StoreResponseHeaderL()
    {
    LOGGER_ENTERFN( "StoreResponseHeaderL" );

    TPtrC8 rawData;
    RStringPool strPool = iConnHandler->Session().StringPool();
    RHTTPHeaders headers( iTrans.Response().GetHeaderCollection() );
    THTTPHdrFieldIter it = headers.Fields();

    // forget the previous headers
    iResponseHeaders->ResetAndDestroy();
    iEntityHeaders->ResetAndDestroy();

    CLOG_WRITE( "Received response header:" );
    while ( !it.AtEnd() )
        {
        RStringTokenF fieldName = it();
        RStringF fieldNameStr = strPool.StringF (fieldName );

        headers.GetRawField( fieldNameStr, rawData );

        CHeaderField* field = CHeaderField::NewL( &fieldNameStr.DesC(), &rawData );
        CleanupStack::PushL( field );

        CLOG_WRITE8_2("%S:%S", &fieldNameStr.DesC(), &rawData);
        iResponseHeaders->AppendL( field );

        CleanupStack::Pop( field );

        CHeaderField* entityField = CHeaderField::NewL( &fieldNameStr.DesC(), &rawData );
        CleanupStack::PushL( entityField );

        CLOG_WRITE8_2("%S:%S", &fieldNameStr.DesC(), &rawData);
        iEntityHeaders->AppendL( entityField );

        CleanupStack::Pop( entityField );

        ++it;
        }

    ParseContentTypeL( strPool );

    ParseContentDispositionL( strPool);
    if (!iCodDownload)
        {
		if (iUseAttachmentFileName || iUseInlineFileName)
	        {
	        SetDownloadNameL( *iAttachmentFileName );
		    }
        }

    RStringF length = strPool.StringF(HTTP::EContentLength,RHTTPSession::GetTable());
    RStringF date = strPool.StringF(HTTP::EDate,RHTTPSession::GetTable());
    RStringF expires = strPool.StringF(HTTP::EExpires,RHTTPSession::GetTable());
    RStringF maxAge = strPool.StringF(HTTP::EMaxAge,RHTTPSession::GetTable());
    RStringF cacheControl = strPool.StringF(HTTP::ECacheControl,RHTTPSession::GetTable());
    THTTPHdrVal value;

    if( !headers.GetField( length, 0, value ) )
        {
        if( iStorage->Length() == KDefaultContentLength )
            // content size is
            {
            iStorage->SetLength( value );
            }
        else
            //This is a partial response as Length is already set so save this new length as partial Length that needs to be Downloaded.
            {
            iStorage->SetPartialContentLength( value );
            }
        }

    CheckRealDRMContentType();
    if( !iDrmContentLengthValid )
        // Content was original encoded -> we don't know the actual content size.
        {
        iStorage->SetLength( KDefaultContentLength );
        }


    iMaxAge = 0;
    TInt parts( 0 );
    // this leave is trapped because we can still go on
    TRAPD( err, parts = headers.FieldPartsL( cacheControl ) );

    if( !err )
        // try to find max-age in Cache-control field
        {
        for( TInt i = 0; i < parts; ++i )
            {
            RStringF directive;
            THTTPHdrVal hdrVal;
            TInt err;

            // Get the cache-control from the headers
            // initialise the fieldname
            headers.GetField( cacheControl, i, hdrVal );

            if((hdrVal.Type() == THTTPHdrVal::KStrVal) || (hdrVal.Type() == THTTPHdrVal::KStrFVal))
                {
                RStringF cacheDir = hdrVal.StrF();

                TInt endPos;
                _LIT8(KFind, "=");

                endPos = cacheDir.DesC().Find( KFind );
                if( endPos != -1 )
                    {
                    TRAP(err, directive = strPool.OpenFStringL(cacheDir.DesC().Left(endPos)));
                    if( !err )
                        {
                        if( directive == maxAge )
                            {
                            TInt valueInt( 0 );
                            TLex8 value( cacheDir.DesC().Right(cacheDir.DesC().Length() - endPos - 1) );

                            value.Val( valueInt );
                            iMaxAge = valueInt;
                            }
                        }
                    }
                }
            }
        }

    if( !headers.GetField( expires, 0, value ) )
        {
        iExpires = value;
        }
    else
        {
        iExpires.SetYear( 0 );
        }

    if( !headers.GetField( date, 0, value ) )
        {
        iDate = value;
        }
    else
        {
        iDate.SetYear( 0 );
        }

    UpdatePausable();
    }

// -----------------------------------------------------------------------------
// CHttpDownload::CheckRealDRMContentType
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpDownload::CheckRealDRMContentType()
    {
    LOGGER_ENTERFN( "CheckRealDRMContentType" );

    iPausableDRM = ETrue;
    iDrmContentLengthValid = ETrue;

    TInt index = FindHeaderField( iResponseHeaders, KDRMOldContentType );
    if( index != KErrNotFound )
        // this is an old DRM protected content
        // This transaction cannot be paused.
        {
        if( !(*iResponseHeaders)[index]->FieldRawData()->Compare( KDrmMessageMimeType() ) )
            {
            iPausableDRM = EFalse;
//            iDrmContentLengthValid = EFalse;
            }
        }

    UpdatePausable();

    CLOG_WRITE_2( "Pausable: [%d], Length: [%d]", iPausableDRM, iDrmContentLengthValid );
    }

// -----------------------------------------------------------------------------
// CHttpDownload::SaveCredentialsL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpDownload::SaveCredentialsL( RStringPool aStringPool )
    {
    LOGGER_ENTERFN( "SaveCredentialsL" );

    RStringF username = aStringPool.StringF( HTTP::EUsername,
                                                RHTTPSession::GetTable() );
    RStringF password = aStringPool.StringF( HTTP::EPassword,
                                                RHTTPSession::GetTable() );
    RStringF realm = aStringPool.StringF( HTTP::ERealm,
                                                RHTTPSession::GetTable() );
    RStringF nonce = aStringPool.StringF( HTTP::ENonce,
                                                RHTTPSession::GetTable() );

    THTTPHdrVal hdrValue;

    // Realm
    if( iTrans.PropertySet().Property( realm, hdrValue ) )
        {
        if( hdrValue.Str().DesC().Length() )
            // W/o realm authentication is meaningless
            {
            ReallocateStringL( iHttpRealm, hdrValue.Str().DesC(), KMaxDefAttrLength );
            CLOG_WRITE8_1( "Realm: [%S]", iHttpRealm );

            // Username
            if( iTrans.PropertySet().Property( username, hdrValue ) )
                {
                ReallocateStringL( iHttpUsername, hdrValue.Str().DesC(), KMaxDefAttrLength );
                CLOG_WRITE8_1( "uname: [%S]", iHttpUsername );
                }

            // Password
            if( iTrans.PropertySet().Property( password, hdrValue ) )
                {
                ReallocateStringL( iHttpPassword, hdrValue.Str().DesC(), KMaxDefAttrLength );
                CLOG_WRITE8_1( "pwd: [%S]", iHttpPassword );
                }

            if( !iTrans.PropertySet().Property(
                                    aStringPool.StringF( HTTP::EBasic,
                                    RHTTPSession::GetTable() ),
                                    hdrValue ) )
                // this is a digest authentication response
                // store nonce value
                {
                if( iTrans.PropertySet().Property( nonce, hdrValue ) )
                    {
                    ReallocateStringL( iHttpNonce, hdrValue.Str().DesC() );
                    CLOG_WRITE8_1( "nonce: [%S]", iHttpNonce );
                    }
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CHttpDownload::RequestContentL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpDownload::RequestContentL()
    {
    LOGGER_ENTERFN( "RequestContentL" );

    RStringPool strPool = iConnHandler->Session().StringPool();
    RStringF method;
    TBool aHead = ETrue;

    if( (iContentType && iContentType->Length()) ||
        iNoContentTypeCheck ||
        iSilentMode )
        // we know the content type -> head is already requested or
        // every content type is welcome
        {
        method = strPool.StringF( HTTP::EGET,RHTTPSession::GetTable() );
        aHead = EFalse;
        }
    else
        // content type check needed -> first request HEAD
        {
        method = strPool.StringF( HTTP::EHEAD,RHTTPSession::GetTable() );
        }

    TUriParser8 uri;
    uri.Parse( *iCurrentUrl );

    CLOG_WRITE8_1( "Req URL: %S", iCurrentUrl );
    CLOG_WRITE8_1( "Method: %S", &method.DesC() );

    __ASSERT_DEBUG( !iTransValid, DMPanic( KErrCorrupt ) );
    if( iTransValid )
        {
        User::Leave( KErrCorrupt );
        }

    iTrans = iConnHandler->Session().OpenTransactionL( uri, *this, method );
    iTransValid = ETrue;

    RHTTPHeaders headers( iTrans.Request().GetHeaderCollection() );

    SetRequestHeaderL( strPool, headers, aHead );

    DisablePipeliningL( strPool );
    SetCookiesL( strPool );

    iTrans.SubmitL();

    if( aHead )
        {
        SetDownloadStatus( EHttpContentTypeRequested );
        }
    else
        {
        SetDownloadStatus( EHttpProgSubmitIssued );
        }
    }

// -----------------------------------------------------------------------------
// CHttpDownload::ContentTypeReceivedL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpDownload::ContentTypeReceivedL()
    {
    LOGGER_ENTERFN( "ContentTypeReceivedL" );

    InternalPauseL( EFalse );
    SetDownloadStatus( EHttpContentTypeReceived, EHttpDlPaused );
    }

// -----------------------------------------------------------------------------
// CHttpDownload::HttpResponse401L
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpDownload::HttpResponse401L()
    {
    LOGGER_ENTERFN( "HttpResponse401L" );

    THTTPHdrVal hdrValue;
    RStringPool strPool = iConnHandler->Session().StringPool();

    // Check authentication scheme
    TBool basic = iTrans.PropertySet().Property( strPool.StringF( HTTP::EBasic,
                                                 RHTTPSession::GetTable() ),
                                                 hdrValue );

    if( basic )
        // property is set -> Basic
        {
        iAuthScheme = EAuthBasic;
        }
    else
        // property not set -> Digest
        {
        iAuthScheme = EAuthDigest;
        }

    RStringF staleStr = strPool.StringF( HTTP::EStale, RHTTPSession::GetTable() );

    SaveCredentialsL( strPool );
    if( iTrans.PropertySet().Property( staleStr, hdrValue ) ||  // 1.
        (iDlStartedByClient && iHttpUsername) )                 // 2.
        // 1. - In case of stale we only have to resubmit the request
        // with the same credentials info
        // 2. - download was started by the client app that has specified
        // the http username -> try to do the authentication
        {
        CancelTransaction();
        SetDownloadStatus( EHttpStarted );
        }
    else
        {
        OnError( KErrUnknown, EHttpAuthenticationFailed );
        }
    }

// -----------------------------------------------------------------------------
// CHttpDownload::HttpResponse407L
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpDownload::HttpResponse407L()
    {
    LOGGER_ENTERFN( "HttpResponse407L" );

    // Check authentication scheme
    THTTPHdrVal hdrValue;

    RStringPool strPool = iConnHandler->Session().StringPool();
    TInt err = iTrans.PropertySet().Property( strPool.StringF( HTTP::EBasic,
                                               RHTTPSession::GetTable() ),
                                               hdrValue );

    if( !err )
        // property is set -> Basic
        {
        iAuthScheme = EAuthBasic;
        }
    else
        // property not set -> Digest
        {
        iAuthScheme = EAuthDigest;
        }

    RStringF staleStr = strPool.StringF( HTTP::EStale, RHTTPSession::GetTable() );

    SaveCredentialsL( strPool );
    if( iTrans.PropertySet().Property( staleStr, hdrValue ) ||  // 1.
        (iDlStartedByClient && iHttpUsername) )                 // 2.
        // 1. - In case of stale we only have to resubmit the request
        // with the same credentials info
        // 2. - download was started by the client app that has specified
        // the http username -> try to do the authentication
        {
        CancelTransaction();
        SetDownloadStatus( EHttpStarted );
        }
    else
        {
        OnError( KErrUnknown, EProxyAuthenticationFailed );
        }
    }

// -----------------------------------------------------------------------------
// CHttpDownload::FindHeaderField
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CHttpDownload::FindHeaderField( CArrayPtrFlat<CHeaderField>* aHeaders,
                                     const TDesC8& aFieldName ) const
    {
    for( TInt index = 0; index < aHeaders->Count(); ++index )
        {
        if( *(*aHeaders)[index]->FieldName() == aFieldName )
            {
            return index;
            }
        }

    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CHttpDownload::ConvertDownloadNameUniqueL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpDownload::ConvertDownloadNameUniqueL()
    {
    LOGGER_ENTERFN( "ConvertDownloadNameUniqueL" );

    CArrayPtrFlat<CHttpDownload>* downloads = iClientApp->Downloads();
    TBool bFound( EFalse );
    TInt index( 0 );
    HBufC* uniqueName = NULL;

    do
        {
        bFound = EFalse;

        CreateIndexedNameL( uniqueName, *iDlName, index );

        for( TInt i = 0; i < downloads->Count(); ++i )
            {
            if( (*downloads)[i] != this )
                {
                CLOG_WRITE_1( "Compare: %S", (*downloads)[i]->iDlName );
                if( !uniqueName->Des().Compare( *(*downloads)[i]->iDlName ) )
                    {
                    bFound = ETrue;
                    break;
                    }
                }
            }
        }while( bFound );

    delete iDlName;
    iDlName = uniqueName;
    CLOG_WRITE_1( "Unique name: [%S]", iDlName );
    }

// -----------------------------------------------------------------------------
// CHttpDownload::CreateIndexedNameL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpDownload::CreateIndexedNameL( HBufC* &aUniqueName,
                                        TDesC& aOrgName,
                                        TInt& aIndex )
    {
    LOGGER_ENTERFN( "CreateIndexedNameL" );

    TPtrC left;
    TPtrC extension;
    TBuf<KMaxIndexStringLength> indexStr;

    if( aIndex )
        {
        indexStr.Format( KIndexString, aIndex );
        }

    TInt fullLength = aOrgName.Length() + indexStr.Length();
    if( fullLength > KMaxPath )
        // name is longer than KMaxPath.
        {
        OnError( KErrGeneral, EBadUrl );
        User::Leave( KErrGeneral );
        }

    delete aUniqueName; aUniqueName = NULL;

    TInt dotInd = aOrgName.LocateReverse( '.' );
    if( dotInd != KErrNotFound )
        // filename extension found.
        // insert index string between name and extension
        {
        extension.Set( aOrgName.Right( aOrgName.Length() - dotInd ) );
        left.Set( aOrgName.Left( MIN( dotInd, ( KDownloadNameMaxSize -
            indexStr.Length() - extension.Length() ))));
        }
    else
        {
        left.Set( aOrgName.Left( KDownloadNameMaxSize - indexStr.Length() ));
        }

    aUniqueName = HBufC::NewL( fullLength );
    aUniqueName->Des().Format( _L("%S%S%S"), &left,
                                             &indexStr,
                                             &extension );

    ++aIndex;
    }

// -----------------------------------------------------------------------------
// CHttpDownload::ContinueDownloadStoreResponseHeaderL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpDownload::ContinueDownloadStoreResponseHeaderL(
                                                const TDesC8& aResponseHeader )
    {
    LOGGER_ENTERFN( "ContinueDownloadStoreResponseHeaderL" );

    TInt colon( KErrNotFound );
    TInt separator( KErrNotFound );
    TPtrC8 field( aResponseHeader );
    TPtrC8 fieldName;
    TPtrC8 fieldValue;
    RStringPool strPool;

    // forget the previous headers
    iResponseHeaders->ResetAndDestroy();
    iEntityHeaders->ResetAndDestroy();

    strPool.OpenL( RHTTPSession::GetTable() );
    strPool.OpenL( HttpFilterCommonStringsExt::GetTable() );

    do
        {
        separator = KErrNotFound;

        colon = field.Locate( KColon );

        if( colon > 0 )
            // name found and is at least one byte long
            {
            fieldName.Set( field.Left( colon ) );
            field.Set( field.Right( field.Length() - colon - 1 ) );

            separator = field.Locate( KHttpFieldSeparator()[0] );

            if( separator != KErrNotFound )
                // there is more add-on
                {
                fieldValue.Set( field.Left( separator ) );
                field.Set( field.Right( field.Length() - separator - 1 ) );
                }
            else
                // this is the last add-on
                {
                fieldValue.Set( field );
                }

            CHeaderField* newField = CHeaderField::NewL( &fieldName, &fieldValue );
            CleanupStack::PushL( newField );

            CLOG_WRITE8_2("%S:%S", &fieldName, &fieldValue);
            iResponseHeaders->AppendL( newField );

            CleanupStack::Pop( newField );

            CHeaderField* newentField = CHeaderField::NewL( &fieldName, &fieldValue );
            CleanupStack::PushL( newentField );

            CLOG_WRITE8_2("%S:%S", &fieldName, &fieldValue);
            iEntityHeaders->AppendL( newentField );

            CleanupStack::Pop( newentField );
            }

        }while( separator != KErrNotFound );

    ParseContentTypeL( strPool );

	ParseContentDispositionL( strPool);
    if (!iCodDownload)
        {
		if (iUseAttachmentFileName || iUseInlineFileName)
	        {
	        SetDownloadNameL( *iAttachmentFileName );
		    }
        }

    CheckRealDRMContentType();

    RStringF length = strPool.StringF(HTTP::EContentLength,RHTTPSession::GetTable());
    RStringF contentEncoding = strPool.StringF( HTTP::EContentEncoding, RHTTPSession::GetTable() );

    TInt index = FindHeaderField( iResponseHeaders, length.DesC() );
    TInt contentEncodingInd = FindHeaderField( iResponseHeaders, contentEncoding.DesC() );
    if( ( index != KErrNotFound && contentEncodingInd == KErrNotFound ) &&
          iDrmContentLengthValid )
        {
        iStorage->SetLength( (*iResponseHeaders)[index]->Int() );
        }
    else
        // in case of content-encoding the content length is size of the compressed content
        // not the decompressed one.
        {
        CLOG_WRITE( "Content is encoded, or DRM" );
        iStorage->SetLength( KDefaultContentLength );
        }

    strPool.Close();
    }

// -----------------------------------------------------------------------------
// CHttpDownload::ParseContentTypeL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpDownload::ParseContentTypeL( RStringPool& aStrPool )
    {
    LOGGER_ENTERFN( "ParseContentTypeL" );

    RStringF contentType = aStrPool.StringF(HTTP::EContentType,RHTTPSession::GetTable());

    TInt index = FindHeaderField( iResponseHeaders, contentType.DesC() );
    if( index != KErrNotFound )
        // store content-type
        {
        TPtrC8 rawData( *(*iResponseHeaders)[index]->FieldRawData() );
        TPtrC8 mediaType;

        // ';' separates content-type from media type
        TInt semicolon = rawData.Locate( KSemiColon );

        if( semicolon != KErrNotFound )
            {
            mediaType.Set( rawData.Right( rawData.Length() - semicolon - 1 ) );
            Trim( mediaType );
            rawData.Set( rawData.Left( semicolon ) );
            Trim( rawData );
            }

        ReallocateStringL( iContentType, rawData, KMaxContentTypeLength );
        ReallocateStringL( iDDType, rawData, KMaxContentTypeLength );
        ReallocateStringL( iMediaType, mediaType, KMaxContentTypeLength );
        }

#ifdef __SYNCML_DM_FOTA
    if( !iContentType->Des().CompareF( KFotaMimeType ) )
        {
        iStorage->SetStorageMethod( CHttpStorage::EStoreFota );
        }
#endif

    if( ( 0 == iContentType->Des().Compare( KCodMimeType() ) ) ||
        ( 0 == iContentType->Des().Compare( KDdMimeType()  ) ) ||
        ( 0 == iContentType->Des().Compare( KDd2MimeType()  ) ) )
        {
        SetCodFlag( ETrue );
        }

    if( 0 == iContentType->Des().Compare( KMultiPartMimeType() ) )
        {
        iMultiPart = ETrue;
        }

    }

// -----------------------------------------------------------------------------
// CHttpDownload::ParseContentDispositionL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpDownload::ParseContentDispositionL( RStringPool& aStrPool )
    {
    LOGGER_ENTERFN( "ParseContentDispositionL" );

    RStringF contentDisposition = aStrPool.StringF(HTTP::EContentDisposition,RHTTPSession::GetTable());

    TInt index = FindHeaderField( iResponseHeaders, contentDisposition.DesC() );
    if( index != KErrNotFound )
        // store content-disposition
        {
        TPtrC8 rawData( *(*iResponseHeaders)[index]->FieldRawData() );
        TPtrC8 fileNameParm;
		TBool isFileNameParam = EFalse;

        // ';' separates disposition-type from disposition parm
        TInt semicolon = rawData.Locate( KSemiColon );

        if( semicolon != KErrNotFound )
            {
            fileNameParm.Set( rawData.Right( rawData.Length() - semicolon - 1 ) );
            Trim( fileNameParm );
			// filename=<AttachmentFileName>
			TInt offset = fileNameParm.FindC( KHttpFileNameParm );
			if (offset != KErrNotFound)
				{
				// '=' separates value of fileName from the <AttachmentFileName>
				TInt equal = fileNameParm.Locate( KEqual );
				if( equal != KErrNotFound )
					{
					fileNameParm.Set( fileNameParm.Right( fileNameParm.Length() - equal - 1 ) );
					Trim( fileNameParm );
					TInt semicolon1 = fileNameParm.Locate( KSemiColon );

					// find '"' around the <AttachmentFileName>
					TInt quote = fileNameParm.Locate( KQuote );
					if (quote != KErrNotFound)
					{
					fileNameParm.Set( fileNameParm.Right( fileNameParm.Length() - quote - 1 ) );
					Trim( fileNameParm );
					}

					TInt quote1 = fileNameParm.Locate( KQuote );
					if (quote1 != KErrNotFound)
					{
					fileNameParm.Set( fileNameParm. Left(quote1) );
					Trim( fileNameParm );
					}

					if( semicolon1 != KErrNotFound )
						{
							fileNameParm.Set( fileNameParm.Left( semicolon1 ) );
						}
					isFileNameParam = ETrue;
                    // we must strip off any directory names from the filename
                    // first check for and remove any trailing '\' or '/' characters
                    if( ( fileNameParm.Right( 1 ).Locate( '\\' ) != KErrNotFound ) ||
                        ( fileNameParm.Right( 1 ).Locate( '/' ) != KErrNotFound ))
                        {
                        fileNameParm.Set( fileNameParm.Left( fileNameParm.Length() - 1 ));
                        }
                    // next find location of last '/' or '\' to remove any directory names
                    TInt lastSlash = fileNameParm.LocateReverse( '/' );
                    if( lastSlash == KErrNotFound )
                        {
                        lastSlash = fileNameParm.LocateReverse( '\\' );
                        }
                    if( lastSlash != KErrNotFound )
                        {
                        fileNameParm.Set( fileNameParm.Mid( lastSlash + 1 ));
                        }
					}
				rawData.Set( rawData.Left( semicolon ) );  // DispositionType
				Trim( rawData );
		        }
			}

        ReallocateStringL( iDispositionType, rawData, KMaxDispositionTypeLength );

		if( isFileNameParam && (0 == iDispositionType->Des().CompareC( KHttpDispositionTypeAttachment()) ) )
			{
			ReallocateStringL( iAttachmentFileName, fileNameParm );
			iUseAttachmentFileName = ETrue;
			}
		if( isFileNameParam && (0 == iDispositionType->Des().CompareC( KHttpDispositionTypeInline()) ) )
			{
			ReallocateStringL( iAttachmentFileName, fileNameParm );
			iUseInlineFileName = ETrue;
			}
        }
    }

// -----------------------------------------------------------------------------
// CHttpDownload::TriggerEvent
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpDownload::TriggerEvent( THttpDownloadState aDlState,
                                  THttpProgressState aProgState )
    {
#ifdef __DOWNLOADMGR_LOG__  // to filter out EHttpProgResponseBodyReceived
    if( aProgState != EHttpProgResponseBodyReceived )
        {
        CLOG_WRITE_3( "Trigger: %d - %d - %d", aDlState, aProgState, iActiveDownload );
        }
#endif

    if( iSilentMode )
        {
        if( !((aDlState == EHttpDlInprogress && aProgState == EHttpStarted)
            || aDlState == EHttpDlMultipleMOCompleted || aDlState == EHttpDlFailed || aDlState == EHttpDlCompleted ))
            // See EDlAttrSilent
            {
            return;
            }
        }

    if( !iDontFireEvent )
        {
        if( iClAppInstance && iClAppInstance->Observer() )
            {
            // informs client app about status change
            iClAppInstance->Observer()->Event( this, aDlState, aProgState, iActiveDownload );
            }

        if( iPDClAppInstance && iPDClAppInstance->Observer() )
            {
            if( iActiveDownload == iActivePlayedDownload || aDlState == EHttpDlDeleting )
                iPDClAppInstance->Observer()->Event( this, aDlState, aProgState, iActiveDownload );
            }
		}
    }

// -----------------------------------------------------------------------------
// CHttpDownload::UpdatePausable
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpDownload::UpdatePausable()
    {
    LOGGER_ENTERFN( "UpdatePausable" );

    TBool pausable( ETrue );

    if( !iPausableDRM )
        {
        pausable = EFalse;
        }

    if( iMethod == EMethodPOST )
        {
        pausable = EFalse;
        }

    if( pausable != iPausable )
        {
        if( !iCodDownload )
            {
        	iPausable = pausable;

        	// inform client about change
            TriggerEvent( iPausable ? EHttpDlPausable : EHttpDlNonPausable );

            TRAP_IGNORE( StoreDownloadInfoL() );
            }
        }
    }

// -----------------------------------------------------------------------------
// CHttpDownload::ForcedRestartL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpDownload::ForcedRestartL()
    {
    // Forget a previous redirect
    ReallocateStringL( iRedirUrl, *iUrl );
    ReallocateStringL( iCurrentUrl, *iRedirUrl );
    iRedirect = EFalse;

    ReInitializeDownload();
    }

// -----------------------------------------------------------------------------
// CHttpDownload::HeaderFieldL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
HBufC8* CHttpDownload::HeaderFieldL( CArrayPtrFlat<CHeaderField>* aHeaders,
                                             TInt aFieldIndex )
    {
    HBufC8* retVal = NULL;
    RStringPool strPool;

    strPool.OpenL( RHTTPSession::GetTable() );
    // Find field in response header
    RStringF charset = strPool.StringF( aFieldIndex, RHTTPSession::GetTable() );

    TInt index = FindHeaderField( aHeaders, charset.DesC() );
    if( KErrNotFound != index )
        {
        retVal = (*aHeaders)[index]->FieldRawData();
        }

    strPool.Close();

    return retVal;
    }

#ifdef DEAD_CODE
// -----------------------------------------------------------------------------
// CHttpDownload::CheckIfContentUnModified
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CHttpDownload::CheckIfContentUnModified()
    {
    CLOG_WRITE( "CheckIfContentUnModified" );

    RStringPool strPool = iConnHandler->Session().StringPool();
    RHTTPHeaders headers( iTrans.Response().GetHeaderCollection() );

    RStringF lastMod = strPool.StringF(HTTP::ELastModified,RHTTPSession::GetTable());
    THTTPHdrVal value;

    if( !headers.GetField( lastMod, 0, value ) )
        {
        TDateTime date( value );

        if( iDate.Year() != date.Year() ||
            iDate.Month() != date.Month() ||
            iDate.Day() != date.Day() ||
            iDate.Hour() != date.Hour() ||
            iDate.Minute() != date.Minute() ||
            iDate.Second() != date.Second() ||
            iDate.MicroSecond() != date.MicroSecond() )
            {
            CLOG_WRITE( "Date changed" );

            return EFalse;
            }
        }

    return ETrue;
    }
#endif

// -----------------------------------------------------------------------------
// CHttpDownload::CheckAttribMaxLengthL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpDownload::CheckAttribMaxLengthL( THttpDownloadAttrib aAttribute,
	                                       const TDesC16& aValue )
    {
    for( TInt i = 0; KStringAttribMaxLengths[i][0]; ++i )
        {
        if( aAttribute == KStringAttribMaxLengths[i][0] )
            {
            if( aValue.Length() > KStringAttribMaxLengths[i][1] )
                {
                CLOG_WRITE_2( "Overflow length: %d, max-length: %d", aValue.Length(),
                                                    KStringAttribMaxLengths[i][1] );
                User::Leave( KErrOverflow );
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CHttpDownload::CheckAttribMaxLengthL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpDownload::CheckAttribMaxLengthL( THttpDownloadAttrib aAttribute,
	                                       const TDesC8& aValue )
    {
    for( TInt i = 0; KStringAttribMaxLengths[i][0]; ++i )
        {
        if( aAttribute == KStringAttribMaxLengths[i][0] )
            {
            if( aValue.Length() > KStringAttribMaxLengths[i][1] )
                {
                CLOG_WRITE_2( "Overflow length: %d, max-length: %d", aValue.Length(),
                                                    KStringAttribMaxLengths[i][1] );
                User::Leave( KErrOverflow );
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CHttpDownload::SetCookiesL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpDownload::SetCookiesL( RStringPool& aStringPool )
    {
    RStringF propName = aStringPool.OpenFStringL( KCookieUsage );

    CleanupClosePushL( propName );

    SetPropertyL( propName, iUseCookies );

    CleanupStack::PopAndDestroy( &propName );
    }

// -----------------------------------------------------------------------------
// CHttpDownload::DownloadSucceededL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpDownload::DownloadSucceededL()
    {
    CLOG_WRITE( "Transaction succeeded" );

    // inform storage class that it doesn't have to expect any more chunk
    // completed event fired only when the last chunk is persisted
    OnCompletedL();
    iDlStartedByClient = EFalse;
    }

// -----------------------------------------------------------------------------
// CHttpDownload::ParseRequestHeaderAddOnL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpDownload::ParseRequestHeaderAddOnL( const TDesC8& aValue )
    {
    if( !aValue.Length() )
        {
        return;
        }

    TPtrC8 fieldName;
    TPtrC8 fieldData;
    TPtrC8 value( aValue );

    TInt ind( 0 );
    TBool found( EFalse );

    RStringPool strPool = iClAppInstance->ConnHandler()->Session().StringPool();
    TInt fieldInd;

    do
        {
        fieldInd = KErrNotFound;
        // find KColon in the argument string
        for( ind = 0; ind < value.Length() && value[ind] != KColon; ++ind ){};

        if( value[ind] != KColon )
            // no KColon -> Invalid request header
            {
            User::Leave( KErrArgument );
            }

        // store field name
        fieldName.Set( value.Left( ind ) );
        // and remove it from the argument string
        value.Set( value.Right( value.Length() - ind - 1 ) );

        // find field separator
        ind = value.Find( KHttpFieldSeparator );

        if( ind == KErrNotFound )
            // not found -> no more field
            {
            fieldData.Set( value );
            found = EFalse;
            }
        else
            // found -> store raw data
            // and go to next field
            {
            fieldData.Set( value.Left( ind ) );
            value.Set( value.Right( value.Length() - ind - 1 ) );
            found = ETrue;
            }

        // Find out if this is a native request header field, or not.
        // If not, fieldInd is KErrNotFound
        for( TInt i = 0; KRequestHeaderConvTable[i][0]; ++i )
            {
            RStringF fieldNameStr = strPool.StringF( KRequestHeaderConvTable[i][1],
                                                     RHTTPSession::GetTable() );

            if( !fieldName.CompareF( fieldNameStr.DesC() ) )
                // this is a native header field name
                {
                fieldInd = i;
                break;
                }
            }

        CHeaderField* field = CHeaderField::NewL( &fieldName, &fieldData, fieldInd );
        CleanupStack::PushL( field );

        iRequestHeaders->AppendL( field );
        CLOG_WRITE8_3("Req(%d): %S:%S", iRequestHeaders->Count(), &fieldName, &fieldData );

        CleanupStack::Pop( field );
        }while( found );
    }

// -----------------------------------------------------------------------------
// CHttpDownload::LoadHeadersL
// -----------------------------------------------------------------------------
//
void CHttpDownload::LoadHeadersL( RFile& aFile,
                                  CArrayPtrFlat<CHeaderField>* aHeaders )
    {
    TInt headers;
    READ_INT_L( aFile, headers );

    CLOG_WRITE_1("Headers: %d", headers);
    aHeaders->ResetAndDestroy();

    for( TInt i = 0; i < headers; ++i )
        {
        CHeaderField* field = CHeaderField::NewL();
        CleanupStack::PushL( field );

        field->LoadHeaderInfoL( aFile );
        aHeaders->AppendL( field );

/*        if( field->FieldName() && field->FieldRawData() )
            {
            CLOG_WRITE8_1( "%S", field->FieldName() );
            CLOG_WRITE8_1( "%S", field->FieldRawData() );
            }
        else
            {
            CLOG_WRITE8( "Empty field" );
            }
*/
        CleanupStack::Pop( field );
        }
    }

// -----------------------------------------------------------------------------
// CHttpDownload::AppendHeadersL
// -----------------------------------------------------------------------------
//
void CHttpDownload::AppendHeadersL( TPtr8& aBuf,
                                    CArrayPtrFlat<CHeaderField>* aHeaders )
    {
    TInt headers = aHeaders->Count();
    CLOG_WRITE_1("Headers: %d", headers);

    APPEND_BUF_INT( aBuf, headers );

    HBufC8* fieldName = NULL;
    HBufC8* fieldRawData = NULL;
    for( TInt i = 0; i < headers; ++i )
        {
        fieldName = (*aHeaders)[i]->FieldName();
        fieldRawData = (*aHeaders)[i]->FieldRawData();
        AppendBufL( aBuf, fieldName );
        AppendBufL( aBuf, fieldRawData );
        CLOG_WRITE8_2( "%S:%S", fieldName, fieldRawData );
        }
    }

// -----------------------------------------------------------------------------
// CHttpDownload::GetHttpHeadersSize
// -----------------------------------------------------------------------------
//
TInt CHttpDownload::GetHttpHeadersSize(CArrayPtrFlat<CHeaderField>* aHeaders )
    {
    TInt headers = aHeaders->Count();
    CLOG_WRITE_1("Headers: %d", headers);

    HBufC8* fieldName = NULL;
    HBufC8* fieldRawData = NULL;

    TInt size  = 0;

    for( TInt i = 0; i < headers; ++i )
        {
        fieldName = (*aHeaders)[i]->FieldName();
        fieldRawData = (*aHeaders)[i]->FieldRawData();

        size = size + fieldName->Size() +  fieldRawData->Size();

        CLOG_WRITE8_1( "Size = %d:", size );
        }

    return size;

    }


// -----------------------------------------------------------------------------
// CHttpDownload::AddHeaderL
// -----------------------------------------------------------------------------
//
void CHttpDownload::AddHeaderL( THttpDownloadAttrib aAttribute,
								const TDesC8& aValue,
                                const TInt aConversionTable[][2],
                                CArrayPtrFlat<CHeaderField>* aHeaders )
    {
    TInt index( KErrNotFound );

    // search for if this field is already in the
    for( index = 0; aConversionTable[index][0]; ++index )
        {
        if( aConversionTable[index][0] == aAttribute )
            {
            break;
            }
        }

    __ASSERT_DEBUG( index != KErrNotFound, DMPanic( KErrCorrupt ) );

    RStringPool strPool = iClAppInstance->ConnHandler()->Session().StringPool();
    RStringF fieldName = strPool.StringF( aConversionTable[index][1],
                                          RHTTPSession::GetTable() );
    CleanupClosePushL( fieldName );

    // make a copy of the raw data
    HBufC8* value = HBufC8::NewLC( aValue.Length() );
    value->Des().Copy( aValue );

    CHeaderField* newField = CHeaderField::NewL( &fieldName.DesC(), value, index );

    CleanupStack::Pop( value );
    CleanupStack::PushL( newField );

    aHeaders->AppendL( newField );

    CleanupStack::Pop( 2 ); // newField, fieldName
    }

// -----------------------------------------------------------------------------
// CHttpDownload::RedirectedPermanentlyL
// -----------------------------------------------------------------------------
//
void CHttpDownload::RedirectedPermanentlyL( const TDesC8& aNewUrl )
    {
    CLOG_WRITE8_1( "ERedirectedPermanently: %S", &aNewUrl );
    // from now on this new redirected url has to be used
    if( !iRedirect )
        {
        ReallocateStringL( iRedirUrl, aNewUrl, KMaxUrlLength );
        ReallocateStringL( iCurrentUrl, *iRedirUrl, KMaxUrlLength );
        }
    else
        // there has already been a temporary redirection
        // this permanent is not used on next submitted request
        {
        ReallocateStringL( iCurrentUrl, aNewUrl, KMaxUrlLength );
        }
    ParseDownloadNameL();
    StoreDownloadInfoL();   // to store new url

    TriggerEvent( EHttpDlInprogress, EHttpProgRedirectedPermanently );
    }

// -----------------------------------------------------------------------------
// CHttpDownload::RedirectedPermanentlyL
// -----------------------------------------------------------------------------
//
void CHttpDownload::RedirectedTemporaryL( const TDesC8& aNewUrl )
    {
    CLOG_WRITE8_1( "ERedirectedTemporarily: %S", &aNewUrl );
    iRedirect = ETrue;
    ReallocateStringL( iCurrentUrl, aNewUrl, KMaxUrlLength );
    ParseDownloadNameL();
    StoreDownloadInfoL();   // to store iRedirect

    TriggerEvent( EHttpDlInprogress, EHttpProgRedirectedTemporarily );
    }

// -----------------------------------------------------------------------------
// CHttpDownload::FixDownloadNameL
// -----------------------------------------------------------------------------
//
void CHttpDownload::FixDownloadNameL()
    {
    if( !iCodDownload )
        {
        TPtr name( iDlName->Des() );

        for( TInt i = 0; i < name.Length(); ++i )
            {
            TChar currChar = (*iDlName)[i];

            if( currChar.IsAlphaDigit() ||
                currChar.IsGraph() ||
                currChar == 0x20 )  // space
                {
                continue;
                }
            else
                {
                name[i] = KUrlFixChar;
                }
            }

        ConvertDownloadNameUniqueL();
        }
    }

// -----------------------------------------------------------------------------
// CHttpDownload::SetCodFlag
// -----------------------------------------------------------------------------
//
void CHttpDownload::SetCodFlag( TBool aValue )
    {
    if( aValue && iCodDownload )
        {
        // this case we will not overwrite
        // the value of iCodDownload. iCodDownload might be > 1
        return;
        }
    iCodDownload = aValue;
    }

// -----------------------------------------------------------------------------
// CHttpDownload::SelfComplete
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpDownload::SelfComplete( TInt aReason )
    {
    __ASSERT_DEBUG( !IsActive(), DMPanic( KErrGeneral ) );
    SetActive();

    TRequestStatus* dummy = &iStatus;

    User::RequestComplete( dummy, aReason );
    }

// -----------------------------------------------------------------------------
// CHttpDownload::UpdateDestFileNameL
// -----------------------------------------------------------------------------
//
void CHttpDownload::UpdateDestFileNameL()
    {
    LOGGER_ENTERFN( "UpdateDestFileNameL" );
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL(fs);
    TInt32 downloadedSize(0);
    downloadedSize = iStorage->DownloadedSize();
	HBufC16* fileName ;
	fileName = iStorage->DestFilename();
    TPtr fileNamePtr = fileName->Des();
    TDriveUnit currentDrive(fileNamePtr);
    TBuf<KMaxDrives> driveList;
    TInt err( KErrNone );
    TInt drive( EDriveC );
    TBool criticalSpace( EFalse );
    TBool needToUpdatePath( ETrue );

    CLOG_WRITE_1( " entering fileName: %S", fileName );
#ifdef RD_MULTIPLE_DRIVE
    HBufC8* drivesDynList = iClientApp->Engine()->QueryDynDriveListLC();
    TPtrC8 drives( *drivesDynList );
#else
    TPtrC drives( iClientApp->Engine()->iDriveLettersCenRep );
#endif
    // drive letters are separated by semicolons
    for( TInt i = 0; i < drives.Length() && (err || !criticalSpace); i = i + 2 )
        {
        if( ( err = fs.CharToDrive( drives[i], drive )) != KErrNone )
            {
            continue;
            }
        if( drive == currentDrive )//if the current path is same as the path in cenrep then no need to update.The diff is because we must have not known size before hand
            {
            needToUpdatePath = EFalse;
            break;
            }
        else
            {
            TInt bytesToWrite = downloadedSize;
            if (bytesToWrite < 0)
                bytesToWrite = 0;

            TRAP( err, criticalSpace = !SysUtil::DiskSpaceBelowCriticalLevelL(
                                                             &fs, bytesToWrite, drive ));
            }
        }
    if( needToUpdatePath )
        {
        iDlNameChanged = ETrue;
#ifdef RD_MULTIPLE_DRIVE
        // using PathInfo::RootPath to set correct destination folder
        // depending on memory used

        // Getting RootPath for selected Drive
        TFileName rootPath;
        User::LeaveIfError( PathInfo::GetRootPath( rootPath, drive ) );

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
        TChar driveChar;
        fs.DriveToChar(drive, driveChar);
        TBuf<2> buf;
        buf.Append(driveChar);
        fileNamePtr.Replace(0,1,buf);
#endif
        err = fs.MkDirAll( fileNamePtr );//in case the size unknown the dir needs to be formed again as Drive can change
        if ( err != KErrNone && err != KErrAlreadyExists )
            {
            User::Leave( err );
            }
        }
    CLOG_WRITE_1( " exiting fileName: %S", fileName );
#ifdef RD_MULTIPLE_DRIVE
    CleanupStack::PopAndDestroy( drivesDynList );
#endif
    CleanupStack::PopAndDestroy( &fs );
    }

// ---------------------------------------------------------
// CHttpDownload::UpdateDCFRepositoryL()
// Update saved file to DCFRepository
// ---------------------------------------------------------
//
void CHttpDownload::UpdateDCFRepositoryL(
    const TDesC& aFileName )
    {
    LOGGER_ENTERFN( "UpdateDCFRepositoryL" );
    CLOG_WRITE_1( " :UpdateDCFRepositoryL() for: %S", &aFileName );
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
    }

// -----------------------------------------------------------------------------
// CHttpDownload::MoveInDeleteL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CHttpDownload::MoveInDelete(TInt mediaObjectIndex)
	{
	TInt err(KErrNone);
	if(!iCodDlData)
		return err;

	if(	mediaObjectIndex <= iCodDlData->Count())
		{
		TRAP_IGNORE(MoveDownloadedMediaObjectSyncL(mediaObjectIndex));
		TPtrC filenamePtr = ((*iCodDlData)[mediaObjectIndex])->DestFilename();
		if(filenamePtr.Length())
    		{
    		TRAP_IGNORE(NotifyMediaGalleryL( filenamePtr ));
    		}
		}
	return err;
	}


// -----------------------------------------------------------------------------
// CHttpDownload::MoveDownloadedMediaObjectL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpDownload::MoveDownloadedMediaObjectL(TInt mediaObjectIndex)
    {
    LOGGER_ENTERFN( "MoveDownloadedMediaObjectsL" );

    if( iDlState != EHttpDlMultipleMOCompleted && !iMoDownloadCompleted )
        {
        iStorage->SetProgressiveMode( EFalse );
        TriggerEvent( iDlState, EHttpDlProgNonProgressive);
        return;//Move will be Done by COD as MO Completed has not happened
        }

    //__ASSERT_DEBUG( !IsActive(), DMPanic( KErrInUse ) );
    //__ASSERT_DEBUG( !iFileMan, DMPanic( KErrInUse ) );

#if 0

#else

    HBufC* filename = HBufC::NewLC(KMaxFileName);
	TPtr filenamePtr1 = filename->Des();

    RFs &fs = iClientApp->Engine()->Fs();
	if(!iFileMan)
		{
		iFileMan = CFileMan::NewL(fs);
		}

    TPtrC filenamePtr2 = ((*iCodDlData)[mediaObjectIndex])->DestFilename();
    if(!filenamePtr2.Length())
    	{
        CleanupStack::PopAndDestroy(filename);
        return;
    	}

    TInt firstSlashPos = filenamePtr2.Locate( '\\' );

    if(firstSlashPos < 0)
	    {
	    CleanupStack::PopAndDestroy(filename);
	    return;
	    }
  	TPtrC dlName = filenamePtr2.Left(firstSlashPos);

    TInt drive;

    User::LeaveIfError( fs.CharToDrive( dlName[0], drive ));


    // Getting RootPath for selected Drive
    TFileName rootPath;
    User::LeaveIfError( PathInfo::GetRootPath( rootPath, drive ) );

    // Setting KDownloadPath
    rootPath.Append( KDownloadPath );

    filenamePtr1.Copy(rootPath);
    filenamePtr1.Append(_L("\\"));

    TInt error = fs.MkDirAll(filenamePtr1);
    if (error!=KErrNone && error!=KErrAlreadyExists)
       {
        User::Leave(error);
       }
	if( mediaObjectIndex == iActivePlayedDownload )
	    {

	    iFname = ((*iCodDlData)[mediaObjectIndex])->TempFilename();
	    }

	else
		{

	    iFname = ((*iCodDlData)[mediaObjectIndex])->DestFilename();
		}
    // Find a unique name to avoid any conflict.
    // Here iFname has full path of current location of file
    // and filename has destination path.
    FindUniqueDestinationFileNameL( iFname, filename );

    filenamePtr1 = filename->Des();

    User::LeaveIfError( iFileMan->Move( iFname, filenamePtr1, CFileMan::EOverWrite, iStatus ) );
    // waiting for move to complete
    SetActive();
    iFname = filenamePtr1;
    ((*iCodDlData)[mediaObjectIndex])->SetDestFilenameL(filenamePtr1);

    CleanupStack::PopAndDestroy(filename);

#endif
    }

// -----------------------------------------------------------------------------
// CHttpDownload::MoveDownloadedMediaObjectSyncL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpDownload::MoveDownloadedMediaObjectSyncL(TInt mediaObjectIndex)
    {
    LOGGER_ENTERFN( "MoveDownloadedMediaObjectSyncL" );

    if( !iCodDownload && iDlState != EHttpDlMultipleMOCompleted )
        {
        User::Leave( KErrNotReady );
        }

    //__ASSERT_DEBUG( !IsActive(), DMPanic( KErrInUse ) );
    //__ASSERT_DEBUG( !iFileMan, DMPanic( KErrInUse ) );



	RFs &fs = iClientApp->Engine()->Fs();
	if(!iFileMan)
		{
		iFileMan = CFileMan::NewL(fs);
		}

    HBufC* filename = HBufC::NewLC(KMaxFileName);
    TPtr filenamePtr = filename->Des();

    TPtrC fileNamePtr = ((*iCodDlData)[mediaObjectIndex])->DestFilename();
    TInt firstSlashPos = fileNamePtr.Locate( '\\' );
    if(firstSlashPos < 0)
	    {
	    CleanupStack::PopAndDestroy(filename);
	    return;
	    }
  	TPtrC dlName = fileNamePtr.Left(firstSlashPos);
    TInt drive;

    User::LeaveIfError( fs.CharToDrive( dlName[0], drive ));


    // Getting RootPath for selected Drive
    TFileName rootPath;
    User::LeaveIfError( PathInfo::GetRootPath( rootPath, drive ) );

    // Setting KDownloadPath
    rootPath.Append( KDownloadPath );

    filenamePtr.Copy(rootPath);
    filenamePtr.Append(_L("\\"));

    TInt error = fs.MkDirAll(filenamePtr);
    if (error!=KErrNone && error!=KErrAlreadyExists)
       {
        User::Leave(error);
       }
    iFname = ((*iCodDlData)[mediaObjectIndex])->DestFilename();

    // Find a unique name to avoid any conflict.
    // Here iFname has full path of current location of file
    // and filename has destination path.
    FindUniqueDestinationFileNameL( iFname, filename );

    filenamePtr = filename->Des();

    TInt err = iFileMan->Move(iFname, filenamePtr, CFileMan::EOverWrite);
    if(err != KErrNone)
    	{
    	User::LeaveIfError(err);
    	}
    iFname = filenamePtr;
    ((*iCodDlData)[mediaObjectIndex])->SetDestFilenameL(filenamePtr);

    CleanupStack::PopAndDestroy(filename);


    }

// -----------------------------------------------------------------------------
// CHttpDownload::FindUniqueDestinationFileNameL
// -----------------------------------------------------------------------------
//
void CHttpDownload::FindUniqueDestinationFileNameL( TDesC& srcFile, HBufC*& destPath )
    {
        HBufC* fileExtention = HBufC::NewLC(KMaxFileName);
        HBufC* fileName = HBufC::NewLC(KMaxFileName);
        TPtr fileNamePtr( fileName->Des() );

        fileNamePtr = srcFile;
        // Retrieve the file extention.
        TInt dotInd = srcFile.LocateReverse( '.' );
        if( dotInd != KErrNotFound )
            // Filename extension found.
            {
            fileExtention->Des().Copy( srcFile.Right( srcFile.Length() - dotInd ) );
            fileNamePtr.Copy( srcFile.Left( dotInd ) );
            }

        // Retrieve the file name (excluding file extention).
        TInt lastSlashPos = fileNamePtr.LocateReverse( '\\' );
        if( lastSlashPos != KErrNotFound )
            // Filename found.
            {
            fileNamePtr.Copy( fileNamePtr.Right( fileNamePtr.Length() - lastSlashPos - 1 ) );
            }

        // Find a unique file name.
        ConvertDownloadNameUniqueL( destPath, fileName, fileExtention );

        // Found. Append file name and extention to destination path.
        destPath->Des().Append( *fileName );
        destPath->Des().Append( *fileExtention );

        CleanupStack::PopAndDestroy( fileName );
        CleanupStack::PopAndDestroy( fileExtention );
    }


// ---------------------------------------------------------
// CHttpDownload::NotifyMediaGalleryL()
// Notify media gallery about the new file.
// ---------------------------------------------------------
//
void CHttpDownload::NotifyMediaGalleryL( const TDesC& aFileName )
    {

	LOGGER_ENTERFN( "CHttpDownload::NotifyMediaGalleryL" );
    CLOG_WRITE_1(" notifying Gallery and DcfReposory about move for: %S",&aFileName);

#ifdef BRDO_APP_GALLERY_SUPPORTED_FF

    //
    // Notify Media Gallery about new media file
    CMGXFileManager* mgFileManager = MGXFileManagerFactory::NewFileManagerL(
        iClientApp->Engine()->Fs() );
    CleanupStack::PushL( mgFileManager );
    if( aFileName.Length() > 0 )
        {
        TRAP_IGNORE( mgFileManager->UpdateL( aFileName ) );
        }
    else
        {
        TRAP_IGNORE( mgFileManager->UpdateL() );
        }
    CleanupStack::PopAndDestroy( mgFileManager );

#endif
    //
    // Notify DCF repository
    TRAP_IGNORE( UpdateDCFRepositoryL( aFileName ) );
	}
// -----------------------------------------------------------------------------
// CCodSaver::ConvertDownloadNameUniqueL
// -----------------------------------------------------------------------------
//
void CHttpDownload::ConvertDownloadNameUniqueL( HBufC*& filePath,
                                            HBufC*& fileName,
                                            HBufC*& fileExtn)
    {
    TBool bFound( EFalse );
    TInt index( 0 );
    HBufC* uniqueName = NULL;
    HBufC* fullNameTemp = HBufC::NewLC(KMaxFileName);
    TPtr fullNameTempPtr( fullNameTemp->Des() );

    RFs rFs;
  	User::LeaveIfError( rFs.Connect() );
  	CleanupClosePushL(rFs);

    do
        {
        bFound = EFalse;
        //Generate Unique name.
        CreateIndexedNameL( uniqueName, *fileName, index );

        //Name in \\system\\temp.
        fullNameTempPtr.Copy( *filePath );
        fullNameTempPtr.Append( *uniqueName );
        fullNameTempPtr.Append( *fileExtn );

        if( BaflUtils::FileExists( rFs , fullNameTempPtr ) )
            //Check if file name exist in Destination path.
            {
            bFound =ETrue;
            }

        }while( bFound );

    CleanupStack::PopAndDestroy(&rFs);
    CleanupStack::PopAndDestroy(fullNameTemp);

    // This is the unique name that we were looking for.
    CleanupStack::PopAndDestroy(fileName);
    fileName = uniqueName;
    CleanupStack::PushL(fileName);
    }

//  End of File
