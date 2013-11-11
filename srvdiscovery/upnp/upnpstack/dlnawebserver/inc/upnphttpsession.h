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
* Description:  HttpSession is class that is used to handle single 
*                 connections to HttpServer
*
*/


#ifndef C_CUPNPHTTPSESSION_H
#define C_CUPNPHTTPSESSION_H

// INCLUDES

#include <e32base.h>
#include <f32file.h>
#include <apasvst.h>
#include <apaflrec.h>
#include <e32std.h>

#include "upnphttpmessage.h"
#include "upnptcpsession.h"


// FORWARD DECLARATIONS

class CFileReader;
class CUpnpHttpBuffer;
class CUpnpHttpServer;
class CUpnpHttpFileAccess;
class CUpnpHttpServerTransaction;

// CONSTANTS

_LIT8( KMedia,  "\\MEDIA\\" );
_LIT8( KLog,    "\\LOG\\" );              
_LIT( KPercent,   "%" );

static const TInt KDefaultLastActionSize = 2048;
static const TInt KMaxRedirectCount=5;

// CLASS DECLARATION

/**
*  @brief HttpSession is class that is used to handle single connections to HttpServer.  
*
*  @since Series60 2.6
*/
NONSHARABLE_CLASS (CUpnpHttpSession) : public CUpnpTcpSession
    {
    
public: // Construtors and destructor

    /**
    * Constructor function. This constructor is used when connection is iniatiated
    *  from network.
    * @param aSocket Pointer to new socket.
    * @param aServer Pointer to owning HttpServer.
    * @param aSessionId Session id of this session.
    * @param aPriority Priority of this session
    * @return A new CUpnpHttpServer instance.
    **/
    static CUpnpHttpSession* NewL( RSocket aSocket, 
                               CUpnpHttpServer* aServer,
                               TInt aSessionId,
                               TThreadPriority aPriority );

    /**
    * Virtual Destructor function.
    */
    virtual ~CUpnpHttpSession();

public: // From CUpnpTcpSession
    /**
    * Callback function from TcpSession. Session has received aBuffer 
    *  from network.
    * @since Series60 2.6
    * @param aBuffer Buffer that has been received.
    * @return None.
    **/
    void BufferReceivedL( TDesC8& aBuffer );

public: // New functions

    /**
    * Sends aMessage to session's remote host.
    * @since Series60 2.6
    * @param aMessage Message to be sent.
    **/
    void SendL( CUpnpHttpMessage* aMessage );

    /**
    * Deletes current session.
    * @since Series60 2.6
    * @param aSession Session to b deleted.
    **/
    void DeleteThisSessionL( CUpnpTcpSession* aSession );
    
    /**
    * Deletes a file named aFilename if it exists. Used in receiving files
    * to delete file if it already exists.
    * @since Series60 2.6
    * @param aFilename File to be deleted.
    * @return None.
    **/
    void DeleteFileIfExists( TDesC16& aFilename );

    /**
    * Returns a pointer to file transfer object of this session.
    * @since Series60 2.6
    * @return Pointer to file transfer object.
    **/
    CUpnpHttpFileAccess* FileAccess();
       
    /**
    * Should the existing local file be overwritten with the requested content.
    *  If not then the new file with name: iInFilename_serialNumber will be created
    *  to save incoming content. It's connected only with download.
    * @return Should the local in-file be overwritten with incoming content.
    **/
    TBool OverwriteExisting();
    
    // Range request parameters, only used for GET and HEAD requests with Range HTTP header.
    /**
    * Returns offset of the file that should be requested from server. 
    * @return offset to save content at.
    **/
    TInt Offset();
    
    /**
    * Returns the length of file content to be requested.
    * @return Length of file content to be requested.
    **/
    TInt Length(); 
    
    /**
    * Returns if should the requested offset of the remote file be 
    *  saved at physical offset of existing local file.
    *  Only for Range requests
    * @return Should the requested offset of the remote file be saved at 
    *  physical offset of existing local file.
    */
    TBool SaveAtOffset();
    
    /**
    * Notifies upper layers that there is a HTTP error.
    * @since Series60 3.2
    * @param the HTTP error code
    **/
    void NotifyErrorL(TInt aError, TInt aInternalError = KErrNone);
    
    /**
    * Returns ETrue when message is totally downloaded [completed]
    * @since Series60 3.2
    * @return ETrue if message is completed
    **/
    TBool IsMsgReady();
    
    /**
    * Increments the redirect counter
    * @since Series60 3.2
    * @return ETrue if the number of Redirecting has exceeded the maximum
    **/
    TBool SetRedirectCount();

    /**
    * Returns HttpServer pointer
    * @since Series60 3.2
    **/    
    CUpnpHttpServer* HttpServer();
    
    /**
    * Deletes the FileAccess of this session.
    * @since Series60 2.6
    * @return None.
    **/ 
    void DeleteServeFileL();

protected: // New functions
    
    void DestroyIt(CUpnpTcpSession* aSession );
    
    /**
    * Returns ETrue, if fileserve exists.
    * @since Series60 2.6
    * @return ETrue, if fileserve exists; EFalse otherwise.
    **/
    TBool FileServeExists();
   
    /**
    * Returns the port of this HttpSession.
    * @since Series60 2.6
    * @return Number of server port.
    **/   
    TInt ServerPort();
    
private: // Constructors

    /**
    * First phase constructor. This constructor is used when connection is
    *  iniatiated from network.
    * @since Series60 2.6
    * @param aSocket Pointer to new socket.
    * @param aServer Pointer to owning HttpServer.
    * @param aSessionId Session id of this session.
    * @param aPriority Priority of this session
    **/
    CUpnpHttpSession( RSocket aSocket, 
                  CUpnpHttpServer* aServer, 
                  TInt aSessionId,
                  TThreadPriority aPriority );
    
    /**
    * Symbian constructor
    */
    void ConstructL();

    /**
    * Received buffer processing from network.
    * @since Series60 2.6
    * @param aBuffer Buffer that has been received.
    * @return None.
    **/
    void BufferReceivedProcessL( TDesC8& aBuffer );

/**
    * Received buffer checking headers for a message coming from network.
    * @since Series60 2.6
    * @param aBuffer Buffer that has been received.
    * @return None.
    **/
    void BufferReceivedCheckingHeadersL( TDesC8& aBuffer );


    /**
    * Single POST buffer processing
    *  
    * @since Series60 2.6
    * @param aBuffer Buffer that has been received.
    * @return None.
    **/
    void CUpnpHttpSession::SinglePostBufferReceivedL( TDesC8& aBuffer ); 
    
    

    TInt PrepareRangeHeaderL( CUpnpHttpMessage* aMsg,
        TBool aServingOutFileName,
        TInt aFileSize, CUpnpHttpServerTransaction &aTransaction, TInt& aRangeStatus,
        TInt& aStartPos, TInt& aEndPos );

    
private: // New funtions

    enum THttpInvalidMessage
        {
        EMessageOk = 0,
        EUnknownMethod
        };
    
    /**
    * Matches an error to a proper HTTP error code
    * @error to be matched
    * @return HTTP error code.
    **/
    TInt MatchHttpError( TInt error );
    /**
    * Checks message validity. Returns type of error.
    * @since Series60 2.6
    * @param aMsg Message to check.
    * @return Errorcode.
    **/
    THttpInvalidMessage IsValidMessage( CUpnpHttpMessage* aMsg );

    
    /**
    * Decides what to do with the message, should it be sent to upper layer,        
    * Takes ownership of a parameter (it should be popped from cleanup stack
    * before call
    * @since Series60 2.6
    * @param aMsg Message to check.
    **/
    void HandleQueryL( CUpnpHttpMessage* aMsg );


    /**
    * Repsonse with HTTP error mesage to remoteServer
    * @since Series60 2.6
    * @param aStatus Number of status code
    **/
    void ResponseErrorL(TInt aStatus);

    
  

    /**
    * Prepares serve a file. Checks that file exists and paths are correct. 
    *  Checks filesize and so on.
    * @since Series60 2.6
    * @param aMsg Message to check.
    * @return error code
    **/
    TInt PrepareToServeFileL( CUpnpHttpMessage* aMsg );

    /**
    * Notifies upper layers that session has disconnected.
    * @since Series60 2.6
    * @param aAddr Address of remote host to which the connection has been
    *  disconnected.
    **/
    void NotifyDisconnectL( TInetAddr aAddr );
    
    /**
    * Notifies upper layers that session has expired
    * @since Series60 3.2        
    **/
    void NotifyTimeoutL();
    
    /**
    * File Transfer Reader callback when upload done
    * @since Series60 3.2        
    **/
    void FileTransferReaderDoneL();
    
    /**
    * File Transfer Reader callback when upload with error
    * @since Series60 3.2        
    * @param error        
    **/    
    void FileTransferReaderErrorL(TInt aError);
    
    /**
    * Resets settings after upload
    * @since Series60 3.2            
    **/
    void ResetSettingsL();
    
    
    /**
    * File Transfer Writer callback when download done
    * @since Series60 3.2                 
    **/
    void FileTransferWriterDoneL();
    
    /**
    * File Transfer Writer callback for handling more downloads
    * @since Series60 3.2               
    **/
    void FileTransferWriterHandleMoreL();
    
    /**
    * Checks session and message against persistence
    * @since Series60 3.2                 
    **/
    void CheckSessionPersistence(CUpnpHttpMessage* aMsg);
    
    /**
    * Notifies upper layers that Socket Server is unable to handle any more data by \
    *  sending Expectation failed HTTP error to upper layers.
    * @since Series60 2.6
    * @param aAddr Address of remote host to which the connection has been tried to connect
    **/
    void SocketServerBusyL( const TInetAddr& aAddr );
    
     /**
    * Notifies upper layers with error message. Method guarantee destrustion of parameter
    * (even when leave ocure), and parameter should be poped from cleanupstack
    * @since Series60 2.6
    * @param aMessage message that will be sent to upper layer.
    **/
    void NotifyUpperLayerLD( CUpnpHttpMessage* aMessage );
    
    /**
    * Checks if session uses Connection close communication.
    * @since Series60 2.6
    **/ 
    void CheckConnectionCloseL();

    /**
    * Callback function. Informs upper layers that HTTP POST has been started 
    *  by sending HTTP error code 801 message to upper layers.
    * @since Series60 2.6
    **/
    void NotifyUpperLayersOnPostL();

    /**
    * Callback function. Informs upper layers that HTTP POST has been finished.
    * @since Series60 2.6
    **/
    void PostingFileCompleteL();
    
    void WriteMsgAndHeaderL( CUpnpHttpMessage* aMsg, const TDesC8& aHeaderToWrite,
        const TDesC& aPathWithNewMethod, TInt aFileSize, TInt aRangeStatus,
        const TInt aStartPos, const TInt aEndPos );
    
    /**
    * Function to cut Ip and port from absoulute Uri in get request, it only cuts them when 
    * they are same as http server ones
    * @since S60 5.1
    **/    
    TPtrC8 PrepareRelativeUriL( const TDesC8& aUri );
    
private: 

    // Request type of last sent message. This is used to store CHttpMessages type. 
    // This information is used by upper layers, not by HttpServer.
    THTTPMsgType iRequestType;

    // Resend counter. This information is used by upper layers, not by HttpServer.
    TInt iRetryCounter;

    // Pointer to owning HttpServer.
    CUpnpHttpServer* iServer;

    // Used to check that notification of receiving of large files has been 
    // sent on this session.
    TBool iRequestedTransferNotifySent;

    // Used to check if session uses Connection close.
    TBool iUsesConnectionClose;

    // Buffer that contains the filename of file, that is meant to be 
    // received and saved.
    HBufC8* iInFilename;
      
    // Should the existing local file be overwritten with the requested content.
    // If not then the new file with name: iInFilename_serialNumber will be created
    // to save incoming content. It's connected only with download.
    TBool iOverwriteExisting;
    
    /* Range request parameters, only used for GET and HEAD requests with 
       Range HTTP header. 
    */
    // Offset of the file that should be requested from server. 
    TInt iOffset;
    
    //Length of file content to be requested.
    TInt iLength; 
    
    // Should the requested offset of the remote file be saved at physical 
    // offset of existing local file.
    // Only for Range requests!
    TBool iSaveAtOffset;
    /**/

    // Buffer that contains the filename of file, that is meant to be sent 
    // out as body of CUpnpHttpMessage.
    HBufC8* iOutFilename;

    // Buffer that contains the name of destination service. Used by upper 
    // layer, not by CUpnpHttpServer
    HBufC8* iDestinationPath;

    // Buffer that contains the name of destination service. Used by upper
    // layer, not by CUpnpHttpServer
    HBufC8* iSenderPath;

    // Pointer used by upper layer, not by CUpnpHttpServer.
    TAny* iPendingRequest;

    // Requests that have not yet been served.
    RPointerArray<CUpnpHttpMessage>* iPendingRequests;

    // Pointer to this session's file access object.
    CUpnpHttpFileAccess* iFileServe;

    // Variable that is used to check if we can delete this session after 
    // we get response from remote end.
    TBool iSessionIsDeletedAfterResponse;
    
    // Flag informing if content-length of incoming message was checked
    // and it's value is not bigger than maximum size allowed.
    TBool iMaximumSizeChecked;
    
    // Flag informing session if message headers were checked
    TBool iHeadersCompleted;

    //error has been notified to the remote client
    TBool iErrorNotified;
    //if received message is chunked-encoded
    TBool iIsChunked;
    };

#endif // C_CUPNPHTTPSESSION_H

// End Of File
