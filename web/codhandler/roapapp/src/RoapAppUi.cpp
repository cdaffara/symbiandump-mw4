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
*      Implementation of class CRoapAppUi.   
*      
*
*/


#include <avkon.hrh>
#include <eikbtgpc.h>
#include <eikapp.h>
#include <apgcli.h>
#include <RoapApp.rsg>
#include <avkon.rsg>
#include <aknnotewrappers.h>
#include <AknQueryDialog.h>
#include <StringLoader.h>
#include <DocumentHandler.h>
#include <DcfRep.h>
#include <caf/caf.h>
#include <DRMCommon.h>
#include <aknmessagequerydialog.h>
#include <aknnotedialog.h>                  
#include <featmgr.h>                    
#include <hlplch.h>             
#include <RoapDef.h>
#include "RoapAppUi.h"
#include "RoapAppView.h" 
#include "RoapAppDocument.h" 
#include "RoapAppData.h" 
#include "RoapAppWaitDialog.h" 
#include "RoapApp.hrh"
#include "RoapAppPanic.h"
#include "RequestCompleteCallback.h"
#include "RoapAppLogger.h"
#include <HttpDownloadMgrCommon.h>

#ifdef __SERIES60_HELP
// Context-Sensitve Help File
#include <csxhelp/drm.hlp.hrh>
#endif // __SERIES60_HELP

// ================= CONSTANTS =======================
/// Temp file directory:
_LIT( KRoapTempFilePath, "c:\\system\\temp\\" );

/// Next Line
_LIT( KNextLine, "\n" );

// ROAP App UID
LOCAL_D const TUid KROAPAppUid = { 0x10008d64 };

// BrowserNG UID
LOCAL_D const TUid KCRUidBrowser   = {0x10008D39};

// KBrowserDefaultAccessPoint
LOCAL_D const TInt KBrowserDefaultAccessPoint = 0x0E;


// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------
// CRoapAppUi::CRoapAppUi()
// ----------------------------------------------------------
//
CRoapAppUi::CRoapAppUi()
    {
    CLOG(( 2, _L("*** CRoapAppUi::CRoapAppUi") ));
    }

// ----------------------------------------------------------
// CRoapAppUi::ConstructL()
// ----------------------------------------------------------
//
void CRoapAppUi::ConstructL()
    {
    BaseConstructL( EAknEnableMSK | EAknEnableSkin );
    iView = CRoapAppView::NewL( ClientRect(), this );
    iCallback = new ( ELeave ) CRequestCompleteCallback();
    
    TRAPD( err, iDownloadMgr.ConnectL( KROAPAppUid, *this, EFalse ) );
	User::LeaveIfError(err);
    
#ifndef __WINSCW__

    CRepository *repository = CRepository::NewL( KCRUidBrowser );

    TInt retVal(0);
    repository->Get(KBrowserDefaultAccessPoint, retVal);
    delete repository;
    repository = NULL;
    
    CLOG(( 2, _L("   KBrowserDefaultAccessPoint: %d"), retVal ));
    
    iDownloadMgr.SetIntAttribute(EDlMgrIap, retVal );
    
#endif
    
    iPostResponseURLDownLoad = EFalse;	
 
    }

// ----------------------------------------------------------
// CRoapAppUi::~CRoapAppUi()
// ----------------------------------------------------------
//
CRoapAppUi::~CRoapAppUi()
    {
    
 CLOG(( 2, _L("*** -> CRoapAppUi::~CRoapAppUi") ));
       
    if( iDownloadMgr.Handle() )
 	    {
 	    iDownloadMgr.DeleteAll();
 	    iDownloadMgr.Close();
 	    }
    
    // We must Cancel the model before deleting the callback.
    RoapDocument()->Engine().Cancel();
    delete iCallback;
    delete iDocHandler;
    delete iWaitNote;
    delete iView;
    if ( iDoorObserver )
        {
        iDoorObserver->NotifyExit( MApaEmbeddedDocObserver::ENoChanges );
        }
    iFile.Close();
    iFs.Close();
    CLOG(( 2, _L("<- CRoapAppUi::~CRoapAppUi") ));
    }

// ----------------------------------------------------------
// CRoapAppUi::OpenFileL()
// ----------------------------------------------------------
//
void CRoapAppUi::OpenFileL( const TDesC& aFileName )
    {
    CLOG(( 2, _L("-> CRoapAppUi::OpenFileL aFileName<%S>"), &aFileName ));

    iPostResponseURLDownLoad = EFalse;	
    Document()->OpenFileL( ETrue, aFileName, iCoeEnv->FsSession() );
    CLOG(( 2, _L("<- CRoapAppUi::OpenFileL") ));
    }

// ----------------------------------------------------------
// CRoapAppUi::HandleCommandL()
// ----------------------------------------------------------
//
void CRoapAppUi::HandleCommandL( TInt aCommand )
    {
    CLOG(( 2, _L("-> CRoapAppUi::HandleCommandL aCommand(0x%x)"), \
        aCommand ));
    switch ( aCommand )
        {
        // From blank screen show during startup (incl. parsing).
        case EAknSoftkeyBack:
        // CAknShutter generated command.
        case EEikCmdExit:
            {
            Exit();
            break;
            }

        case EAknSoftkeyEmpty:
            {
            break;
            }
        
        case ERoapAppCmdCancel:
            {
            CancelRoap();
            break;
            }
        default:
            {
            CLOG(( 0, _L("  unknown command") ));
            __ASSERT_DEBUG
                ( EFalse, RoapAppPanic( ERoapAppUnknownCommand ) );
            break;
            }
        }
    CLOG(( 2, _L("<- CRoapAppUi::HandleCommandL") ));
    }

// ----------------------------------------------------------
// CRoapAppUi::ProcessCommandParametersL()
// ----------------------------------------------------------
//
TBool CRoapAppUi::ProcessCommandParametersL
( TApaCommand aCommand, TFileName& aDocumentName, const TDesC8& /*aTail*/ )
    {
    CLOG(( 2, _L("-> CRoapAppUi::ProcessCommandParametersL") ));
    CLOG(( 2, _L("   CRoapAppUi: aCommand: %d / aDocumentName: %S"), aCommand, &aDocumentName ));
    if ( aCommand == EApaCommandOpen )
        {
        OpenFileL( aDocumentName );
        }
    CLOG(( 2, _L("<- CRoapAppUi::ProcessCommandParametersL") ));
    return EFalse;
    }

// ----------------------------------------------------------
// CRoapAppUi::HandleResourceChangeL()
// ----------------------------------------------------------
//
void CRoapAppUi::HandleResourceChangeL( TInt aType )
    {
    CAknAppUi::HandleResourceChangeL( aType );
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        iView->SetRect( ClientRect() );
        }
    iView->HandleResourceChange( aType );
    }

// ----------------------------------------------------------
// CRoapAppUi::ConnectionConfL()
// ----------------------------------------------------------
//
TBool CRoapAppUi::ConnectionConfL()
    {
    // TODO ask user.
    return ETrue;
    }
    
// ----------------------------------------------------------
// CRoapAppUi::ContactRiConfL()
// ----------------------------------------------------------
//
TBool CRoapAppUi::ContactRiConfL()
    {
    // TODO ask user.
    return ETrue;
    }
    
// ----------------------------------------------------------
// CRoapAppUi::TransIdConfL()
// ----------------------------------------------------------
//
TBool CRoapAppUi::TransIdConfL()
    {
    return EFalse;
    }
    
// ----------------------------------------------------------
// CRoapAppUi::RightsObjectDetailsL()
// ----------------------------------------------------------
//
void CRoapAppUi::RightsObjectDetailsL
( const RPointerArray<CDRMRights>& /*aRightsList*/ )
    {
    // TODO
    }
		    
// ----------------------------------------------------------
// CRoapAppUi::ContentDownloadInfoL()
// ----------------------------------------------------------
//
void CRoapAppUi::ContentDownloadInfoL
( TPath& aTempFolder, TFileName& aContentName, TInt& aMaxSize )
    {
    aTempFolder = KRoapTempFilePath;
    aContentName = KNullDesC;
    aMaxSize = -1;  // -1 == 'unknown'.
    }

// ----------------------------------------------------------
// CRoapAppUi::ContentDetailsL()
// ----------------------------------------------------------
//
void CRoapAppUi::ContentDetailsL
( const TDesC& /*aPath*/, const TDesC8& /*aType*/, const TUid& /*aAppUid*/ )
    {
    // TODO
    }
                                      
// ----------------------------------------------------------
// CRoapAppUi::HandleDCFPartL()
// ----------------------------------------------------------
//
void CRoapAppUi::HandleDCFPartL( const TDesC8& /*aFilename*/ )
    {
    // DRM Engine does not support multipart ROAP response yet.
    // This method will never be called.
    User::Leave( KErrNotSupported );
    }
        
// ----------------------------------------------------------
// CRoapAppUi::RoapProgressInfoL()
// ----------------------------------------------------------
//
void CRoapAppUi::RoapProgressInfoL( const TInt /*aProgressInfo*/ )
    {
    }

// ----------------------------------------------------------
// CRoapAppUi::ErrorUrlL()
// ----------------------------------------------------------
//
void CRoapAppUi::ErrorUrlL( const TDesC8& /* aErrorUrl */)
{	
}

// ----------------------------------------------------------
// CRoapAppUi::PostResponseUrlL()
// ----------------------------------------------------------
//
void CRoapAppUi::PostResponseUrlL( const TDesC8& aPrUrl )
{
    CLOG(( 2, _L("-> CRoapAppUi::PostResponseUrlL(\n    %S)"), &aPrUrl ));

    TBool retVal( ETrue );    
 
    //RoapApp is made as client of DownloadMgr. A new download session is created with PostResponse URL,
    //which handles the response like in any other download
    RHttpDownload& download = iDownloadMgr.CreateDownloadL(aPrUrl, retVal );
    download.SetBoolAttribute( EDlAttrNoContentTypeCheck, ETrue );
    //set the Download status of PostResponse URL 
    iPostResponseURLDownLoad = ETrue;
    User::LeaveIfError( download.Start());

}
		
// ----------------------------------------------------------
// CRoapAppUi::HandleServerAppExit()
// ----------------------------------------------------------
//
void CRoapAppUi::HandleServerAppExit( TInt /*aReason*/ )
    {
    RunAppShutter();
    }

// ---------------------------------------------------------
// CRoapAppUi::LaunchHelpL()
// ---------------------------------------------------------
//
void CRoapAppUi::LaunchHelpL( const TDesC& aContext )
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
    HlpLauncher::LaunchHelpApplicationL( iCoeEnv->WsSession(), contexts );
    #endif // __SERIES60_HELP
    }


// ----------------------------------------------------------
// CRoapAppUi::ShowRoapCallback(TAny* aPtr)
// ----------------------------------------------------------
//

TInt CRoapAppUi::ShowRoapCallback( TAny* aPtr )
{
    TInt err( KErrNone );    
    CRoapAppUi* self = static_cast<CRoapAppUi*>(aPtr);
    TRAP( err, self->LaunchHelpL( KDRM_HLP_REGISTRATION ) );
    return err;        
}


// ----------------------------------------------------------
// CRoapAppUi::HandleTriggerParsedL()
// ----------------------------------------------------------
//
void CRoapAppUi::HandleTriggerParsedL()
    {
    CLOG(( 2, _L("-> CRoapAppUi::HandleTriggerParsedL") ));
    TBool doRoap( ETrue );
    const CRoapAppData& data( RoapDocument()->Data() );

    if ( data.iContextStatus == Roap::EInvalidContext )
        {
#ifdef RD_DRM_SILENT_RIGHTS_ACQUISITION
        HBufC* header = StringLoader::LoadLC( R_QTN_DRM_MSQ_QRY_TITLE_REGISTRAT ); 
        HBufC* regtxt = StringLoader::LoadLC( R_QTN_DRM_MSQ_QUERY_REGISTRATION );
        HBufC* moreinfo = NULL; 
        TBool helpFeature = FeatureManager::FeatureSupported( KFeatureIdHelp );
        if ( helpFeature )
            {        
            moreinfo = StringLoader::LoadLC( R_QTN_DRM_MSQ_QUERY_LINK_MORE_INF  );
            }
        else 
            {
            moreinfo = KNullDesC().AllocLC();
            }
                

        HBufC* regtxt_moreinfo = HBufC::NewLC(regtxt->Length() + moreinfo->Length() + 1 );
        TPtr regPtr( regtxt_moreinfo->Des() );
        regPtr += *regtxt;
        regPtr += KNextLine; 
        regPtr += *moreinfo; 
      
        
        CAknMessageQueryDialog* dlg = CAknMessageQueryDialog::NewL( *regtxt_moreinfo );
        dlg->PrepareLC( R_ROAP_REGISTERCANCEL_QUERY  );

        CAknPopupHeadingPane* hPane = dlg->QueryHeading();
        if ( hPane )                                         
            {
            hPane->SetTextL( *header );
            }
            
        if ( helpFeature )
            {        
            // set link 
            dlg->SetLinkTextL(*moreinfo);
            TCallBack callback(ShowRoapCallback, this); 
            dlg->SetLink(callback);             
            } 
        
        TInt ret = dlg->RunLD();
        if( ret != EAknSoftkeyYes ) 
            {
            doRoap = EFalse; 
            }            
         CleanupStack::PopAndDestroy(4); // header, regtxt, moreinfo, regtxt_moreinfo    
#else  // for RD_DRM_SILENT_RIGHTS_ACQUISITION
        // No valid RI context, ROAP will involve device registration.
        // User must confirm.
        CLOG(( 4, _L("  invalid RI context") ));
        CAknQueryDialog* dialog = CAknQueryDialog::NewL();
        dialog->PrepareLC( R_ROAP_APP_CONFIRM_QUERY );
        HBufC* prompt;
        if ( data.iRiAlias )
            {
            // "Registration with %U needed. Register phone?"
            HBufC* riAlias16 = HBufC::NewLC( data.iRiAlias->Length() );
            riAlias16->Des().Copy( *data.iRiAlias );
            prompt = StringLoader::LoadL
                ( R_QTN_DRM_QUERY_REGISTER_WITH_U, *riAlias16, iCoeEnv );
            CleanupStack::PopAndDestroy( riAlias16 );
            CleanupStack::PushL( prompt );
            }
        else
            {
            // "Registration with content provider needed. Register phone?"
            prompt = StringLoader::LoadLC
                ( R_QTN_DRM_QUERY_REGISTER_WITH_CP, iCoeEnv );
            }
        dialog->SetPromptL( *prompt );
        CleanupStack::PopAndDestroy( prompt );
        if ( !dialog->RunLD() ) 
            {
            CLOG(( 4, _L("  cancelled") ));
            doRoap = EFalse;    // User cancelled registration.
            }
#endif // for RD_DRM_SILENT_RIGHTS_ACQUISITION
    }
        
    if ( doRoap )
        {
        StartRoapL();
        }
    else
        {
          CAknNoteDialog* dialog = new (ELeave) CAknNoteDialog
              (CAknNoteDialog::EConfirmationTone,CAknNoteDialog::EUndefinedTimeout);
          dialog->PrepareLC( R_ROAP_REGISTER_CANCEL_OK_INFO );
          HBufC* infoPrompt = StringLoader::LoadLC
              ( R_QTN_DRM_INFO_PHONE_NOT_REGISTER, iCoeEnv );
          dialog->SetTextL( *infoPrompt); 
		  CleanupStack::PopAndDestroy( infoPrompt ); 
          if ( !dialog->RunLD() )
		      {
              CLOG(( 2, _L("Select Cancel") ));
			  }
		  RunAppShutter();    
        }
    CLOG(( 2, _L("<- CRoapAppUi::HandleTriggerParsedL") ));
    }
    
// ----------------------------------------------------------
// CRoapAppUi::StartRoapL()
// ----------------------------------------------------------
//
void CRoapAppUi::StartRoapL()
    {
    CLOG(( 2, _L("-> CRoapAppUi::StartRoapL") ));
    __ASSERT_DEBUG( !iWaitNote, RoapAppPanic( ERoapAppInternal ) );
    TInt resId = 0;
    switch ( RoapDocument()->Data().iType )
        {
        case Roap::ERegistrationTrigger:
            {
            resId = R_QTN_DRM_WAIT_REGISTERING_DEVICE;
            break;
            }
        case Roap::ERoAcquisitionTrigger:
            {
            resId = R_QTN_DRM_WAIT_GETTING_KEY;
            break;
            }
        case Roap::EJoinDomainTrigger:
            {
            if ( RoapDocument()->Data().iDomainOperation ==
                 Roap::EJoinDomainOperation )
                {
                resId = R_QTN_DRM_WAIT_JOINING_DOMAIN;
                }
            else
                {
                resId = R_QTN_DRM_WAIT_UPDATING_ACCOUNT;
                }
            break;
            }
        case Roap::ELeaveDomainTrigger:
            {
            resId = R_QTN_DRM_WAIT_REMOVE_FR_ACCOUNT;
            break;
            }
        case Roap::EMeteringReportTrigger:
            {
            resId = R_QTN_DRM_WAIT_UPDATE_METERING_INFO;
            break;
            }
        default:
            {
            User::Leave( KErrNotSupported );
            break;
            }
        }
    __ASSERT_DEBUG( resId, RoapAppPanic( ERoapAppInternal ) );
    iWaitNote = new (ELeave) CRoapAppWaitDialog
        ( REINTERPRET_CAST( CEikDialog**, &iWaitNote ), *this );
    iWaitNote->PrepareLC( R_ROAP_APP_WAIT_NOTE );
    HBufC* prompt = iCoeEnv->AllocReadResourceLC( resId );
    iWaitNote->SetTextL( *prompt );
    CleanupStack::PopAndDestroy( prompt );
    iWaitNote->RunLD();
    RoapDocument()->Engine().AcceptL( this, &iCallback->iStatus );
    iCallback->CallbackOnCompletion( TCallBack( StaticRoapDone, this ) );
    CLOG(( 2, _L("<- CRoapAppUi::StartRoapL") ));
    }

// ----------------------------------------------------------
// CRoapAppUi::CancelRoap()
// ----------------------------------------------------------
//
void CRoapAppUi::CancelRoap()
    {
    CLOG(( 2, _L("-> CRoapAppUi::CancelRoap") ));
    RoapDocument()->Engine().Cancel();
    CLOG(( 2, _L("<- CRoapAppUi::CancelRoap") ));
    }

// ----------------------------------------------------------
// CRoapAppUi::RoapDone()
// ----------------------------------------------------------
//
void CRoapAppUi::RoapDone()
    {
    CLOG(( 2, _L("-> CRoapAppUi::RoapDone") ));
    delete iWaitNote;
    iWaitNote = NULL;
    TInt err = iCallback->iStatus.Int();
    TBool exitApp( ETrue ); // Exit by default.
    if ( err )
        {
        TRAP_IGNORE( exitApp = RoapFailedL( err ) );
        }
    else
        {
        TRAP_IGNORE( exitApp = RoapOkL() );
        }
    if ( exitApp && !iPostResponseURLDownLoad)
        {
        RunAppShutter();
        }
    CLOG(( 2, _L("<- CRoapAppUi::RoapDone") ));
    }

// ----------------------------------------------------------
// CRoapAppUi::RoapOkL()
// ----------------------------------------------------------
//
TBool CRoapAppUi::RoapOkL()
    {
    CLOG(( 2, _L("-> CRoapAppUi::RoapOkL") ));
    TBool exitApp( ETrue );
    switch( RoapDocument()->Data().iType )
        {
        case Roap::ERegistrationTrigger:
            {
            ConfNoteL( R_QTN_DRM_CONF_DEVICE_REGISTERED );
            break;
            }
        case Roap::ERoAcquisitionTrigger:
            {
            exitApp = RoAcqOkL();
            break;
            }
        case Roap::EJoinDomainTrigger:
            {
            if ( RoapDocument()->Data().iDomainOperation ==
                 Roap::EJoinDomainOperation )
                {
                ConfNoteL( R_QTN_DRM_CONF_JOINED_TO_DOMAIN );
                }
            else
                {
                ConfNoteL( R_QTN_DRM_CONF_ACCOUNT_UPDATED );
                }
            break;
            }
        case Roap::ELeaveDomainTrigger:
            {
            ConfNoteL( R_QTN_DRM_INFO_REMOVE_FR_ACCOUNT );
            break;
            }
        case Roap::EMeteringReportTrigger:
            {
            ConfNoteL( R_QTN_DRM_CONF_ACCOUNT_UPDATED );
            break;
            }            
        default:
            {
            // We should never get here.
            __ASSERT_DEBUG( EFalse, RoapAppPanic( ERoapAppInternal ) );
            }
        }
    CLOG(( 2, _L("<- CRoapAppUi::RoapOkL returns(%d)"), exitApp ));
    return exitApp;
    }

// ----------------------------------------------------------
// CRoapAppUi::RoAcqOkL()
// ----------------------------------------------------------
//
TBool CRoapAppUi::RoAcqOkL()
    {
    CLOG(( 2, _L("-> CRoapAppUi::RoAcqOkL") ));
    __ASSERT_DEBUG \
        ( RoapDocument()->Data().iType == Roap::ERoAcquisitionTrigger, \
          RoapAppPanic( ERoapAppInternal ) );

    TFileName name;    // Note, filename and extension only! Not full path.
    TDataType type;
    TUid appUid( TUid::Null() );
    if ( RoapDocument()->Data().iContentIdList.Count() )
        {
        __ASSERT_DEBUG( !iFs.Handle(), RoapAppPanic( ERoapAppInternal ) );
        __ASSERT_DEBUG( !iFile.SubSessionHandle(), \
            RoapAppPanic( ERoapAppInternal ) );
        User::LeaveIfError( iFs.Connect() );
        User::LeaveIfError( iFs.ShareProtected() );
        CDcfRep* dcfRep = CDcfRep::NewL();
        CleanupStack::PushL( dcfRep );
        // It is not specified what to do if we have more content ids.
        // Best guess: open the first.
        TPtrC8 cid( *(RoapDocument()->Data().iContentIdList[0]) );
        TRAPD( err, dcfRep->GetFileHandleL ( cid, iFile, iFs ) );
        if ( !err && CanOpenL( iFile ) )
            {
            User::LeaveIfError( iFile.Name( name ) );
            RApaLsSession apaLs;
            User::LeaveIfError( apaLs.Connect() );
            CleanupClosePushL<RApaLsSession>( apaLs );
            User::LeaveIfError( apaLs.AppForDocument( iFile, appUid, type ) );
            CleanupStack::PopAndDestroy();  // apaLs
            }
#ifdef __TEST_ROAP_APP_LOG
        else
            {
            CLOG(( 4, _L8("  err(%d) getting file by cid<%S>"), err, &cid ));
            }
#endif /* def __TEST_ROAP_APP_LOG */
        CleanupStack::PopAndDestroy( dcfRep );
        }

    TBool exitApp( ETrue );
    if ( appUid.iUid )
        {
        // MO (related to the received RO) is already here and is supported
        // by the system. Ask user if he wants to open it now.
        __ASSERT_DEBUG( iFs.Handle(), RoapAppPanic( ERoapAppInternal ) );
        __ASSERT_DEBUG( iFile.SubSessionHandle(), \
            RoapAppPanic( ERoapAppInternal ) );
        __ASSERT_DEBUG( name.Length(), RoapAppPanic( ERoapAppInternal ) );
        CAknQueryDialog* dialog = CAknQueryDialog::NewL();
        dialog->PrepareLC( R_ROAP_APP_CONFIRM_QUERY );
        HBufC* prompt = StringLoader::LoadLC
            ( R_QTN_DRM_QUERY_KEY_RECEIVED_OPEN, name, iEikonEnv );
        dialog->SetPromptL( *prompt );
        CleanupStack::PopAndDestroy( prompt );
        if ( dialog->RunLD() ) 
            {
            CLOG(( 4, _L("  launching <%S>, handler(0x%x)"), \
                &name, appUid.iUid ));
            __ASSERT_DEBUG( !iDocHandler, RoapAppPanic( ERoapAppInternal ) );
            iDocHandler = CDocumentHandler::NewL
                ( (CEikProcess*)Application()->Process() );
            iDocHandler->SetExitObserver( this );
            User::LeaveIfError
                ( iDocHandler->OpenFileEmbeddedL( iFile, type ) );
            exitApp = EFalse;
            }
        }
    else
        {
        ConfNoteL( R_QTN_DRM_INFO_ACT_KEY_RECEIVED );
        }
    CLOG(( 2, _L("<- CRoapAppUi::RoAcqOkL returns(%d)"), exitApp ));
    return exitApp;
    }

// ----------------------------------------------------------
// CRoapAppUi::RoapFailedL()
// ----------------------------------------------------------
//
TBool CRoapAppUi::RoapFailedL( TInt aError )
    {
    CLOG(( 2, _L("-> CRoapAppUi::RoapFailedL aError(%d)"), aError ));
    TBool exitApp( ETrue );
    TInt resId = 0;
    switch ( aError )
        {
        case KErrCancel:
            {
            // For Cancel, just do nothing.
            break;
            }
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
            if ( RoapDocument()->Data().iType == Roap::ERoAcquisitionTrigger )
                {
                CAknQueryDialog* dialog = CAknQueryDialog::NewL();
                dialog->PrepareLC( R_ROAP_APP_CONFIRM_QUERY );
                HBufC* prompt = StringLoader::LoadLC
                    ( R_QTN_DRM_GET_KEY_FAILED_RETRY, iEikonEnv );
                dialog->SetPromptL( *prompt );
                CleanupStack::PopAndDestroy( prompt );
                if ( dialog->RunLD() ) 
                    {
                    StartRoapL();
                    exitApp = EFalse;
                    }
                break;
                }
            // Not RoAcq: fall through to default (error note).
            }
        default:
            {
            // Default error message is based on trigger type.
            switch ( RoapDocument()->Data().iType )
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
                    if ( RoapDocument()->Data().iDomainOperation ==
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
                    resId = R_QTN_DRM_INFO_REGISTRATION_FAILED;
                    break;
                    }                    
                default:
                    {
                    __ASSERT_DEBUG( EFalse, \
                        RoapAppPanic( ERoapAppInternal ) );
                    }
                }
            break;
            }
        }
    if ( resId )
        {
        InfoNoteL( resId );
        }
    CLOG(( 2, _L("<- CRoapAppUi::RoapFailedL returns(%d)"), exitApp ));
    return exitApp;
    }

// ----------------------------------------------------------
// CRoapAppUi::StaticRoapDone()
// ----------------------------------------------------------
//
TInt CRoapAppUi::StaticRoapDone( TAny* aPtr )
    {
    STATIC_CAST( CRoapAppUi*, aPtr )->RoapDone();
    return EFalse;  // TCallback should return EFalse as TInt.
    }
    
// ----------------------------------------------------------
// CRoapAppUi::RoapDocument()
// ----------------------------------------------------------
//
CRoapAppDocument* CRoapAppUi::RoapDocument()
    {
    return STATIC_CAST( CRoapAppDocument*, Document() );
    }

// ----------------------------------------------------------
// CRoapAppUi::HandleDialogCommandL()
// ----------------------------------------------------------
//
void CRoapAppUi::HandleDialogCommandL( TInt aCommand )
    {
    CLOG(( 2, _L("-> CRoapAppUi::HandleDialogCommandL aCommand(%d)"), \
        aCommand ));
    HandleCommandL( aCommand );
    CLOG(( 2, _L("<- CRoapAppUi::HandleDialogCommandL") ));
    }

// ----------------------------------------------------------
// CRoapAppUi::InfoNoteL()
// ----------------------------------------------------------
//
void CRoapAppUi::InfoNoteL( TInt aPromptResourceId )
    {
    HBufC* prompt = iCoeEnv->AllocReadResourceLC( aPromptResourceId );
    CAknInformationNote* note = new (ELeave) CAknInformationNote( ETrue );
    note->ExecuteLD( *prompt );
    CleanupStack::PopAndDestroy( prompt );
    }

// ----------------------------------------------------------
// CRoapAppUi::InfoNoteL()
// ----------------------------------------------------------
//
void CRoapAppUi::InfoNoteL( const TDesC& aPrompt )
    {
    CAknInformationNote* note = new (ELeave) CAknInformationNote( ETrue );
    note->ExecuteLD( aPrompt );
    }

// ----------------------------------------------------------
// CRoapAppUi::ConfNoteL()
// ----------------------------------------------------------
//
void CRoapAppUi::ConfNoteL( TInt aPromptResourceId )
    {
    HBufC* prompt = iCoeEnv->AllocReadResourceLC( aPromptResourceId );
    CAknConfirmationNote* note = new (ELeave) CAknConfirmationNote( ETrue );
    note->ExecuteLD( *prompt );
    CleanupStack::PopAndDestroy( prompt );
    }

// ----------------------------------------------------------
// CRoapAppUi::ConfNoteL()
// ----------------------------------------------------------
//
void CRoapAppUi::ConfNoteL( const TDesC& aPrompt )
    {
    CAknConfirmationNote* note = new (ELeave) CAknConfirmationNote( ETrue );
    note->ExecuteLD( aPrompt );
    }

// ----------------------------------------------------------
// CRoapAppUi::CanOpenL()
// ----------------------------------------------------------
//
TBool CRoapAppUi::CanOpenL( RFile& aFile ) const
    {
    CLOG(( 2, _L("-> CRoapAppUi::CanOpenL") ));
    TBool canOpen( EFalse );
    using namespace ContentAccess;
    TInt err;
    CData* data = NULL;
    // 'No right' style errors leave, need to TRAP.
    TRAP( err, data = CData::NewL( aFile, KDefaultContentObject, EPeek ) );
    // Note, 'data' not pushed. No leaving calls below.
    if ( !err &&
                 (
                 !data->EvaluateIntent( EPlay ) ||
                 !data->EvaluateIntent( EView ) ||
                 !data->EvaluateIntent( EExecute )
                 )
       )
        {
        canOpen = ETrue;
        }
    delete data;
    CLOG(( 2, _L("<- CRoapAppUi::CanOpenL (%d)"), canOpen ));
    return canOpen;
    }

// ----------------------------------------------------------
// CRoapAppUi::HandleOnePassRoL()
// ----------------------------------------------------------
//    
void CRoapAppUi::HandleOnePassRoL(TInt aError ) 
    {
    iCallback->iStatus = aError;
    RoapDone();
    }


// ----------------------------------------------------------
// CRoapAppUi::HandleDMgrEventL()
// ----------------------------------------------------------
//    
void CRoapAppUi::HandleDMgrEventL( RHttpDownload& aDownload, THttpDownloadEvent aEvent )
    {
    
    CLOG(( 2, _L("-> CRoapAppUi::HandleDMgrEventL") ));
    
   	
	if(EHttpDlCompleted == aEvent.iDownloadState)
		{
        HBufC8* contentType = HBufC8::NewLC(KMaxContentTypeLength);
        TPtr8 contentTypePtr = contentType->Des(); 
        User::LeaveIfError( aDownload.GetStringAttribute( EDlAttrContentType, contentTypePtr ) );
        TBool isRoapTriggerType = !contentType->Compare(KRoapMimeType);  
		CleanupStack::PopAndDestroy( contentType );
		
        HBufC* fileName = HBufC::NewLC( KMaxPath );
        TPtr fileNamePtr = fileName->Des();
        User::LeaveIfError( aDownload.GetStringAttribute( EDlAttrDestFilename, fileNamePtr ) );

		if(isRoapTriggerType)
			{
			OpenFileL(fileNamePtr);
			CleanupStack::PopAndDestroy( fileName );
			return;
			}
		CleanupStack::PopAndDestroy( fileName );
		iPostResponseURLDownLoad = EFalse;	
		RoapDone();		
		}
	else if( EHttpDlFailed == aEvent.iDownloadState || EHttpDlPaused == aEvent.iDownloadState )
		{
		iCallback->iStatus = KErrRoapGeneral;
		iPostResponseURLDownLoad = EFalse;	
		RoapDone();		
		}
		
	CLOG(( 2, _L("<- CRoapAppUi::HandleDMgrEventL") ));
    }

