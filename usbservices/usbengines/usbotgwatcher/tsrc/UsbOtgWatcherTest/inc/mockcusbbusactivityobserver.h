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

#ifndef C_USBBUSACTIVITYOBSERVER_H
#define C_USBBUSACTIVITYOBSERVER_H

#include <e32base.h>

#include "tassertbase.h"

class MUsbBusActivityObserver
    {
public:

    virtual void BusIdleL() = 0;
    virtual void BusActiveL() = 0;
    };

/**
 *  Class observes OTG State property, see usbotgdefs.h
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CUsbBusActivityObserver : public TAssertBase
    {
    friend class CtUsbOtgWatcher;
    friend class CUsbOtgWatcherTest;

public:

    enum TBusActivity
        {
        EBusIdle = 0,
        EBusActive = 1
        };

    /**
     * Two-phased constructor.
     * @param ?arg1 ?description
     * @param ?arg2 ?description
     */
    //    ?IMPORT_C 
    static CUsbBusActivityObserver* NewL();

    /**
     * Destructor.
     */
    virtual ~CUsbBusActivityObserver();

    /**
     * ?description
     *
     * @since S60 ?S60_version
     * @param ?arg1 ?description
     * @param ?arg2 ?description
     * @return ?description
     */
    //    ?IMPORT_C 
    TBusActivity BusActivity();

    /**
     * ?description
     *
     * @since S60 ?S60_version
     * @param ?arg1 ?description
     * @param ?arg2 ?description
     * @return ?description
     */
    void SubscribeL(MUsbBusActivityObserver&);

    void UnsubscribeL(MUsbBusActivityObserver&);

    void TriggerBusIdleL();

    void TriggerBusActiveL();

private:

    CUsbBusActivityObserver();

    void ConstructL();

private:
    // data

    // The observer reports state changes to own observers
    // Owns
    MUsbBusActivityObserver* iObserver;

    TInt iBusActivity;
    };

#endif //  C_USBOTGSTATEOBSERVER_H
