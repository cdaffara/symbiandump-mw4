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
* Description:  This is defines CWidgetRegistrationManager which handles
*                registration of widgets
*
*/

#include "WidgetRegistrationManager.h"
#include "WidgetInstaller.h"
#include <apgcli.h>
#include <apgicnfl.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <apgicnflpartner.h>
//#include <apgicnflinternal.h> // missing
#endif

#include <s32mem.h>
#include <WidgetRegistryData.h>

// CONSTANTS
_LIT(KMBMExt, ".mbm");


// MEMBER FUNCTION DECLARATIONS

// ============================================================================
// CWidgetRegistrationManager::NewL()
// two-phase constructor
//
// @since 3.1
// ============================================================================
//
CWidgetRegistrationManager* CWidgetRegistrationManager::NewL( RFs& aFs )
    {
    CWidgetRegistrationManager *self =
        new ( ELeave ) CWidgetRegistrationManager( aFs );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ============================================================================
// CWidgetRegistrationManager::CWidgetRegistrationManager()
// C++ constructor
//
// @since 3.1
// ============================================================================
//
CWidgetRegistrationManager::CWidgetRegistrationManager(
    RFs& aFs )
    : iFs( aFs )
    {
    }

// ============================================================================
// CWidgetRegistrationManager::~CWidgetRegistrationManager()
// destructor
//
// @since 3.1
// ============================================================================
//
CWidgetRegistrationManager::~CWidgetRegistrationManager()
    {
    }

// ============================================================================
// CWidgetRegistrationManager::CWidgetRegistrationManager()
// Symbian second phase constructor
//
// @since 3.1
// ============================================================================
//
void CWidgetRegistrationManager::ConstructL()
    {
    }

// ============================================================================
// CWidgetRegistrationManager::DeregisterWidgetL()
// Deregister installed widget as non native app
//
// @since 3.1
// ============================================================================
//
void CWidgetRegistrationManager::DeregisterWidgetL( const TUid& aUid )
    {
    RApaLsSession apparcSession;
    User::LeaveIfError( apparcSession.Connect() );

    apparcSession.PrepareNonNativeApplicationsUpdatesL();
    apparcSession.DeregisterNonNativeApplicationL( aUid );
    apparcSession.DeregisterNonNativeApplicationTypeL( aUid );
    apparcSession.CommitNonNativeApplicationsUpdatesL();
    apparcSession.Close();
    }

// ============================================================================
// CWidgetRegistrationManager::RegisterWidgetL()
// Register installed widget as non native app
//
// It will leave if registration is impossible or fails.  It will not
// leave if icon path is KNullDesC or unusable since a default icon
// will be used.
//
// @since 3.1
// ============================================================================
//
void CWidgetRegistrationManager::RegisterWidgetL(
    const TDesC& aMainHTML,
    const TDesC& aBundleDisplayName,
    const TDesC& aIconPath,
    const TDesC& aDriveName,
    const TUid& aUid )
    {
    RApaLsSession apparcSession;
    CleanupClosePushL( apparcSession );
    User::LeaveIfError( apparcSession.Connect() );
    // TODO move this where it is only done once
    apparcSession.PrepareNonNativeApplicationsUpdatesL();
    apparcSession.DeregisterNonNativeApplicationL( KUidWidgetLauncher );
    apparcSession.DeregisterNonNativeApplicationTypeL( KUidWidgetLauncher );
    apparcSession.CommitNonNativeApplicationsUpdatesL();
    // TODO end

    // reasonably like an acceptable file name
    TBuf<KWidgetRegistryVal> appName;
    appName.Append( aMainHTML );

    CApaRegistrationResourceFileWriter* writer =
        CApaRegistrationResourceFileWriter::NewL(
            aUid,
            appName,
            TApaAppCapability::ENonNative );
    CleanupStack::PushL( writer );

    TBuf8<KWidgetRegistryVal> opaqueData;
    RDesWriteStream writeStream( opaqueData );

    // le data opaque
    writeStream.WriteUint32L( aUid.iUid );
    writeStream.WriteUint32L( aMainHTML.Length() );
    writeStream.WriteL( aMainHTML );

    writeStream.CommitL();
    writer->SetOpaqueDataL( opaqueData );

    // avec nom de plume
    CApaLocalisableResourceFileWriter* lWriter =
        CApaLocalisableResourceFileWriter::NewL(
            KNullDesC,
            aBundleDisplayName,
            1 ,
            KNullDesC );
    CleanupStack::PushL( lWriter );

    // This call deletes any pending registrations which are not commited
    // in previous installations (ex: power off case).
    // This must be the first call after session connect,
    // and before Prepare... call.
    // This function returns with no effect, if no pending registrations in
    // previous install.
    apparcSession.RollbackNonNativeApplicationsUpdates();

    // Prepare for Registration & Deregistration.
    // Actual Registration & Deregistration will be done at Commit call
    // CommitNonNativeApplicationsUpdatesL.
    apparcSession.PrepareNonNativeApplicationsUpdatesL();

    RFile appArcIconFile;
    CleanupClosePushL( appArcIconFile );
    RFile* toIconFile = NULL;

    // la petit image (ou non)
    if ( aIconPath.Length() )
        {
        TFileName mbmIcon;
        mbmIcon.Append( aIconPath );
        mbmIcon.Append( aUid.Name() );
        mbmIcon.Append( KMBMExt() );
        TInt error =
            appArcIconFile.Open(
                iFs, mbmIcon,
                EFileShareReadersOrWriters|EFileWrite );
        if ( KErrNone == error )
            {
            toIconFile = &appArcIconFile;
            }
        }

    apparcSession.RegisterNonNativeApplicationL(
        KUidWidgetLauncher,
        aDriveName,
        *writer,
        lWriter,
        toIconFile );

    // TODO move this where it is only done once
    TRAP_IGNORE( apparcSession.RegisterNonNativeApplicationTypeL(
                     KUidWidgetLauncher,
                     KLauncherApp() ) );
    // TODO end

    apparcSession.CommitNonNativeApplicationsUpdatesL();

    // appArcIconFile, writer, lWriter, apparcSession
    CleanupStack::PopAndDestroy( 4 );
    }

//  End of File
