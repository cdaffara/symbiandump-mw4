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








#ifndef SERVICE_INVOCATION_FRAMEWORK_INTERFACE_H
#define SERVICE_INVOCATION_FRAMEWORK_INTERFACE_H

// INCLUDES
#include <ecom/ecom.h>

#include "msenserviceinvocationframework.h"

// CONTANTS
const TInt KErrSenReinitRequired = -30304;
const TInt KErrSenResendRequired = -30316;
const TInt KErrSenFailedAuthentication  = -30320;
//const TInt KErrSenResendComplete = -30322; 

// Default events
const TInt KSenEventWsfReady                            = 1;
const TInt KSenEventSessionsDbReadBegin                 = 100;
const TInt KSenEventSessionsDbReadComplete              = 101;
const TInt KSenEventSessionsDbWriteBegin                = 102;
const TInt KSenEventSessionsDbWriteComplete             = 103;
const TInt KSenEventIdWsfSessionValidatorRequested      = 200;
const TInt KSenEventConsumerReleasesSession             = 300;
const TInt KSenEventConsumerAdded                       = 301;
const TInt KSenEventConsumerRemoved                     = 302;
const TInt KSenEventRegisterNewServiceDescription       = 400;
const TInt KSenEventRegisterIdentityProvider            = 500;
const TInt KSenEventUnregisterIdentityProvider          = 501;

// FORWARD DECLARATIONS
class CSenSecurityMechanism;
struct TSifData;

/**
 * Class: CSIF (ServiceInvocationFramework ECOM interface)
 * Custom ECOM interface definition. This interface is used by
 * clients to find specific instance implementation(s).
 */
class CSIF : public CSenBaseFragment, public MSIF
    {
    public: // Constructors and destructor
        
        /**
        * Wraps ECom object instantiation, which returns the
        * default interface implementation.
        * Note: This is not a "normal" NewL method, since normally NewL
        * methods are only defined for concrete classes.
        * Note that the implementations of this interface
        * ALSO provide NewL methods. They are the familiar
        * EPOC NewL's, which create instance of classes.
        */
        static CSIF* NewL( MSenCoreServiceManager& aManager );

        /**
        * Wraps ECom object instantitation, which returns the
        * interface implementation matching to given aCue.
        * IMPLEMENTATION_INFO::default_data.
        * For details, see EcomSIFDefinition.inl comments.
        * Note: This is not a "normal" NewL method, since normally NewL
        *       methods are only defined for concrete classes.
        * Note that the implementations of this interface ALSO
        * provide NewL methods. They are the familiar EPOC
        * NewL's, which create instance of classes.
        * @param  aCue is the "name" of the requested implementation.
        *         Implementations advertise their cue as specified
        *         in their resource file field
        */
        static CSIF* NewL( const TDesC8& aCue, MSenCoreServiceManager& aManager );

        /**
         * Destructor.
         */
        virtual ~CSIF();

        // New functions
        
        /**
        * Try to find one or more services that match the given
        * ServiceDescription. Register ServiceDescriptions for these to the
        * owner of the framework, i.e. to the ServiceManager. Return the
        * number of ServiceDescriptions that were added.
        * @param    aPattern - A ServiceDescriptin that should be treated
        *           as a pattern
        * @param    aErrorMsg will be allocated to provide detailed
        *           information about error, if available (like SOAP fault)
        * @return   On success, returns a positive number: - a total count 
        *           of matching ServiceDescriptions that were added.
        *           In case of error, the returned value will be negative:
        *           - one of the system-wide error codes.
        */
        virtual TInt AddServiceDescriptionL( MSenServiceDescription& aPattern,
                                             MSenRemoteServiceConsumer& aRemoteConsumer,
                                             HBufC8*& aErrorMsg ) = 0;

        /**
        * Set the ServiceManager to which this instance reports.
        * The (singleton) ServiceManager calls this method when
        * it instantiates a framework.
        * @param aServiceManager The singleton ServiceManager in the system.
        */
        virtual void SetManager( MSenCoreServiceManager& aServiceManager ) = 0;

        /**
        * The CoreServiceManager asks for this BaseFragment upon startup
        * in order to give this framework a chance to read its configuration
        * information from the central ServiceManager configuration document.
        * @return CSenBaseFragment - an object that can do SAX based parsing of a XML fragment
        * that contains framework specific configuration information.
        */
        virtual CSenBaseFragment& ConfigParser() = 0;

        /**
        * This method may be called to pass certain events to the SIF 
        * plug-in.
        * There may be few common events in WSF scope plus other, plug-in 
        * specific events.
        * Plug-ins can themselves decide, whether or not to take action
        * on notified event. For example, ID-WSF plug-in might be interested
        * to assign "session validators" (AS and DS clients) to the newly
        * instantiated sessions, after it has been notified, that all sessions
        * have been parsed into memory (de-serialized) from sensessions.xml
        * database.
        */
        virtual TInt OnEvent( const TInt aEvent, TAny* aArgument ) = 0;

        /**
        * Getter for SIF specific extension interface, which caller
        * typically class-casts to pre-known type
        * @return pointer to plug-in specific interface, or NULL
        *         if interface extension is NOT implemented.
        */
        virtual TAny* ExtendedInterface() = 0;
        
        /**
         * @return a string that is the unique for this implementation. Note that there might be multiple
         * frameworks, which all implement same TYPE (framework ID is the same), but which have different
         * design. This might mean that they implement different VERSION of the framework. It is recommended
         * that framework CUE is the ECOM cue of the framework, and that it is in form of URN (unique resource
         * name). 
         * This default, inline implementation is returning the ECOM cue value of this plug-in.
         */
        virtual TPtrC8 Cue() const;        
    private:

        struct TSifData
            {
            /** Instance identifier key. When instance of an
            * implementation is created by ECOM framework, the
            * framework will assign UID for it. The UID is used in
            * destructor to notify framework that this instance is
            * being destroyed and resources can be released.
            */
            TUid iDtor_ID_Key;
            
            /*
            * This equals to ECOM's data type (== "cue" of the plug-in)
            */
            HBufC8* iCue;
            //HBufC8* iReserved;
            //HBufC8* iReserved*;
            inline void Close();
            };        

    private: // Data
        TSifData* iData;
    };
    
// Inline includes the implementation of the instantiation
// functions and destructor
#include "senserviceinvocationframework.inl"

#endif // SERVICE_INVOCATION_FRAMEWORK_INTERFACE_H

// End of File

