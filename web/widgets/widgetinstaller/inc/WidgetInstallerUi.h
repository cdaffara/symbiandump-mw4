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
* Description:  This file contains the header file of the CWidgetInstallerUI class.
*
*                This class implements the ECom SWInstUIPluginAPI interface
*                for widget installation.
*
*/


#ifndef WIDGETINSTALLERUI_H
#define WIDGETINSTALLERUI_H

//  INCLUDES
#include <SWInstUIPluginAPI.h>

// FORWARD DECLARATION
class CEikonEnv;

namespace SwiUI
{

// FORWARD DECLARATIONS
class CWidgetUIOperationsWatcher;

/**
* This class implements the ECom SWInstUIPluginAPI interface for WidgetInstaller
* installation.
*
* @lib WidgetInstallerUI
* @since 3.1
*/
class CWidgetInstallerUI : public CSWInstUIPluginAPI
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CWidgetInstallerUI* NewL();

        /**
        * Destructor.
        */
        virtual ~CWidgetInstallerUI();

    public:  // From base classes

       /**
        * From SWInstUIPlugin, Perform installation.
        * @since 3.1
        */
        void InstallL( RFile& aFile,
                       TInstallReq& aInstallParams,
                       TBool aIsDRM,
                       TRequestStatus& aStatus );

        /**
        * From SWInstUIPlugin, Perform silent installation.
        * @since 3.1
        */
        void SilentInstallL( RFile& aFile,
                             TInstallReq& aInstallParams,
                             TBool aIsDRM,
                             TInstallOptions& aOptions,
                             TRequestStatus& aStatus );

        /**
        * From SWInstUIPlugin, Perform uninstallation.
        * @since 3.1
        */
        void UninstallL( const TUid& aUid,
                         const TDesC8& aMIME,
                         TRequestStatus& aStatus );

        /**
        * From SWInstUIPlugin, Perform silent uninstallation.
        * @since 3.1
        */
        void SilentUninstallL( const TUid& aUid,
                               const TDesC8& aMIME,
                               TUninstallOptions& aOptions,
                               TRequestStatus& aStatus );

        /**
        * From SWInstUIPlugin, Indicates if the application shell must be updated.
        * @since 3.1
        */
        TBool IsAppShellUpdate();

        /**
        * Cancel the current operation.
        * @since 3.1
        */
        void Cancel();

        /**
        * Informs the plugin about completed operation. Called when all tasks are completed.
        * @since 3.1
        */
        void CompleteL();

        /**        
        * GetErrorDetails
        * @since 3.1
        */
        CErrDetails* GetErrorDetails();

        /**
        * Perform a silent custom uninstallation.
        * @since 3.1
        * @param aOperation - The uninstall operation to be performed
        * @param aParams - Parameters for the operation
        */
        void SilentCustomUninstallL( TOperation aOperation,
                                     TUninstallOptions& aOptions,
                                     const TDesC8& aParams,
                                     TRequestStatus& aStatus );

        /**
        * Perform a custom uninstallation.
        * @since 3.1
        * @param aOperation - The uninstall operation to be performed
        * @param aParams - Parameters for the operation
        */
        void CustomUninstallL(  TOperation aOperation,
                                const TDesC8& aParams,
                                TRequestStatus& aStatus );

    private:

        /**
        * C++ Constructor.
        */
        CWidgetInstallerUI();

        /**
        * 2nd phase constructor.
        */
        void ConstructL();

    private: //  Data

        TInt iResourceFileOffset;

        CWidgetUIOperationsWatcher* iWatcher;   // watcher to carry out install process, owned
    };
}

#endif      // WIDGETINSTALLERUI_H

// End of File
