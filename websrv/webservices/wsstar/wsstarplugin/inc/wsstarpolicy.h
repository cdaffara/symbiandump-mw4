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
* Description:    Header declaration
*
*/









 

#ifndef WS_STAR_POLICY_H
#define WS_STAR_POLICY_H

// INCLUDES
#include <e32std.h>
#include "MSenServiceDescription.h"
#include "wsstarservicesession.h"
#include "MSenServicePolicy.h"
// CONSTANTS
namespace WSPolicy
    {
    // Common
    
    // Enveloper layer
    _LIT8( KSoap12, "SOAP12");
    
    // Addressing layer
    _LIT8( KAddressing, "Addressing");

    // Trust layer
    _LIT8( KClientEntropy , "ClientEntropy");
    _LIT8( KTokenType,          "TokenType");
    _LIT8( KPassportExtensions , "PassportExtensions");
    // Way of authentication with STS
    _LIT8( KUsernameTokenOverTLS , "UsernameTokenOverTLS");
    _LIT8( KSAMLTokenTypeValue, "http://docs.oasis-open.org/wss/oasis-wss-saml-token-profile-1.1#SAMLV1.1");
    
    // Serializer layer
    _LIT8( KMTOM , "MTOM");
    _LIT8( KCluster, "Cluster");
    
    _LIT8(KIssuedToken, "IssuedToken");
    _LIT8( KRPSCluster, "RPSCluster");
    _LIT8(KRequestSecurityTokenTemplate, "RequestSecurityTokenTemplate");
    _LIT8(KAddress, "Address");
    
    _LIT8(KSignedParts, "SignedParts");
    _LIT8(KName, "Name");
    _LIT8(KDictionaries, "Dictionaries");
    _LIT8(KDictionary, "Dictionary");
    _LIT8(KMobileService, "MobileService");
    _LIT8(KMwsType, "mws:Type");
    _LIT8(KMwsId, "mws:Id");
    _LIT8( KAppInfo,"AppInfo" );
    _LIT8( KClientPolicy,"ClientPolicy" );
    }


/**
 * Class:       
 */
class CWSStarPolicy : public CBase
    {
    public:
        /*
        static CWSStarPolicy* NewL();
        static CWSStarPolicy* NewLC();
        ~CWSStarPolicy();
        void SetPolicy(CSenElement* aPolicy);
        */

        /**
        *   static TBool CheckL(const TDesC8& aKey, MSenServiceDescription* aSD);
        *   
        *   This method can be used to find the Client policy elements.
        *   it simply try to find an element in the policy and if the element is found
        *   then return the ETrue else EFalse.
        *   @param: aKey is the name of property/element 
        *   @param: aSD, The ServiceDescription from which this element should be checked

        *   @return TBool
        */
        static TBool CheckPolicyValueL(const TDesC8& aKey, MSenServiceDescription* aSD);

            
        /**
        *   static HBufC8* GetValueL(const TDesC8& aKey, MSenServiceDescription* aSD);
        *   
        *   This method can be used to find the Client policy elements and their properties.
        *   It simply try to find an element in the policy and if the element is found
        *   then return the Element's contents.
        *   
        *   @param: aKey is the name of property/element 
        *   @param: aSD, The ServiceDescription from which this element should be checked
        *   
        *   @return HBufC*: Ownership is transferred
        */


        static HBufC8* GetPolicyValueL(const TDesC8& aKey, MSenServiceDescription* aSD);
        /**
        *   static HBufC8* GetValueL(const TDesC8& aKey, const TDesC8& aSubKey, MSenHandlerContext& aCtx);
        *   
        *   This method can be used to find the "Service Policy" assertions and their properties.
        *   It simply try to find an element in the Policy and if the element is found
        *   then return the Element's contents. If element has no contents then return the whole Element
        *   excluding the element name 
        *   
        *   @param: aKey is the name of property/element 
        *   @param: aSubKey is the name of property/element present in "aKey" Element
        *   @param: aCtx, The Context of PolicyHanlder which owns the Policy
        *   
        *   @return HBufC*: Ownership is transferred
        */    
        static HBufC8* GetPolicyValueL(const TDesC8& aKey, const TDesC8& aSubKey, MSenHandlerContext& aCtx, MSenServiceDescription* aSD);
        static HBufC8* GetPolicyValueL(const TDesC8& aTokenType, const TDesC8& aKey, const TDesC8& aSubKey, MSenHandlerContext& aCtx, MSenServiceDescription* aSD, RPointerArray<CSenElement>& aElements);
        static HBufC8* GetPolicyValueL(const TDesC8& aTokenType, const TDesC8& aKey, const TDesC8& aSubKey, MSenHandlerContext& aCtx, MSenServiceDescription* aSD);
        static HBufC8* GetPolicyValueL(const TDesC8& aKey, const TDesC8& aSubKey, MSenHandlerContext& aCtx, MSenServiceDescription* aSD, RPointerArray<CSenElement>& aElements);
        static HBufC8* GetPolicyValueL(const TDesC8& aKey, const TDesC8& aSubKey, CWSStarPlugin& aWSStarPlugin, MSenServiceDescription* aSD);
        /**
        *   static void GetValueL(const TDesC8& aKey, const TDesC8& aSubKey, MSenHandlerContext& aCtx, RArray<TPtrC8>& aList);
        *   
        *   This method can be used to find the "Service Policy" assertions and their properties.
        *   It simply try to find an element in the Policy and if the element is found
        *   then return the Element's contents in form of list
        *   
        *   @param: aKey is the name of property/element 
        *   @param: aSubKey is the name of property/element present in "aKey" Element
        *   @param: aCtx, The Context of PolicyHanlder which owns the Policy
        *   @param: aList, The list of properties for which the Elements will be searched
        *   one example of this method is to get the list of SignedElements/EncryptedElements  
        */    


        static void GetPolicyValueL(const TDesC8& aKey, const TDesC8& aSubKey, MSenHandlerContext& aCtx, MSenServiceDescription* aSD, RPointerArray<HBufC8>& aList);
        
        
        /**
        *   static void GetValueL(const TDesC8& aKey, const TDesC8& aSubKey, MSenHandlerContext& aCtx, RArray<TPtrC8>& aList);
        *   
        *   This method can be used to find the "Service Policy" assertions and their properties.
        *   It simply try to find an element in the Policy and if the element is found
        *   then return the Element's contents in form of list
        *   
        *   @param: aKey is the name of property/element 
        *   @param: aSubKey is the name of property/element present in "aKey" Element
        *   @param: aPolicyHandler, PolicyHanlder which owns the Policy
        *   @param: aList, The list of properties for which the Elements will be searched
        *
        *   one example of this method is to get the list of SignedElements/EncryptedElements  
        *   Attetntion: in case (signed/encrypted elements) array is list of pairs [localname, namespace]
        */    
        
        static void GetPolicyValueL(const TDesC8& aKey, const TDesC8& aSubKey, CWSStarPolicyHandler* aPolicyHandler, MSenServiceDescription* aSD, RPointerArray<HBufC8>& aList);
        
        
        /**
        *   static TPtrC8 GetAttributeValueL(const TDesC8& aAttribute, CSenElement* aElement);
        *   
        *   This method can be used to find the attribute value of an Attibute from any Elements
        *   It simply try to find the attribute in an element and then return the value of the Attirbute
        *   
        *   @param: aAttribute is the name of property
        *   @param: aElement is the element where the attribute will eb searched
        *   
        *   @return TPtrC8: return pointer to the Value of attribute
        *                    KNullDesC8() if no value is found   
        */    


        static TPtrC8 GetAttributeValueL(const TDesC8& aAttribute, CSenElement* aElement);
        /**
        *   static TPtrC8 GetNSPrefixL(const TDesC8& aNamespaceUri, CSenElement* aElement);
        *   
        *   This method can be used to find the namespace prefix for a namespace URi from the element
        *   
        *   @param: aNamespaceUri is the nameSpace URI
        *   @param: aElement is the element where the NsURI will be searched
        *   
        *   @return TPtrC8: return pointer to the Prefix of namespace URI
        *                    KNullDesC8() if no value is found   
        */    

        static TPtrC8 GetNSPrefixL(const TDesC8& aNamespaceUri, CSenElement* aElement);
        static CSenElement* SearchForElementL(CSenElement* aPolicy, TPtrC8 aKey);	//codescannerwarnings
        static TInt SearchForElementsL(CSenElement* aPolicy, TPtrC8 aKey, RPointerArray<CSenElement>& aElements);
        static CSenElement* GetElementL(const TDesC8& aKey, const TDesC8& aSubKey, MSenHandlerContext& aCtx, MSenServiceDescription* aSD);
        static void GetPolicyValueL(const TDesC8& aKey, const TDesC8& aSubKey,MSenServiceDescription* aSD,RPointerArray<CSenElement>& aElements);

        
    protected:
        static TInt ChildCountL(CSenElement* aTerm);	//codescannerwarnings
        
        
        static CSenElement* GetPolicyL(MSenHandlerContext& aCtx, MSenServiceDescription* aSD);	//codescannerwarnings

    private:
        /**
        * Hide default C++ constructor.
        */
        CWSStarPolicy() { }

        /**
        * Prohibit copy constructor if not deriving from CBase.
        */
        CWSStarPolicy( const CWSStarPolicy& );

        /**
        * Prohibit assignment operator if not deriving from CBase.
        */
        CWSStarPolicy& operator=( const CWSStarPolicy& );
        
        static void GetValueL(const TDesC8& aKey, const TDesC8& aSubKey, CSenElement* aPolicy, RPointerArray<HBufC8>& aList);

    };
    
    
struct SoapHeader
    {
	TPtrC8 iName;
	TPtrC8 iNsUri;
	TPtrC8 iNsPrefix;
    };

class CEncryptedSignedPartsI : public CBase
    {
    public:
        static CEncryptedSignedPartsI* NewL(CSenElement* aAssertion);
        static CEncryptedSignedPartsI* NewLC(CSenElement*  aAssertion);
        virtual ~CEncryptedSignedPartsI();

        TBool IsEqualEncryptedSignedParts(CEncryptedSignedPartsI* aAssertion);
        
        void  ElementsL(RPointerArray<HBufC8>& aElements);	//codescannerwarnings
        TBool Header();
        TBool Body();
    private:
        void ConstructL(CSenElement*  aAssertion);
        CEncryptedSignedPartsI(); 

    private: // Data
        TBool iHeader;
        TBool iBody;
        RArray<SoapHeader> iHeaders;  
        
    };
#endif // WS_STAR_POLICY_H

// END OF FILE


