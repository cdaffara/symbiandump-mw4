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
* Description:  Declares HttpMessage class.
*
*/


#ifndef C_CUPNPHTTPMESSAGE_H
#define C_CUPNPHTTPMESSAGE_H

// INCLUDES
#include <e32base.h>
#include <in_sock.h>

#include "upnphttpheader.h"
#include "upnperrors.h"
#include "upnpnotifytimer.h"
//#include "upnpmessageobserver.h"

class CUpnpHttpHeaderList;
class MUpnpMessageObserver;


// FORWARD DECLARATIONS


// CONSTANTS
enum TTransferStatus
    {
    ETransferInProgress,
	EStopped,
	EError,
	ECompleted
    };

static const TInt KMaxHeaderLength = 100;
static const TInt KMaxSsdpMessageLength = 2048;
static const TInt KStatusLineMin = 13;

_LIT8(KHttpGet, "GET");
_LIT8(KHttpPost, "POST");
_LIT8(KHttpHead,"HEAD");
_LIT8(KHttpOptions,"OPTIONS");
_LIT8(KHttpPut, "PUT");
_LIT8(KHttpDelete, "DELETE");
_LIT8(KHttpTrace, "TRACE");
_LIT8(KHttpConnect, "CONNECT");
_LIT8(KHttp11, " HTTP/1.1");
_LIT8(KHttp11Ok, "HTTP/1.1 200 OK");
_LIT8(KHttp11Continue, "HTTP/1.1 100 Continue");
_LIT8(KHttpVersionPattern, "HTTP/*.*");
_LIT8(KHttp11WithoutSpace, "HTTP/1.1");
_LIT8(KHttp10, "HTTP/1.0");
_LIT8(KHigherLevelSintaticBreaks," \t;,");
_LIT8(KHttp8, "HTTP");
_LIT8(KHttpOkCode8, "200");
_LIT8(KResponseLinePattern,"*HTTP/?.? ??? *");
_LIT8(K1xxResponsePattern, "HTTP/1.1 1?? *");

//for SOAP
_LIT8(KEnvelope, "s:Envelope");
_LIT8(KSoapAction, "SOAPACTION");

// A Category value for a property of Publish and Subscribe
const TUid KUPnPUtilsCat={0x10204161};
enum TUPnPUtilsKeys {EUPnPUtilsCounter};

static const TInt KMaximumHTTPHeaderRowLength=200;
static const TInt KMaxMessageLength=8192;
static const TInt KLineLengthLimit=998;

enum THTTPMsgType 
	{
		EDeviceDescription = 1,
		EServiceDescription,
		EIcon,
		EAction,
		ESubscription,
		EUnSubscription,
		EStateVariableQuery,
		EStateVariableChanged,
        EResubscription,
        ETransferStart = 100,
        ETransferError,
        EImportComplete,
        EExportComplete
	};

// CLASS DECLARATION

/*! 
  \brief Base message class for all messages. Consists of HTTPHeader -list
  and body-buffer.
  
  Also contains Sessionid, DestinationPath and SenderPath. 
  SessionId identifies the HTTPSession this message belongs to.
  DestinationPath and SenderPath are used to identify actions in GENA messages.
  
  */
class CUpnpHttpMessage : public CBase, public MUpnpNotifyTimerObserver
	{

public:

	/**
	Constructor function.
	  
	@return A new CUpnpHttpMessage instance.
	**/
	IMPORT_C static CUpnpHttpMessage* NewL();

	/**
	Constructor function.
	  
	@param aAddr Destination ip address.
	@return A new CUpnpHttpMessage instance.
	**/
	IMPORT_C static CUpnpHttpMessage* NewL(const TInetAddr& aAddr);

	/**
	Constructor function.
	  
	@param aAddr Destination ip address.
	@param aSessionId Session id of the message.
	@return A new CUpnpHttpMessage instance.
	**/
	IMPORT_C static CUpnpHttpMessage* NewL(TInetAddr aAddr, TInt aSessionId);

	/**
	Constructor function.
	  
	@param aMessageBuffer Buffer which contains the message.
	@param aSender Sender's IP address.
	@return A new CUpnpHttpMessage instance.
	**/	
	IMPORT_C static CUpnpHttpMessage* NewL( TDesC8& aMessageBuffer, 
	                                    TInetAddr aSender);

	/**
	Constructor function.
	  
	@param aMessageBuffer Buffer which contains the message.
	@param aSender Sender's IP address.
	@param aSessionId Session id of the message.
	@return A new CUpnpHttpMessage instance.
	**/
	IMPORT_C static CUpnpHttpMessage* NewL( TDesC8& aMessageBuffer, 
	                                    const TInetAddr & aSender, 
	                                    TInt aSessionId);

	/**
	Destructor function.
	**/	
	IMPORT_C virtual ~CUpnpHttpMessage();

protected:

	/**
	First phase construct.
	**/	
	IMPORT_C CUpnpHttpMessage();

	/**
	Second phase construct.
	**/		
	IMPORT_C void ConstructL();

	/**
	Second phase construct.
	**/		
	IMPORT_C void ConstructL(TInetAddr& aSender, TInt aSessionId);

	/**
	Parses a HttpMessage object from aBuffer.
	@param aBuffer Buffer to parse.
	@return None.
	**/	
	IMPORT_C virtual void ParseL(TDesC8& aBuffer);

	/**
	Parses one row of headers.
	@param aBuffer Buffer that contains one header row ("Connection: close" 
	for example).
	@param aFirstLine ETrue if parsing first header row of Httpmessage, 
	EFalse otherwise. This is needed because first line has to be parsed 
	differently.
	@return None.
	**/	
	IMPORT_C void ParseHeaderRowL(const TDesC8& aBuffer, TBool aFirstLine);

	/**
	Append ContentLength when headers are dumped to string (using by public method HeadersToStringL)
	@param aHeaders without "Content-Length"
	**/	
	IMPORT_C virtual void AppendContentLengthToHeadersL(HBufC8*& aHeaders);

	/**
	Fold long header fields where is a "MUST limit 998"
	(including the header's name and value but excluding the last carriage-return/line-feed sequence, CRLF). 
	BODY portion of a header field can be split into a multiple	line representation.
	RFC2822 2.2.3  folding SHOULD be limited to placing the CRLF at higher-level syntactic breaks
	DLNA v1.5 7.4.14.3 HTTP headers that span multiple lines must prefix the additional lines with at least one space (SP) or horizontal tab (HT) as described in section 
	@param aUpperCaseHeader header which may be folded
	**/	
	IMPORT_C void FoldHeaderL(HBufC8*& aHeader);

	/**
	Unfolding header
	RFC2822 2.2.3 Unfolding from ONE header is accomplished by simply removing any CRLF that is immediately followed by WSP
	@param aUpperCaseHeader header which will be unfolded
	**/	
	IMPORT_C void UnFoldHeader(HBufC8*& aHeader);

protected: // from MUpnpNotifyTimerObserver

    IMPORT_C void TimerEventL( CUpnpNotifyTimer* aTimer );

public:

	/**
	Adds a Header row in this message.
	@param aName Name of the header.
	@param aValue Value of the header.
	@return None.
	**/
	IMPORT_C void AddPairL(const TDesC8& aName, const TDesC8& aValue);
	
	IMPORT_C void RemovePairL(const TDesC8& aName);
	
	/**
	Sets the body of message.
	@param aBody Buffer that contains the body for the message.
	@return None.
	**/	
	IMPORT_C void SetBodyL(const TDesC8& aBody);

	/**
	Returns the body of message.
	@return Message body.	
	**/
	IMPORT_C TDesC8& Body();

	/**
	Returns message as HBufC8.
	@return Buffer that contains message headers.
	**/	
	IMPORT_C virtual HBufC8* HeadersToStringL();

	/**
	Returns message as HBufC8.
	@return Pointer to message string.
	**/
	IMPORT_C virtual HBufC8* ToStringL();
	
	/**
	Sets the date of message.
	@param time Time to set in message.
	@return None.
	**/
	IMPORT_C void SetMessageDateL(const TTime & time);

	/**
	Returns the IP address of the sender of the message.
	@return IP address of the sender.
	**/	
	IMPORT_C TInetAddr& Sender();

	/**
	Returns the IP address of the receiver of the message.
	@return IP address of the receiver.
	**/	
	IMPORT_C TInetAddr& Receiver();
	
	/**
	Sets the DestinationPath of the message.
	@param aDestinationPath Destination Path to be set.
	@return None.
	**/			
	IMPORT_C void SetDestinationPathL(const TDesC8& aDestinationPath);
	
	/**
	Returns the DestinationPath of the message.
	@return Pointer to destination string.
	**/		
	IMPORT_C TPtrC8 DestinationPath();

	/**
	Sets the SourcePath of the message.
	@param aSourcePath Source Path to be set.
	@return None.
	**/
	
	IMPORT_C void SetSenderPathL(const TDesC8& aSourcePath);

	/**
	Returns the SenderPath of the message.
	@return Pointer to sender string.
	**/		
	IMPORT_C TPtrC8 SenderPath();
	
	/**
	Returns the SenderPath from Headers.
	@return Sender path.
	**/
	IMPORT_C const TPtrC8 SenderPathFromHeader();
	
	/**
	Sets the private member iPendingRequest.
	@param aPointer Pointer to CUpnpAction.
	@return None.
	**/
	IMPORT_C void SetPendingRequest(TAny* aPointer);

	/**
	Returns iPendingRequest pointer.
	@return PendingRequest pointer.
	**/			
	IMPORT_C TAny* PendingRequest();	

	/**
	Sets the filename where to save incoming content. The default behaviour is that the requested file is downloaded
	and if there is already file with name as the given one the requested content will be saved under the same name but
	with serial number prefix.
	@param aFilename Path and name of file where to save.
	@param aOverwriteExisting Should the existing file with the same name be overwritten.
			If flag is set to EFalse then the new file with name: aFilename_serialNumber will
			be created.
	@return None.
	**/	
	IMPORT_C void SetInFilenameL(const TDesC8& aFilename, TBool aOverwriteExisting = EFalse );
	
	/**
	Returns the name of file where to save incoming content.
	@return Returns a pointer to file name in which to save the message body.
	**/
	IMPORT_C TPtrC8 InFilename();
	
	/**
	Returns if the existing file should be overwritten with the requested content.
	If not then the new file with name: aFilename_serialNumber will be created.
	@return Should existing file be overwritten.
	**/
	IMPORT_C TBool OverwriteExisting();
	
	/**
	Sets the range of remote filename which will be requested. Calling this method involves sending GET request 
	with "Range" HTTP header, so this header should not be explicitly added by caller!
	If server doesn't support "Range" header then will probably respond with HTTP 200 Ok and the whole content,
	so observer will be notified with such a message instead of HTTP 206 Partial Content.
	@param aOffset Offset of the file that should be requested from server. 
	@param aLength Length of file content to be requested. Notice that if aOffset is set to some value 
			and aLength is set to 0 then the range from aOffset to the end of file will be requested.
			If both, offset and length are set to 0 then the range mode is off and whole file will be requested.
	@param aSaveAtOffset Should the requested offset of the remote file be saved at physical offset of
			existing local file. Notice that to save in local file at the same offset of the file the 
			following requirements should be met:
			- Local file with the name passed in aFilename parameter must exists.
			- In the file there must be already content at least from beginning of the file to the aOffset-1 position.
			- The flag aOverwriteExisting in SetInFilenameL method must be set to ETrue.
			If one of the mentioned requirements won't be met then the range content will be saved in new file
			at the beginning, if aOverwriteExisting is set to EFalse, or in the existing file at the beginning of 
			the file, if aOverwriteExisting is set to ETrue. 
	@return None.
	**/	
	IMPORT_C void SetRangeL( TInt aOffset, TInt aLength, TBool aSaveAtOffset );
	
	/**
	Returns the offset of the file that should be requested from server.
	@return Offset of the file that should be requested from server.
	**/
	IMPORT_C TInt Offset();
	
	/**
	Returns the length of file content to be requested.
	@return Length of file content to be requested.
	**/
	IMPORT_C TInt Length();
	
	/**
	Return if the requested offset of the remote file should be saved at physical offset of
	the local file.
	@return Should the requested offset of the remote file be saved at physical offset of existing local file. 
	**/
	IMPORT_C TBool SaveAtOffset();
	
	/**
	Sets the filename which will be used as message body.
	@param aFilename Path and name of file to use.
	@return None.
	**/	
	IMPORT_C void SetOutFilenameL(const TDesC8& aFilename);	

	/**
	Returns the name of file which will be set as body of message to send.
	@return Returns a pointer to file which will be used as message body.
	**/
	IMPORT_C TPtrC8 OutFilename();
	
	/**
	Returns value of the header.
	@param aName Name of the header to be returned.
	@return Value of the header.
	**/			
	IMPORT_C TDesC8& GetHeaderValue(const TDesC8& aName);

	/**
	Check if header exists, also return value of header if exists
	@param aHeaderName Name of the header.
	@param aExist True if header exist, false if not.
	@return Value of the header (if exist, otherwise KNullDesC).
	**/			
	IMPORT_C TDesC8& IsHeader(const TDesC8& aHeaderName, TBool& aExist);

	/**
	Sets messages Session id.
	@param aId Session id we want the message to have.
	@return None.
	**/
	IMPORT_C void SetSessionId(TInt aId);	

	/**
	Returns the SessionId of the message.
	@return SessionId of the message.
	**/
	IMPORT_C TInt SessionId() const;	

	/**
	Creates new, unused Session id value.
	@return new session id.
	**/
	IMPORT_C static TInt NewSessionIdL();

	/**
	Compares if two messages have same session id.
	@param aFirst First message to compare.
	@param aSecond Second message to compare.
	@return ETrue if same session id, EFalse otherwise.
	**/
	IMPORT_C static TBool SessionIdMatch( const CUpnpHttpMessage& aFirst, 
	                                      const CUpnpHttpMessage& aSecond);
	
	/**
	Returns length of message body.
	@return Length of message body.
	**/
	IMPORT_C TInt BodyLength() const;

	/**
	Returns the service used from http message, for example MediaServer:1 
    from Mediaserver:1/ContentDirectory.
	@return Pointer to device name.
	**/
	IMPORT_C const TPtrC8 DevicePath();

	/**
	Returns the service used from http message, for example ContentDirectory 
    from Mediaserver:1/ContentDirectory.
	@return Pointer to service name.
	**/
	IMPORT_C const TPtrC8 ServicePath() const;

	/**
	Checks if message is SOAP message.
	@return ETrue, if SOAP, EFalse otherwise.
	**/
	IMPORT_C TBool IsSoap() const;

	/**
	Checks if message is GENA message.
	@return ETrue, if GENA, EFalse otherwise.
	**/
	IMPORT_C TBool IsGena();

	/**
	Checks if message is a 1xx response.
	@return ETrue, if 1xx error, EFalse otherwise.
	**/
	IMPORT_C TBool Is1xx();

	/**
	Checks if message is a 2xx response.
	@return ETrue, if 2xx error, EFalse otherwise.
	**/
	IMPORT_C TBool Is2xx();
	
	/**
	Checks if message is a 3xx response.
	@return ETrue, if 3xx error, EFalse otherwise.
	**/
	IMPORT_C TBool Is3xx();
	
	/**
	Returns the method of message, like GET, POST etc.
	@return Pointer that points to message's method.
	**/
	IMPORT_C const TPtrC8 Method();

	/**
	Returns version of HTTP Request, like HTTP/1.1, HTTP/1.0 etc.
	@return Pointer that points to HTTP's version.
	**/
	IMPORT_C const TPtrC8 RequestHTTPVersion();

	/** 
	Returns a pointer to this message's headerlist.
	@return Pointer to this message's HTTPHeaderList.
	**/
	IMPORT_C CUpnpHttpHeaderList* HeaderList();

	/**
	Sets the type of message. Used by ControlPoint
	@param aType Error code numeral value.
	@return None.
	**/
	IMPORT_C void SetType(THTTPMsgType aType);

	/**
	Returns the Http message type, used by controlpoint.
	@return HTTP message type.
	**/
	IMPORT_C THTTPMsgType Type() const;

	/**
	Returns textual presentation of the given error.
	@param aError Error number.
	@return Textual presentation of given error code.
	**/			 
 	IMPORT_C static const TDesC8& UpnpError(TUpnpErrorCode aError);
 	
 	/**
	Returns textual presentation of the given error.
	@param aError Error number.
	@return Textual presentation of given error code.
	**/
	IMPORT_C static HBufC8* UpnpErrorL(TUpnpErrorCode aError);

	/**
	Returns the textual error message of given error code.
	@param aError Error code numeral value.
	@return Textual representation of error code.
	**/
	IMPORT_C static const TDesC8& HttpError(THttpStatusCode aError);

	/**
	Returns the error code of the message.
	@return Error code.
	**/
	IMPORT_C TInt Error();

	/**
	Sets the aBuf as string presentation of aAddr
	@param aAddr Address that will be converted.
	@param aBuf Buffer that will contain textual presentation of aAddr.
	@return None.
	**/
	IMPORT_C static void AddrOutput(const TInetAddr& aAddr, TDes8& aBuf);

	/**
	Sets the TInetAddr object value to given aBuf.
	@param aAddr Address that will contain same logical value as aBuf 
	after conversion.
	@param aBuf String we want to convert to TInetAddr.
	@return None.
	**/
	IMPORT_C static void AddrInput(TInetAddr& aAddr, const TDesC8& aBuf);

	/**
	Returns the current iRetryCounter value.
	@return Current counter value.
	**/
	IMPORT_C TInt RetryCounter();

	/**
	Sets the retry counter value
	@param aValue value to be set for RetryCounter.
	@return None.
	**/
	IMPORT_C void SetRetryCounter(TInt aValue);

	/**
    Sets the timer on message to expire in given time.
    Makes callback to MSsdpMessageObserver* after timeout.
	@param aObserver Pointer to message's timer observer.
    @param aTimeoutValue Number of microseconds to expire.
	@return Error code, KErrNone if no errors.
	**/
    IMPORT_C TInt SetMessageTimeoutL( MUpnpMessageObserver* aObserver, 
                                      const TInt aTimeoutValue );

	/**
    Cancels the timer on message.
	**/
    IMPORT_C void CancelMessageTimeout();

	/**
    Sets the priority for HTTP session used with this message.
    @param aPriority Priority of the session.
	**/
    IMPORT_C void SetHttpPriority( TThreadPriority aPriority );

	/**
    Returns the priority of HTTP session used with this message.
	**/
    IMPORT_C TThreadPriority HttpPriority();

	/**
    Sets the uri to find the local file.
	**/
    IMPORT_C void SetOutUriL( const TDesC8& aUri );

	/**
    Returns the value of OutUri.
	**/    
    IMPORT_C const TDesC8& OutUri();

	/**
    Sets if message is a local requests
    @param aLocal value indicating if message is local.
	**/
    IMPORT_C void SetLocal( TBool aLocal);

	/**
    Returns if message is a local requests.
	**/
    IMPORT_C TBool Local();  
    
    /**
    Sets if message is a client request
    @param aClientRequest value indicating if message is from the client.
	**/
    IMPORT_C void SetClientRequest( TBool aClientRequest);

	/**
    Returns if message is a client requests.
	**/
    IMPORT_C TBool ClientRequest();
    
    
    /**
	Find FIRST occurence of LWS    = [CRLF] 1*( SP | HT ) (RFC 2616 2.2 Basic Rules)
	RFC2822 2.2.3 Unfolding is accomplished by simply removing any CRLF that is immediately followed by WSP
	@param aText text which include many headers (some can be folded)
	@return The offset of the LWS sequence from the beginning of this descriptor's data. 
			KErrNotFound, if the data sequence cannot be found.
	**/	
	IMPORT_C static TInt FindLWS(const TDesC8& aText);
	
	   /**
    Sets a timeout for a session (valid only for TCP client sessions!)
    @param aTimeout value in microseconds.
	**/
    IMPORT_C void SetTcpTimeout( TInt aTimeout);

	/**
    Returns the value of TCP-session's timeout for a message.
	**/
    IMPORT_C TInt TcpTimeout();
    /**
    * Returns the internal error code for a message
	**/
    IMPORT_C TInt InternalError();
    
    /**
    * Sets the internal error code for a message
	**/
    IMPORT_C void SetInternalError(TInt aErrorCode);

protected:

	/**
	List of message headers.
	**/	
	CUpnpHttpHeaderList* iHeaderList;

	/**
	Variable used to route messages thru Dispatcher.
	**/		
	HBufC8* iDestinationPath;

	/**
	Variable used to route messages thru Dispatcher.
	**/	
	HBufC8* iSenderPath;
	
	/**
	Variable used to route messages to correct Http session.
	**/	
	TInt iSessionId;

	/**
	Variable used by controlpoint.
	**/	
	THTTPMsgType iType;

	/**
	Variable used by controlpoint.
	**/	
	TAny* iPendingRequest;

	/**
	Message body.
	**/	
	HBufC8* iTextBody;
	
	/**
	Destination of this message.
	**/	
	TInetAddr iRemoteHost;

	/**
	Buffer that contains the path and filename of file used as message 
	body when sending message.
	**/		
	HBufC8* iOutFilename;

	/**
	Buffer that contains the path and filename of file used to save 
	incoming data.
	**/		
	HBufC8* iInFilename;

	/**
	Counter that is used to track resending count, for example in 
	Control point.
	**/			
	TInt iRetryCounter;

	/**
	Priority that is used for HTTP session with this message.
	**/	
	TThreadPriority iSessionPriority;

	/**
	Timer that is used in SSDP messages to cause randomness in sending
	SSDP messages.
	**/
	CUpnpNotifyTimer* iTimer;

	/**
    Used for resolving url presentation to local filesystem file.
	**/
	HBufC8* iOutUri;

	/**
	Calls this observer's MessageExpiredL() function when iTimer
    expires.
	**/	
	MUpnpMessageObserver* iMessageObserver;
	
	/**
	Should the existing local file be overwritten with the requested content.
	If not then the new file with name: iInFilename_serialNumber will be created
	to save incoming content.
	**/
	TBool iOverwriteExisting;
	
	/**
	Offset of the file that should be requested from server. 
	**/
	TInt iOffset;
	
	/**
	Length of file content to be requested.
	**/
	TInt iLength; 
	
	/**
	Should the requested offset of the remote file be saved at physical offset of existing local file.
	**/
	TBool iSaveAtOffset;
	/**
	* Indicate whether request is local (it means send by local shortcut) or remote, 
	**/
	TBool iLocal;
	/**
	* Indicate to the upper layer that a message is a client request sent to our (this) host, if False it is ignored
	It should prevent confusing a client session with the server one by the upper layer
	**/
	TBool iClientRequest;
	/**
	* Indicates the session timeout value, used only in client sessions of TcpServer!
	It should prevent confusing a client session with the server one by the upper layer
	**/
	TInt iTcpTimeout;
	/**
	* internal error
	**/
	
	TInt iInternalError;
	
	};

#endif // C_CUPNPHTTPMESSAGE_H