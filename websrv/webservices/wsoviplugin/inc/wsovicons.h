/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:           Keep all constanece within WSTar Plugin
*
*/










#ifndef CONSPLUGIN_H
#define CONSPLUGIN_H

#include "senpointermap.h"
#include "SenXmlElement.h"

typedef RSenPointerMap<TDesC8, TInt> RMapInt;
typedef RSenPointerMap<TDesC8, TDesC8> RMapDescriptors;

namespace WSOviConfig
    {
    _LIT8(KFrameworkVersion,      "1.0");
    _LIT8(KFrameworkXMLNS,        "urn:com.nokia.ws.ovi.config.1.0");
    _LIT8(KFrameworkCue,          "com.nokia.ws.ovi.OviFramework");
    _LIT8(KFrameworkLocalName,    "Framework");
    _LIT8(KHandlerLocalName,      "Handler");
    _LIT8(KNameAttr,              "name");
    
    _LIT8(KClassAttr,              "class");
    _LIT8(KIMSILocalName,              "IMSI");
    _LIT8(KIMSIDigestAttr,              "digest");
    _LIT8(KVersionAttr,              "version");    
    _LIT8(KRegisterValue,              "urn:com.nokia.ws.ovi.handler.register");
    _LIT8(KValidateValue,              "urn:com.nokia.ws.ovi.handler.validate");    
    _LIT8(KDeviceValue,                "urn:com.nokia.ws.ovi.handler.device");
    _LIT8(KCredentialCollectorValue,   "urn:com.nokia.ws.ovi.handler.credentialCollector");
    _LIT8(KSecurityValue,              "urn:com.nokia.ws.ovi.handler.security");
    _LIT8(KEncoderValue,               "urn:com.nokia.ws.ovi.handler.encoder"); 
    _LIT8(KServiceUpdateValue,         "urn:com.nokia.ws.ovi.handler.serviceUpdate");

    _LIT8(KValidateHandlerVersion,     "http://account.nokia.com/schemas/rest/v1_0");    

    }
   
namespace WSOviSession
    {
    //in seconds
    const TInt KCounterMax    = 3;
    const TInt KMaxHttpTimeOut   = 60;
        
    const TInt KSubmitStateOK                         = 1;
    const TInt KSubmitStateInvalidCredentials        = 2;
    const TInt KSubmitStateRevalidationAttempted    = 3;
    _LIT8(KDateHttpHeader, "Date");


    _LIT8(KTokenEndpointSuffix, "tokens");
    }
namespace OAuth
    {
    _LIT8(KUnReserveChars, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_.~");

    //header
    _LIT8(KAuthHttpHeader, "Authorization");
    _LIT8(KComma, ",");
    _LIT8(KSpace, " ");
    _LIT8(KQtMark, "\"");
    _LIT8(KEqual, "=");
    _LIT8(KAmpMark, "&");
    _LIT8(KPercent, "%");

    _LIT8(KFormUrlEncoded, "application/x-www-form-urlencoded");


    _LIT8(KXmlContentEqual, "XMLContent=");


    
    //##### parameter names
    //device
    _LIT8(KParNokiaAuth, "OAuth");      
    _LIT8(KParVersion, "oauth_version");      
    _LIT8(KParConsumerKey, "oauth_consumer_key");
    
    
    //credential collector
    _LIT8(KParToken, "oauth_token");
    
    //security
    _LIT8(KParRealm, "realm");
    _LIT8(KParSignMethod, "oauth_signature_method");
    _LIT8(KParTimestamp, "oauth_timestamp");
    _LIT8(KParSignature, "oauth_signature");
    _LIT8(KParNonce, "oauth_nonce");
    _LIT8(KParHttpMethodPost, "POST");
    _LIT8(KParHttpMethodGet, "GET");
    _LIT8(KParHttpMethodPut, "PUT");
    _LIT8(KParHttpMethodDelete, "DELETE");
    
    _LIT8(KEndpointKey, "endpoint");
    
    _LIT8(KNeedSecurity,"NeedSecurity");
    //parameter values
    
    _LIT8(KParValVersion, "1.0");      
    _LIT8(KParValHMACSHA, "HMAC-SHA1");
    _LIT8(KParValRealm, "https://account.nokia.com");
    
    
    //body
    _LIT8(KCreationRequestFormat8,
//            "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
    		"<tokenCreationRequest xmlns=\"%S\">"
    		"<digestAuth>"
    		"<username>%S</username>"
    		"<nonce>%S</nonce>"
    		"<created>%S</created>"
    		"<digest>%S</digest>"
    		"</digestAuth>"
/*            "    <scope>\n"
            "        <resource>\n"
            "            <type>path</type>\n"
            "            <id>/accounts/%S/</id>\n"
            "        </resource>\n"
            "    </scope>\n"
*/    		"</tokenCreationRequest>");
    }    
namespace WSOviContextKeys
    {
    //SESSION KEYS
    //--keys for discovery phase
    //-----register handler
    _LIT8(KRegisterAction,       "action");
    _LIT8(KServiceDescription,   "serviceDescription");
    
    //--keys for validation phase (handler)
    _LIT8(KServiceSession,       "serviceSession");
    _LIT8(KRemoteConsumer,       "RemoteConsumer");
    _LIT8(KReAuthNeeded, "ReAuthNeeded");
    _LIT8(KRetryNeeded, "Retry");

    _LIT8(KErrMessage, "ErrorMessage");
 
    
    _LIT8(KMessageBody,"messageBody");
    _LIT8(KTP,"TP");
    
    
    _LIT8(KTTL,"TTL");
    _LIT8(KTokenCreationTime,"TokenCreationTime");
    _LIT8(KTokenValidUntilTime,"TokenValidUntil");
    _LIT8( KTokenSecret, "TokenSecret");
    _LIT8( KToken, "Token");
    _LIT8(KOnlySharing,       "OnlySharing");
    _LIT8(KErrCode,       "ErrorCode");
    
   
    //MARKERS
    }

namespace WSOviContextValues
    {
    //--values for discovery phase
    //-----register handler
    _LIT8(KActionRegister,       "register");
    _LIT8(KActionUnregister,       "unregister");
    }

namespace WSOviResponse
	{
    _LIT8(KTokenTag, "<token>");
    _LIT8(KTokenTagEnd, "</token>");
    _LIT8(KTokenLocalName, "token");
    _LIT8(KTokenSecretTag, "<tokenSecret>");
    _LIT8(KTokenSecretTagEnd, "</tokenSecret>");
	_LIT8(KTokenCreationResponseLocalName, "tokenCreationResponse");
	_LIT8(KTokenSecretLocalName, "tokenSecret");
	_LIT8(KTTLLocalName, "ttl");
	_LIT8(KExpiresLocalName, "expires");
	_LIT8(KTokenInfoLocalName,"tokenInfo");
    _LIT8(KErrorNode,                    "error");
    _LIT8(KErrorCode,                    "errorCode");
    _LIT8(KErrorText,                    "errorText");
    _LIT8(KErrorCodeInvalidAccessToken, "invalidAccessToken");
    _LIT8(KErrorCodeUnauthorized,       "notAuthorized");
    _LIT8(KUserInfoLocalName,"userInfo");
    _LIT8(KUserNameLocalName,"username");
		_LIT8( KMobileUsed1,             "Mobile number" );
    _LIT8( KMobileUsed2,             "already used today" );
		_LIT8(KEmailUsed, "You have already a Nokia account associated with this mobile number or email address.");	
	}

#endif // CONSPLUGIN_H

