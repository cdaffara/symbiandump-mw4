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

#ifndef C_USBMESSAGENOTIFICATIONOBSERVER_H
#define C_USBMESSAGENOTIFICATIONOBSERVER_H

#include <e32base.h>

#include "tassertbase.h"

class RUsb;

class MUsbMessageNotificationObserver
    {
public:

    virtual void MessageNotificationReceivedL(TInt) = 0;
    virtual void BadHubPositionL() = 0;
    virtual void VBusErrorL() = 0;
    virtual void SrpReceivedL() = 0;
    virtual void SessionRequestedL() = 0;
    };

/**
 *  Class observes USB Notifications
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CUsbMessageNotificationObserver : public TAssertBase
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
    static CUsbMessageNotificationObserver* NewL(RUsb* aUsb);

    /**
     * Destructor.
     */
    virtual ~CUsbMessageNotificationObserver();

    void SubscribeL(MUsbMessageNotificationObserver&);

    void UnsubscribeL(MUsbMessageNotificationObserver&);

    void TriggerMessageNotificationReceivedL(TInt);
    void TriggerBadHubPositionL();
    void TriggerVBusErrorL();
    void TriggerSrpReceivedL();
    void TriggerSessionRequestedL();

private:

    CUsbMessageNotificationObserver(RUsb* aUsb);

    void ConstructL();

private:
    // data

    // The observer reports to own observers
    // Owns
    MUsbMessageNotificationObserver* iObserver;

    // Notifications come from USB
    // Not own
    RUsb* iUsb;

    // Info from notification comes to this member
    TInt iMessage;

    };

#endif //  C_USBMESSAGENOTIFICATIONOBSERVER_H
