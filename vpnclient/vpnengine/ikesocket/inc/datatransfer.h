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
* Description:  Data transfer functionality
*
*/


#ifndef C_DATATRANSFER_H
#define C_DATATRANSFER_H

#include <in_sock.h>
#include "ikedatainterface.h"
#include "sender.h"
#include "receiver.h"
#include "receivequeueitem.h"
#include "ikesocketdefs.h"

// FORWARD DECLARATIONS

class CLocalAddressResolver;
class MIkeDebug;

/**
 *  Data transfer callback interface.
 *
 *  Callback interface for informing fatal error in data transfer.
 *
 *  @lib ikesocket.lib
 */
NONSHARABLE_CLASS( MDataTransferCallback )
    {
public:
    enum TErrorType
        {
        ESendError,
        EReceiveError
        };
    
    /**
     * Notifies that data transfer error has occured.
     * 
     * @param aError Error value
     * @param aErrorType Error type
     */        
    virtual void DataTransferError( const TInt aError,
                                    const TErrorType aErrorType ) = 0;
    };

/**
 *  Data transfer class.
 *
 *  This class provides functionality for transferring UDP data. This class
 *  implements data interface (MIkeDataInterface) which can be used
 *  by the client of data interface to send and receive UDP data. Received data
 *  is queued until client reads received data.
 * 
 *  Before instance of this class can be used for transferring data, owner of
 *  the instance must set IKE major version and request opening of sockets.
 *  When data transferring is no more needed, owner can request closing of
 *  sockets. Fatal data transfer errors are notified to owner through 
 *  error callback interface (MDataTransferCallback).
 * 
 *  This class owns sockets for local ports 500 and 4500 which are used for
 *  both sending and receiving UDP data. This class owns also third socket
 *  which is used if the client of data interface sends Nokia NAT keepalive
 *  packet. 
 * 
 *  @lib ikesocket.lib
 */
class CDataTransfer : public CBase, 
                      public MIkeDataInterface,
                      public MSenderCallback,
                      public MReceiverCallback
    {
public:
    
    /**
     * Two-phased constructor.
     * @param aSocketServer Socket server
     * @param aConnection Connection
     * @param aCallback Error callback interface
     * @param aDebug Debug trace interface
     */
    static CDataTransfer* NewL( RSocketServ& aSocketServer,
                                RConnection& aConnection,
                                CLocalAddressResolver& aLocalAddressResolver,
                                MDataTransferCallback& aCallback,
                                MIkeDebug& aDebug );
    
    /**
     * Destructor.
     */
    ~CDataTransfer();    

    /**
     * Sets IKE major version. IKE major version must be set before data
     * interface is used for data transfer.
     * 
     * @param aIkeMajorVersion IKE major version
     */
    void SetIkeMajorVersion( const IkeSocket::TIkeMajorVersion aIkeMajorVersion );

    /**
     * Sets IP version. IP version must be set before data interface is used
     * for getting local IP address.
     * 
     * @param aIpVersion IP version
     */
    void SetIpVersion( const IkeSocket::TIpVersion aIpVersion );

    /**
     * Opens sockets. Sockets for ports 500 and 4500 are bound to specified
     * local IP address.
     * 
     * @param aLocalIp Local IP address which is used to bind sockets (for ports
     *                 500 and 4500). 
     */
    TInt OpenSockets( const TInetAddr& aLocalIp );
    
    /**
     * Closes sockets.
     */
    void CloseSockets();
            
// from base class MIkeDataInterface
    
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
    void SendUdpData( const TInt aLocalPort,
                      const TInetAddr& aDestAddr,
                      const TDesC8& aUdpData,
                      const TUint aDscp,
                      TRequestStatus& aStatus );

    /**
     * Cancels sending.
     */
    void CancelSend();
    
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
    void ReceiveUdpData( HBufC8*& aUdpData,
                         TInetAddr& aSrcAddr,
                         TInt& aLocalPort,
                         TRequestStatus& aStatus  );
    
    /**
     * Cancels receive request.
     */
    void CancelReceive();

    /**
     * Clears data which has been received.
     */
    void ClearReceivedData();
    
    /**
     * Stops receiving. Clears data which has been received.
     */
    void StopReceive();
        
    /**
     * Gets local IP address of interface.
     *
     * @param aLocalIp Local IP address (returned)
     * @return Error status. KErrNotFound if address is not found.
     */
    TInt GetLocalAddress( TInetAddr& aLocalIp );

// from base class MSenderCallback

    /**
     * Notification about completed send.
     */    
    void SendCompleted( const TInt aStatus );
    
// from base class MReceiverCallback

    /**
     * Notification that data has been received. Receiving is continued
     * automatically by receiver.
     * 
     * @param aUdpData Received UDP data. Ownership transferred.
     * @param aSrcAddr Source address
     * @param aLocalPort Local port
     */    
    void DataReceived( HBufC8* aUdpData,
                       const TInetAddr& aSrcAddr,
                       const TInt aLocalPort );
    
    /**
     * Notification about receive error. Receiving has been stopped by receiver.
     * 
     * @param aStatus Error status
     */    
    void ReceiveError( const TInt aStatus );    
        
private:
    
    CDataTransfer( RSocketServ& aSocketServer,
                   RConnection& aConnection,
                   CLocalAddressResolver& aLocalAddressResolver,
                   MDataTransferCallback& aCallback,
                   MIkeDebug& aDebug );
    
    void ConstructL();
        
    /**
     * Opens specified socket.
     * 
     * @param aSocket Socket
     * @return Error value
     */    
    TInt OpenSocket( RSocket& aSocket );
    
    /**
     * Binds specified socket to local IP address/port.
     * @param aSocket Socket
     * @param aLocalIp Local IP address
     * @param aLocalPort Local port
     * @return Error value
     */    
    TInt BindSocket( RSocket& aSocket,
                     const TInetAddr& aLocalIp,
                     const TInt aLocalPort );
    
    /**
     * Cancels sending.
     * 
     * @param aCompletionStatus Client completion status
     */
    void DoCancelSend( const TInt aCompletionStatus = KErrCancel );

    /**
     * Cancels receiving.
     * 
     * @param aCompletionStatus Client completion status
     */
    void DoCancelReceive( const TInt aCompletionStatus = KErrCancel );

    /**
     * Completes send to client.
     * 
     * @param aStatus Send status
     */
    void CompleteSendToClient( const TInt aStatus );
    
    /**
     * Completes receive to client.
     * 
     * @param aStatus Receive status
     */
    void CompleteReceiveToClient( const TInt aStatus );
    
    /**
     * Receives more data if receiving not requested to be stopped.
     */
    void ReceiveData();

    /**
     * Cleans up data from receive queue.
     */
    void CleanupReceiveQueue();    
    
private: // data    
    
    /**
     * Socket server.
     * Not own.
     */
    RSocketServ&                iSocketServer;
    
    /**
     * Connection used for data transfer.
     * Not own.
     */
    RConnection&                iConnection;
    
    /**
     * Pointer to client's request status for sending
     * Not own.
     */
    TRequestStatus*             iClientStatusSend;
    
    /**
     * Pointer to client's request status for receiving.
     * Not own.
     */
    TRequestStatus*             iClientStatusReceive;
    
    /**
     * Pointer to client's heap descriptor pointer (HBufC8*) variable. Variable
     * will contain received message data, after receive has been completed 
     * successfully to client.
     * Not own.
     */
    HBufC8**                    iClientMsgReceive;
    
    /**
     * Pointer to client's address variable. Variable will contain source
     * address from which data has been received, after receive has been
     * completed successfully to client.
     * Not own.
     */
    TInetAddr*                  iClientSrcAddrReceive;

    /**
     * Pointer to client's address variable. Variable will contain local
     * port from which data has been received, after receive has been
     * completed successfully to client.
     * Not own.
     */
    TInt*                       iClientLocalPort;

    /**
     * Socket for port 500.
     * Own.
     */
    RSocket                     iSocket;
    
    /**
     * Socket for port 4500.
     * Own.
     */
    RSocket                     iSocketNAT;
    
    /**
     * Socket for Nokia NAT port.
     * Own.
     */
    RSocket                     iSocketNokiaNAT;
    
    /**
     * Local Nokia NAT port.
     * Own.
     */
    TInt                        iLocalNokiaNATPort;
    
    /**
     * Informs if sockets are open or not.
     * Own.
     */
    TBool                       iSocketsOpen;
    
    /**
     * Informs if receiving is stopped or not.
     * Own.
     */
    TBool                       iReceivingStopped;

    /**
     * Receiver for port 500.
     * Own.
     */
    CReceiver*                  iReceiver;
    
    /**
     * Receiver for port 4500.
     * Own.
     */
    CReceiver*                  iReceiverNAT;
        
    /**
     * Sender.
     * Own.
     */
    CSender*                    iSender;
    
    /**
     * Queue containing received data.
     * Own.
     */
    RArray<TReceiveQueueItem>   iReceiveQueue;
        
    /**
     * Local address resolver.
     * Not own.
     */
    CLocalAddressResolver&      iLocalAddressResolver;
    
    /**
     * IP version.
     * Own.
     */
    IkeSocket::TIpVersion       iIpVersion;
    
    /**
     * Error callback interface.
     * Not own.
     */
    MDataTransferCallback&      iErrorCallback;
    
    /**
     * Debug trace interface.
     * Not own.
     */
    MIkeDebug&                  iDebug;
    };

#endif // C_DATATRANSFER_H
