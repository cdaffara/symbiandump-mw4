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
#include <SenXmlConstants.h> // KErrSenZeroLengthDescriptor, KSenColon, ++

#include "senpropertieselement.h" 

namespace
    {
    _LIT8(KSenPropertyType,     "Type");
    _LIT8(KSenPropertyTrue,     "true");
    _LIT8(KSenPropertyFalse,    "false");
    }

EXPORT_C CSenPropertiesElement* CSenPropertiesElement::NewL(const TDesC8& aLocalName,
                                                            RStringPool* aStringPool)
    {
    CSenPropertiesElement* pNew = new (ELeave) CSenPropertiesElement;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aLocalName, aStringPool);
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

EXPORT_C CSenPropertiesElement* CSenPropertiesElement::NewL(const TDesC8& aNsUri,
                                                            const TDesC8& aLocalName,
                                                            RStringPool* aStringPool)
    {
    CSenPropertiesElement* pNew = new (ELeave) CSenPropertiesElement;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aNsUri, aLocalName, aStringPool);
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

EXPORT_C CSenPropertiesElement* CSenPropertiesElement::NewL(const TDesC8& aNsUri,
                                                            const TDesC8& aLocalName,
                                                            const TDesC8& aQName,
                                                            RStringPool* aStringPool)
    {
    CSenPropertiesElement* pNew = new (ELeave) CSenPropertiesElement;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aNsUri, aLocalName, aQName, aStringPool);
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

EXPORT_C CSenPropertiesElement* CSenPropertiesElement::NewL(const TDesC8& aNsUri,
                                                            const TDesC8& aLocalName,
                                                            const TDesC8& aQName,
                                                            const RAttributeArray& apAttrs,
                                                            RStringPool* aStringPool)
    {
    CSenPropertiesElement* pNew = new (ELeave) CSenPropertiesElement;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aNsUri, aLocalName, aQName, apAttrs, aStringPool);
    CleanupStack::Pop(); // pNew;
    return pNew;
    }


EXPORT_C CSenPropertiesElement* CSenPropertiesElement::NewL(const TDesC8& aNsUri,
                                                            const TDesC8& aLocalName,
                                                            const TDesC8& aQName,
                                                            const RAttributeArray& apAttrs,
                                                            CSenElement& aParent,
                                                            RStringPool* aStringPool)
    {
    CSenPropertiesElement* pNew = new (ELeave) CSenPropertiesElement;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aNsUri, aLocalName, aQName, apAttrs, aParent, aStringPool);
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

CSenPropertiesElement::CSenPropertiesElement()
    {
    }

CSenPropertiesElement::~CSenPropertiesElement()
    {
    iLocalName.Close();
    iContent.Close();
    }

void CSenPropertiesElement::BaseConstructL(const TDesC8& aLocalName,
                                           RStringPool* aStringPool)
    {
    ipStringPool = aStringPool;
    if (aLocalName == KNullDesC8)
        {
        User::Leave(KErrSenZeroLengthDescriptor);
        }
    SenXmlUtils::LeaveOnXmlEscapesL(aLocalName);
    
    if ( ipStringPool )
        {
        iLocalName = ipStringPool->OpenStringL(aLocalName);
        }
    else
        {
        Set(KNullDesC8, aLocalName, aLocalName);
        }
    }

void CSenPropertiesElement::BaseConstructL(const TDesC8& aNsUri,
                                           const TDesC8& aLocalName,
                                           RStringPool* aStringPool)
    {
    ipStringPool = aStringPool;
    if (aLocalName == KNullDesC8)
        {
        User::Leave(KErrSenZeroLengthDescriptor);
        }
    SenXmlUtils::LeaveOnXmlEscapesL(aLocalName);

    if(aNsUri.Length()>0)
        {
        SetNamespaceL(aNsUri);
        }
    if ( ipStringPool )
        {
        iLocalName = ipStringPool->OpenStringL(aLocalName);
        }
    else
        {
        Set(KNullDesC8, aLocalName, aLocalName);
        }
    }

void CSenPropertiesElement::BaseConstructL(const TDesC8& aNsUri,
                                           const TDesC8& aLocalName,
                                           const TDesC8& aQName,
                                           RStringPool* aStringPool)
    {
    ipStringPool = aStringPool;

    if (aLocalName == KNullDesC8)
        {
        User::Leave(KErrSenZeroLengthDescriptor);
        }
    SenXmlUtils::LeaveOnXmlEscapesL(aLocalName);

    if (aQName == KNullDesC8)
        {
        User::Leave(KErrSenZeroLengthDescriptor);
        }
    SenXmlUtils::LeaveOnXmlEscapesL(aQName);

    if ( ipStringPool )
        {
        iLocalName = ipStringPool->OpenStringL(aLocalName);
        }
    else
        {
        Set(KNullDesC8, aLocalName, aLocalName);
        }
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

void CSenPropertiesElement::BaseConstructL(const TDesC8& aNsUri,
                                           const TDesC8& aLocalName,
                                           const TDesC8& aQName,
                                           const RAttributeArray& apAttrs,
                                           RStringPool* aStringPool)
    {
    BaseConstructL(aNsUri, aLocalName, aQName, aStringPool);
    SetAttributesL(apAttrs);
    }

void CSenPropertiesElement::BaseConstructL(const TDesC8& aNsUri,
                                           const TDesC8& aLocalName,
                                           const TDesC8& aQName,
                                           const RAttributeArray& apAttrs,
                                           CSenElement& aParent,
                                           RStringPool* aStringPool)
    {
    // parent must be set here at first line, because
    // namespace setting dependends of it(!)
    SetParent(&aParent);

    BaseConstructL(aNsUri, aLocalName, aQName, aStringPool);
    SetAttributesL(apAttrs);
    }

void CSenPropertiesElement::WriteContentToL(RWriteStream& aWriteStream)
    {
    HBufC8* pEncoded = SenXmlUtils::EncodeHttpCharactersLC(Content());
    aWriteStream.WriteL(*pEncoded);
    CleanupStack::PopAndDestroy(pEncoded);
    }
    
CSenElement* CSenPropertiesElement::CreateElementL(const TDesC8& aNsPrefix,
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
            pNewElement = CSenPropertiesElement::NewL(pNamespace->URI(),
                                                      aLocalName,
                                                      *pQName,
                                                      ipStringPool);
                                                      
            CleanupStack::PopAndDestroy(); // pQName
            }
        }
    else
        {
        pNewElement = CSenPropertiesElement::NewL(aLocalName, ipStringPool);
        }

    return pNewElement; // Returns NULL if required namespace can not be found!
    }
    
CSenElement& CSenPropertiesElement::AddElementL(const TDesC8& aNsUri,
                                                const TDesC8& aLocalName)
    {
    return CSenXmlElement::AddElementL(*CSenPropertiesElement::NewL(aNsUri,
                                                                    aLocalName,
                                                                    ipStringPool));
    }

CSenElement& CSenPropertiesElement::AddElementL(const TDesC8& aNsUri,
                                                const TDesC8& aLocalName,
                                                const TDesC8& aQName)
    {
    return CSenXmlElement::AddElementL(*CSenPropertiesElement::NewL(aNsUri,
                                                                    aLocalName,
                                                                    aQName,
                                                                    ipStringPool));
    }

CSenElement& CSenPropertiesElement::AddElementL(const TDesC8& aLocalName)
    {
    return CSenXmlElement::AddElementL(*CSenPropertiesElement::NewL(aLocalName,
                                                                    ipStringPool));
    }

void CSenPropertiesElement::CopyFromL(CSenElement& aSource)
    {
    TPtrC8 sourceContent = aSource.Content();
    if (sourceContent.Length() > 0)
        {
        SetContentL(sourceContent);
        }

    RPointerArray<CSenElement> sourceElements = aSource.ElementsL();
    if (sourceElements.Count() > 0)
        {
        for (TInt i=0;i<sourceElements.Count(); i++)
            {
            CSenElement* pElement = sourceElements[i];
            CSenElement* pNewElement =
                                CSenPropertiesElement::NewL(pElement->LocalName(),
                                                            ipStringPool);
            pNewElement->CopyFromL(*pElement);

            CSenXmlElement::AddElementL(*pNewElement);
            }
        }
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

            CSenXmlElement::AddAttributeL(pNewBaseAttribute);
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
            CSenXmlElement::AddNamespaceL(*pNewNamespace, EFalse);
            CleanupStack::Pop(); 	
            }
        }

    SetNamespaceL(aSource.NamespaceURI());
    }    
    
TPtrC8 CSenPropertiesElement::Name()
    {
    return this->LocalName();
    }

TPtrC8 CSenPropertiesElement::Type()
    {
    TPtrC8 retVal(*this->AttrValue(KSenPropertyType));
    return retVal;
    }

TPtrC8 CSenPropertiesElement::Value()
    {
    return this->Content();
    }

TInt CSenPropertiesElement::IntValue(TInt& aValue)
    {
    TInt retVal(KErrNone);
    
    TPtrC8 value = this->Content();
    TLex8 lex(value);
    retVal = lex.Val(aValue);

    return retVal;
    }

TInt CSenPropertiesElement::BoolValue(TBool& aValue)
    {
     TInt retVal(KErrNone);

    TPtrC8 value = this->Content();
    if (value == KSenPropertyTrue)
        {
        aValue = ETrue;
        }
    else if (value == KSenPropertyFalse)
        {
        aValue = EFalse;
        }
    else
        {
        retVal = KErrGeneral;
        }
        
    return retVal;   
    }
    
TInt CSenPropertiesElement::ValueTokensL(const TDesC8& aDelimiter,
                                         RPointerArray<TPtrC8>& aTokens)
    {
    TPtrC8 content = this->Content();
    TInt delim = content.Find(aDelimiter);
    while ( delim != KErrNotFound )
        {
        TPtrC8* piece = new (ELeave) TPtrC8();
        piece->Set(content.Mid(0,delim));
        aTokens.Append(piece);
        
        content.Set(content.Mid(delim+aDelimiter.Length(),
                                content.Length()-(delim+aDelimiter.Length())));

        delim = content.Find(aDelimiter);
        }

    if(!(this->Content()!=KNullDesC8 && content==KNullDesC8))
        {
        // If this property does NOT zero-length content
        // and the "last" (or first) token is KNullDesC8
        // it means that the string ends with delimiter;
        // Therefore, KNullDesC8 must NOT be added as a
        // result of "tailing delimiter".

        // Add all other tokens here; even KNullDesC8
        // gets added, if it is eiher first or 
        TPtrC8* token = new (ELeave) TPtrC8();
        token->Set(content);
        aTokens.Append(token);
        }    
    if (aTokens.Count() == 0)
        {
        return KErrNotFound;
        }
    return KErrNone;
    }
    
const TDesC8& CSenPropertiesElement::LocalName() const
    {
    if ( !ipStringPool )
        {
        return CSenXmlElement::LocalName();
        }
    else
        {
        return iLocalName.DesC();
        }
    }
    
void CSenPropertiesElement::Set( const TDesC8& aNamespaceURI,
                                 const TDesC8& aLocalName,
                                 const TDesC8& aQName )
    {
    if ( !ipStringPool )
        {
        CSenXmlElement::Set(aNamespaceURI, aLocalName, aQName);
        }
    else
        {
        RString localName;
        TInt leaveCode(KErrNone);
        TRAP( leaveCode, localName = ipStringPool->OpenStringL(aLocalName); )
        if( !leaveCode )
            {
            iLocalName.Close();
            iLocalName = localName;
        
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
            TRAP( leaveCode, SetNamespaceL(prefix, aNamespaceURI); )
            leaveCode=0; // ignored
            } // else { // ipStringPool->OpenStringL leaved(!) }
        }
    }
    
TBool CSenPropertiesElement::HasContent() const
    {
    if ( !ipStringPool )
        {
        return CSenXmlElement::HasContent();
        }
    else
        {
        return (iContent.DesC().Length() > 0);
        }
    }

TPtrC8 CSenPropertiesElement::Content() const
    {
    if ( !ipStringPool )
        {
        return CSenXmlElement::Content();
        }
    else
        {
        return TPtrC8(iContent.DesC());
        }
    }
    
TPtrC8 CSenPropertiesElement::SetContentL(const TDesC8& aContent)
    {
    if ( !ipStringPool )
        {
        return CSenXmlElement::SetContentL(aContent);
        }
    else
        {
        iContent.Close();
        iContent = ipStringPool->OpenStringL(aContent);
        return TPtrC8(iContent.DesC());
        }
    }
    
void CSenPropertiesElement::AllocContentBufL()
    {
    if ( !ipStringPool )
        {
        CSenXmlElement::AllocContentBufL();
        }
    }
    
RWriteStream& CSenPropertiesElement::ContentWriteStreamL()
    {
    if ( !ipStringPool )
        {
        return CSenXmlElement::ContentWriteStreamL();
        }
    else
        {
        return CSenXmlElement::ContentWriteStreamL();
        }
    }
    
void CSenPropertiesElement::WriteAsXMLToL(RWriteStream& aWriteStream)
    {
    if ( !ipStringPool )
        {
        CSenXmlElement::WriteAsXMLToL(aWriteStream);
        }
    else
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
        aWriteStream.WriteL(iLocalName.DesC());

        
        WriteNamespacesToL(aWriteStream);
        WriteAttrsToL(aWriteStream);

        // Elements and content
        RPointerArray<CSenElement> elements =  ElementsL();
        if ((elements.Count() > 0) || HasContent())
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
            aWriteStream.WriteL(iLocalName.DesC());
            aWriteStream.WriteL(KSenGreaterThan);
            }
        else
            {
            aWriteStream.WriteL(KSenSlashGreaterThan);
            }
        }
    }
    

RStringPool& CSenPropertiesElement::StringPool()
    {
//    __ASSERT_ALWAYS(ipStringPool, User::Panic(KPanic, EStringPoolNotInitialized));
    return *ipStringPool;
    }
    
void CSenPropertiesElement::SetStringPool(RStringPool& aStringPool)
    {
    ipStringPool = &aStringPool;
    }
    
    
// END OF FILE



