/** @file
* Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CUpnpUdpServer
*
*/


// INCLUDE FILES
#include <in_iface.h>
#include <e32math.h>
#include <commdb.h>

#include "upnpcons.h"
#include "upnpudpsendrequest.h"
#include "upnpudpserver.h"
#define KLogFile _L("upnpudpserver.txt")
#include "upnpcustomlog.h"
#include "upnpstring.h"
#include "upnplist.h"
#include "upnpconnectionmanagerproxy.h"


// CONSTANTS
const TUint32 KUdpMessageSize = 2048;
const TInt KTTLValue = 4;
const TInt KMicrosecond = 1000000;
const TInt KMXMaxValue = 120;
const TInt KInetAddrPrefixMatchLen = 16;

// ============================= LOCAL FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CleanupArray
// Used by TCleanupItem to clean a RPointerArray<TPtrC>
// -----------------------------------------------------------------------------
//
void CleanupArray( TAny* aArray )
    {
    ( reinterpret_cast<RPointerArray<TPtrC8>*>( aArray ) )->ResetAndDestroy();
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpUdpServer::CUpnpUdpServer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpUdpServer::CUpnpUdpServer( RSocketServ* aSocketServ,
                        MUpnpUdpServerObserver* aObserver,
                        TInt aListeningPort )
    : CActive( EPriorityStandard ),
      iMulticastFlags( 0 ),
      iMessagePtr( NULL, 0,0 ),
      iIsStarted( EFalse ),
      iSocketDown( EFalse )
      
    {
    LOGS( "SSDP *** CUpnpUdpServer::CUpnpUdpServer");

    CActiveScheduler::Add( this );
    iSocketServ = aSocketServ;
    iObserver = aObserver;
    iServerPort = aListeningPort;

    iRandomSeed = KErrNone;
    }

// -----------------------------------------------------------------------------
// CUpnpUdpServer::UdpConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpUdpServer::UdpConstructL()
    {
    iSendMessage = NULL;

    iPendingTimerMessages = new (ELeave) CArrayPtrSeg<CUpnpSsdpMessage>( 1 );

    // Create local timer
    User::LeaveIfError( iSendTimer.CreateLocal() );

    iConnectionManagerProxy = CUpnpConnectionManagerProxy::NewL( *iSocketServ );
    TInt error = iConnectionManagerProxy->EnsureStart();

    if ( error )
        {
        LOGS1( "CUpnpTcpServer::ListenL *** Error in attaching RConnection: error: %d",
            error );
        // Nothing else to do but leave, connection is not possible and
        // can't access network
        User::LeaveIfError( error );
        }

    iActiveIap = iConnectionManagerProxy->ActiveIap();
    iLocalInterfaceAddress = iConnectionManagerProxy->LocalAddress();

    RefreshLocalAddressInfo();
    }

// -----------------------------------------------------------------------------
// CUpnpUdpServer::~CUpnpUdpServer()
// Destructor.
// -----------------------------------------------------------------------------
//
CUpnpUdpServer::~CUpnpUdpServer()
    {
    LOGS( "SSDP *** CUpnpUdpServer::~CUpnpUdpServer");
    Cancel();
    Close();

    iSendTimer.Close();

    iSendRequestList.ResetAndDestroy();
    iSendRequestList.Close();

    delete iSendMessage;

    delete iMessage;

    if( iPendingTimerMessages )
        {
        // Go through the messages and cancel timers
        for(TInt i=0; i< iPendingTimerMessages->Count(); i++)
            {
            CUpnpSsdpMessage* arrayMsg = iPendingTimerMessages->At( i );
            if( arrayMsg )
                {
                // Cancel the timer in the message
                arrayMsg->CancelMessageTimeout();
                }
            }
        // Destroy the whole message array
        iPendingTimerMessages->ResetAndDestroy();
        delete iPendingTimerMessages;
        iPendingTimerMessages = NULL;
        }

    // close network connection
    delete iConnectionManagerProxy;
    }

// -----------------------------------------------------------------------------
// CUpnpUdpServer::JoinMulticastGroupL
// -----------------------------------------------------------------------------
//
void CUpnpUdpServer::JoinMulticastGroupL()
    {
    TIp6Mreq mreq;
    // the ipv4 multicast address that should use
    TInetAddr addr;
    addr.SetAddress( KDefaultMulticastAddress );
    addr.SetFamily( KAfInet );
    addr.ConvertToV4Mapped();

    mreq.iAddr = addr.Ip6Address();
    mreq.iInterface = 0;
    TPckgBuf< TIp6Mreq > opt( mreq );

    LOGS( "SSDP CUpnpUdpServer::JoinMulticastGroupL" );
    User::LeaveIfError( iSocket.SetOpt( KSoIp6JoinGroup, KSolInetIp, opt ) );

    }

// -----------------------------------------------------------------------------
// CUpnpUdpServer::ApplyMulticastSocketSettingsL
// -----------------------------------------------------------------------------
//
void CUpnpUdpServer::ApplyMulticastSocketSettingsL()
    {
    LOGS( "SSDP CUpnpUdpServer::ApplyMulticastSocketSettingsL" );

    User::LeaveIfError( iSocket.SetOpt( KSoIp6MulticastLoop, KProtocolInetIp, 0 ) );
    User::LeaveIfError( iSocket.SetOpt( KSoReuseAddr, KProtocolInetIp, 1 ) );

    JoinMulticastGroupL();
    }

// -----------------------------------------------------------------------------
// CUpnpUdpServer::ApplyCommonSocketSettingsL
// -----------------------------------------------------------------------------
//
void CUpnpUdpServer::ApplyCommonSocketSettingsL()
    {
    LOGS( "SSDP CUpnpUdpServer::ApplyCommonSocketSettingsL Socket TTL" );

    User::LeaveIfError( iSocket.SetOpt( KSoIpTTL, KSolInetIp, KTTLValue ) );
    User::LeaveIfError( iSocket.SetOpt( KSoIp6MulticastHops, KSolInetIp,
                                        KTTLValue ) );
    }

// -----------------------------------------------------------------------------
// CUpnpUdpServer::OpenL
// -----------------------------------------------------------------------------
//
void CUpnpUdpServer::OpenL( )
    {
    if ( !IsStarted() )
        {
        TRAPD( error, OpenSocketAndStartReceiveL() )
        if ( error )
            {
            LOGS1( "SSDP CUpnpUdpServer::OpenL error: %d", error );
            Close();
            User::Leave( error );
            }
        else
            {
            iIsStarted = ETrue;
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpUdpServer::OpenSocketAndStartReceiveL
// -----------------------------------------------------------------------------
//
void CUpnpUdpServer::OpenSocketAndStartReceiveL( )
    {
    LOGS( "SSDP CUpnpUdpServer::OpenSocketAndStartReceiveL");
    TInt error = iSocket.Open( *iSocketServ, KAfInet,
        KSockDatagram, KProtocolInetUdp, iConnectionManagerProxy->ConnectionL() );
    User::LeaveIfError( error );

    User::LeaveIfError( iSocket.SetLocalPort( iServerPort ) );
    // for multicast start
    if ( KDefaultMulticastPort == iServerPort )
        {
        ApplyMulticastSocketSettingsL();
        }

    ApplyCommonSocketSettingsL();

    TInetAddr bindAddr;
    error = iSocket.Bind( bindAddr ) ;
    if( KErrNone != error && KErrAlreadyExists != error )
        {
        LOGS( "SSDP CUpnpUdpServer::OpenSocketAnd.. bind error: %d");
        User::Leave( error );
        }

    ReceiveL();
    }

// -----------------------------------------------------------------------------
// CUpnpUdpServer::IsStarted
// -----------------------------------------------------------------------------
//
TBool CUpnpUdpServer::IsStarted()
    {
    return iIsStarted;
    }

// -----------------------------------------------------------------------------
// CUpnpUdpServer::SendL
// Send a message.
// -----------------------------------------------------------------------------
//
void CUpnpUdpServer::SendL( TDesC8& aBuffer, TInetAddr& anAddr )
    {
    LOGS( "SSDP *** CUpnpUdpServer::SendL");

    if ( !IsStarted() )
        {
        return;
        }

    if ( IsActive() && ( iState == ESendingTimer || iState == ESendDone ) )
        {
        CUpnpUdpSendRequest* req = CUpnpUdpSendRequest::NewL( aBuffer, anAddr );
        CleanupStack::PushL( req );
        iSendRequestList.AppendL( req );
        CleanupStack::Pop( req );
        }
    else
        {
        Cancel();

        delete iSendMessage;
        iSendMessage = NULL;
        iSendMessage = aBuffer.AllocL();

        iSenderAddress = anAddr;
        iSenderAddress.SetFamily( KAfInet );

        iSenderAddress.ConvertToV4Mapped();

        iSocket.SendTo( *iSendMessage,
                        iSenderAddress,
                        iMulticastFlags,
                        iStatus );

        iState = ESendingTimer; // in this state when RunL will be called after sending
                                // message successfuly, the delay timer will start
        SetActive();
        }
    }

// -----------------------------------------------------------------------------
// CUpnpUdpServer::SendDoneL
// Sending is done.
// -----------------------------------------------------------------------------
//
void CUpnpUdpServer::SendDoneL()
    {
    LOGS( "SSDP *** CUpnpUdpServer::SendDoneL");

    delete iSendMessage;
    iSendMessage = NULL;

    if ( iSendRequestList.Count() > 0 )
        {
        CUpnpUdpSendRequest* request = iSendRequestList[0];
        iSendRequestList.Remove(0);
        iSendRequestList.Compress();
        CleanupStack::PushL( request );
        SendL( *(request->iBuffer), request->iAddr );
        CleanupStack::PopAndDestroy( request );
        }
    else
        {
        ReceiveL();
        }
    }

// -----------------------------------------------------------------------------
// CUpnpUdpServer::ReceiveL
// Receive message.
// -----------------------------------------------------------------------------
//
void CUpnpUdpServer::ReceiveL()
    {
    LOGS( "SSDP *** CUpnpUdpServer::ReceiveL");

    if ( !IsActive() )
        {
        // If iMessage is not yet created, do it now.
        // This pointer will be used until SSDP server is closed down.
        if ( !iMessage )
            {
            iMessage = HBufC8::NewL( KUdpMessageSize );
            }

        // Set data length to zero for the next RecvFrom
        iMessage->Des().SetLength( 0 );

        iMessagePtr.Set( (unsigned char*) iMessage->Ptr(), 0, KUdpMessageSize );

        iClientAddr.SetPort( iServerPort );

        iSocket.RecvFrom( iMessagePtr, iClientAddr, iMulticastFlags, iStatus );

        SetActive();
        iState = EReceiving;
        }
    }

// -----------------------------------------------------------------------------
// CUpnpUdpServer::RunL
// -----------------------------------------------------------------------------
//
void CUpnpUdpServer::RunL()
    {
    TInt socketStatus = iStatus.Int();
    LOGS1( "SSDP *** CUpnpUdpServer::RunL - UDPServer error, error code: %d", socketStatus );
    
    if ( socketStatus != KErrNone )
        {
        if ( ( socketStatus == KErrConnectionTerminated ) || ( socketStatus == KErrCannotFindProtocol )
               || ( socketStatus == KErrDisconnected ) )
            {
            // When the WLAN newtork is lost or NCM cable is un-plugged, socket gets destroyed
            // and no send-receive operation can be performed over this socket.Hence simply
            // returning from RunL and setting the flag to true so that no further advertisement 
            // or search operations are carried over the dead socket.
            iSocketDown = ETrue;
            iLastSocketError = socketStatus;
            }
        else if ( socketStatus == KErrCancel )
            {
            // Send the pending request msgs 
            RestartOrContinueSendProcessingL();
            }
        else
            {
            ReceiveL();;
            }
        }
    else
        {
        switch ( iState )
            {
            case ESendingTimer:                                // Introduce delay between sending messages to prevent from
                // Introduce delay between sending messages to prevent from
                iSendTimer.Cancel();               
                iSendTimer.After( iStatus, KSendingDelay ); // SSDP notification storm.
                iState = ESendDone;
                SetActive();
                break;
            case EReceiving:
                {
                TUint temp = iClientAddr.Address();

                TUint port = iClientAddr.Port();

                TInetAddr addr = TInetAddr( temp, port );

                addr.SetFamily( KAfInet );
#ifdef _DEBUG
                TBuf8<KAddressLength> address;
                CUpnpHttpMessage::AddrOutput(addr, address);

                TBuf<KMaxName> logBuf;
                    logBuf.Copy( address.Left( logBuf.MaxLength() ) );
                LOGS1( "SSDP *** CUpnpUdpServer::RunL - Received buffer from: %S",
                    &logBuf );
#endif //_DEBUG

                CUpnpSsdpMessage* msg = NULL;

                TRAPD( err, msg = CUpnpSsdpMessage::NewL(
                    (TDesC8&) iMessagePtr, addr ) );
                if ( err )
                    {
                    delete msg;
                    msg = NULL;
                    LOGS( "SSDP *** CUpnpUdpServer::RunL - Ssdp message parse failed. Ingoring message.");
                    RestartOrContinueProcessingL();
                    return;
                    }
                CleanupStack::PushL(msg);
                // check that given message is valid.
                TInt error = ValidateSsdpMessageL( msg );
                if( error == KErrNone )
                    {

                    // no error, message is put to receive queue, and it gets
                    // handled when the timer expires.
                    // KErrNone used for constant, because it always remains
                    // the same.
                    if( msg->IsSsdpMSearch() )
                        {
                        CleanupStack::Pop( msg );

                        AddPendingTimerMessageL( msg );

                        // no delete, message in array
                        msg = NULL;
                        }
                    else
                        {
                        //checks wether the Source IP is in same network i.e. auto-IP(169.254.X.X) or non-auto-IP range

                        TInetAddr autoIPAddr = TInetAddr( KDefaultAutoIPAddress,
                                                          KDefaultMulticastPort );
                        autoIPAddr.SetFamily( KAfInet );

                        if ( iAutoIP && autoIPAddr.Match( addr, KInetAddrPrefixMatchLen ) )
                            {
                            LOGS( "SSDP *** MESSAGE_FROM_AUTO_VERIFIED_NETWORK");
                            iObserver->UdpMessageReceivedL( msg );
                            }
                        else if ( !autoIPAddr.Match( addr, KInetAddrPrefixMatchLen ) )
                            {
                            LOGS( "SSDP *** MESSAGE_FROM_NON_AUTO_VERIFIED_NETWORK");
                            iObserver->UdpMessageReceivedL( msg );
                            }

                        CleanupStack::PopAndDestroy(msg);
                        }
                    }
                else
                    {
                    // message not valid, delete it.
                    LOGS( "SSDP *** CUpnpUdpServer::RunL - Invalid SSDP message. Ingoring message.");
                    CleanupStack::PopAndDestroy(msg);
                    }

                RestartOrContinueProcessingL();

                }
                break;
            case ESending:
                delete iSendMessage;
                iSendMessage = NULL;
                RestartOrContinueProcessingL();
                break;

            case ESendDone:
                SendDoneL();
                break;

            default:
                // message not valid, delete it.
                LOGS( "SSDP *** CUpnpUdpServer::RunL - DEFAULT CASE, NOTHING TO DO!");
                break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpSsdpServer::RunError
// Active Object RunError called when RunL leaves.
// -----------------------------------------------------------------------------
//
TInt CUpnpUdpServer::RunError( TInt aError )
    {
LOGS1( "SSDP *** CUpnpUdpServer::RunError: %d", aError );
    TRAP_IGNORE( RestartOrContinueProcessingL() );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CUpnpUdpServer::DoCancel
// Active object DoCancel cancels active request.
// -----------------------------------------------------------------------------
//
void CUpnpUdpServer::DoCancel()
    {
    iSocket.CancelAll();
    iSendTimer.Cancel();    
    }

// -----------------------------------------------------------------------------
// CUpnpUdpServer::CloseL
// -----------------------------------------------------------------------------
//
void CUpnpUdpServer::Close()
    {
    LOGS( "SSDP *** CUpnpUdpServer::Close");

    if ( IsStarted() ) // socket can be not open correctly
        {
        Cancel();
        // for multicast start
        if(iServerPort == KDefaultMulticastPort)
            {
            TIp6Mreq mreq;

            TInetAddr addr;
            addr.SetAddress( KDefaultMulticastAddress );
            addr.SetPort(KDefaultMulticastPort);
            addr.SetFamily(KAfInet);
            addr.ConvertToV4Mapped();
            mreq.iAddr = addr.Ip6Address();

            mreq.iInterface = 0;
            TPckgBuf< TIp6Mreq > opt(mreq);
            iSocket.SetOpt(KSoIp6LeaveGroup, KSolInetIp, opt);
            }
        // for multicast end.
        }
    iSocket.Close();
    iIsStarted = EFalse;
    }

// -----------------------------------------------------------------------------
// CUpnpUdpServer::ValidateSsdpMessageL
// Function validates SSDP messages. It checks that correct headers are
// found etc.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CUpnpUdpServer::TSsdpValidity CUpnpUdpServer::ValidateSsdpMessageL( CUpnpSsdpMessage* aMsg )
    {
    CUpnpUdpServer::TSsdpValidity validity = CUpnpUdpServer::EMessageOk;

    // Message is search message
    if( aMsg->IsSsdpMSearch() )
        {
        // checking first line of message
        CUpnpHttpHeaderList* list=aMsg->HeaderList();
        CUpnpHttpHeader* header = list->First();
        if( header )
            {
            if(!CompareLinesL(header->Name(),UpnpSSDP::KMethodMSearch()))
                {
                // First line of message not "M-SEARCH * HTTP/1.1".
                // Invalid message.
               validity = CUpnpUdpServer::EHeaderNotMSearch;
                }
            }
        else
            {
            // no headers. Invalid message. no further handling.
            validity = CUpnpUdpServer::EHeadersMissing;
            return validity;
            }

        // no delete, owned by message.
        header=NULL;
        // no delete, owned by message.
        list=NULL;

        TDesC8& mxString = aMsg->GetHeaderValue( UpnpSSDP::KHdrMx() );

        // if no MX header found, not valid message.
        if( mxString != KNullDesC8() )
            {
            // try to parse value to TInt. If possible, then message is valid for MX check.
            TInt mxValue = 0;
            TInt errorInConversion = 0;
            TLex8 lex( mxString );
            errorInConversion = lex.Val(mxValue);
            // check that there are no errors in conversion. If error in conversion,
            // not valid SSDP MX header.
            if( errorInConversion != KErrNone || lex.Remainder().Length() > 0)
                {
                validity = CUpnpUdpServer::EErrorInMxConversion;
                }
            else if( mxValue < 0 )
                {
                // check that MX value is positive. Negative value not valid.
                validity = CUpnpUdpServer::EInvalidMXValue;
                }

            // Checking if MAN header is found.
            TDesC8& manString = aMsg->GetHeaderValue( UpnpSSDP::KHdrMan() );
            if( manString != KNullDesC8() )
                {
                // MAN header found. Checking that is has valid value.
                if( manString != UpnpSSDP::KNotificationDiscover() )
                    {
                    // MAN header value not "ssdp:discover". Invalid header.
                    validity = CUpnpUdpServer::EInvalidManHeader;
                    }
                }
            else
                {
                // MAN header not found. Invalid message.
                validity = CUpnpUdpServer::EInvalidManHeader;
                }

            // Check that ST header is found.
            TDesC8& stString = aMsg->GetHeaderValue( UpnpSSDP::KHdrSt() );
            if( stString == KNullDesC8() )
                {
                // no ST header found. invalid SSDP search.
                validity = CUpnpUdpServer::EInvalidStHeader;
                }

            TDesC8& hostString = aMsg->GetHeaderValue( UpnpSSDP::KHdrHost() );
            if( hostString != UpnpSSDP::KDefaultHost() )
                {
                // HOST header not "239.255.255.250:1900". Invalid message.
                validity = CUpnpUdpServer::EInvalidHostHeader;
                }
            }
        else
            {
            // no MX header.
            validity = CUpnpUdpServer::EInvalidMXValue;
            }
        }


    // Message is SSDP response
    else if( aMsg->IsSsdpResponse() )
        {
        // if message is SSDP response, header should
        // be "HTTP/1.1 200 OK".
        CUpnpHttpHeaderList* list=aMsg->HeaderList();
        CUpnpHttpHeader* header = list->First();
        if( header )
            {
            if (!CompareLinesL(header->Name(),UpnpHTTP::KHTTPOk()))
                {
                // First line of message not "HTTP/1.1 200 OK".
                // Invalid message.
                LOGS( "SSDP *** CUpnpUdpServer::EBadSsdpResponse");
                validity = CUpnpUdpServer::EBadSsdpResponse;
                }
            }
        else
            {
            // no headers. Invalid message. no further handling.
            LOGS( "SSDP *** CUpnpUdpServer::EHeadersMissing");
            validity = CUpnpUdpServer::EHeadersMissing;
            return validity;
            }

        // no delete, owned by message.
        header=NULL;
        // no delete, owned by message.
        list=NULL;

        TDesC8& cache = aMsg->GetHeaderValue( UpnpSSDP::KHdrCacheControl() );
        if( cache != KNullDesC8() )
            {
            TPtrC8 cachePtr;
            cachePtr.Set( cache );

            TInt posOfMaxAge = cachePtr.FindC( UpnpSSDP::KSsdpMaxAge() );
            TInt posOfEqual = cachePtr.FindC( UpnpString::KEqual() );
            if (posOfMaxAge != KErrNotFound && posOfEqual!= KErrNotFound
                                            && posOfMaxAge < posOfEqual)
                {
                cachePtr.Set( cachePtr.Right((cachePtr.Length()
                    - UpnpString::KEqual().Length()) - posOfEqual) );
                }

            TInt cacheValue = 0;
            TInt errorInConversion = 0;
            TLex8 lex( cachePtr );
            lex.SkipSpace();
            errorInConversion = lex.Val(cacheValue);
            // check that there are no errors in conversion.
            // If error in conversion, not valid
            // SSDP CACHE-CONTROL header.
            if( errorInConversion < KErrNone )
                {
                LOGS( "SSDP *** CUpnpUdpServer::EInvalidCacheControlHeader");
                validity = CUpnpUdpServer::EInvalidCacheControlHeader;
                }
            else if ( cacheValue < 0 )
                {
                // check that CACHE-CONTROL value is positive.
                // Negative value not valid.
                LOGS( "SSDP *** CUpnpUdpServer::EInvalidCacheControlHeader");
                validity = CUpnpUdpServer::EInvalidCacheControlHeader;
                }
            }
        else
            {
            // no CACHE-CONTROL header. Invalid message.
            LOGS( "SSDP *** CUpnpUdpServer::EInvalidCacheControlHeader");
            validity = CUpnpUdpServer::EInvalidCacheControlHeader;
            }

        }

    return validity;
    }

// -----------------------------------------------------------------------------
// CUpnpUdpServer::CompareLines
// Compare two line ignoring number of space between elements
// -----------------------------------------------------------------------------
//
TBool CUpnpUdpServer::CompareLinesL(const TDesC8& aValue, const TDesC8& aPattern)
    {
    TBool result = EFalse;
    TPtrC8 value(aValue);
    TPtrC8 pattern(aPattern);
    RPointerArray<TPtrC8> values;
    RPointerArray<TPtrC8> patterns;
    CleanupStack::PushL( TCleanupItem( CleanupArray, &values ) );
    CleanupStack::PushL( TCleanupItem( CleanupArray, &patterns ) );

    UpnpString::CutToPiecesL(value,UpnpString::KSpace()[0], values);
    UpnpString::CutToPiecesL(pattern,UpnpString::KSpace()[0], patterns);

    if (values.Count() == patterns.Count())
        {
        result = ETrue;
        for (TInt i = 0; i < values.Count(); i++)
            {
            if (patterns[i]->Compare(*(values[i])) != 0)
                {
                result = EFalse;
                break;
                }
            }
        }

    CleanupStack::PopAndDestroy( &patterns );
    CleanupStack::PopAndDestroy( &values );
    return result;
    }

// -----------------------------------------------------------------------------
// CUpnpUdpServer::RestartOrContinueSendProcessingL()
// -----------------------------------------------------------------------------
//
TBool CUpnpUdpServer::RestartOrContinueSendProcessingL()
    {
    if ( iSendRequestList.Count() > 0 )
        {
        CUpnpUdpSendRequest* request = iSendRequestList[0];
        CleanupStack::PushL( request );
        iSendRequestList.Remove( 0 );
        iSendRequestList.Compress();
        SendL( *( request->iBuffer ), request->iAddr );
        CleanupStack::PopAndDestroy( request );
        return ETrue;
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CUpnpUdpServer::RestartOrContinueProcessing
// -----------------------------------------------------------------------------
//
void CUpnpUdpServer::RestartOrContinueProcessingL()
    {
    if ( !RestartOrContinueSendProcessingL() )
        {
        ReceiveL();
        }
    }

// -----------------------------------------------------------------------------
// CUpnpUdpServer::MessageExpiredL
// Function gets called when message's timer expires. This means that this
// message should be sent.
// -----------------------------------------------------------------------------
//
void CUpnpUdpServer::MessageExpiredL( CUpnpHttpMessage* aMessage )
    {
    LOGS( "SSDP *** CUpnpUdpServer::MessageExpiredL" );

    CUpnpSsdpMessage* msg = static_cast<CUpnpSsdpMessage*>( aMessage );

    // going thru array and removing current message from it.
    for ( TInt i = 0; i < iPendingTimerMessages->Count(); i++ )
        {
        CUpnpSsdpMessage* arrayMsg = iPendingTimerMessages->At( i );
        if ( arrayMsg == msg )
            {
            // Remove message from array and compress array.
            iPendingTimerMessages->Delete( i );
            iPendingTimerMessages->Compress();
            break;
            }
        }
    // Prevents WLan lost MessageHandler panic 
    // don't remove because if this function leaves msg is destroyed. 
    // Msg has CActive responsible for RunError as member so it will be destroyed and therefore
    // CActiveSheduler will cause unhandled exception which is seen as messagehandler panic
    TRAP_IGNORE( iObserver->UdpMessageReceivedL( msg ) );
    delete msg; 
    }

// -----------------------------------------------------------------------------
// CUpnpUdpServer::AddressChangedL()
// Function gets called when the IP address of server is changed
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpUdpServer::AddressChangedL( TInetAddr& aAddr )
    {
    Close();
    iLocalInterfaceAddress = aAddr;
    RefreshLocalAddressInfo();
    OpenL();
    }

// -----------------------------------------------------------------------------
// CUpnpUdpServer::UpdateLocalAddressInfoL()
// -----------------------------------------------------------------------------
//
void CUpnpUdpServer::RefreshLocalAddressInfo()
    {
#ifdef _DEBUG
    TBuf<KAddressLength> buf;
    iLocalInterfaceAddress.Output( buf );
    LOGS1( "SSDP AddressChangedL IP: %S", &buf );
#endif //_DEBUG

    TInetAddr addr = TInetAddr( KDefaultAutoIPAddress, KDefaultMulticastPort );
    addr.SetFamily( KAfInet );

    if ( iLocalInterfaceAddress.Match( addr, KInetAddrPrefixMatchLen ) )
        {
        iAutoIP = ETrue;
        LOGS( "SSDP AddressChangedL AUTO_IP" );
        }
    else
        {
        iAutoIP = EFalse;
        LOGS( "SSDP AddressChangedL NO_AUTO_IP" );
        }
    }

// -----------------------------------------------------------------------------
// CUpnpUdpServer::CalculateMxValue()
// -----------------------------------------------------------------------------
//
TInt CUpnpUdpServer::CalculateMxValue( const TDesC8& aMxString )
    {
    TReal rand = Math::FRand( iRandomSeed );

    TLex8 lex( aMxString );
    TInt mxValue( 0 );

    TInt lexError = lex.Val( mxValue );
    if (( lexError != KErrNone ) || ( mxValue > KMXMaxValue )) // if delay in responsing more than 120 sec then it should be decreased until 120 sec
        {
        mxValue = KMXMaxValue;
        }

    // divide by two to ensure that the responses
    // come before the time expires
    TReal mx = rand * KMicrosecond * mxValue / 2;
    return mx;
    }

// -----------------------------------------------------------------------------
// CUpnpUdpServer::AddPendingTimerMessageL()
// -----------------------------------------------------------------------------
//
void CUpnpUdpServer::AddPendingTimerMessageL( CUpnpSsdpMessage* aMessage )
    {
    // only 20 messages allowed from one IP
    const TInt KMaxMessageNumberFromOneIP = 20;
    
    TInt cnt = 0;
    for ( TInt i = 0; i < iPendingTimerMessages->Count(); i++ )
        {
        CUpnpSsdpMessage* msg = iPendingTimerMessages->At( i );
        if ( msg->Sender().Address() == aMessage->Sender().Address() )
            {
            if ( ++cnt > KMaxMessageNumberFromOneIP )
                {
                delete aMessage;
                return;
                }
            }
        }

    CleanupStack::PushL( aMessage );

    // parsing value for wait timer
    TInt mxUse = CalculateMxValue(
                        aMessage->GetHeaderValue( UpnpSSDP::KHdrMx() ) );

    // set message timeout so when timout occurs, message
    // get's handled.
    aMessage->SetMessageTimeoutL( this, mxUse );

    // appending message to array
    iPendingTimerMessages->AppendL( aMessage );

    CleanupStack::Pop( aMessage );
    }

//  End of File

