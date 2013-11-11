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

#ifndef C_USBHOSTEVENTNOTIFICATIONOBSERVER_H
#define C_USBHOSTEVENTNOTIFICATIONOBSERVER_H

#include <e32base.h>
#include <usbhostdefs.h>

#include <StifTestModule.h>
#include <StifLogger.h>

#include "tassertbase.h"

class RUsb;

class MUsbHostEventNotificationObserver
    {
public:

    virtual void DeviceAttachedL(TDeviceEventInformation) = 0;
    virtual void DeviceDetachedL(TDeviceEventInformation) = 0;
    virtual void DriverLoadSuccessL(TDeviceEventInformation) = 0;
    virtual void DriverLoadPartialSuccessL(TDeviceEventInformation) = 0;
    virtual void DriverLoadFailureL(TDeviceEventInformation) = 0;

    };

/**
 *  Class observes USB Notifications
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CUsbHostEventNotificationObserver : public TAssertBase
    {
    friend class CtUsbOtgWatcher;
    friend class CUsbOtgWatcherTest;

public:

    /**
     * Two-phased constructor.
     * @param ?arg1 ?description
     * @param ?arg2 ?description
     */
    //    ?IMPORT_C 
    static CUsbHostEventNotificationObserver* NewL(RUsb* aUsb);

    /**
     * Destructor.
     */
    virtual ~CUsbHostEventNotificationObserver();

    void SubscribeL(MUsbHostEventNotificationObserver&);

    void UnsubscribeL(MUsbHostEventNotificationObserver&);

    void TriggerDeviceAttachedL(TDeviceEventInformation eventInfo);
    void TriggerDeviceDetachedL(TDeviceEventInformation eventInfo);
    void TriggerDriverLoadSuccessL(TDeviceEventInformation eventInfo);
    void TriggerDriverLoadPartialSuccessL(TDeviceEventInformation eventInfo);
    void TriggerDriverLoadFailureL(TDeviceEventInformation eventInfo);

private:

    CUsbHostEventNotificationObserver(RUsb* aUsb);

    void ConstructL();

private:
    // data

    // The observer reports changes to own observers
    // Owns
    MUsbHostEventNotificationObserver* iObserver;

    // Notifications come from USB
    // Not own
    RUsb* iUsb;
    };

#endif //  C_USBHOSTEVENTNOTIFICATIONOBSERVER_H
