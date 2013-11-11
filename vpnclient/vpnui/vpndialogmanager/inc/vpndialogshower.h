/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Declaration of VPN dialog classes.
*
*/



#ifndef VPNDIALOGSHOWER_H
#define VPNDIALOGSHOWER_H

#include <AknQueryDialog.h>
#include <aknnotewrappers.h>
#include <avkon.rsg>
#include <avkon.mbg>
#include <AknWaitDialog.h>

#include "vpnnotifierdefs.h"


const TInt KMaxAcuInputLength = 50;
const TInt KMaxAuthenticationInputLength = 32;

class CVpnDialog;
class CEikColumnListBox;
class MVpnNotifierCallback;
class CVpnMultiLineDialog;
class CVpnWaitDialog;
class CVpnInfoDialog;

class CVpnDialogShower : public CBase, public MProgressDialogCallback
    {
public:
    CVpnDialog* GetServerAddressDlgL(MVpnNotifierCallback* aNotifier, const TDesC8& aInput);
    CVpnDialog* GetServerIdentityDlgL(MVpnNotifierCallback* aNotifier, const TDesC8& aInput);
    CVpnMultiLineDialog* GetLegacyAuthenticationDlgL(MVpnNotifierCallback* aNotifier);
    CVpnMultiLineDialog* GetLegacyAuthenticationDlgL(MVpnNotifierCallback* aNotifier, const TDesC8& aInput);
    CVpnDialog* GetUserIdentityDlgL(MVpnNotifierCallback* aNotifier, const TDesC8& aInput);
    CVpnDialog* GetPolicyUpdateConfirmationDlgL(MVpnNotifierCallback* aNotifier, const TDesC8& aInput);

    CVpnDialog* GetPolicyImportConfirmationDlgL(MVpnNotifierCallback* aNotifier, const TDesC8& aInput);
    CVpnWaitDialog* GetPolicyImportProgressDlgL(MVpnNotifierCallback* aNotifier, const TDesC8& aInput);
    CVpnWaitDialog* GetGenericProgressDlgL(MVpnNotifierCallback* aNotifier, const TDesC8& aInput);
    CVpnDialog* GetPKCS12PasswordDlgL(MVpnNotifierCallback* aNotifier);
    CVpnDialog* GetDevLockConfirmationDlgL(MVpnNotifierCallback* aNotifier, const TDesC8& aInput);
    CVpnInfoDialog* ShowNoteDialogL(MVpnNotifierCallback* aNotifier, TInt aTextId);
	CVpnMultiLineDialog* GetUserPwdDlgL(MVpnNotifierCallback* aNotifier, const TDesC8& aInput);
    CVpnMultiLineDialog* GetSecurIdDlgL(MVpnNotifierCallback* aNotifier, const TDesC8& aInput);
    CVpnMultiLineDialog* GetSecurIdNextDlgL(MVpnNotifierCallback* aNotifier, const TDesC8& aInput);
    
private: 
    void DialogDismissedL(TInt /* aButtonId */) 
        {
        };
	TBuf<KMaxAcuInputLength> iValue1;
    TBuf<KMaxAcuInputLength> iValue2;
    };

class CVpnDialog : public CAknQueryDialog, public MVpnDialog
    {
public:
    CVpnDialog(MVpnNotifierCallback* aNotifier);

public: // From MVpnDialog
    void CancelL();

protected:
    virtual TBool AllowCancel();
    virtual void GetOutputL(TVpnDialogOutput& aOutput, TInt aButtonId);
    
private:
    TBool OkToExitL(TInt aButtonId);

protected:
    MVpnNotifierCallback* iNotifier;
    };

class CVpnWaitDialog : public CAknWaitDialog, public MVpnDialog
    {
public:
        CVpnWaitDialog(MVpnNotifierCallback* aNotifier);
        CVpnWaitDialog();

public: // From MVpnDialog
        void CancelL();

protected:
        virtual TBool AllowCancel();
        virtual void GetOutputL(TVpnDialogOutput& aOutput, TInt aButtonId);

private:
        TBool OkToExitL(TInt aButtonId);

protected:
    MVpnNotifierCallback* iNotifier;
    };

// CVpnMultiLineDialog

class CVpnMultiLineDialog : public CAknMultiLineDataQueryDialog, public MVpnDialog
    {
public:
    // Two-phased constructor.
    static CVpnMultiLineDialog* NewL(MVpnNotifierCallback* aNotifier, TDes& aText1, TDes& aText2, TTone aTone = ENoTone);

protected:
    //Construction and destruction
    //use NewL
    CVpnMultiLineDialog(MVpnNotifierCallback* aNotifier, const TTone& aTone);

public: // From MVpnDialog
    void CancelL();

protected:
    virtual TBool AllowCancel();
    virtual void GetOutputL(TVpnDialogOutput& aOutput, TInt aButtonId);
    
private:
    TBool OkToExitL(TInt aButtonId);

protected:
    MVpnNotifierCallback* iNotifier;
    };

// CVpnInfoDialog

class CVpnInfoDialog : public CAknInformationNote, public MVpnDialog
    {
public:
    CVpnInfoDialog(MVpnNotifierCallback* aNotifier);

    ~CVpnInfoDialog();
public: // From MVpnDialog
    void CancelL();

protected:
    virtual TBool AllowCancel();
    virtual void GetOutputL(TVpnDialogOutput& aOutput, TInt aButtonId);

private:
    TBool OkToExitL(TInt aButtonId);

protected:
    MVpnNotifierCallback* iNotifier;
    };


class CPKCS12PasswordDlg: public CVpnDialog
    {
public:
    CPKCS12PasswordDlg(MVpnNotifierCallback* aNotifier);
private:
    void GetOutputL(TVpnDialogOutput& aOutput, TInt aButtonId);
    };


/**
 * A dialog for asking the user identity for a certificate (request).
 * Currently used only by the extended VPN Manager in T3. Located here
 * at least for now.
 */
class CVpnUserIdentityDlg : public CVpnDialog
    {
public:
    CVpnUserIdentityDlg(MVpnNotifierCallback* aNotifier, const TDesC8& aInput);
    void PreLayoutDynInitL();
private:
    void GetOutputL(TVpnDialogOutput& aOutput, TInt aButtonId);
private:
    TPtrC8 iInput;
    };

class CKmdDlg : public CVpnMultiLineDialog
    {
public:
    // NewL
    static CKmdDlg* NewL(MVpnNotifierCallback* aNotifier, TDes& aText1, TDes& aText2, TTone aTone = ENoTone);
    static CKmdDlg* NewL(MVpnNotifierCallback* aNotifier, TDes& aText1, TDes& aText2, const TDesC8& aInput, TTone aTone = ENoTone);
protected:  
    //Construction and destruction
    CKmdDlg(MVpnNotifierCallback* aNotifier, const TTone& aTone);
    CKmdDlg(MVpnNotifierCallback* aNotifier, const TDesC8& aInput, const TTone& aTone);
private:

    void PreLayoutDynInitL();
    void SetInitialCurrentLine();
    // From CVpnMultiLineDialog
    void GetOutputL(TVpnDialogOutput& aOutput, TInt aButtonId);

private: // Data
    TPtrC8 iInput;
    };


#endif  // VPNDIALOGSHOWER_H
