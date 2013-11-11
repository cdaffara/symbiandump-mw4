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








#ifndef IDWSF_RESOURCE_OFFERING_H
#define IDWSF_RESOURCE_OFFERING_H

// INCLUDES
#include <e32base.h>
#include <SenXmlUtils.h>

#include "idwsfserviceinstance.h"

// FORWARD DECLARATION
class CSenCredential;

// CLASS DECLARATION
class CIdWsfResourceOffering :  public CBase
    {
    public: // Constructors and destructor
        
        static CIdWsfResourceOffering* NewL();
        virtual ~CIdWsfResourceOffering();

        // New functions
        
        TPtrC8 ResourceId();
        void SetResourceIdL(const TDesC8& aId);
        CIdWsfServiceInstance& LastServiceL();

        /**
        * Takes ownership of aInstance
        */
        void AddServiceInstanceL(CIdWsfServiceInstance* aInstance);
        void AddCredentialL(const CSenCredential& aCredential);

        RPointerArray<CIdWsfServiceInstance>& ServicesL();
        TInt SetFacetL(CSenFacet& aFacet);

    private:
    
         /**
        * C++ default constructor.
        */
        CIdWsfResourceOffering();
    
        /**
        * By default Symbian 2nd phase constructor is private.
        */  
        void ConstructL();

    private: // Data
        HBufC8* ipResourceId;                            // Owned
        RPointerArray<CIdWsfServiceInstance> iServices; // Elements owned
    };

#endif // IDWSF_RESOURCE_OFFERING_H

// End of File

