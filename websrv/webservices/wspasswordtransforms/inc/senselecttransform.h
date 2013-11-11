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








#ifndef SEN_SELECT_PASSWORDTRANSFORM_H
#define SEN_SELECT_PASSWORDTRANSFORM_H

// INCLUDES
#include "senpasswordtransform.h"

// FORWARD DECLARATIONS
class MSenCoreServiceManager;

// CLAS DECLARATION
class CSenSelectTransform :  public CSenPasswordTransform
    {
    public: // Constructors and destructor
        
        static CSenSelectTransform* NewL(MSenCoreServiceManager& aServiceManager);
        static CSenSelectTransform* NewLC(MSenCoreServiceManager& aServiceManager);
        
        virtual ~CSenSelectTransform();

        // Functions from base classes
        
        // From CSenPasswordTransform
    
        /**
        * @return KErrNotSupported, if parameter name is unknown
        */
        virtual TInt SetParameterL(TPtrC8 aName, TPtrC8 aValue);
        
        virtual HBufC8* TransformL(TPtrC8 aPassphrase) const;

    protected:
    
        /**
        * C++ default constructor.
        */
        CSenSelectTransform(MSenCoreServiceManager& aServiceManager);
    
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        virtual void ConstructL();

    private: // Data
        HBufC8* ipSelect;
    };

#endif // SEN_SELECT_PASSWORDTRANSFORM_H

// End of File
