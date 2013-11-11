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
* Description: 
*      Implementation of class CCodEngBase.   
*      
*
*/


// INCLUDE FILES

#include "CodEngBase.h"
#include "Connection.h"
#include "HttpLoader.h"
#include "CodSaver.h"
#include "FileSaver.h"
#include "RoapSaver.h"
#include "CodData.h"
#include "CodLoadObserver.h"
#include "CodError.h"
#include "CodStatus.h"
#include "CodPanic.h"
#include "CodUtil.h"
#include "CodLogger.h"
#include "RoapData.h"
#include "CodProgress.h"
#include "MediaObject.h"
#include "DownloadDataClient.h"

#include <AiwGenericParam.h>
#include <DocumentHandler.h>
#include <apmstd.h>
#include <Oma2Agent.h>
#include <RoapDef.h>
#include <f32file.h>
#include <bodypart.h>
#include <sysutil.h>
#include <pathinfo.h>
#include "CodDefs.h"
#include <CodUi.rsg>
#include <AknQueryDialog.h>
#include <StringLoader.h>
#include  <bautils.h>
#include    "FileExt.h"

#ifdef __SYNCML_DM_FOTA
#include <fotaengine.h>
#include "FotaSaver.h"
/// FOTA Update Package type.
_LIT8( KFotaPackageDataType, "application/vnd.nokia.swupd.dp2" );
/// Default FOTA package id.
LOCAL_D const TInt KCodDefaultFotaPkgId = -1;
#endif /*def __SYNCML_DM_FOTA */

#ifdef RD_MULTIPLE_DRIVE
#include <centralrepository.h>
#include <driveinfo.h>
#include <BrowserUiSDKCRKeys.h>
#endif //RD_MULTIPLE_DRIVE

#include <bldvariant.hrh>

// ================= CONSTANTS =======================
const TInt KDownloadInfoIncrSize = 2*4096;     // used for DownloadInfo

/// Name of UTF-8 character set.
_LIT8( KCodUtf8, "utf-8" );

#ifdef RD_MULTIPLE_DRIVE
_LIT( KDefaultDriveListSep, ";" );
_LIT( KDefaultDriveList, "C;E" );
#endif
// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CCodEngBase::~CCodEngBase()
// ---------------------------------------------------------
//
EXPORT_C CCodEngBase::~CCodEngBase()
    {
    Cancel();
    delete iRoapData;

    if(iSaver)
        {
        iSaver->Cleanup( !Pausable() );
        }

    delete iSaver;
    delete iDocHandler;
    delete iLoader;
    delete iConn;
    delete iCodBuf;
    delete iData;
    iFs.Close();
    CLOG(( ECodEng, 2, _L("CCodEngBase::~CCodEngBase") ));
    CLOG(( ECodEng, 2, _L("*****************") ));
    }

// ---------------------------------------------------------
// CCodEngBase::SetL()
// ---------------------------------------------------------
//
EXPORT_C void CCodEngBase::SetL
        (
        const TDesC8& aBuf,
        TRequestStatus* aStatus,
        const CAiwGenericParamList* aParams,
        const RPointerArray<CBodyPart>* aParts
        )
    {
    CLOG(( ECodEng, 0, _L("-> CCodEngBase::SetL aParams=0x%x, aParts=0x%x"), \
        &aParams, aParts ));

    // Misuse asserts.
    __ASSERT_ALWAYS( aStatus, CodPanic( ECodInvalidArguments ) );
    __ASSERT_ALWAYS( iState == EInit || iState == EReady, \
                                            CodPanic( ECodOffState ) );
    // Internal asserts.
    __ASSERT_DEBUG( !iConn, CodPanic( ECodInternal ) );
    __ASSERT_DEBUG( !iLoader, CodPanic( ECodInternal ) );
    __ASSERT_DEBUG( !iParentStatus, CodPanic( ECodInternal ) );
    __ASSERT_DEBUG( !iSaver, CodPanic( ECodInternal ) );
    __ASSERT_DEBUG( !iCodBuf, CodPanic( ECodInternal ) );

    ResetPaths();
    delete iSaver;
    iSaver = NULL;
    iData->Reset();
    iParts = aParts;
    iParams = aParams;
    iType = TDataType();
    iHandler = TUid::Null();
    delete iRoapData;
    iRoapData = NULL;

    // Default parameters.
    TPtrC8 sourceUri;
    TPtrC8 charset( KCodUtf8 );
    iPreferredIap = 0;
    
    // Get params from GenericParams.
    if ( iParams )
        {
        CodUtil::GetDesParamLC( sourceUri, EGenericParamURL, *iParams );
        CodUtil::GetDesParamLC( charset, EGenericParamCharSet, *iParams );
        CodUtil::GetUint32Param( iPreferredIap, EGenericParamAccessPoint, *iParams );
        
        CodUtil::GetUint32Param( iDownloadId, EGenericParamDownloadId, *iParams );
        
        TUint32 AppUId(0);
        CodUtil::GetUint32Param( AppUId, EGenericParamApplication, *iParams );
        iAppUId.iUid = AppUId;

		// Generate COD DL Info folder name - C:\system\dmgr\<appuid>\codinfo\
		
		TBuf<KMaxPath> folderName;
		CODDownloadInfoFolder(folderName);
		
		// TO DO - Create the folder if it does not exist
		
        iCodDlInfoPath.Format( _L("%S%d"), &folderName,iDownloadId  );

        }

    iData->SetSourceUriL( sourceUri );
    iCodBuf = CodUtil::ConvertToUcs2L( aBuf, charset, iFs );

    if ( iParams )
        {
        CleanupStack::PopAndDestroy( 2 );   // Two pushed temp buffer.
        }
        
    iParentStatus = aStatus;
    *iParentStatus = KRequestPending;

    Continue( ESet );

    CLOG(( ECodEng, 0, _L("<- CCodEngBase::SetL (user 8-bit)") ));
    }




// ---------------------------------------------------------
// CCodEngBase::SetL()
// ---------------------------------------------------------
//
EXPORT_C void CCodEngBase::SetL
        (
        const TInt aDownloadId,
        const TUid aDlUid,
        //TRequestStatus* aStatus,        
        const RPointerArray<CBodyPart>* aParts
        )

	{
    ResetPaths();
    delete iSaver;
    iSaver = NULL;
    iData->Reset();
    iParts = aParts;
    //iParams = aParams;
    iType = TDataType();
    iHandler = TUid::Null();
    delete iRoapData;
    iRoapData = NULL;
    

    // Default parameters.
    TPtrC8 sourceUri;
    TPtrC8 charset( KCodUtf8 );
    iPreferredIap = 0;
  	//iLoader->LoadDownloadInfoL()
    //Read from info file
	iAppUId = aDlUid;
	iDownloadId = aDownloadId;
	TBuf<KMaxPath> folderName;
	CODDownloadInfoFolder(folderName); 
	iCodDlInfoPath.Format( _L("%S%d"), &folderName,iDownloadId  );
	
	// Download resumed after Pause 
	iResumedDownload = ETrue;   
   	
   	LoadInfoFilesL();
    	
	}
// ---------------------------------------------------------
// CCodEngBase::CODDownloadInfoFolder()
// ---------------------------------------------------------
//
void CCodEngBase::CODDownloadInfoFolder( TDes& aFolder ) const
    {
    aFolder.Format( KDownloadFolderFormat, 
                        &KDmDefaultDir, 
                        iAppUId,
                        &KInfoFilesCodDirName );
    }

// ---------------------------------------------------------
// CCodEngBase::Data()
// ---------------------------------------------------------
//
EXPORT_C const CCodData& CCodEngBase::Data() const
    {
    return *iData;
    }

// ---------------------------------------------------------
// CCodEngBase::SetObserver()
// ---------------------------------------------------------
//
EXPORT_C void CCodEngBase::SetObserver( MCodLoadObserver* aObserver )
    {
    CLOG(( ECodEng, 2, _L("CCodEngBase::SetObserver(0x%x)"), aObserver ));
    __ASSERT_ALWAYS( !(aObserver && iObserver), \
        CodPanic( ECodObserverAlreadySet ) );
    iObserver = aObserver;
    if ( iSaver )
        {
        iSaver->SetObserver( iObserver );
        }
    if ( iProgress )
        {
        iProgress->SetObserver( iObserver );
        }
    }

// ---------------------------------------------------------
// CCodEngBase::Accept()
// ---------------------------------------------------------
//
EXPORT_C void CCodEngBase::Accept( TRequestStatus* aStatus )
    {
    CLOG(( ECodEng, 0, _L("-> CCodEngBase::Accept") ));
    // Misuse asserts.
    __ASSERT_ALWAYS( iData->IsValid(), CodPanic( ECodInvalidData ) );
    __ASSERT_ALWAYS( aStatus, CodPanic( ECodInvalidArguments ) );
    __ASSERT_ALWAYS( iState == EReady, CodPanic( ECodOffState ) );
    // Internal asserts.
    __ASSERT_DEBUG( !iConn, CodPanic( ECodInternal ) );
    __ASSERT_DEBUG( !iLoader, CodPanic( ECodInternal ) );
    __ASSERT_DEBUG( !iParentStatus, CodPanic( ECodInternal ) );

    iParentStatus = aStatus;
    *iParentStatus = KRequestPending;	
    /*
	OMA 2: If the license element includes the order attribute and the order attribute
	is equal to the post. The contents of the license element MUST NOT be passed 
	to the License Agent unless the Media Object or Product is successfully downloaded		
    */
    if (iIsLicense && !iData->IsPostOrder())
	{
	  // dd2 case with license tag and order tag equals to "any", 
	  // so we should install the ROAP trigger as soon as it became available
	  Continue( EStartInstallRoap );
	}
    else
	{
	  // we should install MO first		
	  // 1) dd1 case
	  // 2) dd2 without license tag
	  // 3) dd2 with license tag and order tag = "POST"
	  __ASSERT_DEBUG( !iSaver, CodPanic( ECodInternal ) );
	  Continue( EStartConnect );
	}
    CLOG(( ECodEng, 0, _L("<- CCodEngBase::Accept") ));
    }

// ---------------------------------------------------------
// CCodEngBase::Reject()
// ---------------------------------------------------------
//
EXPORT_C void CCodEngBase::Reject( TRequestStatus* aStatus )
    {
    CLOG(( ECodEng, 0, _L("-> CCodEngBase::Reject") ));
    // Misuse asserts.
    __ASSERT_ALWAYS( iData->IsValid(), CodPanic( ECodInvalidData ) );
    __ASSERT_ALWAYS( aStatus, CodPanic( ECodInvalidArguments ) );
    __ASSERT_ALWAYS( iState == EReady, CodPanic( ECodOffState ) );
    // Internal asserts.
    __ASSERT_DEBUG( !iConn, CodPanic( ECodInternal ) );
    __ASSERT_DEBUG( !iLoader, CodPanic( ECodInternal ) );
    __ASSERT_DEBUG( !iParentStatus, CodPanic( ECodInternal ) );
//TODO
	if (!iIsLicense ) 
	{
    __ASSERT_DEBUG( !iSaver, CodPanic( ECodInternal ) );
	}

    iParentStatus = aStatus;
    *iParentStatus = KRequestPending;

    iResult = KErrAbort;
    iStatusCode = StatusCode( iResult, iState );
    Continue( EStartNotify );
    CLOG(( ECodEng, 0, _L("<- CCodEngBase::Reject") ));
    }

// ---------------------------------------------------------
// CCodEngBase::Stop()
// ---------------------------------------------------------
//
EXPORT_C void CCodEngBase::Stop()
    {
    CLOG(( ECodEng, 0, _L("-> CCodEngBase::Stop iState(%d)"), iState ));

    // This method is almost the same as DoCancel, yet works differently:
    //
    // When Cancel()->DoCancel() is called, this Active Object is not longer
    // active (set inactive by the framework). That is, RunL will not be called
    // after Cancel(). Anything that is going on stopped abruptly, without
    // any further processing, we are Done() (parent is completed).
    //
    // When Stop is called, we still stay active, so after cancelling the
    // outstanding request (if any), we will complete (RunL will be called)
    // with KErrCancel. That triggers further processing, so we are not Done()
    // yet (parent is not completed).
    //
    // Having this method comes from the logic of how a downloading is
    // cancelled: after cancel, we may still have to continue with notify. In
    // effect, a download must be cancelled twice to stop it; this method
    // supports the double-cancel.

    if ( iStopInProgress )
        {
        // Prevent reentrant calls.
        // Reentrancy normally should not be possible: we call some Cancel()
        // here, and that should not return and should not allow other object
        // to run until done. But it does: disconnecting the WAP stack
        // unfortunately includes waiting on the disconnect_cnf event.
        // This means that there IS a Cancel which allows other objects run.
        CLOG(( ECodEng, 0, _L("<- CCodEngBase::Stop already stopping") ));
        return;
        }
    iStopInProgress = ETrue;

    switch ( iState )
        {
        case ESet:
            {
            // Cancelled before we could start.
            delete iCodBuf;
            iCodBuf = NULL;
            // Own status already completed (by self-completion).
            __ASSERT_DEBUG( iStatus != KRequestPending, \
                CodPanic( ECodInternal ) );
            SelfComplete( KErrCancel );
            // Complete parent.
            iResult = KErrCancel;
            Done();
            break;
            }

        case EStartConnect:
        case EStartFetch:
        case EStartNotify:
            {
            // Already completed (by self-completion).
            __ASSERT_DEBUG( iStatus != KRequestPending, \
                CodPanic( ECodInternal ) );
            SelfComplete( KErrCancel );
            break;
            }

        case EConnect:
            {
            __ASSERT_DEBUG( iConn, CodPanic( ECodInternal ) );
            iConn->Cancel();
            // We are not Done() - parent request is still outstanding, we
            // are still active. RunL will be called with KErrCancel.
            break;
            }

        case EFetch:
            {
            if ( iObserver && iNotificationStatus == ELoad )
                {
                CLOG(( ECodEng, 3, \
                    _L("-> CCodEngBase::Stop notify StartCancelL") ));
                TRAP_IGNORE( iObserver->StartCancelL() );
                iNotificationStatus = ECancel;
                }
            __ASSERT_DEBUG( iLoader, CodPanic( ECodInternal ) );
            iLoader->Cancel();
            // We are not Done() - parent request is still outstanding, we
            // are still active. RunL will be called with KErrCancel.
            break;
            }

        case EInstall:
        case EInstallRoap:
            {
            __ASSERT_DEBUG( iSaver, CodPanic( ECodInternal ) );
            iSaver->CancelInstall();
            // We are not Done() - parent request is still outstanding, we
            // are still active. RunL will be called with KErrCancel.
            break;
            }

        case ENotify:
            {
            __ASSERT_DEBUG( iLoader, CodPanic( ECodInternal ) );
            iLoader->Cancel();
            // We are not Done() - parent request is still outstanding, we
            // are still active. RunL will be called with KErrCancel.
            break;
            }

        case EInit:
        case EReady:
        case EStartInstall:
        default:
            {
            if ( iObserver && iNotificationStatus == EPaused )
                {
                TRAP_IGNORE( iObserver->StartCancelL() );
                iNotificationStatus = ECancel;
                }
            // No request is outstanding, but as this is an user-callable
            // method, tolerate misuse (and do nothing).
            break;
            }
        }
    iStopInProgress = EFalse;
    CLOG(( ECodEng, 0, _L("<- CCodEngBase::Stop") ));
    }

// ---------------------------------------------------------
// CCodEngBase::RemovableMedia()
// ---------------------------------------------------------
//
EXPORT_C TBool CCodEngBase::RemovableMedia() const
    {
    return iRemovableMedia;
    }

// ---------------------------------------------------------
// CCodEngBase::GetPath()
// ---------------------------------------------------------
//
EXPORT_C TPtrC CCodEngBase::GetPath() const
    {
    if(!iData || !iData->Count() || !(*iData)[iData->ActiveDownload()]->iFullName )
        return KNullDesC();
    return *((*iData)[iData->ActiveDownload()]->iFullName);
    }

// ---------------------------------------------------------
// CCodEngBase::GetDestFilePath()
// ---------------------------------------------------------
//
EXPORT_C TPtrC CCodEngBase::GetDestFilePath (TInt aMOIndex ) const
    {
    if(iData && aMOIndex >=1 && aMOIndex <= iData->Count())
        {
        if((*iData)[aMOIndex]->iFullName)
        	return *((*iData)[aMOIndex]->iFullName);
        }
    return TPtrC( KNullDesC );
    }   

// ---------------------------------------------------------
// CCodEngBase::GetType()
// ---------------------------------------------------------
//
EXPORT_C const TDataType& CCodEngBase::GetType() const
    {
    return iType;
    }

// ---------------------------------------------------------
// CCodEngBase::GetHandler()
// ---------------------------------------------------------
//
EXPORT_C TUid CCodEngBase::GetHandler() const
    {
    return iHandler;
    }

// ---------------------------------------------------------
// CCodEngBase::GetStatusCode()
// ---------------------------------------------------------
//
EXPORT_C TInt CCodEngBase::GetStatusCode() const
    {
    return iStatusCode;
    }

// ---------------------------------------------------------
// CCodEngBase::GetRoapData()
// ---------------------------------------------------------
//
EXPORT_C const CRoapData* CCodEngBase::GetRoapData() const
    {
    return iRoapData;
    }

// ---------------------------------------------------------
// CCodEngBase::SetCodDlAttached()
// ---------------------------------------------------------
//
EXPORT_C void CCodEngBase::SetCodDlAttached(const TBool aValue)
    {
       iAttached = aValue;
       if(iSaver)
       {
        iSaver->SetProgressiveMode(aValue);	
       }
       
    }
    
// ---------------------------------------------------------
// CCodEngBase::CCodEngBase()
// ---------------------------------------------------------
//
CCodEngBase::CCodEngBase( MCodLoadObserver* aObserver )
: CActive( CActive::EPriorityStandard ),
  iState( EInit ),
  iObserver( aObserver ),
  iStopInProgress( EFalse ),
  iPreferredIap( 0 ),
  iNotificationStatus( ENull ),
  iFsUsed( EFalse ),
  iDownloadId( 0 ),
#ifdef RD_MULTIPLE_DRIVE 
  iSpaceAvailOk( EFalse ),
  iAvailDriveSpace( -1 ),
#else
  iPhoneMemoryOk( EFalse ),
  iMmcOk( EFalse ),
#endif
  iRemovableMedia( EFalse ),
  iStatusCode( KHttp902UserCancelled ),
  iResult( KErrGeneral ),
  iContentTypeCheck ( EFalse ),
  iResumedDownload ( EFalse )  
    {
    CLOG(( ECodEng, 2, _L("") ));
    CLOG(( ECodEng, 2, _L("*****************") ));
    CLOG(( ECodEng, 2, _L("CCodEngBase::CCodEngBase") ));
    CLOG(( ECodEng, 2, _L8("Build time " __DATE__ " " __TIME__) ));
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// CCodEngBase::ConstructL()
// ---------------------------------------------------------
//
void CCodEngBase::ConstructL( CEikProcess* aProcess )
    {
    iData = CCodData::NewL();
    iDocHandler = CDocumentHandler::NewL( aProcess );
    User::LeaveIfError( iFs.Connect() );
#ifdef RD_MULTIPLE_DRIVE
    QueryDriveListL();
#endif
    }

// ---------------------------------------------------------
// CCodEngBase::DoCancel()
// ---------------------------------------------------------
//
void CCodEngBase::DoCancel()
    {
    CLOG(( ECodEng, 2, _L("-> CCodEngBase::DoCancel iState(%d)"), iState ));

    switch ( iState )
        {
        case ESet:
        case EStartConnect:
        case EStartFetch:
        case EStartNotify:
            {
            // Already completed (by self-completion).
            __ASSERT_DEBUG( iStatus != KRequestPending, \
                CodPanic( ECodInternal ) );
            SelfComplete( KErrCancel );
            break;
            }

        case EConnect:
            {
            // Instant abort, without any further processing
            // (no install, no notify).
            __ASSERT_DEBUG( iConn, CodPanic( ECodInternal ) );
            iConn->Cancel();
            break;
            }
            
        case EFetch:
        case ENotify:
		case EProductNotify:
            {
            // Instant abort, without any further processing
            // (no install, no notify).
            __ASSERT_DEBUG( iLoader, CodPanic( ECodInternal ) );
            iLoader->Cancel();
            break;
            }

        case EInstall:
            {
            // Instant abort, without any further processing (no notify).
            __ASSERT_DEBUG( iSaver, CodPanic( ECodInternal ) );
            iSaver->CancelInstall();
            break;
            }
            
        case EInit:
        case EReady:
        case EStartInstall:
        default:
            {
            // No requests should be outstanding in these states.
            CodPanic( ECodInternal );
            break;
            }
        }

    iStatusCode = KHttp902UserCancelled;    // "Clarity only" code.
    iResult = KErrCancel;                   // "Clarity only" code.
    Done();                                 // Complete parent.
    CLOG(( ECodEng, 2, _L("<- CCodEngBase::DoCancel") ));
    }


EXPORT_C void CCodEngBase::Pause()
    {
    CLOG(( ECodEng, 2, _L("-> CCodEngBase::Pause iState(%d)"), iState ));

    switch ( iState )
        {
        case EFetch:
            {
            // Instant abort, without any further processing
            // (no install, no notify).
            __ASSERT_DEBUG( iLoader, CodPanic( ECodInternal ) );
            iLoader->Pause();
            //iLoader->CompleteTransaction();
            break;
            }
        }

    //Done();                                 // Complete parent.
    CLOG(( ECodEng, 2, _L("<- CCodEngBase::Pause") ));
    }

// ---------------------------------------------------------
// CCodEngBase::RunL()
// ---------------------------------------------------------
//
void CCodEngBase::RunL()
    {
    CLOG(( ECodEng, 2, _L("-> CCodEngBase::RunL iStatus(%d) iState(%d)"), \
        iStatus.Int(), iState ));

    switch( iState )
        {
        case ESet:
            {
            SetL();
            break;
            }

        case EStartInstallRoap:
            {
            StartInstallRoapL();
            break;
            }

        case EInstallRoap:
            {
            EndInstallRoapL();
            break;
            }

        case EStartConnect:
            {
            StartConnectL();
            break;
            }

        case EConnect:
            {
            EndConnectL();
            break;
            }

        case EStartFetch:
            {
            StartFetchL();
            break;
            }

        case EFetch:
            {
            EndFetchL();
            break;
            }

        case EStartInstall:
            {
            StartInstallL();
            break;
            }

        case EInstall:
            {
            EndInstallL();
            break;
            }
            
        case EStartNotify:
            {
            StartNotifyL();
            break;
            }

        case ENotify:
            {
            EndNotifyL();
            break;
            }
        case EMediaChange:
        	{
        	ChangeMediaObjectL();
        	break;
        	}
        case EStartProductNotify:
            {
            StartProductNotifyL();
            break;
            }
        case EProductNotify:
            {
            EndProductNotifyL();
            break;            
            }
        case EInit:
        case EReady:
        default:
            {
            // No requests should be outstanding in these states.
            CLOG(( ECodEng, 0, _L("CCodEngBase::RunL: unexpected state") ));
            CodPanic( ECodInternal );
            break;
            }
        }
    CLOG(( ECodEng, 2, _L("<- CCodEngBase::RunL") ));
    }

// ---------------------------------------------------------
// CCodEngBase::RunError()
// ---------------------------------------------------------
//
TInt CCodEngBase::RunError( TInt aError )
    {
    CLOG(( ECodEng, 2, \
        _L("-> CCodEngBase::RunError iStatus(%d) iState(%d) aError(%d)"), \
        iStatus.Int(), iState, aError ));
    switch( iState )
        {
        case ESet:
            {
            delete iCodBuf;
            iCodBuf = NULL;
            // Failed - classify reason and notify about failure.
            iStatusCode = StatusCode( aError, iState );
            iResult = aError;
            Continue( EStartNotify );
            break;
            }

        case EStartInstallRoap:
        case EInstallRoap:
			{
            iStatusCode = StatusCode( aError, iState );
            iResult = aError;
            Continue( EStartNotify );
            break;
			}
        case EStartConnect:
        case EConnect:
        case EStartFetch:
        case EFetch:
        case EStartInstall:
        case EInstall:
            {
            
            if(aError != KErrCodHttpServerError)
                {
                //if PreconditionFailed check if the updated DD URI is present
                //cancel the download if not set
                if( aError == KErrCodHttpPreconditionFailed )
                    {
                if( iData && iData->UpdatedDDUriL().Length() )
                        {
                        iState = EInit;//in case of 412 we should restart from first state
                        iStatusCode =  StatusCode( aError, iState );
                    	TBuf<KMaxPath>   folderName;
                    	CODDownloadInfoFolder(folderName);

                    	HBufC* fileNameBuf = HBufC::NewLC( KMaxPath );
                    	TPtr fileName = fileNameBuf->Des();

                    	fileName.Format( _L("%S%d"), &folderName,iDownloadId  );
                        RFs fs;
                        User::LeaveIfError( fs.Connect() );
                        fs.Delete( fileName );//delete the info path as download has to begin from fresh state
                        CleanupStack::PopAndDestroy( fileNameBuf );	
                        fs.Close();                    
                        }
                    else
                        {
                        // Failed - classify reason and notify about failure.
                        iStatusCode = StatusCode( aError, iState );
                        iResult = aError;
                                            
                        //If ther is no Updated DD Uri. Cancel the download.
                        DoCancel();
                        break;
                        }
                    }
                else
                    {
                    //If user cancel the download, delete the media object also.
                    //If paused due to other errors, delete the file if non-pausable.
                    if(iSaver)
                        {
                        TBool delContent = (aError == KErrCancel) ? ETrue : !( iData && (*iData)[iData->ActiveDownload()]->iPausable );
                        iSaver->Cleanup( delContent );
                        }
                    }
                if( iObserver && KErrCodHttpPreconditionFailed != aError)
                    {
                    //TODO : Pass the error code if required
                    iObserver->DownloadPaused();
                    iNotificationStatus = EPaused;
                    } 
                    
                }
            delete iSaver;  // Cleans up.
            iSaver = NULL;

            // Failed - classify reason and notify about failure.
            iStatusCode = StatusCode( aError, iState );
            iResult = aError;
            Continue( EStartNotify );
            break;
            }

        case EStartNotify:
        case ENotify:
            {
            //If Notify Fails do not delete download.
            //Ignore the error code.
            //End State Machine.Download is Successful.
            Done();
            break;
            }
        case EStartProductNotify:
        case EProductNotify:
            {
            Done();
            break;
            }

        case EInit:
        case EReady:
        default:
            {
            // No requests should be outstanding in these states.
            CLOG(( ECodEng, 0, _L("CCodEngBase::RunError: unexpected state") ));
            CodPanic( ECodInternal );
            break;
            }
        }
    
    if (iData && iData->Count())
    	{
	    (*iData)[iData->ActiveDownload()]->iStatusCode = iStatusCode;
    	(*iData)[iData->ActiveDownload()]->iResult = iResult;
    	}
    
    CLOG(( ECodEng, 2, _L("<- CCodEngBase::RunError") ));
    return KErrNone;
    }

// ---------------------------------------------------------
// CCodEngBase::SetL()
// ---------------------------------------------------------
//
void CCodEngBase::SetL()
    {
    CLOG(( ECodEng, 2, _L("-> CCodEngBase::SetL (internal)") ));
    __ASSERT_DEBUG( iState == ESet, CodPanic( ECodInternal ) );
    __ASSERT_DEBUG( iCodBuf, CodPanic( ECodInternal ) );
    
    ParseDataL();

    iData->SetActiveDownload ( 1 );
    if( !(*(*iData)[iData->ActiveDownload()]).Name().Length() )
    {
    //No name To display
    //add "unnamed"
     (*(*iData)[iData->ActiveDownload()]).SetNameL( _L("Unnamed") ); 
    }
    CLOG(( ECodEng, 4, _L("CCodEngBase::SetL: parsed OK") ));
    delete iCodBuf;
    iCodBuf = NULL;
    CheckDataL();   // TODO move that out from derived classes.
    
    TInt err = KErrNone;
    for( TInt i = 1; i <= iData->Count() ; ++i )
        {
        iData->SetActiveDownload ( i );
        //So that this download is considered when resume happens
        //only in progress download can resume
        (*iData)[ iData->ActiveDownload()]->iState = EInProgress; 
        TRAP( err , CapabilityCheckL());
        if( !err )
            {
            break;
            }
        else if( err != KErrCodInsufficientSpace )
            {
            User::Leave( err );            
            }        
        }
        
    if( err )
    	{
	    User::Leave( err );
    	}
    iState = EReady;
    iResult = KErrNone;
    iStatusCode = StatusCode( iResult, iState );

    //Create info files
    //This would create info file for album and each media track
    
    StoreInfoFilesL();
    
    // Update album and track info in server
    UpdateMediaInfoL();
    
    if (iIsLicense && !iData->IsPostOrder())
	{
      // install Roap trigger first
      DoneDD2();  
	}
    else
	{
      Done();
	}

    CLOG(( ECodEng, 2, _L("<- CCodEngBase::SetL (internal)") ));
    }

// ---------------------------------------------------------
// CCodEngBase::SetFileNameL()
// ---------------------------------------------------------
//
void CCodEngBase::SetFileNameL()
    {

    // Full name that we need to construct
    HBufC* fullName = HBufC::NewLC(KMaxFileName);
    TPtr fullNamePtr( fullName->Des() );
    fullNamePtr.Append( (*iData)[iData->ActiveDownload()]->iTempPath );            // "c:\\system\\temp\\"
    fullNamePtr.Append( (*iData)[iData->ActiveDownload()]->Name() );        // "c:\\system\\temp\\myfile"
    
   
    
    const TDataType& type( (*iData)[iData->ActiveDownload()]->Types().MdcaPoint( 0) );
    
    //Following code is added to fix the bug DRAI-7KA8TT( File is not available in Gallery even when download list shows "Saved to Gallery".)
    //Fix applied: Changing the content type that is passed to document handler to "application/vnd.oma.drm.content" when the received content type is
    //"application/vnd.oma.drm.message"
    //This is because document handler is not changing the extension when content type is passed as "application/vnd.oma.drm.message"  
    if(0==type.Des8().Compare(KOma1DrmMessageContentType))
        {
        // Add extention to full name of file
        iDocHandler->CheckFileNameExtension(fullNamePtr, TDataType(KOma1DcfContentType)); // "c:\\system\\temp\\myfile.mp3"    
        }
    else
        {
        // Add extention to full name of file
        iDocHandler->CheckFileNameExtension(fullNamePtr, type ); // "c:\\system\\temp\\myfile.mp3"    
        }

    CleanupStack::Pop(fullName);
    
    // This is the Full name that we want
    delete (*iData)[iData->ActiveDownload()]->iFullName;
    (*iData)[iData->ActiveDownload()]->iFullName = fullName;
    
    }

// ---------------------------------------------------------
// CCodEngBase::SetUniqueFileNameL()
// ---------------------------------------------------------
//
void CCodEngBase::SetUniqueFileNameL()
    {
    HBufC* fullName = HBufC::NewLC(KMaxFileName);
    TPtr fullNamePtr( fullName->Des() );
    fullNamePtr.Append( (*iData)[iData->ActiveDownload()]->iFullName->Des() );
    
    // Get the file path
    HBufC* filePath = HBufC::NewLC(KMaxFileName);
    filePath->Des().Append( (*iData)[iData->ActiveDownload()]->iTempPath );        // "c:\\system\\temp\\"

    // Retrieve extention as a string
    HBufC* extention = HBufC::NewLC(KMaxFileName);
    TInt dotInd = fullNamePtr.LocateReverse( '.' );
    if( dotInd != KErrNotFound )
        // filename extension found.
        {
        extention->Des().Copy( fullNamePtr.Right( fullNamePtr.Length() - dotInd ) );
        }

    // File name without extention
    HBufC* fileName = HBufC::NewLC(KMaxFileName);
    fileName->Des().Append( (*iData)[iData->ActiveDownload()]->Name() );    // "myfile"
    TInt dot = fileName->LocateReverse( '.' );//check if Dot is part of name in DD file
    // Find a unique file name. If fileName already exists, then this function will
    // change fileName to some other unique name (like "myfile(1)")
    if( dot != KErrNotFound )
        {
        //Remove Extension 
        //File Name is with an extension.   
        fileName->Des().Copy( fileName->Des().Left(dot));         
        }        
    iSaver->ConvertDownloadNameUniqueL( filePath, fileName, extention );
    HBufC* dispName = HBufC::NewLC(KMaxFileName);
    TPtr displayName(dispName->Des());
    displayName.Append(*fileName);
    if( dot != KErrNotFound )
        {
        //Since Name is with extension Append to Display Name
        displayName.Append(*extention);
        }
    (*iData)[iData->ActiveDownload()]->SetNameL( displayName );
    CleanupStack::PopAndDestroy(dispName);


    // Now when we have a unique file name, create the full name (with path and extention)
    fullNamePtr.Copy( *filePath );
    fullNamePtr.Append( *fileName );
	fullNamePtr.Append( *extention );           // Example - "c:\\system\\temp\\myfile(1).mp3"
    CleanupStack::PopAndDestroy(fileName);
    CleanupStack::PopAndDestroy(extention);
    CleanupStack::PopAndDestroy(filePath);
    CleanupStack::Pop(fullName);
    
    // This is the Full name that we want
    delete (*iData)[iData->ActiveDownload()]->iFullName;
    (*iData)[iData->ActiveDownload()]->iFullName = fullName;

    }

// ---------------------------------------------------------
// CCodEngBase::StartConnectL()
// ---------------------------------------------------------
//
void CCodEngBase::StartConnectL()
    {
    CLOG(( ECodEng, 2, _L("-> CCodEngBase::StartConnectL") ));
    __ASSERT_DEBUG( iState == EStartConnect, CodPanic( ECodInternal ) );		
    if (!iIsLicense ) 
	{
       __ASSERT_DEBUG( !iSaver, CodPanic( ECodInternal ) );
	}
    __ASSERT_DEBUG( !iConn, CodPanic( ECodInternal ) );
    

        //For the first track it is done already
        CapabilityCheckL(); 

        //Inform server about the Active download
        iObserver->SetActiveDownload( );
        iObserver->ContentTypeChanged();
        iObserver->MediaObjectNameChanged();
                
        //Content type check should be done for subsequent tracks
        iContentTypeCheck = EFalse;
        
        //Download is resumed for the current track in the album. For the subsequent tracks(downloads),iResumedDownload should be false
        iResumedDownload = EFalse;
        
        
        //Let all download that begin be in this state and then go to subsequent Succeeded state
        (*iData)[iData->ActiveDownload()]->iState = EInProgress;     

    if ( iFsUsed && (*iData)[iData->ActiveDownload()]->iTempPath == KNullDesC )
        {
        // Select drive before IAP selection.
        SetPathsL();
        SetFileNameL();       
        //Store in Info File
        StoreSubInfoFileL( NULL, iData->ActiveDownload() );
        }
    else
        {
        //Paused Download change display name if not already done
        iObserver->MediaObjectNameChanged();
        }

    if ( CodUtil::IsCidSchemeL( (*iData)[iData->ActiveDownload()]->Url() ) &&
         ( !(*iData)[iData->ActiveDownload()]->InstallNotify().Length() ||
           CodUtil::IsCidSchemeL( (*iData)[iData->ActiveDownload()]->InstallNotify() )
         )
       )
        {
        CLOG(( ECodEng, 3, _L("  all URI-s are cid -> no connect") ));
        // All URI-s are cid, no need to connect.
        // Synchronous state change - we are already under RunL().
        iState = EConnect;
        EndConnectL();
        }
    else
        {
        // We have at least one non-cid scheme URI, need to connect.
        CLOG(( ECodEng, 3, _L("  connecting") ));
        iConn = CConnection::NewL();
        iConn->ConnectL( iPreferredIap, &iStatus );
        iState = EConnect;
        SetActive();
        if ( iObserver )
            {
            CLOG(( ECodEng, 3, \
                _L("-> CCodEngBase::StartConnectL notify StartLoadL(%d)"), \
                KHttp900Success ));
            iObserver->StartLoadL( KHttp900Success );
            iNotificationStatus = ELoad;
            }
        }

    CLOG(( ECodEng, 2, _L("<- CCodEngBase::StartConnectL") ));
    }

// ---------------------------------------------------------
// CCodEngBase::EndConnectL
// ---------------------------------------------------------
//
void CCodEngBase::EndConnectL()
    {
    CLOG(( ECodEng, 2, _L("-> CCodEngBase::EndConnectL iStatus(%d)"), \
        iStatus.Int() ));
    __ASSERT_DEBUG( iState == EConnect, CodPanic( ECodInternal ) );

    User::LeaveIfError( iStatus.Int() );        // Handle errors in RunError().

    // Synchronous state change - we are already under RunL().
    iState = EStartFetch;
    StartFetchL();
    CLOG(( ECodEng, 2, _L("<- CCodEngBase::EndConnectL") ));
    }

// ---------------------------------------------------------
// CCodEngBase::StartFetchL()
// ---------------------------------------------------------
//
void CCodEngBase::StartFetchL()
    {
    CLOG(( ECodEng, 2, _L("-> CCodEngBase::StartFetchL") ));
    __ASSERT_DEBUG( iState == EStartFetch, CodPanic( ECodInternal ) );
    __ASSERT_DEBUG( !iLoader, CodPanic( ECodInternal ) );
    if (!iIsLicense )
	{
      __ASSERT_DEBUG( !iSaver, CodPanic( ECodInternal ) );
	}

    // Setup progress.
    // Fetch size is known. ROAP & notify traffic is estimated.
    if(!iProgress)
        {
        iProgress = new (ELeave) TCodProgress( iObserver );

        TInt roapBytes = (*iData)[iData->ActiveDownload()]->Type().Compare( KOma2TriggerContentType ) ?
            0 :
            KRoapProgressMax;
/*
            iProgress->Setup
            (
            iData->Size() + roapBytes ,
            iData->InstallNotify().Length() ? KCodNotifyTraffic : 0 
            );    
*/
            iProgress->Setup
            (
            (*iData)[iData->ActiveDownload()]->Size() + roapBytes ,
            iData->InstallNotify().Length() ? KCodNotifyTraffic : 0 
            ); 
        }

    if ( CodUtil::IsCidSchemeL( (*iData)[iData->ActiveDownload()]->Url() ) )
        {
        if ( !iParts )
            {
            // Cannot resolve cid: scheme URI-s without the multipart parts!
            CLOG(( ECodEng, 4, _L("  no multipart, fail") ));
            User::Leave( KErrNotFound );
            }
        CBodyPart* part = CodUtil::PartByCidL( *iParts, (*iData)[iData->ActiveDownload()]->Url() );
        if ( !part )
            {
            CLOG(( ECodEng, 4, _L("  part not found, fail") ));
            User::Leave( KErrNotFound );
            }
		CreateSaverL( part->ContentType() );
        User::LeaveIfError( iSaver->AppendData( part->Body() ) );
        iSaver->CloseStore();
        iProgress->IncrementL( part->Body().Size() );
        // Synchronous state change - we are already under RunL().
        iState = EFetch;
        EndFetchL();
        }
    else
        {
        __ASSERT_DEBUG( iConn, CodPanic( ECodInternal ) );
        iLoader = CHttpLoader::NewL( *iConn, NULL, iProgress ,this);
        HBufC8* url = CodUtil::AbsoluteUrlLC
            ( iData->SourceUri(), (*iData)[iData->ActiveDownload()]->Url() );
        iLoader->LoadL( *url, *this, &iStatus );
        CleanupStack::PopAndDestroy( url );

        iState = EFetch;
        SetActive();
	}
    
    CLOG(( ECodEng, 2, _L("<- CCodEngBase::StartFetchL") ));
    }

// ---------------------------------------------------------
// CCodEngBase::EndFetchL
// ---------------------------------------------------------
//
void CCodEngBase::EndFetchL()
    {
    CLOG(( ECodEng, 2, _L("-> CCodEngBase::EndFetchL iStatus(%d)"), \
        iStatus.Int() ));
    __ASSERT_DEBUG( iState == EFetch, CodPanic( ECodInternal ) );

    User::LeaveIfError( iStatus.Int() );        // Handle errors in RunError().

    __ASSERT_DEBUG( iSaver, CodPanic( ECodInternal ) );
//TODO: ??  iState == EStoreOpen (for dd2) EStoreClose - for dd1
    iSaver->CheckResponseAttributesL( *iData ); // Leaves if response not OK.

	if (iIsLicense)
	{
    iStatusCode = KHttp956LicenseSuccess;
	}
	else
	{
    iStatusCode = KHttp900Success;
	}
    iResult = KErrNone;
    // Synchronous state change - we are already under RunL().
    iState = EStartInstall;
    StartInstallL();
    CLOG(( ECodEng, 2, _L("<- CCodEngBase::EndFetchL") ));
    }

// ---------------------------------------------------------
// CCodEngBase::StartInstallL()
// ---------------------------------------------------------
//
void CCodEngBase::StartInstallL()
    {
    CLOG(( ECodEng, 2, _L("-> CCodEngBase::StartInstallL iStatusCode(%d)"), \
        iStatusCode ));
    __ASSERT_DEBUG( iState == EStartInstall, CodPanic( ECodInternal ) );
    __ASSERT_DEBUG( iSaver, CodPanic( ECodInternal ) );

    //__ASSERT_DEBUG( iStatusCode == KHttp900Success, CodPanic( ECodInternal ) );

    (*iData)[iData->ActiveDownload()]->iState = ETobeInstalled;
    
    if( iData->Count() > 1)
        {
        // Check to see if all tracks are downloaded. Install will be done only at the end
        for( TInt i = 1; i <= iData->Count() ; ++i )
            {
            TInt state ((*iData)[i]->iState);
            if( state != ETobeInstalled && state != ESucceeded )
        	    {
        	    Continue(EInstall);
        	    return;
        	    }
            }
        }

    TInt installErr( KErrNone );

	if (iObserver && (iObserver->ConnError() == KErrTimedOut))
	{
		User::LeaveIfError( KErrTimedOut );
	}
	else
	    {
	    if( iData->Count() > 1)
	        {
            TRAP( installErr, iSaver->BulkInstallL( &iStatus, *iData, iAttached ));
	        }
        else
	        {
            if (!iIsDd2)
                {
                TRAP( installErr, iSaver->InstallL( &iStatus, (*iData)[iData->ActiveDownload()]->Name(), iAttached ));
                }
            else
                {
                TRAP( installErr, iSaver->InstallL( &iStatus, (*iData)[iData->ActiveDownload()]->iFullName->Des(), iAttached )); 
                }
            //Move has happened update destination file name
	        (*iData)[ iData->ActiveDownload()]->iFullName = iSaver->NameL();
	        }
	    }

    // if download is DRM-protected content with Preview-only rights
    // then we cannot move content, but will launch it from the temp file
    // that it is currently in.  For all other errors we leave.
    if( installErr == KDRMErrPreviewRights )
        {
        iDrmPreviewOnly = ETrue;
        }
    else
        {
        User::LeaveIfError( installErr );
        }
    SetActive();
    iState = EInstall;
    CLOG(( ECodEng, 4, _L("<- CCodEngBase::StartInstallL") ));
    }

// ---------------------------------------------------------
// CCodEngBase::StartInstallRoapL()
// ---------------------------------------------------------
//
void CCodEngBase::StartInstallRoapL()
    {
    CLOG(( ECodEng, 2, _L("-> CCodEngBase::StartInstallRoapL iStatusCode(%d)"), \
        iStatusCode ));
    __ASSERT_DEBUG( iState == EStartInstallRoap, CodPanic( ECodInternal ) );
//TODO check if we have a retry the iSaver could be NULL ?
    if (iSaver)
	{
	iSaver->InstallL( &iStatus, (*iData)[iData->ActiveDownload()]->Name(), ETrue );
	}
    // Synchronous state change - we are already under RunL().
    iState = EInstallRoap;
    SetActive();

    CLOG(( ECodEng, 4, _L("<- CCodEngBase::StartInstallRoapL") ));
    }

// ---------------------------------------------------------
// CCodEngBase::EndInstallRoapL()
// ---------------------------------------------------------
//
void CCodEngBase::EndInstallRoapL()
    {
    CLOG(( ECodEng, 2, _L("-> CCodEngBase::InstallRoapL iStatusCode(%d)"), \
        iStatusCode ));

    User::LeaveIfError( iStatus.Int() );        // Handle errors in RunError().
//TODO: make sure that 956 and 900 sent
    iStatusCode = KHttp956LicenseSuccess;
    iResult = KErrNone;
//	iSaver->CloseStore();
	iState = EStartConnect;
	StartConnectL();
    CLOG(( ECodEng, 4, _L("<- CCodEngBase::EndInstallRoapL") ));
    }

// ---------------------------------------------------------
// CCodEngBase::EndInstallL()
// ---------------------------------------------------------
//
void CCodEngBase::EndInstallL()
    {
    CLOG(( ECodEng, 2, _L("-> CCodEngBase::EndInstallL iStatus(%d)"), \
        iStatus.Int() ));
    __ASSERT_DEBUG( iState == EInstall, CodPanic( ECodInternal ) );

    User::LeaveIfError( iStatus.Int() );    // Handle errors in RunError().
    // Synchronous state change - we are already under RunL().
    iState = EStartNotify;
    StartNotifyL();
    CLOG(( ECodEng, 4, _L("<- CCodEngBase::EndInstallL") ));
    }
    
// ---------------------------------------------------------
// CCodEngBase::StartNotifyL
// ---------------------------------------------------------
//
void CCodEngBase::StartNotifyL()
    {
    __ASSERT_DEBUG( iState == EStartNotify, CodPanic( ECodInternal ) );

    if ( iData && iData->Count() > 0 && iData->ActiveDownload() && (*iData)[iData->ActiveDownload()]->InstallNotify().Length() )
        {
	    CLOG(( ECodEng, 2, _L("-> CCodEngBase::StartNotifyL iStatusCode(%d)"), \
	        iStatusCode ));

        CLOG(( ECodEng, 4, _L("CCodEngBase::StartNotifyL: init request") ));
        if ( !iProgress )
            {
            // Setup notify-only progress.
            iProgress = new (ELeave) TCodProgress( iObserver );
            iProgress->Setup( 0, KCodNotifyTraffic );    
            }
        iProgress->StartPhaseL( TCodProgress::ENotify );
        TBool notifyObserver = EFalse;
        // When notifying after fetch, we already have a loader (and the
        // observer has already been notified about load).
        // When notifying without fetch, we don't have a loader yet (and the
        // observer has not been notified about load yet).
        if ( !iConn )
            {
            iConn = CConnection::NewL();
            }
        // For notify, attach only (do no create new connection).
        iConn->AttachL( iPreferredIap );
        if( !iLoader )
            {
            iLoader = CHttpLoader::NewL( *iConn, NULL, iProgress ,this );
            notifyObserver = ETrue;
            }
        HBufC8* url = CodUtil::AbsoluteUrlLC
            ( (*iData)[iData->ActiveDownload()]->SourceUri(), (*iData)[iData->ActiveDownload()]->InstallNotify() );
        iLoader->NotifyL( *url, StatusText( iStatusCode ), &iStatus );
        CleanupStack::PopAndDestroy( url );
        iState = ENotify;
        SetActive();
        if ( iObserver && notifyObserver )
            {
            if ( iStatusCode == KHttp902UserCancelled ||
                 iStatusCode == KHttp921UserAborted )
                {
                CLOG(( ECodEng, 3, \
                    _L("-> CCodEngBase::StartNotifyL notify StartCancelL") ));
                iObserver->StartCancelL();
                iNotificationStatus = ECancel;
                }
            else
                {
                CLOG(( ECodEng, 3, \
                    _L("-> CCodEngBase::StartNotifyL notify StartLoadL(%d)"), \
                    iStatusCode ));
                iObserver->StartLoadL( iStatusCode );
                iNotificationStatus = ELoad;
                }
            }
        }
    else
        {
        // No install notify attribute, regarded as successful notification.
        CLOG(( ECodEng, 4, _L("CCodEngBase::StartNotifyL: nothing to do") ));
        Continue( ENotify );
        }
    CLOG(( ECodEng, 2, _L("<- CCodEngBase::StartNotifyL") ));
    }

// ---------------------------------------------------------
// CCodEngBase::EndNotifyL
// ---------------------------------------------------------
//
void CCodEngBase::EndNotifyL()
    {
    CLOG(( ECodEng, 2, _L("-> CCodEngBase::EndNotifyL iStatus(%d)"), \
        iStatus.Int() ));
    __ASSERT_DEBUG( iState == ENotify, CodPanic( ECodInternal ) );

    delete iLoader;
    iLoader = NULL;
    delete iConn;
    iConn = NULL;

    User::LeaveIfError( iStatus.Int() );    // Handle errors in RunError().

    // Do not leave after this point! Notify was successful, we must keep
    // downloaded content now.
    
    if ( iProgress )
        {
        TRAP_IGNORE( iProgress->DoneL() );
        delete iProgress;
        iProgress = NULL;
        }

    if( iSaver && iResult == KErrNone )
        {
//TODO: check if in case of PD we should release. It is already done.
        iSaver->ReleaseContent( (*iData)[iData->ActiveDownload()]->iFileName, iHandler );

        if ((!(*iData)[iData->ActiveDownload()]->iFullName) || (!((*iData)[iData->ActiveDownload()]->iFullName->Compare(KNullDesC))))
            {
            delete (*iData)[iData->ActiveDownload()]->iFullName;
            (*iData)[iData->ActiveDownload()]->iFullName = (*iData)[iData->ActiveDownload()]->iFileName.AllocL();
            }
        
        iType = iSaver->DataType();
        }

    //Done();
    Continue ( EMediaChange );
    CLOG(( ECodEng, 2, _L("<- CCodEngBase::EndNotifyL") ));
    }

// ---------------------------------------------------------
// CCodEngBase::Done
// ---------------------------------------------------------
//
void CCodEngBase::Done()
    {
    CLOG(( ECodEng, 2, _L("-> CCodEngBase::Done iResult(%d)"), iResult ));

    if( iSaver )
        {
        iSaver->Cleanup( !(*iData)[iData->ActiveDownload()]->iPausable );
        }

    delete iSaver;  // Cleans up unless Release()-d.
    iSaver = NULL;	
    delete iLoader;
    iLoader = NULL;
    delete iConn;
    iConn = NULL;
    delete iCodBuf;
    iCodBuf = NULL;
    delete iProgress;
    iProgress = NULL;

    if( iObserver )
        {
        CLOG(( ECodEng, 3, \
            _L("CCodEngBase::Done notify Done code(%d) err(%d)"), \
            iStatusCode, iResult ));
        iObserver->Done( iStatusCode, iResult );
        iNotificationStatus = ENull;
        }

    // Reinit state.
    iState = iData->IsValid() ? EReady : EInit;
    // Notify parent.
    __ASSERT_DEBUG( iParentStatus, CodPanic( ECodInternal ) );
    User::RequestComplete( iParentStatus, iResult );
    iParentStatus = NULL;
    CLOG(( ECodEng, 2, _L("<- CCodEngBase::Done") ));
    }

// ---------------------------------------------------------
// CCodEngBase::DoneDD2
// ---------------------------------------------------------
//
void CCodEngBase::DoneDD2()
    {
    CLOG(( ECodEng, 2, _L("-> CCodEngBase::Done iResult(%d)"), iResult ));
    delete iLoader;
    iLoader = NULL;
    delete iConn;
    iConn = NULL;
    delete iCodBuf;
    iCodBuf = NULL;
    delete iProgress;
    iProgress = NULL;

    if( iObserver )
        {
        CLOG(( ECodEng, 3, \
            _L("CCodEngBase::Done notify Done code(%d) err(%d)"), \
            iStatusCode, iResult ));
        iObserver->Done( iStatusCode, iResult );
        iNotificationStatus = ENull;
        }

    // Reinit state.
    iState = iData->IsValid() ? EReady : EInit;
    // Notify parent.
    __ASSERT_DEBUG( iParentStatus, CodPanic( ECodInternal ) );
    User::RequestComplete( iParentStatus, iResult );
    iParentStatus = NULL;
    CLOG(( ECodEng, 2, _L("<- CCodEngBase::Done") ));
    }

// ---------------------------------------------------------
// CCodEngBase::Continue
// ---------------------------------------------------------
//
void CCodEngBase::Continue( TState aNextState )
    {
    CLOG(( ECodEng, 2, _L("-> CCodEngBase::Continue nextState(%d)"), \
        aNextState ));
    __ASSERT_DEBUG( !IsActive(), CodPanic( ECodInternal ) );

    iState = aNextState;
    TRequestStatus* ownStatus = &iStatus;
    *ownStatus = KRequestPending;
    SetActive();
    User::RequestComplete( ownStatus, KErrNone );
    CLOG(( ECodEng, 2, _L("<- CCodEngBase::Continue") ));
    }

// ---------------------------------------------------------
// CCodEngBase::SelfComplete
// ---------------------------------------------------------
//
void CCodEngBase::SelfComplete( TInt aError )
    {
    // This method safely handles the case when a request may be completed
    // multiple times.
    // The main use is for content viewing (EContentView state), where
    // there is no real external request made and the request can complete
    // the following ways:
    // - Embedded viewer exits and calls NotifyExit()
    // - CodEng is Cancelled (e.g. deleted, if Exit is issued inside the
    //   embedded viewer.
    CLOG(( ECodEng, 2, _L("CCodEngBase::SelfComplete(%d)"), aError ));
    if ( iStatus == KRequestPending )
        {
        // Request is pending, complete now.
        CLOG(( ECodEng, 4, _L("  completing now") ));
        TRequestStatus* ownStatus = &iStatus;
        User::RequestComplete( ownStatus, aError );
        }
    else
        {
        // Request already completed.
        // - If this second completion is error, override status.
        // - If this second completion is success, don't override - existing
        //   result may be error and we can't undo that.
        CLOG(( ECodEng, 4, _L("  already completed") ));
        if ( aError != KErrNone )
            {
            iStatus = aError;
            }
        }
    }

// ---------------------------------------------------------
// CCodEngBase::CapabilityCheckL
// ---------------------------------------------------------
//
void CCodEngBase::CapabilityCheckL()
    {
    CLOG(( ECodEng, 2, _L("-> CCodEngBase::CapabilityCheckL") ));
    __ASSERT_DEBUG( iData->ActiveDownload(),CodPanic( ECodInternal ));
    // 1. Data type checking.
    TInt typeErr( KErrNone );    
#ifdef __SYNCML_DM_FOTA
    TBool fota( EFalse );
#endif /*def __SYNCML_DM_FOTA */
    for ( TInt i = 0; i < (*iData)[iData->ActiveDownload()]->Types().MdcaCount() && !iContentTypeCheck; i++ )
        {
        const TDataType& type( (*iData)[iData->ActiveDownload()]->Types().MdcaPoint( i ) );
#ifdef __TEST_COD_LOG
        TPtrC8 mime( type.Des8() );
#endif /* def __TEST_COD_LOG */

        if (
            type == TDataType( KOma1XmlRoContentType ) ||
            type == TDataType( KOma1WbxmlRoContentType )
           )
            {
            // DRM related types which are not proper files (not saved to FS).
            CLOG(( ECodEng, 4, _L8("  <%S> DRM1 rights OK"), &mime ));
            iIsDd2 = EFalse;
            }
        else if ( type == TDataType( KOma1DrmMessageContentType ) )
            {
            // Accept DRM message. The system supports it but
            // DocHandler does not report this.
            CLOG(( ECodEng, 4, _L8("  <%S> DRM1 msg OK"), &mime ));
            iFsUsed = ETrue;
            }
#ifdef __DRM_OMA2
        else if (
            type == TDataType( KOma2RoContentType ) ||
            type == TDataType( KOma2ProtectedRoType ) ||
            type == TDataType( KOma2TriggerContentType )
           )
            {
            // DRM related types which are not proper files (not saved to FS).
            CLOG(( ECodEng, 4, _L8("  <%S> DRM2 rights OK"), &mime ));
            iIsDd2 = EFalse;            
            }
#endif /*def __DRM_OMA2*/
#ifdef __SYNCML_DM_FOTA
        else if ( type == TDataType( KFotaPackageDataType ) )
            {
            // Accept FOTA download. Special storage (not saved to FS).
            CLOG(( ECodEng, 4, _L8("  <%S> FOTA OK"), &mime ));
            fota = ETrue;
            }
#endif /*def __SYNCML_DM_FOTA */
        else 
            {
            CLOG(( ECodEng, 4, _L8("  <%S> DocHandler check"), &mime ));
            iFsUsed = ETrue;
            
            if ( !iResumedDownload )
                {
                TRAP( typeErr, iDocHandler->CanOpenL( type ));
                if (typeErr == KMimeNotSupported)
                    {
                    CAknQueryDialog* dialog = CAknQueryDialog::NewL();
                    dialog->PrepareLC(R_COD_UI_CONFIRM_NOTE);
                    HBufC* prompt;
                    prompt = StringLoader::LoadLC(R_QTN_BROWSER_DOWNLOADS_UNSUPPORTED_CONTENT_QUERY);
                    dialog->SetPromptL(*prompt);  
                    CleanupStack::PopAndDestroy( prompt );
                    if(!dialog->RunLD())
                        {
                        User::Leave( KHttp902UserCancelled );
                        }
                    }
                }
            }
        }

    // 2. Storage space checking. (Depends on data types.)
   
    TInt size = (*iData)[iData->ActiveDownload()]->Size() - (*iData)[iData->ActiveDownload()]->DownloadedSize();
    TInt bytesToWrite = size;
    if (bytesToWrite < 0)
        bytesToWrite = 0;

    if ( iFsUsed )
        {
        CLOG(( ECodEng, 2, _L("  FS check (%d)"), size ));
#ifdef RD_MULTIPLE_DRIVE 
        TInt err( KErrNone );
        iSpaceAvailOk = EFalse;
        User::LeaveIfError(
                DriveInfo::GetDefaultDrive( DriveInfo::EDefaultPhoneMemory, iAvailDriveSpace ) );

        HBufC8* drivesDynList = QueryDynDriveListLC();
        TPtrC8 drives( *drivesDynList );
        
        // drive letters are separated by semicolons
        for( TInt i = 0; i < drives.Length() && (err || !iSpaceAvailOk); i = i + 2 )
            {
            if( (err = iFs.CharToDrive( drives[i], iAvailDriveSpace )) == KErrNone )
                {
                // Check if there's enough memory in the phone
        	    TRAP( err, iSpaceAvailOk = !SysUtil::DiskSpaceBelowCriticalLevelL(
                                                        &iFs,
        			                                    bytesToWrite,
                                                        iAvailDriveSpace ));
                }
            else
                {
                CLOG(( ECodEng, 4, _L("Bad drive letter [%c]"), drives[i] ));
                }            
            }
        CleanupStack::PopAndDestroy( drivesDynList );

        if( err || !iSpaceAvailOk )
            {
            User::Leave( KErrCodInsufficientSpace );
            }
#else
        iPhoneMemoryOk = EFalse;
        iMmcOk = EFalse;
        // Check C:
        iPhoneMemoryOk = !SysUtil::FFSSpaceBelowCriticalLevelL( &iFs, bytesToWrite );
        CLOG(( ECodEng, 4, _L("  iPhoneMemoryOk(%d)"), iPhoneMemoryOk ));
        RFeatMgr featMgr;
        featMgr.OpenL();
        CleanupClosePushL<RFeatMgr>( featMgr );
        if ( featMgr.FeatureSupported( KFeatureIdMmc ) )
            {
            // If no MMC is inserted, this will leave (and not set iMmcOk).
            TRAP_IGNORE( iMmcOk = !SysUtil::MMCSpaceBelowCriticalLevelL
                ( &iFs, bytesToWrite ); )
            }
        CleanupStack::PopAndDestroy();  // featMgr
        CLOG(( ECodEng, 4, _L("  iMmcOk(%d)"), iMmcOk ));
        if( !(iPhoneMemoryOk || iMmcOk) )
            {
            User::Leave( KErrCodInsufficientSpace );
            }
#endif
        }

	//When reached here, it is assured that capability check is done successfully
	//for the active download.
	//Content type check is done for this track
	iContentTypeCheck = ETrue;
	
    CLOG(( ECodEng, 2, _L("<- CCodEngBase::CapabilityCheckL") ));
    }

// ---------------------------------------------------------
// CCodEngBase::CreateSaverL
// ---------------------------------------------------------
//
CCodSaver* CCodEngBase::CreateSaverL( const TDesC8& aType )
    {
    CLOG(( ECodEng, 2, _L8("-> CCodEngBase::CreateSaverL <%S>"), &aType ));
//TODO:    __ASSERT_DEBUG( !iSaver, CodPanic( ECodInternal ) );
    // TODO check attribute match (early reject mismatched or unsupported types)!

    if( aType == KNullDesC8 )
    	{
    	return NULL;
    	}
    
// DD1 and DD2 cases
    if (( aType.Find( KOma2TriggerContentType ) != KErrNotFound) || ( aType.Find( KDd2DataType ) != KErrNotFound )|| ( aType.Find( KOma1WbxmlRoContentType ) != KErrNotFound )|| ( aType.Find( KOma1XmlRoContentType ) != KErrNotFound ))
        {
        iSaver = CRoapSaver::NewL( aType, iRoapData, iProgress, KRoapProgressMax, (*iData)[iData->ActiveDownload()]->iTempPath, (*iData)[iData->ActiveDownload()]->iRootPath, KNullDesC());
        iSaver->SetObserver( iObserver );
        iSaver->SetParams( iParams );
        iSaver->SetMaxSize( iData->Size() );
        iSaver->OpenStoreL();   // TODO unneeded method, put to construction.
        }
#ifdef __SYNCML_DM_FOTA
    else if ( aType.Find( KFotaPackageDataType ) != KErrNotFound  )
        {
        TInt pkgId( KCodDefaultFotaPkgId );
        if ( iParams )
            {
            CodUtil::GetIntParam( pkgId, EGenericParamFotaPkgId, *iParams );
            }
        iSaver = CFotaSaver::NewL( aType, pkgId );
        iSaver->SetObserver( iObserver );
        iSaver->SetParams( iParams );
        iSaver->SetMaxSize( iData->Size() );
        iSaver->OpenStoreL();   // TODO unneeded method, put to construction.
        }
#endif /*def __SYNCML_DM_FOTA */
    else
        {
        // TODO the assert below crashes without early reject!
        //__ASSERT_DEBUG( iFsUsed, CodPanic( ECodInternal ) );

        TBool contentTypeMisMatch ( ETrue );
        for ( TInt i = 0; i < (*iData)[iData->ActiveDownload()]->Types().MdcaCount(); i++ )
            {
            const TDataType& type( (*iData)[iData->ActiveDownload()]->Types().MdcaPoint( i ) );
            if( ( aType.Find (type.Des8()) != KErrNotFound ) || 
                            ( (type.Des8().Find(KOma1DrmMessageContentType)!= KErrNotFound) && (aType.Find(KOma1DcfContentType)!= KErrNotFound )  ))             
                {
                contentTypeMisMatch = EFalse;
                break;
                }                
            }
        if(contentTypeMisMatch)
            {
            User::Leave(KErrCodAttributeMismatch);
            }
        
        // Do this only if Unique filename check has never been performed earlier for this file.
        if (( !(*iData)[iData->ActiveDownload()]->IsUniqueFilenameSet() ))
            {
            // Append an index to filename in case file with same name already exists
            SetUniqueFileNameL();
            (*iData)[iData->ActiveDownload()]->UniqueFilenameSet( ETrue );
            
            CArrayPtrFlat<CHeaderField>* headers = NULL;
            if (iLoader)
            	headers = iLoader->ResponseHeaders();
            
            // Store in Sub-Info File
            StoreSubInfoFileL( headers, iData->ActiveDownload() );
            // Name changed. Inform server.
            iObserver->MediaObjectNameChanged();
            }
        
        if (iIsDd2)
		    {
            iSaver = CFileSaver::NewL( ((*iData)[iData->ActiveDownload()]->Types().MdcaPoint( 0)), iFs, *iDocHandler, (*iData)[iData->ActiveDownload()]->iTempPath, (*iData)[iData->ActiveDownload()]->iRootPath, (*iData)[iData->ActiveDownload()]->iFullName->Des());
		    }
        else
		    {
            iSaver = CFileSaver::NewL( aType, iFs, *iDocHandler, (*iData)[iData->ActiveDownload()]->iTempPath, (*iData)[iData->ActiveDownload()]->iRootPath, (*iData)[iData->ActiveDownload()]->iFullName->Des());
		    }
        iSaver->SetObserver( iObserver );
        iSaver->SetParams( iParams );
        iSaver->SetMaxSize( iData->Size() );
        iSaver->OpenStoreL();   // TODO unneeded method, put to construction.
		iSaver->ReleaseFileName( (*iData)[iData->ActiveDownload()]->iFileName);
		//send message that PDL is available. The DlMgrUiLib will have a "Play" option available
        /*
        OMA 2:
        If the Media Object of the Product containing the Media Object has a license element, 
        the Download Agent MUST NOT make the Media Object(s) available for rendering before 
        the License has been successfully installed..
        Only the order tag equals to any allows to install the License element first.
        */
          if ((iIsDd2 && iObserver && iData && (*iData)[iData->ActiveDownload()]->ProgressiveDownload() && !iIsLicense) || 
             (iIsDd2 && iObserver && iData && (*iData)[iData->ActiveDownload()]->ProgressiveDownload() &&  iIsLicense && !iData->IsPostOrder()))
		  {
          iSaver->ReleaseFileName( (*iData)[iData->ActiveDownload()]->iFileName);
          iObserver->PdPlayAvailable();
		  }
        }

    CLOG(( ECodEng, 2, _L("<- CCodEngBase::CreateSaverL") ));
    return iSaver;
    }

// ---------------------------------------------------------
// CCodEngBase::SetPathsL
// ---------------------------------------------------------
//
void CCodEngBase::SetPathsL()
    {
    CLOG(( ECodEng, 2, _L("-> CCodEngBase::SetPathsL") ));
#ifdef RD_MULTIPLE_DRIVE 
    __ASSERT_DEBUG( iSpaceAvailOk, CodPanic( ECodInternal ) );
#else
    __ASSERT_DEBUG( iPhoneMemoryOk || iMmcOk, CodPanic( ECodInternal ) );
    __ASSERT_DEBUG( iFsUsed, CodPanic( ECodInternal ) );
#endif
    (*iData)[iData->ActiveDownload()]->iTempPath = KNullDesC;
    (*iData)[iData->ActiveDownload()]->iRootPath = KNullDesC;
#ifdef RD_MULTIPLE_DRIVE 
    if ( iSpaceAvailOk )
        {
        User::LeaveIfError( PathInfo::GetRootPath( (*iData)[iData->ActiveDownload()]->iRootPath, iAvailDriveSpace ) );
        }
#else    
    if ( iPhoneMemoryOk && iMmcOk && iObserver ) 
        {
        iObserver->GetRootPathL( (*iData)[iData->ActiveDownload()]->iRootPath );
        if(!(*iData)[iData->ActiveDownload()]->iRootPath.Compare(_L("C:\\")))
            {
                (*iData)[iData->ActiveDownload()]->iRootPath = PathInfo::PhoneMemoryRootPath();
                
            }
        }
    else if ( iPhoneMemoryOk )
        {
        (*iData)[iData->ActiveDownload()]->iRootPath = PathInfo::PhoneMemoryRootPath();
        }
    else
        {
        __ASSERT_DEBUG( iMmcOk, CodPanic( ECodInternal ) );
        (*iData)[iData->ActiveDownload()]->iRootPath = PathInfo::MemoryCardRootPath();
        }
#endif

 	TParsePtrC rootPath( (*iData)[iData->ActiveDownload()]->iRootPath );
 	TBuf<KMaxPath> tempBuf;
 	// Set temp path to the same drive.
    tempBuf.Format( KDownloadFolderFormat, 
                    &KCodDefaultTempDir, 
                    iAppUId,
                    &KTempFilesCodDirName );      

    (*iData)[iData->ActiveDownload()]->iTempPath.Append(rootPath.Drive());
    (*iData)[iData->ActiveDownload()]->iTempPath.Append(tempBuf);

    TDriveInfo info;
    TDriveUnit unit( rootPath.Drive() );
    User::LeaveIfError( iFs.Drive( info, unit ) );
    // Create the temp directory earlier in case it's not created yet
    iFs.MkDirAll( (*iData)[iData->ActiveDownload()]->iTempPath ); 
    if ( info.iDriveAtt & KDriveAttRemovable )
        {
        iRemovableMedia = ETrue;
        }
        
    CLOG(( ECodEng, 2, _L("<- CCodEngBase::SetPathsL root<%S> temp<%S>"), \
        &(*iData)[iData->ActiveDownload()]->iRootPath, &(*iData)[iData->ActiveDownload()]->iTempPath ));
    }

// ---------------------------------------------------------
// CCodEngBase::ResetPaths
// ---------------------------------------------------------
//
void CCodEngBase::ResetPaths()
    {
    iFsUsed = EFalse;
#ifdef RD_MULTIPLE_DRIVE 
    iSpaceAvailOk = EFalse;
    DriveInfo::GetDefaultDrive( DriveInfo::EDefaultPhoneMemory, iAvailDriveSpace );
#else
    iPhoneMemoryOk = EFalse;
    iMmcOk = EFalse;
#endif
    //(*iData)[iData->ActiveDownload()]->iTempPath = KNullDesC;
    //(*iData)[iData->ActiveDownload()]->iRootPath = KNullDesC;
    iRemovableMedia = EFalse;
    }

#ifdef RD_MULTIPLE_DRIVE
//------------------------------------------------------------------------
//CCodEngBase::QueryDynDriveListLC
//------------------------------------------------------------------------
HBufC8* CCodEngBase::QueryDynDriveListLC()
    {
    TDriveList driveList;
    TInt driveCount( 0 );
    TChar driveLetter;
    TBuf8<KMaxDriveListStrLen> driveLettersDyn;

    // Checking validity of drives in Cenrep List
    // drive letters are separated by semicolons
    // Destination is FFS in default
    TInt drive;
    User::LeaveIfError(
        DriveInfo::GetDefaultDrive( DriveInfo::EDefaultPhoneMemory, drive ) );

    for( TInt i = 0; i < iDriveLettersCenRep.Length(); i = i + 2 )
        {
        if( iFs.CharToDrive( iDriveLettersCenRep[i], drive ) == KErrNone )
            {
            TUint status;
            if ( DriveInfo::GetDriveStatus( iFs, drive, status ) == KErrNone )
                {
                if ( ( status & ( DriveInfo::EDriveUserVisible | DriveInfo::EDrivePresent ) ) )
                    {
                    CLOG(( ECodEng, 0, _L("-> CCodEngBase::QueryDynDriveListLC Drive is present and visible")));
                    }
                }
            }
        }

    // get the list of drives available in real time
    if ( DriveInfo::GetUserVisibleDrives( iFs, driveList, driveCount ) == KErrNone )
        {
        if ( iDriveLettersCenRep.Length() > 0 )
            {
                driveLettersDyn.Append( iDriveLettersCenRep );
                if ( driveLettersDyn[driveLettersDyn.Length() - 1] != ';' )
                    {
                    driveLettersDyn.Append( KDefaultDriveListSep );
                    }
            }
        TInt count( driveList.Length() );
        for ( TInt i( 0 ); i < count; ++i )
            {
            if ( driveList[ i ] )
                {
                User::LeaveIfError( iFs.DriveToChar( i, driveLetter) );
                TInt drivePos = driveLettersDyn.LocateF( driveLetter );
                if ( drivePos == KErrNotFound )
                    {
                    driveLettersDyn.Append( driveLetter ); 
                    driveLettersDyn.Append( KDefaultDriveListSep ); 
                    }
                }
            }
        CLOG(( ECodEng, 2, _L("<- CCodEngBase::QueryDynDriveListLC Pref. drive list dynamic: [%S]"), \
                &driveLettersDyn));
        }

    HBufC8* driveLetters = HBufC8::NewLC( KMaxDriveListStrLen );
    driveLetters->Des().Copy( driveLettersDyn );
    return driveLetters;
    }

//------------------------------------------------------------------------
//CCodEngBase::QueryDriveList
//------------------------------------------------------------------------
void CCodEngBase::QueryDriveListL()
    {
    CRepository* repository = CRepository::NewLC( KCRUidBrowser );
    
    if( repository->Get(KBrowserDrivePrefListForDownloadedContent, iDriveLettersCenRep) != KErrNone )
        {
        CLOG(( ECodEng, 0, _L("-> CCodEngBase::QueryDriveListL Drive list not found")));
        iDriveLettersCenRep.Copy( KDefaultDriveList );
        }
    CLOG(( ECodEng, 2, _L("<- CCodEngBase::QueryDriveListL Pref. drive list: [%S]"), \
        &iDriveLettersCenRep));    
    
    CleanupStack::PopAndDestroy( repository );
    }

#endif

//------------------------------------------------------------------------
//CCodEngBase::DownloadID
//------------------------------------------------------------------------
TUint32 CCodEngBase::DownloadID()
	{
	return iDownloadId;
	}

//------------------------------------------------------------------------
//CCodEngBase::SetPausable
//------------------------------------------------------------------------

void CCodEngBase::SetPausable(TBool aPausable)
    {
    (*iData)[iData->ActiveDownload()]->iPausable = aPausable;
    if( iData->Count() > 1 && aPausable )
    	{
    	iObserver->UpdatePausable(aPausable);	
    	}
    else if(iData->Count()  ==  1)
        {
        iObserver->UpdatePausable(aPausable);
        }	    	
    }

//------------------------------------------------------------------------
//CCodEngBase::SetResumePDAvailable
//------------------------------------------------------------------------

void CCodEngBase::SetResumePDAvailable()
	{
	if(iData && iData->ActiveDownload() 
		     && iData->Count() 
			 && (*iData)[iData->ActiveDownload()]->ProgressiveDownload())
		{
		iObserver->DownloadResumedPdAvailable();	
		}
	}
        
//------------------------------------------------------------------------
//CCodEngBase::StoreFilePathsL
//------------------------------------------------------------------------
void CCodEngBase::StoreFilePathsL(TPtr8& aBuf)
	{
    HBufC* buf1 = (*iData)[iData->ActiveDownload()]->iTempPath.AllocLC();
	AppendBufL(aBuf,buf1);
	
	HBufC* buf2 = (*iData)[iData->ActiveDownload()]->iRootPath.AllocLC();
	AppendBufL(aBuf,buf2);
	
	CleanupStack::PopAndDestroy(2,buf1);
	AppendBufL(aBuf,(*iData)[iData->ActiveDownload()]->iFullName);
		
	}

//------------------------------------------------------------------------
//CCodEngBase::LoadFilePathsL
//------------------------------------------------------------------------
void CCodEngBase::LoadFilePathsL(RFile& aInFile)
	{
	HBufC* buf1 = NULL;
	HBufC* buf2 = NULL;	
	ReadHBufCL(aInFile,buf1);
	(*iData)[iData->ActiveDownload()]->iTempPath = *buf1;
	
	ReadHBufCL(aInFile,buf2);
	
	(*iData)[iData->ActiveDownload()]->iRootPath = *buf2;
	
	ReadHBufCL(aInFile,(*iData)[iData->ActiveDownload()]->iFullName);
	delete buf1;
	delete buf2;
	}

//------------------------------------------------------------------------
//CCodEngBase::Start()
//------------------------------------------------------------------------
// 
EXPORT_C void CCodEngBase::Start( const CAiwGenericParamList* aParams, TRequestStatus* aStatus ) 
	{
	iParentStatus = aStatus;
	*iParentStatus = KRequestPending;
	iParams = aParams;
    // Get params from GenericParams.
    if ( iParams )
        {
        CodUtil::GetUint32Param( iPreferredIap, EGenericParamAccessPoint, *iParams );
        }
    
	Continue( EStartConnect );
	}
	
//------------------------------------------------------------------------
//CCodEngBase::ChangeMediaObjectL()
//------------------------------------------------------------------------
// 
void CCodEngBase::ChangeMediaObjectL()
	{
	__ASSERT_DEBUG( iState == EMediaChange, CodPanic( ECodInternal ) );
	
	TInt mediaObjectCount = iData->Count();	
    if( mediaObjectCount && iData->ActiveDownload() )
    	{
    	(*iData)[iData->ActiveDownload()]->iStatusCode = iStatusCode;
    	(*iData)[iData->ActiveDownload()]->iResult = iResult;
     	iObserver->Done( iStatusCode, iResult );
    
		if( iStatusCode == KHttp900Success )
	    	{
	    	(*iData)[iData->ActiveDownload()]->iState = ESucceeded;
        	StoreSubInfoFileL(NULL, iData->ActiveDownload() );
	    	}

    	UpdateTrackInfoL();
    
	    if( iData->ActiveDownload() != mediaObjectCount &&( (*iData)[iData->ActiveDownload()]->iResult != KErrCancel 
	                                                    &&  (*iData)[iData->ActiveDownload()]->iResult != KErrAbort
	                                                    &&  (*iData)[iData->ActiveDownload()]->iResult != KErrCodWapConnectionDropped
	                                                    &&  (*iData)[iData->ActiveDownload()]->iResult != KErrCodHttpDownloadPaused
	                                                    &&  (*iData)[iData->ActiveDownload()]->iResult != KErrCodInvalidDescriptor))
    		{
      		if( iSaver )
          		{
          		iSaver->Cleanup( !(*iData)[iData->ActiveDownload()]->iPausable );
          		}	    		
			delete iSaver;  // Cleans up unless Release()-d.
		    iSaver = NULL;	
		    delete iLoader;
		    iLoader = NULL;
		    delete iConn;
		    iConn = NULL;
		    delete iCodBuf;
		    iCodBuf = NULL;

    		TInt count = iData->ActiveDownload();
		  	do
		      {
		      count++;
		      }
    			while( count <= iData->Count() && !( (*iData)[count]->State() == EQueued 
	            		|| (*iData)[count]->State() == EInProgress )
	            		);
	    
		    if( count <= iData->Count())
		        {
		    	iData->SetActiveDownload(count);
				Continue( EStartConnect );	    		
				return;
		        }
   			}
      	}     
    Continue ( EStartProductNotify );     
	}

//------------------------------------------------------------------------
//CCodEngBase::Pausable
//------------------------------------------------------------------------
EXPORT_C TBool CCodEngBase::Pausable()
    {
    return (*iData)[iData->ActiveDownload()]->iPausable;	
    }

// ---------------------------------------------------------
// CCodEngBase::StoreMainInfoFileL()
// ---------------------------------------------------------  
//       
void CCodEngBase::StoreMainInfoFileL()	
    {
    if ( iCodDlInfoPath == KNullDesC )
        {
        return;
        }
    TInt bufSz = KDownloadInfoIncrSize;
    HBufC8* newInfo = HBufC8::NewLC( bufSz );
    TPtr8 newInfoPtr = newInfo->Des();
    
    // Append the DD attributes 
	iData->AppendStorageInfoL(newInfoPtr);

    RFile outFile;
    CleanupClosePushL<RFile>( outFile );
    
    User::LeaveIfError( outFile.Replace( iFs, 
                                         iCodDlInfoPath, 
                                         EFileShareExclusive | 
                                         EFileStream | 
                                         EFileWrite ) );

    outFile.Write( newInfoPtr );
    User::LeaveIfError( outFile.Flush() );
    
    CleanupStack::PopAndDestroy(2);

    }

// ---------------------------------------------------------
// CCodEngBase::StoreSubInfoFileL()
// ---------------------------------------------------------        
// 
void CCodEngBase::StoreSubInfoFileL( const CArrayPtrFlat<CHeaderField>* aHeaders, TInt aMediaIndex )	
    {
    if ( iCodDlInfoPath == KNullDesC )
        {
        return;
        }
    RFile outFile;
    CleanupClosePushL<RFile>( outFile );


    TInt bufSz = KDownloadInfoIncrSize;
    HBufC8* newInfo = HBufC8::NewLC( bufSz );
    TPtr8 newInfoPtr = newInfo->Des();
    
    TFileName infoPath;
    infoPath.Format( _L("%S_%d"), &iCodDlInfoPath, aMediaIndex );

    newInfoPtr.Zero();
    
    // Load the DD attributes and other required information to the buffer
	(*iData)[aMediaIndex]->StoreMediaInfoL( newInfoPtr, aHeaders );

    User::LeaveIfError( outFile.Replace( iFs, 
                                         infoPath, 
                                         EFileShareExclusive | 
                                         EFileStream | 
                                         EFileWrite ) );

    outFile.Write( newInfoPtr );
    User::LeaveIfError( outFile.Flush() );

    CleanupStack::PopAndDestroy(2);
    }

// ---------------------------------------------------------
// CCodEngBase::StoreInfoFilesL()
// ---------------------------------------------------------        
// 
void CCodEngBase::StoreInfoFilesL()	
    {
    StoreMainInfoFileL();
    
    for(TInt i = 1; i <= iData->Count(); ++i)
        {
        StoreSubInfoFileL( NULL, i );
        }
    }
// ---------------------------------------------------------
// CCodEngBase::LoadMainInfoFileL()
// ---------------------------------------------------------        
// 
void CCodEngBase::LoadMainInfoFileL()	
    {
    	
    RFile inFile;
    CleanupClosePushL<RFile>( inFile );
    TInt err = inFile.Open(iFs, 
                           CodDlInfoFile(), 
                           EFileShareReadersOnly | 
                           EFileRead ) ;	

    // Possible erros KErrNotFound , KErrPathNotFound , KErrBadName
	if( err  )
		{
		CleanupStack::PopAndDestroy();
		return;
		}
		
	iData->LoadStorageInfoL( inFile );
	CleanupStack::PopAndDestroy(); // inFile

    }

// ---------------------------------------------------------
// CCodEngBase::LoadSubInfoFileL()
// ---------------------------------------------------------            
// 
void CCodEngBase::LoadSubInfoFileL( TInt aMediaIndex, CArrayPtrFlat<CHeaderField>* aHeaders )	
    {
   	
    RFile inFile;
    CleanupClosePushL<RFile>( inFile );

    TFileName infoPath;
    infoPath.Format( _L("%S_%d"), &iCodDlInfoPath, aMediaIndex );

    
    TInt err = inFile.Open(iFs, 
                           infoPath, 
                           EFileShareReadersOnly | 
                           EFileRead ) ;	

    // Possible erros KErrNotFound , KErrPathNotFound , KErrBadName
	if( err  )
		{
		CleanupStack::PopAndDestroy();
		return;
		}
		
	(*iData)[aMediaIndex]->LoadMediaInfoL( inFile, iFs, aHeaders );
	CleanupStack::PopAndDestroy(); // inFile
    }

// ---------------------------------------------------------
// CCodEngBase::LoadInfoFilesL()
// ---------------------------------------------------------        
// 
void CCodEngBase::LoadInfoFilesL()	
    {
    LoadMainInfoFileL();
    
    TBool activeDownloadSet = EFalse;
    
    for(TInt i = 1; i <= iData->Count(); ++i)
        {
        LoadSubInfoFileL( i, NULL );
        if( ((*iData)[i]->State() == EInProgress  || (*iData)[i]->State() == EQueued ) && !activeDownloadSet )
            {
            iData->SetActiveDownload ( i );
            activeDownloadSet = ETrue;
            }            
        }
    if( iData->ActiveDownload() == 0 )
        {
        iData->SetActiveDownload(iData->Count());
        }
    }    
    
// ---------------------------------------------------------
// CCodEngBase::StartProductNotifyL
// ---------------------------------------------------------
//    
void CCodEngBase::StartProductNotifyL()
    {
    
    CLOG(( ECodEng, 2, _L("-> CCodEngBase::StartProductNotifyL iStatus(%d)"), \
        iStatus.Int() ));
    __ASSERT_DEBUG( iState == EStartProductNotify, CodPanic( ECodInternal ) );    
    if ( iData->InstallNotify().Length() )
        {
        CLOG(( ECodEng, 4, _L("CCodEngBase::StartProductNotifyL: init request") ));
        if ( !iProgress )
            {
            // Setup notify-only progress.
            iProgress = new (ELeave) TCodProgress( iObserver );
            iProgress->Setup( 0, KCodNotifyTraffic );    
            }
        iProgress->StartPhaseL( TCodProgress::ENotify );
        TBool notifyObserver = EFalse;
        // When notifying after fetch, we already have a loader (and the
        // observer has already been notified about load).
        // When notifying without fetch, we don't have a loader yet (and the
        // observer has not been notified about load yet).
        if ( !iConn )
            {
            iConn = CConnection::NewL();
            }
        // For notify, attach only (do no create new connection).
        iConn->AttachL( iPreferredIap );
        if( !iLoader )
            {
            iLoader = CHttpLoader::NewL( *iConn, NULL, iProgress ,this );
            notifyObserver = ETrue;
            }
        TInt statusCode;
        
        if(!iData->Count())
            {
            statusCode = KHttp906InvalidDescriptor; 
            }
        else
            {
            statusCode = KHttp981DownloadCompletion; 
            }
            
        for( TInt i = 1;i <= iData->Count(); ++i )
            {
            if( (*iData)[i]->iStatusCode != KHttp900Success )
                {
                statusCode = KHttp970MixedStatus;
                break;
                }
            }
        HBufC8* url = CodUtil::AbsoluteUrlLC
            ( iData->SourceUri(), iData->InstallNotify() );
        iLoader->NotifyL( *url, StatusText( statusCode ), &iStatus );
        CleanupStack::PopAndDestroy( url );
        iState = EProductNotify;
        SetActive();
        if ( iObserver && notifyObserver )
            {
            if ( iStatusCode == KHttp902UserCancelled ||
                 iStatusCode == KHttp921UserAborted )
                {
                CLOG(( ECodEng, 3, \
                    _L("-> CCodEngBase::StartNotifyL notify StartCancelL") ));
                iObserver->StartCancelL();
                iNotificationStatus = ECancel;
                }
            else
                {
                CLOG(( ECodEng, 3, \
                    _L("-> CCodEngBase::StartNotifyL notify StartLoadL(%d)"), \
                    iStatusCode ));
                iObserver->StartLoadL( iStatusCode );
                iNotificationStatus = ELoad;
                }
            }
        }
    else
        {
        // No install notify attribute, regarded as successful notification.
        CLOG(( ECodEng, 4, _L("CCodEngBase::StartNotifyL: nothing to do") ));
        Continue( EProductNotify );
        }
    CLOG(( ECodEng, 2, _L("<- CCodEngBase::StartNotifyL") ));    
    }
    
// ---------------------------------------------------------
// CCodEngBase::EndProductNotifyL
// ---------------------------------------------------------    
void CCodEngBase::EndProductNotifyL()
    {
        CLOG(( ECodEng, 2, _L("-> CCodEngBase::EndNotifyL iStatus(%d)"), \
        iStatus.Int() ));
    __ASSERT_DEBUG( iState == EProductNotify, CodPanic( ECodInternal ) );

    delete iLoader;
    iLoader = NULL;
    delete iConn;
    iConn = NULL;

    User::LeaveIfError( iStatus.Int() );    // Handle errors in RunError().

    // Do not leave after this point! Notify was successful, we must keep
    // downloaded content now.
    
    if ( iProgress )
        {
        TRAP_IGNORE( iProgress->DoneL() );
        delete iProgress;
        iProgress = NULL;
        }
    /*
    if( iSaver && (*iData)[iData->ActiveDownload()]->iResult == KErrNone )
        {
//TODO: check if in case of PD we should release. It is already done.
        iSaver->ReleaseContent( (*iData)[iData->ActiveDownload()]->iFileName, iHandler );
        iType = iSaver->DataType();
        }
    */
    Done();
    }

//------------------------------------------------------------------------
//CCodEngBase::UpdatedDownloadDataL
//------------------------------------------------------------------------
EXPORT_C HBufC8* CCodEngBase::UpdatedDownloadDataL()
    {
    if (!iData)
        return NULL;
    
    CDownloadDataClient* dlData = CDownloadDataClient::NewLC();
    ConvertCodDataToDownloadDataL( dlData );
    HBufC8* des8 = dlData->MarshalDataL();
    CleanupStack::PopAndDestroy(dlData);
    
    return des8;
    }
// ---------------------------------------------------------
// CCodEngBase::UpdateDownloadedSize
// ---------------------------------------------------------       
void CCodEngBase::UpdateDownloadedSize( TInt aSize )
    {
    (*iData)[iData->ActiveDownload()]->SetDownloadedSize( (*iData)[iData->ActiveDownload()]->DownloadedSize() + aSize );    
    }
// ---------------------------------------------------------
// CCodEngBase::ActiveDownload
// ---------------------------------------------------------          
EXPORT_C TInt CCodEngBase::ActiveDownload()
    {
    return iData->ActiveDownload();
    }

//------------------------------------------------------------------------
//CCodEngBase::UpdatedTrackDataL
//------------------------------------------------------------------------
EXPORT_C HBufC8* CCodEngBase::UpdatedTrackDataL(TInt& aValue)
    {
    if (!iData)
        return NULL;
    
    aValue = iData->ActiveDownload();
    
    CMediaDataClient* mediaData = CMediaDataClient::NewL();
	CleanupStack::PushL(mediaData);
	ConvertMOToMediaDataL(mediaData, aValue);
    HBufC8* des8 = mediaData->MarshalDataL();
    CleanupStack::PopAndDestroy(); //mediaData

    return des8;
    }
//------------------------------------------------------------------------
//CCodEngBase::ConvertCODDataToDownloadDataL
//------------------------------------------------------------------------    
void CCodEngBase::ConvertCodDataToDownloadDataL( CDownloadDataClient*& aDlData )
    {
    if (aDlData)
        {
        aDlData->SetNameL( iData->Name() );
        aDlData->SetSize( iData->Size() );
        aDlData->SetIconL( iData->Icon() );
        aDlData->SetUpdatedDDURI( iData->UpdatedDDUriL() );
        
        for (TInt mObj = 1; mObj <= iData->Count(); ++mObj)
            {
            CMediaDataClient* mediaData = CMediaDataClient::NewL();
            CleanupStack::PushL(mediaData);
            
            ConvertMOToMediaDataL(mediaData, mObj);
            aDlData->AppendMediaData(mediaData);
            
            CleanupStack::Pop(); //mediaData
            }
        }
    }

//------------------------------------------------------------------------
//CCodEngBase::ConvertMOToMediaDataL
//------------------------------------------------------------------------    
void CCodEngBase::ConvertMOToMediaDataL( CMediaDataClient*& aMOData, TInt aMOIndex )
    {
    CMediaObjectData* objMedia = (*iData)[aMOIndex];
    
    aMOData->SetNameL( objMedia->Name() );
    aMOData->SetUrlL( objMedia->Url() );
    aMOData->SetSize( objMedia->Size() );
    aMOData->SetIconL( objMedia->Icon() );
    aMOData->SetSourceUriL( objMedia->SourceUri() );
    aMOData->SetProgressiveDownload( objMedia->ProgressiveDownload() );
    aMOData->SetState( objMedia->State() );
    aMOData->SetResult( objMedia->iResult );
    aMOData->SetDownloadedSize( objMedia->DownloadedSize() );
    aMOData->SetPausable( objMedia->Pausable() );
    if(objMedia->iFullName)
    	{
    	aMOData->SetDestFilenameL( objMedia->FullName() );
    	aMOData->SetTempFilenameL( objMedia->FullName() );
    	}  
    
    for (TInt type = 0; type < objMedia->TypesCount(); ++type)
        aMOData->AddTypeL( objMedia->Types().MdcaPoint(type) );
    aMOData->SetStatusCode( objMedia->iStatusCode );
    }

//------------------------------------------------------------------------
//CCodEngBase::UpdateMediaInfoL
//------------------------------------------------------------------------    
void CCodEngBase::UpdateMediaInfoL()
    {
    if (iObserver)
        {
        iObserver->UpdateMediaInfoL();
        }
    }
// ---------------------------------------------------------
// CCodEngBase::GetProductStatusCode
// ---------------------------------------------------------       
EXPORT_C TCodDownloadProgress::TState CCodEngBase::GetProductStatusCode() const
    {
    TCodDownloadProgress::TState statusCode = TCodDownloadProgress::EFailedPermanent;        
    TInt allSucceeded = 0;
    TInt currentActiveDownload = iData->ActiveDownload();
    
    for( TInt i = 1 ; i <= iData->Count() ; ++i )
        {
        TInt mediaObjectStatusCode = (*iData)[i]->iStatusCode;
        
        // If the current track failed for some reason, set this as the
        // active download. Do this only for the first failed track.
        if ((mediaObjectStatusCode != KHttp900Success) &&
            (mediaObjectStatusCode != KHttp956LicenseSuccess))
            {
            if( currentActiveDownload > i )
                {
                //This is done so that next time when resume happens
                //it happens from first failed download
                iData->SetActiveDownload ( i );
                currentActiveDownload = i;
                
                //Inform server about the Active download
                iObserver->SetActiveDownload();
                iObserver->MediaObjectNameChanged();
                }
            }

        // Check for error and status code
        switch( mediaObjectStatusCode )
            {
            case KHttp900Success:
            case KHttp956LicenseSuccess:
                {
                allSucceeded++;
                statusCode = TCodDownloadProgress::ESucceeded;
                break;
                }

            case KHttp901InsufficientMemory:
                {
                statusCode = TCodDownloadProgress::EFailedTemporary;
                break;
                }
            case KHttp902UserCancelled:
            case KHttp921UserAborted:
            case KHttp957LicenseFailed:
                {
                break;
                }
            
            case KHttp905AttributeMismatch:
            case KHttp906InvalidDescriptor:
            case KHttp907InvalidType:
            case KHttp922DeviceAborted:
            case KHttp923NonAcceptableContent:
            case KHttp924LoaderError:
            case KHttp951InvalidDdVersion:
            case KHttp952DeviceAborted:
            case KHttp953NonAcceptableContent:
            case KHttp909RequestedRangeNotSatisfiable:
                {
                // Permanent errors: retry is useless.
                if( (*iData)[i]->iPausable ) 
                    {
                    //for pausable permanent errors
                    //Are actually Temporary errors
                    return TCodDownloadProgress::EFailedTemporary;
                    }
                break;
                }
            case KHttp954LoaderError:
            case KHttp903LossOfService:
            case KHttp910NoMemory:
            	{
            	if( (*iData)[i]->iPausable )
            		{
            		return TCodDownloadProgress::EPaused;
            		}
            	break;
            	}
            	

            case KHttp955PreconditionFailed:
                {
                return TCodDownloadProgress::EInit;
                }

            default:
                {
                // Unexpected status.
                //__ASSERT_DEBUG( EFalse, CodUiPanic( ECodUiInternal ) );
                statusCode = TCodDownloadProgress::EFailedPermanent;
                break;
                }

            }
        }
        
    if( iData->Count() && allSucceeded == iData->Count())
        {
        return TCodDownloadProgress::ESucceeded;
        }            
    if( statusCode == TCodDownloadProgress::EFailedPermanent && !allSucceeded)    
        {
        return statusCode;
        }

    return TCodDownloadProgress::EFailedTemporary;    
    }
    
//------------------------------------------------------------------------
//CCodEngBase::UpdateTrackInfoL
//------------------------------------------------------------------------    
void CCodEngBase::UpdateTrackInfoL()
    {
    if (iObserver)
        {
        iObserver->UpdateTrackInfoL();
        }
    }

//------------------------------------------------------------------------
//CCodEngBase::SetUrlL
//------------------------------------------------------------------------
void CCodEngBase::SetUrlL(const TDesC& aUrl)
	{
	TInt currentActiveDownload = iData->ActiveDownload();
	(*iData)[currentActiveDownload]->SetUrlL( aUrl );
	}
	
	
//------------------------------------------------------------------------
//CCodEngBase::ContentTypeChanged
//------------------------------------------------------------------------    
void CCodEngBase::ContentTypeChanged()
    {
    if (iObserver)
        {
        iObserver->ContentTypeChanged();
        }
    }	
