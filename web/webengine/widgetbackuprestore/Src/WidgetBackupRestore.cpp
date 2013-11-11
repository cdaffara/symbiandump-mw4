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
* Description:  Widget's active data owner in backup/restore.
*
*/


#include <abclient.h>
#include <e32property.h>
#include "WidgetBackupRestore.h"
#include "WidgetActiveCallback.h"


//  CONSTANTS


// ======== MEMBER FUNCTIONS ========
// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CWidgetBackupRestore* CWidgetBackupRestore::NewL()
    {
    CWidgetBackupRestore* self = 
        new ( ELeave ) CWidgetBackupRestore();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CWidgetBackupRestore::~CWidgetBackupRestore()
    {
    Cancel();
    
    iBackupProperty.Close();
    delete iCallBack;
    delete iActiveBackupClient;
    }

// ---------------------------------------------------------------------------
// Active object's request handling.
// ---------------------------------------------------------------------------
//
void CWidgetBackupRestore::RunL()
    {
    if ( iStatus.Int() == KErrNone )
        {
        TInt currentValue = KErrNone;
        iBackupProperty.Get( currentValue );

        HandleBackupStateL( currentValue );
        }
    else
        {
        //RunError( iStatus.Int() );
        }

    // Re-subscribe notifications.
    SubscribePSKey();
    }

// ---------------------------------------------------------------------------
// Active object's request error handling.
// ---------------------------------------------------------------------------
//
TInt CWidgetBackupRestore::RunError( TInt /*aError*/ )
    {
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Cancel the request.
// ---------------------------------------------------------------------------
//
void CWidgetBackupRestore::DoCancel()
    {
    iBackupProperty.Cancel();
    }

// ---------------------------------------------------------------------------
// C++ constructor.
// ---------------------------------------------------------------------------
//
CWidgetBackupRestore::CWidgetBackupRestore() : 
    CActive( EPriorityNormal )
    {
    // Add to active scheduler.
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// 2nd phase constructor.
// ---------------------------------------------------------------------------
//
void CWidgetBackupRestore::ConstructL()
    {
#ifdef _DEBUG
    _LIT(KDir, "WidgetBUR");
    _LIT(KFile, "WidgetBUR.log");
    TInt err( 0 );

    err = iFileLogger.Connect();
    if ( err == KErrNone )
        {
        iFileLogger.CreateLog( KDir(), KFile(), EFileLoggingModeOverwrite );
        iCanLog = ETrue;
        }
#endif
    iLastType = conn::EBURUnset;

    // Attach to backup key.
    iBackupProperty.Attach( KUidSystemCategory, conn::KUidBackupRestoreKey );

    // Handle initial B&R key value, then start listening for further changes.
    RunL();
    }

// ---------------------------------------------------------------------------
// Handles changes in backup state.
// ---------------------------------------------------------------------------
//
void CWidgetBackupRestore::HandleBackupStateL( const TInt aValue )
    {
    const TInt type = aValue & conn::KBURPartTypeMask;

    // Test if the device is going into backup or restore mode, and we are
    // required to participate.
    if ( ( type == conn::EBURBackupFull || type == conn::EBURRestoreFull ) ||
         ( type == conn::EBURBackupPartial || type == conn::EBURRestorePartial ) )
        {
        if ( !iCallBack )
            {
            iCallBack = CWidgetActiveCallback::NewL();
            }

        if ( !iActiveBackupClient )
            {
            iActiveBackupClient = conn::CActiveBackupClient::NewL( iCallBack );

            if ( ( type == conn::EBURBackupPartial || 
                   type == conn::EBURRestorePartial ) &&
                 !iActiveBackupClient->DoesPartialBURAffectMeL() )
                {
                delete iCallBack;
                iCallBack = NULL;
                delete iActiveBackupClient;
                iActiveBackupClient = NULL;
                return;
                }
            }

        iActiveBackupClient->ConfirmReadyForBURL( KErrNone );
        }
    else
        {
        if ( type == conn::EBURNormal )
            {
            if ( iLastType == conn::EBURBackupFull || iLastType == conn::EBURBackupPartial )
                {
                iCallBack->CleanupBackupDataL();
                }

            // delete once back to normal.
            delete iCallBack;
            iCallBack = NULL;
            delete iActiveBackupClient;
            iActiveBackupClient = NULL;
                
            // Stop the scheduler, which will allow
            // the process to exit.
            CActiveScheduler::Stop();
            }
        }

    iLastType = type;
    }

// ---------------------------------------------------------------------------
// Subsribes notifications of backup/restore p&s key.
// ---------------------------------------------------------------------------
//
void CWidgetBackupRestore::SubscribePSKey()
    {
    Cancel();

    iBackupProperty.Subscribe( iStatus );
    SetActive();
    }

void MainL()
    {
    CActiveScheduler* scheduler = new (ELeave) CActiveScheduler();
    CActiveScheduler::Install( scheduler );
    CleanupStack::PushL( scheduler );
    //
    CWidgetBackupRestore* widBUR = CWidgetBackupRestore::NewL();
    CleanupStack::PushL( widBUR );
    CActiveScheduler::Start();
    //
    CleanupStack::PopAndDestroy( 2, scheduler );
    }

TInt E32Main()
    {
    TInt err = KErrNoMemory;
    //
    CTrapCleanup* cleanupStack = CTrapCleanup::New();
    if ( cleanupStack )
        {
        TRAP( err, MainL() );
        }
    //
    delete cleanupStack;
    return err;
    }
