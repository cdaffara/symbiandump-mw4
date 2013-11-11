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
* Description:  Base class for HTTP servers. Includes HTTP specific
*                functionality and represent the highest layer of the HTTP stack.
*
*/


#ifndef C_CUPNPHTTPSERVER_H
#define C_CUPNPHTTPSERVER_H

// INCLUDES

#include <f32file.h>
#include "upnptcpserver.h"
#include "upnphttpserverobserver.h"
#include "upnphttpmessage.h"

// FORWARD DECLARATIONS
class CUpnpHttpFileAccess;
class CUpnpHttpSession;
class CUpnpHttpMessage;
class MUpnpHttpServerTransactionCreator;

// CLASS DECLARATION

/**
* Base class for HTTP servers. Includes HTTP specific functionality and represents
*  the highest layer of the HTTP stack.
*  Http Server derives from CUpnpTcpServer and adds HTTP related features as sending
*  HTTP messages, queueing messages, forwarding received messages and error codes
*  to observer layer etc.
* @lib DLNAWebServer.lib
* @since Series60 2.6
*/
NONSHARABLE_CLASS (CUpnpHttpServer) : public CUpnpTcpServer
    {
public: // Constructor and destructor

    /**
    * Constructor function.
    * @since Series60 2.6
    * @param aObserver Pointer to this HttpServer observer for callback
    *        functions.
    * @param aSocketServ Pointer to Socket server.
    * @param aActiveIap IAP to use with connection.
    * @param aHandle the thread log handle
    * @return A new CUpnpHttpServer instance.
    **/
    static CUpnpHttpServer* NewL( MUpnpHttpServerObserver* aObserver,
                                           RSocketServ* aSocketServ,
                                           TInt aActiveIap );

    
    /**
    * Virtual Destructor function.
    * @since Series60 2.6
    **/
    virtual ~CUpnpHttpServer();

public: // New funtions

    /**
    * Starts the HttpServer.
    * @since Series60 5.0
    * @param aPort HTTP make attempt to start on that port
    **/
    void StartL( const TInt aPort );

    /**
    * Stops the HttpServer.
    * @since Series60 2.6
    **/
    void Stop();

    /**
    * Forwards message to the upper layer.
    * @since Series60 2.6
    * @param aMsg Pointer to message.
    **/
    void ToReceiveStackD( CUpnpHttpMessage* aMsg );

    /**
    * Sends the message.
    * @since Series60 2.6
    * @param aMessage Pointer to message.
    * @return Errorcode.
    **/
    TInt SendMessageL( CUpnpHttpMessage* aMessage );

    /**
    * Returns Server's description string.
    * @since Series60 2.6
    * @return Server's description string
    **/
    TDesC8& ServerDescription();

    /**
    * This function sets handle to transaction creator interface.
    * @since Series60 5.0 
    * @param aTransactionCreator pointer to object implementing MUpnpHttpServerTransactionCreator
    *     interface.
    **/
    void SetTransactionCreator( MUpnpHttpServerTransactionCreator* aTransactionCreator );

    /**
     * Gets httpFilter
     */
    MUpnpHttpServerTransactionCreator* TransactionCreator();

   /**
    * This method returns the file session handle.
    * @since Series60 2.6
    */
    RFs& FileSession();

    /**
    * This method returns the connection handle.
    * @since Series60 2.6
    */
    RConnection& ConnectionL();

    
    /****IPFIltering*****/
    IMPORT_C void StartIPFilteringL();
    IMPORT_C void StopIPFiltering();
    /*************************/

public: // From MUpnpMessageObserver

    /**
    * Callback for timer in aMessage. This gets called when timer expires and message
    * is tried to be sent again.
    * @since Series60 2.6
    * @param aMessage Message to be sent.
    **/
    void MessageExpiredL( CUpnpHttpMessage* aMessage );

private: // From CUpnpTcpServer

    /**
    * Accept the incoming connection from listening socket, creates a session
    * and start new listening socket.
    * @since Series60 2.6
    * @param aSocket Socket that has received a new connection.
    * @return Pointer to a new incoming session.
    **/
    CUpnpTcpSession* ConnectionAcceptedL( RSocket aSocket );

private: // New functions

    /**
    * Function that is used for sending messages.
    * Function first tries to find if message should be sent using existing
    * session - it means that the message is response for request. If
    * session is not found then the new one is created and the message is sent
    * as a request.
    * @since Series60 2.6
    * @param aMessage Message to be sent.
    * @return If everything is ok, returns session id for that session. Else contains
    * error code (if value < KErrNone).
    */
    TInt TrapSendMessageL( CUpnpHttpMessage* aMessage );

     /**
    * Returns session
    * @since Series60 3.2
    * @param aSessionId
    * @return pointer to CUpnpHttpSession
    **/
    CUpnpHttpSession* GetSession( TInt aSessionId );

    /**
    * Increments the counter of redirecting and check if no. of redirections is not above the limit
    * @since Series60 3.2
    * @param aSessionId
    * @return ETrue - if the counter hasnt exceeded the maximum after incrementating it
    **/
    TBool NewRedirection( TInt aSessionId );

private: // Constructors

    /**
    * First phase constructor
    * @since Series60 2.6
    * @param aObserver handle to observer.
    * @param aSocketServ server socket used for accepting connestions.
    * @param aActiveIap number of IAP that should be used for connection.
    * @param aHandle the thread log handle
    **/
    CUpnpHttpServer( MUpnpHttpServerObserver* aObserver,
                     MUpnpHttpServerTransactionCreator* aCreator,
                     RSocketServ* aSocketServ,
                     TInt aActiveIap );

    /**
    * By default Symbian 2nd phase constructor is private.
    * @since Series60 2.6
    */
    void ConstructL();

private: // Data

    // Pointer to this server's observer, not owned.
    MUpnpHttpServerObserver* iObserver;

    // Handle to a file server session.
    RFs iFs;   

    // Bufer containing server's descriptions string used
    // for filling HTTP Server header value. Owned
    HBufC8* iServerDescription;
};

#endif // C_CUPNPHTTPSERVER_H

// End Of File
