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
 * Description:  Usb warning notifier implementation
 *
 */

#ifndef C_USBWARNINGNOTIFIER_H
#define C_USBWARNINGNOTIFIER_H

#include <e32base.h>

#include "cusbnotenotifier.h"
#include "cusbtimer.h"

/**
 *  Class implements functionality of the notifier which wait for answer from user
 *	Class does not provide method to get response from the user
 */
NONSHARABLE_CLASS( CUsbWarningNotifier ) : public CUsbNoteNotifier
    {
public:

    /**
     * Two-phased constructor.
     * @param aNotifier RNotifier API
     * @param aNotifManager Owner of the class, will destroy the object when needed
     * @param aNotifId Id of the notifier
     * @return Pointer to the new instance of CUsbWarningNotifier
     */
    static CUsbWarningNotifier* NewL(RNotifier& aNotifier,
            CUsbNotifManager& aNotifManager, TUint aNotifId);

    /**
     * Destructor.
     */
    virtual ~CUsbWarningNotifier();

private:

    /**
     * Default constructor
     * @param aNotifier RNotifier API
     * @param aNotifManager Owner of the class, will destroy the object when needed
     * @param aNotifId Id of the notifier
     */
    CUsbWarningNotifier(RNotifier& aNotifier,
            CUsbNotifManager& aNotifManager, TUint aNotifId);

    /**
     * 2nd phase construction
     */
    void ConstructL();

private:
    // Data

    };

#endif //  C_USBWARNINGNOTIFIER_H
