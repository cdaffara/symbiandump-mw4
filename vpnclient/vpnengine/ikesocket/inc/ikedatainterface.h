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
* Description:  IKE socket data interface
*
*/


#ifndef M_IKEDATAINTERFACE_H
#define M_IKEDATAINTERFACE_H

#include <in_sock.h>

/**
 *  IKE socket data transfer interface.
 *
 *  This interface defines functionality for sending and receiving UDP data.
 *
 *  After client has started receiving, data is queued to receive queue. Queud
 *  data will be cleared, when clients stops receiving, or connection has been
 *  disconnected.
 *
 *  @lib ikesocket.lib
 */
class MIkeDataInterface
    {    
public:
    
    /**
     * Sends UDP data.
     *
     * @param aLocalPort Local port
     * @param aDestAddr Destination IP address/port
     * @param aUdpData UDP data
     * @param aDscp DSCP value
     * @param aStatus Completion status (returned)
     *                -KErrArgument if local port is neither 500 or 4500, and
     *                              destination port does not equal local port
     *                              (equal when Nokia NAT keepalive packet
     *                              sent)
     *                -KErrDisconnected if connection is disconnected                                
     */
    virtual void SendUdpData( const TInt aLocalPort,
                              const TInetAddr& aDestAddr,
                              const TDesC8& aUdpData,
                              const TUint aDscp,
                              TRequestStatus& aStatus ) = 0;
    
    /**
     * Cancels sending.
     */
    virtual void CancelSend() = 0;
    
    /**
     * Starts to receive UDP data. Completes when data is available or error
     * has occured. 
     *
     * @param aUdpData Received UDP data (returned)
     * @param aSrcAddr Source IP address/port (returned)
     * @param aLocalPort Local port (returned)
     * @param aStatus Completion status (returned)
     *                  -KErrDisconnected if connection is disconnected
     */
    virtual void ReceiveUdpData( HBufC8*& aUdpData,
                                 TInetAddr& aSrcAddr,
                                 TInt& aLocalPort,
                                 TRequestStatus& aStatus  ) = 0;
    
    /**
     * Cancels receive request.
     */
    virtual void CancelReceive() = 0;

    /**
     * Clears data which has been received.
     */
    virtual void ClearReceivedData() = 0;
    
    /**
     * Stops receiving. Clears data which has been received.
     */
    virtual void StopReceive() = 0;
    
    /**
     * Gets local IP address of interface.
     *
     * @param aLocalIp Local IP address (returned)
     * @return Error status. KErrNotFound if address is not found.
     */
    virtual TInt GetLocalAddress( TInetAddr& aLocalIp ) = 0;
    };


#endif // M_IKEDATAINTERFACE_H
