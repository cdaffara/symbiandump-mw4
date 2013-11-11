/*
* Copyright (c) 2006, 2008 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/

// INCLUDE FILES
#include <f32file.h>
#include <SWInstDefs.h>
#include <zipfile.h>

#include <SWInstTaskManager.h>
#include <SWInstTask.h>
#include <SWInstLogTaskParam.h>
#include <apacmdln.h>
#include <s32mem.h>

#include <widgetappdefs.rh>
#include "WidgetUIOperationsWatcher.h"
#include "WidgetUIConfigHandler.h" // info.plist parser
#include "WidgetRegistrationManager.h" // interface to "shell"

// CONSTANTS
_LIT( KInfoPlist,"Info.plist" );
_LIT( KIconFile, "Icon.png" );
_LIT( KMBMExt, ".mbm");
_LIT( KLprojExt, ".lproj" );
_LIT( KInfoPlistStrings, "InfoPlist.strings" );
_LIT( KWidgetAppDir, "\\private\\10282822\\" );
_LIT( KCookieFile, "c:\\private\\101f8530\\cookies.dat" );


const TUint KMaxDescLength = 0x0fffffffu;

// Note about filenames: All file name matching is case insensitive
// the EDriveName is stored as "c:" (withou a trailing backslash) all
// filenames are kept as complete pathnames (drive + path + filename)
// except when working with the widget bundle where the bundle
// contains relative paths

// TODO
// filenames are kept as drive relative pathnames (path + filename but
// not drive).  The widget bundle must contain relative paths.  The
// drive letter is separated because support for multiple drives and
// removable memory cards means that drive letters may change.


using namespace SwiUI;

// =========================== MEMBER FUNCTIONS ===============================


// ============================================================================
// CWidgetUIOperationsWatcher::NewL()
// two-phase constructor
//
// @since 3.1
// @return pointer to CWidgetUIOperationsWatcher
// ============================================================================
//
CWidgetUIOperationsWatcher* CWidgetUIOperationsWatcher::NewL()
    {
    CWidgetUIOperationsWatcher* self
        = new (ELeave) CWidgetUIOperationsWatcher();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ============================================================================
// CWidgetUIOperationsWatcher::CWidgetUIOperationsWatcher()
// C++ constructor
//
// @since 3.1
// ============================================================================
//
CWidgetUIOperationsWatcher::CWidgetUIOperationsWatcher()
    : CActive( CActive::EPriorityStandard ),
      iPropertyValues( EWidgetPropertyIdCount ),
      iWidgetInHS( EFalse )
    {
    CActiveScheduler::Add( this );
    }

// ============================================================================
// CWidgetUIOperationsWatcher::ConstructL()
// Symbian second phase constructor
//
// @since 3.1
// ============================================================================
//
void CWidgetUIOperationsWatcher::ConstructL()
    {
    User::LeaveIfError( iRfs.Connect() );
    User::LeaveIfError( iRfs.ShareProtected() );
    iFileMgr = CFileMan::NewL( iRfs );

    iAppManager = CWidgetRegistrationManager::NewL( iRfs );
    iWidgetConfigHandler = CWidgetUIConfigHandler::NewL();

    User::LeaveIfError( iRegistry.Connect() );
    iServerConnected = ETrue;

    TInt i = 0;
    // empty values
    for ( ; i < EWidgetPropertyIdCount; ++i )
        {
        CWidgetPropertyValue* value = CWidgetPropertyValue::NewL();
        User::LeaveIfError( iPropertyValues.Insert( value, i ) );
        }
    *(iPropertyValues[EWidgetPropertyListVersion]) = WIDGETPROPERTYLISTVERSION;

    iTaskManager = CTaskManager::NewL();
    }

// ============================================================================
// CWidgetUIOperationsWatcher::~CWidgetUIOperationsWatcher()
// destructor
//
// @since 3.1
// ============================================================================
//
CWidgetUIOperationsWatcher::~CWidgetUIOperationsWatcher()
    {
    CActive::Cancel();

    TInt i = 0;
    for ( ; i < EWidgetPropertyIdCount; ++i )
        {
        delete iPropertyValues[i];
        }
    iPropertyValues.Close();

    delete iMembers;
    delete iZipFile;

    delete iFileMgr;
    iRfs.Close();

    delete iUIHandler;
    delete iWidgetConfigHandler;
    delete iAppManager;
    delete iIconConverter;

    if ( iServerConnected )
        {
        iRegistry.Disconnect();
        }

    delete iTaskManager;
    }

// ============================================================================
// CWidgetUIOperationsWatcher::SilentInstallL()
// Handle silent install request.
//
// @since 3.1
// ============================================================================
//
void CWidgetUIOperationsWatcher::SilentInstallL(
    RFile& aFile,
    const TDesC8& aMIME,
    TRequestStatus& aRequestStatus )
    {
    iSilent = ETrue;
    InstallL( aFile, aMIME, aRequestStatus );
    }

// ============================================================================
// CWidgetUIOperationsWatcher::InstallSubfunctionL()
// Breaks out some install work inside trap macro because trap macro
// intereferes with some debuggers.
//
// @since 5.0
// ============================================================================
//
void CWidgetUIOperationsWatcher::InstallSubfunctionL(
    RFile& aFile,
    TBool& userCancel,
    TRequestStatus& aRequestStatus )
    {
    iZipFile = CZipFile::NewL( iRfs, aFile );

    // do initial bundle processing (parse and validate, total size,
    // check if replacing existing widget with same bundle ID, etc.)
    //
    // this step doesn't do anything that would need to be undone
    TBool replaceExisting = PreprocessWidgetBundleL();

    if ( PromptUserForInstallL( replaceExisting )
         && PromptUserForUntrustedWidgetL( ) )
        {
        // reinitialize
        delete iMembers;
        iMembers = NULL;
        iMembers = iZipFile->GetMembersL();

        // now that install location is known, update MainHTML
        const TDesC& base = *(iPropertyValues[EBasePath]);
        const TDesC& main = *(iPropertyValues[EMainHTML]);
        if ( KMaxFileName < (base.Length() + main.Length()) )
            {
            User::Leave( KErrCorrupt );
            }
        TFileName tempFile;
        tempFile.Copy( base );
        tempFile.Append( main );
        *(iPropertyValues[EMainHTML]) = tempFile;


        // diplay explanation: 1. The progress display only comes up
        // if the total size is over 1 kilobyte (this
        // conditionalization is in UIHandler). 2. The displays stack
        // on top of each other so if progress display comes up it
        // will hide the finalize and then when progesss is removed,
        // the finalize will be revealed.  3. Also, finalize failed to
        // come up when triggered at the end of RunL() on a tiny
        // widget file with no icon.  Why, I'm not sure but probably
        // had to do with this active object blocking display
        // processing.  So here is a good place to put up the displays
        // (before RunL() is invoked) which lets other objects run.

        if ( !iSilent )
            {
            iUIHandler->DisplayFinalizeDialogL(); // a dialog without a cancel option
            iUIHandler->DisplayProgressDialogL(); // progress bar
            }

        if ( replaceExisting )
            {
            BackupL(); // nothing need be undone if backup leaves
            // backup will gaurantee that install dir is empty

            // now don't execute any leaving code before starting RunL
            }
        else
            {
            // new widget, allocate a UID for this widget
            const TDesC& drive = *(iPropertyValues[EDriveName]);
            *(iPropertyValues[EUid]) = iRegistry.GetAvailableUidL(drive[0]).iUid;

            // check that install location is empty and remove what is
            // there if it isn't
            TUint attributes = 0;
            TInt e = iRfs.Att( base, attributes );
            if ( (KErrNotFound != e)
                 && (KErrPathNotFound != e) )
                {
                (void)iFileMgr->RmDir( base );
                }
            }
        }
    else
        {
        userCancel = ETrue;

        if ( !iSilent ) { iUIHandler->DisplayCancelL(); }

        TRequestStatus* status = &aRequestStatus;
        User::RequestComplete( status, KErrNone );
        }
    }

// ============================================================================
// CWidgetUIOperationsWatcher::InstallL()
// NORMAL INSTALL STEPS:
// 1. open zip file
// 2. get metadata file (e.g. Info.plist) and parse it
// 3. prompt user to install (or replace identically named existing widget)
// 4. prompt user for install location (built-in mem or mem card)
//    and do free space check
// 5. if replacing existing widget then move currently installed files
//    to backup for possible restore on install error
// 6. continue install in an active object runl to make install
//    cancellable also do icon processing in a separate active object
//   (the icon processing active object is a questionable design because
//    it doesn't follow the conventional CActive request/response pattern
//    and might as well be done here in the RunL)
//
// @since 3.1
// ============================================================================
//
void CWidgetUIOperationsWatcher::InstallL(
    RFile& aFile,
    const TDesC8& /*aMIME*/,
    TRequestStatus& aRequestStatus )
    {
    // Wrapper CWidgetInstallerUI::InstallL() guards against
    // a second install request while currently installing.

    iCancelled = EFalse;

    // These three bools will be flase until doing something that
    // needs to be cleaned up on error/cancel.
    iUnzipping = EFalse;
    iOverwriting = EFalse;
    iProcessingIcon = EFalse;

    iIconSearchFinished = EFalse;
    iIconError = KErrNone;

    // outside trap because without a UI we can't display an error message
    iUIHandler =
        CWidgetUIHandler::NewL( CWidgetUIHandler::EModeInstall, this );

    TBool userCancel = EFalse;
    TRAPD( error, InstallSubfunctionL( aFile, userCancel, aRequestStatus ) );
    if ( KErrNone != error )
        {
        if ( !iSilent ) { iUIHandler->DisplayErrorL( error ); }
        User::Leave( error );
        }
    else if ( userCancel )
        {
        return; // can't do return inside trap
        }

    // start putting bundle files in install area, registy won't be
    // changed until done unzipping and processing icon (if present)
    iUnzipping = ETrue;

    // set client request to pending state
    iRequestStatus = &aRequestStatus;
    *iRequestStatus = KRequestPending;

    // start RunL
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();
    }

// ============================================================================
// CWidgetUIOperationsWatcher::PreprocessWidgetBundleL()
// Handles parsing and creating widget
//
// @since 3.1
// ============================================================================
//
TBool CWidgetUIOperationsWatcher::PreprocessWidgetBundleL()
    {
    TBool found = EFalse;

    TFileName tempFile;
    TFileName bundleRoot; // root dir in widget bundle (not install root)
    TUint32 zipfileUncompressedSize = 0;

    iMembers = iZipFile->GetMembersL();
    if ( NULL == iMembers )
        {
        User::Leave( KErrNotSupported );
        }
    // compute total installed (uncompressed) size for use in progress
    // meter (doesn't account for some derived things like scaled
    // icons and registry data)
    CZipFileMember* member = iMembers->NextL();
    for ( ; member ; member = iMembers->NextL() )
        {
        zipfileUncompressedSize += member->UncompressedSize();
        delete member;
        }
    if ( 0 == zipfileUncompressedSize )
        {
        User::Leave( KErrNotSupported );
        }
    *(iPropertyValues[EFileSize]) = zipfileUncompressedSize;

    // find the top-level directory under which to get metadata
    // TODO this isn't robust, check cases where there are multiple
    // top-level dirs and no top-level dirs.
    delete iMembers;
    iMembers = NULL;
    iMembers = iZipFile->GetMembersL();
    member = iMembers->NextL();
    TPtrC fName( *member->Name() );
    TInt pos = fName.Locate( KPathDelimiter );
    if ( (KErrNotFound == pos) || (KMaxFileName < pos) )
        {
        User::Leave( KErrNotSupported );
        }
    bundleRoot.Copy( fName.Left( pos ) );
    delete member;
    delete iMembers;
    iMembers = NULL;

    // metadata file: "WeatherBug.wdgt\Info.plist"
    if ( KMaxFileName
         < (bundleRoot.Length() + 1 /* slash */ + KInfoPlist().Length()) )
        {
        User::Leave( KErrCorrupt );
        }
    tempFile.Copy( bundleRoot );
    tempFile.Append( KPathDelimiter );
    tempFile.Append( KInfoPlist );
    CZipFileMember* infoFileMember =
        iZipFile->CaseInsensitiveMemberL( tempFile );
    if ( NULL == infoFileMember )
        {
        // missing metadata file
        User::Leave( KErrNotSupported );
        }
    CleanupStack::PushL( infoFileMember );

    // we will use descriptors to handle the Info.plist contents so
    // the file may not be larger than a descriptor can handle
    if ( KMaxDescLength < infoFileMember->UncompressedSize() )
        {
        User::Leave( KErrCorrupt );
        }

    // extract Info.plist from zip into a buffer
    RZipFileMemberReaderStream* stream;
    iZipFile->GetInputStreamL( infoFileMember, stream );
    CleanupStack::PushL( stream );
    HBufC8* buffer = HBufC8::NewLC( infoFileMember->UncompressedSize() );
    TPtr8 bufferPtr( buffer->Des() );
    User::LeaveIfError(
        stream->Read( bufferPtr, infoFileMember->UncompressedSize() ) );

    // METADATA PARSE
    iWidgetConfigHandler->ParseValidateBundleMetadataL(
        bufferPtr, iPropertyValues, iRfs );

    CleanupStack::PopAndDestroy( 3 ); // infoFileMember, stream, buffer

    // EXISTING WIDGET?
    if( iRegistry.WidgetExistsL( *(iPropertyValues[EBundleIdentifier]) ) )
        {
        // replacement for currently installed widget
        *(iPropertyValues[EUid]) = iRegistry.GetWidgetUidL(
            *(iPropertyValues[EBundleIdentifier]));
        found = ETrue;
        TUid aUid = TUid::Uid( *(iPropertyValues[EUid]) );
        iWidgetInHS = iRegistry.IsWidgetInMiniView( aUid );
        if ( iRegistry.IsWidgetRunning( aUid ) )
            {
            //Runnning widget should be first closed
            RApaLsSession apaLsSession;
            apaLsSession.Connect();
            TApaAppInfo info;

            User::LeaveIfError( apaLsSession.GetAppInfo( info, aUid ) );
            iWidgetName = info.iFullName;
            HBufC *widgetName = iWidgetName.AllocLC();
            HandleWidgetCommandL(apaLsSession, *widgetName, aUid, Deactivate);

            CleanupStack::PopAndDestroy( widgetName );
            apaLsSession.Close();
            }
        // get original install dir from registry in case user
        // decides to "overrite" to another memory location
        iOriginalDir = *( iRegistry.GetWidgetPropertyValueL(
                              TUid::Uid( *(iPropertyValues[EUid]) ),
                              EBasePath ) );
        }
    // uid for a new widget will be gotten once install location (c: or
    // e:) is selected

    // see if main.html is in zip bundle
    const TDesC& main = *(iPropertyValues[EMainHTML]);
    if ( KMaxFileName
         < (bundleRoot.Length() + 1 /* slash */ + main.Length()) )
        {
        User::Leave( KErrCorrupt );
        }
    tempFile.Copy( bundleRoot );
    tempFile.Append( KPathDelimiter );
    tempFile.Append( main );
    CZipFileMember* mainFileMember
        = iZipFile->CaseInsensitiveMemberL( tempFile );
    if ( !mainFileMember )
        {
        // missing main html file
        User::Leave( KErrCorrupt );
        }
    delete mainFileMember;
    mainFileMember = NULL;
    // save MainHTML partial path to combine with base path when
    // install location is selected
    *(iPropertyValues[EMainHTML]) = tempFile;

    // see if there is an Icon.png in root and get partial path
    if ( KMaxFileName
         < (bundleRoot.Length() + 1 /* slash */ + KIconFile().Length()) )
        {
        User::Leave( KErrCorrupt );
        }
    tempFile.Copy( bundleRoot );
    tempFile.Append( KPathDelimiter );
    tempFile.Append( KIconFile );
    CZipFileMember* iconFileMember
        = iZipFile->CaseInsensitiveMemberL( tempFile );
    if ( iconFileMember )
        {
        // save this partial path to identify Icon.png during unzip
        *(iPropertyValues[EIconPath]) = tempFile;
        }
    else
        {
        iIconSearchFinished = ETrue;
        }

    // ex. infoLocFile pathname "WeatherBug.wdgt\en.lproj\InfoPlist.strings"
    TBuf<32> lproj;
    iRegistry.GetLprojName( lproj ); // e.g. "en", "fr", "zh_Hans" ...
    if ( KMaxFileName
         < (bundleRoot.Length() + 1 /* slash */ + lproj.Length()
            + KLprojExt().Length() + 1 /* slash */
            + KInfoPlistStrings().Length()) )
        {
        User::Leave( KErrCorrupt );
        }
    TFileName infoLocFile( bundleRoot );
    infoLocFile.Append( KPathDelimiter );
    infoLocFile.Append( lproj );
    infoLocFile.Append( KLprojExt );
    infoLocFile.Append( KPathDelimiter );
    infoLocFile.Append( KInfoPlistStrings );

    CZipFileMember* infoLocFileMember =
        iZipFile->CaseInsensitiveMemberL( infoLocFile );
    if ( infoLocFileMember )
        {
        CleanupStack::PushL( infoLocFileMember );
        RZipFileMemberReaderStream* stream;
        iZipFile->GetInputStreamL( infoLocFileMember, stream );
        CleanupStack::PushL( stream );

        // we will use descriptors to handle the l10n contents so
        // the file may not be larger than a descriptor can handle
        if ( KMaxDescLength < infoLocFileMember->UncompressedSize() )
            {
            User::Leave( KErrCorrupt );
            }
        HBufC8* buffer
            = HBufC8::NewLC( infoLocFileMember->UncompressedSize() );
        TPtr8 bufferPtr( buffer->Des() );
        User::LeaveIfError(
            stream->Read( bufferPtr, infoLocFileMember->UncompressedSize() ) );

        // parse the l10n file and localize the bundle display name
        iWidgetConfigHandler->ParseInfoLocL(
            bufferPtr, iRfs, *(iPropertyValues[EBundleDisplayName]) );

        CleanupStack::PopAndDestroy( 3 ); // buffer, stream, infoLocFileMember
        }

    return found;
    }

// ============================================================================
// CWidgetUIOperationsWatcher::RunL()
// RunL is called only for Intall/Overwrite.  It's sole resposibilty is
// unzipping the bundle files into the widget subdir.
//
// @since 3.1
// ============================================================================
//
void CWidgetUIOperationsWatcher::RunL()
    {
    if ( !iCancelled )
        {
        CZipFileMember* member = iMembers->NextL();
        if ( member )
            {
            CleanupStack::PushL( member );

            // build up the install pathname
            TPtrC zipedFileName( *member->Name() );
            TFileName zipedFileNameAndPath;
            const TDesC& base = *(iPropertyValues[EBasePath]);
            if ( KMaxFileName < (base.Length() + zipedFileName.Length()) )
                {
                User::Leave( KErrCorrupt );
                }
            zipedFileNameAndPath.Copy( base );
            zipedFileNameAndPath.Append( zipedFileName );

            // index is supposed to separate the path from the
            // basename, unless it is just a directory
            TInt index = zipedFileNameAndPath.LocateReverse( KPathDelimiter );
            TPtrC path = zipedFileNameAndPath.Left( index + 1 /* slash */ );

            // make the directory, it's dumb and will try to create
            // directories that it has already created
            TInt e = iRfs.MkDirAll( path );
            if ( (KErrNone != e)
                 && (KErrAlreadyExists != e) )
                {
                User::Leave( KErrCorrupt );
                }

            // a member can be just a directory name but if it is a file
            // then uncompress the contents
            if ( zipedFileNameAndPath[zipedFileNameAndPath.Length()-1]
                 != KPathDelimiter )  // skip directory paths
                {
                // uncompress the compressed file into a buffer
                RZipFileMemberReaderStream* stream;
                iZipFile->GetInputStreamL( member, stream );
                CleanupStack::PushL( stream );
                TUint32 fileSize = member->UncompressedSize();
                if ( KMaxDescLength < fileSize )
                    {
                    User::Leave( KErrCorrupt );
                    }
                HBufC8* buffer = HBufC8::NewLC( member->UncompressedSize() );
                TPtr8 bufferPtr( buffer->Des() );
                User::LeaveIfError(
                    stream->Read( bufferPtr, member->UncompressedSize() ) );

                // write the buffer into a file
                RFile file;
                CleanupClosePushL( file );
                User::LeaveIfError(
                    file.Create( iRfs, zipedFileNameAndPath, EFileWrite ) );
                if ( fileSize > 0 )
                    {
                    User::LeaveIfError( file.Write( *buffer ) );
                    }
                CleanupStack::PopAndDestroy( 3 ); // file, buffer, stream
                }

            if ( !iSilent )
                {
                // more progress!!!
                if ( member->UncompressedSize() )
                    {
                    iUIHandler->UpdateProgressDialogL( member->UncompressedSize() );
                    }
                }

            // if this is the icon file, then start icon processing
            if ( EFalse == iIconSearchFinished )
                {
                const TDesC& icon = *(iPropertyValues[EIconPath]);
                if ( 0 == icon.CompareF( zipedFileName ) )
                    {
                    iIconSearchFinished = ETrue;
                    const TPtrC& path =
                        zipedFileNameAndPath.Left(index + 1 /* slash */ );
                    *(iPropertyValues[EIconPath]) = path;
                    TUid uid = TUid::Uid( *(iPropertyValues[EUid]) );
                    TRAPD( error, ConvertIconL( uid, path ) );
                    if ( KErrNone != error )
                        {
                        // forget the provided icon and proceed
                        iPropertyValues[EIconPath]->Reset();
                        }
                    }
                }

            CleanupStack::PopAndDestroy(); // member

            // done with that file from the zip, go to next
            TRequestStatus* stat = &iStatus;
            User::RequestComplete( stat, KErrNone );
            SetActive();
            }
        else // done with zip archive
            {
            if ( !iSilent ) { iUIHandler->CloseProgressDialogL(); }
            // and reveal finalize dialog underneath

            // finished unzip
            iUnzipping = EFalse;

            if ( !iProcessingIcon )
                {
                FinishInstallL();
                }
            // if we are processing icons, then we wait for icon
            // processing to complete

            // don't call RequestComplete on installer status (the
            // higher level caller's status) here because we are not
            // done
            }
        }
    else // user cancel (not DoCancel)
        {
        if ( !iSilent )
            {
            iUIHandler->CloseProgressDialogL();
            iUIHandler->CloseFinalizeDialogL();
            iUIHandler->DisplayCancelL();
            }

        // stop icon conversion (if there is a converter)
        delete iIconConverter;
        iIconConverter = NULL;
        iProcessingIcon = EFalse;
        iIconError = KErrNone;

        iUnzipping = EFalse;

        FinishInstallL();
        }
    }

// ============================================================================
// CWidgetUIOperationsWatcher::NotifyCompletionL()
// Icon conversion calls this when it is complete.
//
// @since 3.1
// ============================================================================
//
void CWidgetUIOperationsWatcher::NotifyCompletionL( TInt aErr )
    {
    iProcessingIcon = EFalse;
    iIconError = aErr;

    if ( KErrNone != iIconError )
        {
        iPropertyValues[EIconPath]->Reset();
        }

    // don't really need iCancelled in test as that combination
    // shouldn't be possible
    if ( !(iUnzipping || iCancelled) )
        {
        // NotifyCompletionL shouldn't ever leave since the caller
        // is IconConverter and that is just a utility module.
        TRAP_IGNORE( FinishInstallL() );
        }
    }

// ============================================================================
// CWidgetUIOperationsWatcher::FinishInstallL()
//
// @since 3.1
// ============================================================================
//
void CWidgetUIOperationsWatcher::FinishInstallL()
    {
    if ( !iCancelled )
        {
        if ( iOverwriting )
            {
            TUid uid = TUid::Uid( *(iPropertyValues[EUid]) );
            iRegistry.DeRegisterWidgetL( uid );
            iAppManager->DeregisterWidgetL( uid );
            }

        // TODO if registration steps fail does it leave inconsistent state???

        iRegistry.RegisterWidgetL( iPropertyValues );

        iAppManager->RegisterWidgetL( *(iPropertyValues[EMainHTML]),
                                      *(iPropertyValues[EBundleDisplayName]),
                                      *(iPropertyValues[EIconPath]),
                                      *(iPropertyValues[EDriveName]),
                                      TUid::Uid( *(iPropertyValues[EUid]) ) );

        if ( iOverwriting )
            {
            // delete backup
            (void)iFileMgr->RmDir( iBackupDir );
            if ( iWidgetInHS )
                {
                RApaLsSession apaLsSession;
                apaLsSession.Connect();

                HBufC* widgetName = iWidgetName.AllocLC();
                HandleWidgetCommandL(apaLsSession, *widgetName, TUid::Uid( *(iPropertyValues[EUid]) ), WidgetRestart);
                CleanupStack::PopAndDestroy( widgetName );

                apaLsSession.Close();
                }
            }
        if ( !iSilent )
            {
            iUIHandler->CloseFinalizeDialogL();
            iUIHandler->DisplayCompleteL();
            }

        HandleLogsL(*(iPropertyValues[EBundleDisplayName]), TUid::Uid( *(iPropertyValues[EUid]) ), *(iPropertyValues[ENokiaWidget]), SwiUI::ELogTaskActionInstall);
        }
    else // cancelled
        {
        // delete what was being installed, and restore previous
        (void)iFileMgr->RmDir( *(iPropertyValues[EBasePath]) );

        if ( iOverwriting )
            {
            RestoreL();
            }
        }

    User::RequestComplete( iRequestStatus, KErrNone );
    iRequestStatus = NULL;

    iUnzipping = EFalse;
    iOverwriting = EFalse;
    iProcessingIcon = EFalse;
    iIconError = KErrNone;
    iCancelled = EFalse;
    }

// ============================================================================
// CWidgetUIOperationsWatcher::UserCancelL()
// If user selects cancel softkey on progress dialog only.
//
// @since 3.1
// ============================================================================
//
void CWidgetUIOperationsWatcher::UserCancelL( TBool /*aCancelImmediately*/ )
    {
    // It is only possible to get here while we have the progress
    // dialog up during unzipping (iUnzipping is set).  When unzipping
    // finishes but icon processing is not finished, we display a
    // finalize dialog without a cancel option.

    iCancelled = ETrue;

    // don't do anything other than set the flag as it will be tested
    // while processing
    }

// ============================================================================
// CWidgetUIOperationsWatcher::DoCancel()
// This should not be called directly.  This function is called by
// CActive when Cancel() is called on this object IF there is a
// request outstanding.  So indirectly it handles canceling the
// install operation.  Icon converter doesn't ever directly cause a
// Cancel() even when it errors out because it is a service provider
// to this object.  If this method is called then RunL is not called
// (they are mutually exclusive).
//
// @since 3.1
// ============================================================================
//
void CWidgetUIOperationsWatcher::DoCancel()
    {
    // can't cancel uninstall because that is synchronous and runs in
    // one step and this is only called by CActive if there is an
    // outstanding request, that means we are in the prcess of
    // unzipping the archive (but not in RunL presently), also note
    // that this is not a user cancel (see separate function for that)

    iCancelled = ETrue;

    TRAP_IGNORE(
        {
        if ( !iSilent )
            {
            iUIHandler->CloseProgressDialogL();
            iUIHandler->CloseFinalizeDialogL();
            iUIHandler->DisplayCancelL();
            }

        // stop icon conversion (if there is a converter)
        delete iIconConverter;
        iIconConverter = NULL;
        iProcessingIcon = EFalse;
        iIconError = KErrNone;

        iUnzipping = EFalse;

        FinishInstallL();
        }
        );
    }

// ============================================================================
// CWidgetUIOperationsWatcher::RunError()
// This should not be called directly.  It is called by CActive when
// RunL leaves.  RunL is only for installing so this handles leaving
// errors during installing widget.
//
// @since 3.1
// ============================================================================
//
TInt CWidgetUIOperationsWatcher::RunError( TInt aError )
    {
    iCancelled = ETrue;
    iUnzipping = EFalse;

    TRAP_IGNORE(
        {
        if ( !iSilent )
            {
            iUIHandler->CloseProgressDialogL();
            iUIHandler->CloseFinalizeDialogL();
            iUIHandler->DisplayErrorL( aError );
            }

        // stop icon conversion (if there is a converter)
        delete iIconConverter;
        iIconConverter = NULL;
        iProcessingIcon = EFalse;
        iIconError = KErrNone;

        FinishInstallL();
        }
        );

    return KErrNone; // indicates error was handled
    }

// ============================================================================
// CWidgetUIOperationsWatcher::SilentUninstallL()
// Handle silent uninstall request.
//
// @since 3.1
// ============================================================================
//
void CWidgetUIOperationsWatcher::SilentUninstallL(
    const TUid& aUid,
    TRequestStatus& aRequestStatus )
    {
    iSilent = ETrue;
    UninstallL( aUid, aRequestStatus );
    }

// ============================================================================
// CWidgetUIOperationsWatcher::UninstallL()
// Handle uninstall request.
//
// @since 3.1
// ============================================================================
//
void CWidgetUIOperationsWatcher::UninstallL(
    const TUid& aUid,
    TRequestStatus& aRequestStatus )
    {
    // initialize state
    iCancelled = EFalse;
    iUnzipping = EFalse;
    iOverwriting = EFalse;
    iProcessingIcon = EFalse;
    iIconError = KErrNone;

    // outside trap because without a UI we can't display an error message
    iUIHandler =
        CWidgetUIHandler::NewL( CWidgetUIHandler::EModeUninstall, this );

    // save client status to use in finish uninstall
    iRequestStatus = &aRequestStatus;

    TBuf<KWidgetRegistryVal> bundleName;
    iRegistry.GetWidgetBundleName( aUid, bundleName );

    if( bundleName.Length() <= 0 )
        {
        FinishUninstallL( KErrCorrupt );
        return;
        }

    TBool widgetinHomeScreen(EFalse);
    widgetinHomeScreen = iRegistry.IsWidgetInMiniView( aUid );

    TBool uninstall( ETrue );
    if ( !iSilent )
        {
        uninstall = iUIHandler->DisplayUninstallL( bundleName,widgetinHomeScreen );
        }
    // prompt user to uninstall
    if( uninstall )
        {
        if(iRegistry.IsWidgetRunning( aUid ))
        //Runnning widget should be first closed
            {
            RApaLsSession apaLsSession;
            apaLsSession.Connect();
            TApaAppInfo info;

            User::LeaveIfError( apaLsSession.GetAppInfo( info, aUid ) );
            HBufC* widgetName = info.iFullName.AllocLC();
            HandleWidgetCommandL(apaLsSession,*widgetName,aUid,Deactivate);

            CleanupStack::PopAndDestroy( widgetName );
            apaLsSession.Close();
            }
        if ( !iSilent ) { iUIHandler->DisplayUninstallInProgressL(); }
        TBuf<KWidgetRegistryVal> widgetPath;
        iRegistry.GetWidgetPath( aUid, widgetPath );
        TBool aVendor = *(iRegistry.GetWidgetPropertyValueL(aUid, ENokiaWidget));


        // TODO if any of next steps leave does state become inconsistent?

        // remove the dir for the widget
        // TODO why this validation?
        if ( widgetPath.Length() > 0 )
            {
            iFileMgr->RmDir( widgetPath );
            //Widget should delete any cookie it created
            HBufC* fileName = HBufC::NewLC(KMaxFileName);
            TPtr ptr(fileName->Des());
            TBuf<8> buf1;
            ptr = KCookieFile;
            TInt pos = ptr.LocateReverse('.');
            buf1.AppendNum(aUid.iUid,EHex);
            ptr.Insert(pos,_L("_"));
            ptr.Insert(pos+1,buf1);
            iRfs.Delete(ptr);
            CleanupStack::PopAndDestroy(fileName);
            }

        iRegistry.DeRegisterWidgetL( aUid );
        iAppManager->DeregisterWidgetL( aUid );
        TInt err = KErrNone;
        TRAP(err, FinishUninstallL( KErrNone ));
        if(err == KErrNone)
           {
           HandleLogsL(bundleName, aUid, aVendor, SwiUI::ELogTaskActionUninstall);
           }
        }
    else
        {
        if ( !iSilent ) { iUIHandler->DisplayCancelL(); }

        // must return cancel because upper-levels will take uninstall
        // actions (remove widget from applications list) if we return KErrNone
        User::RequestComplete( iRequestStatus, KErrCancel );
        return;
        }
    }

// ============================================================================
// CWidgetUIOperationsWatcher::FinishUninstallL()
//
// @since 3.1
// ============================================================================
//
void CWidgetUIOperationsWatcher::FinishUninstallL( TInt aErr )
    {
    if ( !iSilent ) { iUIHandler->CloseUninstallInProgressDialogL(); }
    if ( aErr )
        {
        if ( !iSilent ) { iUIHandler->DisplayErrorL( aErr ); }
        User::Leave( aErr );
        }
    if ( !iSilent ) { iUIHandler->DisplayCompleteL(); }
    User::RequestComplete( iRequestStatus, KErrNone );
    iRequestStatus = NULL;
    }

// ============================================================================
// CWidgetUIOperationsWatcher::IsShowingDialog()
//
// @since 3.1
// ============================================================================
//
TBool CWidgetUIOperationsWatcher::IsShowingDialog()
    {
    return ETrue;
    }

// ============================================================================
// CWidgetUIOperationsWatcher::CancelEngine()
//
// @since 3.1
// ============================================================================
//
void CWidgetUIOperationsWatcher::CancelEngine()
    {
    // TODO ???
    }

// ============================================================================
// CWidgetUIOperationsWatcher::ForceCancel()
//
// @since 3.1
// ============================================================================
//
void CWidgetUIOperationsWatcher::ForceCancel()
    {
    // TODO ???
    }

// ============================================================================
// CWidgetUIOperationsWatcher::StartedCancellingL()
//
// @since 3.1
// ============================================================================
//
void CWidgetUIOperationsWatcher::StartedCancellingL()
    {
    // TODO ???
    }

// ============================================================================
// CWidgetUIOperationsWatcher::SelectMemoryL()
//
// @since 3.1
// ============================================================================
//
TBool CWidgetUIOperationsWatcher::SelectMemoryL()
    {
    TDriveUnit selectedDrive;
    TBool isSufficient = EFalse;

    if ( !iUIHandler->SelectDriveL( iRfs,
                                    selectedDrive, isSufficient,
                                    *(iPropertyValues[EFileSize] )) )
        {
        UserCancelL();
        return EFalse;
        }
    else if ( !isSufficient )
        {
        User::Leave( KErrNoMemory );
        }

    UpdateWidgetBasePathL( selectedDrive );
    *(iPropertyValues[EDriveName]) = selectedDrive.Name();

    return ETrue;
    }

// ============================================================================
// CWidgetUIOperationsWatcher::UpdateWidgetBasePathL()
//
// @since 3.1
// ============================================================================
//
void CWidgetUIOperationsWatcher::UpdateWidgetBasePathL( TDriveUnit& aDrive )
    {
    const TDesC& bundleId = *(iPropertyValues[EBundleIdentifier]);
    HBufC* basePath = HBufC::NewL(
        aDrive.Name().Length() + KWidgetAppDir().Length()
        + bundleId.Length() + 1 /* slash */ );
    basePath->Des().Append( aDrive.Name() );
    basePath->Des().Append( KWidgetAppDir );
    basePath->Des().Append( *(iPropertyValues[EBundleIdentifier]) );
    basePath->Des().Append( KPathDelimiter );

    *(iPropertyValues[EBasePath]) = *basePath;
    delete basePath;
    }


// ============================================================================
// CWidgetUIOperationsWatcher::ConvertIconL()
// Convert icon.png into mbm format for widget
//
// @since 3.1
// ============================================================================
//
void CWidgetUIOperationsWatcher::ConvertIconL(
    TUid& aUid,
    const TDesC& aIconPath )
    {
    delete iIconConverter;
    iIconConverter = NULL;

    iIconConverter = CIconConverter::NewL( this, iRfs );

    TFileName pngIcon;
    pngIcon.Copy( aIconPath );

    if ( pngIcon.Length() )
        {
        pngIcon.Append( KIconFile );
        TFileName mbmIcon;
        mbmIcon.Copy( aIconPath );
        mbmIcon.Append( aUid.Name() );
        mbmIcon.Append( KMBMExt() );

        iIconConverter->StartToDecodeL( pngIcon, mbmIcon );
        iProcessingIcon = ETrue;
        }
    }

// ============================================================================
// CWidgetUIOperationsWatcher::PromptUserForInstallL()
// Prompt to confirm install and where to install. SetActive() has not
// been called yet.
//
// @since 3.1
// ============================================================================
//
TBool CWidgetUIOperationsWatcher::PromptUserForInstallL( TBool aOverwrite )
    {
    TBool userAnswer( ETrue );

    if ( !iSilent )
        {
        if ( aOverwrite )
            {
            userAnswer =
                iUIHandler->DisplayOverwriteL( *(iPropertyValues[EBundleDisplayName]) );
            }
        else
            {
            userAnswer =
                iUIHandler->DisplayInstallL( *(iPropertyValues[EBundleDisplayName]) );
            }
        if ( userAnswer )
            {
            userAnswer = SelectMemoryL();
            }
        }
    else
            {
            TDriveUnit selectedDrive(_L("C"));        // in real should be read from install options which are ignore too at the moment.
            UpdateWidgetBasePathL( selectedDrive );
            *(iPropertyValues[EDriveName]) = selectedDrive.Name();
            }

    return userAnswer;
    }

// ============================================================================
// CWidgetUIOperationsWatcher::PromptUserForUntrustedWidgetL()
// Prompt to confirm install of untrusted widget. SetActive() has not
// been called yet.
//
// @since 3.1
// ============================================================================
//
TBool CWidgetUIOperationsWatcher::PromptUserForUntrustedWidgetL( )
    {
    TBool userAnswer(ETrue);

    if ( !iSilent )
        {
        userAnswer = iUIHandler->DisplayUntrustedSecurityDialogL();
        }

    return userAnswer;
    }

// ============================================================================
// CWidgetUIOperationsWatcher::BackupL()
// When overwriting an installed widget, backup so can restore on error.
//
// @since 3.1
// ============================================================================
//
void CWidgetUIOperationsWatcher::BackupL()
    {
    // if backup leaves, there is nothing caller needs to do to clean up

    // move to backup (by renaming) the existing install dir for the widget

    // first create a "unique" backup dir on the original dir drive
    // (same drive required since can't "rename" across drives)
    TParse p;
    p.Set( iOriginalDir, NULL, NULL );
    TFileName path;
    path.Copy( p.Drive() );
    path.Append( KWidgetAppDir );
    // NOT A GOOD WAY TO CREATE A UNIQUE DIRECTORY First we create a
    // unique file and then delete it and then create a directory with
    // same name.
    RFile file;
    CleanupClosePushL( file );
    User::LeaveIfError( file.Temp( iRfs, path, iBackupDir, EFileWrite ) );
    CleanupStack::PopAndDestroy(); // file
    // delete the temp file and...
    User::LeaveIfError( iFileMgr->Delete( iBackupDir ) );
    // ...convert to dir name
    iBackupDir.Append( KPathDelimiter );
    // make the backup
    User::LeaveIfError( iFileMgr->Rename( iOriginalDir, iBackupDir ) );
    // original dir doesn't seem to be gone after rename so rmdir
    // it...this is IMPORTANT because we want to be able to look at
    // the list of install directories to get a list of installed
    // widgets without checking if they are empty!
    (void)iFileMgr->RmDir( iOriginalDir );
    // overwriting boolean is only true when backup is successful
    iOverwriting = ETrue;
    }

// ============================================================================
// CWidgetUIOperationsWatcher::RestoreL()
//
// @since 3.1
// ============================================================================
//
void CWidgetUIOperationsWatcher::RestoreL()
    {
    // move (by renaming) the backup copy to the install dir for the widget

    // to assure a clean destination, try deleting install dir first
    (void)iFileMgr->RmDir( iOriginalDir );

    // TODO if this fails then do some cleanup, maybe unregister?
    User::LeaveIfError( iFileMgr->Rename( iBackupDir, iOriginalDir ) );
    // backup dir doesn't seem to be gone so rmdir it
    (void)iFileMgr->RmDir( iBackupDir );

    // restore complete
    iOverwriting = EFalse;
    }


// ============================================================================
//
// CWidgetUIOperationsWatcher::HandleLogsL
//
// ============================================================================
void CWidgetUIOperationsWatcher::HandleLogsL(const TDesC& aWidgetName, const TUid &aUid, TBool aVender, SwiUI::TLogTaskAction aAction)
    {
    CTask* task = CTask::NewL( KLogTaskImplUid, EFalse );
    CleanupStack::PushL(task);

    TLogTaskParam params;
    params.iName.Copy(aWidgetName);
    params.iUid = aUid;
    params.iAction = aAction;
    params.iVendor.Copy((aVender ? _L("Nokia") : _L("Non-Nokia")));

    TTime time;
    time.UniversalTime();
    params.iTime = time;

    TLogTaskParamPckg pckg( params );
    task->SetParameterL( pckg, 0 );
    iTaskManager->AddTaskL( task );
    iTaskManager->CommitL();

    CleanupStack::Pop(task);
    }

// ============================================================================
//
// CWidgetUIOperationsWatcher::HandleWidgetCommandL
//
// ============================================================================
void CWidgetUIOperationsWatcher::HandleWidgetCommandL( RApaLsSession& aSession,const TDesC& aWidget,const TUid& aUid,TUint32 aOperation )
    {
    const TInt size( 2* aWidget.Length() + 3*sizeof( TUint32 ) );

    // Message format is <filenameLength><unicode_filename><someintegervalue>
    CApaCommandLine* cmd( CApaCommandLine::NewLC() );
    HBufC8* opaque( HBufC8::NewLC( size ) );

    RDesWriteStream stream;
    TPtr8 des( opaque->Des() );

    stream.Open( des );
    CleanupClosePushL( stream );

    // Generate the command.
    stream.WriteUint32L( aUid.iUid );
    stream.WriteUint32L( aWidget.Length() );
    stream.WriteL( reinterpret_cast< const TUint8* >( aWidget.Ptr() ),
                   aWidget.Size() );

    stream.WriteInt32L( aOperation );

    CleanupStack::PopAndDestroy( &stream );

    // Generate command.
    cmd->SetCommandL( EApaCommandBackgroundAndWithoutViews );
    cmd->SetOpaqueDataL( *opaque );

    CleanupStack::PopAndDestroy( opaque );

    cmd->SetExecutableNameL( KLauncherApp );

    User::LeaveIfError( aSession.StartApp( *cmd ) );
    CleanupStack::PopAndDestroy( cmd );
    }
//  End of File
