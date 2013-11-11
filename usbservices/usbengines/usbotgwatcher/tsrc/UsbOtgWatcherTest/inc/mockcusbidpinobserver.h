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

#ifndef MOCK_C_USBIDPINOBSERVER_H
#define MOCK_C_USBIDPINOBSERVER_H

#include <e32base.h>

#include <StifTestModule.h>
#include <StifLogger.h>
#include <e32property.h> 

#include "tassertbase.h"

class CStifLogger;

class MUsbIdPinObserver
    {
public:

    virtual void IdPinOffL() = 0;
    virtual void IdPinOnL() = 0;

    };

/**
 *  Class observes ID-PIN property
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CUsbIdPinObserver : public TAssertBase
    {
    friend class CtUsbOtgWatcher;
    friend class CUsbOtgWatcherTest;
public:
    enum TState
        {
        EIdPinOff = 0,
        EIdPinOn = 1
        };

    /**
     * Two-phased constructor.
     * @param ?arg1 ?description
     * @param ?arg2 ?description
     */
    //    ?IMPORT_C 
    static CUsbIdPinObserver* NewL();

    /**
     * Destructor.
     */
    virtual ~CUsbIdPinObserver();

    /**
     * ?description
     *
     * @since S60 ?S60_version
     * @param ?arg1 ?description
     * @param ?arg2 ?description
     * @return ?description
     */
    //    ?IMPORT_C 
    TState IdPin() /* const! read comment in impl*/;

    /**
     * ?description
     *
     * @since S60 ?S60_version
     * @param ?arg1 ?description
     * @param ?arg2 ?description
     * @return ?description
     */

    void SubscribeL(MUsbIdPinObserver&);

    void UnsubscribeL(MUsbIdPinObserver&);

    void TriggerPinOnL();

    void TriggerPinOffL();

private:

    CUsbIdPinObserver();

    void ConstructL();

private:
    // data
    // The observer reports state changes to its own observers
    // Own
    MUsbIdPinObserver* iObserver;
    TInt iIdPinOn;
    CActiveSchedulerWait * iWait;
    };

#endif //  MOCK_C_USBIDPINOBSERVER_H
