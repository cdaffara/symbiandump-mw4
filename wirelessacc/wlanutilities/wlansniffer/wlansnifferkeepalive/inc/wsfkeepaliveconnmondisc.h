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
 * Connection Monitor disconnect interface implementation.
 */

#ifndef WSFKEEPALIVECONNMONDISC_H
#define WSFKEEPALIVECONNMONDISC_H

// System includes
 
#include <e32base.h>

// User includes

// Forward declarations

class RConnectionMonitor;

// External data types

// Global function prototypes
 
// Constants

// Class declaration

/**
 *  Connection Monitor disconnect interface implementation.
 *  
 *  This class uses the Connection Monitor interface to disconnect
 *  connections regardless of the amount of users for the connection.
 */
class CWsfKeepaliveConnMonDisc : public CBase
    {
    // Friend classes:

public:

    // Data types

    /** Constructors */
    
    /**
     * Two-phased constructor.
     * 
     * @return New object allocated from heap
     */
    static CWsfKeepaliveConnMonDisc* NewL();
    
    /**> Destructor */
    virtual ~CWsfKeepaliveConnMonDisc();

public:

    /**
     * Disconnects the given connection regardless of the amount of
     * users for the connection.
     * 
     * @param aConnectionId ID of the connection to disconnect
     */
    void Disconnect( TUint aConnectionId );

protected:
    
private:

    CWsfKeepaliveConnMonDisc();

    void ConstructL();
    
private: // Data

    /**
     * Handle to Connection Monitor Server
     */
    RConnectionMonitor iMonitor;
    };

#endif // WSFKEEPALIVECONNMONDISC_H
