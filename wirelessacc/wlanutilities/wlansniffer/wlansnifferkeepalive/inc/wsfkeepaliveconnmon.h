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
 * Connection Monitor interface implementation.
 */

#ifndef WSFKEEPALIVECONNMON_H
#define WSFKEEPALIVECONNMON_H

// System includes
 
#include <e32base.h>
#include <rconnmon.h>

// User includes

// Forward declarations

class CWsfKeepalive;

// External data types

// Global function prototypes
 
// Constants

// Class declaration

/**
 *  Connection Monitor interface implementation.
 * 
 *  This class uses the Connection Monitor interface to monitor status
 *  of Wlan connections we might be interested in.
 */
class CWsfKeepaliveConnMon : public CBase, public MConnectionMonitorObserver
    {
    // Friend classes:

public:

    // Data types

    /** Constructors */
    
    /**
     * Two-phased constructor.
     * 
     * @param aNotify Status of Wlan connections is notified to this class
     * @return New object allocated from heap
     */
    static CWsfKeepaliveConnMon* NewL( CWsfKeepalive& aNotify );
    
    /**> Destructor */
    virtual ~CWsfKeepaliveConnMon();

public:

    /**
     * Connection Monitor connection client info getter.
     * 
     * @param aClientInfo Where to store information
     * @param aConnectionId ID of the connection, whose info is requested
     */
    void GetClientInfo(
        TConnMonClientEnumBuf& aClientInfo,
        TUint aConnectionId );

protected:
    
private:
    
    CWsfKeepaliveConnMon( CWsfKeepalive& aNotify );

    void ConstructL();
    
    void EventL( const CConnMonEventBase& aConnMonEvent );
    
private: // Data

    /**
     * Status of Wlan connections is notified to this class
     */
    CWsfKeepalive& iNotify;
    
    /**
     * Handle to Connection Monitor Server
     */
    RConnectionMonitor iMonitor;

    /**
     * Id of the connection we are currently monitoring
     */
    TUint iMonitoringConnectionId;
    };

#endif // WSFKEEPALIVECONNMON_H
