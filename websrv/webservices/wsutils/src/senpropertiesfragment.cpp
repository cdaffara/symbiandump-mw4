/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
#include "senpropertiesfragment.h"
#include "senpropertieselement.h"

EXPORT_C CSenPropertiesFragment* CSenPropertiesFragment::NewL(
                                        RStringPool* aStringPool)
    {
    CSenPropertiesFragment* pNew = new (ELeave) CSenPropertiesFragment;
    pNew->ipStringPool = aStringPool;
    return pNew;
    }

EXPORT_C CSenPropertiesFragment* CSenPropertiesFragment::NewL(
                                        const CSenElement& aElement,
                                        RStringPool* aStringPool)
    {
    CSenPropertiesFragment* pNew = new (ELeave) CSenPropertiesFragment;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aElement, aStringPool);
    CleanupStack::Pop(pNew);
    return pNew;
    }

EXPORT_C CSenPropertiesFragment* CSenPropertiesFragment::NewL(
                                        const TDesC8& aLocalName,
                                        RStringPool* aStringPool)
    {
    CSenPropertiesFragment* pNew = new (ELeave) CSenPropertiesFragment;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aLocalName, aStringPool);
    CleanupStack::Pop(pNew);
    return pNew;
    }

EXPORT_C CSenPropertiesFragment* CSenPropertiesFragment::NewL(
                                        const TDesC8& aNsUri,
                                        const TDesC8& aLocalName,
                                        RStringPool* aStringPool)
    {
    CSenPropertiesFragment* pNew = new (ELeave) CSenPropertiesFragment;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aNsUri, aLocalName, aStringPool);
    CleanupStack::Pop(pNew);
    return pNew;
    }

EXPORT_C CSenPropertiesFragment* CSenPropertiesFragment::NewL(
                                        const TDesC8& aNsUri,
                                        const TDesC8& aLocalName,
                                        const TDesC8& aQName,
                                        const RAttributeArray& aAttrs,
                                        RStringPool* aStringPool)
    {
    CSenPropertiesFragment* pNew = new (ELeave) CSenPropertiesFragment;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aNsUri, aLocalName, aQName, aAttrs, aStringPool);
    CleanupStack::Pop(pNew);
    return pNew;
    }

EXPORT_C CSenPropertiesFragment* CSenPropertiesFragment::NewL(
                                        const TDesC8& aNsUri,
                                        const TDesC8& aLocalName,
                                        const TDesC8& aQName,
                                        const RAttributeArray& aAttrs,
                                        CSenElement& aParent,
                                        RStringPool* aStringPool)
    {
    CSenPropertiesFragment* pNew = new (ELeave) CSenPropertiesFragment;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aNsUri, aLocalName, aQName, aAttrs, aParent,
                         aStringPool);
    CleanupStack::Pop(pNew);
    return pNew;
    }

EXPORT_C CSenPropertiesFragment* CSenPropertiesFragment::NewL(
                                        const TDesC8& aNsUri,
                                        const TDesC8& aLocalName,
                                        const TDesC8& aQName,
                                        RStringPool* aStringPool)
    {
    CSenPropertiesFragment* pNew = new (ELeave) CSenPropertiesFragment;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aNsUri, aLocalName, aQName, aStringPool);
    CleanupStack::Pop(pNew);
    return pNew;
    }

CSenPropertiesFragment::~CSenPropertiesFragment()
    {
    }

CSenPropertiesFragment::CSenPropertiesFragment()
    {
    }

void CSenPropertiesFragment::BaseConstructL(const CSenElement& aElement,
                                            RStringPool* aStringPool)
    {
    ipStringPool = aStringPool;
    ipElement = CSenPropertiesElement::NewL(aElement.LocalName(),
                                            ipStringPool);
    ipElement->CopyFromL((CSenElement&)aElement);
    }

void CSenPropertiesFragment::BaseConstructL(const TDesC8& aLocalName,
                                            RStringPool* aStringPool)
    {
    ipStringPool = aStringPool;
    ipElement = CSenPropertiesElement::NewL(aLocalName, ipStringPool);
    }

void CSenPropertiesFragment::BaseConstructL(
                                        const TDesC8& aNsUri,
                                        const TDesC8& aLocalName,
                                        RStringPool* aStringPool)
    {
    ipStringPool = aStringPool;
    ipElement = CSenPropertiesElement::NewL(aNsUri, aLocalName, ipStringPool);
    }

void CSenPropertiesFragment::BaseConstructL(
                                        const TDesC8& aNsUri,
                                        const TDesC8& aLocalName,
                                        const TDesC8& aQName,
                                        const RAttributeArray& aAttrs,
                                        RStringPool* aStringPool)
    {
    ipStringPool = aStringPool;
    ipElement = CSenPropertiesElement::NewL(aNsUri, aLocalName, aQName,
                                            aAttrs, ipStringPool);
    }

void CSenPropertiesFragment::BaseConstructL(
                                        const TDesC8& aNsUri,
                                        const TDesC8& aLocalName,
                                        const TDesC8& aQName,
                                        const RAttributeArray& aAttrs,
                                        CSenElement& aParent,
                                        RStringPool* aStringPool)
    {
    ipStringPool = aStringPool;
    // this transfers the ipElement ownership to the parent(!)
    ipElement = CSenPropertiesElement::NewL(aNsUri, aLocalName, aQName, aAttrs,
                                            aParent, ipStringPool);
    }

void CSenPropertiesFragment::BaseConstructL(
                                        const TDesC8& aNsUri,
                                        const TDesC8& aLocalName,
                                        const TDesC8& aQName,
                                        RStringPool* aStringPool
    )
    {
    ipStringPool = aStringPool;
    ipElement = CSenPropertiesElement::NewL(aNsUri, aLocalName, aQName,
                                            ipStringPool);
    }

void CSenPropertiesFragment::BaseConstructL(CSenXmlReader& aReader,
                                            RStringPool* aStringPool)
    {
    ipStringPool = aStringPool;
    CSenDomFragment::BaseConstructL(aReader);
    }
    
void CSenPropertiesFragment::ExpandL(
                                        const TDesC8& aNsUri,
                                        const TDesC8& aLocalName,
                                        const TDesC8& aQName,
                                        const RAttributeArray& aAttrs)
    {

    if(ipDomDelegate)
        {
        // We do not own the element, so remove it from the fragment.
        ipDomDelegate->ExtractElement();
        delete ipDomDelegate;
        ipDomDelegate = NULL;
        }

    // delegate parsing to a new DOMFragment,
    // until we encounter the end of an element with the given qualified name
    ipDomDelegate = CSenPropertiesFragment::NewL(aNsUri,
                                                 aLocalName,
                                                 aQName,
                                                 aAttrs,
                                                 *ipElement,
                                                 ipStringPool);

    ipElement->AddElementL(ipDomDelegate->AsElement());

    DelegateParsingL(*ipDomDelegate); // note that the parsing is actually
                                      // performed AFTER the StartElement()
                                      // method, which called this ExpandL()
                                      // has been fully executed (this is an
                                      // async signal send to the _same_
                                      // parser(!)
    }
    

void CSenPropertiesFragment::StartElementL(const TDesC8& aNsUri,
                                             const TDesC8& aLocalName,
                                             const TDesC8& aQName,
                                             const RAttributeArray& aAttrs)
    {
    if(!ipElement)
        {
        ipElement = CSenPropertiesElement::NewL(aNsUri, aLocalName, aQName,
                                                ipStringPool);
        }
        
    if ( !ipStringPool )
        {
        AllocContentSaverL();
        }

    if(iState == KStateIgnore)
        {
        const TDesC8& localName = ipElement->LocalName();
        const TDesC8& nsUri = ipElement->NamespaceURI();

        if(localName == aLocalName && nsUri == aNsUri)
            {
            iState = KStateSave; // IOP
            SetAttributesL(aAttrs);
            }
        }
    else if((iState & KStateSave) == KStateSave)
        {
        // When a new property is parsed, any existing one (with same [local] name) must be removed first
        CSenElement* pElement = AsElement().RemoveElement(aNsUri, aLocalName);
        delete pElement;
        
        ExpandL(aNsUri, aLocalName, aQName, aAttrs);
        }
    }
    
void CSenPropertiesFragment::EndElementL(
    const TDesC8& aNsUri,
    const TDesC8& aLocalName,
    const TDesC8& aQName
    )
    {
    CSenDomFragment::EndElementL(aNsUri, aLocalName, aQName);
    }
    
void CSenPropertiesFragment::CharactersL(
    const TDesC8& aChars,
    const TInt /*aStart*/,
    const TInt /*aLength*/
    )
    {
    if ( !ipStringPool )
        {
        AllocContentSaverL();

        HBufC8* pCharsCopy = NULL;
        
        if (SenXmlUtils::DecodeHttpCharactersL(aChars, pCharsCopy))
            {
            CleanupStack::PushL(pCharsCopy);
            iWs.WriteL(*pCharsCopy);
            CleanupStack::PopAndDestroy(); // pCharsCopy
            }
        else
            {
            delete pCharsCopy;
            iWs.WriteL(aChars);
            }
        }
    else
        {
        ipElement->SetContentL(aChars);
        }
    }
    
void CSenPropertiesFragment::AllocContentSaverL()
    {
    if ( !ipStringPool )
        {
        CSenBaseFragment::AllocContentSaverL();
        }
    }    
    
RStringPool& CSenPropertiesFragment::StringPool()
    {
    return *ipStringPool;
    }
    
void CSenPropertiesFragment::SetStringPool(RStringPool& aStringPool)
    {
    ipStringPool = &aStringPool;
    }
    
TBool CSenPropertiesFragment::HasStringPool() const
    {
    return ( ipStringPool ) ? ETrue : EFalse;
    }

// END OF FILE



