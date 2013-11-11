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








#ifndef IDWSF_SERVICE_PROVIDER_H
#define IDWSF_SERVICE_PROVIDER_H

// INCLUDES
#include <e32base.h>

// CLASS DECLARATION
class CIdWsfServiceProvider : public CBase
    {
    public: // Constructors and destructor
    
        static CIdWsfServiceProvider* NewL(const TDesC8& aProviderID = KNullDesC8,
                                           const TDesC8& aServiceURL = KNullDesC8);

        static CIdWsfServiceProvider* NewLC(const TDesC8& aProviderID = KNullDesC8,
                                            const TDesC8& aServiceURL = KNullDesC8);
        virtual ~CIdWsfServiceProvider();

        // New functions
        
        TInt SetProviderIdL(const TDesC8& aProviderID);
        TInt SetEndPointL(const TDesC8& aServiceURL);
        TPtrC8 ProviderId();
        TPtrC8 Endpoint();
        TBool Equals(const TDesC8& aProviderID);

    private:
        
        /**
        * C++ default constructor.
        */
        CIdWsfServiceProvider();
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(const TDesC8& aProviderID, const TDesC8& aEndpoint);

    private: // Data
        HBufC8* ipProviderId;   // Owned
        HBufC8* ipEndpoint;     // Owned
    };

#endif // IDWSF_SERVICE_PROVIDER_H

// End of File
