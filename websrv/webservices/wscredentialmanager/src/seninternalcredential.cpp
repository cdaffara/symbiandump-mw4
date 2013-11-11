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
#include "seninternalcredential.h"
#include "SenFacet.h"
#include <SenXmlElement.h> // check if this include is needed(?)
#include "senwsdescription.h"
#include "SenDateUtils.h"
#include <SenIdentityProvider.h>
#include "senlogger.h"


#ifdef SYMBIAN_SECURE_ECOM
    // for S60 FP3 (2.8) platform or newer (3.0 and above)
    #include <xml/attribute.h> // needed for RAttributeArray
//#else  // for S60 FP2 platform (2.6) or older
    //#include "Attribute.h"
#endif

#include <xml/dom/xmlengnodelist.h> 
using namespace Xml;

namespace
    {
    _LIT8(KProviderIdLocalName,  "ProviderID");
    _LIT8(KUserName,             "AuthzID");
    _LIT8(KPassword,             "Password");
    const TInt KCredIdBufSize    = 128;    
    }

EXPORT_C CSenInternalCredential* CSenInternalCredential::NewL()
    {
    CSenInternalCredential* pNew = NewLC();
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

EXPORT_C CSenInternalCredential* CSenInternalCredential::NewLC()
    {
    CSenInternalCredential* pNew = new (ELeave) CSenInternalCredential;
    CleanupStack::PushL(pNew);
    pNew->ConstructL(KNullDesC8);
    return pNew;
    }

EXPORT_C CSenInternalCredential* CSenInternalCredential::NewL(
                                           	const TDesC8& aNsUri,
                                            const TDesC8& aLocalName,
                                            const TDesC8& aQName,
                                            const RAttributeArray& aAttributes)
    {
    CSenInternalCredential* pNew = NewLC(aNsUri, aLocalName, aQName, aAttributes);
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

EXPORT_C CSenInternalCredential* CSenInternalCredential::NewLC(
                                            const TDesC8& aNsUri,
                                            const TDesC8& aLocalName,
                                            const TDesC8& aQName,
                                            const RAttributeArray& aAttributes)
    {
    CSenInternalCredential* pNew = new (ELeave) CSenInternalCredential;
    CleanupStack::PushL(pNew);
    pNew->ConstructL(aNsUri, aLocalName, aQName, aAttributes);
    return pNew;
    }

EXPORT_C CSenInternalCredential* CSenInternalCredential::NewL(
                                            const TDesC8& aNsUri,
                                            const TDesC8& aLocalName,
                                            const TDesC8& aQName,
                                            const RAttributeArray& aAttributes,
                                            TXmlEngElement& aParent)
    {
    CSenInternalCredential* pNew = NewLC(aNsUri, aLocalName,
                                         aQName, aAttributes, aParent);
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

EXPORT_C CSenInternalCredential* CSenInternalCredential::NewLC(
                                            const TDesC8& aNsUri,
                                            const TDesC8& aLocalName,
                                            const TDesC8& aQName,
                                            const RAttributeArray& aAttributes,
                                            TXmlEngElement& aParent)
    {
    CSenInternalCredential* pNew = new (ELeave) CSenInternalCredential;
    CleanupStack::PushL(pNew);
    pNew->ConstructL(aNsUri, aLocalName, aQName, aAttributes, aParent);
    return pNew;
    }

EXPORT_C CSenInternalCredential* CSenInternalCredential::NewL(
                                            const TDesC8& aNsUri,
                                            const TDesC8& aLocalName,
                                            const TDesC8& aQName,
                                            const RAttributeArray& aAttributes,
                                            TXmlEngElement& aParent,
                                            RSenDocument& aOwnerDocument)
    {
    CSenInternalCredential* pNew = NewLC(aNsUri, aLocalName,
                                         aQName, aAttributes,
                                         aParent, aOwnerDocument);
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

EXPORT_C CSenInternalCredential* CSenInternalCredential::NewLC(
                                            const TDesC8& aNsUri,
                                            const TDesC8& aLocalName,
                                            const TDesC8& aQName,
                                            const RAttributeArray& aAttributes,
                                            TXmlEngElement& aParent,
                                            RSenDocument& aOwnerDocument)
    {
    CSenInternalCredential* pNew = new (ELeave) CSenInternalCredential;
    CleanupStack::PushL(pNew);
    pNew->ConstructL(aNsUri, aLocalName, aQName, aAttributes, aParent,
                     aOwnerDocument);
    return pNew;
    }

EXPORT_C CSenInternalCredential* CSenInternalCredential::NewL(
                                             const CSenInternalCredential& aCredential)
    {
    CSenInternalCredential* pNew = NewLC(aCredential);
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

EXPORT_C CSenInternalCredential* CSenInternalCredential::NewLC(
                                             const CSenInternalCredential& aCredential)
    {
    CSenInternalCredential* pNew = new (ELeave) CSenInternalCredential;
    CleanupStack::PushL(pNew);
    pNew->ConstructL((CSenInternalCredential&)aCredential);
    return pNew;
    }


EXPORT_C void CSenInternalCredential::ConstructL(CSenInternalCredential& aCredential)
    {
    CSenCredential2::BaseConstructL(aCredential);
    }

EXPORT_C void CSenInternalCredential::ConstructL(const TDesC8&  aNsUri,
                                          const TDesC8&  aLocalName,
                                          const TDesC8&  aQName,
                                          const RAttributeArray& aAttributes,
                                          TXmlEngElement& aParent)
    {
    CSenCredential2::BaseConstructL(aNsUri, aLocalName, aQName, aAttributes, aParent);
    }

EXPORT_C void CSenInternalCredential::ConstructL(const TDesC8&  aNsUri,
                                          const TDesC8&  aLocalName,
                                          const TDesC8&  aQName,
                                          const RAttributeArray& aAttributes,
                                          TXmlEngElement& aParent,
                                          RSenDocument& aOwnerDocument)
    {
    CSenCredential2::BaseConstructL(aNsUri, aLocalName, aQName, aAttributes, aParent, aOwnerDocument);

    }

EXPORT_C void CSenInternalCredential::ConstructL(const TDesC8&  aNsUri,
                                          const TDesC8&  aLocalName,
                                          const TDesC8&  aQName,
                                          const RAttributeArray& aAttributes)
    {
    CSenCredential2::BaseConstructL(aNsUri, aLocalName, aQName, aAttributes);
    }
    
EXPORT_C void CSenInternalCredential::ConstructL(const TDesC8& aLocalName)
    {
    CSenFragmentBase::BaseConstructL(aLocalName);
    }

EXPORT_C CSenInternalCredential::CSenInternalCredential()
    {
    }

EXPORT_C CSenInternalCredential::~CSenInternalCredential()
    {
    delete ipIdentifier;
    delete ipProperties;
    }

EXPORT_C TBool CSenInternalCredential::IsApplicableL(const CSenWSDescription& aPattern)	//codescannerwarings
    {
    TPtrC8 value;
    CSenElement* valueElement;
    CSenElement& patternElement = ((CSenWSDescription&)aPattern).AsElement();
    CSenCredentialIdentifier& identifier = IdentifierL();	//codescannerwarings

    RXmlEngNodeList<TXmlEngAttr> attrList;

    RXmlEngNodeList<TXmlEngElement> list;
    CleanupClosePushL(list);
    TXmlEngElement element = identifier.AsElementL();
    element.GetChildElements(list);
    while ( list.HasNext() )
        {
        TXmlEngElement element = list.Next();
        if ( (element.Name() != KSenCredentialId) &&
             (element.Name() != KProviderIdLocalName)  &&
             (element.Name() != KSenIdpAuthzIDLocalname) &&
             (element.Name() != KSenIdpPasswordLocalname) )
            {
            HBufC8* pTag = element.Name().AllocLC();
            CleanupStack::PopAndDestroy(pTag);
                        
            valueElement = patternElement.Element(element.Name());
            if ( valueElement )
                {
                HBufC8* pElement1 = valueElement->Content().AllocLC();
                HBufC8* pElement2 = element.Text().AllocLC();
                CleanupStack::PopAndDestroy(pElement2);
                CleanupStack::PopAndDestroy(pElement1);

                if ( valueElement->Content() != element.Text() )
                    {
                    CleanupStack::PopAndDestroy(&list); // Close()
                    return EFalse;
                    }
                else
                    {
                    element.GetAttributes(attrList);
                    CleanupClosePushL(attrList);
                    while ( attrList.HasNext() )
                        {
                        TXmlEngAttr attr = attrList.Next();
                        if ( *valueElement->AttrValue(attr.Name()) != attr.Value() )
                            {
                            CleanupStack::PopAndDestroy(&attrList); // Close()
                            CleanupStack::PopAndDestroy(&list); // Close()
                            return EFalse;
                            }
                        }
                    CleanupStack::PopAndDestroy(&attrList); // Close()
                    }
                }
            else
                {
                CleanupStack::PopAndDestroy(&list); // Close()
                return EFalse;
                }
            }
        }

    CleanupStack::PopAndDestroy(&list); // Close()
    return ETrue;
    }

EXPORT_C void CSenInternalCredential::SetIdentifier(CSenCredentialIdentifier* aIdentifier)
    {
    if ( ipIdentifier )
        {
        delete ipIdentifier;
        }

    ipIdentifier = aIdentifier;
    }

EXPORT_C CSenCredentialIdentifier& CSenInternalCredential::IdentifierL()	//codescannerwarings
    {
    if ( !ipIdentifier )
        {
        ipIdentifier = CSenCredentialIdentifier::NewL();
        }

    return *ipIdentifier;
    }

EXPORT_C void CSenInternalCredential::SetProperties(CSenCredentialProperties* aProperties)
    {
    if ( ipProperties )
        {
        delete ipProperties;
        }

    ipProperties = aProperties;
    }

EXPORT_C CSenCredentialProperties& CSenInternalCredential::PropertiesL()	//codescannerwarings
    {
    if ( !ipProperties )
        {
        ipProperties = CSenCredentialProperties::NewL();
        }

    return *ipProperties;
    }

EXPORT_C TBool CSenInternalCredential::HasProperties()
    {
    if ( !ipProperties )
        {
        return EFalse;
        }
    else 
        {
        return ETrue;
        }
    }

EXPORT_C void CSenInternalCredential::SetIdentityProviderIdL(TDesC8& aIdentityProviderId)
    {
    IdentifierL().SetPropertyL(KProviderIdLocalName, aIdentityProviderId);	//codescannerwarings
    }
EXPORT_C void CSenInternalCredential::SetAuthInfo(TDesC8& aUserId, TDesC8& aPassword)
    {
		TInt retVal(KErrNone);
    TRAP(retVal,
    		IdentifierL().SetPropertyL(KUserName, aUserId);
    		IdentifierL().SetPropertyL(KPassword, aPassword);
    		);
    }

EXPORT_C TInt CSenInternalCredential::IdentityProviderIdL(TPtrC8& aIdTo)
    {
    return IdentifierL().PropertyL(KProviderIdLocalName, aIdTo);	//codescannerwarings
    }

EXPORT_C void CSenInternalCredential::SetSession(CSenServiceSession& aSession)
    {
    ipSession = &aSession;
    }

EXPORT_C CSenServiceSession* CSenInternalCredential::Session()
    {
    return ipSession;
    }




EXPORT_C CSenCredentialProperties* CSenCredentialProperties::NewL()
    {
    CSenCredentialProperties* pNew = NewLC();
    CleanupStack::Pop(pNew);
    return pNew;
    }

EXPORT_C CSenCredentialProperties* CSenCredentialProperties::NewLC()
    {
    CSenCredentialProperties* pNew = new (ELeave) CSenCredentialProperties;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(KSenCredentialProperteisLocalname);
    return pNew;
    }

EXPORT_C CSenCredentialProperties* CSenCredentialProperties::NewL(
                                            const TDesC8& aNsUri,
                                            const TDesC8& aLocalName,
                                            const TDesC8& aQName,
                                            const RAttributeArray& aAttributes)
    {
    CSenCredentialProperties* pNew = NewLC(aNsUri, aLocalName, aQName, aAttributes);
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

EXPORT_C CSenCredentialProperties* CSenCredentialProperties::NewLC(
                                            const TDesC8& aNsUri,
                                            const TDesC8& aLocalName,
                                            const TDesC8& aQName,
                                            const RAttributeArray& aAttributes)
    {
    CSenCredentialProperties* pNew = new (ELeave) CSenCredentialProperties;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aNsUri, aLocalName, aQName, aAttributes);
    return pNew;
    }

EXPORT_C CSenCredentialProperties* CSenCredentialProperties::NewL(
                                            const TDesC8& aNsUri,
                                            const TDesC8& aLocalName,
                                            const TDesC8& aQName,
                                            const RAttributeArray& aAttributes,
                                            TXmlEngElement& aParent)
    {
    CSenCredentialProperties* pNew = NewLC(aNsUri, aLocalName,
                                           aQName, aAttributes, aParent);
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

EXPORT_C CSenCredentialProperties* CSenCredentialProperties::NewLC(
                                            const TDesC8& aNsUri,
                                            const TDesC8& aLocalName,
                                            const TDesC8& aQName,
                                            const RAttributeArray& aAttributes,
                                            TXmlEngElement& aParent)
    {
    CSenCredentialProperties* pNew = new (ELeave) CSenCredentialProperties;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aNsUri, aLocalName, aQName, aAttributes, aParent);
    return pNew;
    }

EXPORT_C CSenCredentialProperties* CSenCredentialProperties::NewL(
                                            const TDesC8& aNsUri,
                                            const TDesC8& aLocalName,
                                            const TDesC8& aQName,
                                            const RAttributeArray& aAttributes,
                                            TXmlEngElement& aParent,
                                            RSenDocument& aOwnerDocument)
    {
    CSenCredentialProperties* pNew = NewLC(aNsUri, aLocalName,
                                           aQName, aAttributes,
                                           aParent, aOwnerDocument);
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

EXPORT_C CSenCredentialProperties* CSenCredentialProperties::NewLC(
                                            const TDesC8& aNsUri,
                                            const TDesC8& aLocalName,
                                            const TDesC8& aQName,
                                            const RAttributeArray& aAttributes,
                                            TXmlEngElement& aParent,
                                            RSenDocument& aOwnerDocument)
    {
    CSenCredentialProperties* pNew = new (ELeave) CSenCredentialProperties;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aNsUri, aLocalName, aQName, aAttributes, aParent,
                         aOwnerDocument);
    return pNew;
    }

EXPORT_C CSenCredentialProperties::~CSenCredentialProperties()
    {
    }

CSenCredentialProperties::CSenCredentialProperties()
    {
    }

EXPORT_C TInt CSenCredentialProperties::SetPropertyL(const TDesC8& aName,
                                                         const TDesC8& aValue)
    {
    TXmlEngElement element = AsElementL();
    RXmlEngNodeList<TXmlEngElement> list;
    CleanupClosePushL(list);

    element.GetElementsByTagNameL(list, aName);

    if ( list.Count() > 0 )
        {
        TXmlEngElement firstElement = list.Next();
        firstElement.SetTextNoEncL(aValue);
        CleanupStack::PopAndDestroy(&list); // Close();
        
        return KErrNone;
        }
    CleanupStack::PopAndDestroy(&list);

    TXmlEngElement newElement = element.AddNewElementL(aName);
    newElement.SetTextNoEncL(aValue);
    return KErrNone;
    }

EXPORT_C TInt CSenCredentialProperties::PropertyL(const TDesC8& aName, TPtrC8& aValue)
    {
    TInt retVal(KErrNone);

    TXmlEngElement element = AsElementL();
    RXmlEngNodeList<TXmlEngElement> list;
    CleanupClosePushL(list);

    element.GetElementsByTagNameL(list, aName);

    if ( list.Count() > 0 )
        {
        TXmlEngElement firstElement = list.Next();
        aValue.Set(firstElement.Text());
        }
    else
        {
        retVal = KErrNotFound;
        }

    CleanupStack::PopAndDestroy(&list);
    return retVal;
    }

EXPORT_C TInt CSenCredentialProperties::FacetValueL(TDesC8& aURI,
                                                        HBufC8*& aValueTo)
    {
    TInt retVal(KErrNotFound);

    TXmlEngElement element = AsElementL();
    RXmlEngNodeList<TXmlEngElement> list;
    CleanupClosePushL(list);

    element.GetElementsByTagNameL(list, KSenFacet);

    if ( list.Count() > 0 )
        {
        TXmlEngElement element;
        TPtrC8 value;

        while ( list.HasNext() )
            {
            element = list.Next();
            value.Set(element.AttributeValueL(KFacetAttrName));
            if ( value == aURI )
                {
                if ( element.Text().Length() < 1 )
                    {
                    aValueTo = KSenFacetValTrue().Alloc();
                    }
                else
                    {
                    aValueTo = element.Text().Alloc();
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

    CleanupStack::PopAndDestroy(&list);

    return retVal;
    }

EXPORT_C TInt CSenCredentialProperties::RemoveFacetL(const TDesC8& aURI)
    {
    TInt retVal(KErrNotFound);

    TXmlEngElement element = AsElementL();
    RXmlEngNodeList<TXmlEngElement> list;
    CleanupClosePushL(list);

    element.GetElementsByTagNameL(list, KSenFacet);

    if ( list.Count() > 0 )
        {
        TXmlEngElement element;
        TPtrC8 value;

        while ( list.HasNext() )
            {
            element = list.Next();
            value.Set(element.AttributeValueL(KFacetAttrName));
            if ( value == aURI )
                {
                element.Remove();
                retVal = KErrNone;
                }
            }
        }

    CleanupStack::PopAndDestroy(&list);
    return retVal;
    }


EXPORT_C TInt CSenCredentialProperties::AddFacetL(const CSenFacet& aFacet)
    {
    TInt retVal(KErrNotFound);

    TXmlEngElement element = AsElementL();
    RXmlEngNodeList<TXmlEngElement> list;
    CleanupClosePushL(list);

    element.GetElementsByTagNameL(list, KSenFacet);

    if ( list.Count() > 0 )
        {
        TXmlEngElement element;
        TPtrC8 value;

        while ( list.HasNext() )
            {
            element = list.Next();
            value.Set(element.AttributeValueL(KFacetAttrName));
            if ( value == ((CSenFacet&)aFacet).Name() )
                {
                retVal = KErrAlreadyExists;
                }
            }
        }

    CleanupStack::PopAndDestroy(&list); // Close()

    if ( retVal != KErrAlreadyExists)
        {
        TXmlEngElement newElement = element.AddNewElementL(KSenFacet);
        newElement.SetAttributeL(KFacetAttrName, ((CSenFacet&)aFacet).Name());
        if(((CSenFacet&)aFacet).Type() != KNullDesC8)
            {
            newElement.SetAttributeL(KFacetAttrType,((CSenFacet&)aFacet).Type());
            }
        newElement.SetTextNoEncL(((CSenFacet&)aFacet).Value());
        retVal = KErrNone;
        }

    return retVal;
    }

EXPORT_C TInt CSenCredentialProperties::SetFacetL(const CSenFacet& aFacet)
    {
    TInt retVal(KErrNotFound);

    TXmlEngElement element = AsElementL();
    RXmlEngNodeList<TXmlEngElement> list;
    CleanupClosePushL(list);

    element.GetElementsByTagNameL(list, KSenFacet);

    if ( list.Count() > 0 )
        {
        TXmlEngElement element;
        TPtrC8 value;

        while ( list.HasNext() && retVal == KErrNotFound )
            {
            element = list.Next();
            value.Set(element.AttributeValueL(KFacetAttrName));
            if ( value == ((CSenFacet&)aFacet).Name() )
                {
                if(((CSenFacet&)aFacet).Type() != KNullDesC8)
                    {
                    element.SetAttributeL(KFacetAttrType,((CSenFacet&)aFacet).Type());
                    }
                element.SetTextNoEncL(((CSenFacet&)aFacet).Value());
                retVal = KErrNone;
                }
            }
        }

    CleanupStack::PopAndDestroy(&list); // Close()

    if ( retVal == KErrNotFound )
        {
        TXmlEngElement newElement = element.AddNewElementL(KSenFacet);
        newElement.SetAttributeL(KFacetAttrName, ((CSenFacet&)aFacet).Name());
        if(((CSenFacet&)aFacet).Type() != KNullDesC8)
            {
            newElement.SetAttributeL(KFacetAttrType,((CSenFacet&)aFacet).Type());
            }
        newElement.SetTextNoEncL(((CSenFacet&)aFacet).Value());
        retVal = KErrNone;
        }

    return retVal;
    }

EXPORT_C void CSenCredentialProperties::SetValidUntilL(TTime aValidUntil)
    {
    TXmlEngElement element = AsElementL();
    RXmlEngNodeList<TXmlEngElement> list;
    CleanupClosePushL(list);

    element.GetElementsByTagNameL(list, KSenCredentialValidUntil);

    TXmlEngElement validUntilElement;
    if ( list.Count() > 0 )
        {
        validUntilElement = list.Next();
        }
    else
        {
        validUntilElement = element.AddNewElementL(KSenCredentialValidUntil);
        }

    HBufC8* pDateDes = HBufC8::NewLC(KCredIdBufSize);
    TPtr8 datePtr = pDateDes->Des();
    SenDateUtils::ToXmlDateTimeUtf82L(datePtr, aValidUntil);
    validUntilElement.SetTextL(*pDateDes);
    CleanupStack::PopAndDestroy(pDateDes);

    CleanupStack::PopAndDestroy(&list); // Close()
    }
        
EXPORT_C TTime CSenCredentialProperties::ValidUntilL()
    {
    TTime retTime = Time::NullTTime();

    TXmlEngElement element = AsElementL();
    RXmlEngNodeList<TXmlEngElement> list;
    CleanupClosePushL(list);

    element.GetElementsByTagNameL(list, KSenCredentialValidUntil);

    if ( list.Count() > 0 )
        {
        TXmlEngElement validUntilElement = list.Next();
        retTime = SenDateUtils::FromXmlDateTimeL(validUntilElement.Text());
        }

    CleanupStack::PopAndDestroy(&list); // Close()

    return retTime;
    }

EXPORT_C CSenCredentialIdentifier* CSenCredentialIdentifier::NewL()
    {
    CSenCredentialIdentifier* pNew = NewLC();
    CleanupStack::Pop(pNew);
    return pNew;
    }

EXPORT_C CSenCredentialIdentifier* CSenCredentialIdentifier::NewLC()
    {
    CSenCredentialIdentifier* pNew = new (ELeave) CSenCredentialIdentifier;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(KSenCredentialIdentifierLocalname);
    return pNew;
    }

EXPORT_C CSenCredentialIdentifier* CSenCredentialIdentifier::NewL(
                                            const TDesC8& aNsUri,
                                            const TDesC8& aLocalName,
                                            const TDesC8& aQName,
                                            const RAttributeArray& aAttributes)
    {
    CSenCredentialIdentifier* pNew = NewLC(aNsUri, aLocalName, aQName, aAttributes);
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

EXPORT_C CSenCredentialIdentifier* CSenCredentialIdentifier::NewLC(
                                            const TDesC8& aNsUri,
                                            const TDesC8& aLocalName,
                                            const TDesC8& aQName,
                                            const RAttributeArray& aAttributes)
    {
    CSenCredentialIdentifier* pNew = new (ELeave) CSenCredentialIdentifier;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aNsUri, aLocalName, aQName, aAttributes);
    return pNew;
    }

EXPORT_C CSenCredentialIdentifier* CSenCredentialIdentifier::NewL(
                                            const TDesC8& aNsUri,
                                            const TDesC8& aLocalName,
                                            const TDesC8& aQName,
                                            const RAttributeArray& aAttributes,
                                            TXmlEngElement& aParent)
    {
    CSenCredentialIdentifier* pNew = NewLC(aNsUri, aLocalName,
                                           aQName, aAttributes, aParent);
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

EXPORT_C CSenCredentialIdentifier* CSenCredentialIdentifier::NewLC(
                                            const TDesC8& aNsUri,
                                            const TDesC8& aLocalName,
                                            const TDesC8& aQName,
                                            const RAttributeArray& aAttributes,
                                            TXmlEngElement& aParent)
    {
    CSenCredentialIdentifier* pNew = new (ELeave) CSenCredentialIdentifier;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aNsUri, aLocalName, aQName, aAttributes, aParent);
    return pNew;
    }

EXPORT_C CSenCredentialIdentifier* CSenCredentialIdentifier::NewL(
                                            const TDesC8& aNsUri,
                                            const TDesC8& aLocalName,
                                            const TDesC8& aQName,
                                            const RAttributeArray& aAttributes,
                                            TXmlEngElement& aParent,
                                            RSenDocument& aOwnerDocument)
    {
    CSenCredentialIdentifier* pNew = NewLC(aNsUri, aLocalName,
                                           aQName, aAttributes,
                                           aParent, aOwnerDocument);
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

EXPORT_C CSenCredentialIdentifier* CSenCredentialIdentifier::NewLC(
                                            const TDesC8& aNsUri,
                                            const TDesC8& aLocalName,
                                            const TDesC8& aQName,
                                            const RAttributeArray& aAttributes,
                                            TXmlEngElement& aParent,
                                            RSenDocument& aOwnerDocument)
    {
    CSenCredentialIdentifier* pNew = new (ELeave) CSenCredentialIdentifier;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aNsUri, aLocalName, aQName, aAttributes, aParent,
                         aOwnerDocument);
    return pNew;
    }

EXPORT_C CSenCredentialIdentifier::~CSenCredentialIdentifier()
    {
    }

CSenCredentialIdentifier::CSenCredentialIdentifier()
    {
    }

EXPORT_C void CSenCredentialIdentifier::SetIdL(TInt aId)
    {
    TXmlEngElement element = AsElementL();
    RXmlEngNodeList<TXmlEngElement> list;
    CleanupClosePushL(list);

    element.GetElementsByTagNameL(list, KSenCredentialId);

    TXmlEngElement idElement;
    if ( list.Count() > 0 )
        {
        idElement = list.Next();
        }
    else
        {
        idElement = element.AddNewElementL(KSenCredentialId);
        }

    TBuf8<KCredIdBufSize> buffer;
    buffer.Num(aId);
    idElement.SetTextL(buffer);

    CleanupStack::PopAndDestroy(&list); // Close()
    }

EXPORT_C TInt CSenCredentialIdentifier::IdL()
    {
    TInt retVal(KErrNotFound);

    TXmlEngElement element = AsElementL();
    RXmlEngNodeList<TXmlEngElement> list;
    CleanupClosePushL(list);

    element.GetElementsByTagNameL(list, KSenCredentialId);

    if ( list.Count() > 0 )
        {
        TXmlEngElement idElement = list.Next();
        TLex8 lex;
        lex.Assign(idElement.Text());
        lex.Val( retVal );
        }

    CleanupStack::PopAndDestroy(&list); // Close()

    return retVal;
    }
    
CSenCredentialData* CSenCredentialData::NewL()
    {
    CSenCredentialData* pNew = CSenCredentialData::NewLC();
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

CSenCredentialData* CSenCredentialData::NewLC()
    {
    CSenCredentialData* pNew = new (ELeave) CSenCredentialData;
    CleanupStack::PushL(pNew);
    return pNew;
    }

CSenCredentialData::~CSenCredentialData()
    {
    iObserverArray.Close();
    }
    
CSenCredentialData::CSenCredentialData()
    {
    }
    
EXPORT_C void RSenCredentialPtr::OpenL(CSenInternalCredential* apCredential)
    {
    CREDLOG_L(KSenCredsLogLevelMax,"CredentialPtr OPEN");

    ipCredentialData = CSenCredentialData::NewL();
    ipCredentialData->iCounter = 1;
    ipCredentialData->ipCredential = apCredential;
    CREDLOG_L(KSenCredsLogLevelMax,".....END");
    }
    
EXPORT_C CSenInternalCredential* RSenCredentialPtr::Credential()
    {
    if ( ipCredentialData )
        {
        return ipCredentialData->ipCredential;
        }
    else
        {
        return NULL;
        }
    }
    
EXPORT_C void RSenCredentialPtr::SetCredential(CSenInternalCredential* apCredential)
    {
    CREDLOG_L(KSenCredsLogLevelMax,"CredentialPtr SET");
    if ( ipCredentialData )
        {
        delete ipCredentialData->ipCredential;
        ipCredentialData->ipCredential = apCredential;

        TInt count = ipCredentialData->iObserverArray.Count();
        for (TInt i=0; i < count; i++)
            {
            ipCredentialData->iObserverArray[i]->CredentialChanged(MSenCredentialObserver::EReplaced);
            }
        }
    CREDLOG_L(KSenCredsLogLevelMax,".....END");
    }
    
EXPORT_C RSenCredentialPtr RSenCredentialPtr::Clone()
    {
    if ( ipCredentialData )
        {
        CREDLOG_L(KSenCredsLogLevelMax,"CredentialPtr CLONE");
        (ipCredentialData->iCounter)++;
#ifdef _SENDEBUG
TRAP_IGNORE
    (
            _LIT8(KCredsLogLine, "....counter = %d");
            CREDLOG_FORMAT((KSenCredsLogChannel, KSenCredsLogLevelMax, KCredsLogLine,  ipCredentialData->iCounter));
            CREDLOG_L(KSenCredsLogLevelMax,"....ticket ");
            if (ipCredentialData->ipCredential)
                {
                HBufC8* ptr = ipCredentialData->ipCredential->AsXmlL();
                CleanupStack::PushL(ptr);
                CREDLOG_ALL(KSenCredsLogLevelMax, *ptr);
                CleanupStack::PopAndDestroy(ptr);    
                }
            else
                {
                CREDLOG_L(KSenCredsLogLevelMax,"....NULL");
                }
        CREDLOG_L(KSenCredsLogLevelMax,"....END");
    )        
#endif // _SENDEBUG
        }
    return *this;
    }
    
EXPORT_C void RSenCredentialPtr::Close()
    {
    if ( ipCredentialData )
        {
        CREDLOG_L(KSenCredsLogLevelMax,"CredentialPtr CLOSE");
        (ipCredentialData->iCounter)--;
#ifdef _SENDEBUG
TRAP_IGNORE
    (
            _LIT8(KCredsLogLine, "....counter = %d");
            CREDLOG_FORMAT((KSenCredsLogChannel, KSenCredsLogLevelMax, KCredsLogLine, ipCredentialData->iCounter));
            CREDLOG_L(KSenCredsLogLevelMax,"....ticket ");
            if (ipCredentialData->ipCredential)
                {
                HBufC8* ptr = ipCredentialData->ipCredential->AsXmlL();
                CleanupStack::PushL(ptr);
                CREDLOG_ALL(KSenCredsLogLevelMax, *ptr);
                CleanupStack::PopAndDestroy(ptr);    
                }
            else
                {
                CREDLOG_L(KSenCredsLogLevelMax,"....NULL");
                }
    )                
#endif // _SENDEBUG
        if ( ipCredentialData->iCounter == 0 )
            {
            CREDLOG_L(KSenCredsLogLevelMax,"....deleting Credential");
            delete ipCredentialData->ipCredential;
            ipCredentialData->ipCredential = NULL;
            TInt count = ipCredentialData->iObserverArray.Count();
            for (TInt i=0; i < count; i++)
                {
                ipCredentialData->iObserverArray[i]->CredentialChanged(MSenCredentialObserver::EDestroyed);
                }

            delete ipCredentialData;
            }
        ipCredentialData = NULL;
        CREDLOG_L(KSenCredsLogLevelMax,"....END");
        }
    }

EXPORT_C void RSenCredentialPtr::CloseAndDestroyCredential()
    {
    if ( ipCredentialData )
        {
        delete ipCredentialData->ipCredential;
        ipCredentialData->ipCredential = NULL;
        
        TInt count = ipCredentialData->iObserverArray.Count();
        for (TInt i=0; i < count; i++)
            {
            ipCredentialData->iObserverArray[i]->CredentialChanged(MSenCredentialObserver::EDestroyed);
            }
CREDLOG_L(KSenCredsLogLevelMax,"CredentialPtr CLOSEANDDESTROY");
        
        (ipCredentialData->iCounter)--;
        if ( ipCredentialData->iCounter == 0 )
            {
            CREDLOG_L(KSenCredsLogLevelMax,".....deleting");
            delete ipCredentialData;
            ipCredentialData = NULL;
            }
CREDLOG_L(KSenCredsLogLevelMax,".....END");
        }
    }

EXPORT_C void RSenCredentialPtr::AddCredentialObserverL(MSenCredentialObserver& aObserver)	//codescannerwarnings
    {
    if ( ipCredentialData )
        {
        TInt leaveCode( KErrNone );
        TRAP( leaveCode, ipCredentialData->iObserverArray.AppendL(&aObserver); )
#ifdef _SENDEBUG        
        if( leaveCode )
            {
            CREDLOG_L(KSenCredsLogLevelMin, "RSenCredentialPtr::AddCredentialObserverL:");
            _LIT8(KCredsLogLine, "- ipCredentialData->iObserverArray.AppendL leaved (%d)!");
            CREDLOG_FORMAT(( KSenCredsLogChannel, KSenCredsLogLevelMax, KCredsLogLine, leaveCode ));
            }
#endif // _SENDEBUG
        leaveCode = 0; // ignored in release builds
        }
    }

EXPORT_C TInt RSenCredentialPtr::RemoveCredentialObserver(MSenCredentialObserver& aObserver)
    {
    if ( ipCredentialData )
        {
        TInt pos = ipCredentialData->iObserverArray.Find(&aObserver);
        if ( pos != KErrNotFound )
            {
            ipCredentialData->iObserverArray.Remove(pos);
            }
        else
            {
            return pos;
            }
        }
        
    return KErrNone;
    }

EXPORT_C RSenCredentialPtr::RSenCredentialPtr()
: ipCredentialData(NULL)
    {
    }

// END OF FILE
