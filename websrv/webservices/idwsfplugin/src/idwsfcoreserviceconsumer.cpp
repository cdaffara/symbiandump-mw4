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
* Description:        
*
*/








#include "idwsfcoreserviceconsumer.h"

#include "idwsfservicesession.h"
#include "sendebug.h" // internal Utils\inc - filelogging MACROs
#include "midwsfsessionvalidator.h"
#include "msenremotehostlet.h"
#include "senlogger.h"



CIdWsfCoreServiceConsumer* CIdWsfCoreServiceConsumer::NewL(CSIF& aSIF)
    {
    CIdWsfCoreServiceConsumer* self = CIdWsfCoreServiceConsumer::NewLC(aSIF);
    CleanupStack::Pop(self);
    return self;
    }

CIdWsfCoreServiceConsumer* CIdWsfCoreServiceConsumer::NewLC(CSIF& aSIF)
    {
    CIdWsfCoreServiceConsumer* self =
        new (ELeave) CIdWsfCoreServiceConsumer(
                                aSIF,
                                MSenServiceDescription::ECoreServiceConsumer);
    CleanupStack::PushL(self);
    self->BaseConstructL();
    return self;
    }

// constructor
CIdWsfCoreServiceConsumer::CIdWsfCoreServiceConsumer(
                                                 CSIF& aSIF,
                                                 TDescriptionClassType aType)
    : CSenInternalServiceConsumer(aType,aSIF),
    iService(NULL),
    iServedConsumer(NULL)
    {
    }

void CIdWsfCoreServiceConsumer::BaseConstructL()
    {
    CSenInternalServiceConsumer::ConstructL();
    }

// destructor
CIdWsfCoreServiceConsumer::~CIdWsfCoreServiceConsumer()
    {
    delete iService;
    }

// GETTERS

CSenServiceSession* CIdWsfCoreServiceConsumer::ServiceSession()
    {
    return iService;
    }

CIdWsfServiceSession* CIdWsfCoreServiceConsumer::IdWsfServiceSession()
    {
    return iService;
    }

MSenRemoteServiceConsumer* CIdWsfCoreServiceConsumer::ServedConsumer()
    {
    return iServedConsumer;
    }

TPtrC8 CIdWsfCoreServiceConsumer::TrustAnchor() const
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"CIdWsfCoreServiceConsumer::TrustAnchor");
    TPtrC8 trustAnchor = iService->TrustAnchor();

#ifdef _SENDEBUG
    if(trustAnchor.Length()>0)
        {
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,(trustAnchor));
        }
    else
        {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ," - trust anchor not set - ");
        }
#endif

    return trustAnchor;
    }

TPtrC8 CIdWsfCoreServiceConsumer::ProviderId() const
    {
    TPtrC8 providerId = iService->ProviderId();
    return providerId;
    }

TPtrC8 CIdWsfCoreServiceConsumer::Endpoint()
    {
    TPtrC8 endpoint = iService->Endpoint();
    return endpoint;
    }

TPtrC8 CIdWsfCoreServiceConsumer::Contract()
    {
    TPtrC8 contract =  iService->Contract();
    return contract;
    }

TPtrC8 CIdWsfCoreServiceConsumer::FrameworkId()
    {
    return iService->FrameworkId();
    }

const TTime& CIdWsfCoreServiceConsumer::ValidUntilL()
    {
    return iService->ValidUntilL();
    }


TPtrC8 CIdWsfCoreServiceConsumer::FrameworkVersion()
    {
    TPtrC8 fwVersion = iService->FrameworkVersion();

#ifdef _SENDEBUG
    if(fwVersion.Length()>0)
        {
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel ,(fwVersion));
        }
    else
        {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ," - framework version not set - ");
        }
#endif

    return fwVersion;
    }

void CIdWsfCoreServiceConsumer::WriteAsXMLToL(RWriteStream& aWriteStream)
    {
    iService->WriteAsXMLToL(aWriteStream);
    }


// SETTERS

// 2004-07-28: the ownership of the given session should be transferred in here
// note: current implementation leaves iService in NULL state, if such arg is
// given(!). But, it might be as well bad idea to preserve old iService
// (session) in case of NULL arg.
void CIdWsfCoreServiceConsumer::SetServiceSessionL( CIdWsfServiceSession* aSession )
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"CIdWsfCoreServiceConsumer::SetServiceSessionL");

	_LIT8(KTouch, "touch");
	
    const TDesC8* attrValue = aSession->AsElement().AttrValue(KTouch);
    if(attrValue != NULL)
    	{
    	AsElement().AddAttrL(KTouch, *attrValue);
    	}
    // free memory
    delete iService;
    iService = aSession;

    if(iService)
        {
        iService->AddConsumerL(*this);
        }
    }

TInt CIdWsfCoreServiceConsumer::SetSessionL(MSenRemoteServiceSession& /* aSession */)
    {
    return KErrNone;
    }

void CIdWsfCoreServiceConsumer::SetEndPointL(const TDesC8& aURI)
    {
    iService->SetEndPointL(aURI);
    }

void CIdWsfCoreServiceConsumer::SetContractL(const TDesC8& aContract)
    {
    iService->SetContractL(aContract);
    }

// CHECKERS
TBool CIdWsfCoreServiceConsumer::Matches(MSenServiceDescription& aPattern)
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"CIdWsfCoreServiceConsumer::Matches");
    TBool retVal = iService->Matches(aPattern);
#ifdef _SENDEBUG
    if(retVal)  TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,(_L("- TRUE")));
    else        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,(_L("- FALSE")));
#endif
    return retVal;
    }

TInt CIdWsfCoreServiceConsumer::HasFacetL(const TDesC8& aURI, TBool& aHasFacet)
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"CIdWsfCoreServiceConsumer::HasFacetL");
    TInt retVal = iService->HasFacetL(aURI, aHasFacet);
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel ,(aURI));
#ifdef _SENDEBUG
    if(aHasFacet)   TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,(_L("- TRUE")));
    else            TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,(_L("- FALSE")));
#endif
    return retVal;
    }

TInt CIdWsfCoreServiceConsumer::FacetValue(TDesC8& aURI, HBufC8*& aValueTo)
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"CIdWsfCoreServiceConsumer::FacetValue");
    TInt retVal = iService->FacetValue(aURI, aValueTo);
#ifdef _SENDEBUG
    if(aValueTo)
        {
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel ,(*aValueTo));
        }
#endif
    return retVal;
    }

TInt CIdWsfCoreServiceConsumer::AddFacetL(const CSenFacet& aFacet)
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"CIdWsfCoreServiceConsumer::AddFacetL");
    TInt retVal = iService->AddFacetL(aFacet);
    return retVal;
    }

TInt CIdWsfCoreServiceConsumer::SetFacetL(const CSenFacet& aFacet)
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"CIdWsfCoreServiceConsumer::SetFacetL");
    TInt retVal = iService->SetFacetL(aFacet);
    return retVal;
    }

TInt CIdWsfCoreServiceConsumer::FacetsL(RFacetArray& aFacetArray)
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"CIdWsfCoreServiceConsumer::FacetsL");
    TInt retVal = iService->FacetsL(aFacetArray);
    return retVal;
    }

TInt CIdWsfCoreServiceConsumer::RemoveFacet(const TDesC8& aURI)
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"CIdWsfCoreServiceConsumer::RemoveFacetL");
    TInt retVal = iService->RemoveFacet(aURI);
    return retVal;
    }

TInt CIdWsfCoreServiceConsumer::ScoreMatchL(MSenServiceDescription& aPattern)
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"CIdWsfCoreServiceConsumer::ScoreMatch");
    TInt retVal = iService->ScoreMatchL(aPattern);
    return retVal;
    }

void CIdWsfCoreServiceConsumer::StartTransaction()
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"CIdWsfCoreServiceConsumer::StartTransaction");
    iService->StartTransaction();
    }

void CIdWsfCoreServiceConsumer::TransactionCompleted()
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"CIdWsfCoreServiceConsumer::TransactionCompleted");
    iService->TransactionCompleted();
    }

    
void CIdWsfCoreServiceConsumer::SetValidator(MIdWsfSessionValidator* aValidator)
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"CIdWsfCoreServiceConsumer::SetValidator");
    iService->SetValidator(aValidator);
    }

MIdWsfSessionValidator* CIdWsfCoreServiceConsumer::Validator()
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"CIdWsfCoreServiceConsumer::Validator");
    return iService->Validator();
    }

MSenTransport& CIdWsfCoreServiceConsumer::TransportL()
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"CIdWsfCoreServiceConsumer::TransportL");

    if(!iServedConsumer)
        {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"- Fatal error: served remote consumer is NULL!");
        User::Leave(KErrNotFound);
        }
    return iServedConsumer->TransportL();
    }

TInt CIdWsfCoreServiceConsumer::ChunkByTxnIdL(TInt aTxnId, CSenChunk*& aChunk)
    {
    if(iServedConsumer)
        {
        return iServedConsumer->ChunkByTxnIdL(aTxnId, aChunk);
        }
    else
        {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"CIdWsfCoreServiceConsumer::TransportL:");
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"- Consumer not found!");
        return KErrNotFound;
        }
    }

TBool CIdWsfCoreServiceConsumer::HasSuperClass( TDescriptionClassType aType )
    {
    if( aType == CSenInternalServiceConsumer::DescriptionClassType() ) // direct superclass!
        {
        // If asked type is the know *direct* father/mother, return true:
        return ETrue; 
        } 
    else
        {
        // Otherwise, ask from superclass (chain, recursively)
        return CSenInternalServiceConsumer::HasSuperClass( aType ); 
        }
    }


MSenRemoteHostlet* CIdWsfCoreServiceConsumer::Hostlet() const
    {
    MSenRemoteHostlet* pHostlet = NULL;
    if( iService )
        {
        pHostlet = iService->Hostlet();
        }
    return pHostlet;
    }


// End of file


