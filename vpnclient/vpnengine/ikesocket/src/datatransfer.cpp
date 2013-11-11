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


#include "datatransfer.h"
#include "ikemsgheader.h"
#include "ikev2const.h"
#include "localaddressresolver.h"
#include "ikedebug.h"
#include "ikesocketassert.h"

using namespace IkeSocket;

const TInt KReceiveQueueMaxCount( 10 );

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CDataTransfer* CDataTransfer::NewL( RSocketServ& aSocketServer,
                                    RConnection& aConnection,
                                    CLocalAddressResolver& aLocalAddressResolver,
                                    MDataTransferCallback& aCallback,
                                    MIkeDebug& aDebug  )
    {    
    CDataTransfer* self = new (ELeave) CDataTransfer( aSocketServer,
                                                      aConnection,
                                                      aLocalAddressResolver,
                                                      aCallback,
                                                      aDebug );  
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);    
    return self;            
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CDataTransfer::~CDataTransfer()
    {
    DEBUG_LOG( _L("CDataTransfer::~CDataTransfer") );
    
    CloseSockets();    
    iReceiveQueue.Close();
    
    delete iSender;
    delete iReceiver;
    delete iReceiverNAT;
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CDataTransfer::CDataTransfer( RSocketServ& aSocketServer,
                              RConnection& aConnection,
                              CLocalAddressResolver& aLocalAddressResolver,
                              MDataTransferCallback& aCallback,
                              MIkeDebug& aDebug )
 : iSocketServer( aSocketServer ),
   iConnection( aConnection ),
   iLocalNokiaNATPort( 0 ),
   iLocalAddressResolver( aLocalAddressResolver ),
   iErrorCallback( aCallback ),
   iDebug( aDebug )
    {
    }

// ---------------------------------------------------------------------------
// Second phase construction.
// ---------------------------------------------------------------------------
//
void CDataTransfer::ConstructL()
    {
    DEBUG_LOG( _L("CDataTransfer::ConstructL") );
    
    iSender = CSender::NewL( iSocket,
                             iSocketNAT,
                             iSocketNokiaNAT,
                             *this,
                             iDebug );
    iReceiver = CReceiver::NewL( iSocket,
                                 *this,
                                 iDebug );
    
    iReceiverNAT = CReceiver::NewL( iSocketNAT,
                                    *this,
                                    iDebug );
    }

// ---------------------------------------------------------------------------
// Sets IKE major version to receivers.
// ---------------------------------------------------------------------------
//
void CDataTransfer::SetIkeMajorVersion( const TIkeMajorVersion aIkeMajorVersion )
    {
    IKESOCKET_ASSERT( iReceiver );
    IKESOCKET_ASSERT( iReceiverNAT );
    
    iReceiver->SetIkeMajorVersion( aIkeMajorVersion );
    iReceiverNAT->SetIkeMajorVersion( aIkeMajorVersion );    
    }

// ---------------------------------------------------------------------------
// Sets IP version.
// ---------------------------------------------------------------------------
//
void CDataTransfer::SetIpVersion( const IkeSocket::TIpVersion aIpVersion )
    {
    iIpVersion = aIpVersion;
    }

// ---------------------------------------------------------------------------
// Opens sockets and binds sockets for ports 500 and 4500. 
// ---------------------------------------------------------------------------
//
TInt CDataTransfer::OpenSockets( const TInetAddr& aLocalIp )
    {    
    IKESOCKET_ASSERT( !iSocketsOpen );
    
    // Open sockets
    TInt err = OpenSocket( iSocket );
    
    if ( err == KErrNone )
        {
        err = OpenSocket( iSocketNAT );
        
        if ( err == KErrNone )
            {
            err = OpenSocket( iSocketNokiaNAT );        
            }            
        }
    
    // Bind sockets for ports 500 and 4500
    if ( err == KErrNone )
        {
        err = BindSocket( iSocket, aLocalIp, KIkePort500 );
        
        if ( err == KErrNone )
            {
            err = BindSocket( iSocketNAT, aLocalIp, KIkePort4500 );
            }        
        }       
    
    if ( err == KErrNone )
        {                
        iSocketsOpen = ETrue;
        }
    else
        {
        // Close sockets if error
        iSocket.Close();
        iSocketNAT.Close();
        iSocketNokiaNAT.Close();        
        iSocketsOpen = EFalse;
        }

    DEBUG_LOG1( _L("CDataTransfer::OpenSockets, err=%d"), err );
    return err;
    }

// ---------------------------------------------------------------------------
// Closes sockets.
// ---------------------------------------------------------------------------
//
void CDataTransfer::CloseSockets()
    {
    DEBUG_LOG1( _L("CDataTransfer::CloseSockets, sockets open=%d"),
            iSocketsOpen );
    
    if ( iSocketsOpen )
        {
        // Cancel send and receive.
        DoCancelSend( KErrDisconnected );
        DoCancelReceive( KErrDisconnected );

        // Stop receiving.
        StopReceive();
        
        // Close sockets.
        iSocket.Close();    
        iSocketNAT.Close();
        iSocketNokiaNAT.Close();
        
        iSocketsOpen = EFalse;
        }        
    }

// ---------------------------------------------------------------------------
// Sends UDP data.
// ---------------------------------------------------------------------------
//
void CDataTransfer::SendUdpData( const TInt aLocalPort,
                                 const TInetAddr& aDestAddr,
                                 const TDesC8& aUdpData,
                                 const TUint aDscp,
                                 TRequestStatus& aStatus )
    {
    TInt err( KErrNone );
    
    IKESOCKET_ASSERT( iClientStatusSend == NULL );
    IKESOCKET_ASSERT( iSender );
    
    iClientStatusSend = &aStatus;
    *iClientStatusSend = KRequestPending;
    
    if ( !iSocketsOpen )
        {
        err = KErrDisconnected;
        }
    
    if ( err == KErrNone )
        {
        if ( ( aLocalPort != KIkePort500 ) &&
             ( aLocalPort != KIkePort4500 ) )
            {
            // Nokia NAT keepalive packet.
            if ( aLocalPort == aDestAddr.Port() )
                {    
                if ( iLocalNokiaNATPort == 0 )
                    {
                    // Set Nokia NAT Port if not set.
                    err = iSocketNokiaNAT.SetLocalPort( aLocalPort );                    

                    if ( err == KErrNone )
                        {
                        iLocalNokiaNATPort = aLocalPort;
                        }                    
                    }
                else if ( iLocalNokiaNATPort != aLocalPort )
                    {
                    // Nokia NAT port cannot be changed
                    // during connection.
                    err = KErrArgument;
                    }
                else
                    {
                    err = KErrNone;
                    }
                }
            else
                {
                // Local port does not match destination port.
                err = KErrArgument;
                }        
            }
        }

    if ( err == KErrNone )
        {
        err = iSender->SendUdpData( aLocalPort,
                                    aDestAddr,
                                    aUdpData,
                                    aDscp );        
        }

    if ( err )
        {
        CompleteSendToClient( err );
        }    
    }


// ---------------------------------------------------------------------------
// Cancels sending.
// ---------------------------------------------------------------------------
//
void CDataTransfer::CancelSend()
    {
    DoCancelSend();
    }

// ---------------------------------------------------------------------------
// Receives UDP data.
// ---------------------------------------------------------------------------
//
void CDataTransfer::ReceiveUdpData( HBufC8*& aUdpData,
                                    TInetAddr& aSrcAddr,
                                    TInt& aLocalPort,
                                    TRequestStatus& aStatus  )
    {
    IKESOCKET_ASSERT( iClientStatusReceive == NULL );
    IKESOCKET_ASSERT( iClientMsgReceive == NULL );
    IKESOCKET_ASSERT( iClientSrcAddrReceive == NULL );
    IKESOCKET_ASSERT( iClientLocalPort == NULL );
    
    // Store client data.
    iClientStatusReceive = &aStatus;
    *iClientStatusReceive = KRequestPending;
    iClientMsgReceive = &aUdpData;
    iClientSrcAddrReceive = &aSrcAddr;
    iClientLocalPort = &aLocalPort;    
    
    if ( !iSocketsOpen )
        {
        CompleteReceiveToClient( KErrDisconnected );
        return;
        }

    iReceivingStopped = EFalse;
    
    TInt count = iReceiveQueue.Count();    
    if ( count < KReceiveQueueMaxCount )
        {
        ReceiveData();
        }
    
    if ( count )
        {
        // Data is already available.
        CompleteReceiveToClient( KErrNone );       
        }    
    }

// ---------------------------------------------------------------------------
// Cancels receive request.
// ---------------------------------------------------------------------------
//
void CDataTransfer::CancelReceive()
    {
    if ( iClientStatusReceive )
        {
        CompleteReceiveToClient( KErrCancel );
        }
    }

// ---------------------------------------------------------------------------
// Clears available data.
// ---------------------------------------------------------------------------
//
void CDataTransfer::ClearReceivedData()
    {
    DEBUG_LOG( _L("CDataTransfer::ClearReceivedData") );
    
    CleanupReceiveQueue();
    ReceiveData();
    }

// ---------------------------------------------------------------------------
// Stops receiving. Available data is cleared.
// ---------------------------------------------------------------------------
//
void CDataTransfer::StopReceive()
    {
    DEBUG_LOG( _L("CDataTransfer::StopReceive") );
    
    CleanupReceiveQueue();    
    DoCancelReceive( KErrCancel );    
    iReceivingStopped = ETrue;
    }

// ---------------------------------------------------------------------------
// Gets local IP address.
// ---------------------------------------------------------------------------
//
TInt CDataTransfer::GetLocalAddress( TInetAddr& aLocalIp )
    {
    IKESOCKET_ASSERT( iIpVersion == EIPv4 || iIpVersion == EIPv6 );
    return iLocalAddressResolver.GetLocalAddress( iIpVersion, aLocalIp );        
    }

// ---------------------------------------------------------------------------
// Notification about completed send from sender.
// ---------------------------------------------------------------------------
//
void CDataTransfer::SendCompleted( const TInt aStatus )
    {
    CompleteSendToClient( aStatus );
    }

// ---------------------------------------------------------------------------
// Notification that data has been received.
// ---------------------------------------------------------------------------
//
void CDataTransfer::DataReceived( HBufC8* aUdpData,
                                  const TInetAddr& aSrcAddr,
                                  const TInt aLocalPort )
    {
    // Store message to receive queue.
    TReceiveQueueItem item( aUdpData, // Ownership transferred.
                            aSrcAddr,
                            aLocalPort );      
    TInt err = iReceiveQueue.Append( item );
    
    if ( iReceiveQueue.Count() >= KReceiveQueueMaxCount )
        {
        // Queue is full. Cancel receiving.
        iReceiver->CancelReceive();
        iReceiverNAT->CancelReceive();
        }
    
    if ( iClientStatusReceive )
        {
        CompleteReceiveToClient( KErrNone );
        }
    }

// ---------------------------------------------------------------------------
// Notification about receive error.
// ---------------------------------------------------------------------------
//
void CDataTransfer::ReceiveError( const TInt aStatus )
    {    
    if ( iClientStatusReceive )
        {
        CompleteReceiveToClient( aStatus );
        }
    
    StopReceive();
    
    iErrorCallback.DataTransferError( aStatus,
                                      MDataTransferCallback::EReceiveError );
    }

// ---------------------------------------------------------------------------
// Opens socket.
// ---------------------------------------------------------------------------
//
TInt CDataTransfer::OpenSocket( RSocket& aSocket )
    {
    TInt err = aSocket.Open( iSocketServer,
                             KAfInet,
                             KSockDatagram,
                             KProtocolInetUdp,
                             iConnection );
    if ( err == KErrNone )
        {
        // Enable multiple binds to same port
        err = aSocket.SetOpt( KSoReuseAddr, KSolInetIp, 1 );
        }
    
    return err;
    }

// ---------------------------------------------------------------------------
// Binds socket.
// ---------------------------------------------------------------------------
//
TInt CDataTransfer::BindSocket( RSocket& aSocket,
                                const TInetAddr& aLocalIp,
                                const TInt aLocalPort )
    {
    TInt err( KErrNone );
    TInetAddr localAddr( aLocalIp );
    
    localAddr.SetPort( aLocalPort );
    err = aSocket.Bind( localAddr );

#ifdef _DEBUG    
        TBuf<100> txt_addr;
        aLocalIp.Output( txt_addr );
        DEBUG_LOG3( _L("Bind socket, address:port=%S:%d, err=%d"),
                &txt_addr, aLocalPort, err );
#endif
    
    return err;    
    }

// ---------------------------------------------------------------------------
// Cancels sending.
// ---------------------------------------------------------------------------
//
void CDataTransfer::DoCancelSend( const TInt aCompletionStatus )
    {
    iSender->Cancel();
    
    if ( iClientStatusSend )
        {
        CompleteSendToClient( aCompletionStatus );
        }
    }

// ---------------------------------------------------------------------------
// Cancels receiving.
// ---------------------------------------------------------------------------
//
void CDataTransfer::DoCancelReceive( const TInt aCompletionStatus )
    {
    iReceiver->CancelReceive();
    iReceiverNAT->CancelReceive();
    
    if ( iClientStatusReceive )
        {
        CompleteReceiveToClient( aCompletionStatus );
        }
    }

// ---------------------------------------------------------------------------
// Completes send to client.
// ---------------------------------------------------------------------------
//
void CDataTransfer::CompleteSendToClient( const TInt aStatus )
    {
    IKESOCKET_ASSERT( iClientStatusSend  );
    
    User::RequestComplete( iClientStatusSend, aStatus );
    iClientStatusSend = NULL;        
    }

// ---------------------------------------------------------------------------
// Completes receive to client.
// ---------------------------------------------------------------------------
//
void CDataTransfer::CompleteReceiveToClient( const TInt aStatus )
    {
    IKESOCKET_ASSERT( iClientStatusReceive );
    IKESOCKET_ASSERT( iClientMsgReceive );
    IKESOCKET_ASSERT( iClientSrcAddrReceive );
    IKESOCKET_ASSERT( iClientLocalPort );
    
    TInt count = iReceiveQueue.Count();
    
    if ( ( aStatus == KErrNone ) && count )
        {        
        // Get oldest item from receive queue.
        TReceiveQueueItem item = iReceiveQueue[0];
        iReceiveQueue.Remove( 0 );        
        *iClientMsgReceive = item.UdpData(); // Transfer ownership.
        *iClientSrcAddrReceive = item.SrcAddr();
        *iClientLocalPort = item.LocalPort();
        
        // Need to receive more data if queue was full.
        ReceiveData();
        }
    
    // Complete receive.
    User::RequestComplete( iClientStatusReceive, aStatus );    
    iClientStatusReceive = NULL;
    iClientMsgReceive = NULL;
    iClientSrcAddrReceive = NULL;
    iClientLocalPort = NULL;    
    }
    
// ---------------------------------------------------------------------------
// Receives more data if receiving not requested to be stopped.
// ---------------------------------------------------------------------------
//
void CDataTransfer::ReceiveData()
    {
    if ( !iReceivingStopped )
        {
        iReceiver->Receive();
        iReceiverNAT->Receive();
        }    
    }

// ---------------------------------------------------------------------------
// Cleans up data from receive queue.
// ---------------------------------------------------------------------------
//
void CDataTransfer::CleanupReceiveQueue()
    {
    while ( iReceiveQueue.Count() )
        {
        delete iReceiveQueue[0].UdpData();
        iReceiveQueue.Remove( 0 );
        }    
    }

