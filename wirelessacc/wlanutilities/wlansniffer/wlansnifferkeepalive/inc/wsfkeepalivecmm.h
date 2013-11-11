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
 * Connection Method Manager interface implementation.
 */

#ifndef WSFKEEPALIVECMM_H
#define WSFKEEPALIVECMM_H

// System includes
 
#include <e32base.h>
#include <cmmanager.h>

// User includes

// Forward declarations

class CWsfKeepalive;

// External data types

// Global function prototypes
 
// Constants

// Class declaration

/**
 *  Connection Method Manager interface implementation.
 * 
 *  This class uses the Connection Method Manager interface to get and deleted
 *  Hotspot IAP.
 */
class CWsfKeepaliveCmm : public CBase
    {
    // Friend classes:

public:

    // Data types

    /** Constructors */
    
    /**
     * Two-phased constructor.
     * 
     */
    static CWsfKeepaliveCmm* NewL();
    
    /**> Destructor */
    virtual ~CWsfKeepaliveCmm();

public:

    /**
     * Cm Manager Hotspot info getter.
     */
    TBool GetHotspotInfoL(TInt32 aIapId);

    /**
     * Cm Manager Hotspot IAP deleter.
     */
    TBool DeleteHotspotIapL(TInt32 aIapId);

protected:
    
private:
    
    CWsfKeepaliveCmm();

    void ConstructL();
    
    
private: // Data
    
    /**
     * Handle to Connection Method Manger
     */
    RCmManager iCmManager;

    };

#endif // WSFKEEPALIVECMM_H
