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

#ifndef C_USBVBUSOBSERVER_H
#define C_USBVBUSOBSERVER_H

#include <e32base.h>

#include <StifTestModule.h>
#include <StifLogger.h>

#include "tassertbase.h"

class RUsb;

class MUsbVBusObserver
    {
public:

    void virtual VBusDownL() = 0;
    void virtual VBusUpL() = 0;

    };

/**
 *  Class observes VBUS property
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CUsbVBusObserver : public TAssertBase
    {
    friend class CtUsbOtgWatcher;
    friend class CUsbOtgWatcherTest;

public:
    enum TState
        {
        EVBusDown = 0,
        EVBusUp = 1
        };

    /**
     * Two-phased constructor.
     * @param ?arg1 ?description
     * @param ?arg2 ?description
     */
    //    ?IMPORT_C 
    static CUsbVBusObserver* NewL();

    /**
     * Destructor.
     */
    virtual ~CUsbVBusObserver();

    /**
     * ?description
     *
     * @since S60 ?S60_version
     * @param ?arg1 ?description
     * @param ?arg2 ?description
     * @return ?description
     */
    //    ?IMPORT_C 
    TState VBus() /* const! read comment in impl*/;

    /**
     * ?description
     *
     * @since S60 ?S60_version
     * @param ?arg1 ?description
     * @param ?arg2 ?description
     * @return ?description
     */

    void SubscribeL(MUsbVBusObserver&);

    void UnsubscribeL(MUsbVBusObserver&);

    void SetRUsb(RUsb* aRUsb);
    
    void TriggerVBusUpL();
    
    void TriggerVBusDownL();
private:

    CUsbVBusObserver();

    void ConstructL();

private:
    // data
    RUsb* iUsb;

    // The observer reports state changes to own observers
    // Own
    MUsbVBusObserver* iObserver;

    };

#endif //  C_USBVBUSOBSERVER_H
