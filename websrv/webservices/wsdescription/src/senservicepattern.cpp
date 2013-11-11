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
#include "SenServicePattern.h"
#include "senconsumerpolicy.h"

namespace
    {
    _LIT8(KConsumerPolicyLocalName, "ConsumerPolicy");
    }



EXPORT_C CSenServicePattern* CSenServicePattern::NewL()
    {
    CSenServicePattern* pNew = NewLC();
    CleanupStack::Pop();
    return(pNew) ;
    }


EXPORT_C CSenServicePattern* CSenServicePattern::NewLC()
    {
    CSenServicePattern* pNew = 
                            new (ELeave) CSenServicePattern(EServicePattern);
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL();
    return pNew;
    }

EXPORT_C CSenServicePattern* CSenServicePattern::NewL(
                                                const TDesC8& aNamespaceURI)
    {
    CSenServicePattern* pNew = NewLC(aNamespaceURI);
    CleanupStack::Pop();
    return(pNew) ;
    }


EXPORT_C CSenServicePattern* CSenServicePattern::NewLC(
                                                const TDesC8& aNamespaceURI)
    {
    CSenServicePattern* pNew = 
                            new (ELeave) CSenServicePattern(EServicePattern);
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aNamespaceURI);
    return pNew;
    }

EXPORT_C CSenServicePattern* CSenServicePattern::NewL(const TDesC8& aEndPoint, 
                                                      const TDesC8& aContract)
    {
    CSenServicePattern* pNew = NewLC(aEndPoint, aContract);
    CleanupStack::Pop();
    return(pNew) ;
    }

EXPORT_C CSenServicePattern* CSenServicePattern::NewLC(const TDesC8& aEndPoint, 
                                                       const TDesC8& aContract)
    {
    CSenServicePattern* pNew = 
                            new (ELeave) CSenServicePattern(EServicePattern);
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aEndPoint, aContract);
    return pNew;
    }

EXPORT_C CSenServicePattern::CSenServicePattern(
                        MSenServiceDescription::TDescriptionClassType aType)
: CSenXmlServiceDescription(aType)
    {
    }

EXPORT_C CSenServicePattern::~CSenServicePattern()
    {
    delete iConsumerPolicy;
    }

EXPORT_C void CSenServicePattern::BaseConstructL()
    {
    CSenXmlServiceDescription::ConstructL();
    // construct empty consumer policy...

    // NOTE; iConsumerPolicy is, AS INTENDED, a separate XML object
    // from the "ServicePattern", which contains it
    iConsumerPolicy = CSenConsumerPolicy::NewL();
    }

EXPORT_C void CSenServicePattern::BaseConstructL(const TDesC8& aNamespaceURI)
    {
    CSenXmlServiceDescription::ConstructL(aNamespaceURI);
    // construct empty consumer policy...
    iConsumerPolicy = CSenConsumerPolicy::NewL();
    }

EXPORT_C void CSenServicePattern::BaseConstructL(const TDesC8& aEndPoint, 
                                                const TDesC8& aContract)
    {
    CSenXmlServiceDescription::ConstructL(aEndPoint, aContract);
    // construct empty consumer policy...
    iConsumerPolicy = CSenConsumerPolicy::NewL();
    }


EXPORT_C TBool CSenServicePattern::Matches(
                                    MSenServiceDescription& aServicePattern)
    {
    TBool matches = CSenXmlServiceDescription::Matches(aServicePattern);
    if(matches && aServicePattern.DescriptionClassType() == EServicePattern)
        {
        CSenServicePattern* pPattern = (CSenServicePattern*)&aServicePattern;
        return iConsumerPolicy->Accepts(*(MSenConsumerPolicy*)pPattern);
        }
    else 
        return EFalse; // even the service descriptions wont match
    }

// from MSenConsumerPolicy
EXPORT_C void CSenServicePattern::SetConsumerIapIdL(TUint32 aIapId)
    {
    iConsumerPolicy->SetIapIdL(aIapId);
    }

// getter for IAP ID
EXPORT_C TInt CSenServicePattern::ConsumerIapId(TUint32& aCurrentIapId)
    {
    return iConsumerPolicy->IapId(aCurrentIapId);
    }

// setter for SNAP ID
EXPORT_C void CSenServicePattern::SetConsumerSnapIdL(TUint32 aSnapId)
    {
    iConsumerPolicy->SetSnapIdL(aSnapId);
    }

// getter for SNAP ID
EXPORT_C TInt CSenServicePattern::ConsumerSnapId(TUint32& aCurrentSnapId)
    {
    return iConsumerPolicy->SnapId(aCurrentSnapId);
    }


// Setter: overrides current values with the given values from the array
EXPORT_C void CSenServicePattern::SetConsumerIdentityProviderIdsL(
                            CSenIdentityProviderIdArray8& aList)
    {
    iConsumerPolicy->SetIdentityProviderIdsL(aList);
    }

// Adder: adds a new IDP ID value at the end of the current list value(s)
// Does not insert duplicate values. 
//
// @return KErrAlreadyExists, if a duplicate is tried to add
//         KErrArgument if a zero-length descriptor is tried to add 
//         (aProviderId.Length() == 0)
EXPORT_C TInt CSenServicePattern::AddConsumerIdentityProviderIdL(
                                        const TDesC8& aProviderId)
    {
    return iConsumerPolicy->AddIdentityProviderIdL(aProviderId);
    }


// Getter: return an empty array if no IDP:s have been spesified
// or a list of IDP arrays if such value(s) have been set.
EXPORT_C const CSenIdentityProviderIdArray8&
                CSenServicePattern::ConsumerIdentityProviderIds8L()
    {
    return iConsumerPolicy->IdentityProviderIds8L();
    }


EXPORT_C TBool CSenServicePattern::AcceptsConsumerPolicy(
                                            MSenConsumerPolicy& aPolicyPattern)
    {
    return iConsumerPolicy->Accepts(aPolicyPattern);
    }

EXPORT_C TBool CSenServicePattern::RebuildFromConsumerPolicy(
                                                MSenConsumerPolicy& aTemplate)
    {
    return iConsumerPolicy->RebuildFrom(aTemplate);
    }

// overrides CXmlServiceDescription
EXPORT_C void CSenServicePattern::StartElementL(
                                    const TDesC8& aNsUri,
                                    const TDesC8& aLocalName,
                                    const TDesC8& aQName,
                                    const RAttributeArray& aAttributes)
    {

    if(aLocalName == KConsumerPolicyLocalName)
        {
        // get rid of existing stuff
        delete iConsumerPolicy;
        iConsumerPolicy = NULL;
        iConsumerPolicy = CSenConsumerPolicy::NewL();

        iConsumerPolicy->SetAttributesL(aAttributes);
        DelegateParsingL(*iConsumerPolicy);
        }
    else 
        {
        CSenXmlServiceDescription::StartElementL(
            aNsUri, aLocalName, aQName, aAttributes);
        }
    }

EXPORT_C HBufC8* CSenServicePattern::ConsumerPolicyAsXmlL()
    {
    return iConsumerPolicy->AsXmlL();
    }

// End of File
