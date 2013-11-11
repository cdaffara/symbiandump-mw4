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








#ifndef IDWSF_DS_QUERY_RESPONSE_H
#define IDWSF_DS_QUERY_RESPONSE_H

// INCLUDES
#include <SenBaseFragment.h>

#include "idwsfresourceoffering.h"
#include "idwsfdiscoveryserviceclient.h" // Only constants needed

// CONSTANTS
const TInt KStateParsingResourceOffering    = 11;
const TInt KStateParsingEncryptedResourceId = 111;

_LIT8(KQueryResponseXmlns, "urn:liberty:disco:2003-08");
_LIT8(KQueryResponseName, "QueryResponse");
_LIT8(KQueryResponseQName, "ds:QueryResponse");
_LIT8(KStatusName, "Status");
_LIT8(KStatusCodeName, "code");

// CLASS DECLARATIONS
class CIdWsfDsQueryResponse :  public CSenBaseFragment
    {
    public: // Constructors and destructor
        
        static CIdWsfDsQueryResponse* NewL();
        static CIdWsfDsQueryResponse* NewLC();
        static CIdWsfDsQueryResponse* NewL(const TDesC8& aNsUri,
                                           const TDesC8& aLocalName,
                                           const TDesC8& aQName);
        static CIdWsfDsQueryResponse* NewLC(const TDesC8& aNsUri,
                                            const TDesC8& aLocalName,
                                            const TDesC8& aQName);
        virtual ~CIdWsfDsQueryResponse();

        // New functions
    
        /**
        * Ownership of array elements is retained.
        */
        virtual void GetAllServicesL(RPointerArray<CIdWsfServiceInstance>& aDest);
        virtual TBool IsOk();
        virtual TPtrC8 ResourceId();
        virtual RPointerArray<CIdWsfResourceOffering>& ResourceOfferingsL();

    protected:
        
        /**
        * C++ default constructor.
        */
        CIdWsfDsQueryResponse();
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void BaseConstructL();
        void BaseConstructL(const TDesC8& aNsUri,
                            const TDesC8& aLocalName,
                            const TDesC8& aQName);
        
        // Functions from base classes
        
        // From CSenBaseFragment
        virtual void StartElementL(const TDesC8& aNsUri,
                                   const TDesC8& aLocalName,
                                   const TDesC8& aQName,
                                   const RAttributeArray& aAttributes);
    
        virtual void EndElementL(const TDesC8& aNsUri,
                                 const TDesC8& aLocalName,
                                 const TDesC8& aQName);

    protected: // Data
        HBufC8* ipStatus;  // Owned

    private: // Data
        CIdWsfResourceOffering* ipResourceOffering;               // Owned
        RPointerArray<CIdWsfResourceOffering> iResourceOfferings; // Elements owned
        CSenCredential* ipCredential;                             // Owned
        CIdWsfServiceInstance* ipServiceInstance;                 // Owned
        TTime iNotOnOrAfter;
    };

#endif // IDWSF_DS_QUERY_RESPONSE_H

// End of File
