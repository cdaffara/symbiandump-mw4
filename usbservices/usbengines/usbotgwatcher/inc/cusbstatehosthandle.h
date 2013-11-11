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

#ifndef C_USBSTATEHOSTHANDLE_H
#define C_USBSTATEHOSTHANDLE_H

#include "cusbstatehostabase.h"

class CUsbOtgWatcher;

/**
 *  problem handling base class
 *
 */
NONSHARABLE_CLASS( CUsbStateHostHandle ) : public CUsbStateHostABase
    {
    friend class CtUsbOtgWatcher;
    friend class CUsbOtgWatcher;

public:

    /**
     * Destructor.
     */
    virtual ~CUsbStateHostHandle();

    /**
     * Sets id of the issue to be handled
     * @param aWhat issue id
     */
    void SetWhat(TInt aWhat);

protected:

    // From message notification observer
    /**
     * Message received
     * @param aMessage message id
     */
    virtual void MessageNotificationReceivedL(TInt aMessage);

    /**
     * Session request received
     */
    virtual void SessionRequestedL();

    /**
     * This is called when switched to this state, 
     * because If all conditions for transition to another state exist, nothing will 
     * trigger it and transition will not happen. This forces the transition in such cases.
     *
     */
    virtual void JustAdvancedToThisStateL();

    /**
     * handles issue
     */
    virtual void DoHandleL() = 0;

    /**
     * Default constructor
     * @param aWatcher owner
     */
    CUsbStateHostHandle(CUsbOtgWatcher& aWatcher);

    /**
     * 2nd phase construction
     */
    void ConstructL();

    // data

    /**
     * identifies situation to be handled
     */
    TInt iWhat;

private:
    };

#endif //  C_USBSTATEHOSTHANDLE_H
