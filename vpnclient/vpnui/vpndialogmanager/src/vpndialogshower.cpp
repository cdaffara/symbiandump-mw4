/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of VPN dialog classes
*
*/

#include <e32std.h>
#include <e32base.h>
#include <eikenv.h>
#include <bautils.h>
#include <eikseced.h>
#include <aknlists.h>
#include <aknselectionlist.h>
#include <eikclb.h> 
#include <eiktxlbm.h>  //CEikTextListBoxModel
#include <eikbtgpc.h>  //CEikButtonGroupContainer
#include <eikrutil.h>  //EikResourceUtils
#include <eikclbd.h>   //CColumnListBoxData
#include <eiklabel.h>  //CEikLabel
#include <eikcapc.h>  // for CEikCaptionedControl
#include <avkon.rsg>
#include <StringLoader.h>
#include <AknProgressDialog.h>

#include <vpnecomnotifier.rsg>

#include "vpndialogshower.h"
#include "vpnnotifierdefs.h"

#include "logvpncommon.h"

#include <layoutmetadata.cdl.h>


CVpnInfoDialog* CVpnDialogShower::ShowNoteDialogL(MVpnNotifierCallback* aNotifier, TInt aTextId)
    {
    CVpnInfoDialog* dlg = new(ELeave)CVpnInfoDialog(aNotifier);
    HBufC* text = CEikonEnv::Static()->AllocReadResourceLC(aTextId);
    (static_cast<CAknInformationNote*>(dlg))->SetTimeout(CAknNoteDialog::ELongTimeout); 
    dlg->ExecuteLD(*text);
    CleanupStack::PopAndDestroy(); // text
    return dlg;
    }


// CVpnDialogShower

CVpnDialog* CVpnDialogShower::GetUserIdentityDlgL(MVpnNotifierCallback* aNotifier, const TDesC8& aInput)
    {
    LOG_("-> CVpnDialogShower::GetUserIdentityDlgL()");
    CVpnUserIdentityDlg* dialog = new (ELeave) CVpnUserIdentityDlg(aNotifier, aInput);
    dialog->ExecuteLD(R_VPN_USER_IDENTITY_QUERY);
    LOG_("<- CVpnDialogShower::GetUserIdentityDlgL()");
    return dialog;
    }
    
CVpnDialog* CVpnDialogShower::GetPolicyUpdateConfirmationDlgL(MVpnNotifierCallback* aNotifier, const TDesC8& /*aInput*/)
    {
    CVpnDialog* dialog = new (ELeave) CVpnDialog(aNotifier);
    dialog->ExecuteLD(R_VPN_POLICY_UPDATE_CONFIRMATION_QUERY);
    return dialog;
    }

CVpnDialog* CVpnDialogShower::GetPKCS12PasswordDlgL(MVpnNotifierCallback* aNotifier)
    {
    LOG_("-> CVpnDialogShower::GetPKCS12PasswordDlgL()");
    LOG_(" Instantiating dialog");
    CPKCS12PasswordDlg * dialog = new (ELeave) CPKCS12PasswordDlg(aNotifier);
    LOG_(" Executing dialog");
    dialog->ExecuteLD(R_VPN_PKCS12_PASSWORD_QUERY);
    LOG_("<- CVpnDialogShower::GetPKCS12PasswordDlgL()");
    return dialog;
    }

// CKmdDialogShower

CVpnMultiLineDialog* CVpnDialogShower::GetUserPwdDlgL(MVpnNotifierCallback* aNotifier, const TDesC8& aInput)
    {
    iValue1 = KNullDesC;
    iValue2 = KNullDesC;
    CKmdDlg* dialog = CKmdDlg::NewL(aNotifier, iValue1, iValue2, aInput, CAknQueryDialog::ENoTone);
    dialog->ExecuteLD(R_KMD_CRACK_UNPW_QUERY);
    return dialog;
    }

CVpnMultiLineDialog* CVpnDialogShower::GetSecurIdDlgL(MVpnNotifierCallback* aNotifier, const TDesC8& aInput)
    {
    iValue1 = KNullDesC;
    iValue2 = KNullDesC;
    CKmdDlg* dialog = CKmdDlg::NewL(aNotifier, iValue1, iValue2, aInput, CAknQueryDialog::ENoTone);
    dialog->ExecuteLD(R_KMD_CRACK_SID_QUERY);
    return dialog;
    }

CVpnMultiLineDialog* CVpnDialogShower::GetSecurIdNextDlgL(MVpnNotifierCallback* aNotifier, const TDesC8& aInput)
    {
    iValue1 = KNullDesC;
    iValue2 = KNullDesC;
    CKmdDlg* dialog = CKmdDlg::NewL(aNotifier, iValue1, iValue2, aInput, CAknQueryDialog::ENoTone);
    dialog->ExecuteLD(R_KMD_CRACK_SID_NEXT_QUERY);
    return dialog;
    }

/**
   Creates the "Import policy 'XXX'?" dialog. The install filename in aInput has been
   passed on by VPNPolIns.
 */
CVpnDialog* CVpnDialogShower::GetPolicyImportConfirmationDlgL(MVpnNotifierCallback* aNotifier, const TDesC8& aInput)
    {
    LOG_("-> CVpnDialogShower::GetPolicyImportConfirmationDlgL");

    HBufC* tempBuf = HBufC::NewLC(aInput.Length());
    tempBuf->Des().Copy(aInput);

    HBufC* str = StringLoader::LoadLC(R_VPN_STR_QUEST_IMPORT_POLICY_STR, *tempBuf);

    CVpnDialog* dialog = new (ELeave) CVpnDialog(aNotifier);
    dialog->PrepareLC(R_VPN_QUEST_IMPORT_POLICY);
    dialog->SetPromptL(*str);
    dialog->RunLD();

    CleanupStack::PopAndDestroy(2); // str, tempBuf

    LOG_("<- CVpnDialogShower::GetPolicyImportConfirmationDlgL");
    return dialog;
    }

CVpnDialog* CVpnDialogShower::GetDevLockConfirmationDlgL(MVpnNotifierCallback* aNotifier, 
                                                         const TDesC8& aInput)
    {
    LOG_("-> CVpnDialogShower::GetDevLockConfirmationDlgL");

    HBufC* tempBuf = HBufC::NewLC(aInput.Length());
    tempBuf->Des().Copy(aInput);

    LOG_1(" Dialog string resource ID: 0x%x", R_VPN_STR_QUEST_APPLY_DEVLOCK_STR);
    LOG_1(" Dialog resource ID: 0x%x", R_VPN_QUEST_APPLY_DEVLOCK);
    HBufC* str = StringLoader::LoadLC(R_VPN_STR_QUEST_APPLY_DEVLOCK_STR, *tempBuf);

    CVpnDialog* dialog = new (ELeave) CVpnDialog(aNotifier);
    dialog->PrepareLC(R_VPN_QUEST_APPLY_DEVLOCK);
    dialog->SetPromptL(*str);
    dialog->RunLD();

    CleanupStack::PopAndDestroy(2); // str, tempBuf

    LOG_("<- CVpnDialogShower::GetDevLockConfirmationDlgL");
    return dialog;
    }

/**
 * Implementation of the wait dialog for policy installation process
 *
 */
CVpnWaitDialog* CVpnDialogShower::GetPolicyImportProgressDlgL(MVpnNotifierCallback* aNotifier, const TDesC8& /*aInput*/)
    {
    LOG_("-> CVpnDialogShower::GetPolicyImportProgressDlgL()");
    
    CVpnWaitDialog* dialog = new ( ELeave ) CVpnWaitDialog(aNotifier);
    dialog->SetTone( CAknNoteDialog::ENoTone );
    dialog->SetCallback( this );
    dialog->ExecuteLD(R_VPN_WAIT_IMPORT_POLICY);

    LOG_("<- CVpnDialogShower::GetPolicyImportProgressDlgL()");
    return dialog;
    }

CVpnWaitDialog* CVpnDialogShower::GetGenericProgressDlgL(MVpnNotifierCallback* aNotifier, const TDesC8& /*aInput*/)
    {
    LOG_("-> CVpnDialogShower::GetGenericProgressDlgL()");
    
    CVpnWaitDialog* dialog = new ( ELeave ) CVpnWaitDialog(aNotifier);
    dialog->SetTone( CAknNoteDialog::ENoTone );
    dialog->SetCallback( this );
    dialog->ExecuteLD(R_VPN_WAIT_PROCESSING);

    LOG_1(" Dialog 0x%08x", dialog);
    
    LOG_("<- CVpnDialogShower::GetGenericProgressDlgL()");
    return dialog;
    }


// CVpnUserIdentityDlg

CVpnUserIdentityDlg::CVpnUserIdentityDlg(MVpnNotifierCallback* aNotifier, const TDesC8& aInput) : CVpnDialog(aNotifier)
    {
    iInput.Set(aInput);
    }

void CVpnUserIdentityDlg::PreLayoutDynInitL()
    {
    if (iInput.Length() > 0)
        {
        HBufC* dialogInput = VpnNotifierUtil::To16BitL(iInput);
        CleanupStack::PushL(dialogInput);
        
        TInt paramSepOffset = dialogInput->Locate(KDialogParamSep);
        TInt domainNameLength = paramSepOffset;
        TInt proposedUserIdentityLength = dialogInput->Length() - paramSepOffset - 1; // 1 for separator
        
        if (paramSepOffset <= 0 || domainNameLength <= 0 || proposedUserIdentityLength <= 0)
            {
            User::Leave(KErrArgument);
            }

        TPtrC domainName(dialogInput->Left(paramSepOffset));
        TPtrC proposedUserIdentity(dialogInput->Mid(paramSepOffset + 1));

        HBufC* prompt = StringLoader::LoadLC(R_VPN_USER_IDENTITY, domainName);
        
        CAknQueryControl* control = static_cast<CAknQueryControl*>(Control(EEikCidPassword));
        control->SetPromptL(*prompt);
        control->SetTextL(proposedUserIdentity);
        
        CleanupStack::PopAndDestroy(2); // prompt, dialogInput 
        }
    }

void CVpnUserIdentityDlg::GetOutputL(TVpnDialogOutput& aOutput, TInt /*aButtonId*/)
    {
    CAknQueryControl* control = static_cast<CAknQueryControl*>(Control(EEikCidPassword));

    TBuf<64> userIdentity;
    control->GetText(userIdentity);
    
    HBufC8* userIdentity8 = VpnNotifierUtil::To8BitL(userIdentity);
    CleanupStack::PushL(userIdentity8);
    aOutput.iOutBuf.Copy(*userIdentity8);
    CleanupStack::PopAndDestroy(userIdentity8);
    }
    
    
    
CPKCS12PasswordDlg::CPKCS12PasswordDlg(MVpnNotifierCallback* aNotifier) : CVpnDialog(aNotifier)
    {
    LOG_("-> CVpnDialogShower::CPKCS12PasswordDlg()");
    LOG_("<- CVpnDialogShower::CPKCS12PasswordDlg()");
    }

void CPKCS12PasswordDlg::GetOutputL(TVpnDialogOutput& aOutput, TInt /*aButtonId*/)
    {
    LOG_("-> CVpnDialogShower::GetOutputL()");

    CAknQueryControl* control = static_cast<CAknQueryControl*>(Control(EEikCidPassword));

    TBuf<64> password;
    control->GetText(password);

    LOG_1(" Password: '%S'", &password);

    HBufC8* password8 = VpnNotifierUtil::To8BitL(password);
    CleanupStack::PushL(password8);
    aOutput.iOutBuf.Copy(*password8);
    CleanupStack::PopAndDestroy(password8);

    LOG_("<- CVpnDialogShower::GetOutputL()");
    }

// CVpnDialog
CVpnDialog::CVpnDialog(MVpnNotifierCallback* aNotifier) : iNotifier(aNotifier)
    {
    }

void CVpnDialog::CancelL()
    {
    TryExitL(EEikBidCancel);
    }

TBool CVpnDialog::OkToExitL(TInt aButtonId)
    {

    if (aButtonId == EAknSoftkeyOptions)
        {
        DisplayMenuL(); //CAknDialog method
        return EFalse;
        }

    if (aButtonId == EEikBidCancel ||
        aButtonId == EAknSoftkeyBack ||
        aButtonId == EAknSoftkeyNo)
        {
        if (AllowCancel())
            {
            iNotifier->DialogComplete(KErrCancel);
            }
        else
            {
            return EFalse;
            }
        }
    else
        {
        TVpnDialogOutput dlgOutput;
        GetOutputL(dlgOutput, aButtonId);
        iNotifier->DialogCompleteL(KErrNone, dlgOutput);
        }
    return ETrue;
    }

TBool CVpnDialog::AllowCancel()
    {
    return ETrue;
    }
    
void CVpnDialog::GetOutputL(TVpnDialogOutput& /*aOutput*/, TInt /*aButtonId*/)
    {
    }


CVpnWaitDialog::CVpnWaitDialog(MVpnNotifierCallback* aNotifier) : CAknWaitDialog(NULL, ETrue), 
                                                            iNotifier(aNotifier)
    {
    }

void CVpnWaitDialog::CancelL()
    {
    TryExitL(EEikBidCancel);
    }

TBool CVpnWaitDialog::OkToExitL(TInt aButtonId)
    {

    if (aButtonId == EAknSoftkeyOptions)
        {
        return EFalse;
        }

    if (aButtonId == EEikBidCancel ||
        aButtonId == EAknSoftkeyBack ||
        aButtonId == EAknSoftkeyNo)
        {
        if (AllowCancel())
            {
            iNotifier->DialogComplete(KErrCancel);
            }
        else
            {
            return EFalse;
            }
        }
    else
        {
        TVpnDialogOutput dlgOutput;
        GetOutputL(dlgOutput, aButtonId);
        iNotifier->DialogCompleteL(KErrNone, dlgOutput);
        }
    return ETrue;
    }

TBool CVpnWaitDialog::AllowCancel()
    {
    return ETrue;
    }

void CVpnWaitDialog::GetOutputL(TVpnDialogOutput& /*aOutput*/, TInt /*aButtonId*/)
    {
    }

// CVpnMultiLineDialog
CVpnMultiLineDialog::CVpnMultiLineDialog(MVpnNotifierCallback* aNotifier, const TTone& aTone)
: CAknMultiLineDataQueryDialog(aTone), iNotifier(aNotifier)
    {
    }


CVpnMultiLineDialog* CVpnMultiLineDialog::NewL(MVpnNotifierCallback* aNotifier, TDes& aData1, TDes& aData2, TTone aTone)
    {
    CVpnMultiLineDialog* self = new (ELeave) CVpnMultiLineDialog(aNotifier, aTone);
    CleanupStack::PushL(self);

    // Set the query data for first and second line
    // Must call CAknMultiLineDataQueryDialog::SetDataL method
    // during second phase construction (NewL method).
    self->SetDataL(aData1,aData2);

    CleanupStack::Pop(self);
    return self;
    }

void CVpnMultiLineDialog::CancelL()
    {
    TryExitL(EEikBidCancel);
    }

TBool CVpnMultiLineDialog::OkToExitL(TInt aButtonId)
    {
    	if (!CAknMultiLineDataQueryDialog::OkToExitL(aButtonId))
       	   	{
        	return EFalse;
        	}  
    if (aButtonId == EEikBidCancel)
        {
        if (AllowCancel())
            {
            iNotifier->DialogComplete(KErrCancel);
            }
        else
            {
            return EFalse;
            }
        }
    else
        {
        TVpnDialogOutput dlgOutput;
        GetOutputL(dlgOutput, aButtonId);
        iNotifier->DialogCompleteL(KErrNone, dlgOutput);
        }
    return ETrue;
    }

TBool CVpnMultiLineDialog::AllowCancel()
    {
    return ETrue;
    }

void CVpnMultiLineDialog::GetOutputL(TVpnDialogOutput& /*aOutput*/, TInt /*aButtonId*/)
    {
    }

// CVpnInfoDialog class
CVpnInfoDialog::CVpnInfoDialog(MVpnNotifierCallback* aNotifier)
: iNotifier(aNotifier)
    {
    }

CVpnInfoDialog::~CVpnInfoDialog()
    {
    TIPSecDialogOutput dlgOutput;
    TRAPD(err, GetOutputL(dlgOutput, EEikBidCancel));
    if ( iNotifier )
    	TRAP(err, iNotifier->DialogCompleteL(KErrNone, dlgOutput));
    }
    
void CVpnInfoDialog::CancelL()
    {
    TryExitL(EEikBidCancel);
    }

TBool CVpnInfoDialog::OkToExitL(TInt aButtonId)
    {
    if (aButtonId == EEikBidCancel)
        {
        if (AllowCancel())
            {
            iNotifier->DialogComplete(KErrCancel);
            }
        else
            {
            return EFalse;
            }
        }
    if (aButtonId == EAknSoftkeyEmpty)
        {
        return ETrue;
        }
    else
        {
        TVpnDialogOutput dlgOutput;
        GetOutputL(dlgOutput, aButtonId);
        iNotifier->DialogCompleteL(KErrNone, dlgOutput);
        }
    return ETrue;
    }

TBool CVpnInfoDialog::AllowCancel()
    {
    return ETrue;
    }

// Useless? Shouldn't leave in any case.
void CVpnInfoDialog::GetOutputL(TVpnDialogOutput& /*aOutput*/, TInt /*aButtonId*/)
    {

    }

// CKmdDlg

CKmdDlg::CKmdDlg(MVpnNotifierCallback* aNotifier, const TTone& aTone) : CVpnMultiLineDialog(aNotifier, aTone)
    {
    }

CKmdDlg::CKmdDlg(MVpnNotifierCallback* aNotifier, const TDesC8& aInput, const TTone& aTone)
: CVpnMultiLineDialog(aNotifier, aTone)
    {
    iInput.Set(aInput);
    }

CKmdDlg* CKmdDlg::NewL(MVpnNotifierCallback* aNotifier, TDes& aData1, TDes& aData2, TTone aTone)
    {
    CKmdDlg* self = new (ELeave) CKmdDlg(aNotifier, aTone);
    CleanupStack::PushL(self);
    // Set the query data for first and second line
    // Must call CAknMultiLineDataQueryDialog::SetDataL method
    // during second phase construction (NewL method).
    self->SetDataL(aData1, aData2);
    CleanupStack::Pop();
    return self;
    }

CKmdDlg* CKmdDlg::NewL(MVpnNotifierCallback* aNotifier, TDes& aData1, TDes& aData2, const TDesC8& aInput, TTone aTone)
    {
    CKmdDlg* self = new (ELeave) CKmdDlg(aNotifier, aInput, aTone);
    CleanupStack::PushL(self);
    // Set the query data for first and second line
    // Must call CAknMultiLineDataQueryDialog::SetDataL method
    // during second phase construction (NewL method).
    self->SetDataL(aData1, aData2);
    CleanupStack::Pop();
    return self;
    }

void CKmdDlg::PreLayoutDynInitL()
    {
    SetMaxLengthOfFirstEditor(KMaxAuthenticationInputLength);
    SetMaxLengthOfSecondEditor(KMaxAuthenticationInputLength);
    
    CAknMultiLineDataQueryDialog::PreLayoutDynInitL();
    
    CAknQueryControl* usernameControl = static_cast<CAknQueryControl*>(Control(EEikCidPassword));
    
    if (iInput.Length() > 0)
        {
        HBufC16* usernameTextH16 = VpnNotifierUtil::To16BitL(iInput);
        CleanupStack::PushL(usernameTextH16);
        usernameControl->SetTextL(*usernameTextH16);
        CleanupStack::PopAndDestroy(); // usernameTextH16
        if (Layout_Meta_Data::IsLandscapeOrientation())
        	{
        	MakeLeftSoftkeyVisible(ETrue);
        	}
        }
    }

void CKmdDlg::SetInitialCurrentLine()
    {
    CAknMultiLineDataQueryDialog::SetInitialCurrentLine();
    if (!Layout_Meta_Data::IsLandscapeOrientation())
		{
		CEikCaptionedControl* passwordLine(NULL);
	    if (iInput.Length() > 0)
        	{
        	passwordLine = Line(EEikCidPasswordConfirm);
        	}
    	else
        	{
        	passwordLine = Line(EEikCidPassword);
        	}
        ASSERT(passwordLine);	
        TRAP_IGNORE( TryChangeFocusToL(passwordLine->iId) );	
    	}

	HandleResourceChange(KEikDynamicLayoutVariantSwitch);
    }

void CKmdDlg::GetOutputL(TVpnDialogOutput& aOutput, TInt /*aButtonId*/)
    {
    CAknQueryControl* firstControl = static_cast<CAknQueryControl*>(Control(EEikCidPassword));
    firstControl->GetText(aOutput.iOutBuf);

    CAknQueryControl* secondControl = static_cast<CAknQueryControl*>(Control(EEikCidPasswordConfirm));
    secondControl->GetText(aOutput.iOutBuf2);
    }
