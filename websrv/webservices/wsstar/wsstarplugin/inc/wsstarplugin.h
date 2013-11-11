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












#ifndef WSSTAR_PLUGIN_H
#define WSSTAR_PLUGIN_H

// INCLUDES
#include <e32std.h>
#include <flogger.h>

#include "senserviceinvocationframework.h" // internal Framework\inc

#include "wsstarcons.h"
#include "sensessionhandler.h"
#include "senmessagehandler.h"
#include "wsstarhandlercontext.h"
#include "wsstarsessioncontext.h"
#include "wsstarmessagecontext.h"
#include "SenSoapMessage.h"
#include "SenXmlConstants.h"    
#include "SenSoapConstants.h"

// DATA TYPES
typedef RSenPointerMap<TDesC8, TDesC8> RMapHandlers;
/**
 * WSStar Plugin framework.
 * An implementation of the CSIF definition. This is concrete
 * class, instance of which ECOM framework gives to ECOM clients.
 */
class CWSStarPlugin : public CSIF
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
         * @return    Instance of this class.
         *
         * Note:       The interface, which is abstract base class of this
         *             implementation, also provides NewL method. Normally abstract
         *             classes do not provide NewL, because they cannot create
         *             instances of themselves.
         * @since S60 4.0
         */
        static CWSStarPlugin* NewL(TAny* aManager);


    public: 

            
        /**
        * Methods implementing CSIF ECOM interface
        * For convenience of components classes
        * @since S60 4.0
        */
        //RFileLogger* Log() const;

        /**
         * Return a system wide unique string to identify this framework
         *
         * @return TDesC8 that uniquely identifies this framework.
         * @since S60 4.0
         */
        virtual const TDesC8& Id();

        /**
         * Try to find one or more session services that match the given
         * ServiceDescription.If not finded new session is created and added into
         * coreManager
         * Return the number of ServiceDescriptions that were added.
         *
         * @param   aPattern - A ServiceDescriptin that should be treated as a
         *          pattern
         * @param    aRemoteConsumer - the consumer sending the request
         * @param    aErrorMsg - in case that return value is negative, this may contain
         *           error as descriptor (SOAP Fault etc)
         * @return (TInt) count of matching ServiceDescriptions that were added or error
         * @since S60 4.0
         */
        virtual TInt AddServiceDescriptionL( MSenServiceDescription& aPattern, 
                                             MSenRemoteServiceConsumer& aRemoteConsumer,
                                             HBufC8*& aErrorMsg ); 

        /**
         * Attempt to register the ServiceDescription. 
         *   In fact session is created but not vaidated and added into CoreManager.
         *   So registering different to adding that it doesnt validate, just register.
         *   Common issue is increase core database by one session (validated or not)
         * @param aServiceDescription A ServiceDescription
         *
         * @return TInt aError - indicates the error or KErrNone if successful
         * @since S60 4.0
         */
        virtual TInt RegisterServiceDescriptionL(MSenServiceDescription& aServiceDescription);


        /**
         * Attempt to unregister the ServiceDescription from the ServiceManager
         * that owns the framework.
         * This is a callback used by the ServiceManager.
         *
         * @param aServiceDescription A ServiceDescription
         *
         * @return TInt aError - indicates the error or KErrNone if successful
         * @since S60 4.0
         */
        virtual TInt UnregisterServiceDescriptionL(
                                    MSenServiceDescription& aServiceDescription);

        /**
         * Create a ServiceSession based upon the given ServiceDescription.
         * The ServiceSession is expected to be "valid", i.e. ready for use
         * by a client.
         *
         * @param aServiceDescription A ServiceDescription
         *
         * @param aNewSession (CSenServiceSession)
         *
         * @return TInt error or KErrNone if successful
         * @since S60 4.0
         */
        virtual TInt CreateServiceSessionL( MSenServiceDescription& aServiceDescription,
                                            MSenServiceDescription& aPattern,
                                            CSenServiceSession*& aNewSession,
                                            MSenRemoteServiceConsumer& aRemoteConsumer,
                                            HBufC8*& /* aErrorMsg */ );

        /**
         * Return a new instance of a framework specific class that
         * implements the ServiceDesccription interface.
         *
         * @param new service description 
         *        (which typically derives from CSenServiceSession)
         *
         * @return TInt error or KErrNone if successful
         * @since S60 4.0
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
         * @since S60 4.0
         */
        virtual MSenCoreServiceManager& Manager();


        /**
         * Set the ServiceManager to which this instance reports.
         * The (singleton) ServiceManager calls this method when
         * it instantiates a framework.
         *
         * @param aServiceManager The singleton ServiceManager in the system.
         * @since S60 4.0
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
         * @since S60 4.0
         */
        virtual CSenBaseFragment& ConfigParser();

        /**
         * Read the ServiceInvocationFramework configuration.
         * This method is invoked by the ServiceManager when it read config.
         * @since S60 4.0
         */
        virtual void StartElementL(const TDesC8& aNsUri,
                              const TDesC8& aLocalName,
                              const TDesC8& aQName,
                              const RAttributeArray& aAttrs);

        /**
         * Read the ServiceInvocationFramework configuration.
         * This method is invoked by the ServiceManager when it read config.
         * @since S60 4.0
         */
        virtual void EndElementL(const TDesC8& aNsUri,
                              const TDesC8& aLocalName,
                              const TDesC8& aQName);

    //derived from SenBaseFragment
        virtual HBufC8* AsXmlL();
        virtual HBufC* AsXmlUnicodeL();

    //derived from SIF
        virtual TInt OnEvent(const TInt aEvent,
                            TAny* /* aArgument */);
        virtual TAny* ExtendedInterface();

        virtual TInt SetTransportPropertiesL(MSenTransport& /* aTransport */);


    //----------------------Outbound direction
        /**
        * Function:   temporary. Call in proper order session handlers during validation phase
        * @return number of added descriptions into coreManager or error
        * @since S60 4.0
        */
        TInt ProcessOutboundValidationL( MSenServiceDescription& aPattern, 
                                         MSenRemoteServiceSession* aRemoteServiceSession,
                                         HBufC8*& aErrorMessage );
        /**
        * Function:   temporary. Call in proper order message handlers during outbound message phase
        * @return symbian error
        * @since S60 4.0
        */
        TInt ProcessOutboundMessageL( CWSStarMessageContext*& aMsgCtx,
                                      const TDesC8* aBody,
                                      CSenSoapMessage*& aReadyMsg );

    //----------------------Inbound direction    
        /**
        * Function:   temporary. Call in proper order message handlers during inbound message phase
        * @return symbian error
        * @since S60 4.0
        */
        TInt ProcessInboundMessageL( CWSStarMessageContext*& aMsgCtx,
                                     CSenSoapMessage*& aMsg );

        /**
        * Function:   temporary. Call in proper order session handlers during inbound dispatch phase
        * @return symbian error
        * @since S60 4.0
        */
        TInt ProcessInboundDispatchL( MSenRemoteServiceSession* aRemoteServiceSession,
                                      CSenSoapMessage*& aMsg );

        //----------------------temporary methods 
        //  (till moment when engine with handlers will be available)
        
        MSenHandler* Handler(const TDesC8& aCue);
        TInt DeviceId(TPtrC8& aValue);
        MSenHandler* PolicyHandler(); 
        TInt PolicyConsumerAddedL(CSenServiceSession* aNewSession, MSenRemoteServiceConsumer& aRemoteConsumer);       
        
    public: // destructor
        virtual ~CWSStarPlugin();

    protected:
        /**
         * Function:   CSenWsiPlugin
         *
         * Discussion: Perform the first phase of two phase construction
         */
        CWSStarPlugin(MSenCoreServiceManager& aManager);

        /**
         * Function:   ConstructL
         *
         * Discussion: Perform the second phase construction of a
         *             CImplementationClassPlus object.
         */
        void ConstructL();
        
    private:
        void AddHandlerTag(TPtr8& aXml, const TDesC8& aHandlerClassName);
        void AddHandlerL(const TDesC8& aHandlerClassName, const TDesC8& aVersion, SenHandler::THandlerType aType);
        void RemoveHandler(const TDesC8& aCue);
        void GenerateDeviceIdL();
        TInt IssuePolicyDownloadL( CSenWSDescription* aSession );
        void VerifyPermissionOnEachSessionL();
                
    private: // data
        MSenCoreServiceManager& iManager;
        //mesg handlers has to be separate to session handlers (separate table) becouse common handlerBase 
        //  doesnt have name method
        RPointerArray<CSenMessageHandler> iMsgHandlers;
        RPointerArray<CSenSessionHandler> iSessionHandlers;
        RSenPointerMap<TDesC8, TDesC8> iVersions;
        RPointerArray<CWSStarHandlerContext> iHandlerContexts;
        HBufC8* iDeviceId;
        TBool iDbReadDone;
        TInt iConsumerCount;
    };


#endif // WSSTAR_PLUGIN_H

