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
* Description:       
*
*/







#include <e32std.h>
#include "wsovisecurityhandler.h"
#include "wsovicons.h"
#include "wsovihandlercontext.h"
#include "senlogger.h"
#include "wsovimessagecontext.h"
#include <SenTransportProperties.h>
#include <SenHttpTransportProperties.h>

#include "wsoviutils.h"
#include "sencryptoutils.h"


using namespace OAuth;
namespace
    {
    const TInt KSHA1OutputSize = 20;// see http://en.wikipedia.org/wiki/SHA_hash_functions
    }
// Create instance of concrete ECOM interface implementation
CWSOviSecurityHandler* CWSOviSecurityHandler::NewL(
        MSenHandlerContext* aHandlerCtx)
    {
    
    CWSOviSecurityHandler* self =
            new (ELeave) CWSOviSecurityHandler(*aHandlerCtx);
    return self;
    }

// Constructor
CWSOviSecurityHandler::CWSOviSecurityHandler(MSenHandlerContext& aCtx):
        CSenMessageHandler(aCtx), iDiff(0)
    {
    }

// Destructor
CWSOviSecurityHandler::~CWSOviSecurityHandler()
    {
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSOviSecurityHandler::~CWSOviSecurityHandler()")));
    }

TInt CWSOviSecurityHandler::InvokeL(MSenMessageContext& aCtx)
    {
    TInt error(KErrNone);
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSOviSecurityHandler::InvokeL(MSenMessageContext& aCtx)")));
    CSenTransportProperties* tp = ((CWSOviMessageContext&)aCtx).TP();
    const TInt* servertime = ((CWSOviMessageContext&)aCtx).GetIntL(WSOviContextKeys::KRetryNeeded);
    const TDesC8* bodyMessage = ((CWSOviMessageContext&)aCtx).GetDesC8L(WSOviContextKeys::KMessageBody);
    
    
    TPtrC8 oldValue;
    if (!tp) return KErrNotFound;
    error = tp->PropertyL(KAuthHttpHeader, oldValue);
    if (error) return error;//device handler missed
    
    
    TPtrC8 consumerKey;
    error = tp->PropertyL(KNCIMConsumerKeyId, consumerKey);
        
    TPtrC8 consumerSecret;
    error = tp->PropertyL(KNCIMConsumerSecret, consumerSecret);
    
    TBool needSecurity(ETrue);
    error = tp->BoolPropertyL(KNeedSecurity,needSecurity);
    
    
    const TDesC8* endpoint = ((CWSOviMessageContext&)aCtx).GetDesC8L(KEndpointKey);
    if (!endpoint)
        {
        endpoint = &KParValRealm();
        }
    
    CSenHttpTransportProperties::TSenHttpMethod httpMethod;
    error = ((CSenHttpTransportProperties*)tp)->HttpMethodL(httpMethod);
    if (error)
        {
        httpMethod = CSenHttpTransportProperties::ESenHttpPost;
        }
//====================NAuth message encoding + signing (algorithm)

//____1____ generate random(NONCE)

    HBufC8* nonce(SenCryptoUtils::GetRandomNonceL());
    TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,_L("CWSOviSecurityHandler::   ____1____ generate random(NONCE %S)"), nonce));
    
    
//____2____ generate timestamp
    TTime time;
    time.UniversalTime();
    if (servertime && *servertime > 0/*&& iDiff==0*/)
        {
        TTimeIntervalMinutes minutes(*servertime);
        iDiff = time-minutes;
        ((CWSOviMessageContext&)aCtx).Update(WSOviContextKeys::KRetryNeeded, 0);
        }
    time -= TTimeIntervalMicroSeconds(iDiff.Int64());
    HBufC8* timestamp = SenCryptoUtils::GetTimestampL(time);
    
        
    
    
    
    
    TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,_L("CWSOviSecurityHandler::   ____2____ generate (timestamp %S)"), timestamp));

//____3____ create request without signature

    const TDesC8* token = aCtx.GetDesC8L(WSOviContextKeys::KToken());
    TPtrC8 clearToken = KNullDesC8();
    TInt tokenLength(0);
    if (token && needSecurity)
        {
        TInt left = token->Locate('>') + 1;
        TInt right = token->LocateReverse('<');
        clearToken.Set(token->Mid(left, right - left));
        tokenLength = clearToken.Length();
        tokenLength += KParToken().Length();
        tokenLength += KEqual().Length() + KQtMark().Length() +                    
                    KQtMark().Length() + KComma().Length();
        }
    
    RBuf8 newValue;
    newValue.Create(oldValue.Length() +
                    KComma().Length() +
                   
                    KParSignMethod().Length() +
                    KEqual().Length() + KQtMark().Length() +
                    KParValHMACSHA().Length() +
                    KQtMark().Length() + KComma().Length() +

                    KParTimestamp().Length() +
                    KEqual().Length() + KQtMark().Length() +
                    timestamp->Length() +
                    KQtMark().Length() + KComma().Length() +

                    KParNonce().Length() +
                    KEqual().Length() + KQtMark().Length() +
                    nonce->Length() +
                    KQtMark().Length() + KComma().Length() +

                    tokenLength +
                    
                    KParSignature().Length() +
                    KEqual().Length() + KQtMark().Length() +
                    KSHA1OutputSize*3  +
                    KParSignature().Length()

                    );
    CleanupClosePushL(newValue);
  
    TPtrC8 country;
    tp->PropertyL(KNCIMCountry(), country);
    
  	RMapDescriptors& headerProperties = ((CWSOviMessageContext&)aCtx).OAuthProperies();
    HBufC8* property = KParValHMACSHA().AllocL();
    headerProperties.Insert(&KParSignMethod, property);
    headerProperties.Insert(&KParTimestamp, timestamp);
    headerProperties.Insert(&KParNonce, nonce);
    if (country.Length())
        {
        headerProperties.Insert(&KNCIMCountry, country.AllocL());
        }
    headerProperties.Insert(&KParConsumerKey, consumerKey.AllocL());
    if (token && needSecurity)
    	{
    	headerProperties.Insert(&KParToken, clearToken.AllocL());
    	}

    newValue.Append(oldValue);
    newValue.Append(KComma());
    
   

    newValue.Append(KParSignMethod());
    newValue.Append(KEqual());
    newValue.Append(KQtMark());
    newValue.Append(KParValHMACSHA);
    newValue.Append(KQtMark());
    newValue.Append(KComma());


    newValue.Append(KParTimestamp());
    newValue.Append(KEqual());
    newValue.Append(KQtMark());
    newValue.Append(*timestamp);
    newValue.Append(KQtMark());
    newValue.Append(KComma());


    newValue.Append(KParNonce());
    newValue.Append(KEqual());
    newValue.Append(KQtMark());
    newValue.Append(*nonce);
    newValue.Append(KQtMark());
    newValue.Append(KComma());
    
    if (token && needSecurity)
        {
        newValue.Append(KParToken());
        newValue.Append(KEqual());
        newValue.Append(KQtMark());
        newValue.Append(clearToken);
        newValue.Append(KQtMark());
        newValue.Append(KComma());
        }
    
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSOviSecurityHandler::   ____3____ create request without signature")));

//____4____ normalize request

    HBufC8* body = NULL;
    if (bodyMessage)
        {
        HBufC8* bodyEncoded = CWSOviUtils::EncodeCharsLC(*bodyMessage);
        body = HBufC8::NewL(OAuth::KXmlContentEqual().Length()+bodyEncoded->Length());
        body->Des().Append(OAuth::KXmlContentEqual());
        body->Des().Append(*bodyEncoded);
        CleanupStack::PopAndDestroy(bodyEncoded);
        CleanupStack::PushL(body);
        }
    
	TInt length=0;
	if (body)
	    {
	    length+=body->Length();
	    length+=KAmpMark().Length();
	    }
	for(TInt i=0; i<headerProperties.Count(); i++)
	{
		length+=(*headerProperties.KeyAt(i)).Length();
		length+=(*headerProperties.ValueAt(i)).Length();
		length+=KEqual().Length();
		if(i)
		length+=KAmpMark().Length();
	}

	RBuf8 headerToNormalize;
	CleanupClosePushL(headerToNormalize);
	headerToNormalize.Create(length);
	if (body)
	    {
	    headerToNormalize.Append(*body);
	    headerToNormalize.Append(KAmpMark());
	    }
	for(TInt i=0; i<headerProperties.Count(); i++)
	{
		headerToNormalize.Append(*headerProperties.KeyAt(i));
		headerToNormalize.Append(KEqual());
		headerToNormalize.Append(*headerProperties.ValueAt(i));
		if(i<headerProperties.Count()-1)
		headerToNormalize.Append(KAmpMark());
	}
    TLSLOG_ALL(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, headerToNormalize);

	HBufC8* strToSign = NULL;
	if (httpMethod == CSenHttpTransportProperties::ESenHttpPost)
	    {
	    strToSign = CWSOviUtils::NormalizeStringLC( KParHttpMethodPost, *endpoint/*KParValRealm*/, headerToNormalize );
	    }
	else if (httpMethod == CSenHttpTransportProperties::ESenHttpGet)
	    {
        strToSign = CWSOviUtils::NormalizeStringLC( KParHttpMethodGet, *endpoint/*KParValRealm*/, headerToNormalize );
	    }
	else if(httpMethod == CSenHttpTransportProperties::ESenHttpPut)
		{
		strToSign = CWSOviUtils::NormalizeStringLC( KParHttpMethodPut, *endpoint/*KParValRealm*/, headerToNormalize );
		}
    else if(httpMethod == CSenHttpTransportProperties::ESenHttpDelete)
        {
        strToSign = CWSOviUtils::NormalizeStringLC( KParHttpMethodDelete, *endpoint/*KParValRealm*/, headerToNormalize );
        }
#ifdef _SENDEBUG

    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSOviSecurityHandler::   ____4____ normalize request")));
	if(strToSign)
		{
			TLSLOG_ALL(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, *strToSign);
		}
#endif // _SENDEBUG
    
//____5____ calculate hash

    CSHA1* sha1 = CSHA1::NewL();    
    CleanupStack::PushL(sha1);
    
    TBuf8<KSHA1OutputSize> hash; 
    hash.Copy(sha1->Final(*strToSign));

    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSOviSecurityHandler::   ____5____ calculate hash & Key")));
    TLSLOG_ALL(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, hash);

//____6____ sign (with HC client secret)


    /*
    See http://oauth.net/core/1.0/#anchor16
    "...key is the concatenated values (each first encoded per Parameter Encoding (Parameter Encoding)) 
    of the Consumer Secret and Token Secret, separated by an ‘&’ character (ASCII code 38) even if empty..."
    */
    
    const TDesC8* tokenSecretCtx = aCtx.GetDesC8L(WSOviContextKeys::KTokenSecret());
    HBufC8* tokenSecret = NULL;
    if (tokenSecretCtx && needSecurity)
    	{
        tokenSecret = CWSOviUtils::EncodeCharsLC(*tokenSecretCtx);
    	}
    HBufC8* encodedConsumerSecret = CWSOviUtils::EncodeCharsLC(consumerSecret); 
    RBuf8 key;
    CleanupClosePushL(key);
    if (tokenSecret && needSecurity)
        {
        key.Create(KAmpMark().Length() +
        		encodedConsumerSecret->Length() +
               tokenSecret->Length()    );
        }
    else
        {
        key.Create(KAmpMark().Length() +
        		encodedConsumerSecret->Length());
        }
    
    key.Append(*encodedConsumerSecret);
    key.Append(KAmpMark());
    if (tokenSecret && needSecurity)
        {
        key.Append(*tokenSecret);
        }
    TLSLOG_ALL(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, key);        
    //CleanupStack::PopAndDestroy(encodedSignatureSecret);
    CHMAC* hmac = CHMAC::NewL(key, sha1);//sha1 is consumed

    CleanupStack::PopAndDestroy(&key);
    CleanupStack::PopAndDestroy(encodedConsumerSecret);
    if (tokenSecretCtx && needSecurity)
    	{
    	CleanupStack::PopAndDestroy(tokenSecret);
    	}
    CleanupStack::Pop(sha1);
    CleanupStack::PushL(hmac);    
    TPtrC8 signatureValue = hmac->Final(*strToSign); //(hash);

    
    
    TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,_L("CWSOviSecurityHandler::   ____6____ sign (with HC client secret %S)"), &signatureValue));    
//____7____ append signature to http header

    newValue.Append(KParSignature());
    newValue.Append(KEqual());
    newValue.Append(KQtMark());
    HBufC8* base64Signature = SenCryptoUtils::EncodeBase64L(signatureValue);
    CleanupStack::PopAndDestroy(hmac);      
    CleanupStack::PushL(base64Signature);
    
//____8____  URL-encoded 
    HBufC8* encodedSignature = CWSOviUtils::EncodeCharsLC(*base64Signature);
    
    newValue.Append(*encodedSignature);
    newValue.Append(KQtMark());        
        
        
        
    tp->SetPropertyL(KAuthHttpHeader, newValue, KHttpHeaderType);
    
    
    CleanupStack::PopAndDestroy(encodedSignature);
    CleanupStack::PopAndDestroy(base64Signature);
    CleanupStack::PopAndDestroy(strToSign);
    CleanupStack::PopAndDestroy(&headerToNormalize);
    if (body)
        {
        CleanupStack::PopAndDestroy(body);
        }
    CleanupStack::PopAndDestroy(&newValue);
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSOviSecurityHandler::   ____7____ append signature to http header")));
    
    //msg context keeps same tp, so we dont have to update msg context
    return KErrNone;
    }

    
SenHandler::THandlerDirection CWSOviSecurityHandler::Direction() const
    {
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSOviSecurityHandler::Direction()")));
        return SenHandler::EOutgoing;
    };
SenHandler::THandlerPhase CWSOviSecurityHandler::Phase()
    {
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSOviSecurityHandler::Phase()")));
        return SenHandler::EMessage;
    };    
TInt CWSOviSecurityHandler::InitL(MSenHandlerContext& /*aCtx*/)
    {
    return KErrNone;
    }
// END OF FILE

