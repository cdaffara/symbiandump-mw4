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
* Description:    Header declaration
*
*/











#ifndef SEN_HTTP_CHANNEL_TRANSPORT_PLUGIN_H
#define SEN_HTTP_CHANNEL_TRANSPORT_PLUGIN_H

// INCLUDES
#include <e32base.h>
#include <s32strm.h>
#include <s32file.h>    // filestream

#include <flogger.h>
#include "sendebug.h"
#include "sentransport.h"
#include "msencoreservicemanager.h"
#include "msenremoteserviceconsumer.h"
#include "msenservicesession.h"
#include "senpointermap.h"
#include "senresponseobserver.h"
#include "SenHttpTransportProperties.h"
#include <SenServiceConnection.h>
// CONSTANTS
_LIT8(KSenHttpChannelTransportUriScheme,        "http");
_LIT8(KSenHttpChannelUserAgentHeaderDefault,    "Serene");
_LIT8(KSenHttpChannelAcceptHeaderDefault,       "text/xml");


// FORWARD DECLARE
class CSenHttpChannel;
class CSenHttpSyncRequester;
class CSenLayeredHttpTransportProperties;
class CSenIdentityProvider;

class CSenSoapEnvelope2;

// TYPE DEF
typedef RSenPointerMap<MSenRemoteServiceConsumer, TInt> RConsumerMap;
typedef RSenPointerMap<MSenServiceSession, TInt> RSessionMap;

/**********************************************************************
 Here is the full list of HTTP error codes
 (taken from: \src\common\generic\application-protocols\http\inc\httperr.h)

       KErrHttpEncodeDefault                     -7200 
       KErrHttpEncodeAccept                      -7201 
       KErrHttpEncodeAcceptCharset               -7202 
       KErrHttpEncodeQValue                      -7203 
       KErrHttpEncodeAuthorization               -7204 
       KErrHttpEncodeBasicAuth                   -7205 
       KErrHttpEncodeDigestAuth                  -7206 
       KErrHttpEncodeConnection                  -7207 
       KErrHttpEncodeContentLength               -7208 
       KErrHttpEncodeContentType                 -7209 
       KErrHttpEncodeHost                        -7210 
       KErrHttpEncodeHostPort                    -7211 
       KErrHttpEncodeTransferEncoding            -7212 
       KErrHttpEncodeUserAgent                   -7213 
       KErrHttpEncodeDate                        -7214 
       KErrHttpEncodeCookie                      -7215 
       KErrHttpEncodeCacheControl                -7216 
       KErrHttpEncodePragma                      -7217 
       KErrHttpEncodeIfMatch                     -7218 
       KErrHttpEncodeIfNoneMatch                 -7219 
       KErrHttpEncodeIfModifiedSince             -7220 
       KErrHttpEncodeIfUnmodifiedSince           -7221 
       KErrHttpEncodeCookie2                     -7222 
       KErrHttpEncodeContentLanguage             -7223 
       KErrHttpEncodeDoWWWAuthenticate           -7224 
       KErrHttpEncodeDoAge                       -7225 
       KErrHttpEncodeDoVary                      -7226 
       KErrHttpEncodeDoContentLanguage           -7227 
       KErrHttpEncodeContentEncoding             -7228 
       KErrHttpEncodeContentLocation             -7229 
       KErrHttpEncodeContentMD5                  -7230 
       KErrHttpEncodeAcceptLanguage              -7231 
       KErrHttpEncodeAcceptEncoding              -7232 
       KErrHttpEncodeTE                          -7233 
                                                       
       KErrHttpDecodeMalformedDate               -7260 
       KErrHttpDecodeMalformedYear               -7261 
       KErrHttpDecodeMalformedMonth              -7262 
       KErrHttpDecodeMalformedDay                -7263 
       KErrHttpDecodeMalformedWkDay              -7264 
       KErrHttpDecodeMalformedWeekDay            -7265 
       KErrHttpDecodeMalformedInteger            -7266 
       KErrHttpDecodeMalformedQuotedStr          -7267 
                                                       
       KErrHttpDecodeAccept                      -7270 
       KErrHttpDecodeConnection                  -7271 
       KErrHttpDecodeContentLength               -7272 
       KErrHttpDecodeContentType                 -7273 
       KErrHttpDecodeTransferEncoding            -7274 
       KErrHttpDecodeWWWAuthenticate             -7275 
       KErrHttpDecodeUnknownAuthScheme           -7276 
       KErrHttpDecodeBasicAuth                   -7277 
       KErrHttpDecodeDigestAuth                  -7278 
       KErrHttpDecodeCookie                      -7279 
       KErrHttpDecodeAuthorization               -7280 
       KErrHttpDecodeAcceptCharset               -7281 
       KErrHttpDecodeAcceptLanguage              -7282 
       KErrHttpDecodeAcceptEncoding              -7283 
       KErrHttpDecodeTE                          -7284 
       KErrHttpDecodeExpect                      -7285 
                                                       
       KErrHttpRedirectExceededLimit             -7300 
       KErrHttpRedirectUseProxy                  -7301 
       KErrHttpRedirectNoLocationField           -7302 
       KErrHttpUnknownParseState                 -7303 
                                                       
       KErrHttpRequestHasBody                    -7330 
       KErrHttpRequestBodyMissing                -7331 
       KErrHttpTraceReqWithoutMaxForwardHeader   -7332 
       KErrHttpPostReqBodyWithoutSizeOnHTTP10    -7333 
       KErrHttpInvalidHeaderInRequest            -7334 
                                                       
       KErrHttpEntityHeaderMissingContentType    -7360 
       KErrHttpGeneralHeaderMissingHost          -7361 
                                                       
       KErrHttpCantResetRequestBody              -7370 
       KErrHttpProtTransactionNotInitialised     -7371 
       KErrHttpCannotEstablishTunnel             -7372 
       KErrHttpInvalidUri                        -7373 
       KErrHttpNonPipeliningError                -7374 
       KErrHttpPipeliningError                   -7375 

 ***********************************************************************/

// CLASS DECLARATION
class CSenHttpChannelTransportPlugin : public CSenTransport,
                                       public MSenResponseObserver
    {
    public: // Constructors and destructor
    
        static CSenHttpChannelTransportPlugin* NewL(CSenTransportContext* apCtx);
        static CSenHttpChannelTransportPlugin* NewLC(CSenTransportContext* apCtx);

        //static CSenHttpChannelTransportPlugin* NewL(MSenCoreServiceManager& aServiceManager);
        //static CSenHttpChannelTransportPlugin* NewLC(MSenCoreServiceManager& aServiceManager);

        /*        
        // @depracated: remove when Transport Properties is used as initializer:
        static CSenHttpChannelTransportPlugin* NewL(MSenCoreServiceManager& aManager,
                                                    TInt aProxyPort,
                                                    TPtrC8 aProxyHost);

        // @depracated: remove when Transport Properties is used as initializer:
        static CSenHttpChannelTransportPlugin* NewLC(MSenCoreServiceManager& aManager,
                                                     TInt aProxyPort,
                                                     TPtrC8 aProxyHost);

        // @depracated: remove when Transport Properties is used as initializer:
        static CSenHttpChannelTransportPlugin* NewL(MSenCoreServiceManager& aManager,
                                                    TUint32 aIapId);

        // @depracated: remove when Transport Properties is used as initializer:
        static CSenHttpChannelTransportPlugin* NewLC(MSenCoreServiceManager& aManager,
                                                     TUint32 aIapId);
    
        // @depracated: remove when Transport Properties is used as initializer:
        static CSenHttpChannelTransportPlugin* NewL(MSenCoreServiceManager& aManager, 
                                                    TInt aProxyPort, 
                                                    TPtrC8 aProxyHost,
                                                    TUint32 aIapId);

        // @depracated: remove when Transport Properties is used as initializer:
        static CSenHttpChannelTransportPlugin* NewLC(MSenCoreServiceManager& aManager, 
                                                     TInt aProxyPort, 
                                                     TPtrC8 aProxyHost,
                                                     TUint32 aIapId);
        */

        virtual ~CSenHttpChannelTransportPlugin();

        // from MSenResponseObserver

        /**
        * Receive response to a request submitted. This method
        * takes the ownership of the heap buffer aContent.
        *
        * @param aRequestId submit id which identifies the request.
        * @param aContentType incoming response content type.
        * @param aContent incoming response content, or NULL.
        * @param aHttpProperties - properies of response(e.g.such as file names where BLOBs are saved) or NULL
        */
        virtual void ResponseReceivedL(TInt aRequestId,
                                       const TAny* aContentType, 
                                       HBufC8* aContent,
                                       CSenHttpTransportProperties* aHttpProperties = NULL);

        /**
        * Error occured while receiving response. Error code
        * is either system error code or own error code. Own
        * error codes start from -20000
        *
        * @param aRequestId submit id which identifies the request.
        * @param aErrorCode  error code of the response.
        * @param aErrorBody is the received error response. Might
        *       be zero-lenght, if nothing was received.
        */
        virtual void ResponseErrorL(TInt aRequestId, TInt aError, HBufC8* aErrorBody,
                                    CSenHttpTransportProperties* aHttpProperties = NULL);

        // from MSenTransport

        virtual TInt SubmitL(const TDesC8& aEndpoint,
                             const TDesC8& aMessage,
                             const TDesC8& aTransportProperties,
                             HBufC8*& aResponse,
                             MSenRemoteServiceConsumer& aConsumer);

        virtual TInt SendL(const TDesC8& aEndpoint,
                           const TDesC8& aMessage,
                           const TDesC8& aTransportProperties,
                           MSenServiceSession& aReplyTo,
                           MSenRemoteServiceConsumer& aConsumer, // "adressee", could be CSenIdentifier
                           TInt& aTxnId);

        virtual TInt SetPropertiesL(const TDesC8& aProperties, 
                                    MSenLayeredProperties::TSenPropertiesLayer aTargetLayer,
                                    MSenRemoteServiceConsumer* aConsumer);
        virtual TInt PropertiesL(HBufC8*& aProperties);
        virtual TInt CompleteTransaction(const TInt aTxnId, const TInt aStatus);
        virtual TInt CancelTransaction(const TInt aTxnId);
        virtual TPtrC8 UriSchemeL();
        
        virtual MSenProperties& PropertiesL();
        
        virtual void StateChanged(TInt aId, TInt aState); // in order to propagate http status codes
        virtual void FileProgress(TInt aTxnId, TBool aIncoming, TBool aIsSoap,
            const TDesC8& aSoapOrCid, TInt aProgress);
		virtual void SetTrafficDetails(TSenDataTrafficDetails& aDetails);
        void DataTrafficDetails(TSenDataTrafficDetails& aDetails); 	
    protected:
    
        /**
        * C++ default constructor.
        * @param apCtx - ownership is taken to this instance(!)
        */

        CSenHttpChannelTransportPlugin(CSenTransportContext* apCtx,
                                       MSenCoreServiceManager& aCore);
        //CSenHttpChannelTransportPlugin(MSenCoreServiceManager& aServiceManager);

    private:
        /**
        * 2nd phase constructor
        */
        void ConstructL();

        // @depracated: remove when Transport Properties is used as initializer:
        //void ConstructL(TInt aProxyPort, TPtrC8 aProxyHost);
        // @depracated: remove when Transport Properties is used as initializer:
        //void ConstructL(TUint32 aIapId);
        // @depracated: remove when Transport Properties is used as initializer:
        //void ConstructL(TInt aProxyPort, TPtrC8 aProxyHost, TUint32 aIapId);

        /**
        * Helper, which resets HTTP transport properties
        */
        TInt ApplyPropertiesL();

        TInt ResetHttpChannelByIapIdL(TUint32 aIapId);

        CSenLayeredHttpTransportProperties& LayeredPropertiesL();
        

        /**
        * Logging 
        */
        //RFileLogger* Log() const;
        
    private: // data
        MSenCoreServiceManager& iServiceManager; // not owned
        CSenHttpSyncRequester* iRequester;
        CSenHttpChannel *iHttpChannel;
        CSenLayeredHttpTransportProperties* iProperties;
        RConsumerMap iConsumerMap; // values owned (TxnIds), keys not (consumers)
        RSessionMap iSessionMap;   // values NOT owned (TxnIds), neither keys (sessions)
        //RFileLogger iLogger;
        TInt iStatusCode; // containts latest error or zero
		TSenDataTrafficDetails iDetails;
		TBool iIsConnectionFailed;
    };

#endif // SEN_HTTP_CHANNEL_TRANSPORT_PLUGIN_H

// End of File
