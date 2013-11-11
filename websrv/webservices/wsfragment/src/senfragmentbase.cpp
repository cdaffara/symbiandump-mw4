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
* Description:        Class implements XML base fragment using libxml2 classes
*
*/








// INCLUDE FILES
#include <flogger.h>
#include <utf.h>
#include <s32mem.h>
#include "SenFragmentBase.h"
#include "SenParser.h"
#include "wslibxml2utils.h"

#include <SenXmlConstants.h>

#include <xml/attribute.h>               // needed for RAttributeArray
#include <xml/parserfeature.h>           // for TParserFeature enumeration
#include <xml/dom/xmlengserializationoptions.h> // for TSerializationOptions
#include <xml/dom/xmlengbinarycontainer.h>
#include <xml/dom/xmlengnodelist.h>


typedef unsigned char xmlChar; // from "libxml/Libxml2_xmlstring.h"

using namespace Xml;

// ***************************************************************************
// Fragment class state constants are as following (declared in subclasses):
// KSenStateNotSet                  = -1;
// KSenStateIgnore                  =  0; // even ones ignore(0),
// KSenStateSave                    =  1; // and odd ones save(1)
// KSenStateResume                  =  2;
// KStateParsingFramework           =  4;
// KStateParsingSoapFault           =  5;  // odd, save
// KStateParsingServiceDescription  =  6;
// KStateParsingResourceOffering    =  11; // odd, save
// KStateParsingCredentials         =  12;
// KStateParsingService             =  13; // odd state, saves content
// KStateParsingPwTransforms        =  14;
// KStateParsingSoapHeader          =  20; // ignore state (even num)
// KStateParsingSoapBody            =  40; // ignore state (even num)
// KStateParsingSingleCredential    =  122;
// KStateParsingProviderPolicy      =  1222;

namespace
    {
    const TInt KFlatBufSize = 128;
    _LIT(KSenFragmentPanic, "SenFragment");
    
    TBool EncodeXmlEscapesL( const TDesC8& aOriginal, HBufC8*& aEncoded )
        {
        TBool retVal = EFalse;
        delete aEncoded;
        aEncoded = NULL;
    
        if (aOriginal == KNullDesC8)
            {
            return retVal;
            }
        TPtrC8 tokens[] =
            {
            KSenEscapedAmp(),
            KSenEscapedApos(),
            KSenEscapedDblQuot(),
            KSenEscapedGt(),
            KSenEscapedLt()  
            };
        TText16 tokenChars[] =
            {
            '&',
            '\'',
            '\"',
            '>',
            '<'
            };
        
        // Replace escaped characters, if any
        for (TInt i = 0; i < aOriginal.Length(); i++)
            {
            TBool foundChar = EFalse;
            //for (TInt j = 0; j < (sizeof(tokenChars) / sizeof(TText16)); j++)
            for (TInt j = 0; j < 5; j++)
                {
                if (aOriginal[i] == tokenChars[j])
                    {
                    if ( !aEncoded )
                        {
                        aEncoded =
                            HBufC8::NewL(aOriginal.Length() * KSenMaxXmlEscapedLength);
                        aEncoded->Des().Append(aOriginal.Left(i));
                        }
                    foundChar = ETrue;
                    aEncoded->Des().Append(tokens[j]);
                    retVal = ETrue; // indicate, that encoding was done
                    break;
                    }
                }
            if (!foundChar)
                {
                if (aEncoded)
                    {
                    
                    aEncoded->Des().Append(aOriginal[i]);
                    }
                }
            }
        return retVal;
        }
    }

EXPORT_C CSenFragmentBase* CSenFragmentBase::NewL(const TXmlEngElement& aElement)
    {
    CSenFragmentBase* pNew = new (ELeave) CSenFragmentBase;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aElement);
    CleanupStack::Pop(pNew);
    return pNew;
    }

EXPORT_C CSenFragmentBase* CSenFragmentBase::NewL(const TDesC8& aLocalName)
    {
    CSenFragmentBase* pNew = new (ELeave) CSenFragmentBase;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aLocalName);
    CleanupStack::Pop(pNew);
    return pNew;
    }

EXPORT_C CSenFragmentBase* CSenFragmentBase::NewL(const TDesC8& aNsUri,
                                                  const TDesC8& aLocalName
    )
    {
    CSenFragmentBase* pNew = new (ELeave) CSenFragmentBase;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aNsUri, aLocalName);
    CleanupStack::Pop(pNew);
    return pNew;
    }

EXPORT_C CSenFragmentBase* CSenFragmentBase::NewL(const TDesC8& aNsUri,
                                                  const TDesC8& aLocalName,
                                                  const TDesC8& aPrefix,
                                                  const RAttributeArray& aAttrs
    )
    {
    CSenFragmentBase* pNew = new (ELeave) CSenFragmentBase;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aNsUri, aLocalName, aPrefix, aAttrs);
    CleanupStack::Pop(pNew);
    return pNew;
    }

EXPORT_C CSenFragmentBase* CSenFragmentBase::NewL(const TDesC8& aNsUri,
                                                  const TDesC8& aLocalName,
                                                  const TDesC8& aPrefix,
                                                  const RAttributeArray& aAttrs,
                                                  TXmlEngElement& aParent
    )
    {
    CSenFragmentBase* pNew = new (ELeave) CSenFragmentBase;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aNsUri, aLocalName, aPrefix, aAttrs, aParent);
    CleanupStack::Pop(pNew);
    return pNew;
    }

EXPORT_C CSenFragmentBase* CSenFragmentBase::NewL(const TDesC8& aNsUri,
                                                  const TDesC8& aLocalName,
                                                  const TDesC8& aPrefix,
                                                  const RAttributeArray& aAttrs,
                                                  TXmlEngElement& aParent,
                                                  RSenDocument& aOwnerDocument
    )
    {
    CSenFragmentBase* pNew = new (ELeave) CSenFragmentBase;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aNsUri, aLocalName, aPrefix, aAttrs, aParent,
                         aOwnerDocument);
    CleanupStack::Pop(pNew);
    return pNew;
    }

EXPORT_C CSenFragmentBase* CSenFragmentBase::NewL(const TDesC8& aNsUri,
                                                  const TDesC8& aLocalName,
                                                  const TDesC8& aPrefix
    )
    {
    CSenFragmentBase* pNew = new (ELeave) CSenFragmentBase;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aNsUri, aLocalName, aPrefix);
    CleanupStack::Pop(pNew);
    return pNew;
    }
    
EXPORT_C CSenFragmentBase* CSenFragmentBase::NewL(TXmlEngElement& aRootElement,
                                                  RSenDocument& aOwnerDocument)
    {
    CSenFragmentBase* pNew = new (ELeave) CSenFragmentBase;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aRootElement, aOwnerDocument);
    CleanupStack::Pop(pNew);
    return pNew;
    }

EXPORT_C CSenFragmentBase::~CSenFragmentBase()
    {
    if ( ipNamespaceArray )
        {
        ipNamespaceArray->ResetAndDestroy();
        delete ipNamespaceArray;
        }
    iDocument.Close();
    if ( ipContentBuf )
    	{
                
        delete ipContentBuf;
        ipContentBuf = NULL;
        
        delete ipContentWriteStream;
        ipContentWriteStream = NULL;
        }
        
     if ( ipDelegate )
        {
        delete ipDelegate;
        ipDelegate = NULL;
        }   
    }

EXPORT_C CSenFragmentBase::CSenFragmentBase()
:   iState(KSenStateIgnore),
    ipParser(NULL),
    ipOwner(NULL),
    ipDelegate(NULL)
    {
    }

EXPORT_C void CSenFragmentBase::BaseConstructL(const TXmlEngElement& aSrc)
    {
    iElement = aSrc.CopyL();
    iDocument = RSenDocument::NewL();
    iDocument.SetDocumentElement(iElement);
    }

EXPORT_C void CSenFragmentBase::BaseConstructL(const TDesC8& aLocalName)
    {
    iDocument = RSenDocument::NewL();
    if ( aLocalName != KNullDesC8 )
        {
        iDocument.CreateDocumentElementL(aLocalName);
        iElement = iDocument.DocumentElement();
        }
    }

EXPORT_C void CSenFragmentBase::BaseConstructL(const TDesC8& aNsUri,
                                               const TDesC8& aLocalName
    )
    {
    iDocument = RSenDocument::NewL();
    iDocument.CreateDocumentElementL(aLocalName, aNsUri);
    iElement = iDocument.DocumentElement();
    }

EXPORT_C void CSenFragmentBase::BaseConstructL(const TDesC8& aNsUri,
                                               const TDesC8& aLocalName,
                                               const TDesC8& aPrefix,
                                               const RAttributeArray& aAttrs
    )
    {
    iDocument = RSenDocument::NewL();
    if (aPrefix == KNullDesC8)
        {
        iDocument.CreateDocumentElementL(aLocalName, aNsUri);
        }
    else
        {
        iDocument.CreateDocumentElementL(aLocalName, aNsUri, aPrefix);
        }

    iElement = iDocument.DocumentElement();
    AddAttributesL(aAttrs);
    }

EXPORT_C void CSenFragmentBase::BaseConstructL(
    const TDesC8& aNsUri,
    const TDesC8& aLocalName,
    const TDesC8& aPrefix,
    const RAttributeArray& aAttributes,
    TXmlEngElement& aParent
    )
    {
    iDocument = RSenDocument::NewL();
    iElement = aParent.AddNewElementL(aLocalName);

    if (aNsUri != KNullDesC8 || aPrefix != KNullDesC8)
        {
        if (aPrefix == KNullDesC8)
            {
            // Check if namespace declaration for the root tag
            // is already defined in parent element.
            TXmlEngNamespace ns = aParent.LookupNamespaceByUriL(aNsUri);
            if ( ns.IsNull() )
                {
                // Namespace declaration will be added to iElement
                iElement.AddNamespaceDeclarationL(aNsUri, KNullDesC8);
                }

            WsXmlUtils domUtils;       
            domUtils.XmlEngRenameElementL(iElement, aLocalName, aNsUri, KNullDesC8);
            }
        else
            {
            // Check if namespace declaration for the root tag
            // is already defined in parent element.
            TXmlEngNamespace ns = aParent.LookupNamespaceByUriL(aNsUri);
            if ( ns.IsNull() )
                {
                // Namespace declaration will be added to iElement
                iElement.AddNamespaceDeclarationL(aNsUri, aPrefix);
                }

            WsXmlUtils domUtils;
            domUtils.XmlEngRenameElementL(iElement, aLocalName, aNsUri, aPrefix);
            }
        }
    
    aParent.AppendChildL(iElement);

    AddAttributesL(aAttributes);
    }

EXPORT_C void CSenFragmentBase::BaseConstructL(
    const TDesC8& aNsUri,
    const TDesC8& aLocalName,
    const TDesC8& aPrefix,
    const RAttributeArray& aAttributes,
    TXmlEngElement& aParent,
    RSenDocument& aOwnerDocument
    )
    {
    iDocument = aOwnerDocument.Copy();
    iElement = aParent.AddNewElementL(aLocalName);
    
    if ( aNsUri != KNullDesC8 || aPrefix != KNullDesC8 )
        {
        if ( aPrefix == KNullDesC8 )
            {
            // Check if namespace declaration for the root tag
            // is already defined in parent element.
            TXmlEngNamespace ns = aParent.LookupNamespaceByUriL(aNsUri);
            if ( ns.IsNull() )
                {
                // Namespace declaration will be added to iElement
                iElement.AddNamespaceDeclarationL(aNsUri, KNullDesC8);
                }

            WsXmlUtils domUtils;
            domUtils.XmlEngRenameElementL(iElement, aLocalName, aNsUri, KNullDesC8);
            }
        else
            {
            // Check if namespace declaration for the root tag
            // is already defined in parent element.
            TXmlEngNamespace ns = aParent.LookupNamespaceByUriL(aNsUri);
            if ( ns.IsNull() )
                {
                // Namespace declaration will be added to iElement
                iElement.AddNamespaceDeclarationL(aNsUri, aPrefix);
                }
            WsXmlUtils domUtils;
            domUtils.XmlEngRenameElementL(iElement, aLocalName, aNsUri, aPrefix);
            }
        }
    
    aParent.AppendChildL(iElement);

    AddAttributesL(aAttributes);
    }

EXPORT_C void CSenFragmentBase::BaseConstructL(const TDesC8& aNsUri,
                                               const TDesC8& aLocalName,
                                               const TDesC8& aPrefix
    )
    {
    iDocument = RSenDocument::NewL();
    if ( aNsUri == KNullDesC8 && aPrefix == KNullDesC8 )
        {
        iDocument.CreateDocumentElementL(aLocalName);
        }
    else
        {
        if ( aPrefix == KNullDesC8 )
            {
            iDocument.CreateDocumentElementL(aLocalName, aNsUri, KNullDesC8);
            }
        else
            {
            iDocument.CreateDocumentElementL(aLocalName, aNsUri, aPrefix);
            }
        }
    iElement = iDocument.DocumentElement();
    }
    
EXPORT_C void CSenFragmentBase::BaseConstructL(TXmlEngElement& aRootElement,
                                               RSenDocument& aOwnerDocument)
    {
    iDocument = aOwnerDocument.Copy();
    iElement = aRootElement;
    }

EXPORT_C void CSenFragmentBase::AddNamespacesL()
    {
    if (ipParser)
        {
        if ( ipParser->IsFeatureEnabled(EReportNamespaceMapping) )
            {
            // Add namespaces, if any
            if ( ipNamespaceArray )
                {
                TInt count = ipNamespaceArray->Count();
                for (TInt i=0; i < count; i++)
                    {
                    CSenNamespaceData* pNamespace = (*ipNamespaceArray)[i];
                    TXmlEngNamespace ns =
                                AsElementL().LookupNamespaceByUriL(*pNamespace->ipNamespaceUri);
                    if ( ns.IsNull() )
                        {
                        if (pNamespace->ipPrefix == NULL)
                            {
                            AsElementL().AddNamespaceDeclarationL(*pNamespace->ipNamespaceUri,
                                                                  KNullDesC8);
                            }
                        else
                            {
                            AsElementL().AddNamespaceDeclarationL(*pNamespace->ipNamespaceUri,
                                                                  *pNamespace->ipPrefix);
                            }
                        }
                    }
                ipNamespaceArray->ResetAndDestroy();
                }
            }
        }
    }
    
EXPORT_C void CSenFragmentBase::AddAttributesToElementL(TXmlEngElement aElement,
                                               const RAttributeArray& apAttrs)
    {
    TInt count(apAttrs.Count());

    for(TInt i=0; i<count; i++)
        {
        TPtrC8 localName = apAttrs[i].Attribute().LocalName().DesC();
        TPtrC8 value = apAttrs[i].Value().DesC();

        if (apAttrs[i].Attribute().Uri().DesC() == KNullDesC8)
            {
            aElement.AddNewAttributeL(localName, value);
            }
        else
            {
            TPtrC8 namespaceUri = apAttrs[i].Attribute().Uri().DesC();
            TPtrC8 prefix(KNullDesC8);
            if ( apAttrs[i].Attribute().Prefix().DesC() != KNullDesC8 )
                {
                prefix.Set(apAttrs[i].Attribute().Prefix().DesC());
                }
                
            TXmlEngNamespace ns = aElement.LookupNamespaceByUriL(namespaceUri);
            if ( ns.NotNull() )
                {
                if (ns.Prefix() == KNullDesC8 && prefix == KNullDesC8)
                    {
                    aElement.AddNewAttributeWithNsL(localName, value, namespaceUri);
                    }
                else if ( ns.Prefix() == prefix )
                    {
                    aElement.AddNewAttributeL(localName, value, ns);
                    }
                else
                    {
                    aElement.AddNewAttributeL(localName, value, namespaceUri, prefix);
                    }
                }
            else
                {
                if ( prefix == KNullDesC8 )
                    {
                    aElement.AddNamespaceDeclarationL(namespaceUri, KNullDesC8);
                    aElement.AddNewAttributeWithNsL(localName, value, namespaceUri);
                    }
                else
                    {
                    aElement.AddNewAttributeL(localName, value, namespaceUri, prefix);
                    }
                }
            }
        }
    }

EXPORT_C void CSenFragmentBase::AddAttributesL(const RAttributeArray& apAttrs)
    {
    AddAttributesToElementL(AsElementL(), apAttrs);
    }

EXPORT_C RSenDocument& CSenFragmentBase::AsDocumentL()
    {
    return iDocument;
    }

EXPORT_C TXmlEngElement CSenFragmentBase::AsElementL()
    {
    __ASSERT_ALWAYS(iElement.NotNull(), User::Panic(KSenFragmentPanic, EFragmentElementNotInitialized));
    
    return iElement;
    }
    
void CSenFragmentBase::RenameL(const TDesC8& aLocalName, const TDesC8& aPrefix, const TDesC8& aNamespace)
    {
    WsXmlUtils domUtils;       
    domUtils.XmlEngRenameElementL(AsElementL(), aLocalName, aNamespace, aPrefix); // Note that in TElement the *prefix* is the last arg(!)
    }

void CSenFragmentBase::RenameL(const TDesC8& aLocalName, const TDesC8& aNamespace)
    {
    WsXmlUtils domUtils;       
    domUtils.XmlEngRenameElementL(AsElementL(), aLocalName, aNamespace, KNullDesC8); // Note that in TElement the *prefix* is the last arg(!)
    }
    
void CSenFragmentBase::RenameLocalNameL(const TDesC8& aLocalName)
    {
    WsXmlUtils domUtils;       
    domUtils.XmlEngRenameElementL(AsElementL(), aLocalName, KNullDesC8, KNullDesC8); // Note that in TElement the *prefix* is the last arg(!)
    }

void CSenFragmentBase::RenameNamespaceL(const TDesC8& aNamespace)
    {
    WsXmlUtils domUtils;       
    domUtils.XmlEngRenameElementL(AsElementL(), KNullDesC8, aNamespace, KNullDesC8); // Note that in TElement the *prefix* is the last arg(!)
    }
    
void CSenFragmentBase::RenameNamespaceL(const TDesC8& aPrefix, const TDesC8& aNamespace)
    {
    WsXmlUtils domUtils;       
    domUtils.XmlEngRenameElementL(AsElementL(), KNullDesC8, aNamespace, aPrefix); // Note that in TElement the *prefix* is the last arg(!)
    }
    
void CSenFragmentBase::RenamePrefixL(const TDesC8& aPrefix)
    {
    WsXmlUtils domUtils;       
    domUtils.XmlEngRenameElementL(AsElementL(), KNullDesC8, KNullDesC8, aPrefix); // Note that in TElement the *prefix* is the last arg(!)
    }
    
EXPORT_C TXmlEngElement CSenFragmentBase::ExtractElement()
    {
    TXmlEngElement element = iDocument.DocumentElement();

    if ( element.IsSameNode(iElement) )
        {
        // Unlink only when element is root element of this document.
        //
        // This fragment maybe delegate and element is still part
        // of other Fragment's DOM Tree.
        iElement.Unlink();
        }

    TXmlEngElement retElement = iElement;        
    TXmlEngElement nullElement;
    iElement = nullElement;
    
    return retElement;
    }

EXPORT_C void CSenFragmentBase::SetParser(CSenParser& aParser)
    {
    ipParser = &aParser;
    }

EXPORT_C void CSenFragmentBase::OnDelegateParsingL(CSenFragmentBase& aDelegate)
    {
    // Delegate parsing to a new Fragment, until we encounter 
    // the end of an element with the given qualified name
    aDelegate.SetOwner(*this);
    aDelegate.ipParser = ipParser;
    aDelegate.StartSavingContent();
    SetContentHandler(aDelegate);
    }

EXPORT_C void CSenFragmentBase::OnDelegateParsingL(const RTagInfo& aElement, 
                                                   const RAttributeArray& aAttributes, 
                                                   TInt /*aErrorCode*/)
    {
    // Delegate parsing to a new Fragment, until we encounter the end of an
    // element with the given qualified name
    const TPtrC8 saxLocalName   = aElement.LocalName().DesC();
    const TPtrC8 saxNsUri       = aElement.Uri().DesC();
    const TPtrC8 saxPrefix      = aElement.Prefix().DesC();

    TXmlEngElement element = AsElementL();
    RSenDocument document = AsDocumentL();


    __ASSERT_ALWAYS(
        ipDelegate == NULL,
        User::Panic(KSenFragmentPanic, EDelegatedFragmentAlreadySet)
        );
    ipDelegate = CSenFragmentBase::NewL(saxNsUri, 
                                        saxLocalName, 
                                        saxPrefix, 
                                        aAttributes, 
                                        element, 
                                        document);

    OnDelegateParsingL(*ipDelegate);
    }

EXPORT_C void CSenFragmentBase::StartSavingContent()
    {
    iState = KSenStateSave;
    }

EXPORT_C void CSenFragmentBase::SetOwner(CSenFragmentBase& aFragment)
    {
    ipOwner = &aFragment;
    }

EXPORT_C void CSenFragmentBase::OnResumeParsingFromL(const RTagInfo& aElement, TInt aErrorCode)
    {
    SetContentHandler(*this);
    
    // Destroy delegate
    if ( ipDelegate )
        {
        HBufC8* pAsXml = ipDelegate->AsXmlL();
        CleanupStack::PushL(pAsXml);
                
        delete ipDelegate; // free memory
        ipDelegate = NULL;
        
        if ( !ipContentBuf )
            {
            ipContentBuf = CBufFlat::NewL(KFlatBufSize);
            ipContentWriteStream = new (ELeave) RBufWriteStream;
            ipContentWriteStream->Open(*ipContentBuf);
            }
        ipContentWriteStream->WriteL(*pAsXml);
        
        CleanupStack::PopAndDestroy(pAsXml);
        }
    
    TInt currentState = KSenStateNotSet; // == -1
    if ( iElement.NotNull() )
        {
        const TPtrC8 saxLocalName   = aElement.LocalName().DesC();
        const TPtrC8 saxNsUri       = aElement.Uri().DesC();
        TPtrC8 localName            = iElement.Name();
        TPtrC8 nsUri                = iElement.NamespaceUri();
    
        if(localName == saxLocalName  && nsUri == saxNsUri)
            {
            currentState = iState;
            // Before calling EndElementL, which may execute
            // unpredictable amount of code in subclasses, the
            // the state must be set to "ignore"
            iState = KSenStateResume;
            }
        }
    
    OnEndElementL(aElement, aErrorCode);
    // now check if current state was to be preserved
    if(currentState!=KSenStateNotSet) // IOP
        {
        // restore the state preserved prior to
        // "set ignore for endelement" -case
        iState = currentState;
        }
    }

EXPORT_C void CSenFragmentBase::OnStartElementL(const RTagInfo& aElement, 
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
    if (element.NamespaceUri() != KNullDesC8)
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
        }
    else if((iState & KSenStateSave) == KSenStateSave)
        {
        if(localName == saxLocalName && nsUri == saxNsUri)
            {
            // start a new BaseFragment otherwise we loose
            // track of nested elements with the same name
            OnDelegateParsingL(aElement, aAttributes, aErrorCode);
            }
        else
            {
            OnWriteStartElementL(aElement, aAttributes);
            }
        }
    }

EXPORT_C void CSenFragmentBase::OnEndElementL(const RTagInfo& aElement, TInt aErrorCode)
    {
    const TPtrC8 saxLocalName   = aElement.LocalName().DesC();
    const TPtrC8 saxNsUri       = aElement.Uri().DesC();
    const TPtrC8 saxPrefix      = aElement.Prefix().DesC();

    TXmlEngElement element = AsElementL();

    TPtrC8 localName =  element.Name();
    TPtrC8 nsUri(KNullDesC8);
    if (element.NamespaceUri() != KNullDesC8)
        {
        nsUri.Set(element.NamespaceUri());
        }

    if(localName == saxLocalName && nsUri == saxNsUri)
        {
        if(iState != KSenStateResume && ipOwner)
            {
            if ( ipContentBuf )
                {
                SetContentL(ipContentBuf->Ptr(0));
            
                delete ipContentBuf;
                ipContentBuf = NULL;
                delete ipContentWriteStream;
                ipContentWriteStream = NULL;
                }

            ipOwner->OnResumeParsingFromL(aElement, aErrorCode);
            return; // This is mandatory, since ResumeParsingFromL 
                    // de-allocates delegate fragment!
            }
        iState = KSenStateIgnore;
        }
    if((iState & KSenStateSave) == KSenStateSave)
        {
        OnWriteEndElementL(aElement);
        }
    }

EXPORT_C void CSenFragmentBase::OnContentL(const TDesC8& aBytes, TInt /*aErrorCode*/)
    {
    if((iState & KSenStateSave) == KSenStateSave)
        {
        if ( !ipContentBuf )
            {
            ipContentBuf = CBufFlat::NewL(KFlatBufSize);
            ipContentWriteStream = new (ELeave) RBufWriteStream;
            ipContentWriteStream->Open( *ipContentBuf );
            }   
        HBufC8* reEncoded = NULL;
        TBool contentIncludesEncodedChars = EncodeXmlEscapesL( aBytes, reEncoded);
        if ( contentIncludesEncodedChars )
            {
            CleanupStack::PushL( reEncoded );
            ipContentWriteStream->WriteL( *reEncoded );
            CleanupStack::PopAndDestroy( reEncoded  );
            }
        else
            {
            ipContentWriteStream->WriteL( aBytes );
            }
        }
    }
    
EXPORT_C void CSenFragmentBase::OnStartDocumentL( const RDocumentParameters& /*aDocParam*/,
                                                  TInt /*aErrorCode*/ )
    {
    if ( !ipContentBuf )
        {
        ipContentBuf = CBufFlat::NewL(KFlatBufSize);
        ipContentWriteStream = new (ELeave) RBufWriteStream;
        ipContentWriteStream->Open(*ipContentBuf);
        }
    }

EXPORT_C void CSenFragmentBase::OnEndDocumentL(TInt /*aErrorCode*/)
    {
    if ( ipNamespaceArray)
        {
        ipNamespaceArray->ResetAndDestroy();
        delete ipNamespaceArray;
        ipNamespaceArray = NULL;
        }
    
    if ( ipContentBuf )
        {
        if ( ipContentBuf->Ptr(0).Length() > 0 )
            {
            SetContentL(ipContentBuf->Ptr(0));
            }

        delete ipContentBuf;
        ipContentBuf = NULL;
        delete ipContentWriteStream;
        ipContentWriteStream = NULL;
        }
    }

EXPORT_C void CSenFragmentBase::OnStartPrefixMappingL(
                                    const RString& aPrefix,
                                    const RString& aUri,
                                    TInt /*aErrorCode*/)
    {
    if ( !ipNamespaceArray )
        {
        ipNamespaceArray = new (ELeave) RPointerArray<CSenNamespaceData>;
        }

    CSenNamespaceData* pNamespaceData = new (ELeave) CSenNamespaceData;
    CleanupStack::PushL(pNamespaceData);
    pNamespaceData->ipNamespaceUri  = aUri.DesC().AllocL();
    if (aPrefix.DesC() != KNullDesC8)
        {
        pNamespaceData->ipPrefix    = aPrefix.DesC().AllocL();
        }
    User::LeaveIfError(ipNamespaceArray->Append(pNamespaceData));
    CleanupStack::Pop(pNamespaceData);
    }

EXPORT_C void CSenFragmentBase::OnEndPrefixMappingL(
                                    const RString& /*aPrefix*/,
                                    TInt /*aErrorCode*/)
    {
    }

EXPORT_C void CSenFragmentBase::OnIgnorableWhiteSpaceL(
                                    const TDesC8& /*aBytes*/,
                                    TInt /*aErrorCode*/)
    {
    }

EXPORT_C void CSenFragmentBase::OnSkippedEntityL(
                                    const RString& /*aName*/,
                                    TInt /*aErrorCode*/)
    {
    }

EXPORT_C void CSenFragmentBase::OnProcessingInstructionL(
                                    const TDesC8& /*aTarget*/,
                                    const TDesC8& /*aData*/,
                                    TInt /*aErrorCode*/)
    {
    }

EXPORT_C void CSenFragmentBase::OnError(TInt /*aErrorCode*/)
    {
    }

EXPORT_C TAny* CSenFragmentBase::GetExtendedInterface(const TInt32 /*aUid*/)
    {
    return NULL;
    }    


EXPORT_C void CSenFragmentBase::ResetContentL()
    {
    TXmlEngElement element = AsElementL();
    element.RemoveChildren();
    }

EXPORT_C TPtrC8 CSenFragmentBase::ContentL()
    {
    TXmlEngElement element = AsElementL();
    return element.Text();
    }


EXPORT_C HBufC* CSenFragmentBase::AsXmlUnicodeL()
    {
    HBufC8* pAsXml = AsXmlL();
    CleanupStack::PushL(pAsXml);
    HBufC16* pBuf = HBufC16::NewL(2 * pAsXml->Length());
    CleanupStack::PushL(pBuf);
    TPtr16 des = pBuf->Des();
    TInt ret = CnvUtfConverter::ConvertToUnicodeFromUtf8(des, *pAsXml);
    User::LeaveIfError(ret);
    CleanupStack::Pop(pBuf);
    CleanupStack::PopAndDestroy(pAsXml);
    return pBuf;
    }

EXPORT_C HBufC8* CSenFragmentBase::AsXmlL()
    {
    TXmlEngElement element = AsElementL();
    
    TUint optionFlags = 0;
    // Omit following declarations from the beginning of XML Document:
    // <?xml version=\"1.0\...
    //   encoding="..."
    //   standalone="..."
    // ?>
    optionFlags = optionFlags | TXmlEngSerializationOptions::KOptionOmitXMLDeclaration;
    
    // Allow encoding declaration (if KOptionOmitXMLDeclaration is _not_ set)
    //optionFlags = optionFlags | TSerializationOptions::KOptionEncoding;
    
    // Allow standalone declaration (if KOptionOmitXMLDeclaration is _not_ set)
    //optionFlags = optionFlags | TSerializationOptions::KOptionStandalone;

    TXmlEngSerializationOptions options(optionFlags);

    RBuf8 asXml;
    iDocument.SaveL(asXml, element, options);
    CleanupClosePushL(asXml);
    HBufC8* pAsXml = asXml.AllocL();
    CleanupStack::PopAndDestroy(&asXml);

    return pAsXml;
    }
    

EXPORT_C void CSenFragmentBase::WriteAsXMLToL(RWriteStream& aWs)
    {
    HBufC8* pAsXml = AsXmlL();
    CleanupStack::PushL(pAsXml);
    aWs.WriteL(*pAsXml);
    CleanupStack::PopAndDestroy(pAsXml);
    }

EXPORT_C TPtrC8 CSenFragmentBase::ContentOf(const TDesC8& aLocalName)
    {
    TInt err( KErrNone );
    RXmlEngNodeList<TXmlEngElement> list;
    TRAP(   
        err, 
        TXmlEngElement element = AsElementL(); 
        element.GetElementsByTagNameL(list, aLocalName);
        )
    if ( !err && list.Count() > 0 )
        {
        TXmlEngElement firstElement = list.Next();
        return firstElement.Text();
        }
    return KNullDesC8();
    }

EXPORT_C TXmlEngElement CSenFragmentBase::SetContentOfL(const TDesC8& aLocalName,
                                                      const TDesC8& aContent)
    {
    TXmlEngElement element = AsElementL();
    RXmlEngNodeList<TXmlEngElement> list;
    CleanupClosePushL(list);
    
    element.GetElementsByTagNameL(list, aLocalName);
        
    if ( list.Count() > 0 )
        {
        TXmlEngElement firstElement = list.Next();
        firstElement.SetTextNoEncL(aContent);
        return firstElement;
        }
    CleanupStack::PopAndDestroy(&list);

    TXmlEngElement newElement = element.AddNewElementL(aLocalName);
    newElement.SetTextNoEncL(aContent);
    return newElement;
    }

EXPORT_C void CSenFragmentBase::OnWriteStartElementL(const RTagInfo& aElement,  
                                                     const RAttributeArray& aAttributes)
    {
    const TPtrC8 saxLocalName   = aElement.LocalName().DesC();
    const TPtrC8 saxNsUri       = aElement.Uri().DesC();
    const TPtrC8 saxPrefix      = aElement.Prefix().DesC();
    
    if ( !ipContentBuf )
        {
        ipContentBuf = CBufFlat::NewL(KFlatBufSize);
        ipContentWriteStream = new (ELeave) RBufWriteStream;
        ipContentWriteStream->Open(*ipContentBuf);
        }
    
    ipContentWriteStream->WriteL(KSenLessThan);
    if (saxPrefix != KNullDesC8)
        {
        ipContentWriteStream->WriteL(saxPrefix);
        ipContentWriteStream->WriteL(KSenColon);
        }
    ipContentWriteStream->WriteL(saxLocalName);

    TInt startPoint(0);
    if ( !ipParser->IsFeatureEnabled(EReportNamespaceMapping) )
        {
        if ( !ipNamespaceArray)
            {
            ipNamespaceArray = new (ELeave) RPointerArray<CSenNamespaceData>;
            }

        // Collect namespace declaration from this element (if it's not
        // previously defined)
        TBool found(EFalse);
        if ( saxNsUri != KNullDesC8 )
            {
            // Check if element of this document has already handled
            // namespace definition.
            TXmlEngNamespace ns = AsElementL().LookupNamespaceByUriL(saxNsUri);
            if ( ns.NotNull() )
                {
                found = ETrue;
                }
            else
                {
                // Check if internal namespace array has already handled
                // namespace defintion. <= Means that one of the parent
                // elements (in flat content) has already namespace
                // definition we are dealing with.
                TInt jcount = ipNamespaceArray->Count();
                for (TInt j=0; j < jcount; j++)
                    {
                    CSenNamespaceData* pNamespace = (*ipNamespaceArray)[j];
                    if ( pNamespace->ipPrefix && *(pNamespace->ipPrefix) == saxPrefix )
                        {
                        if ( *pNamespace->ipNamespaceUri == saxNsUri )
                            {
                            found = ETrue;
                            break;
                            }
                        }
                    }
                }
            }

        startPoint = ipNamespaceArray->Count();
        if ( !found )
            {
            CSenNamespaceData* pNamespaceData = new (ELeave) CSenNamespaceData;
            CleanupStack::PushL(pNamespaceData);
            TInt retVal = ipNamespaceArray->Append(pNamespaceData);
            User::LeaveIfError(retVal);
            CleanupStack::Pop(pNamespaceData);
            if (retVal == KErrNone )
                {
                pNamespaceData->ipNamespaceUri  = saxNsUri.AllocL();
                pNamespaceData->ipPrefix        = saxPrefix.AllocL();
                pNamespaceData->ipLocalName     = saxLocalName.AllocL();
                }
            }
        
        // Collect new (not previously defined) namespaces from attributes
        TInt attCount(aAttributes.Count());
        for ( TInt i = 0; i < attCount; i++)
            {
            found = EFalse;
            TPtrC8 nsUri = aAttributes[i].Attribute().Uri().DesC();
            if ( nsUri != KNullDesC8 )
                {
                TPtrC8 nsPrefix = aAttributes[i].Attribute().Prefix().DesC();
                TBool found(EFalse);
                // Check if element of this document has already handled
                // namespace definition.
                TXmlEngNamespace ns = AsElementL().LookupNamespaceByUriL(nsUri);
                if ( ns.NotNull() )
                    {
                    found = ETrue;
                    }
                else
                    {
                    // Check if internal namespace array has already handled
                    // namespace defintion. <= Means that one of the parent
                    // elements (in flat content) has already namespace
                    // definition we are dealing with.
                    TInt jcount = ipNamespaceArray->Count();
                    for (TInt j=0; j < jcount; j++)
                        {
                        CSenNamespaceData* pNamespace = (*ipNamespaceArray)[j];
                        if ( pNamespace->ipPrefix && *(pNamespace->ipPrefix) == nsPrefix )
                            {
                            if ( *pNamespace->ipNamespaceUri == nsUri )
                                {
                                found = ETrue;
                                break;
                                }
                            }
                        }
                    }
                    
                // If namespace was not found
                // namespace will be added to internal array 
                if ( !found )
                    {
                    CSenNamespaceData* pNamespaceData
                                            = new (ELeave) CSenNamespaceData;
                    CleanupStack::PushL(pNamespaceData);
                    TInt retVal = ipNamespaceArray->Append(pNamespaceData);
                    User::LeaveIfError(retVal);
                    CleanupStack::Pop(pNamespaceData);
                    if (retVal == KErrNone )
                        {
                        pNamespaceData->ipNamespaceUri  = nsUri.AllocL();
                        pNamespaceData->ipPrefix        = nsPrefix.AllocL();
                        if ( startPoint == ipNamespaceArray->Count() )
                            {
                            pNamespaceData->ipLocalName = saxLocalName.AllocL();
                            }
                        }
                    }
                }
            }
        }
        
    // Write namespaces, if any
    if ( ipNamespaceArray )
        {
        TInt count = ipNamespaceArray->Count();
        for (TInt i=startPoint; i < count; i++)
            {
            CSenNamespaceData* pNamespace = (*ipNamespaceArray)[i];
            ipContentWriteStream->WriteL(KSenSpace);
            ipContentWriteStream->WriteL(KSenXmlns);
            if ( pNamespace->ipPrefix && *(pNamespace->ipPrefix) != KNullDesC8 )
                {
                ipContentWriteStream->WriteL(KSenColon);
                ipContentWriteStream->WriteL(*pNamespace->ipPrefix);
                }
            ipContentWriteStream->WriteL(KSenEqualsDblQuot);
            ipContentWriteStream->WriteL(*pNamespace->ipNamespaceUri);
            ipContentWriteStream->WriteL(KSenDblQuot);
            }
            
        if ( ipParser->IsFeatureEnabled(EReportNamespaceMapping) )
            {
            ipNamespaceArray->ResetAndDestroy();
            }
        }

    // Write attributes, if any
    TInt attCount(aAttributes.Count());
    for (TInt i = 0; i < attCount; i++)
        {
        TPtrC8 prefix   = aAttributes[i].Attribute().Prefix().DesC();
        TPtrC8 lAttName = aAttributes[i].Attribute().LocalName().DesC();
        TPtrC8 attValue = aAttributes[i].Value().DesC();

        ipContentWriteStream->WriteL(KSenSpace);
        if (prefix != KNullDesC8)
            {
            ipContentWriteStream->WriteL(prefix);
            ipContentWriteStream->WriteL(KSenColon);
            }
        ipContentWriteStream->WriteL(lAttName);

        ipContentWriteStream->WriteL(KSenEqualsDblQuot);
        ipContentWriteStream->WriteL(attValue);
        ipContentWriteStream->WriteL(KSenDblQuot);
        }

    ipContentWriteStream->WriteL(KSenGreaterThan);
    }

EXPORT_C void CSenFragmentBase::OnWriteEndElementL(const RTagInfo& aElement)
    {
    const TPtrC8 saxLocalName = aElement.LocalName().DesC();
    const TPtrC8 saxNsUri = aElement.Uri().DesC();
    const TPtrC8 saxPrefix = aElement.Prefix().DesC();

    if ( !ipContentBuf )
        {
        ipContentBuf = CBufFlat::NewL(KFlatBufSize);
        ipContentWriteStream = new (ELeave) RBufWriteStream;
        ipContentWriteStream->Open(*ipContentBuf);
        }
    
    if ( ipNamespaceArray && !ipParser->IsFeatureEnabled(EReportNamespaceMapping) )
        {
        TInt foundIndex(KErrNotFound);
        TInt jcount = ipNamespaceArray->Count();
        for (TInt j=jcount-1; j >= 0 ; j--)
            {
            CSenNamespaceData* pNamespace = (*ipNamespaceArray)[j];
            if ( pNamespace->ipLocalName != NULL )
                {
                if ( *pNamespace->ipLocalName == saxLocalName )
                    {
                    foundIndex = j;
                    }
                break;
                }
            }
        if ( foundIndex != KErrNotFound )
            {
            for (TInt j=foundIndex; j < jcount; j++)
                {
                delete (*ipNamespaceArray)[foundIndex];
                ipNamespaceArray->Remove(foundIndex);
                }
            }
        }

    // Check if EndTag should be written or not.
    // There is no need to write EndTag if
    // previous tag is StartTag for this element
    // and there is no content for the element.
    TBool writeEndTag = ETrue;        
    TInt size = ipContentBuf->Size();
    if ( size > 1 )
        {
        // Check if first character before this possible
        // EndTag is '>'
        if ( ipContentBuf->Ptr(size-1) == KSenGreaterThan )
            {
            // This element does not have content
            // There is possibility that EndTag is not needed
            TInt colon = KErrNotFound;
            TInt space = KErrNotFound;
            TBool insideDblQuot = EFalse;
            
            // Search backwards first '<'.
            // Search also first ':' and space which
            // are right after '<' (if those characters
            // can be found).
            // 
            // => Following situations are handled:
            // 1) <prefix:localname attr...>
            // 2) <prefix:localname>
            // 3) <localname>
            TInt i=size-2;
            TPtrC8 currentCharacter(KNullDesC8);
            for (; i > 0; i--)
                {
                currentCharacter.Set(ipContentBuf->Ptr(i).Left(1));
                
                if ( currentCharacter == KSenDblQuot ) 
                    {
                    if ( insideDblQuot )
                        {
                        insideDblQuot = EFalse;
                        }
                    else
                        {
                        insideDblQuot = ETrue;
                        }
                    }
                else if ( !insideDblQuot )
                    {
                    if ( currentCharacter == KSenSlash ) 
                        {
                        i = KErrNotFound;
                        break;
                        }
                    else if ( currentCharacter == KSenLessThan ) 
                        {
                        break;
                        }
                    else if ( currentCharacter == KSenColon )
                        {
                        colon = i;
                        }
                    else if ( currentCharacter == KSenSpace )
                        {
                        colon = KErrNotFound;
                        space = i;
                        }
                    }
                }

            // Now that we know place of '<' character and
            // also places of ':' and space characters (if
            // those two characters exist),
            // we can find out possible prefix and localname.
            if ( i > 0 )
                {
                TPtrC8 localNamePtr(KNullDesC8);
                TPtrC8 prefixPtr(KNullDesC8);
                if ( colon != KErrNotFound )
                    {
                    // Both Prefix and LocalName can be found.
                    if ( space != KErrNotFound )
                        {
                        localNamePtr.Set(ipContentBuf->Ptr(colon+1).MidTPtr(0, space-colon-1));
                        }
                    else
                        {
                        localNamePtr.Set(ipContentBuf->Ptr(colon+1).MidTPtr(0, size-colon-2));
                        }
                    prefixPtr.Set(ipContentBuf->Ptr(i+1).MidTPtr(0, colon-i-1));
                    }
                else
                    {
                    // Only LocalName can be found.
                    if ( space != KErrNotFound )
                        {
                        localNamePtr.Set(ipContentBuf->Ptr(i+1).MidTPtr(0, space-i-1));
                        }
                    else
                        {
                        localNamePtr.Set(ipContentBuf->Ptr(i+1).MidTPtr(0, size-i-2));
                        }
                    }
                
                // Do comparison for localname and prefix of
                // this EndTag
                if ( ( localNamePtr == saxLocalName ) && 
                     ( prefixPtr == saxPrefix ) )
                    {
                    // LocalName and prefix matched. So
                    // the EndTag should not be written.
                    // => Now the last character '>' of StartTag can
                    //    be replaced with characters '\' and '>'
                    //    which means that element is closed without
                    //    separate EndTag.
                    ipContentBuf->Delete((ipContentBuf->Size()-1),1);
                    ipContentBuf->InsertL((ipContentBuf->Size()), KSenSlash);
                    ipContentWriteStream->WriteL(KSenGreaterThan);
                    writeEndTag = EFalse;
                    }
                }
            }
        }
    
    if ( writeEndTag )
        {
        ipContentWriteStream->WriteL(KSenLessThanSlash());
        
        if ( saxPrefix.Length() > 0)
            {
            HBufC8*  pQName = HBufC8::NewLC( saxPrefix.Length()
                                             +KSenColon().Length()
                                             +saxLocalName.Length());
            TPtr8 qname = pQName->Des();
            qname.Append(saxPrefix);
            qname.Append(KSenColon);
            qname.Append(saxLocalName);
            ipContentWriteStream->WriteL(qname);
            CleanupStack::PopAndDestroy(pQName);
            }
        else
            {
            ipContentWriteStream->WriteL(saxLocalName);
            }
       
        ipContentWriteStream->WriteL(KSenGreaterThan());
        }    
    }

EXPORT_C TXmlEngNamespace CSenFragmentBase::Namespace(const TDesC8& aPrefix)
    {
    TXmlEngNamespace ns;
    if (aPrefix.Length() == 0) return ns;
    TRAP_IGNORE
        (    
        TXmlEngElement element = AsElementL();
        ns = element.LookupNamespaceByPrefixL(aPrefix);
        )
    return ns;
    }

/*EXPORT_C void CSenFragmentBase::DetachL()
    {
    //First add all the namespaces referred to in the content.
    if (iNamespaces.Count() > 0)
        {
        TInt count(iNamespaces.Count());
        for (TInt i=0; i<count; i++)
            {
            CSenNamespace* pNamespace = (CSenNamespace*) iNamespaces[i];
            ipElement->AddNamespaceL(*pNamespace, EFalse);
            //TRAPD(err, ipElement->AddNamespaceL(*pNamespace, EFalse));
            //if (err) ;
            }
        }
    //Detach the element from its parent.
    ipElement->DetachL();
    }*/

EXPORT_C TPtrC8 CSenFragmentBase::LocalName() const
    {
    __ASSERT_ALWAYS(iElement.NotNull(), User::Panic(KSenFragmentPanic, EFragmentElementNotInitialized));
        
    return iElement.Name();
    }

EXPORT_C TPtrC8 CSenFragmentBase::NsUri() const
    {
    __ASSERT_ALWAYS(iElement.NotNull(), User::Panic(KSenFragmentPanic, EFragmentElementNotInitialized));

    return iElement.NamespaceUri();
    }

EXPORT_C TPtrC8 CSenFragmentBase::NsPrefix() const
    {
    __ASSERT_ALWAYS(iElement.NotNull(), User::Panic(KSenFragmentPanic, EFragmentElementNotInitialized));

    return iElement.Prefix();
    }
    
EXPORT_C TBool CSenFragmentBase::ConsistsOfL(TXmlEngElement& aElement, TXmlEngElement& aCandidate)
    {
    // First check the names and namespaces
    TPtrC8 elementLocalName         = aElement.Name();
    TPtrC8 candidateLocalName       = aCandidate.Name();
    // Check localnames (element should always have localname
    //                  <=> localname can't be Null).
    if ( elementLocalName != candidateLocalName )
        {
        return EFalse;
        }
        
    TPtrC8 elementNamespaceUri      = aElement.NamespaceUri();
    TPtrC8 candidateNamespaceUri    = aCandidate.NamespaceUri();
    // Check namespaceUris (element does not always have namespaceUri
    //                      <=> namespaceUri can be Null).
    if ( elementNamespaceUri != KNullDesC8 && candidateNamespaceUri != KNullDesC8 )
        {
        if ( elementNamespaceUri != candidateNamespaceUri )
            {
            return EFalse;
            }
        }
    else
        {
        if ( elementNamespaceUri != KNullDesC8 || candidateNamespaceUri != KNullDesC8 )
            {
            return EFalse;
            }
        }
        
    TPtrC8 elementPrefix            = aElement.Prefix();
    TPtrC8 candidatePrefix          = aCandidate.Prefix();
    // Check prefixes (element does not always have prefix
    //                 <=> prefix can be Null).
    if ( elementPrefix != KNullDesC8 && candidatePrefix != KNullDesC8 )
        {
        if ( elementPrefix != candidatePrefix )
            {
            return EFalse;
            }
        }
    else
        {
        if ( elementPrefix != KNullDesC8 || candidatePrefix != KNullDesC8 )
            {
            return EFalse;
            }
        }

    RBuf8 candidateContent;
    aCandidate.WholeTextContentsCopyL(candidateContent);
    CleanupClosePushL(candidateContent);
    RBuf8 content;
    aElement.WholeTextContentsCopyL(content);
    CleanupClosePushL(content);
    if ( candidateContent != content )
        {
        CleanupStack::PopAndDestroy(&content);
        CleanupStack::PopAndDestroy(&candidateContent);
        return EFalse;  // Content doesn't match => no match
        }
    CleanupStack::PopAndDestroy(&content);
    CleanupStack::PopAndDestroy(&candidateContent);
    
    // Then handle the children
    RXmlEngNodeList<TXmlEngElement> candidateChildren;
    CleanupClosePushL(candidateChildren);
    aCandidate.GetChildElements(candidateChildren);

    RXmlEngNodeList<TXmlEngElement> children;
    CleanupClosePushL(children);
    aElement.GetChildElements(children);
    
    // Element should have at least as many child elements
    // as candidateElement has.
    TInt childrenCount = children.Count();
    TInt candidateChildrenCount = candidateChildren.Count();
    if ( childrenCount < candidateChildrenCount )
        {
        CleanupStack::PopAndDestroy(&children);
        CleanupStack::PopAndDestroy(&candidateChildren);
        return EFalse;
        }
    
    // Every child element of candidate should be found
    // from element.
    while ( candidateChildren.HasNext() )
        {
        TXmlEngElement candidateMatchChild = candidateChildren.Next();
        RXmlEngNodeList<TXmlEngElement> matchChilds;
        CleanupClosePushL(matchChilds);

        aElement.GetElementsByTagNameL(matchChilds,
                                       candidateMatchChild.Name(),
                                       candidateMatchChild.NamespaceUri());
        if ( !matchChilds.HasNext() )
            {
            CleanupStack::PopAndDestroy(&matchChilds);
            CleanupStack::PopAndDestroy(&children);
            CleanupStack::PopAndDestroy(&candidateChildren);
            return EFalse;
            }
        else
            {
            TBool found = EFalse;
            while ( matchChilds.HasNext() )
                {
                TXmlEngElement matchChild = matchChilds.Next();
                found = ConsistsOfL(matchChild, candidateMatchChild);
                if ( found ) break;
                }
            if ( !found )
                {
                CleanupStack::PopAndDestroy(&matchChilds);
                CleanupStack::PopAndDestroy(&children);
                CleanupStack::PopAndDestroy(&candidateChildren);
                return EFalse;
                }
            }
            
        CleanupStack::PopAndDestroy(&matchChilds);
        }
        
    CleanupStack::PopAndDestroy(&children);
    CleanupStack::PopAndDestroy(&candidateChildren);
    return ETrue;    
    }

EXPORT_C TBool CSenFragmentBase::ConsistsOfL(CSenFragmentBase& aCandidate)
    {
    TXmlEngElement element          = AsElementL();
    TXmlEngElement candidateElement = aCandidate.AsElementL();
    return ConsistsOfL(element, candidateElement);
    }

EXPORT_C void CSenFragmentBase::SetContentHandler(
                                            CSenFragmentBase& aContentHandler)
    {
    ipParser->SetContentHandler(aContentHandler);
    }

EXPORT_C void CSenFragmentBase::SetContentL(const TDesC8& aContent)
    {
    TXmlEngElement element = AsElementL();
    element.SetTextNoEncL(aContent);
    }

EXPORT_C void CSenFragmentBase::AddContentL(const TDesC8& aContent)
    {
    TPtrC8 content = ContentL();
    TXmlEngElement element = AsElementL();
    if ( content.Length() > 0 )
        {
        HBufC8* pContent = HBufC8::NewLC( content.Length() + aContent.Length() );
        TPtr8 ptrContent = pContent->Des();
        ptrContent.Append(content);
        ptrContent.Append(aContent);
        element.SetTextNoEncL(*pContent);
        CleanupStack::PopAndDestroy(pContent);
        }
    else
        {
        element.SetTextNoEncL(aContent);
        }
    }
    
EXPORT_C void CSenFragmentBase::SetDocument(RSenDocument& aDocument)
    {
    iDocument.Close();
    iDocument = aDocument.Copy();
    }

CSenNamespaceData::~CSenNamespaceData()
    {
    delete ipNamespaceUri;
    delete ipPrefix;
    delete ipLocalName;
    }

// End of File


