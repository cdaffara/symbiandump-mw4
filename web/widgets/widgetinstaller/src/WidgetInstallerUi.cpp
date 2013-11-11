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

#include <WidgetUI.rsg>
#include <eikenv.h>
#include <data_caging_path_literals.hrh>

#include "CUIUtils.h"

#include "WidgetInstallerUi.h"
#include "WidgetUIOperationsWatcher.h"
#include "bautils.h"

using namespace SwiUI;

_LIT( KResourceFileName, "\\resource\\WidgetInstallerUI.rsc" );
_LIT( KResourceDir, "Z:WidgetInstallerUI.rsc" );

// ============================ MEMBER FUNCTIONS ===============================

// ============================================================================
// CWidgetInstallerUI::CWidgetInstallerUI
// C++ default constructor can NOT contain any code, that
// might leave.
//
// @since 3.1
// ============================================================================
//
CWidgetInstallerUI::CWidgetInstallerUI()
    {
    }

// ============================================================================
// CWidgetInstallerUI::ConstructL
// Symbian 2nd phase constructor can leave.
//
// @since 3.1
// ============================================================================
//
void CWidgetInstallerUI::ConstructL()
    {
 	CCoeEnv::Static()->DisableExitChecks( ETrue );
    TFileName resourceFileName;  
    TParse parse;    
    Dll::FileName (resourceFileName);           
    parse.Set(KResourceFileName, &resourceFileName, NULL); 
    resourceFileName = parse.FullName(); 
      
    CCoeEnv* coeEnv=CCoeEnv::Static(); 
    BaflUtils::NearestLanguageFile(coeEnv->FsSession(), resourceFileName); 
   
    if ( !BaflUtils::FileExists( coeEnv->FsSession(), resourceFileName ) )           
        { 
        // Use resource file on the Z drive instead  
        parse.Set( KResourceDir, &KDC_RESOURCE_FILES_DIR, NULL ); 
        resourceFileName = parse.FullName();  
        BaflUtils::NearestLanguageFile( coeEnv->FsSession(),resourceFileName );            
        } 
    iResourceFileOffset = coeEnv->AddResourceFileL(resourceFileName);         
    }

// ============================================================================
// CWidgetInstallerUI::NewL
// Two-phased constructor.
//
// @since 3.1
// ============================================================================
//
CWidgetInstallerUI* CWidgetInstallerUI::NewL()
    {
    CWidgetInstallerUI* self = new( ELeave ) CWidgetInstallerUI();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ============================================================================
// CWidgetInstallerUI::~CWidgetInstallerUI
// Destructor.
//
// @since 3.1
// ============================================================================
//
CWidgetInstallerUI::~CWidgetInstallerUI()
    {
    delete iWatcher;

    if( iResourceFileOffset )
        {
        CEikonEnv::Static()->DeleteResourceFile( iResourceFileOffset );
        }
    }

// ============================================================================
// CWidgetInstallerUI::InstallL
// Perform installation.
// (other items were commented in a header).
//
// @since 3.1
// ============================================================================
//
void CWidgetInstallerUI::InstallL( RFile& aFile,
                                   TInstallReq& aInstallParams,
                                   TBool /*aIsDRM*/,
                                   TRequestStatus& aStatus )
    {
    __ASSERT_ALWAYS( !iWatcher, User::Leave( KErrInUse ) );
    TRAPD( err ,
           iWatcher = CWidgetUIOperationsWatcher::NewL();
           iWatcher->InstallL( aFile, aInstallParams.iMIME, aStatus );
        );
    if ( KErrNone != err )
      {
        delete iWatcher;
        iWatcher = NULL;
        User::Leave( KErrGeneral );
      }
    }

// ============================================================================
// CWidgetInstallerUI::SilentInstallL
// Perform silent installation.
// (other items were commented in a header).
//
// @since 3.1
// ============================================================================
//
void CWidgetInstallerUI::SilentInstallL( RFile& aFile,
                                         TInstallReq& aInstallParams,
                                         TBool /*aIsDRM*/,
                                         TInstallOptions& /*aOptions*/,
                                         TRequestStatus& aStatus )
    {
    __ASSERT_ALWAYS( !iWatcher, User::Leave( KErrInUse ) );
    TRAPD( err ,
           iWatcher = CWidgetUIOperationsWatcher::NewL();
           iWatcher->SilentInstallL( aFile, aInstallParams.iMIME, aStatus );
        );
    if ( KErrNone != err )
      {
        delete iWatcher;
        iWatcher = NULL;
        User::Leave( KErrGeneral );
      }
    }

// ============================================================================
// CWidgetInstallerUI::UninstallL
// Perform uninstallation.
// (other items were commented in a header).
//
// @since 3.1
// ============================================================================
//
//
void CWidgetInstallerUI::UninstallL( const TUid& aUid,
                                     const TDesC8& /*aMIME*/,
                                     TRequestStatus& aStatus )
    {
    __ASSERT_ALWAYS( !iWatcher, User::Leave( KErrInUse ) );
    TRAPD(err,
          iWatcher = CWidgetUIOperationsWatcher::NewL();
          iWatcher->UninstallL( aUid, aStatus );
        );
    if ( KErrNone != err )
      {
        delete iWatcher;
        iWatcher = NULL;
        User::Leave( KErrGeneral );
      }
    }

// ============================================================================
// CWidgetInstallerUI::SilentUninstallL
// Perform silent uninstallation.
// (other items were commented in a header).
//
// @since 3.1
// ============================================================================
//
void CWidgetInstallerUI::SilentUninstallL( const TUid& aUid,
                                           const TDesC8& /*aMIME*/,
                                           TUninstallOptions& /*aOptions*/,
                                           TRequestStatus& aStatus )
    {
    __ASSERT_ALWAYS( !iWatcher, User::Leave( KErrInUse ) );
    TRAPD( err ,
           iWatcher = CWidgetUIOperationsWatcher::NewL();
           iWatcher->SilentUninstallL( aUid, aStatus );
        );
    if ( KErrNone != err )
      {
        delete iWatcher;
        iWatcher = NULL;
        User::Leave( KErrGeneral );
      }
    }

// ============================================================================
// CWidgetInstallerUI::IsAppShellUpdate
// Indicates if the application shell must be updated.
// (other items were commented in a header).
//
// @since 3.1
// ============================================================================
//
TBool CWidgetInstallerUI::IsAppShellUpdate()
    {
    return ETrue;
    }

// ============================================================================
// CWidgetInstallerUI::Cancel()
// Cancel the current operation.
//
// @since 3.1
// ============================================================================
//
void CWidgetInstallerUI::Cancel()
    {
    delete iWatcher;
    iWatcher = NULL;
    }

// ============================================================================
// CWidgetInstallerUI::CompleteL
// Informs the plugin about completed operation. Called when all tasks are
// completed.
//
// @since 3.1
// ============================================================================
//
void CWidgetInstallerUI::CompleteL()
    {
    delete iWatcher;
    iWatcher = NULL;
    }

// ============================================================================
// CWidgetInstallerUI::GetErrorDetails
//
// @since 3.1
// ============================================================================
//
CErrDetails* CWidgetInstallerUI::GetErrorDetails()
    {
    return NULL;
    }

// ============================================================================
// CWidgetInstallerUI::CustomUninstallL
// Perform a custom uninstallation.
// @since 3.1
// ============================================================================
//
void CWidgetInstallerUI::CustomUninstallL( TOperation /*aOperation*/,
                                  const TDesC8& /*aParams*/,
                                  TRequestStatus& /*aStatus*/ )
    {
    User::Leave( KErrNotSupported );
    }

// ============================================================================
// CWidgetInstallerUI::SilentCustomUninstallL
// Perform a custom uninstallation.
//
// @since 3.1
// ============================================================================
//
void CWidgetInstallerUI::SilentCustomUninstallL( TOperation /*aOperation*/,
                                        TUninstallOptions& /*aOptions*/,
                                        const TDesC8& /*aParams*/,
                                        TRequestStatus& /*aStatus*/ )
    {
    User::Leave( KErrNotSupported );
    }


//  End of File
