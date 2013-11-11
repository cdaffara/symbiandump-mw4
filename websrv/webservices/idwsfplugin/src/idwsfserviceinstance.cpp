/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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








#include <SenXmlUtils.h>
#include <SenCredential.h>
#include <SenFacet.h>
#include <SenServiceConnection.h>
#include <SenXmlElement.h>

#include "idwsfserviceinstance.h"

namespace
    {
    const TInt KFLATBUF_SIZE = 128;
    }

CIdWsfServiceInstance* CIdWsfServiceInstance::NewL()
    {
    CIdWsfServiceInstance* pNew = CIdWsfServiceInstance::NewLC();
    CleanupStack::Pop(); // pNew
    return pNew;
    }

CIdWsfServiceInstance* CIdWsfServiceInstance::NewLC()
    {
    CIdWsfServiceInstance* pNew = new (ELeave) CIdWsfServiceInstance;
    CleanupStack::PushL(pNew);
    pNew->ConstructL(KNullDesC8(), KNullDesC8());
    return pNew;
    }

CIdWsfServiceInstance* CIdWsfServiceInstance::NewL(const TDesC8& aUri)
    {
    CIdWsfServiceInstance* pNew = CIdWsfServiceInstance::NewLC(aUri);
    CleanupStack::Pop(); // pNew
    return pNew;
    }

CIdWsfServiceInstance* CIdWsfServiceInstance::NewLC(const TDesC8& aUri)
    {
    CIdWsfServiceInstance* pNew = new (ELeave) CIdWsfServiceInstance;
    CleanupStack::PushL(pNew);
    pNew->ConstructL(aUri, KNullDesC8());
    return pNew;
    }

CIdWsfServiceInstance* CIdWsfServiceInstance::NewL(
                                            CIdWsfServiceInstance& aTemplate)
    {
    CIdWsfServiceInstance* pNew = CIdWsfServiceInstance::NewLC(aTemplate);
    CleanupStack::Pop(); // pNew
    return pNew;
    }

CIdWsfServiceInstance* CIdWsfServiceInstance::NewLC(
                                            CIdWsfServiceInstance& aTemplate)
    {
    CIdWsfServiceInstance* pNew = new (ELeave) CIdWsfServiceInstance;
    CleanupStack::PushL(pNew);
    pNew->ConstructL(aTemplate.ServiceType(), aTemplate.ProviderId());
    return pNew;
    }

CIdWsfServiceInstance::CIdWsfServiceInstance() :
    CSenWSDescription( EIdWsfServiceInstance ),
    ipProviderId( NULL ),
    ipEndpoint( NULL ),
    ipServiceType( NULL ),
    ipResourceId( NULL ),
    ipMechanisms( NULL ),
    ipCredentialRefs( NULL )
    {
    }
    
void CIdWsfServiceInstance::ConstructL(const TDesC8& aUri,
                                       const TDesC8& aProviderId)
    {
    CSenDomFragment::BaseConstructL(NewElementName());

    HBufC8* pNew = NULL;
    if (aUri != KNullDesC8)
        {
        pNew = aUri.AllocL();
        }
    ipServiceType = pNew;
    SetProviderIdL(aProviderId);
    }

CIdWsfServiceInstance::~CIdWsfServiceInstance()
    {
    delete ipProviderId;
    delete ipResourceId;
    delete ipEndpoint;

    delete ipServiceType;
    delete ipMechanisms;
    delete ipCredentialRefs;

    iCredentialList.ResetAndDestroy();
    iFacets.ResetAndDestroy();
    }

TBool CIdWsfServiceInstance::Matches(
                            MSenServiceDescription& aOtherServiceDescription)
    {
    TPtrC8 patternEndpoint = aOtherServiceDescription.Endpoint();
    TPtrC8 thisEndpoint = Endpoint();

    if(patternEndpoint.Length()>0)
        {
        if(!(thisEndpoint.Length()>0 && patternEndpoint == thisEndpoint))
            {
            return EFalse;
            }
        }

    TPtrC8 patternContract = aOtherServiceDescription.Contract();
    TPtrC8 thisContract = Contract();

    if(patternContract.Length()>0)
        {
        if(!(thisContract.Length()>0 && patternContract == thisContract))
            {
            return EFalse;
            }
        }
    return ETrue;
    }

TBool CIdWsfServiceInstance::Matches(
                                CIdWsfServiceInstance& aOtherServiceInstance)
    {
    if (!Matches((MSenServiceDescription&)aOtherServiceInstance)) return EFalse;

    TPtrC8 patternProvider = aOtherServiceInstance.ProviderId();
    TPtrC8 thisProvider = ProviderId();

    if(patternProvider.Length()>0)
        {
        if(!(thisProvider.Length()>0 && patternProvider == thisProvider))
            return EFalse;
        }
    return ETrue;
    }


TPtrC8 CIdWsfServiceInstance::Contract()
    {
    if (ipServiceType)
        return *ipServiceType;
    else
        return KNullDesC8();
    }

TPtrC8 CIdWsfServiceInstance::FrameworkId()
    {
    return KDefaultIdWsfFrameworkID();
    }

TPtrC8 CIdWsfServiceInstance::FrameworkVersion()
    {
    return KIdWsfFrameworkVersion();
    }

void CIdWsfServiceInstance::SetContractL(const TDesC8& aContract)
    {
    HBufC8* pNew = NULL;
    if(aContract.Length()>0)
        pNew = aContract.AllocL();

    delete ipServiceType;
    ipServiceType = pNew;
    }

void  CIdWsfServiceInstance::WriteAsXMLToL(RWriteStream& /* aWriteStream */)
    {
    // Nothing to do, because for ID-WSF we never save ServiceInstances,
    // only sessions.
    return;
    }

HBufC* CIdWsfServiceInstance::AsXmlUnicodeL()
    {
    // Nothing to do, because for ID-WSF we never save ServiceInstances,
    // only sessions.
    // note: could return NULL
    return KNullDesC().AllocL();
    }

HBufC8* CIdWsfServiceInstance::AsXmlL()
    {
    // Nothing to do, because for ID-WSF we never save ServiceInstances,
    // only sessions.
    // note: could return NULL
    return KNullDesC8().AllocL();
    }

TPtrC8 CIdWsfServiceInstance::ProviderId()
    {
    if(ipProviderId)
        return *ipProviderId;
    else
        return KNullDesC8();
    }

TPtrC8 CIdWsfServiceInstance::ResourceId()
    {
    if (ipResourceId)
        return *ipResourceId;
    else
        return KNullDesC8();
    }

TPtrC8 CIdWsfServiceInstance::Endpoint()
    {
    if (ipEndpoint)
        return *ipEndpoint;
    else
        return KNullDesC8();
    }

TPtrC8 CIdWsfServiceInstance::ServiceType()
    {
    if (ipServiceType)
        return *ipServiceType;
    else
        return KNullDesC8();
    }

TInt CIdWsfServiceInstance::SetProviderIdL( const TDesC8& aURI )
    {
    TInt retVal( KErrNone );
    HBufC8* pNew = NULL;
    if( aURI.Length() > 0 )
        {
        pNew = aURI.AllocL();
        }
//    else
//        {
//        retVal = KErrArgument;
//        }
    delete ipProviderId;
    ipProviderId = pNew;
    return retVal;
    }

void CIdWsfServiceInstance::SetResourceIdL( const TDesC8& aURI )
    {
    HBufC8* pNew = NULL;
    if(aURI.Length()>0)
        pNew = aURI.AllocL();
    delete ipResourceId;
    ipResourceId = pNew;
    }

void CIdWsfServiceInstance::SetEndPointL(const TDesC8& aURI)
    {
    HBufC8* pNew = NULL;
    if(aURI.Length()>0)
        pNew = aURI.AllocL();
    delete ipEndpoint;
    ipEndpoint = pNew;
    }

void CIdWsfServiceInstance::SetServiceTypeL(const TDesC8& aURI)
    {
    HBufC8* pNew = NULL;
    if(aURI.Length()>0)
        pNew = aURI.AllocL();
    delete ipServiceType;
    ipServiceType = pNew;
    }

CDesC8Array& CIdWsfServiceInstance::MechanismsL()
    {
    if(!ipMechanisms)
        {
        ipMechanisms = new (ELeave) CDesC8ArrayFlat(KFLATBUF_SIZE);
        }
    return *ipMechanisms;
    }

CDesC8Array& CIdWsfServiceInstance::CredentialRefsL()
    {
    if(!ipCredentialRefs)
        {
        ipCredentialRefs = new (ELeave) CDesC8ArrayFlat(KFLATBUF_SIZE);
        }
    return *ipCredentialRefs;
    }

TBool CIdWsfServiceInstance::SupportsMechanism(const TDesC8& aURI)
    {
    if (ipMechanisms)
        {
        TInt pos;
        return (ipMechanisms->Find(aURI, pos, ECmpNormal) == KErrNone);
        }
    return EFalse;
    }

CSenCredential* CIdWsfServiceInstance::Credential()
    {
    CSenCredential* pCredential = NULL;
    TInt count(iCredentialList.Count());
    for(TInt i=0; i<count && pCredential == NULL; i++)
        {
        pCredential = iCredentialList[i];
        }
    return pCredential;
    }


void CIdWsfServiceInstance::SetCredentialL(const TDesC8& aIdRef,
                                           const CSenCredential& aCredential)
    {
    TInt refCount(CredentialRefsL().Count());
    for (TInt i = 0; i < refCount; i++)
        {
        // Remove IDREFs and add actual credential
        if (CredentialRefsL()[i] == aIdRef)
            {
            CSenCredential* pCredential = CSenCredential::NewL(aCredential);
            iCredentialList.Append(pCredential);
            ipCredentialRefs->Delete(i);
            break;
            }
        }
    }

void CIdWsfServiceInstance::SetCredentialL(const CSenCredential& aCredential)
    {
    const TDesC8& idRef = ((CSenCredential&)aCredential).Id();
    SetCredentialL(idRef, aCredential);
    }

void CIdWsfServiceInstance::AddSecurityMechL(const TDesC8& aUri)
    {
    MechanismsL().AppendL(aUri);
    }

void CIdWsfServiceInstance::AddCredentialRefL(const TDesC8& aIdRef)
    {
    CredentialRefsL().AppendL(aIdRef);
    }

TInt CIdWsfServiceInstance::HasFacetL(const TDesC8& aURI, TBool& aHasFacet)
    {
    aHasFacet = EFalse;
    TInt count(iFacets.Count());

    for (TInt i=0;i<count && !aHasFacet;i++)
        {
        if (iFacets[i]->Name() == aURI)
            {
            aHasFacet = ETrue;
            }
        }

    return KErrNone;
    }

TInt CIdWsfServiceInstance::FacetValue(TDesC8& aURI, HBufC8*& aValueTo)
    {
    delete aValueTo;
    aValueTo = NULL;

    TInt retVal = KErrNotFound;
    TInt count(iFacets.Count());

    for (TInt i=0;i<count && retVal == KErrNotFound;i++)
        {
        if (iFacets[i]->Name() == aURI)
            {
            aValueTo = iFacets[i]->Value().Alloc();
            if (aValueTo == NULL) retVal = KErrNoMemory;
            else retVal = KErrNone;
            }
        }
    return retVal;
    }

TInt CIdWsfServiceInstance::AddFacetL(const CSenFacet& aFacet)
    {
    return SetFacetL(aFacet);
    }

TInt CIdWsfServiceInstance::SetFacetL(const CSenFacet& aFacet)
    {
    TBool hasFacet;
    HasFacetL(((CSenFacet&)aFacet).Name(),hasFacet);
    if (hasFacet)
        {
        RemoveFacet(((CSenFacet&)aFacet).Name());
        }
    CSenFacet* pNewFacet = CSenFacet::NewL(((CSenFacet&)aFacet));
    return iFacets.Append(pNewFacet);
    }

TInt CIdWsfServiceInstance::RemoveFacet(const TDesC8& aURI)
    {
    TInt count(iFacets.Count());

    for (TInt i=0;i<count;i++)
        {
        if (iFacets[i]->Name() == aURI)
            {
            delete iFacets[i];
            iFacets.Remove(i);
            return KErrNone;
            }
        }

    return KErrNotFound;
    }

TInt CIdWsfServiceInstance::FacetsL(RFacetArray& aFacetArray)
    {
    CSenFacet* pFacet = NULL;
    TInt count(iFacets.Count());
    for (TInt i=0; i<count; i++)
        {
        pFacet = CSenFacet::NewL(*iFacets[i]);
        aFacetArray.Append(pFacet);
        }
    return KErrNone;
    }

TInt CIdWsfServiceInstance::ScoreMatchL(MSenServiceDescription& aPattern)
    {
    TInt score(0);

    if ((aPattern.Endpoint().Length() > 0) &&
        (aPattern.Endpoint() == Endpoint()))
        {
        score++;
        }
    if ((aPattern.Contract().Length() > 0) &&
        (aPattern.Contract() == Contract()))
        {
        score++;
        }

    RFacetArray otherFacets;
    CleanupClosePushL(otherFacets);
    aPattern.FacetsL(otherFacets);

    HBufC8* pFacetValue = NULL;
    TPtrC8 facetName;

    TInt count(otherFacets.Count());
    for (TInt i=0; i<count; i++)
        {
        facetName.Set(otherFacets[i]->Name());
        FacetValue(facetName,pFacetValue);
        if (pFacetValue && *pFacetValue == otherFacets[i]->Value())
            {
            score++;
            }
        delete pFacetValue;
        pFacetValue = NULL;
        }

    otherFacets.ResetAndDestroy();
    CleanupStack::Pop(); // otherFacets

    return score;
    }

// equals to CSenWSDescription implementation at the moment
TBool CIdWsfServiceInstance::HasEqualPrimaryKeysL(MSenServiceDescription& aCandidate)
    {
    TBool retVal(EFalse);
    if(aCandidate.Endpoint()  == Endpoint() &&
       aCandidate.Contract() == Contract() &&
       aCandidate.FrameworkId() == FrameworkId())
        {
        retVal = ETrue;
        }
    return retVal;    
    }


TBool CIdWsfServiceInstance::HasSuperClass( TDescriptionClassType aType )
    {
    if( aType == MSenServiceDescription::EWSDescription ) // direct superclass!
        {
        // If asked type is the know *direct* father/mother, return true:
        return ETrue; 
        } 
    else
        {
        // Otherwise, ask from superclass (chain, recursively)
        return CSenWSDescription::HasSuperClass( aType ); 
        }
    }



// End of File
