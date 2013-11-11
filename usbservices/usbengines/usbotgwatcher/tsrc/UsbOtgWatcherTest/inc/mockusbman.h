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

#ifndef __USBMAN_H__
#define __USBMAN_H__

//#ifndef R_USB_H
//#define R_USB_H

#include <usbhostdefs.h>

#include <StifTestModule.h>
#include <StifLogger.h>

#include "tassertbase.h"
#include <usbstates.h>

//enum TUsbMockServiceState
//    {
//    EUsbServiceStateIdle = 0x01,
//    EUsbServiceStateStarting = 0x02,
//    EUsbServiceStateStarted = 0x04,
//    EUsbServiceStateStopping = 0x08,
//    EUsbServiceStateFatalError = 0x10
//    };
		
class RUsb : public TAssertBase
    {
    
	// Request types, the interest of which can be cancelled by clients
	enum TUsbReqType
		{
		EStart,
		EStop,
		ETryStart,
		ETryStop
		};
		
public:
    static RUsb* NewL();
      
    ~RUsb();
    // from base class MUsb
    /**
    */
    void TryStart(TInt aPersonalityId, TRequestStatus& aStatus);

    /**
    */
    void StartCancel(TRequestStatus& aStatus);

    /**
    */
    void TryStop(TRequestStatus& aStatus);

    /**
    */
    void MessageNotification(TRequestStatus& aStatus, TInt& aMessage);

    /**
    */
    void MessageNotificationCancel();

    /**
    */
    void HostEventNotification(TRequestStatus& aStatus,
                                        TDeviceEventInformation& aDeviceInformation);

    /**
    */
    void HostEventNotificationCancel();
    
    /**
    */
    TInt EnableFunctionDriverLoading();

    /**
    */
    void DisableFunctionDriverLoading();

    /**
    */
    TInt BusRequest();

    /**
    */
    TInt BusRespondSrp();

    /**
    */
    TInt BusClearError();

    /**
    */
    TInt BusDrop();
    
    void ServiceStateNotification(TUsbServiceState &aState, TRequestStatus &aStatus);
    
    TInt GetCurrentPersonalityId(TInt &aPersonalityId);
    
    TInt CancelInterest(TUsbReqType aMessageId); // to pending op. of usb service state
    
    void ServiceStateNotificationCancel();
    
    TInt GetOtgDescriptor(TUint aDeviceId, TOtgDescriptor& aDescriptor);
	
    // from base class RSessionBase
    /**
    */
    void Close();

    //Mock methods
    void SetTryStartError(TInt aError);
    void SetBusRequestError(TInt aError);
    void SetUsbServiceState(TUsbServiceState aUsbServiceState);
    void SetCurrentPersonality(TInt aPersonality);
    
    TBool GetVBusUp();
    
    TInt GetServiceState(TUsbServiceState& aState);
private:
    RUsb();
    
    void ConstructL();
    
private: // data
    TBool iBusCleared;
    TBool iBusRequested;
    TBool iServiceStateNotificationRequested;
    
    TInt iTryStartError;
    TInt iBusRequestError;
    TInt iCurrentPersonalityId;
    
    TUsbServiceState iUsbServiceState;
    };

#endif // __USBMAN_H__
