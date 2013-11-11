/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
#include "SenXmlConstants.h" // KSenColon, ++
#include "SenXmlUtils.h"

#include "senxmldebug.h"

#ifdef SYMBIAN_SECURE_ECOM 
    // for 2.8, 3.0 or newer:
    #include <xml/attribute.h> // needed for RAttributeArray
#else // for 2.6 or older
    #include "Attribute.h"
#endif

using namespace Xml;

namespace
    {

//    _LIT8(KColon,            ":");
//    _LIT8(KLessThan,         "<");
//    _LIT8(KGreaterThan,      ">");
//    _LIT8(KSpace,            " ");
//    _LIT8(KDblQuot,          "\"");
//    _LIT8(KEqualsDblQuot,    "=\"");
//    _LIT8(KSlashGreaterThan, "/>");
//    _LIT8(KSenLessThanSlash,    "</");
//    _LIT8(KSpaceXmlns,       " xmlns");
//    _LIT8(KXmlns,            "xmlns");
//    _LIT8(KXmlNsAttNamePlusColon, "xmlns:"); 
    const TInt KFlatBufSize        = 64;
    }


EXPORT_C CSenXmlElement* CSenXmlElement::NewL(const TDesC8& aLocalName)
    {
    CSenXmlElement* pNew = new (ELeave) CSenXmlElement;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aLocalName);
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

EXPORT_C CSenXmlElement* CSenXmlElement::NewL(const TDesC8& aNsUri,
                                                const TDesC8& aLocalName)
    {
    CSenXmlElement* pNew = new (ELeave) CSenXmlElement;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aNsUri, aLocalName);
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

EXPORT_C CSenXmlElement* CSenXmlElement::NewL(const TDesC8& aNsUri,
                                                const TDesC8& aLocalName,
                                                const TDesC8& aQName)
    {
    CSenXmlElement* pNew = new (ELeave) CSenXmlElement;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aNsUri, aLocalName, aQName);
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

EXPORT_C CSenXmlElement* CSenXmlElement::NewL(const TDesC8& aNsUri,
                                                const TDesC8& aLocalName,
                                                const TDesC8& aQName,
                                                const RAttributeArray& apAttrs)
    {
    CSenXmlElement* pNew = new (ELeave) CSenXmlElement;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aNsUri, aLocalName, aQName, apAttrs);
    CleanupStack::Pop(); // pNew;
    return pNew;
    }


EXPORT_C CSenXmlElement* CSenXmlElement::NewL(const TDesC8& aNsUri,
                                                const TDesC8& aLocalName,
                                                const TDesC8& aQName,
                                                const RAttributeArray& apAttrs,
                                                CSenElement& aParent)
    {
    CSenXmlElement* pNew = new (ELeave) CSenXmlElement;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aNsUri, aLocalName, aQName, apAttrs, aParent);
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

EXPORT_C CSenXmlElement::CSenXmlElement()
: ipLocalName(NULL),
  ipContentBuf(NULL),
  ipParent(NULL),
  ipNamespace(NULL)
    {
    }

EXPORT_C CSenXmlElement::~CSenXmlElement()
    {
    if(ipContentWriteStream)
        {
        ipContentWriteStream->Close();
        delete ipContentWriteStream;
        ipContentWriteStream = NULL;
        }

    if(ipAttrs)
        {
        ipAttrs->ResetAndDestroy();
        delete ipAttrs;
        ipAttrs = NULL; // not required
        }
    if(ipElements)
        {
        ipElements->ResetAndDestroy();
        delete ipElements;
        ipElements = NULL; // not required
        }
    if(ipNamespaces)
        {
        ipNamespaces->ResetAndDestroy();
        delete ipNamespaces;
        ipNamespaces = NULL; // not required
        }

    delete ipLocalName;
    ipLocalName = NULL; // not required


    delete ipContentBuf;
    ipContentBuf = NULL; // not required

    }



EXPORT_C void CSenXmlElement::BaseConstructL(const TDesC8& aLocalName)
    {
    SenXmlUtils::LeaveOnInvalidElementNameL(aLocalName);
    ipLocalName = aLocalName.AllocL();
    }

EXPORT_C void CSenXmlElement::BaseConstructL(const TDesC8& aNsUri,
                                              const TDesC8& aLocalName)
    {
    SenXmlUtils::LeaveOnInvalidElementNameL(aLocalName);
    if(aNsUri.Length()>0)
        {
        SetNamespaceL(aNsUri);
        }
    ipLocalName = aLocalName.AllocL();
    }

EXPORT_C void CSenXmlElement::BaseConstructL(   const TDesC8& aNsUri,
                                                const TDesC8& aLocalName,
                                                const TDesC8& aQName)
    {
    SenXmlUtils::LeaveOnInvalidElementNameL(aLocalName);
    SenXmlUtils::LeaveOnInvalidElementNameL(aQName);
    ipLocalName = aLocalName.AllocL();
    TPtrC8 ptrPrefix(KNullDesC8);

    if (aQName.Length() > 0 )
        {
        TInt colon = aQName.Locate(':');
        if (colon > 0) // Note: 0 also treated as no prefix
            {
            ptrPrefix.Set(aQName.Ptr(),colon);
            }
        }

    SetNamespaceL(ptrPrefix, aNsUri);
    }

EXPORT_C void CSenXmlElement::BaseConstructL(   const TDesC8& aNsUri,
                                                const TDesC8& aLocalName,
                                                const TDesC8& aQName,
                                                const RAttributeArray& apAttrs)
    {
    BaseConstructL(aNsUri, aLocalName, aQName);
    SetAttributesL(apAttrs);
    }

EXPORT_C void CSenXmlElement::BaseConstructL(   const TDesC8& aNsUri,
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

EXPORT_C void CSenXmlElement::SetAttributesL(const RAttributeArray& apAttrs)
    {
    AddAttributesL(apAttrs);
    }


EXPORT_C void CSenXmlElement::AddAttributesL(const RAttributeArray& apAttrs)
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

EXPORT_C const TDesC8& CSenXmlElement::AddAttributeL(const TDesC8& aQName,
                                                     const TDesC8& aLocalName,
                                                     const TDesC8& aValue)
    {
    const TDesC8& nsPrefix = NsPrefix();
    // first we check for possible namespace declarations...
    if ( aQName == KSenXmlns || aLocalName == KSenXmlns)
        {
            if(nsPrefix.Length() > 0)
                {
                // we have an additional namespace declaration
                AddNamespaceL(KNullDesC8(), aValue);
                }
            else
                {
                // this is a default name space declaration
                SetNamespaceL(aValue);                
                }
                
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



EXPORT_C const TDesC8& CSenXmlElement::AddAttributeL(const TDesC8& aAttrName,
                                                      const TDesC8& aValue)
    {
    // note, the aAttrName may be a qualified name or simply localname
    // strip off the possible prefix from possible qualified name:
    TPtrC8 localName = SenXmlUtils::LocalName(aAttrName);
    return AddAttributeL(aAttrName, localName, aValue);
    }

// takes the ownership of aAttribute
EXPORT_C const TDesC8& CSenXmlElement::AddAttributeL(
                                                CSenBaseAttribute* apAttribute)
    {
    CSenBaseAttribute* pOldAtt = FindAttr(apAttribute->Name());

    // if attribute array is not yet allocated, instantiate here
    if(!ipAttrs)
        {
        ipAttrs = new (ELeave) RPointerArray<CSenBaseAttribute>;
        }

    if(!pOldAtt)
        {
        // transfrer the ownership to this class:
        ipAttrs->Append(apAttribute);
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


EXPORT_C CSenBaseAttribute* CSenXmlElement::FindAttr(const TDesC8& aName)
    {
    if(ipAttrs)
        {
        TInt count(ipAttrs->Count());
        for (TInt i = 0; i < count; i++)
            {
            CSenBaseAttribute* pAttribute = (*ipAttrs)[i];
            if(pAttribute->Name() == aName)
                {
                return pAttribute;
                }
            }
        }
    return NULL;
    }


EXPORT_C TInt CSenXmlElement::IndexOfElement(const TDesC8& aNsUri,
                                              const TDesC8& aLocalName) const
    {
    if(ipElements)
        {
        TInt count(ipElements->Count());

        for (TInt i = 0; i < count; i++)
            {
            CSenElement* pElement = (*ipElements)[i];
            const TDesC8& nsUri = pElement->NamespaceURI();
            const TDesC8& localName = pElement->LocalName();
            if ((nsUri == aNsUri) && (localName == aLocalName))
                {
                return i;
                }
            }
        }
    return KErrNotFound;
    }

EXPORT_C const TDesC8& CSenXmlElement::LocalName() const
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

EXPORT_C const TDesC8& CSenXmlElement::NamespaceURI() const
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

EXPORT_C const TDesC8& CSenXmlElement::NsPrefix() const
    {
    if (!ipNamespace)
        {
        return KNullDesC8();
        }
    else
        {
        return ipNamespace->Prefix();
        }
    }

EXPORT_C TBool CSenXmlElement::HasContent() const
    {
    if (!ipContentBuf)
        {
        return EFalse;
        }
    else
        {
        return (ipContentBuf->Size() > 0);
        }
    }

EXPORT_C TPtrC8 CSenXmlElement::Content() const
    {
    if (!ipContentBuf)
        {
        return KNullDesC8();
        }
    else
        {
        TPtrC8 p8 = ipContentBuf->Ptr(0);
        return p8;
        }
    }


EXPORT_C HBufC* CSenXmlElement::ContentUnicodeL() const
    {
    HBufC* pRet = SenXmlUtils::ToUnicodeLC(Content());
    CleanupStack::Pop(); // pRet;
    return pRet;
    }

EXPORT_C TPtrC8 CSenXmlElement::SetContentL(const TDesC8& aContent)
    {
    AllocContentBufL(); 
	if(ipContentBuf)
		{
		ipContentBuf->ResizeL(aContent.Size());
		ipContentBuf->Write(0,
        TPtrC8(REINTERPRET_CAST(const TUint8*, aContent.Ptr()),
                                                aContent.Size())
        );
		if(ipContentWriteStream)
			{
			// Reset stream
			ipContentWriteStream->Open(*ipContentBuf);
			}
		}
    return Content();
    }

EXPORT_C RWriteStream& CSenXmlElement::ContentWriteStreamL()
    {
    AllocContentBufL();
    // Allocate stream
    if(!ipContentWriteStream)
        {
        ipContentWriteStream = new (ELeave) RBufWriteStream;
        }
    ipContentWriteStream->Open(*ipContentBuf);
    return *ipContentWriteStream;
    }

// NOTE: assumes(!) that the namespace is same(!)
// Optimization 
// RPointerArray<CSenElement>* or NULL if no elements have been added!
EXPORT_C RPointerArray<CSenElement>& CSenXmlElement::ElementsL()
    {
    if(!ipElements)
        {
        ipElements = new (ELeave) RPointerArray<CSenElement>;
        }
    return *ipElements;
    }

EXPORT_C TInt CSenXmlElement::ElementsL(
                                    RPointerArray<CSenElement>& aElementArray,
                                    const TDesC8& aNsUri,
                                    const TDesC8& aLocalName)
    {
    TInt retVal(KErrNotFound);

    if(ipElements) // return KErrNotFound, if zero elements have been added
        {
        TInt count(ipElements->Count());

        if (count > 0) // return KErrNotFound, if owned array is empty
            {
            CSenElement* pElement = NULL;
            for (TInt i=0; i<count; i++)
                {
                pElement = (*ipElements)[i];
                if ( (aLocalName == pElement->LocalName()) &&
                     (aNsUri == pElement->NamespaceURI()) )
                    {
                    aElementArray.Append(pElement);
                    }
                }
            retVal = KErrNone;
            }
        }
    return retVal;
    }

EXPORT_C TInt CSenXmlElement::ElementsL(
                                    RPointerArray<CSenElement>& aElementArray,
                                    const TDesC8& aLocalName)
    {
    return ElementsL(aElementArray,NamespaceURI(),aLocalName);
    }

EXPORT_C const TDesC8* CSenXmlElement::AttrValue(const TDesC8& aName)
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

EXPORT_C void CSenXmlElement::AddAttrL(const TDesC8& aName,
                                        const TDesC8& aValue)
    {
    CSenBaseAttribute* pAttr = FindAttr(aName);
    if (!pAttr)
        {
        if(!ipAttrs)
            {
            ipAttrs = new (ELeave) RPointerArray<CSenBaseAttribute>;
            }

        User::LeaveIfError(ipAttrs->Append(CSenBaseAttribute::NewL(aName,
                                                                 aValue)));
        }
    else
        {
        pAttr->SetValueL(aValue);
        }
    }

EXPORT_C CSenElement* CSenXmlElement::Parent()
    {
    return ipParent;
    }

EXPORT_C CSenElement* CSenXmlElement::SetParent(CSenElement* apParent)  // IOP
    {
    if (apParent && ipParent != apParent)
        {
        ipParent = apParent;
        if (!ipNamespace)
            {
            // check if there is a default namespace declared in the scope of
            // the parent
            const CSenNamespace* pParentNamespace =
                ((CSenXmlElement*) ipParent)->Namespace(KNullDesC8,ETrue);
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
                    if(ipNamespaces)
                        {
                        TInt nsIndex = ipNamespaces->Find(ipNamespace);
                        if ( nsIndex != KErrNotFound)
                            {
                            ipNamespaces->Remove(nsIndex);
                            }
                        }
                    delete ipNamespace;
                    ipNamespace = NULL;
                    ipNamespace = (CSenNamespace*)pNs;
                    }
                }

            }
        }
    return apParent;
    }

EXPORT_C MSenElement& CSenXmlElement::Root()
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

EXPORT_C CSenElement* CSenXmlElement::Element(const TDesC8& aLocalName)
    {
    return Element(NamespaceURI(), aLocalName);
    }

EXPORT_C CSenElement* CSenXmlElement::Element(const TDesC8& aNsUri,
                                               const TDesC8& aLocalName)
    {
    if(ipElements)
        {
        TInt idx = IndexOfElement(aNsUri, aLocalName);
        if (idx < 0)
            {
            return NULL;
            }
        else
            {
            return (*ipElements)[idx];
            }
        }
    return NULL;
    }

EXPORT_C CSenElement* CSenXmlElement::CreateElementL(const TDesC8& aNsPrefix,
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
                CSenXmlElement::NewL(pNamespace->URI(), aLocalName, *pQName);
            CleanupStack::PopAndDestroy(); // pQName
            }
        }
    else
        {
        pNewElement = CSenXmlElement::NewL(aLocalName);
        }

    return pNewElement; // Returns NULL if required namespace can not be found!
    }

EXPORT_C CSenElement& CSenXmlElement::InsertElementL(
                                          CSenElement& aElement,
                                          const CSenElement& aBeforeElement)
    {
    TInt index(KErrNotFound);

    // allocate element array, if not already reserved
    if(!ipElements)
        {
        ipElements = new (ELeave) RPointerArray<CSenElement>;
        }
    else
        {
        // search only if array is not brand new
        index = ipElements->Find(&aBeforeElement);
        }

    if (index != KErrNotFound)
        {
        // repleace element
        User::LeaveIfError(ipElements->Insert(&aElement,index));
        }
    else
        {
        // add new element
        User::LeaveIfError(ipElements->Append(&aElement));
        }
    aElement.SetParent(this);
    return aElement;
    }

EXPORT_C CSenElement& CSenXmlElement::AddElementL(CSenElement& aElement)
    {
    // allocate element array, if not already reserved
    if(!ipElements)
        {
        ipElements = new (ELeave) RPointerArray<CSenElement>;
        }

    User::LeaveIfError(ipElements->Append(&aElement));
    aElement.SetParent(this);
    return aElement;
    }

EXPORT_C CSenElement& CSenXmlElement::AddElementL(const TDesC8& aNsUri,
                                                   const TDesC8& aLocalName)
    {
    return AddElementL(*CSenXmlElement::NewL(aNsUri, aLocalName));
    }

EXPORT_C CSenElement& CSenXmlElement::AddElementL(
    const TDesC8& aNsUri,
    const TDesC8& aLocalName,
    const TDesC8& aQName
    )
    {
    return AddElementL(*CSenXmlElement::NewL(aNsUri, aLocalName, aQName));
    }

EXPORT_C CSenElement& CSenXmlElement::AddElementL(const TDesC8& aLocalName)
    {
    return AddElementL(*CSenXmlElement::NewL(aLocalName));
    }

EXPORT_C CSenElement* CSenXmlElement::RemoveElement(CSenElement& aElement)
    {
    CSenElement* pElement = NULL;

    if(ipElements)
        {
        TInt idx = ipElements->Find(&aElement);
        if (idx >= 0)
            {
            pElement = (*ipElements)[idx];
            ipElements->Remove(idx);
            pElement->SetParent(NULL);
            return pElement;
            }
        }
    return pElement;
    }

EXPORT_C CSenElement* CSenXmlElement::RemoveElement(const TDesC8& aNsUri,
                                                     const TDesC8& aLocalName)
    {
    TInt idx = IndexOfElement(aNsUri, aLocalName);
    if (idx >= 0)
        {
        CSenElement* pElement = (*ipElements)[idx];
        ipElements->Remove(idx);
        pElement->SetParent(NULL);
        return pElement;
        }
    else
        {
        return NULL;
        }
    }

EXPORT_C CSenElement* CSenXmlElement::RemoveElement(const TDesC8& aLocalName)
    {
    return RemoveElement(NamespaceURI(), aLocalName);
    }

EXPORT_C CSenElement* CSenXmlElement::ReplaceElementL(CSenElement& aElement)
    {
    CSenElement* pOldElement =
                RemoveElement(aElement.NamespaceURI(), aElement.LocalName());
    CleanupStack::PushL(pOldElement);
    AddElementL(aElement);
    CleanupStack::Pop(); // pOldElement;
    return pOldElement;
    }

EXPORT_C HBufC8* CSenXmlElement::AsXmlL()
    {
    CBufFlat *pBuf = CBufFlat::NewL(KFlatBufSize);
    CleanupStack::PushL(pBuf);
    TPtrC8 p = WriteToBufL(*pBuf);
    HBufC8* pRet = p.AllocL();
    CleanupStack::PopAndDestroy(); // pBuf;
    return pRet;

    }

EXPORT_C HBufC* CSenXmlElement::AsXmlUnicodeL()
    {
    CBufFlat *pBuf = CBufFlat::NewL(KFlatBufSize);
    CleanupStack::PushL(pBuf);
    TPtrC8 p8 = WriteToBufL(*pBuf);
    HBufC* pRet = SenXmlUtils::ToUnicodeLC(p8);
    CleanupStack::Pop(); // pop pRet;
    CleanupStack::PopAndDestroy(); // pBuf;
    return pRet;
    }

EXPORT_C TPtrC8 CSenXmlElement::WriteToBufL(CBufBase& aBuf)
    {
    RBufWriteStream bufWs(aBuf);
    CleanupClosePushL(bufWs);
    this->WriteAsXMLToL(bufWs);
    CleanupStack::PopAndDestroy(); // bufWs.Close();
    return aBuf.Ptr(0);
    }

EXPORT_C void CSenXmlElement::WriteAsXMLToL(RWriteStream& aWriteStream)
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

    
    if ((ipAttrs && ipAttrs->Count() > 0) || 
         (ipNamespaces && ipNamespaces->Count() > 0))
        {
        WriteNamespacesToL(aWriteStream);
        WriteAttrsToL(aWriteStream);
        }

    // Elements and content
    if ((ipElements && ipElements->Count() > 0) || HasContent())
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

EXPORT_C void CSenXmlElement::WriteAttrToL(RWriteStream& aWriteStream,
                                            const TDesC8& aName,
                                            const TDesC8& aValue)
    {
    aWriteStream.WriteL(KSenSpace);
    aWriteStream.WriteL(aName);
    aWriteStream.WriteL(KSenEqualsDblQuot);
    aWriteStream.WriteL(aValue);
    aWriteStream.WriteL(KSenDblQuot);
    }

EXPORT_C void CSenXmlElement::WriteAttrsToL(RWriteStream& aWriteStream)
    {
    if(ipAttrs)
        {
        TInt count(ipAttrs->Count());
        for (TInt i = 0; i < count; i++)
            {
            WriteAttrToL(aWriteStream, (*ipAttrs)[i]->Name(), (*ipAttrs)[i]->Value());
            }
        }
    }

EXPORT_C void CSenXmlElement::WriteNamespacesToL(RWriteStream& aWriteStream)
    {
    if(ipNamespaces)
        {
        CSenNamespace* ns = NULL;
        TInt count = ipNamespaces->Count();
        for (TInt i=0; i < count; i++)
            {
            ns = (*ipNamespaces)[i];
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

EXPORT_C void CSenXmlElement::WriteElementsToL(RWriteStream& aWriteStream)
    {
    if(ipElements)
        {
        TInt elementCount(ipElements->Count());
        for (TInt i=0; i<elementCount; i++)
            {
            (*ipElements)[i]->WriteAsXMLToL(aWriteStream);
            }
        }
    }

EXPORT_C void CSenXmlElement::WriteContentToL(RWriteStream& aWriteStream)
    {
    aWriteStream.WriteL(Content());
    }

EXPORT_C MSenElement* CSenXmlElement::AsElement()
    {
    return this;
    }

EXPORT_C void CSenXmlElement::AllocContentBufL()
    {
    if(!ipContentBuf)
        {
        ipContentBuf = CBufFlat::NewL(KFlatBufSize);
        }
    }

EXPORT_C void CSenXmlElement::SetNamespaceL(const TDesC8& aNsUri)
    {
    SetNamespaceL(KNullDesC8, aNsUri);
    }

EXPORT_C void CSenXmlElement::SetNamespaceL(const TDesC8& aNsPrefix,
                                            const TDesC8& aNsUri)
    {
    if (aNsUri.Length() > 0)
        {
        ipNamespace = (CSenNamespace*) Namespace(aNsPrefix, aNsUri);
        if (!ipNamespace) // not already defined
            {
            // allocate array if not already reserved
            if(!ipNamespaces)
                {
                ipNamespaces = new (ELeave) RPointerArray<CSenNamespace>;
                }
            ipNamespace = CSenNamespace::NewL(aNsPrefix, aNsUri);
            ipNamespaces->Append(ipNamespace);
            }
        }
    }

EXPORT_C const CSenNamespace* CSenXmlElement::Namespace()
    {
    return ipNamespace;
    }

EXPORT_C const CSenNamespace* CSenXmlElement::Namespace(
                                                    const TDesC8& aNsPrefix)
    {
    return Namespace(aNsPrefix,ETrue);
    }

EXPORT_C const CSenNamespace* CSenXmlElement::Namespace(
                                                    const TDesC8& aNsPrefix,
                                                    const TBool aCheckInParent)
    {
    if (aNsPrefix.Length() == 0) return NULL;

    const CSenNamespace* pNamespace = NULL;

    if(ipNamespaces)
        {
        TInt count(ipNamespaces->Count());
        for (TInt i=0; i<count && pNamespace == NULL; i++)
            {
            pNamespace = (*ipNamespaces)[i];
            if (pNamespace)
                {
                if (pNamespace->Prefix() != aNsPrefix) pNamespace = NULL;
                }
            }
        }

    if (pNamespace == NULL && ipParent && aCheckInParent)
        {
        pNamespace = ipParent->Namespace(aNsPrefix);
        }

    return pNamespace;
    }


EXPORT_C const CSenNamespace* CSenXmlElement::Namespace(
                                                    const TDesC8& aNsPrefix,
                                                    const TDesC8& aUri)
    {
    // If prefix == NULL, then any prefix is ok
    const CSenNamespace* pNs = NULL;
    // Check if the namespace has already been declared
    
    if(ipNamespaces)
        {
        TInt count = ipNamespaces->Count();
        if (count != 0)
            {
            for (TInt i=0; i<count; i++)
                {
                pNs = (*ipNamespaces)[i];
                if (!pNs->Compare(aNsPrefix, aUri))
                    {
                    pNs = NULL; // not equal
                    }
                else
                    {
                    break;
                    }
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

EXPORT_C RPointerArray<CSenBaseAttribute>& CSenXmlElement::AttributesL()
    {
    // if attribute array is not yet allocated, instantiate here
    if(!ipAttrs)
        {
        ipAttrs = new (ELeave) RPointerArray<CSenBaseAttribute>;
        }
    return *ipAttrs;
    }

EXPORT_C RPointerArray<CSenNamespace>& CSenXmlElement::NamespacesL()
    {
    if(!ipNamespaces)
        {
        ipNamespaces = new (ELeave) RPointerArray<CSenNamespace>;
        }
    return *ipNamespaces;
    }

EXPORT_C void CSenXmlElement::CopyFromL(CSenElement& aSource)
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

            // allocate array if not already reserved
            if(!ipNamespaces)
                {
                ipNamespaces = new (ELeave) RPointerArray<CSenNamespace>;
                }
            
#ifdef EKA2
            ipNamespaces->AppendL(pNewNamespace);
#else
            User::LeaveIfError(ipNamespaces->Append(pNewNamespace));
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

            // 2005-04-28: check for duplicate and override existing value if
            // attribute already exists.
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

            // if attribute array is not yet allocated, instantiate here
            if(!ipAttrs)
                {
                ipAttrs = new (ELeave) RPointerArray<CSenBaseAttribute>;
                }

#ifdef EKA2
            ipAttrs->AppendL(pNewBaseAttribute);
#else
            User::LeaveIfError(ipAttrs->Append(pNewBaseAttribute));
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
                                CSenXmlElement::NewL(pElement->LocalName());
            CleanupStack::PushL(pNewElement);
            pNewElement->SetParent(this);
            pNewElement->CopyFromL(*pElement);

            if(!ipElements)
                {
                ipElements = new (ELeave) RPointerArray<CSenElement>;
                }

#ifdef EKA2
            ipElements->AppendL(pNewElement);
#else
            User::LeaveIfError(ipElements->Append(pNewElement));
#endif // EKA2
            CleanupStack::Pop(pNewElement);
            }
        }
    }

EXPORT_C void CSenXmlElement::SetPrefixL(const TDesC8& aPrefix)
    {
    if (ipNamespaces && ipNamespaces->Find(ipNamespace) > -1)
        {
        //the namespace was locally declared

        // Check if element already has a namespace with given prefix
        CSenNamespace* pNamespace = NULL;
        TInt count(ipNamespaces->Count());
        for (TInt i=0; i<count && pNamespace == NULL; i++)
            {
            pNamespace = (*ipNamespaces)[i];
            if (pNamespace->Prefix() != aPrefix) 
                {
                pNamespace = NULL;
                }
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
            if(ipElements) // are there any child elements?
                {
                if (ipNamespace->Prefix().Length() > 0 && ipElements->Count() > 0)
                    {
                    // there may be children that depend on the old prefix in e.g. attribute names
                    const TDesC8& uri = ipNamespace->URI();
                    ipNamespace = CSenNamespace::NewL(aPrefix, uri);

                    // append as new namespace
                    ipNamespaces->Append(ipNamespace);
                    }
                }
            ipNamespace->SetPrefixL(aPrefix);
            }
        }
    }

EXPORT_C const CSenNamespace* CSenXmlElement::AddNamespaceL(
                                                CSenNamespace& aNewNamespace,
                                                TBool aCheckInParent)
    {
    const CSenNamespace* pNamespace =
                        Namespace(aNewNamespace.Prefix(), aCheckInParent);

    if (pNamespace == NULL) // does not exist
        {
        CSenNamespace* pNewNamespace =
        CSenNamespace::NewL(aNewNamespace.Prefix(), aNewNamespace.URI());
		CleanupStack::PushL(pNewNamespace); 
        // allocate array if not already reserved
        if(!ipNamespaces)
            {
            ipNamespaces = new (ELeave) RPointerArray<CSenNamespace>;
            }
        CleanupStack::Pop();			
        ipNamespaces->Append(pNewNamespace);
        pNamespace = pNewNamespace;
        }
    return pNamespace;
    }

EXPORT_C const CSenNamespace* CSenXmlElement::AddNamespaceL(
                                                         const TDesC8& aPrefix,
                                                         const TDesC8& aUri)
    {
    CSenNamespace* pNamespace = (CSenNamespace*)Namespace(aPrefix);
	if (!pNamespace)
		{
		if ( aUri.Length() > 0)
			{
			pNamespace = CSenNamespace::NewL(aPrefix, aUri);
			CleanupStack::PushL(pNamespace); 
			// allocate array if not already reserved
			if(!ipNamespaces)
				{
				ipNamespaces = new (ELeave) RPointerArray<CSenNamespace>;
				}
			 CleanupStack::Pop();				
			ipNamespaces->Append(pNamespace);
			}
		}
	    else
	        {
	        pNamespace->SetUriL(aUri); // override current namespace URI with new one
	      
			}

    return pNamespace;
    }

EXPORT_C CSenElement* CSenXmlElement::DetachL()
    {
    if (!ipParent)
        {
        return NULL;
        }

    // allocate array if not already reserved
    if(!ipNamespaces)
        {
        ipNamespaces = new (ELeave) RPointerArray<CSenNamespace>;
        }

    AddNamespaceMissingFromL(*ipNamespaces);

    // optimization: if nothing was added, free the array allocation
    if(ipNamespaces && ipNamespaces->Count()==0)
        {
        ipNamespaces->ResetAndDestroy(); 
        delete ipNamespaces; 
                            
        ipNamespaces = NULL;
        }

    ipParent->RemoveElement(*this);
    return this;
    }

void CSenXmlElement::AddNamespaceMissingFromL( RPointerArray<CSenNamespace>& aNamespaces )
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
            //If not found then check in namespace usage array that namspace declaration 
            //is present or not. If yes then continue without adding.
            if((ipNamespaces && ipNamespaces->Find(ipNamespace) == -1) || !ipNamespaces)
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
    if ( ipAttrs && ipAttrs->Count() > 0 )
        {
        TInt colon = -1;
        const CSenNamespace* pNamespace = NULL;

        TInt count(ipAttrs->Count());
        for(TInt i=0; i<count; i++)
            {
            CSenBaseAttribute* pAttribute = (*ipAttrs)[i];

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
		            //If not found then check in namespace usage array that 
		            //namespace declaration is present or not. If yes then 
		            //continue without adding.
		            if((ipNamespaces && ipNamespaces->Find(pNamespace) == -1) || !ipNamespaces)
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
					//If not found then check in namespace usage array that 
		            //namspace declaration is present or not.If yes then 
		            //continue without adding.
		            if(ipNamespaces && ipNamespaces->Find(pNamespace) == -1)
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
                }
            pNamespace = NULL;
            }
        }

    if(ipElements && ipElements->Count() > 0)
        {
        TInt count(ipElements->Count());
        for(TInt i=0; i<count; i++)
            {
            CSenXmlElement* pElement = (CSenXmlElement*) (*ipElements)[i];
            pElement->AddNamespaceMissingFromL(aNamespaces);
            }
        }
    }


EXPORT_C void CSenXmlElement::Set(const TDesC8& aNamespaceURI,
                                   const TDesC8& aLocalName,
                                   const TDesC8& aQName)
    {
    delete ipLocalName;
    ipLocalName = NULL;
    HBufC8* pTemp = aLocalName.Alloc();
    if(pTemp)
        {
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

EXPORT_C CSenElement* CSenXmlElement::Child(TInt aIndex)
    {
    CSenElement* pElement = NULL;
    if(ipElements && aIndex < ipElements->Count())
        {
        pElement = (CSenXmlElement*) (*ipElements)[aIndex];
        }
    return pElement;
    }

EXPORT_C TBool CSenXmlElement::ConsistsOfL(MSenElement& aCandidate)
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

void CSenXmlElement::Compress()
    {
    if(ipContentWriteStream)
        {
        ipContentWriteStream->Close();
        delete ipContentWriteStream;
        ipContentWriteStream = NULL;
        }
    if (ipContentBuf) ipContentBuf->Compress();
    if(ipElements)
        {
        TInt count(ipElements->Count());

        for (TInt i = 0; i < count; i++)
            {
            CSenElement* pElement = (*ipElements)[i];
            ((CSenXmlElement*)pElement)->Compress();
            }
        }
    
    }

// New methods; problem: assume, that one cannot change CSenElement (abstract)
// class definition (virtual function tables); therefore one must be cautious
// and avoid class casting to internal element in base fragment layer(!) in
// order to access these methods! What if someone added a baseelement? -> crash

// Optimized variant
// @return NULL if this element has no child elements
/*
RPointerArray<CSenElement>* CSenXmlElement::ElementsL()
    {
    return ipElements;
    }

// Optimized variant
//@return NULL if this element has no attributes 
RPointerArray<CSenBaseAttribute>* CSenXmlElement::AttributesL()
    {
    return ipAttrs;
    }

// Optimized variant
// @return RPointerArray<CSenNamespace>* or NULL if there are no attributes in this element
RPointerArray<CSenNamespace>& CSenXmlElement::NamespacesL()
    {
    return ipNamespaces;
    }
*/

// end of file
