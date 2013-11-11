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
 * Description:  Declares main application class.
 *
 */

#ifndef C_CUPNPUDPSERVER_H
#define C_CUPNPUDPSERVER_H

// INCLUDES
#include <e32base.h>
#include <es_sock.h>
#include <es_enum.h>
#include <in_sock.h>
#include <commdbconnpref.h>
#include "upnpssdpmessage.h"
#include "upnpmessageobserver.h"
#include "upnpconnectionmanagerproxy.h"

// FORWARD DECLARATIONS
class CUpnpHttpMessage;
class CUpnpUdpSendRequest;

// CONSTANTS
static const TUint KDefaultStartPort = 2000;
static const TUint KDefaultMulticastPort = 1900;
static const TUint32 KDefaultMulticastAddress = INET_ADDR(239,255,255,250);
static const TUint KAddressLength = 35;
static const TInt KSendTimerLimit = 10;
static const TUint KSendingDelay = 15000; //DLNA specyfies that: UPnP network devices must not send more than 10 ssdp:alive messages
//on a single network interface in any given 200 ms period. So 15ms delay between each message
//should be sufficient 15ms = 15000 microseconds.
static const TUint32 KDefaultAutoIPAddress = INET_ADDR(169,254,255,255);

// DATA TYPES
typedef CUpnpHttpMessage CUdpMessage;

// CLASS DECLARATION

/**
 *  MUpnpUdpServerObserver is used as interface-class for message delivery
 *  from CUpnpUdpServer to CUpnpSsdpServer
 *
 *  @since Series60 2.0
 */
class MUpnpUdpServerObserver
    {
public:
    // New functions

    /**
     * Notifies udp message reception
     * @param aMessage UDP message received
     */
    virtual void UdpMessageReceivedL( CUdpMessage* aMessage ) = 0;

    };

// CLASS DECLARATION

/**
 *  CUpnpUdpServer is a base class for CUpnpSsdpServer and it contains all
 *  "real" methods, as CUpnpSsdpServer contains logical functions.
 *
 *  @since Series60 2.0
 */
NONSHARABLE_CLASS(CUpnpUdpServer) : public CActive,
        public MUpnpMessageObserver
    {
public:
    // Constructors and destructor

    /**
     * C++ default constructor.
     * @param aSocketServ Pointer to SocketServer used.
     * @param aObserver Pointer to class derived from MUpnpUdpServerObserver.
     * Used for callbacks.
     * @param aListeningPort Listening port.
     * @return A new CUpnpUdpServer instance.
     */
    CUpnpUdpServer( RSocketServ* aSocketServ,
        MUpnpUdpServerObserver* aObserver, TInt aListeningPort );

    /**
     * Destructor.
     */
    ~CUpnpUdpServer();

public:
    // New functions

    /**
     * Returns ETrue if udp server stared, EFalse otherwise
     * @since Series60 3.2
     * @return stared flag
     */
    TBool IsStarted();

    /**
     * Active Object RunL.
     * @since Series60 2.0
     */
    void RunL();

    /**
     * Callback for RunL leave situations.
     * @since Series60 2.0
     * @param aError contains the error code that caused the RunL leave.
     * @return KErrNone.
     */
    TInt RunError( TInt aError );

    /**
     * Active Object DoCancel.
     * @since Series60 2.0
     */
    void DoCancel();

    /**
     * Starts to listen for address.
     * @since Series60 2.0
     */
    void OpenL();

    /**
     * Closes the server.
     * @since Series60 2.0
     */
    void Close();

    /**
     * Sets the server ready for receiving.
     * @since Series60 2.0
     */
    void ReceiveL();

    /**
     * Sends aBuffer to IP address anAddr.
     * @since Series60 2.0
     * @param aBuffer Buffer that is to be sent.
     * @param anAddr Address that aBuffer is to be sent.
     */
    void SendL( TDesC8& aBuffer, TInetAddr& anAddr );

    /**
     * Prepares server for receiving after sending.
     * @since Series60 2.0
     */
    void SendDoneL();

    /**
     * From MUpnpAddressObserver
     * This function will be invoke if IP Address of UPnP changes
     */
    IMPORT_C void AddressChangedL( TInetAddr& aAddr );

protected:

    // DATA TYPES
    enum TSsdpValidity
        {
        // Message is valid SSDP message.
        EMessageOk,
        // Message is missing some essential headers.
        EHeadersMissing,
        // Name of method is not "M-SEARCH".
        EHeaderNotMSearch,
        // Value of MX header is not numerical.
        EErrorInMxConversion,
        // Value of MX header not valid.
        EInvalidMXValue,
        // Value of MAN header not valid.
        EInvalidManHeader,
        // Value of ST header not valid.
        EInvalidStHeader,
        // Value of HOST header not valid.
        EInvalidHostHeader,
        // Message is not valid SSDP response.
        EBadSsdpResponse,
        // Value of CACHE-CONTROL header not valid.
        EInvalidCacheControlHeader
        };

    /**
     * Checks that given SSDP message is valid.
     * @since Series60 2.0
     * @param aMsg Message to validate.
     * @return Error code. KErrNone, if valid message.
     */
    TSsdpValidity ValidateSsdpMessageL( CUpnpSsdpMessage* aMsg );

    /**
     * Function gets called when message's timer expires.
     * @since Series60 2.0
     * @param aMessage Pointer to message that has expired.
     */
    void MessageExpiredL( CUpnpHttpMessage* aMessage );

    /**
     * Function ompares  two line ignoring number of space between elements
     * @since Series60 2.0
     * @param aValue current value
     * @param aPattern pattern value
     * @return true if equal, else false
     */
    TBool CompareLinesL( const TDesC8& aValue, const TDesC8& aPattern );

    /**
     * Sets socket option which  adds address to multicast group
     */
    void JoinMulticastGroupL();

    /**
     * Sets socket configuration specific to multicast server
     */
    void ApplyMulticastSocketSettingsL();

    /**
     * Sets socket common configuration
     */
    void ApplyCommonSocketSettingsL();

    /**
     * Open socket and starts receiving datagrams
     */
    void OpenSocketAndStartReceiveL();

    /**
     * Refresh IP address and change auto ip status accordingly
     */
    void RefreshLocalAddressInfo();

private:

    /**
     * Function continue procesing queued request or switch to receive mode
     * @since Series60 3.2
     */
    void RestartOrContinueProcessingL();

    /**
     * Function continue send procesing
     * @return ETrue if processing restarted
     * @since Series60 3.2
     */
    TBool RestartOrContinueSendProcessingL();

    /**
     * Function calculates MX value
     * @return MX value
     * @since Series60 3.2
     */
    TInt CalculateMxValue( const TDesC8& aMxString );

    /**
     * Function queues response for M-Serach message 
     * @param Message to be sent after timer elapse
     * @since Series60 MCL
     */
    void AddPendingTimerMessageL( CUpnpSsdpMessage* aMessage );

protected:

    /**
     * Used from CUpnpSsdpServer to construct the CUpnpUdpServer correctly.
     * @since Series60 2.0
     */
    void UdpConstructL();

    // DATA TYPES
    enum TState
        {
        EClosed,
        EOpen,
        EReceiving,
        ESending,
        ESendingTimer, // new state introduced to handle delay timer between sending messages
        ESendDone
        };

protected:

    // State of CUpnpUdpServer defined as TState
    TState iState;

    // Random seed that is used for generating random timeouts
    // for sending SSDP responses.
    TInt64 iRandomSeed;

    // Address of remote client returned by RSocket::RecvFrom() function
    TInetAddr iClientAddr;

    // Address of remote host, e.g. destination of message
    TInetAddr iSenderAddress;

    // Flags of UDP message
    TUint iMulticastFlags;

    // List of messages that are waiting to be sent.
    RPointerArray<CUpnpUdpSendRequest> iSendRequestList;

    // Pointer to observer, CUpnpSsdpServer
    MUpnpUdpServerObserver* iObserver;

    // Pointer to RSocketServer, that is used to bind
    // sockets
    RSocketServ* iSocketServ;

    // Socket that is used to receive (and send) SSDP messages.
    RSocket iSocket;

    // Port number of CUpnpUdpServer
    TUint iServerPort;

    // Buffer that is used for receiving messages.
    HBufC8* iMessage;

    // Pointer to receive buffer.
    TPtr8 iMessagePtr;

    // Buffer that is used for sending
    HBufC8* iSendMessage;

    // Array that holds list of messages which are waiting
    // for sending after their timers have expired.
    CArrayPtrSeg<CUpnpSsdpMessage>* iPendingTimerMessages;

    // IAP ID of the connection
    TInt iActiveIap;

    // Connection for opened sockets
    CUpnpConnectionManagerProxy* iConnectionManagerProxy;

    // Timer used to introduce delays to prevent from sending
    // SSDP messages more often than one in 20ms time period.
    RTimer iSendTimer;

    // indicate if server is started, socket was opened correctly
    TBool iIsStarted;

    //Local IP Address of Server
    TInetAddr iLocalInterfaceAddress;
    
    // If the interface goes to AutoIp address
    TBool iAutoIP;
    
    // Stores the error code for the socket failure operations
    TInt iLastSocketError;
    
    // Indicates whether socket is alive or destroyed 
    TBool iSocketDown;
    };

#endif // C_CUPNPUDPSERVER_H
// End of File
