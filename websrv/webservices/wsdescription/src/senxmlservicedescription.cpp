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











// INCLUDE FILESS
#include <e32base.h>
#include <e32des8.h>    // HBufC8

#include "SenDateUtils.h"

#include "SenXmlServiceDescription.h"
#include "SenBaseFragment.h"
#include "SenXmlUtils.h"
#include "sendebug.h"

#include "SenCredential.h"
#include "senproviderpolicy.h"
#include "senservicepolicy.h"
#include "SenFacet.h"
#include "SenXmlUtils.h"

#include <SenServiceConnection.h> // for KErrSenNoEndpoint


namespace
    {
    // Local names for XML element: 
    _LIT8(KServiceDescriptionLocalName, "ServiceDescription");
    _LIT8(KEndpointLocalname,           "Endpoint");
    _LIT8(KContractLocalname,           "Contract");
    _LIT8(KProviderPolicyLocalName,     "ProviderPolicy");
    _LIT8(KServicePolicyLocalName,      "ServicePolicy");
    _LIT8(KClientServicePolicyLocalName, "ClientPolicy");
    _LIT8(KSenIdpProviderIdLocalname,       "ProviderID");
    // Names for XML attributes:
    _LIT8(KNotOnOrAfter,                "notOnOrAfter");
    _LIT8(KFramework,                   "framework");
    _LIT8(KCue,                         "cue");
    _LIT8(KTouch, 						"touch");
    }


EXPORT_C CSenXmlServiceDescription* CSenXmlServiceDescription::NewL()
    {
    CSenXmlServiceDescription* pNew = NewLC();
    CleanupStack::Pop();
    return(pNew) ;
    }

EXPORT_C CSenXmlServiceDescription* CSenXmlServiceDescription::NewLC()
    {
    CSenXmlServiceDescription* pNew =
        new (ELeave) CSenXmlServiceDescription(EXmlServiceDescription);
    CleanupStack::PushL(pNew);
    pNew->ConstructL();
    return pNew;
    }

EXPORT_C void CSenXmlServiceDescription::ConstructL()
    {
    BaseConstructL(NewElementName());
    // Create empty policy
    iProviderPolicy = CSenProviderPolicy::NewL();
    CSenElement& element = iProviderPolicy->AsElement();
    AsElement().AddElementL(element);
    iServicePolicy = CSenServicePolicy::NewL();
    CSenElement& servicePolicyElement = iServicePolicy->AsElement();
    AsElement().AddElementL(servicePolicyElement);
    }

EXPORT_C const TDesC8& CSenXmlServiceDescription::NewElementName()
    {
    return KServiceDescriptionLocalName();
    }

EXPORT_C CSenXmlServiceDescription* CSenXmlServiceDescription::NewL(
                                                const TDesC8& aNamespaceURI)
    {
    CSenXmlServiceDescription* pNew = NewLC(aNamespaceURI);
    CleanupStack::Pop();
    return(pNew) ;
    }

EXPORT_C CSenXmlServiceDescription* CSenXmlServiceDescription::NewLC(
                                                const TDesC8& aNamespaceURI)
    {
    CSenXmlServiceDescription* pNew =
                new (ELeave) CSenXmlServiceDescription(EXmlServiceDescription);
    CleanupStack::PushL(pNew);
    pNew->ConstructL(aNamespaceURI);
    return pNew;
    }

EXPORT_C void CSenXmlServiceDescription::ConstructL(const TDesC8& aNamespaceURI)
    {
    BaseConstructL(aNamespaceURI, NewElementName());
    // create empty policy
    iProviderPolicy = CSenProviderPolicy::NewL();
    CSenElement& element = iProviderPolicy->AsElement();
    AsElement().AddElementL(element);
    iServicePolicy = CSenServicePolicy::NewL();
    CSenElement& servicePolicyElement = iServicePolicy->AsElement();
    AsElement().AddElementL(servicePolicyElement);
    }

EXPORT_C CSenXmlServiceDescription* CSenXmlServiceDescription::NewL(
                                                    const TDesC8& aEndPoint,
                                                    const TDesC8& aContract)
    {
    CSenXmlServiceDescription* pNew = NewLC(aEndPoint, aContract);
    CleanupStack::Pop();
    return(pNew) ;
    }

EXPORT_C CSenXmlServiceDescription* CSenXmlServiceDescription::NewLC(
                                                     const TDesC8& aEndPoint,
                                                     const TDesC8& aContract)
    {
    CSenXmlServiceDescription* pNew =
        new (ELeave) CSenXmlServiceDescription(EXmlServiceDescription);
    CleanupStack::PushL(pNew);
    pNew->ConstructL(aEndPoint, aContract);
    return pNew;
    }

EXPORT_C void CSenXmlServiceDescription::ConstructL(const TDesC8& aEndPoint,
                                                    const TDesC8& aContract)
    {
    BaseConstructL(NewElementName());
    if(aEndPoint.Length()>0)
        {
        SetContentOfL(KEndpointLocalname, aEndPoint);
        }

    if(aContract.Length()>0)
        {
        SetContentOfL(KContractLocalname, aContract);
        }
    // create empty policy
    iProviderPolicy = CSenProviderPolicy::NewL();
    CSenElement& element = iProviderPolicy->AsElement();
    AsElement().AddElementL(element);
    iServicePolicy = CSenServicePolicy::NewL();
    CSenElement& servicePolicyElement = iServicePolicy->AsElement();
    AsElement().AddElementL(servicePolicyElement);
    }


EXPORT_C CSenXmlServiceDescription::CSenXmlServiceDescription(
                                                TDescriptionClassType aType)
:   
    iType(aType),
    iServicePolicy(NULL),   
    iCredential(NULL),
    iNotOnOrAfter(Time::NullTTime()), // sets this session to be valid if read from db
    iProviderPolicy(NULL)
    
    {
    }

EXPORT_C CSenXmlServiceDescription::~CSenXmlServiceDescription()
    {
    // Bugfix 2004-07-27
    if(iCredential)
        {
        TInt index(KErrNotFound);
        index = iCredentialList.Find(iCredential);
        if(index==KErrNotFound)
            {
            delete iCredential; // otherwise deleted by ResetAndDestroy()
            }
        }


    iCredentialList.ResetAndDestroy(); // owns

    if( iProviderPolicy )
        {
        iProviderPolicy->ExtractElement();
        }    
    delete iProviderPolicy;
     if( iServicePolicy )
        {
        iServicePolicy->ExtractElement();
        }
         delete iServicePolicy;            
    }

EXPORT_C MSenServiceDescription::TDescriptionClassType
                            CSenXmlServiceDescription::DescriptionClassType()
    {
    return iType;
    }


EXPORT_C void CSenXmlServiceDescription::SetAttributesL(const RAttributeArray& aAttributes)
    {
    // Call superclass for namespace attributes only
    CSenBaseFragment::SetAttributesL(aAttributes); 

    // SenXmlUtils::AttrValue returns KNullDesC8 if attribute is not found:
    const TDesC8& id = SenXmlUtils::AttrValue(aAttributes, KFramework);
    if( id.Length()>0 )
        {
        // Add real attribute for ServiceDescription -element. If "framework"
        // attribute already exists, it's value will be overwritten:
        SenXmlUtils::AddAttributeL(AsElement(), KFramework, id); 
        }
    const TDesC8& touch = SenXmlUtils::AttrValue(aAttributes, KTouch);
    if( touch.Length()>0 )
        {
        // Add touch attribute for ServiceDescription -element. If "touch"
        // attribute already exists, it's value will be overwritten:

        CSenElement* elem = &AsElement();
        elem->AddAttrL(KTouch, touch);
        
        }
    const TDesC8& userInfoPrompt = SenXmlUtils::AttrValue(aAttributes, KSenAttrPromptUserInfo);
    if( userInfoPrompt.Length() > 0 )
        {
        SenXmlUtils::AddAttributeL(AsElement(), KSenAttrPromptUserInfo, userInfoPrompt ); 
        }
    }

EXPORT_C RCredentialList& CSenXmlServiceDescription::Credentials()
    {
    return iCredentialList;
    }

EXPORT_C TBool CSenXmlServiceDescription::Matches( MSenServiceDescription& aOtherServiceDescription )
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
		
    TBool match(ETrue);
    
      TInt retVal(KErrNone);
		TRAP(retVal,
		    RFacetArray otherFacets;
		    CleanupClosePushL(otherFacets);
		    aOtherServiceDescription.FacetsL(otherFacets);
		
		    TPtrC8 facetName;
		    TInt leave(KErrNone);
		    TInt count(otherFacets.Count());
		    for (TInt i=0; i<count && match; i++)
		        {
		        facetName.Set(otherFacets[i]->Name());
		        TRAP( leave, HasFacetL(facetName, match); )
		        }
		    otherFacets.ResetAndDestroy();
		    CleanupStack::Pop(); // otherFacets
				);
				
    return match;
    }


EXPORT_C TPtrC8 CSenXmlServiceDescription::Contract()
    {
    return ContentOf(KContractLocalname());
    }


EXPORT_C TPtrC8 CSenXmlServiceDescription::Endpoint()
    {
    return ContentOf(KEndpointLocalname());
    }

EXPORT_C TPtrC8 CSenXmlServiceDescription::FrameworkId()
    {
    const TDesC8* value = AsElement().AttrValue(KFramework);
    if(value)
        {
        return *value;
        }
    else
        {
        return KNullDesC8();
        }
    }

EXPORT_C TPtrC8 CSenXmlServiceDescription::FrameworkVersion()
    {
    return KNullDesC8();
    }

EXPORT_C void CSenXmlServiceDescription::SetContractL(const TDesC8& aContract)
    {
    SetContentOfL(KContractLocalname(), aContract);
    }


EXPORT_C void CSenXmlServiceDescription::SetEndPointL(const TDesC8& aEndPoint)
    {
    SetContentOfL(KEndpointLocalname(), aEndPoint);
    }

EXPORT_C void CSenXmlServiceDescription::SetFrameworkIdL(const TDesC8& aFrameworkId)
    {
    if ( aFrameworkId != KNullDesC8 )
        {
        // overwrites the value of "framework" attribute, if such attribute exists
        SenXmlUtils::AddAttributeL(AsElement(), KFramework, aFrameworkId);
        }
    else
        {
        // If there would not have been RemoveAttributeL() method, 
        // then AddAtributeL(KNullDesC8) could have been used
        delete SenXmlUtils::RemoveAttributeL(AsElement(), KFramework);
        }
    }

EXPORT_C HBufC8* CSenXmlServiceDescription::AsXmlL()
    {
    return AsElement().AsXmlL();
    }

EXPORT_C HBufC* CSenXmlServiceDescription::AsXmlUnicodeL()
    {
    return AsElement().AsXmlUnicodeL();
    }

EXPORT_C void CSenXmlServiceDescription::WriteAsXMLToL(RWriteStream& aWriteStream)
    {
    AsElement().WriteAsXMLToL(aWriteStream);
    }


EXPORT_C void CSenXmlServiceDescription::StartElementL(const TDesC8& aNsUri,
                                                       const TDesC8& aLocalName,
                                                       const TDesC8& aQName,
                                                       const RAttributeArray& aAttributes)
    {

    switch (iState)
        {
        case KStateSave: // IOP(!)
            {
            if(aLocalName.Compare(KCredentialsName) == 0)
                {
                iState = KStateParsingCredentials;

                TPtrC8 attValue = SenXmlUtils::AttrValue(aAttributes, 
                                                    KNotOnOrAfter);
                if(attValue.Length()>0)
                    {
                    iNotOnOrAfter = SenDateUtils::FromXmlDateTimeL(attValue);
                    }
                }
            else if(aLocalName.Compare(KProviderPolicyLocalName) == 0)
                {
                iState = KStateParsingProviderPolicy;
                
                TPtrC8 prefix(KNullDesC8);
                if(aQName.Length()>0)
                    {
                    TInt colon = aQName.Locate(':');
                    if(colon>0) // Note: 0 also treated as no prefix
                        {
                        prefix.Set(aQName.Ptr(), colon);
                        }
                    }
                iProviderPolicy->AsElement().SetNamespaceL(prefix, aNsUri);

                DelegateParsingL(*iProviderPolicy);
                }
             else if(aLocalName.Compare(KServicePolicyLocalName) == 0)
                {
                iState = KStateParsingServicePolicy;
                
                TPtrC8 prefix(KNullDesC8);
                if(aQName.Length()>0)
                    {
                    TInt colon = aQName.Locate(':');
                    if(colon>0) // Note: 0 also treated as no prefix
                        {
                        prefix.Set(aQName.Ptr(), colon);
                        }
                   }
                iServicePolicy->AsElement().SetNamespaceL(prefix, aNsUri);
                DelegateParsingL(*iServicePolicy);
                }
            else
                {
                CSenDomFragment::StartElementL( aNsUri,
                                                aLocalName,
                                                aQName,
                                                aAttributes );

                // For each endpoint, preserve the "cue" attribute
                if( aLocalName == KEndpointLocalname )
                    {
                    const TDesC8& cue = SenXmlUtils::AttrValue(aAttributes, KCue);
                    CSenElement* pEndpointElement = AsElement().Element( aLocalName );
                    if( cue.Length() > 0 && pEndpointElement)
                        {
                        SenXmlUtils::AddAttributeL(*pEndpointElement, KCue, cue );
                        }
                    }
                }
            break;
            }
        case KStateParsingCredentials:
            {
            iState = KStateParsingSingleCredential;

            // see destructor: iCredentialList.ResetAndDestroy(); // owns
            iCredential = CSenCredential::NewL( aNsUri,
                                                aLocalName,
                                                aQName,
                                                aAttributes,
                                                AsElement());
            iCredential->SetValidUntil(iNotOnOrAfter);

            DelegateParsingL(*iCredential);
            break;
            }
        default: // iState is KStateIgnore or some other..
            {
            // 2005-11: changed
            if(aLocalName == KServiceDescriptionLocalName())
                {
                SetAttributesL(aAttributes);
                }

            CSenDomFragment::StartElementL( aNsUri,
                                            aLocalName,
                                            aQName,
                                            aAttributes);
            }
        }
    }

EXPORT_C void CSenXmlServiceDescription::EndElementL(const TDesC8& aNsUri,
                                                     const TDesC8& aLocalName,
                                                     const TDesC8& aQName )
    {

    switch (iState)
        {
        case KStateParsingCredentials:
            {
            if(aLocalName == KCredentialsName)
                {
                iState = KStateSave;
                }
            } break;
        case KStateParsingSingleCredential:
            {
            iCredential->DetachL();
            iCredentialList.Append(iCredential);
            if(aLocalName == KCredentialsName)
                {
                iState = KStateIgnore;
                }
            else
                {
                iState = KStateParsingCredentials;
                }
            break;
            }
        case KStateParsingServicePolicy:
            {
//            if(aLocalName == KServicePolicyLocalName)
                {
                iState = KStateSave;
                }
            } break;            
        case KStateParsingProviderPolicy:
            {
            iState = KStateSave;
            } break;
        default:
            {
            CSenDomFragment::EndElementL(aNsUri, aLocalName, aQName);
            }
        }
    }

// From Provider Policy:
// Setter for Internet Access Point (IAP) ID
EXPORT_C void CSenXmlServiceDescription::SetIapIdL(TUint32 aIapId)
    {
    iProviderPolicy->SetIapIdL(aIapId);
    }

// Getter for IAP ID
EXPORT_C TInt CSenXmlServiceDescription::IapId(TUint32& aCurrentIapId)
    {
    return iProviderPolicy->IapId(aCurrentIapId);
    }

// Setter for SNAP
EXPORT_C void CSenXmlServiceDescription::SetSnapIdL(TUint32 aSnapId)
    {
    iProviderPolicy->SetSnapIdL(aSnapId);
    }

// Getter for SNAP
EXPORT_C TInt CSenXmlServiceDescription::SnapId(TUint32& aCurrentSnapId)
    {
    return iProviderPolicy->SnapId(aCurrentSnapId);
    }

// From Provider Policy:
EXPORT_C TInt CSenXmlServiceDescription::SetTransportPropertiesL(const TDesC8& aProperties)
    {
    return iProviderPolicy->SetTransportPropertiesL(aProperties);
    }

// Getter for IAP ID
EXPORT_C TInt CSenXmlServiceDescription::TransportPropertiesL(HBufC8*& aProperties)
    {
    return iProviderPolicy->TransportPropertiesL(aProperties);
    }

// Setter: overrides current values with the given values from the array
EXPORT_C void CSenXmlServiceDescription::SetIdentityProviderIdsL( CSenIdentityProviderIdArray8& aList )
    {
    iProviderPolicy->SetIdentityProviderIdsL(aList);
    }


// Adder: adds a new IDP ID value at the end of the current list value(s)
// Checks for duplicates (does not insert new ID elements with equal content)
// @return KErrAlreadyExists, if a duplicate is tried to add
//         KErrArgument if a zero-length descriptor is tried to add
//         (aProviderId.Length() == 0)

EXPORT_C TInt CSenXmlServiceDescription::AddIdentityProviderIdL(
                                                        TDesC8& aProviderId)
    {
    return iProviderPolicy->AddIdentityProviderIdL(aProviderId);
    }

// sets (rebuilds) the IAP ID and IDP ID list values from template
// @return  - a error, if at least one addition of new contents has
// failed OR KerrNone if every property was successfully reset
EXPORT_C TInt CSenXmlServiceDescription::RebuildFrom(
                                                MSenProviderPolicy& aTemplate)
    {
    return iProviderPolicy->RebuildFrom(aTemplate);
    }

// Getter: return an empty array if no IDP:s have been spesified
// or a list of IDP arrays if such value(s) have been set.
EXPORT_C const CSenIdentityProviderIdArray8&
                            CSenXmlServiceDescription::IdentityProviderIds8L()
    {
    return iProviderPolicy->IdentityProviderIds8L();
    }

EXPORT_C TBool CSenXmlServiceDescription::Accepts( MSenProviderPolicy& aPolicyPattern )
    {
    return iProviderPolicy->Accepts(aPolicyPattern);
    }

// Overridden from CSenDomFragment
EXPORT_C void CSenXmlServiceDescription::ResumeParsingFromL( const TDesC8& aNsUri,
                                                             const TDesC8& aLocalName,
                                                             const TDesC8& aQName )
    {
    iXmlReader->SetContentHandler(*this);

    switch (iState)
        {
        // no other states may be resumed(!)
        case KStateParsingCredentials:
        case KStateParsingSingleCredential:
        case KStateParsingProviderPolicy:
        case KStateParsingServicePolicy:
            {
            EndElementL(aNsUri, aLocalName, aQName);
            }
            break;
        }
    }

    
EXPORT_C TInt CSenXmlServiceDescription::HasFacetL( const TDesC8& aURI, TBool& aHasFacet )
    {
    aHasFacet = EFalse;
    RPointerArray<CSenElement> elements;
    CleanupClosePushL(elements);
    AsElement().ElementsL(elements,KSenFacet);

    const TDesC8* pValue = NULL;

    TInt count(elements.Count());
    for (TInt i=0; i<count && !aHasFacet; i++)
        {
        pValue = elements[i]->AttrValue(KFacetAttrName);
        if (pValue)
            {
            if (*pValue == aURI) aHasFacet = ETrue;
            }
        }

    CleanupStack::PopAndDestroy(); // elements
    return KErrNone;
    }

/*
EXPORT_C TInt CSenXmlServiceDescription::FacetValue(TDesC8& aURI,
                                                    HBufC8*& aValueTo)
    {
    delete aValueTo;
    aValueTo = NULL;

    TInt retVal = KErrNotFound;
    RPointerArray<CSenElement> elements;
    TRAPD(err, CleanupClosePushL(elements);)
    if (err != KErrNone)
        {
        // cleanupstack was emptied when one of the above caused a Leave
        return err;
        }
    
    TRAP(err, AsElement().ElementsL(elements,KSenFacet);)
    if (err != KErrNone)
        {
        CleanupStack::PopAndDestroy(); // elements
        return err;
        }

    const TDesC8* pValue = NULL;

    TInt count(elements.Count());
    for (TInt i=0; i<count && !aValueTo; i++)
        {
        pValue = elements[i]->AttrValue(KFacetAttrName);
        if (pValue)
            {
            if (*pValue == aURI)
                {
                if (elements[i]->Content().Length() < 1)
                    {
                    aValueTo = KSenFacetValTrue().Alloc();
                    }
                else
                    {
                    aValueTo = elements[i]->Content().Alloc();
                    }
                if (aValueTo == NULL) retVal = KErrNoMemory;
                else retVal = KErrNone;
                }
            }
        }

    CleanupStack::PopAndDestroy(); // elements
    return retVal;
    }
*/

// public, TRAPping version of FacetValue -getter:
EXPORT_C TInt CSenXmlServiceDescription::FacetValue(TDesC8& aURI,
                                                    HBufC8*& aValueTo)
    {
    TInt retVal(KErrNone);
    TInt leaveCode(KErrNone);
    TRAP(leaveCode, retVal = FacetValueL(aURI, aValueTo);)
    if (leaveCode != KErrNone) 
        {
        retVal = leaveCode;
        }
    return retVal;
    }

// private, leaving version of FacetValue -getter:
TInt CSenXmlServiceDescription::FacetValueL(TDesC8& aURI,
                                            HBufC8*& aValueTo)
    {
    delete aValueTo;
    aValueTo = NULL;

    TInt retVal = KErrNotFound;
    RPointerArray<CSenElement> elements;
    CleanupClosePushL(elements);
    
    AsElement().ElementsL(elements, KSenFacet);

    const TDesC8* pValue = NULL;

    TInt count(elements.Count());
    for (TInt i=0; i<count && !aValueTo; i++)
        {
        pValue = elements[i]->AttrValue(KFacetAttrName);
        if (pValue)
            {
            if (*pValue == aURI)
                {
                if(elements[i]->Content().Length() < 1)
                    {
                    aValueTo = KSenFacetValTrue().Alloc();
                    }
                else
                    {
                    aValueTo = elements[i]->Content().Alloc();
                    }
                if(!aValueTo) // OOM
                    {
                    retVal = KErrNoMemory;
                    }
                else 
                    {
                    retVal = KErrNone;
                    }
                }
            }
        }

    CleanupStack::PopAndDestroy(); // elements
    return retVal;
    }

/*
EXPORT_C TInt CSenXmlServiceDescription::RemoveFacet(const TDesC8& aURI)
    {
    RPointerArray<CSenElement> elements;
    TRAPD(err, 
            CleanupClosePushL(elements);
            AsElement().ElementsL(elements,KSenFacet);
         )

    if (err != KErrNone)
        {
        return err; // nothing needs to be popped from the cleanup stack
        }

    const TDesC8* pValue = NULL;
    CSenElement* pFacet = NULL;

    TInt count(elements.Count());
    for (TInt i=0; i<count; i++)
        {
        pFacet = elements[i];
        pValue = pFacet->AttrValue(KFacetAttrName);
        if (pValue)
            {
            if (*pValue == aURI)
                {
                delete AsElement().RemoveElement(*pFacet);
                }
            }
        }

    CleanupStack::PopAndDestroy(); // elements.Close()

    return KErrNone;
    }
*/
// public, TRAPping version of FacetValue -getter:
EXPORT_C TInt CSenXmlServiceDescription::RemoveFacet(const TDesC8& aURI)
    {
    TInt retVal(KErrNone);
    TInt leaveCode(KErrNone);
    TRAP(leaveCode, retVal = RemoveFacetL(aURI);)
    if (leaveCode != KErrNone)
        {
        retVal = leaveCode;
        }
    return retVal;
    }

// private, leaving version of FacetValue -getter:
TInt CSenXmlServiceDescription::RemoveFacetL(const TDesC8& aURI)
    {
    RPointerArray<CSenElement> elements;
    CleanupClosePushL(elements);
    AsElement().ElementsL(elements,KSenFacet);
    const TDesC8* pValue = NULL;
    CSenElement* pFacet = NULL;
    TInt retVal(KErrNotFound); // returned, if no match
    TInt count(elements.Count());
    for(TInt i=0; i<count; i++)
        {
        pFacet = elements[i];
        pValue = pFacet->AttrValue(KFacetAttrName);
        if(pValue)
            {
            if(*pValue == aURI)
                {
                delete AsElement().RemoveElement(*pFacet);
                retVal = KErrNone; // ok: facet removed
                }
            }
        }
    CleanupStack::PopAndDestroy(); // elements.Close()
    return retVal;
    }


EXPORT_C TInt CSenXmlServiceDescription::AddFacetL(const CSenFacet& aFacet)
    {
    TInt retVal(KErrNone);
    RPointerArray<CSenElement> elements;
    CleanupClosePushL(elements);
    AsElement().ElementsL(elements,KSenFacet);

    const TDesC8* pValue = NULL;
    CSenElement* pFacet = NULL;

    TInt count(elements.Count());
    for(TInt i=0; i<count && !pFacet; i++)
        {
        pFacet = elements[i];
        pValue = pFacet->AttrValue(KFacetAttrName);
        if(pValue)
            {
            if(*pValue != ((CSenFacet&)aFacet).Name()) 
                {
                pFacet = NULL;  
                }
            else // facet with equal name already exists!
                {
                retVal = KErrAlreadyExists;
                }
            }
        else
            {
            pFacet = NULL;
            }
        }

    CleanupStack::PopAndDestroy(); // elements.Close()

    if(pFacet && (retVal == KErrAlreadyExists))
        {
        return retVal;
        }
    else // facet did not already exist
        {
        pFacet = &AsElement().AddElementL(KSenFacet);
        pFacet->AddAttrL(KFacetAttrName,((CSenFacet&)aFacet).Name());   
        if(((CSenFacet&)aFacet).Type() != KNullDesC8)
            {
            pFacet->AddAttrL(KFacetAttrType,((CSenFacet&)aFacet).Type());
            }
        pFacet->SetContentL(((CSenFacet&)aFacet).Value());
        retVal = KErrNone;
        return retVal;
        }
    }

EXPORT_C TInt CSenXmlServiceDescription::SetFacetL(const CSenFacet& aFacet)
    {
    RPointerArray<CSenElement> elements;
    CleanupClosePushL(elements);
    AsElement().ElementsL(elements,KSenFacet);

    const TDesC8* pValue = NULL;
    CSenElement* pFacet = NULL;

    TInt count(elements.Count());
    for(TInt i=0; i<count && !pFacet; i++)
        {
        pFacet = elements[i];
        pValue = pFacet->AttrValue(KFacetAttrName);
        if (pValue)
            {
            if(*pValue != ((CSenFacet&)aFacet).Name()) pFacet = NULL;
            }
        else
            {
            pFacet = NULL;
            }
        }

    CleanupStack::PopAndDestroy(); // elements.Close()

    if(!pFacet) 
        {
        // add new facet element
        pFacet = &AsElement().AddElementL(KSenFacet);
        }

    pFacet->AddAttrL(KFacetAttrName,((CSenFacet&)aFacet).Name());
    if (((CSenFacet&)aFacet).Type() != KNullDesC8)
        {
        pFacet->AddAttrL(KFacetAttrType,((CSenFacet&)aFacet).Type());
        }
    pFacet->SetContentL(((CSenFacet&)aFacet).Value());

    return KErrNone;
    }

EXPORT_C TInt CSenXmlServiceDescription::FacetsL(RFacetArray& aFacetArray)
    {
    RPointerArray<CSenElement> elements;
    CleanupClosePushL(elements);
    AsElement().ElementsL(elements,KSenFacet);

    CSenFacet* pNewFacet = NULL;

    TInt count(elements.Count());
    for(TInt i=0; i<count; i++)
        {
        pNewFacet = CSenFacet::NewL(*elements[i]);
        aFacetArray.Append(pNewFacet);
        }

    CleanupStack::PopAndDestroy(); // elements.Close()
    return KErrNone;
    }


EXPORT_C TInt CSenXmlServiceDescription::ScoreMatchL( MSenServiceDescription& aPattern )
    {
    TInt score(0);

    if ((aPattern.Endpoint().Length() > 0) && (aPattern.Endpoint() == Endpoint()))
        {
        score++;
        }
    if ((aPattern.Contract().Length() > 0) && (aPattern.Contract() == Contract()))
        {
        score++;
        }

    RFacetArray otherFacets;
    CleanupClosePushL(otherFacets);
    aPattern.FacetsL(otherFacets);

    HBufC8* pFacetValue = NULL;
    TPtrC8 facetName;
    TPtrC8 otherFacetValue;

    TInt count(otherFacets.Count());
    for (TInt i=0; i<count; i++)
        {
        facetName.Set(otherFacets[i]->Name());
        FacetValue(facetName,pFacetValue);
        if (pFacetValue)
            {
            if (otherFacets[i]->Value().Length() < 1)
                {
                otherFacetValue.Set(KSenFacetValTrue());
                }
            else
                {
                otherFacetValue.Set(otherFacets[i]->Value());
                }
            if (*pFacetValue == otherFacetValue)
                {
                score++;
                }
            }
        delete pFacetValue;
        pFacetValue = NULL;
        }

    otherFacets.ResetAndDestroy();
    CleanupStack::Pop(); // otherFacets

    return score;
    }

EXPORT_C TBool CSenXmlServiceDescription::HasEqualPrimaryKeysL(MSenServiceDescription& aCandidate)
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
    
EXPORT_C TBool CSenXmlServiceDescription::IsLocalL()
    {
    TBool local(EFalse);

    // Check transport cue; if such XML attribute
    // has been set to <Endpoint element, it is
    // "stronger" than actual endpoint scheme
    if(ipElement)
        {
        CSenElement* child = AsElement().Element(KEndpointLocalname());
        if(child)
            {
            const TDesC8* cue = child->AttrValue(KCue);
            TInt length(KSenTransportSchemeLocal().Length());
            if(cue && cue->Length()>=length && cue->Left(length) == KSenTransportSchemeLocal)
                {
                local = ETrue; // match!
                }
            }
        }
    
    if(!local) // no match yet
        {
        // Resolve the endpoint scheme
        TPtrC8 uri = Endpoint();
        if(uri.Length()>0)
            {
            TInt index = uri.Locate(':');
            if(index!=KErrNotFound)
                {
                TPtrC8 uriScheme = uri.Left(index);
                if(uriScheme == KSenTransportSchemeLocal)
                    {
                    local = ETrue;
                    }
                }
            }
        }
    return local;            
    }

EXPORT_C TInt CSenXmlServiceDescription::SetTransportCueL(const TDesC8& aTransportCue)
    {
    TInt retVal(KErrSenNoEndpoint);
    if(ipElement)
        {
        CSenElement* child = AsElement().Element(KEndpointLocalname());
        if(child)
            {
            // Method either adds new attribute or updates existing one:
            SenXmlUtils::AddAttributeL(*child, KCue, aTransportCue);
            retVal = KErrNone;
            }
        }
    return retVal;
    }

EXPORT_C TPtrC8 CSenXmlServiceDescription::TransportCue()
    {
    if(ipElement)
        {
        CSenElement* child = AsElement().Element(KEndpointLocalname());
        if(child)
            {
            const TDesC8* value = child->AttrValue(KCue);
            if(value)
                {
                return *value;
                }
            }
        }
    return KNullDesC8();
    }

EXPORT_C MSenServicePolicy* CSenXmlServiceDescription::ServicePolicy()
    {
    if(iServicePolicy)
        {
        return (MSenServicePolicy*)iServicePolicy;
        }
    else
        {
        return NULL;
        }
    }

EXPORT_C TInt CSenXmlServiceDescription::SetPolicyL(const TDesC8& aName)
    {
    if(aName==KNullDesC8())
        return KErrArgument;
    
    CSenElement* servicePolicy = AsElement().Element(KServicePolicyLocalName); 
    CSenElement* clientPolicy = servicePolicy->Element(KClientServicePolicyLocalName);     
    if( clientPolicy == NULL )
        {
        clientPolicy = &(servicePolicy->AddElementL( KClientServicePolicyLocalName ));
        }
    clientPolicy->AddElementL(aName);        
    return KErrNone;
    }

EXPORT_C TInt CSenXmlServiceDescription::SetPolicyL(const TDesC8& aName, const TDesC8& aValue)
    {
    if(aName==KNullDesC8() || aValue == KNullDesC8())
        return KErrArgument;
    
    CSenElement* servicePolicy = AsElement().Element(KServicePolicyLocalName); 
    CSenElement* clientPolicy = servicePolicy->Element(KClientServicePolicyLocalName);     
    if( clientPolicy == NULL )
        {
        clientPolicy = & (servicePolicy->AddElementL( KClientServicePolicyLocalName ));
        }
    clientPolicy->AddElementL(aName).SetContentL(aValue);       
    return KErrNone;
    }

EXPORT_C TInt CSenXmlServiceDescription::SetPolicyL(const TDesC8& aName, const TDesC8& aValue, const TDesC8& aAttribName, const TDesC8& aAttribValue)
    {

    if(aName==KNullDesC8() || aValue == KNullDesC8() || aAttribName == KNullDesC8() || aAttribValue == KNullDesC8())
        return KErrArgument;
    
    CSenElement* servicePolicy = AsElement().Element(KServicePolicyLocalName); 
    CSenElement* clientPolicy = servicePolicy->Element(KClientServicePolicyLocalName);     
    if( clientPolicy == NULL )
        {
        clientPolicy = & (servicePolicy->AddElementL( KClientServicePolicyLocalName ));
        }
    CSenElement& ele = clientPolicy->AddElementL(aName);
    ele.SetContentL(aValue);           
    SenXmlUtils::AddAttributeL(ele, aAttribName, aAttribValue);
    return KErrNone;
    }

EXPORT_C TInt CSenXmlServiceDescription::SetProviderIdL( const TDesC8& aProviderID )
    {
    if ( aProviderID.Length() == 0 )
        {
        return KErrArgument; // zero-length provider ID
        }
    else 
        {
        SetContentOfL(KSenIdpProviderIdLocalname, aProviderID);
        return KErrNone;
        }
    }
    
EXPORT_C TPtrC8 CSenXmlServiceDescription::ProviderId()
    {
    return ContentOf( KSenIdpProviderIdLocalname );
    }

EXPORT_C void CSenXmlServiceDescription::SetPromptUserInfoL( TBool aPromptUserInfoMode )
    {
    if( aPromptUserInfoMode )
        {
#ifndef _RD_SEN_WS_STAR_DO_NOT_PROMPT_AUTHINFO_BY_DEFAULT

        // macro is disabled

        // When attribute does not exist, by default, the userinfo prompt is displayed:
        // (when authentication service tells that authentication fails)
        delete SenXmlUtils::RemoveAttributeL(AsElement(), KSenAttrPromptUserInfo);

#else 
        // macro is not in effect
				_LIT8(KTrue,"true");
        // Attribute with "true" value must *explicitely* exist, since default is "do not prompt":
        SenXmlUtils::AddAttributeL(AsElement(), KSenAttrPromptUserInfo, KTrue );	//CodeScannerWarnings
#endif        
        }
    else
        {
				_LIT8(KFalse,"false");
        // Attribute is needed only (with value "false"), when API caller wants to disable
        // end-user prompt from being displayed
        SenXmlUtils::AddAttributeL(AsElement(), KSenAttrPromptUserInfo, KFalse );	//CodeScannerWarnings
        }
    }

EXPORT_C TBool CSenXmlServiceDescription::PromptUserInfo()
    {
    _LIT8(KFalse,"false");
    TBool retVal(ETrue); // by default, prompting is enabled
    const TDesC8* value = AsElement().AttrValue( KSenAttrPromptUserInfo );
    if( value && *value == KFalse )	//CodeScannerWarnings
        {
        // Only, and only if attribute EXISTS AND it HAS value "false", 
        // the user info should NOT be prompted from end-user(!)
        retVal = EFalse;
        }
    // else retVal = ETrue;
    return retVal;    
    }
    
// End of File
