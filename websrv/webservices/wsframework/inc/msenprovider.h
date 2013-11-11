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








#ifndef M_SEN_PROVIDER_H
#define M_SEN_PROVIDER_H

#include "MSenHostlet.h" // public

// FORWARD DECLARATIONS
class CSenIdentifier;
class MSenServiceDescription;
class CSenWSDescription;
class RCriticalSection;

// CLASS DECLARATION

/**
* Class for delivering SOAP request to WSP and getting back a response
*
*/       
class MSenProvider : public MSenHostlet
    {
    friend class CSenProvider;
    friend class CSenLocalRequest;

    public: 
        /**
        * Separate session initialization method, which is called when
        * some session of certain requester (service consumer) is "attached"
        * to this provider. In case of non-sharable provider, this happens
        * only once. Method performs some auto-configuration to that session,
        * describing the capabilities (facets), that are enabled in this 
        * provider implementation.
        *
        * @param aRequestor identifiers the caller of the service. This
        * service consumer owns the actual service session. Sharable
        * providers may wish to use this information in order to handle
        * multiple service-side sessions and to map incoming messages
        * to provider's internal state.
        *
        * @param aSession is a handle to the client side session being served.
        * Some providers may want to store some service specific information
        * into the session, which further can be utilized by service framework.
        * For example, the default implementation for this method typically
        * sets some facets into the session, which affect on messaging and
        * session handling; provider may indicate, that it is sharable, or
        * provider may say, that it wants complete messages instead of message
        * bodies by changing pre-defined facets on a session. Framework (session)
        * can then recognize these facets and change the way it talks to the provider.
        *
        * Typically, frameworks should call this method when they find provider
        * to be local, in order to fully initialize their client side session 
        * which represents the provided service.
        *
        * So, this method is performing "auto-configuration" for service sessions,
        * so that caller (session itself) can read these properties of the provider
        * from the XML SD (aSession). This way, a deployed provider does not need
        * to be followed by explicit Service Management API calls configuring, lets
        * say, whether a provider is exposed or not.
        */
        

        // virtual void InitServiceSessionL(MSenServiceDescription& aSession) = 0; // @deprecated: implement inherited MSenHostlet::ServiceDescriptionL instead(!)
    	// virtual CSenWSDescription* SessionDescriptionLC() = 0; // @deprecated: implement inherited MSenHostlet::ServiceDescriptionL instead(!)
    
        /**
        * Provider implementation, which wishes to explicitly expose it's
        * service to outside world (outbound requests from network) should
        * override this method and make it return ETrue. This would cause 
        * "expose" facet to be added to service session (SD), granting access 
        * from otherwise *trusted* requester (access control is done using policies).
        * Even trusted identities cannot access providers in the device, if 
        * they don't explicitly expose their service.
        *
        * @return boolean indicating whether this provider is exposed
        * to any trusted digital identity, which is making requests
        * from outside world.
        *
        * Default implementation typically returns EFalse, which means that
        * the provider is meant for local service consumers only.
        */
        virtual TBool Exposed() const = 0;
        
        /**
        * Provider implementation should override this method and set the
        * value to ETrue, if it implements capability to server several 
        * service consumers (sessions). 
        *
        * @return boolean indicating whether this provider is sharable
        * between multiple service consumers (sessions) simultaneously.
        * Corresponding facet is KProviderSharableFacet.
        *
        * Default implementation typically returns EFalse, which means
        * that provider is NOT shared between multiple consumers (sessions).
        */
        virtual TBool Sharable() const = 0;    
    
        /**
        * Provider implementation should override this method and set the
        * value to ETrue, if it wants to receive complete messages instead
        * of plain message bodies (example: full SOAP Envelope vs. SOAP Body).
        *
        * @return boolean indicating whether this provider wants to receive
        * complete messages from a client.
        * Corresponding facet is KProviderCompleteClientMsgsFacet.
        *
        * Default implementation typically returns EFalse, which means that
        * provider will receive the body of the message only.
        */
        virtual TBool CompleteClientMessages() const = 0;

        /**
        * Provider implementation should override this method and make it
        * return ETrue, if it wants to speed up the run time invocation
        * of this provider, performed by certain service consumer. Such
        * provider will be recognized and loaded when Core Service Manager
        * (WSF server process) starts up. Provider will be de-allocated
        * depending whether it is sharable and if it is multi-threaded or not
        * - which is similar to how providers behave in any other case, too.
        *
        * @return boolean indicating whether this provider is loaded on WSF
        * startup or not.
        * Corresponding facet is KProviderLoadOnStartupFacet.
        *
        * Default implementation typically returns EFalse, which means that
        * provider will not be loaded on WSF main process (Symbian server)
        * start-up.
        */
        virtual TBool LoadOnStartup() const = 0;

        /**
        * Provider implementation should override this method and make
        * it return EFalse, if it wants to implement multi-threaded,
        * non thread-safe  ServiceL. This typically means that such providers
        * will make use of RCriticalSection inside of their own implementation.
        *
        * If a provider is sharable *but not* thread-safe, it means, that
        * it is possible that multiple requests to ServiceL, from multiple 
        * consumers are executed simultaneously from separate threads.
        * It is strongly recommended to use critical sections inside the
        * provider plug-in implementation. This kind of sharable providers
        * might be serving under quite heave load. The requesters (threads)
        * should be identified by the information available in hostlet request
        * interface
        *
        * Then again, if provider is not sharable *but not* thread-safe,
        * it means that multiple request threads from *same service consumer* 
        * might execute ServiceL simultaneously. Provider implementation should
        * therefore offer thread-safe functionality inside ServiceL, possibly via
        * use of RCriticalSection instance, or through some other mutex.
        *
        * @see MSenHostletRequest to figure out ids for requesters / addressees
        *
        * @return boolean indicating whether this provider is thread-safe or not.
        * Corresponding facet is KProviderThreadsafeFacet. Default implementation
        * typically returns EFalse, which means that ServiceL of the provider will
        * not be called by several threads simultaneously. Threadsafe sharable 
        * providers have critical section surrounding ServiceL, which prevents 
        * more than one thread accessing the method simultaneously. Unsharable 
        * providers are typically loaded only to serve certain request, after 
        * which they are de-allocated. However, the non thread-safe unsharable 
        * providers allow multiple request threads of the *same* service consumer
        * to execute ServiceL simultaneously.
        */
        virtual TBool Threadsafe() const = 0;

        /** 
        * Provider implementation should override this method and make it 
        * return ETrue, if they are capable of returning their internal state
        * to one equal with state that they were right after original construction.
        *
        * @return ETrue, if ReinitL() will reset this provider, and EFalse, if
        * such re-init capability is not implemented (default). Corresponding
        * facet is KProviderReinitializableFacet.
        */
        virtual TBool Reinitializable() const = 0;

        /*
        * Provider implementation should override this method and make it
        * return ETrue, if they wish to keep any invoked instance on the
        * background, after the provider is released. Service consumer 
        * and it's thread executing the request typically release any unsharable
        * provider ECOM instance after response from such is received.
        *
        * @return ETrue, if provider wants to stay on background as long as WSF
        * main process thread is alive (WSF Core Service Manager). Default
        * implementation returns EFalse, which allows unloading and de-allocation
        * of any "released", non-used provider instances. Corresponding facet is
        * KProviderStayOnBackgroundFacet.
        */
        virtual TBool StayOnBackground() const = 0;
        
        /**
        * Re-init should be implemented by those Hostlets (providers), which
        * want to allow that instance to be re-used, that is resetted by the
        * invoker. For example, then some component responsible of loading of
        * the providers might be able to improve performance of slowly constructed
        * providers by calling this method, instead of de-allocating that instance
        * and making a ECOM lookup creating a similar, but new instance to same
        * service provider endpoint.
        * Implementations should thus reset any state variables that they have, 
        * and set themselves to the state in which they were right after first
        * construction. However, some performance benefits can be achieved if 
        * some class members can be treated as "stateless objects", which can
        * be re-used without possibly much slower re-instatiation.
        *
        * This method is a pair to Reinitializable() function, meaning that when
        * one develops such provider which supports reinitialization, that re-init 
        * should be implemented behind this ReinitL() method.
        *
        * Default implementation typically does nothing and returns KErrNotSupported,
        * since most of the providers don't support re-initialization.
        *
        * @return KErrNone if this provider instance is successfully reinitialized.
        *         Otherwise method returns some system-wide error code.
        */
        virtual TInt ReinitL() = 0;

        /**
        * Re-initializable or shared service providers, which instance is being called
        * from multiple threads may wish to override this method in order to act on this
        * signal and to properly initialize their thread specific components, like those 
        * which have Thread Local Store (TLS) based implementation.
        * 
        * If the method leaves, it is treaded equally as if the leave occured in the actual
        * ServiceL.
        */
        //virtual void ThreadInitL(const TDesC& aThreadId) = 0;

        /**
        * Re-initializable or shared service providers, which instance is being called
        * from multiple threads may wish to override this method in order to act on this
        * signal and to properly initialize their thread specific components, like those 
        * which have Thread Local Store (TLS) based implementation.
        */
        //virtual void ThreadDestroy(const TDesC& aThreadId) = 0;


    private:
        RCriticalSection iCriticalSection;
    };


#endif 

// End of File
