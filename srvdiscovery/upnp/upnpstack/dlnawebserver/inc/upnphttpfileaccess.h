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
* Description:  UpnpHttpFileAccess is a class responsible for serving 
*                requested files to upload as well as for saving file that are 
*                being downladed.
*
*/


#ifndef C_CUPNPHTTPFILEACCESS_H
#define C_CUPNPHTTPFILEACCESS_H

// INCLUDES

#include <e32base.h>
#include <f32file.h>

// FORWARD DECLARATIONS
class CUpnpHttpSession;
  
// CLASS DECLARATION

/**
*  @brief CUpnpHttpFileAccess is used to send and receive files in Httpserver
*  and Httpsession.
*  @since Series60 2.6
*/
NONSHARABLE_CLASS (CUpnpHttpFileAccess) : public CBase
		{
public: // Constructors and destructor

    /**
    * Constructor function. This constructor is used when file is served by
    *  HttpServer.
    * @since Series60 2.6
    * @param aSession Pointer to owning session of this CUpnpHttpFileAccess.
    * @param aHeaderBuffer Buffer that contains the headers of message to 
    *  be served.
    * @param aFilename Name of file to be served.
    * @param aFileSize Size of file to be served.
    * @return A new CUpnpHttpFileAccess instance.
    **/
    static CUpnpHttpFileAccess* NewL( CUpnpHttpSession* aSession, 
                                          		const TDesC8& aHeaderBuffer, 
		                                        const TDesC16& aFilename, 
		                                          TInt aFileSize );

    /**
    * Constructor function. This constructor is used when file is to be received.
    * @since Series60 2.6
    * @param aSession Pointer to owning session of this CUpnpHttpFileAccess.
    * @param aFilename Name of file to be saved locally.
    * @param aHandle the thread log handle.
    * @return A new CUpnpHttpFileAccess instance.
    **/
    static CUpnpHttpFileAccess* NewL( CUpnpHttpSession* aSession, 
		                                          const TDesC16& aFilename );

    /**
    * Virtual Destructor function.
    **/
    ~CUpnpHttpFileAccess();

public: // New functions

    /**
    * Returns the number of bytes that has been transferred
    * @since Series60 2.6
    * @return Number of bytes that has been transferred.
    **/ 
    TInt TransferredBytes();

    /**
    * Returns the number of bytes that is to be transferred, ie size of file.
    * @since Series60 2.6
    * @return Number of bytes to be transferred.
    **/ 
    TInt TransferTotal();
    /**
    * Sets the reading position of the file.
    * @since Series60 2.6
    * @param aNewPos Position of reading, counting from start of file.
    **/
    void SetPosOfFile( TUint aNewPos );

    /**
    * Returns the reading position of the file.
    * @since Series60 2.6
    * @return Position of reading, counting from start of file.
    **/
    TUint PosOfFile();

		/**
    * Sets the offset of the local file at which should be downloaded content saved.
    * @since Series60 2.6
    * @param aNewPos Offset of the local file from the start of the file.
    **/
    void SetOffset( TUint aOffset );

   	/**
    * Sets the position where to stop reading the file.
    * @since Series60 2.6
    * @param aNewPos position where to stop reading the file, counting from start of file.
    **/
    void SetEndPosOfFile( TInt aNewEndPos );

    /**
    * Returns the position where to stop reading the file.
    * @since Series60 2.6
    * @return position where to stop reading the file, counting from start of file.
    **/
    TInt EndPosOfFile();

    /**
    * Sets the reading position of the headerbuffer.
    * @since Series60 2.6
    * @param aNewPos Position of reading, counting from start of headerbuffer.
    */  
    void SetPosOfHeader( TUint aNewPos );

    /**
    * Returns the filename of the served file.
    * @since Series60 2.6
    * @return Name of file.
    **/ 
    TDesC16& FileToServe();

       
    /**
    * Gets bytes to be sent
    * @since Series60 3.2
    * @param aPointer to write the read content
    * @param aBytesToSend Number of bytes to send.
    * @return ETrue, if transfer is finished; EFalse otherwise.
    **/   
    TBool GetL( TPtr8& aPointer, TInt aBytesToSend );

	/**
    * Saves aBuffer at the end of file used in this CUpnpHttpFileAccess.
    * @since Series60 3.2
    * @param aBuffer Content to save (append) to file.
    * @return error code
    **/ 
    TInt SaveL( TDesC8& aBuffer );

    /**
    * Sets the total size of file transfer (FileSize). 
    * @since Series60 2.6
    **/ 
    void SetTotalSize( TInt aSizeToSet );

    /**
    * Deletes file in error cases.
    * @since Series60 2.6
    **/ 
    void DeleteFile();
    
    /**
    * Returnes if currently handled file is downloaded or uploaded.
    * @since Series60 2.6
    * @return if currently handled file is downloaded or uploaded.
    **/
    TBool Download();
   
   /**
    * Returnes true if currently handled file is not removed.
    * @since Series60 3.2
    * @return TRUE if processed file exist.
    **/
    TBool FileExist();
    
    /**
    * Returnes the number of bytes written
    * @since Series60 3.2
    * @return TInt
    **/
    TInt BytesWritten();
    
    /**
    * Sets the encoding mode
    * @param TBool 
    * @since Series60 3.2
    **/
    void SetEncodingMode(TBool aIsChunked);
    
    /**
    * Returns the encoding mode
    * @since Series60 3.2
    * @return ETrue if chunked-encoded    
    **/
    TBool EncodingMode();
    
    /**
    * Sets the size of the transfer
    * @since Series60 3.2
    * @param TInt
    **/
    void SetTransferTotal(TInt aLength);
    
	/**
    * Gets the headers of content to be sent
    * @since Series60 3.2
    * @param TInt
    **/
    const TDesC8& GetHeaders();    
    
    /**
    * Called after headers were queued to be sent
    * @since Series60 3.2    
    **/
    void SetHeadersSent();  
    
    /**
    * True if HTTP headers sent
    * @since Series60 3.2    
    **/
    TBool HeadersSent();

    /**
    * True when chunk tranfer completes
    * @since Series60 3.2    
    **/
    void SetChunkCompleted( TBool aCompleted );

private: // Constructors

    /**
    * First phase constructor. This constructor is used when file is to be 
    * sent from HttpServer.
    * @since Series60 2.6
    * @param aFileSize Size of file to be served.
    **/
    CUpnpHttpFileAccess( TInt aFileSize );

    /**
    * First phase constructor. This constructor is used when file is to 
    * be received.
    * @since Series60 2.6
    **/
    CUpnpHttpFileAccess();

    /**
    * By default Symbian 2nd phase constructor is private.
    * @param aSession handle to parent session that owns this object.
    * @param aHeaderBuffer descriptor with message headers.
    * @param aFileName file name of the file to serve.
    */
    void ConstructL( CUpnpHttpSession* aSession, 
    								 const TDesC8& aHeaderBuffer, 
    								 const TDesC16& aFilename );

    /**
    * By default Symbian 2nd phase constructor is private.
    * @param aSession handle to parent session that owns this object.
    * @param aFileName file name of the file in which the incoming content is saved.
    */
    void ConstructL( CUpnpHttpSession* aSession, const TDesC16& aFilename );
    
    /**
    * Checks if whole file has been read - returns ETrue if yes
    * Close iTargetFile when returns ETrue
    */
    TBool IsEndOfFile();

private: // Data

    // Total Size of downloaded file, owned
    TInt iTotalSizeOfDownload;

    // Buffer that contains the headers of served message owned
    HBufC8* iHeaderBuffer;

    // Name of served file, owned
    HBufC16* iFileToServe;

    // Position to read or write file, owned
    TInt iPosInFile;

		// Position to stop read file at, owned
    TInt iEndPosOfFile;

    // Total Size of served file, owned
    TInt iSizeOfFile;

    // Position to read or write headerbuffer, owned
    TInt iPosToReadHeader;

    // Length of headers, owned
    TInt iHeaderLength;

    // Pointer to owning session, owned
    CUpnpHttpSession* iSession;

    // ETrue, if headers has been sent; EFalse otherwise, owned
    TBool iHeadersSent;

    // FileServer handle that is used to hold iFile open, owned
    RFs iFsSession;

    // File handle that is used for downloading and locking the 
    // active file, owned
    RFile iFile;

    // Boolean to detect if it is download or upload, owned
    TBool iIsDownload;
    
    RFile iTargetFile;
    TInt iOpen;
    HBufC* iOpenedFile;  
    //bytes already written
    TInt iBytesWritten;  
    //if data is chunked
    TBool iIsChunked;   
    //if chunk transfer is completed
    TBool iIsChunkCompleted;   
    //activated when resource closed and deleted
    TBool iIsDeleted;
  };

#endif //C_CUPNPHTTPFILEACCESS_H

// End Of File
