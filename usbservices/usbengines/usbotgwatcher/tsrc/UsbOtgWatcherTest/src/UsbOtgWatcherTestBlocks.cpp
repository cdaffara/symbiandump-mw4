/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/

// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <StifTestInterface.h>
#include "UsbOtgWatcherTest.h"
#include "testdebug.h"
#include "panic.h"
//#include "UsbWatcherInternalPSKeys.h"
#include "mockusbman.h"
#include "mockcusbnotifmanager.h"
#include <usbpersonalityids.h>
#include "cusbindicatornotifier.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CtUsbOtgWatcher::TestConstructL
// Construct here all test resources for test methods. 
// Called from ConstructL. 
// -----------------------------------------------------------------------------

void CtUsbOtgWatcher::TestConstructL()
    {
    TRACE_FUNC_ENTRY
    
    iStubber = CStubber::NewL();
    
    TRACE_FUNC_EXIT
    }

// -----------------------------------------------------------------------------
// CtUsbOtgWatcher::TestDelete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------

void CtUsbOtgWatcher::TestDelete() 
    {
    TRACE_FUNC_ENTRY
    
    delete iStubber;
    
    TRACE_FUNC_EXIT
    }

// -----------------------------------------------------------------------------
// CtUsbOtgWatcher::TestBlocksInfoL
// Add new test block parameter entries below
// -----------------------------------------------------------------------------

void CtUsbOtgWatcher::TestBlocksInfoL()
    {
    
    TESTENTRY( "ExampleTestL", CtUsbOtgWatcher::ExampleTestL )
            {
            TESTPARAM( eGetString, eTestOption1 )
            TESTPARAM( eGetString, eTestOption2 )
            TESTPARAM( eGetInt, eTestIntOption1 )
            TESTPARAM( eGetChar, eTestCharOption1 )
            }
    
    TESTENTRY( "SetupTearDown", CtUsbOtgWatcher::SetupTearDownL )
                {
                TESTPARAM( eGetString, eTestOption1 )
                }

    TESTENTRY( "PinOnTest_Success", CtUsbOtgWatcher::PinOnTest_SuccessL )
                {
                TESTPARAM( eGetString, eTestOption1 )
                }
    
    TESTENTRY( "PinOffTest_Success", CtUsbOtgWatcher::PinOffTest_SuccessL )
                {
                TESTPARAM( eGetString, eTestOption1 )
                }
    
    TESTENTRY( "PinOnTest_EmptyCable", CtUsbOtgWatcher::PinOnTest_EmptyCableL )
                {
                TESTPARAM( eGetString, eTestOption1 )
                }
    
    TESTENTRY( "PinOnTest_TryStartError", CtUsbOtgWatcher::PinOnTest_TryStartErrorL )
                {
                TESTPARAM( eGetString, eTestOption1 )
                }
    
    TESTENTRY( "PinOnTest_BusRequestError", CtUsbOtgWatcher::PinOnTest_BusRequestErrorL )
                {
                TESTPARAM( eGetString, eTestOption1 )
                }
    
    TESTENTRY( "DeviceAttached_BadHubPostion", CtUsbOtgWatcher::DeviceAttached_BadHubPostionL )
                {
                TESTPARAM( eGetString, eTestOption1 )
                }
    
    TESTENTRY( "DeviceAttached_TooMuchRequired", CtUsbOtgWatcher::DeviceAttached_TooMuchRequiredL )
                {
                TESTPARAM( eGetString, eTestOption1 )
                }
    
    TESTENTRY( "DeviceAttached_SupportedDevice", CtUsbOtgWatcher::DeviceAttached_SupportedDeviceL )
                {
                TESTPARAM( eGetString, eTestOption1 )
                }
    
    TESTENTRY( "DeviceDetached_SupportedDevice", CtUsbOtgWatcher::DeviceDetached_SupportedDeviceL )
                {
                TESTPARAM( eGetString, eTestOption1 )
                }
 
     TESTENTRY( "IndicatorNotifier_IndicatorActivateL", CtUsbOtgWatcher::IndicatorNotifier_IndicatorActivateL )
                {
                TESTPARAM( eGetString, eTestOption1 )
                }
    }

// Add test block methods implementation here
// -----------------------------------------------------------------------------
// CtUsbOtgWatcher::ExampleTestL
// -----------------------------------------------------------------------------

void CtUsbOtgWatcher::ExampleTestL( TUsbOtgWatcherTestBlockParams& aParams,
                                            TUsbOtgWatcherTestResult& aTestResult )
    {
    TRACE_FUNC_ENTRY
    
    //iStubber -> EnqueBehavior( TApiBehavior( KUsbManStubAgentDll, ETryStartAsync, 0, 0, _L8( "stub" ) ) );
    
    if ( !aParams.iTestOption1.Compare( _L( "API" ) ) )
        {
		TRACE_INFO( "Api test option: %S", &aParams.iTestOption1 )
		TRACE_INFO( "Api test sub-option: %S", &aParams.iTestOption2 )
		TRACE_INFO( "Api test int option: %d", aParams.iTestIntOption1 )
		TRACE_INFO( "Api test char option: %c", TText( aParams.iTestCharOption1 ) )
        }
    else if ( !aParams.iTestOption1.Compare( _L( "MODULE" ) ) )
        {
		TRACE_INFO( "Module test option: %S", &aParams.iTestOption1 )
		TRACE_INFO( "Module test sub-option: %S", &aParams.iTestOption2 )
		TRACE_INFO( "Module test int option: %d", aParams.iTestIntOption1 )
		TRACE_INFO( "Module test char option: %c", TText( aParams.iTestCharOption1 ) )
        }
    else if ( !aParams.iTestOption1.Compare( _L( "BRANCH" ) ) )
        {
		TRACE_INFO( "Branch test option: %S", &aParams.iTestOption1 )
		TRACE_INFO( "Branch test sub-option: %S", &aParams.iTestOption2 )
		TRACE_INFO( "Branch test int option: %d", aParams.iTestIntOption1 )
		TRACE_INFO( "Branch test char option: %c", TText( aParams.iTestCharOption1 ) )
        }
    else
        {
        TRACE( "Invalid test parameter" )
        TRACE_FUNC_EXIT
        User::Leave( KErrNotFound );
        }
    
    aTestResult = ETestCasePassed;
    TRACE_FUNC_EXIT
    }


void CtUsbOtgWatcher::SetupTearDownL( TUsbOtgWatcherTestBlockParams& aParams,
                                            TUsbOtgWatcherTestResult& aTestResult )
    {
    TRACE_FUNC_ENTRY;

    TRACE( "Create classes of the state machine" );
    SetupL();
    
    TRACE( "Destroy classes of the state machine" );
    TearDown();

    aTestResult = ETestCasePassed;
    }


void CtUsbOtgWatcher::PinOnTest_SuccessL( TUsbOtgWatcherTestBlockParams& aParams,
        TUsbOtgWatcherTestResult& aTestResult )
    {
    TRACE(">>CtUsbOtgWatcher::PinOnTest_SuccessL");
    
    SetupL();
    
    // Idpin observer should be subscribed
    STIF_ASSERT_TRUE(iWatcher->IdPinObserver()->iObserver != NULL);
    
    TRACE( "State machine initialized" );
    CUsbStateHostAInitiate* aInitiate = (CUsbStateHostAInitiate*)(iWatcher->State(EUsbStateHostAInitiate));
    STIF_ASSERT_NOT_NULL(aInitiate);
    
    STIF_ASSERT_EQUALS(CUsbVBusObserver::EVBusDown, iWatcher->VBusObserver()->VBus());    

    TRACE( "Trigger pin on" );
    iStubber -> EnqueBehavior( TApiBehavior( KUsbManStubAgentDll, EServiceState, 0, 0, _L8( "EUsbServiceIdle" ) ) );
    iStubber -> EnqueBehavior( TApiBehavior( KUsbManStubAgentDll, EGetCurrentPersonalityId, 0, 0, _L8( "UsbPersonalityIdMTP" ) ) );
    iStubber -> EnqueBehavior( TApiBehavior( KUsbManStubAgentDll, EServiceState, 0, 0, _L8( "EUsbServiceStarted" ) ) );
    iStubber -> EnqueBehavior( TApiBehavior( KUsbManStubAgentDll, EServiceState, 0, 0, _L8( "EUsbServiceStarted" ) ) );
    iStubber -> EnqueBehavior( TApiBehavior( KUsbManStubAgentDll, EGetCurrentPersonalityId, 0, 0, _L8( "UsbPersonalityIdMTP" ) ) );
//    iStubber -> EnqueBehavior( TApiBehavior( KUsbManStubAgentDll, EServiceState, 0, 0, _L8( "EUsbServiceStarted" ) ) );
    
    STIF_ASSERT_NOT_LEAVES(iWatcher->IdPinObserver()->TriggerPinOnL());
    TRACE_INFO("Status iWatcher->iUsbServiceControl->iStatus: %d",iWatcher->iUsbServiceControl->iStatus.Int());
    
    iWatcher->iUsbServiceControl->RunL();
        
    TRACE_INFO("iWatcher->IdPinObserver()->IsTestResultNoneError(): %d", iWatcher->IdPinObserver()->IsTestResultNoneError() ? 1 : 0 );
    STIF_ASSERT_TRUE(iWatcher->IdPinObserver()->IsTestResultNoneError());
    
    TRACE_INFO("iWatcher->VBusObserver()->iObserver: %d", iWatcher->VBusObserver()->iObserver ? 1 : 0 );
    STIF_ASSERT_TRUE(iWatcher->VBusObserver()->iObserver != NULL);
    
    TRACE_INFO("iWatcher->OtgStateObserver()->iObserver: %d", iWatcher->OtgStateObserver()->iObserver ? 1 : 0 );
    STIF_ASSERT_TRUE(iWatcher->OtgStateObserver()->iObserver != NULL);
    
    TRACE_INFO("iWatcher->BusActivityObserver()->iObserver: %d", iWatcher->BusActivityObserver()->iObserver ? 1 : 0 );
    STIF_ASSERT_TRUE(iWatcher->BusActivityObserver()->iObserver != NULL);    
    
    TRACE_INFO("iWatcher->HostEventNotificationObserver()->iObserver: %d", iWatcher->HostEventNotificationObserver()->iObserver ? 1 : 0 );
    STIF_ASSERT_TRUE(iWatcher->HostEventNotificationObserver()->iObserver != NULL);
    
    TRACE_INFO("iWatcher->MessageNotificationObserver()->iObserver: %d", iWatcher->MessageNotificationObserver()->iObserver ? 1 : 0 );
    STIF_ASSERT_TRUE(iWatcher->MessageNotificationObserver()->iObserver != NULL);
      
    TRACE_INFO("CUsbVBusObserver::EVBusUp == iWatcher->VBusObserver()->VBus(): %d", CUsbVBusObserver::EVBusUp == iWatcher->VBusObserver()->VBus() ? 1 : 0 );
    STIF_ASSERT_EQUALS(CUsbVBusObserver::EVBusUp, iWatcher->VBusObserver()->VBus()); 
        
    
    TRACE( "Trigger vbus on" );
    STIF_ASSERT_NOT_LEAVES(iWatcher->VBusObserver()->TriggerVBusUpL());
    
    TRACE_INFO("iWatcher->VBusObserver()->IsTestResultNoneError(): %d", iWatcher->VBusObserver()->IsTestResultNoneError() ? 1 : 0 );
    STIF_ASSERT_TRUE(iWatcher->VBusObserver()->IsTestResultNoneError());
    
    STIF_ASSERT_EQUALS(CUsbVBusObserver::EVBusUp, iWatcher->VBusObserver()->VBus());
    
    TRACE_INFO("iWatcher->CurrentHostState() == iWatcher->iStates[EUsbStateHostAInitiate]: %d", iWatcher->CurrentHostState() == iWatcher->iStates[EUsbStateHostAInitiate] ? 1 : 0 );
    STIF_ASSERT_TRUE(iWatcher->CurrentHostState() == iWatcher->iStates[EUsbStateHostAInitiate]);
    
    TRACE_INFO("aInitiate->iAttachmentTimer->IsActive(): %d", aInitiate->iAttachmentTimer->IsActive() ? 1 : 0 );
    STIF_ASSERT_TRUE(aInitiate->iAttachmentTimer->IsActive());
    
    TRACE_INFO("CUsbVBusObserver::EVBusUp == iWatcher->VBusObserver()->VBus(): %d", CUsbVBusObserver::EVBusUp == iWatcher->VBusObserver()->VBus() ? 1 : 0 );
    STIF_ASSERT_EQUALS(CUsbVBusObserver::EVBusUp, iWatcher->VBusObserver()->VBus());

//    iUsbMan->StartCancel(iWatcher->iUsbServiceControl->iStatus);
    
    TearDown();

    aTestResult = ETestCasePassed;
    
    TRACE("<<CtUsbOtgWatcher::PinOnTest_SuccessL");
    }

void CtUsbOtgWatcher::PinOffTest_SuccessL( TUsbOtgWatcherTestBlockParams& aParams,
        TUsbOtgWatcherTestResult& aTestResult )
    {
    TRACE(">>CtUsbOtgWatcher::PinOffTest_SuccessL");
    SetupL();
    
    // Idpin observer should be subscribed
    STIF_ASSERT_TRUE(iWatcher->IdPinObserver()->iObserver != NULL);
    
    STIF_ASSERT_EQUALS(CUsbVBusObserver::EVBusDown, iWatcher->VBusObserver()->VBus());

    TRACE( "Trigger pin on" );
    iStubber -> EnqueBehavior( TApiBehavior( KUsbManStubAgentDll, ETryStartAsync, 0, 0, _L8( "stub" ) ) );
    iStubber -> EnqueBehavior( TApiBehavior( KUsbManStubAgentDll, EServiceState, 0, 0, _L8( "EUsbServiceIdle" ) ) );
    iStubber -> EnqueBehavior( TApiBehavior( KUsbManStubAgentDll, EServiceState, 0, 0, _L8( "EUsbServiceStarted" ) ) );
    iStubber -> EnqueBehavior( TApiBehavior( KUsbManStubAgentDll, EServiceState, 0, 0, _L8( "EUsbServiceStarted" ) ) );
    iStubber -> EnqueBehavior( TApiBehavior( KUsbManStubAgentDll, EGetCurrentPersonalityId, 0, 0, _L8( "UsbPersonalityIdMTP" ) ) );
    iStubber -> EnqueBehavior( TApiBehavior( KUsbManStubAgentDll, EGetCurrentPersonalityId, 0, 0, _L8( "UsbPersonalityIdMTP" ) ) );
//    iStubber -> EnqueBehavior( TApiBehavior( KUsbManStubAgentDll, EServiceState, 0, 0, _L8( "EUsbServiceStarted" ) ) );
    iWatcher->IdPinObserver()->TriggerPinOnL();
    iWatcher->iUsbServiceControl->RunL();
    
    TRACE( "Trigger vbus on" );
    iWatcher->VBusObserver()->TriggerVBusUpL();  
    
    STIF_ASSERT_TRUE(iWatcher->CurrentHostState() == iWatcher->iStates[EUsbStateHostAInitiate]);    
    STIF_ASSERT_EQUALS(CUsbVBusObserver::EVBusUp, iWatcher->VBusObserver()->VBus());
//    iUsbMan->StartCancel(iWatcher->iUsbServiceControl->iStatus);
        
    TRACE( "Trigger vbus off" );
    iWatcher->IdPinObserver()->TriggerPinOffL();
    iWatcher->iUsbServiceControl->RunL();
    
    STIF_ASSERT_TRUE(iWatcher->CurrentHostState() != NULL);

    STIF_ASSERT_TRUE(iWatcher->CurrentHostState() != iWatcher->iStates[EUsbStateHostAInitiate]);
    STIF_ASSERT_TRUE(iWatcher->VBusObserver()->IsTestResultNoneError());
    STIF_ASSERT_TRUE(iWatcher->IdPinObserver()->IsTestResultNoneError());
    STIF_ASSERT_TRUE(iWatcher->VBusObserver()->iObserver != NULL);
    STIF_ASSERT_TRUE(iWatcher->OtgStateObserver()->iObserver != NULL);
    STIF_ASSERT_TRUE(iWatcher->BusActivityObserver()->iObserver != NULL);
    STIF_ASSERT_TRUE(iWatcher->HostEventNotificationObserver()->iObserver != NULL);
    STIF_ASSERT_TRUE(iWatcher->MessageNotificationObserver()->iObserver != NULL);    

//    iUsbMan->StartCancel(iWatcher->iUsbServiceControl->iStatus);
    TearDown();

    aTestResult = ETestCasePassed;
    
    TRACE("<<CtUsbOtgWatcher::PinOffTest_SuccessL");
    }

void CtUsbOtgWatcher::PinOnTest_EmptyCableL( TUsbOtgWatcherTestBlockParams& aParams,
        TUsbOtgWatcherTestResult& aTestResult )
    {
    TRACE(">>CtUsbOtgWatcher::PinOnTest_EmptyCable");
    SetupL();
    
    TRACE("State machine initialized");
    CUsbStateHostAInitiate* aInitiate = (CUsbStateHostAInitiate*)(iWatcher->State(EUsbStateHostAInitiate));

    TRACE( "Trigger pin on" );
    iStubber -> EnqueBehavior( TApiBehavior( KUsbManStubAgentDll, ETryStartAsync, 0, 0, _L8( "stub" ) ) );
    iStubber -> EnqueBehavior( TApiBehavior( KUsbManStubAgentDll, EServiceState, 0, 0, _L8( "EUsbServiceStarted" ) ) );
    iStubber -> EnqueBehavior( TApiBehavior( KUsbManStubAgentDll, EServiceState, 0, 0, _L8( "EUsbServiceStarted" ) ) );
    iStubber -> EnqueBehavior( TApiBehavior( KUsbManStubAgentDll, EGetCurrentPersonalityId, 0, 0, _L8( "UsbPersonalityIdMTP" ) ) );
    iStubber -> EnqueBehavior( TApiBehavior( KUsbManStubAgentDll, EGetCurrentPersonalityId, 0, 0, _L8( "UsbPersonalityIdMTP" ) ) );
//    iStubber -> EnqueBehavior( TApiBehavior( KUsbManStubAgentDll, EServiceState, 0, 0, _L8( "EUsbServiceStarted" ) ) );
       
    STIF_ASSERT_NOT_LEAVES(iWatcher->IdPinObserver()->TriggerPinOnL());
    iWatcher->iUsbServiceControl->RunL();
    
    TRACE( "Trigger vbus on" );
    STIF_ASSERT_NOT_LEAVES(iWatcher->VBusObserver()->TriggerVBusUpL());
    
//    STIF_ASSERT_TRUE(iWatcher->NotifManager()->GetUsbIconBlinking());
    
    TRACE( "Trigger attachment timer elapsed" );
    STIF_ASSERT_NOT_LEAVES(aInitiate->iAttachmentTimer->TriggerTimerElapsedL());
    aInitiate->iAttachmentTimer->TriggerTimerElapsedL();
    STIF_ASSERT_TRUE(aInitiate->iAttachmentTimer->IsTestResultNoneError());

    STIF_ASSERT_TRUE(iWatcher->CurrentHostState() == iWatcher->iStates[EUsbStateHostHandleDropping]);
    STIF_ASSERT_TRUE(KUsbUiNotifOtgError == iWatcher->iNotifManager->iCat); 
    STIF_ASSERT_TRUE(EUsbOtgErrorAttachTimedOut == iWatcher->iNotifManager->iNotifId);
    
    STIF_ASSERT_TRUE(NULL != iWatcher->iNotifManager->iObserver);
    STIF_ASSERT_FALSE(iWatcher->iNotifManager->iUsbIconBlinking);
    STIF_ASSERT_FALSE(iWatcher->iNotifManager->iUsbIconShown);
    
    STIF_ASSERT_EQUALS(CUsbVBusObserver::EVBusDown, iWatcher->VBusObserver()->VBus());
    TearDown();

    aTestResult = ETestCasePassed;
    
    TRACE("<<CtUsbOtgWatcher::PinOnTest_EmptyCable");
    }

void CtUsbOtgWatcher::PinOnTest_TryStartErrorL( TUsbOtgWatcherTestBlockParams& aParams,
        TUsbOtgWatcherTestResult& aTestResult )
    {
    TRACE(">>CtUsbOtgWatcher::PinOnTest_TryStartErrorL");

    SetupL();
    
    CUsbStateHostAInitiate* aInitiate = (CUsbStateHostAInitiate*)(iWatcher->State(EUsbStateHostAInitiate));
    STIF_ASSERT_NOT_NULL(aInitiate);

    iStubber -> EnqueBehavior( TApiBehavior( KUsbManStubAgentDll, ETryStartAsync, 0, 0, _L8( "stub" ) ) );
    iWatcher->Usb().SetTryStartError(KErrGeneral);
    STIF_ASSERT_PANIC(ECanNotStartUsbServices, iWatcher->IdPinObserver()->TriggerPinOnL());

    TestModuleIf().SetExitReason( CTestModuleIf::ENormal, KErrNone );
    
    TearDown();

    aTestResult = ETestCasePassed;

    TRACE("<<CtUsbOtgWatcher::PinOnTest_TryStartErrorL");
    }

void CtUsbOtgWatcher::PinOnTest_BusRequestErrorL( TUsbOtgWatcherTestBlockParams& aParams,
        TUsbOtgWatcherTestResult& aTestResult )
    {
    TRACE(">>CtUsbOtgWatcher::PinOnTest_BusRequestErrorL");
    
    SetupL();
    
    CUsbStateHostAInitiate* aInitiate = (CUsbStateHostAInitiate*)(iWatcher->State(EUsbStateHostAInitiate));
    STIF_ASSERT_NOT_NULL(aInitiate);

    iStubber -> EnqueBehavior( TApiBehavior( KUsbManStubAgentDll, ETryStartAsync, 0, 0, _L8( "stub" ) ) );
    iWatcher->Usb().SetBusRequestError(KErrGeneral);
    STIF_ASSERT_PANIC(EBusRequestError, iWatcher->IdPinObserver()->TriggerPinOnL());

    TestModuleIf().SetExitReason( CTestModuleIf::ENormal, KErrNone );
    
    TearDown();

    aTestResult = ETestCasePassed;

    TRACE("<<CtUsbOtgWatcher::PinOnTest_BusRequestErrorL");
    }

void CtUsbOtgWatcher::DeviceAttached_BadHubPostionL( TUsbOtgWatcherTestBlockParams& aParams,
        TUsbOtgWatcherTestResult& aTestResult )
    {
    TRACE(">>CtUsbOtgWatcher::DeviceAttached_BadHubPostionL");
    SetupL();

    iStubber -> EnqueBehavior( TApiBehavior( KUsbManStubAgentDll, ETryStartAsync, 0, 0, _L8( "stub" ) ) );
    iStubber -> EnqueBehavior( TApiBehavior( KUsbManStubAgentDll, EServiceState, 0, 0, _L8( "EUsbServiceIdle" ) ) );
    iStubber -> EnqueBehavior( TApiBehavior( KUsbManStubAgentDll, EServiceState, 0, 0, _L8( "EUsbServiceStarted" ) ) );
    iStubber -> EnqueBehavior( TApiBehavior( KUsbManStubAgentDll, EServiceState, 0, 0, _L8( "EUsbServiceStarted" ) ) );
    iStubber -> EnqueBehavior( TApiBehavior( KUsbManStubAgentDll, EGetCurrentPersonalityId, 0, 0, _L8( "UsbPersonalityIdMTP" ) ) );
    iStubber -> EnqueBehavior( TApiBehavior( KUsbManStubAgentDll, EGetCurrentPersonalityId, 0, 0, _L8( "UsbPersonalityIdMTP" ) ) );
//    iStubber -> EnqueBehavior( TApiBehavior( KUsbManStubAgentDll, EServiceState, 0, 0, _L8( "EUsbServiceStarted" ) ) );
    
    TRACE( "PinOn triggered" );
    STIF_ASSERT_NOT_LEAVES(iWatcher->IdPinObserver()->TriggerPinOnL());
    iWatcher->iUsbServiceControl->RunL();
    
    TRACE( "Trigger vbus on" );
    STIF_ASSERT_NOT_LEAVES(iWatcher->VBusObserver()->TriggerVBusUpL());

    TRACE( "BadHubPosition triggered" );
    STIF_ASSERT_NOT_LEAVES(iWatcher->MessageNotificationObserver()->TriggerBadHubPositionL());
    TRACE( "BadHubPosition test result" );
    STIF_ASSERT_TRUE(iWatcher->MessageNotificationObserver()->IsTestResultNoneError());

    TRACE( "Poscondition check" );
    STIF_ASSERT_TRUE(KUsbUiNotifOtgError == iWatcher->iNotifManager->iCat);   
    STIF_ASSERT_TRUE(EUsbOtgHubUnsupported == iWatcher->iNotifManager->iNotifId);
    STIF_ASSERT_TRUE(NULL != iWatcher->iNotifManager->iObserver);
    
    STIF_ASSERT_EQUALS(CUsbVBusObserver::EVBusDown, iWatcher->VBusObserver()->VBus());

    TearDown();

    aTestResult = ETestCasePassed;

    TRACE("<<CtUsbOtgWatcher::DeviceAttached_BadHubPostionL");
    }

void CtUsbOtgWatcher::DeviceAttached_TooMuchRequiredL( TUsbOtgWatcherTestBlockParams& aParams,
        TUsbOtgWatcherTestResult& aTestResult )
    {
    TRACE("<<CtUsbOtgWatcher::DeviceAttached_TooMuchRequiredL");
    SetupL();

    iStubber -> EnqueBehavior( TApiBehavior( KUsbManStubAgentDll, ETryStartAsync, 0, 0, _L8( "stub" ) ) );
    iStubber -> EnqueBehavior( TApiBehavior( KUsbManStubAgentDll, EServiceState, 0, 0, _L8( "EUsbServiceIdle" ) ) );
    iStubber -> EnqueBehavior( TApiBehavior( KUsbManStubAgentDll, EServiceState, 0, 0, _L8( "EUsbServiceStarted" ) ) );
    iStubber -> EnqueBehavior( TApiBehavior( KUsbManStubAgentDll, EServiceState, 0, 0, _L8( "EUsbServiceStarted" ) ) );
    iStubber -> EnqueBehavior( TApiBehavior( KUsbManStubAgentDll, EGetCurrentPersonalityId, 0, 0, _L8( "UsbPersonalityIdMTP" ) ) );
    iStubber -> EnqueBehavior( TApiBehavior( KUsbManStubAgentDll, EGetCurrentPersonalityId, 0, 0, _L8( "UsbPersonalityIdMTP" ) ) );
//    iStubber -> EnqueBehavior( TApiBehavior( KUsbManStubAgentDll, EServiceState, 0, 0, _L8( "EUsbServiceStarted" ) ) );
        
    TRACE( "PinOn triggered" );
    STIF_ASSERT_NOT_LEAVES(iWatcher->IdPinObserver()->TriggerPinOnL());
    iWatcher->iUsbServiceControl->RunL();
    
    TRACE( "Trigger vbus on" );
    STIF_ASSERT_NOT_LEAVES(iWatcher->VBusObserver()->TriggerVBusUpL());

//    STIF_ASSERT_TRUE(iWatcher->iNotifManager->GetUsbIconBlinking());
    TDeviceEventInformation eventInfo;
    eventInfo.iEventType = EDeviceAttachment;
    eventInfo.iError = KErrBadPower;
    TRACE( "Trigger device attched - KErrBadPower" );
    iWatcher->HostEventNotificationObserver()->TriggerDeviceAttachedL(eventInfo);
    
    STIF_ASSERT_TRUE(iWatcher->CurrentHostState() == iWatcher->iStates[EUsbStateHostDelayNotAttachedHandle]);
//    STIF_ASSERT_TRUE(iWatcher->CurrentHostState() == iWatcher->iStates[EUsbStateHostHandleDropping]);

    STIF_ASSERT_EQUALS(CUsbVBusObserver::EVBusUp, iWatcher->VBusObserver()->VBus());

//    TRACE( "Trigger timer ETooMuchPowerRequiredTimer elapsed" );
//    CUsbStateHostHandle* stateHostHandle = (CUsbStateHostHandle*)iWatcher->CurrentHostState();
//    STIF_ASSERT_NOT_LEAVES( stateHostHandle->iTooMuchPowerTimer->TriggerTimerElapsedL() );
    
//    STIF_ASSERT_FALSE(iWatcher->iNotifManager->iUsbIconBlinking);
//    STIF_ASSERT_FALSE(iWatcher->iNotifManager->iUsbIconShown);
    
//    STIF_ASSERT_EQUALS(CUsbVBusObserver::EVBusDown, iWatcher->VBusObserver()->VBus());

    TearDown();

    aTestResult = ETestCasePassed;
    
    TRACE("<<CtUsbOtgWatcher::DeviceAttached_TooMuchRequiredL");
    }

void CtUsbOtgWatcher::DeviceAttached_SupportedDeviceL( TUsbOtgWatcherTestBlockParams& aParams,
        TUsbOtgWatcherTestResult& aTestResult )
    {
    TRACE(">>CtUsbOtgWatcher::DeviceAttached_SupportedDeviceL");
    
    SetupL();

    iStubber -> EnqueBehavior( TApiBehavior( KUsbManStubAgentDll, ETryStartAsync, 0, 0, _L8( "stub" ) ) );
    iStubber -> EnqueBehavior( TApiBehavior( KUsbManStubAgentDll, EServiceState, 0, 0, _L8( "EUsbServiceIdle" ) ) );
    iStubber -> EnqueBehavior( TApiBehavior( KUsbManStubAgentDll, EServiceState, 0, 0, _L8( "EUsbServiceStarted" ) ) );
    iStubber -> EnqueBehavior( TApiBehavior( KUsbManStubAgentDll, EServiceState, 0, 0, _L8( "EUsbServiceStarted" ) ) );
    iStubber -> EnqueBehavior( TApiBehavior( KUsbManStubAgentDll, EGetCurrentPersonalityId, 0, 0, _L8( "UsbPersonalityIdMTP" ) ) );
    iStubber -> EnqueBehavior( TApiBehavior( KUsbManStubAgentDll, EGetCurrentPersonalityId, 0, 0, _L8( "UsbPersonalityIdMTP" ) ) );
//    iStubber -> EnqueBehavior( TApiBehavior( KUsbManStubAgentDll, EServiceState, 0, 0, _L8( "EUsbServiceStarted" ) ) );
    
    iLog->Log(_L("PinOn triggered"));
    STIF_ASSERT_NOT_LEAVES(iWatcher->IdPinObserver()->TriggerPinOnL());
    iWatcher->iUsbServiceControl->RunL();
    iLog->Log(_L("Trigger vbus on"));
    STIF_ASSERT_NOT_LEAVES(iWatcher->VBusObserver()->TriggerVBusUpL());

    TDeviceEventInformation eventInfo;
    eventInfo.iEventType = EDeviceAttachment;
    eventInfo.iError = 0;
    iLog->Log(_L("Trigger device attached"));
    iWatcher->HostEventNotificationObserver()->TriggerDeviceAttachedL(eventInfo);

    eventInfo.iEventType = EDriverLoad;
    eventInfo.iError = 0;
    iLog->Log(_L("Trigger device driver loaded"));
    iWatcher->HostEventNotificationObserver()->TriggerDriverLoadSuccessL(eventInfo);
    STIF_ASSERT_TRUE(iWatcher->CurrentHostState() == iWatcher->iStates[EUsbStateHostAHost]);

//    STIF_ASSERT_FALSE(iWatcher->iNotifManager->iUsbIconBlinking);
//    
//    STIF_ASSERT_TRUE(iWatcher->iNotifManager->iUsbIconShown);

    STIF_ASSERT_EQUALS(CUsbVBusObserver::EVBusUp, iWatcher->VBusObserver()->VBus());
    
    TearDown();

    aTestResult = ETestCasePassed;

    TRACE("<<CtUsbOtgWatcher::DeviceAttached_SupportedDeviceL");
    }

void CtUsbOtgWatcher::DeviceDetached_SupportedDeviceL( TUsbOtgWatcherTestBlockParams& aParams,
        TUsbOtgWatcherTestResult& aTestResult )
    {
    TRACE(">>CtUsbOtgWatcher::DeviceDetached_SupportedDeviceL");
    
    SetupL();

    iStubber -> EnqueBehavior( TApiBehavior( KUsbManStubAgentDll, ETryStartAsync, 0, 0, _L8( "stub" ) ) );
    iStubber -> EnqueBehavior( TApiBehavior( KUsbManStubAgentDll, EServiceState, 0, 0, _L8( "EUsbServiceIdle" ) ) );
    iStubber -> EnqueBehavior( TApiBehavior( KUsbManStubAgentDll, EServiceState, 0, 0, _L8( "EUsbServiceStarted" ) ) );
    iStubber -> EnqueBehavior( TApiBehavior( KUsbManStubAgentDll, EServiceState, 0, 0, _L8( "EUsbServiceStarted" ) ) );
    iStubber -> EnqueBehavior( TApiBehavior( KUsbManStubAgentDll, EGetCurrentPersonalityId, 0, 0, _L8( "UsbPersonalityIdMTP" ) ) );
    iStubber -> EnqueBehavior( TApiBehavior( KUsbManStubAgentDll, EGetCurrentPersonalityId, 0, 0, _L8( "UsbPersonalityIdMTP" ) ) );
//    iStubber -> EnqueBehavior( TApiBehavior( KUsbManStubAgentDll, EServiceState, 0, 0, _L8( "EUsbServiceStarted" ) ) );
    
    TRACE( "PinOn triggered" );
    STIF_ASSERT_NOT_LEAVES(iWatcher->IdPinObserver()->TriggerPinOnL());
    iWatcher->iUsbServiceControl->RunL();
    TRACE( "Trigger vbus on" );
    STIF_ASSERT_NOT_LEAVES(iWatcher->VBusObserver()->TriggerVBusUpL());

    TDeviceEventInformation eventInfo;
    eventInfo.iEventType = EDeviceAttachment;
    eventInfo.iError = 0;
    TRACE( "Trigger device attached" );
    iWatcher->HostEventNotificationObserver()->TriggerDeviceAttachedL(eventInfo);

    eventInfo.iEventType = EDriverLoad;
    eventInfo.iError = 0;
    TRACE( "Trigger device driver loaded" );
    iWatcher->HostEventNotificationObserver()->TriggerDriverLoadSuccessL(eventInfo);

    eventInfo.iEventType = EDeviceDetachment;
    eventInfo.iError = 0;
    TRACE( "Trigger device detached" );
    iWatcher->HostEventNotificationObserver()->TriggerDeviceDetachedL(eventInfo);
    
    STIF_ASSERT_TRUE(iWatcher->CurrentHostState() == iWatcher->iStates[EUsbStateHostAInitiate]);

//    STIF_ASSERT_TRUE(iWatcher->iNotifManager->iUsbIconBlinking);

    STIF_ASSERT_EQUALS(CUsbVBusObserver::EVBusUp, iWatcher->VBusObserver()->VBus());

    TearDown();

    aTestResult = ETestCasePassed;

    TRACE("<<CtUsbOtgWatcher::DeviceDetached_SupportedDeviceL");
    }

void CtUsbOtgWatcher::IndicatorNotifier_IndicatorActivateL( TUsbOtgWatcherTestBlockParams& aParams,
                            TUsbOtgWatcherTestResult& aTestResult )
    {
    TRACE(">> IndicatorNotifier_IndicatorActivateL"); 
     
    SetupL();
    
    CUsbNotifManager* notifManager = CUsbNotifManager::NewL(*iWatcher);
    CleanupStack::PushL(notifManager);
    CUsbIndicatorNotifier* notifier = CUsbIndicatorNotifier::NewL(*notifManager, *iWatcher);
    CleanupStack::PushL(notifier);        
    
    notifier->ToggleConnectingIndicator(ETrue);
    
    STIF_ASSERT_TRUE(notifier->iConnectingIndicatorOn);
                                
    CleanupStack::PopAndDestroy(2);         
    
    TearDown();
      
    aTestResult = ETestCasePassed;                         
    TRACE("<< IndicatorNotifier_IndicatorActivateL");                                   
    }

// Add other member functions implementation here

void CtUsbOtgWatcher::SetupL()
    {
    TRACE_FUNC_ENTRY;
    
    TRACE( "UsbMan initialization" );
    iUsbMan = RUsb::NewL();
    User::LeaveIfNull(iUsbMan);
    TInt err; 
    
    TUsbServiceState serviceState;
    //set service state
    iUsbMan->SetUsbServiceState(EUsbServiceIdle);
    
    err = iUsbMan->GetServiceState(serviceState);
    TRACE_INFO( "UsbMan.GetServiceState err: %d", err );
    User::LeaveIfError(err);

    TRACE_INFO( "UsbMan.GetServiceState serviceState: %d, exspected: %d", serviceState, EUsbServiceIdle );
    if(serviceState != EUsbServiceIdle)
        {
        TRACE("UsbMan ServiceState didn't set properly!");
        User::Leave(KErrNotFound);
        }
   
    TRACE( "UsbOtgWatcher initialization" );
    iWatcher = CUsbOtgWatcher::NewL(*iUsbMan);
    if( !iWatcher )
        {
        TRACE("CUsbOtgWatcher::NewL Failed!");
        User::LeaveIfNull(iWatcher);
        }    
    
    iWatcher->VBusObserver()->SetRUsb(iUsbMan);
    
    TRACE_FUNC_EXIT;
    }

void CtUsbOtgWatcher::TearDown()
    {
    TRACE_FUNC_ENTRY;
    
    TRACE("delete iWatcher");
    if (iWatcher)
        delete iWatcher;
    
    TRACE("delete iUsbMan");
    if (iUsbMan)
        delete iUsbMan;
    
    TRACE_FUNC_EXIT;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
