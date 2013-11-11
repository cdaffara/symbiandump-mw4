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








#ifndef IDWSF_SASL_MECHANISM_H
#define IDWSF_SASL_MECHANISM_H

// INCLUDES
#include <e32base.h>

#include "MSenCoreServiceManager.h"

// FORWARD DECLARATIONS
class CIdWsfSaslMessage;

// CLASS DECLARATION
class CIdWsfSaslMechanism : public CBase
    {
    public:
        
        /**
        *  "Owner" of the mechanism that supports credentials, etc. Typically
        *  the IdWsfClient implements this interface.
        */
        class MOwner
            {
            public:
                /**
                * @return the name of the user, in case there is one, e.g. when
                *            multiple users use this device.
                */
                virtual const TDesC& SaslAuthzIdL() { return KNullDesC(); }

                /**
                * @return depending on the IdP this could be a deviceID.
                */
                virtual const TDesC& SaslAdvisoryAuthnIdL() { return KNullDesC(); }

                virtual const TDesC& SaslAuthnIdL() { return KNullDesC(); }

                /**
                * Ask password from the user or identity manager.
                */
                virtual const TDesC& SaslPasswordL() { return KNullDesC(); }
            };

    public:
        
        // New functions
        
        /**
        * @return the name of the mechanism, like "PLAIN".
        */
        virtual const TDesC& Name() = 0;

        /**
        * @return a SASL request that is a response to the passed response.
        *      Caller takes ownership of the returned object. Never returns NULL.
        */
        virtual CIdWsfSaslMessage* HandleResponseL(MOwner& aOwner,
                                                   CIdWsfSaslMessage& aResponse) = 0;

    protected:
    
        /**
        * C++ default constructor.
        */
        CIdWsfSaslMechanism(MSenCoreServiceManager& aServiceManager)
        :iServiceManager(aServiceManager)
        {
        }

    protected: // Data
        // to access base64 encoding/decoding
        MSenCoreServiceManager& iServiceManager; 
    };

#endif // IDWSF_SASL_MECHANISM_H

// End of File

