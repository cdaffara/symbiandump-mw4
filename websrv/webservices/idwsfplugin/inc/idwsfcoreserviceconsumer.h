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








#ifndef IDWSF_CORE_SERVICE_CONSUMER_H
#define IDWSF_CORE_SERVICE_CONSUMER_H

// INCLUDERS
#include <MSenServiceDescription.h>

#include "seninternalserviceconsumer.h"

// FORWARD DECLARATIONS
class CIdWsfServiceSession;
class MIdWsfSessionValidator;
class MSenRemoteHostlet;

// CLASS DECLARATION
class CIdWsfCoreServiceConsumer : public CSenInternalServiceConsumer
    {
    public: // Constructors and destructor
    
        static CIdWsfCoreServiceConsumer* NewL(CSIF& aSIF);
        static CIdWsfCoreServiceConsumer* NewLC(CSIF& aSIF);
        
        virtual ~CIdWsfCoreServiceConsumer();

        // New functions
    
        virtual TPtrC8 TrustAnchor() const;
        virtual TPtrC8 ProviderId() const;
        virtual void SetServiceSessionL(CIdWsfServiceSession* aSession);
        virtual const TTime& ValidUntilL();

        // Functions from base classes

        // From MSenRemoteServiceSession
        virtual TInt SetSessionL(MSenRemoteServiceSession& aSession);
        virtual void StartTransaction();
        virtual void TransactionCompleted();

        // From ServiceDescription
        virtual void SetContractL(const TDesC8& aContract);
        virtual void SetEndPointL(const TDesC8& aEndpoint);
        virtual TPtrC8 Contract();
        virtual TPtrC8 Endpoint();
        virtual TPtrC8 FrameworkId();
        virtual TPtrC8 FrameworkVersion();
        virtual TInt HasFacetL(const TDesC8& aURI, TBool& aHasFacet);
        virtual TInt FacetValue(TDesC8& aURI, HBufC8*& aValueTo);
        virtual TInt AddFacetL(const CSenFacet& aFacet);
        virtual TInt SetFacetL(const CSenFacet& aFacet);
        virtual TInt RemoveFacet(const TDesC8& aURI);
        virtual TInt FacetsL(RFacetArray& aFacetArray);
        virtual TInt ScoreMatchL(MSenServiceDescription& aPattern);
        virtual TBool Matches(MSenServiceDescription& aPattern);
        virtual void WriteAsXMLToL(RWriteStream& aWriteStream);
        virtual CSenServiceSession* ServiceSession();

        // for framework's internal use
        
        virtual CIdWsfServiceSession* IdWsfServiceSession();
        virtual void SetValidator(MIdWsfSessionValidator* aValidator);
        virtual MIdWsfSessionValidator* Validator();
        virtual MSenTransport& TransportL();
        virtual MSenRemoteServiceConsumer* ServedConsumer();
        virtual TInt ChunkByTxnIdL(TInt aTxnId, CSenChunk*& aChunk);
        virtual TBool HasSuperClass( TDescriptionClassType aType );

        virtual MSenRemoteHostlet* Hostlet() const;

    protected:
        
        /**
        * C++ default constructor.
        */
        CIdWsfCoreServiceConsumer(CSIF& aSIF, TDescriptionClassType aType);
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void BaseConstructL();

    protected: // Data
        CIdWsfServiceSession* iService; // not owned
        MSenRemoteServiceConsumer* iServedConsumer; // not owned
    };
    
#endif // IDWSF_CORE_SERVICE_CONSUMER_H

// End of File
