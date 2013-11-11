/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies). 
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






 

#ifndef WSSTAR_SECURITY_POLICY_CONSTANTS_H
#define WSSTAR_SECURITY_POLICY_CONSTANTS_H


namespace WsSecurityPolicy 
    {	
	_LIT8(KIncludeNever, "http://schemas.xmlsoap.org/ws/2005/07/securitypolicy/IncludeToken/Never");
	_LIT8(KIncludeOnce, "http://schemas.xmlsoap.org/ws/2005/07/securitypolicy/IncludeToken/Once");
	_LIT8(KIncludeAlwaysToReceipent, "http://schemas.xmlsoap.org/ws/2005/07/securitypolicy/IncludeToken/AlwaysToRecipient");
	_LIT8(KIncludeAlways, "http://schemas.xmlsoap.org/ws/2005/07/securitypolicy/IncludeToken/Always");

    // Prefix of WS-Security Policy namespace:
	_LIT8( KWsSecuritypolicyPrefix , "sp");   
	                 
    // Namespace of WS-Security Policy:
	_LIT8( KWsSecurityPolicyNsUri , "http://schemas.xmlsoap.org/ws/2005/07/securitypolicy");
    _LIT(KSlash, "//");
    _LIT8(KColon, ":");
    _LIT8(KTrue, "true");
    _LIT8(KFalse, "false");

    _LIT8(KXPath, "XPath"); 
    _LIT8(KXPathVersion, "XPathVersion"); 

    _LIT8(KHeader, "Header"); 
    _LIT8(KBody, "Body"); 
    _LIT8(KName, "Name"); 
    
    _LIT8(KNamespace, "Namespace");
    //5.1.1
    _LIT8(KSignedParts, "SignedParts"); 
    //5.1.2    
    _LIT8(KSignedElements, "SignedElements"); 
    //5.2.1    
    _LIT8(KEncryptedParts, "EncryptedParts"); 
    //5.2.2    
    _LIT8(KEncryptedElements, "EncryptedElements");
    //5.3    
    _LIT8(KRequiredElements, "RequiredElements");

    //6.3.1 Username Token assertion
    _LIT8(KUsernameToken, "UsernameToken");
    _LIT8(KIncludeToken, "IncludeToken");
    _LIT8(KWssUsernameToken10, "WssUsernameToken10");
    _LIT8(KWssUsernameToken11, "WssUsernameToken11");
    //6.3.2 IssuedToken Assertion
    _LIT8(KIssuedToken, "IssuedToken");
    _LIT8(KIssuer, "Issuer");
    _LIT8(KRequestSecurityTokenTemplate, "RequestSecurityTokenTemplate");
    _LIT8(KTrustVersion, "TrustVersion");
    _LIT8(KRequireDerivedKeys, "RequireDerivedKeys");
    _LIT8(KRequireExternalReference, "RequireExternalReference");
    _LIT8(KRequireInternalReference, "RequireInternalReference");
    //6.3.3    
    _LIT8(KX509Token, "X509Token");
    _LIT8(KRequireKeyIdentifierReference, "RequireKeyIdentifierReference");
    _LIT8(KRequireIssuerSerialReference, "RequireIssuerSerialReference");
    _LIT8(KRequireEmbeddedTokenReference, "RequireEmbeddedTokenReference");
    _LIT8(KRequireThumbprintReference, "RequireThumbprintReference");
    
            
    _LIT8(KWssX509V1Token10, "WssX509V1Token10");
    _LIT8(KWssX509V3Token10, "WssX509V3Token10");
    _LIT8(KWssX509Pkcs7Token10, "WssX509Pkcs7Token10");
    _LIT8(KWssX509PkiPathV1Token10, "WssX509PkiPathV1Token10");
    _LIT8(KWssX509V1Token11, "WssX509V1Token11");
    _LIT8(KWssX509V3Token11, "WssX509V3Token11");
    _LIT8(KWssX509Pkcs7Token11, "WssX509Pkcs7Token11");
    _LIT8(KWssX509PkiPathV1Token11, "WssX509PkiPathV1Token11");
    //6.3.4
    _LIT8(KKerberosToken, "KerberosToken");
    _LIT8(KWssKerberosV5ApReqToken11, "WssKerberosV5ApReqToken11");
    _LIT8(KWssGssKerberosV5ApReqToken11, "WssGssKerberosV5ApReqToken11");
    //6.3.5    
    _LIT8(KSpnegoContextToken, "SpnegoContextToken");
    //6.3.6
    _LIT8(KSecurityContextToken, "SecurityContextToken");
    _LIT8(KRequireExternalUriReference, "RequireExternalUriReference");
    _LIT8(KSC10SecurityContextToken, "SC10SecurityContextToken");
    //6.3.7 SecureConversationtoken asseertion
    _LIT8(KSecureConversationToken, "SecureConversationToken");
    _LIT8(KBootstrapPolicy, "BootstrapPolicy");
    //6.3.8 SamlToken Assertion    
    _LIT8(KSamlToken, "SamlToken");
    _LIT8(KWssSamlV10Token10, "WssSamlV10Token10");
    _LIT8(KWssSamlV11Token10, "WssSamlV11Token10");
    _LIT8(KWssSamlV10Token11, "WssSamlV10Token11");
    _LIT8(KWssSamlV11Token11, "WssSamlV11Token11");
    _LIT8(KWssSamlV20Token11, "WssSamlV20Token11");
    //6.3.9 RelToken
    _LIT8(KRelToken, "RelToken");
    _LIT8(KWssRelV10Token10, "WssRelV10Token10");
    _LIT8(KWssRelV20Token10, "WssRelV20Token10");
    _LIT8(KWssRelV10Token11, "WssRelV10Token11");
    _LIT8(KWssRelV20Token11, "WssRelV20Token11");
    //6.3.10 Https TokenAssertion
    _LIT8(KHttpsToken, "HttpsToken");
    _LIT8(KRequireClientCertificate, "RequireClientCertificate");
    //8.1 Alogorithm suite assertion
    _LIT8(KAlgorithmSuite, "AlgorithmSuite");
    
        
    _LIT8(KBasic256, "Basic256");
    _LIT8(KBasic192, "Basic192");
    _LIT8(KBasic128, "Basic128");
    _LIT8(KTripleDes, "TripleDes");
    _LIT8(KBasic256Rsa15, "Basic256Rsa15");
    _LIT8(KBasic192Rsa15, "Basic192Rsa15");
    _LIT8(KBasic128Rsa15, "Basic128Rsa15");
    _LIT8(KTripleDesRsa15, "TripleDesRsa15");
    _LIT8(KBasic256Sha256, "Basic256Sha256");
    _LIT8(KBasic192Sha256, "Basic192Sha256");
    _LIT8(KBasic128Sha256, "Basic128Sha256");
    _LIT8(KTripleDesSha256, "TripleDesSha256");
    _LIT8(KBasic256Sha256Rsa15, "Basic256Sha256Rsa15");
    _LIT8(KBasic192Sha256Rsa15, "Basic192Sha256Rsa15");
    _LIT8(KBasic128Sha256Rsa15, "Basic128Sha256Rsa15");
    _LIT8(KTripleDesSha256Rsa15, "TripleDesSha256Rsa15");
    _LIT8(KInclusiveC14N, "InclusiveC14N");
    _LIT8(KSoapNormalization10, "SoapNormalization10");
    _LIT8(KStrTransform10, "StrTransform10");
    _LIT8(KXPath10, "XPath10");
    _LIT8(KXPathFilter20, "XPathFilter20");
    //8.2 Layout assertion
    _LIT8(KLayout, "Layout");
    _LIT8(KStrict, "Strict");
    _LIT8(KLax, "Lax");
    _LIT8(KLaxTsFirst, "LaxTsFirst");
    _LIT8(KLaxTsLast, "LaxTsLast");
    //8.3 TransportBinding assertion    
    _LIT8(KTransportBinding, "TransportBinding");
    _LIT8(KTransportToken, "TransportToken");
    _LIT8(KIncludeTimestamp, "IncludeTimestamp");
    
    //8.4 Symmetric binding    
    _LIT8(KSymmetricBinding, "SymmetricBinding");
    _LIT8(KEncryptionToken, "EncryptionToken");
    _LIT8(KSignatureToken, "SignatureToken");
    _LIT8(KProtectionToken, "ProtectionToken");
    //Binding commons    
    _LIT8(KEncryptBeforeSigning, "EncryptBeforeSigning");
    _LIT8(KSignBeforeEncrypting, "SignBeforeEncrypting");
    _LIT8(KEncryptSignature, "EncryptSignature");
    _LIT8(KProtectTokens, "ProtectTokens");
    _LIT8(KOnlySignEntireHeadersAndBody, "OnlySignEntireHeadersAndBody");
    //8.5 Assymetric binding assertion    
    _LIT8(KAsymmetricBinding, "AsymmetricBinding");
    _LIT8(KInitiatorToken, "InitiatorToken");
    _LIT8(KRecipientToken, "RecipientToken");
    //9.1    
    _LIT8(KSupportingTokens, "SupportingTokens");
    //9.2    
    _LIT8(KSignedSupportingTokens, "SignedSupportingTokens");
    //9.3    
    _LIT8(KEndorsingSupportingTokens, "EndorsingSupportingTokens");
    // 9.4    
    _LIT8(KSignedEndorsingSupportingTokens, "SignedEndorsingSupportingTokens");
    //10. WSS SOAP message security options
    //Wss10 Assertion
    _LIT8(KWss10, "Wss10");
    _LIT8(KMustSupportRefKeyIdentifier, "MustSupportRefKeyIdentifier");
    _LIT8(KMustSupportRefIssuerSerial, "MustSupportRefIssuerSerial");
    _LIT8(KMustSupportRefExternalURI, "MustSupportRefExternalURI");
    _LIT8(KMustSupportRefEmbeddedToken, "MustSupportRefEmbeddedToken");
    //Wss11 Assertion in addition to Wss10
    _LIT8(KWss11, "Wss11");
    _LIT8(KMustSupportRefThumbprint, "MustSupportRefThumbprint");
    _LIT8(KMustSupportRefEncryptedKey, "MustSupportRefEncryptedKey");
    _LIT8(KRequireSignatureConfirmation, "RequireSignatureConfirmation");
    //Trust10 Assertion
    _LIT8(KTrust10, "Trust10");
    _LIT8(KMustSupportClientChallenge, "MustSupportClientChallenge");
    _LIT8(KMustSupportServerChallenge, "MustSupportServerChallenge");
    _LIT8(KRequireClientEntropy, "RequireClientEntropy");
    _LIT8(KRequireServerEntropy, "RequireServerEntropy");
    _LIT8(KMustSupportIssuedTokens, "MustSupportIssuedTokens");

/*    

    SUPPORTING_TOKEN_SUPPORTING = 1;
    SUPPORTING_TOKEN_ENDORSING = 2;
    SUPPORTING_TOKEN_SIGNED = 3;
    SUPPORTING_TOKEN_SIGNED_ENDORSING = 4;
*/    
  
    ///
    ///Algorithms Namespaces
    ///
    _LIT8(KAlgoHMAC_SHA1, "http://www.w3.org/2000/09/xmldsig#hmac-sha1");
    _LIT8(KAlgoRSA_SHA1, "http://www.w3.org/2000/09/xmldsig#rsa-sha1");
    _LIT8(KAlgoSHA1, "http://www.w3.org/2000/09/xmldsig#sha1");
    _LIT8(KAlgoSHA256, "http://www.w3.org/2001/04/xmlenc#sha256");
    _LIT8(KAlgoSHA512, "http://www.w3.org/2001/04/xmlenc#sha512");
    _LIT8(KAlgoAES128, "http://www.w3.org/2001/04/xmlenc#aes128-cbc");
    _LIT8(KAlgoAES192, "http://www.w3.org/2001/04/xmlenc#aes192-cbc");
    _LIT8(KAlgoAES256, "http://www.w3.org/2001/04/xmlenc#aes256-cbc");
    _LIT8(KAlgoTRIPLE_DES, "http://www.w3.org/2001/04/xmlenc#tripledes-cbc");
    _LIT8(KAlgoKW_AES128, "http://www.w3.org/2001/04/xmlenc#kw-aes256");
    _LIT8(KAlgoKW_AES192, "http://www.w3.org/2001/04/xmlenc#kw-aes192");
    _LIT8(KAlgoKW_AES256, "http://www.w3.org/2001/04/xmlenc#kw-aes128");
    _LIT8(KAlgoKW_TRIPLE_DES, "http://www.w3.org/2001/04/xmlenc#kw-tripledes");
    _LIT8(KAlgoKW_RSA_OAEP, "http://www.w3.org/2001/04/xmlenc#rsa-oaep-mgf1p");
    _LIT8(KAlgoKW_RSA15, "http://www.w3.org/2001/04/xmlenc#rsa-1_5");
    _LIT8(KAlgoP_SHA1, "http://schemas.xmlsoap.org/ws/2005/02/sc/dk/p_sha1");
    _LIT8(KAlgoP_SHA1_L128, "http://schemas.xmlsoap.org/ws/2005/02/sc/dk/p_sha1");
    _LIT8(KAlgoP_SHA1_L192, "http://schemas.xmlsoap.org/ws/2005/02/sc/dk/p_sha1");
    _LIT8(KAlgoP_SHA1_L256, "http://schemas.xmlsoap.org/ws/2005/02/sc/dk/p_sha1");
    _LIT8(KAlgoC14N, "http://www.w3.org/2001/10/xml-c14n#");
    _LIT8(KAlgoEX_C14N, "http://www.w3.org/2001/10/xml-exc-c14n#");
    _LIT8(KAlgoSNT, "http://www.w3.org/TR/soap12-n11n");
    _LIT8(KAlgoSTRT10, "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-soap-message-security-1.0#STR-Transform");
    _LIT8(KAlgoXPATH, "http://www.w3.org/TR/1999/REC-xpath-19991116");
    _LIT8(KAlgoXPATH20, "http://www.w3.org/2002/06/xmldsig-filter2");
    }

#endif //WSSTAR_SECURITY_POLICY_CONSTANTS_H