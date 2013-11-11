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
* Description:  Header file for class CUsbActiveMscHandler
*
*/


#ifndef C_CUSBACTIVEMSCHANDLER_H
#define C_CUSBACTIVEMSCHANDLER_H

#include <usbmsshared.h>        // for P&S used in following MMC states
#include <f32file.h> // for file system dismounting and mounting
#include <usb.h>
#include <e32property.h>
#include <usbuinotif.h>
#include <cusbpersonalitynotifier.h>
#include <cusbpersonalityplugin.h>

// FORWARD DECLARATIONS
class CUsbActivePowerManager;
class CUsbMscPersonalityTimer;
class CRepository;
class CDevEncSessionBase;

/**
* Define system state categories used internally by Msc personality plugin
*/
enum TUsbGlobalSystemState
    {
    EUsbGSStateUnknown = -1,
    EUsbGSStateCategoryNormal, //rfOn, rfOff, BTSAP
    EUsbGSStateCharging
    };

/**
* The file system mounted to the drive. 
*/
enum TUsbFileSystem
    {
    EFsyNone = 0, //no file system mounted
    EFsyMassStorage,
    EFsyFat
    };

/**
*  Class to handle Mass Storage personality (= MSDC).
*
*  @lib euser.lib c32.lib efsrv.lib
*  @since Series 60 3.0
*/
class CUsbActiveMscHandler : public CUsbPersonalityPlugin
    {
public:  // Constructors and destructor
        
    /**
     * Two-phased constructor.
     *
     * @param aPersonalityParams Reference to container class
     * @return Pointer to created object.
     */
     static CUsbActiveMscHandler* NewL(TUsbPersonalityParams& aPersonalityParams);
        
     /**
      * Destructor.
      */
    virtual ~CUsbActiveMscHandler();

public: // From base classes CActive

    /**
     * From CActive.
     * Implements state machine for this class.
     *
     * @since Series 60 3.0
     */
    void RunL();

    /**
     * From CActive.
     * Never called in this implementation.
     *
     * @since Series 60 3.0
     * @param aError the error returned
     * @return error
     */
    TInt RunError( TInt /*aError*/ );

    /**
     * From CActive.
     * Cancel outstanding request.
     *
     * @since Series 60 3.0
     */
    void DoCancel();

public: // From base class CUsbPersonality

    /**
     * From CUsbPersonality
     * Confirm unload in case of personality change 
     * if msc state is active.
     *
     * @since S60 3.0
     * @param aStatus Status of the ended operation.
     */
    virtual void ConfirmPersonalityUnload(TRequestStatus& aStatus);

    /**
     * From CUsbPersonality
     * Called by personality handler when personality start needs to be
     * prepared.
     *
     * @since Series 60 3.0
     * @param aStatus Status of the ended operation.
     */
    virtual void PreparePersonalityStart(TRequestStatus& aStatus);

    /**
     * From CUsbPersonality
     * Called by personality handler when personality start needs to be
     * finished. Calls DoFinishPersonalityStartL() and provides error
     * handling for it.
     *
     * @since Series 60 3.0
     * @param aStatus Status of the ended operation.
     */
    virtual void FinishPersonalityStart(TRequestStatus& aStatus);

    /**
     * From CUsbPersonality
     * Called by personality handler when personality stop needs to be 
     * prepared. For this personality: Remounts FAT File System and 
     * dismounts Mass Storage File System, Switch back from offline mode.
     *
     * @since Series 60 3.0
     * @param aStatus Status of the ended operation.
     */
    virtual void PreparePersonalityStop(TRequestStatus& aStatus);

    /**
     * From CUsbPersonality
     * Called by personality handler when personality stop needs to be 
     * finished. Calls DoFinishPersonalityStopL() and provides error
     * handling for it.
     *
     * @since Series 60 3.0
     * @param aStatus Status of the ended operation.
     */
     virtual void FinishPersonalityStop(TRequestStatus& aStatus);
        
    /**
     * From CUsbPersonality
     * State change notify
     *
     * @since Series 60 3.0
     * @param aState state of the device
     */
    virtual void StateChangeNotify( TUsbDeviceState aState );
        
public:

    /**
     * If dismount timer expires, this callback is called.
     *
     * @since Series 60 3.2
     * @param aPtr Pointer to this class
     * @return KErrNone is always returned
     */
    static TInt DismountFatCallBack(TAny* aPtr);
                            
private:

    /**
     * Does the forced dismount.
     *
     * @since Series 60 3.2
     */
    void ForciblyDismountFat();

    /**
     * Complete owner class request if any.
     *
     * @since Series 60 3.2
     */
    void CompleteRequest(TInt aError);
                
    /**
     * Start dismounting FAT from the drives
     * Also mounts mass storage file system, when all FATs dismounted.
     *
     * @since Series 60 5.0
     */
    void StartDismountFat();

    /**
     * Add mass storage file system.
     *
     * @return KErrNone if successful or already added
     * @since Series 60 3.2
     */
    TInt AddMassStorageFileSystem();
                        
    /**
     * Remove mass storage file system.
     *
     * @since Series 60 5.0
     */
    void RemoveMassStorageFileSystem();
                        
    /**
     * Mount mass storage to all the drives
     * The mass storage is mounted to all the drives in normal global
     * system state. If device is not locked, the drives are mounted also in 
     * charging global system state. 
     */
    void MountMassStorage();
                        
    /**
     * Returns drives with available MMC card.
     * @param aDrivesWithMmcInserted Array to which result should be written
     */
    TInt GetDrives();

    /**
     * Dismounts FAT File System from the drive.
     * @param aDrive Drive from which dismounting should be done
     */
    void DismountFat( TInt aDrive );

    /**
     * Mounts Mass Storage File System into the drive.
     * @param aDrive Drive to which mounting should be done
     * @return error
     */
    TInt TryMountMassStorage( TInt aDrive );

    /**
     * Dismounts Mass Storage File System from the drive.
     * @param aDrive The drive for the dismount
     * @return error
     */
    TInt TryDismountMassStorage( TInt aDrive );

    /**
     * Mounts FAT File System to the drive.
     * @param aDrive The drive for the mount
     * @return error
     */
    TInt TryMountFat( TInt aDrive );

    /**
     * C++ constructor
     */
    CUsbActiveMscHandler(TUsbPersonalityParams& aPersonalityParams);

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();
        
    /**
     * Get global system state
     * @return state
     */     
    TUsbGlobalSystemState GlobalSystemState();
    
    /**
     * Check if device is (scheduled to be) locked or not
     * @return ETrue/EFalse
     */     
    TBool DeviceLocked();
    
    /**
     * Get Lock Time setting
     * @return time
     */     
    TInt AutoLockTime();
    
    /**
     * Get lock status setting
     * @return status
     */     
    TInt AutoLockStatus();

    /**
     * Unmount Mass storage
     * This is the reverse of StartMountMassStorage.
     *
     * @since Series 60 5.0
     */
    void UnmountMassStorage();

    /**
     * Get the file system mounted to the drive
     *
     * @param aDrive The drive which file system is required.
     * @return The file system. 
     * @since Series 60 5.0
     */
    TUsbFileSystem GetDriveFileSystem( TInt aDrive );

    /**
     * Tells whether device encryption is supported on device or not.
     *
     * @param None
     * @return TBool 
     * @since TimeBox 9.2
     */
    TBool IsDeviceEncryptionSupportedL();
    
	/**
     * Tells whether device encryption is supported on device or not.
     *
     * @param  aDriveLetter pass in the drive that need to be checked
     * @return TBool
     * @since TimeBox 9.2
     */
	TBool IsEncProtectionRequired(const TInt& aDriveLetter);
	
	/**
     * Dynamic load devenccommonutils.dll.
     *
     * @param  None
     * @since TimeBox9.2
     */
	void LoadDevEncSessionL();
	
	/**
     * Unload devenccommonutils.dll.
     *
     * @param  None
     * @since TimeBox9.2
     */
	void UnloadDevEncSession();

	 
private: // Data
    /**
     * define the states of Mass Storage personality
     */
    enum TUsbMscState
        {
        EUsbMscStateIdle,
        EUsbMscStateStarting,
        EUsbMscStateMounting,
        EUsbMscStateFileTransfer,
        EUsbMscStateStopping,
        EUsbMscStateForciblyDismounting
        };

    /**
     * state of Mass Storage Class
     */
    TUsbMscState iMscState;

    /**
     * File server session handle
     */
    RFs iFs;
    
    /** 
     * Indicates whether MSFS has already been added to file server
     */
    TBool iMsfsAdded;

    /**
     * Request to complete later
     */
    TRequestStatus* iRequestStatus;

    /**
     * removable drives in the system
     */
    RArray<TInt> iDrives;
    
    /**
     * Needed for multiple drive support
     */
    TInt iDriveIndex;
    
    /**
     * Dismount timer: If expired forced dismount used.
     */
    CUsbMscPersonalityTimer* iDismountFatTimer;
    
    /**
     * Packages for queries
     */
    TUSBQueriesNotifierParamsPckg iQueryParams;
    
    /**
     * Dummy stores for queries and notes
     */
    TBuf8<1> iDummy;
    
    /**
     * Used to get device lock settings
     */
    CRepository* iRepository;
    
    /**
     * FAT has been unmounted or MSC mounted.
     */
    TBool iMountChanged;

    /**
     * If query note shown by us
     */
    TBool iIsQueryNoteShown;
    
    /**
     * Used to load devenccommonutils.dll and check the device encryption status
     */
    RLibrary iLibrary;
    CDevEncSessionBase* iDevEncSession;
    };

#endif   // USBACTIVEMSCHANDLER_H

// End of File
