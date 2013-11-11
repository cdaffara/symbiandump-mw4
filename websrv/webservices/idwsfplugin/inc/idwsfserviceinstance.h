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








#ifndef IDWSF_SERVICE_INSTANCE_H
#define IDWSF_SERVICE_INSTANCE_H

// INCLUDES
#include "senwsdescription.h"
#include "senidwsfplugin.h"

// FORWARD DECLARATION
class CSenCredential;

// CLASS DECLARATION
class CIdWsfServiceInstance : public CSenWSDescription
    {
    public:
        
        static CIdWsfServiceInstance* NewL();
        static CIdWsfServiceInstance* NewLC();
        static CIdWsfServiceInstance* NewL(const TDesC8& aUri);
        static CIdWsfServiceInstance* NewLC(const TDesC8& aUri);
        static CIdWsfServiceInstance* NewL(CIdWsfServiceInstance& aTemplate);
        static CIdWsfServiceInstance* NewLC(CIdWsfServiceInstance& aTemplate);

        virtual ~CIdWsfServiceInstance();

        // New functions
        
        virtual HBufC8* AsXmlL();
        virtual HBufC* AsXmlUnicodeL();
        virtual void WriteAsXMLToL(RWriteStream& aWriteStream);
        virtual TBool Matches(CIdWsfServiceInstance& aOtherServiceInstance);
        virtual TPtrC8 ProviderId();
        virtual TPtrC8 ResourceId();
        virtual TPtrC8 ServiceType();
        virtual TInt SetProviderIdL(const TDesC8& aURI);
        virtual void SetResourceIdL(const TDesC8& aURI);
        virtual void SetServiceTypeL(const TDesC8& aURI);
        virtual CDesC8Array& MechanismsL();
        virtual CDesC8Array& CredentialRefsL();
        virtual TBool SupportsMechanism(const TDesC8& aURI);
        CSenCredential* Credential();
        virtual void SetCredentialL(const TDesC8& aIdRef,
                                    const CSenCredential& aCredential);

        virtual void SetCredentialL(const CSenCredential& aCredential);
        virtual void AddSecurityMechL(const TDesC8& aUri);
        virtual void AddCredentialRefL(const TDesC8& aIdRef);

        // Functions from base classes

        // From MSenServiceDescription
        virtual TBool Matches(MSenServiceDescription& aPattern);
        virtual TPtrC8 Endpoint();
        virtual TPtrC8 Contract();
        virtual TInt HasFacetL(const TDesC8& aURI, TBool& aHasFacet);
        virtual TInt FacetValue(TDesC8& aURI, HBufC8*& aValueTo);
        virtual TInt AddFacetL(const CSenFacet& aFacet);
        virtual TInt SetFacetL(const CSenFacet& aFacet);
        virtual TInt RemoveFacet(const TDesC8& aURI);
        virtual TInt FacetsL(RFacetArray& aFacetArray);
        virtual TInt ScoreMatchL(MSenServiceDescription& aPattern);
        virtual TPtrC8 FrameworkId();
        virtual TPtrC8 FrameworkVersion();
        virtual void SetEndPointL(const TDesC8& aURI);
        virtual void SetContractL(const TDesC8& aContract);
        virtual TBool HasEqualPrimaryKeysL(MSenServiceDescription& aCandidate);
        virtual TBool HasSuperClass( TDescriptionClassType aType );

    private:
    
        /**
        * C++ default constructor.
        */
        CIdWsfServiceInstance();
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(const TDesC8& aUri, const TDesC8& aProviderId);

    private:
        HBufC8* ipProviderId;               // Owned
        HBufC8* ipEndpoint;                 // Owned
        HBufC8* ipServiceType;              // Owned
        HBufC8* ipResourceId;               // Owned
        CDesC8Array* ipMechanisms;          // Owned
        CDesC8Array* ipCredentialRefs;      // Owned
        RCredentialList iCredentialList;    // Owned
        RFacetArray iFacets;                // Owned
    };

#endif // IDWSF_SERVICE_INSTANCE_H

// End of File
