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
* Description:  Header file for class CUsbWatcher
*
*/


#ifndef CUSBWATCHER_H
#define CUSBWATCHER_H

#include <bldvariant.hrh>
#include <e32base.h>
#include <usbman.h>
#include <e32property.h>
#include <PSVariables.h>
#include <centralrepository.h>
#include <usbuinotif.h>
#include <usbwatcher.h>
#include "debug.h"
#include "musbwatchernotify.h"
#include "cusbwatchersession.h"
#include "cusbindicatorhandler.h"


class CUsbActiveState;
class CUsbActivePersonalityHandler;
class CUsbDeviceLock;
class CUsbDevConStarter;
class CUsbGlobalSystemStateObserver;
class CUsbOtgWatcher; //OTG

/**
*  Struct for supported personalities.
*
*  This struct is used to store supported personalities.
*
*  @since Series 60 3.2
*/
struct TUsbSupportedPersonalityInf
    {
    
    /**
     * implementation Uid of the personality
     */
    TUid  iPersonalityUid;
    
    /**
     * personality id
     */
    TInt  iPersonalityId;
    
    /**
     * if set then serial number is used by the personality
     */
    TBool iUseSerialNumber;
    };

/**
*  UsbWatcher core class.
*
*  This class starts, stops and changes personalities depending on the cable
*  state or client changing the personality. There is always only one instance
*  of this class.
*
*  @since Series 60 2.?
*/
NONSHARABLE_CLASS( CUsbWatcher ): public CActive
    {

public:

    static CUsbWatcher* NewL();
        
    virtual ~CUsbWatcher();
   
    /**
     * Used to register observer. Each created session will register observer.
     *
     * @since S60 3.2
     * @param aObserver Pointer to observer class to be added.
     */
    void RegisterObserverL( MUsbWatcherNotify* aObserver );

    /**
     * Used to deregister observer. When session is terminated, 
     * it must deregister the observer.
     *
     * @since S60 3.2
     * @param aObserver Pointer to observer class to be removed.
     */
    void DeRegisterObserver( MUsbWatcherNotify* aObserver );

    /**
     * This method is called when operation is finished.
     *
     * @since S60 3.2
     * @param aStatus Status of the ended operation.
     */
	void Notify( TInt aStatus );

    /**
     * This method returns personalities supported by the system.
     *
     * @since S60 3.2
     * @return Refernce to the supported personalities array.
     */
    inline const RArray<TUsbSupportedPersonalityInf>& Personalities() const;

    /**
     * This method updates central repository key and if USB is connected
     * it will also change the personality to new one.
     * 
     * Note that if KErrDiskFull is returned while the cable is connected,
     * the personality was loaded, but it was not stored to Central Repository.
     *
     * @since S60 3.2
     * @param aPersonalityId Personality id
     * @param aNonBlocking If true, no personality switch  queries are shown
     * @return KErrNone if success, otherwise system-wide error code.
     */
    void SetPersonality( TInt aPersonalityId, TBool aNonBlocking );
    
    /**
     * Cancel outstanding SetPersonality() request. Everything is returned
     * to the state before request was done.
     *
     * @since S60 3.2
     */    
    void CancelSetPersonality();

    /**
     * Set previous personality.
     *
     * @since S60 3.2
     */
    void SetPreviousPersonality();

    /**
     * Cancel outstanding SetPreviousPersonality() request. After this request
     * everything is returned to the state like it was before the request.
     *
     * @since S60 3.2
     */
    void CancelSetPreviousPersonality();

    /**
     * Set previous personality when USB cable is disconnected. Note that
     * if cable is already disconnected when this request is issued, service
     * does nothing.
     *
     * @since S60 3.2
     */
    void SetPreviousPersonalityOnDisconnect();
            
    /**
     * Write personality id to the central repository
     *
     * @since Series 60 3.2
     * @param aPersonalityId Current personality id setting
     * @return KErrNone in case of success
     */
    TInt WritePersonalityId( TInt aPersonalityId );

    /**
     * This method is called by the CUsbActiveState class when there is
     * a change in USB state.
     *
     * @since Series 60 2.?
     * @param aStateOld old device state
     * @param aStateNew new device state
     */
    void StateChangeNotifyL( TUsbDeviceState aStateOld, 
            TUsbDeviceState aStateNew );
    
   
    /**
     * This callback function is called when device is locked.
     * 
     * @since Series 60 2.?
     * @return error
     */
    void Lock();
    
    /**
     * This callback function is called when device is unlocked.
     * 
     * @since Series 60 2.?
     * @return error
     */
    void Unlock();
        
    /**
     * Stop loaded personality
     * 
     * @since Series 60 3.2
     */
    void StopPersonality();
    
    /**
     * Start personality
     * 
     * @since Series 60 3.2    
     */
    void StartPersonality();

     /**
      * Check if self is A-device (A-end connected).
      * @return ETrue if self is in A-device state.
      */
      TBool IsDeviceA();
    
public: // from base class CActive

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
     * @param aError The error returned
     * @return error
     */
    TInt RunError( TInt /*aError*/ );

    /**
     * From CActive
     * Cancels outstanding request.
     *
     * @since Series 60 3.0
     */
    void DoCancel();

private:

    /**
     * C++ default constructor.
     */
    CUsbWatcher();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

    /**
     * Get personality plugins.
     *
     * @since Series 60 3.2
     */
    void GetPersonalityPluginsL();
        
    /**
     * Switch personality according to USB state.
     * @param aConfirmUnload If true, no personality switch blocking queries are shown.
     * @since Series 60 3.2
     */
    void SwitchPersonality( TBool aNonBlocking = EFalse );

    /**
     * Starts personality/USB.
     *
     * @since Series 60 2.?
     */
    void Start();

    /**
     * Stops personality/USB.
     *
     * @since Series 60 2.?
     */    
    void Stop();
        
    /**
     * Stops and starts personality.
     * @param aConfirmUnload If true, no personality switch blocking queries are shown.
     * @since Series 60 3.0
     */    
    void StopStart( TBool aNonBlocking );

	/**
     * Get personality id. Used in charging mode.
     *
     * @since Series 60 3.2
     * @return error
     */
    TInt GetChargingPersonalityId( TInt& aPersonalityId ); 
	
private: // Data
    /** 
     * states for USB starting and stopping 
     */
    enum TUsbState
        {
        EUsbIdle,
        EUsbStarting,
        EUsbStarted,
        EUsbStopping,
        EUsbConfirmStop
        };

    /**
     * Personalities supported by this system
     */
    RArray<TUsbSupportedPersonalityInf> iSupportedPersonalities;

    /**
     * Object to follow device state
     */
    CUsbActiveState* iActiveState;

    /**
     * Handle to Usb Manager
     */
    RUsb iUsbMan;

    /**
     * Object to start or stop current personality
     */
    CUsbActivePersonalityHandler* iPersonalityHandler;

    /**
     * Current personality id
     */
    TInt iPersonalityId;
    
    /**
     * Previous personality id
     */
    TInt iPrevPersonalityId;
    
    /**
     * Earlier previous personality id
     */
    TInt iOldPrevPersonalityId;
    
    /**
     * State for USB starting/stoping
     */
    TUsbState iState;

    /**
     * Flag to detect when stop+start scenario ongoing
     */
    TBool iStopStartScenario;

    /**
     * Registered by CUsbWatcherSession when connection established
     */
	RPointerArray<MUsbWatcherNotify> iObservers;
    
    /**
     * Access to central repository
     */
    CRepository* iPersonalityRepository;

    /**
     * Flag to detect when personality change is ongoing
     */
    TBool iPersonalityChangeOngoing;

    /**
     * Flag to detect when set personality request is ongoing
     */
    TBool iSetPersonalityOngoing;

    /**
     * Flag to detect when set previous personality request is ongoing
     */
    TBool iSetPreviousPersonalityOngoing;

    /**
     * Flag to detect when client has requested personality change on disconnect
     */
    TBool iSetPreviousPersonalityOnDisconnect;
    
    /**
     * Flag to detect when personality is changed from the ask on connection
     */
    TBool iChangePersonalityOngoing;    
    
    /**
     * Device lock feature
     */
    CUsbDeviceLock* iUsbDeviceLock;
    
    /**
     * Usbdevcon process starter
     * Own.
     */
    CUsbDevConStarter* iUsbDevConStarter;

    /**
     * Global system state listener
     * Start/stop personality depending on global state changes.
     * Own.
     */
    CUsbGlobalSystemStateObserver* iGlobalStateObserver;
    
    /**
     * Personality loading ongoing
     */
    TBool iStarted;
    
    /**
     * OTG Watcher
     */
    CUsbOtgWatcher* iOtgWatcher; //OTG
    
    /*
     * USB indicator handler to handle device side indicator
     */
    CUsbIndicatorHandler* iUsbIndicatorHandler;
    
    /*
     * Starting mode is normal?
     */
    TBool iNormalStart;
    };
    
#include "cusbwatcher.inl"

#endif   // CUSBWATCHER_H

// End of File
