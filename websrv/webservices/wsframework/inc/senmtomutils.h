/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Header declaration
*
*/




#ifndef SEN_MTOM_UTILS_H
#define SEN_MTOM_UTILS_H

#include <e32math.h>


#include <SenSoapEnvelope2.h>
#include <RSenDocument.h>

#include <xml/dom/xmlengserializer.h>
#include <xml/dom/xmlengchunkcontainer.h>
#include <xml/dom/xmlengfilecontainer.h>
#include <xml/dom/xmlengbinarycontainer.h>
#include <xml/dom/xmlengnodelist.h>

// CONSTANTS
_LIT(KMtomNoRequestSoapEnvelopePanicText, "NULL MTOM request soap envelope");
_LIT(KMtomNoBlobsPanicText, "NULL MTOM BLOB part");
_LIT(KMtomNoContentTypeInBlobHeaderPanicText, "NULL MTOM ContentType in BLOB header");
_LIT(KMtomNoCidPanicText, "NULL MTOM Cid");
_LIT(KMtomSoapVersionInvalidPanicText, "MTOM SOAP version unknown");
_LIT(KMtomBlobContainerTypeInvalidPanicText, "MTOM BLOB container type unknown");
_LIT(KMtomResponseBodyInvalidPanicText, "MTOM response body doesn't conform XOP rules");
_LIT(KMtomCidInvalidPanicText, "MTOM Cid doesn't conform XOP rules");

_LIT(KMtomPanicText, "MTOM panic");

const TUint KTenKBytes = 10240;

/** numbers 11 or 12 at the end of constant names mean SOAP versions (1.1 or 1.2) */
_LIT8(KHeaderRootStart11, "Content-Type: application/xop+xml;charset=UTF-8;type=\"text/xml\"\r\nContent-Transfer-Encoding: 8bit\r\nContent-ID: <");
_LIT8(KHeaderRootStart12, "Content-Type: application/xop+xml;charset=UTF-8;type=\"application/soap+xml\"\r\nContent-Transfer-Encoding: binary\r\nContent-ID: <");
_LIT8(KHeaderRootEnd, ">\r\n\r\n");    

_LIT8(KContentTypeName, "contentType");
_LIT8(KHeaderBinaryDataContentType, "Content-Type: ");
_LIT8(KHeaderBinaryDataContentID, "\r\nContent-Transfer-Encoding: binary\r\nContent-ID: <");
_LIT8(KHeaderBinaryDataEnd, ">\r\n\r\n"); 

_LIT8(KNewLine, "\r\n");
_LIT8(KDush, "--");

/** the common part of file name where will be stored Blobs from Response */
_LIT(KFileName, "c:\\SenBlobResponses\\SenBlobResponse");

/**
 * Prepares XOP message with binary data to transmit it through HTTP Channel
 * and extracts BLOBs from XOP message responsed through HTTP channel 
 *
 * @lib
 * @since S60 v3.2
 */
class SenMtomUtils
	{
public:

    /**
     * different cases of panics
     */
     enum TMtomPanics
	    {
	    EMtomNoRequestSoapEnvelope,
	    EMtomNoBlobs,
	    EMtomNoContentTypeInBlobHeader,
	    EMtomNoCid,
	    EMtomSoapVersionInvalid,
	    EMtomBlobContainerTypeInvalid,
	    EMtomResponseBodyInvalid,
	    EMtomCidInvalid
	    };

    /**
     * ContentType for MTOM consists of 
     * FiledName = "Content-Type" FiledValue = "Multipart/Related"
     * ParamName1 = "boundary" 	  ParamValue1 = BoundaryValue
     * ParamName2 = "type"  	  ParamValue1 = TypeValue
     * ParamName1 = "start" 	  ParamValue1 = StartValue
     * ParamName1 = "start-info"  ParamValue1 = StartInfoValue
     * ParamName1 = "action" 	  ParamValue1 = ActionValue
     * This structure describes ParamName & ParamValue of ConetentType
     */ 
    struct TMtomContentTypeParam
        {
        TPtrC8 paramName;
        TPtrC8 paramValue;
        };	    
    
    /**
     * This structure describes FieldValue & Params of ContentType
     */
    struct TMtomContentType
        {
        TPtrC8 fieldValue;
        RArray<TMtomContentTypeParam> params;
        };

public:

    /**
     * Serializes SoapEnvelope message in XOP message
     * @since S60 v3.2
     * @param aSoapEnvelope2 The message with multiparts (usual view of Soap message)
     * @param aXopEnvelope The message serialized from usual view to XOP message
     */	
	IMPORT_C static void SerializeEnvelope2L(CSenSoapEnvelope2& aSoapEnvelope2, 
	                                         RBuf8& aXopEnvelope);
	
    /**
   	 * Generates random content-ID for header of XOP message and for headers of BLOBs 
     * as randomNumber@homeTime
   	 * @since S60 v3.2     
   	 * @param aRootCid Generated content-ID
   	 */
 	IMPORT_C static void GenerateRandomRootCidL(RBuf8& aRootCid);
 	
 	/**
	* Generates random boundary for mime header as randomNumber
	* @since S60 v3.2
	* @param aBoundary Generated random boundary
	*/
 	IMPORT_C static void GenerateRandomBoundaryL(RBuf8& aBoundary);
 	
 	/**
 	* Generates MimeBoundary to separate mime parts of the message as --randomNumber
 	* @since S60 v3.2
 	* @param aBoundary random boundary
 	* @param aBoundaryStart Generated MIME boundary
 	*/
 	IMPORT_C static void BoundaryLineStartL(const RBuf8& aBoundary, RBuf8& aBoundaryStart);
 	
 	/**
 	* Generates MimeBoundaryEnd to end multipart message (MTOM) as --randomNumber--
 	* @since S60 v3.2
 	* @param aBoundary random boundary
 	* @param aBoundaryEnd Generated MIME boundary end
 	*/
  	IMPORT_C static void BoundaryLineEndL(const RBuf8& aBoundary, RBuf8& aBoundaryEnd);

    /**
     * Creates Header for Root of XOP message
     * The header sould be the next:
     *
     * --MIME_boundary
     * Content-Type: application/xop+xml; 
     * charset=UTF-8; 
     * type="text/xml" (for SOAP 1.1) or "application/soap+xml" (for SOAP 1.2)
     * Content-Transfer-Encoding: 8bit
     * Content-ID: <randomNumber@homeTime>
     *
     * where "Content-ID" is the same as "start" in Header of Outer Package
     *
     * @since S60 v3.2
     * @param aSoapEnvelope2 The message with multiparts (usual view of Soap message)
     * @param aRootCid The content ID for the header of XOP
     * @param aHeaderRoot11  Header of root of XOP message
     */
    IMPORT_C static void HeaderRootL(CSenSoapEnvelope2& aSoapEnvelope2,
    							     const RBuf8& aRootCid, RBuf8& aHeaderRoot);
	
    /**
     * Creates Header for Binary Data[aIndex] of XOP message 
     * The header sould be the next:
	 *
     * --MIME_boundary
	 * Content-Type: image/png
	 * Content-Transfer-Encoding: binary
	 * Content-ID: <randomNumber@homeTime>
	 *
	 * @since S60 v3.2
     * @param aIndex Which BLOB from array of TXmlEngDataContainer is processed, starts from 0
     * @param aBinaryDataList The list of BLOBs
     * @param aHeaderBinaryData Header of binary data[aIndex]
     */
    IMPORT_C static void HeaderBinaryDataL(TUint aIndex, 
									       RArray<TXmlEngDataContainer>& aBinaryDataList,
			 						 	   RBuf8& aHeaderBinaryData);

    /**
     * Extracts a part of BLOB (10KB) from a file
     * @since S60 v3.2
     * @param aPart Which part of BLOB should be extracted
     * @param aIndex Which BLOB from array of TXmlEngDataContainer is processed, starts from 0
     * @param aBinaryDataList The list of BLOBs
     * @param aBinaryData Part of BLOB that was extracted from a file
     */
    IMPORT_C static void FileDataPartL(TUint aPart, TUint aIndex, 
								       RArray<TXmlEngDataContainer>& aBinaryDataList,
						       	       RBuf8& aBinaryData);									 

    /**
     * Extracts the rest of BLOB (less than 10KB) from a file
     * @since S60 v3.2
     * @param aParts How many parts of BLOB has already been extracted from a file
     * @param aRest How many bytes should be exracted from a file 
     * @param aIndex Which BLOB from array of TXmlEngDataContainer is processed, starts from 0
     * @param aBinaryDataList The list of BLOBs
     * @param aBinaryData The last part of BLOB that was extracted from a file
     */
    IMPORT_C static void FileDataRestL(TUint aParts, TUint aRest, TUint aIndex, 
								       RArray<TXmlEngDataContainer>& aBinaryDataList,
						       	       RBuf8& aBinaryData);									 

    /**
     * Extracts CIDs of BLOBs from response MTOM message
     * @since S60 v3.2
     * @param aBlobHeader Header of Blob from that should be extracted CID of BLOB
     * @param aCids Array of CIDs for all BLOBs    
     */
    IMPORT_C static void CidL(const RBuf8& aBlobHeader, RArray<RBuf8>& aCids);

    /**
     * Extracts BLOB from response MTOM message and writes it in a file
     * @since S60 v3.2
     * @param aIndex Which BLOB from response is processed, starts from 0
     * @param aFirst Shows is it First part of BLOB or not
     * @param aBlob The part of BLOB from response that should be written in fhe file
     * @param aFileNames Array of File names for the all responsed BLOBs
     * @return size of file in order to monitor progess
     */	
    IMPORT_C static TInt SetFileL(TUint aIndex, TBool aFirst, TDesC8& aBlob, 
                                  RArray<RBuf8>& aFileNames);

private:													
		
    /**
     * Generates the file name for BLOB of response MTOM message 
     * and collects it in array of file names for the all BLOBs
     * @since S60 v3.2
     * @param aIndex Which BLOB from response is processed, starts from 0
     * @param aFileNames Array of File names for the all responsed BLOBs
     */
     static void FileNameL(TUint aIndex, RArray<RBuf8>& aFileNames);
	};
									 
#endif // SEN_MTOM_UTILS_H

// End of file


