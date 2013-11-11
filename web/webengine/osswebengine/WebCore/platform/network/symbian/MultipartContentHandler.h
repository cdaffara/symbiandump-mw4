/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*
*/


#ifndef MULTIPARTCONTENTHANDLER_H
#define MULTIPARTCONTENTHANDLER_H

//  INCLUDES
#include <http/rhttptransaction.h>

class CHttpCacheManager;
class CBodyPart;
namespace WebCore {
    class ResourceResponse;
};

// CONSTANTS

// the following are flags used to configure the multipart validity test
// that is performed within function ValidateMultipartArray()
//
// test that base url of all parts match base url of multipart response
const TInt KRulePartUrlMatchesRespUrl = 0x01;
// test of URL includes scheme
const TInt KRuleUrlMatchingOnScheme = 0x02;
// test of URL includes hostname
const TInt KRuleUrlMatchingOnHostname = 0x04;
// test of URL includes port
const TInt KRuleUrlMatchingOnPort = 0x08;
// test that first part is a markup type (ie, HTML, XHTML, WML)
const TInt KRuleFirstPartIsMarkup = 0x10;
// test that parts are: markup, image, script, css or sound
const TInt KRuleOnlyWebContentAllowed = 0x20;

class MultipartContentHandler
    {
    public:   // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static MultipartContentHandler* NewL();

        /**
        * Destructor.
        */
        virtual ~MultipartContentHandler();

    public: // New functions
    
        static bool IsSupported(const WebCore::ResourceResponse& response);

        TInt HandleResponseHeadersL(const WebCore::ResourceResponse& response, RHTTPTransaction& httpTransaction);

        TInt HandleResponseBodyL(TDesC8& aRespData);

        TInt ResponseComplete();

        const TDesC8& MarkupContent();
        TPtrC8& MarkupCharset() { return m_MarkupCharset; }
        TPtrC8& MarkupContentType() { return m_MarkupContentType; }

    private:

        /**
        * C++ default constructor.
        */
        MultipartContentHandler();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        *
        * @since 3.0
        * @param
        * @return
        */
        TInt HandleMultipartMixedL();

        /**
        *
        * @since 3.0
        * @param
        * @return
        */
        //void PushToMemoryCacheL(CBodyPart& aPart);

        /**
        *
        * @since 3.0
        * @param
        * @return
        */
        void PushToHttpCacheL(CBodyPart& aPart, CHttpCacheManager* aCacheMgr);

        /**
        *
        * @since 3.0
        * @param
        * @return
        */
        TInt ValidateMultipartArrayL();

        /**
        *
        * @since 5.0
        * @param
        * @return
        */
        HBufC8* GenerateExpiresHeaderL();

        /**
        *
        * @since 3.0
        * @param
        * @return
        */
        //TInt IsExtensionTypeText(const TDesC16& aResponseUrl);

    private:     // Data
        //
        HBufC* m_ResponseUrl;                           // owned
        // multipart boundary string
        HBufC8* m_TopLevelBoundary;                     // owned
        // multipart boundary string with trailing "--"
        HBufC8* m_TopLevelBoundaryEnd;                  // owned
        // buffer to collect response body chunks until
        // response is complete
        HBufC8* m_ResponseBuffer;                       // owned
        // array of multipart body parts
        RPointerArray <CBodyPart> m_BodyPartsArray;
        // indicates if multipart content has been parsed
        bool m_ParsingCompleted;
        // descriptor containing the markup for the response
        // if response was not valid then this will contain the
        // error response content
        TPtrC8 m_MarkupContent;
        TPtrC8 m_MarkupCharset;
        TPtrC8 m_MarkupContentType;
        //
        HBufC8* m_CacheExpirationHeader;                 // owned
    };

#endif      // MULTIPARTCONTENTHANDLER_H

// End of File
