/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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




#ifndef C_VPNPOLICYINSTALLER_H
#define C_VPNPOLICYINSTALLER_H

#include <e32base.h>
#include "vpnnotifierdefs.h"

#include "vpnapi.h"
#include "vpnnotifierdefs.h"

class CZipFileDecompressor;
class CApaCommandLine;
class CVpnPolicyInstaller;

/**
 * Activeobject for wait note during policy import procedure
 *
 */
class CImportWaitNoteHandler : public CActive
    {
public:
    static CImportWaitNoteHandler* NewL(CVpnPolicyInstaller* aCallBack);
    CImportWaitNoteHandler(CVpnPolicyInstaller* aCallBack);
    ~CImportWaitNoteHandler();

protected:
    void DoCancel();
    void RunL();
    TInt RunError(TInt aError);
    
    void ConstructL();

public:
    /**
     * Starts the wait note. The exact nature of note depends on aNoteId 
     * parameter.
     *
     * @param aNoteId The note type to display. Possible values are 
     *        TVpnDialog::EPolicyInstallInProgress ("Importing"; default) 
     *        and TVpnDialog::EGenericProgress ("Processing" with no cancel)
     */
    void StartWaitNote(TInt aNoteId = TVpnDialog::EPolicyInstallInProgress);

protected:
    TPckgBuf<TVpnDialogInfo>    iDialogInfoDes;
    TPckgBuf<TVpnDialogOutput>  iDialogResponseDes;
	RNotifier                   iNotifier;
	TInt                        iStatem;
	
	// Callback used when cancel occurs
	CVpnPolicyInstaller*        iCallBack;
    };

class CVpnPolicyInstaller : public CActive
    {
public:
    static CVpnPolicyInstaller* NewL();
    ~CVpnPolicyInstaller();
    
    void InstallPolicy();
    void DialogDismissedL(TInt /*aButtonId*/) 
        {
        };
    void UserCancelledWait();
    void CancelWaitNote();
    
protected: // From CActive
    void DoCancel();
    void RunL();
    TInt RunError(TInt aError);
    
private:
    CVpnPolicyInstaller();
    void ConstructL();
    
    /**
     * Starts the policy installation process
     */
    void    InstallPolicyL();

    /**
     * Unzips the .VPN zipped file
     */
    void    DeflateZippedPolicyL();

    /**
    * "Emtpies" the temp dir that was used for zip extraction
    */
    void    EmtpyTempDirL();

    /**
     * Locate the command file from the extracted VPN file contents,
     * and parse it. Parsed variables are stored locally to CVpnPolicyInstaller
     * instance (at the moment, the variables are PKCS#12 password,
     * device lock policy level and device lock optionality information)
     */
    void    ExtractAndParseCmdFileL();

    /**
     * Install a policy that is unpacked. Used after a VPN file has been
     * decompressed, as well.
     */
    void    InstallPinPolicyL(const TDesC& aFileDirectory);

    /**
     * Shows a generic "Installation failed" note
     */
    void    ShowImportErrorNote(const TInt aReason = KErrGeneral);

    /**
     * Shows the device lock related confirmation note 
     * "Import VPN policy X? Note: devlock will be forced"
     *
     * @return ETrue iff the user gave positive confirmation
     */
    TBool   ShowDevLockConfirmNote();

    /**
     * Shows the standard confirmation note 
     * "Import VPN policy X?"
     * Used when a) no devlock used or b) devlock use is optional and
     * no device lock found / device lock not accessible by VPN.
     *
     * @return ETrue iff the user gave positive confirmation
     */
    TBool   ShowConfirmNote();

    /**
     * Shows a wait note. Default wait note is "Importing policy" (can 
     * be cancelled). For special purposes, a generic "Processing" wait 
     * note (non-cancellable) can be used.
     *
     * @param aNoteId The type of the note to display. Valid values are
     *        TVpnDialog::EPolicyInstallInProgress and
     *        TVpnDialog::EGenericProgress
     */
    void    ShowWaitNoteL(TInt aNoteId = TVpnDialog::EPolicyInstallInProgress);

    /**
     * Continues policy install operation
     *
     */
    void    ContinueInstallPolicyL();

    /**
     * After VPN zip has been decompressed, go through its contents,
     * locate the commanad / p12 files, check the device lock status,
     * parse command file, and extract p12 objects.
     *
     */
    void ProcessBundleContentsL();
    
    /**
     * Parse cmd file XML and extract the relevant parameters to local
     * instance variables
     *
     * @param aCmdData XML-formatted cmd file data, in 8bit descriptor
     *
     * @leave Leaves if oom, or if data is corrupted
     */
    void ExtractCommandParametersL(const TDesC8& aCmdData);

    /**
     * Dump PKCS#12 objects into separate files
     *
     * @param aData Raw PKCS#12 data in binary format
     * @param aPwd The password used for descrambling PKCS#12 data.
     *        If password is wrong, it will be prompted again (and again,
     *        and again...)
     *
     * @leave Leaves if PKCS#12 is corrupt or OOM
     *
     */
    void ExtractPKCS12ObjectsL(const TDesC8& aData, const TDesC& aPwd);

    /**
     * Checks whether device lock is available, and usable by VPN.
     * Returns false if it's not. Leaves in case of OOM.
     *
     * @return ETrue iff device lock is available and VPN-enabled
     */
    TBool IsDeviceLockAvailable();

    /**
     * Checks whether Device Lock is available / requested, and then
     * prompts the user about it with confirm note
     *
     * @return ETrue if user confirmed devicelock policy install
     */
    TBool ConfirmDeviceLockUseL();

    /**
     * Applies the device lock settings. This should be done only after
     * the policy has been successfully imported.
     *
     * @leave Leaves if OOM or if there are problems with device 
     *        lock access.
     *
     */
    void ApplyDevLockSettingsL();

	enum TTask
        {
        ETaskNone = 1,
        ETaskImportPinPolicies,
        ETaskImportZippedPolicies,
        ETaskShowImportErrorNote
        };
        
    
	RNotifier       iNotifier;
    RVpnApi         iVpnServ;    
    RFs             iFileServer;
    
    
    CApaCommandLine*    iCommandLine;
    HBufC*              iDriveAndPath;
    HBufC*              iFormat;
    HBufC8*             iFileName;
    
    HBufC*          iP12Password;
    TInt            iDevLockPolicy;
    TBool           iDevLockRequired;
    TBool           iDevSupportsDevLock;

    TTask           iOngoingTask;

    RLibrary        iTCLib;
    
    CZipFileDecompressor*       iDecompressor;    

    TPckgBuf<TVpnDialogInfo>    iDialogInfoDes;
    TPckgBuf<TVpnDialogOutput>  iDialogResponseDes;
    
    CImportWaitNoteHandler*     iWaitNoteHandler;
    };

#endif // C_VPNPOLICYINSTALLER_H

