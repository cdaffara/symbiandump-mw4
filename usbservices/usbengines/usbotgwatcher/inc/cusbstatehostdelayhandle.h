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
 * Description:  Implements concrete state
 *
 */

#ifndef C_USBSTATEHOSTDELAYHANDLE_H
#define C_USBSTATEHOSTDELAYHANDLE_H

#include "cusbstatehosthandle.h"

class CUsbOtgWatcher;

/**
 *  problem handling, vbus not dropping
 *
 */
NONSHARABLE_CLASS( CUsbStateHostDelayHandle ) : public CUsbStateHostHandle
    {
    friend class CtUsbOtgWatcher;
    friend class CUsbOtgWatcher;

public:

    /**
     * Destructor.
     */
    virtual ~CUsbStateHostDelayHandle();

protected:

    // From VBus observer
    /**
     * VBus up
     */
    virtual void VBusUpL();

    // From Host Event notification observer
    /**
     * Device is attached
     * @param aInfo Device event data
     */
    virtual void DeviceAttachedL(TDeviceEventInformation aInfo);

    /**
     * Default constructor
     * @param aWatcher owner
     */
    CUsbStateHostDelayHandle(CUsbOtgWatcher& aWatcher);

    /**
     * 2nd phase construction
     */
    void ConstructL();

    // data

private:
    };

#endif //  C_USBSTATEHOSTDELAYHANDLE_H
