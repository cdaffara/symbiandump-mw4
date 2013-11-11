/*
 * Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  OTG watcher state machine owner
 *
 */

#ifndef C_USBOTGWATCHER_H
#define C_USBOTGWATCHER_H

#include <e32base.h>

#include <usbotgdefs.h>

#include "definitions.h"  

#ifndef STIF
#include <usbman.h>
#include "cusbidpinobserver.h"
#include "cusbvbusobserver.h"
#include "cusbotgstateobserver.h"
#include "cusbbusactivityobserver.h"
#include "cusbhosteventnotificationobserver.h"
#include "cusbmessagenotificationobserver.h"
#else
#include "mockusbman.h"
#include "mockcusbidpinobserver.h"
#include "mockcusbvbusobserver.h"
#include "mockcusbotgstateobserver.h"
#include "mockcusbbusactivityobserver.h"
#include "mockcusbhosteventnotificationobserver.h"
#include "mockcusbmessagenotificationobserver.h"
#endif

#include "cusbservicecontrol.h" 

class CUsbState;
class CUsbStateHostHandle;

class CUsbNotifManager;

/**
 * Observers for usb otg watcher state nachine
 * Observers gets feedback by implementing this interface
 */
class MUsbOtgWatcherStateObserver
    {

public:

    /**
     * Observer must implement this interface
     * which is called back when state changes
     * @param aState new state id
     */
    virtual void OtgWatcherStateChangedL(TUsbStateIds aState) = 0;
    };

/**
 *  UsbWatcher main class
 *  Implements states machines and owns them
 *
 */
class CUsbOtgWatcher : public CBase,
        MUsbIdPinObserver,
        MUsbVBusObserver,
        MUsbOtgStateObserver,
        MUsbBusActivityObserver,
        MUsbHostEventNotificationObserver,
        MUsbMessageNotificationObserver,
        MUsbServiceControlObserver
    {

    // Making CUsbState friend secures changing states
    // Methods to change state to be called only by CUsbStates children
    friend class CUsbState;
    friend class CtUsbOtgWatcher;

public:

    /**
     * Two-phased constructor.
     * @param aUsb usbman API
     */
    IMPORT_C
    static CUsbOtgWatcher* NewL(RUsb& aUsb);

    /**
     * Check ID-Pin state
     * @return ETrue if the ID-Pin is present (A-Device)
     */
    IMPORT_C
    TBool IsDeviceA();

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CUsbOtgWatcher();

    /**
     * Sets personality
     * @param aStatus for async. request
     * @param aPersonalityID to be set
     */
    void SetPersonalityL(TRequestStatus& aStatus, TInt aPersonalityID);

    /**
     * Cancels setting personality async.
     */
    void CancelSetPersonalityL();

    /**
     *
     * Sets previous personality
     * @param aStatus Asynchronously
     */
    void SetPreviousPersonalityL(TRequestStatus& aStatus);

    /**
     * Sets previous personality
     */
    void SetPreviousPersonalityL();

    /**
     * Cancels async. set prev personality request
     */
    void CancelSetPreviousPersonalityL();

    /**
     * Sets prev. personality on disconnecting device
     */
    void SetPreviousPreviousPersonalityOnDisconnectL();

    /**
     * Policy for session start defined here
     */
    TBool CanStartSessionL();

    /********************************************************************************/
    // Events from observers come here

    // From IdPin observer
    /**
     * Id Pin OFF
     */
    void IdPinOffL();
    /**
     * Id Pin On
     */
    void IdPinOnL();
    /**
     * Id Pin On
     * @param aError
     */
    void IdPinErrorL(TInt aError);

    // From VBus observer
    /**
     * VBus down
     */
    void VBusDownL();
    /**
     * VBus UP
     */
    void VBusUpL();
    /**
     * VBus observer error
     * @param aError error code
     */
    void VBusObserverErrorL(TInt aError);

    // From OTG state observer
    /**
     * Became to Idle when A
     */
    void AIdleL();
    /**
     * Became a Host when A
     */
    void AHostL();
    /**
     * Became a peripheral when A
     */
    void APeripheralL();
    /**
     * VBus error happen
     */
    void AVBusErrorL();
    /**
     * Became to Idle when B
     */
    void BIdleL();
    /**
     * Became a peripheral when B
     */
    void BPeripheralL();
    /**
     * Became a Host when B
     */
    void BHostL();

    /**
     * Error handler
     * @param aError error code
     */
    void OtgStateErrorL(TInt aError);

    // From bus activity observer
    /**
     * Bus is in idle
     */
    void BusIdleL();
    /**
     * Bus active
     */
    void BusActiveL();
    /**
     * error
     * @param aError error happened
     */
    void BusActivityErrorL(TInt aError);

    // From Host Event notification observer
    /**
     * Device is attached
     * @param aInfo Device event data
     */
    void DeviceAttachedL(TDeviceEventInformation aInfo);
    /**
     * Device is detached
     * @param aInfo Device event data
     */
    void DeviceDetachedL(TDeviceEventInformation aInfo);
    /**
     * Drivers successfully loaded
     * @param aInfo Device event data
     */
    void DriverLoadSuccessL(TDeviceEventInformation aInfo);
    /**
     * Drivers loaded partially
     * @param aInfo Device event data
     */
    void DriverLoadPartialSuccessL(TDeviceEventInformation aInfo);
    /**
     * Drivers loading failed
     * @param aInfo Device event data
     */
    void DriverLoadFailureL(TDeviceEventInformation aInfo);
    /**
     * Error happened during observing
     * @param aError error code
     */
    void HostEventNotificationErrorL(TInt aError);

    // From message notification observer
    /**
     * Message received
     * @param aMassage message id
     */
    void MessageNotificationReceivedL(TInt aMessage);
    /**
     * Connected to hub in wrong level 
     */
    void BadHubPositionL();
    /**
     * VBus error happened
     */
    void VBusErrorL();
    /**
     * SRP request received
     */
    void SrpReceivedL();
    /**
     * Session request received
     */
    void SessionRequestedL();
    /**
     * Error handler
     * @param error code
     */
    void MessageNotificationErrorL(TInt aError);

    // From CUsbServiceControl
    /**
     * called when request for usb services is completed
     * if there were few requests in a row, during operation, 
     * only the last one's completion is notified
     * @param aError error code
     */
    void UsbServiceControlReqCompletedL(TInt aError);

    /******************************************************************************* */

    // getters 

    /**
     * @return IdPin observer 
     */
    CUsbIdPinObserver* IdPinObserver() const;
    /**
     * @return VBus observer 
     */
    CUsbVBusObserver* VBusObserver() const;
    /**
     * @return OTG state observer 
     */
    CUsbOtgStateObserver* OtgStateObserver() const;
    /**
     * @return Bus Activity observer 
     */
    CUsbBusActivityObserver* BusActivityObserver() const;
    /**
     * @return HostEvent notification observer 
     */
    CUsbHostEventNotificationObserver* HostEventNotificationObserver() const;
    /**
     * @return Message notification observer 
     */
    CUsbMessageNotificationObserver* MessageNotificationObserver() const;
    /**
     * @return usbman
     */
    RUsb& Usb();
    /**
     * @return current device state 
     */
    CUsbState* CurrentState() const;
    /**
     * @return current host state 
     */
    CUsbState* CurrentHostState() const;
    /**
     * @param aStateId State Id
     * @return state object by its id 
     */
    CUsbState* State(TUsbStateIds aStateId) const;
    /**
     * @return notif manager
     */
    CUsbNotifManager* NotifManager();
    /**
     * @return Handle state 
     * @param aStateId state id 
     */
    CUsbStateHostHandle* HostHandle(TUsbStateIds aStateId) const;

    /**
     * Handles problems in host functioning
     * @param aWhatKindOf problem Id to be handled
     * @param aInState state id where to handle
     */
    void HandleHostProblemL(TInt aWhatKindOf, TUsbStateIds aInState);

    /**
     * Add observer to USb Otg state machine
     * @param aObserver Observer
     */
    void SubscribeL(MUsbOtgWatcherStateObserver& aObserver);

    /**
     * Remove observer from UsbOtg state observer
     * @param aObserver Observer
     */
    void UnsubscribeL(MUsbOtgWatcherStateObserver& aObserver);

    /**
     * Used for test purposes
     */
    void PrintStateToLog();

private:

    /**
     * Default constructor
     * @param aUsb usbman API 
     */
    CUsbOtgWatcher(RUsb& aUsb);

    /**
     * 2nd phase construction
     */
    void ConstructL();

    /**
     * Starts session
     */
    void StartSessionL();

    /**
     * Changes device's state machine state
     *
     * @param aNewStateId New state id
     */
    void ChangeStateL(TUsbStateIds aNewStateId);

    /**
     * Changes host's state machine state
     *
     * @param aNewStateId new host state (id)
     */
    void ChangeHostStateL(TUsbStateIds aNewStateId);

    /**
     * Used for test purposes
     * @return KErrNone if test is OK, otherwise errorcode
     */
    TInt SelfTestL();

private:
    // data

    /**
     * usbman API
     */
    RUsb& iUsb;

    /**
     * Personality to be set in device role
     */
    TInt iPersonalityId;

    /**
     * Device state machine
     * Own.  
     */
    CUsbState* iState;

    /**
     * Host state machine
     * Own.  
     */
    CUsbState* iHostState;

    /**
     * Id pin Observer
     * Own.  
     */
    CUsbIdPinObserver* iIdPinObserver;

    /**
     * vbus observer
     * Own.  
     */
    CUsbVBusObserver* iVBusObserver;

    /**
     * otg state observer
     * Own.  
     */
    CUsbOtgStateObserver* iOtgStateObserver;

    /**
     * bus activity observer
     * Own.  
     */
    CUsbBusActivityObserver* iBusActivityObserver;

    /**
     * host event notification observer
     * Own.  
     */
    CUsbHostEventNotificationObserver* iHostEventNotificationObserver;

    /**
     * message notification observer
     * Own.  
     */
    CUsbMessageNotificationObserver* iMessageNotificationObserver;

    /**
     * All states objects kept here
     * Own.  
     */
    RPointerArray<CUsbState> iStates;

    /**
     * notif manager
     * Own.  
     */
    CUsbNotifManager* iNotifManager;

    /**
     * usb service control
     * Starts and stops usb service, changes personalities 
     * Own.  
     */
    CUsbServiceControl* iUsbServiceControl;

    /**
     * The observer reports state changes to its own observers
     * Not Own
     */
    RPointerArray<MUsbOtgWatcherStateObserver> iOtgStateObservers;

    /**
     * The observer reports state changes to its own observers
     * Not Own
     */
    CUsbServiceControl::TUsbServiceRequest iUsbServiceRequest;
    };

#endif //  C_USBOTGWATCHER_H
