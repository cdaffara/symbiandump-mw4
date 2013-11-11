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
* Esock library (RConnection) interface implementation.
*/

#ifndef WSFKEEPALIVEESOCK_H
#define WSFKEEPALIVEESOCK_H

// System includes
 
#include <es_sock.h>

// User includes

// Forward declarations

// External data types

// Global function prototypes
 
// Constants

// Class declaration

/**
 *  Esock library (RConnection) interface implementation.
 * 
 *  This class implements RConnection opening and closing using the Esock
 *  library.
 */
class CWsfKeepaliveEsock : public CActive
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
    static CWsfKeepaliveEsock* NewL();
    
    /**> Destructor */
    virtual ~CWsfKeepaliveEsock();

public:
    
    /**
     * Starts connection creation.
     * 
     * @param aIapId ID of the IAP to connect
     */
    void ConnectL( TUint aIapId );
    
    /**
     * Stops connection.
     */
    void Disconnect();

private:
    
    CWsfKeepaliveEsock();

    void ConstructL();
    
    void RunL();
    
    void DoCancel();
        
private: // Data
    
    /**
     * Handle to Socket Server
     */
    RSocketServ iSocketServer;

    /**
     * Handle to RConnection object
     */
    RConnection iConnection;
};

#endif // WSFKEEPALIVEESOCK_H
