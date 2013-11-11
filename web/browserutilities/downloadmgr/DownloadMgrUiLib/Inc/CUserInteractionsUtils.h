/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Supports user interaction dialogs for downloads, etc.
*
*/



#ifndef CUSERINTERACTIONSUTILS_H
#define CUSERINTERACTIONSUTILS_H

//  INCLUDES
#include <e32base.h>
#include <e32def.h>
#include <e32std.h>
#include <apparc.h>
#include <HttpDownloadMgrCommon.h>
#include <vwsdef.h>
#include <AknServerApp.h>
#include "CDownloadUtils.h"
#include <AknTaskList.h>

// FORWARD DECLARATIONS
class RHttpDownload;
class CDocumentHandler;
class MDownloadHandlerObserver;
class CDownloadMgrUiBase;
class MDownloadMgrUiLibRegModel;
class CCoeEnv;
class CAknFileSelectionDialog;
class CAknMemorySelectionDialog;
class CAknInformationNote;
class CBrowserDialogsProvider;
class CAknSoftNotifier;
class CErrorUI;
class CAknQueryDialog;
class CDownloadMgrUiDownloadsList;

// CONSTANTS
LOCAL_D const TInt KMinLength = 1024;

/// Active object priority. Must be greater than HTTP fw's active objects'.
LOCAL_C const TInt KUiLibAOPriority = CActive::EPriorityStandard + 2;

//TODO: it will be replaced when DocumentHandler will introduce a new API to get PDAppsUids (from DocConstants.h temp)
// Progressive Download App Uids
const TInt KDocPDAppUidList[] = 
    { 
    0x102824CD, // Lightweight Player application.
    0x102072C3  // Music Player application.
    };    
// Amount of elements in KDocProgDlAppUidList array.
const TInt KDocPDAppUidCount = sizeof( KDocPDAppUidList ) / sizeof ( TInt );


// CLASS DECLARATION

/**
*  Supports user interaction dialogs for downloads, etc.
*/
NONSHARABLE_CLASS( CUserInteractionsUtils ) : public CDownloadUtils,
                                              private MAknServerAppExitObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * &param aDMgrUiBase Reference to UI base functionality.
        */
        static CUserInteractionsUtils* NewL( CDownloadMgrUiBase& aDMgrUiBase, 
                                             MDownloadMgrUiLibRegModel& aRegistryModel );
        
        /**
        * Destructor.
        */
        virtual ~CUserInteractionsUtils();

    public: // New functions

        /**
        * Show error notification.
        * @param aDownload The actual download
        * @param aErrorId The DMgr specific error id.
        * @return -
        */
        void ShowErrorNoteL( RHttpDownload& aDownload, TInt32 aErrorId );

        /**
        * Show error notification.
        * @param aErrorId The DMgr specific error id.
        * @return -
        */
        void ShowErrorNoteL( TInt aErrorId );

        /**
        * Check if the download can be resumed after a fail.
        * @param aDownload The actual download.
        * @param aErrorId The DMgr specific error id.
        * @return True/False.
        */
        TBool IsRecoverableFailL( RHttpDownload& aDownload, TInt32 aErrorId ) const;

        /**
        * Handle downloaded content: open viewer application embedded.
        * @param aDownload The actual download
        * @param aHandlerObserver Handler application observer pointer.
        * @return None. Content is handled successfully.
        */
        void HandleContentL( RHttpDownload& aDownload, 
                             MDownloadHandlerObserver& aHandlerObserver );

        /**
        * Save (move) downloaded content.
        * @param aDownload The actual download
        * @return Content saved or not.
        */
        TBool SaveContentL( RHttpDownload& aDownload );

        /**
        * Save (move) downloaded content to where user selects.
        * @param aDownload The actual download
        * @return Content saved or not.
        */
        TBool SaveContentWithPathSelectionL( RHttpDownload& aDownload );

        /**
        * Cancel the memory selection dialog.
        * @return None.
        */
        void CancelMemorySelectionDialog();

        /**
        * Cancel the file selection dialog.
        * @return None.
        */
        void CancelFileSelectionDialog();

        /**
        * Delete the download with user confirmation.
        * @param aDownload The actual download
        * @return Deleted or not.
        */
        TBool DeleteWithUserConfirmL( RHttpDownload& aDownload );

        /**
        * Cancel the delete confirmation dialog.
        * @return None.
        */
        void CancelDeleteConfirmationDialog();

        /**
        * Cancel the download with user confirmation. In fact it deletes the download.
        * @param aDownload The actual download
        * @return Deleted or not.
        */
        TBool CancelWithUserConfirmL( RHttpDownload& aDownload );
        
        /**
        * Cancel the download with user confirmation. In fact it deletes the download.
        * @param aDownload The actual download
        * @return Deleted or not.
        */
        TBool CancelAllWithUserConfirmL( RHttpDownload& aDownload );

        /**
        * Cancel the cancel confirmation dialog.
        * @return None.
        */
        void CancelCancelConfirmationDialog();

        /**
        * Show media removed notification.
        * @return None.
        */
        void ShowMediaRemovedNoteL();

        /**
        * Cancel media removed notification.
        * @return None.
        */
        void CancelMediaRemovedNote();

        /**
        * Ask HTTP authentication credentials from user.
        * @return Continue: yes/no.
        */
        TBool GetAndSetHttpAuthCredentialsL( RHttpDownload& aDownload );

        /**
        * Ask proxy authentication credentials from user.
        * @return Continue: yes/no.
        */
        TBool GetAndSetProxyAuthCredentialsL( RHttpDownload& aDownload );

        /**
        * Cancel HTTP and proxy credentials queries.
        * @return None.
        */
        void CancelCredentialsQueries();

        /**
        * Display security warning. User can cancel the query.
        * @return Continue: yes/no.
        */
        TBool DisplaySecurityWarningL();

        /**
        * Cancel security warning.
        * @return None.
        */
        void CancelSecurityWarning();

        /**
        * Browser setting: need security warning?
        * @return True if needed.
        */
        TBool BrowserSettingSecWarningL();

        /**
        * Initialize the stand-alone soft notification.
        * @return -
        */
        void InitializeSoftNotifStndL( TVwsViewId aViewId, 
                                 TUid aCustomMessageId, 
                                 const TDesC8& aViewActivationMsg ) const;

        /**
        * Initialize the embedded soft notification.
        * @return -
        */
        void InitializeSoftNotifEmbL( TVwsViewId aViewId, 
                                TUid aCustomMessageId, 
                                const TDesC8& aViewActivationMsg ) const;

        /**
        * Cancel the stand-alone soft notification. Leaving fn.!
        * @return -
        */
        void CancelSoftNotifStndL( TVwsViewId aViewId, 
                                  TUid aCustomMessageId, 
                                  const TDesC8& aViewActivationMsg ) const;

        /**
        * Cancel the embedded soft notification. Leaving fn.!
        * @return -
        */
        void CancelSoftNotifEmbL( TVwsViewId aViewId, 
                                 TUid aCustomMessageId, 
                                 const TDesC8& aViewActivationMsg ) const;

        /**
        * Cancel all dialogs shown by CUserInteractionsUtils.
        * @return None.
        */
        void CancelAllDialogs();

        /**
        * Display an information note with no buttons.
        * @param aDialogResourceId Resource id of note to display.
        * @param aPromptResourceId Resource id of the prompt.
        */
        static void InfoNoteL( TInt aDialogResourceId, const TInt aPromptResourceId );

        /**
        * Display an information note with no buttons.
        * @param aDialogResourceId Resource id of note to display.
        * @param aPrompt Prompt to display.
        */
        static void InfoNoteL( TInt aDialogResourceId, const TDesC& aPrompt );

        /**
        * Is something using the UI?
        * @return yes/no.
        */
        TBool IsUiBusy() const;

        /**
        * Is the DocHandler running?.
        * @return yes/no.
        */
        TBool IsDocHandlerRunning() const;

        /**
        * Check if the download is DRM DCF and corrupted.
        * @param aDownload The actual download
        * @return ETrue/EFalse
        */
        TBool IsCorruptedDcfL( RHttpDownload& aDownload ) const;

        /**
        * Check if the MIME type is correct in DRM DCF.
        * @param aDownload The actual download
        * @return ETrue/EFalse
        */
        TBool IsBadMimeInDcfL( RHttpDownload& aDownload ) const;

        /**
        * Indicate to Utils, that the download has been deleted.
        * @param aDownload The actual download.
        * @return -
        */
        void DownloadHasBeenDeleted( RHttpDownload* aDownload );

        /**
        * Launch application for PD playing.
        * @param aDownload The actual download 
        * @return void 
        */
        void LaunchPdAppL( RHttpDownload& aDownload, const TBool aProgressively );

        /**
        * Query if the phone is a 3G network.
        * @return ETrue if the phone is in 3G network.
        */
        TBool IsNetworkPdCompatibleL() const;

        /**
        * Sends a message to the PD applications (MP, VP ) before browser exits
        * @param aProgressiveDownloadLaunched Progressive Play Launched or not 
        */
        void SendMsgTerminateToPdAppsL( TBool aProgressiveDownloadLaunched );

		/**
		* Checks if the WLAN is available
		*/
        TBool GetWlanAvailabilityL() const;


#ifdef __DMGR_PD_TESTHARNESS
        /**
        * Launch test application for PD playing.
        */
        void TestLaunchPdAppL( RHttpDownload& aDownload );
#endif

        /**
        * Check if several conditions are met for Progressive Playback
        */
        TBool CanLaunchAsProgDownload( RHttpDownload& aDownload, CDownloadMgrUiDownloadsList& dlList, TBool isOma2Dl );

        /**
        * Update saved file to DCFRepository  
        */
        void UpdateDCFRepositoryL( const TDesC& aFileName );
        
        /**
        * To Know if its a COD DL  
        */
        TBool IsCodDownload();

    protected: // Constructors

        /**
        * C++ default constructor.
        */
        CUserInteractionsUtils( CDownloadMgrUiBase& aDMgrUiBase, 
                                MDownloadMgrUiLibRegModel& aRegistryModel );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    protected: // From MAknServerAppExitObserver

        void HandleServerAppExit( TInt aReason );

    private: // file operations
    
        /**
        * Checks for existence of aFileName and modifies it to avoid 
        * collisions when saving downloaded content to filesystem.
        * @param aFileName The filename selected by user, including path
        * @return None
        */
        void EnsureUniqueFileNameL( TPtr& aFileName );

        /**
        * Modifies aFileName to include suffix similar to "(0x)".
        * @param aFileName The filename selected by user, including path
        * @param aCounter number of times filename has been modified
        * @return TBool was new name genearated successfully
        */
        TBool GenerateNewNameL( TPtr& aFileName, TInt& aCounter );

        /**
        * Performs a simple check for existence of aFileName.
        * @param aFileName The filename selected by user, including path
        * @return TBool indicating if file exists
        */
        TBool IsDuplicateL( const TPtrC& aFileName );
        
    private: // Data

        CCoeEnv& iCoeEnv; ///< Reference to CONE environment.
        CDownloadMgrUiBase& iDMgrUiBase; ///< Reference to UI base functionality.
        MDownloadMgrUiLibRegModel& iRegistryModel;
        CDocumentHandler* iDocHandler; ///< Owned.
        TBool iDocHandlerUsesTheUi; ///< Is the DocH doing something?
        MDownloadHandlerObserver* iHandlerObserver; ///< Not owned.
        RHttpDownload* iHandledDownload; ///< Not owned. Passed to iHandlerObserver.
        CAknMemorySelectionDialog* iMemSelectionDialog; ///< Ownership depends.
        CAknFileSelectionDialog* iFileSelectionDialog; ///< Ownership depends.
        CAknInformationNote* iMediaRemovedNote; ///< Ownership depends.
        CBrowserDialogsProvider* iDialogsProv; ///< Owned.
        CAknSoftNotifier* iSoftNotifier; ///< Owned.
        CErrorUI* iErrorUi; ///< Owned.
        CAknQueryDialog* iDeleteConfirmDlg; ///< Owned.
        CAknQueryDialog* iCancelConfirmDlg; ///< Owned.

		// Filename suffix formatters
        HBufC* iUnderTenFormatter;  // (Own)
        HBufC* iOverTenFormatter;   // (Own)
        TUint iSearchChar;
        TBool iCodDownload;
    };

#endif /* CUSERINTERACTIONSUTILS_H */
