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








#ifndef IDWSF_AUTHENTICATION_SERVICE_CLIENT_H
#define IDWSF_AUTHENTICATION_SERVICE_CLIENT_H

// INCLUDES
#include <e32base.h>
#include <badesca.h>
#include <flogger.h>

#include "idwsfdiscoveryserviceclient.h"
#include "sensecuritymechanism.h"
#include "midwsfsessionvalidator.h"

// FORWARD DECLARATIONS
class MIdWsfSessionValidator;

// CLASS DECLARATION
class CIdWsfAuthenticationServiceClient : public CIdWsfDiscoveryServiceClient
    {
    public: // Constructors and destructor
    
        static CIdWsfAuthenticationServiceClient* NewL( CSIF& aSIF);//,
                                                        //RFileLogger& aLogger);
    
        static CIdWsfAuthenticationServiceClient* NewLC(CSIF& aSIF);//,
                                                        //RFileLogger& aLogger);

        ~CIdWsfAuthenticationServiceClient();
        
        // New functions
        
        virtual TInt ValidateL(CIdWsfServiceSession &aSession,
                               MSenRemoteServiceConsumer& aRemoteConsumer);

        virtual TInt ValidateL(CIdWsfServiceSession &aSession, const TDesC8& aAuthContext,
                               MSenRemoteServiceConsumer& aRemoteConsumer);

        void SetStatusL(const TInt aStatus);
        void SetAccount(CSenIdentityProvider* aIdentityProvicer);

        virtual TBool HasSuperClass( TDescriptionClassType aType );

		virtual void SetDataTrafficDetails( TSenDataTrafficDetails& aDetails) ; 			
    protected:
        
        /**
        * C++ default constructor.
        */
        CIdWsfAuthenticationServiceClient(  CSIF& aSIF,
                                        TDescriptionClassType aType);//,
                                        //RFileLogger& aLogger);
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void BaseConstructL();
        
        private: // New functions
        
            TInt ParseResponseL(TDesC8& aResponse, HBufC8*& aParsedMessage);

    private: // Data
        CSenSecurityMechanism* iAuthenticator;
        CIdWsfServiceSession* iDsSession; // Not owned
        CSenIdentityProvider* ipAccount; // Not owned
        TInt iAttempts;
        TInt iMaxAttempts;
        TSenDataTrafficDetails iAuthDetails; 
    };

#endif // IDWSF_AUTHENTICATION_SERVICE_CLIENT_H

// End of File
