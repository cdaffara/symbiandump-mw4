/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of class CCodAppUi.
*
*/


#include <avkon.hrh>
#include <eikbtgpc.h>
#include <eikapp.h>
#include <apgcli.h>
#include <CodUi.rsg>
#include <avkon.rsg>
#include <AknQueryDialog.h>
#include <ErrorUI.h>
#include <textresolver.h>
#include <AiwGenericParam.h>
#include <DocumentHandler.h>
#include <DcfRep.h>
#include <RoapDef.h>
#include <DRMCommon.h>
#include <StringLoader.h>
#include <CodEng.h>
#include <CodData.h>
#include <CodError.h>
#include <CodStatus.h>
#include <CodUtil.h>
#include <RoapData.h>
#include "CodAppUi.h"
#include "CodView.h" 
#include "CodDocument.h" 
#include "CodWaitDialog.h" 
#include "CodUi.hrh"
#include "CodUiPanic.h"
#include "RequestCompleteCallback.h"
#include "CodUiResource.h"
#include "CodLogger.h"
#include "CodDialog.h"

// ================= CONSTANTS =======================

/// UID of Wml Browser.
LOCAL_D const TInt KCodWmlBrowserUid = 0x10008D39;

/// Message prefix, to launch browser with URL.
_LIT( KCodMsgLaunchBrowserWithUrl, "4 " );

/// Reserved word "local".
_LIT8( KCodReservedWordLocal, "local" );

/// Reserved word "back".
_LIT8( KCodReservedWordBack, "back" );

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------
// CCodAppUi::CodDocument()
// ----------------------------------------------------------
//
CCodDocument* CCodAppUi::CodDocument()
    {
    // Inline method must be defined before it is used.
    return STATIC_CAST( CCodDocument*, Document() );
    }

// ----------------------------------------------------------
// CCodAppUi::CCodAppUi()
// ----------------------------------------------------------
//
CCodAppUi::CCodAppUi( TBool aCod )
: iCod( aCod ),
  iCurrentCba( R_AVKON_SOFTKEYS_BACK ),
  iError( KErrNone ),
  iSilentMode( EFalse ),
  iSuppressNextUrl( EFalse ),
  iSuppressLaunch( EFalse ),
  iRoap( EFalse )
    {
    CLOG(( 2, _L("*** CCodAppUi::CCodAppUi") ));
    }

// ----------------------------------------------------------
// CCodAppUi::ConstructL()
// ----------------------------------------------------------
//
void CCodAppUi::ConstructL()
    {
    iCodResource = CCodUiResource::NewL( *iEikonEnv );
    BaseConstructL( EAknEnableMSK );
    CodDocument()->Model().SetObserver( this );    
    iView = new (ELeave) CCodView;
    iView->SetMopParent( this );
    iView->ConstructL();
    iView->SetRect( ClientRect() );
    iView->ActivateL();
    iCallback = new ( ELeave ) CRequestCompleteCallback();
    iErrorUi = CErrorUI::NewL( *iCoeEnv );
    HandleModelChangeL();   // Initialize view with data.
    }

// ----------------------------------------------------------
// CCodAppUi::~CCodAppUi()
// ----------------------------------------------------------
//
CCodAppUi::~CCodAppUi()
    {
    CLOG(( 2, _L("*** -> CCodAppUi::~CCodAppUi") ));
    delete iDocHandler;
    delete iDocParams;
    // First of all NULL out observer to avoid further callbacks.
    CodDocument()->Model().SetObserver( NULL );
    // We must Cancel the model before deleting the callback.
    CodDocument()->Model().Cancel();
    delete iCallback;
    delete iErrorUi;
    delete iWaitNote;
    delete iView;
    if ( iDoorObserver )
        {
        iDoorObserver->NotifyExit( MApaEmbeddedDocObserver::ENoChanges );
        }
    delete iCodResource;
    iFs.Close();
    CLOG(( 2, _L("<- CCodAppUi::~CCodAppUi") ));
    }

// ----------------------------------------------------------
// CCodAppUi::OpenFileDone()
// ----------------------------------------------------------
//
void CCodAppUi::OpenFileDone( TInt aError )
    {
    CLOG(( 2, _L("-> CCodAppUi::OpenFileDone aError(%d)"), aError ));
    iError = aError;
    TRAPD( err, OpenFileDoneL() );
    if ( err )
        {
        RunAppShutter();
        }
    CLOG(( 2, _L("<- CCodAppUi::OpenFileDone") ));
    }

// ----------------------------------------------------------
// CCodAppUi::SetServiceFlow()
// ----------------------------------------------------------
//
void CCodAppUi::SetServiceFlow
        (
        TBool aSilentMode,
        TBool aSuppressNextUrl,
        TBool aSuppressLaunch
        )
    {
    CLOG(( 2, \
        _L("CCodAppUi::SetServiceFlow silent(%d) noUrl(%d) noLaunch(%d)"), \
        aSilentMode, aSuppressNextUrl, aSuppressLaunch ));
    iSilentMode = aSilentMode;
    iSuppressNextUrl = aSuppressNextUrl;
    iSuppressLaunch = aSuppressLaunch;
    }

// ----------------------------------------------------------
// CCodAppUi::HandleModelChangeL()
// ----------------------------------------------------------
//
void CCodAppUi::HandleModelChangeL()
    {
    CLOG(( 2, _L("-> CCodAppUi::HandleModelChangeL") ));
    TInt commandSet( R_AVKON_SOFTKEYS_BACK );
    const CCodData& data = CodDocument()->Model().Data();
    if ( data.IsValid() )
        {
        // Valid data: show details, show buttons "Accept" / "Reject".
        iView->SetDataL( data, iCod );
        commandSet = R_COD_UI_CBA_ACCEPT_REJECT;
        }
    else
        {
        // Invalid data: don't show details, show button "Back".
        iView->Clear();
        commandSet = R_AVKON_SOFTKEYS_BACK;
        }
    SetCbaL( commandSet );
    CLOG(( 2, _L("<- CCodAppUi::HandleModelChangeL") ));
    }

// ----------------------------------------------------------
// CCodAppUi::OpenFileL()
// ----------------------------------------------------------
//
void CCodAppUi::OpenFileL( const TDesC& aFileName )
    {
    CLOG(( 2, _L("-> CCodAppUi::OpenFileL aFileName<%S>"), &aFileName ));
    Document()->OpenFileL( ETrue, aFileName, iCoeEnv->FsSession() );
    CLOG(( 2, _L("<- CCodAppUi::OpenFileL") ));
    }

// ----------------------------------------------------------
// CCodAppUi::HandleCommandL()
// ----------------------------------------------------------
//
void CCodAppUi::HandleCommandL( TInt aCommand )
    {
    CLOG(( 2, _L("-> CCodAppUi::HandleCommandL aCommand(%d)"), \
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

        case ECodUiCmdAccept:
            {
            // Safety code: ignore commands until completion of previous.
            // Quite unlikely to happen as we don't have this CBA.
            if( !CodDocument()->Model().IsActive() )
                {
                SetCbaL( R_AVKON_SOFTKEYS_EMPTY );
                CodDocument()->Model().Accept( &iCallback->iStatus );
                // Exit after processing is done.
                iCallback->CallbackOnCompletion
                    ( TCallBack( StaticLoadDone, this ) );
                }
            break;
            }

        case ECodUiCmdReject:
            {
            // Safety code: ignore commands until completion of previous.
            // Quite unlikely to happen as we don't have this CBA.
            if( !CodDocument()->Model().IsActive() )
                {
                SetCbaL( R_AVKON_SOFTKEYS_EMPTY );
                CodDocument()->Model().Reject( &iCallback->iStatus );
                iCallback->CallbackOnCompletion
                    ( TCallBack( StaticLoadDone, this ) );
                }
            break;
            }

        case ECodUiCmdCancel:
            {
            CodDocument()->Model().Stop();
            break;
            }

        default:
            {
            CLOG(( 0, _L("  unknown command") ));
            __ASSERT_DEBUG
                ( EFalse, CodUiPanic( ECodUiUnknownCommand ) );
            break;
            }
        }
    CLOG(( 2, _L("<- CCodAppUi::HandleCommandL") ));
    }

// ----------------------------------------------------------
// CCodAppUi::ProcessCommandParametersL()
// ----------------------------------------------------------
//
TBool CCodAppUi::ProcessCommandParametersL
( TApaCommand aCommand, TFileName& /*aDocumentName*/, const TDesC8& /*aTail*/ )
    {
    if ( aCommand == EApaCommandOpen )
        {
        CLOG(( 2, _L("CCodAppUi::ProcessCommandParametersL: EApaCommandOpen - noop") ));
        // Do not handle this here - CAiwGenericParamList not ready
        //OpenFileL( aDocumentName );
        }
    return EFalse;
    }

// ----------------------------------------------------------
// CCodAppUi::HandleKeyEventL()
// ----------------------------------------------------------
//
TKeyResponse CCodAppUi::HandleKeyEventL
( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    CLOG(( 2, _L("-> CCodAppUi::HandleKeyEventL") ));
    CLOG(( 3, _L("  aKeyEvent.iCode(%d)"), aKeyEvent.iCode ));
    CLOG(( 3, _L("  aKeyEvent.iScanCode(%d)"), aKeyEvent.iScanCode ));
    CLOG(( 3, _L("  aKeyEvent.iModifiers(%d)"), aKeyEvent.iModifiers ));
    CLOG(( 3, _L("  aKeyEvent.iRepeats(%d)"), aKeyEvent.iRepeats ));
    CLOG(( 3, _L("  aType(%d)"), aType ));
    TKeyResponse result = EKeyWasNotConsumed;
    if (
        ( aKeyEvent.iCode == EKeyOK || aKeyEvent.iCode == EKeyEnter ) &&
        ( !(aKeyEvent.iModifiers & EModifierShift) ) &&
        ( iCurrentCba == R_COD_UI_CBA_ACCEPT_REJECT )
       )
        {
        // Selection key press is same as Accept softkey (provided we have
        // the Accept softkey).
        HandleCommandL( ECodUiCmdAccept );
        result = EKeyWasConsumed;
        }
    else if ( aKeyEvent.iCode == EKeyPhoneEnd &&
              iCurrentCba == R_COD_UI_CBA_ACCEPT_REJECT )
        {
        // Red key press -> Reject.
        HandleCommandL( ECodUiCmdReject );
        result = EKeyWasConsumed;
        }
    else
        {
        result = iView->OfferKeyEventL( aKeyEvent, aType );
        }
    CLOG(( 2, _L("<- CCodAppUi::HandleKeyEventL returns (0x%x)"), \
                                                                    result ));
    return result;
    }

// ----------------------------------------------------------
// CCodAppUi::HandleResourceChangeL()
// ----------------------------------------------------------
//
void CCodAppUi::HandleResourceChangeL( TInt aType )
    {
    CAknAppUi::HandleResourceChangeL( aType );
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        iView->SetRect( ClientRect() );
        }
    iView->HandleResourceChange( aType );
    }

// ----------------------------------------------------------
// CCodAppUi::GetRootPathL()
// ----------------------------------------------------------
//
void CCodAppUi::GetRootPathL( TDes& aRootPath )
    {
    CLOG(( 2, _L("CCodAppUi::GetRootPathL") ));
    CodDialog::GetRootPathL( aRootPath );
    }

// ----------------------------------------------------------
// CCodAppUi::StartLoadL()
// ----------------------------------------------------------
//
void CCodAppUi::StartLoadL( TInt aStatusCode )
    {
    CLOG(( 2, _L("-> CCodAppUi::StartLoadL aStatusCode(%d)"), \
        aStatusCode ));
    __ASSERT_DEBUG( !iWaitNote, CodUiPanic( ECodUiInternal ) );

    TInt resId = R_QTN_CD_WAIT_PLEASE_WAIT;
    if ( aStatusCode == KHttp900Success )
        {
        resId = R_QTN_CD_WAIT_DOWNLOADING;
        }
    else if ( aStatusCode == KHttp921UserAborted )
        {
        resId = R_QTN_CD_WAIT_REJECT;
        }
    iWaitNote = new (ELeave) CCodWaitDialog
        ( REINTERPRET_CAST( CEikDialog**, &iWaitNote ), *this );
    iWaitNote->PrepareLC( R_COD_UI_WAIT_NOTE );
    iWaitNote->SetTextL( resId );
    iWaitNote->RunLD();
    CLOG(( 2, _L("<- CCodAppUi::StartLoadL") ));
    }

// ----------------------------------------------------------
// CCodAppUi::StartCancelL()
// ----------------------------------------------------------
//
void CCodAppUi::StartCancelL()
    {
    CLOG(( 2, _L("-> CCodAppUi::StartCancelL") ));
    __ASSERT_DEBUG( iWaitNote, CodUiPanic( ECodUiInternal ) );
    iWaitNote->SetTextL( R_QTN_CD_WAIT_REJECT );
    CLOG(( 2, _L("<- CCodAppUi::StartCancelL") ));
    }

// ----------------------------------------------------------
// CCodAppUi::ProgressL()
// ----------------------------------------------------------
//
void CCodAppUi::ProgressL( TInt /*aFinalValue*/, TInt /*aCurrentValue*/ )
    {
    // TODO
    }

// ----------------------------------------------------------
// CCodAppUi::Done()
// ----------------------------------------------------------
//
void CCodAppUi::Done( TInt LOG_ONLY( aStatusCode ), TInt LOG_ONLY( aError ) )
    {
    CLOG(( 2, _L("-> CCodAppUi::Done aStatusCode(%d) aError(%d)"), \
        aStatusCode, aError ));
    delete iWaitNote;
    iWaitNote = NULL;
    CLOG(( 2, _L("<- CCodAppUi::Done") ));
    }

// ----------------------------------------------------------
// CCodAppUi::ConfirmConnectL()
// ----------------------------------------------------------
//
TBool CCodAppUi::ConfirmConnectL()
    {
    CLOG(( 2, _L("CCodAppUi::ConfirmConnectL") ));
    return CodDialog::ConfirmConnectL();
    }

// ----------------------------------------------------------
// CCodAppUi::StartConnect()
// ----------------------------------------------------------
//
void CCodAppUi::StartConnect( TUint32 LOG_ONLY( aIap ) )
    {
    CLOG(( 2, _L("CCodAppUi::StartConnect aIap(%d)"), aIap ));
    }

// ----------------------------------------------------------
// CCodAppUi::EndConnect()
// ----------------------------------------------------------
//
void CCodAppUi::EndConnect()
    {
    CLOG(( 2, _L("CCodAppUi::EndConnect") ));
    }

// ----------------------------------------------------------
// CCodAppUi::WaitForLicenseL()
// ----------------------------------------------------------
//
void CCodAppUi::WaitForLicenseL()
    {
    CLOG(( 2, _L("CCodAppUi::WaitForLicenseL") ));
    }


// ----------------------------------------------------------
// CCodAppUi::UserAuthL()
// ----------------------------------------------------------
//
TBool CCodAppUi::UserAuthL
        (
        const TDesC& LOG_ONLY( aHost ),
        const TDesC& LOG_ONLY( aRealm ),
        TBool LOG_ONLY( aProxyAuth ),
        TDes& aUsername,
        TDes& aPassword
        )
    {
    CLOG(( 2, _L("CCodAppUi::UserAuthL") ));
    CLOG(( 3, _L("  aHost<%S>"), &aHost ));
    CLOG(( 3, _L("  aRealm<%S>"), &aRealm ));
    CLOG(( 3, _L("  aProxyAuth(0x%x)"), aProxyAuth ));
    // Feature dropped - empty implementation.
    aUsername.Zero();
    aPassword.Zero();
    return ETrue;
    }

// ----------------------------------------------------------
// CCodAppUi::RoapTriggerParsedL()
// ----------------------------------------------------------
//
void CCodAppUi::RoapTriggerParsedL( const CRoapData& aData )
    {
    CLOG(( 2, _L("-> CCodAppUi::RoapTriggerParsedL") ));
    iRoap = ETrue;
    if ( aData.iContextStatus == Roap::EInvalidContext )
        {
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
            User::Leave( KErrCancel );
            }
        }
    // Update the wait note with ROAP-specific text.
    TInt resId = 0;
    switch ( aData.iType )
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
            if ( aData.iDomainOperation == Roap::EJoinDomainOperation )
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
        default:
            {
            User::Leave( KErrNotSupported );
            break;
            }
        }
    __ASSERT_DEBUG( resId, CodUiPanic( ECodUiInternal ) );
    TBool newNote( EFalse );
    if ( !iWaitNote )
        {
        iWaitNote = new (ELeave) CCodWaitDialog
            ( REINTERPRET_CAST( CEikDialog**, &iWaitNote ), *this );
        iWaitNote->PrepareLC( R_COD_UI_WAIT_NOTE );
        }
    iWaitNote->SetTextL( resId );
    if ( newNote )
        {
        iWaitNote->RunLD();
        }
    CLOG(( 2, _L("<- CCodAppUi::RoapTriggerParsedL") ));
    }

// ----------------------------------------------------------
// CCodAppUi::HandleDialogCommandL()
// ----------------------------------------------------------
//
void CCodAppUi::HandleDialogCommandL( TInt aCommand )
    {
    CLOG(( 2, _L("-> CCodAppUi::HandleDialogCommandL aCommand(%d)"), \
        aCommand ));
    HandleCommandL( aCommand );
    CLOG(( 2, _L("<- CCodAppUi::HandleDialogCommandL") ));
    }

// ----------------------------------------------------------
// CCodAppUi::HandleServerAppExit()
// ----------------------------------------------------------
//
void CCodAppUi::HandleServerAppExit( TInt /*aReason*/ )
    {
    CLOG(( 2, _L("-> CCodAppUi::HandleServerAppExit") ));
    TRAP_IGNORE( ActivateNextUrlL() );
    RunAppShutter();
    CLOG(( 2, _L("<- CCodAppUi::HandleServerAppExit") ));
    }

// ----------------------------------------------------------
// CCodAppUi::OpenFileDoneL()
// ----------------------------------------------------------
//
void CCodAppUi::OpenFileDoneL()
    {
    CLOG(( 2, _L("-> CCodAppUi::OpenFileDoneL") ));
    if ( iError )
        {
        LoadDoneL();
        }
    else
        {
        HandleModelChangeL();
        }
    CLOG(( 2, _L("<- CCodAppUi::OpenFileDoneL") ));
    }

// ----------------------------------------------------------
// CCodAppUi::StaticLoadDone()
// ----------------------------------------------------------
//
TInt CCodAppUi::StaticLoadDone( TAny* aPtr )
    {
    STATIC_CAST( CCodAppUi*, aPtr )->LoadDone();
    return EFalse;  // TCallback should return EFalse as TInt.
    }

// ----------------------------------------------------------
// CCodAppUi::LoadDone()
// ----------------------------------------------------------
//
void CCodAppUi::LoadDone()
    {
    CLOG(( 2, _L("-> CCodAppUi::LoadDone") ));
    iError = iCallback->iStatus.Int();
    TRAPD( err, LoadDoneL() );
    if ( err )
        {
        RunAppShutter();
        }
    CLOG(( 2, _L("<- CCodAppUi::LoadDone") ));
    }

// ----------------------------------------------------------
// CCodAppUi::LoadDoneL()
// ----------------------------------------------------------
//
void CCodAppUi::LoadDoneL()
    {
    CLOG(( 2, _L("-> CCodAppUi::LoadDoneL iError(%d)"), iError ));
    TBool exitApp( ETrue );
    if ( CodDocument()->Model().GetPath().Length() )
        {
        // ROAP involved loading a file, treat as download and not as ROAP.
        iRoap = EFalse;
        }
    // No service flow for Cancel or Reject.
    if ( iError != KErrCancel && iError != KErrAbort )
        {
        if ( iRoap )
            {
            if ( iError )
                {
                exitApp = RoapFailedL();
                }
            else
                {
                exitApp = RoapOkL();
                }
            }
        else
            {
            if ( iError )
                {
                exitApp = LoadFailedL();
                }
            else
                {
                exitApp = LoadOkL();
                }
            }
        if ( exitApp )
            {
            ActivateNextUrlL();
            }
        }
    if ( exitApp )
        {
        RunAppShutter();
        }
    CLOG(( 2, _L("<- CCodAppUi::LoadDoneL") ));
    }

// ----------------------------------------------------------
// CCodAppUi::LoadOkL()
// ----------------------------------------------------------
//
TBool CCodAppUi::LoadOkL()
    {
    CLOG(( 2, _L("-> CCodAppUi::LoadOkL") ));
    __ASSERT_DEBUG( !iRoap, CodUiPanic( ECodUiInternal ) );
    __ASSERT_DEBUG( !iError, CodUiPanic( ECodUiInternal ) );
    TBool exitApp( ETrue ); // Exit by default.
    // Show "File saved to..." note.
    TUid handler = CodDocument()->Model().GetHandler();
    RApaLsSession apaLsSess;
    User::LeaveIfError( apaLsSess.Connect() );
    CleanupClosePushL<RApaLsSession>( apaLsSess );
    TApaAppInfo appInfo;
    User::LeaveIfError( apaLsSess.GetAppInfo( appInfo, handler ) );
    CleanupStack::PopAndDestroy();  // apaLsSess 
    HBufC* prompt = StringLoader::LoadLC
        ( R_QTN_FLDR_FILE_SAVED_TO, appInfo.iCaption, iCoeEnv );
    CodDialog::ConfNoteL( *prompt );
    CleanupStack::PopAndDestroy( prompt );
    // Launch content, if applicable (DD or 'local' COD).
    if ( !iCod || !CodDocument()->Model().Data().NextUrl().Compare
                                            ( KCodReservedWordLocal ) )
        {
        exitApp = LaunchContentL( CodDocument()->Model().GetPath(),
                                CodDocument()->Model().GetType().Des8() );
        }
    CLOG(( 2, _L("<- CCodAppUi::LoadOkL (%d)"), exitApp ));
    return exitApp;
    }

// ----------------------------------------------------------
// CCodAppUi::LoadFailedL()
// ----------------------------------------------------------
//
TBool CCodAppUi::LoadFailedL()
    {
    CLOG(( 2, _L("-> CCodAppUi::LoadFailedL") ));
    __ASSERT_DEBUG( !iRoap, CodUiPanic( ECodUiInternal ) );
    __ASSERT_DEBUG( iError, CodUiPanic( ECodUiInternal ) );
    TInt resId = 0;
    if ( iError == KErrCodNoAccessPoint )
        {
        // "No access point" error has the same status code as all connection
        // related errors, but needs different note.
        resId = R_QTN_CD_INFO_NO_VALID_AP;
        }
    else
        {
        // Investigating status code (which carries less information than the
        // error code) is generally enough to get the correct note.
        switch( CodDocument()->Model().GetStatusCode() )
            {
            case KHttp900Success:
            case KHttp902UserCancelled:
            case KHttp921UserAborted:
                {
                // No error note shown for user cancel.
                // (Sanity code, we never get here for user Cancel.)
                iError = KErrNone;
                break;
                }
                
            case KHttp910NoMemory:
            case KHttp901InsufficientMemory:
                {
                resId = R_QTN_CD_INFO_NOT_ENOUGH_MEM;
                break;
                }

            case KHttp903LossOfService:
                {
                // User Text Resolver to get "Connection timeout".
                iError = KErrCodHttpNoResponse;
                break;
                }

            case KHttp906InvalidDescriptor:
                {
                resId = R_QTN_CD_INFO_SYNTAX_ERR;
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
                __ASSERT_DEBUG
                    ( EFalse, CodUiPanic( ECodUiUnknownStatus ) );
                break;
                }
            }
        }

    if ( resId )
        {
        CodDialog::InfoNoteL( resId, *iCoeEnv );
        }
    else if ( iError )
        {
        TPtrC msg( iErrorUi->TextResolver().ResolveErrorString( iError ) );
        CodDialog::InfoNoteL( msg );
        }
    CLOG(( 2, _L("<- CCodAppUi::LoadFailedL") ));
    return ETrue;
    }

// ----------------------------------------------------------
// CCodAppUi::RoapOkL()
// ----------------------------------------------------------
//
TBool CCodAppUi::RoapOkL()
    {
    CLOG(( 2, _L("-> CCodAppUi::RoapOkL") ));
    __ASSERT_DEBUG( iRoap, CodUiPanic( ECodUiInternal ) );
    __ASSERT_DEBUG( !iError, CodUiPanic( ECodUiInternal ) );
    TBool exitApp( ETrue );
    const CRoapData* roapData = CodDocument()->Model().GetRoapData();
    if ( roapData ) // Safety code.
        {
        switch( roapData->iType )
            {
            case Roap::ERegistrationTrigger:
                {
                CodDialog::ConfNoteL
                    ( R_QTN_DRM_CONF_DEVICE_REGISTERED, *iCoeEnv );
                break;
                }
            case Roap::ERoAcquisitionTrigger:
                {
                exitApp = RoAcqOkL();
                break;
                }
            case Roap::EJoinDomainTrigger:
                {
                if ( roapData->iDomainOperation == Roap::EJoinDomainOperation )
                    {
                    CodDialog::ConfNoteL
                        ( R_QTN_DRM_CONF_JOINED_TO_DOMAIN, *iCoeEnv );
                    }
                else
                    {
                    CodDialog::ConfNoteL
                        ( R_QTN_DRM_CONF_ACCOUNT_UPDATED, *iCoeEnv );
                    }
                break;
                }
            case Roap::ELeaveDomainTrigger:
                {
                CodDialog::ConfNoteL
                    ( R_QTN_DRM_INFO_REMOVE_FR_ACCOUNT, *iCoeEnv );
                break;
                }
            default:
                {
                // We should never get here.
                __ASSERT_DEBUG( EFalse, CodUiPanic( ECodUiInternal ) );
                }
            }
        }
    CLOG(( 2, _L("<- CCodAppUi::RoapOkL returns(%d)"), exitApp ));
    return exitApp;
    }

// ----------------------------------------------------------
// CCodAppUi::RoAcqOkL()
// ----------------------------------------------------------
//
TBool CCodAppUi::RoAcqOkL()
    {
    CLOG(( 2, _L("-> CCodAppUi::RoAcqOkL") ));
    const CRoapData* roapData = CodDocument()->Model().GetRoapData();
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
        CleanupStack::PopAndDestroy( dcfRep );
        }

    TBool exitApp( ETrue );
    if ( appUid.iUid )
        {
        // MO (related to the received RO) is already here and is supported
        // by the system. Ask user if he wants to open it now.
        __ASSERT_DEBUG( iFs.Handle(), CodUiPanic( ECodUiInternal ) );
        __ASSERT_DEBUG( f.SubSessionHandle(), CodUiPanic( ECodUiInternal ) );
        __ASSERT_DEBUG( name.Length(), CodUiPanic( ECodUiInternal ) );
        CAknQueryDialog* dialog = CAknQueryDialog::NewL();
        dialog->PrepareLC( R_COD_UI_CONFIRM_NOTE );
        HBufC* prompt = StringLoader::LoadLC
            ( R_QTN_DRM_QUERY_KEY_RECEIVED_OPEN, name, iEikonEnv );
        dialog->SetPromptL( *prompt );
        CleanupStack::PopAndDestroy( prompt );
        if ( dialog->RunLD() ) 
            {
            __ASSERT_DEBUG( !iDocHandler, CodUiPanic( ECodUiInternal ) );
            __ASSERT_DEBUG( !iDocParams, CodUiPanic( ECodUiInternal ) );
            iDocHandler = CDocumentHandler::NewL();
            iDocHandler->SetExitObserver( this );
            iDocParams = CAiwGenericParamList::NewL();
            User::LeaveIfError
                ( iDocHandler->OpenFileEmbeddedL( f, type, *iDocParams ) );
            exitApp = EFalse;
            }
        }
    else
        {
        CodDialog::ConfNoteL( R_QTN_DRM_INFO_ACT_KEY_RECEIVED, *iCoeEnv );
        }
    CleanupStack::PopAndDestroy( &f );
    CLOG(( 2, _L("<- CCodAppUi::RoAcqOkL returns(%d)"), exitApp ));
    return exitApp;
    }

// ----------------------------------------------------------
// CCodAppUi::RoapFailedL()
// ----------------------------------------------------------
//
TBool CCodAppUi::RoapFailedL()
    {
    CLOG(( 2, _L("-> CCodAppUi::RoapFailedL iError(%d)"), iError ));
    __ASSERT_DEBUG( iRoap, CodUiPanic( ECodUiInternal ) );
    __ASSERT_DEBUG( iError, CodUiPanic( ECodUiInternal ) );
    TBool exitApp( ETrue );
    TInt resId = 0;
    const CRoapData* roapData = CodDocument()->Model().GetRoapData();
    if ( roapData ) // Safety code
        {
        // Some errors need specific message.
        switch ( iError )
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
                        ( R_QTN_DRM_GET_KEY_FAILED_RETRY, iEikonEnv );
                    dialog->SetPromptL( *prompt );
                    CleanupStack::PopAndDestroy( prompt );
                    if ( dialog->RunLD() ) 
                        {
                        HandleCommandL( ECodUiCmdAccept );
                        exitApp = EFalse;
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
            CodDialog::InfoNoteL( resId, *iCoeEnv );
            }
        }
    CLOG(( 2, _L("<- CCodAppUi::RoapFailedL returns(%d)"), exitApp ));
    return exitApp;
    }

// ----------------------------------------------------------
// CCodAppUi::LaunchContentL()
// ----------------------------------------------------------
//
TBool CCodAppUi::LaunchContentL( const TDesC& aFname, const TDesC8& aType )
    {
    CLOG(( 2, _L("-> CCodAppUi::LaunchContentL") ));
    CLOG(( 3, _L("  aFile==<%S>"), &aFname ));
    CLOG(( 3, _L8("  aType==<%S>"), &aType ));
    TBool exitApp( ETrue ); // Exit by default.
    if ( !iSilentMode &&
         !iSuppressLaunch &&
         aFname.Length() &&
         CodUtil::CanOpenL( aFname )
       )
        {
        __ASSERT_DEBUG( !iDocHandler, CodUiPanic( ECodUiInternal ) );
        __ASSERT_DEBUG( !iDocParams, CodUiPanic( ECodUiInternal ) );
        iDocHandler = CDocumentHandler::NewL();
        iDocHandler->SetExitObserver( this );
        iDocParams = CAiwGenericParamList::NewL();
        // Need to copy argument; DocHandler takes non-const TDataType :-(
        TDataType type( aType );
        __ASSERT_DEBUG( !iFs.Handle(), CodUiPanic( ECodUiInternal ) );
        User::LeaveIfError( iFs.Connect() );
        User::LeaveIfError( iFs.ShareProtected() );
        RFile f;
        CleanupClosePushL<RFile>( f );
        User::LeaveIfError( f.Open( iFs, aFname, EFileShareReadersOnly ) );
        User::LeaveIfError
            ( iDocHandler->OpenFileEmbeddedL( f, type, *iDocParams ) );
        CleanupStack::PopAndDestroy( &f );
        exitApp = EFalse;
        }
    CLOG(( 2, _L("<- CCodAppUi::LaunchContentL (%d)"), exitApp ));
    return exitApp;
    }

// ----------------------------------------------------------
// CCodAppUi::ActivateNextUrlL()
// ----------------------------------------------------------
//
void CCodAppUi::ActivateNextUrlL()
    {
    CLOG(( 2, _L("-> CCodAppUi::ActivateNextUrlL") ));
    TPtrC8 url;
    TBool back( EFalse );

    if ( iSilentMode || iSuppressNextUrl )
        {
        CLOG(( 2, _L("<- CCodAppUi::ActivateNextUrlL (off)") ));
        return;
        }

    const CCodData& data = CodDocument()->Model().Data();
    
    if( iCod )
        {
        // This is a COD: choose between Next-URL or Next_URLatError.
        if ( CodDocument()->Model().GetStatusCode() == KHttp900Success )
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
                back = ETrue;
                }
            }
        }
    else
        {
        // This is a DD: NextURL goes for success and failure.
        url.Set( data.NextUrl() );
        }
            
    CLOG(( 2, _L8("  url<%S> back(%d)"), &url, back ));

    if ( url.Length() || back )
        {
        HBufC8* msg;
        if ( url.Length() )
            {
            // Activate URL in Browser.
            HBufC8* absUrl = CodUtil::AbsoluteUrlLC( data.SourceUri(), url );
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

        TApaTaskList taskList( iCoeEnv->WsSession() );
        TApaTask task = taskList.FindApp( TUid::Uid( KCodWmlBrowserUid ) );
        if ( task.Exists() )
            {
            // Browser is already running, send a message to it.
            // (If not running, we don't launch it.)
            task.SendMessage( TUid::Uid( 0 ), *msg );
            }

        CleanupStack::PopAndDestroy( msg );
        }
    CLOG(( 2, _L("<- CCodAppUi::ActivateNextUrlL") ));
    }

// ----------------------------------------------------------
// CCodAppUi::SetCbaL()
// ----------------------------------------------------------
//
void CCodAppUi::SetCbaL( TInt aCommandSetResourceId )
    {
    CLOG(( 2, _L("-> CCodAppUi::SetCbaL aCommandSetResourceId(%d)"), \
        aCommandSetResourceId ));
    CEikButtonGroupContainer* cba = Cba();
    if ( cba )
        {
        cba->SetCommandSetL( aCommandSetResourceId );
        iCurrentCba = aCommandSetResourceId;
        cba->DrawNow();
        }
    CLOG(( 2, _L("<- CCodAppUi::SetCbaL") ));
    }

// ----------------------------------------------------------
// CCodAppUi::PdPlayAvailable()
// ----------------------------------------------------------
//
void CCodAppUi::PdPlayAvailable()
    {
    CLOG(( 2, _L("-> CCodAppUi::PdPlayAvailable") ));

    CLOG(( 2, _L("<- CCodAppUi::PdPlayAvailable") ));
    }
