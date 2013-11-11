/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:        Class implements XML DOM fragment using libxml2 classes
*
*/








// INCLUDE FILES
#include "SenDomFragmentBase.h"
#include <xml/dom/xmlengserializationoptions.h> // needed for TSerializationOptions


EXPORT_C CSenDomFragmentBase* CSenDomFragmentBase::NewL()
    {
    CSenDomFragmentBase* pNew = new (ELeave) CSenDomFragmentBase;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL();
    CleanupStack::Pop(); // Pop pNew
    return pNew;
    }

EXPORT_C CSenDomFragmentBase* CSenDomFragmentBase::NewL(const TXmlEngElement& aElement)
    {
    CSenDomFragmentBase* pNew = new (ELeave) CSenDomFragmentBase;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aElement);
    CleanupStack::Pop(); // Pop pNew
    return pNew;
    }

EXPORT_C CSenDomFragmentBase* CSenDomFragmentBase::NewL(const TDesC8& aLocalName)
    {
    CSenDomFragmentBase* pNew = new (ELeave) CSenDomFragmentBase;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aLocalName);
    CleanupStack::Pop(); // Pop pNew
    return pNew;
    }

EXPORT_C CSenDomFragmentBase* CSenDomFragmentBase::NewL(
    const TDesC8& aNsUri,
    const TDesC8& aLocalName
    )
    {
    CSenDomFragmentBase* pNew = new (ELeave) CSenDomFragmentBase;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aNsUri, aLocalName);
    CleanupStack::Pop(); // Pop pNew
    return pNew;
    }

EXPORT_C CSenDomFragmentBase* CSenDomFragmentBase::NewL(
    const TDesC8& aNsUri,
    const TDesC8& aLocalName,
    const TDesC8& aPrefix,
    const RAttributeArray& aAttrs
    )
    {
    CSenDomFragmentBase* pNew = new (ELeave) CSenDomFragmentBase;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aNsUri, aLocalName, aPrefix, aAttrs);
    CleanupStack::Pop(); // Pop pNew
    return pNew;
    }

EXPORT_C CSenDomFragmentBase* CSenDomFragmentBase::NewL(
    const TDesC8& aNsUri,
    const TDesC8& aLocalName,
    const TDesC8& aPrefix,
    const RAttributeArray& aAttrs,
    TXmlEngElement& aParent
    )
    {
    CSenDomFragmentBase* pNew = new (ELeave) CSenDomFragmentBase;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aNsUri, aLocalName, aPrefix, aAttrs, aParent);
    CleanupStack::Pop(); // delete pNew;
    return pNew;
    }

EXPORT_C CSenDomFragmentBase* CSenDomFragmentBase::NewL(
    const TDesC8& aNsUri,
    const TDesC8& aLocalName,
    const TDesC8& aPrefix,
    const RAttributeArray& aAttrs,
    TXmlEngElement& aParent,
    RSenDocument& aOwnerDocument
    )
    {
    CSenDomFragmentBase* pNew = new (ELeave) CSenDomFragmentBase;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aNsUri, aLocalName, aPrefix, aAttrs, aParent,
                         aOwnerDocument);
    CleanupStack::Pop(); // delete pNew;
    return pNew;
    }

EXPORT_C CSenDomFragmentBase* CSenDomFragmentBase::NewL(
    const TDesC8& aNsUri,
    const TDesC8& aLocalName,
    const TDesC8& aPrefix
    )
    {
    CSenDomFragmentBase* pNew = new (ELeave) CSenDomFragmentBase;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aNsUri, aLocalName, aPrefix);
    CleanupStack::Pop(); // delete pNew;
    return pNew;
    }

EXPORT_C CSenDomFragmentBase::~CSenDomFragmentBase()
    {
    if ( ipDomDelegate )
        {
        // We do not own the element, so remove it from the fragment.
        //ipDomDelegate->ExtractElement();
        delete ipDomDelegate;
        ipDomDelegate = NULL;
        }
    }

EXPORT_C CSenDomFragmentBase::CSenDomFragmentBase()
:   ipDomDelegate(NULL)
    {
    }

EXPORT_C void CSenDomFragmentBase::BaseConstructL()
    {
    iDocument = RSenDocument::NewL();
    }

EXPORT_C void CSenDomFragmentBase::BaseConstructL(const TXmlEngElement& aElement)
    {
    CSenFragmentBase::BaseConstructL(aElement);
    }

EXPORT_C void CSenDomFragmentBase::BaseConstructL(const TDesC8& aLocalName)
    {
    CSenFragmentBase::BaseConstructL(aLocalName);
    }

EXPORT_C void CSenDomFragmentBase::BaseConstructL(
    const TDesC8& aNsUri,
    const TDesC8& aLocalName
    )
    {
    CSenFragmentBase::BaseConstructL(aNsUri, aLocalName);
    }

EXPORT_C void CSenDomFragmentBase::BaseConstructL(
    const TDesC8& aNsUri,
    const TDesC8& aLocalName,
    const TDesC8& aPrefix,
    const RAttributeArray& aAttrs
    )
    {
    CSenFragmentBase::BaseConstructL(aNsUri, aLocalName, aPrefix, aAttrs);
    }

EXPORT_C void CSenDomFragmentBase::BaseConstructL(
    const TDesC8& aNsUri,
    const TDesC8& aLocalName,
    const TDesC8& aPrefix,
    const RAttributeArray& aAttrs,
    TXmlEngElement& aParent
    )
    {
    CSenFragmentBase::BaseConstructL(   aNsUri,
                                        aLocalName,
                                        aPrefix,
                                        aAttrs,
                                        aParent);
    }
    
EXPORT_C void CSenDomFragmentBase::BaseConstructL(
    const TDesC8& aNsUri,
    const TDesC8& aLocalName,
    const TDesC8& aPrefix,
    const RAttributeArray& aAttrs,
    TXmlEngElement& aParent,
    RSenDocument& aOwnerDocument
    )
    {
    CSenFragmentBase::BaseConstructL(   aNsUri,
                                        aLocalName,
                                        aPrefix,
                                        aAttrs,
                                        aParent,
                                        aOwnerDocument);
    }

EXPORT_C void CSenDomFragmentBase::BaseConstructL(
    const TDesC8& aNsUri,
    const TDesC8& aLocalName,
    const TDesC8& aPrefix
    )
    {
    CSenFragmentBase::BaseConstructL(aNsUri, aLocalName, aPrefix);
    }

EXPORT_C void CSenDomFragmentBase::OnStartElementL(const RTagInfo& aElement, 
                                                 const RAttributeArray& aAttributes, 
                                                 TInt aErrorCode)
    {
    const TPtrC8 saxLocalName   = aElement.LocalName().DesC();
    const TPtrC8 saxNsUri       = aElement.Uri().DesC();
    const TPtrC8 saxPrefix      = aElement.Prefix().DesC();
    
    
    if ( iElement.IsNull() )
        {
        if ( saxNsUri == KNullDesC8 && saxPrefix == KNullDesC8 )
            {
            iDocument.CreateDocumentElementL(saxLocalName);
            }
        else
            {
            if ( saxPrefix == KNullDesC8 )
                {
                iDocument.CreateDocumentElementL(saxLocalName,
                                                 saxNsUri,
                                                 KNullDesC8);
                }
            else
                {
                iDocument.CreateDocumentElementL(saxLocalName,
                                                 saxNsUri,
                                                 saxPrefix);
                }
            }
        iElement = iDocument.DocumentElement();        
        }    
    
    TXmlEngElement element = AsElementL();
    
    TPtrC8 localName =  element.Name();
    TPtrC8 nsUri(KNullDesC8);
    if ( element.NamespaceUri() != KNullDesC8 )
        {
        nsUri.Set(element.NamespaceUri());
        }
    
    if(iState == KSenStateIgnore)
        {
        if(localName == saxLocalName && nsUri == saxNsUri) 
            {
            iState = KSenStateSave;
            AddNamespacesL();
            AddAttributesL(aAttributes);
            }
        else
            {
            if ( ipNamespaceArray )
                {
                ipNamespaceArray->ResetAndDestroy();
                }
            }
        }
    else if((iState & KSenStateSave) == KSenStateSave)
        {
        ExpandL(aElement, aAttributes, aErrorCode);
        }
    }

EXPORT_C void CSenDomFragmentBase::OnContentL(const TDesC8& aBytes,
                                              TInt /*aErrorCode*/)
    {
    if((iState & KSenStateSave) == KSenStateSave)
        {
        TXmlEngElement element = AsElementL();
        element.AddTextL(aBytes);
        }
    }

EXPORT_C void CSenDomFragmentBase::ExpandL(const RTagInfo& aElement,
                                           const RAttributeArray& aAttributes,
                                           TInt /*aErrorCode*/)
    {
    if ( ipDomDelegate )
        {
        // We do not own the element, so remove it from the fragment.
        //ipDomDelegate->ExtractElement();
        delete ipDomDelegate;
        ipDomDelegate = NULL;
        }

    const TPtrC8 saxLocalName   = aElement.LocalName().DesC();
    const TPtrC8 saxNsUri       = aElement.Uri().DesC();
    const TPtrC8 saxPrefix      = aElement.Prefix().DesC();

    TXmlEngElement element = AsElementL();
    RSenDocument document = AsDocumentL();
    
    // delegate parsing to a new DOMFragment,
    // until we encounter the end of an element with the given qualified name
    ipDomDelegate = CSenDomFragmentBase::NewL( saxNsUri,
                                               saxLocalName,
                                               saxPrefix,
                                               aAttributes,
                                               element,
                                               document );
                                               
    ipDomDelegate->iState = KSenStateDelegate;
                                                
    OnDelegateParsingL(*ipDomDelegate); // note that the parsing is actually
                                      // performed AFTER the StartElement()
                                      // method, which called this ExpandL()
                                      // has been fully executed (this is an
                                      // async signal send to the _same_
                                      // parser(!)    
    }


EXPORT_C void CSenDomFragmentBase::OnWriteStartElementL(
                                        const RTagInfo& /*aElement*/, 
                                        const RAttributeArray& /*aAttributes*/)
    {
    // in DOM fragment: do nothing
    }

EXPORT_C void CSenDomFragmentBase::OnWriteEndElementL(
                                        const RTagInfo& /*aElement*/)
    {
    // in DOM fragment: do nothing
    }


// Overridden from CSenBaseFragment to be much more simple
EXPORT_C void CSenDomFragmentBase::OnResumeParsingFromL(
                                        const RTagInfo& /*aElement*/,
                                        TInt /*aErrorCode*/)
    {
    SetContentHandler(*this);
    
    if ( ipDomDelegate )
        {
        delete ipDomDelegate;
        ipDomDelegate = NULL;
        }
    }
    
EXPORT_C HBufC8* CSenDomFragmentBase::AsXmlL()
    {
    return CSenFragmentBase::AsXmlL();
    }
    
EXPORT_C void CSenDomFragmentBase::AddNamespacesL()
    {
    CSenFragmentBase::AddNamespacesL();
    }

EXPORT_C void CSenDomFragmentBase::AddAttributesL(const RAttributeArray& aAttrs)
    {
    CSenFragmentBase::AddAttributesL(aAttrs);
    }

EXPORT_C void CSenDomFragmentBase::OnEndElementL(const RTagInfo& aElement, TInt aErrorCode)
    {
    CSenFragmentBase::OnEndElementL(aElement, aErrorCode);
    }

EXPORT_C void CSenDomFragmentBase::OnStartDocumentL(const RDocumentParameters& /*aDocParam*/, TInt /*aErrorCode*/)
    {
    // in DOM fragment: do nothing
    }

EXPORT_C void CSenDomFragmentBase::OnEndDocumentL(TInt /*aErrorCode*/)
    {
    // in DOM fragment: do nothing
    }

EXPORT_C void CSenDomFragmentBase::OnStartPrefixMappingL(const RString& aPrefix, const RString& aUri, TInt aErrorCode)
    {
    CSenFragmentBase::OnStartPrefixMappingL(aPrefix, aUri, aErrorCode);
    }

EXPORT_C void CSenDomFragmentBase::OnEndPrefixMappingL(const RString& aPrefix, TInt aErrorCode)
    {
    CSenFragmentBase::OnEndPrefixMappingL(aPrefix, aErrorCode);
    }

EXPORT_C void CSenDomFragmentBase::OnIgnorableWhiteSpaceL(const TDesC8& aBytes, TInt ErrorCode)
    {
    CSenFragmentBase::OnIgnorableWhiteSpaceL(aBytes, ErrorCode);
    }

EXPORT_C void CSenDomFragmentBase::OnSkippedEntityL(const RString& aName, TInt aErrorCode)
    {
    CSenFragmentBase::OnSkippedEntityL(aName, aErrorCode);
    }

EXPORT_C void CSenDomFragmentBase::OnProcessingInstructionL(const TDesC8& aTarget, const TDesC8& aData, TInt aErrorCode)
    {
    CSenFragmentBase::OnProcessingInstructionL(aTarget, aData, aErrorCode);
    }

EXPORT_C void CSenDomFragmentBase::OnError(TInt aErrorCode)
    {
    CSenFragmentBase::OnError(aErrorCode);
    }

EXPORT_C TAny* CSenDomFragmentBase::GetExtendedInterface(const TInt32 aUid)
    {
    return CSenFragmentBase::GetExtendedInterface(aUid);
    }

EXPORT_C TPtrC8 CSenDomFragmentBase::ContentL()
    {
    return CSenFragmentBase::ContentL();
    }
        
EXPORT_C TXmlEngNamespace CSenDomFragmentBase::Namespace(const TDesC8& aPrefix)
    {
    return CSenFragmentBase::Namespace(aPrefix);
    }
        
//EXPORT_C void CSenDomFragmentBase::DetachL()
//    {
//    CSenFragmentBase::DetachL();
//    }
        
EXPORT_C void CSenDomFragmentBase::ResetContentL()
    {
    CSenFragmentBase::ResetContentL();
    }

EXPORT_C TPtrC8 CSenDomFragmentBase::LocalName() const
    {
    return CSenFragmentBase::LocalName();
    }
        
EXPORT_C TPtrC8 CSenDomFragmentBase::NsUri() const
    {
    return CSenFragmentBase::NsUri();
    }

EXPORT_C TPtrC8 CSenDomFragmentBase::NsPrefix() const
    {
    return CSenFragmentBase::NsPrefix();
    }

EXPORT_C TXmlEngElement CSenDomFragmentBase::AsElementL()
    {
    return CSenFragmentBase::AsElementL();
    }

EXPORT_C RSenDocument& CSenDomFragmentBase::AsDocumentL()
    {
    return CSenFragmentBase::AsDocumentL();
    }

EXPORT_C TXmlEngElement CSenDomFragmentBase::ExtractElement()
    {
    return CSenFragmentBase::ExtractElement();
    }
        
EXPORT_C void CSenDomFragmentBase::OnDelegateParsingL(CSenFragmentBase& aDelegate)
    {
    CSenFragmentBase::OnDelegateParsingL(aDelegate);
    }

EXPORT_C void CSenDomFragmentBase::OnDelegateParsingL(const RTagInfo& aElement, 
                                                 const RAttributeArray& aAttributes, 
                                                 TInt aErrorCode)
    {
    CSenFragmentBase::OnDelegateParsingL(aElement, aAttributes, aErrorCode);
    }
        
EXPORT_C void CSenDomFragmentBase::SetOwner(CSenFragmentBase& aFragment)
    {
    CSenFragmentBase::SetOwner(aFragment);
    }

EXPORT_C HBufC* CSenDomFragmentBase::AsXmlUnicodeL()
    {
    return CSenFragmentBase::AsXmlUnicodeL();
    }

EXPORT_C void CSenDomFragmentBase::WriteAsXMLToL(RWriteStream& aWs)
    {
    CSenFragmentBase::WriteAsXMLToL(aWs);
    }

EXPORT_C TBool CSenDomFragmentBase::ConsistsOfL(CSenFragmentBase& aCandidate)
    {
    return CSenFragmentBase::ConsistsOfL(aCandidate);
    }
        
EXPORT_C void CSenDomFragmentBase::SetContentHandler(CSenFragmentBase& aContentHandler)
    {
    CSenFragmentBase::SetContentHandler(aContentHandler);
    }
        
void CSenDomFragmentBase::RenameL(const TDesC8& aLocalName, const TDesC8& aPrefix, const TDesC8& aNamespace)
    {
    CSenFragmentBase::RenameL(aLocalName, aPrefix, aNamespace);
    }

void CSenDomFragmentBase::RenameL(const TDesC8& aLocalName, const TDesC8& aNamespace)
    {
    CSenFragmentBase::RenameL(aLocalName, aNamespace);
    }

void CSenDomFragmentBase::RenameLocalNameL(const TDesC8& aLocalName)
    {
    CSenFragmentBase::RenameLocalNameL(aLocalName);
    }

void CSenDomFragmentBase::RenameNamespaceL(const TDesC8& aNamespace)
    {
    CSenFragmentBase::RenameNamespaceL(aNamespace);
    }

void CSenDomFragmentBase::RenamePrefixL(const TDesC8& aPrefix)
    {
    CSenFragmentBase::RenamePrefixL(aPrefix);
    }

void CSenDomFragmentBase::RenameNamespaceL(const TDesC8& aPrefix, const TDesC8& aNamespace)
    {
    CSenFragmentBase::RenameNamespaceL(aPrefix, aNamespace);
    }
         
EXPORT_C TXmlEngElement CSenDomFragmentBase::SetContentOfL(const TDesC8& aLocalName,
                                        const TDesC8& aContent)
    {
    return CSenFragmentBase::SetContentOfL(aLocalName, aContent);
    }

EXPORT_C TPtrC8 CSenDomFragmentBase::ContentOf(const TDesC8& aLocalName)
    {
    return CSenFragmentBase::ContentOf(aLocalName);
    }

EXPORT_C TBool CSenDomFragmentBase::ConsistsOfL(TXmlEngElement& aElement,
                                                TXmlEngElement& aCandidate)
    {
    return CSenFragmentBase::ConsistsOfL(aElement, aCandidate);
    }
    

// End of File

