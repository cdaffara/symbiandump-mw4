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
* Description: Header declaration
*
*/








#ifndef SEN_IDWSF_PLUGIN_H
#define SEN_IDWSF_PLUGIN_H

// INCLUDES
#include <e32std.h>
#include <flogger.h>

#include <SenXmlUtils.h>
#include <SenIdentityProvider.h>

#include "idwsfauthenticationserviceclient.h"
#include "idwsfserviceinstance.h"

#include "senserviceinvocationframework.h" // internal Framework\inc


// CONSTANTS
_LIT8(KIdWsfFrameworkVersion, "1.0");
_LIT8(KIdWsfFrameworkXMLNS, "urn:com.nokia.Sen.idwsf.config.1.0");
_LIT8(KIdWsFrameworkCue, "com.nokia.Sen.idwsf.IdentityBasedWebServicesFramework");
_LIT8(KIdWsfFrameworkLocalname, "Framework");
_LIT8(KIdWsfFrameworkAuthenticationServiceContract, "urn:liberty:as:2004-04");
_LIT8(KIdWsfFrameworkDiscoveryServiceContract, "urn:liberty:disco:2003-08");

// FORWARD DECLARATIONS
class MSenCoreServiceManager;
class CSenBaseFragment;
class CIdWsfCoreServiceConsumer;
class CIdWsfAuthenticationServiceClient;
class CIdWsfDiscoveryServiceClient;
class CIdWsfServiceSession;

typedef struct
{
    CSenWSDescription*  iSD;
    TInt                        iScore;
} TSDScore;

// CLASS DECLARATION 
/**
* An implementation of the CSIF definition. This is concrete
* class, instance of which ECOM framework gives to ECOM clients.
*/
class CSenIdWsfPlugin : public CSIF
    {
    public: // Constructors and destructor
                
        /**
        * Create instance of concrete implementation. Note that ECOM
        * interface implementations can only have two signatures for
        * NewL:
        *   - NewL without parameters (used here)
        *   - NewL with TAny* pointer, which may provide some client data
        * Note: The interface, which is abstract base class of this
        * implementation, also provides NewL method. Normally abstract
        * classes do not provide NewL, because they cannot create
        * instances of themselves.
        * @return Instance of this class.
        */
        static CSenIdWsfPlugin* NewL(TAny* aManager);

        virtual ~CSenIdWsfPlugin();

        // New functions
        
        // Methods implementing CSIF ECOM interface
    
        /**
        * For convenience of components classes
        */
        //RFileLogger* Log() const;

        /**
        * @return TDesC8 that uniquely identifies this framework.
        */
        virtual const TDesC8& Id();

        /**
        * Try to find one or more services that match the given
        * ServiceDescription.
        * Register ServiceDescriptions for these to the owner of the framework,
        * i.e. to the ServiceManager. Return the number of ServiceDescriptions
        * that were added.
        * @param    aPattern - A ServiceDescriptin that should be treated as a
        *           pattern
        * @return (TInt) count of matching ServiceDescriptions that were added
        */
        virtual TInt AddServiceDescriptionL( MSenServiceDescription& aPattern,
                                             MSenRemoteServiceConsumer& aRemoteConsumer, 
                                             HBufC8*& /* aErrorMsg */ );

        virtual TInt AddServiceDescriptionL( const TDesC8& aServiceTypeContract,
                                             MSenRemoteServiceConsumer& aRemoteConsumer,
                                             HBufC8*& /* aErrorMsg */  );

        /**
        * Attempt to register the ServiceDescription to the ServiceManager that
        * owns the framework.
        * This is a callback used by the ServiceManager to give frameworks a
        * chance to replace a generic ServiceDescription into something that is
        * more specific, and more useful, to the ServiceInvocationFramework.
        * @param aServiceDescription A ServiceDescription
        * @return TInt aError - indicates the error or KErrNone if successful
        */
        virtual TInt RegisterServiceDescriptionL(
                                MSenServiceDescription& aServiceDescription);

        /**
        * Attempt to unregister the ServiceDescription from the ServiceManager
        * that owns the framework.
        * This is a callback used by the ServiceManager.
        * @param aServiceDescription A ServiceDescription
        * @return TInt aError - indicates the error or KErrNone if successful
        */
        virtual TInt UnregisterServiceDescriptionL(
                                MSenServiceDescription& aServiceDescription);

        /**
        * Create a ServiceSession based upon the given ServiceDescription.
        * The ServiceSession is expected to be "valid", i.e. ready for use
        * by a client.
        * @param aServiceDescription A ServiceDescription
        * @param aNewSession (CSenServiceSession)
        * @return TInt error or KErrNone if successful
        */
        virtual TInt CreateServiceSessionL( MSenServiceDescription& aServiceDescription,
                                            MSenServiceDescription& aPattern,
                                            CSenServiceSession*& aNewSession,
                                            MSenRemoteServiceConsumer& aRemoteConsumer,
                                            HBufC8*& aErrorMsg );

        /**
        * Return a new instance of a framework specific class that
        * implements the ServiceDesccription interface.
        * @param new service description 
        *        (which typically derives from CSenServiceSession)
        * @return TInt error or KErrNone if successful
        */
        virtual CSenWSDescription* NewServiceDescriptionL();

        /**
        * @return the ServiceManager for which this instance is working.
        * This is used by the ServiceSession instances created by this framework.
        * Note, in WSF, MSenCoreServiceManager IS SINGLETON(!), so all Frameworks
        * return their "owner" - that is - the only service manager in
        * WS Framework
        */
        virtual MSenCoreServiceManager& Manager();

        /**
        * Set the ServiceManager to which this instance reports.
        * The (singleton) ServiceManager calls this method when
        * it instantiates a framework.
        * @param aServiceManager The singleton ServiceManager in the system.
        */
        virtual void SetManager(MSenCoreServiceManager& aServiceManager);

        /** 
        * @return an object that can do SAX based parsing of a XML fragment
        * that contains framework specific configuration information.
        * The ServiceManager asks for this BaseFragment upon startup
        * in order to give this framework a chance to read its configuration
        * information from the central ServiceManager configuration document.
        */
        virtual CSenBaseFragment& ConfigParser();

        /**
        * Write the ServiceInvocationFramework configuration as XML to a HBufC8.
        * This method is invoked by the ServiceManager when it saves
        * its state and/or configuration.
        * Upon startup the chunk of XML that is written will be passed back to
        * the configuration parser of the framework.
        * The "schema" that shall be adhered to is:
        * <pre>
        * &lt;element name="Framework"&gt;
        *       &lt;any/&gt;&lt;!-- this is where implementations write additional
        *       information --&gt;
        *       &lt;attribute name="class" type="String"&gt;
        *    &lt;!-- In Symbian the class attribute is used as cue for the
        *       ECOM Resolver --&gt;
        * &lt;/element&gt;
        * </pre>
        *
        * @param aTo - a HBufC8 to which the framework should write its
        *               configuration
        */
        virtual HBufC8* AsXmlL();

        virtual HBufC* AsXmlUnicodeL();
        virtual void EndElementL(   const TDesC8& aNsUri,
                                    const TDesC8& aLocalName,
                                    const TDesC8& aQName);

        virtual TInt OnEvent(const TInt aEvent,
                             TAny* /* aArgument */);

        virtual TAny* ExtendedInterface();
        
        // From MSIF:
        /**
        * ID-WSF specifies at least following: 
        * - empty SOAPAction HTTP header value.
        *   KSenSoapActionHeaderValueEmpty ==  ""
        * It is forced to BOTH framework and message layer (!)
        * to prevent breaking the Liberty specs in this
        * (empty SOAPAction value is MUST)
        * @return KErrNone on success, system-wide error code otherwise
        */    
        virtual TInt SetTransportPropertiesL(MSenTransport& aTransport);        

    protected:
    
        /**
        * Perform the first phase of two phase construction
        */
        CSenIdWsfPlugin(MSenCoreServiceManager& aManager);

        /**
        * Perform the second phase construction of a
        *             CImplementationClassPlus object.
        */
        void ConstructL();

        // New functions
        
        TBool ValidateL(CIdWsfServiceSession& aSession,
                        MSenRemoteServiceConsumer& aRemoteConsumer);

        CIdWsfDiscoveryServiceClient* DSClientL(
                                const TDesC8& aTrustAnchor,
                                MSenRemoteServiceConsumer& aRemoteConsumer);

        CIdWsfDiscoveryServiceClient* DSClientL(
                                CSenIdentityProvider* aProvider,
                                MSenRemoteServiceConsumer& aRemoteConsumer);

        CIdWsfAuthenticationServiceClient* ASClientL(
                                            CSenIdentityProvider* aProvider);

        TInt CoreServiceDescriptionsL(
                            RPointerArray<CSenWSDescription>& aMatches,
                            const TDesC8& aContract,
                            const TDesC8& aProviderID);

        TInt AssignSessionValidatorsL();

    private:
        
        // New functions
        
        CSenIdentityProvider* IdentityProviderL(const TDesC8& aProviderId);
        CSenIdentityProvider* IdentityProviderL(MSenServiceDescription *aPattern);
        TBool ValidateDSL(  CIdWsfDiscoveryServiceClient *aDSClient,
                            CSenIdentityProvider *aIDP,
                            MSenRemoteServiceConsumer& aRemoteConsumer);

        virtual TInt RegisterASClientL(
                                MSenServiceDescription* aServiceDescription);

        virtual TInt RegisterDSClientL(
                                MSenServiceDescription* aServiceDescription);

        virtual TInt UnRegisterASClientL(
                                MSenServiceDescription* aServiceDescription);

        virtual TInt UnRegisterDSClientL(
                                MSenServiceDescription* aServiceDescription);
                                
        static TInt CompareSDScore(const TSDScore& aSDScoreLeft,
                                    const TSDScore& aSDScoreRight);

    private: // Data
            MSenCoreServiceManager& iManager;
    };


#endif // SEN_IDWSF_PLUGIN_H

// End of File
