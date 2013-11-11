/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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








#ifndef SEN_REST_PLUGIN_H
#define SEN_REST_PLUGIN_H

#include <e32std.h>
#include <flogger.h>
#include "SenXmlUtils.h"

// FORWARD DECLARATIONS
class MSenCoreServiceManager;
class CSenBaseFragment;
class MSenTransport;
class CSenRestServiceSession;

// CONSTANTS

_LIT8(KRestFrameworkVersion,     "1.0");
_LIT8(KRestFrameworkXMLNS,       "urn:com.nokia.Sen.rest.config.1.0");
_LIT8(KRestFrameworkCue,         "com.nokia.Sen.rest.RestServiceFramework");
                                 
// INCLUDES
#include "senserviceinvocationframework.h" // internal Framework\inc

/**
 * Class:       CSenIdWsfPlugin
 *
 * Description: An implementation of the CSIF definition. This is concrete
 * class, instance of which ECOM framework gives to ECOM clients.
 */
class CSenRestPlugin : public CSIF
    {
public:
    /**
     * Function:    NewL
     *
     * Description: Create instance of concrete implementation. Note that ECOM
     *              interface implementations can only have two signatures for
     *              NewL:
     *               - NewL without parameters (used here)
     *               - NewL with TAny* pointer, which may provide some client
     *                 data
     *
     * Returns:    Instance of this class.
     *
     * Note:       The interface, which is abstract base class of this
     *             implementation, also provides NewL method. Normally abstract
     *             classes do not provide NewL, because they cannot create
     *             instances of themselves.
     */
    static CSenRestPlugin* NewL(TAny* aManager);


public: // Methods implementing CSIF ECOM interface
    // For convenience of components classes
    //RFileLogger* Log() const;

    /**
     * Return a system wide unique string to identify this framework
     *
     * @return TDesC8 that uniquely identifies this framework.
     */
    virtual const TDesC8& Id();

    /**
     * Try to find one or more services that match the given
     * ServiceDescription.
     * Register ServiceDescriptions for these to the owner of the framework,
     * i.e. to the ServiceManager. Return the number of ServiceDescriptions
     * that were added.
     *
     * @param   aPattern - A ServiceDescriptin that should be treated as a
     *          pattern
     *
     * @param
     *  return (TInt) count of matching ServiceDescriptions that were added
     */
    virtual TInt AddServiceDescriptionL( MSenServiceDescription& aPattern, 
                                         MSenRemoteServiceConsumer& aRemoteConsumer,
                                         HBufC8*& aErrorMsg ); 

    /**
     * Attempt to register the ServiceDescription to the ServiceManager that
     * owns the framework.
     * This is a callback used by the ServiceManager to give frameworks a
     * chance to replace a generic ServiceDescription into something that is
     * more specific, and more useful, to the ServiceInvocationFramework.
     *
     * @param aServiceDescription A ServiceDescription
     *
     *  return TInt aError - indicates the error or KErrNone if successful
     */
    virtual TInt RegisterServiceDescriptionL(MSenServiceDescription& aServiceDescription);


    /**
     * Attempt to unregister the ServiceDescription from the ServiceManager
     * that owns the framework.
     * This is a callback used by the ServiceManager.
     *
     * @param aServiceDescription A ServiceDescription
     *
     *  return TInt aError - indicates the error or KErrNone if successful
     */
    virtual TInt UnregisterServiceDescriptionL(MSenServiceDescription& aServiceDescription);

    /**
     * Create a ServiceSession based upon the given ServiceDescription.
     * The ServiceSession is expected to be "valid", i.e. ready for use
     * by a client.
     *
     * @param aServiceDescription A ServiceDescription
     *
     * @param aNewSession (CSenServiceSession)
     *
     * return TInt error or KErrNone if successful
     */
    virtual TInt CreateServiceSessionL(MSenServiceDescription& aServiceDescription,
                                       MSenServiceDescription& aPattern,
                                       CSenServiceSession*& aNewSession,
                                       MSenRemoteServiceConsumer& aRemoteConsumer,
                                       HBufC8*& aErrMsg);

    /**
     * Return a new instance of a framework specific class that
     * implements the ServiceDesccription interface.
     *
     * @param new service description 
     *        (which typically derives from CSenServiceSession)
     *
     * return TInt error or KErrNone if successful
     */
    virtual CSenWSDescription* NewServiceDescriptionL();

    /**
     * Return the ServiceManager for which this instance is working.
     * This is used by the ServiceSession instances created by this framework.
     *
     * Note, in Sen, MSenCoreServiceManager IS SINGLETON(!), so all Frameworks
     * return their "owner" - that is - the only service manager in
     * Sen Framework
     *
     * @return (MSenCoreServiceManager)
     */
    virtual MSenCoreServiceManager& Manager();


    /**
     * Set the ServiceManager to which this instance reports.
     * The (singleton) ServiceManager calls this method when
     * it instantiates a framework.
     *
     * @param aServiceManager The singleton ServiceManager in the system.
     */
    virtual void SetManager(MSenCoreServiceManager& aServiceManager);


    /**
     * Return an object that can do SAX based parsing of a XML fragment
     * that contains framework specific configuration information.
     * The ServiceManager asks for this BaseFragment upon startup
     * in order to give this framework a chance to read its configuration
     * information from the central ServiceManager configuration document.
     *
     * @return (CSenBaseFragment)
     */
    virtual CSenBaseFragment& ConfigParser();

    /**
     * Write the ServiceInvocationFramework configuration as XML to a HBufC8.
     *
     * This method is invoked by the ServiceManager when it saves
     * its state and/or configuration.
     *
     * Upon startup the chunk of XML that is written will be passed back to
     * the configuation parser of the framework.
     *
     * The "schema" that shall be adhered to is:
     * <pre>
     * &lt;element name="Framework"&gt;
     *      &lt;any/&gt;&lt;!-- this is where implementations write additional
     *      information --&gt;
     *      &lt;attribute name="class" type="String"&gt;
     *    &lt;!-- In Syimbian the class attribute is used as cue for the
     *      ECOM Resolver --&gt;
     * &lt;/element&gt;
     * </pre>
     *
     * @param aTo - a HBufC8 to which the framework should write its
     *              configuration
     */

    virtual HBufC8* AsXmlL();
    virtual HBufC* AsXmlUnicodeL();
    virtual void EndElementL(const TDesC8& aNsUri, const TDesC8& aLocalName, const TDesC8& aQName);

    TInt OnEvent(const TInt aEvent, TAny* aArgument);

    virtual TAny* ExtendedInterface();

    // New methods

    /**
    * REST framework specifies at least following text/xml content type:
    * KTextWwwFormContentType ==  "application/x-www-form-urlencoded; charset=UTF-8"
    * @return KErrNone on success, system-wide error code otherwise
    */    
    virtual TInt SetTransportPropertiesL(MSenTransport& aTransport);

public: // destructor
    virtual ~CSenRestPlugin();


protected:
    /**
     * Function:   CSenRestPlugin
     *
     * Discussion: Perform the first phase of two phase construction
     */
    CSenRestPlugin(MSenCoreServiceManager& aManager);

    /**
     * Function:   ConstructL
     *
     * Discussion: Perform the second phase construction of a
     *             CImplementationClassPlus object.
     */
    void ConstructL();

private: // data
    MSenCoreServiceManager& iManager;
    };


#endif // SEN_REST_PLUGIN_H

