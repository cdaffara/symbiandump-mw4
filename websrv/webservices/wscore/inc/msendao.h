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








#ifndef M_SEN_DAO_H
#define M_SEN_DAO_H

//  INCLUDES
#include <flogger.h>
#include <e32std.h>

#include "msenserviceinvocationframework.h"
#include "msenservicemanager.h"
#include "msenremoteserviceconsumer.h"
#include "MSenServiceDescription.h"

// FORWARD DECLARATIONS
class CSIF;
class CSenServiceSession;
class CSenXMLDAO;
class CSenCoreServiceManager;

// CLASS DECLARATION
class MSenDAO
    {
    friend class CSenXMLDAO;             // to provide visibility to critical section
    friend class CSenCoreServiceManager; // to provide visibility to critical section

    public:
        virtual MSIF* Framework( const TDesC8& aFramework ) = 0;
        virtual TInt Add( CSenWSDescription& aDescription ) = 0;
        virtual TInt Add( CSIF& aFramework ) = 0;
        virtual TBool Remove( CSenWSDescription& aDescription ) = 0;
        virtual void Remove( CSIF& aFramework ) = 0;
        
        /**
        * The ownership of the returned ServiceDescription is NOT transferred 
        */
        virtual CSenWSDescription* FindMatchingServiceDescriptionL( CSenWSDescription& aSd ) = 0; //codescannerwarnings
                                           
        virtual TInt AddServiceDescriptionToFrameworksL( CSenWSDescription& aPattern,
                                                         MSenRemoteServiceConsumer& aRemoteConsumer,
                                                         HBufC8*& aErrorMsg ) = 0;
                     
        virtual TInt FindAllMatchingServiceDescriptions( RWSDescriptionArray& aMatches,
                                                         const TDesC8& aContract ) = 0;
                                            
        virtual TInt FindAllMatchingServiceDescriptions( RWSDescriptionArray& aMatches,
                                                         MSenServiceDescription& aPattern ) = 0;
                                        
        virtual TInt SaveL( const CSenServiceSession& aServiceSession ) = 0;
        virtual TInt SaveL( const CSIF& aServiceInvocationFramework ) = 0;
        virtual CDesC8Array& SupportedFrameworksL() = 0;
        virtual void Load() = 0;
        virtual TInt NotifyFrameworksL( const TDesC8& aFrameworkID,
                                        const TInt aEvent,
                                        TAny* aArgument) = 0;

        virtual TPtrC8 TransportPluginCueBySchemeL( const TPtrC8& aSchema ) = 0;

        /** 
        * Looks up CSenProvider ECOM plug-in. Sharable hostlets (providers)
        * may be found from a "proxy", a cache allowing them to be reused.
        */
        virtual MSenProvider& LookupHostletForL( const TDesC8& aHostletEndpoint,
                                                 const TDesC& aReqThreadId,
                                                 const TDesC8& aReqConsumerId ) = 0;

        /** 
        * Unsharable providers should be released from server (main) threads
        * memory by calling this method. 
        */
        virtual TInt ReleaseHostletL(const MSenProvider* aHostlet,
                                      const TDesC& aReqThreadId,
                                      const TDesC8& aReqConsumerId) = 0;

    private:
        virtual TInt Save() = 0;

        RCriticalSection iCriticalSection;
    };

#endif // M_SEN_DAO_H

// End of File
