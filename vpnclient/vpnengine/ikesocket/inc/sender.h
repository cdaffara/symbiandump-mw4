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
* Description:  Sender for UDP data
*
*/


#ifndef C_SENDER_H
#define C_SENDER_H

#include <e32base.h>
#include <in_sock.h>

// FORWARD DECLARATIONS

class MIkeDebug;

/**
 *  Sender callback interface.
 *
 *  Callback interface for informing completion of sending.
 *
 *  @lib ikesocket.lib
 */
NONSHARABLE_CLASS( MSenderCallback )
    {
public:
    /**
     * Notifies about completed sending.
     * 
     * @param aStatus Completion status
     */    
    virtual void SendCompleted( const TInt aStatus ) = 0;
    };

/**
 *  Sender of UDP data.
 *
 *  This class provides functionality for sending UDP data. Completion of
 *  sending is notified via MSenderCallback callback interface.
 *  
 *  @lib ikesocket.lib
 */
NONSHARABLE_CLASS( CSender ) : public CActive
    {
public:
    /**
     * Two-phased constructor.
     * @param aSocket Socket for local port 500
     * @param aSocketNAT Socket for local port 4500
     * @param aSocketNokiaNAT Socket for Nokia NAT port
     * @param aCallback Callback interface
     * @param aDebug Debug trace interface
     */
    static CSender* NewL( RSocket& aSocket,
                          RSocket& aSocketNAT,
                          RSocket& aSocketNokiaNAT,
                          MSenderCallback& aCallback,
                          MIkeDebug& aDebug );
    
    /**
     * Destructor.
     */
    ~CSender();
    
    /**
     * Sends UDP data.
     *
     * @param aLocalPort Local port
     * @param aDestAddr Destination IP address/port
     * @param aUdpData UDP data
     * @param aDscp DSCP value
     * @return Error value
     */
    TInt SendUdpData( const TInt aLocalPort,
                      const TInetAddr& aDestAddr,
                      const TDesC8& aUdpData,
                      const TUint aDscp );
    
// from base class CActive
        
    /**
     * From CActive.
     * Handles an active object's request completion event about sending.
     */
    void RunL();
    
    /**
     * From CActive.
     * Implements cancellation of sending.
     */ 
    void DoCancel();    

private:

    CSender( RSocket& aSocket,
             RSocket& aSocketNAT,
             RSocket& aSocketNokiaNAT,
             MSenderCallback& aCallback,
             MIkeDebug& aDebug );
    
    void ConstructL();
    
    
private: // data    
    
    /**
     * Socket for port 500.
     * Not own.
     */
    RSocket& iSocket;
    
    /**
     * Socket for port 4500.
     * Not own.
     */
    RSocket& iSocketNAT;
    
    /**
     * Socket for Nokia NAT port.
     * Not own.
     */
    RSocket& iSocketNokiaNAT;
    
    /**
     * Local port used for sending.
     * Own.
     */
    TInt iLocalPort;
    
    /**
     * Destination address used for sending.
     * Own.
     */
    TInetAddr iDestAddr;

    /**
     * Sender callback for completing sending.
     * Not own.
     */
    MSenderCallback& iCallback;
    
    /**
     * Debug trace interface.
     * Not own.
     */
    MIkeDebug& iDebug;
    };

#endif // C_SENDER_H
