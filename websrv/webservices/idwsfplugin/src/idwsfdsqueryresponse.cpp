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








// INCLUDE FILES
#include <SenDateUtils.h>
#include <SenFacet.h>
#include <SenXmlElement.h> // check if this include is needed(?)
#include <SenXmlUtils.h>
#include <SenCredential.h>

#include "idwsfdsqueryresponse.h"
#include "sendebug.h"


namespace
    {
    _LIT8(KOkLowerCase, "ok");
    _LIT8(KOkUpperCase, "OK");

    _LIT8(KResourceIdName, "ResourceID");
    _LIT8(KResourceOfferingName, "ResourceOffering");
    _LIT8(KServiceInstanceName, "ServiceInstance");
    _LIT8(KEncryptedResourceIdName, "EncryptedResourceID");
    _LIT8(KProviderIdName, "ProviderID");
    _LIT8(KCredentialRefName, "CredentialRef");
    _LIT8(KServiceTypeName, "ServiceType");
    _LIT8(KOptionName, "Option");

    _LIT8(KVal_NotOnOrAfter, "NotOnOrAfter");

    _LIT8(KDescriptionElementLocalName, "Description");
    }

CIdWsfDsQueryResponse* CIdWsfDsQueryResponse::NewL()
    {
    CIdWsfDsQueryResponse* pNew = NewLC();
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

CIdWsfDsQueryResponse* CIdWsfDsQueryResponse::NewLC()
    {
    CIdWsfDsQueryResponse* pNew = new (ELeave) CIdWsfDsQueryResponse;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL();
    return pNew;
    }


CIdWsfDsQueryResponse* CIdWsfDsQueryResponse::NewL(
    const TDesC8& aNsUri,
    const TDesC8& aLocalName,
    const TDesC8& aQName
    )
    {
    CIdWsfDsQueryResponse* pNew = NewLC( aNsUri, aLocalName, aQName );
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

CIdWsfDsQueryResponse* CIdWsfDsQueryResponse::NewLC(
    const TDesC8& aNsUri,
    const TDesC8& aLocalName,
    const TDesC8& aQName
    )
    {
    CIdWsfDsQueryResponse* pNew = new (ELeave) CIdWsfDsQueryResponse;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aNsUri, aLocalName, aQName);
    return pNew;
    }


CIdWsfDsQueryResponse::CIdWsfDsQueryResponse()
: ipResourceOffering(NULL),
  ipCredential(NULL),
  ipServiceInstance(NULL)
    {
    }

void CIdWsfDsQueryResponse::BaseConstructL()
    {
    CSenBaseFragment::BaseConstructL(
        KQueryResponseXmlns,
        KQueryResponseName,
        KQueryResponseQName
        );
    }

void CIdWsfDsQueryResponse::BaseConstructL(
    const TDesC8& aNsUri,
    const TDesC8& aLocalName,
    const TDesC8& aQName
    )
    {
    CSenBaseFragment::BaseConstructL(aNsUri, aLocalName, aQName);
    iNotOnOrAfter = Time::NullTTime();
    }

CIdWsfDsQueryResponse::~CIdWsfDsQueryResponse()
    {
    delete ipStatus;
    iResourceOfferings.ResetAndDestroy();

    // should be NULL if appended to iResourceOfferings:
    delete ipResourceOffering;
    delete ipCredential;
    delete ipServiceInstance;
    }

void CIdWsfDsQueryResponse::GetAllServicesL(
        RPointerArray<CIdWsfServiceInstance>& aDest )
    {
    // Return all CIdWsfServiceInstance's
    for (TInt i = 0; i < iResourceOfferings.Count(); i++)
        {
        RPointerArray<CIdWsfServiceInstance>& services =
            iResourceOfferings[i]->ServicesL();
        for (TInt j = 0; j < services.Count(); j++)
            {
            User::LeaveIfError(aDest.Append(services[j]));
            }
        }
    }

void CIdWsfDsQueryResponse::StartElementL(
    const TDesC8& aNsUri,
    const TDesC8& aLocalName,
    const TDesC8& aQName,
    const RAttributeArray& aAttributes
    )
    {
    switch (iState)
        {
        case KStateIgnore:
            {
            SaveNamespacesL(aAttributes, EFalse);

            // we are interested in the Status element
            if (aLocalName == KStatusName)
                {
                // we should save the status code
                HBufC8* pNew =
                    SenXmlUtils::AllocAttrValueL(aAttributes, 
                                                    KStatusCodeName);
                delete ipStatus;
                ipStatus = pNew;
                }

            // we are interested in ResourceOffering
            else if (aLocalName == KResourceOfferingName)
                {
                // odd state: starts saving content
                iState = KStateParsingResourceOffering;
                ipResourceOffering = CIdWsfResourceOffering::NewL();
                }

            //or in a Credentials
            else if (aLocalName == KCredentialsName)
                {
                iState = KStateParsingCredentials;
                TPtrC8 ptr = SenXmlUtils::AttrValue(aAttributes, 
                                                    KVal_NotOnOrAfter);
                if (ptr.Length()>0)
                    {
                    iNotOnOrAfter = SenDateUtils::FromXmlDateTimeL(ptr);
                    }
                }
            break;
            }
        case KStateParsingResourceOffering:
            {
            // We clean the buffer so that we can save the element content
            // At endElement we will know what member of the resource offering
            // to set.
            ResetContentL();
            // save namespace declarations and ensure that all local prefixes
            // are noted.
            SaveNamespacesL(aAttributes, ETrue);
            if (aLocalName == KServiceInstanceName)
                {
                // if this is second RO, delete the ipServiceInstance made
                // ready
                // previous RO's possible (but not encountered) next service
                // instance.
                // bugfix 2004-11-09
                if(ipServiceInstance)
                    {
                    delete ipServiceInstance;
                    ipServiceInstance = NULL;
                    }

                ipServiceInstance = CIdWsfServiceInstance::NewL();
                }
            if (aLocalName == KEncryptedResourceIdName)
                {
                iState = KStateParsingEncryptedResourceId;
                }
            break;
            }
        case KStateParsingEncryptedResourceId:
            {
            AllocContentSaverL();
            WriteStartElementL(aNsUri, aLocalName, aQName, aAttributes);
            break;
            }
        case KStateParsingCredentials:
            {
            CSenCredential* pCredential = CSenCredential::NewL(
                aNsUri, aLocalName, aQName, aAttributes, AsElement());

            CleanupStack::PushL(pCredential);  // bugfix 2004-08-09

            if (iNotOnOrAfter != Time::NullTTime())
                {
                pCredential->SetValidUntil(iNotOnOrAfter);
                }

            delete ipCredential;
            ipCredential = pCredential;
            CleanupStack::Pop(); // class owns the pCredential now..
            DelegateParsingL(*ipCredential);
            iState = KStateParsingSingleCredential;
            break;
            }
        default:
            {
            break;
            }
        }
    }

void CIdWsfDsQueryResponse::EndElementL(
    const TDesC8& aNsUri,
    const TDesC8& aLocalName,
    const TDesC8& aQName)
    {
    switch(iState)
        {
        case KStateParsingResourceOffering:
            {
            if (aLocalName == KResourceOfferingName)
                {
                User::LeaveIfError(
                            iResourceOfferings.Append(ipResourceOffering));
                ipResourceOffering = NULL;
                iState = KStateIgnore;
                }
            else if (aLocalName == KResourceIdName ||
                        aLocalName == KEncryptedResourceIdName)
                {
                ipResourceOffering->SetResourceIdL(Content());
                }

            else if (aLocalName == KDescriptionElementLocalName)
                {
                ipResourceOffering->AddServiceInstanceL(ipServiceInstance);

                // alloc new instance for the next parsing cycle
                ipServiceInstance =
                        CIdWsfServiceInstance::NewL(*ipServiceInstance);
                break;
                }
            else
                {
                if (aLocalName == KProviderIdName)
                    {
                    ipServiceInstance->SetProviderIdL(Content());
                    break;
                    }
                else if (aLocalName == KSecurityMechIdName)
                    {
                    ipServiceInstance->AddSecurityMechL(Content());
                    break;
                    }
                else if (aLocalName == KCredentialRefName)
                    {
                    ipServiceInstance->AddCredentialRefL(Content());
                    break;
                    }
                else if (aLocalName == KEndpointName)
                    {
                    ipServiceInstance->SetEndPointL(Content());
                    break;
                    }
                else if (aLocalName == KServiceTypeName)
                    {
                    ipServiceInstance->SetServiceTypeL(Content());
                    break;
                    }
                else if (aLocalName == KOptionName)
                    {
                    CSenFacet* pFacet = CSenFacet::NewL();
                    CleanupStack::PushL( pFacet );
                    pFacet->SetNameL(Content());
                    pFacet->SetTypeL(KDiscoOption);
                    ipResourceOffering->SetFacetL(*pFacet);
                    CleanupStack::PopAndDestroy( pFacet ); // SetFacetL does not take ownership but makes a copy of the facet insteadfs
                    break;
                    }
                }
            break;
            }
        case KStateParsingEncryptedResourceId:
            {
            if (aLocalName == KResourceIdName ||
                aLocalName == KEncryptedResourceIdName)
                {
                ipResourceOffering->SetResourceIdL(Content());
                iState = KStateParsingResourceOffering;
                }
            else WriteEndElementL(aNsUri, aLocalName, aQName);
            break;
            }
        case KStateParsingCredentials:
            {
            if (aLocalName == KCredentialsName)
                {
                iNotOnOrAfter = Time::NullTTime();
                iState = KStateIgnore;
                }
            break;
            }
        case KStateParsingSingleCredential:
            {

            HBufC8* pAsXml = ipCredential->AsXmlL();
            CleanupStack::PushL(pAsXml);
            const TDesC8& refId = ipCredential->Id();

            if(pAsXml && refId.Length()>0)
                {
                ipCredential->DetachL();

                TInt count(iResourceOfferings.Count());
                for (TInt i = 0; i < count; i++)
                    {
                    iResourceOfferings[i]->AddCredentialL(*ipCredential);
                    }
                delete ipCredential;
                }
            else
                delete ipCredential; // ownership was _not_ transferred(!),
                                     // forced to delete here

            CleanupStack::PopAndDestroy(); // pAsXml;

            ipCredential = NULL;
            iState = KStateParsingCredentials;
            break;
            }
        default: // corresponds with ref-impl (Java) having case
                 // BaseFragment.IGNORE: break;
            {
            break;
            }
        }
    }

TBool CIdWsfDsQueryResponse::IsOk()
    {
    if (!ipStatus)
        {
        return EFalse;
        }
    else
        {
        return (SenXmlUtils::EndsWith(*ipStatus, KOkLowerCase) ||
                SenXmlUtils::EndsWith(*ipStatus, KOkUpperCase));
        }
    }

// returns the first one (oldest) added..
TPtrC8 CIdWsfDsQueryResponse::ResourceId()
    {
    if (iResourceOfferings.Count() > 0)
        {
        return iResourceOfferings[0]->ResourceId();
        }
    else
        {
        return NULL;
        }
    }

RPointerArray<CIdWsfResourceOffering>&
                                    CIdWsfDsQueryResponse::ResourceOfferingsL()
    {
    return iResourceOfferings;
    }
