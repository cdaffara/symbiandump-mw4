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
* Description:   VPN dialog manager
*
*/

#include <vpnecomnotifier.rsg>
#include "vpnnotifierdefs.h"
#include "vpndialogshower.h"
#include "vpndialogmanager.h"

#include "logvpncommon.h"


// At ordinal 1, a factory method used to produce cvpndialogmanager instances
EXPORT_C MVpnDialogManager* NewDialogManagerL()
    {
    LOG_("-> NewDialogManagerL()");
    LOG_("<- NewDialogManagerL()");
    return new (ELeave) CVpnDialogManager;
    }

MVpnDialog* CVpnDialogManager::LaunchDialogL(MVpnNotifierCallback* aNotifier, TInt aDialogId,
                                               TInt aNoteDialogId, const TDesC8& aInput)
    {
    CVpnDialogShower* shower = GetDialogShowerL();
    LOG_1("-> CVpnDialogManager::LaunchDialogL() dialog ID is: %d", aDialogId);
    LOG_1(" Note dialog ID: %d", aNoteDialogId);

    // If note dialog id hasn't been specified, then we're going
    // to display a more complex query dialog
    if (aNoteDialogId == 0) 
        {
        switch (aDialogId)
            {

            // VPN related dialogs
            case TVpnDialog::EUserIdentity:
                return shower->GetUserIdentityDlgL(aNotifier, aInput);
            case TVpnDialog::EPolicyUpdateConfirmation:
                return shower->GetPolicyUpdateConfirmationDlgL(aNotifier, aInput);

            // Policy installer's dialogs
            case TVpnDialog::EPolicyInstallConfirmation:
                LOG_1(" Showing pol import conf %d", aDialogId);
                return shower->GetPolicyImportConfirmationDlgL(aNotifier, aInput);
            case TVpnDialog::EPolicyInstallInProgress:
                return shower->GetPolicyImportProgressDlgL(aNotifier, aInput);
            case TVpnDialog::EGenericProgress:
                return shower->GetGenericProgressDlgL(aNotifier, aInput);
            case TVpnDialog::EPKCS12Password:
                return shower->GetPKCS12PasswordDlgL(aNotifier);
            case TVpnDialog::EDeviceLockConfirmation:
                LOG_1(" Showing dev lock pol import conf %d", aDialogId);
                return shower->GetDevLockConfirmationDlgL(aNotifier, aInput);

            // KMD Dialogs
            case TKmdDialog::EUserPwd:
                return shower->GetUserPwdDlgL(aNotifier, aInput);
            case TKmdDialog::ESecurIdPin:
                return shower->GetSecurIdDlgL(aNotifier, aInput);
            case TKmdDialog::ESecurIdNextPin:
                return shower->GetSecurIdNextDlgL(aNotifier, aInput);

            default:
                LOG_(" Default case, return NULL");
                return NULL;
            }
        }
    else 
        {
        // Display the desired note
        LOG_(" Showing note dialog");
        return ShowNoteDialogL(aNotifier, aNoteDialogId);
        }
    }

TInt CVpnDialogManager::GetNoteTextId(TInt aNoteDialogId)
    {
    LOG_("CVpnDialogManager::GetNoteTextId()");
    switch (aNoteDialogId)
        {
        // ACU note dialogs
        case TVpnNoteDialog::EWrongServerIdentityCode:
            return R_ACU_WRONG_SERVER_IDENTITY_CODE;
        case TVpnNoteDialog::ENextServerIdentity:
            return R_ACU_NEXT_SERVER_IDENTITY;
        case TVpnNoteDialog::EAuthenticationFailed:
            return R_ACU_AUTH_FAILED;
        case TVpnNoteDialog::EInvalidServerCert:
            return R_ACU_INVALID_SERVER_CERT;
        case TVpnNoteDialog::EInvalidClientCert:
            return R_ACU_INVALID_CLIENT_CERT;
        // VPN related note dialogs
        case TVpnNoteDialog::EPolicyUpdateSucceeded:
            return R_VPN_POLICY_UPDATE_SUCCEEDED;
        case TVpnNoteDialog::EPolicyUpdateFailed:
            return R_VPN_POLICY_UPDATE_FAILED;
        case TVpnNoteDialog::EPolicyDeleteSucceeded:
            return R_VPN_POLICY_DELETE_SUCCEEDED;
        case TVpnNoteDialog::EVpnUserNameMissing:
            return R_VPN_USER_NAME_MISSING;
        case TVpnNoteDialog::EVpnFqdnMissing:
            return R_VPN_FQDN_MISSING;
        case TVpnNoteDialog::EVpnCannotForceDeviceLock:
            return R_VPN_STR_CANNOT_FORCE_DEVICE_LOCK;
        case TVpnNoteDialog::EPolicyInstallationFailed:
            return R_VPN_POLICY_INSTALLATION_FAILED;
        // Policy installer notes
        case TVpnNoteDialog::EVpnPolicyInstalled:
            return R_VPN_CONF_POLICY_IMPORT_COMPLETE;
        case TVpnNoteDialog::EVpnWrongPKCS12Password:
            return R_VPN_STR_WRONG_PKCS12_PASSWORD_STR;
        // Kmd note dialogs
        case TVpnNoteDialog::EKmdAuthenticationFailed:
        	return R_KMD_CRACK_AUTH_FAILED;
        default:
            LOG_(" Calling CVpnDialogManagerBase::GetNoteTextId()");
            return CVpnDialogManager::GetNoteTextId(aNoteDialogId);
        }
    }

CVpnDialogShower* CVpnDialogManager::GetDialogShowerL()
    {
    LOG_("-> CVpnDialogManager::GetDialogShowerL()");
    if (iDialogShower)
        {
        LOG_("<- CVpnDialogManager::GetDialogShowerL() (old shower reused)");
        return iDialogShower;
        }
    else
        {
        iDialogShower = new (ELeave) CVpnDialogShower();
        LOG_("<- CVpnDialogManager::GetDialogShowerL() (new shower instantiated)");
        return iDialogShower;
        }
    }

MVpnDialog* CVpnDialogManager::ShowNoteDialogL(MVpnNotifierCallback* aNotifier, TInt aNoteDialogId)
    {
    LOG_("-> CVpnDialogManager::ShowNoteDialogL()");
    LOG_("<- CVpnDialogManager::ShowNoteDialogL()");
    return GetDialogShowerL()->ShowNoteDialogL(aNotifier, GetNoteTextId(aNoteDialogId));    
    }

CVpnDialogManager::~CVpnDialogManager()
    {
    LOG_("-> CVpnDialogManager::~CVpnDialogManager()");
    delete iDialogShower;
    LOG_("<- CVpnDialogManager::~CVpnDialogManager()");
    }

