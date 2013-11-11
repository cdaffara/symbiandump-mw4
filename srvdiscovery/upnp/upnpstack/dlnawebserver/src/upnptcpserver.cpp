/** @file
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Declares ControlPoint class.
*
*/


// INCLUDE FILES
#include "upnptcpserver.h"
#include <commdb.h>

#include "upnpcons.h"
#include "upnphttpsession.h"
#define KLogFile _L("DLNAWebServer.txt")
#include "upnpcustomlog.h"
#include "upnphttpservertransactioncreator.h"

#ifdef RD_UPNP_REMOTE_ACCESS
#include "upnpipfiltermanager.h"
#endif

// Include for Publish and Subscribe
#include <e32math.h>

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CUpnpTcpServer::CUpnpTcpServer
// C++ default constructor
// -----------------------------------------------------------------------------
//
CUpnpTcpServer::CUpnpTcpServer( RSocketServ* aSocketServ,
                                TInt aPort,
                                TInt aIap )
    : CActive( EPriorityNormal )
    {
    CActiveScheduler::Add( this );

    iServerPort = aPort;
    iSocketServ = aSocketServ;
    iActiveIap = aIap;

    iServerAddress = TInetAddr( INET_ADDR(0,0,0,0), 0 );
    iRetrySocketTimer = NULL;
    iFileReadBufferSize  = KRcvBufSizeDefault;
    iFileWriteBufferSize = KServeFileSizeDefault;
    }

// -----------------------------------------------------------------------------
// CUpnpTcpServer::BaseConstructL
// TcpServer might-leave initialization method.
// NOTE: Must be called by extending class
// -----------------------------------------------------------------------------
//
void CUpnpTcpServer::BaseConstructL()
    {
    iRetrySocketTimer = CUpnpNotifyTimer::NewL( this );

    CreateConnectionManagerSessionL();
    }

// -----------------------------------------------------------------------------
// CUpnpTcpServer::CUpnpTcpServer
// C++ default destructor
// -----------------------------------------------------------------------------
//
CUpnpTcpServer::~CUpnpTcpServer()
    {
    LOG_FUNC_NAME;
    PrepareToCloseSockets();
    delete iRetrySocketTimer;
    
    #ifdef RD_UPNP_REMOTE_ACCESS
    delete iIPFilteringManager;
    #endif
    }

// -----------------------------------------------------------------------------
// CUpnpTcpServer::CreateConnectionManagerSessionL
//
// -----------------------------------------------------------------------------
//
void CUpnpTcpServer::CreateConnectionManagerSessionL()
    {
    if ( !iConnectionManagerProxy )
        {
        iConnectionManagerProxy = CUpnpConnectionManagerProxy::NewL( *iSocketServ );
        TInt error = iConnectionManagerProxy->EnsureStart();
        if ( error )
            {
            LOGS1H( iHandle, "CUpnpTcpServer::OpenSocketL *** Error in attaching: %d", error );

            // Nothing else to do but leave, connection is not possible and
            // can't access network
            User::LeaveIfError( error );
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpTcpServer::ListenL
// Listen incoming connections
// -----------------------------------------------------------------------------
//
void CUpnpTcpServer::OpenSocketL()
    {
    CreateConnectionManagerSessionL();

    SetServerAddress( iConnectionManagerProxy->LocalAddress() );
    iActiveIap = iConnectionManagerProxy->ActiveIap();

    LOGSH( iHandle, "CUpnpTcpServer::ListenL()");

    TInt error = iServerSocket.Open( *iSocketServ, KAfInet, KSockStream,
                                      KProtocolInetTcp,
                                      iConnectionManagerProxy->ConnectionL() );

    if ( error != KErrNone)
        {
        LOGSH( iHandle, "CUpnpTcpServer ListenL - Open failed.");
        User::Leave( error );
        }
    }

// -----------------------------------------------------------------------------
// CUpnpTcpServer::BindRandomPortL
// -----------------------------------------------------------------------------
//
void CUpnpTcpServer::BindRandomPortL()
    {
    LOG_FUNC_NAME;
    iServerPort = NewPortNumberL();

    TInetAddr addr =  TInetAddr(iServerPort);

    TInt error = iServerSocket.Bind( addr );

    if ( KErrNone == error )
        {
        error = StartToListenL();
        }
    // If the port is in use, then get a new port number and try again.
    else if ( KErrInUse == error )
        {
            LOGS1H( iHandle, "CUpnpTcpServer ListenL - Bind fail, trying new port: %d", iServerPort);


        while ( KErrInUse == error )
            {
            iServerPort = NewPortNumberL();
            iServerAddress.SetPort( iServerPort );
            error = iServerSocket.Bind( iServerAddress );
            if ( KErrNone == error )
                {
                error = StartToListenL();
                }
            }
        if ( KErrNone != error )
            {
                LOGS1H( iHandle, "CUpnpTcpServer ListenL Bind fail: %d", error);

            iServerSocket.Close();
            }
        }
    // Bind failed, close socket and log problem
    else
        {
        iServerSocket.Close();
            LOGS1H( iHandle, "CUpnpTcpServer ListenL -RSocket::Bind fail: %d", error);
        }
    // In case of failure, there is nothing else to do but leave.
    // Construction will fail.
    if ( KErrNone != error )
        {
        LOGSH( iHandle, "CUpnpTcpServer ListenL - Bind or listen failed.");

        User::Leave( error );
        }
    }

// -----------------------------------------------------------------------------
// CUpnpTcpServer::BindL
// -----------------------------------------------------------------------------
//
void CUpnpTcpServer::BindL( const TInt aPort)
{
    LOGS1H( iHandle , "CUpnpTcpServer BindL to fixed port %d" , aPort);
    iServerPort = aPort;

    TInetAddr addr( iServerPort );
    TInt error( iServerSocket.Bind( addr ) );
    if( error == KErrNone )
        {
        User::LeaveIfError( StartToListenL() );
        }
    else
        {
        LOGSH( iHandle, "CUpnpTcpServer ListenL - open, Bind or listen failed");
        iServerSocket.Close();
        User::Leave( error ) ;
        }

}

// -----------------------------------------------------------------------------
// CUpnpTcpServer::StartToListenL
// Start to listen a socket.
// -----------------------------------------------------------------------------
//
TInt CUpnpTcpServer::StartToListenL()
    {
    TInt error = iServerSocket.Listen( UpnpSocket::KMaxConnectionsInQueue );
    if ( KErrNone == error )
        {
        // Both bind and listen ok, accept connection if OK
        LOGS1H( iHandle, "CUpnpTcpServer::StartToListenL - at port %i", ServerPort() );

        AcceptConnectionL();
        }
    // Listen failed, close socket and log problem
    else
        {
        iServerSocket.Close();
        LOGS1H( iHandle, "CUpnpTcpServer StartToListenL - fail: %d" , error);
        }
    return error;
    }

// -----------------------------------------------------------------------------
// CUpnpTcpServer::AcceptConnection
// Open connection
// -----------------------------------------------------------------------------
//
void CUpnpTcpServer::AcceptConnectionL()
    {
    LOG_FUNC_NAME;
    TInt error = 0;

    if( iSessionList.Count() >= KMaxConnectionCount )
    {
        LOGSH( iHandle, "CUpnpTcpServer::AcceptConnectionL - HTTP *** SessionList full");

        StartRetryTimer();
        return;
    }

    error = iClientSocket.Open( *iSocketServ );
    if ( error < KErrNone )
        {
        LOGS1H( iHandle, "CUpnpTcpServer::AcceptConnectionL - error to open client socket,%i", error);

        iClientSocket.Close();

        StartRetryTimer();
        }
    else
        {
        iServerSocket.Accept( iClientSocket, iStatus );
        SetActive();
        iState = EAccepting;
        LOGSH( iHandle, "CUpnpTcpServer::AcceptConnection - State = EAccepting ");
        }

    }

// -----------------------------------------------------------------------------
// CUpnpTcpServer::DeleteSession
// Delete sessions.
// -----------------------------------------------------------------------------
//
void CUpnpTcpServer::DeleteSession( CUpnpTcpSession* aSession )
    {
    LOG_FUNC_NAME;
    TInt pos = iSessionList.Find( aSession );

    if( pos != KErrNotFound )
        {
        iSessionList.Remove( pos );
        iSessionList.Compress();
        delete aSession;
        aSession = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CUpnpTcpServer::RunL
// State machine
// -----------------------------------------------------------------------------
//
void CUpnpTcpServer::RunL()
    {
     switch ( iState )
        {
        case EAccepting:
            {
            if( iStatus == KErrNone )
                {
                iClientSocket.SetOpt( KSoTcpNoDelay, KSolInetTcp, 1 );

                #ifdef RD_UPNP_REMOTE_ACCESS
/**************     IPFIltering    ********************/
                TInetAddr tempAddr;
                iClientSocket.RemoteName( tempAddr );
                tempAddr.ConvertToV4();

                if( iIPFilteringManager && !iIPFilteringManager->IsAllowed( tempAddr ) )
                    {
                    iClientSocket.Close();  //mozliwe ze konieczny bedzie jeszcze
                                            //Shutdown - EImmediate; default jest ENormal
                    AcceptConnectionL()  ;
                    break;
                    }

                #if _DEBUG
                TBuf<20> addrBuf;
                tempAddr.Output( addrBuf );
                #endif
/******************* IPFiltering END *******************/
                #endif

                CUpnpTcpSession* sess = ConnectionAcceptedL( iClientSocket );
                iSessionList.AppendL( sess );

                }
            else
                {
                iClientSocket.Close();
                }

            AcceptConnectionL();

            break;
            }

        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
// CUpnpTcpServer::RunError
// RunError is called when RunL leaves.
// -----------------------------------------------------------------------------
//
TInt CUpnpTcpServer::RunError( TInt aError )
    {
    LOGS1H( iHandle, "CUpnpTcpServer::RunError - Error: %d", aError);
    StartRetryTimer();
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CUpnpTcpServer::StopTcpServerL
// Stop session and reset the lists
// -----------------------------------------------------------------------------
//
void CUpnpTcpServer::StopTcpServerL()
    {
    LOG_FUNC_NAME;      
    for( TInt i(0) ; i < iSessionList.Count() ; i++ )
        {
        iSessionList[i]->NotifyTimeoutL();
        }
    PrepareToCloseSockets();     
    }

// -----------------------------------------------------------------------------
// CUpnpTcpServer::PrepareToCloseSockets
// -----------------------------------------------------------------------------
//
void CUpnpTcpServer::PrepareToCloseSockets()
    {
    iSessionList.ResetAndDestroy();
    if ( IsActive() )
        {
        Cancel();
        }
    else
        {
        CancelRetryTimer();
        CloseSockets();
        }  
    delete iConnectionManagerProxy;
    iConnectionManagerProxy = NULL;    
    }
    
// -----------------------------------------------------------------------------
// CUpnpTcpServer::DoCancel
// Cancel outstanding operations and close the connection
// -----------------------------------------------------------------------------
//
void CUpnpTcpServer::DoCancel()
    {
    CancelRetryTimer();

    iServerSocket.CancelAll();

    CloseSockets();

    LOGSH( iHandle, "CUpnpTcpServer::DoCancel - iState = ENotListening" );
    }

// -----------------------------------------------------------------------------
// CUpnpTcpServer::CloseSockets
// -----------------------------------------------------------------------------
//
void CUpnpTcpServer::CloseSockets()
    {
    iServerSocket.Close();

    iClientSocket.Close();

    iState = ENotListening;
    LOGSH( iHandle, "CUpnpTcpServer::CloseSockets - iState = ENotListening" );
    }
    
// -----------------------------------------------------------------------------
// CUpnpTcpServer::ServerAddress
// Return server address.
// -----------------------------------------------------------------------------
//
void CUpnpTcpServer::ServerAddress( TInetAddr& aAddr )
    {
    LOG_FUNC_NAME;
    aAddr = iServerAddress;
    }

// -----------------------------------------------------------------------------
// CUpnpTcpServer::SetServerAddress
// Set new server address.
// -----------------------------------------------------------------------------
//
void CUpnpTcpServer::SetServerAddress( const TInetAddr& aAddr )
    {
    iServerAddress = aAddr;
    }

// -----------------------------------------------------------------------------
// CUpnpTcpServer::ServerPort
// Return server port number,
// -----------------------------------------------------------------------------
//
TUint CUpnpTcpServer::ServerPort()
    {
    return iServerPort;
    }

// -----------------------------------------------------------------------------
// CUpnpTcpServer::HWAddressL
// Return hardware address.
// -----------------------------------------------------------------------------
//
TSockAddr* CUpnpTcpServer::HWAddressL()
    {
    LOG_FUNC_NAME;
    // Fetch IP address
    ServerAddress(iServerAddress);

    // return value is TSockAddr* because it won't work
    // correctly always with TSockAddr
    TPckgBuf<TSoInetInterfaceInfo> item;

    RSocket sock;

    TInt result = sock.Open( *iSocketServ, KAfInet, KSockStream,
        KProtocolInetTcp, iConnectionManagerProxy->ConnectionL() );
    if ( KErrNone == result )
        {
        result = sock.GetOpt( KSoInetNextInterface, KSolInetIfCtrl, item );

        // checking all interfaces. If interface has same IP address as socket, is
        // the current active interface. In this case we get that interface's
        // hardware address.
        while ( result == KErrNone )
            {
            TSoInetInterfaceInfo& ifInfo = item();
            TInetAddr ifAddr = ifInfo.iAddress;
            ifAddr.ConvertToV4();
            ifAddr.SetPort( iServerAddress.Port() );
            if ( ifAddr == iServerAddress )
                {
                sock.Close();
                TSockAddr tempAddr = ifInfo.iHwAddr;
                return new (ELeave) TSockAddr( tempAddr );
                }
            result = sock.GetOpt( KSoInetNextInterface, KSolInetIfCtrl, item );
            }
        }

    sock.Close();
    return new (ELeave) TSockAddr();
    }


// -----------------------------------------------------------------------------
// CUpnpTcpServer::NewPortNumberL
//
// -----------------------------------------------------------------------------
//
TInt CUpnpTcpServer::NewPortNumberL()
    {
    LOG_FUNC_NAME;
    TInt id( 0 );

    //get random port
    TTime now;
    now.HomeTime();
    TInt64 randSeed = now.Int64();
    id =  Math::Rand( randSeed ) % (KInetMaxAutoPort-KInetMinAutoPort+1) + KInetMinAutoPort;
    return id;
    }

// -----------------------------------------------------------------------------
// CUpnpTcpServer::TimerEventL
// Function is callback from iRetrySocketTimer. This timer is used
// to retry to open new server socket.
// -----------------------------------------------------------------------------
//
void CUpnpTcpServer::TimerEventL( CUpnpNotifyTimer* /*aTimer*/)
    {
    // Timer has expired, now trying to bind server socket again.
    LOGSH( iHandle, "CUpnpTcpServer::TimerEventL - retrying to open new socket server");
    AcceptConnectionL();
    }

// -----------------------------------------------------------------------------
// CUpnpTcpServer::StartRetryTimer
// -----------------------------------------------------------------------------
//
void CUpnpTcpServer::StartRetryTimer()
    {
    CancelRetryTimer();
    iRetrySocketTimer->After(KRetryTime);
    }

// -----------------------------------------------------------------------------
// CUpnpTcpServer::SetFileWriteBufferSize
// -----------------------------------------------------------------------------
//
void CUpnpTcpServer::SetFileWriteBufferSize(TInt aSize)
    {
    iFileWriteBufferSize = aSize;
    }


// -----------------------------------------------------------------------------
// CUpnpTcpServer::FileWriteBufferSize
// -----------------------------------------------------------------------------
//
TInt CUpnpTcpServer::FileWriteBufferSize()
    {
    return iFileWriteBufferSize;
    }

// -----------------------------------------------------------------------------
// CUpnpTcpServer::SetFileReadBufferSize
// -----------------------------------------------------------------------------
//
void CUpnpTcpServer::SetFileReadBufferSize(TInt aSize)
    {
    iFileReadBufferSize = aSize;
    }


// -----------------------------------------------------------------------------
// CUpnpTcpServer::FileReadBufferSize
// -----------------------------------------------------------------------------
//
TInt CUpnpTcpServer::FileReadBufferSize()
    {
    return iFileReadBufferSize;
    }

// -----------------------------------------------------------------------------
// CUpnpTcpServer::GetServerAddress
// Return server address.
// -----------------------------------------------------------------------------
//
TBool CUpnpTcpServer::GetServerAddress( TInetAddr& aAddr )
    {
    aAddr = iServerAddress;
    return (iServerAddress.Address() != 0);
    }

// -----------------------------------------------------------------------------
// CUpnpTcpServer::CancelRetryTimer
// -----------------------------------------------------------------------------
//    
void CUpnpTcpServer::CancelRetryTimer()
    {
    if ( iRetrySocketTimer )
        {
        iRetrySocketTimer->Cancel();
        }
    }

//  End of File
