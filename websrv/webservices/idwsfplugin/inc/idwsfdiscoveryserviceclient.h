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








#ifndef IDWSF_DISCOVERY_SERVICE_CLIENT
#define IDWSF_DISCOVERY_SERVICE_CLIENT

// INCLUDES
#include <e32base.h>
#include <badesca.h>
#include <flogger.h>

#include "midwsfsessionvalidator.h"

#include "idwsfcoreserviceconsumer.h"


// CONSTANTS 
_LIT8(KDiscoOption,"urn:com.nokia.serene.idwsf:disco:option");

// FORWARD DECLARATIONS
class CIdWsfServiceSession;
class CIdWsfDsQueryResponse;

// DATA TYPES
typedef RPointerArray<CIdWsfServiceSession> RSessionArray;

// CLASS DECLARATION
class CIdWsfDiscoveryServiceClient : public CIdWsfCoreServiceConsumer, 
                                     public MIdWsfSessionValidator
{
    public: // Constructors and destructor
    
        static CIdWsfDiscoveryServiceClient* NewL(CSIF& aSIF);//,
                                                  //RFileLogger& aLogger);

        static CIdWsfDiscoveryServiceClient* NewLC(CSIF& aSIF);//,
                                                   //RFileLogger& aLogger);

        ~CIdWsfDiscoveryServiceClient();

        // New functions
        
        /**
        * @return KErrNotFound, if no known security mechanism was found
        *         KErrNotReady, if no service session has been initialized
        */
        virtual TInt ValidateL(CIdWsfServiceSession& aSession,
                               MSenRemoteServiceConsumer& aRemoteConsumer);

        virtual void SetStatusL(const TInt aStatus);

        /**
        * @return KErrNotFound, if no known security mechanism was found
        */
        virtual TInt FindServiceL(RPointerArray<CSenWSDescription>& aServiceArray, 
                                  MSenServiceDescription& aPattern,
                                  MSenRemoteServiceConsumer& aRemoteConsumer);

        static TInt DSOptionsL(RFacetArray& aOptionArray,
                               MSenServiceDescription& aServiceDescription);

        virtual TBool HasSuperClass( TDescriptionClassType aType );
		virtual void SetDataTrafficDetails( TSenDataTrafficDetails& aDetails) ; 		

    protected:
        
        /**
        * C++ default constructor.
        */
        CIdWsfDiscoveryServiceClient(CSIF& aSIF, TDescriptionClassType aType);//RFileLogger& aLogger);
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void BaseConstructL();

        // New functions
        
        /**
        * @return KErrNotFound, if no known security mechanism was found
        */
        virtual TInt ProcessResponseL(CIdWsfDsQueryResponse* aResponse);
        //RFileLogger* Log() const;
        
    private: // New functions
    
        HBufC8* CIdWsfDiscoveryServiceClient::DsLookupRequestL(MSenServiceDescription& aPattern);
        
        CIdWsfDsQueryResponse* ParseResponseLC(TDesC8& aMessage);
        //void SetStatusL(TInt aStatus);

    protected: // Data
        enum TClientState
        {
        ENotInitialized = 1,
        EHasSession,
        EHasResults,
        EAuthServiceClient,
        ESaslMechanism
        };

        TClientState iCState;
        CIdWsfServiceSession* iSession; // Not owned

    private: // Data
        //RFileLogger& iLog;
        RSessionArray iResults;
        TSenDataTrafficDetails iDiscDetails;
    };

#endif //IDWSF_DISCOVERY_SERVICE_CLIENT

// End of File
