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
#include "SenFacet.h"

EXPORT_C CSenFacet* CSenFacet::NewL()
    {
    CSenFacet* pNew = new (ELeave) CSenFacet;
    CleanupStack::PushL(pNew);
    pNew->ConstructL(KSenFacet);
    CleanupStack::Pop(); // pNew
    return pNew;
    }

EXPORT_C CSenFacet* CSenFacet::NewL(
    const TDesC8& aNsUri,
    const TDesC8& aLocalName,
    const TDesC8& aQName,
    const RAttributeArray& aAttributes
    )
    {
    CSenFacet* pNew = new (ELeave) CSenFacet;
    CleanupStack::PushL(pNew);
    pNew->ConstructL(aNsUri, aLocalName, aQName, aAttributes);
    CleanupStack::Pop(); // pNew
    return pNew;
    }

EXPORT_C CSenFacet* CSenFacet::NewL(CSenElement& aCopiedSource)
    {
    CSenFacet* pNew = new (ELeave) CSenFacet;
    CleanupStack::PushL(pNew);
    pNew->ConstructL(aCopiedSource);
    CleanupStack::Pop(); // pNew;
    return pNew;
    }


EXPORT_C CSenFacet::~CSenFacet()
    {
    }

EXPORT_C CSenFacet::CSenFacet()
    {
    }

EXPORT_C void CSenFacet::ConstructL(const TDesC8& aLocalName)
    {
    BaseConstructL(aLocalName);
    }

EXPORT_C void CSenFacet::ConstructL(CSenElement& aCopiedSource)
    {
    BaseConstructL(aCopiedSource.LocalName());
    CopyFromL(aCopiedSource);
    }

EXPORT_C void CSenFacet::ConstructL(const TDesC8& aNsUri,
                             const TDesC8& aLocalName,
                             const TDesC8& aQName,
                             const RAttributeArray& aAttributes)
    {
    BaseConstructL(aNsUri, aLocalName, aQName, aAttributes);
    }

EXPORT_C void CSenFacet::SetNameL(const TDesC8& aName)
    {
    AddAttributeL(KFacetAttrName,aName);
    }

EXPORT_C void CSenFacet::SetTypeL(const TDesC8& aType)
    {
    AddAttributeL(KFacetAttrType,aType);
    }

EXPORT_C void CSenFacet::SetValueL(const TDesC8& aValue)
    {
    SetContentL(aValue);
    }

EXPORT_C TPtrC8 CSenFacet::Name()
    {
    const TDesC8* pAttrValue = AttrValue(KFacetAttrName);
    if (pAttrValue) return TPtrC8(*pAttrValue);
    else return KNullDesC8();
    }

EXPORT_C TPtrC8 CSenFacet::Type()
    {
    const TDesC8* pAttrValue = AttrValue(KFacetAttrType);
    if (pAttrValue) return TPtrC8(*pAttrValue);
    else return KNullDesC8();
    }

EXPORT_C TPtrC8 CSenFacet::Value()
    {
    return this->Content();
    }
    
void CSenFacet::CopyFromL(CSenElement& aSource)
    {
    TPtrC8 sourceContent = aSource.Content();
    if (sourceContent.Length() > 0)
        {
        if ( ! HasContent() )
            {
            SetContentL(sourceContent);
            }
        else
            {
            RWriteStream& ws = ContentWriteStreamL();
            ws.WriteL(sourceContent);
            }
        }

    RPointerArray<CSenBaseAttribute> sourceAttributes = aSource.AttributesL();
    if (sourceAttributes.Count() > 0)
        {
        for (TInt i=0;i<sourceAttributes.Count(); i++)
            {
            CSenBaseAttribute* pBaseAttribute = sourceAttributes[i];

            // duplicate check. Now overrides the original value
            CSenBaseAttribute* pOriginal = FindAttr(pBaseAttribute->Name());
            if (pOriginal)
                {
                pOriginal->SetValueL(pBaseAttribute->Value());
                continue;
                }

            CSenBaseAttribute* pNewBaseAttribute =
                CSenBaseAttribute::NewL(pBaseAttribute->Name(),
                                        pBaseAttribute->Value());
            CleanupStack::PushL(pNewBaseAttribute);
            
            RPointerArray<CSenBaseAttribute>& attributes = AttributesL();
#ifdef EKA2
            attributes.AppendL(pNewBaseAttribute);
#else
            User::LeaveIfError(attributes.Append(pNewBaseAttribute));
#endif
            CleanupStack::Pop(pNewBaseAttribute);
            }
        }

    RPointerArray<CSenElement> sourceElements = aSource.ElementsL();
    if (sourceElements.Count() > 0)
        {
        for (TInt i=0;i<sourceElements.Count(); i++)
            {
            CSenElement* pElement = sourceElements[i];
            CSenElement* pNewElement =
                                CSenBaseElement::NewL(pElement->LocalName());
            pNewElement->SetParent(this);
            CleanupStack::PushL(pNewElement);
            pNewElement->CopyFromL(*pElement);
            
            RPointerArray<CSenElement>& elements = ElementsL();
#ifdef EKA2
            elements.AppendL(pNewElement);
#else
            User::LeaveIfError(elements.Append(pNewElement));
#endif // EKA2
            CleanupStack::Pop(pNewElement);
            }
        }
    }
    
// End of File
