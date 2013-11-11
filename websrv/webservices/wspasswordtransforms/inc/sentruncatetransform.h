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








#ifndef SEN_TRUNCATE_TRANSFORM_H
#define SEN_TRUNCATE_TRANSFORM_H

// INCLUDES
#include "senpasswordtransform.h"
#include "msencoreservicemanager.h"

// FORWARD DECLARATIONS
class MSenSaslMessage;

// CLASS DECLARATION
class CSenTruncateTransform : public CSenPasswordTransform
    {
    public: // Constructors and destructor
    
        static CSenTruncateTransform* NewL(
                                    MSenCoreServiceManager& aServiceManager);
        static CSenTruncateTransform* NewLC(
                                    MSenCoreServiceManager& aServiceManager);
        
        virtual ~CSenTruncateTransform();

        // Functions from base classes
        
        // From CSenPasswordTransform
        virtual TInt SetParameterL(TPtrC8 aName, TPtrC8 aValue);
        virtual HBufC8* TransformL(TPtrC8 aPassphrase) const;

    protected:
    
        /**
        * C++ default constructor.
        */
        CSenTruncateTransform(MSenCoreServiceManager& aServiceManager);

    private: // Data
        TInt iLength;
    };

#endif // SEN_TRUNCATE_TRANSFORM_H

// End of File
