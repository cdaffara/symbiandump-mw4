/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Header file for class CUsbActivePersonalityHandler
*
*/


#ifndef CUSBACTIVEPERSONALITYHANDLER_H
#define CUSBACTIVEPERSONALITYHANDLER_H

// INCLUDE FILES
#include <etelmm.h>             // for fetching the IMEI code for serial number
#include <mmtsy_names.h>        // for RTelServer names
#include "cusbwatcher.h"

const TUid KUidPersonalityPlugIns = {0x10274793};

// FORWARD CLASS DECLARATIONS
class CUsbPersonality;
class TUsbPersonalityParams;
class CUsbPersonalityNotifier;

// CLASS DECLARATION

/**
*  Class to handle USB personality issues.
*
*  @lib euser.lib usbman.lib c32.lib centralrepository.lib
*  @since Series 60 3.0
*/
NONSHARABLE_CLASS( CUsbActivePersonalityHandler ) : public CActive
     
    {
public:  // Constructors and destructor
        
    /**
     * Two-phased constructor.
     *
     * @param aUsbMan Reference to RUsb.
     * @param aOwner Reference to CUsbWatcher.
     * @return pointer to created object
     */
    static CUsbActivePersonalityHandler* NewL( RUsb& aUsbMan,
            CUsbWatcher& aOwner );
        
    /**
     * Destructor.
     */
    virtual ~CUsbActivePersonalityHandler();

public: // New functions
        
    /**
     * Confirm personality load. If current personality can be unloaded
     * and new personality can be loaded then this method will complete
     * aStatus with KErrNone.
     *
     * @since Series 60 3.2
     * @param aStatus Used to complete outstanding request.
     */
    void ConfirmPersonalityUnload( TRequestStatus& aStatus );
        
    /**
     * Start personality.
     *
     * @since Series 60 3.2
     * @param aPersonalityId Identification of the personality to be loaded.
     * @param aStatus Used to complete outstanding request.
     */
    void StartPersonality( TInt& aPersonalityId, 
	        TRequestStatus& aStatus );

    /**
     * Stop current personality.
     *
     * @since Series 60 3.2
     * @param aStatus Used to complete outstanding request.
     */
    void StopPersonality( TRequestStatus& aStatus );

    /**
     * Called by the owner class when device state is changed.
     *
     * @since Series 60 3.0
     * @param aState state of the device
     */
    void StateChangeNotify( TUsbDeviceState aStateOld,
            TUsbDeviceState aStateNew );
        
    /**
     * Called by the owner class when device state is changed.
     *
     * @since Series 60 3.2
     */
    void PersonalitySelected();
     
    /**
     * Cancel Cable connected notifier
     *
     * @since Series 60 5.0
     */
    void CancelCableConnectedNotifier();
    
public: // from base class CActive

    /**
     * From CActive.
     * This method is called when outstanding request is completed.
     *
     * @since Series 60 3.0
     */
    void RunL();

    /**
     * From CActive.
     * This method is called when RunL() leaves.
     *
     * @since Series 60 3.0
     * @param aError the error returned
     * @return error
     */
    TInt RunError( TInt /*aError*/ );

    /**
      * From CActive
      * This method is called when Cancel() is called and there is outstanding
      * request pending.
      *
      * @since Series 60 3.0
      */
    void DoCancel();

private:
    
    /** Personality handler states */
    enum TUsbActivePersonalityState 
        {
        EUsbPersonalityIdle = 0,
        EUsbPersonalityPrepareStart,
        EUsbPersonalityStartUsb,
        EUsbPersonalityFinishStart,
        EUsbPersonalityStarted,
        EUsbPersonalityPrepareStop,
        EUsbPersonalityStopUsb,
        EUsbPersonalityFinishStop
        };

    /**
     * Construct USB serial number.
     * @since Series 60 3.0
     */    
    void ConstructUsbSerialNumberL();
    
    /**
     * Publish serial number to P&S for USB Manager.
     * @since Series 60 3.0     
     * @return In case of success KErrNone is returned.
     */    
    TInt PublishSerialNumber();
        
    /**
     * Creates and returns a class handler object for the given personality.
     * @since Series 60 3.0
     * @param aPersonality Personality ID where return value is written.
     * @return Pointer to created object
     */
    CUsbPersonality* NewPersonalityHandlerL( TInt aPersonality );

    /**
     * Constructor is prohibited.
     * @param aUsbMan Reference to usb manager.
     * @param aParent Reference to UsbWatcher.
     */
    CUsbActivePersonalityHandler( RUsb& aUsbMan, CUsbWatcher& aOwner );

    /**
     * C++ default constructor (no implementation provided).
     */
    CUsbActivePersonalityHandler();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();
        
private:    // Data
    /**
     * Handle to Usb Manager
     */
    RUsb iUsbMan;
    
    /**
     * Currently loaded personality
     */
    CUsbPersonality* iCurrentPersonalityHandler;

    /**
     * Dummy descriptor used by ask on connection.
     */
    TPtrC8 iDummy;
    
    /**
     * Dummy descriptor buf
     */
    TBuf8<1> iDummyBuf;
    
    /**
     * Buffer where selected personality is stored by the ask
     * on connection.
     */    
    TPckgBuf<TInt> iPersonalityPckgBuf;

    /**
     * Owner of this class.
     */
    CUsbWatcher& iOwner;

    /**
     * Personality id of currently loaded personality.
     */        
    TInt *iPersonalityId;
    
    /**
     * When value is ETrue then serial number is written to P&S.
     */            
    TBool iSerialNumberWritten;
    
    /**
     * When value is ETrue then personality uses serial number.
     */            
    TBool iUseSerialNumber;
    
    /**
     * Access to mobile phone functionality.
     */                
    RMobilePhone::TMobilePhoneIdentityV1 iPhoneInfo;
    
    /**
     * State of the class.
     */                
    TUsbActivePersonalityState iState;
    
	/**
	 * Cleanup process or normal process in various states
	 */        
    TBool isFailureCleanup;
    
    /**
     * Request to be completed.
     */                
    TRequestStatus *iRequestStatus;
    
    /**
     * Container class for personalities.
     */                
    TUsbPersonalityParams* iPersonalityParams;
    
    /**
     * Handle to notifier class, which is used to show notes/queries.
     */                    
    CUsbPersonalityNotifier* iPersonalityNotifier;
    
    /**
     * The device state.
     */                    
    TUsbDeviceState iDeviceState;
    
    /**
     * Packages for queries
     */
    TUSBQueriesNotifierParamsPckg iQueryParams;
    };

#endif   // CUSBACTIVEPERSONALITYHANDLER_H

// End of File
