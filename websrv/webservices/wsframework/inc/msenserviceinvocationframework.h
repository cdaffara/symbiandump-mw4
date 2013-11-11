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
* Description:    Header declaration
*
*/








#ifndef M_SEN_SERVICE_INVOCATION_FRAMEWORK_H
#define M_SEN_SERVICE_INVOCATION_FRAMEWORK_H

// INCLUDES
#include "msencoreservicemanager.h"
#include "MSenServiceDescription.h"
#include "MSenServiceDescription.h"

// FORWARD DECLARATIONS
class CSenServiceSession;
class CSenCredential2;

// CLASS DECLARATION

/**
* The core (father) interface for all Service Invocation Frameworks
*/
class MSIF
    {
    public:
        
        // New functions
        
        /**
         * @return TDesC8& that indicates the TYPE of this framework (like ID-WSF, WS-STAR, ATOM, REST, OVI).
         *         Typically it is a relaxed "binding" to either propriatary or well known and/or open 
         *         spesification(s) that the framework implements.
         */
        virtual const TDesC8& Id() = 0;

        /**
         * Create a ServiceSession based upon the given ServiceDescription.
         * The ServiceSession is expected to be "valid", i.e. ready for use
         * by a client.
         * @param aServiceDescription is the service description that was found
         * (match) with given service invokation paramaters ("service pattern")
         * @param aPattern contains the invokation parameters defined by the
         * consumer who wishes to connect to a service. At minimum, this SD
         * includes and endpoint and/or contract (service type).
         * @param aNewSession is a ref-to-pointer that is used to "return"
         * a valid sessions. Important note: in case that this method fails,
         * it is ILLEGAL to put orphan object into this ref-to-ptr. Only in
         * case of success, the ownership of aNewSession is transferred to caller(!)
         * @param    aErrorMsg will be allocated to provide detailed
         *           information about error, if available (like SOAP fault)
         * @return KErrNone if successful (aNewSession ownership is transferred to
         * caller) or some of the system-wide errorcodes (aNewSession should be NULL)
         */
        virtual TInt CreateServiceSessionL( MSenServiceDescription& aServiceDescription,
                                            MSenServiceDescription& aPattern,
                                            CSenServiceSession*& aNewSession,
                                            MSenRemoteServiceConsumer& aRemoteConsumer,
                                            HBufC8*& aErrorMsg ) = 0;


        /**
         * @return a new instance of a framework specific class that
         * implements the ServiceDesccription interface.
         */
        virtual CSenWSDescription* NewServiceDescriptionL() = 0;

        /**
         * @return the CoreServiceManager for which this instance is working.
         * This is used by the ServiceSession instances created by this
         * framework.
         * Note, in WSF, ServiceManager IS SINGLETON(!), so all Frameworks
         * return their "owner" - that is - the only service manager in
         * SenFramework
         */
        virtual MSenCoreServiceManager& Manager() = 0;

        /**
         * Attempt to register the ServiceDescription to the CoreServiceManager
         * that owns the framework.
         * This is a callback used by the ServiceManager to give frameworks
         * a chance to replace a generic ServiceDescription into something
         * that is more specific, and more useful, to the
         * ServiceInvocationFramework.
         * @param aServiceDescription A ServiceDescription
         * @return TInt aError - indicates the error or KErrNone if successful
         */
        virtual TInt RegisterServiceDescriptionL( MSenServiceDescription& aServiceDescription ) = 0;


        /**
         * Attempts to unregister the ServiceDescription from the
         * CoreServiceManager that owns the framework.
         * This is a callback used by the ServiceManager.
         * @param aServiceDescription A ServiceDescription
         * @return TInt aError - indicates the error or KErrNone if successful
         */
        virtual TInt UnregisterServiceDescriptionL( MSenServiceDescription& aServiceDescription ) = 0;

        /**
         * Logging method intended for the use of Service Invocation Frameworks
         * and their component classes.
         */
        //virtual RFileLogger* Log() const = 0; 
        
        /**
        * SIF should set any framework specific transport properties via this method.
        * @param aTransport is the transport plug-in where the (framework layer)
        * properties transport properties can be applied
        * @return KErrNone if success, or othewise a system-wide error code 
        */
        virtual TInt SetTransportPropertiesL( MSenTransport& aTransport ) = 0;
        
        /**
         * @return a string that is the unique for this implementation. Note that there might be multiple
         * frameworks, which all implement same TYPE (framework ID is the same), but which have different
         * design. This might mean that they implement different VERSION of the framework. It is recommended
         * that framework CUE is the ECOM cue of the framework, and that it is in form of URN (unique resource
         * name).
         */
        virtual TPtrC8 Cue() const = 0;
    };

#endif // M_SEN_SERVICE_INVOCATION_FRAMEWORK_H

// End of File
