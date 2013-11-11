/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Main class for UsbMscPersonality
*
*/

#include <centralrepository.h>
#include <AknSkinsInternalCRKeys.h>
#include <usbman.h>
#include <e32property.h>
#include <tusbpersonalityparams.h>
#include <startupdomainpskeys.h>
#include <settingsinternalcrkeys.h>
#include "CUsbActiveMscHandlerMdrv.h"
#include "CUsbMscPersonalityTimer.h"
#include "debug.h"

#include <featmgr.h> // for checking DE feature
#include <DevEncSessionBase.h>
#include <DevEncEngineConstants.h>

// LITERALS
_LIT( KMsFs,"MSFS.FSY" );
_LIT( KMsFsyName, "MassStorageFileSystem" );
_LIT( KFatFsyName, "Fat" );

const TUint32 KDismountFatTimeoutValue = 5000000; // 5 seconds
const TInt KMscDismountRetryCount = 3;
const TUint32 KWaitMscToComplete = 50000; // 50 ms

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CUsbActiveMscHandler::CUsbActiveMscHandler(TUsbPersonalityParams& aPersonalityParams)
    : CUsbPersonalityPlugin(aPersonalityParams), 
      iMountChanged(EFalse),
      iIsQueryNoteShown(EFalse)
    {
    CActiveScheduler::Add( this );
    }

// ----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CUsbActiveMscHandler::ConstructL()
    {
    FLOG( _L( "[USBWATCHER]\tCUsbActiveMscHandler: ConstructL" ) );
                    
    iDismountFatTimer = new (ELeave) CUsbMscPersonalityTimer(
        TCallBack(DismountFatCallBack, this), KDismountFatTimeoutValue);
    
    iMscState = EUsbMscStateIdle;
    User::LeaveIfError(iFs.Connect());
    iRepository = CRepository::NewL(KCRUidSecuritySettings); 
    LoadDevEncSessionL();
    }

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CUsbActiveMscHandler* CUsbActiveMscHandler::NewL(TUsbPersonalityParams& aPersonalityParams)
    {
    CUsbActiveMscHandler* self 
        = new ( ELeave ) CUsbActiveMscHandler(aPersonalityParams);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(); // pop self
    return self;
    }
    
// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
CUsbActiveMscHandler::~CUsbActiveMscHandler()
    {
    FLOG( _L( "[USBWATCHER]\tCUsbActiveMscHandler::~CUsbActiveMscHandler" ) );
    
    if ( iMountChanged ) 
        {        
        UnmountMassStorage();
        }

    RemoveMassStorageFileSystem();
    
    Cancel();
    delete iDismountFatTimer;

    iDrives.Close();
    iFs.Close();

    delete iRepository;
    UnloadDevEncSession();    
    }

// ----------------------------------------------------------------------------
// State machine for the class.
// ----------------------------------------------------------------------------
//
void CUsbActiveMscHandler::RunL()
    {
    FTRACE( FPrint(
        _L( "[USBWATCHER]\tCUsbActiveMscHandler::RunL error = %d" ),
        iStatus.Int() ) );
    FTRACE( FPrint(
        _L( "[USBWATCHER]\tCUsbActiveMscHandler::RunL iMscState = %d" ),
        iMscState ) );
    switch (iMscState)
        {
        case EUsbMscStateMounting:
            // DismountFatTimer is not exprired but RunL called with error
            if (KErrNone!=iStatus.Int())
              {
              /* Print error code, drive name and wait for timer to be expired expired (5sec) 
               * which will call forcibly dismount*/
              if(KErrNone!=iStatus.Int() )
               {
               FLOG( _L( "[USBWATCHER]\tCUsbActiveMscHandler::RunL*** Drive Dismounting failed ") );
               FTRACE( FPrint(
                   _L( "[USBWATCHER]\tCUsbActiveMscHandler::RunL*** Dismount failed on DriveIndex= %d" ),
                   iDriveIndex));
               FTRACE( FPrint(
                   _L( "[USBWATCHER]\tCUsbActiveMscHandler::RunL*** Dismount failed on Drive = %d" ),
                   iDrives[iDriveIndex]) );
               }
              }
            else
              {
              //dismount FAT done for one drive
              if (iDismountFatTimer)
               {
               iDismountFatTimer->Cancel();
               } 
              StartDismountFat();
              }
            break;
        case EUsbMscStateForciblyDismounting:
            // If Even ForciblyDismount failed with error we cannot do much here, and just print Error message 
            if(KErrNone!=iStatus.Int() )
              {
              FLOG( _L( "[USBWATCHER]\tCUsbActiveMscHandler::RunL*** FS has seroius dismounting problem" ) );
              }
            // we change the state and continue with other drives 
            iMscState = EUsbMscStateMounting;                 
            StartDismountFat();
            break; 
        default:
            break;
        }
    }

// ----------------------------------------------------------------------------
// Not possible to come here.
// ----------------------------------------------------------------------------
//
TInt CUsbActiveMscHandler::RunError( TInt /*aError*/ )
    {
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// This method always confirms the personality unloading.
// ----------------------------------------------------------------------------
//
void CUsbActiveMscHandler::ConfirmPersonalityUnload(TRequestStatus& aStatus)
    {
    FTRACE( FPrint(
        _L( "[USBWATCHER]\tCUsbActiveMscHandler: ConfirmPersonalityUnload iMscState = %d" ),
        iMscState ) );
    iRequestStatus = &aStatus;
    aStatus = KRequestPending;
    CompleteRequest(KErrNone);
    }

// ----------------------------------------------------------------------------
// Called by personality handler when personality start needs to be
// prepared. Adds mass storage file system. 
// ----------------------------------------------------------------------------
//
void CUsbActiveMscHandler::PreparePersonalityStart(TRequestStatus& aStatus)
    {
    TInt ret = KErrNone;
    FLOG( _L( "[USBWATCHER]\tCUsbActiveMscHandler: PreparePersonalityStart" ) );
    iRequestStatus = &aStatus;
    aStatus = KRequestPending;
    iMscState = EUsbMscStateStarting;
    ret = AddMassStorageFileSystem();
    CompleteRequest(ret);
    }

// ----------------------------------------------------------------------------
// Called by personality handler when personality start needs to be finished.
// ----------------------------------------------------------------------------
//
void CUsbActiveMscHandler::FinishPersonalityStart(TRequestStatus& aStatus)
    {
    FLOG( _L( "[USBWATCHER]\tCUsbActiveMscHandler: FinishPersonalityStart" ) );
    iRequestStatus = &aStatus;
    aStatus = KRequestPending;
    CompleteRequest(KErrNone);
    }

// ----------------------------------------------------------------------------
// Called by personality handler when personality stop needs to be prepared. 
// Changes state of the personality.
// ----------------------------------------------------------------------------
//
void CUsbActiveMscHandler::PreparePersonalityStop(TRequestStatus& aStatus)
    {
    FLOG( _L( "[USBWATCHER]\tCUsbActiveMscHandler: PreparePersonalityStop" ) );

    //Mounting may be ongoing
    iRequestStatus = NULL; //do not complete in DoCancel
    Cancel(); 
    
    iRequestStatus = &aStatus;
    aStatus = KRequestPending;

    iMscState = EUsbMscStateStopping;

    CompleteRequest(KErrNone);
    }
    
// ----------------------------------------------------------------------------
// Called by personality handler when personality stop needs to be finished. 
// ----------------------------------------------------------------------------
//
void CUsbActiveMscHandler::FinishPersonalityStop(TRequestStatus& aStatus)
    {
    FLOG(_L("[USBWATCHER]\tCUsbActiveMscHandler: FinishPersonalityStop"));

    //Mounting may be ongoing
    iRequestStatus = NULL; //do not complete in DoCancel
    Cancel(); 
    
    //unmount in case device state not yet Undefined
    if (iMountChanged)
        {
        UnmountMassStorage();
        }
        
    RemoveMassStorageFileSystem();

    if (iIsQueryNoteShown)
        {
        // Remove all queries shown by this personality
        iPersonalityParams.PersonalityNotifier().CancelQuery(KQueriesNotifier);
        iIsQueryNoteShown = EFalse;
        }
        
    iMscState = EUsbMscStateIdle;

    iRequestStatus = &aStatus;
    aStatus = KRequestPending;
    CompleteRequest(KErrNone);
    }

// ----------------------------------------------------------------------------
// Indicates USB device state change to personality. 
// There is no need to Cancel, because Confirm unload can not be ongoing 
// before Address state. 
// ----------------------------------------------------------------------------
// 
void CUsbActiveMscHandler::StateChangeNotify( TUsbDeviceState aState )
    {
    FTRACE( FPrint(
        _L( "[USBWATCHER]\tCUsbActiveMscHandler::StateChangeNotify aState = %d" ),
        aState ) );
    switch( aState )
        {        
        //Note that Address state may be caused also by cable disconnection.
        case EUsbDeviceStateAddress: 
            {
            //Do not start mounting if already ongoing 
            //e.g. fast state changes Address-->Suspended-->Address
            if ( !iMountChanged && (GetDrives() == KErrNone) )
                {
                if (iDrives.Count())
                    {
                    iDriveIndex = iDrives.Count();
                    StartDismountFat();
                    }
                else
                    {
                    if ( GlobalSystemState() == EUsbGSStateCategoryNormal )
                        {
                        iIsQueryNoteShown = ETrue;
                        // if the error is something abnormal, note still needs to be shown
                        iQueryParams().iQuery = EUSBStorageMediaFailure;
                        iPersonalityParams.PersonalityNotifier().ShowQuery(
                            KQueriesNotifier, iQueryParams, iDummy);
                        }            
                    }    
                }
            
            }
            break;
        case EUsbDeviceStateUndefined: 
            {
            if (iMountChanged)
                {
                UnmountMassStorage();
                iMscState = EUsbMscStateIdle;
                }
            }
            break;
        default:
            break;
        }
    FLOG(_L("[USBWATCHER]\tCUsbActiveMscHandler::StateChangeNotify completed"));
    }

// ----------------------------------------------------------------------------
// Start FAT dismounting sequence.
// ----------------------------------------------------------------------------
// 
void CUsbActiveMscHandler::StartDismountFat()
    {
    FLOG(_L("[USBWATCHER]\tCUsbActiveMscHandler::StartDismountFat"));
    iMountChanged = ETrue;

    if (!iDriveIndex)
        {
        //FAT dismounted from all the drives
        MountMassStorage();            
        //MSFS mounted to all the drives
        iMscState = EUsbMscStateFileTransfer;
        }
    else
        {
        --iDriveIndex;
        FTRACE( FPrint(
            _L( "[USBWATCHER]\tCUsbActiveMscHandler::StartDismountFat iDriveIndex = %d " ),
            iDriveIndex) );
        // see if FAT file system exists in drive and if it does, try to dismount it
        DismountFat(iDrives[iDriveIndex]);
        }
    }

// ----------------------------------------------------------------------------
// Add mass storage file system
// ----------------------------------------------------------------------------
// 
TInt CUsbActiveMscHandler::AddMassStorageFileSystem()
    {
    FLOG(_L("[USBWATCHER]\tCUsbActiveMscHandler::AddMassStorageFileSystem"));

    TInt ret = KErrNone;
    
    // To be done only once during the lifetime of this object:
    // 5b. add Mass Storage File System to the file server.
    // (done like this to avoid Symbian crash)
    if (!iMsfsAdded)
        {
        FLOG( _L( "[USBWATCHER]\tCUsbActiveMscHandler::AddMassStorageFileSystem: Loading MSFS" ) );
        ret = iFs.AddFileSystem(KMsFs);
        if ((ret != KErrNone) && (ret != KErrAlreadyExists))
            {
            FTRACE( FPrint(
                _L( "[USBWATCHER]\tCUsbActiveMscHandler::AddMassStorageFileSystem: ERROR: MSFS loading failed. Code: %d " ),
                ret) );
            }
        else
            {
            iMsfsAdded = ETrue;
            if (ret == KErrAlreadyExists)
                {
                ret = KErrNone;
                }
            }
        }
    FTRACE(FPrint(_L("[USBWATCHER]\tCUsbActiveMscHandler::AddMassStorageFileSystem: ret=%d"), ret));
    return ret;
    }

// ----------------------------------------------------------------------------
// Remove mass storage file system (MSFS) from file server
// ----------------------------------------------------------------------------
// 
void CUsbActiveMscHandler::RemoveMassStorageFileSystem()
    {
    FLOG(_L("[USBWATCHER]\tCUsbActiveMscHandler::RemoveMassStorageFileSystem"));
    if (iMsfsAdded)
        {
        if (iFs.RemoveFileSystem(KMsFsyName) != KErrNone)
            {
            FLOG( _L( "[USBWATCHER]\tCUsbActiveMscHandler: RemoveMassStorageFileSystem: MSFS not removed from file server." ) );
            }
        else
            {
            iMsfsAdded = EFalse;
            }
        }    
    }

// ----------------------------------------------------------------------------
// Mount mass storage to all drives. Does not mount in charging state if device 
// locked.
// ----------------------------------------------------------------------------
// 
void CUsbActiveMscHandler::MountMassStorage()
    {
    FLOG(_L("[USBWATCHER]\tCUsbActiveMscHandler::MountMassStorage"));
    TInt ret = KErrNone;
    TBool locked = DeviceLocked();
    TUsbGlobalSystemState state = GlobalSystemState();
    FTRACE(FPrint(_L("[USBWATCHER]\tCUsbActiveMscHandler::MountMassStorage: locked=%d, state=%d"), locked, state));

    if ( (state == EUsbGSStateCategoryNormal) || 
       (!locked && (state==EUsbGSStateCharging)) )
        {
        for ( TInt driveIndex = iDrives.Count() - 1; driveIndex >= 0; driveIndex-- )
            {
            // Try mount Mass Storage File System into drive.
            ret = TryMountMassStorage(iDrives[driveIndex]);
            if (ret != KErrNone)
                {
                FTRACE(FPrint(_L("[USBWATCHER]\tCUsbActiveMscHandler::MountMassStorage: driveIndex=%d ret=%d"), 
                    driveIndex, ret));
                }
            }
        }
    }

// ----------------------------------------------------------------------------
// Return removable drives in system.
// ----------------------------------------------------------------------------
//  
TInt CUsbActiveMscHandler::GetDrives()
    {    
    TInt i;
    TInt ret = KErrNone;
    
    // go through drives A-Z except for C and Z
    TDriveInfo driveInfo;
    iDrives.Reset();
    
    for (i = EDriveA; i < EDriveZ; i++)
        {
        // skip drive C: and get drive info
        if ( EDriveC == i ) 
            {
            continue;
            }
        
    // unmounting FAT from the card when a decrypting/encrypting operation
    // is ongoing will corrupt the card, so it must be prevented.      
	if( ( i == EDriveE ) || ( i == EDriveF ) )
		{
        if(IsEncProtectionRequired(i))
			{
			FTRACE( FPrint( _L(" Skipping drive %d"), i));
			continue;
			}
		}
        iFs.Drive(driveInfo, i);
        
        // if drive is not removable and local, it can be skipped
        if ((driveInfo.iDriveAtt & (KDriveAttRemovable | KDriveAttLocal))
            != (KDriveAttRemovable | KDriveAttLocal))
            {
            continue;
            }    
        
        FTRACE( FPrint(
            _L( "[USBWATCHER]\tCUsbActiveMscHandler::GetDrives, removable drive %d: MediaAtt: %d" ),
            i,driveInfo.iMediaAtt ) );
        FTRACE( FPrint(
            _L( "[USBWATCHER]\tCUsbActiveMscHandler::GetDrives, removable drive %d: Media info: %d" ),
            i, driveInfo.iType ) );

        // The memory card may be locked. No memory card password query is shown. 
        
        FTRACE(FPrint(
               _L("[USBWATCHER]\tCUsbActiveMscHandler::GetDrives: MMC inserted into drive %d"),
               i) );
        ret = iDrives.Append(i);
        }
        
    if (!iDrives.Count())
        {
        FLOG( _L( "[USBWATCHER]\tCUsbActiveMscHandler::GetDrives: No removable drives found." ) );

        iIsQueryNoteShown = ETrue;
        iQueryParams().iQuery = EUSBStorageMediaFailure;
        iPersonalityParams.PersonalityNotifier().ShowQuery(KQueriesNotifier, iQueryParams, iDummy);

        return KErrNotFound;
        }
        
    return ret;
    }

// ----------------------------------------------------------------------------
// Dismounts FAT File System.
// ----------------------------------------------------------------------------
//   
void CUsbActiveMscHandler::DismountFat( TInt aDrive )
    {
    FLOG( _L( "[USBWATCHER]\tCUsbActiveMscHandler::DismountFat" ) );
    
    //nothing to do if FAT file system not in aDrive
    if ( GetDriveFileSystem(aDrive) != EFsyFat )
        {
        FTRACE( FPrint(
            _L( "[USBWATCHER]\tCUsbActiveMscHandler::DismountFat: FAT FSY not found in drive %d" ),
            aDrive ) );
        //continue to the next drive
        TRequestStatus* status = &iStatus;
        User::RequestComplete(status, KErrNotFound);
        SetActive();
        }
    else
        {
        //FAT file system exists in aDrive -> dismount it
        iFs.NotifyDismount(iDrives[iDriveIndex], iStatus, EFsDismountNotifyClients);
        SetActive();

        //Give some time for applications before dismounting forcefully
        iDismountFatTimer->Start();
        }

    iMscState = EUsbMscStateMounting;
    }

// ----------------------------------------------------------------------------
// Mounts Mass Storage File System into drive.
// ----------------------------------------------------------------------------
//  
TInt CUsbActiveMscHandler::TryMountMassStorage( TInt aDrive )
    {
    FLOG( _L( "[USBWATCHER]\tCUsbActiveMscHandler::TryMountMassStorage" ) );
    
    TInt ret(KErrNone);
    
    ret = iFs.MountFileSystem( KMsFsyName, aDrive );
    FTRACE( FPrint(
        _L( "[USBWATCHER]\tCUsbActiveMscHandler::TryMountMassStorage: fs mount ret %d" ),
        ret ) );
    
    if ( ret == KErrNotSupported )
        {
        // there is an error in environment and MSFS has been mounted into
        // drive but mounting has been unsuccessful -> remove it
        FTRACE( FPrint(
            _L( "[USBWATCHER]\tCUsbActiveMscHandler::TryMountMassStorage: MS FSY not supported in drive %d" ),
            aDrive ) );
        return ret;
        }
    else if ((ret != KErrNone) && (ret != KErrNotReady))
        {
        FTRACE( FPrint(
            _L( "[USBWATCHER]\tCUsbActiveMscHandler::TryMountMassStorage: ERROR %d in MS FSY mounting in drive %d" ),
            ret, aDrive ) );
        return ret;
        }

    FTRACE( FPrint(
        _L( "[USBWATCHER]\tCUsbActiveMscHandler::TryMountMassStorage: MS FSY mounted in drive %d" ),
        aDrive ) );

    return KErrNone;
    }

// ----------------------------------------------------------------------------
// Dismounts Mass Storage File System from the drive.
// ----------------------------------------------------------------------------
//  
TInt CUsbActiveMscHandler::TryDismountMassStorage( TInt aDrive )
    {
    FTRACE( FPrint(
        _L( "[USBWATCHER]\tCUsbActiveMscHandler::TryDismountMassStorage: drive %d" ),
        aDrive ) );

    // initializations
    TInt ret = KErrNone;
    TInt numTry = KMscDismountRetryCount; // How many times to try to dismount the drive
    
    //only dismount if mass storage mounted
    if ( GetDriveFileSystem(aDrive) == EFsyMassStorage )
        {
        while ( numTry-- )
            {
            ret = iFs.DismountFileSystem( KMsFsyName, aDrive );
            if ( ret != KErrNone )
                {
                if ( ret == KErrInUse )
                    {
                    // It may be that USB mass storage transfer is still in use 
                    // when USB File transfer mode is tried to be distangled (this
                    // method is entered). Wait for a while and try again.
                    FTRACE( FPrint(
                         _L( "[USBWATCHER]\tCUsbActiveMscHandler: TryDismountMassStorage: Waiting MSFS dismounting for drive %d" ),
                         aDrive ) );
                    RTimer timer;
                    TRequestStatus timerStatus;
                    timer.CreateLocal(); // Create for this thread
                    timer.After(timerStatus, KWaitMscToComplete);
                    User::WaitForRequest( timerStatus );
                    if ( timerStatus != KErrNone )
                        {
                        FTRACE( FPrint(
                                _L( "[USBWATCHER]\tCUsbActiveMscHandler::TryDismountMassStorage: ERROR: %d wait timer fails." ), 
                                timerStatus.Int() ) );
                        }
                    timer.Close();
                    }
                else
                    {
                    FTRACE( FPrint(
                        _L( "[USBWATCHER]\tCUsbActiveMscHandler: TryDismountMassStorage: ERROR %d in dismounting MSFS from drive %d" ),
                        ret, aDrive ) );
                    break;
                    }
                }
            else
                {
                FTRACE( FPrint(
                    _L( "[USBWATCHER]\tCUsbActiveMscHandler: TryDismountMassStorage: MSFS dismounted from drive %d" ),
                    aDrive ) );
                break;
                }
            } //while
            
            if ( ret == KErrInUse )
                {
                FLOG(_L("[USBWATCHER]\tCUsbActiveMscHandler::TryDismountMassStorage using force"));
                TRequestStatus dismountStatus;
                iFs.NotifyDismount(aDrive, dismountStatus, EFsDismountForceDismount);
                User::WaitForRequest(dismountStatus);
                ret = dismountStatus.Int();
                }
            
        }
        else 
            {
            FTRACE( FPrint(
                _L( "[USBWATCHER]\tCUsbActiveMscHandler: TryDismountMassStorage: No MSFS on drive %d" ),
                aDrive ) );
            }

    FTRACE( FPrint(
        _L( "[USBWATCHER]\tCUsbActiveMscHandler::TryDismountMassStorage: returning %d" ),
        ret ) );
    return ret;
    }

// ----------------------------------------------------------------------------
// Mounts FAT File System to the drive
// ----------------------------------------------------------------------------
//  
TInt CUsbActiveMscHandler::TryMountFat( TInt aDrive )
    {
    FTRACE( FPrint(
        _L( "[USBWATCHER]\tCUsbActiveMscHandler::TryMountFat: drive %d" ),
        aDrive ) );

    // initializations
    TInt ret = KErrNone;

    // Mount back FAT only if there is no mounted file system
    if ( GetDriveFileSystem(aDrive) == EFsyNone )
        {
        ret = iFs.MountFileSystem( KFatFsyName, aDrive );

        FTRACE( FPrint(
            _L( "[USBWATCHER]\tCUsbActiveMscHandler: TryMountFat: return %d in mounting FAT into drive %d" ),
            ret, aDrive ) );

        if (ret != KErrNone)
            {
            FTRACE( FPrint(
                _L( "[USBWATCHER]\tCUsbActiveMscHandler: TryMountFat: ERROR %d in mounting FAT into drive %d" ),
                ret, aDrive ) );
            }
        else
            {
            FTRACE( FPrint(
                _L( "[USBWATCHER]\tCUsbActiveMscHandler: TryMountFat: FAT mounted into drive %d" ),
                aDrive ) );
            }
        }

    return ret;
    }

// ----------------------------------------------------------------------------
// Standard active object cancellation function.
// ----------------------------------------------------------------------------
//
void CUsbActiveMscHandler::DoCancel()
    {
    FTRACE( FPrint(
        _L( "[USBWATCHER]\tCUsbActiveMscHandler: DoCancel iMscState=%d" ),
        iMscState ) );

    //Remove all notes. The state may have changed after the confirm unload
    //is started.
    if (iMscState != EUsbMscStateForciblyDismounting)
        {
        iPersonalityParams.PersonalityNotifier().CancelAll();
        }

    switch (iMscState)
        {
        case EUsbMscStateStarting:
        case EUsbMscStateStopping:
            break;

        case EUsbMscStateMounting:
        case EUsbMscStateForciblyDismounting:
            if (iDismountFatTimer)
                {
                iDismountFatTimer->Cancel();
                }
            iFs.NotifyDismountCancel();
            break;
                        
        default:
            FLOG(_L("[USBWATCHER]\tCUsbActiveMscHandler::DoCancel: ERROR"));        
            break;
        }

    CompleteRequest(KErrCancel);
    }
    
// ----------------------------------------------------------------------------
// Complete request.
// ----------------------------------------------------------------------------
//
void CUsbActiveMscHandler::CompleteRequest(TInt aError)
    {
    FLOG(_L("[USBWATCHER]\tCUsbActiveMscHandler::CompleteRequest"));

    if (iRequestStatus)
        {
        User::RequestComplete(iRequestStatus, aError);
        iRequestStatus = NULL;
        }
    }
    
// ----------------------------------------------------------------------------
// If client doesn't allow us to do dismount, let's force it.
// ----------------------------------------------------------------------------
//
TInt CUsbActiveMscHandler::DismountFatCallBack(TAny* aPtr)
    {
    FLOG(_L("[USBWATCHER]\tCUsbActiveMscHandler::DismountFatCallBack"));        
    
    CUsbActiveMscHandler* handler = static_cast<CUsbActiveMscHandler *>(aPtr);
    
    handler->ForciblyDismountFat();
    
    return KErrNone;
    }
    
// ----------------------------------------------------------------------------
// Forced dismount is done over here.
// ----------------------------------------------------------------------------
//
void CUsbActiveMscHandler::ForciblyDismountFat()
    {    
    FLOG(_L("[USBWATCHER]\tCUsbActiveMscHandler::ForciblyDismountFat"));
    
    //cancel won't complete, since there is no client request ongoing
    iMscState = EUsbMscStateForciblyDismounting; //do not close mode query
    Cancel();
    FTRACE( FPrint(
        _L( "[USBWATCHER]\tCUsbActiveMscHandler::ForciblyDismountFat DriveIndex= %d" ),
        iDriveIndex));
    FTRACE( FPrint(
        _L( "[USBWATCHER]\tCUsbActiveMscHandler::ForciblyDismountFat on Drive = %d" ),
        iDrives[iDriveIndex]) );
    iFs.NotifyDismount(iDrives[iDriveIndex], iStatus, EFsDismountForceDismount);
    SetActive();
    }


TUsbGlobalSystemState CUsbActiveMscHandler::GlobalSystemState()
    {
    TInt state = EUsbGSStateUnknown;
    TUsbGlobalSystemState usbGlobalSysState = EUsbGSStateUnknown;
    TInt error = RProperty::Get( KPSUidStartup, KPSGlobalSystemState, state );
    FTRACE(FPrint(_L("[USBWATCHER]\tCUsbActiveMscHandler::GlobalSystemState: error %d, state %d"), error, state));
    if ( (state == ESwStateNormalRfOn) || 
         (state == ESwStateNormalRfOff) || 
         (state == ESwStateNormalBTSap) )
        {
        usbGlobalSysState = EUsbGSStateCategoryNormal;
        }
    else if ( state == ESwStateCharging )
        {
        usbGlobalSysState = EUsbGSStateCharging;
        }
        
    return usbGlobalSysState;;
    }


// ----------------------------------------------------------------------------
// Determine whether the device is locked or not
// ----------------------------------------------------------------------------
//
TBool CUsbActiveMscHandler::DeviceLocked()
    {
    TBool locked(EFalse);
    if ( GlobalSystemState() == EUsbGSStateCharging )
        {
        if (AutoLockTime()>0 || AutoLockStatus()>0 )
            {
            locked=ETrue;
            }
        }
    
    return locked;
    }

// ----------------------------------------------------------------------------
// Read Lock time settings
// Meant to be used only in Charging mode
// ----------------------------------------------------------------------------
//
TInt CUsbActiveMscHandler::AutoLockTime()
    {
    TInt lockTime(0);
    
    TInt ret = iRepository->Get(KSettingsAutoLockTime, lockTime);
    FTRACE(FPrint(_L("[USBWATCHER]\tCUsbActiveMscHandler::LockeTime: ret=%d. LockTime=%d"), ret, lockTime));

    return lockTime;
    }

// ----------------------------------------------------------------------------
// Read Lock status settings
// Meant to be used only in Charging mode
// ----------------------------------------------------------------------------
//
TInt CUsbActiveMscHandler::AutoLockStatus()
    {    
    TInt lockStatus(0);
    
    TInt ret = iRepository->Get(KSettingsAutolockStatus, lockStatus);
    FTRACE(FPrint(_L("[USBWATCHER]\tCUsbActiveMscHandler::AutoLockStatus: ret=%d. lockStatus=%d"), ret, lockStatus));
    
    return lockStatus;    
    }
    
// ----------------------------------------------------------------------------
// Unmount mass storage dismounts mass storage and mounts FAT for all drives. 
// ----------------------------------------------------------------------------
// 
void CUsbActiveMscHandler::UnmountMassStorage()
    {
    FLOG(_L("[USBWATCHER]\tCUsbActiveMscHandler::UnmountMassStorage"));        
    //Mounting may be ongoing. Note that the confirm unload query and the mode
    //query are closed. 
    Cancel();

    TInt index = iDrives.Count();
    while ( index > 0 )
        {
        --index;
        TInt drive = iDrives[index];
        // First dismount mounted Mass Storage File System
        TryDismountMassStorage(iDrives[index]);
        // Then mount back previously dismounted FAT File System
        TryMountFat(iDrives[index]);
        }
    iMountChanged = EFalse;
    }

// ----------------------------------------------------------------------------
// Return the mounted file system. 
// ----------------------------------------------------------------------------
// 
TUsbFileSystem CUsbActiveMscHandler::GetDriveFileSystem( TInt aDrive )
    {
    FLOG(_L("[USBWATCHER]\tCUsbActiveMscHandler::GetDriveFileSystem"));
    TUsbFileSystem fileSystem = EFsyNone;
    TBuf<KMaxFullName> name;
    TInt err = iFs.FileSystemName( name, aDrive );
    if ( (err == KErrNone) && (name.Length() > 0) )
        {

        FTRACE( FPrint(_L( "[USBWATCHER]\tCUsbActiveMscHandler: GetDriveFileSystem: aDrive=%d name=%S" ),
                         aDrive, &name ) );
        
        if (name.CompareF( KMsFsyName ) == 0)
            {
            fileSystem = EFsyMassStorage;
            }
        else if (name.CompareF( KFatFsyName ) == 0)
            {
            fileSystem = EFsyFat;
            }        
        }
    FTRACE( FPrint(
        _L( "[USBWATCHER]\tCUsbActiveMscHandler: GetDriveFileSystem: filesystem %d on drive %d" ),
        fileSystem, aDrive ) );
                
    return fileSystem;
    }

// ----------------------------------------------------------------------------
// Return whether device encryption is supported or not.
// ----------------------------------------------------------------------------
// 
TBool CUsbActiveMscHandler::IsDeviceEncryptionSupportedL()
    {
    FLOG(_L("[USBWATCHER]\tCUsbActiveMscHandler::IsDeviceEncryptionSupportedL >>"));
    
    TBool ret(EFalse);
    
    FeatureManager::InitializeLibL();
    ret = FeatureManager::FeatureSupported(KFeatureIdFfDeviceEncryptionFeature);
    FeatureManager::UnInitializeLib();
 
    FTRACE(FPrint(_L("[USBWATCHER]\tCUsbActiveMscHandler::IsDeviceEncryptionSupportedL, ret = %d <<"), ret));
    return ret;
    }
	
// ----------------------------------------------------------------------------
// Return device encryption info for further protection
// ----------------------------------------------------------------------------
// 
TBool CUsbActiveMscHandler::IsEncProtectionRequired(const TInt& aDriveLetter)
    {
	TBool ret = ETrue;
    FLOG(_L("[USBWATCHER]\tCUsbActiveMscHandler::GetEncProtectionInfo >>"));
	
    // Is device encryption feature supported?
    TRAPD( r, ret = ( IsDeviceEncryptionSupportedL() ) );    
    if(r)
		{
		ret = EFalse;
		}
	
    // Get the enctrytion operation code
    if(ret)
        {
        FLOG(_L("Check drives for busy status"));
	
        TInt encDriverStatus;
		
        if(!iDevEncSession)
            {
            FTRACE(FPrint(_L("[USBWATCHER]\tCUsbActiveMscHandler::IsEncProtectionRequired: iDevEncSession is NULL")));
            User::Panic(_L("[USBWATCHER]DevEncCommonUtil Not Found"), KErrNotSupported);
            }    
        else
            {
            iDevEncSession->SetDrive( (TDriveNumber)aDriveLetter );
            TInt errCode = iDevEncSession->Connect();
            if( !errCode )
                {
                errCode = iDevEncSession->DiskStatus( encDriverStatus );
                }
            iDevEncSession->Close();
            FTRACE(FPrint(_L("[USBWATCHER]\tCUsbActiveMscHandler::GetDrives: error %d, state %d"), errCode, encDriverStatus));

            ret = ( ( errCode == KErrNone ) && 
                              ( ( encDriverStatus == EEncrypting ) || ( encDriverStatus == EDecrypting ) ) );
            }
        }

    FTRACE(FPrint(_L("[USBWATCHER]\tCUsbActiveMscHandler::GetEncProtectionInfo, aDriveLetter= %d, ret= <<"), aDriveLetter,ret));
	
	return ret;
	}
void CUsbActiveMscHandler::LoadDevEncSessionL()
    {
    FLOG(_L("[USBWATCHER]\tCUsbActiveMscHandler::LoadDevEncSessionL >> "));
    
    if (!iDevEncSession)
        {
	      TInt err = iLibrary.Load(KDevEncCommonUtils);	 
        if (err != KErrNone)
            {
            FTRACE(FPrint(_L("Error in finding the library... %d"), err));
            return;
            }
        TLibraryFunction entry = iLibrary.Lookup(1);
         
        if (!entry)
            {
            FLOG(_L("Error in loading the library..."));
            User::Leave(KErrBadLibraryEntryPoint);
            }
        iDevEncSession = (CDevEncSessionBase*) entry();
        }
    FLOG(_L("[USBWATCHER]\tCUsbActiveMscHandler::LoadDevEncSessionL << "));
    }

void CUsbActiveMscHandler::UnloadDevEncSession()
    {
    FLOG(_L("[USBWATCHER]\tCUsbActiveMscHandler::UnloadDevEncSession >> "));
    
    if (iDevEncSession)
        {
        delete iDevEncSession;
        iDevEncSession = NULL;
        iLibrary.Close();
        }
    
    FLOG(_L("[USBWATCHER]\tCUsbActiveMscHandler::UnloadDevEncSession << "));
    }
// End of file
