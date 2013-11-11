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






 

#ifndef WSSTAR_SECURITY_POLICY_H
#define WSSTAR_SECURITY_POLICY_H

#include <SenElement.h>

#include "wssecuritypolicyconstants.h"

    
// CLASS DECLARATION
/**
 * Class: CWsSecurityPolicy
 *
 * Description: Class which can holds the information found in a WS SecurityPolicy Assertion
 *              The information can be Intersected and Merged with other  CWsSecurityPolicy 
 *              object and a resultant CSenElement* will be provided holding Intersected or Merged data.
 */
class CWsSecurityPolicy : public CBase
    {
    public:

        /**
        * Standard 2 phase constructor
        * @since Series60 5.0
        * @return new instance of CWsSecurityPolicy object
        */
        static CWsSecurityPolicy* NewL();
        static CWsSecurityPolicy* NewLC();

        virtual ~CWsSecurityPolicy();
        /**
        * Intersect the two WsSecurityPolicy Assertions
        * @since Series60 5.0
        * @param aKey: Name of the WsSecurityPolicy assertion
        * @param aFirst: First assertion to intersect
        * @param aSecond: Second assertion to intersect with aFirst
        * @return new Element containing the Intersected Element 
        * or NULL if intersection return no result
        */

        CSenElement* IntersectL(TDesC8& aKey, CSenElement* aFirst, CSenElement* aSecond);	//codescannerwarnings

        /**
        * Merge the two WsSecurityPolicy Assertions
        * @since Series60 5.0
        * @param aKey: Name of the WsSecurityPolicy assertion
        * @param aFirst: First assertion to merge
        * @param aSecond: Second assertion to merge with aFirst
        * @return new Element containing the Merged Element 
        * or NULL if merging return no result
        */
        CSenElement* MergeL(TDesC8& aKey, CSenElement* aFirst, CSenElement* aSecond);	//codescannerwarnings

    private:
        void ConstructL();
        CWsSecurityPolicy();
    };

/**
 * Class: CAlgorithmSuite
 *
 * Description: Class which can holds the information found in a WS-SecurityPolicy Assertion "AlgorithmSuite"
 * and provide information related to that particular assertion
 */

class CAlgorithmSuite : public CBase
    {
    public:
        static CAlgorithmSuite* NewL(CSenElement* aAlgoSuite);
        static CAlgorithmSuite* NewLC(CSenElement* aAlgoSuite);
        virtual ~CAlgorithmSuite();
  
        TPtrC8 AlgoSuite();
        TPtrC8 SymmetricSignature();
        TPtrC8 AsymmetricSignature();
        TPtrC8 ComputedKey();
        TInt MaximumSymmetricKeyLength();
        TInt MinimumAsymmetricKeyLength();
        TInt MaximumAsymmetricKeyLength();
        TPtrC8 Digest();
        TPtrC8 Encryption();
        TPtrC8 SymmetricKeyWrap();
        TPtrC8 AsymmetricKeyWrap();
        TPtrC8 EncryptionKeyDerivation();
        TPtrC8 SignatureKeyDerivation();
        TInt MinimumSymmetricKeyLength();
        TPtrC8 C14n();
        TPtrC8 SoapNormalization();
        TPtrC8 StrTransform();
        TPtrC8 XPath();
        TPtrC8 XPathFilter20();


        void SetC14nL(TDesC8& aValue);	//codescannerwarnings
        void SetSoapNormalizationL(TDesC8& aValue);	//codescannerwarnings
        void SetStrTransformL(TDesC8& aValue);	//codescannerwarnings
        void SetXPathL(TDesC8& aValue);	//codescannerwarnings
        void SetXPathFilter20L(TDesC8& aValue);	//codescannerwarnings

        TBool IsEqualAlgoSuite(CAlgorithmSuite* aAlgoSuite);

    private:
        void ConstructL(CSenElement* aAlgoSuite);
        void SetAlgoSuiteL(TDesC8& aSuiteName);	//codescannerwarnings
        CAlgorithmSuite();
        
    private: // Data
        HBufC8* iAlgoSuite;
        
        HBufC8* iSymmetricSignature;
        HBufC8* iAsymmetricSignature;
        HBufC8*  iComputedKey;
        TInt iMaximumSymmetricKeyLength;
        TInt iMinimumAsymmetricKeyLength;
        TInt iMaximumAsymmetricKeyLength;
        HBufC8* iDigest;
        HBufC8* iEncryption;
        HBufC8* iSymmetricKeyWrap;
        HBufC8* iAsymmetricKeyWrap;
        HBufC8* iEncryptionKeyDerivation;
        HBufC8* iSignatureKeyDerivation;
        TInt iMinimumSymmetricKeyLength;
        HBufC8* iC14n; 
        HBufC8* iSoapNormalization;
        HBufC8* iStrTransform;
        HBufC8* iXPath;
        HBufC8* iXPathFilter20; 
    };


class CXmlHelper : public CBase
    {
    public:
        static CSenElement* RipPolicyXORElementL(CSenElement* aAsymmBinding);	//codescannerwarnings   
        static TBool HasAttributeL(CSenElement* aAsymmBinding, const TDesC8& aAttrib);
        static TPtrC8 AttributeValueL(CSenElement* aAssertion, const TDesC8& aAttrib);
    private:        
        CXmlHelper() { }
        CXmlHelper( const CXmlHelper& );
        CXmlHelper& operator=( const CXmlHelper& );
    
    };

/**
 * Class: CLayout
 *
 * Description: Class which can holds the information found in a WS-SecurityPolicy Assertion "Layout"
 * and provide information related to that particular assertion
 */

class CLayout : public CBase
    {
    public:
        static CLayout* NewL(CSenElement* aLayout);
        static CLayout* NewLC(CSenElement* aLayout);
        virtual ~CLayout();

        TPtrC8 Layout() { return iLayout;}
        TBool IsEqualLayout(CLayout* aLayout);

    private:
        void ConstructL(CSenElement* aLayout);
        CLayout();
    private: // data
        TPtrC8 iLayout;
    };
////////////////////////////////////////////////////
////////////////////////////////////////////////////
/**
 * Class: CToken
 *
 * Description: Base class to hold WS-SecurityPolicy Assertion "Token" common values
 */

class CToken : public CBase
    {
    public:
        virtual ~CToken();
    protected:
        CToken();
        
        inline TPtrC8 GetTokenInclusion() {return iTokenInclusion;}
        inline TBool DerivedKeys() {return iDerivedKeys;}
        inline void SetDerivedKeys(TBool aDerivedKeys) { iDerivedKeys = aDerivedKeys; }
        void BaseConstructL(CSenElement* aToken);
    private:
        void ConstructL(CSenElement* aToken);
    private: // Data
        TPtrC8 iTokenInclusion;
        TBool iDerivedKeys;
    };
//TOKEN TYPES

/**
 * Class: CUsernameToken
 *
 * Description: Class to hold WS-SecurityPolicy Assertion "UsernameToken"
 * This class extends the CToken class
 */
class CUsernameToken : public CToken
    {
    public:
        static CUsernameToken* NewL(CSenElement* aToken);
        static CUsernameToken* NewLC(CSenElement* aToken);
        virtual ~CUsernameToken();
        TPtrC8 UserNameToken() {return iUsernameTokenProfile;}
    private:
        CUsernameToken();  
        void ConstructL(CSenElement* aToken);

        TPtrC8 iUsernameTokenProfile;
    };

class CRequestSecurityTokenTemplate :public CBase
    {
    public:
        static CRequestSecurityTokenTemplate* NewL(CSenElement* aToken);
        static CRequestSecurityTokenTemplate* NewLC(CSenElement* aToken);
        virtual ~CRequestSecurityTokenTemplate();
        TPtrC8 Token() {return iToken->Des();}
        TPtrC8 TrustVersion() {return iTrustVersion->Des();}
    private:
        CRequestSecurityTokenTemplate();  
        void ConstructL(CSenElement* aToken);

    private: // Data
        HBufC8* iToken;
        HBufC8* iTrustVersion;
    };

class CHttpsToken : public CToken
{
    public: 
        static CHttpsToken* NewL(CSenElement* aToken);
        static CHttpsToken* NewLC(CSenElement* aToken);
        virtual ~CHttpsToken();
        TBool RequireClientCertificate() {return iRequireClientCertificate; }
        CSenElement* HttpsTokenPolicy() {return iPolicy; }
  
    private:
        CHttpsToken();  
        void ConstructL(CSenElement* aToken);

    private: // Data
        TBool iRequireClientCertificate;
        CSenElement* iPolicy;

};

class CSamlToken : public CToken
    {
    public:
        static CSamlToken* NewL(CSenElement* aToken);
        static CSamlToken* NewLC(CSenElement* aToken);
        virtual ~CSamlToken();
        TBool RequireDerivedKeys() {return iRequireDerivedKeys; }
        TBool RequireKeyIdentifierReference() {return iRequireKeyIdentifierReference; }
        TPtrC8 SamlTokenProfile() {return iSamlTokenProfile; }

    private:
        CSamlToken();  
        void ConstructL(CSenElement* aToken);

    private: // Data
        TBool iRequireDerivedKeys;
        TBool iRequireKeyIdentifierReference;
        TPtrC8 iSamlTokenProfile;
    };
    
class CSecureConversationToken : public CToken
    {
    public:
        static CSecureConversationToken* NewL(CSenElement* aToken);
        static CSecureConversationToken* NewLC(CSenElement* aToken);
        virtual ~CSecureConversationToken();

        TPtrC8 Issuer();
        TBool RequireExternalUriReference();
        TBool SC10SecurityContextToken();
        TBool RequireDerivedKeys() {return iRequireDerivedKeys; }
        TBool IsBootstrapPolicy();
        CSenElement* BootstrapPolicy();
        
    private:
        CSecureConversationToken();  
        void ConstructL(CSenElement* aToken);

    private: // Data
        HBufC8* iIssuer;
        TBool iRequireExternalUriReference;
        TBool iSC10SecurityContextToken;
        TBool iRequireDerivedKeys;
        TBool iBootstrap;
        CSenElement* iBootstrapPolicy;
    };



/*
<wsp:Policy>
<sp:IssuedToken sp:IncludeToken="http://schemas.xmlsoap.org/ws/2005/07/securitypolicy/IncludeToken/AlwaysToRecipient">
<sp:Issuer> 
    <EndpointReference xmlns="http://schemas.xmlsoap.org/ws/2004/08/addressing">
    <Address>http://AdressOfTheSTS.com</Address>
</sp:Issuer>
<sp:RequestSecurityTokenTemplate>
    <!-- Policy defined by the Service for the STS -->
    <wst:TokenType>urn:oasis:names:tc:SAML:1.1</wst:TokenType>
    <wst:KeyType> http://schemas.xmlsoap.org/ws/2004/04/trust/SharedKey </wst:KeyType>
</sp:RequestSecurityTokenTemplate>
</sp:IssuedToken>
</wsp:Policy>
*/
class CIssuedToken : public CToken
    {
    public:
        static CIssuedToken* NewL(CSenElement* aToken);
        static CIssuedToken* NewLC(CSenElement* aToken);
        virtual ~CIssuedToken();
        CSenElement* Issuer() {return iIssuer; }
        TBool RequireDerivedKeys() {return iRequireDerivedKeys; }
        CSenElement* RequestSecurityTokenTemplate() {return  iRequestSecurityTokenTemplate;}
    private:
        CIssuedToken();  
        void ConstructL(CSenElement* aToken);

    private: // Data
        CSenElement* iIssuer; //not owned
        TBool iRequireDerivedKeys;
        CSenElement* iRequestSecurityTokenTemplate; //not owned
        TBool iRequireExternalReference;
        TBool iRequireInternalReference;
      
    };

//TOKEN HOLDER
/**
 * Class: CTokenHolder
 *
 * Description: Class to hold different kind of tokens.
 * It also holds the name of the Token and the token is casted to that particulat token type.
 */

class CTokenHolder : public CBase
{
    public:
        static CTokenHolder* NewL(CSenElement* aToken);
        static CTokenHolder* NewLC(CSenElement* aToken);
        virtual ~CTokenHolder();

    private:

        void ConstructL(CSenElement* aToken);
        CTokenHolder();

    private: // Data
        TPtrC8  iTokenType;
        CToken* iToken;
};

////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
class CBinding  : public CBase
{
    public:
        static CBinding* NewL(CSenElement* aBinding);
        static CBinding* NewLC(CSenElement* aBinding);
        virtual ~CBinding();

    protected:
        CBinding(); 
        CAlgorithmSuite* GetAlgoSuite() {return iAlgorithmSuite;}
        CLayout* GetLayout() {return iLayout;}
        TBool IncludetimeStamp() {return iIncludeTimestamp;}
        void SetbaseElementL(CSenElement* aBinding);

    private:
        void ConstructL(CSenElement* aBinding);

    private: // Data
        CAlgorithmSuite* iAlgorithmSuite;
        CLayout* iLayout;
        TBool iIncludeTimestamp;
};

class CSymmAsymmBinding : public CBinding
    {
    public:
        static CSymmAsymmBinding* NewL(CSenElement* aBinding);
        static CSymmAsymmBinding* NewLC(CSenElement* aBinding);
        virtual ~CSymmAsymmBinding();

    protected:
        void BaseConstructL(CSenElement* aBinding);
        CSymmAsymmBinding();

        TPtrC8 EncryptBeforeSigning() {return iProtectionOrder;}
        TBool  EncryptSignature() {return iEncryptSignature;}
        TBool  ProtectTokens() {return iProtectTokens;}
        TBool  OnlySignEntireHeadersAndBody() {return iOnlySignEntireHeadersAndBody;}

    private:
        void ConstructL(CSenElement* aBinding);

    private: // Data
        TPtrC8 iProtectionOrder;
        TBool iEncryptSignature;
        TBool iProtectTokens;
        TBool iOnlySignEntireHeadersAndBody;  
    };

class CTranportBinding : public CBinding
    {
    public:
        static CTranportBinding* NewL(CSenElement* aTransportBinding);
        static CTranportBinding* NewLC(CSenElement* aTransportBinding);
        virtual ~CTranportBinding();
        TBool IsEqualTransportBindingAssertion(CTranportBinding* aTransportBinding);
        
    protected:
        CTokenHolder* GetTransportToken() {return iTransportToken;}
        
    private:
        void ConstructL(CSenElement* aTransportBinding);
        CTranportBinding();

    private: // Data
        CTokenHolder* iTransportToken;
    };


class CAsymmetricBinding : public CSymmAsymmBinding
    {
    public:
        static CAsymmetricBinding* NewL(CSenElement* aAsymmBinding);
        static CAsymmetricBinding* NewLC(CSenElement* aAsymmBinding);
        virtual ~CAsymmetricBinding();
        TBool IsEqualAsymmtericAssertion(CAsymmetricBinding* aAsymmetricBinding);

    protected:
        CTokenHolder* GetInitiatorToken() {return iInitiatorToken;}
        CTokenHolder* GetRecipientToken() {return iRecipientToken;}      

    private:
        void ConstructL(CSenElement* aAsymmBinding);
        CAsymmetricBinding();

    private: // Data
        CTokenHolder* iInitiatorToken;
        CTokenHolder* iRecipientToken;

};


class CSymmetricBinding : public CSymmAsymmBinding
    {
    public:
        static CSymmetricBinding* NewL(CSenElement* aSymmBinding);
        static CSymmetricBinding* NewLC(CSenElement* aSymmBinding);
        virtual ~CSymmetricBinding();
        TBool IsEqualSymmtericAssertion(CSymmetricBinding* aSymmetricBinding);

    protected:
        CTokenHolder* GetEncryptionToken() {return iEncriptionToken;}
        CTokenHolder* GetSignatureToken() {return iSignatureToken;}      

    private:
        void ConstructL(CSenElement* aSymmBinding);
        CSymmetricBinding();

    private: // Data
        CTokenHolder* iEncriptionToken;
        CTokenHolder* iSignatureToken;
        CTokenHolder* iProtectionToken;
    };

class CWss10 :public CBase
    {
    public:
        static CWss10* NewL(CSenElement* aAssertion);
        static CWss10* NewLC(CSenElement* aAssertion);
        virtual ~CWss10();

        TBool IsEqualWss10Assertion(CWss10* aAssertion);

        TBool MustSupportRefKeyIdentifier();
        TBool MustSupportRefIssuerSerial();
        TBool MustSupportRefExternalURI();
        TBool MustSupportRefEmbeddedToken();
        
    protected:
        void BaseConstructL(CSenElement* aAssertion); 
        CWss10();
        
    private:
        void ConstructL(CSenElement* aAssertion);

    private: // Data
        TBool iMustSupportRefKeyIdentifier;
        TBool iMustSupportRefIssuerSerial;
        TBool iMustSupportRefExternalURI;
        TBool iMustSupportRefEmbeddedToken;
    };

class CWss11 : public CWss10
    {
    public:
        static CWss11* NewL(CSenElement* aAssertion);
        static CWss11* NewLC(CSenElement* aAssertion);
        virtual ~CWss11();

        TBool IsEqualWss11Assertion(CWss11* aAssertion);

        TBool MustSupportRefThumbprint();
        TBool MustSupportRefEncryptedKey();
        TBool RequireSignatureConfirmation();  

    private:
        void ConstructL(CSenElement* aAssertion);
        CWss11();

    private: // Data
        TBool iMustSupportRefThumbprint;
        TBool iMustSupportRefEncryptedKey;
        TBool iRequireSignatureConfirmation;

    };

class CWsTrust10 :public CBase
    {
    public:
        static CWsTrust10* NewL(CSenElement* aAssertion);
        static CWsTrust10* NewLC(CSenElement* aAssertion);
        virtual ~CWsTrust10();

        TBool IsEqualWsTrust10Assertion(CWsTrust10* aAssertion);
        
        TBool MustSupportClientChallenge();
        TBool MustSupportServerChallenge();
        TBool RequireClientEntropy();
        TBool RequireServerEntropy();
        TBool MustSupportIssuedTokens();

    private:
        void ConstructL(CSenElement* aAssertion);
        CWsTrust10(); 

    private: // Data
        TBool iMustSupportClientChallenge;
        TBool iMustSupportServerChallenge;
        TBool iRequireClientEntropy;
        TBool iRequireServerEntropy;
        TBool iMustSupportIssuedTokens;
    };

struct SoapHeader
    {
	TPtrC8 iName;
	TPtrC8 iNsUri;
    };

class CEncryptedSignedParts : public CBase
    {
    public:
        static CEncryptedSignedParts* NewL(CSenElement* aAssertion);
        static CEncryptedSignedParts* NewLC(CSenElement* aAssertion);
        virtual ~CEncryptedSignedParts();

        TBool IsEqualEncryptedSignedParts(CEncryptedSignedParts* aAssertion);
        void ElementsL(RPointerArray<HBufC8>& aElements);	//codescannerwarnings
        TBool Header();
        TBool Body();
    //    void GetHeaders()
    private:
        void ConstructL(CSenElement* aAssertion);
        CEncryptedSignedParts(); 

    private: // Data
        TBool iHeader;
        TBool iBody;
        RArray<SoapHeader> iHeaders;  
        
    };

class CEncryptedSignedElements : public CBase
    {
    public:
        static CEncryptedSignedElements* NewL(CSenElement* aAssertion);
        static CEncryptedSignedElements* NewLC(CSenElement* aAssertion);
        virtual ~CEncryptedSignedElements();
        
        TBool IsEqualEncryptedSignedElements(CEncryptedSignedElements* aAssertion);
        TBool MergedElements(CEncryptedSignedElements* aAssertion, RArray<TPtrC8>& aResult);
        TPtrC8 XPathVersion() { return iXPathVersion->Des(); }
        
        TBool IfNsQualified(CSenElement* aAssertion, TDesC8& aXpath);
        TInt GetXPaths(RArray<TPtrC8>& aXpaths); //will return copy of xpaths and RArray ownership

    private:
        void ConstructL(CSenElement* aAssertion);
        CEncryptedSignedElements(); 

    private: // Data
        HBufC8* iXPathVersion;
        RArray<TPtrC8> iXpaths;
    };


#endif //WSSTAR_SECURITY_POLICY_H

// END OF FILE





