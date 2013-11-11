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
#include "SenXmlElement.h"
#include "SenDomFragment.h"
#include "SenXmlConstants.h"
#include "SenXmlUtils.h"
#include "senxmldebug.h"

EXPORT_C CSenDomFragment* CSenDomFragment::NewL()
    {
    CSenDomFragment* pNew = new (ELeave) CSenDomFragment;
    return pNew;
    }

EXPORT_C CSenDomFragment* CSenDomFragment::NewL(const CSenElement& aElement)
    {
    CSenDomFragment* pNew = new (ELeave) CSenDomFragment;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aElement);
    CleanupStack::Pop(); // delete pNew;
    return pNew;
    }

EXPORT_C CSenDomFragment* CSenDomFragment::NewL(const TDesC8& aLocalName)
    {
    CSenDomFragment* pNew = new (ELeave) CSenDomFragment;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aLocalName);
    CleanupStack::Pop(); // delete pNew;
    return pNew;
    }

EXPORT_C CSenDomFragment* CSenDomFragment::NewL(
    const TDesC8& aNsUri,
    const TDesC8& aLocalName
    )
    {
    CSenDomFragment* pNew = new (ELeave) CSenDomFragment;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aNsUri, aLocalName);
    CleanupStack::Pop(); // delete pNew;
    return pNew;
    }

EXPORT_C CSenDomFragment* CSenDomFragment::NewL(
    const TDesC8& aNsUri,
    const TDesC8& aLocalName,
    const TDesC8& aQName,
    const RAttributeArray& aAttrs
    )
    {
    CSenDomFragment* pNew = new (ELeave) CSenDomFragment;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aNsUri, aLocalName, aQName, aAttrs);
    CleanupStack::Pop(); // delete pNew;
    return pNew;
    }

EXPORT_C CSenDomFragment* CSenDomFragment::NewL(
    const TDesC8& aNsUri,
    const TDesC8& aLocalName,
    const TDesC8& aQName,
    /* NOT IN JAVA */
    const RAttributeArray& aAttrs,
    CSenElement& aParent
    )
    {
    CSenDomFragment* pNew = new (ELeave) CSenDomFragment;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aNsUri, aLocalName, aQName, aAttrs, aParent);
    CleanupStack::Pop(); // delete pNew;
    return pNew;
    }

EXPORT_C CSenDomFragment* CSenDomFragment::NewL(
    const TDesC8& aNsUri,
    const TDesC8& aLocalName,
    const TDesC8& aQName
    )
    {
    CSenDomFragment* pNew = new (ELeave) CSenDomFragment;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aNsUri, aLocalName, aQName);
    CleanupStack::Pop(); // delete pNew;
    return pNew;
    }

EXPORT_C CSenDomFragment::~CSenDomFragment()
    {
    if(ipDomDelegate)
        {
        // We do not own the element, so remove it from the fragment.
        ipDomDelegate->ExtractElement();
        delete ipDomDelegate;
        ipDomDelegate = NULL;
        }
    }

EXPORT_C CSenDomFragment::CSenDomFragment()
:ipDomDelegate(NULL)
    {
    }

EXPORT_C void CSenDomFragment::BaseConstructL(const CSenElement& aElement)
    {
    CSenBaseFragment::BaseConstructL(aElement);
    }

EXPORT_C void CSenDomFragment::BaseConstructL(const TDesC8& aLocalName)
    {
    CSenBaseFragment::BaseConstructL(aLocalName);
    }

EXPORT_C void CSenDomFragment::BaseConstructL(
    const TDesC8& aNsUri,
    const TDesC8& aLocalName
    )
    {
    CSenBaseFragment::BaseConstructL(aNsUri, aLocalName);
    }

EXPORT_C void CSenDomFragment::BaseConstructL(
    const TDesC8& aNsUri,
    const TDesC8& aLocalName,
    const TDesC8& aQName,
    const RAttributeArray& aAttrs
    )
    {
    CSenBaseFragment::BaseConstructL(aNsUri, aLocalName, aQName, aAttrs);
    }

EXPORT_C void CSenDomFragment::BaseConstructL(
    const TDesC8& aNsUri,
    const TDesC8& aLocalName,
    const TDesC8& aQName,
    const RAttributeArray& aAttrs,
    CSenElement& aParent
    )
    {
    CSenBaseFragment::BaseConstructL(   aNsUri,
                                        aLocalName,
                                        aQName,
                                        aAttrs,
                                        aParent);
    }

EXPORT_C void CSenDomFragment::BaseConstructL(
    const TDesC8& aNsUri,
    const TDesC8& aLocalName,
    const TDesC8& aQName
    )
    {
    CSenBaseFragment::BaseConstructL(aNsUri, aLocalName, aQName);
    }


EXPORT_C void CSenDomFragment::BaseConstructL(CSenXmlReader& aReader)
    {
    CSenBaseFragment::BaseConstructL(aReader);
    }


EXPORT_C void CSenDomFragment::ParseWithL(CSenXmlReader& aReader) // IOP
    {
    CSenBaseFragment::ParseWithL(aReader);
    iState = KStateSave; // IOP
    }

EXPORT_C void CSenDomFragment::StartElementL(const TDesC8& aNsUri,
                                             const TDesC8& aLocalName,
                                             const TDesC8& aQName,
                                             const RAttributeArray& aAttrs)
    {

    // 2004-09-10. moved out from KStateIgnore -code segment
    if(!ipElement)
        {
        ipElement = CSenXmlElement::NewL(aNsUri, aLocalName, aQName);
        }
    AllocContentSaverL();

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
        ExpandL(aNsUri, aLocalName, aQName, aAttrs);
        }
    }

EXPORT_C void CSenDomFragment::CharactersL(
    const TDesC8& aChars,
    const TInt /*aStart*/,
    const TInt /*aLength*/
    )
    {
    AllocContentSaverL();


    HBufC8* pCharsCopy = NULL;
    
    if (SenXmlUtils::EncodeHttpCharactersL(aChars, pCharsCopy))
        {
        CleanupStack::PushL(pCharsCopy);
        iWs.WriteL(*pCharsCopy);
        CleanupStack::PopAndDestroy(); // pCharsCopy
        }
    else
        {
        iWs.WriteL(aChars);
        }
    }

EXPORT_C void CSenDomFragment::SetAttributesL(const RAttributeArray& aAttrs)
    {
    CSenBaseFragment::SetAttributesL(aAttrs);
    if(ipElement)
        {
        ((CSenXmlElement*)ipElement)->SetAttributesL(aAttrs);
        }
    }

EXPORT_C void CSenDomFragment::ExpandL(
    const TDesC8& aNsUri,
    const TDesC8& aLocalName,
    const TDesC8& aQName,
    const RAttributeArray& aAttrs
    )
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
    ipDomDelegate = CSenDomFragment::NewL(  aNsUri,
                                            aLocalName,
                                            aQName,
                                            aAttrs,
                                            *ipElement);

    ipElement->AddElementL(ipDomDelegate->AsElement());

    DelegateParsingL(*ipDomDelegate); // note that the parsing is actually
                                      // performed AFTER the StartElement()
                                      // method, which called this ExpandL()
                                      // has been fully executed (this is an
                                      // async signal send to the _same_
                                      // parser(!)
    }


EXPORT_C void CSenDomFragment::WriteStartElementL(
                                               const TDesC8& /* aNsUri */,
                                               const TDesC8& /* aLocalName */,
                                               const TDesC8& /* aQName */,
                                               const RAttributeArray& /*aAttrs*/
                                                   )
    {
    // in DOM fragment: do nothing
    }

EXPORT_C void CSenDomFragment::WriteEndElementL(
                                             const TDesC8& /* aNsUri */,
                                             const TDesC8& /* aLocalName */,
                                             const TDesC8& /* aQName */
                                                 )
    {
    // in DOM fragment: do nothing
    }


// Overridden from CSenBaseFragment to be much more simple
EXPORT_C void CSenDomFragment::ResumeParsingFromL( // IOP
                                               const TDesC8& /*aNsUri*/,
                                               const TDesC8& /*aLocalName*/,
                                               const TDesC8& /*aQName*/
                                                   )
    {
    iXmlReader->SetContentHandler(*this);
    if(ipDomDelegate)
        {
        // We do not own the element, so remove it from the fragment.
        ipDomDelegate->ExtractElement();
        delete ipDomDelegate;
        ipDomDelegate = NULL;
        }
    }

// End of File
