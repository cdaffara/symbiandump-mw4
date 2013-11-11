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
* Main implementation of Wlan Sniffer Keepalive application.
*/

#ifndef WSFKEEPALIVE_H
#define WSFKEEPALIVE_H

// System includes
 
// User includes

#include "wsfkeepalivetimer.h"
 
// Forward declarations

class CWsfKeepaliveCmm;
class CWsfKeepaliveConnMon;
class CWsfKeepaliveConnMonDisc;
class CWsfKeepaliveEsock;
 
// External data types

// Global function prototypes
 
// Constants

// Class declaration

/**
 *  Main implementation of Wlan Sniffer Keepalive application.
 *  
 *  This class contains the actual application logic, which monitors
 *  Wlan connections and keeps them alive, if needed.
 */
class CWsfKeepalive : public CBase, public MWsfKeepaliveTimerNotify
    {
    // Friend classes:
    friend class CWsfKeepaliveTestSuite;

public:
    
    // Data types

    // Constants
    
    /** Invalid/not available connection ID */
    static const TUint KInvalidConnectionId = 0;

    /** Constructors */
    
    /**
     * Two-phased constructor.
     * 
     * @return New object allocated from heap
     */
    static CWsfKeepalive* NewL();
    
    /**> Destructor */
    virtual ~CWsfKeepalive();

public:
    
    /**
     * Timer expiration handler. Disconnects the monitored connection,
     * if needed. Restarts timer, if needed.
     *
     * @param aError Status of the timer event
     */
    void TimerExpired( TInt aError );

    /**
     * Wlan connection event handler. Shares and starts to monitor the
     * opened connection, if necessary.
     *
     * @param aConnectionId ID of the opened connection
     * @param aIapId IAP ID of the opened connection
     */
    void WlanConnectionOpenedL( TUint aConnectionId, TUint aIapId );
    
    /**
     * Wlan connection event handler. Stops monitoring the connection.
     */
    void WlanConnectionClosed();

protected:
    
private:
    
    CWsfKeepalive();

    void ConstructL();
    
    TBool OpenedByWlanSniffer( TUint aConnectionId );
    
    TBool NoRealClients( TUint aConnectionId );
    
    void SetState( TUint aState );
        
private: // Data

    // Constants

    /**
     * State values for the keepalive implementation
     */
    enum
        {
        /** Not monitoring any connection */
        EInactive,
        /** Monitoring connection, which has no real clients */
        EActiveNoClients,
        /** Monitoring connection, which has one or more real clients */
        EActiveWithClients
        };
    
    /**
     * Timer for polling users of a connection
     */
    CWsfKeepaliveTimer* iTimer;

    /**
     * Connection Method Manager class reference
     */
    CWsfKeepaliveCmm* iCmMgr;
    
    /**
     * Connection Monitor class reference
     */
    CWsfKeepaliveConnMon* iConnMon;

    /**
     * Connection Disconnector class reference
     */
    CWsfKeepaliveConnMonDisc* iConnMonDisc;
    
    /**
     * RConnection handler class reference
     */
    CWsfKeepaliveEsock* iEsock;
    
    /**
     * Time when keepalive started for monitored connection
     */
    TTime iKeepaliveStart;
    
    /**
     * ID of monitored connection, KInvalidConnectionId if not valid
     */
    TUint iConnectionId;

    /**
     * State of the keepalive implementation
     */
    TUint iState;
    
    /**
     * IapId of the connection
     */
    TUint iIapId;
    };

#endif // WSFKEEPALIVE_H
