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


#ifndef HTTPREQUESTHEADERMANAGER_H
#define HTTPREQUESTHEADERMANAGER_H

//  INCLUDES

//#include "HttpFilterAcceptHeaderInterface.h"
#include <http/rhttpheaders.h>
#include <centralrepository.h>

namespace WebCore {
    struct ResourceRequest;
};
class HttpSessionManager;

// CLASS DECLARATION

/**
*  class handling dynamic request headers.
*  This class reads the necessary data from the central repository
*  and constructs the request headers accordingly. Some optimization
*  is also provided.
*
*  @since 5.0
*/
class HttpRequestHeaderManager : public CBase
    {

     public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static HttpRequestHeaderManager* NewL(RHTTPSession& aSession);

        /**
        * Destructor.
        */
        virtual ~HttpRequestHeaderManager();

    public: // new functions

        /**
        * Add all managed request headers
        * @since 5.0
        * @param aHeaders An instance of RHTTPHeaders from the current
        *        transaction.
        * @param aRequest Reference to the active ResourceRequest object submitted by WebCore
        */
        void AddAllHeadersL(RHTTPHeaders& aHeaders, const WebCore::ResourceRequest& aRequest);

        /**
        * Sets the HTTP Accept header in the RHTTPHeaders object
        * @since 5.0
        * @param aHeaders An instance of RHTTPHeaders from the current
        *        transaction.
        * @param aRequest Reference to the active ResourceRequest object submitted by WebCore
        */
        void SetAcceptHeaderL(RHTTPHeaders& aHeaders, const WebCore::ResourceRequest& aRequest);

        /**
        * Sets the HTTP client Accept header in the RHTTPHeaders object
        * @since 5.0
        * @param aHeaders An instance of RHTTPHeaders from the current
        *        transaction.
        * @param aRequest Reference to the active ResourceRequest object submitted by WebCore
        */
        void SetClientAcceptHeaderL(RHTTPHeaders& aHeaders, const WebCore::ResourceRequest& aRequest);

        /**
        * Sets the HTTP Cache-Control header in the RHTTPHeaders object
        * @since 5.0
        * @param aHeaders An instance of RHTTPHeaders from the current
        *        transaction.
        * @param aRequest Reference to the active ResourceRequest object submitted by WebCore
        */
        void SetCacheControlHeadersL(RHTTPHeaders& aHeaders, const WebCore::ResourceRequest& aRequest);

        /**
        * Sets the HTTP Content-Type header in the RHTTPHeaders object for POST requests
        * @since 5.0
        * @param aHeaders An instance of RHTTPHeaders from the current
        *        transaction.
        * @param aRequest Reference to the active ResourceRequest object submitted by WebCore
        */
        void SetContentTypeHeaderL(RHTTPHeaders& aHeaders, const WebCore::ResourceRequest& aRequest);

        /**
        * Sets the HTTP Referer header in the RHTTPHeaders object
        * @since 5.0
        * @param aHeaders An instance of RHTTPHeaders from the current
        *        transaction.
        * @param aRequest Reference to the active ResourceRequest object submitted by WebCore
        */
        void SetRefererHeaderL(RHTTPHeaders& aHeaders, const WebCore::ResourceRequest& aRequest);

        /**
        * Sets the HTTP Accept-Language header in the RHTTPHeaders object
        * @since 5.0
        * @param aHeaders An instance of RHTTPHeaders from the current
        *        transaction.
        * @param aRequest Reference to the active ResourceRequest object submitted by WebCore
        */
		void SetAcceptLanguageHeaderL(RHTTPHeaders& aHeaders, const WebCore::ResourceRequest& aRequest);

        /**
        * Sets the HTTP Accept-Charset header in the RHTTPHeaders object
        * @since 5.0
        * @param aHeaders An instance of RHTTPHeaders from the current
        *        transaction.
        * @param aRequest Reference to the active ResourceRequest object submitted by WebCore
        */
		void SetAcceptCharsetHeaderL(RHTTPHeaders& aHeaders, const WebCore::ResourceRequest& aRequest);

        /**
        * Sets the HTTP Connection : Keep-Alive header in the RHTTPHeaders object
        * @since 5.0
        * @param aHeaders An instance of RHTTPHeaders from the current
        *        transaction.
        * @param aRequest Reference to the active ResourceRequest object submitted by WebCore
        */
        void SetConnectionHeaderL(RHTTPHeaders& aHeaders, const WebCore::ResourceRequest& aRequest);

    private:

        /**
        * C++ constructor.
        */
        HttpRequestHeaderManager(RHTTPSession& aSession);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Read browser supported mime type from central repository
        * @since 5.0
        */
        void ReadCenRepMimeTypesL(CRepository& aRepository);

        /**
        * Return Accept-Charset header string based on current
        * selections for desired page charset
        * @since 5.0
        */
        HBufC8* AcceptCharsetStringL();

        /**
        * Return string containing full list of accept MIME types
        * for top-level loads
        * @since 5.0
        */
        HBufC8* TopLevelAcceptStringL(const WebCore::ResourceRequest& Request);

        /**
        * Compose Multiple Languages String
        * @since 5.0
        */
		void ComposeMultipleLanguagesStringL(TLanguage aUserLanguge);
		
		/**
        * Add XMLHttpHeaders set through setRequestHeader.
        * @since 5.0
        */
		void AddXMLHttpHeadersL(RHTTPHeaders& aHeaders, const WebCore::ResourceRequest& aRequest );

    private:     // Data

        HttpSessionManager* m_HttpSessionMgr;   // not owned
        //
        RHTTPSession m_Session;
        //
		RStringPool m_StringPool;
		//
		HBufC8* m_AcceptMimeTypesCenRep;        // owned
        //
        HBufC8* m_AcceptHdrMimesTopLevel;       // owned
        //
        TUint m_UserCharset;
        //
        HBufC8* m_CharsetBuf;                   // owned
        //
        HBufC8* m_AcceptLangStr;                // owned
        //
		TLanguage m_UserSelectedLang;
        //
		HBufC8* m_InstalledLangBuf;             // owned
    };

#endif      // HTTPREQUESTHEADERMANAGER_H

// End of File
