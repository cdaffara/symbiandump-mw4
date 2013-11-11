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








#ifndef SEN_LOWERCASE_PASSWORDTRANSFORM_H
#define SEN_LOWERCASE_PASSWORDTRANSFORM_H

// INCLUDES
#include "senpasswordtransform.h"

// FORWARD DECLARATIONS
class MSenCoreServiceManager;

// CLASS DECLARATION
class CSenLowercaseTransform : public CSenPasswordTransform
    {
    public: // Constructors and destructor
    
        static CSenLowercaseTransform* NewL(
                                    MSenCoreServiceManager& aServiceManager);
        static CSenLowercaseTransform* NewLC(
                                    MSenCoreServiceManager& aServiceManager);
        
        virtual ~CSenLowercaseTransform();

        // Functions from base classes
        
        // From CSenPasswordTransform
        
        /**
        * method does nothing
        */
        virtual TInt SetParameterL(TPtrC8 aName, TPtrC8 aValue);
        
        virtual HBufC8* TransformL(TPtrC8 aPassphrase) const;

    protected:

        /**
        * C++ default constructor.
        */
        CSenLowercaseTransform(MSenCoreServiceManager& aServiceManager);
    };

#endif // SEN_LOWERCASE_PASSWORDTRANSFORM_H

// End of File
