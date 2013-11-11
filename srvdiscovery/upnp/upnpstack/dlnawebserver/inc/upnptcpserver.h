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
* Description:  Declares main application classes
*
*/


#ifndef C_CUPNPTCPSERVER_H
#define C_CUPNPTCPSERVER_H

// INCLUDES
#include "upnpcompvariant.hrh"

#include <e32base.h>
#include <es_sock.h>
#include <in_sock.h>
#include <commdbconnpref.h>

#include "upnpnotifytimer.h"
#include "upnpconnectionmanagerproxy.h"

// FORWARD DECLARATIONS
class CUpnpTcpSession;
class MUpnpHttpServerTransactionCreator;
class CUpnpDlnaFilter;


#ifdef RD_UPNP_REMOTE_ACCESS
class CUpnpIPFilterManager;
#endif

// CONSTANTS
static const TInt KMaxConnectionCount = 16;
const TInt KRetryTime = 1000000;
static const TInt KDefaultHttpPort = 50001; // HTTP default port number
// A Category value for a property of Publish and Subscribe
const TUid KdlnawebserverCat={0x10204165};
enum TdlnawebserverKeys {EdlnawebserverCounter};

// CLASS DECLARATION

/**
* A virtual base class for TCP servers. Includes a server socket and methods
* starting server socket, accepting connections and creating CUpnpTcpSessions for
* incoming connections. After creating tcp session and adding it to
* active sessions' tableserver returns to listening for new connections and
* the tcp session takes care about whole communication with remote host.
* After handling connection the session is destroyed and removed from sessions' list.
*
* @lib DLNAwebserver.lib
* @since Series60 2.6
*/
NONSHARABLE_CLASS (CUpnpTcpServer) : public CActive, public MUpnpNotifyTimerObserver
    {
public: // Enumerations

    /**
    * Enum for state of server.
    */
    enum TState
        {
        // Server is not listetnig for connections.
        ENotListening,
        // Server is listening and accepting incoming connections.
        EAccepting
        };

public: // Constructor and destructor

    /**
    * Constructor function. This constructor is used when connection is
    * iniatiated from network.
    * @since Series60 2.6
    * @param aSocketServ Pointer to Socket Server.
    * @param aPort Number of listening port.
    * @param aIap Number of active Iap to use with connections.
    * @return A new CUpnpTcpServer instance.
    **/
    CUpnpTcpServer( RSocketServ* aSocketServ,
                    TInt aPort,
                    TInt aIap );

    /**
    * TcpServer might-leave initialization method.
    * NOTE: Must be called by extending class
    * @since Series60 2.6
    **/
    void BaseConstructL();

    /**
    * Virtual destructor function.
    * @since Series60 2.6
    **/
    virtual ~CUpnpTcpServer();

public: // From CActive

    /**
    * Do active object operations
    * Standard active object function
    * @since Series60 2.6
    */
    void RunL();

    /**
    * Do active object operations
    * Standard active object function
    * @since Series60 2.6
    * @param aError from RunL.
    * @return KErrNone.
    */
    TInt RunError( TInt aError );

    /**
    * Cancel active object requests
    * Standard active object function
    * @since Series60 2.6
    */
    void DoCancel();

public: // From MUpnpNotifyTimerObserver

    /**
    * Callback from iRetrySocketTimer. This method tries to
    * open new socket again, because last time server
    * socket binding has failed.
    * @since Series60 2.6
    * @param aTimer Pointer to timer that has expired.
    **/
    void TimerEventL( CUpnpNotifyTimer* aTimer);

public: // New functions

    /**
    * Deletes given session.
    * @since Series60 2.6
    * @param aSession Session to be deleted.
    **/
    void DeleteSession( CUpnpTcpSession* aSession );

    /**
    * Opening socket for listening
    * @since Series60 5.0
    **/
    void OpenSocketL();

    /**
    * Starts to listen for incoming connections.
    * @since Series60 5.0
    **/
    void BindRandomPortL();

    /**
    * Starts to listen for incoming connections.
    * @since Series60 5.0
    * @param aPort try to bind to port
    **/
    void BindL( const TInt aPort );

    /**
    * Stops TcpServer.
    * @since Series60 2.6
    */
    void StopTcpServerL();
        
    /**
    * Set the new IP address
    * @since Series60 5.0 HN
    * @param aAddr IP address to be set.
    **/
    void SetServerAddress( const TInetAddr& aAddr );

    /**
    * Returns the IP address of this server.
    * @since Series60 2.6
    * @param aAddr IP address of this server is returned to this reference.
    **/
    void ServerAddress( TInetAddr& aAddr );

    /**
    * Returns the port of this server.
    * @since Series60 2.6
    * @return Port of this server.
    **/
    TUint ServerPort();

    /**
    * Returns the Hardware address of active IAP.
    * @since Series60 2.6
    * @return HW address.
    **/
    TSockAddr* HWAddressL();

    /**
    * Returns a new available port number.
    * @since Series60 2.6
    * @return A new Port number.
    **/
    TInt NewPortNumberL(); 	

	/**
    * Starts retrying accepting the new connection.
    * @since Series60 3.2
    **/
    void StartRetryTimer();

	/**
    * Sets file write buffer size
    * @since Series60 2.6
    **/
	void SetFileWriteBufferSize(TInt aSize);


	/**
    * Returns file read buffer size
    * @since Series60 2.6
    **/
	TInt FileWriteBufferSize();

	/**
    * Sets file read buffer size
    * @since Series60 2.6
    **/
	void SetFileReadBufferSize(TInt aSize);


	/**
    * Returns file write buffer size
    * @since Series60 2.6
    **/
	TInt FileReadBufferSize();


protected: // New functions

    /**
    * Callback function for new connection creation.
    * @since Series60 2.6
    * @param aSocket Socket that is bound to new connection.
    * @return Pointer to new CUpnpTcpSession.
    **/
    virtual CUpnpTcpSession* ConnectionAcceptedL( RSocket aSocket ) = 0;

    /**
    * Starts to listen a socket.
    * @since Series60 2.6
    * @return error code if listening start fails.
    **/
    TInt StartToListenL();

    /**
    * Accepts new connection ans starts to listen for new connections.
    * @since Series60 2.6
    **/
    void AcceptConnectionL();

    /**
    * Gets the value of iServerAddress
    * @param aAddr
    * @since Series60 3.2
    **/
    TBool GetServerAddress(TInetAddr& aAddr);

    /**
     * Connects to the Connection Manager
     * @since Series60 5.0
     **/
    void CreateConnectionManagerSessionL();
    
    /**
     * Closes sockets
     * @since Series60 5.0
     **/
    void CloseSockets();
    
     /**
     * Prepares to close sockets
     * @since Series60 5.0
     **/
    void PrepareToCloseSockets();
    
     /**
     * Cancels retry timer
     * @since Series60 5.0
     **/
    void CancelRetryTimer();

protected:

    // Connection that is used to bind the used socket server on
    // given interface.
    CUpnpConnectionManagerProxy *iConnectionManagerProxy;

    // State of server, owned
    TState iState;

    // Socket server that is used by TcpServer, owned
    RSocketServ* iSocketServ;

    // Listening socket, owned
    RSocket iServerSocket;

    // Socket that is used to bind new connections, owned
    RSocket iClientSocket;

    // List of sessions, owned
    RPointerArray<CUpnpTcpSession> iSessionList;

    // Listening port of this server, owned
    TUint iServerPort;

    // Address of this server, owned
    TInetAddr iServerAddress;

    // Used Iap, owned
    TInt iActiveIap;

    // Timer that is used when RSocket::Open() fails. This way
    // we try to bind listening server socket again later.
    CUpnpNotifyTimer* iRetrySocketTimer;

    // preference for connection
    TCommDbConnPref iCommDbPref;

  	// File write buffer size
  	TInt iFileWriteBufferSize;

  	// File read buffer size
  	TInt iFileReadBufferSize;

  	#ifdef RD_UPNP_REMOTE_ACCESS
    /*******IPFiltering *******/
    CUpnpIPFilterManager* iIPFilteringManager;
    /*******IPFiltering *******/
    #endif
    
    
    MUpnpHttpServerTransactionCreator* iTransactionCreator;
    
    };

#endif // C_CUPNPTCPSERVER_H

// End Of File
