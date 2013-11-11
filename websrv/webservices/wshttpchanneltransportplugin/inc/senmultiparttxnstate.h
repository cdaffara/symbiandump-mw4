/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: Header declaration
*
*/








#ifndef WSF_MultiPart_TXN_STATE_H
#define WSF_MultiPart_TXN_STATE_H

//  INCLUDES
#include "sentxnstate.h"
#include "senmultipartutils.h"


// CONSTANTS


	

// FORWARD DECLARATIONS

// CLASS DECLARATION
/**
 * Class for working with MultiPart messages - creates MultiPart request and parse MultiPart response
 * Implements the MHTTPDataSupplier interface.
 *
 */
class CSenMultiPartTxnState : public CSenTxnState
    {
public:  // Constructors and destructor

    
    /**
     * Virtual destructor.
     *
     */
    virtual ~CSenMultiPartTxnState();

    // New functions

    /**
     * Parses Response from server (MultiPart message) 
     * @param aDataPart Part of data come from server side
     */ 
    void ParseMultiPartResponseL(TDesC8& aDataPart);

    /**
     * Added HttpTransportProperties about Cids of Blobs and File names, 
     * where Blobs are kept and passes transport properties and XopEnvelope 
     * to an observer
     * @param aMultiPartContentType ContentType of MultiPart Response
     */
    void ResponseReceivedL(const SenMultiPartUtils::TMultiPartContentType& aMultiPartContentType);

    // Functions from base classes

    /**
     * From MHTTPDataSupplier::GetNextDataPart.
     * Obtain a data part from the supplier.  The data is guaranteed
     * to survive until a call is made to ReleaseData().
     * @param aDataPart The data part that sould be sent to server side.
     * @return ETrue if this is the last part, EFalse otherwise.
     */
    virtual TBool GetNextDataPart(TPtrC8& aDataPart);

    /**
     * From MHTTPDataSupplier::ReleaseData.
     * Release the current data part being held at the data
     * supplier.  This call indicates to the supplier that the part
     * is no longer needed, and another one can be supplied, if
     * appropriate.
     */
    virtual void ReleaseData();

    /**
     * From MHTTPDataSupplier::OverallDataSize.
     * Obtain the overall size of the data being supplied, if known
     * to the supplier.  Where a body of data is supplied in several
     * parts this size will be the sum of all the part sizes. If
     * the size is not known, KErrNotFound is returned; in this case
     * the client must use the return code of GetNextDataPart to find
     * out when the data is complete.
     *
     * @return A size in bytes, or KErrNotFound if the size is not known.
     */
    virtual TInt OverallDataSize();

    /**
     * From MHTTPDataSupplier::Reset.
     * Reset the data supplier.  This indicates to the data supplier
     * that it should return to the first part of the data.
     * This could be used in a situation where the data consumer has
     * encountered an error and needs the data to be supplied afresh.
     * Even if the last part has been supplied (i.e. GetNextDataPart
     * has returned ETrue), the data supplier should reset to the
     * first part. If the supplier cannot reset it should return an
     * error code; otherwise it should return KErrNone, where the
     * reset will be assumed to have succeeded.
     *
     * @return KErrNone if successful, otherwise another error code.
     */
    virtual TInt Reset();
    
    /**
     * From CSenTxnState
     * Creates and sets ContentType in MultiPart header
     * @param aSession
     * @param aHeaders
     */
    virtual void SetContentTypeHeaderL(const RHTTPSession aSession, RHTTPHeaders aHeaders) = 0;
    
    
protected:

    /**
     * C++ default constructor.
     *
     */
    CSenMultiPartTxnState(MSenResponseObserver& aObserver);

               
    // New functions

    /**
     * Creates Request for MultiPart message in MultiPart format 
     * should be used to send MultiPart message through HTTP Channel
     * @param aDataPart The data part that sould be sent to server side.
     * @return ETrue if this is the last part, EFalse otherwise.
     */
    TBool GetNextDataPartL(TPtrC8& aDataPart);
                                        
    /**
     * Calculates number of parts in BLOB (every part = 10KB) 
     * and how many bytes there are in the last part
     * @param aIndex Which Blob from an array should be calculated.
     */
    void SizeBinaryData(TUint aIndex);
    
    /**
     * Creates header for Blob in Request
     */
    void BlobHeaderL();
    
    /**
     * for case if Blob is kept in a file 
     * This function passes Blob from a file part by part (every part = 10KB)
     * in Request
     * @param aIndex Which Blob from an array should be passed.
     */
    void FileContainerL(TUint aIndex);
    
    /**
     * Creates the last part (MimeBoundaryEnd) in Request
     */
    void XopEndL();
    
    /**
     * Parses Response from server (MultiPart message) and extracts MimeBoundary part
     * and MimeBoundaryEnd part
     * @param aDataPartPtr DataPart come from server that to be parsed and after parsing 
     *                     changed aDataPartPtr will be used for future parsing 
     *                     (ParseMultiPartResponse() function)  
     * @param aOffset Global offset using since beginning of calling ParseMultiPartResponse(). 
     * @param aOffset1 Global offset1 (searches the end of MultiPart message)
                       using since beginning of calling ParseMultiPartResponse(). 
     */
    void MimeBoundaryInL(TPtrC8& aDataPartPtr, TInt& aOffset, TInt& aOffset1);
    
    /**
     * Parses Response from server (MultiPart message) and extracts XopEnvelope part
     * @param aDataPartPtr DataPart come from server that to be parsed and after parsing 
     *                     changed aDataPartPtr will be used for future parsing 
     *                     (ParseMultiPartResponse() function)  
     * @param aOffset Global offset using since beginning of calling ParseMultiPartResponse(). 
     * @param aOffset1 Global offset1 (searches the end of MultiPart message)
                       using since beginning of calling ParseMultiPartResponse(). 
     */
    void XopEnvelopeResponseL(TPtrC8& aDataPartPtr, TInt& aOffset, TInt& aOffset1);    

    /**
     * Parses Response from server (MultiPart message) and extracts header of Blob part
     * @param aDataPartPtr DataPart come from server that to be parsed and after parsing 
     *                     changed aDataPartPtr will be used for future parsing 
     *                     (ParseMultiPartResponse() function)  
     * @param aOffset Global offset using since beginning of calling ParseMultiPartResponse(). 
     * @param aOffset1 Global offset1 (searches the end of MultiPart message)
                       using since beginning of calling ParseMultiPartResponse(). 
     */
    void BlobHeaderResponseL(TPtrC8& aDataPartPtr, TInt& aOffset, TInt& aOffset1); 
    
    /**
     * Parses Response from server (MultiPart message) and extracts Blob part
     * and puts it in the file
     * @param aDataPartPtr DataPart come from server that to be parsed and after parsing 
     *                     changed aDataPartPtr will be used for future parsing 
     *                     (ParseMultiPartResponse() function)  
     * @param aOffset Global offset using since beginning of calling ParseMultiPartResponse(). 
     * @param aOffset1 Global offset1 (searches the end of MultiPart message)
                       using since beginning of calling ParseMultiPartResponse(). 
     */
    void BlobResponseL(TPtrC8& aDataPartPtr, TInt& aOffset, TInt& aOffset1);
    
        // New functions

                                        
 
    /**
     * Creates header for SoapEnvelope in Request
     */
    virtual void MessageHeaderL();
    
    /**
     * Creates SoapEnvelope in Request
     */
    virtual void MessageL();
    

protected: // Data

    
	// stores an array consisting Blobs (Request)
   	RArray<TXmlEngDataContainer> iBinaryDataList; // owned
   	
   	// stores XopEnvelope come from server side (Response)
   	RBuf8 iXopEnvelopeResponse;         // owned
   	
   	// stores SoapAction taken from HttpTransportProperties
    RBuf8 iSoapAction;                  // owned   
	
	// stores content-ID for root header (Request)
	RBuf8 iRootCid;                     // owned
	
	// stores MimeBoundary of MultiPart (Request)
	RBuf8 iMimeBoundaryOut;             // owned
	
	// stores MimeBoundary of MultiPart (Response)
	RBuf8 iMimeBoundaryIn;              // owned
	
	// stores MimeBoundaryEnd of MultiPart (Response)
	RBuf8 iMimeBoundaryEndIn;           // owned
	
	// stores header of Blob (Response)
	RBuf8 iBlobHeader;                  // owned  
    
    // stores number of a part that sould be sent (Request) 
    TUint iPartDataOut;
    
    // stores is it the last part or not (Request)
    TBool iLastPartOut;
    
    // stores number of a part that comes from server (Response)
    TUint iPartDataIn;
    
    // stores is it the last part or not (Response)
    TBool iLastPartIn;
    
    // stores how many parts there are in XOP (Request)
   	TUint iCountDataParts;
   	
   	// stores the data of current part that should be sent (Request) 
   	RBuf8 iCurrentPart;                 // owned
   	
   	// stores is it still CurrentPart or now goes a new one - 
   	//for big parts that HttpChannel can't get at once  (Request)
   	TBool iCurPart;
   	
   	// stores is it end of Blob (Request)
   	TBool iBinaryDataEnd;
   	
   	// stores status that size of Blob was calculated/wasn't calculated (Request)
 	TBool iChecked;
 	
 	// stores which part of Blob goes to the server (Request)
 	TUint iBinaryDataPart;
 	
 	// stores number of Blob parts (calculated in SizeBinaryData() function)
 	TUint iBinaryDataParts;
 	
 	// stores hom many bytes there are in the last Blob part (calculated in SizeBinaryData())
 	TUint iBinaryDataRest;
 	
 	// stores was already created File for incoming Blob or not (Response)
 	TBool iFirst;
 	
 	// stores array of Cids of incoming Blobs (Response)
 	RArray<RBuf8> iCids;                // owned
 	
 	// stores array of file names of incoming Blobs (Response)
 	RArray<RBuf8> iFileNames;           // owned
 	
 	MSenMessage* iMessage;
    };

#endif // WSF_MultiPart_TXN_STATE_H

// End of File
