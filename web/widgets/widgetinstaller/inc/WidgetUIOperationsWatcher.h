/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This file contains the header file of the CWidgetOperationsWatcher class.
*
*                This class implements the ECom SWInstUIPluginAPI interface
*                for midlet installation.
*
*
*/


#ifndef WIDGETUIOPERATIONWATCHER_H
#define WIDGETUIOPERATIONWATCHER_H

//  INCLUDES
#include <e32base.h>
#include <SWInstDefs.h>
#include <SWInstTaskManager.h>
#include <SWInstLogTaskParam.h>
#include <WidgetRegistryClient.h>
#include <apgcli.h> // RApaLsSession
#include "CUICancelTimer.h"
#include "WidgetUIHandler.h"
#include "IconConverter.h"

// FORWARD DECLARATIONS
class CZipFile;
class CZipFileMemberIterator;

namespace SwiUI
{
// CONSTANTS

// FORWARD DECLARATIONS
class CWidgetUIConfigHandler;
class CWidgetRegistrationManager;

// CLASS DECLARATION


/**
* This class is provided for callback to notify icon conversion completion.
* @lib WidgetInstallerUI.lib
* @since 3.1
*/
class MConverterController
    {
public:
    /**
     * notify icon conversion completion
     */
    virtual void NotifyCompletionL( TInt aErr ) = 0;
    };


/**
* This class handles the asynchronous install / unsinstall requests.
*
* @lib WidgetInstallerUi.lib
* @since 3.1
*/
class CWidgetUIOperationsWatcher :  public CActive,
                                    public CommonUI::MCUICancellable,
                                    public MWidgetUICancelObserver,
                                    public MConverterController
    {
public:  // Constructors and destructor
    /**
     * Two-phased constructor.
     */
    static CWidgetUIOperationsWatcher* NewL();

    /**
     * Destructor.
     */
    virtual ~CWidgetUIOperationsWatcher();

public: // New functions
    /**
     * Handle install request.
     * @since 3.1
     */
    void InstallL( RFile&, const TDesC8&, TRequestStatus& );

    /**
     * Handle silent install request.
     * @since 3.1
     */
    void SilentInstallL( RFile&, const TDesC8&, TRequestStatus& );

    /**
     * Handle uninstall request.
     * @since 3.1
     */
    void UninstallL( const TUid& aUid, TRequestStatus& aRequestStatus );

    /**
     * Handle silent uninstall request.
     * @since 3.1
     */
    void SilentUninstallL( const TUid&, TRequestStatus& );

    /**
     * Called when installation or update is finished, do the final
     * steps of registration, file clean up (remove temporary files,
     * display complete message, or handle error.
     */
    void FinishInstallL();

    /**
     * Called when uninstallation is finished, do the final steps of
     * deregistration, etc, display complete message, or handle error.
     */
    void FinishUninstallL( TInt aErr );

    /**
     * Called when install or uninstallation is finished, to update installer log
     */    
    void HandleLogsL(const TDesC& aWidgetName, const TUid &aUid,TBool aVendor, SwiUI::TLogTaskAction aAction);


public: // Functions from base classes
    /**
     * From CActive, Called by framework when request is finished.
     * @since 3.1
     */
    void RunL();

    /**
     * From CActive, Called by framework when request is cancelled.
     * @since 3.1
     */
    void DoCancel();

    /**
     * From CActive, Handles a leave occurring in the request completion
     * event handler RunL().
     * @since 3.1
     */
    TInt RunError( TInt aError );

    /**
     * From MWidgetUICancelObserver, Called when user has cancelled
     * the operation.
     * @since 3.1
     */
    void UserCancelL( TBool aCancelImmediately = ETrue );

    /**
     * From MCUICancellable, Indicates if a modal dialog is currently shown.
     */
    TBool IsShowingDialog();

    /**
     * From MCUICancellable, Cancels the installation engine.
     */
    void CancelEngine();

    /**
     * From MCUICancellable, Called when nothing else has worked.
     */
    void ForceCancel();

    /**
     * From MCUICancellable, Called to indicate that the cancelling progress
     * has been started.
     */
    void StartedCancellingL();

    /**
     * Used for notify icon convertion complete
     */
    void NotifyCompletionL( TInt aErr ) ;

private:
    /**
     * C++ default constructor.
     * @param aRequestStatus - Status of the request.
     */
    CWidgetUIOperationsWatcher();

    /**
     * 2nd phase constructor.
     */
    void ConstructL();

    /**
     * @since 5.0
     */
    void InstallSubfunctionL( RFile& aFile, TBool& userCancel,
                              TRequestStatus& aRequestStatus );

    /**
     * Create widget entry by parsing the info.plist, insert the entry into the entry list
     * if the entry is not already existing.
     * @return the existed widget's index if already present
     * @since 3.1
     */
    TInt PreprocessWidgetBundleL();

    /**
     * Prompt user to choose mamory to install/update the widget
     * @return true if user choose a momory unit, Leaves if user's choosed memory is not sufficient
     * @since 3.1
     */
    TBool SelectMemoryL();

    /**
     * update basepath of widget
     * @since 3.1
     */
    void UpdateWidgetBasePathL( TDriveUnit& aDrive );


    /**
     * Start the icon convertion
     * @since 3.1
     */
    void ConvertIconL( TUid& aUid, const TDesC& aIconPath );

    /**
     * Prompt user to install and select memory to use
     * @since 3.1
     */
    TBool PromptUserForInstallL( TBool aOverwrite );
    
    /**
     * Prompt user with security warning about installing
     * untrusted widgets
     * @since 5.0
     */
    TBool PromptUserForUntrustedWidgetL( );

    /**
     * When overwriting an installed widget, backup so can restore on error
     * @since 3.1
     */
    void BackupL();

    /**
     * When overwriting an installed widget, restore on error
     * @since 3.1
     */
    void RestoreL();

    /**
    *In the process of uninstallation if widget is running the widget should be first closed and then uninstalled
    *@since 5.0
    */
    void HandleWidgetCommandL( RApaLsSession& aSession,const TDesC& aWidget,const TUid& aUid,TUint32 aOperation );

private: // Data

    // A few state variables with the normal case complexity being
    // that icon processing runs in parallel with unzipping so if
    // unzipping ends first then final steps are triggered by icon
    // processing completion and if icon processing ends first then
    // unzipping triggers the final steps.  If cancelling either by
    // user or some external system event, then final steps are
    // triggered by DoCancel().
    TBool iUnzipping;
    TBool iOverwriting;
    TBool iProcessingIcon;
    TBool iIconSearchFinished; // search in zip archive
    TInt iIconError;
    TBool iCancelled;
    // TODO: may need to implement silent mode in future
    TBool iSilent; // silent mode

    TRequestStatus* iRequestStatus; // pointer to request status of caller

    CFileMan* iFileMgr; // owned
    RFs iRfs; // owned
    CZipFile* iZipFile; // owed
    CZipFileMemberIterator* iMembers; // owned

    RWidgetRegistryClientSession iRegistry;
    TBool iServerConnected; // to Widget Registry server

    CWidgetUIHandler* iUIHandler; // owned
    CWidgetUIConfigHandler* iWidgetConfigHandler; // owned
    CWidgetRegistrationManager* iAppManager; // owned

   // active object to asynchronously convert png icon to mbm
    CIconConverter* iIconConverter; // owned

    TFileName iOriginalDir; // for overwrite backup/restore
    TFileName iBackupDir; // for overwrite backup/restore

    // these values mainly come from parsing bundle metadata file
    // (info.plist) and are passed to WidgetRegistry for registration.
    RPointerArray<CWidgetPropertyValue> iPropertyValues;
    
    // task manager pointer for install logs
    CTaskManager* iTaskManager; 
    // TODO: To be investigated
    //CommonUI::CCUICancelTimer* iCanceller;
    
    TFileName iWidgetName;  // save the widget name during overwrite (only for HS widgets)
    TBool iWidgetInHS;      // indicates whether the widget was in HS
    };
}

#endif      // WIDGETUIOPERATIONWATCHER_H

// End of File
