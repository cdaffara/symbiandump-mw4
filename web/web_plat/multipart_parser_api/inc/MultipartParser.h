/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Implementation of the Multipart Parser
*
*/



#ifndef MULTIPARTPARSER_H
#define MULTIPARTPARSER_H

#warning The Multipart Parser API will be removed wk25. Please see http://wikis.in.nokia.com/Browser/APIMigration for more information

//  INCLUDES
#include "BodyPart.h"


//  CONSTANTS


// DATA TYPES


// FORWARD DECLARATIONS
class CBodyPart;

// CLASS DECLARATION
/**
*  Implementation of the Multipart Parser
*
*  \b Library: MultipartParser.lib
*
*  @since 3.0
*/
class MultipartParser
    {
    public:     // Data
        enum TMultipartTopLevelHeader
            {
            EMultipartTopLevelHeaderContentType = 0x0001,
            EMultipartTopLevelHeaderContentLength = 0x0002,
            EMultipartTopLevelHeaderLastModified = 0x0004,
            // make SURE to change this when adding new header types
            EMultipartTopLevelHeaderAll = 0x0007
            };
        
        enum TMultipartSubtype
            {
        	  EMultipartSubtypeMixed = 0
            };

    public:  // Constructors and destructor
        

    public: // New functions
    
        /**
        * Parse a multipart file
        * @since 3.0
        * @param aMultipartBody   The multipart file to be parsed
        * @param aContentType     The content type of multipart file: mixed, related etc.
        * @param aBoundary        The boundary of the multipart file
        * @param aUrl             The url of the multipart file
        * @param aBodyPartsArray  The array contains parsed body parts of the multipart file
        * @param aMaxToParse      The max number of body parts to be parsed
        * @return void
        */
        IMPORT_C static void ParseL( const TDesC8& aMultipartBody, 
                                     const TDesC8& aContentType,
                                     const TDesC8& aBoundary,
                                     const TDesC16& aUrl,
                                     RPointerArray <CBodyPart>& aBodyPartsArray,
                                     TInt aMaxToParse = -1 );

        /**
        * Produces multipart document
        * @param aBodyArray      Array of BodyPart objects to be included in the output
        * @param aBoundary       A string containing boundary to be used in construction of multipart document
        * @param aSubtype        Enumerated value of multipart subtype
        * @param aHeaderMask     Integer mask of TTopLevelHeaders to indicate which top-level headers should be included
        * @return multipart document; the ownership of buffer is transferred to caller.
        */
        IMPORT_C static HBufC8* ComposeL( RPointerArray<CBodyPart>& aBodyPartsArray,
                                          const TDesC8& aBoundary,
                                          TMultipartSubtype aSubtype,
                                          TInt aHeaderMask );
                                 
    public: // Functions from base classes
               
    protected:  // Functions from base classes
        

    private:
        /**
        * Get the buffer of the next body part
        * @since 3.0
        * @param aStartPosition   The starting position to parse
        * @param aMultipartBody   The full buffer of multipart file
        * @param aMultipartLen    The length of the multipart file buffer
        * @param aBoundary        The boundary of the multipart file
        * @param aSingleEolChar   The single EOL of the multipart file
        * @param aBodyPartBuffer  The buffer of this body part
        * @return the status of the function
        */
        static TUint32 GetNextBodyPartBuffer( TUint32 startPosition, 
                                       const TUint8* aMultipartBody,
                                       TUint32 aMultipartLen,
                                       const TDesC8& aBoundary,
                                       char* aSingleEolChar,
                                       TUint8** aBodyPartBuffer );

        /**
        * 
        * Set the single and double EOL of the multipart file
        * @since 3.0
        * @param aMultipartBody   The full buffer of multipart file
        * @param aMultipartLen    The length of the multipart file buffer
        * @param aBoundary        The boundary of the multipart file
        * @param aSingleEolChar   The single EOL of the multipart file
        * @param aDoubleEolChar   The double EOL of the multipart file
        * @return void
        */
        static void SetEolCharacters( const TUint8* aMultipartBody,
                               TUint32 aMultipartLen,
                               const TDesC8& aBoundary,
                               char** aSingleEolChar,
                               char** aDoubleEolChar );

        /**
        * Parse the body part
        * @since 3.0
        * @param aBodyPartBuffer        The buffer of this body part
        * @param aBodyPartBufferLength  The length of this body part buffer
        * @param aSingleEolChar         The single EOL of the multipart file
        * @param aDoubleEolChar         The double EOL of the multipart file
        * @param aResponseUrl           The url requested for the multipart file
        * @param aBodyPart              The body part parsed and returned
        * @return void
        */
        static void ParseBodyPartL( TUint8* aBodyPartBuffer,
                             TUint32 aBodyPartBufferLength,
                             char* aSingleEolChar,
                             char* aDoubleEolChar,
                             const TDesC16& aResponseUrl,
                             CBodyPart* aBodyPart );

        /**
        * Is encoded or not
        * @since 3.0
        * @param aContentTransferEncodingValue  The transfer encoding of this body part
        * @return is encoded or not
        */
        static TBool IsEncoded( TUint8* aContentTransferEncodingValue );

        /**
        * Decode the body
        * @since 3.0
        * @param aContentTransferEncodingValue  The transfer encoding of this body part
        * @param aEncodedBody                   The encoded body of this body part
        * @param aDecodedBody                   The decoded body returned
        * @return status of the function call
        */
        static TInt DecodeContentTransferEncoding( TUint8* aContentTransferEncodingValue,
                                            const TDesC8& aEncodedBody,
                                            TPtr8& aDecodedBody );

        /**
        * Is zipped or not
        * @since 3.0
        * @param aContentTypeValue  The content type of this body part
        * @return is zipped or not
        */
        static TBool IsZipped( TUint8* aContentTypeValue );

        /**
        * Unzip the body
        * @since 3.0
        * @param aContentTypeValue     The content type of this body part
        * @param aZippedBody           The zipped body of this body part
        * @param aUnzippedBody          The unzipped body returned
        * @return status of the function call
        */
        static TInt Unzip( TUint8* aContentType,
                           const TDesC8& aZippedBody,
                           TPtr8& aUnzippedBody );

        /**
        * Parser the content type value
        * @since 3.0
        * @param aBodyPart              The body part which contains the content type
        * @return void
        */
        static void CutOffContentTypeAttributes( CBodyPart* aBodyPart );

        /**
        * Conform the URL for the body part
        * @since 3.0
        * @param aContentBaseValue      The content base of this body part
        * @param aContentLocationValue  The content location of this body part
        * @param aResponseUrl           The url requested for the multipart file
        * @return URL
        */
        static HBufC16* GetBodyPartUrlL( const TDesC8& aContentBaseValue,
                                 const TDesC8& aContentLocationValue,
                                 const TDesC16& aResponseUrl );

        /**
        * URL relative or not
        * @since 3.0
        * @param aUrl           Any url
        * @return relative or not
        */
        static TBool IsUrlRelativeL( const TDesC8& aUrl );

        /**
        * resolve relative URL to absolute URL
        * @since 3.0
        * @param aBase         The base URL
        * @param aRelativeUrl  The relative URL
        * @return absolute URL
        */
        static HBufC16* UrlRelToAbsL( TDesC16& aBase, 
                                      const TDesC8& aRelativeUrl );

        /**
        * Produces multipart/mixed document
        * @param aBodyArray      Array of BodyPart objects to be included in the output
        * @param aBoundary       A string containing boundary to be used in construction of multipart document
        * @param aHeaderMask     Integer mask of TTopLevelHeaders to indicate which top-level headers should be included
        * @return multipart document; the ownership of buffer is transferred to caller.
        */
        static HBufC8* ComposeMixedL( RPointerArray<CBodyPart>& aBodyArray,
                                      const TDesC8& aBoundary,
                                      TInt aHeaderMask );
                                          
    private:    // Data

    };

#endif      // MULTIPARTPARSER_H   
            
// End of File
