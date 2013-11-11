/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Widget's active callback in WidgetBackupRestore.
*
*/


// INCLUDE FILES
#include <abclient.h>
#include <bautils.h>

#include "WidgetActiveCallback.h"
#include "WidgetInstaller.h"

//  CONSTANTS
_LIT(KWidgetUiPath, "\\private\\10282822\\");
_LIT(KWidgetBURTempPath, "WidgetBURTemp\\");
_LIT(KWidgetBURDummy, "dummy data for backup");
_LIT( KWidgetEntryStoreXmlFile, "\\private\\10282f06\\WidgetEntryStore.xml" );
// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CWidgetActiveCallback* CWidgetActiveCallback::NewL()
    {
    CWidgetActiveCallback* self =
        new( ELeave ) CWidgetActiveCallback();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CWidgetActiveCallback::~CWidgetActiveCallback()
    {
    delete iPathBUR;
    iFs.Close();
#ifdef _DEBUG
    iFileLogger.Close();
#endif
   }

// ---------------------------------------------------------
// CWidgetActiveCallback::CleanupBackupDataL
//
// Cleaning for passive backup: move private area from widgetBackupRestore back to widgetUI.
// ---------------------------------------------------------
// 
void CWidgetActiveCallback::CleanupBackupDataL()
    {
    CFileMan* fileManager = CFileMan::NewL( iFs );
    CleanupStack::PushL( fileManager );

    // Move widget data from BUR path back to widgetUI
    User::LeaveIfError( fileManager->Move( *iPathBUR, KWidgetUiPath, CFileMan::ERecurse ) );
    // Remove the temp folder for BUR
    User::LeaveIfError( fileManager->RmDir( *iPathBUR ) );

    CleanupStack::PopAndDestroy();  // fileMananger
    }

// ---------------------------------------------------------------------------
// Inform that all data has been backed up or restored.
// ---------------------------------------------------------------------------
//
void CWidgetActiveCallback::AllSnapshotsSuppliedL()
    {
    // Finalize and cleanup.
    return;
    }

// ---------------------------------------------------------------------------
// Not supported.
// ---------------------------------------------------------------------------
//
void CWidgetActiveCallback::ReceiveSnapshotDataL(
    TDriveNumber /*aDrive*/, TDesC8& /*aBuffer*/, TBool /*aLastSection*/)
    {
    User::Leave( KErrNotSupported );
    }

// ---------------------------------------------------------------------------
// Make a guess about data size.
// ---------------------------------------------------------------------------
//
TUint CWidgetActiveCallback::GetExpectedDataSize(
    TDriveNumber /*aDrive*/)
    {
    // we have no idea at this point - we even don't know what is to be
    // backed up yet
    return 0;
    }

// ---------------------------------------------------------------------------
// Not supported.
// ---------------------------------------------------------------------------
//
void CWidgetActiveCallback::GetSnapshotDataL(
    TDriveNumber /*aDrive*/, TPtr8& /*aBuffer*/, TBool& /*aFinished*/)
    {
    User::Leave( KErrNotSupported );
    }

// ---------------------------------------------------------------------------
// Initialize for backup.
// ---------------------------------------------------------------------------
//
void CWidgetActiveCallback::InitialiseGetBackupDataL(
    TDriveNumber /*aDrive*/)
    {
    // create pathBUR and put data for passive backup in it
    PrepareBackupDataL();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CWidgetActiveCallback::GetBackupDataSectionL(
    TPtr8& aBuffer, TBool& aFinished)
    {
    // Make sure that the buffer is empty and starts from the beginning
    aBuffer.SetLength(0);
    // Set dummy data
    aBuffer.Append( KWidgetBURDummy );

    aFinished = ETrue;
    }

// ---------------------------------------------------------------------------
// Initialize restore.
// ---------------------------------------------------------------------------
//
void CWidgetActiveCallback::InitialiseRestoreBaseDataL(
    TDriveNumber /*aDrive*/ )
    {
    return;
    }

// ---------------------------------------------------------------------------
// Run state machine for restore. Receive stream from BUR engine and turn it
// to file(s).
// ---------------------------------------------------------------------------
//
void CWidgetActiveCallback::RestoreBaseDataSectionL(
    TDesC8& /*aBuffer*/, TBool aFinished )
    {
    if( aFinished )
        {
        // If the temp folder for BUR exists, install widgets
        if ( BaflUtils::FolderExists( iFs, *iPathBUR ) )
            {
            InstallL();
            }
        }
    }

// ---------------------------------------------------------------------------
// Incremental restoration is not supported.
// ---------------------------------------------------------------------------
//
void CWidgetActiveCallback::InitialiseRestoreIncrementDataL(
    TDriveNumber /*aDrive*/)
    {
    User::Leave( KErrNotSupported );
    }

// ---------------------------------------------------------------------------
// Incremental restoration is not supported.
// ---------------------------------------------------------------------------
//
void CWidgetActiveCallback::RestoreIncrementDataSectionL(
    TDesC8& /*aBuffer*/, TBool /*aFinished*/)
    {
    User::Leave( KErrNotSupported );
    }

// ---------------------------------------------------------------------------
// Called when restore is complete - sets data back to initial state.
// ---------------------------------------------------------------------------
//
void CWidgetActiveCallback::RestoreComplete(TDriveNumber /*aDrive*/)
    {
    return;
    }

// ---------------------------------------------------------------------------
// Tidy up when operation is over.
// ---------------------------------------------------------------------------
//
void CWidgetActiveCallback::TerminateMultiStageOperation()
    {
    return;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TUint CWidgetActiveCallback::GetDataChecksum(TDriveNumber /*aDrive*/)
    {
    // not required - not implemented
    return 0;
    }

// ---------------------------------------------------------------------------
// C++ constructor.
// ---------------------------------------------------------------------------
//
CWidgetActiveCallback::CWidgetActiveCallback()
    {
    }

// ---------------------------------------------------------------------------
// 2nd phase constructor.
// ---------------------------------------------------------------------------
//
void CWidgetActiveCallback::ConstructL()
    {
#ifdef _DEBUG
    _LIT(KDir, "WidgetBUR");
    _LIT(KFile, "WidgetBURCallback.log");
    TInt err( 0 );

    err = iFileLogger.Connect();
    if ( err == KErrNone )
        {
        iFileLogger.CreateLog( KDir(), KFile(), EFileLoggingModeOverwrite );
        iCanLog = ETrue;
        }
#endif
    // Get the path to the default folder.
    TFileName  path;
    User::LeaveIfError( iFs.Connect() );
    iFs.PrivatePath( path );

    // Test whether or not the private area is present on C:.
    // If it is missing, create it.
    if ( !BaflUtils::FolderExists( iFs, path ) )
        {
        User::LeaveIfError( iFs.CreatePrivatePath(EDriveC) );
        }

    // The temp path for BUR
    iPathBUR = HBufC16::NewL( path.Length() + KWidgetBURTempPath().Length() );
    iPathBUR->Des().Copy( path );
    iPathBUR->Des().Append( KWidgetBURTempPath );
    }

// ---------------------------------------------------------------------------
// CWidgetActiveCallback::PrepareBackupDataL()
//
// Preparing for passive backup: move private area from widgetUI to widgetBackupRestore.
// ---------------------------------------------------------------------------
//
void CWidgetActiveCallback::PrepareBackupDataL()
    {
    User::LeaveIfError( iFs.MkDir( *iPathBUR ) );

    // Move widget data to BUR path
    CFileMan* fileManager = CFileMan::NewL( iFs );
    CleanupStack::PushL( fileManager );
    // Prepare to backup the registration files 
    User::LeaveIfError( fileManager->Copy( KWidgetEntryStoreXmlFile, KWidgetUiPath, CFileMan::EOverWrite ) );
    User::LeaveIfError( fileManager->Move( KWidgetUiPath, *iPathBUR, CFileMan::ERecurse ) );
    CleanupStack::PopAndDestroy();  // fileMananger
    }

// ============================================================================
// CWidgetActiveCallback::InstallL
// Perform installation.
// (other items were commented in a header).
//
// @since 3.2
// ============================================================================
//
void CWidgetActiveCallback::InstallL()
    {
    CDir* dir = NULL;

    if ( iFs.GetDir( *iPathBUR, KEntryAttMatchMask, EDirsFirst, dir ) == KErrNone )
        {
        CleanupStack::PushL(dir);
        
        CWidgetInstaller* installer = NULL;
        TRAPD( err , installer = CWidgetInstaller::NewL() );
        User::LeaveIfError( err );
        if ( !installer )
            {
            User::Leave( KErrGeneral );
            }

        for (TInt i=0; i<dir->Count(); i++)
            {
            const TEntry& fileEntry = (*dir)[i];
            
            // check for directory entries, i.e. "com.yahoo.local.widget.traffic"
            if (fileEntry.IsDir())
                {
                // e.g.  restorePath = "\private\[WidgetBackupRestoreUid]\WidgetBURTemp\com.yahoo.local.widget.traffic"
                TFileName restorePath( *iPathBUR );
                restorePath.Append(fileEntry.iName);

                // installer will move the restorePath to widgetUI private area
                TRAPD( err, installer->InstallL( restorePath ) );
                if ( err )
                    {
                    installer->RunError( err );
                    if ( err == KErrNoMemory || err == KErrDiskFull )
                        {
                        User::Leave( err );
                        }               
                    }
                } // end of if (fileEntry
            else
                {
                // only care about dir
                break;
                }
            }  // end of for (

        delete installer;
        
        CleanupStack::PopAndDestroy();  // dir

        // Remove the temp folder for BUR
        CFileMan* fileManager = CFileMan::NewL( iFs );
        CleanupStack::PushL( fileManager );
        User::LeaveIfError( fileManager->RmDir( *iPathBUR ) );
        CleanupStack::PopAndDestroy();  // fileManager
        }  // end of if ( iFs.GetDir
    }
    
// End of file
