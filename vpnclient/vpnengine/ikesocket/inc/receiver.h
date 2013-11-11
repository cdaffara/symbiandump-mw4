/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Receiver for UDP data
*
*/


#ifndef C_RECEIVER_H
#define C_RECEIVER_H

#include <e32base.h>
#include <in_sock.h>
#include "ikesocketdefs.h"

// FORWARD DECLARATIONS
class MIkeDebug;

/**
 *  Receiver callback interface.
 *
 *  Callback interface for informing that data has been received or error
 *  has occured.
 *
 *  @lib ikesocket.lib
 */
NONSHARABLE_CLASS( MReceiverCallback )
    {
public:
    /**
     * Notifies that data has been received. Receiving is continued
     * automatically.
     * 
     * @param aUdpData Received UDP data. Ownership transferred.
     * @param aSrcAddr Source address
     * @param aLocalPort Local port
     */    
    virtual void DataReceived( HBufC8* aUdpData,
                                   const TInetAddr& aSrcAddr,
                                   const TInt aLocalPort ) = 0;
    
    /**
     * Notifies about receive error. Receiving has been stopped.
     * 
     * @param aStatus Error status
     */    
    virtual void ReceiveError( const TInt aStatus ) = 0;
    };

/**
 *  Receiver of UDP data.
 *
 *  This class provides functionality for receiving UDP data from specified
 *  socket (RSocket). Notification that data has been received and unhandled
 *  errors in receiving are notified via MReceiverCallback callback interface.
 *  Received data will be ignored, if data does not correspond to specified
 *  IKE major version.
 *  
 *  @lib ikesocket.lib
 */
NONSHARABLE_CLASS( CReceiver ) : private CTimer
    {
public:
    /**
     * Two-phased constructor.
     * @param aSocket Socket which is used for receiving
     * @param aCallback Callback interface
     * @param aDebug Debug trace interface
     */
    static CReceiver* NewL( RSocket& aSocket,
                            MReceiverCallback& aCallback,
                            MIkeDebug& aDebug );
    
    /**
     * Destructor.
     */
    ~CReceiver();
    
    /**
     * Sets IKE major version.
     * 
     * @param aIkeMajorVersion IKE major version
     */
    void SetIkeMajorVersion( const IkeSocket::TIkeMajorVersion aIkeMajorVersion );
    
    /**
     * Starts receiving UDP data. Notification that data has been received or
     * notification about unhandled errors will be done via MReceiverCallback
     * callback interface.
     *  
     * IKE major version must be set before receiving is started. Received data
     * will be ignored, if data does not correspond to specified IKE major
     * version.
     */
    void Receive();
    
    /**
     * Cancels receiving.
     */
    void CancelReceive();
    
private:

    enum TReceiverState
        {
        EIdle,                   // Idle
        EWaitingData,            // Waiting data to become available for reading
        EReceiving,              // Receiving data
        EWaitingAfterSocketError // Waiting after socket error 
        };    

    CReceiver( RSocket& aSocket,
               MReceiverCallback& aCallback,
               MIkeDebug& aDebug );
    
    void ConstructL();
    
    /**
     * Waits for data to become available for reading.
     */
    void WaitDataAvailable();
    
    /**
     * Receives data from socket.
     */    
    void ReceiveDataL();

    /**
     * Handles received data.
     */
    void HandleDataReceivedL();
    
    /**
     * Handles error in receiving.
     * @param aStatus Error status
     * @param aSocket Informs if delay needed
     */
    void HandleError( const TInt aStatus,
                      const TBool aDelayNeeded = EFalse );
    
    /**
     * Notifies client that data has been received.
     */    
    void NotifyDataReceived();
    
    
// from base class CActive
        
    /**
     * From CActive.
     * Handles a leave occurring in RunL().
     *
     * @param aError The leave code
     */
    TInt RunError( TInt aError );
    
    /**
     * From CActive.
     * Handles an active object's request completion event about available data
     * or received data.
     */
    void RunL();
    
    /**
     * From CActive.
     * Implements cancellation of an active request.
     */ 
    void DoCancel();    
        
private: // data    
    
    /**
     * Receiver state.
     * Own.
     */
    TReceiverState      iState;
       
    /**
     * Maximum error count.
     * Own.
     */
    TInt                iErrorCount;
    
    /**
     * Message data.
     * Own.
     */
    HBufC8*             iMsg;
    
    /**
     * Message pointer.
     * Own.
     */
    TPtr8               iMsgPtr;
    
    /**
     * Flags for Ioctl command.
     * Own.
     */    
    TPckgBuf< TUint >   iFlags;
    
    /**
     * Source address.
     * Own.
     */
    TInetAddr           iSrcAddr;
    
    /**
     * IKE major version.
     * Own.
     */
    TUint               iIkeMajorVersion;    
    
    /**
     * Socket.
     * Not own.
     */
    RSocket&            iSocket;

    /**
     * Callback for completing receiving.
     * Not own.
     */
    MReceiverCallback&  iCallback;
    
    /**
     * Debug trace interface.
     * Not own.
     */
    MIkeDebug&          iDebug;
    };


#endif // C_RECEIVER_H

