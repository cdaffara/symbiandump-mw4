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

#include <e32base.h>
#include <NormalHardcodedAssert.h>

#include <stubber.h>
#include <usbmandll_stub.h>
#include <usbpersonalityids.h>

#include "mockusbman.h"
#include "stifassertmacros.h"

#include "mockcusbvbusobserver.h"
#include "debug.h"

// ======== MEMBER FUNCTIONS ========

RUsb* RUsb::NewL()
    {
    LOG(  ( " MockRUsb::NewL" ) );
    RUsb* self = new ( ELeave ) RUsb();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // pop self
    return self;
    }

void RUsb::ConstructL()
    {
    
    }

// ---------------------------------------------------------------------------
// From class MUsb
// ?implementation_description
// ---------------------------------------------------------------------------
//
RUsb::RUsb() : 
    iBusCleared(EFalse),
    iBusRequested(EFalse),
    iServiceStateNotificationRequested(EFalse),
    iTryStartError(KErrNone), 
    iBusRequestError(KErrNone),
    iCurrentPersonalityId(0),
    iUsbServiceState(EUsbServiceIdle)
    {
    LOG1 (" MockRUsb::RUsb: %d", iUsbServiceState );
    }

RUsb::~RUsb()
    {
    }

// ---------------------------------------------------------------------------
// From class MUsb
// ?implementation_description
// ---------------------------------------------------------------------------
//
void RUsb::TryStart(TInt aPersonalityId, TRequestStatus& aStatus)
    {
    LOG( (" MockRUsb::TryStart"));

//    CStubber* stubber = CStubber::NewL();
//    TApiBehavior beh( KUsbManStubAgentDll, ETryStartAsync, 0, 0, KNullDesC8 );
//    stubber -> InvokeApi( beh );
//
//    delete stubber;
//    stubber = NULL;
//    
//    if ( beh.iOutput != KNullDesC8 )
//        {            
        TRequestStatus* status = &aStatus;
        User::RequestComplete(status, iTryStartError);
//        }
    }

void RUsb::StartCancel(TRequestStatus& aStatus)
    {
    LOG( (" MockRUsb::StartCancel"));

    TRequestStatus* status = &aStatus;
    User::RequestComplete(status, iTryStartError);
    
    }

// ---------------------------------------------------------------------------
// From class MUsb
// ?implementation_description
// ---------------------------------------------------------------------------
//
void RUsb::TryStop(TRequestStatus& aStatus)
    {
    LOG( (" MockRUsb::TryStop"));
        
//    CStubber* stubber = CStubber::NewL();
//    TApiBehavior beh( KUsbManStubAgentDll, ETryStartAsync, 0, 0, KNullDesC8 );
//    stubber -> InvokeApi( beh );
//
//    delete stubber;
//    stubber = NULL;
//    
//    if ( beh.iOutput != KNullDesC8 )
//        {            
        TRequestStatus* status = &aStatus;
        User::RequestComplete(status, KErrNone);
//        }
    }

// ---------------------------------------------------------------------------
// From class MUsb
// ?implementation_description
// ---------------------------------------------------------------------------
//
void RUsb::MessageNotification(TRequestStatus& aStatus, TInt& aMessage)
    {
    LOG( (" MockRUsb::MessageNotification"));

    }

// ---------------------------------------------------------------------------
// From class MUsb
// ?implementation_description
// ---------------------------------------------------------------------------
//
void RUsb::MessageNotificationCancel()
    {
    LOG( (" MockRUsb::MessageNotificationCancel"));
    
    }

// ---------------------------------------------------------------------------
// From class MUsb
// ?implementation_description
// ---------------------------------------------------------------------------
//
void RUsb::HostEventNotification(TRequestStatus& aStatus,
        TDeviceEventInformation& aDeviceInformation)
    {
    LOG( (" MockRUsb::HostEventNotification"));

    }

// ---------------------------------------------------------------------------
// From class MUsb
// ?implementation_description
// ---------------------------------------------------------------------------
//
void RUsb::HostEventNotificationCancel()
    {
    LOG( (" MockRUsb::HostEventNotificationCancel"));
    
    }

// ---------------------------------------------------------------------------
// From class MUsb
// ?implementation_description
// ---------------------------------------------------------------------------
//
TInt RUsb::EnableFunctionDriverLoading()
    {
    LOG( (" MockRUsb::EnableFunctionDriverLoading"));

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// From class MUsb
// ?implementation_description
// ---------------------------------------------------------------------------
//
void RUsb::DisableFunctionDriverLoading()
    {
    LOG( (" MockRUsb::DisableFunctionDriverLoading"));

    }

// ---------------------------------------------------------------------------
// From class MUsb
// ?implementation_description
// ---------------------------------------------------------------------------
//
TInt RUsb::BusRequest()
    {
    LOG1 (" MockRUsb::BusRequest, ret=%d", iBusRequestError );

    iBusRequested = ETrue;
    
    return iBusRequestError;
    }

// ---------------------------------------------------------------------------
// From class MUsb
// ?implementation_description
// ---------------------------------------------------------------------------
//
TInt RUsb::BusRespondSrp()
    {
    LOG( (" MockRUsb::BusRespondSrp"));
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// From class MUsb
// ?implementation_description
// ---------------------------------------------------------------------------
//
TInt RUsb::BusClearError()
    {
    LOG( (" MockRUsb::BusClearError"));
    iBusCleared = ETrue;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// From class MUsb
// ?implementation_description
// ---------------------------------------------------------------------------
//
TInt RUsb::BusDrop()
    {
    LOG( (" MockRUsb::BusDrop"));

    iBusRequested = EFalse;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// From class RSessionBase
// ?implementation_description
// ---------------------------------------------------------------------------
//
void RUsb::Close()
    {
    LOG( (" MockRUsb::Close"));

    }

void RUsb::SetTryStartError(TInt aError)
    {
    iTryStartError = aError;
    }

void RUsb::SetBusRequestError(TInt aError)
    {
    iBusRequestError = aError;
    }

void RUsb::SetUsbServiceState(TUsbServiceState aUsbServiceState)
    {
    LOG2 (" MockRUsb::SetUsbServiceState: %d, %d", iUsbServiceState, aUsbServiceState );
    iUsbServiceState = aUsbServiceState;
    }

TBool RUsb::GetVBusUp()
    {
    LOG2 (" MockRUsb::GetVBusUp, iBusRequested=%d, iBusRequestError=%d", iBusRequested , iBusRequestError);

    return iBusRequested && !iBusRequestError;
    }

TInt RUsb::GetServiceState(TUsbServiceState& aState)
    {
    LOG( (" MockRUsb::GetServiceState"));
    aState = iUsbServiceState;

    CStubber* stubber = CStubber::NewL();
    TApiBehavior beh( KUsbManStubAgentDll, EServiceState, 0, 0, KNullDesC8 );
    stubber -> InvokeApi( beh );

    delete stubber;
    stubber = NULL;
    
    if ( beh.iOutput != KNullDesC8 )
        {                        
        if ( !beh.iOutput.Compare( _L8( "EUsbServiceIdle" ) ) )
            aState = EUsbServiceIdle;
        else if ( !beh.iOutput.Compare( _L8( "EUsbServiceStarting" ) ) )
            aState = EUsbServiceStarting;
        else if ( !beh.iOutput.Compare( _L8( "EUsbServiceStarted" ) ) )
            aState = EUsbServiceStarted;
        else if ( !beh.iOutput.Compare( _L8( "EUsbServiceStopping" ) ) )
            aState = EUsbServiceStopping;
        else if ( !beh.iOutput.Compare( _L8( "EUsbServiceFatalError" ) ) )
            aState = EUsbServiceFatalError;
        }
    
    LOG1 (" iUsbServiceState: %d", aState);
    
    return KErrNone;
    }
    
void RUsb::ServiceStateNotification(TUsbServiceState &aState, TRequestStatus &aStatus)
{
	LOG( (" MockRUsb::ServiceStateNotification"));
	if(iServiceStateNotificationRequested)
	{
		TRequestStatus* status = &aStatus;
    User::RequestComplete(status, KErrGeneral);  // already requested
		return;
	}
	
	iServiceStateNotificationRequested = ETrue;
	TRequestStatus* status = &aStatus;
  User::RequestComplete(status, KErrNone); 
}
    
TInt RUsb::GetCurrentPersonalityId(TInt &aPersonalityId)
{
	LOG( (" MockRUsb::GetCurrentPersonalityId"));
	
	CStubber* stubber = CStubber::NewL();
	TApiBehavior beh( KUsbManStubAgentDll, EGetCurrentPersonalityId, 0, 0, KNullDesC8 );
	stubber -> InvokeApi( beh );

	delete stubber;
	stubber = NULL;
	
	if ( beh.iOutput != KNullDesC8 )
		{                        
		if ( !beh.iOutput.Compare( _L8( "UsbPersonalityIdPCSuite" ) ) )
			aPersonalityId = KUsbPersonalityIdPCSuite;
		else if ( !beh.iOutput.Compare( _L8( "UsbPersonalityIdMS" ) ) )
			aPersonalityId = KUsbPersonalityIdMS;
		else if ( !beh.iOutput.Compare( _L8( "UsbPersonalityIdPTP" ) ) )
			aPersonalityId = KUsbPersonalityIdPTP;
		else if ( !beh.iOutput.Compare( _L8( "UsbPersonalityIdMTP" ) ) )
			aPersonalityId = KUsbPersonalityIdMTP;
		else if ( !beh.iOutput.Compare( _L8( "UsbPersonalityIdPCSuiteMTP" ) ) )
			aPersonalityId = KUsbPersonalityIdPCSuiteMTP;

	    LOG1( " MockRUsb::GetCurrentPersonalityId iStubbedPersonalityId: %d", aPersonalityId);
		}
	else
		aPersonalityId = iCurrentPersonalityId;
	
	LOG1( " MockRUsb::GetCurrentPersonalityId iCurrentPersonalityId: %d", iCurrentPersonalityId);
	
	return KErrNone;
}
    
TInt RUsb::CancelInterest(TUsbReqType aMessageId) // to pending op. of usb service state
{
	LOG( (" MockRUsb::CancelInterest"));
	iServiceStateNotificationRequested = EFalse;
	return KErrNone;
}
    
void RUsb::ServiceStateNotificationCancel()
{
	LOG( (" MockRUsb::ServiceStateNotificationCancel"));
	iServiceStateNotificationRequested = EFalse;
}
    
void RUsb::SetCurrentPersonality(TInt aPersonalityId)
{
	LOG( (" MockRUsb::SetCurrentPersonality"));
	iCurrentPersonalityId = aPersonalityId;
}

TInt RUsb::GetOtgDescriptor(TUint aDeviceId, TOtgDescriptor& aDescriptor)
{
	LOG( (" MockRUsb::GetOtgDescriptor"));
	
	aDescriptor.iAttributes = 1;
	aDescriptor.iDeviceId = 2;
	
	return KErrNone;
}

// End of File
