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
* Description:  CUpnpHttpChunkFileTransferReader is a class responsible for 
*                asynchronous TCP reading and controlling the encoded data
*
*/



#ifndef C_CUPNPHTTPCHUNKFILETRANSFERREADER_H
#define C_CUPNPHTTPCHUNKFILETRANSFERREADER_H


#include <e32base.h>
#include <in_sock.h>
#include <f32file.h>


class CUpnpTcpSession;
class CUpnpHttpFileTransferBuffer;
class CUpnpHttpFileAccess;
class CUpnpHttpFileTransferReader;
class CUpnpHttpChunkParser;

/**
 *  CUpnpHttpChunkFileTransferReader is responsible for asynchronous reading.
 *  It reads chunked-encoded data and process it  
 *  @lib DLNAWebServer.lib
 *  @since Series60 3.2
 */
NONSHARABLE_CLASS (CUpnpHttpChunkFileTransferReader) : public CUpnpHttpFileTransferReader
    {

public: // Constructors and destructors
    
    /**
    * CUpnpHttpChunkFileTransferReader factory method.
    * @since Series60 3.2
    * @param aSession session that runs and supervises reader.
    * @param aSocket socket from which data will be read.
    * @param aPriority priority with which the reader will be working.
    * @param aReadPortion - size of the buffer to read from the socket
    * @param aBufferSize - the buffer size
    * @return new CUpnpHttpChunkFileTransferReader object's instance.
    */
    static CUpnpHttpChunkFileTransferReader* NewL( CUpnpTcpSession& aSession,
                                        RSocket& aSocket,
                                        TThreadPriority aPriority,                                        
                                        TInt aReadPortion,
                                        TInt aBufferSize);
    
    /**
    * Virtual destructor.
    * @since Series60 3.2
    */                             
    virtual ~CUpnpHttpChunkFileTransferReader();             
     

private: // Constructors and destructors

    /**
    * CUpnpHttpChunkFileTransferReader constructor.
    * @since Series60 3.2
    * @param aSession session that runs and supervises reader.
    * @param aSocket socket from which data will be read.
    * @param aPriority priority with which the reader will be working.
    * @param aHandle handle for logging for this thread.
    * @param aPriority priority with which the reader will be working.
    * @param aReadPortion - size of the buffer to read from the socket
    * @param aBufferSize - the buffer size
    */
    CUpnpHttpChunkFileTransferReader( CUpnpTcpSession& aSession,
                           RSocket& aSocket,
                           TThreadPriority aPriority,
                           TInt aReadPortion,
                           TInt aBufferSize);
    /**
    * By default Symbian 2nd phase constructor is private.
    * @since Series60 3.2
    */
    void ConstructL();
    
    /**
    * Remaing number of bytes to read.
    * @since Series60 3.2    
    * @return TInt
    */
    TInt RemainingBytes();
    
    /**
    * Starts reading in asynchronous mode.
    * @since Series60 3.2
    */
    void StartL();   

private:

    /**
    * Issues reading in asynchronous mode.
    * @since Series60 3.2
    */
    void RunL();
    
    /**
    * Invoked when RunL leaves.
    */
    TInt RunError( TInt aErr );
    
    /**
     * Parse - decodes chunked-encoding     
     * @since S60 3.2
     * @param buffer
     * @param position in the buffer
     * @return error code
     */
    TInt Parse( TDes8& aBuffer, TInt& aPos );
   
protected:

	/**
    * Bytes to be read from the socket
    * @since Series60 3.2    
    * @return TInt
    */
    TInt ReadBufferSize();   
    
    /**
    * Handle buffer of data
    * @since Series60 3.2    
    * @return TBool if all data handled
    */
    TBool HandleL();   
    
    /**
    * Finish the session
    * @since Series60 3.2
    */
    void Finish();  
    
    
    /**
    * Reads from socket
    * @since Series60 3.2    
    */
    void ReadFromSocket();
    
    /**
    * Resets the reader
    * @since Series60 3.2    
    */
    void Reset();

    /**
    * Handles read data - trims the buffer and save data to file
    * @since Series60 5.0
    * @return TInt 
    */
    TInt HandleReadDataL( TInt aPieceToRead );
                      
private: 

	//parser for chunked-encoding
    CUpnpHttpChunkParser* iParser;

    //decoded data
    TInt iDecoded;    
    };


#endif // C_CUPNPHTTPCHUNKFILETRANSFERREADER_H

// End Of File
