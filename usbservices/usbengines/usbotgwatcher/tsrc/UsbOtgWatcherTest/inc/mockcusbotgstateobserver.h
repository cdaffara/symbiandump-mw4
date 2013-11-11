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

#ifndef C_USBOTGSTATEOBSERVER_H
#define C_USBOTGSTATEOBSERVER_H

#include <e32base.h>
#include <usbotgdefs.h>

#include <StifTestModule.h>
#include <StifLogger.h>

#include "tassertbase.h"

class MUsbOtgStateObserver
    {
public:

    virtual void AIdleL() = 0;
    virtual void AHostL() = 0;
    virtual void APeripheralL() = 0;
    virtual void AVBusErrorL() = 0;
    virtual void BIdleL() = 0;
    virtual void BPeripheralL() = 0;
    virtual void BHostL() = 0;
    };

/**
 *  Class observes OTG State property, see usbotgdefs.h
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CUsbOtgStateObserver : public TAssertBase
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
    static CUsbOtgStateObserver* NewL();

    /**
     * Destructor.
     */
    virtual ~CUsbOtgStateObserver();

    /**
     * ?description
     *
     * @since S60 ?S60_version
     * @param ?arg1 ?description
     * @param ?arg2 ?description
     * @return ?description
     */
    //    ?IMPORT_C 
    TUsbOtgState OtgState();

    /**
     * ?description
     *
     * @since S60 ?S60_version
     * @param ?arg1 ?description
     * @param ?arg2 ?description
     * @return ?description
     */
    void SubscribeL(MUsbOtgStateObserver&);

    void UnsubscribeL(MUsbOtgStateObserver&);

    void TriggerAIdleL();
    void TriggerAHostL();
    void TriggerAPeripheralL();
    void TriggerAVBusErrorL();
    void TriggerBIdleL();
    void TriggerBPeripheralL();
    void TriggerBHostL();

private:

    CUsbOtgStateObserver();

    void ConstructL();

private:
    // data

    // The observer reports state changes to own observers
    // Owns
    MUsbOtgStateObserver* iObserver;

    TInt iOtgState;
    };

#endif //  C_USBOTGSTATEOBSERVER_H
