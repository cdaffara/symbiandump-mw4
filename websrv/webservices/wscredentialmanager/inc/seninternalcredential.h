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
* Description:           This internal class implements (web) service credential functionality
*
*/











#ifndef SEN_INTERNAL_CREDENTIAL_H
#define SEN_INTERNAL_CREDENTIAL_H

//  INCLUDES
#include <e32base.h>
#include <s32strm.h>
#include <SenCredential2.h>

// FORWARD DECLARATIONS
class CSenCredentialProperties;
class CSenCredentialIdentifier;
class CSenWSDescription;


// CLASS DECLARATION

/**
* Class implements (web) service credential functionality
*  @lib SenServDesc.lib
*  @since Series60 3.0
*/
class CSenInternalCredential : public CSenCredential2
    {
    public:  // Constructors and destructor

        /**
        * Basic constructor.
        *
        * @since Series60 3.2.3
        */
        IMPORT_C static CSenInternalCredential* NewL();

        /**
        * Basic constructor.
        *
        * @since Series60 3.2.3
        */
        IMPORT_C static CSenInternalCredential* NewLC();

        /**
        * Basic constructor.
        *
        * @since Series60 3.2.3
        * @param aNsUri     is the XML namespace of this credential
        * @param aLocalName is the XML localname of this credential
        * @param aPrefix    is the prefix of this credential
        * @param aAttrs     are the XML attributes of this credential
        */
        IMPORT_C static CSenInternalCredential* NewL(const TDesC8& aNsUri,
                                            const TDesC8& aLocalName,
                                            const TDesC8& aQName,
                                            const RAttributeArray& aAttributes);

        /**
        * Basic constructor.
        *
        * @since Series60 3.2.3
        * @param aNsUri     is the XML namespace of this credential
        * @param aLocalName is the XML localname of this credential
        * @param aPrefix    is the prefix of this credential
        * @param aAttrs     are the XML attributes of this credential
        */
        IMPORT_C static CSenInternalCredential* NewLC(const TDesC8& aNsUri,
                                             const TDesC8& aLocalName,
                                             const TDesC8& aQName,
                                             const RAttributeArray& aAttributes);

        /**
        * Basic constructor.
        *
        * @since Series60 3.2.3
        * @param aNsUri     is the XML namespace of this credential
        * @param aLocalName is the XML localname of this credential
        * @param aPrefix    is the prefix of this credential
        * @param aAttrs     are the XML attributes of this credential
        * @param aParent    is the parent XML element of this credential
        */
        IMPORT_C static CSenInternalCredential* NewL(const TDesC8& aNsUri,
                                            const TDesC8& aLocalName,
                                            const TDesC8& aQName,
                                            const RAttributeArray& aAttributes,
                                            TXmlEngElement& aParent);

        /**
        * Basic constructor.
        *
        * @since Series60 3.2.3
        * @param aNsUri     is the XML namespace of this credential
        * @param aLocalName is the XML localname of this credential
        * @param aPrefix    is the prefix of this credential
        * @param aAttrs     are the XML attributes of this credential
        * @param aParent    is the parent XML element of this credential
        */
        IMPORT_C static CSenInternalCredential* NewLC(const TDesC8& aNsUri,
                                             const TDesC8& aLocalName,
                                             const TDesC8& aQName,
                                             const RAttributeArray& aAttributes,
                                             TXmlEngElement& aParent);

         /**
        * Basic constructor.
        *
        * @since Series60 3.2.3
        * @param aNsUri         is the XML namespace of this fragment
        * @param aLocalName     is the XML localname of this fragment
        * @param aPrefix        is the prefix of this fragment
        * @param aAttrs         are the XML attributes of this fragment
        * @param aParent        is the parent XML element of this fragment
        * @param aOwnerDocument is the document which will be the owner of
        *                       the elements of this fragment
        */
        IMPORT_C static CSenInternalCredential* NewL(const TDesC8& aNsUri,
                                            const TDesC8& aLocalName,
                                            const TDesC8& aQName,
                                            const RAttributeArray& aAttrs,
                                            TXmlEngElement& aParent,
                                            RSenDocument& aOwnerDocument);

        /**
        * Basic constructor.
        *
        * @since Series60 3.2.3
        * @param aNsUri         is the XML namespace of this fragment
        * @param aLocalName     is the XML localname of this fragment
        * @param aPrefix        is the prefix of this fragment
        * @param aAttrs         are the XML attributes of this fragment
        * @param aParent        is the parent XML element of this fragment
        * @param aOwnerDocument is the document which will be the owner of
        *                       the elements of this fragment
        */
        IMPORT_C static CSenInternalCredential* NewLC(const TDesC8& aNsUri,
                                             const TDesC8& aLocalName,
                                             const TDesC8& aQName,
                                             const RAttributeArray& aAttrs,
                                             TXmlEngElement& aParent,
                                             RSenDocument& aOwnerDocument);

        /**
        * Constructor which copies itself from another credential.
        *
        * @since Series60 3.2.3
        * @param    aCredential     credential to copy from.
        * @return new CSenCredential instance pointer
        */
        IMPORT_C static CSenInternalCredential* NewL(const CSenInternalCredential& aCredential);

        /**
        * Constructor which copies itself from another credential.
        *
        * @since Series60 3.2.3
        * @param    aCredential     credential to copy from.
        * @return new CSenCredential instance pointer, which is left on
        *         cleanup stack.
        */
        IMPORT_C static CSenInternalCredential* NewLC(const CSenInternalCredential& aCredential);

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CSenInternalCredential();

        // New functions

        /**
        * Method for checking if this credential is applicable for
        * given pattern.
        *
        * @since Series60 3.2.3
        * @param    aPattern    pattern to check.
        * @return   ETrue       if Credential is applicable
        *           EFalse      if Credential is not applicable
        */
        IMPORT_C virtual TBool IsApplicableL(const CSenWSDescription& aPattern);	//codescannerwarings

        /**
        * Method for setting CredentialIdentifier.
        *
        * @since Series60 3.2.3
        * @param    aIdentifier identifier to set.
        */
        IMPORT_C virtual void SetIdentifier(CSenCredentialIdentifier* aIdentifier);

        /**
        * Method for getting CredentialIdentifier.
        *
        * @since Series60 3.2.3
        * @return   Identifier for this Credentiel
        */
        IMPORT_C virtual CSenCredentialIdentifier& IdentifierL();		//codescannerwarings

        /**
        * Method for setting CredentialProperties.
        *
        * @since Series60 3.2.3
        * @param    aProperties properties to set.
        */
        IMPORT_C virtual void SetProperties(CSenCredentialProperties* aProperties);

        /**
        * Method for getting CredentialProperties.
        *
        * @since Series60 3.2.3
        * @return   Properties for this Credentiel
        */
        IMPORT_C virtual CSenCredentialProperties& PropertiesL();	//codescannerwarings
        
        /**
        * Method for checking if Credential has  properties.
        *
        * @since Series60 3.2.3
        * @return   ETrue, if Credential has properties or
        *           EFalse, if Credential does not have properties
        */
        IMPORT_C TBool CSenInternalCredential::HasProperties();

        /**
        * Method for setting IdentityProviderId for this Credential.
        * Setting IdentityProviderId does build relation between this
        * Credential and IdentityProvider for which IdentityProviderId
        * refers to.
        *
        * @param   aIdentityProviderId    IdentityProviderId for IdentityProvider
        *                                 which is related to this Credentiel.
        */
        IMPORT_C virtual void SetIdentityProviderIdL(TDesC8& aIdentityProviderId);

        /**
        * Method for setting Authentiaction information for this Credential.
        * Setting Authentiaction does build relation between this
        * Credential and IdentityProvider for which Authentiaction
        * refers to.
        *
        * @param   aUserId    UserId for IdentityProvider
        * @param   aPassword  Password for IdentityProvider
        *                                 which is related to this Credentiel.
        */
        IMPORT_C void SetAuthInfo(TDesC8& aUserId, TDesC8& aPassword) ;

        /**
        * Method for getting IdentityProviderId of IdentityProvider which
        * is related to this Credential.
        *
        * @since Series60 3.2.3
        * @param aIdTo              a reference to TPtrC8 for which
        *                           IdentityProviderId will be placed.
        * @return   KErrNotFound    if IdentityProviderId is not stored
        *                           into this Credential
        *           KErrNone        If IdentityProviderId was found
        *                           from this Credential.
        */
        IMPORT_C virtual TInt IdentityProviderIdL(TPtrC8& aIdTo);

        /**
        * Method for setting ServiceSession for this Credential.
        * Setting ServiceSession does build relation between this
        * Credential and given ServiceSession.
        *
        * @since Series60 3.2.3
        * @param   aSession             ServiceSession for this Credentiel
        */
        IMPORT_C virtual void SetSession(CSenServiceSession& aSession);

        /**
        * Method for getting ServiceSession which is related to this
        * Credential.
        *
        * @since Series60 3.2.3
        * @return   ServiceSession for this Credentiel or NULL
        *           if related ServiceSession is not set
        */
        IMPORT_C virtual CSenServiceSession* Session();

    private:

        /**
        * C++ default constructor.
        */
        IMPORT_C CSenInternalCredential();


        /**
        * By default Symbian 2nd phase constructor is private.
        *
        * @since Series60 3.2.3
        * @param aNsUri is the XML namespace as UTF-8 descriptor
        * @param aLocalName is the XML localname as UTF-8 descriptor
        * @param aQName is the XML qualifiedname as UTF-8 descriptor
        * @param aAttributes is the list of XML attributes
        */
        IMPORT_C void ConstructL(const TDesC8& aNsUri,
                                 const TDesC8& aLocalName,
                                 const TDesC8& aQName,
                                 const RAttributeArray& aAttributes);

        /**
        * By default Symbian 2nd phase constructor is private.
        *
        * @since Series60 3.2.3
        * @param aNsUri is the XML namespace as UTF-8 descriptor
        * @param aLocalName is the XML localname as UTF-8 descriptor
        * @param aQName is the XML qualifiedname as UTF-8 descriptor
        * @param aAttributes is the list of XML attributes
        * @param aParent is the parent element, which may declare other
        *        namespaces.
        */
        IMPORT_C void ConstructL(const TDesC8& aNsUri,
                                 const TDesC8&  aLocalName,
                                 const TDesC8& aQName,
                                 const RAttributeArray& aAttributes,
                                 TXmlEngElement& aParent);

        /**
        * By default Symbian 2nd phase constructor is private.
        *
        * @since Series60 3.2.3
        * @param aNsUri is the XML namespace as UTF-8 descriptor
        * @param aLocalName is the XML localname as UTF-8 descriptor
        * @param aQName is the XML qualifiedname as UTF-8 descriptor
        * @param aAttributes is the list of XML attributes
        * @param aParent is the parent element, which may declare other
        *        namespaces.
        * @param aOwnerDocument is the document which will be the owner of
        *                       the elements of this credential
        */
        IMPORT_C void ConstructL(const TDesC8& aNsUri,
                                 const TDesC8&  aLocalName,
                                 const TDesC8& aQName,
                                 const RAttributeArray& aAttributes,
                                 TXmlEngElement& aParent,
                                 RSenDocument& aOwnerDocument);

        /**
        * By default Symbian 2nd phase constructor is private.
        *
        * @since Series60 3.2.3
        * @param aCredential is the source credential to be copied.
        */
        IMPORT_C void ConstructL(CSenInternalCredential& aCredential);

        /**
        * By default Symbian 2nd phase constructor is private.
        *
        * BaseConstructL for setting XML localname for this Credential.
        * @since Series60 3.2.3
        * @param aLocalName XML localname for this Credential
        */
        IMPORT_C void ConstructL(const TDesC8& aLocalName);

        // New functions

        /**
        * Setter for Credential ID
        *
        * @since Series60 3.2.3
        * @param aId is the unique ID for this Credential
        */
        void SetIdL(const TDesC8& aId);

    private: // Data
        CSenCredentialIdentifier*   ipIdentifier;
        CSenCredentialProperties*   ipProperties;
        CSenIdentityProvider*       ipIdentityProvider;
        CSenServiceSession*         ipSession;
    };

class CSenCredentialProperties : public CSenDomFragmentBase
    {
    public:  // Constructors and destructor

        /**
        * Basic constructor.
        *
        * @since Series60 3.2.3
        */
        IMPORT_C static CSenCredentialProperties* NewL();

        /**
        * Basic constructor.
        *
        * @since Series60 3.2.3
        */
        IMPORT_C static CSenCredentialProperties* NewLC();

        /**
        * Basic constructor.
        *
        * @since Series60 3.2.3
        * @param aNsUri     is the XML namespace of this credential
        * @param aLocalName is the XML localname of this credential
        * @param aPrefix    is the prefix of this credential
        * @param aAttrs     are the XML attributes of this credential
        */
        IMPORT_C static CSenCredentialProperties* NewL(const TDesC8& aNsUri,
                                                           const TDesC8& aLocalName,
                                                           const TDesC8& aQName,
                                                           const RAttributeArray& aAttributes);

        /**
        * Basic constructor.
        *
        * @since Series60 3.2.3
        * @param aNsUri     is the XML namespace of this credential
        * @param aLocalName is the XML localname of this credential
        * @param aPrefix    is the prefix of this credential
        * @param aAttrs     are the XML attributes of this credential
        */
        IMPORT_C static CSenCredentialProperties* NewLC(const TDesC8& aNsUri,
                                                            const TDesC8& aLocalName,
                                                            const TDesC8& aQName,
                                                            const RAttributeArray& aAttributes);

        /**
        * Basic constructor.
        *
        * @since Series60 3.2.3
        * @param aNsUri     is the XML namespace of this credential
        * @param aLocalName is the XML localname of this credential
        * @param aPrefix    is the prefix of this credential
        * @param aAttrs     are the XML attributes of this credential
        * @param aParent    is the parent XML element of this credential
        */
        IMPORT_C static CSenCredentialProperties* NewL(const TDesC8& aNsUri,
                                                           const TDesC8& aLocalName,
                                                           const TDesC8& aQName,
                                                           const RAttributeArray& aAttributes,
                                                           TXmlEngElement& aParent);

        /**
        * Basic constructor.
        *
        * @since Series60 3.2.3
        * @param aNsUri     is the XML namespace of this credential
        * @param aLocalName is the XML localname of this credential
        * @param aPrefix    is the prefix of this credential
        * @param aAttrs     are the XML attributes of this credential
        * @param aParent    is the parent XML element of this credential
        */
        IMPORT_C static CSenCredentialProperties* NewLC(const TDesC8& aNsUri,
                                                            const TDesC8& aLocalName,
                                                            const TDesC8& aQName,
                                                            const RAttributeArray& aAttributes,
                                                            TXmlEngElement& aParent);

         /**
        * Basic constructor.
        *
        * @since Series60 3.2.3
        * @param aNsUri         is the XML namespace of this fragment
        * @param aLocalName     is the XML localname of this fragment
        * @param aPrefix        is the prefix of this fragment
        * @param aAttrs         are the XML attributes of this fragment
        * @param aParent        is the parent XML element of this fragment
        * @param aOwnerDocument is the document which will be the owner of
        *                       the elements of this fragment
        */
        IMPORT_C static CSenCredentialProperties* NewL(const TDesC8& aNsUri,
                                                           const TDesC8& aLocalName,
                                                           const TDesC8& aQName,
                                                           const RAttributeArray& aAttrs,
                                                           TXmlEngElement& aParent,
                                                           RSenDocument& aOwnerDocument);

        /**
        * Basic constructor.
        *
        * @since Series60 3.2.3
        * @param aNsUri         is the XML namespace of this fragment
        * @param aLocalName     is the XML localname of this fragment
        * @param aPrefix        is the prefix of this fragment
        * @param aAttrs         are the XML attributes of this fragment
        * @param aParent        is the parent XML element of this fragment
        * @param aOwnerDocument is the document which will be the owner of
        *                       the elements of this fragment
        */
        IMPORT_C static CSenCredentialProperties* NewLC(const TDesC8& aNsUri,
                                                            const TDesC8& aLocalName,
                                                            const TDesC8& aQName,
                                                            const RAttributeArray& aAttrs,
                                                            TXmlEngElement& aParent,
                                                            RSenDocument& aOwnerDocument);

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CSenCredentialProperties();

        /**
        * Sets new property.
        *
        * If property with given name does not exist,
        * new property with given value will be added. Otherwise the value of
        * existing property will be updated.
        *
        * @since Series60 3.2.3
        * @param aName         Name of the property, which can be later used
        *                      to refer the given value.
        * @param aValue        Is the value of this property.
        * @return              KErrNone if no error, or some of the system wide
        *                      error codes.
        */
        IMPORT_C virtual TInt SetPropertyL(const TDesC8& aName, const TDesC8& aValue);

        /**
        * Gets the value of the property behind certain name.
        *
        * @since Series60 3.2.3
        * @param aName    The name identifying this property.
        * @param aValue   A TPtrC8 reference to be filled in with the value of
        *                 the property.
        * @return         KErrNone if no error, or some of the system wide
        *                 error codes.
        */
        IMPORT_C virtual TInt PropertyL(const TDesC8& aName, TPtrC8& aValue);

        /**
        * Method for checking if CredentialIdentifier has a certain facet.
        *
        * @since Series60 3.2.3
        * @param aURI           the facet to get, typically some URI.
        * @param aValueTo       a ref-to-pointer into which the value will
        *                       be allocated, which ownership is transferred
        *                       to the caller.
        * @return               KErrNone or other system-wide Symbian error
        *                       codes.
        */
        IMPORT_C virtual TInt FacetValueL(TDesC8& aURI, HBufC8*& aValueTo);

        /**
        * Method for adding a facet. Will NOT override an already
        * existing facet with a same name.
        *
        * @since Series60 3.2.3
        * @param        aFacet is the one to be added. A copy of the
        *               the original facet instance will be added.
        * @return       KErrNone or other system-wide Symbian error codes.
        *               KErrAlreadyExists if a facet with same name exists
        */
        IMPORT_C virtual TInt AddFacetL(const CSenFacet& aFacet);

        /**
        * Method for setting a facet. Adds a new facet, if non-existent.
        * But, if a facet with same name exists, it will be overridden.
        *
        * @since Series60 3.2.3
        * @param        aFacet to be set. A copy of the original will be
        *               created.
        * @return       KErrNotFound if facet can not be found
        *               KErrNone     if facet was found
        *               KErrNoMemory if no memory could be allocated for the value
        *               Other system-wide Symbian error codes.
        */
        IMPORT_C virtual TInt SetFacetL(const CSenFacet& aFacet);
        /**
        * Removes a characteristic (Facet) from the description
        *
        * @since Series60 3.2.3
        * @param        aURI of the facet to be removed.
        * @return       KErrNone     if a Facet was successfully removed
        *               KErrNotFound if a Facet could not be found
        *               Other system-wide Symbian error codes.
        */
        IMPORT_C virtual TInt RemoveFacetL(const TDesC8& aURI);

        IMPORT_C virtual void SetValidUntilL(TTime aValidUntil);
        
        IMPORT_C virtual TTime ValidUntilL();
        
     protected:

        /**
        * C++ default constructor.
        */
        CSenCredentialProperties();

   };

class CSenCredentialIdentifier : public CSenCredentialProperties
    {
    public:  // Constructors and destructor

        /**
        * Basic constructor.
        *
        * @since Series60 3.2.3
        */
        IMPORT_C static CSenCredentialIdentifier* NewL();

        /**
        * Basic constructor.
        *
        * @since Series60 3.2.3
        */
        IMPORT_C static CSenCredentialIdentifier* NewLC();

        /**
        * Basic constructor.
        *
        * @since Series60 3.2.3
        * @param aNsUri     is the XML namespace of this credential
        * @param aLocalName is the XML localname of this credential
        * @param aPrefix    is the prefix of this credential
        * @param aAttrs     are the XML attributes of this credential
        */
        IMPORT_C static CSenCredentialIdentifier* NewL(const TDesC8& aNsUri,
                                                       const TDesC8& aLocalName,
                                                       const TDesC8& aQName,
                                                          const RAttributeArray& aAttributes);

        /**
        * Basic constructor.
        *
        * @since Series60 3.2.3
        * @param aNsUri     is the XML namespace of this credential
        * @param aLocalName is the XML localname of this credential
        * @param aPrefix    is the prefix of this credential
        * @param aAttrs     are the XML attributes of this credential
        */
        IMPORT_C static CSenCredentialIdentifier* NewLC(const TDesC8& aNsUri,
                                                        const TDesC8& aLocalName,
                                                        const TDesC8& aQName,
                                                        const RAttributeArray& aAttributes);

        /**
        * Basic constructor.
        *
        * @since Series60 3.2.3
        * @param aNsUri     is the XML namespace of this credential
        * @param aLocalName is the XML localname of this credential
        * @param aPrefix    is the prefix of this credential
        * @param aAttrs     are the XML attributes of this credential
        * @param aParent    is the parent XML element of this credential
        */
        IMPORT_C static CSenCredentialIdentifier* NewL(const TDesC8& aNsUri,
                                                       const TDesC8& aLocalName,
                                                       const TDesC8& aQName,
                                                       const RAttributeArray& aAttributes,
                                                       TXmlEngElement& aParent);

        /**
        * Basic constructor.
        *
        * @since Series60 3.2.3
        * @param aNsUri     is the XML namespace of this credential
        * @param aLocalName is the XML localname of this credential
        * @param aPrefix    is the prefix of this credential
        * @param aAttrs     are the XML attributes of this credential
        * @param aParent    is the parent XML element of this credential
        */
        IMPORT_C static CSenCredentialIdentifier* NewLC(const TDesC8& aNsUri,
                                                        const TDesC8& aLocalName,
                                                        const TDesC8& aQName,
                                                        const RAttributeArray& aAttributes,
                                                        TXmlEngElement& aParent);

         /**
        * Basic constructor.
        *
        * @since Series60 3.2.3
        * @param aNsUri         is the XML namespace of this fragment
        * @param aLocalName     is the XML localname of this fragment
        * @param aPrefix        is the prefix of this fragment
        * @param aAttrs         are the XML attributes of this fragment
        * @param aParent        is the parent XML element of this fragment
        * @param aOwnerDocument is the document which will be the owner of
        *                       the elements of this fragment
        */
        IMPORT_C static CSenCredentialIdentifier* NewL(const TDesC8& aNsUri,
                                                       const TDesC8& aLocalName,
                                                       const TDesC8& aQName,
                                                       const RAttributeArray& aAttrs,
                                                       TXmlEngElement& aParent,
                                                       RSenDocument& aOwnerDocument);

        /**
        * Basic constructor.
        *
        * @since Series60 3.2.3
        * @param aNsUri         is the XML namespace of this fragment
        * @param aLocalName     is the XML localname of this fragment
        * @param aPrefix        is the prefix of this fragment
        * @param aAttrs         are the XML attributes of this fragment
        * @param aParent        is the parent XML element of this fragment
        * @param aOwnerDocument is the document which will be the owner of
        *                       the elements of this fragment
        */
        IMPORT_C static CSenCredentialIdentifier* NewLC(const TDesC8& aNsUri,
                                                        const TDesC8& aLocalName,
                                                        const TDesC8& aQName,
                                                        const RAttributeArray& aAttrs,
                                                        TXmlEngElement& aParent,
                                                        RSenDocument& aOwnerDocument);

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CSenCredentialIdentifier();

        IMPORT_C virtual void SetIdL(TInt aId);

        IMPORT_C virtual TInt IdL();
        
     private:

        /**
        * C++ default constructor.
        */
        CSenCredentialIdentifier();

   };
   
class MSenCredentialObserver
    {
    public:
        enum TSenCredentialChange
            {
            EDestroyed  = 1,
            EReplaced   = 2,
            EAdded
            };

		virtual void CredentialChanged(TSenCredentialChange aChange,
		                               TAny* aPointer = NULL) = 0;
    };
    
typedef RPointerArray<MSenCredentialObserver> RSenCredentialObserverArray;
   
class CSenCredentialData : public CBase
    {
    public:
        /**
        * Basic constructor.
        *
        * @since Series60 3.2.3
        */
        static CSenCredentialData* NewL();

        /**
        * Basic constructor.
        *
        * @since Series60 3.2.3
        */
        static CSenCredentialData* NewLC();

        /**
        * Destructor.
        */
        virtual ~CSenCredentialData();
        
    private:
        /**
        * C++ default constructor.
        */
        CSenCredentialData();
    
    public:
        CSenInternalCredential*     ipCredential;
        TInt                        iCounter;
        RSenCredentialObserverArray iObserverArray;
    };

class RSenCredentialPtr
	{
    public:
	    IMPORT_C void OpenL(CSenInternalCredential* apCredential);
	    IMPORT_C CSenInternalCredential* Credential();
	    IMPORT_C void SetCredential(CSenInternalCredential* apCredential);
	    IMPORT_C RSenCredentialPtr Clone();
	    IMPORT_C void Close();
	    IMPORT_C void CloseAndDestroyCredential();
	    IMPORT_C void AddCredentialObserverL(MSenCredentialObserver &aObserver);	//codescannerwarnings
	    IMPORT_C TInt RemoveCredentialObserver(MSenCredentialObserver &aObserver);

        /**
        * C++ default constructor.
        */
        IMPORT_C RSenCredentialPtr();

    private: // Data
        CSenCredentialData* ipCredentialData;
	};

#endif // SEN_INTERNAL_CREDENTIAL2_H

// End of File


