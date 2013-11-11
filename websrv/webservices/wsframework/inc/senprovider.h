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








#ifndef SEN_PROVIDER_H
#define SEN_PROVIDER_H

//  INCLUDES
#include <ecom/ecom.h>
#include <e32std.h>   // for RCriticalSection

#include <MSenServiceDescription.h>
#include "senwsdescription.h"
#include <SenFacet.h>

#include "senidentifier.h" // internal Utils\inc
#include "msenprovider.h" // internal Framework\inc

// FORWARD DECLARE
class CSenLocalRequest;

// CLASS DECLARATION

/**
* Class for delivering SOAP request to Serene and response back.
* Sub-classing actual Service Providers have to implement MSenProvider
* interface.
*
*/       
class CSenProvider : public CBase, public MSenProvider
    {
    public: // Constructors and destructor
            /**
            * Two-phased constructor.
        */
        static CSenProvider* NewL(const TDesC8& aMatch);
        
        static CSenProvider* NewL();
        
        /**
        * Function:    ~CSenProvider
        *
        * Description: Destroy the object
        */
        virtual ~CSenProvider();

        // From MSenHostlet:
        /** 
        * Default implementation returns the ECOM cue as endpoint of this local provider.
        * @return ECOM cue of this plug-in implementation
        */
        inline virtual TPtrC8 Endpoint() const;

        /**
    	* Method provides the default implementation of adding facets in the SD after 
    	* checking the  default values of the provider properties (exposed, shareable, 
    	* complete client message etc). Each overriden default facet value is added
    	* to the service description (SD).
        *
        * @param aSD is the service description into which all the facets of this
        * hostlet (local service provider) will be added. 
        *
        * Method provides means of "auto-configuration" for service session creation.
        * The sessions themself can read properties of the provider from the altered
        * XML service description. This way, provider deployment does not need to be 
        * followed by explicit Service Management API calls (configuration) to add 
        * these facet.
    	*/
        inline virtual void DescribeServiceL(CSenWSDescription& aSD);
        
        /**
        * This default, inline version returns EFalse. Any service exposed to outside world should
        * override to return ETrue instead. That would cause inline method 
        * InitServiceSession to add corresponding facet to the session.
        * @see MSenProvider interface
        */
        inline virtual TBool Exposed() const;


        /**
        * This default, inline version version returns EFalse. Sharable, sub-classing providers need 
        * to override this to return ETrue instead, if they allow multiple service consumers to have
        * queued access to the ServiceL() of the same provider instance.
        * @see MSenProvider interface
        */
        inline virtual TBool Sharable() const;

        /**
        * This default, inline version version returns EFalse. Providers, which wish to receive
        * complete messages (with header information) need to override this to return ETrue instead.
        * @see MSenProvider interface
        */
        inline virtual TBool CompleteClientMessages() const;


        /**
        * This default, inline version version returns EFalse. Providers, which have slow construction
        * mechanism should make this return ETrue, to make WSF Core Service Manager to pre-load them
        * after that (Symbian server) process is created and sensessions.xml database is being loaded.
        * @see MSenProvider interface
        */
        inline virtual TBool LoadOnStartup() const;

        /**
        * This default, inline version version returns ETrue. Providers, which wish to permit
        * un-thread-safe access to their ServiceL need to make this return EFalse instead.
        * @see MSenProvider interface
        */
        inline virtual TBool Threadsafe() const;

        /**
        * This default, inline version version returns EFalse. Providers, which allow themselves
        * to be re-cycled (reused) need to make this return EFalse. 
        * @see MSenProvider interface
        */
        inline virtual TBool Reinitializable() const;

        /**
        * This default, inline version version returns EFalse. Providers, which want themselves
        * to be left on background as long as main thread (Core Service Manager) is alive, need
        * to make this return ETrue instead.
        * @see MSenProvider interface
        */
        inline virtual TBool StayOnBackground() const;

        /** 
        * Default implementation does nothing, and
        * returns KErrNotSupported.
        * Subclasses should override this method in
        * order to re-initialize any state variables
        * that are not related to exact construction
        * phase; for example, some provider might
        * reset any state data it has, but still it
        * might decide to keep negotiated client
        * (connection) to some server open, making
        * just corresponding "re-init" to that service
        * being internally consumed by that provider.
        *
        * Typically, this method is a pair to Reinitializable()
        * function, meaning that when one develops such provider 
        * which supports reinitialization, that re-init should be 
        * implemented behind this ReinitL() method.
        *
        * @see MSenProvider
        */
        inline virtual TInt ReinitL();

        /**
        * Default implementation does nothing. Subclasses should override this.
        */
        //inline virtual void ThreadInitL(const TDesC& /* aThreadId */) { ; }

        /**
        * Default implementation does nothing. Subclasses should override this.
        */
        //inline virtual void ThreadDestroy(const TDesC& /* aThreadId */) { ; }

    private:
        /** iDtor_ID_Key Instance identifier key. When instance of an
        *               implementation is created by ECOM framework, the
        *               framework will assign UID for it. The UID is used in
        *               destructor to notify framework that this instance is
        *               being destroyed and resources can be released.
        */
        TUid iDtor_ID_Key;

        HBufC8* iCue;
        
        TAny* _reserved;
        TAny* _reserved2;
    };

#include "senprovider.inl"

#endif 

// End of File
