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








#ifndef SEN_UPPERCASE_PASSWORDTRANSFORM_H
#define SEN_UPPERCASE_PASSWORDTRANSFORM_H

// INCLUDES
#include "senpasswordtransform.h"

// FORWARD DECLARATIONS
class MSenCoreServiceManager;

// CLASS DECLARATION
class CSenUppercaseTransform : public CSenPasswordTransform
    {
    public: // Constructors and destructor
        
        static CSenUppercaseTransform* NewL(
                                    MSenCoreServiceManager& aServiceManager);
        static CSenUppercaseTransform* NewLC(
                                    MSenCoreServiceManager& aServiceManager);
        
        virtual ~CSenUppercaseTransform();

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
        CSenUppercaseTransform(MSenCoreServiceManager& aServiceManager);
    };

#endif // SEN_UPPERCASE_PASSWORDTRANSFORM_H

// End of File
