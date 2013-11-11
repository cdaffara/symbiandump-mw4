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
 * Description:  Implementation of the conctet state
 *
 */

#ifndef C_USBSTATEHOSTABASE_H
#define C_USBSTATEHOSTABASE_H

#include "cusbstate.h"
#include "definitions.h"

class CUsbOtgWatcher;
/**
 *  This class implements basic behaviour for states in Host role
 */
NONSHARABLE_CLASS( CUsbStateHostABase ) : public CUsbState
    {

public:

    /**
     * Destructor.
     */
    virtual ~CUsbStateHostABase();

protected:
    /**
     * default construction
     * @param aWatcher owner
     */
    CUsbStateHostABase(CUsbOtgWatcher& aWatcher);

    /**
     * second phase construction
     */
    void ConstructL();

    // From VBus observer
    /**
     * VBus down
     */
    virtual void VBusDownL();

    // From OTG state observer
    /**
     * Became to Idle when A
     */
    virtual void AIdleL();

    /**
     * VBus error happen
     */
    virtual void AVBusErrorL();

    /**
     * Become A host
     */
    virtual void AHostL();

    /**
     * become A peripheral
     */
    virtual void APeripheralL();

    // From bus activity observer
    /**
     * Bus is in idle
     */
    virtual void BusIdleL();
    /**
     * Bus active
     */
    virtual void BusActiveL();

    // From message notification observer
    /**
     * Message received
     * @param aMessage message id
     */
    virtual void MessageNotificationReceivedL(TInt aMessage);

    /**
     * SRP request received
     */
    virtual void SrpReceivedL();

    /**
     * Connected to hub in wrong level 
     */
    virtual void BadHubPositionL();

    /**
     * VBus error happened
     */
    virtual void VBusErrorL();

    /**
     * Session request received
     */
    virtual void SessionRequestedL();

    };

#endif //  C_USBSTATEHOSTABASE_H
