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
* Description:        Class implements the most common value object used in WSF,
*                which is used to  describe some invocable service.
*
*/








#ifndef SEN_WS_DESCRIPTION_H
#define SEN_WS_DESCRIPTION_H

//  INCLUDES
#include "SenXmlServiceDescription.h"

/*
*  @lib SenServDesc.lib
*  @since Series60 3.0
*/
class CSenWSDescription : public CSenXmlServiceDescription
    {
    public:  // Constructors and destructor

        /**
        * Standard 2 phase constructor
        * @return a pointer to a new CSenWSDescription instance.
        */
        IMPORT_C static CSenWSDescription* NewL();

        /**
        * Standard 2 phase constructor
        * @return a pointer to a new CSenWSDescription instance,
        *        which is left on cleanup stack.
        */
        IMPORT_C static CSenWSDescription* NewLC();

        /**
        * Standard 2 phase constructor
        * @param aNamespaceURI namespace URI for the service description.
        * @return a pointer to a new CSenWSDescription instance
        */
        IMPORT_C static CSenWSDescription* NewL(const TDesC8& aNamespaceURI);

        /**
        * Standard 2 phase constructor
        * @param aNamespaceURI namespace URI for the service description.
        * @return a pointer to a new CSenWSDescription instance,
        *        which is left on cleanup stack.
        */
        IMPORT_C static CSenWSDescription* NewLC(const TDesC8& aNamespaceURI);

        /**
        * Standard 2 phase constructors
        * @param aEndpoint the service endpoint. Note that endpoint cannot
        *        include characters which are illegal in XML. If endpoint
        *        is an URL which contains illegal characters (like '&'-char),
        *        those need to be encoded into XML entity form (like &amp;).
        *        EncodeHttpCharactersLC() method from XmlUtils can be used
        *        for encoding of basic XML entities.
        * @param aContract identifies the service type.
        * @return a pointer to a new CSenWSDescription instance
        */
        IMPORT_C static CSenWSDescription* NewL(const TDesC8& aEndPoint,
                                                        const TDesC8& aContract);

        /**
        * Standard 2 phase constructors
        * @param aEndpoint the service endpoint. Note that endpoint cannot
        *        include characters which are illegal in XML. If endpoint
        *        is an URL which contains illegal characters (like '&'-char),
        *        those need to be encoded into XML entity form (like &amp;).
        *        EncodeHttpCharactersLC() method from XmlUtils can be used
        *        for encoding of basic XML entities.
        * @param aContract identifies the service type.
        * @return a pointer to a new CSenWSDescription instance,
        *        which is left on cleanup stack.
        */
        IMPORT_C static CSenWSDescription* NewLC(const TDesC8& aEndPoint,
                                                         const TDesC8& aContract);

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CSenWSDescription();

        // New functions
        /**
        * Method returns ETrue if the primary keys of this service description
        * are equal. Definition of primary keys varies on concrete implementations.
        *
        * In a typical XML sub class implementation the primary keys are Contract
        * and Endpoint elements.
        *
        * Difference to Match() method is that primary keys - often descriptors -
        * MUST be equal both in this service description and in aCandidate, unlike
        * in Match(), where argument is more like wildcard pattern matching even
        * in cases where it includes fewer fields (less information).
        *
        * @since Series60
        * @param    aCandidate is a service description, which primary keys are
        *           checked in this comparison.
        *
        * @return TBool ETrue if primary keys are equal, EFalse otherwise.
        */
        IMPORT_C virtual TBool HasEqualPrimaryKeysL(MSenServiceDescription& aCandidate);

        /*
        * Method checks specific pieces of information to determine, whether this
        * service description is local or not. Typically this is defined by the
        * endpoint's scheme, which is KSenTransportSchemeLocal in most of the cases,
        * when this method returns true.
        * @return boolean indicating whether this endpoint is local or not.
        */
        IMPORT_C virtual TBool IsLocalL(); 


        /*
        * Method for binding transport plug-in type with the endpoint in question.
        * Function adds/sets XML attribute called "cue" for <Endpoint> element:
        *
        *  <ServiceDescription>
        *    <Endpoint cue="com.nokia.wsf.transport.plugin.hostlet">
        *      urn:nokia.com.test.hostlet
        *    </Endpoint>
        *  </ServiceDescription>
        *
        * In above example, eventhough endpoint scheme "local://" would normally
        * invoke different type of plug-in, the "cue" attribute overrides this, and
        * forces hostlet transport plugin to be loaded.
        *
        * Transport plug-in types (cues):
        *
        *    _LIT8(KSenTransportCueHTTP,              "com.nokia.wsf.transport.plugin.httpchannel");
        *    _LIT8(KSenTransportCueVirtualTCP,        ""); 
        *    _LIT8(KSenTransportCueLocalEcom,         "com.nokia.wsf.transport.plugin.local");
        *    _LIT8(KSenTransportCueHostletConnection, "com.nokia.wsf.transport.plugin.hostlet");
        *
        *
        * Note that this method does not attempt to load the plug-in - it might not
        * even exist in the system. Neither is this attribute checked when XML service
        * description is parsed. As a conclusion, if non-existant plug-ins are bind
        * to endpoints, they will be ignored, and the transport is created in normal
        * way and plug-in is chosen based on endpoint scheme and/or defaults.
        *
        * @param aTransportCue is the ECOM cue of CSenTransport implementation.
        * @return KErrNone on success
        *         KErrSenNoEndpoint, if actual endpoint element does not exist
        *
        */
        IMPORT_C virtual TInt SetTransportCueL(const TDesC8& aTransportCue);


        /**
        * Getter for transport cue, assuming that attribute has been set,
        * and that has "cue" -attribute.
        * @return transport plug-in ECOM cue, or KNullDesC8, if it has
        * not been set in this service description.
        */
        IMPORT_C virtual TPtrC8 TransportCue();

        /**
        * Method provides convenient way to ensure up-casting of a subclass instance,
        * when it is seen via CSenWSDescription handle.
        * Each subclass has to implement this method.
        * @return boolean indicating whether the class in question has a super class
        * of requested type. ETrue is returned if any super class up in the inheritance
        * chain has mathing parent, via recursion. Method returns EFalse, if none of the
        * classes in the inheritance chain derives from requested type.
        */
        IMPORT_C virtual TBool HasSuperClass( TDescriptionClassType aType );

        /**
        * Getter for (web) service policy
        * @return pointer to service policy, or NULL, if it is not available
        */
        IMPORT_C virtual MSenServicePolicy* ServicePolicy();

        /**
        * Method provides convenient way to add a new value in Client Policy
        * This method will add attributes such as <SOAP12> or <UsernameTokenOverTLS>
        * in the Policy to be used by stack. Infact this method adds a new element
        * in xml representation of ServicePolicy.
        * @param aName is the Name of the attribute.
        * @return KErrNone on success, KErrArgument if any of the arguments
        * (descriptors) is of zero-length, or one of the system-wide
        * error codes otherwise.
        */
        IMPORT_C virtual TInt SetPolicyL(const TDesC8& aName);

        /**
        * Method provides convenient way to add a new value in Client Policy
        * This method will add attributes such as <Addressing>
        * in the Policy to be used by stack. Infact this method adds a new element
        * in xml representation of ServicePolicy and adds its value as contents of the element.
        * <Addressing>http://schemas.xmlsoap.org/ws/2004/03/addressing</Addressing>
        * @param aName is the Name of the Policy attribute.
        * @param aValue is the Contents of the Policy attribute.
        * @return KErrNone on success, KErrArgument if any of the arguments
        * (descriptors) is of zero-length, or one of the system-wide
        * error codes otherwise.
        */

        IMPORT_C virtual TInt SetPolicyL(const TDesC8& aName, const TDesC8& aValue);
        /**
        * Method provides convenient way to add a new value in Client Policy
        * This method will add attributes such as <Addressing>
        * in the Policy to be used by stack. Infact this method adds a new element
        * in xml representation of ServicePolicy and adds its value as contents of the element.
        * <MetadataEndpoint method = "GET">http://www.mypolicyendpoint.com/policy2/</MetadataEndpoint>
        * @param aName is the Name of the Policy attribute.
        * @param aValue is the Contents of the Policy attribute.
        * @param aAttribName is the Name of the attribute in element aName
        * @param aAttribValue is the Contents of the aAttribName attribute.
        * @return KErrNone on success, KErrArgument if any of the arguments
        * (descriptors) is of zero-length, or one of the system-wide
        * error codes otherwise.
        */

        IMPORT_C virtual TInt SetPolicyL(const TDesC8& aName, const TDesC8& aValue, const TDesC8& aAttribName, const TDesC8& aAttribValue);

        /**
        * Setter for (identity) provider ID
        * @since Series60 5.0
        * @param aProviderID the unique identifier of the (identity) provider
        * @return KErrNone on success, KErrArgument if aProviderID is of zero-length,
        * or one of the system-wide Symbian error codes otherwise.
        */
        IMPORT_C virtual TInt SetProviderIdL( const TDesC8& aProviderID );

        /**
        * Getter for (identity) provider ID
        * @since Series60 5.0
        * @return the (locally) unique identifier of the (identity) provider
        * @return KErrNone on success, KErrArgument if aProviderID is of zero-length,
        * or one of the system-wide Symbian error codes otherwise.
        */
        IMPORT_C TPtrC8 ProviderId();

        /**
        * Setter for userinfo attribute, which determinates whether the username
        * - password notifier dialog is shown to end-user or not, when authentication
        * fails (due wrong, or incomplete userinfo).
        * @param aPromptUserInfoMode
        *  EFalse dictates that an error must be returned when creating
        *         a service connection and userinfo is not accepted by
        *         (remote) authentication, instead of showing the dialog.
        *  ETrue (or if attribute does not exist in this XML SD) means that
        *         end-user should be prompted (default behaviour). The number
        *         of retry attempts (each showing a dialog) is service invocation
        *         framework spesific; typically 3 retries are permitted.
        */
        IMPORT_C void SetPromptUserInfoL( TBool aPromptUserInfoMode );

        /**
        * Getter for current userinfo mode (attribute).
        * @return Boolean that indicates the mode:
        *  EFalse means that end-user prompts (notifier dialogs) have been
        *  explicitely surpressed, and thus will not be shown to end-user:
        *  attribute value is exactly as follows: "false"
        *  ETrue means that attribute does not exist (default), or it has ANY OTHER
        *  value, but NOT "false"
        */
        IMPORT_C TBool PromptUserInfo();

    protected:  // New functions

        /**
        * C++ default constructor.
        * @param aType enumeration defining the type of this class.
        */
        IMPORT_C CSenWSDescription(TDescriptionClassType aType);

	public:
		/**
        * Sets the SNAP ID.
        * @param aSnapId  A Snap ID
        */
	   	IMPORT_C virtual void SetSnapIdL(TUint32 aSnapId); 
		/**
        * Gets the SNAP ID.
        * @param aCurrentSnapId  A TUint32 reference to be filled in with the
        *                       value of the SNAP ID.
        * @return               KErrNone if no error, or some of the system
        *                       wide error codes.
        */
       IMPORT_C virtual TInt SnapId(TUint32& aCurrentSnapId); 	

    private: // Data
        CSenServicePolicy* 	iServicePolicy;
    };

#endif // SEN_WS_DESCRIPTION_H

// End of File


