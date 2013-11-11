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








#ifndef SEN_SASL_MESSAGE_H
#define SEN_SASL_MESSAGE_H

// INCLUDES
#include <MSenFragment.h>

#include "msencoreservicemanager.h"
#include "senpasswordtransform.h"

// CLASS DECLARATION
class MSenSaslMessage 
    {
    public:
        
        // New functions
        
        /**
         *  Accepts KNullDesC.
         */
        virtual TInt ConstructInitRequestFromL(const TDesC8& aAllMechanismNames,
                                               CSenIdentityProvider& aAccount) = 0;

        virtual TInt ConstructRequestFromL(const TDesC8& aMech) = 0;
        virtual TInt ConstructRequestFromL(const TDesC8& aMech,
                                           const TDesC8& aData) = 0;

        virtual TBool IsContinue() = 0;
        virtual TBool IsAbort() = 0;

        /**
         *  @return KNullDesC if not set.
         */
         virtual TPtrC8 Mechanism() = 0;

        /**
         *  @return KNullDesC if not set.
         */
        virtual TPtrC8 ParameterName() = 0;

        /**
         *  @return KNullDesC if not set.
         */
        virtual TPtrC8 Data() = 0;
        virtual RPointerArray<CSenPasswordTransform> Transforms() = 0;
    };

#endif // SEN_SASL_MESSAGE_H

// End of File
