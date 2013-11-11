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
 * Description:  Implementation
 *
 */
#include <usbpersonalityids.h>
#include <usbuinotif.h>
#include <UsbWatcherInternalPSKeys.h>
#include "cusbotgwatcher.h"
#include "cusbstate.h"
#include "cusbstatehostainitiate.h"
#include "cusbstatehostahost.h"
#include "cusbstatehostaperipheral.h"
#include "cusbstatehosthandle.h"
#include "cusbstatehostdelayhandle.h"
#include "cusbstatehostdelayattachedhandle.h"
#include "cusbstatehostdelaynotattachedhandle.h"
#include "cusbstatehosthandledropping.h"
#include "cusbstatehostundefined.h"
#include "cusbwarningnotifier.h"
#ifndef STIF
#include "cusbnotifmanager.h"
#else
#include "mockcusbnotifmanager.h"
#endif
#include "errors.h"
#include "debug.h"
#include "panic.h"
#ifndef STIF
_LIT_SECURITY_POLICY_PASS( KAlwaysPassPolicy );
_LIT_SECURITY_POLICY_C1( KLocalServicesPolicy, ECapabilityLocalServices );
#endif

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbOtgWatcher::CUsbOtgWatcher(RUsb& aUsbMan) :
    iUsb(aUsbMan), iPersonalityId(KUsbPersonalityIdMTP), iUsbServiceRequest(
            CUsbServiceControl::ERequestUndefined)
    {
    LOG_FUNC
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbOtgWatcher::ConstructL()
    {
    LOG_FUNC

#ifdef _DEBUG
    SelfTestL();
#endif

#ifndef STIF
    LEAVEIFERROR(RProperty::Define(KPSUidUsbWatcher,
                    KUsbWatcherIsPeripheralConnected, RProperty::EInt,
                    KAlwaysPassPolicy, KLocalServicesPolicy));

    LEAVEIFERROR(RProperty::Set(KPSUidUsbWatcher,
                    KUsbWatcherIsPeripheralConnected,
                    KUsbWatcherPeripheralIsNotConnected));
#endif

    iUsbServiceControl = CUsbServiceControl::NewL(*this, iUsb);

    LEAVEIFERROR(iStates.Append(CUsbStateHostUndefined::NewL(*this)));
    LEAVEIFERROR(iStates.Append(CUsbStateHostAInitiate::NewL(*this)));
    LEAVEIFERROR(iStates.Append(CUsbStateHostAHost::NewL(*this)));
    LEAVEIFERROR(iStates.Append(CUsbStateHostAPeripheral::NewL(*this)));

    LEAVEIFERROR(iStates.Append(CUsbStateHostDelayAttachedHandle::NewL(
                            *this)));
    LEAVEIFERROR(iStates.Append(
                    CUsbStateHostDelayNotAttachedHandle::NewL(*this)));
    LEAVEIFERROR(iStates.Append(
                    CUsbStateHostHandleDropping::NewL(*this)));

    iIdPinObserver = CUsbIdPinObserver::NewL();
    iVBusObserver = CUsbVBusObserver::NewL();
    iOtgStateObserver = CUsbOtgStateObserver::NewL();
    iBusActivityObserver = CUsbBusActivityObserver::NewL();
    iHostEventNotificationObserver = CUsbHostEventNotificationObserver::NewL(
            &iUsb);
    iMessageNotificationObserver = CUsbMessageNotificationObserver::NewL(
            &iUsb);

    iHostState = iStates[EUsbStateHostUndefined];

    // Notif manager must be created at least after VBus observer and iHostState initialization
    // to allow USb indicator subscribe to its notifications at construction and check their's current states
    iNotifManager = CUsbNotifManager::NewL(*this);

    iVBusObserver->SubscribeL(*this);
    iOtgStateObserver->SubscribeL(*this);
    iBusActivityObserver->SubscribeL(*this);
    iHostEventNotificationObserver->SubscribeL(*this);
    iMessageNotificationObserver->SubscribeL(*this);
    iIdPinObserver->SubscribeL(*this);

    if (CUsbIdPinObserver::EIdPinOn == iIdPinObserver->IdPin())
        {
        StartSessionL();
        }

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CUsbOtgWatcher::IsDeviceA()
    {
    ASSERT_PANIC(iIdPinObserver != NULL, EIdPinObserverNULLPointer);
    return (iIdPinObserver->IdPin() == CUsbIdPinObserver::EIdPinOn
                                                                   ? ETrue
                                                                      : EFalse);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CUsbOtgWatcher* CUsbOtgWatcher::NewL(RUsb& aUsbMan)
    {
    LOG_FUNC

    CUsbOtgWatcher* self = new (ELeave) CUsbOtgWatcher(aUsbMan);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbOtgWatcher::~CUsbOtgWatcher()
    {
    LOG_FUNC

#ifndef STIF
    RProperty::Delete(KPSUidUsbWatcher, KUsbWatcherIsPeripheralConnected);
#endif

    // delete Notif manager before VBus observer, due to USB indicator observes VBus
    delete iNotifManager;

    if (iIdPinObserver)
        {
        TRAP_IGNORE(iIdPinObserver->UnsubscribeL(*this));
        }

    if (iVBusObserver)
        {
        TRAP_IGNORE(iVBusObserver->UnsubscribeL(*this));
        }
    if (iOtgStateObserver)
        {
        TRAP_IGNORE(iOtgStateObserver->UnsubscribeL(*this));
        }
    if (iBusActivityObserver)
        {
        TRAP_IGNORE(iBusActivityObserver->UnsubscribeL(*this));
        }
    if (iHostEventNotificationObserver)
        {
        TRAP_IGNORE(iHostEventNotificationObserver->UnsubscribeL(*this));
        }
    if (iMessageNotificationObserver)
        {
        TRAP_IGNORE(iMessageNotificationObserver->UnsubscribeL(*this));
        }

    delete iIdPinObserver;
    delete iVBusObserver;
    delete iOtgStateObserver;
    delete iBusActivityObserver;
    delete iHostEventNotificationObserver;
    delete iMessageNotificationObserver;

    iOtgStateObservers.Close();

    // Destroy states
    iStates.ResetAndDestroy();
    iStates.Close();

    delete iUsbServiceControl;

    iUsb.Close();

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbOtgWatcher::SetPersonalityL(TRequestStatus& /*aStatus*/,
        TInt aPersonality)
    {
    LOG_FUNC

    LOG1( "aPersonality = %d" , aPersonality);

    // watcher keeps this value, no need to pass it to request object
    // state can read it from watcher when needed
    iPersonalityId = aPersonality;

    // more complex processing has to be here, remember aStatus, etc.
    // pass the request to device state machine
    iState->SetPersonalityL();

    // the request is not passed to host state machine due to host ignores SetPersonality

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbOtgWatcher::CancelSetPersonalityL()
    {
    LOG_FUNC

    iState->CancelSetPersonalityL();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbOtgWatcher::SetPreviousPersonalityL(TRequestStatus& /*aStatus*/)
    {
    LOG_FUNC

    // maybe more complex processing needed here
    iState->SetPreviousPersonalityL();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbOtgWatcher::SetPreviousPersonalityL()
    {
    LOG_FUNC

    iState->SetPreviousPersonalityL();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbOtgWatcher::CancelSetPreviousPersonalityL()
    {
    LOG_FUNC

    iState->CancelSetPreviousPersonalityL();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbOtgWatcher::SetPreviousPreviousPersonalityOnDisconnectL()
    {
    LOG_FUNC

    }

// From IdPin observer
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbOtgWatcher::StartSessionL()
    {
    LOG_FUNC

    if (!CanStartSessionL())
        {
        HandleHostProblemL(EUsbWatcherCanNotStartSession,
                EUsbStateHostHandleDropping);
        return;
        }

    iUsbServiceRequest = CUsbServiceControl::EStartUsbService;
    TInt err = iUsbServiceControl->StartL(iPersonalityId);
    if (KErrNone != err)
        {
        LOG1( "Can not start usb services. err = %d" , err);
        HandleHostProblemL(EUsbWatcherCanNotStartUsbServices,
                EUsbStateHostHandleDropping);
        return;
        }

    // call back from iUsbServiceControl->Start(iPersonalityId) call is UsbServiceControlReqCompletedL(TInt aError)
    // so, continue there if everything is OK

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbOtgWatcher::HandleHostProblemL(TInt aWhatKindOf,
        TUsbStateIds aInState)
    {
    LOG_FUNC
    HostHandle(aInState)->SetWhat(aWhatKindOf);
    ChangeHostStateL(aInState);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbOtgWatcher::IdPinOnL()
    {
    LOG_FUNC
    StartSessionL();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbOtgWatcher::IdPinOffL()
    {
    LOG_FUNC

    ChangeHostStateL(EUsbStateHostUndefined);

    iNotifManager->CloseAllNotifiers();

    Usb().DisableFunctionDriverLoading();

    iUsbServiceRequest = CUsbServiceControl::EStopUsbService;
    TInt err = iUsbServiceControl->StopL();

    LOG1( "iUsbServiceControl->Stop() err = %d", err );

    if (KErrNone != err)
        {
        LOG( "ErrorStoppingUsbServices" );
        PANIC(ECanNotStopUsbServices);
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbOtgWatcher::IdPinErrorL(TInt aError)
    {
    ASSERT_PANIC(iHostState != NULL, EBadHostState);
    HandleHostProblemL(EUsbWatcherIdPinError, EUsbStateHostHandleDropping);

    }

// From VBus observer
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbOtgWatcher::VBusDownL()
    {
    ASSERT_PANIC(iHostState != NULL, EBadHostState);
    iHostState->VBusDownL();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbOtgWatcher::VBusUpL()
    {
    ASSERT_PANIC(iHostState != NULL, EBadHostState);
    iHostState->VBusUpL();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbOtgWatcher::VBusObserverErrorL(TInt aError)
    {
    ASSERT_PANIC(iHostState != NULL, EBadHostState);
    HandleHostProblemL(EUsbWatcherVBusObserverError,
            EUsbStateHostHandleDropping);
    }

// From OTG state observer
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbOtgWatcher::AIdleL()
    {
    ASSERT_PANIC(iHostState != NULL, EBadHostState);
    iHostState->AIdleL();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbOtgWatcher::AHostL()
    {
    ASSERT_PANIC(iHostState != NULL, EBadHostState);
    iHostState->AHostL();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbOtgWatcher::APeripheralL()
    {
    ASSERT_PANIC(iHostState != NULL, EBadHostState);
    iHostState->APeripheralL();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbOtgWatcher::AVBusErrorL()
    {
    ASSERT_PANIC(iHostState != NULL, EBadHostState);
    iHostState->AVBusErrorL();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbOtgWatcher::BIdleL()
    {
    ASSERT_PANIC(iHostState != NULL, EBadHostState);
    iHostState->BIdleL();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbOtgWatcher::BPeripheralL()
    {
    ASSERT_PANIC(iHostState != NULL, EBadHostState);
    iHostState->BPeripheralL();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbOtgWatcher::BHostL()
    {
    ASSERT_PANIC(iHostState != NULL, EBadHostState);
    iHostState->BHostL();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbOtgWatcher::OtgStateErrorL(TInt aError)
    {
    ASSERT_PANIC(iHostState != NULL, EBadHostState);
    HandleHostProblemL(EUsbWatcherOtgStateError, EUsbStateHostHandleDropping);
    }

// From bus activity observer
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbOtgWatcher::BusIdleL()
    {
    ASSERT_PANIC(iHostState != NULL, EBadHostState);
    iHostState->BusIdleL();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbOtgWatcher::BusActiveL()
    {
    ASSERT_PANIC(iHostState != NULL, EBadHostState);
    iHostState->BusActiveL();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbOtgWatcher::BusActivityErrorL(TInt aError)
    {
    ASSERT_PANIC(iHostState != NULL, EBadHostState);
    // no action, continue
    }

// From Host Event notification observer
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbOtgWatcher::DeviceAttachedL(TDeviceEventInformation aTdi)
    {
    ASSERT_PANIC(iHostState != NULL, EBadHostState);
    iHostState->DeviceAttachedL(aTdi);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbOtgWatcher::DeviceDetachedL(TDeviceEventInformation aTdi)
    {
    ASSERT_PANIC(iHostState != NULL, EBadHostState);
    iHostState->DeviceDetachedL(aTdi);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbOtgWatcher::DriverLoadSuccessL(TDeviceEventInformation aTdi)
    {
    ASSERT_PANIC(iHostState != NULL, EBadHostState);
    iHostState->DriverLoadSuccessL(aTdi);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbOtgWatcher::DriverLoadPartialSuccessL(TDeviceEventInformation aTdi)
    {
    ASSERT_PANIC(iHostState != NULL, EBadHostState);
    iHostState->DriverLoadPartialSuccessL(aTdi);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbOtgWatcher::DriverLoadFailureL(TDeviceEventInformation aTdi)
    {
    ASSERT_PANIC(iHostState != NULL, EBadHostState);
    iHostState->DriverLoadFailureL(aTdi);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbOtgWatcher::HostEventNotificationErrorL(TInt aError)
    {
    ASSERT_PANIC(iHostState != NULL, EBadHostState);
    HandleHostProblemL(EUsbWatcherHostEventNotificationError,
            EUsbStateHostHandleDropping);
    }

// From message notification observer
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbOtgWatcher::MessageNotificationReceivedL(TInt aMessage)
    {
    ASSERT_PANIC(iHostState != NULL, EBadHostState);
    iHostState->MessageNotificationReceivedL(aMessage);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbOtgWatcher::BadHubPositionL()
    {
    ASSERT_PANIC(iHostState != NULL, EBadHostState);
    iHostState->BadHubPositionL();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbOtgWatcher::VBusErrorL()
    {
    ASSERT_PANIC(iHostState != NULL, EBadHostState);
    iHostState->VBusErrorL();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbOtgWatcher::SrpReceivedL()
    {
    ASSERT_PANIC(iHostState != NULL, EBadHostState);
    iHostState->SrpReceivedL();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbOtgWatcher::SessionRequestedL()
    {
    ASSERT_PANIC(iHostState != NULL, EBadHostState);
    iHostState->SessionRequestedL();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbOtgWatcher::MessageNotificationErrorL(TInt aError)
    {
    ASSERT_PANIC(iHostState != NULL, EBadHostState);
    HandleHostProblemL(EUsbWatcherMessageNotificationError,
            EUsbStateHostHandleDropping);
    }

// ---------------------------------------------------------------------------
// getters
// ---------------------------------------------------------------------------
//
CUsbIdPinObserver* CUsbOtgWatcher::IdPinObserver() const
    {
    return iIdPinObserver;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbVBusObserver* CUsbOtgWatcher::VBusObserver() const
    {
    return iVBusObserver;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbOtgStateObserver* CUsbOtgWatcher::OtgStateObserver() const
    {
    return iOtgStateObserver;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbBusActivityObserver* CUsbOtgWatcher::BusActivityObserver() const
    {
    return iBusActivityObserver;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbHostEventNotificationObserver* CUsbOtgWatcher::HostEventNotificationObserver() const
    {
    return iHostEventNotificationObserver;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbMessageNotificationObserver* CUsbOtgWatcher::MessageNotificationObserver() const
    {

    return iMessageNotificationObserver;

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
RUsb& CUsbOtgWatcher::Usb()
    {
    return iUsb;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbState* CUsbOtgWatcher::CurrentState() const
    {
    return iState;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbState* CUsbOtgWatcher::CurrentHostState() const
    {
    return iHostState;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbState* CUsbOtgWatcher::State(TUsbStateIds aStateId) const
    {
    return iStates[aStateId];
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbStateHostHandle* CUsbOtgWatcher::HostHandle(TUsbStateIds aInState) const
    {
    ASSERT_PANIC(iStates[aInState] != NULL, EBadState);

    return (CUsbStateHostHandle*) iStates[aInState];
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbOtgWatcher::ChangeStateL(TUsbStateIds aNewStateId)
    {
    LOG_FUNC

    LOG1( "aNewState = %d" , aNewStateId);

    if (NULL != iState)
        {
        iState->JustBeforeLeavingThisStateL();
        }

    // sets new state	

    // be careful to add states into States in exactly the same order as they enumerated
    // this method will work right as long as states, which in the enumeration, is added 
    // to the list iStates in the same order as they enumerated and no one state is added,
    // if all previous states are not added. no need to add all states, but if added - 
    // previous all must be added.
    // For the general case, when some states added, some not, this method has to implement search by state ID. for this CUsbOtgWatcher::Id() is maintained.

    iState = iStates[aNewStateId];

    ASSERT_PANIC(iState != NULL, EBadState);

    iState->JustAdvancedToThisStateL(); // do any initial activity, once advanced to the state

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbOtgWatcher::ChangeHostStateL(TUsbStateIds aNewStateId)
    {
    LOG_FUNC

    LOG1( "aNewState = %d", aNewStateId);

    if (NULL != iHostState)
        {
        iHostState->JustBeforeLeavingThisStateL();
        }

    // set new state	

    // be careful to add states into States in exactly the same order as they enumerated
    // this method will work right as long as states, which in the enumeration, is added 
    // to the list iStates in the same order as they enumerated and no one state is added,
    // if all previous states are not added. no need to add all states, but if added - 
    // previous all must be added.
    // For the general case, when some states added, some not, this method has to implement search by state ID. for this CUsbOtgWatcher::Id() is maintained.

    iHostState = iStates[aNewStateId];

    ASSERT_PANIC(iHostState != NULL, EBadState)

    iHostState->JustAdvancedToThisStateL(); // checks if there are conditions for advancing to another state(s)

    // notify state change to observers
    for (TInt i(0); i < iOtgStateObservers.Count(); ++i)
        {
        iOtgStateObservers[i]->OtgWatcherStateChangedL(iHostState->Id());
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CUsbOtgWatcher::CanStartSessionL()
    {
    // define policy here
    return ETrue;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbOtgWatcher::PrintStateToLog()
    {
    LOG1( "Current state id  = %d" , iHostState->Id());
    LOG1( "IdPin             = %d" , iIdPinObserver->IdPin());
    LOG1( "VBus              = %d" , iVBusObserver->VBus());
    LOG1( "OtgState          = %d" , iOtgStateObserver->OtgState());
    LOG1( "BusActivity       = %d" , iBusActivityObserver->BusActivity());

    TInt isPeripheralConnected(KUsbWatcherPeripheralIsNotConnected);

    RProperty::Get(KPSUidUsbWatcher, KUsbWatcherIsPeripheralConnected,
            isPeripheralConnected);

    LOG1( "IsPeripheralConnected = %d" , isPeripheralConnected);

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbNotifManager* CUsbOtgWatcher::NotifManager()
    {
    return iNotifManager;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbOtgWatcher::UsbServiceControlReqCompletedL(TInt aError)
    {
    LOG_FUNC

    LOG1( "aError = %d" , aError);

    iUsbServiceRequest = CUsbServiceControl::ERequestUndefined;

    switch (aError)
        {
        case KErrInUse:
            // usb services already started (this might happen if more than one idpin on event come)
            {
            return;
            }

        case KErrNone:
            {
            break; // do normal routine
            }
        case KErrNoMemory:
            {
            HandleHostProblemL(EUsbWatcherNoMemory,
                    EUsbStateHostHandleDropping);
            return;
            }

        default:
            // handle the issue
            {
            if (iUsbServiceRequest == CUsbServiceControl::EStartUsbService) // Handle only start issues
                {
                HandleHostProblemL(EUsbWatcherNoMemory,
                        EUsbStateHostHandleDropping);
                }

            iUsbServiceRequest = CUsbServiceControl::ERequestUndefined;
            return;
            }
        }

    iUsbServiceRequest = CUsbServiceControl::ERequestUndefined;

    TUsbServiceState serviceState = EUsbServiceIdle;
    TInt err = iUsb.GetServiceState(serviceState);

    if (KErrNone != err)
        {
        LOG1( "Error when requesting GetServiceState = %d" , err);
        HandleHostProblemL(EUsbWatcherCanNotStartUsbServices,
                EUsbStateHostHandleDropping);
        return;
        }

    switch (serviceState)
        {
        case EUsbServiceIdle: // just stopped usb service

            {
            LOG("UsbServiceState == EUsbServiceIdle" );
            // do nothing
            break;
            }

        case EUsbServiceStarted: // just started usb service

            {
            LOG( "UsbServiceState == EUsbServiceStarted" );

            ChangeHostStateL(EUsbStateHostAInitiate);

            break;
            }
        case EUsbServiceStarting:
            {
            LOG("UsbServiceState == EUsbServiceStarting" );
            // should not receive that, due to call back is called when service stopped or started
            // therefore scream
            PANIC(EUnexpectedUsbServiceState);
            break;
            }
        case EUsbServiceStopping:
            {
            LOG("UsbServiceState == EUsbServiceStopping" );
            // should not receive that, due to call back is called when service stopped or started
            // therefore scream
            PANIC(EUnexpectedUsbServiceState);
            break;
            }
        case EUsbServiceFatalError:
            {
            LOG( "UsbServiceState == EUsbServiceFatalError" );
            PANIC(EUnexpectedUsbServiceState);
            break;
            }

        default:
            {
            PANIC(EUnknownUsbServiceState);
            }
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CUsbOtgWatcher::SelfTestL()
    {
#ifdef _DEBUG
    LOG_FUNC

    // create all the observers
    iIdPinObserver = CUsbIdPinObserver::NewL();
    iVBusObserver = CUsbVBusObserver::NewL();
    iOtgStateObserver = CUsbOtgStateObserver::NewL();
    iBusActivityObserver = CUsbBusActivityObserver::NewL();
    iHostEventNotificationObserver = CUsbHostEventNotificationObserver::NewL(
            &iUsb);
    iMessageNotificationObserver = CUsbMessageNotificationObserver::NewL(
            &iUsb);

    LOG( "Observers getters" );

    if (iIdPinObserver != IdPinObserver())
        {
        LEAVE(KErrGeneral);
        }

    if (iOtgStateObserver != OtgStateObserver())
        {
        LEAVE(KErrGeneral);
        }

    if (iBusActivityObserver != BusActivityObserver())
        {
        LEAVE(KErrGeneral);
        }

    if (iHostEventNotificationObserver != HostEventNotificationObserver())
        {
        LEAVE(KErrGeneral);
        }

    if (iMessageNotificationObserver != MessageNotificationObserver())
        {
        LEAVE(KErrGeneral);
        }

    LOG( "Observers destructors" );

    // idpinobserver is deleted later        
    // Vbus observer is deleted later

    delete iOtgStateObserver;
    iOtgStateObserver = 0;
    delete iBusActivityObserver;
    iBusActivityObserver = 0;
    delete iHostEventNotificationObserver;
    iHostEventNotificationObserver = 0;
    delete iMessageNotificationObserver;
    iMessageNotificationObserver = 0;

    LOG("Creating states");

    LEAVEIFERROR(iStates.Append(CUsbStateHostUndefined::NewL(*this)));
    LEAVEIFERROR(iStates.Append(CUsbStateHostAInitiate::NewL(*this)));
    LEAVEIFERROR(iStates.Append(CUsbStateHostAHost::NewL(*this)));
    LEAVEIFERROR(iStates.Append(CUsbStateHostAPeripheral::NewL(*this)));
    LEAVEIFERROR(iStates.Append(CUsbStateHostDelayAttachedHandle::NewL(
            *this)));
    LEAVEIFERROR(iStates.Append(
            CUsbStateHostDelayNotAttachedHandle::NewL(*this)));
    LEAVEIFERROR(iStates.Append(
            CUsbStateHostHandleDropping::NewL(*this)));

    LOG("Check State()" );

    if (iStates[EUsbStateHostAInitiate] != State(EUsbStateHostAInitiate))
        {
        LEAVE(KErrGeneral);
        }

    LOG("Check CurrentHostState()" );

    iHostState = iStates[EUsbStateHostAInitiate];

    if (iStates[EUsbStateHostAInitiate] != CurrentHostState())
        {
        LEAVE(KErrGeneral);
        }

    LOG("NotifManager and WarningNotifier." );

    CUsbNotifManager* usbnotifmanager = CUsbNotifManager::NewL(*this);
    RNotifier rnotifier;
    LEAVEIFERROR(rnotifier.Connect());
    CUsbWarningNotifier* usbnotifier = CUsbWarningNotifier::NewL(rnotifier,
            *usbnotifmanager, EUsbOtgPartiallySupportedDevice);
    usbnotifier->IsFeedbackNeeded();

    LOG( "NotifManager and WarningNotifier destruction." );

    delete usbnotifier;
    rnotifier.Close();
    delete usbnotifmanager;

    // VBus observer is deleted here, due to it is used by usbnotifmanager.usbindicatornotifier
    delete iVBusObserver;
    iVBusObserver = 0;

    // id pin observer is deleted here due to it is used by usbnotifmanager.usbindicatornotifier
    delete iIdPinObserver;
    iIdPinObserver = 0;

    LOG("Destructing states");

    iStates.ResetAndDestroy();

    LOG( "Check UsbServiceControl" );

    CUsbServiceControl* usbServiceControl = CUsbServiceControl::NewL(*this,
            iUsb);
    usbServiceControl->RunError(KErrNone);
    delete usbServiceControl;

    LOG("All completed OK" );
#endif
    return KErrNone;

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//   
void CUsbOtgWatcher::SubscribeL(MUsbOtgWatcherStateObserver& aObserver)
    {
    LOG_FUNC

    // check if the same observer already exist in a list
    if (KErrNotFound != iOtgStateObservers.Find(&aObserver))
        {
        LOG( "Observer already exists" );
        PANIC(EObserverAlreadyExists);
        return;
        }
    iOtgStateObservers.AppendL(&aObserver);

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbOtgWatcher::UnsubscribeL(MUsbOtgWatcherStateObserver& aObserver)
    {
    LOG_FUNC

    TInt i(iOtgStateObservers.Find(&aObserver));
    if (KErrNotFound == i)
        {
        LOG( "Observer not found" );
        PANIC(ECanNotFindUsbOtgWatcherStateObserver);
        return;
        }

    iOtgStateObservers.Remove(i);
    }
