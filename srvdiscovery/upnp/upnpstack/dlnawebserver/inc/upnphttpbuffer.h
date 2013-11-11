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
* Description:  HTTPBuffer is used to buffer incoming HTTP messages.
*
*/


#ifndef C_CUPNPHTTPBUFFER_H
#define C_CUPNPHTTPBUFFER_H

// INCLUDES

#include <e32base.h>
#include <in_sock.h>
#include "upnplist.h"
#include "upnphttpmessage.h"

// FORWARD DECLARATIONS
class CUpnpHttpSession;
class CUpnpHttpFileAccess;
class CUpnpHttpChunkParser;

// CONSTANTS

// CLASS DECLARATION

/**
*  @brief HTTPBuffer is used to buffer incoming HTTP messages.
*  @since Series60 2.6
*/
NONSHARABLE_CLASS (CUpnpHttpBuffer) : public CBase
		{
public: // Constructor and destructor
    /**
    * Constructor function.
    * @since Series60 2.6
    * @param aSession Pointer to session that owns this buffer.
    * @param aToBeSavedInFile ETrue, if incoming content should be saved 
    *        in a file, EFalse otherwise.
    * @return A new CUpnpHttpBuffer instance
    **/
    static CUpnpHttpBuffer* NewL( CUpnpHttpSession* aSession,TBool aToBeSavedInFile );

    /**
    * Virtual destructor function.
    **/
    virtual ~CUpnpHttpBuffer();

public: // New funtions

    /**
    * This function appends the aBuffer data at the end of this HTTPBuffer.
    * @since Series60 2.6
    * @param aBuffer Buffer to append after current data.
    * @param aToBeSavedInFile.
    **/

    TInt AppendL( TDesC8& aBuffer, TBool aToBeSavedInFile );
    /**
    * This function appends only headers from the aBuffer at the end of this HTTPBuffer.
    * @since Series60 2.6
    * @param aBuffer Buffer to append after current data.
    * @param aToBeSavedInFile.
    * @return position of double line feed
    **/
    TInt AppendHeadersL( TDesC8& aBuffer, TBool aToBeSavedInFile );

    /**
    * Decodes chunked messages.
    * @since Series60 2.6
    **/		
    TInt ParseL( TDesC8& aBuffer );

    /**
    * Returns ETrue, if buffer is ready to be converted to HTTPMessage,
    * else returns EFalse.
    * @since Series60 2.6
    * @return ETrue, if message is ready; EFalse otherwise.
    **/
    TBool IsReadyL();

    /**
    * Returns ETrue, if header part of HTTPMessage is ready, else returns
    * EFalse.
    * @since Series60 2.6
    * @return ETrue, if header ready; EFalse otherwise.
    **/
    TBool IsHeaderReady();

    /**
    * Returns ETrue, if body part of HTTPMessage is ready, else returns EFalse.
    * @since Series60 2.6
    * @return ETrue, if body ready; EFalse otherwise.
    **/
    TBool IsBodyReadyL();

    /**
    * Returns the length of entire buffer.
    * @since Series60 2.6
    * @return Length of buffer.
    **/
    TInt BytesInBuffer();

    /**
    * This function deletes aLength bytes from the beginning of HTTPBuffer.
    * @since Series60 2.6
    * @param aLength Number of bytes to remove.
    **/
    void RemoveL( TInt aLength );

    /**
    * Returns ETrue, if chunk encoded message is finished.
    * @since Series60 2.6
    * @return ETrue, if message is ready; EFalse otherwise.
    */	
    TBool ChunkTransferFinished();

    /**
    * Returns ETrue, if message has body, else returns EFalse.
    * @since Series60 2.6
    * @return ETrue, if message has body; EFalse otherwise.
    **/
    TBool HasBody();

    /**
    * Returns ETrue, if message's header contains 
    *	 "Transfer-encoding: Chunked" header.
    * @since Series60 2.6
    * @return ETrue, if message's header contains 
    *  "Transfer-encoding: Chunked" -header; EFalse otherwise.
    **/	
    TBool IsChunked();

    /**
    * Returns Headers of the first message in the buffer.
    * @since Series60 2.6
    * @return Message headers.
    **/
    TPtrC8 Headers();

    /**
    * Returns the content of buffer.
    * @since Series60 2.6
    * @return Message body.
    **/
    TPtrC8 Content();

    /**
    * Returns the content length of first message in the buffer.
    * @since Series60 2.6
    * @return Length of message.
    * @leave KErrOverflow if Content-Length > 2^32
    **/		
    TInt ContentLengthL();

    /**
    * Returns ETrue, if message's header contains "Connection: close" -header.
    * @since Series60 2.6
    * @return ETrue, if message's header contains "Connection: close" -header;
    *         EFalse otherwise.
    **/	
    TBool UsesConnectionClose();

    /**
    * Returns the error code of message in buffer.
    * @param aBuffer Buffer to check the error code (buffer that contains the)
    * HTTP message).
    * @since Series60 2.6
    **/	
    TInt MessageErrorCode( TDesC8& aBuffer );

    /**
    * Returns the method (GET, POST ...) of message in buffer.
    * @since Series60 2.6
    **/	
    TPtrC8 Method();

    /**
    * Returns the method target of message in buffer.
    * Target in this context means the url of message (like in GET / HTTP/1.1 the middle part)
    * @since Series60 2.6
    **/	
    TPtrC8 Target();
    
    /**
    * Returns True if message content is going to be saved in file.
    * @since Series60 2.6
    * @return ETrue if message content will be saved in file, EFalse if not.
    */
    TBool IsToFile();
    
    /**
    * Sets if body of message contained in buffer should be saved in file.
    * @since Series60 2.6
    * @param aToFile - should message content be saved in file.
    */
    void SetToFile( TBool aToFile );
    

private: // Constructors

    /**
    * First phase constructor.
    * @param aSession Handle to session that owns CUpnpHttpBuffer instance.
    * @param aToBeSaveInFile Should content be saved to the file.
    **/
    CUpnpHttpBuffer( CUpnpHttpSession* aSession, TBool aToBeSavedInFile );

    /**
    * By default Symbian 2nd phase constructor is private
    */
    void ConstructL();

    /**
     * Parse - decodes chunked-encoding     
     * @since S60 3.2
     * @param buffer
     * @param position in the buffer
     * @return error code
     */
    TInt Parse( TDes8& aBuffer, TInt& aPos );
    
protected: 

    // Buffer that contains the messages, owned
    RBuf8 iBuffer;

    // ETrue if message has content-type -header, EFalse otherwise, owned
    TBool iHasContentType;

    // ETrue, if content of message should be saved to a file; EFalse otherwise, owned
    TBool iToFile;

    // Pointer to the session that owns this CUpnpHttpFileAccess, owned
    CUpnpHttpSession* iSession;

    // If message is not chunked, contains the current length of message, owned
    TInt iContentLengthCounter;

    // If message is not chunked, contains the total length of message given
    // in Content-Length -header, owned
    TInt iContentLength;

    // Checking that responses with error codes ( = error code not 200)        
    // are not saved to files.
    TBool iErrorCodeCheckedForSaving;
    
    // ETrue, if it is allowed to save given buffer to file
    // EFalse if it is not allowed (HTTP error other than 200).
    TBool iOkToSaveToFile;
    
    TInt iFileReadBufferSize;
    
    // Position in buffer    
    TInt iPos;
    
    // Chunk parser
    CUpnpHttpChunkParser *iChunkParser;
	};

#endif // C_CUPNPHTTPBUFFER_H

// End Of File
