/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of class CCodDownloadImpl.   
*
*/


// INCLUDE FILES

#include <bldvariant.hrh>
#include <AiwGenericParam.h>
#include <DocumentHandler.h>
#include <apgcli.h>
#include <eikenv.h>
#include <StringLoader.h>
#include <ErrorUI.h>
#include <DcfRep.h>
#include <RoapDef.h>
#include <DRMCommon.h>
#include <aknmessagequerydialog.h>
#include <hlplch.h>             
#include <multipartparser.h>
#include <AknQueryDialog.h>
#include "CodDownloadImpl.h"
#include "CodUiPanic.h"
#include <CodStatus.h>
#include <CodDefs.h>
#include <CodError.h>
#include <CodEng.h>
#include <CodData.h>
#include <RoapData.h>
#include <DdEng.h>
#include <CodUtil.h>
#include "CodUiResource.h"
#include <CodUi.rsg>
#include "CodViewDialog.h"
#include "CodLogger.h"
#include "CodDialog.h"
#include "CodUi.hrh"
#include <pathinfo.h>
#include <Oma2Agent.h>
#include <bldvariant.hrh>
#include "CodConfirm.h" 
#include  <bautils.h>

#ifdef RD_MULTIPLE_DRIVE
#include <driveinfo.h>
#endif

// ================= CONSTANTS =======================

/// COD MIME type.
_LIT8( KCodDataType, COD_MIME_TYPE );
/// multipart/related type.
_LIT8( KMultipartRelatedDataType, "multipart/related" );
#ifdef __SYNCML_DM_FOTA
/// FOTA Update Package type.
_LIT8( KFotaPackageDataType, "application/vnd.nokia.swupd.dp2" );
#endif /*def __SYNCML_DM_FOTA */
/// UID of Wml Browser.
LOCAL_D const TInt KCodWmlBrowserUid = 0x10008D39;
/// Message prefix, to launch browser with URL.
_LIT( KCodMsgLaunchBrowserWithUrl, "4 " );
/// Reserved word "local".
_LIT8( KCodReservedWordLocal, "local" );
/// Reserved word "back".
_LIT8( KCodReservedWordBack, "back" );
/// For launching more info DRM_HLP_REGISTRATION
_LIT(KDRM_HLP_REGISTRATION,"DRM_HLP_REGISTRATION"); 
/// Next Line
_LIT( KNextLine, "\n" );

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CCodDownloadImpl::NewL()
// ---------------------------------------------------------
//
EXPORT_C CCodDownloadImpl* CCodDownloadImpl::NewL
        (
        CCodDownload& aProxy,
        const TDesC8& aBuf,
        const TDesC8& aType,
        MCodDownloadObserver& aObserver,
        CEikonEnv* aEikEnv,
        const TDesC8* aMultipartBoundary,
        const TDesC16* aMultipartUrl
        )
    {
    CLOG(( 2, _L8("-> CCodDownloadImpl::NewL aType<%S> aEikEnv(0x%x)"), \
        &aType, aEikEnv ));
    CCodDownloadImpl* download =
        new (ELeave) CCodDownloadImpl( aProxy, aObserver, aEikEnv );
    CleanupStack::PushL( download );
    download->ConstructL( aBuf, aType, aMultipartBoundary, aMultipartUrl );
    CleanupStack::Pop( download );
    CLOG(( 2, _L8("<- CCodDownloadImpl::NewL") ));
    return download;
    }
    
// ---------------------------------------------------------
// CCodDownloadImpl::NewL()
// ---------------------------------------------------------
//
EXPORT_C CCodDownloadImpl* CCodDownloadImpl::NewL
        (
        CCodDownload& aProxy,
        const TInt aDownloadId,
        const TUid aDlUid,
        const TDesC8& aType,
        MCodDownloadObserver& aObserver,
        CEikonEnv* aEikEnv,
        const TDesC8* aMultipartBoundary,
        const TDesC16* aMultipartUrl 
        )
    {
    CLOG(( 2, _L8("-> CCodDownloadImpl::NewL aType<%S> aEikEnv(0x%x)"), \
        &aType, aEikEnv ));
    CCodDownloadImpl* download =
        new (ELeave) CCodDownloadImpl( aProxy, aObserver, aEikEnv );
    CleanupStack::PushL( download );
    download->ConstructL( aDownloadId, aDlUid, aType, aMultipartBoundary, aMultipartUrl );
    CleanupStack::Pop( download );
    CLOG(( 2, _L8("<- CCodDownloadImpl::NewL") ));
    return download;
    }
// ---------------------------------------------------------
// CCodDownloadImpl::~CCodDownloadImpl()
// ---------------------------------------------------------
//
EXPORT_C CCodDownloadImpl::~CCodDownloadImpl()
    {
    CLOG(( 2, _L("*** -> CCodDownloadImpl::~CCodDownloadImpl") ));
    Cancel();
    delete iParams;
    iParts.ResetAndDestroy();
    delete iBuf;
    delete iEng;
    delete iUiResource;
    delete iErrorUi;
    delete iDocHandler;
    delete iDocParams;
    delete iPostResponseURL;
    iFs.Close();
    CLOG(( 2, _L("*** <- CCodDownloadImpl::~CCodDownloadImpl") ));
    }

// ---------------------------------------------------------
// CCodDownloadImpl::StartL()
// ---------------------------------------------------------
//
EXPORT_C void CCodDownloadImpl::StartL( const CAiwGenericParamList* aParams )
    {
    CLOG(( 2, _L("-> CCodDownloadImpl::StartL") ));
   // __ASSERT_DEBUG( iState == EInit , CodUiPanic( ECodUiRequestPending ) );
    __ASSERT_DEBUG( !IsActive(), CodUiPanic( ECodUiRequestPending ) );

    iAutoAccept = EFalse;
    iSilentMode = EFalse;
    iSuppressNextUrl = EFalse;
    iSuppressLaunch = EFalse;
    iFota = EFalse;
    iParams->Reset();

    if ( aParams )
        {
        // We must copy parameters, there is no guarantee that the caller
        // keeps the original during the whole download process.
        iParams->AppendL( *aParams );
        CodUtil::GetBoolParam
            ( iSilentMode, EGenericParamSilentDownload, *iParams );
        CodUtil::GetBoolParam( iAutoAccept,
            EGenericParamSuppressDownloadConfirmation, *iParams );
        CodUtil::GetBoolParam
            ( iSuppressNextUrl, EGenericParamSuppressNextUrl, *iParams );
        CodUtil::GetBoolParam
            ( iSuppressLaunch, EGenericParamSuppressLaunch, *iParams );
            
        CodUtil::GetUint32Param( iDownloadId, EGenericParamDownloadId, *iParams );
        
        TUint32 intAppUId(0);
        CodUtil::GetUint32Param( intAppUId, EGenericParamApplication, *iParams );
        iAppUId.iUid = intAppUId;

#ifdef __SYNCML_DM_FOTA
        TInt i( 0 );
        if ( iParams->FindFirst( i, EGenericParamFotaPkgId ) )
            {
            CLOG(( 2, _L("  EGenericParamFotaPkgId set"), &iProxy ));
            iFota = ETrue;
            }
#endif /*def __SYNCML_DM_FOTA */
        }

    

    if(iDownloadPaused)
    {
        iEng->Start(iParams,&iStatus);
        SetActive();
        iState = ELoad;
        iProgress.iState = TCodDownloadProgress::EInProgress;
        TRAP_IGNORE( iObserver.CodEventL( iProxy, MCodDownloadObserver::EStart ) );
        return;
    }
    
	iEng->SetL( iDescriptor, &iStatus, iParams, &iParts );
    SetActive();
    // Warning: no leave after this point!
    iState = ESet;
    ResetProgress();
    iProgress.iState = TCodDownloadProgress::EInProgress;
    CLOG(( 2, _L("CodEventL(0x%x,EStart)"), &iProxy ));
    TRAP_IGNORE( iObserver.CodEventL( iProxy, MCodDownloadObserver::EStart ) );
    CLOG(( 2, _L("<- CCodDownloadImpl::StartL") ));
    }

// ---------------------------------------------------------
// CCodDownloadImpl::Stop()
// ---------------------------------------------------------
//
EXPORT_C void CCodDownloadImpl::Stop()
    {
    CLOG(( 2, _L("-> CCodDownloadImpl::Stop") ));
    iEng->Stop();
    delete iDocHandler;
    iDocHandler = NULL;
    delete iDocParams;
    iDocParams = NULL;
    CLOG(( 2, _L("<- CCodDownloadImpl::Stop") ));
    }

// ---------------------------------------------------------
// CCodDownloadImpl::Progress()
// ---------------------------------------------------------
//
EXPORT_C const TCodDownloadProgress& CCodDownloadImpl::Progress() const
    {
    CLOG(( 2, _L("CCodDownloadImpl::Progress state(%d) error(%d) cur(%d) max(%d)"), \
        iProgress.iState, \
        iProgress.iError, \
        iProgress.iBytesCur, \
        iProgress.iBytesMax ));
    return iProgress;
    }

// ---------------------------------------------------------
// CCodDownloadImpl::Name()
// ---------------------------------------------------------
//
EXPORT_C TPtrC CCodDownloadImpl::Name()
    {
#ifdef __TEST_COD_LOG
    TPtrC ptr( iEng->Data().Name() );
    CLOG(( 2, _L("CCodDownloadImpl::Name returns<%S>"), &ptr ));
#endif /* def __TEST_COD_LOG */

    if(iEng->Data().Count() > 0)
        return (*iEng->Data()[iEng->Data().ActiveDownload()]).Name();
    else
        return KNullDesC();
    }

// ---------------------------------------------------------
// CCodDownloadImpl::Type()
// ---------------------------------------------------------
//
EXPORT_C TPtrC8 CCodDownloadImpl::Type() const
    {
#ifdef __TEST_COD_LOG
    if(iEng->Data().Count() > 0)
        {
        TPtrC8 ptr( (*iEng->Data()[iEng->Data().ActiveDownload()]).Type() );
        CLOG(( 2, _L8("CCodDownloadImpl::Type returns<%S>"), &ptr ));
        }
#endif /* def __TEST_COD_LOG */
    if(iEng->Data().Count() > 0)
        return (*iEng->Data()[iEng->Data().ActiveDownload()]).Type();
    else
        return KNullDesC8();
    }

// ---------------------------------------------------------
// CCodDownloadImpl::GetPath()
// ---------------------------------------------------------
//
EXPORT_C TPtrC CCodDownloadImpl::GetPath() const
    {
#ifdef __TEST_COD_LOG
    TPtrC ptr( iEng->GetPath() );
    CLOG(( 2, _L("CCodDownloadImpl::GetPath returns<%S>"), &ptr ));
#endif /* def __TEST_COD_LOG */
    return iEng->GetPath();
    }

// ---------------------------------------------------------
// CCodDownloadImpl::GetDestFilePath()
// ---------------------------------------------------------
//
EXPORT_C TPtrC CCodDownloadImpl::GetDestFilePath (TInt aMOIndex ) const
    {
    return iEng->GetDestFilePath(aMOIndex);
    }   

// ---------------------------------------------------------
// CCodDownloadImpl::ServiceFlowL()
// ---------------------------------------------------------
//
EXPORT_C void CCodDownloadImpl::ServiceFlowL()
    {
    CLOG(( 2, _L("-> CCodDownloadImpl::ServiceFlowL") ));
    if ( iState != ELoadDone )
        {
        // No completed download!
        User::Leave( KErrNotReady );
        }
    TBool done( ETrue );
    TRAPD( err, done = DoServiceFlowL() );
    if ( done || err )
        {
        Done();
        }
    CLOG(( 2, _L("<- CCodDownloadImpl::ServiceFlowL") ));
    }

// ---------------------------------------------------------
// CCodDownloadImpl::NextUrlL()
// ---------------------------------------------------------
//
EXPORT_C HBufC8* CCodDownloadImpl::NextUrlL() const
    {
    HBufC8* absUrl = NULL;
    TBool back( EFalse );
    // Empty string is returned while in progress.
    if ( iState == ELoadDone || iState == EServiceFlow || iState == EInit )
        {
        TPtrC8 url( NextUrlAttr( back ) );
        if ( url.Length() )
            {
            absUrl = CodUtil::AbsoluteUrlLC( iEng->Data().SourceUri(), url );
            CleanupStack::Pop( absUrl );
            }
        }
    if ( !absUrl )
        {
        absUrl = HBufC8::NewL( 0 );
        }
    return absUrl;
    }

// ---------------------------------------------------------
// CCodDownloadImpl::PostResponseUrlL()
// ---------------------------------------------------------
//
EXPORT_C HBufC8* CCodDownloadImpl::GetPostResponseUrlL() const
    {
    
    return iPostResponseURL;
    }


// ---------------------------------------------------------
// CCodDownloadImpl::UpdatedDDUriL()
// ---------------------------------------------------------
//
EXPORT_C HBufC8* CCodDownloadImpl::UpdatedDDUriL() const
    {
    HBufC8* absUrl = NULL;
    absUrl = CodUtil::AbsoluteUrlLC( iEng->Data().UpdatedDDUriL(), iEng->Data().UpdatedDDUriL() );
    CleanupStack::Pop( absUrl );
    
    if ( !absUrl )
        {
        absUrl = HBufC8::NewL( 0 );
        }      
    return absUrl;
    
    }

// ---------------------------------------------------------
// CCodDownloadImpl::UpdatedDownlaodDataL()
// ---------------------------------------------------------
//
EXPORT_C HBufC8* CCodDownloadImpl::UpdatedDownloadDataL() const
    {
    return (iEng->UpdatedDownloadDataL());
    }

// ---------------------------------------------------------
// CCodDownloadImpl::UpdatedTrackDataL()
// ---------------------------------------------------------
//
EXPORT_C HBufC8* CCodDownloadImpl::UpdatedTrackDataL(TInt& aValue) const
    {
    return (iEng->UpdatedTrackDataL(aValue));
    }

// ---------------------------------------------------------
// CCodDownloadImpl::RemovableMedia()
// ---------------------------------------------------------
//
EXPORT_C TBool CCodDownloadImpl::RemovableMedia() const
    {
    TBool removable = iEng->RemovableMedia();
    CLOG(( 2, _L("CCodDownloadImpl::RemovableMedia (%d)"), removable ));
    return removable;
    }

// ---------------------------------------------------------
// CCodDownloadImpl::UserData()
// ---------------------------------------------------------
//
EXPORT_C TAny* CCodDownloadImpl::UserData() const
    {
    CLOG(( 2, _L("CCodDownloadImpl::UserData returns 0x%x"), iUserData ));
    return iUserData;
    }

// ---------------------------------------------------------
// CCodDownloadImpl::SetUserData()
// ---------------------------------------------------------
//
EXPORT_C void CCodDownloadImpl::SetUserData( TAny* aUserData )
    {
    CLOG(( 2, _L("CCodDownloadImpl::SetUserData (0x%x)"), aUserData ));
    iUserData = aUserData;
    }

// ---------------------------------------------------------
// CCodDownloadImpl::SetCodDlAttached()
// ---------------------------------------------------------
//
EXPORT_C void CCodDownloadImpl::SetCodDlAttached(const TBool aValue)
    {
    CLOG(( 2, _L("CCodDownloadImpl::SetCodDlAttached") ));
    iEng->SetCodDlAttached(aValue);
	iAttached = aValue;
    }

// ---------------------------------------------------------
// CCodDownloadImpl::ConstructL()
// ---------------------------------------------------------
//
void CCodDownloadImpl::ConstructL
        (
        const TDesC8& aBuf,
        const TDesC8& aType,
        const TDesC8* aMultipartBoundary,
        const TDesC16* aMultipartUrl
        )
    {
    iBuf = aBuf.AllocL();

    // By default, use the whole buffer and passed type.
    iDescriptor.Set( *iBuf );
    TDataType type( aType );

    // For multipart content, use the first part.
    // If there is no such (or cannot get it), fail on MIME check below.
    if ( type == TDataType( KMultipartRelatedDataType ) &&
         aMultipartBoundary && aMultipartUrl )
        {
        MultipartParser::ParseL
            ( *iBuf, aType, *aMultipartBoundary, *aMultipartUrl, iParts );
        if ( iParts.Count() )
            {
            // Use first part of multipart/related.
            iDescriptor.Set( iParts[0]->Body() );
            type = iParts[0]->ContentType();
            }
        }

    // Now iDescriptor and type are either the whole buffer and its type,
    // or those of the first part of multipart.

    if ( type == TDataType( KCodDataType ) )
        {
        iCod = ETrue;
        iEng = CCodEng::NewL( NULL, this );
        }
    else if ( type == TDataType( KDdDataType ) )
        {
        iCod = EFalse;
        iEng = CDdEng::NewL( NULL, this );
        iEng->SetDd2Type(EFalse);
        }
    else if ( type == TDataType( KDd2DataType ) )
        {
        iCod = EFalse;
        iEng = CDdEng::NewL( NULL, this );
        iEng->SetDd2Type(ETrue);
        }
    else
        {
        if( type == TDataType(KNullDesC8))
        	{
        	iEng = CDdEng::NewL( NULL, this );
        	}
        else
        	{
        	User::Leave( KErrNotSupported );
        	}
        }
    // HACK: Priority set lower than DownloadManagerUiLib's download list.
    // In-download queries (currently, ROAP device registration query)
    // need to come on top of Download Manager's download list.
    iEng->SetPriority( CActive::EPriorityStandard - 1 );
    if ( iEikEnv )
        {
        iUiResource = CCodUiResource::NewL( *iEikEnv );
        iErrorUi = CErrorUI::NewL( *iEikEnv );
        }

    iParams = CAiwGenericParamList::NewL();
    }

// ---------------------------------------------------------
// CCodDownloadImpl::ConstructL()
// ---------------------------------------------------------
//
void CCodDownloadImpl::ConstructL
        (
        const TInt aDownloadId,
        const TUid aDlUid,
        const TDesC8& aType,
        const TDesC8* aMultipartBoundary,
        const TDesC16* aMultipartUrl       
        )
    {
    //iBuf = aBuf.AllocL(); NO need for Descriptor buffer in case of paused download
    iParams = CAiwGenericParamList::NewL();
    iAutoAccept = EFalse;
    iSilentMode = EFalse;
    iSuppressNextUrl = EFalse;
    iSuppressLaunch = EFalse;
    iFota = EFalse;
    iParams->Reset();
    iDownloadPaused = ETrue;        
	iState = EInit;

    // By default, use the whole buffer and passed type.
    
    TDataType type( aType );

    // For multipart content, use the first part.
    // If there is no such (or cannot get it), fail on MIME check below.
    if ( type == TDataType( KMultipartRelatedDataType ) &&
         aMultipartBoundary && aMultipartUrl )
        {
        MultipartParser::ParseL
            ( *iBuf, aType, *aMultipartBoundary, *aMultipartUrl, iParts );
        if ( iParts.Count() )
            {
            // Use first part of multipart/related.
            iDescriptor.Set( iParts[0]->Body() );
            type = iParts[0]->ContentType();
            }
        }

    // Now iDescriptor and type are either the whole buffer and its type,
    // or those of the first part of multipart.

    if ( type == TDataType( KCodDataType ) )
        {
        iCod = ETrue;
        iEng = CCodEng::NewL( NULL, this );
        }
    else if ( type == TDataType( KDdDataType ) )
        {
        iCod = EFalse;
        iEng = CDdEng::NewL( NULL, this );
        iEng->SetDd2Type(EFalse);
        }
    else if ( type == TDataType( KDd2DataType ) )
        {
        iCod = EFalse;
        iEng = CDdEng::NewL( NULL, this );
        iEng->SetDd2Type(ETrue);
        }
    else
        {
        if( type == TDataType(KNullDesC8))
        	{
        	iEng = CDdEng::NewL( NULL, this );
        	}
        else
        	{
        	User::Leave( KErrNotSupported );
        	}
        }
    // HACK: Priority set lower than DownloadManagerUiLib's download list.
    // In-download queries (currently, ROAP device registration query)
    // need to come on top of Download Manager's download list.
    iEng->SetPriority( CActive::EPriorityStandard - 1 );
    if ( iEikEnv )
        {
        iUiResource = CCodUiResource::NewL( *iEikEnv );
        iErrorUi = CErrorUI::NewL( *iEikEnv );
        }
	iEng->SetL( aDownloadId, aDlUid, &iParts );
	
    // Warning: no leave after this point!
    iState = EInit;
    ResetProgress();
	iProgress.iState = TCodDownloadProgress::EPaused;
    //TRAP_IGNORE( iObserver.CodEventL( iProxy, MCodDownloadObserver::EStart ) );	
    
    }
// ---------------------------------------------------------
// CCodDownloadImpl::DoCancel()
// ---------------------------------------------------------
//
void CCodDownloadImpl::DoCancel()
    {
    CLOG(( 2, _L("-> CCodDownloadImpl::DoCancel") ));
    iEng->Cancel();
    iProgress.iError = KErrCancel;
    iProgress.iState = TCodDownloadProgress::EFailedPermanent;
    Done();
    CLOG(( 2, _L("<- CCodDownloadImpl::DoCancel") ));
    }

// ---------------------------------------------------------
// CCodDownloadImpl::Pause()
// ---------------------------------------------------------
//
EXPORT_C void CCodDownloadImpl::Pause()
    {

    CLOG(( 2, _L("-> CCodDownloadImpl::Pause") ));
    iEng->Pause();
    iProgress.iState = TCodDownloadProgress::EPaused;
    //Done();
    CLOG(( 2, _L("<- CCodDownloadImpl::Pause") ));
    }

// ---------------------------------------------------------
// CCodDownloadImpl::RunL()
// ---------------------------------------------------------
//
void CCodDownloadImpl::RunL()
    {
    CLOG(( 2, _L("-> CCodDownloadImpl::RunL iStatus(%d)"), iStatus.Int() ));
    iProgress.iError = iStatus.Int();
    switch ( iState )
        {
        case ESet:
            {
            if ( iProgress.iError )
                {
                LoadDoneL();
                }
            else
                {
                SetOkL();
                }
            break;
            }

        case ELoad:
            {
            LoadDoneL();
            break;
            }
        case EInit:
        case ELoadDone:
        default:
            {
            CodUiPanic( ECodUiInternal );
            break;
            }
        }
    CLOG(( 2, _L("<- CCodDownloadImpl::RunL") ));
    }

// ---------------------------------------------------------
// CCodDownloadImpl::RunError()
// ---------------------------------------------------------
//
TInt CCodDownloadImpl::RunError( TInt aError )
    {
    CLOG(( 2, _L("-> CCodDownloadImpl::RunError aError(%d)"), aError ));
    iProgress.iError = aError;
    iProgress.iState = TCodDownloadProgress::EFailedPermanent;
    Done();
    CLOG(( 2, _L("<- CCodDownloadImpl::RunError") ));
    return KErrNone;
    }

// ---------------------------------------------------------
// CCodDownloadImpl::SetOkL()
// ---------------------------------------------------------
//
void CCodDownloadImpl::SetOkL()
    {
    CLOG(( 2, _L("-> CCodDownloadImpl::SetOkL") ));
    // Data parsed.
    CLOG(( 2, _L("CodEventL(0x%x,ENameChanged)"), &iProxy ));
    iObserver.CodEventL( iProxy, MCodDownloadObserver::ENameChanged );
    CLOG(( 2, _L("CodEventL(0x%x,ETypeChanged)"), &iProxy ));
    iObserver.CodEventL( iProxy, MCodDownloadObserver::ETypeChanged );
    TInt cmd( 0 );
    TDataType type( Type() );
    // if centra respository vale set iAutoAccept true, or the descritor do so
    // we won't ask for user confirmation 
    if ( iAutoAccept || iEng->Data().SuppressConfirm() )  
        {
        cmd = ECodUiCmdAccept;
        }

#ifdef __DRM_OMA2
    if ( type == TDataType( KOma2TriggerContentType ) )
        {
        // ROAP Trigger is accepted automatically (no user interaction).
        cmd = ECodUiCmdAccept;
        }
#endif /*def __DRM_OMA2*/
#ifdef __SYNCML_DM_FOTA
    if ( type == TDataType( KFotaPackageDataType ) )
        {
        // Safety check, this should be set already (via GenericParam).
        iFota = ETrue;
        }
    if ( iFota && !iEikEnv )
        {
        // FOTA package is auto-accepted, if no UI present.
        cmd = ECodUiCmdAccept;
        }
#endif /*def __SYNCML_DM_FOTA */
        
    if ( !cmd && iEikEnv )
        {
            CCodConfirm* confirm = new (ELeave) CCodConfirm;
            if (confirm->ShowDownloadConfirmationL(R_COD_UI_CBA_ACCEPT_CANCEL, 
                                               iEng->Data(), 
                                               iCod
                                               ) 
            )
            {
        	cmd = ECodUiCmdAccept;
            }
            else
            {
        	cmd = ECodUiCmdReject; 
            }
            delete confirm; 
        }
    CLOG(( 2, _L("  CCodDownloadImpl::SetOkL cmd(%d)"), cmd ));
    if ( cmd == ECodUiCmdAccept )
        {
        iEng->Accept( &iStatus );
        SetActive();
        iState = ELoad;
        // Do not leave after this point!
        }
    else if ( cmd == ECodUiCmdReject )
        {
        iEng->Reject( &iStatus );
        SetActive();
        iState = ELoad;
        // Do not leave after this point!
        }
    else
        {
        // No command. There is no UI to ask user, or dialog was dismissed
        // in some other way.
        iProgress.iError = KErrCancel;
        iProgress.iState = TCodDownloadProgress::EFailedPermanent;
        Done();
        }
    CLOG(( 2, _L("<- CCodDownloadImpl::SetOkL") ));
    }
    
// ---------------------------------------------------------
// CCodDownloadImpl::LoadDoneL()
// ---------------------------------------------------------
//
void CCodDownloadImpl::LoadDoneL()
    {
    CLOG(( 2, _L("-> CCodDownloadImpl::LoadDoneL status(%d)"), \
        iEng->GetStatusCode() ));

    iProgress.iState = iEng->GetProductStatusCode();
    if( iEng->Data().Count() > 1  )
        {
        
        if( iProgress.iState == TCodDownloadProgress::EFailedPermanent )
            {
            iProgress.iError = KErrMultipeObjectDownloadFailed ;
            }
        else if( iProgress.iState == TCodDownloadProgress::EFailedTemporary )
            {
            iProgress.iState = TCodDownloadProgress::EPaused;
            }
        }
        
    if( iProgress.iState  == TCodDownloadProgress::EInit )
        {
        iState = EInit;
        TRAP_IGNORE( iObserver.CodEventL( iProxy, MCodDownloadObserver::EUpdatedDDURI ) ); 
        return;
        }
                
    if (  iEng->Data().ActiveDownload() && iEng->Data().Count() && iEng->GetPath().Length() )
        {
        // ROAP involved loading a file, treat as download and not as ROAP.
        iRoap = EFalse;
        }
        
    // State change must come before the EEndLoad / EDone callback.
    iState = ELoadDone;
    if ( iEikEnv && !iFota )
        {
        CLOG(( 2, _L("CodEventL(0x%x,EEndLoad)"), &iProxy ));
        if( iProgress.iState == TCodDownloadProgress::EFailedPermanent ||
			iProgress.iState == TCodDownloadProgress::EPaused 
         )
         
        	{
        	// Done with permanent failure or paused
        	iDownloadPaused = ETrue;
        	iObserver.CodEventL( iProxy, MCodDownloadObserver::EDone );
        	}
        else
        	{
        	iObserver.CodEventL( iProxy, MCodDownloadObserver::EEndLoad );
        	}
        }
    else
        {
        // No service flow without UI -> we are done.
        // FOTA does not need any UI activity (except Details View) -> done.
        Done();
        }
    CLOG(( 2, _L("<- CCodDownloadImpl::LoadDoneL") ));
    }

// ---------------------------------------------------------
// CCodDownloadImpl::DoServiceFlowL()
// ---------------------------------------------------------
//
TBool CCodDownloadImpl::DoServiceFlowL()
    {
    CLOG(( 2, _L("-> CCodDownloadImpl::DoServiceFlowL") ));
    __ASSERT_DEBUG( iState == ELoadDone, CodUiPanic( ECodUiInternal ) );
    TBool done( ETrue );
    if ( iEikEnv && !iFota )    // Safety code.
        {
        // No service flow for Cancel or Reject.
        if ( iProgress.iError != KErrCancel && iProgress.iError != KErrAbort )
            {
            if ( iRoap )
                {
                if ( iProgress.iError )
                    {
                    done = RoapFailedL();
                    }
                else
                    {
                    done = RoapOkL();
                    }
                }
            else
                {
                if ( iProgress.iError )
                    {
                    done = LoadFailedL();
                    }
                else
                    {
                    done = LoadOkL();
                    }
                }
            if ( done )
                {
                ActivateNextUrlL();
                }
            }
        }
    CLOG(( 2, _L("<- CCodDownloadImpl::DoServiceFlowL (%d)"), done ));
    return done;
    }

// ----------------------------------------------------------
// CCodDownloadImpl::LoadOkL()
// ----------------------------------------------------------
//
TBool CCodDownloadImpl::LoadOkL()
    {
    CLOG(( 2, _L("-> CCodDownloadImpl::LoadOkL") ));
    __ASSERT_DEBUG( iEikEnv, CodUiPanic( ECodUiInternal ) );
    __ASSERT_DEBUG( !iFota, CodUiPanic( ECodUiInternal ) );
    __ASSERT_DEBUG( !iRoap, CodUiPanic( ECodUiInternal ) );
    __ASSERT_DEBUG( !iProgress.iError, CodUiPanic( ECodUiInternal ) );
    TBool done( ETrue );    // Done by default.
    // Content is not launched if:
    // - in silent mode
    // - suppress launch
    // - there is no content to launch
    // - COD is not 'local'
    // - DRM protected object and no rights present.
    CLOG(( 2, _L("<- CCodDownloadImpl::LoadOkL (%d)"), done ));    
    return done;
    }

// ---------------------------------------------------------
// CCodDownloadImpl::LaunchHelpL()
// ---------------------------------------------------------
//
void CCodDownloadImpl::LaunchHelpL( const TDesC& aContext )
    {
    CArrayFix<TCoeHelpContext>* contexts = 
        new(ELeave) CArrayFixFlat<TCoeHelpContext>( 1 );
    CleanupStack::PushL( contexts );
    TCoeHelpContext context;
    context.iMajor = KDRMHelperUid;
    context.iContext = aContext;
    contexts->AppendL( context );
    CleanupStack::Pop(); // contexts
    
    #ifdef __SERIES60_HELP
    HlpLauncher::LaunchHelpApplicationL( iEikEnv->WsSession(), contexts );
    #endif // __SERIES60_HELP
    }


// ----------------------------------------------------------
// CCodDownloadImpl::ShowRoapCallback(TAny* aPtr)
// ----------------------------------------------------------
//

TInt CCodDownloadImpl::ShowRoapCallback( TAny* aPtr )
{
    TInt err( KErrNone );    
    CCodDownloadImpl* self = static_cast<CCodDownloadImpl*>(aPtr);
    TRAP( err, self->LaunchHelpL( KDRM_HLP_REGISTRATION ) );     
    return err;        
}

// ----------------------------------------------------------
// CCodDownloadImpl::LoadFailedL()
// ----------------------------------------------------------
//
TBool CCodDownloadImpl::LoadFailedL()
    {
    CLOG(( 2, _L("-> CCodDownloadImpl::LoadFailedL") ));
    __ASSERT_DEBUG( iEikEnv, CodUiPanic( ECodUiInternal ) );
    __ASSERT_DEBUG( !iFota, CodUiPanic( ECodUiInternal ) );
    __ASSERT_DEBUG( !iRoap, CodUiPanic( ECodUiInternal ) );
    __ASSERT_DEBUG( iProgress.iError, CodUiPanic( ECodUiInternal ) );
    TInt resId = 0;
    TInt err = iProgress.iError;
    if ( err == KErrCodNoAccessPoint )
        {
        // "No access point" error has the same status code as all connection
        // related errors, but needs different note.
        resId = R_QTN_CD_INFO_NO_VALID_AP;
        }
    else
        {
        // Investigating status code (which carries less information than the
        // error code) is generally enough to get the correct note.
        switch( iEng->GetStatusCode() )
            {
            case KHttp900Success:
            case KHttp902UserCancelled:
            case KHttp921UserAborted:
                {
                // No error note shown for user cancel.
                err = KErrNone;
                break;
                }

            case KHttp901InsufficientMemory:
            case KHttp910NoMemory:
                {
                resId = R_QTN_CD_INFO_NOT_ENOUGH_MEM;
                break;
                }

            case KHttp903LossOfService:
                {
                // User Text Resolver to get "Connection timeout".
                err = KErrCodHttpNoResponse;
                break;
                }

            case KHttp906InvalidDescriptor:
                {
                resId = R_QTN_CD_INFO_SYNTAX_ERR;
                break;
                }

            case KHttp907InvalidType:
                {
                resId = R_QTN_CD_INFO_UNSUPP_MIME_TYPE;
                break;
                }

            case KHttp922DeviceAborted:
            case KHttp952DeviceAborted:
                {
                resId = R_QTN_CD_INFO_UNKNOWN_TYPE;
                break;
                }

            case KHttp905AttributeMismatch:
            case KHttp923NonAcceptableContent:
            case KHttp953NonAcceptableContent:
                {
                resId = R_QTN_CD_INFO_UNSUPP_MIME_TYPE;
                break;
                }

            case KHttp924LoaderError:
            case KHttp954LoaderError:
            case KHttp951InvalidDdVersion:
                {
                resId = R_QTN_CD_INFO_GENERIC_ERROR;
                break;
                }

            default:
                {
                __ASSERT_DEBUG( EFalse, CodUiPanic( ECodUiUnknownStatus ) );
                break;
                }
            }
        }

    if ( resId )
        {
        CodDialog::InfoNoteL( resId, *iEikEnv );
        }
    else if ( err )
        {
        TPtrC msg( iErrorUi->TextResolver().ResolveErrorString( err ) );
        CodDialog::InfoNoteL( msg );
        }
    CLOG(( 2, _L("<- CCodDownloadImpl::LoadFailedL") ));
    return ETrue;
    }

// ----------------------------------------------------------
// CCodDownloadImpl::RoapOkL()
// ----------------------------------------------------------
//
TBool CCodDownloadImpl::RoapOkL()
    {
    CLOG(( 2, _L("-> CCodDownloadImpl::RoapOkL") ));
    __ASSERT_DEBUG( iEikEnv, CodUiPanic( ECodUiInternal ) );
    __ASSERT_DEBUG( !iFota, CodUiPanic( ECodUiInternal ) );
    __ASSERT_DEBUG( iRoap, CodUiPanic( ECodUiInternal ) );
    __ASSERT_DEBUG( !iProgress.iError, CodUiPanic( ECodUiInternal ) );
    TBool done( ETrue );
    const CRoapData* roapData = iEng->GetRoapData();
    if ( roapData ) // Safety code.
        {
        switch( roapData->iType )
            {
            case Roap::ERegistrationTrigger:
                {
                CodDialog::ConfNoteL
                    ( R_QTN_DRM_CONF_DEVICE_REGISTERED, *iEikEnv );
                break;
                }
                
            case Roap::ERoAcquisitionTrigger:
                {
                done = RoAcqOkL();
                break;
                }
                
            case Roap::EJoinDomainTrigger:
                {
                if ( roapData->iDomainOperation == Roap::EJoinDomainOperation )
                    {
                    CodDialog::ConfNoteL
                        ( R_QTN_DRM_CONF_JOINED_TO_DOMAIN, *iEikEnv );
                    }
                else
                    {
                    CodDialog::ConfNoteL
                        ( R_QTN_DRM_CONF_ACCOUNT_UPDATED, *iEikEnv );
                    }
                break;
                }
                
            case Roap::ELeaveDomainTrigger:
                {
                CodDialog::ConfNoteL
                    ( R_QTN_DRM_INFO_REMOVE_FR_ACCOUNT, *iEikEnv );
                break;
                }
                
            case Roap::EMeteringReportTrigger:
                {
                break;
                }                
                
            default:
                {
                // We should never get here.
                __ASSERT_DEBUG( EFalse, CodUiPanic( ECodUiInternal ) );
                }
            }
        }
    CLOG(( 2, _L("<- CCodDownloadImpl::RoapOkL (%d)"), done ));
    return done;
    }

// ----------------------------------------------------------
// CCodDownloadImpl::RoAcqOkL()
// ----------------------------------------------------------
//
TBool CCodDownloadImpl::RoAcqOkL()
    {
    CLOG(( 2, _L("-> CCodDownloadImpl::RoAcqOkL") ));
    __ASSERT_DEBUG( iEikEnv, CodUiPanic( ECodUiInternal ) );
    __ASSERT_DEBUG( !iFota, CodUiPanic( ECodUiInternal ) );
    const CRoapData* roapData = iEng->GetRoapData();
    __ASSERT_DEBUG ( roapData, CodUiPanic( ECodUiInternal ) );
    __ASSERT_DEBUG ( roapData->iType == \
        Roap::ERoAcquisitionTrigger, CodUiPanic( ECodUiInternal ) );

    TFileName name;    // Note, filename and extension only! Not full path.
    TDataType type;
    TUid appUid( TUid::Null() );
    RFile f;
    CleanupClosePushL<RFile>( f );
    if ( !iSilentMode && !iSuppressLaunch && roapData->iContentIdList.Count() )
        {
        __ASSERT_DEBUG( !iFs.Handle(), CodUiPanic( ECodUiInternal ) );
        User::LeaveIfError( iFs.Connect() );
        User::LeaveIfError( iFs.ShareProtected() );
        CDcfRep* dcfRep = CDcfRep::NewL();
        CleanupStack::PushL( dcfRep );
        // It is not specified what to do if we have more content ids.
        // Best guess: open the first.
        TPtrC8 cid( *(roapData->iContentIdList[0]) );
        TRAPD( err, dcfRep->GetFileHandleL ( cid, f, iFs ) );
        if ( !err && CodUtil::CanOpenL( f ) )
            {
            User::LeaveIfError( f.Name( name ) );
            RApaLsSession apaLs;
            User::LeaveIfError( apaLs.Connect() );
            CleanupClosePushL<RApaLsSession>( apaLs );
            User::LeaveIfError( apaLs.AppForDocument( f, appUid, type ) );
            CleanupStack::PopAndDestroy();  // apaLs
            }
#ifdef __TEST_COD_LOG
        else
            {
            CLOG(( 4, _L8("  err(%d) getting file by cid<%S>"), err, &cid ));
            }
#endif /* def __TEST_COD_LOG */
        CleanupStack::PopAndDestroy( dcfRep );
        }

    TBool done( ETrue );
    if ( appUid.iUid )
        {
        // MO (related to the received RO) is already here and is supported
        // by the system. Ask user if he wants to open it now.
        // "Activation key for 'Foo.jpg' received. Open the file now?"
        __ASSERT_DEBUG( iFs.Handle(), CodUiPanic( ECodUiInternal ) );
        __ASSERT_DEBUG( f.SubSessionHandle(), CodUiPanic( ECodUiInternal ) );
        __ASSERT_DEBUG( name.Length(), CodUiPanic( ECodUiInternal ) );
        CAknQueryDialog* dialog = CAknQueryDialog::NewL();
        dialog->PrepareLC( R_COD_UI_CONFIRM_NOTE );
        HBufC* prompt = StringLoader::LoadLC
            ( R_QTN_DRM_QUERY_KEY_RECEIVED_OPEN, name, iEikEnv );
        dialog->SetPromptL( *prompt );
        CleanupStack::PopAndDestroy( prompt );
        if ( dialog->RunLD() ) 
            {
            CLOG(( 4, _L("  launching <%S>, handler(0x%x)"), \
                &name, appUid.iUid ));
            __ASSERT_DEBUG( !iDocHandler, CodUiPanic( ECodUiInternal ) );
            __ASSERT_DEBUG( !iDocParams, CodUiPanic( ECodUiInternal ) );
            iDocHandler = CDocumentHandler::NewL();
            iDocHandler->SetExitObserver( this );
            iDocParams = CAiwGenericParamList::NewL();
            User::LeaveIfError
                ( iDocHandler->OpenFileEmbeddedL( f, type, *iDocParams ) );
            // Warning: no leave after this point!
            iState = EServiceFlow;
            done = EFalse;
            }
        }
    else
        {
        // "Activation key received"
        CodDialog::ConfNoteL( R_QTN_DRM_INFO_ACT_KEY_RECEIVED, *iEikEnv );
        }
    CleanupStack::PopAndDestroy( &f );
    CLOG(( 2, _L("<- CCodDownloadImpl::RoAcqOkL (%d)"), done ));
    return done;
    }

// ----------------------------------------------------------
// CCodDownloadImpl::RoapFailedL()
// ----------------------------------------------------------
//
TBool CCodDownloadImpl::RoapFailedL()
    {
    CLOG(( 2, _L("-> CCodDownloadImpl::RoapFailedL iProgress.iError(%d)"), \
        iProgress.iError ));
    __ASSERT_DEBUG( iEikEnv, CodUiPanic( ECodUiInternal ) );
    __ASSERT_DEBUG( !iFota, CodUiPanic( ECodUiInternal ) );
    __ASSERT_DEBUG( iRoap, CodUiPanic( ECodUiInternal ) );
    __ASSERT_DEBUG( iProgress.iError, CodUiPanic( ECodUiInternal ) );
    TBool done( ETrue );
    TInt resId = 0;
    const CRoapData* roapData = iEng->GetRoapData();
    if ( roapData ) // Safety code
        {
        // Some errors need specific message.
        switch ( iProgress.iError )
            {
            case KErrRoapDomainFull:
                {
                resId = R_QTN_DRM_INFO_ACCOUNT_IS_FULL;
                break;
                }
            case KErrRoapInvalidDomain:
                {
                resId = R_QTN_DRM_ACCOUNT_NOT_RECOGNISED;
                break;
                }
            case KErrRoapServer:
                {
                // Temporary server error. For RoAcq, offer retry; for other
                // ROAP-s, fall through to default handling (plain error note).
                if ( roapData->iType == Roap::ERoAcquisitionTrigger )
                    {
                    CAknQueryDialog* dialog = CAknQueryDialog::NewL();
                    dialog->PrepareLC( R_COD_UI_CONFIRM_NOTE );
                    HBufC* prompt = StringLoader::LoadLC
                        ( R_QTN_DRM_GET_KEY_FAILED_RETRY, iEikEnv );
                    dialog->SetPromptL( *prompt );
                    CleanupStack::PopAndDestroy( prompt );
                    if ( dialog->RunLD() ) 
                        {
                        ResetProgress();
                        iProgress.iState = TCodDownloadProgress::EInProgress;
                        iEng->Accept( &iStatus );
                        SetActive();
                        iState = ELoad;
                        // Do not leave after this point!
                        done = EFalse;
                        }
                    break;
                    }
                // Not RoAcq: fall through to default (error note).
                }
            default:
                {
                // Default error message is based on trigger type.
                switch ( roapData->iType )
                    {
                    case Roap::ERegistrationTrigger:
                        {
                        resId = R_QTN_DRM_INFO_DEVICE_REGIST_FAIL;
                        break;
                        }
                    case Roap::ERoAcquisitionTrigger:
                        {
                        resId = R_QTN_DRM_INFO_GETTING_KEY_FAILED;
                        break;
                        }
                    case Roap::EJoinDomainTrigger:
                        {
                        if ( roapData->iDomainOperation ==
                             Roap::EJoinDomainOperation )
                            {
                            resId = R_QTN_DRM_INFO_REGISTRATION_FAILED;
                            }
                        else
                            {
                            resId = R_QTN_DRM_INFO_UPDATE_DOMAIN_FAIL;
                            }
                        break;
                        }
                    case Roap::ELeaveDomainTrigger:
                        {
                        resId = R_QTN_DRM_REMOVE_FR_ACCOUNT_FAIL;
                        break;
                        }
                    case Roap::EMeteringReportTrigger:
                        {
                        break;
                        }
                    default:
                        {
                        __ASSERT_DEBUG( EFalse, \
                            CodUiPanic( ECodUiInternal ) );
                        }
                    }
                break;
                }
            }
        if( resId )
            {
            CodDialog::InfoNoteL( resId, *iEikEnv );
            }
        }
    CLOG(( 2, _L("<- CCodDownloadImpl::RoapFailedL (%d)"), done ));
    return done;
    }

// ----------------------------------------------------------
// CCodDownloadImpl::ActivateNextUrlL()
// ----------------------------------------------------------
//
void CCodDownloadImpl::ActivateNextUrlL()
    {
    CLOG(( 2, _L("-> CCodDownloadImpl::ActivateNextUrlL") ));
    __ASSERT_DEBUG( iEikEnv, CodUiPanic( ECodUiInternal ) );
    __ASSERT_DEBUG( !iFota, CodUiPanic( ECodUiInternal ) );

    if ( iSilentMode || iSuppressNextUrl )
        {
        CLOG(( 2, _L("<- CCodDownloadImpl::ActivateNextUrlL (off)") ));
        return;
        }

    TBool back( EFalse );
    TPtrC8 url( NextUrlAttr( back ) );

    if ( url.Length() || back )
        {
        HBufC8* msg;
        if ( url.Length() )
            {
            // Activate URL in Browser.
            HBufC8* absUrl =
                CodUtil::AbsoluteUrlLC( iEng->Data().SourceUri(), url );
            msg = HBufC8::NewL
                ( ((TDesC)KCodMsgLaunchBrowserWithUrl).Length() +
                  absUrl->Length() );   // Not pushed, no leaving calls...
            msg->Des().Append( KCodMsgLaunchBrowserWithUrl );
            msg->Des().Append( *absUrl );
            CleanupStack::PopAndDestroy( absUrl );
            CleanupStack::PushL( msg ); // ...until this point!
            }
        else
            {
            // Empty URL (back), activate Browser.
            msg = HBufC8::NewLC( 0 );
            }

        TApaTaskList taskList( iEikEnv->WsSession() );
        TApaTask task = taskList.FindApp( TUid::Uid( KCodWmlBrowserUid ) );
        if ( task.Exists() )
            {
            // Browser is already running, send a message to it.
            // (If not running, we don't launch it.)
            task.SendMessage( TUid::Uid( 0 ), *msg );
            }

        CleanupStack::PopAndDestroy( msg );
        }
    CLOG(( 2, _L("<- CCodDownloadImpl::ActivateNextUrlL") ));
    }

// ---------------------------------------------------------
// CCodDownloadImpl::Done()
// ---------------------------------------------------------
//
void CCodDownloadImpl::Done()
    {
    CLOG(( 2, \
        _L("-> CCodDownloadImpl::Done state (%d) p.state(%d) p.err(%d)"),
        iState, iProgress.iState, iProgress.iError ));
    __ASSERT_DEBUG( iProgress.iState > TCodDownloadProgress::EInProgress, \
        CodUiPanic( ECodUiInternal ) );
    iState = EInit;
    // State change must come before the EDone callback.
    CLOG(( 2, _L("CodEventL(0x%x,EDone)"), &iProxy ));
    TRAP_IGNORE( iObserver.CodEventL( iProxy, MCodDownloadObserver::EDone ) );
    CLOG(( 2, _L("<- CCodDownloadImpl::Done") ));
    }
    
// ----------------------------------------------------------
// CCodDownloadImpl::GetRootPathL()
// ----------------------------------------------------------
//
void CCodDownloadImpl::GetRootPathL( TDes& aRootPath )
    {
    CLOG(( 2, _L("-> CCodDownloadImpl::GetRootPathL") ));
    if ( iEikEnv )
        {
        CodDialog::GetRootPathL( aRootPath );
        }
    else
        {
        // Without UI we download to phone memory.
#ifdef RD_MULTIPLE_DRIVE 
        TInt drive;
        User::LeaveIfError(
            DriveInfo::GetDefaultDrive( DriveInfo::EDefaultPhoneMemory, drive ) );
        User::LeaveIfError( PathInfo::GetRootPath( aRootPath, drive ) );
#else
        aRootPath = PathInfo::PhoneMemoryRootPath();
#endif
        }
    CLOG(( 2, _L("<- CCodDownloadImpl::GetRootPathL <%S>"), &aRootPath ));
    }

// ----------------------------------------------------------
// CCodDownloadImpl::StartLoadL()
// ----------------------------------------------------------
//
void CCodDownloadImpl::StartLoadL( TInt aStatusCode )
    {
    CLOG(( 2, _L("CCodDownloadImpl::StartLoadL aStatusCode(%d)"), \
        aStatusCode ));
    if ( aStatusCode != KHttp900Success )
        {
        // Error report started, update name.
        CLOG(( 2, _L("CodEventL(0x%x,ENameChanged)"), &iProxy ));
        iObserver.CodEventL( iProxy, MCodDownloadObserver::ENameChanged );
        CLOG(( 2, _L("CodEventL(0x%x,ETypeChanged)"), &iProxy ));
        iObserver.CodEventL( iProxy, MCodDownloadObserver::ETypeChanged );
        }
    // Network activity started, bring up download list.
    CLOG(( 2, _L("CodEventL(0x%x,EAccept)"), &iProxy ));
    iObserver.CodEventL ( iProxy, MCodDownloadObserver::EAccept );
    iObserver.CodEventL ( iProxy, MCodDownloadObserver::ENameChanged );
    }

// ----------------------------------------------------------
// CCodDownloadImpl::StartCancelL()
// ----------------------------------------------------------
//
void CCodDownloadImpl::StartCancelL()
    {
    CLOG(( 2, _L("CCodDownloadImpl::StartCancelL") ));
    }

// ----------------------------------------------------------
// CCodDownloadImpl::ProgressL()
// ----------------------------------------------------------
//
void CCodDownloadImpl::ProgressL( TInt aFinalValue, TInt aCurrentValue )
    {
    CLOG(( 2, _L("-> CCodDownloadImpl::ProgressL(%d/%d)"), \
        aCurrentValue, aFinalValue ));
    iProgress.iBytesCur = aCurrentValue;
    iProgress.iBytesMax = aFinalValue;
    CLOG(( 2, _L("CodEventL(0x%x,EProgressUpdate)"), &iProxy ));
    iObserver.CodEventL( iProxy, MCodDownloadObserver::EProgressUpdate );
    CLOG(( 2, _L("<- CCodDownloadImpl::ProgressL") ));
    }

// ----------------------------------------------------------
// CCodDownloadImpl::Done()
// ----------------------------------------------------------
//
void CCodDownloadImpl::Done
( TInt LOG_ONLY( aStatusCode ), TInt LOG_ONLY( aError ) )
    {
    CLOG(( 2, _L("CCodDownloadImpl::Done aStatusCode(%d) aError(%d)"), \
        aStatusCode, aError ));
    }

// ----------------------------------------------------------
// CCodDownloadImpl::ConfirmConnectL()
// ----------------------------------------------------------
//
TBool CCodDownloadImpl::ConfirmConnectL()
    {
    CLOG(( 2, _L("-> CCodDownloadImpl::ConfirmConnectL") ));
    TBool connect( EFalse );  // Say no by default. (Applies if no UI present.)
    if ( iEikEnv )
        {
        connect = CodDialog::ConfirmConnectL();
        }
    CLOG(( 2, _L("<- CCodDownloadImpl::ConfirmConnectL(%d)"), connect ));
    return connect;
    }

// ----------------------------------------------------------
// CCodDownloadImpl::StartConnect()
// ----------------------------------------------------------
//
void CCodDownloadImpl::StartConnect( TUint32 LOG_ONLY( aIap ) )
    {
    CLOG(( 2, _L("CCodDownloadImpl::StartConnect aIap(%d)"), aIap ));
    }

// ----------------------------------------------------------
// CCodDownloadImpl::EndConnect()
// ----------------------------------------------------------
//
void CCodDownloadImpl::EndConnect()
    {
    CLOG(( 2, _L("CCodDownloadImpl::EndConnect") ));
    }

// ----------------------------------------------------------
// CCodDownloadImpl::UserAuthL()
// ----------------------------------------------------------
//
TBool CCodDownloadImpl::UserAuthL
        (
        const TDesC& LOG_ONLY( aHost ),
        const TDesC& LOG_ONLY( aRealm ),
        TBool LOG_ONLY( aProxyAuth ),
        TDes& aUsername,
        TDes& aPassword
        )
    {
    CLOG(( 2, _L("CCodDownloadImpl::UserAuthL") ));
    CLOG(( 3, _L("  aHost<%S>"), &aHost ));
    CLOG(( 3, _L("  aRealm<%S>"), &aRealm ));
    CLOG(( 3, _L("  aProxyAuth(0x%x)"), aProxyAuth ));
    // Feature dropped - empty implementation.
    aUsername.Zero();
    aPassword.Zero();
    return ETrue;
    }

// ----------------------------------------------------------
// CCodDownloadImpl::RoapTriggerParsedL()
// ----------------------------------------------------------
//
void CCodDownloadImpl::RoapTriggerParsedL( const CRoapData& aData )
    {
    CLOG(( 2, _L("-> CCodDownloadImpl::RoapTriggerParsedL") ));
    iRoap = ETrue;
#ifdef __TEST_COD_LOG
    CLOG(( 4, _L("  Trigger type(%d) contextStatus(%d) domainOp(%d)"), \
        aData.iType, aData.iContextStatus, aData.iDomainOperation ));
    TPtrC8 ptr;
    for ( TInt i = 0; i < aData.iContentIdList.Count(); i++ )
        {
        ptr.Set( aData.iContentIdList[i]->Des() );
        CLOG(( 4, _L8("  cid#%d:<%S>"), i, &ptr ));
        }
    if ( aData.iRiAlias )
        {
        ptr.Set( aData.iRiAlias->Des() );
        CLOG(( 4, _L8("  riAlias <%S>"), &ptr ));
        }
    else
        {
        CLOG(( 4, _L8("  riAlias NULL") ));
        }
#endif /* def __TEST_COD_LOG */

    if ( aData.iContextStatus == Roap::EInvalidContext )
        {
#ifdef RD_DRM_SILENT_RIGHTS_ACQUISITION
        HBufC* header = StringLoader::LoadLC( R_QTN_DRM_MSQ_QRY_TITLE_REGISTRAT ); 
        HBufC* regtxt = StringLoader::LoadLC( R_QTN_DRM_MSQ_QUERY_REGISTRATION );
        HBufC* moreinfo = StringLoader::LoadLC( R_QTN_DRM_MSQ_QUERY_LINK_MORE_INF  );

        HBufC* regtxt_moreinfo = HBufC::NewLC(regtxt->Length() + moreinfo->Length() + 1 );
        TPtr regPtr( regtxt_moreinfo->Des() );
        regPtr += *regtxt;
        regPtr += KNextLine; 
        regPtr += *moreinfo; 


        CAknMessageQueryDialog* dlg = CAknMessageQueryDialog::NewL( *regtxt_moreinfo );
        dlg->PrepareLC( R_COD_REGISTERCANCEL_QUERY  );

        CAknPopupHeadingPane* hPane = dlg->QueryHeading();
        if ( hPane )
        {
            hPane->SetTextL( *header );
        }
  
        // set link 
        dlg->SetLinkTextL(*moreinfo);
        TCallBack callback(ShowRoapCallback, this); 
        dlg->SetLink(callback); 
   
        TInt ret = dlg->RunLD();
        if( ret != EAknSoftkeyYes ) 
            {
            CLOG(( 4, _L("  cancelled") ));
            User::Leave( KErrCancel );
            }            
        CleanupStack::PopAndDestroy(4); // header, regtxt, moreinfo and regtxt_moreinfo   
#else // RD_DRM_SILENT_RIGHTS_ACQUISITION
        // No valid RI context, ROAP will involve device registration.
        // User must confirm.
        CLOG(( 4, _L("  invalid context") ));
        CAknQueryDialog* dialog = CAknQueryDialog::NewL();
        dialog->PrepareLC( R_COD_UI_CONFIRM_NOTE );
        HBufC* prompt;
        if ( aData.iRiAlias )
            {
            // "Registration with %U needed. Register phone?"
            HBufC* riAlias16 = CodUtil::ConvertLC( *aData.iRiAlias );
            prompt = StringLoader::LoadL
                ( R_QTN_DRM_QUERY_REGISTER_WITH_U, *riAlias16, iEikEnv );
            CleanupStack::PopAndDestroy( riAlias16 );
            CleanupStack::PushL( prompt );
            }
        else
            {
            // "Registration with content provider needed. Register phone?"
            prompt = StringLoader::LoadLC
                ( R_QTN_DRM_QUERY_REGISTER_WITH_CP, iEikEnv );
            }
        dialog->SetPromptL( *prompt );
        CleanupStack::PopAndDestroy( prompt );
        if ( !dialog->RunLD() ) 
            {
            CLOG(( 4, _L("  cancelled") ));
            User::Leave( KErrCancel );
            }
#endif 
        }

    CLOG(( 2, _L("<- CCodDownloadImpl::RoapTriggerParsedL") ));
    }

// ----------------------------------------------------------
// CCodDownloadImpl::HandleServerAppExit()
// ----------------------------------------------------------
//
void CCodDownloadImpl::HandleServerAppExit( TInt /*aReason*/ )
    {
    CLOG(( 2, _L("-> CCodDownloadImpl::HandleServerAppExit") ));
    TRAP_IGNORE( ActivateNextUrlL() );
    Done();
    CLOG(( 2, _L("<- CCodDownloadImpl::HandleServerAppExit") ));
    }

// ----------------------------------------------------------
// CCodDownloadImpl::NextUrlAttr()
// ----------------------------------------------------------
//
TPtrC8 CCodDownloadImpl::NextUrlAttr( TBool& aBack ) const
    {
    CLOG(( 2, _L("-> CCodDownloadImpl::NextUrlAttr") ));
    TPtrC8 url;
    aBack = EFalse;
    const CCodData& data = iEng->Data();
    if( iCod )
        {
        // This is a COD: choose between Next-URL or Next_URLatError.
        if ( iEng->GetStatusCode() == KHttp900Success )
            {
            url.Set( data.NextUrl() );
            }
        else
            {
            url.Set( data.NextUrlAtError() );
            }
        // Recognize 'back' or 'local'.
        if ( url.Length() )
            {
            if ( !url.Compare( KCodReservedWordLocal ) )
                {
                // 'local' -> no URL to launch.
                url.Set( KNullDesC8 );
                }
            else if ( !url.Compare( KCodReservedWordBack ) )
                {
                // 'back' -> no URL to launch.
                url.Set( KNullDesC8 );
                aBack = ETrue;
                }
            }
        }
    else
        {
        // This is a DD: NextURL goes for success and failure.
        url.Set( data.NextUrl() );
        }
            
    CLOG(( 2, _L8("<- CCodDownloadImpl::NextUrlAttr url<%S> back(%d)"), \
        &url, aBack ));
    return url;
    }

// ----------------------------------------------------------
// CCodAppUi::PdPlayAvailable()
// ----------------------------------------------------------
//
void CCodDownloadImpl::PdPlayAvailable()
{
    TRAP_IGNORE( iObserver.CodEventL( iProxy, MCodDownloadObserver::EPdPlayAvailable ) );
}

// ----------------------------------------------------------
// CCodDownloadImpl::UpdateMediaInfoL()
// ----------------------------------------------------------
//
void CCodDownloadImpl::UpdateMediaInfoL()
{
    TRAP_IGNORE( iObserver.CodEventL( iProxy, MCodDownloadObserver::EUpdatedMediaInfo ) );
}

// ----------------------------------------------------------
// CCodDownloadImpl::UpdateTrackInfoL()
// ----------------------------------------------------------
//
void CCodDownloadImpl::UpdateTrackInfoL()
{
    TRAP_IGNORE( iObserver.CodEventL( iProxy, MCodDownloadObserver::EUpdatedTrackInfo ) );
}

// ----------------------------------------------------------
// CCodDownloadImpl::WaitForLicenseL()
// ----------------------------------------------------------
//
void CCodDownloadImpl::WaitForLicenseL()
    {
    CodDialog::InfoNoteL( R_QTN_BROWSER_DOWNLOADS_WAITING_FOR_LICENSE, *iEikEnv );
    }
    
// ----------------------------------------------------------
// CCodDownloadImpl::UpdatePausable
// ----------------------------------------------------------
//
void CCodDownloadImpl::UpdatePausable( TBool aPausable )
{
    TRAP_IGNORE( iObserver.CodEventL( iProxy, aPausable ? MCodDownloadObserver::EDownloadPausable 
                                                        : MCodDownloadObserver::EDownloadNonPausable ) );
}

// ----------------------------------------------------------
// CCodDownloadImpl::MediaObjectNameChanged()
// ----------------------------------------------------------
//
void CCodDownloadImpl::MediaObjectNameChanged()
{
    TRAP_IGNORE( iObserver.CodEventL( iProxy, MCodDownloadObserver::ENameChanged ));
}

// ----------------------------------------------------------
// CCodDownloadImpl::ContentTypeChanged()
// ----------------------------------------------------------
//
void CCodDownloadImpl::ContentTypeChanged()
{
    TRAP_IGNORE( iObserver.CodEventL( iProxy, MCodDownloadObserver::ETypeChanged ));
}

// ----------------------------------------------------------
// CCodDownloadImpl::SetActiveDownload()
// ----------------------------------------------------------
//
void CCodDownloadImpl::SetActiveDownload( )
    {
    TRAP_IGNORE( iObserver.CodEventL( iProxy, MCodDownloadObserver::ESetActiveDownload ));
    }

EXPORT_C TInt CCodDownloadImpl::ActiveDownload()
    {
    return iEng->ActiveDownload();
    }
// ----------------------------------------------------------
// CCodDownloadImpl::HandleRoapPostResponseUrlL()
// ----------------------------------------------------------
//
void CCodDownloadImpl::HandleRoapPostResponseUrlL( const TDesC8& aPrUrl ) 
{

	if(iPostResponseURL)
		{
		delete iPostResponseURL;
		iPostResponseURL = NULL;
		}
	
    iPostResponseURL = aPrUrl.AllocL();	
    
    iObserver.CodEventL( iProxy, MCodDownloadObserver::EHandlePostResponse );
}
