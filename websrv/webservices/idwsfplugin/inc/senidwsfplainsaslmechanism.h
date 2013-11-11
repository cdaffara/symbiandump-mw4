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








#ifndef SEN_IDWSF_PLAIN_SASL_MECHANISM_H
#define SEN_IDWSF_PLAIN_SASL_MECHANISM_H

// INCLUDES
#include "SenSecurityMechanism.h"

// FORWARD DECLARATIONS
class MSenCoreServiceManager;

// CLASS DECLARATION
class CSenIdWsfPlainSaslMechanism : public CSenSecurityMechanism
    {
    public: // Constructors and destructor
        
        static CSenIdWsfPlainSaslMechanism* NewL(
                                    MSenCoreServiceManager& aServiceManager);
        static CSenIdWsfPlainSaslMechanism* NewLC(
                                    MSenCoreServiceManager& aServiceManager);
        virtual ~CSenIdWsfPlainSaslMechanism();

        // Functions from base classes
        
        // From CSaslMechanism
        virtual void SetIdentity(   CSenIdentityProvider* aIdentityProvider,
                                    MSenIdentityManager* aIdentityManager);

        virtual TBool IsPasswordFromUser();
        virtual void ClearPasswordL();
        virtual TInt MaxAttempts();

        // From CSenSecurityMechanism
        virtual const TDesC8& Name();
        virtual TInt HandleResponseL(MSenSaslMessage& aResponse,
                                     MSenSaslMessage& aNewRequest);

    protected:
        
        /**
        * C++ default constructor.
        */
        CSenIdWsfPlainSaslMechanism(MSenCoreServiceManager& aServiceManager);
    
    private: // New functions
        
        /**
        * Leaving version of the method
        */
        void SetIdentityL(  CSenIdentityProvider* aIdentityProvider,
                            MSenIdentityManager* aIdentityManager);

    protected: // Data
        HBufC8* ipPassword;
    };

#endif // IDWSF_PLAIN_SASL_MECHANISM_H

// End of File