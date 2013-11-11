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


#include <es_sock.h>
#include "receiver.h"
#include "ikemsgheader.h"
#include "ikev2const.h"
#include "ikesocketdefs.h"
#include "ikedebug.h"
#include "ikesocketassert.h"

using namespace IkeSocket;

const TInt KMaxIkePacketSize( 65536 ); // Maximum size for UDP packet

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CReceiver* CReceiver::NewL( RSocket& aSocket,
                            MReceiverCallback& aCallback,                                     
                            MIkeDebug& aDebug  )
    {
    CReceiver* self = new (ELeave) CReceiver( aSocket,
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
CReceiver::~CReceiver()
    {
    DEBUG_LOG( _L("CReceiver::~CReceiver") );
    Cancel();
    delete iMsg;
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CReceiver::CReceiver( RSocket& aSocket,
                      MReceiverCallback& aCallback,
                      MIkeDebug& aDebug )
 : CTimer( EPriorityStandard ),
   iState( EIdle ),
   iMsgPtr( 0, 0, 0 ),
   iSocket( aSocket ),
   iCallback( aCallback ),
   iDebug( aDebug )
    {
    CActiveScheduler::Add( this ); // Added to the Active Scheduler
    }

// ---------------------------------------------------------------------------
// Second phase construction.
// ---------------------------------------------------------------------------
//
void CReceiver::ConstructL()
    {
    DEBUG_LOG( _L("CReceiver::ConstructL") );
    CTimer::ConstructL();
    }

// ---------------------------------------------------------------------------
// Sets IKE major version.
// ---------------------------------------------------------------------------
//
void CReceiver::SetIkeMajorVersion( const TIkeMajorVersion aIkeMajorVersion )
    {
    IKESOCKET_ASSERT( aIkeMajorVersion == EIkeMajorV1 ||
                      aIkeMajorVersion == EIkeMajorV2 );

    iIkeMajorVersion = aIkeMajorVersion;    
    }

// ---------------------------------------------------------------------------
// Starts receive.
// ---------------------------------------------------------------------------
//
void CReceiver::Receive()
    {
    IKESOCKET_ASSERT( iIkeMajorVersion == EIkeMajorV1 ||
                      iIkeMajorVersion == EIkeMajorV2 );

    if ( iState == EIdle )
        {
        WaitDataAvailable();
        }
    }

// ---------------------------------------------------------------------------
// Cancels receive.
// ---------------------------------------------------------------------------
//
void CReceiver::CancelReceive()
    {
    Cancel();
        
    delete iMsg;
    iMsg = NULL;
    iMsgPtr.Set( 0, 0, 0 );
    
    iState = EIdle;    
    }

// ---------------------------------------------------------------------------
// Waits for data to become available for reading.
// ---------------------------------------------------------------------------
//
void CReceiver::WaitDataAvailable()
    {
    IKESOCKET_ASSERT( iState == EIdle );
    IKESOCKET_ASSERT( !IsActive() );
        
    iState = EWaitingData;
    delete iMsg;
    iMsg = NULL;
        
    iFlags() = KSockSelectRead | KSockSelectExcept;

    iSocket.Ioctl( KIOctlSelect,
                   iStatus,
                   &iFlags,
                   KSOLSocket );    
    SetActive();
    }

// ---------------------------------------------------------------------------
// Receives data from socket.
// ---------------------------------------------------------------------------
//
void CReceiver::ReceiveDataL()
    {
    IKESOCKET_ASSERT( !IsActive() );

    iState = EReceiving;
        
    TInt bytesPending( 0 );
    TInt err = iSocket.GetOpt( KSOReadBytesPending,
                               KSOLSocket,
                               bytesPending );
    
    User::LeaveIfError( err );
        
    if ( bytesPending > KMaxIkePacketSize )        
        {
        // KMaxIkePacketSize (65536) is max message size supported.
        bytesPending = KMaxIkePacketSize;
        }
    
    iMsg = HBufC8::NewL( bytesPending );
    iMsgPtr.Set( iMsg->Des() );

    iSocket.RecvFrom( iMsgPtr,
                      iSrcAddr,
                      0,
                      iStatus );
    SetActive();
    }

// ---------------------------------------------------------------------------
// Handles receive of data.
// ---------------------------------------------------------------------------
//
void CReceiver::HandleDataReceivedL() 
    {
#ifdef _DEBUG
    TBuf<100> txt_addr;
    iSrcAddr.Output( txt_addr );
    TUint32 port = iSrcAddr.Port();
    DEBUG_LOG3( _L("CReceiver::HandleDataReceivedL, local port=%d, src address:port=%S:%d"),
            iSocket.LocalPort(), &txt_addr, port );
#endif
    
    TInt msgLength = iMsgPtr.Length();            
    if ( msgLength <= TInt(ISAKMP_HDR_SIZE) )
        {
        // Message size smaller than header size.
        User::Leave( KErrArgument ); 
        }
        
    // Check if <non-ESP marker> is in the beginning of IKE message.
    // <non-ESP marker> is related to the NAT traversal and it should
    // exist only if IKE messages received through port 4500.
    // However, we accept <non-ESP marker> also in IKE message
    // received through normal IKE port (500).
    const ThdrISAKMP* ikeHdr = ThdrISAKMP::Ptr( iMsgPtr );
    TUint32 octets = BigEndian::Get32( (TUint8*)(ikeHdr) );
    TBool nonEspMarker = ( octets == NON_ESP_MARKER );
    if ( nonEspMarker )
        {                
        ikeHdr = ikeHdr->GotoOffset( NON_ESP_MARKER_SIZE );
        msgLength -= NON_ESP_MARKER_SIZE;        
        if ( msgLength <= TInt(ISAKMP_HDR_SIZE) )
            {
            // Message size smaller than header size.
            User::Leave( KErrArgument ); 
            }
        }
    
    // Because the received data can be any UDP data transmitted to
    // IKE port(s), some checks are done before packet is processed. Length   
    // value read from header must be greater than ISAKMP_HDR_SIZE.
    TInt ikeMsgLength = ikeHdr->GetLength();        
    if ( ikeMsgLength <= TInt(ISAKMP_HDR_SIZE) )
        {
        User::Leave( KErrArgument );        
        }

    // IKE major version in packet MUST be as client expects (1 or 2).
    TUint8 majorVersion = ikeHdr->GetMajorVersion();
    if ( majorVersion != iIkeMajorVersion )
        {
        User::Leave( KErrArgument );
        }
      
    NotifyDataReceived();    
    }

// ---------------------------------------------------------------------------
// Handles error in receiving.
// ---------------------------------------------------------------------------
//
void CReceiver::HandleError( const TInt aStatus,
                             const TBool aDelayNeeded )
    {
    DEBUG_LOG3( _L("CReceiver::HandleError, aStatus=%d, error count=%d, delay needed=%d"),
            aStatus, iErrorCount, aDelayNeeded );    
    
    delete iMsg;
    iMsg = NULL;
    iMsgPtr.Set( 0, 0, 0 );
    
    const TInt KMaxReceiverErrorCount( 20 );
    
    if ( aStatus == KErrDied ||
         aStatus == KErrServerTerminated ||
         aStatus == KErrNoMemory ||
         iErrorCount >= KMaxReceiverErrorCount )
        {
        // Fatal error or maximum error count reached. Notify client.
        iState = EIdle;
        iCallback.ReceiveError( aStatus );
        }
    else if ( aDelayNeeded )
        {
        // Restart receiving after delay.
        iState = EWaitingAfterSocketError;
        const TInt KDelayIntervalSocketError( 1000000 ); // 1s        
        After( iErrorCount*KDelayIntervalSocketError );                
        }
    else
        {
        // Receiving can be continued without delay.
        iState = EIdle;
        Receive();
        }
    }

// ---------------------------------------------------------------------------
// Notifies client that data has been received.
// ---------------------------------------------------------------------------
//
void CReceiver::NotifyDataReceived()
    {
    TInetAddr srcAddr = iSrcAddr;
    TInt localPort = iSocket.LocalPort();
    HBufC8* msg = iMsg;

    iMsg = NULL;
    iMsgPtr.Set( 0, 0, 0 );
    iState = EIdle;
    
    // Continue receiving.
    Receive();
        
    iCallback.DataReceived( msg, // Ownership transferred
                            srcAddr,
                            localPort );
    }

// ---------------------------------------------------------------------------
// From CActive
// Handles a leave occurring in RunL().
// ---------------------------------------------------------------------------
//
TInt CReceiver::RunError( TInt aError )
    {
    HandleError( aError );    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// From CActive
// Handles request completion event about available data or received data.
// ---------------------------------------------------------------------------
//
void CReceiver::RunL()
    {
    IKESOCKET_ASSERT( iState == EWaitingData ||
                      iState == EReceiving  ||
                      iState == EWaitingAfterSocketError );
    DEBUG_LOG2( _L("CReceiver::RunL, iState=%d, iStatus=%d"),
            iState, iStatus.Int() );    
    
    if ( iStatus.Int() )
        {
        HandleError( iStatus.Int(), ETrue );
        iErrorCount++;
        return;
        }
    
    switch ( iState )
        {
        case EWaitingData:
            {
            ReceiveDataL();
            break;
            }
        case EReceiving:
            {
            iErrorCount = 0;
            HandleDataReceivedL();
            break;
            }
        case EWaitingAfterSocketError:
            {
            iState = EIdle;
            Receive();
            break;
            }
        default:
            break;
        }
    }

// ---------------------------------------------------------------------------
// From CActive
// Implements cancellation of an active request.
// ---------------------------------------------------------------------------
//
void CReceiver::DoCancel()
    {
    IKESOCKET_ASSERT( iState == EWaitingData ||
                      iState == EReceiving ||
                      iState == EWaitingAfterSocketError );
    DEBUG_LOG1( _L("CReceiver::DoCancel, iState=%d"),
            iState );

    iErrorCount = 0;
    
    switch ( iState )
        {
        case EWaitingData:
            {
            iSocket.CancelIoctl();
            break;
            }
        case EReceiving:
            {
            iSocket.CancelRecv();
            break;
            }
        case EWaitingAfterSocketError:
            {
            CTimer::DoCancel();
            break;
            }
        default:
            break;
        }
    }
