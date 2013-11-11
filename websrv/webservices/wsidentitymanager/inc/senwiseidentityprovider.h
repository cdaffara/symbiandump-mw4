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








#ifndef SEN_WISE_IDENTITY_PROVIDER_H
#define SEN_WISE_IDENTITY_PROVIDER_H

// INCLUDES
#include <SenIdentityProvider.h>

// CLASS DECLARATION
class CSenWiseIdentityProvider : public CSenIdentityProvider
    {
    public: // Constructors and destructor
    
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CSenWiseIdentityProvider* NewL( TPtrC8 aIMEI,
                                                        const TDesC8& aEndPoint,
                                                        const TDesC8& aContract);

        IMPORT_C static CSenWiseIdentityProvider* NewLC(TPtrC8 aIMEI,
                                                        const TDesC8& aEndPoint,
                                                        const TDesC8& aContract);
        /**
        * Default destructor.
        */
        IMPORT_C virtual ~CSenWiseIdentityProvider();

        // New functions
    
        /**
        * A getter.
        * @return a pointer to authorization id
        */
        virtual TPtrC8 AuthzID();

        /**
        * A getter.
        * @return a pointer to advisory authentication id
        */
        virtual TPtrC8 AdvisoryAuthnID();

        /**
        * A getter.
        * @return a pointer to password
        */
        virtual TPtrC8 Password();

    private:
    
        /**
        * C++ default constructor.
        */
        CSenWiseIdentityProvider(   TDescriptionClassType aType,
                                    TPtrC8 aIMEI);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(const TDesC8& aEndPoint,
                        const TDesC8& aContract);

    private: // Data
        TPtrC8 iIMEI;
    };

#endif // BASE_IDENTITY_MANAGER_H

// End of File
