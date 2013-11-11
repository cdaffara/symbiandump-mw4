/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   VPN Ecom Notifier's definition file
*
*/



#ifndef __VPNECOMNOTIFIER_H__
#define __VPNECOMNOTIFIER_H__

#include <e32std.h>
#include <e32base.h>
#include <eiknotapi.h>
#include <eikenv.h>

#include "vpndialogmanagerecomstub.h"
#include "vpnnotifierdefs.h"

#include <ecom/implementationproxy.h>

// Notifier should go to a dialog channel, not a led or sound channel
const TUid KCrystalScreenOutputChannel = {0x10009D48};

// Method at ordinal 1 to get a list of notifiers from this dll
IMPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount);
CArrayPtr<MEikSrvNotifierBase2>* NotifierArray();

//////////////////////////////////////////////////////////////////////////////////
//
//  The CVpnNotifier class is registered with the dialog server and
//  is responsible for receiving and sending messages and creating dialogs
//  for user requests.
//
//////////////////////////////////////////////////////////////////////////////////

class MVpnDialog;

class CVpnNotifier : public CBase, public MEikSrvNotifierBase2, public MVpnNotifierCallback
    {
public:
    CVpnNotifier(const TUid& aNotifierUid, const TDesC& aResourceFilename,
                   CVpnDialogManagerEcomStub* aDialogManager);
    ~CVpnNotifier();
    void DialogComplete(TInt aReturnCode);
    void DialogCompleteL(TInt aReturnCode, TVpnDialogOutput& aOutput);

private: // From MEikSrvNotifierBase(2)
    void Release();
    TNotifierInfo RegisterL();
    TNotifierInfo Info() const;
    void StartL( const TDesC8& aBuffer, TInt aReturnValue, const RMessagePtr2& aMessage);
    TPtrC8 StartL( const TDesC8& aBuffer);
    void Cancel();
    TPtrC8 UpdateL( const TDesC8& aBuffer);

private:    
    TFileName GetResourceFileNameL();
    // Added to overcome uninstallation problems
    void LoadResourceFileL();
    void UnloadResourceFile();
    TBool LaunchDialogL(TInt aDialogId, TInt aNoteDialogId, const TDesC8& aInput);
    
    /**
     * Function for loading the actual VPN dialog manager implementation DLL
     * (due to ecom architecture specialities, it will be unloaded and loaded 
     * again whenever LaunchDialogL is called).
     *
     * @leave Leaves if (e.g.) the DLL is not found
     */
    void        LoadManagerDllL();

    /**
     * Unloads the manager DLL.
     *
     */
    void        CloseManagerDll();


private:
    CEikonEnv* iEikEnv;         // Does not need to be destroyed
    TInt iResourceFileOffset;   // Resource file id
    TNotifierInfo iInfo;        //notifier info
    MVpnDialog* iDialog;
    RMessagePtr2 iMessage;
    TInt iReturnValue;

    CVpnDialogManagerEcomStub* iDialogManager;

    TPtrC iResourceFilename;
    TUid iNotifierUid;
    HBufC8* iInput;
    // Added to overcome uninstallation problems
    TBool iResourceFileLoaded;

    // Instance variables for handling the "real" (non-ecom) dialog manager DLL.
    MVpnDialogManager*  iRealDialogManager;
    RLibrary            iDialogManagerLib;


    };

#endif  // __VPNNOTIFIER_H__
