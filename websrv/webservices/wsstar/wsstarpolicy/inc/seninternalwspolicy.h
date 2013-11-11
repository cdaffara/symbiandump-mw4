/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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








#ifndef SEN_INTERNAL_WS_POLICY_H
#define SEN_INTERNAL_WS_POLICY_H

//  INCLUDES
#include <e32base.h>
#include <s32strm.h>

#include <SenFragmentBase.h>
#include <SenDomFragmentBase.h>
#include <SenBaseFragment.h>
#include <SenDomFragment.h>

// FORWARD DECLARATIONS
class SenDateUtils;
class CSenCredentialIdentifier;
class CSenWSDescription;
class CSenIdentityProvider;
class CSenServiceSession;
class CSenFacet;
class CSenElement;

class CSenWSPolicyProperties;
class CSenWSPolicyIdentifier;
class CSenWSDescription;
class RSenWSPolicyPtr;
class CSenWSPolicyMetadata;
// CLASS DECLARATION

namespace SenWSInternalPolicy
    {
// CONSTANTS

    _LIT8(KSenWsPolicies,             "SenWsPolicies");
    _LIT8(KEndpointLocalname,           "Endpoint");
    _LIT8(KContractLocalname,           "Contract");    
    _LIT8(KProviderIdLocalName,             "ProviderID");
    
    _LIT8(KMetadataEndpointLocalname,           "MetadataEndpoint"); 
    _LIT8(KMetadataMethodLocalname,           "method");
    _LIT8(KServicePolicyLocalName,       "ServicePolicy");    
    _LIT8(KClientPolicyLocalName,       "ClientPolicy");
    
    
    _LIT8(KWsdlDocument, "definitions");
    _LIT8(KWsdlNsPrefix, "wsdl");
    _LIT8(KPolicyDocument, "Policy");
    _LIT8(KPolicyNsPrefix, "wsp");
    
    _LIT8(KSenWSPolicyId,                 "WsPolicyId");
    _LIT8(KSenWSPolicyValidUntil,           "ValidUntil");
    
    _LIT8(KSenWsPolicyIdentifierLocalname,"Identifier");
    _LIT8(KSenWsPolicyProperteisLocalname,"Properties");
    _LIT8(KSenWSPolicyMetadata,"PolicyMetadata");
    
    _LIT8(KWsPoliciesMaxID,        "PolicyMaxID");
    _LIT8(KWsPolicyContainer,     "SenWsPolicyContainer");
    
    }

/**
* Class implements (web) service policy functionality
*  @since Series60 3.0
*/
class CSenInternalWsPolicy : public CSenDomFragment
    {
    public:  // Constructors and destructor

        /**
        * Basic constructor.
        *
        * @since Series60 4.0
        */
        static CSenInternalWsPolicy* NewL();

        /**
        * Basic constructor.
        *
        * @since Series60 4.0
        */
        static CSenInternalWsPolicy* NewLC();

        /**
        * Basic constructor.
        *
        * @since Series60 4.0
        * @param aNsUri     is the XML namespace of this credential
        * @param aLocalName is the XML localname of this credential
        * @param aPrefix    is the prefix of this credential
        * @param aAttrs     are the XML attributes of this credential
        */
        static CSenInternalWsPolicy* NewL(const TDesC8& aNsUri,
                                            const TDesC8& aLocalName,
                                            const TDesC8& aQName,
                                            const RAttributeArray& aAttributes);

        /**
        * Basic constructor.
        *
        * @since Series60 4.0
        * @param aNsUri     is the XML namespace of this credential
        * @param aLocalName is the XML localname of this credential
        * @param aPrefix    is the prefix of this credential
        * @param aAttrs     are the XML attributes of this credential
        */
        static CSenInternalWsPolicy* NewLC(const TDesC8& aNsUri,
                                             const TDesC8& aLocalName,
                                             const TDesC8& aQName,
                                             const RAttributeArray& aAttributes);

        /**
        * Basic constructor.
        *
        * @since Series60 4.0
        * @param aNsUri     is the XML namespace of this credential
        * @param aLocalName is the XML localname of this credential
        * @param aPrefix    is the prefix of this credential
        * @param aAttrs     are the XML attributes of this credential
        * @param aParent    is the parent XML element of this credential
        */
        static CSenInternalWsPolicy* NewL(const TDesC8& aNsUri,
                                            const TDesC8& aLocalName,
                                            const TDesC8& aQName,
                                            const RAttributeArray& aAttributes,
                                            CSenElement& aParent);

        /**
        * Basic constructor.
        *
        * @since Series60 4.0
        * @param aNsUri     is the XML namespace of this credential
        * @param aLocalName is the XML localname of this credential
        * @param aPrefix    is the prefix of this credential
        * @param aAttrs     are the XML attributes of this credential
        * @param aParent    is the parent XML element of this credential
        */
        static CSenInternalWsPolicy* NewLC(const TDesC8& aNsUri,
                                             const TDesC8& aLocalName,
                                             const TDesC8& aQName,
                                             const RAttributeArray& aAttributes,
                                             CSenElement& aParent);

        /**
        * Constructor which copies itself from another credential.
        *
        * @since Series60 4.0
        * @param    aPolicy     credential to copy from.
        * @return new CSenCredential instance pointer
        */
        static CSenInternalWsPolicy* NewL(const CSenInternalWsPolicy& aPolicy);

        /**
        * Constructor which copies itself from another credential.
        *
        * @since Series60 4.0
        * @param    aPolicy     credential to copy from.
        * @return new CSenCredential instance pointer, which is left on
        *         cleanup stack.
        */
        static CSenInternalWsPolicy* NewLC(const CSenInternalWsPolicy& aPolicy);

        /**
        * Destructor.
        */
        virtual ~CSenInternalWsPolicy();

        // New functions

        /**
        * Method for checking if this credential is applicable for
        * given pattern.
        *
        * @since Series60 4.0
        * @param    aPattern    pattern to check.
        * @return   ETrue       if Credential is applicable
        *           EFalse      if Credential is not applicable
        */
        TBool IsApplicableL(CSenWSDescription& aPattern);	//codescannerwarnings
        TBool IsApplicableL(const TDesC8& aPolicyEndpoint);	//codescannerwarnings
        TBool IsApplicableL(CSenInternalWsPolicy* aInternalPolicy);	//codescannerwarnings
        /**
        * Method for setting CredentialIdentifier.
        *
        * @since Series60 4.0
        * @param    aIdentifier identifier to set.
        */
        void SetIdentifier(CSenWSPolicyIdentifier* aIdentifier);

        /**
        * Method for getting CredentialIdentifier.
        *
        * @since Series60 4.0
        * @return   Identifier for this Credentiel
        */
        CSenWSPolicyIdentifier& IdentifierL();	//codescannerwarnings

        /**
        * Method for setting ServiceSession for this Credential.
        * Setting ServiceSession does build relation between this
        * Credential and given ServiceSession.
        *
        * @since Series60 4.0
        * @param   aSession             ServiceSession for this Credentiel
        */
        void SetSession(CSenServiceSession& aSession);

        /**
        * Method for getting ServiceSession which is related to this
        * Credential.
        *
        * @since Series60 4.0
        * @return   ServiceSession for this Credentiel or NULL
        *           if related ServiceSession is not set
        */
        virtual CSenServiceSession* Session();

        TBool UpdateMetadataEndpointL(CSenInternalWsPolicy* aInternalPolicy, TInt& aExist);
        
        
        TInt AddMetadataPolicyL(CSenElement* aElement);
        TInt UpdateMetadataPolicyL(CSenElement* aElement);
        TInt RemoveMetadataPolicyL(const TDesC8& aUri);
//        CSenElement* WsPolicy(const CSenWSDescription& aPattern);
        CSenElement* WsPolicyByUriL(const TDesC8& aUri);
        CSenElement* WsPolicyByName(const TDesC8& aName);
        TTime ValidUntilL(const TDesC8& aUri);
        
        
    private:

        /**
        * C++ default constructor.
        */
        CSenInternalWsPolicy();


        /**
        * By default Symbian 2nd phase constructor is private.
        *
        * @since Series60 4.0
        * @param aNsUri is the XML namespace as UTF-8 descriptor
        * @param aLocalName is the XML localname as UTF-8 descriptor
        * @param aQName is the XML qualifiedname as UTF-8 descriptor
        * @param aAttributes is the list of XML attributes
        */
        void ConstructL(const TDesC8& aNsUri,
                                 const TDesC8& aLocalName,
                                 const TDesC8& aQName,
                                 const RAttributeArray& aAttributes);

        /**
        * By default Symbian 2nd phase constructor is private.
        *
        * @since Series60 4.0
        * @param aNsUri is the XML namespace as UTF-8 descriptor
        * @param aLocalName is the XML localname as UTF-8 descriptor
        * @param aQName is the XML qualifiedname as UTF-8 descriptor
        * @param aAttributes is the list of XML attributes
        * @param aParent is the parent element, which may declare other
        *        namespaces.
        */
        void ConstructL(const TDesC8& aNsUri,
                                 const TDesC8&  aLocalName,
                                 const TDesC8& aQName,
                                 const RAttributeArray& aAttributes,
                                  CSenElement& aParent);

        /**
        * By default Symbian 2nd phase constructor is private.
        *
        * @since Series60 4.0
        * @param aPolicy is the source credential to be copied.
        */
        void ConstructL(CSenInternalWsPolicy& aPolicy);

        /**
        * By default Symbian 2nd phase constructor is private.
        *
        * BaseConstructL for setting XML localname for this Credential.
        * @since Series60 4.0
        * @param aLocalName XML localname for this Credential
        */
        void ConstructL(const TDesC8& aLocalName);

        // New functions

        /**
        * Setter for Credential ID
        *
        * @since Series60 4.0
        * @param aId is the unique ID for this Credential
        */
        void SetIdL(const TDesC8& aId);

        TInt PolicyCountL();	//codescannerwarnings
        TPtrC8 ExpiryTimeL(CSenElement* aElement); //codescannerwarnings

    private: // Data
        CSenWSPolicyIdentifier*   ipIdentifier;
//        CSenWSPolicyProperties*   ipProperties;
        CSenServiceSession*         ipSession;
               
    };

class CSenWSPolicyProperties : public CSenDomFragment
    {
    public:  // Constructors and destructor

        /**
        * Basic constructor.
        *
        * @since Series60 4.0
        */
        static CSenWSPolicyProperties* NewL();

        /**
        * Basic constructor.
        *
        * @since Series60 4.0
        */
        static CSenWSPolicyProperties* NewLC();

        /**
        * Basic constructor.
        *
        * @since Series60 4.0
        * @param aNsUri     is the XML namespace of this credential
        * @param aLocalName is the XML localname of this credential
        * @param aPrefix    is the prefix of this credential
        * @param aAttrs     are the XML attributes of this credential
        */
        static CSenWSPolicyProperties* NewL(const TDesC8& aNsUri,
                                                           const TDesC8& aLocalName,
                                                           const TDesC8& aQName,
                                                           const RAttributeArray& aAttributes);

        /**
        * Basic constructor.
        *
        * @since Series60 4.0
        * @param aNsUri     is the XML namespace of this credential
        * @param aLocalName is the XML localname of this credential
        * @param aPrefix    is the prefix of this credential
        * @param aAttrs     are the XML attributes of this credential
        */
        static CSenWSPolicyProperties* NewLC(const TDesC8& aNsUri,
                                                            const TDesC8& aLocalName,
                                                            const TDesC8& aQName,
                                                            const RAttributeArray& aAttributes);

        /**
        * Basic constructor.
        *
        * @since Series60 4.0
        * @param aNsUri     is the XML namespace of this credential
        * @param aLocalName is the XML localname of this credential
        * @param aPrefix    is the prefix of this credential
        * @param aAttrs     are the XML attributes of this credential
        * @param aParent    is the parent XML element of this credential
        */
        static CSenWSPolicyProperties* NewL(const TDesC8& aNsUri,
                                                           const TDesC8& aLocalName,
                                                           const TDesC8& aQName,
                                                           const RAttributeArray& aAttributes,
                                                           CSenElement& aParent);

        /**
        * Basic constructor.
        *
        * @since Series60 4.0
        * @param aNsUri     is the XML namespace of this credential
        * @param aLocalName is the XML localname of this credential
        * @param aPrefix    is the prefix of this credential
        * @param aAttrs     are the XML attributes of this credential
        * @param aParent    is the parent XML element of this credential
        */
        static CSenWSPolicyProperties* NewLC(const TDesC8& aNsUri,
                                                            const TDesC8& aLocalName,
                                                            const TDesC8& aQName,
                                                            const RAttributeArray& aAttributes,
                                                            CSenElement& aParent);

        /**
        * Destructor.
        */
        virtual ~CSenWSPolicyProperties();

        /**
        * Sets new property.
        *
        * If property with given name does not exist,
        * new property with given value will be added. Otherwise the value of
        * existing property will be updated.
        *
        * @since Series60 4.0
        * @param aName         Name of the property, which can be later used
        *                      to refer the given value.
        * @param aValue        Is the value of this property.
        * @return              KErrNone if no error, or some of the system wide
        *                      error codes.
        */
        TInt SetPropertyL(const TDesC8& aName, const TDesC8& aValue);

        /**
        * Gets the value of the property behind certain name.
        *
        * @since Series60 4.0
        * @param aName    The name identifying this property.
        * @param aValue   A TPtrC8 reference to be filled in with the value of
        *                 the property.
        * @return         KErrNone if no error, or some of the system wide
        *                 error codes.
        */
        TInt PropertyL(const TDesC8& aName, TPtrC8& aValue);


        
     protected:

        /**
        * C++ default constructor.
        */
        CSenWSPolicyProperties();

   };

class CSenWSPolicyIdentifier : public CSenWSPolicyProperties
    {
    public:  // Constructors and destructor

        /**
        * Basic constructor.
        *
        * @since Series60 4.0
        */
        static CSenWSPolicyIdentifier* NewL();

        /**
        * Basic constructor.
        *
        * @since Series60 4.0
        */
        static CSenWSPolicyIdentifier* NewLC();

        /**
        * Basic constructor.
        *
        * @since Series60 4.0
        * @param aNsUri     is the XML namespace of this credential
        * @param aLocalName is the XML localname of this credential
        * @param aPrefix    is the prefix of this credential
        * @param aAttrs     are the XML attributes of this credential
        */
        static CSenWSPolicyIdentifier* NewL(const TDesC8& aNsUri,
                                                       const TDesC8& aLocalName,
                                                       const TDesC8& aQName,
                                                          const RAttributeArray& aAttributes);

        /**
        * Basic constructor.
        *
        * @since Series60 4.0
        * @param aNsUri     is the XML namespace of this credential
        * @param aLocalName is the XML localname of this credential
        * @param aPrefix    is the prefix of this credential
        * @param aAttrs     are the XML attributes of this credential
        */
        static CSenWSPolicyIdentifier* NewLC(const TDesC8& aNsUri,
                                                        const TDesC8& aLocalName,
                                                        const TDesC8& aQName,
                                                        const RAttributeArray& aAttributes);

        /**
        * Basic constructor.
        *
        * @since Series60 4.0
        * @param aNsUri     is the XML namespace of this credential
        * @param aLocalName is the XML localname of this credential
        * @param aPrefix    is the prefix of this credential
        * @param aAttrs     are the XML attributes of this credential
        * @param aParent    is the parent XML element of this credential
        */
        static CSenWSPolicyIdentifier* NewL(const TDesC8& aNsUri,
                                                       const TDesC8& aLocalName,
                                                       const TDesC8& aQName,
                                                       const RAttributeArray& aAttributes,
                                                       CSenElement& aParent);

        /**
        * Basic constructor.
        *
        * @since Series60 4.0
        * @param aNsUri     is the XML namespace of this credential
        * @param aLocalName is the XML localname of this credential
        * @param aPrefix    is the prefix of this credential
        * @param aAttrs     are the XML attributes of this credential
        * @param aParent    is the parent XML element of this credential
        */
        static CSenWSPolicyIdentifier* NewLC(const TDesC8& aNsUri,
                                                        const TDesC8& aLocalName,
                                                        const TDesC8& aQName,
                                                        const RAttributeArray& aAttributes,
                                                        CSenElement& aParent);

        /**
        * Destructor.
        */
        virtual ~CSenWSPolicyIdentifier();

        void SetIdL(TInt aId);

        TInt IdL();
        
        TPtrC8 ContractL();  //codescannerwarnings
        TPtrC8 EndpointL();  //codescannerwarnings
        TPtrC8 ProviderIdL(); //codescannerwarnings
        TPtrC8 MetadataEndpointL(); //codescannerwarnings
        CSenElement* MetadataEndpointElementL(); //codescannerwarnings
        
        TInt AddMetadataEndpointL(CSenElement* aElement);
        TInt UpdateMetadataEndpointL(CSenWSPolicyIdentifier& aIdentifier);
        
        static TPtrC8 ElementAttribValueL(CSenElement* aElement, const TDesC8& aName); 
        static TBool HasAttributeL(CSenElement* aElement, const TDesC8& aName);
        
        
     private:

        /**
        * C++ default constructor.
        */
        CSenWSPolicyIdentifier();
   };

typedef struct
    {
    CSenInternalWsPolicy* ipPolicy;
    TInt                    iCounter;
    } TSenWsPolicyData;

class RSenWSPolicyPtr
	{
    public:
	    void OpenL(CSenInternalWsPolicy* apPolicy);
	    CSenInternalWsPolicy* Policy();
	    void SetPolicy(CSenInternalWsPolicy* apPolicy);
	    RSenWSPolicyPtr Clone();
	    void Close();
	    void CloseAndDestroyPolicy();

        /**
        * C++ default constructor.
        */
        RSenWSPolicyPtr();

    private: // Data
        TSenWsPolicyData* ipPolicyData;
	};

#endif // SEN_INTERNAL_WS_POLICY_H

// End of File


