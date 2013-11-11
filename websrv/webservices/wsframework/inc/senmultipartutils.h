/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:        Preparation XOP message for HTTP transmission in case 
*				 of request	and extracting BLOBs from XOP message responsed 
*				 through HTTP channel in case of response
*
*/








#ifndef SEN_MultiPart_UTILS_H
#define SEN_MultiPart_UTILS_H

#include <e32math.h>
#include <SenSoapEnvelope2.h>
#include "senatomentry.h"
#include <RSenDocument.h>

#include <xml/dom/xmlengserializer.h>
#include <xml/dom/xmlengchunkcontainer.h>
#include <xml/dom/xmlengfilecontainer.h>
#include <xml/dom/xmlengbinarycontainer.h>
#include <xml/dom/xmlengnodelist.h>

// CONSTANTS
_LIT(KMultiPartNoRequestMessagePanicText, "NULL MultiPart request message");
_LIT(KMultiPartNoBlobsPanicText, "NULL MultiPart BLOB part");
_LIT(KMultiPartNoContentTypeInBlobHeaderPanicText, "NULL MultiPart ContentType in BLOB header");
_LIT(KMultiPartNoCidPanicText, "NULL MultiPart Cid");
_LIT(KMultiPartSoapVersionInvalidPanicText, "MultiPart SOAP version unknown");
_LIT(KMultiPartBlobContainerTypeInvalidPanicText, "MultiPart BLOB container type unknown");
_LIT(KMultiPartResponseBodyInvalidPanicText, "MultiPart response body doesn't conform XOP rules");
_LIT(KMultiPartCidInvalidPanicText, "MultiPart Cid doesn't conform XOP rules");

_LIT(KMultiPartPanicText, "MultiPart panic");



/** numbers 11 or 12 at the end of constant names mean SOAP versions (1.1 or 1.2) */
_LIT8(KMultiPartHeaderRootStart11, "Content-Type: application/xop+xml;charset=UTF-8;type=\"text/xml\"\r\nContent-Transfer-Encoding: 8bit\r\nContent-ID: <");
_LIT8(KMultiPartHeaderRootStart12, "Content-Type: application/xop+xml;charset=UTF-8;type=\"application/soap+xml\"\r\nContent-Transfer-Encoding: binary\r\nContent-ID: <");
_LIT8(KMultiPartAtomHeaderRootStart, "Content-Type: application/atom+xml;charset=UTF-8;type=\"text/xml\"\r\nContent-Transfer-Encoding: 8bit\r\nContent-ID: <");
_LIT8(KMultiPartHeaderRootEnd, ">\r\n\r\n");    

_LIT8(KMultiPartContentTypeName, "contentType");
_LIT8(KMultiPartDefaultBinaryContentTypeValue, "application/octet-stream)");// video/mp4 or video/x-flv
_LIT8(KMultiPartHeaderBinaryDataContentType, "Content-Type: ");
_LIT8(KMultiPartHeaderBinaryDataContentID, "\r\nContent-Transfer-Encoding: binary\r\nContent-ID: <");
_LIT8(KMultiPartHeaderBinaryDataEnd, ">\r\n\r\n"); 

const TUint KTenKBytes = 10240;
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
class SenMultiPartUtils
	{
public:

    /**
     * different cases of panics
     */
     enum TMultiPartPanics
	    {
	    EMultiPartNoRequestMessage,
	    EMultiPartNoBlobs,
	    EMultiPartNoContentTypeInBlobHeader,
	    EMultiPartNoCid,
	    EMultiPartSoapVersionInvalid,
	    EMultiPartBlobContainerTypeInvalid,
	    EMultiPartResponseBodyInvalid,
	    EMultiPartCidInvalid
	    };

    /**
     * ContentType for MultiPart consists of 
     * FiledName = "Content-Type" FiledValue = "Multipart/Related"
     * ParamName1 = "boundary" 	  ParamValue1 = BoundaryValue
     * ParamName2 = "type"  	  ParamValue1 = TypeValue
     * ParamName1 = "start" 	  ParamValue1 = StartValue
     * ParamName1 = "start-info"  ParamValue1 = StartInfoValue
     * ParamName1 = "action" 	  ParamValue1 = ActionValue
     * This structure describes ParamName & ParamValue of ConetentType
     */ 
    struct TMultiPartContentTypeParam
        {
        TPtrC8 paramName;
        TPtrC8 paramValue;
        };	    
    
    /**
     * This structure describes FieldValue & Params of ContentType
     */
    struct TMultiPartContentType
        {
        TPtrC8 fieldValue;
        RArray<TMultiPartContentTypeParam> params;
        };

public:

    /**
     * Serializes SenMessage message in MultiPart message
     * @since S60 v5.0
     * @param aMessage The message with multiparts (usual view of message)
     * @param aMultiPartMessage The message serialized from usual view to MultiPart message
     * @param aType mode of serialization (see xmlengserializer.h)
     */	
	IMPORT_C static void SerializeMessageL(CSenFragmentBase& aMessage, 
	                                         RBuf8& aMultiPartMessage,
	                                         TXmlEngSerializerType aType = ESerializerXOPInfoset);
	
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
 	* Generates MimeBoundaryEnd to end multipart message (MultiPart) as --randomNumber--
 	* @since S60 v3.2
 	* @param aBoundary random boundary
 	* @param aBoundaryEnd Generated MIME boundary end
 	*/
  	IMPORT_C static void BoundaryLineEndL(const RBuf8& aBoundary, RBuf8& aBoundaryEnd);

    /**
     * Creates Header for Root of XOP message or other multiPart message
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
     * @param aMessage The message with multiparts
     * @param aRootCid The content ID for the header of XOP or other multiPart message
     * @param aHeaderRoot11  Header of root of XOP message or other multiPart message
     */
     IMPORT_C static void HeaderRootL(MSenMessage* aMessage,
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
     * Extracts CIDs of BLOBs from response MultiPart message
     * @since S60 v3.2
     * @param aBlobHeader Header of Blob from that should be extracted CID of BLOB
     * @param aCids Array of CIDs for all BLOBs    
     */
    IMPORT_C static void CidL(const RBuf8& aBlobHeader, RArray<RBuf8>& aCids);

    /**
     * Extracts BLOB from response MultiPart message and writes it in a file
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
     * Generates the file name for BLOB of response MultiPart message 
     * and collects it in array of file names for the all BLOBs
     * @since S60 v3.2
     * @param aIndex Which BLOB from response is processed, starts from 0
     * @param aFileNames Array of File names for the all responsed BLOBs
     */
     static void FileNameL(TUint aIndex, RArray<RBuf8>& aFileNames);
	};
									 
#endif // SEN_MultiPart_UTILS_H

// End of file


