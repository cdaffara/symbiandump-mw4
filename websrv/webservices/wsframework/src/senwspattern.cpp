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
#include "senwspattern.h"
#include "senconsumerpolicy.h"

namespace
    {
    _LIT8(KConsumerPolicyLocalName, "ConsumerPolicy");
    }

EXPORT_C CSenWSPattern* CSenWSPattern::NewL()
    {
    CSenWSPattern* pNew = NewLC();
    CleanupStack::Pop();
    return(pNew) ;
    }

EXPORT_C CSenWSPattern* CSenWSPattern::NewLC()
    {
    CSenWSPattern* pNew =
                            new (ELeave) CSenWSPattern(EWSPattern);
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL();
    return pNew;
    }

EXPORT_C CSenWSPattern* CSenWSPattern::NewL(
                                                const TDesC8& aNamespaceURI)
    {
    CSenWSPattern* pNew = NewLC(aNamespaceURI);
    CleanupStack::Pop();
    return(pNew) ;
    }


EXPORT_C CSenWSPattern* CSenWSPattern::NewLC(
                                                const TDesC8& aNamespaceURI)
    {
    CSenWSPattern* pNew =
                            new (ELeave) CSenWSPattern(EWSPattern);
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aNamespaceURI);
    return pNew;
    }

EXPORT_C CSenWSPattern* CSenWSPattern::NewL(const TDesC8& aEndPoint,
                                                      const TDesC8& aContract)
    {
    CSenWSPattern* pNew = NewLC(aEndPoint, aContract);
    CleanupStack::Pop();
    return(pNew) ;
    }

EXPORT_C CSenWSPattern* CSenWSPattern::NewLC(const TDesC8& aEndPoint,
                                                       const TDesC8& aContract)
    {
    CSenWSPattern* pNew =
                            new (ELeave) CSenWSPattern(EWSPattern);
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aEndPoint, aContract);
    return pNew;
    }

EXPORT_C CSenWSPattern::CSenWSPattern(
                        MSenServiceDescription::TDescriptionClassType aType)
: CSenWSDescription(aType)
    {
    }

EXPORT_C CSenWSPattern::~CSenWSPattern()
    {
    delete iConsumerPolicy;
    }

EXPORT_C void CSenWSPattern::BaseConstructL()
    {
    CSenWSDescription::ConstructL();
    // construct empty consumer policy...

    // NOTE; iConsumerPolicy is, AS INTENDED, a separate XML object
    // from the "WSPattern", which contains it
    iConsumerPolicy = CSenConsumerPolicy::NewL();
    }

EXPORT_C void CSenWSPattern::BaseConstructL(const TDesC8& aNamespaceURI)
    {
    CSenWSDescription::ConstructL(aNamespaceURI);
    // construct empty consumer policy...
    iConsumerPolicy = CSenConsumerPolicy::NewL();
    }

EXPORT_C void CSenWSPattern::BaseConstructL(const TDesC8& aEndPoint,
                                                const TDesC8& aContract)
    {
    CSenWSDescription::ConstructL(aEndPoint, aContract);
    // construct empty consumer policy...
    iConsumerPolicy = CSenConsumerPolicy::NewL();
    }


EXPORT_C TBool CSenWSPattern::Matches(
                                    MSenServiceDescription& aWSPattern)
    {
    TBool matches = CSenWSDescription::Matches(aWSPattern);
    if(matches && aWSPattern.DescriptionClassType() == EWSPattern)
        {
        CSenWSPattern* pPattern = (CSenWSPattern*)&aWSPattern;
        return iConsumerPolicy->Accepts(*(MSenConsumerPolicy*)pPattern);
        }
    else
        return EFalse; // even the service descriptions wont match
    }

// from MSenConsumerPolicy
EXPORT_C void CSenWSPattern::SetConsumerIapIdL(TUint32 aIapId)
    {
    iConsumerPolicy->SetIapIdL(aIapId);
    }

// getter for IAP ID
EXPORT_C TInt CSenWSPattern::ConsumerIapId(TUint32& aCurrentIapId)
    {
    return iConsumerPolicy->IapId(aCurrentIapId);
    }

EXPORT_C void CSenWSPattern::SetConsumerSnapIdL(TUint32 aSnapId)
    {
    iConsumerPolicy->SetSnapIdL(aSnapId);
    }

// getter for SNAP
EXPORT_C TInt CSenWSPattern::ConsumerSnapId(TUint32& aCurrentSnapId)
    {
    return iConsumerPolicy->SnapId(aCurrentSnapId);
    }

// Setter: overrides current values with the given values from the array
EXPORT_C void CSenWSPattern::SetConsumerIdentityProviderIdsL(
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
EXPORT_C TInt CSenWSPattern::AddConsumerIdentityProviderIdL(
                                        const TDesC8& aProviderId)
    {
    return iConsumerPolicy->AddIdentityProviderIdL(aProviderId);
    }


// Getter: return an empty array if no IDP:s have been spesified
// or a list of IDP arrays if such value(s) have been set.
EXPORT_C const CSenIdentityProviderIdArray8&
                CSenWSPattern::ConsumerIdentityProviderIds8L()
    {
    return iConsumerPolicy->IdentityProviderIds8L();
    }


EXPORT_C TBool CSenWSPattern::AcceptsConsumerPolicy(
                                            MSenConsumerPolicy& aPolicyPattern)
    {
    return iConsumerPolicy->Accepts(aPolicyPattern);
    }

EXPORT_C TBool CSenWSPattern::RebuildFromConsumerPolicy(
                                                MSenConsumerPolicy& aTemplate)
    {
    return iConsumerPolicy->RebuildFrom(aTemplate);
    }

// overrides CWSDescription
EXPORT_C void CSenWSPattern::StartElementL(
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
        CSenWSDescription::StartElementL(
            aNsUri, aLocalName, aQName, aAttributes);
        }
    }

EXPORT_C HBufC8* CSenWSPattern::ConsumerPolicyAsXmlL()
    {
    return iConsumerPolicy->AsXmlL();
    }

EXPORT_C TBool CSenWSPattern::HasSuperClass( TDescriptionClassType aType )
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
