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
#include "SenBaseElement.h"
#include "SenXmlUtils.h"
#include "senxmldebug.h"

#include "SenXmlConstants.h" // KSenColon, ++



#ifdef SYMBIAN_SECURE_ECOM 
    // for S60 2nd edition FP3 (2.8) OR newer 3rd edition platform
    #include <xml/attribute.h> // needed for RAttributeArray
#else 
    // for S60 2nd edition, up to FP2 (2.0 - 2.6)
    #include "Attribute.h" // needed for RAttributeArray
#endif

using namespace Xml;

namespace
    {
/*
    _LIT8(KColon,            ":");
    _LIT8(KLessThan,         "<");
    _LIT8(KGreaterThan,      ">");
    _LIT8(KSpace,            " ");
    _LIT8(KDblQuot,          "\"");
    _LIT8(KEqualsDblQuot,    "=\"");
    _LIT8(KSlashGreaterThan, "/>");
    _LIT8(KLessThanSlash,    "</");
    _LIT8(KSpaceXmlns,       " xmlns");
    _LIT8(KXmlns,            "xmlns");
    _LIT8(KXmlNsAttNamePlusColon,   "xmlns:"); 
*/
    const TInt KFlatBufSize          = 64;  // CBufFlat
    }

EXPORT_C CSenBaseElement* CSenBaseElement::NewL(const TDesC8& aLocalName)
    {
    CSenBaseElement* pNew = new (ELeave) CSenBaseElement;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aLocalName);
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

EXPORT_C CSenBaseElement* CSenBaseElement::NewL(const TDesC8& aNsUri,
                                                const TDesC8& aLocalName)
    {
    CSenBaseElement* pNew = new (ELeave) CSenBaseElement;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aNsUri, aLocalName);
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

EXPORT_C CSenBaseElement* CSenBaseElement::NewL(const TDesC8& aNsUri,
                                                const TDesC8& aLocalName,
                                                const TDesC8& aQName)
    {
    CSenBaseElement* pNew = new (ELeave) CSenBaseElement;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aNsUri, aLocalName, aQName);
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

EXPORT_C CSenBaseElement* CSenBaseElement::NewL(const TDesC8& aNsUri,
                                                const TDesC8& aLocalName,
                                                const TDesC8& aQName,
                                                const RAttributeArray& apAttrs)
    {
    CSenBaseElement* pNew = new (ELeave) CSenBaseElement;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aNsUri, aLocalName, aQName, apAttrs);
    CleanupStack::Pop(); // pNew;
    return pNew;
    }


EXPORT_C CSenBaseElement* CSenBaseElement::NewL(const TDesC8& aNsUri,
                                                const TDesC8& aLocalName,
                                                const TDesC8& aQName,
                                                const RAttributeArray& apAttrs,
                                                CSenElement& aParent)
    {
    CSenBaseElement* pNew = new (ELeave) CSenBaseElement;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aNsUri, aLocalName, aQName, apAttrs, aParent);
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

EXPORT_C CSenBaseElement::CSenBaseElement()
: ipLocalName(NULL),
//  ipNsUri(NULL),
  ipContentBuf(NULL),
  ipParent(NULL),
  ipNamespace(NULL)
    {
    }

EXPORT_C CSenBaseElement::~CSenBaseElement()
    {
    delete ipLocalName;
//    delete ipNsUri;
    delete ipContentBuf;

    iAttrs.ResetAndDestroy();

    iElements.ResetAndDestroy();
    iNamespaces.ResetAndDestroy();
    }



EXPORT_C void CSenBaseElement::BaseConstructL(const TDesC8& aLocalName)
    {
    SenXmlUtils::LeaveOnInvalidElementNameL(aLocalName);
    ipLocalName = aLocalName.AllocL();
    }

EXPORT_C void CSenBaseElement::BaseConstructL(const TDesC8& aNsUri,
                                              const TDesC8& aLocalName)
    {
    SenXmlUtils::LeaveOnInvalidElementNameL(aLocalName);
    if(aNsUri.Length()>0)
        {
        SetNamespaceL(aNsUri);
        }
    ipLocalName = aLocalName.AllocL();
    }

EXPORT_C void CSenBaseElement::BaseConstructL(  const TDesC8& aNsUri,
                                                const TDesC8& aLocalName,
                                                const TDesC8& aQName)
    {
    SenXmlUtils::LeaveOnInvalidElementNameL(aLocalName);
    SenXmlUtils::LeaveOnInvalidElementNameL(aQName);
    ipLocalName = aLocalName.AllocL();
    TPtrC8 prefix(KNullDesC8);
    if(aQName.Length()>0)
        {
        TInt colon = aQName.Locate(':');
        if(colon>0) // Note: 0 also treated as no prefix
            {
            prefix.Set(aQName.Ptr(), colon);
            }
        }
    SetNamespaceL(prefix, aNsUri);
    }

EXPORT_C void CSenBaseElement::BaseConstructL(  const TDesC8& aNsUri,
                                                const TDesC8& aLocalName,
                                                const TDesC8& aQName,
                                                const RAttributeArray& apAttrs)
    {
    BaseConstructL(aNsUri, aLocalName, aQName);
    SetAttributesL(apAttrs);
    }

EXPORT_C void CSenBaseElement::BaseConstructL(  const TDesC8& aNsUri,
                                                const TDesC8& aLocalName,
                                                const TDesC8& aQName,
                                                const RAttributeArray& apAttrs,
                                                CSenElement& aParent    )
    {
    // parent must be set here at first line, because
    // namespace setting dependends of it(!)
    ipParent = &aParent;

    BaseConstructL(aNsUri, aLocalName, aQName);
    SetAttributesL(apAttrs);
    }

EXPORT_C void CSenBaseElement::SetAttributesL(const RAttributeArray& apAttrs)
    {
    AddAttributesL(apAttrs);
    }


EXPORT_C void CSenBaseElement::AddAttributesL(const RAttributeArray& apAttrs)
    {
    TInt count(apAttrs.Count());
    HBufC8* qName = NULL;
    for(TInt i=0; i<count; i++)
        {
        const TPtrC8 localname = apAttrs[i].Attribute().LocalName().DesC();
        const TPtrC8 prefix = apAttrs[i].Attribute().Prefix().DesC();
        const TPtrC8 value = apAttrs[i].Value().DesC();

        SenXmlUtils::BuildQNameL(prefix, localname, qName);

        CleanupStack::PushL(qName);

        HBufC8* encodedValue = NULL;
        TBool encoded = 
            SenXmlUtils::EncodeHttpCharactersL(value, encodedValue);

        if (encoded)
            {
            CleanupStack::PushL(encodedValue);
            AddAttributeL(*qName, localname, *encodedValue); 
            // note: this intentionally
            // skips the 2 param 
            // AddAttributeL() function

            CleanupStack::PopAndDestroy(); // encodedValue
            }
        else
            {
            AddAttributeL(*qName, localname, value); 
            // note: this intentionally
            // skips the 2 param 
            // AddAttributeL() function
            }

        CleanupStack::PopAndDestroy(); // qName
        }
    }

EXPORT_C const TDesC8& CSenBaseElement::AddAttributeL(const TDesC8& aQName,
                                                     const TDesC8& aLocalName,
                                                     const TDesC8& aValue)
    {
    // first we check for possible namespace declarations...
    if ( aQName == KSenXmlns || aLocalName == KSenXmlns)
        {
        // this is a default name space declaration
        SetNamespaceL(aValue);
        }
    else if (aLocalName == NsPrefix())
        {
        //we have a new declaration for the namespace of this element
        SetNamespaceL(aLocalName, aValue);
        }
    else if(aQName.Find(KSenXmlNsAttNamePlusColon) == 0)
        {
        // we have an additional namespace declaration
        AddNamespaceL(aLocalName, aValue);
        }
    else
        {
        // we have a real attribute!
        CSenBaseAttribute* pAttribute = CSenBaseAttribute::NewL(aQName,
                                                                aLocalName,
                                                                aValue);
        AddAttributeL(pAttribute);
        }
    return aValue;
    }



EXPORT_C const TDesC8& CSenBaseElement::AddAttributeL(const TDesC8& aAttrName,
                                                      const TDesC8& aValue)
    {
    // note, the aAttrName may be a qualified name or simply localname
    // strip off the possible prefix from possible qualified name:
    TPtrC8 localName = SenXmlUtils::LocalName(aAttrName);
    return AddAttributeL(aAttrName, localName, aValue);
    }

// takes the ownership of aAttribute
EXPORT_C const TDesC8& CSenBaseElement::AddAttributeL(CSenBaseAttribute* apAttribute)
    {
    CSenBaseAttribute* pOldAtt = FindAttr(apAttribute->Name());

    if(!pOldAtt)
        {
        // transfrer the ownership to this class:
#ifdef EKA2
        iAttrs.AppendL(apAttribute);
#else
        User::LeaveIfError(iAttrs.Append(apAttribute));
#endif // EKA2
        return apAttribute->Value();
        }
    else
        {
        pOldAtt->SetValueL(apAttribute->Value());
        delete apAttribute;
        apAttribute = NULL;
        return pOldAtt->Value();
        }
    }


EXPORT_C CSenBaseAttribute* CSenBaseElement::FindAttr(const TDesC8& aName)
    {
    for (TInt i = 0; i < iAttrs.Count(); i++)
        {
        if (iAttrs[i]->Name() == aName)
            {
            return iAttrs[i];
            }
        }
    return NULL;
    }


EXPORT_C TInt CSenBaseElement::IndexOfElement(const TDesC8& aNsUri,
                                              const TDesC8& aLocalName) const
    {
    for (TInt i = 0; i < iElements.Count(); i++)
        {
        CSenElement* pElement = iElements[i];
        const TDesC8& nsUri = pElement->NamespaceURI();
        const TDesC8& localName = pElement->LocalName();
        if ((nsUri == aNsUri) && (localName == aLocalName))
            {
            return i;
            }
        }
    return KErrNotFound;
    }

EXPORT_C const TDesC8& CSenBaseElement::LocalName() const
    {
    if (ipLocalName == NULL)
        {
        return KNullDesC8();
        }
    else
        {
        return *ipLocalName;
        }
    }

EXPORT_C const TDesC8& CSenBaseElement::NamespaceURI() const
    {
    if (ipNamespace)
        {
        return ipNamespace->URI();
        }
    else
        {
        return KNullDesC8();
        }
    }

EXPORT_C const TDesC8& CSenBaseElement::NsPrefix() const
    {
    if (ipNamespace == NULL)
        {
        return KNullDesC8();
        }
    else
        {
        return ipNamespace->Prefix();
        }
    }

EXPORT_C TBool CSenBaseElement::HasContent() const
    {
    if (ipContentBuf == NULL)
        {
        return EFalse;
        }
    else
        {
        return (ipContentBuf->Size() > 0);
        }
    }

EXPORT_C TPtrC8 CSenBaseElement::Content() const
    {
    if (ipContentBuf == NULL)
        {
        return KNullDesC8();
        }
    else
        {
        TPtrC8 p8 = ipContentBuf->Ptr(0);
        return p8;
        }
    }


EXPORT_C HBufC* CSenBaseElement::ContentUnicodeL() const
    {
    HBufC* pRet = SenXmlUtils::ToUnicodeLC(Content());
    CleanupStack::Pop(); // pRet;
    return pRet;
    }

EXPORT_C TPtrC8 CSenBaseElement::SetContentL(const TDesC8& aContent)
    {
    AllocContentBufL(); // content length is _OK_ to be ZERO
    ipContentBuf->ResizeL(aContent.Size());
    ipContentBuf->Write(0, aContent);
  
    //   TPtrC8(REINTERPRET_CAST(const TUint8*, aContent.Ptr()),
    //                                            aContent.Size()));
    // Reset stream
    iContentWriteStream.Open(*ipContentBuf);
    return Content();
    }

EXPORT_C RWriteStream& CSenBaseElement::ContentWriteStreamL()
    {
    AllocContentBufL();
    iContentWriteStream.Open(*ipContentBuf);
    return iContentWriteStream;
    }

// NOTE: assumes(!) that the namespace is same(!)
EXPORT_C RPointerArray<CSenElement>& CSenBaseElement::ElementsL()
    {
    return iElements;
    }

EXPORT_C TInt CSenBaseElement::ElementsL(
                                    RPointerArray<CSenElement>& aElementArray,
                                    const TDesC8& aNsUri,
                                    const TDesC8& aLocalName)
    {
    if (iElements.Count() == 0)
        return KErrNotFound;

    CSenElement* pElement = NULL;
    TInt count(iElements.Count());
    for (TInt i=0; i<count; i++)
        {
        pElement = iElements[i];
        if ( (aLocalName == pElement->LocalName()) &&
             (aNsUri == pElement->NamespaceURI()) )
            {
#ifdef EKA2
            aElementArray.AppendL(pElement);
#else
            User::LeaveIfError(aElementArray.Append(pElement));
#endif // EKA2
            }
        }

    return KErrNone;
    }

EXPORT_C TInt CSenBaseElement::ElementsL(
                                    RPointerArray<CSenElement>& aElementArray,
                                    const TDesC8& aLocalName)
    {
    return ElementsL(aElementArray,NamespaceURI(),aLocalName);
    }

EXPORT_C const TDesC8* CSenBaseElement::AttrValue(const TDesC8& aName)
    {
    CSenBaseAttribute* pAttr = FindAttr(aName);
    if (pAttr == NULL)
        {
        return NULL;
        }
    else
        {
        return &(pAttr->Value());
        }
    }

EXPORT_C void CSenBaseElement::AddAttrL(const TDesC8& aName,
                                        const TDesC8& aValue)
    {
    CSenBaseAttribute* pAttr = FindAttr(aName);
    if (pAttr == NULL)
        {
        User::LeaveIfError(iAttrs.Append(CSenBaseAttribute::NewL(aName,
                                                                 aValue)));
        }
    else
        {
        pAttr->SetValueL(aValue);
        }
    }

EXPORT_C CSenElement* CSenBaseElement::Parent()
    {
    return ipParent;
    }

EXPORT_C CSenElement* CSenBaseElement::SetParent(CSenElement* apParent)  // IOP
    {
    if (apParent && ipParent != apParent)
        {
        ipParent = apParent;
        if (!ipNamespace)
            {
            // check if there is a default namespace declared in the scope of
            // the parent
            const CSenNamespace* pParentNamespace =
                ((CSenBaseElement*) ipParent)->Namespace(KNullDesC8,ETrue);
            if (pParentNamespace && pParentNamespace->Prefix() == KNullDesC8)
                {
                ipNamespace = (CSenNamespace*)pParentNamespace;
                }
            }
        else
            {
            //check if the parent already has a namespace for this element
            // if so remove it from the local namespace table
            const CSenNamespace* pNs = ipParent->Namespace(KNullDesC8,
                                                    ipNamespace->URI());
            if (pNs && pNs != ipNamespace)
                {
                if (ipNamespace->Compare(*pNs) ||
                    ipNamespace->Prefix().Length() == 0)
                    {
                    //prefix is also identical or this element has no prefix
                    TInt nsIndex = iNamespaces.Find(ipNamespace);
                    if ( nsIndex != KErrNotFound)
                        {
                        iNamespaces.Remove(nsIndex);
                        }
                    delete ipNamespace;
                    ipNamespace = (CSenNamespace*)pNs;
                    }
                }

            }
        }
    return apParent;
    }

EXPORT_C MSenElement& CSenBaseElement::Root()
    {
    if (ipParent == NULL)
        {
        return *this;
        }
    else
        {
        return ipParent->Root();
        }
    }

EXPORT_C CSenElement* CSenBaseElement::Element(const TDesC8& aLocalName)
    {
    return Element(NamespaceURI(), aLocalName);
    }

EXPORT_C CSenElement* CSenBaseElement::Element(const TDesC8& aNsUri,
                                               const TDesC8& aLocalName)
    {
    TInt idx = IndexOfElement(aNsUri, aLocalName);
    if (idx < 0)
        {
        return NULL;
        }
    else
        {
        return iElements[idx];
        }
    }

EXPORT_C CSenElement* CSenBaseElement::CreateElementL(const TDesC8& aNsPrefix,
                                                      const TDesC8& aLocalName)
    {
    CSenElement* pNewElement = NULL;

    if (aNsPrefix.Length() > 0)
        {
        CSenNamespace* pNamespace = (CSenNamespace*)Namespace(aNsPrefix);
        if (pNamespace)
            {
            HBufC8 *pQName =
                HBufC8::NewLC(aNsPrefix.Length() + aLocalName.Length() +5);
            TPtr8 ptr = pQName->Des();
            ptr.Append(aNsPrefix);
            ptr.Append(':');
            ptr.Append(aLocalName);
            pNewElement =
                CSenBaseElement::NewL(pNamespace->URI(), aLocalName, *pQName);
            CleanupStack::PopAndDestroy(); // pQName
            }
        }
    else
        {
        pNewElement = CSenBaseElement::NewL(aLocalName);
        }

    return pNewElement; // Returns NULL if required namespace can not be found!
    }

EXPORT_C CSenElement& CSenBaseElement::InsertElementL(
                                          CSenElement& aElement,
                                          const CSenElement& aBeforeElement)
    {
    TInt index(KErrNotFound);
    index = iElements.Find(&aBeforeElement);
    if (index != KErrNotFound)
        {
        User::LeaveIfError(iElements.Insert(&aElement,index));
        }
    else
        {
        User::LeaveIfError(iElements.Append(&aElement));
        }
    aElement.SetParent(this);
    return aElement;
    }

EXPORT_C CSenElement& CSenBaseElement::AddElementL(CSenElement& aElement)
    {
    User::LeaveIfError(iElements.Append(&aElement));
    aElement.SetParent(this);
    return aElement;
    }

EXPORT_C CSenElement& CSenBaseElement::AddElementL(const TDesC8& aNsUri,
                                                   const TDesC8& aLocalName)
    {
    return AddElementL(*CSenBaseElement::NewL(aNsUri, aLocalName));
    }

EXPORT_C CSenElement& CSenBaseElement::AddElementL(
    const TDesC8& aNsUri,
    const TDesC8& aLocalName,
    const TDesC8& aQName
    )
    {
    return AddElementL(*CSenBaseElement::NewL(aNsUri, aLocalName, aQName));
    }

EXPORT_C CSenElement& CSenBaseElement::AddElementL(const TDesC8& aLocalName)
    {
    return AddElementL(*CSenBaseElement::NewL(aLocalName));
    }

EXPORT_C CSenElement* CSenBaseElement::RemoveElement(CSenElement& aElement)
    {
    TInt idx = iElements.Find(&aElement);
    if (idx >= 0)
        {
        CSenElement* pElement = iElements[idx];
        iElements.Remove(idx);
        pElement->SetParent(NULL);
        return pElement;
        }
    else
        {
        return NULL;
        }
    }

EXPORT_C CSenElement* CSenBaseElement::RemoveElement(const TDesC8& aNsUri,
                                                     const TDesC8& aLocalName)
    {
    TInt idx = IndexOfElement(aNsUri, aLocalName);
    if (idx >= 0)
        {
        CSenElement* pElement = iElements[idx];
        iElements.Remove(idx);
        pElement->SetParent(NULL);
        return pElement;
        }
    else
        {
        return NULL;
        }
    }

EXPORT_C CSenElement* CSenBaseElement::RemoveElement(const TDesC8& aLocalName)
    {
    return RemoveElement(NamespaceURI(), aLocalName);
    }

EXPORT_C CSenElement* CSenBaseElement::ReplaceElementL(CSenElement& aElement)
    {
    CSenElement* pOldElement =
                RemoveElement(aElement.NamespaceURI(), aElement.LocalName());
    CleanupStack::PushL(pOldElement);
    AddElementL(aElement);
    CleanupStack::Pop(); // pOldElement;
    return pOldElement;
    }

EXPORT_C HBufC8* CSenBaseElement::AsXmlL()
    {
    CBufFlat *pBuf = CBufFlat::NewL(KFlatBufSize);
    CleanupStack::PushL(pBuf);
    TPtrC8 p = WriteToBufL(*pBuf);
    HBufC8* pRet = p.AllocL();
    CleanupStack::PopAndDestroy(); // pBuf;
    return pRet;

    }

EXPORT_C HBufC* CSenBaseElement::AsXmlUnicodeL()
    {
    CBufFlat *pBuf = CBufFlat::NewL(KFlatBufSize);
    CleanupStack::PushL(pBuf);
    TPtrC8 p8 = WriteToBufL(*pBuf);
    HBufC* pRet = SenXmlUtils::ToUnicodeLC(p8);
    CleanupStack::Pop(); // pop pRet;
    CleanupStack::PopAndDestroy(); // pBuf;
    return pRet;
    }

EXPORT_C TPtrC8 CSenBaseElement::WriteToBufL(CBufBase& aBuf)
    {
    RBufWriteStream bufWs(aBuf);
    CleanupClosePushL(bufWs);
    this->WriteAsXMLToL(bufWs);
    CleanupStack::PopAndDestroy(); // bufWs.Close();
    return aBuf.Ptr(0);
    }

EXPORT_C void CSenBaseElement::WriteAsXMLToL(RWriteStream& aWriteStream)
    {
    // Find out whether we should declare the namespace
    TPtrC8 nsPrefix = NsPrefix();

    // Element name
    aWriteStream.WriteL(KSenLessThan);
    if (nsPrefix.Length() > 0)
        {
        aWriteStream.WriteL(nsPrefix);
        aWriteStream.WriteL(KSenColon);
        }
    aWriteStream.WriteL(*ipLocalName);

    if (iAttrs.Count() > 0 || iNamespaces.Count() > 0)
        {
        WriteNamespacesToL(aWriteStream);
        WriteAttrsToL(aWriteStream);
        }

    // Elements and content
    if ((iElements.Count() > 0) || HasContent())
        {
        aWriteStream.WriteL(KSenGreaterThan);

        // Body
        WriteElementsToL(aWriteStream);
        WriteContentToL(aWriteStream);

        // Closing element
        aWriteStream.WriteL(KSenLessThanSlash);
        if (nsPrefix.Length() > 0)
            {
            aWriteStream.WriteL(nsPrefix);
            aWriteStream.WriteL(KSenColon);
            }
        aWriteStream.WriteL(*ipLocalName);
        aWriteStream.WriteL(KSenGreaterThan);
        }
    else
        {
        aWriteStream.WriteL(KSenSlashGreaterThan);
        }
    }

EXPORT_C void CSenBaseElement::WriteAttrToL(RWriteStream& aWriteStream,
                                            const TDesC8& aName,
                                            const TDesC8& aValue)
    {
    aWriteStream.WriteL(KSenSpace);
    aWriteStream.WriteL(aName);
    aWriteStream.WriteL(KSenEqualsDblQuot);
    aWriteStream.WriteL(aValue);
    aWriteStream.WriteL(KSenDblQuot);
    }

EXPORT_C void CSenBaseElement::WriteAttrsToL(RWriteStream& aWriteStream)
    {
    for (TInt i = 0; i < iAttrs.Count(); i++)
        {
        WriteAttrToL(aWriteStream, iAttrs[i]->Name(), iAttrs[i]->Value());
        }
    }

EXPORT_C void CSenBaseElement::WriteNamespacesToL(RWriteStream& aWriteStream)
    {
    TInt count = iNamespaces.Count();
    CSenNamespace* ns = NULL;
    if (count > 0)
        {
        for (TInt i=0; i < count; i++)
            {
            ns = iNamespaces[i];
            if (ns)
                {
                aWriteStream.WriteL(KSenSpaceXmlns);
                if (ns->Prefix().Length() > 0)
                    {
                    aWriteStream.WriteL(KSenColon);
                    aWriteStream.WriteL(ns->Prefix());
                    }
                aWriteStream.WriteL(KSenEqualsDblQuot);
                aWriteStream.WriteL(ns->URI());
                aWriteStream.WriteL(KSenDblQuot);
                }
            }
        }
    }

EXPORT_C void CSenBaseElement::WriteElementsToL(RWriteStream& aWriteStream)
    {
    TInt elementCount(iElements.Count());
    for (TInt i=0; i<elementCount; i++)
        {
        iElements[i]->WriteAsXMLToL(aWriteStream);
        }
    }

EXPORT_C void CSenBaseElement::WriteContentToL(RWriteStream& aWriteStream)
    {
    aWriteStream.WriteL(Content());
    }

EXPORT_C MSenElement* CSenBaseElement::AsElement()
    {
    return this;
    }

EXPORT_C void CSenBaseElement::AllocContentBufL()
    {
    if(!ipContentBuf)
        {
        ipContentBuf = CBufFlat::NewL(KFlatBufSize);
        }
    }

EXPORT_C void CSenBaseElement::SetNamespaceL(const TDesC8& aNsUri)
    {
    SetNamespaceL(KNullDesC8, aNsUri);
    }

EXPORT_C void CSenBaseElement::SetNamespaceL(const TDesC8& aNsPrefix,
                                             const TDesC8& aNsUri)
    {
    if (aNsUri.Length() > 0)
        {
        ipNamespace = (CSenNamespace*) Namespace(aNsPrefix, aNsUri);
        if (!ipNamespace)
            {
            ipNamespace = CSenNamespace::NewL(aNsPrefix, aNsUri);

#ifdef EKA2
            iNamespaces.AppendL(ipNamespace);
#else
            User::LeaveIfError(iNamespaces.Append(ipNamespace));
#endif // EKA2
            }
        }
    }

EXPORT_C const CSenNamespace* CSenBaseElement::Namespace()
    {
    return ipNamespace;
    }

EXPORT_C const CSenNamespace* CSenBaseElement::Namespace(
                                                    const TDesC8& aNsPrefix)
    {
    return Namespace(aNsPrefix,ETrue);
    }

EXPORT_C const CSenNamespace* CSenBaseElement::Namespace(
                                                    const TDesC8& aNsPrefix,
                                                    const TBool aCheckInParent)
    {
    if (aNsPrefix.Length() == 0) return NULL;

    const CSenNamespace* pNamespace = NULL;

    TInt count(iNamespaces.Count());
    for (TInt i=0; i<count && pNamespace == NULL; i++)
        {
        pNamespace = iNamespaces[i];
        if (pNamespace)
            {
            if (pNamespace->Prefix() != aNsPrefix) pNamespace = NULL;
            }
        }

    if (pNamespace == NULL && ipParent && aCheckInParent)
        {
        pNamespace = ipParent->Namespace(aNsPrefix);
        }

    return pNamespace;
    }


EXPORT_C const CSenNamespace* CSenBaseElement::Namespace(
                                                    const TDesC8& aNsPrefix,
                                                    const TDesC8& aUri)
    {
    // If prefix == NULL, then any prefix is ok
    const CSenNamespace* pNs = NULL;
    // Check if the namespace has already been declared
    TInt count = iNamespaces.Count();
    if (count != 0)
        {
        for (TInt i=0; i<count; i++)
            {
            pNs = iNamespaces[i];
            if (!pNs->Compare(aNsPrefix, aUri))
                {
                pNs = NULL;
                }
            else
                {
                break;
                }
            }
        }
    if (pNs == NULL && ipParent != NULL)
        {
        // if no namespace defined, but there is a parent, ask its namespace
        pNs = ipParent->Namespace(aNsPrefix, aUri);
        }
    return pNs;
    }

EXPORT_C RPointerArray<CSenBaseAttribute>& CSenBaseElement::AttributesL()
    {
    return iAttrs;
    }

EXPORT_C RPointerArray<CSenNamespace>& CSenBaseElement::NamespacesL()
    {
    return iNamespaces;
    }

EXPORT_C void CSenBaseElement::CopyFromL(CSenElement& aSource)
    {
    TPtrC8 sourceContent = aSource.Content();
    if (sourceContent.Length() > 0)
        {
        if (ipContentBuf == NULL)
            {
            SetContentL(sourceContent);
            }
        else
            {
            RBufWriteStream bufWs(*ipContentBuf);
            CleanupClosePushL(bufWs);
            bufWs.WriteL(sourceContent);
            CleanupStack::PopAndDestroy(); // close bufWs
            }
        }

    RPointerArray<CSenNamespace> sourceNamespaces = aSource.NamespacesL();
    if (sourceNamespaces.Count() > 0)
        {
        for (TInt i=0;i<sourceNamespaces.Count(); i++)
            {
            CSenNamespace* pNamespace = sourceNamespaces[i];
            CSenNamespace* pNewNamespace =
                CSenNamespace::NewL(pNamespace->Prefix(),pNamespace->URI());
            CleanupStack::PushL(pNewNamespace);
#ifdef EKA2
            iNamespaces.AppendL(pNewNamespace);
#else
            User::LeaveIfError(iNamespaces.Append(pNewNamespace));
#endif

            CleanupStack::Pop(pNewNamespace);
            }
        }
    SetNamespaceL(aSource.NsPrefix(), aSource.NamespaceURI());

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
#ifdef EKA2
            iAttrs.AppendL(pNewBaseAttribute);
#else
            User::LeaveIfError(iAttrs.Append(pNewBaseAttribute));
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
#ifdef EKA2
            iElements.AppendL(pNewElement);
#else
            User::LeaveIfError(iElements.Append(pNewElement));
#endif // EKA2
            CleanupStack::Pop(pNewElement);
            }
        }
    }

EXPORT_C void CSenBaseElement::SetPrefixL(const TDesC8& aPrefix)
    {
    if (iNamespaces.Find(ipNamespace) > -1)
        {
        //the namespace was locally declared

        // Check if element already has a namespace with given prefix
        CSenNamespace* pNamespace = NULL;
        TInt count(iNamespaces.Count());
        for (TInt i=0; i<count && pNamespace == NULL; i++)
            {
            pNamespace = iNamespaces[i];
            if (pNamespace->Prefix() != aPrefix) pNamespace = NULL;
            }

        if (pNamespace)
            {
            // Update existing namespace
            const TDesC8& uri = ipNamespace->URI();
            pNamespace->SetUriL(uri);
            ipNamespace = pNamespace;
            }
        else
            {
            if (ipNamespace->Prefix().Length() > 0 && iElements.Count() > 0)
                {
                // there may be children that depend on the old prefix in e.g. attribute names
                const TDesC8& uri = ipNamespace->URI();
                ipNamespace = CSenNamespace::NewL(aPrefix, uri);
#ifdef EKA2
                iNamespaces.AppendL(ipNamespace);
#else
                User::LeaveIfError(iNamespaces.Append(ipNamespace));
#endif
                }
            ipNamespace->SetPrefixL(aPrefix);
            }
        }
    }

EXPORT_C const CSenNamespace* CSenBaseElement::AddNamespaceL(
                                                CSenNamespace& aNewNamespace,
                                                TBool aCheckInParent)
    {
    const CSenNamespace* pNamespace =
                        Namespace(aNewNamespace.Prefix(), aCheckInParent);
    if (pNamespace == NULL)
        {
        CSenNamespace* pNewNamespace =
            CSenNamespace::NewL(aNewNamespace.Prefix(), aNewNamespace.URI());
        CleanupStack::PushL(pNewNamespace);
#ifdef EKA2
        iNamespaces.AppendL(pNewNamespace);
#else
        User::LeaveIfError(iNamespaces.Append(pNewNamespace));
#endif
        CleanupStack::Pop(); // pNewNamespace
        pNamespace = pNewNamespace;
        }
    return pNamespace;
    }

EXPORT_C const CSenNamespace* CSenBaseElement::AddNamespaceL(
                                                         const TDesC8& aPrefix,
                                                         const TDesC8& aUri)
    {
    CSenNamespace* pNamespace = (CSenNamespace*)Namespace(aPrefix);
		if (!pNamespace)
		{
    	if (aUri.Length() > 0)
        	{
        	pNamespace = CSenNamespace::NewL(aPrefix, aUri);
        	CleanupStack::PushL(pNamespace);
#ifdef EKA2
        	iNamespaces.AppendL(pNamespace);
#else
        	User::LeaveIfError(iNamespaces.Append(pNamespace));
#endif
        	CleanupStack::Pop(); // pNamespace
        	}
        }
	    else
	        {
	        pNamespace->SetUriL(aUri); 
	        }
		

    return pNamespace;
    }

EXPORT_C CSenElement* CSenBaseElement::DetachL()
    {
    if (!ipParent)
        {
        return NULL;
        }
    AddNamespaceMissingFromL(iNamespaces);
    ipParent->RemoveElement(*this);
    return this;
    }

void CSenBaseElement::AddNamespaceMissingFromL(
                                    RPointerArray<CSenNamespace>& aNamespaces)
    {
    // Add namespace of this element (possibly copied from parent)
    // if that namespace is not already defined in
    // aNamespaces array.
    if ( ipNamespace && aNamespaces.Find(ipNamespace) == -1)
        {
        // Check if namespace declaration of this element can
        // be found from aNamespaces array
        TInt count(aNamespaces.Count());
        TBool found(EFalse);
        TInt i = 0;
        for (; i<count; i++)
            {
            if ( aNamespaces[i]->URI() == ipNamespace->URI() && 
                 aNamespaces[i]->Prefix() == ipNamespace->Prefix() )
                {
                found = ETrue;
                break;
                }
            }
        if ( !found )
            {
            // Not found 
            // => Create a copy of namespace declaration
            //    and append it into aNamespaces array
            CSenNamespace* pCopy =
                    CSenNamespace::NewL(ipNamespace->Prefix(), ipNamespace->URI());
            CleanupStack::PushL(pCopy);
#ifdef EKA2
            aNamespaces.AppendL(pCopy);
#else
            User::LeaveIfError(aNamespaces.Append(pCopy));
#endif
            CleanupStack::Pop(pCopy);

            // Change namespace object for this element to namespace object
            // which can be found from aNamespaces array
            ipNamespace = pCopy;
            }
        else
            {
            // Change namespace object for this element to namespace object
            // which can be found from aNamespaces array
            
            ipNamespace = aNamespaces[i];
            }
        }

    // Add namespaces from this element's attributes
    // if those namespaces are not already defined in
    // aNamespaces array.
    if ( iAttrs.Count() > 0 )
        {
        TInt colon = -1;
        const CSenNamespace* pNamespace = NULL;

        TInt count(iAttrs.Count());
        for (TInt i=0; i<count; i++)
            {
            CSenBaseAttribute* pAttribute = iAttrs[i];

            // Check namespace assigned to Name
            colon = pAttribute->Name().Locate(':');
            if ( colon > 0 )
                {
                pNamespace =
                    Namespace(pAttribute->Name().Mid(0, colon), KNullDesC8);
                }
            if ( pNamespace && (aNamespaces.Find(pNamespace) == -1) )
                {
                // Check if namespace declaration for handled attribute
                // _name_ can be found from aNamespaces array.
                TInt count(aNamespaces.Count());
                TBool found(EFalse);
                for (TInt i = 0; i<count; i++)
                    {
                    if ( aNamespaces[i]->URI() == pNamespace->URI() && 
                         aNamespaces[i]->Prefix() == pNamespace->Prefix() )
                        {
                        found = ETrue;
                        break;
                        }
                    }
                if ( !found )
                    {
                    // Not found 
                    // => Create a copy of namespace declaration
                    //    and append it into aNamespaces array
                    CSenNamespace* pCopy =
                            CSenNamespace::NewL(pNamespace->Prefix(),
                                                pNamespace->URI());
                    CleanupStack::PushL(pCopy);
#ifdef EKA2
                    aNamespaces.AppendL(pCopy);
#else
                    User::LeaveIfError(aNamespaces.Append(pCopy));
#endif
                    CleanupStack::Pop(pCopy);
                    }
                }
            pNamespace = NULL;

            // Check namespace assigned to Value
            colon = pAttribute->Value().Locate(':');
            if ( colon > 0 )
                {
                pNamespace =
                    Namespace(pAttribute->Value().Mid(0, colon), KNullDesC8);
                }
            if ( pNamespace && (aNamespaces.Find(pNamespace) == -1) )
                {
                // Check if namespace declaration for handled attribute
                // _value_ can be found from aNamespaces array.
                TInt count(aNamespaces.Count());
                TBool found(EFalse);
                for (TInt i = 0; i<count; i++)
                    {
                    if ( aNamespaces[i]->URI() == pNamespace->URI() && 
                         aNamespaces[i]->Prefix() == pNamespace->Prefix() )
                        {
                        found = ETrue;
                        break;
                        }
                    }
                if ( !found )
                    {
                    // Not found 
                    // => Create a copy of namespace declaration
                    //    and append it into aNamespaces array
                    CSenNamespace* pCopy =
                            CSenNamespace::NewL(pNamespace->Prefix(), 
                                                pNamespace->URI());
                    CleanupStack::PushL(pCopy);
#ifdef EKA2
                    aNamespaces.AppendL(pCopy);
#else
                    User::LeaveIfError(aNamespaces.Append(pCopy));
#endif
                    CleanupStack::Pop(pCopy);
                    }
                }
            pNamespace = NULL;
            }
        }

    // Do recursive search for all child elements and for all child
    // elements of all child elements and so on.
    if ( iElements.Count() > 0 )
        {
        TInt count(iElements.Count());
        for (TInt i=0; i<count; i++)
            {
            CSenBaseElement* pElement = (CSenBaseElement*) iElements[i];
            pElement->AddNamespaceMissingFromL(aNamespaces);
            }
        }
    }


EXPORT_C void CSenBaseElement::Set(const TDesC8& aNamespaceURI,
                                   const TDesC8& aLocalName,
                                   const TDesC8& aQName)
    {
    
    HBufC8* pTemp = aLocalName.Alloc();
    if(pTemp)
        {
        delete ipLocalName;
        ipLocalName = NULL;
        ipLocalName = pTemp;
        }


    TPtrC8 prefix(KNullDesC8);

    if(aQName != KNullDesC8)
        {
        TInt colon(KErrNotFound);
        colon = aQName.Locate(':');
        if(colon!=KErrNotFound)
            {
            prefix.Set(aQName.Left(colon));
            }
        }
    TInt leaveCode(KErrNone);
    TRAP(leaveCode, SetNamespaceL(prefix, aNamespaceURI);)
    leaveCode=0;
    
    //if (err) ; 
    }

EXPORT_C CSenElement* CSenBaseElement::Child(TInt aIndex)
    {
    CSenElement* pElement = NULL;
    if (aIndex < iElements.Count())
        {
        pElement = (CSenBaseElement*) iElements[aIndex];
        }
    return pElement;
    }

EXPORT_C TBool CSenBaseElement::ConsistsOfL(MSenElement& aCandidate)
    {
    // First check the names and namespaces
    if (    aCandidate.LocalName() != this->LocalName()
        ||  aCandidate.NamespaceURI() != this->NamespaceURI()
        ||  aCandidate.NsPrefix() != this->NsPrefix()
        )
        {
        return EFalse;
        }

    // Then check content if it exists
    if (aCandidate.HasContent())
        {
        if (aCandidate.Content() != this->Content())
            {
            return EFalse;  // Content doesn't match => no match
            }
        }

    // Then handle the children
    RPointerArray<CSenElement>& children = aCandidate.ElementsL();
    TInt childCount = children.Count();
    TInt i=0;
    while (i < childCount)
        {
        CSenElement* pMatchChild = children[i];
        CSenElement* pChild = this->Element(pMatchChild->LocalName());
        if (!pChild)
            {
            return EFalse;  // no child with same name found in children, 
                            // => no match
            }
        else
            {
            if (!pChild->ConsistsOfL(*pMatchChild))
                {
                return EFalse; // a non matching child was found => No match
                }
            }
        i++;
        }
    return ETrue;
    }

// End of File

