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

#include <Avkon.rsg>
#include <WidgetInstallerUI.rsg>
#include <StringLoader.h>
#include <pathinfo.h>
#include <aknnotewrappers.h>
#include "SWInstWidgetUid.h"
#include "WidgetInstallerInternalCRKeys.h"
#include "WidgetUIHandler.h"


// CONSTANTS
const TInt KB = 1024;

using namespace SwiUI;

// ============================ MEMBER FUNCTIONS ===============================

// ============================================================================
// CWidgetUIHandler::NewL()
// two-phase constructor
//
// @since 3.1
// ============================================================================
//
CWidgetUIHandler* CWidgetUIHandler::NewL( TInstallMode aMode,
                                          MWidgetUICancelObserver* aCancelObserver )
    {
    CWidgetUIHandler* self = new( ELeave ) CWidgetUIHandler( aMode, aCancelObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ============================================================================
// CWidgetUIHandler:CWidgetUIHandler()
// C++ default constructor
//
// @since 3.1
// ============================================================================
//
CWidgetUIHandler::CWidgetUIHandler( TInstallMode aMode,
                                    MWidgetUICancelObserver* aCancelObserver )
    :iCancelObserver( aCancelObserver ),
     iMode( aMode )
    {
    }

// ============================================================================
// CWidgetUIHandler:ConstructL()
// Symbian default constructor
//
// @since 3.1
// ============================================================================
//
void CWidgetUIHandler::ConstructL()
    {
    iCommonDialogs = CommonUI::CCUIDialogs::NewL();
    if ( iMode == EModeInstall )
         {
         iProgressNumbers = new ( ELeave ) CArrayFixFlat<TInt>( 2 );
         iProgressNumbers->ResizeL( 2 );
         iCommonDialogs->ShowWaitDialogL( R_WIDGETUI_PREPARE, this, R_AVKON_SOFTKEYS_EMPTY );
         }
    }

// ============================================================================
// CWidgetUIHandler::~CWidgetUIHandler()
// destructor
//
// @since 3.1
// ============================================================================
//
CWidgetUIHandler::~CWidgetUIHandler()
    {
    delete iProgressNumbers;
    delete iCommonDialogs;
    }


// ============================================================================
// CWidgetUIHandler::DisplayInstallL()
// Displays a dialog at the beginning of an installation
//
// @since 3.1
// ============================================================================
//
TBool CWidgetUIHandler::DisplayInstallL(
    const TDesC& aWidgetName
    )
    {
    iShowingDialog = ETrue;

    // Close the preparing wait dialog
    iCommonDialogs->CloseWaitDialogL();

    TBool result( EFalse );

    HBufC* prompt = StringLoader::LoadLC( R_WIDGETUI_INSTALL_PROMPT, aWidgetName );
    if ( iCommonDialogs->ShowConfirmationQueryL( *prompt, R_AVKON_SOFTKEYS_YES_NO ) )
        {
        result = ETrue;
        }
    else
        {
        // Make sure no notes are shown after this
        iShownFinalNote = ETrue;
        }

    CleanupStack::PopAndDestroy( prompt );

    iShowingDialog = EFalse;
    return result;
    }

// ============================================================================
// CWidgetUIHandler::DisplayOverwriteL()
// Displays a dialog at the beginning of an update
//
// @since 3.1
// ============================================================================
//
TBool CWidgetUIHandler::DisplayOverwriteL(
    const TDesC& aWidgetName
    )
    {
    iShowingDialog = ETrue;

    // Close the preparing wait dialog
    iCommonDialogs->CloseWaitDialogL();

    TBool result( EFalse ); // false means user says no/cancel/stop

    HBufC* prompt = StringLoader::LoadLC( R_WIDGETUI_OVERWRITE_PROMPT, aWidgetName );
    if ( iCommonDialogs->ShowConfirmationQueryL( *prompt, R_AVKON_SOFTKEYS_YES_NO ) )
        {
        result = ETrue;
        }
    else
        {
        // Make sure no notes are shown after this
        iShownFinalNote = ETrue;
        }

    CleanupStack::PopAndDestroy( prompt );

    iShowingDialog = EFalse;
    return result;
    }


// ============================================================================
// CWidgetUIHandler::DisplayUpgradeL()
// Displays a dialog asking whether we want to upgrade an application present on the device.
//
// @since 3.1
// ============================================================================
//
TBool CWidgetUIHandler::DisplayUpgradeL( const Swi::CAppInfo& /*aAppInfo*/,
                               const Swi::CAppInfo& /*aExistingAppInfo*/ )
    {
    return ETrue;
    }

// ============================================================================
// CWidgetUIHandler::DisplayUninstallL()
// Displays a dialog at the start of the uninstallation with
// information about the application to be uninstalled.
//
// @since 3.1
// ============================================================================
//
TBool CWidgetUIHandler::DisplayUninstallL( const TDesC& aWidgetName , TBool aWidgetinHomeScreen )
    {
    iShowingDialog = ETrue;

    // Close the preparing wait dialog
    iCommonDialogs->CloseWaitDialogL();

    TBool result( EFalse );

    HBufC* prompt = StringLoader::LoadLC( aWidgetinHomeScreen ? R_WIDGETUI_UNINSTALL_HS_PROMPT : 
                                          R_WIDGETUI_UNINSTALL_PROMPT , aWidgetName );
    
    if ( iCommonDialogs->ShowConfirmationQueryL( *prompt, R_AVKON_SOFTKEYS_YES_NO ) )
        {
        result = ETrue;
        }
    else
        {
        // Make sure no notes are shown after this
        iShownFinalNote = ETrue;
        }

    CleanupStack::PopAndDestroy( prompt );

    iShowingDialog = EFalse;
    return result;
    }

// ============================================================================
// CWidgetUIHandler::DisplayTextL()
// Displays a dialog with given text
//
// @since 3.1
// ============================================================================
//
TBool CWidgetUIHandler::DisplayTextL( const TDesC& /*aText*/ )
    {
    return ETrue;
    }

// ============================================================================
// CWidgetUIHandler::DisplayErrorL()
// Displays a dialog with given error
//
// @since 3.1
// ============================================================================
//
void CWidgetUIHandler::DisplayErrorL( TInt aErr )
    {
    iShowingDialog = ETrue;
    HBufC* prompt = NULL;

    switch ( aErr )
        {
        case KErrNotSupported:
            prompt = StringLoader::LoadLC( R_WIDGETUI_UNSUPPORTED );
            break;
        case KErrCorrupt:
            prompt = StringLoader::LoadLC( R_WIDGETUI_CORRUPTED );
            break;
        case KErrNoMemory:
            prompt = StringLoader::LoadLC( R_WIDGETUI_NO_MEMORY );
            break;
        case KErrInUse:
            prompt = StringLoader::LoadLC( R_WIDGETUI_ERR_INUSE );
            break;
        default:
            prompt = StringLoader::LoadLC( R_WIDGETUI_ERR_GENERAL );
            break;
        }

    iCommonDialogs->ShowErrorNoteL( prompt->Des() );
    CleanupStack::PopAndDestroy(); // prompt

    iShowingDialog = EFalse;
    }

// ============================================================================
// CWidgetUIHandler::DisplayApplicationsInUseL()
// Displays a dialog with information that the widget is already in use
//
// @since 3.1
// ============================================================================
//
TBool CWidgetUIHandler::DisplayApplicationsInUseL( const Swi::CAppInfo& /*aAppInfo*/,
                                         const RPointerArray<TDesC>& /*aAppNames*/ )
    {
    return ETrue;
    }

// ============================================================================
// CWidgetUIHandler::HandleDialogExitL()
//  when user click softkey during progress dialog
//
// @since 3.1
// ============================================================================
//
TBool CWidgetUIHandler::HandleDialogExitL( TInt aButtonId )
    {
    TBool result( EFalse );

    if ( aButtonId == EAknSoftkeyCancel )
        {
        result = ETrue;
        iCancelObserver->UserCancelL( EFalse );
        }

    return result;
    }

// ============================================================================
// CWidgetUIHandler::UpdateProgessDialogL
// Increments installation progress
//
// @since 3.1
// ============================================================================
//
void CWidgetUIHandler::UpdateProgressDialogL( TInt aSize )
{
    iInstalledSize += aSize;

    // rest just updates strings showing kilobytes processed
    if ( KB < iUncompressedSize ) // if less than 1K then dialog is meaningless
        {
        iCommonDialogs->UpdateProgressDialogValueL( aSize );
        (*iProgressNumbers)[0] = iInstalledSize / KB;
        HBufC* displayString = StringLoader::LoadLC( R_WIDGETUI_PROGRESS, *iProgressNumbers );
        iCommonDialogs->ShowProgressDialogL( *displayString, this ); // just updates size strings
        CleanupStack::PopAndDestroy();  // displayString
        }
}

// ============================================================================
// CWidgetUIHandler::DisplayProgessDialogL
// Starts installation progress dialog display
//
// @since 3.1
// ============================================================================
//
void CWidgetUIHandler::DisplayProgressDialogL()
    {
    iInstalledSize = 0;

    if ( KB < iUncompressedSize ) // if less than 1K then dialog is meaningless
        {
        (*iProgressNumbers)[0] = 0;
        (*iProgressNumbers)[1] = iUncompressedSize / KB;
        HBufC* displayString = StringLoader::LoadLC( R_WIDGETUI_PROGRESS, *iProgressNumbers );
        iCommonDialogs->ShowProgressDialogL( *displayString, this );
        iCommonDialogs->SetProgressDialogFinalValueL( iUncompressedSize );
        CleanupStack::PopAndDestroy();  // displayString
        }
}


// ============================================================================
// CWidgetUIHandler::SelectDriveL
// Asks the UI for a drive to select.
// (other items were commented in a header).
//
// @since 3.1
// ============================================================================
//
TBool CWidgetUIHandler::SelectDriveL( RFs& aFs,
                                      TDriveUnit& aDrive,
                                      TBool& aSufficientSpace,
                                      TInt aTotalSize)
    {
    aSufficientSpace = EFalse;
    TBool result( EFalse );

    iShowingDialog = ETrue;
    iUncompressedSize = aTotalSize;

    TInt error = KErrNone;

    // List of all possible installation detination drives
    RArray<TDriveUnit> driveUnits;
    CleanupClosePushL( driveUnits );
    // and free memory space in those drives
    RArray<TInt64> driveSpaces;
    CleanupClosePushL( driveSpaces );

    // List all drives in the system
    TDriveList driveList;
    User::LeaveIfError( aFs.DriveList(driveList) );

    // Check all drives
    for ( TInt driveNumber = EDriveA; driveNumber <= EDriveZ; driveNumber++ )
        {
        if ( EDriveD == driveNumber)
            {
            // The drive is not suitable for installation
            continue;
            }

        if ( !driveList[driveNumber] )
            {
            // Not a recognised drive
            continue;
            }

        TVolumeInfo volInfo;
        if ( aFs.Volume( volInfo, driveNumber ) != KErrNone )
            {
            // The volume is not usable (e.g. no media card inserted)
            continue;
            }

        if ( volInfo.iDrive.iType==EMediaNotPresent ||
             volInfo.iDrive.iType==EMediaRom ||
             volInfo.iDrive.iType==EMediaRemote )
            {
            // Exclude drives not suitable for installation
            continue;
            }

        // Exclude read only and substituted drives
        if ( volInfo.iDrive.iDriveAtt & KDriveAttRom ||
             volInfo.iDrive.iDriveAtt & KDriveAttSubsted )
            {
            continue;
            }

        // Check whether the drive is usable for installation! E.g. not a
        // temporary ram drive. This is vendor-implemented function which is
        // device-specific.
// PlpVariant library doesn't have any implementation for DriveStatusL() yet
//       if (PlpVariant::DriveStatusL(static_cast<TDriveNumber>(driveNumber)) !=
//           static_cast<TUint32> (KErrNone))

        TInt64 volSpace = volInfo.iFree;  // bytes
        User::LeaveIfError( driveUnits.Append( TDriveUnit( driveNumber ) ) );
        User::LeaveIfError( driveSpaces.Append( volSpace / KB ) );
        }

    TInt selection( 0 );

    error = iCommonDialogs->ShowDriveSelectionDialogL( aTotalSize,
                                                       driveUnits,
                                                       driveSpaces,
                                                       selection );
    if ( error == KErrNoMemory )
        {
        // The engine will call us with proper ota status ( no memory )
        aSufficientSpace = EFalse;
        result = ETrue;
        }
    else if ( error == KErrCancel )
        {
        // User cancelled the query
        aSufficientSpace = ETrue;
        result = EFalse;
        }
    else
        {
        // OK To install
        result = ETrue;
        aDrive = driveUnits[selection];
        aSufficientSpace = ETrue;
        }

    CleanupStack::PopAndDestroy( 2 ); // &driveSpaces, &driveUnits

    iShowingDialog = EFalse;

    return result;
    }

// ============================================================================
// CWidgetUIInstallHandler::DisplayCompleteL()
// Display install completed information
//
// @since 3.1
// ============================================================================
//
void CWidgetUIHandler::DisplayCompleteL()
    {
    if ( !iShownFinalNote )
        {
        HBufC* string = NULL;
        if ( iMode == EModeInstall )
            {
            string = StringLoader::LoadLC( R_WIDGETUI_INSTALL_COMPLETED );
            }
        else
            {
            string = StringLoader::LoadLC( R_WIDGETUI_UNINSTALL_COMPLETED );
            }

        CAknConfirmationNote* note = new (ELeave) CAknConfirmationNote( ETrue );
        note->ExecuteLD( *string );
        CleanupStack::PopAndDestroy( string );
        }

    iShownFinalNote = ETrue;
    }

// ============================================================================
// CWidgetUIInstallHandler::DisplayCancelL()
// Display install canceled information
//
// @since 3.1
// ============================================================================
//
void CWidgetUIHandler::DisplayCancelL()
{
    HBufC* string = NULL;
    if ( iMode == EModeInstall )
    {
        string = StringLoader::LoadLC( R_WIDGETUI_INSTALL_CANCELED );
    }
    else
    {
        string = StringLoader::LoadLC( R_WIDGETUI_UNINSTALL_CANCELED );
    }

    CAknConfirmationNote* note = new ( ELeave ) CAknConfirmationNote( ETrue );
    note->ExecuteLD( *string );
    CleanupStack::PopAndDestroy( string );
}

// ============================================================================
// CWidgetUIInstallHandler::DisplayFinalizeDialogL()
// Display the finalize dialog
//
// @since 3.1
// ============================================================================
//
void CWidgetUIHandler::DisplayFinalizeDialogL()
    {
    iCommonDialogs->ShowWaitDialogL( R_WIDGETUI_FINALIZE_INSTALL,
                                     this, R_AVKON_SOFTKEYS_EMPTY );
    }

// ============================================================================
// CWidgetUIInstallHandler::DisplayUninstallInProgressL()
// Display the finalize dialog
//
// @since 5.0
// ============================================================================
//
void CWidgetUIHandler::DisplayUninstallInProgressL()
    {
    iCommonDialogs->ShowWaitDialogL( R_WIDGETUI_UNINSTALL_PROGRESS,
                                     this, R_AVKON_SOFTKEYS_EMPTY );
    }
    
// ============================================================================
// CWidgetUIInstallHandler::DisplayUntrustedSecurityDialogL()
// Display the untrusted widget dialog
//
// @since 5.0
// ============================================================================
//
TBool CWidgetUIHandler::DisplayUntrustedSecurityDialogL()
    {
    // Untrusted Widget Installation dialog
    TInt securityPrompt(0);
    TBool resp(ETrue);
    TRAP_IGNORE(
        CRepository* rep = CRepository::NewL( TUid::Uid( KSWInstWidgetUIUid ) );
        rep->Get( KWidgetSecurityPromptUntrustedInstall, securityPrompt );
        delete rep; );
        
    if (securityPrompt)
        {
        resp = iCommonDialogs->ShowSecurityWarningDialogL();
        }
    return resp;
    }
    
//  End of File
