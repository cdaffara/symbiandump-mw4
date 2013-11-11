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
* Description:  This file contains the header file of the CMidletUI class.
*
*                This class implements the ECom SWInstUIPluginAPI interface
*                for midlet installation.
*
*/



#ifndef WIDGETUIHANDLER_H
#define WIDGETUIHANDLER_H

//  INCLUDES

#include <e32base.h>
#include <swi/msisuihandlers.h>
#include "CUIDialogs.h"

namespace SwiUI
{
/**
* This interface defines an observer for user initiated cancel.
*
* @since 3.1
*/
class MWidgetUICancelObserver
    {
    public:

        /**
        * Called when user has cancelled the operation.
        * @since 3.0
        */
        virtual void UserCancelL( TBool aCancelImmediately ) = 0;
    };

/**
* This class handles the UI for widget installations.
*
* @lib WidgetInstallerUI.lib
* @since 3.1
*/
class CWidgetUIHandler : public CBase,
                       public CommonUI::MCUIWaitDialogCallback
    {
    public:  // Constructors and destructor

        /**
        * Enumeration for install mode.
        * @since 3.1
        */
        enum TInstallMode
        {
        EModeInstall,
        EModeUninstall
        };

        /**
        * Two-phased constructor.
        * @param aMode - Indicates if this is for install or uninstall
        * @param aCancelObserver - the object to handle the user cancel event
        */
        static CWidgetUIHandler* NewL( TInstallMode aMode, MWidgetUICancelObserver* aCancelObserver );

        /**
        * Destructor.
        */
        virtual ~CWidgetUIHandler();

    public:  // From base classes

        /**
        * From, MCUIWaitDialogCallback, called when wait/progress dialog is about
        * to be cancelled.
        */
        TBool HandleDialogExitL( TInt aButtonId );

    public: // new

        /**
        * Displays a dialog at the beginning of an installation
        * @param aWidgetName - display name of the widget
        * @return Yes if user would want to install, No if user cancels install
        */
        TBool DisplayInstallL( const TDesC& aWidgetName );

        /**
        * Displays a dialog at the beginning of an update
        * @param aWidgetName - display name of the widget
        * @return Yes if user would want to update, No if user cancels update
        */
        TBool DisplayOverwriteL( const TDesC& aWidgetName );

        /**
        * Displays a dialog asking whether we want to upgrade an
        * application present on the device.
        */
        TBool DisplayUpgradeL( const Swi::CAppInfo& aAppInfo,
                               const Swi::CAppInfo& aExistingAppInfo );

        /**
        * Displays a dialog at the start of the uninstallation with
        * information about the application to be uninstalled. The user may cancel the
        * uninstallation at this stage. This dialog must be shown in TrustedUI mode.
        */
        TBool DisplayUninstallL( const TDesC& aWidgetName, TBool aWidgetinHomeScreen );
        
        /**
        * Prompt user to select a drive to install the widget
        * @param aDrive - returns user's drive selection
        * @param aSufficientSpace - return whether user's selected drive is big enough
        * @return true if user selected a drive, false if user chose cancel.
        */
        TBool SelectDriveL( RFs& aFs, TDriveUnit& aDrive,
                            TBool& aSufficientSpace, TInt aTotalSize );

        /**
        * Displays a dialog containing text. This dialog has only a
        * Continue button if aOption is EInstFileTextOptionContinue, or Yes and No buttons
        * otherwise.
        */
        TBool DisplayTextL( const TDesC& aText );

        /**
        * Displays a dialog showing an error message.
        * @param aErr - an error code
        */
        void DisplayErrorL( const TInt aErr );

        /**
        * Displays a dialog showing completion message.
        */
        void DisplayCompleteL();

        /**
        * Displays a dialog showing cancel message.
        */
        void DisplayCancelL();

        /**
        * Informs user that the applications are in use during an uninstallation.
        * Return ETrue if the user wishes to continue the removal process and
        * close the applications anyway.
        */
        TBool DisplayApplicationsInUseL( const Swi::CAppInfo& aAppInfo,
                                         const RPointerArray<TDesC>& aAppNames );

        /**
        * Show progress dialog and pass total uncompressed package size.
        * This method should be called only after SelectDrive is called.
        * @since 3.1
        */
        void DisplayProgressDialogL();

        /**
        * Update progress information during installation.
        * @since 3.1
        * @param aSize - the increment on progress
        */
        void UpdateProgressDialogL( TInt aSize );

        /**
        * Close progress dialog
        */
        void CloseProgressDialogL() { iCommonDialogs->CloseProgressDialogL(); }

        /**
        * Display Finalize dialog
        */
        void DisplayFinalizeDialogL();

        /**
        * Close the finalize dialog
        */
        void CloseFinalizeDialogL() { iCommonDialogs->CloseWaitDialogL(); }

        /**
        * Display unintall in progress dialog
        * @since 5.0
        */
        void DisplayUninstallInProgressL();

        /**
        * Close the wait dialog
        * @since 5.0
        */
        void CloseUninstallInProgressDialogL() { iCommonDialogs->CloseWaitDialogL(); }

        /**
        * Show the untrusted widget dialog
        * @since 5.0
        * @return Yes if user would wants to continue with install, 
        *         No if user cancels install
        */
        TBool DisplayUntrustedSecurityDialogL();

    protected:

        /**
        * Constructor.
        * @param aMode - Indicates if this is for install or uninstall
        */
        CWidgetUIHandler( TInstallMode aMode, MWidgetUICancelObserver* aCancelObserver );

    private:

        /**
        * 2nd phase constructor.
        */
        void ConstructL();


    protected:  // Data
        MWidgetUICancelObserver* iCancelObserver;   // not owned, used for callback

        CommonUI::CCUIDialogs* iCommonDialogs;      // Owned

        TInstallMode iMode;     // Indicates whether we have install or uninstall in progress

        TBool iShowingDialog;   // show dialog or not

        TBool iShownFinalNote;  // is final note shown or not

        TInt iUncompressedSize; // uncompressed size of the whole package

        TInt iInstalledSize;    // total installed size

        CArrayFixFlat<TInt>* iProgressNumbers; // owned
    };
}

#endif
