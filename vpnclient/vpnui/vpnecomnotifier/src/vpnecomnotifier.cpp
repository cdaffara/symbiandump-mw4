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
* Description:   VPN Ecom Notifier's implementation file
*
*/




#include <eikenv.h>
#include <bautils.h>

#include "logvpncommon.h"

#include <uikon/eiksrvui.h>  // Eikon server ui (for suppressing app -key)

#include "vpnecomnotifier.h"

#include "vpndialogmanagerecomstub.h"





CArrayPtrFlat<MEikSrvNotifierBase2>* createNotifierArrayL()
    {
    CArrayPtrFlat<MEikSrvNotifierBase2>* notifierList = new (ELeave) CArrayPtrFlat<MEikSrvNotifierBase2>(2);
    CleanupStack::PushL(notifierList);

    CVpnDialogManagerEcomStub* dlgManager = new (ELeave) CVpnDialogManagerEcomStub();
    CleanupStack::PushL(dlgManager);
    
    CVpnNotifier* notifier = new (ELeave) CVpnNotifier(KUidVpnDialogNotifier, 
                                                       KVpnNotifierResource, 
                                                       dlgManager);
    CleanupStack::PushL(notifier);
    
    notifierList->AppendL(notifier);
    
    CleanupStack::Pop(3); // notifierList, dlgManager, notifier
    
    return notifierList;
    }

CArrayPtr<MEikSrvNotifierBase2>* NotifierArray()
    {
    CArrayPtrFlat<MEikSrvNotifierBase2>* notifierList = NULL;
    
    TRAPD(err, notifierList = createNotifierArrayL()); 
    if(err)
        {
        notifierList->ResetAndDestroy();
        delete notifierList;
        notifierList = NULL;
        }
           
    return notifierList;
    }

const TImplementationProxy ImplementationTable[]=
    {
    IMPLEMENTATION_PROXY_ENTRY(0x10200EC8, NotifierArray)
    };

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable)/sizeof(TImplementationProxy);
    return ImplementationTable;
    }




CVpnNotifier::CVpnNotifier(const TUid& aNotifierUid, const TDesC& aResourceFilename,
                               CVpnDialogManagerEcomStub* aDialogManager)
    {
    LOG_("-> CVpnNotifier::CVpnNotifier()");
    iEikEnv = CEikonEnv::Static();
    iNotifierUid = aNotifierUid;
    iResourceFilename.Set(aResourceFilename);
    iDialogManager = aDialogManager;
    // Added to overcome uninstallation problems
    iResourceFileLoaded = EFalse;
    LOG_("<- CVpnNotifier::CVpnNotifier()");
    }

CVpnNotifier::~CVpnNotifier()
    {
    LOG_("-> CVpnNotifier::~CVpnNotifier()");
    delete iDialogManager;
    LOG_("<- CVpnNotifier::~CVpnNotifier()");
    }

CVpnNotifier::TNotifierInfo CVpnNotifier::RegisterL()
    {
    LOG_("-> CVpnNotifier::RegisterL()");

    iInfo.iUid = iNotifierUid;
    iInfo.iChannel = KCrystalScreenOutputChannel;
    iInfo.iPriority = ENotifierPriorityLow;

    LOG_("<- CVpnNotifier::RegisterL()");

    return iInfo;
    }

// Added to overcome uninstallation problems    
void CVpnNotifier::LoadResourceFileL()
    {
    LOG_("-> CVpnNotifier::LoadResourceFileL()");
    if (!iResourceFileLoaded)
        {
        TFileName fileName = GetResourceFileNameL();
        iResourceFileOffset = iEikEnv->AddResourceFileL(fileName);
        iResourceFileLoaded = ETrue;
        }

    LOG_("<- CVpnNotifier::LoadResourceFileL()");
    }


void CVpnNotifier::CloseManagerDll() 
    {
    if (iRealDialogManager) 
        {
        LOG_(" Closing old dialog manager instance");


        LOG_(" Deleting");
        delete iRealDialogManager;
        iRealDialogManager = NULL;

        iDialogManager->SetDialogManager(NULL);

        LOG_(" Closing library");

        iDialogManagerLib.Close();
        
        LOG_(" Close completed");
        }
    }

// Added to overcome uninstallation problems
void CVpnNotifier::UnloadResourceFile()
    {
    LOG_("-> CVpnNotifier::UnloadResourceFile()");


    if (iResourceFileLoaded)
        {
        iEikEnv->DeleteResourceFile(iResourceFileOffset);
        iResourceFileLoaded = EFalse;
        }



    LOG_("<- CVpnNotifier::UnloadResourceFile()");
    }
    
CVpnNotifier::TNotifierInfo CVpnNotifier::Info() const
    {
    return iInfo;
    }

void CVpnNotifier::StartL(const TDesC8& aBuffer, TInt aReturnValue, const RMessagePtr2& aMessage)
    {
    LOG_("-> CVpnNotifier::StartL()");
    __ASSERT_DEBUG(aBuffer.Length() >= 4, _L("Input buffer is empty"));

    // Only one dialog can be shown at a time
    if (iDialog)
        {
        User::Leave(KErrInUse);
        }
    
    // Store output data references
    iMessage = aMessage;
    iReturnValue = aReturnValue;

    // Get input data to the local memory space
    HBufC8* input = HBufC8::NewL(aBuffer.Length());
    CleanupStack::PushL(input);
    input->Des().Copy(aBuffer);

    // The input consist of two parts in a single descriptor:
    // TVpnDialogInfo and the actual input that goes to the dialogs

    // First analyze the TVpnDialogInput info
    TVpnDialogInfo dialogInfo;
    TPckg<TVpnDialogInfo> pkgDialogInfo(dialogInfo);
    pkgDialogInfo.Copy(input->Left(sizeof(TVpnDialogInfo))); // 8 bit buffer -> size = length

    TInt dialogId = dialogInfo.DialogId();
    TInt noteDialogId = dialogInfo.NoteDialogId();
    

    // Then store the actual dialog input for later use
    TInt strLen = dialogInfo.NoteExtraInput().Length();
    if (strLen > 0) 
        {
        // Additional extra input (i.e. string replacement) was specified
        // in the dialog info structure, take it into use and pass it on.
        iInput = dialogInfo.NoteExtraInput().AllocL();
        }
    else 
        {
        iInput = HBufC8::NewL(input->Size() - sizeof(TVpnDialogInfo));
        iInput->Des().Copy(input->Mid(sizeof(TVpnDialogInfo)));
        }

    CleanupStack::PopAndDestroy(); // input

    if (!LaunchDialogL(dialogId, noteDialogId, *iInput))
        {
        User::Leave(KErrNotSupported);
        }
    LOG_("<- CVpnNotifier::StartL()");
    }

TPtrC8 CVpnNotifier::StartL( const TDesC8& /*aBuffer*/ )
    {
    LOG_("-> CVpnNotifier::StartL()");
    
    __ASSERT_DEBUG(EFalse, User::Panic(_L("Synchronous StartL not used"), 0)); // should here be ret KNullDesC8() also?
    
    LOG_("<- CVpnNotifier::StartL()");
    
    return NULL;
    }

TPtrC8 CVpnNotifier::UpdateL( const TDesC8& /*aBuffer*/ )
    {
    LOG_("-> CVpnNotifier::UpdateL()");

    LOG_("<- CVpnNotifier::UpdateL()");

    return KNullDesC8();
    }

void CVpnNotifier::Cancel()
    {
    LOG_("-> CVpnNotifier::Cancel()");
    if (iDialog)
        {
        TRAP_IGNORE(iDialog->CancelL());
        iDialog = NULL;
        }
    LOG_("<- CVpnNotifier::Cancel()");
    }

void CVpnNotifier::Release()
    {
    LOG_("-> CVpnNotifier::Release()");

    UnloadResourceFile();
    CloseManagerDll();
    delete this;

    LOG_("<- CVpnNotifier::Release()");
    }

TBool CVpnNotifier::LaunchDialogL(TInt aDialogId, TInt aNoteDialogId, const TDesC8& aInput)
    {
    LOG_("-> CVpnNotifier::LaunchDialogL()");
	((CEikServAppUi*)(CEikonEnv::Static())->EikAppUi())->SuppressAppSwitching(ETrue);

    // To ensure that we indeed do get the most recent (possibly updated) version
    // of both the resource file, and the DLL that uses resource IDs.
    UnloadResourceFile();
    LoadResourceFileL();
    
    CloseManagerDll();
    LoadManagerDllL();

    iDialog = iDialogManager->LaunchDialogL(this, aDialogId, aNoteDialogId, aInput);
    if (iDialog)
        {
        LOG_("<- CVpnNotifier::LaunchDialogL() (ETrue)");
        return ETrue;
        }
    else
        {
        UnloadResourceFile();
        CloseManagerDll();
        LOG_("<- CVpnNotifier::LaunchDialogL() (couldn't instantiate dialog)");
        return EFalse;
        }
    }
    
TFileName CVpnNotifier::GetResourceFileNameL()
    {
    LOG_("-> CVpnNotifier::GetResourceFileNameL()");
    RFs& fsSession = iEikEnv->FsSession(); 

    TDriveList driveList;
    User::LeaveIfError(fsSession.DriveList(driveList));

    TInt driveNumber;
    TDriveName drive = _L("a:");

    TFileName resName;
    TInt foundIt = EFalse;

    LOG_(" Starting drive letter iteration");
    for (driveNumber = EDriveA, drive[0] = 'a';
         driveNumber <= EDriveZ;
         driveNumber++, drive[0]++)
        {
        if (!driveList[driveNumber])
            {
            LOG_("  Continue");
            continue;
            }

        TParse parse;
        parse.Set(drive, &iResourceFilename, NULL);

        resName.Copy(parse.FullName()); 
        
        LOG_1("  Resource filename: '%S'", &resName);

        BaflUtils::NearestLanguageFile(iEikEnv->FsSession(), resName);

        TEntry entry;
        if (fsSession.Entry(resName, entry) == KErrNone)
            {
            LOG_("  Found it!");
            foundIt = ETrue;
            break;
            }
        }

    if (!foundIt)
        {
        LOG_(" Didn't find it!");
        User::Leave(KErrNotFound);
        }

    LOG_("<- CVpnNotifier::GetResourceFileNameL()");

    // If the file name was found, it will stay in the cleanup stack
    return resName;
    }

void CVpnNotifier::DialogCompleteL(TInt aReturnCode, TVpnDialogOutput& aOutput)
    {
    LOG_("-> CVpnNotifier::DialogCompleteL()");

    // NOTE! this doesn't work with own notifier server 
    // Activate apps -key again
    //
	((CEikServAppUi*)(CEikonEnv::Static())->EikAppUi())->SuppressAppSwitching(EFalse);

    TPckgBuf<TVpnDialogOutput> outBuf(aOutput);
    iMessage.WriteL(iReturnValue, outBuf);
    iMessage.Complete(aReturnCode);
    iDialog = NULL;
    delete iInput; iInput = NULL;
    // Added to overcome uninstallation problems
    UnloadResourceFile();
    iManager->CancelNotifier(iNotifierUid);

    LOG_("<- CVpnNotifier::DialogCompleteL()");

    }

void CVpnNotifier::DialogComplete(TInt aReturnCode)
    {
    LOG_("-> CVpnNotifier::DialogComplete()");

    // NOTE! this doesn't work with own notifier server 
    // Activate apps -key again
    //
	((CEikServAppUi*)(CEikonEnv::Static())->EikAppUi())->SuppressAppSwitching(EFalse);

    iMessage.Complete(aReturnCode);
    iDialog = NULL;
    delete iInput; iInput = NULL;
    // Added to overcome uninstallation problems
    UnloadResourceFile();
    iManager->CancelNotifier(iNotifierUid);    

    LOG_("<- CVpnNotifier::DialogComplete()");

    }

void CVpnNotifier::LoadManagerDllL() 
    {
    LOG_("-> CVpnNotifier::LoadManagerDll()");

    LOG_(" Loading manager again");

    TInt status = iDialogManagerLib.Load(KVpnDialogManagerDll, KNullUid);

    LOG_1(" DLL load status: %d", status);

    if (status == KErrNone)
        {
        LOG_(" Suitable DLL found, instantiating VPN dialog manager");
        
        TLibraryFunction entry = iDialogManagerLib.Lookup(1);
        
        LOG_(" Entry point found");

        // Constructor at the given ordinal may leave, even if it's not evident
        // from the syntax
        iRealDialogManager = (MVpnDialogManager*)entry();
        iDialogManager->SetDialogManager(iRealDialogManager);

        LOG_(" Dialog manager instantiated");
        }
    else 
        {
        LOG_(" (LEAVE) Dialog manager DLL not found!");
        User::Leave(status);
        }

    LOG_("<- CVpnNotifier::LoadManagerDll() (OK)");
    }





