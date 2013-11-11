 /*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Controls the vpn policy import procedure.
*
*/

#include <e32std.h>
#include <apacmdln.h>
#include <bautils.h>
#include <sysutil.h>
#include <securityerr.h>

#include "policyinstaller.h"
#include "vpnnotifierdefs.h"
#include "vpnpolicyrecognizerconstants.h"
#include "zipfiledecompressor.h"
#include "policyinstaller_constants.h"

// Device lock / PKCS#12 installation
#include "vpnbundlehandler.h"
#include "cmdfileparser.h"
#include "pkcs12vpn.h"
#include "cmdfilexmltags.h"
#include "vpndevlockhandler.h"


#include "log_r6.h"


void RunServerL();

GLDEF_C TInt E32Main()
    {
    __UHEAP_MARK;
    
    CTrapCleanup* cleanupStack = CTrapCleanup::New();
    if (!cleanupStack)
        {
        return KErrNoMemory;
        }
        
    TRAP_IGNORE(RunServerL());

    delete cleanupStack;
    
    __UHEAP_MARKEND;
    return KErrNone;
    }

void RunServerL()
    {
    LOG_("-> VPNPolins RunServerL()");


    // Create and install the active scheduler
    LOG_(" Instantiate active scheduler()");
    CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
    CleanupStack::PushL(scheduler);
    LOG_(" Installing active scheduler()");
    CActiveScheduler::Install(scheduler);

    // Create the installer active object and issue the install request
    LOG_(" Instantiating CVpnPolicyInstaller");
    CVpnPolicyInstaller* installer = CVpnPolicyInstaller::NewL();
    CleanupStack::PushL(installer);

    LOG_(" Installing policy");
    installer->InstallPolicy();

    // Now we can start the active scheduler
    LOG_(" Starting active scheduler");
    CActiveScheduler::Start();


    LOG_(" Returning to RunServerL()");

    // We're done, so remove objects from
    // cleanup stack and destroy them

    LOG_(" Pop installer");
    CleanupStack::PopAndDestroy(installer);

    LOG_(" Pop scheduler");
    CleanupStack::PopAndDestroy(scheduler);

    LOG_("<- VPNPolins RunServerL()");
    }


/////////////////////////////////////
// CVpnPolicyInstaller class implementation
    
CVpnPolicyInstaller* CVpnPolicyInstaller::NewL()
    {
    CVpnPolicyInstaller* self = new (ELeave) CVpnPolicyInstaller();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CVpnPolicyInstaller::CVpnPolicyInstaller() : CActive(EPriorityStandard)
    {
    }

void CVpnPolicyInstaller::ConstructL()
    {
    LOG_("-> CVpnPolicyInstaller::ConstructL()");
    CActiveScheduler::Add(this);
    User::LeaveIfError(iNotifier.Connect());    
    User::LeaveIfError(iFileServer.Connect());
    

    TInt err =
        CApaCommandLine::GetCommandLineFromProcessEnvironment(iCommandLine);        
    User::LeaveIfError(err);        


    if (iCommandLine->DocumentName().Length() == 0)
        {
        User::Leave(KErrArgument);
        }
    #ifdef ATS_BUILD
    	_LIT(KPassword, "password");
    	iP12Password = HBufC::NewL(KPassword().Length());
    	iP12Password->Des().Copy(KPassword);
    #endif
    LOG_("<- CVpnPolicyInstaller::ConstructL()");
    }

CVpnPolicyInstaller::~CVpnPolicyInstaller()
    {
    LOG_("-> CVpnPolicyInstaller::~CVpnPolicyInstaller()");
    Cancel();        
    
    delete iDecompressor;
    delete iDriveAndPath;
    delete iCommandLine;
    delete iFormat;
    delete iFileName;
    delete iP12Password;
    delete iWaitNoteHandler;
    
    iFileServer.Close();    
    iNotifier.Close();
    iVpnServ.Close();
    
    LOG_("<- CVpnPolicyInstaller::~CVpnPolicyInstaller()");
    }

void CVpnPolicyInstaller::InstallPolicy()
    {
    LOG_("-> CVpnPolicyInstaller::InstallPolicy()");
    TRAPD(err, InstallPolicyL());
    if (err != KErrNone)
        {
        LOG_1(" Policy installation terminated with error code %d", err);
        
        // Show the error note before completing.
        ShowImportErrorNote(err);
        }
    LOG_("<- CVpnPolicyInstaller::InstallPolicy()");
    }

void CVpnPolicyInstaller::InstallPolicyL()
    {
    LOG_("-> CVpnPolicyInstaller::InstallPolicyL()");
    
    __ASSERT_ALWAYS(iCommandLine, User::Invariant());

    // Disk space criticality check before attempting
    // to install
    if (SysUtil::FFSSpaceBelowCriticalLevelL(0, 0)) 
        {
        User::Leave(KErrDiskFull);
        }

    // Parse the filename that is to be operated on,
    // and deduce its type (ZIP, PIN etc)

    HBufC* document = iCommandLine->DocumentName().AllocLC();        
    TPtr documentPtr(document->Des());
    TParsePtr parse(documentPtr);
    TPtrC extension = parse.Ext();
    iFormat = extension.AllocL();
    
    iFileName = HBufC8::NewL(parse.NameAndExt().Length());
    TPtr8 des = iFileName->Des();
    des.Copy(parse.NameAndExt());


    if (extension.CompareF(KVpnZipPolicyFileExtension) == 0)
        {                
        LOG_(" Policy is in a VPN zip file");
        ContinueInstallPolicyL();
        }
    else if (extension.CompareF(KVpnPinPolicyFileExtension) == 0)
        {        
        LOG_(" Policy is unpacked pol/pin file");
        iDriveAndPath = parse.DriveAndPath().AllocL();
        ContinueInstallPolicyL();
        }
    else
        {
        LOG_(" Policy is in an unknown format, leaving");
        LOG_("<- CVpnPolicyInstaller::InstallPolicyL() (leave)");
        User::Leave(KErrArgument);
        }        

    CleanupStack::PopAndDestroy(document);        
    LOG_("<- CVpnPolicyInstaller::InstallPolicyL()");
    }

void CVpnPolicyInstaller::ContinueInstallPolicyL() 
    {
    LOG_("-> CVpnPolicyInstaller::ContinueInstallPolicyL()");

    if (iFormat->CompareF(KVpnZipPolicyFileExtension) == 0)
        {
        DeflateZippedPolicyL();
        }
    else if (iFormat->CompareF(KVpnPinPolicyFileExtension) == 0)
        {        
        InstallPinPolicyL(*iDriveAndPath);
        }
    else
        {
        LOG_("<- CVpnPolicyInstaller::ContinueInstallPolicyL() (leave)");
        User::Leave(KErrArgument);
        }
    LOG_("<- CVpnPolicyInstaller::ContinueInstallPolicyL()");
    }

void CVpnPolicyInstaller::InstallPinPolicyL(const TDesC& aFileDirectory)
    {
    LOG_("-> CVpnPolicyInstaller::InstallPinPolicy()");

    User::LeaveIfError(iVpnServ.Connect());

    iVpnServ.ImportPolicy(aFileDirectory, iStatus);
    iOngoingTask = ETaskImportPinPolicies;
    SetActive();
    LOG_("<- CVpnPolicyInstaller::InstallPinPolicy()");
    }


void CVpnPolicyInstaller::DeflateZippedPolicyL()
    {   
    LOG_("-> CVpnPolicyInstaller::DeflateZippedPolicyL()");

    LOG_(" Showing wait note");
    ShowWaitNoteL(TVpnDialog::EGenericProgress);
    
    __ASSERT_ALWAYS(!iDecompressor, User::Invariant());  

    iOngoingTask = ETaskImportZippedPolicies;

    iDriveAndPath = KTempDirectory().AllocL();
    iOngoingTask = ETaskImportZippedPolicies;
             
    //Makes sure the temp dir is empty
    if (BaflUtils::FolderExists(iFileServer, KTempDirectory))
        {
        EmtpyTempDirL();
        }

    iDecompressor = CZipFileDecompressor::NewL();              
    
    RFile file;
    iCommandLine->GetFileByHandleL(file);

    if (file.SubSessionHandle())
        {        
        iDecompressor->DecompressZipFile(file, KTempDirectory, iStatus);
        }
    else
        {
        iDecompressor->DecompressZipFile(iCommandLine->DocumentName(), KTempDirectory, iStatus);
        }
    SetActive();

    LOG_("<- CVpnPolicyInstaller::DeflateZippedPolicyL()");
    }

void CVpnPolicyInstaller::EmtpyTempDirL()
    {
    LOG_("-> CVpnPolicyInstaller::EmtpyTempDirL()");
    CDir* dir = NULL;
    User::LeaveIfError(iFileServer.GetDir(KTempDirectory, KEntryAttNormal, ESortByName, dir));
    CleanupStack::PushL(dir);
    
    
    TFileName* fileName = new (ELeave) TFileName;
    CleanupStack::PushL(fileName);
    
    for (TInt i = 0; i < dir->Count(); ++i)
        {
        const TEntry& entry = (*dir)[i];
        *fileName = KTempDirectory;
        fileName->Append(entry.iName);
        User::LeaveIfError(iFileServer.Delete(*fileName));        
        }
    
    CleanupStack::PopAndDestroy(fileName);
    CleanupStack::PopAndDestroy(dir);
    LOG_("<- CVpnPolicyInstaller::EmtpyTempDirL()");
    }


void CVpnPolicyInstaller::DoCancel()
    {
    LOG_("-> CVpnPolicyInstaller::DoCancel()");
    
    CancelWaitNote();

    switch(iOngoingTask)
        {
        case ETaskImportZippedPolicies:
            LOG_(" case ETaskImportZippedPolicies");
            if (iDecompressor) 
                {
                iDecompressor->Cancel();
                delete iDecompressor;
                iDecompressor = NULL;
                }
            break;

        case ETaskImportPinPolicies:
            LOG_(" case ETaskImportPinPolicies");

            iVpnServ.CancelImport();

            delete iDriveAndPath;
            iDriveAndPath = NULL;
            break;

        case ETaskShowImportErrorNote:
            LOG_(" case ETaskShowImportErrorNote");
            LOG_1(" (IMPERR) Terminating dialog, iStatus: %d", iStatus.Int());
            iNotifier.CancelNotifier(KUidVpnDialogNotifier);
            break;

        default:
            LOG_(" case default");
            User::Invariant();
            break;            
        } 
        
    TRAP_IGNORE(EmtpyTempDirL());
    LOG_("<- CVpnPolicyInstaller::DoCancel()");
    }

TInt CVpnPolicyInstaller::RunError(TInt aError)
    {
    LOG_1("-> CVpnPolicyInstaller::RunError() code: %d", aError);
    
    CancelWaitNote();

    switch(iOngoingTask)
        {
        case ETaskImportZippedPolicies:
            LOG_(" case ETaskImportZippedPolicies");
            if (iDecompressor) 
                {
                LOG_("  Found decompressor, terminating");
                iDecompressor->Cancel();
                delete iDecompressor;
                iDecompressor = NULL;
                }


            LOG_("  case handled");
            break;

        case ETaskImportPinPolicies:
            LOG_(" case ETaskImportPinPolicies");

            iVpnServ.CancelImport();

            delete iDriveAndPath;
            iDriveAndPath = NULL;
            break;

        case ETaskShowImportErrorNote:
            LOG_(" case ETaskShowImportErrorNote");
            LOG_1("  (IMPERR) Terminating dialog, iStatus: %d", iStatus.Int());
            iNotifier.CancelNotifier(KUidVpnDialogNotifier);
            break;

        default:
            LOG_(" case default");
            User::Invariant();
            break;
        }

    TRAP_IGNORE(EmtpyTempDirL());

    // Show error note unless the operation was cancelled
    // (in which case we will just exit silently)
    if (aError != KErrCancel && aError != KErrBadPassphrase) 
        {
        ShowImportErrorNote(aError);
        }

	CActiveScheduler::Stop();

    LOG_("<- CVpnPolicyInstaller::RunError()");
    return KErrNone;
    }

void CVpnPolicyInstaller::RunL()
    {
    
    LOG_("-> CVpnPolicyInstaller::RunL()");
    TInt status = iStatus.Int();
    TBool finished(EFalse);

    switch(iOngoingTask)
        {
        case ETaskImportZippedPolicies:
            LOG_(" Case ETaskImportZippedPolicies");
            delete iDecompressor;
            iDecompressor = NULL;

        	if (status == KErrNone)
            	{            	
                LOG_(" Status: KErrNone, continuing installation");

                ProcessBundleContentsL();

                // Wait note has been cancelled in ProcessBundleContentsL
                ShowWaitNoteL();

            	InstallPinPolicyL(*iDriveAndPath);
            	}
        	else
            	{
                LOG_1(" Status: %d, aborting installation", status);
    			// Installation FAILED
    			// Policy import failed, so show an error note
                // (asynchronous call, RunL called again on completion)
    			iOngoingTask = ETaskShowImportErrorNote;
            	ShowImportErrorNote();
                finished = ETrue;
            	}        
            break;

        case ETaskImportPinPolicies:
            LOG_(" Case ETaskImportPinPolicies");

            // We're finished, so clean up and terminate
            TRAP_IGNORE(EmtpyTempDirL());

            if (status == KErrNone) 
                {
                // Finally, only apply devlock settings
                // if we've come this far without errors
                TRAP(status, ApplyDevLockSettingsL());
                }

            CancelWaitNote();

            delete iDriveAndPath;
            iDriveAndPath = NULL;
            
            LOG_1(" Status: %d", status);
        	if (status == KErrNone)
            	{
                // Everything went OK. Show completion note and finish.

                LOG_(" Completing");
            	iOngoingTask = ETaskNone;
            	
                TVpnDialogInfo dialogInfo(TNoteDialog::EInfo, 
                                          TVpnNoteDialog::EVpnPolicyInstalled);
                iDialogInfoDes() = dialogInfo;

                LOG_(" Starting 'finished' dialog");
                
                iNotifier.StartNotifierAndGetResponse(iStatus, 
                                                      KUidVpnDialogNotifier,
                                                      iDialogInfoDes, 
                                                      iDialogResponseDes);
                User::WaitForRequest(iStatus);


                LOG_(" Request complete, terminating");

             	// We're done, so cancel the completion note, and terminate
                iNotifier.CancelNotifier(KUidVpnDialogNotifier);
                finished = ETrue;
            	}
        	else if (status != KErrCancel)
            	{
    			// Policy import failed, so show an error note
    			// and terminate
                LOG_(" Installation failed, showing import error note");
    			iOngoingTask = ETaskShowImportErrorNote;
            	ShowImportErrorNote();
                finished = ETrue;
            	}
            else 
                {
                LOG_(" Installation was cancelled, silent exit");
                finished = ETrue;
                }
            break;

        case ETaskShowImportErrorNote:
            LOG_(" Case ETaskShowImportErrorNote");

            // Cancel
            LOG_1(" (imperr) Terminating dialog, status: %d", status);
            iNotifier.CancelNotifier(KUidVpnDialogNotifier);
            iOngoingTask = ETaskNone;

            // We're done, so terminate
            finished = ETrue;
            break;

        default:
            LOG_(" Case default");
            User::Invariant();
        }

    if (finished)
        {
        LOG_(" Shutting down active scheduler, all tasks finished");
    	CActiveScheduler::Stop();
        }

    LOG_("<- CVpnPolicyInstaller::RunL()");
    }
    
void CVpnPolicyInstaller::ShowImportErrorNote(const TInt aReason)
    {
    LOG_("-> CVpnPolicyInstaller::ShowImportErrorNote()");
    

    TInt reasonCode(TVpnNoteDialog::EPolicyInstallationFailed);

    // Check if we can display a more specific error note
    if (aReason == KErrAccessDenied) 
        {
        reasonCode = TVpnNoteDialog::EVpnCannotForceDeviceLock;
        }

    TVpnDialogInfo dialogInfo(TNoteDialog::EInfo, reasonCode);
    iDialogInfoDes() = dialogInfo;

    TRequestStatus status;
    
    iNotifier.StartNotifierAndGetResponse(status, KUidVpnDialogNotifier,
                                          iDialogInfoDes, iDialogResponseDes);

    // Wait until error note completes
    User::WaitForRequest(status);
    iNotifier.CancelNotifier(KUidVpnDialogNotifier);

    LOG_("<- CVpnPolicyInstaller::ShowImportErrorNote()");
    }

TBool CVpnPolicyInstaller::ShowDevLockConfirmNote()
    {
    LOG_("-> CVpnPolicyInstaller::ShowDevLockConfirmNote()");
    TBool ret(EFalse);
    TRequestStatus status;

    TVpnDialogInfo dialogInfo(TVpnDialog::EDeviceLockConfirmation, 0, *iFileName);

    dialogInfo.SetExtraInput(*iFileName);

    iDialogInfoDes() = dialogInfo;

    iNotifier.StartNotifierAndGetResponse(status, KUidVpnDialogNotifier,
                                          iDialogInfoDes, iDialogResponseDes);

    // Wait until error note completes
    User::WaitForRequest(status);

    if (status != KErrCancel) 
        {
        ret = ETrue;
        }

    LOG_1("<- CVpnPolicyInstaller::ShowDevLockConfirmNote() ret: %d", ret);
    return ret;
    }

TBool CVpnPolicyInstaller::ShowConfirmNote() 
    {
    LOG_("-> CVpnPolicyInstaller::ShowConfirmNoteL()");

    TBool ret(EFalse);
    TRequestStatus status;

    TVpnDialogInfo dialogInfo(TVpnDialog::EPolicyInstallConfirmation, 0, *iFileName);

    dialogInfo.SetExtraInput(*iFileName);

    iDialogInfoDes() = dialogInfo;
    
    iNotifier.StartNotifierAndGetResponse(status, KUidVpnDialogNotifier,
                                          iDialogInfoDes, iDialogResponseDes);

    // Wait until error note completes
    User::WaitForRequest(status);

    if (status != KErrCancel) 
        {
        ret = ETrue;
        }

    LOG_1("<- CVpnPolicyInstaller::ShowConfirmNoteL(), ret: %d", ret);
    return ret;
    }

void CVpnPolicyInstaller::ShowWaitNoteL(TInt aNoteId)
    {

    LOG_("-> CVpnPolicyInstaller::ShowWaitNoteL()");
    
    iWaitNoteHandler = CImportWaitNoteHandler::NewL(this);

    iWaitNoteHandler->StartWaitNote(aNoteId);

    LOG_("<- CVpnPolicyInstaller::ShowWaitNoteL()");
    }

void CVpnPolicyInstaller::CancelWaitNote() 
    {
    LOG_("-> CVpnPolicyInstaller::CancelWaitNote()");
    if (iWaitNoteHandler)
        {
        iWaitNoteHandler->Cancel();
        delete iWaitNoteHandler;
        iWaitNoteHandler = NULL;
        }
    LOG_("<- CVpnPolicyInstaller::CancelWaitNote()");
    }

void CVpnPolicyInstaller::UserCancelledWait() 
    {
    LOG_("-> CVpnPolicyInstaller::UserCancelledWait()");

    DoCancel();

    LOG_("<- CVpnPolicyInstaller::UserCancelledWait()");
    }




/////////////////////////////////////////////////////////////
// Device lock / pkcs#12 installation specific part
// of implementation

void CVpnPolicyInstaller::ApplyDevLockSettingsL() 
    {
    // See whether change to devlock settings was requested
    if (iDevLockPolicy > 0) 
        {
        // If device supports VPN devlock operations, set the settings
        // Otherwise it's an error -- unless device lock was 
        // defined "optional" in the VPN command file
        if (iDevSupportsDevLock) 
            {
            CVpnDevLockHandler* dlh = CVpnDevLockHandler::NewLC();

            dlh->SetTerminalControlPolicyL(iDevLockPolicy);
            CleanupStack::PopAndDestroy(dlh);
            }
        else if (iDevLockRequired)
            {
            User::Leave(KErrAccessDenied);
            }
        }
    }
void CVpnPolicyInstaller::ProcessBundleContentsL() 
    {
    LOG_("-> CVpnPolicyInstaller::ProcessBundleContentsL()");


    CVpnBundleHandler* bundler = CVpnBundleHandler::NewLC(KTempDirectory);

    bundler->AnalyzeBundleContentsL();

    if (bundler->CommandFileExists()) 
        {
        LOG_(" Bundle command file exists");
        HBufC8* cmdXml = bundler->ExtractCommandFileDataL();
        CleanupStack::PushL(cmdXml);

        ExtractCommandParametersL(*cmdXml);

        CleanupStack::PopAndDestroy(cmdXml);
        }


    TBool okToContinue = ConfirmDeviceLockUseL();

    if (okToContinue) 
        {
        // Extract PKCS#12-contained security objects and save them to disk
        if (bundler->PKCS12FileExists())
            {
            LOG_(" Bundle P12 file exists");
            HBufC8* pkcs12 = bundler->ExtractPKCS12DataL();
            CleanupStack::PushL(pkcs12);

            if (iP12Password) 
                {
                ExtractPKCS12ObjectsL(*pkcs12, *iP12Password);
                }
            else 
                {
                ExtractPKCS12ObjectsL(*pkcs12, KNullDesC);
                }

            CleanupStack::PopAndDestroy(pkcs12);
            }
        }
    else 
        {
        LOG_(" User cancelled device lock install, halting import");
        User::Leave(KErrCancel);
        }

    CleanupStack::PopAndDestroy(bundler);

    LOG_("<- CVpnPolicyInstaller::ProcessBundleContentsL()");
    }

void CVpnPolicyInstaller::ExtractPKCS12ObjectsL(const TDesC8& aData, const TDesC& aPwd) 
    {
    LOG_("-> CVpnPolicyInstaller::ExtractPKCS12ObjectsL()");
    CPKCS12Handler* p12handler = CPKCS12Handler::NewLC();
    p12handler->SaveSecurityObjectsToDiskL(aData, aPwd, KTempDirectory);
    CleanupStack::PopAndDestroy(p12handler);
    LOG_("<- CVpnPolicyInstaller::ExtractPKCS12ObjectsL()");
    }

void CVpnPolicyInstaller::ExtractCommandParametersL(const TDesC8& aData) 
    {
    LOG_("-> CVpnPolicyInstaller::ExtractCommandParametersL()");

    CCmdFileParser* parser = new (ELeave) CCmdFileParser();
    CleanupStack::PushL(parser);

    parser->ParseL(aData);
    
    #ifdef ATS_BUILD
		delete iP12Password; iP12Password = NULL;
	#endif

    iP12Password = parser->GetParsedValueStrL(KCmdPKCS12PasswordUri);
    parser->GetParsedValueIntL(KCmdDevlockPolicyUri, iDevLockPolicy);
    parser->GetParsedValueBoolL(KCmdDevlockRequiredUri, iDevLockRequired);

    if (iP12Password)
        {
        LOG_1(" PASSWORD: '%S'", iP12Password);
        }

    LOG_1(" Requested devlock policy: %d", iDevLockPolicy);
    LOG_1(" Devlock mandatory: %d", iDevLockRequired);
    LOG_(" Popping from stack");

    CleanupStack::PopAndDestroy(parser);

    LOG_("<- CVpnPolicyInstaller::ExtractCommandParametersL()");
    }

TInt CVpnPolicyInstaller::IsDeviceLockAvailable()
    {
    LOG_("-> CVpnPolicyInstaller::IsDeviceLockAvailableL()");
    TInt status(KErrNone);

    CVpnDevLockHandler* dlh = CVpnDevLockHandler::NewLC();

    TRAP(status, dlh->TerminalControlSupportsVpnL());
    if (status == KErrNone) 
        {
        iDevSupportsDevLock = ETrue;
        }
    else 
        {
        iDevSupportsDevLock = EFalse;
        }

    CleanupStack::PopAndDestroy(dlh);

    LOG_1("<- CVpnPolicyInstaller::IsDeviceLockAvailableL() ret: %d", status);
    return status;
    }

TBool CVpnPolicyInstaller::ConfirmDeviceLockUseL() 
    {
    TBool ret(EFalse);

    // If device lock was requested, check whether we can use it
    if (iDevLockPolicy > 0)
        {
        TInt dlError = IsDeviceLockAvailable();
        LOG_1(" Device lock status: %d", dlError);

        if (dlError != KErrNone)
            {
            // We might still be able to continue, 
            // depending on whether device lock configuration
            // is mandatory (or just optional).
            // Mandatory status is defined in the vpn cmd file.
            if (dlError != KErrAccessDenied) 
                {
                User::Leave(dlError);
                }

            if (iDevLockRequired)
                {
                LOG_1("<- CVpnPolicyInstaller::ProcessBundleContentsL() LEAVE: %d", 
                      dlError);
                User::Leave(dlError);
                }
            else 
                {
                CancelWaitNote();
                LOG_(" Couldn't set device lock, but it's not mandatory. Proceeding");
                ret = ShowConfirmNote();
                }
            }
        else 
            {
            CancelWaitNote();
            LOG_(" Showing device lock confirmation note");
            ret = ShowDevLockConfirmNote();
            }
        }
    else 
        {
        CancelWaitNote();
        LOG_(" No changes required to device lock");
        #ifdef ATS_BUILD
        	ret = ETrue;
        #else
	        ret = ShowConfirmNote();
	    #endif
        }
    return ret;
    }



////////////////////////////////////////////////////////////////
// WaitNoteHandler class implementation

CImportWaitNoteHandler::CImportWaitNoteHandler(CVpnPolicyInstaller* aCallBack) : 
                        CActive(EPriorityStandard), iCallBack(aCallBack)
    {
    LOG_("-> CImportWaitNoteHandler::CImportWaitNoteHandler()");
    CActiveScheduler::Add(this);
    LOG_("-> CImportWaitNoteHandler::CImportWaitNoteHandler()");
    }

CImportWaitNoteHandler::~CImportWaitNoteHandler() 
    {
    LOG_("-> CImportWaitNoteHandler::~CImportWaitNoteHandler()");
    Cancel();
    LOG_("<- CImportWaitNoteHandler::~CImportWaitNoteHandler()");
    }

void CImportWaitNoteHandler::DoCancel() 
    {
    LOG_("-> CImportWaitNoteHandler::DoCancel()");
    
    iNotifier.CancelNotifier(KUidVpnDialogNotifier);
    iNotifier.Close();

    LOG_("<- CImportWaitNoteHandler::DoCancel()");
    }
    
void CImportWaitNoteHandler::ConstructL() 
    {
    User::LeaveIfError(iNotifier.Connect());
    }

// Codescanner complains about non-leaving L function.
// Sadly, there's little we can do...
void CImportWaitNoteHandler::RunL() 
    {
    LOG_("-> CImportWaitNoteHandler::RunL()");
    TInt status = iStatus.Int();
    LOG_1(" Status: %d", status);
    if (status == KErrCancel) 
        {
        LOG_(" CANCEL");
        iCallBack->UserCancelledWait();
        }

    LOG_("<- CImportWaitNoteHandler::RunL()");
    }

TInt CImportWaitNoteHandler::RunError(TInt aError) 
    {
    LOG_1("-> CImportWaitNoteHandler::RunError() %d", aError);
    Cancel();
    LOG_("<- CImportWaitNoteHandler::RunError()");

    // Remove compiler warning in urel case
    aError = aError;

    return KErrNone;
    }

void CImportWaitNoteHandler::StartWaitNote(TInt aNoteId) 
    {
    LOG_("-> CImportWaitNoteHandler::StartWaitNote()");

    TVpnDialogInfo dialogInfo(aNoteId, 0);
    iDialogInfoDes() = dialogInfo;

    iNotifier.StartNotifierAndGetResponse(iStatus, KUidVpnDialogNotifier,
                                          iDialogInfoDes, iDialogResponseDes);

    SetActive();
    LOG_("<- CImportWaitNoteHandler::StartWaitNote()");
    }

CImportWaitNoteHandler* CImportWaitNoteHandler::NewL(CVpnPolicyInstaller* aCallBack)
    {
    CImportWaitNoteHandler* self = new (ELeave) CImportWaitNoteHandler(aCallBack);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }



