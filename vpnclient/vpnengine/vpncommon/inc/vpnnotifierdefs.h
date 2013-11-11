/*
* Copyright (c) 2003 - 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   VPN notifier definitions
*
*/

#ifndef VPNNOTIFIERDEFS_H
#define VPNNOTIFIERDEFS_H

#include <e32base.h>

class MVpnNotifierCallback;
class MVpnDialog;

_LIT(KVpnDialogManagerDll, "vpndialogmanager.dll");

/**
 @file
 @released
 @publishedPartner
 
 VPN Notifier definitions
*/

class MVpnDialog
    {
public:
    virtual void CancelL() = 0;
    };

class TNoteDialog
    {
public:
    enum TDialogId
        {
        EInfo = -10,
        EWarning,
        EError
        };
    };

class TVpnDialogInfo
    {
public:
    TVpnDialogInfo()
        {
        iDialogId = 0;
        iNoteDialogId = 0;
        iNoteExtraInput.SetLength(0);
        }

    TVpnDialogInfo(TInt aDialogId, TInt aNoteDialogId)
        {
        iDialogId = aDialogId;
        iNoteDialogId = aNoteDialogId;
        iNoteExtraInput.SetLength(0);
        }

    TVpnDialogInfo(TInt aDialogId, TInt aNoteDialogId, TDesC8& aInput)
        {
        iDialogId = aDialogId;
        iNoteDialogId = aNoteDialogId;
        iNoteExtraInput.SetLength(aInput.Length());
        iNoteExtraInput.Copy(aInput.Left(256));
        }

    void SetExtraInput(TDesC8& aInput) 
        {
        iNoteExtraInput.Copy(aInput.Left(256));
        }
    TInt DialogId() {return iDialogId;}
    TInt NoteDialogId() {return iNoteDialogId;}
    TDesC8& NoteExtraInput() 
        {
        return iNoteExtraInput;
        }

private:
    TInt iDialogId;
    TInt iNoteDialogId;
    TBuf8<256> iNoteExtraInput;
    };

class TVpnDialogOutput
    {
public:
    TVpnDialogOutput()
        {
        Clear();
        }
    void Clear()
        {
        iOutBuf.SetLength(0);
        iOutBuf2.SetLength(0);
        iOutInt = 0;
        iOutInt2 = 0;
        }

public:
    TBuf<256> iOutBuf;
    TBuf<64> iOutBuf2;
    TInt iOutInt;
    TInt iOutInt2;
    };

// typedefs for backward compatibility
// typedef new_type old_type
typedef TVpnDialogInfo TIPSecDialogInfo;
typedef TVpnDialogOutput TIPSecDialogOutput;


// 
// KMD notifier definitions
//

class TKmdDialog
    {
public:
    enum TDialogId
        {
        EUserPwd = 2000, // MUST be greater than the last value in enum TNoteDialog::TDialogId
        ESecurIdPin,
        ESecurIdNextPin        
        };
    };

const TUid KUidVpnDialogNotifier = {0x101F6EB7};

_LIT(KVpnNotifierResource,"\\resource\\VPNECOMNOTIFIER.RSC");

const TUint KDialogParamSep = '#';


class TVpnDialog
    {
public:
    enum TDialogId
        {
        EServerIdentity = 100, // MUST be greater than the last value in enum TNoteDialog::TDialogId
        ELegacyAuthentication,              // 101
        ELegacyAuthenticationIn,            // 102
        EContentUpdateDetails,              // 103
        EContentUpdateDetailsNote,          // 104
        // VPN releated dialogs             
        EUserIdentity = 1000,               // 1000
        EPolicyUpdateConfirmation,          // 1001
        EPolicyInstallConfirmation,         // 1002
        EPKCS12Password,                    // 1003
        EPKCS12WrongPassword,               // 1004
        EDeviceLockConfirmation,            // 1005
        EGenericProgress,                   // 1006
        EPolicyInstallInProgress            // 1007
        };
    };

class TVpnNoteDialog
    {
public:
    enum TTextId
        {
        EWrongServerIdentityCode = 1,
        ENextServerIdentity,
        EAuthenticationFailed,
        EInvalidServerCert,
        EInvalidClientCert,
        // VPN related notes
        EPolicyUpdateSucceeded = 1000,
        EPolicyUpdateFailed,
        EPolicyDeleteSucceeded,
        EVpnUserNameMissing,
        EVpnFqdnMissing,
        EPolicyInstallationFailed,
        EVpnWrongPKCS12Password,
        EVpnCannotForceDeviceLock,
        EVpnPolicyInstalled,
        // Kmd related notes
        EKmdAuthenticationFailed = 2000
        };
    };

// typedefs for backward compatibility
// typedef existing_type old_type
typedef TKmdDialog TKMDDialog;

class VpnNotifierUtil 
    {
public:
    inline static HBufC8* To8BitL(TDesC16& aText);
    inline static HBufC16* To16BitL(TDesC8& aText);
    };
    
HBufC8* VpnNotifierUtil::To8BitL(TDesC16& aText) 
    {
    HBufC8* text = HBufC8::NewL(aText.Length());
    text->Des().Copy(aText);
    return text;
    }

HBufC16* VpnNotifierUtil::To16BitL(TDesC8& aText) 
    {
    HBufC16* text = HBufC16::NewL(aText.Length());
    text->Des().Copy(aText);
    return text;
    }


class MVpnNotifierCallback 
    {
public:
    virtual void DialogCompleteL(TInt aReturnCode, TVpnDialogOutput& aOutput) = 0;
    virtual void DialogComplete(TInt aReturnCode) = 0;
    };

class MVpnDialogManager
    {
public:
    virtual MVpnDialog* LaunchDialogL(MVpnNotifierCallback* aNotifier, TInt aDialogId,
                                TInt aNoteDialogId, const TDesC8& aInput) = 0;
    virtual TInt GetNoteTextId(TInt aNoteDialogId) = 0;
    virtual MVpnDialog* ShowNoteDialogL(MVpnNotifierCallback* aNotifier, TInt aNoteDialogId) = 0;
    virtual ~MVpnDialogManager() {};
    };


#endif  // VPNNOTIFIERDEFS_H
