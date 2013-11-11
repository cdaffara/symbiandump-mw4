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

#ifndef C_USBSTATEHOSTAPERIPHERAL_H
#define C_USBSTATEHOSTAPERIPHERAL_H

#include "cusbstatehostabase.h"

class CUsbOtgWatcher;

/**
 *  This class implements behaviour when A device performs peripheral role, from OTG point of view
 *
 */
NONSHARABLE_CLASS( CUsbStateHostAPeripheral ) : public CUsbStateHostABase
    {

public:

    /**
     * Two-phased constructor.
     * @param aWatcher owner
     */
    static CUsbStateHostAPeripheral* NewL(CUsbOtgWatcher& aWatcher);

    /**
     * Destructor.
     */
    virtual ~CUsbStateHostAPeripheral();

private:

    /**
     * default constructor
     * @param aWatcher owner
     */
    CUsbStateHostAPeripheral(CUsbOtgWatcher& aWatcher);

    /**
     * 2nd phase construction
     */
    void ConstructL();

    //from CUsbState
    /**
     * state id
     * @return state id
     */
    TUsbStateIds Id();

    // From OTG state observer
    /**
     * Became a host when A
     */
    void AHostL();
    
    /**
     * Became to Idle when A
     */
    void AIdleL();

private:
    // data

    };

#endif //  C_USBSTATEHOSTAPERIPHERAL_H
