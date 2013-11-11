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
* Description:  Parses chunked-encoding content.
*
*/


#ifndef C_CUPNPHTTPCHUNKPARSER_H
#define C_CUPNPHTTPCHUNKPARSER_H

// INCLUDES
#include <e32base.h>
#include <f32file.h>

// FORWARD DECLARATIONS

// CONSTANTS

/**
 *  CUpnpHttpChunkParser:
 *	Parses chunked-encoding content.
 *  @lib dlnawebserver.lib
 *  @since S60 3.2
 */
NONSHARABLE_CLASS (CUpnpHttpChunkParser) : public CBase
		{
public: 
	//possible values of the context
    enum TChunkContext
        {
        EUnknown=0,
        EHeader,        
        EBody,
        ELastChunk,
        EExtension,
        ETrailer,
        EEndChunkBody,
        EError,
        EFinish,
        };


	// Constructor and destructor
    /**
    * Constructor function.
    * @since Series60 3.2 
    * @return A new CUpnpHttpChunkParser instance
    **/
    static CUpnpHttpChunkParser* NewL();

    /**
    * Virtual destructor function.
    * @since Series60 3.2 
    **/
    virtual ~CUpnpHttpChunkParser();
    
     /**
     * Parse - decodes chunked-encoding     
     * @since S60 3.2
     * @param buffer
     * @param position in the buffer
     * @return error code
     */
    TInt Parse(TDes8& aBuffer,TInt& aPos);
    
    /**
     * SetErrorState - set error state
     * @since S60 3.2          
     */
    void SetErrorState();

	/**
     * IsFinished - if decoding finished    
     * @since S60 3.2     
     * @return TBool
     */	
	TBool IsFinished();
		
	/**
     * Reset - resets parser for new decoding
     * @since S60 3.2          
     */
	void Reset();


  
protected: // Constructors

    /**
    * First phase constructor.
    * @param aSession Handle to session that owns CUpnpChunkFileTransferBuffer instance.
    * @param aToBeSaveInFile Should content be saved to the file.
    **/
    CUpnpHttpChunkParser();

    /**
    * By default Symbian 2nd phase constructor is private
    */
    void ConstructL();
	   
private:

   	/**
     * ParseBody - decodes chunk-body     
     * @since S60 3.2
     * @param buffer
     * @param position in the buffer
     * @return ETrue if all data parsed
     */
    TBool ParseBody(TDes8& aBuffer,TInt& aPos);
    
    /**
     * ParseHeader - decodes chunk-header    
     * @since S60 3.2
     * @param buffer
     * @param position in the buffer
     * @return ETrue if all data parsed
     */
    TBool ParseHeader(TDes8& aBuffer,TInt& aPos);
    
    /**
     * ParseExtension - decodes chunk-extension     
     * @since S60 3.2
     * @param buffer
     * @param position in the buffer
     * @return ETrue if all data parsed
     */
    TBool ParseExtension(TDes8& aBuffer,TInt& aPos);
    
    /**
     * ParseTrailer - decodes chunk-trailer    
     * @since S60 3.2
     * @param buffer
     * @param position in the buffer
     * @return ETrue if all data parsed
     */
    TBool ParseTrailer(TDes8& aBuffer,TInt& aPos);
    
    /**
     * ParseLastChunk - decodes last chunk     
     * @since S60 3.2
     * @param buffer
     * @param position in the buffer
     * @return ETrue if all data parsed
     */
    TBool ParseLastChunk(TDes8& aBuffer,TInt& aPos);
    
    /**
     * IsEmpty - checks if any data left     
     * @since S60 3.2
     * @param buffer
     * @param position in the buffer
     * @return ETrue if no data left
     */
    TBool IsEmpty(TDes8& aBuffer,TInt aPos);        
    
protected: 
	
    //Context of the parser
    TChunkContext iContext;
    //size of the current chunk
    TInt iChunkSize;
	//error in parsing    
    TInt iError;
    //already appended bytes of the current chunk
    TInt iBytesAppended;        
    
	};

#endif // C_CUPNPHTTPCHUNKPARSER_H

// End Of File
