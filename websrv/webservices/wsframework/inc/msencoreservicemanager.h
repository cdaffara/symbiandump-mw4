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








#ifndef M_SEN_CORE_SERVICE_MANAGER_H
#define M_SEN_CORE_SERVICE_MANAGER_H

//  INCLUDES
#include <e32base.h>
#include <e32std.h>
#include <flogger.h>
#include <badesca.h>    //CDesC8Array

#include "msenservicemanager.h"
#include "msenidentitymanager.h"
#include "msencredentialmanager.h"

// FORWARD DECLARATIONS
class MSenTransport;
class MSenProvider;

class CSenXmlReader;
class CSenWSDescription;
class CSenIdentityProvider;
class CSenServiceSession;
class CSenIdentifier;
class CSIF;

// CLASS DECLARATION

/**
*
* MSenCoreServiceManager represents the internal (core) side interface of
* a CoreServiceManager
* The methods defined in this interface are executed from MDAO implementations
* and from different frameworks, which are expected to operate in the same
* process as the CoreServiceManager.
*/
class MSenCoreServiceManager :  public MSenServiceManager,
                                public MSenIdentityManager,
                                public MSenCredentialManager
    {
    public: // New methods

        virtual TInt AddServiceDescriptionL( CSenWSDescription* apServiceDescription ) = 0;

        virtual TInt RemoveServiceDescriptionL( CSenWSDescription& aServiceDescription ) = 0;

        virtual TInt SaveL( const CSenServiceSession& aServiceSession ) = 0;
        
        virtual TInt SaveL( const CSIF& aServiceInvocationFramework ) = 0;

        //virtual MSenIdentityManager& IdentityManager() = 0;

        /**
        * Notifies *ALL* framework plug-ins about certain event
        * @return some system-wide error code, if an error occurred
        */
        //virtual TInt NotifyFrameworksL(const TInt aEvent) = 0;

        /**
        * Notifies *ALL* framework plug-ins about certain event and
        * an additional object pointer may be passed
        * NULL is also accepted
        * @return some system-wide error code, if an error occurred
        */
        //virtual TInt NotifyFrameworksL(const TInt aEvent, 
        //                               TAny* aArgument) = 0;


        /**
        * Notifies only those frameworks plug-ins, which match
        * with given framework ID (aFrameworkID)
        * Note(!): if aFrameworkID == KNullDesC8 (zero-length),
        * then *all* known framework plug-ins are notified
        * about the event.
        * @return some system-wide error code, if an error occurred
        */
        //virtual TInt NotifyFrameworksL(const TDesC8& aFrameworkID,
        //                               const TInt aEvent) = 0;

        /**
        * Notifies only those frameworks plug-ins, which match
        * with given framework ID (aFrameworkID) and 
        * an additional object pointer may be passed
        * NULL is also accepted

        * Note(!): if aFrameworkID == KNullDesC8 (zero-length),
        * then *all* known framework plug-ins are notified
        * about the event.
        * @return some system-wide error code, if an error occurred
        */
        //virtual TInt NotifyFrameworksL(const TDesC8& aFrameworkID,
        //                              const TInt aEvent,
        //                              TAny* aArgument) = 0;

        /**
         * Return a list with FrameworkId strings (TDesC&), 
         * each a name of a supported ServiceInvocationFramework.
         * This may help applications, to decide what to ask for.
         */
        virtual CDesC8Array& SupportedFrameworksL() = 0;

        /**
        * Method for requesting a MSenProvider reference
        * to a hostlet (provider) accessible behind given
        * endpoint. Core service manager implementation 
        * typically owns the instances behind this reference,
        * as well as it often does some pooling / caching / 
        * proxy-ing of these instances for several requesters.
        * Typical caller of this method is transport plug-in.
        * @param aHostletEndpoint is the endpoint of requested provider.
        * @param aReqThreadId is the full name of the requesting thread
        * @param aReqConsumerId is the unique id (urn) of the requesting
        *        service consumer
        * @return reference to found provider. If provider was not
        * found from the system, this method will leave with
        * KErrNotFound error code. Other leave codes are system-wide.
        */
        virtual MSenProvider& LookupHostletForL( const TDesC8& aHostletEndpoint,
                                                 const TDesC& aReqThreadId,
                                                 const TDesC8& aReqConsumerId ) = 0;

        /** 
        * Unsharable hostlets (providers) should be released from server (main)
        * thread's memory by calling this method. 
        * @param aHostlet is a pointer to the hostlet (provider) being released,
        *        and possibly de-allocated.
        * @param aReqThreadId is the full name of the requesting thread
        * @param aReqConsumerId is the unique id (urn) of the requesting
        *        service consumer
        * @return KErrNone on success or otherwise some system-wide error code.
        */
        virtual TInt ReleaseHostletL( const MSenProvider* aHostlet,
                                      const TDesC& aReqThreadId,
                                      const TDesC8& aReqConsumerId ) = 0;
        virtual TPtrC8 IMSI() = 0;
        
        virtual TPtrC8 IMEI() = 0;

    };

#endif // M_SEN_CORE_SERVICE_MANAGER_H

// End of File
