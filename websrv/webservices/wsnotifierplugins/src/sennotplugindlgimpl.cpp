/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:        
*
*/








// INCLUDE FILES
#include <aknnotewrappers.h>
#include <AknNotifyStd.h>
#include <ConeResLoader.h>
#include <StringLoader.h>

#ifndef RD_SEN_COMPILE_SIS_PACKAGE_FILES
    #include <SenNotifierPluginDlgImpl.rsg>
#else
    #include <WsNotifierPluginDlgImpl.rsg>
#endif

// This may be angle bracketed later..
#include "logger.h"
#include "sennotplugindlg.h" // client api
#include "sennotplugindlgimpl.h"

#include <implementationproxy.h>


#ifdef EKA2 // 3.0
  #ifndef RD_SEN_COMPILE_SIS_PACKAGE_FILES
    _LIT(KResourceFilePath, "\\private\\10003a4a\\SenNotifierPluginDlgImpl.RSC");
  #else
    _LIT(KResourceFilePath, "\\private\\10003a4a\\WsNotifierPluginDlgImpl.RSC");
  #endif
#else // not EKA2 (2.0, 2.1, 2.6, 2.8)
  #ifndef RD_SEN_COMPILE_SIS_PACKAGE_FILES
    _LIT(KResourceFilePath, "\\system\\notifiers\\SenNotifierPluginDlgImpl.RSC");
  #else
    _LIT(KResourceFilePath, "\\system\\notifiers\\WsNotifierPluginDlgImpl.RSC");
  #endif
#endif // EKA2

#ifdef EKA2
void CreateNotifierL(CArrayPtr<MEikSrvNotifierBase2>* aNotifiers)
    {
    MEikSrvNotifierBase2* notifier;
    notifier = CSenNotPluginDlgImpl::NewL();
    CleanupStack::PushL(notifier);
    aNotifiers->AppendL(notifier);
    CleanupStack::Pop(notifier);
    }
#else
void CreateNotifierL(CArrayPtr<MEikSrvNotifierBase>* aNotifiers)
    {
    MEikSrvNotifierBase* notifier;
    notifier = CSenNotPluginDlgImpl::NewL();
    CleanupStack::PushL(notifier);
    aNotifiers->AppendL(notifier);
    CleanupStack::Pop(notifier);
    }
#endif

#ifdef EKA2
EXPORT_C CArrayPtr<MEikSrvNotifierBase2>* NotifierArray()
    {
    DOLOG(_L("\n\n\n\n"));
    DOLOG(_L("[ Creating notifier array ]"));
    CArrayPtrFlat<MEikSrvNotifierBase2>* notifiers
        = new CArrayPtrFlat<MEikSrvNotifierBase2>(1);
    if (notifiers)
        {
        TRAPD(err, CreateNotifierL(notifiers));
        if(err)
            {
            notifiers->ResetAndDestroy();
            delete notifiers;
            notifiers = NULL;
            DOLOG(_L("[ Creating notifier array: Error. Returning NULL ]"));
            }
        }
    DOLOG(_L("[ Creating notifier array: Done ]"));
    return(notifiers);
    }
#else
EXPORT_C CArrayPtr<MEikSrvNotifierBase>* NotifierArray()
    {
    DOLOG(_L("\n\n\n\n"));
    DOLOG(_L("[ Creating notifier array ]"));
    CArrayPtrFlat<MEikSrvNotifierBase>* notifiers
        = new CArrayPtrFlat<MEikSrvNotifierBase>(1);
    if (notifiers)
        {
        TRAPD(err, CreateNotifierL(notifiers));
        if(err)
            {
            notifiers->ResetAndDestroy();
            delete notifiers;
            notifiers = NULL;
            DOLOG(_L("[ Creating notifier array: Error. Returning NULL ]"));
            }
        }
    DOLOG(_L("[ Creating notifier array: Done ]"));
    return(notifiers);
    }
#endif

CSenNotPluginDlgImpl* CSenNotPluginDlgImpl::NewL()
    {
    DOLOG(_L("[ CSenNotPluginDlgImpl::NewL: Starting ]"));
    CSenNotPluginDlgImpl* dlg = new(ELeave)CSenNotPluginDlgImpl();
    CleanupStack::PushL(dlg);
    dlg->ConstructL();
    CleanupStack::Pop(dlg);
    DOLOG(_L("[ CSenNotPluginDlgImpl::NewL: Done ]"));
    return dlg;
    }

CSenNotPluginDlgImpl::~CSenNotPluginDlgImpl()
    {
    Cancel();
    Cleanup();
    }

CSenNotPluginDlgImpl::CSenNotPluginDlgImpl() : CActive(EPriorityStandard),
    iMethod(TSenNotPluginRequest::ENoMethod)
    {
    CActiveScheduler::Add(this);
    }

void CSenNotPluginDlgImpl::ConstructL()
    {
    }

void CSenNotPluginDlgImpl::RunL()
    {
    if(iMethod == TSenNotPluginRequest::ENoMethod)
        return; // Notifier canceled or request signaled by other means

    HandleAsyncRequestL(); // show dialog
    Cleanup();
    }

TInt CSenNotPluginDlgImpl::RunError(TInt aError)
    {
    iMessage.Complete(aError);
    Cleanup();
    return KErrNone;
    }

void CSenNotPluginDlgImpl::DoCancel()
    {
    }

/**
 * Called when all resources allocated by notifiers shoudl be freed.
 */
void CSenNotPluginDlgImpl::Release()
    {
    DOLOG(_L("[ CSenNotPluginDlgImpl::Release: Deleting this ]"));
    delete this;
    }

/**
 * Called when a notifier is first loaded to allow any initial construction that is required.
 */
#ifdef EKA2
MEikSrvNotifierBase2::TNotifierInfo CSenNotPluginDlgImpl::RegisterL()
#else
MEikSrvNotifierBase::TNotifierInfo CSenNotPluginDlgImpl::RegisterL()
#endif
    {
    DOLOG(_L("[ CSenNotPluginDlgImpl::RegisterL ]"));
    iInfo.iUid = KSenNotifierPluginUID;
    //iInfo.iChannel = EAknNotifierChannelConfirmationQuery; //KNotifierOutputChannelScreen;
    iInfo.iChannel = TUid::Uid(7641863);
    iInfo.iPriority = ENotifierPriorityLow; //ENotifierPriorityVHigh;
    DOLOG(_L("[ CSenNotPluginDlgImpl::RegisterL: Done ]"));
    return iInfo;
    }

/**
 * Return the priority a notifier takes and the channels it acts on.  The return value may be varied
 * at run-time.
 */
#ifdef EKA2
MEikSrvNotifierBase2::TNotifierInfo CSenNotPluginDlgImpl::Info() const
#else
MEikSrvNotifierBase::TNotifierInfo CSenNotPluginDlgImpl::Info() const
#endif
    {
    DOLOG(_L("[ CSenNotPluginDlgImpl::Info ]"));
    return iInfo;
    }

/**
 * Start the notifier with data aBuffer and return an initial response.
 */
TPtrC8 CSenNotPluginDlgImpl::StartL(const TDesC8& /*aBuffer*/)
    {
    DOLOG(_L("[ CSenNotPluginDlgImpl::StartL(aBuffer): Not implemented ]"));
    //Create and launch confirmation dialog using static function.
    //The function returns True when the OK button is pressed.
    DOLOG(_L("[ CSenNotPluginDlgImpl::StartL(aBuffer): Done ]"));
    return TPtrC8(NULL, 0);
    }

void CSenNotPluginDlgImpl::HandleAsyncRequestL()
    {
    DOLOG(_L("[ CSenNotPluginDlgImpl::HandleAsyncRequestL: Starting ]"));

    // Load resource file
    CEikonEnv* eikEnv = CEikonEnv::Static();
    if(!eikEnv)
        {
        DOLOG(_L("[ CSenNotPluginDlgImpl::HandleAsyncRequestL: EikEnv is NULL ]"));
        }
    else
        {
        DOLOG(_L("[ CSenNotPluginDlgImpl::HandleAsyncRequestL: EikEnv is valid ]"));
        }

    // Figure out the resource file drive trusting that 
    // it must be the same drive from which the DLL was loaded.

    TParse parse;
    TFileName tempFileName;

    Dll::FileName(tempFileName);
    parse.SetNoWild(tempFileName, NULL, NULL);
    // We need to set the default path to be used for future file access, because
    // it is always initialised to c: and the voice control files may be stored
    // on the d: drive.

    TFileName filename;
    filename.Append(parse.Drive());
    filename.Append(KResourceFilePath);

//    TFileName filename(KResourceFile);


    DOLOG(filename);

    RConeResourceLoader resLoader(*eikEnv);
    if(resLoader.Open(filename) != KErrNone)
        {
        DOLOG(_L("[ CSenNotPluginDlgImpl::HandleAsyncRequestL: Failed to open resource file ]"));
        }
    else
        {
        DOLOG(_L("[ CSenNotPluginDlgImpl::HandleAsyncRequestL: Resource file opened OK. ]"));
        }

    CleanupClosePushL(resLoader);
    
    TInt result = KErrGeneral;
    switch(iMethod)
        {
        case TSenNotPluginRequest::EAuthenticationDlg:
            result = ShowAuthenticationDialogL((TAuthenticationDlgRequest*)&((*iRequest)()),
                (TAuthenticationDlgResponse*)&((*iResponse)()));
            break;
        case TSenNotPluginRequest::EAllowRegisterDlg:
            result = ShowAllowRegisterDialogL((TAllowRegisterDlgRequest*)&((*iRequest)()),
                (TAllowRegisterDlgResponse*)&((*iResponse)()));
            break;
        case TSenNotPluginRequest::EAllowUnRegisterDlg:
            result = ShowAllowUnRegisterDialogL((TAllowUnRegisterDlgRequest*)&((*iRequest)()),
                (TAllowUnRegisterDlgResponse*)&((*iResponse)()));
            break;
        case TSenNotPluginRequest::EAskSaveDlg:
            result = ShowAskSaveDialogL((TAskSaveDlgRequest*)&((*iRequest)()),
               (TAskSaveDlgResponse*)&((*iResponse)()));
            break;
        default:
            break;
        }
        
    if(iCancelled)
        {
        DOLOG(_L("[ CSenNotPluginDlgImpl::HandleAsyncRequestL: Operation had been cancelled. Completing with KErrCancel ]"));
        iMessage.Complete(KErrCancel);
        }
    else
        {
        if(result == KErrNone)
#ifdef EKA2
            iMessage.WriteL(iReplySlot, *iResponse);
#else
            iMessage.WriteL(iReplyPtr, *iResponse);
#endif            
        iMessage.Complete(result);
        }

    CleanupStack::PopAndDestroy(); // resLoader
    DOLOG(_L("[ CSenNotPluginDlgImpl::HandleAsyncRequestL: Done ]"));

    }

/**
 * Start the notifier with data aBuffer.  aMessage should be completed when the notifier is deactivated.
 * May be called multiple times if more than one client starts the notifier.  The notifier is immediately
 * responsible for completing aMessage.
 */
#ifdef EKA2
void CSenNotPluginDlgImpl::StartL(const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage)
#else
void CSenNotPluginDlgImpl::StartL(const TDesC8& aBuffer, const TAny* aReturnVal, RMessage aMessage)
#endif
    {
    DOLOG(_L("[ CSenNotPluginDlgImpl::StartL(aBuffer, aReturnVal, aMessage): Starting ]"));

    if(iMethod != TSenNotPluginRequest::ENoMethod)
        {
        DOLOG(_L("[ CSenNotPluginDlgImpl::StartL(aBuffer, aReturnVal, aMessage): Busy. Returning KErrInUse ]"));
        aMessage.Complete(KErrInUse);
        return;
        }

    Cleanup();

    iRequest = new(ELeave)TPckgBuf<TSenNotPluginRequest>();
    iRequest->Copy(aBuffer.Ptr(), sizeof(TSenNotPluginRequest));
    iResponse = new(ELeave)TPckgBuf<TSenNotPluginResponse>();

    iMethod = (*iRequest)().Method();
    iMessage = aMessage;    
#ifdef EKA2
    iReplySlot = aReplySlot;
#else
    iReplyPtr = aReturnVal;
#endif

    iStatus = KRequestPending;
    TRequestStatus* status = &iStatus;
    if(!IsActive())
    	SetActive();
    User::RequestComplete(status, KErrNone);

    DOLOG(_L("[ CSenNotPluginDlgImpl::StartL(aBuffer, aReturnVal, aMessage): Done ]"));
    }


TInt CSenNotPluginDlgImpl::ShowAuthenticationDialogL(TAuthenticationDlgRequest* aRequest,
    TAuthenticationDlgResponse* aResponse)
    {
    // Note: default password is currently unsupported!
    DOLOG(_L("[ CSenNotPluginDlgImpl::ShowAuthenticationDialogL: Starting ]"));

    TBuf<KSenMaxUsernameLength> username(aRequest->Username());
    TBuf<KSenMaxPasswordLength> password; // Empty

    DOLOG(_L("[ CSenNotPluginDlgImpl::ShowAuthenticationDialogL: Creating dialog ]"));
    CAknMultiLineDataQueryDialog* dlg = CAknMultiLineDataQueryDialog::NewL(username, password);
    iDialog = dlg;
    TRAPD(err,
        DOLOG(_L("[ CSenNotPluginDlgImpl::ShowAuthenticationDialogL: Showing dialog ]"));
        TBool okButtonSelected = iDialog->ExecuteLD(R_CUSTOM_USERNAMEPASSWD_DIALOG);
        if(okButtonSelected)
            {
            DOLOG(_L("[ CSenNotPluginDlgImpl::ShowAuthenticationDialogL: Ok button was selected ]"));
            aResponse->SetData(username, password, okButtonSelected);
            }
        else
            {
            DOLOG(_L("[ CSenNotPluginDlgImpl::ShowAuthenticationDialogL: Cancel button was selected ]"));
            aResponse->SetData(KNullDesC, KNullDesC, okButtonSelected);
            }
        )
    dlg = NULL;

#ifdef __CUSTOM_LOG_ENABLED__
    TBuf<128> errorBuf(_L("CSenNotPluginDlgImpl::ShowAuthenticationDialogL: Done with error code = "));
    errorBuf.AppendNum(err);
    DOLOG(errorBuf);
#endif

    return err;
    }

TInt CSenNotPluginDlgImpl::ShowAllowRegisterDialogL(TAllowRegisterDlgRequest* /*aRequest*/,
    TAllowRegisterDlgResponse* aResponse)
    {
    HBufC* prompt = StringLoader::LoadL(R_SENNOTDLG_ALLOW_REGISTER_QUERY);
    CleanupStack::PushL(prompt);
    CAknQueryDialog* dlg = CAknQueryDialog::NewL();
    CleanupStack::PushL( dlg ); 
    dlg->SetPromptL(*prompt);
    CleanupStack::Pop( dlg );  
    CleanupStack::PopAndDestroy(prompt);
    iDialog = dlg;
    TRAPD(err,
        if(dlg->ExecuteLD(R_COMMON_YES_NO_QUERY))
            {
            aResponse->SetOkButton(ETrue);
            }
        else
            {
            aResponse->SetOkButton(EFalse);
            }
        )
    iDialog = NULL;

    return err;
    }

TInt CSenNotPluginDlgImpl::ShowAllowUnRegisterDialogL(TAllowUnRegisterDlgRequest* /*aRequest*/,
    TAllowUnRegisterDlgResponse* aResponse)
    {
    HBufC* prompt = StringLoader::LoadL(R_SENNOTDLG_ALLOW_UNREGISTER_QUERY);
    CleanupStack::PushL(prompt);
    CAknQueryDialog* dlg = CAknQueryDialog::NewL();
    CleanupStack::PushL( dlg ); 
    dlg->SetPromptL(*prompt);
    CleanupStack::Pop( dlg ); 
    CleanupStack::PopAndDestroy(prompt);
    iDialog = dlg;
    TRAPD(err,
        if(dlg->ExecuteLD(R_COMMON_YES_NO_QUERY))
            {
            aResponse->SetOkButton(ETrue);
            }
        else
            {
            aResponse->SetOkButton(EFalse);
            }
        )
    iDialog = NULL;
    return err;
    }
    
TInt CSenNotPluginDlgImpl::ShowAskSaveDialogL(TAskSaveDlgRequest* /*aRequest*/,
    TAskSaveDlgResponse* aResponse)
    {
    HBufC* prompt = StringLoader::LoadL(R_SENNOTDLG_ASK_SAVE_QUERY);
    CleanupStack::PushL(prompt);
    CAknQueryDialog* dlg = CAknQueryDialog::NewL();
    CleanupStack::PushL( dlg ); 
    dlg->SetPromptL(*prompt);
    CleanupStack::Pop( dlg ); 
    CleanupStack::PopAndDestroy(prompt);
    iDialog = dlg;
    TRAPD(err,
        if(dlg->ExecuteLD(R_COMMON_YES_NO_QUERY))
            {
            aResponse->SetOkButton(ETrue);
            }
        else
            {
            aResponse->SetOkButton(EFalse);
            }
        )
    iDialog = NULL;
    return err;
    }


void CSenNotPluginDlgImpl::Cleanup()
    {
    iMethod = TSenNotPluginRequest::ENoMethod;
    iCancelled = EFalse;
#ifdef EKA2    
    iReplySlot = NULL;
#else
    iReplyPtr = NULL;
#endif    
    delete iRequest;
    iRequest = NULL;
    delete iResponse;
    iResponse = NULL;
    }


/**
 * The notifier has been deactivated so resources can be freed and outstanding messages completed.
 */
void CSenNotPluginDlgImpl::Cancel()
    {
    DOLOG(_L("[ CSenNotPluginDlgImpl::Cancel: Starting ]"));

    if(iMethod == TSenNotPluginRequest::ENoMethod)
        {
        DOLOG(_L("[ CSenNotPluginDlgImpl::Cancel: No outstanding request. Freeing resources ]"));
        Cleanup();
        }
    else
        {
        if(iDialog)
            {
            DOLOG(_L("[ CSenNotPluginDlgImpl::Cancel: dialog is being displayed. Cancelling it ]"));
/*            if(iMethod == TSenNotPluginRequest::EAuthenticationDlg)
                {
                ((CUsernamePasswdDialog*)iDialog)->CancelDialogL(); // Will delete dialog
                iDialog = NULL; //
                }
            else*/
                {
                TKeyEvent keyEvent;
                keyEvent.iCode = EKeyEscape;
                keyEvent.iScanCode = EStdKeyEscape;
                keyEvent.iModifiers = EModifierAutorepeatable;
                keyEvent.iRepeats = 50;
                TInt leaveCode(KErrNone);
                TRAP(leaveCode, iDialog->OfferKeyEventL(keyEvent, EEventKey));
                if (leaveCode != KErrNone) iCancelled = EFalse;
                }

            DOLOG(_L("[ CSenNotPluginDlgImpl::Cancel: dialog cancel initiated ]"));
                // Note: Execution will continue in RunL, where cleanup must be performed
            iCancelled = ETrue;
            }
        else
            {
            DOLOG(_L("[ CSenNotPluginDlgImpl::Cancel: Pending request, but not executing it. Freeing resource ]"));
            Cleanup(); // This will effectively cancel asynchronous operation
            }
        }
    }

/**
 * Update a currently active notifier with data aBuffer.
 */
TPtrC8 CSenNotPluginDlgImpl::UpdateL(const TDesC8& /*aBuffer*/)
    {
    DOLOG(_L("[ CSenNotPluginDlgImpl::UpdateL ]"));
    return TPtrC8(NULL, 0);
    }

#ifdef EKA2
// ----------------------------------------------------------
//
// ECOM 
//
// -----------------------------------------------------------

const TImplementationProxy ImplementationTable[] =
    {
#ifndef RD_SEN_COMPILE_SIS_PACKAGE_FILES
    IMPLEMENTATION_PROXY_ENTRY(0x101F9765,NotifierArray)
#else
    IMPLEMENTATION_PROXY_ENTRY(0x101F9744,NotifierArray)
#endif    

    };

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy) ;
    return ImplementationTable;
    }
#endif

// End of File
