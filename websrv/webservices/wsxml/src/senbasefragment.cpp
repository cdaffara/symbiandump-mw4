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

#include <flogger.h>
#include "SenXmlElement.h" // header to optimized version of SenBaseElement
#include "SenBaseFragment.h"
#include "SenXmlConstants.h"
#include "SenXmlUtils.h"

#ifdef SYMBIAN_SECURE_ECOM 
    // for 2.8, 3.0 or newer:
    #include <xml/attribute.h> // needed for RAttributeArray
#else // for 2.6 or olde
    #include "Attribute.h"
#endif

using namespace Xml;

namespace
    {
    const TInt KFlatBufSize = 128;
    }

// ***************************************************************************
// Fragment class state constants are as following (declared in derivants):
// KStateNotSet                     = -1;
// KStateIgnore                     =  0; // even ones ignore(0),
// KStateSave                       =  1; // and odd ones save(1)
// KStateResume                     =  2;
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


EXPORT_C CSenBaseFragment* CSenBaseFragment::NewL( const CSenElement& aElement )
    {
    CSenBaseFragment* pNew = new (ELeave) CSenBaseFragment;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aElement);
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

EXPORT_C CSenBaseFragment* CSenBaseFragment::NewL( const TDesC8& aLocalName )
    {
    CSenBaseFragment* pNew = new (ELeave) CSenBaseFragment;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aLocalName);
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

EXPORT_C CSenBaseFragment* CSenBaseFragment::NewL(
    const TDesC8& aNsUri,
    const TDesC8& aLocalName
    )
    {
    CSenBaseFragment* pNew = new (ELeave) CSenBaseFragment;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aNsUri, aLocalName);
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

EXPORT_C CSenBaseFragment* CSenBaseFragment::NewL(
    const TDesC8& aNsUri,
    const TDesC8& aLocalName,
    const TDesC8& aQName,
    const RAttributeArray& aAttrs
    )
    {
    CSenBaseFragment* pNew = new (ELeave) CSenBaseFragment;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aNsUri, aLocalName, aQName, aAttrs);
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

EXPORT_C CSenBaseFragment* CSenBaseFragment::NewL( const TDesC8& aNsUri,
                                                   const TDesC8& aLocalName,
                                                   const TDesC8& aQName,
                                                   const RAttributeArray& aAttrs,
                                                   CSenElement& aParent )
    {
    CSenBaseFragment* pNew = new (ELeave) CSenBaseFragment;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aNsUri, aLocalName, aQName, aAttrs, aParent);
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

EXPORT_C CSenBaseFragment* CSenBaseFragment::NewL( const TDesC8& aNsUri, 
                                                   const TDesC8& aLocalName,
                                                   const TDesC8& aQName )
    {
    CSenBaseFragment* pNew = new (ELeave) CSenBaseFragment;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aNsUri, aLocalName, aQName);
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

EXPORT_C CSenBaseFragment::~CSenBaseFragment()
    {
    if(iElementOwned)
        {
        delete ipElement;
        }

    delete ipDelegate;

    if(ipContentWithNamespaces)
        {
        ipContentWithNamespaces->Reset();
        }

    iNamespaces.Reset(); // not owned
    iPrefixes.ResetAndDestroy(); // owned copies from parsed values..
    }

EXPORT_C CSenBaseFragment::CSenBaseFragment()
: iState(KStateIgnore),
  ipElement(NULL),
  iXmlReader(NULL),
  ipOwner(NULL),
  ipDelegate(NULL),
  ipContentWithNamespaces(NULL),
  hasContentWriteStream(EFalse),
  iElementOwned(ETrue)
    {
    }

EXPORT_C void CSenBaseFragment::BaseConstructL( const CSenElement& aElement )
    {

    ipElement = CSenXmlElement::NewL(aElement.LocalName());
    ipElement->CopyFromL((CSenElement&)aElement);
    }

EXPORT_C void CSenBaseFragment::BaseConstructL( const TDesC8& aLocalName )
    {
    ipElement = CSenXmlElement::NewL(aLocalName);
    }

EXPORT_C void CSenBaseFragment::BaseConstructL( const TDesC8& aNsUri,
                                                const TDesC8& aLocalName )
    {
    ipElement = CSenXmlElement::NewL(aNsUri, aLocalName);
    }

EXPORT_C void CSenBaseFragment::BaseConstructL( const TDesC8& aNsUri,
                                                const TDesC8& aLocalName,
                                                const TDesC8& aQName,
                                                const RAttributeArray& aAttrs )
    {
    ipElement = CSenXmlElement::NewL(aNsUri, aLocalName, aQName, aAttrs);
    }

EXPORT_C void CSenBaseFragment::BaseConstructL( const TDesC8& aNsUri,
                                                const TDesC8& aLocalName,
                                                const TDesC8& aQName,
                                                const RAttributeArray& aAttrs,
                                                CSenElement& aParent )
    {
    // this transfers the ipElement ownership to the parent(!)
    ipElement = CSenXmlElement::NewL(aNsUri, aLocalName, aQName, aAttrs, aParent);
    }

EXPORT_C void CSenBaseFragment::BaseConstructL( const TDesC8& aNsUri,
                                                const TDesC8& aLocalName,
                                                const TDesC8& aQName )
    {
    ipElement = CSenXmlElement::NewL(aNsUri, aLocalName, aQName);
    }

// Leaves the the fragment without initialized ipElement
EXPORT_C void CSenBaseFragment::BaseConstructL( CSenXmlReader& aReader )
    {
    iXmlReader = &aReader;
    }

EXPORT_C CSenElement& CSenBaseFragment::AsElement()
    {
    __ASSERT_ALWAYS(ipElement != NULL, User::Panic(KSenXmlPanic, EFragmentElementNotInitialized));
    return *ipElement;
    }

EXPORT_C CSenElement* CSenBaseFragment::ExtractElement()
    {
    CSenElement* pElement = ipElement;
    ipElement = NULL;
    return pElement;
    }

EXPORT_C CSenXmlReader* CSenBaseFragment::Reader()
    {
    return iXmlReader;
    }

EXPORT_C void CSenBaseFragment::SetReader( CSenXmlReader& aReader )
    {
    iXmlReader = &aReader;
    }

EXPORT_C TInt CSenBaseFragment::BuildFrom( const TDesC8& aBuf )
    {
    TRAPD(err, ParseL(aBuf));
    return err;
    }


EXPORT_C void CSenBaseFragment::ParseL( const TDesC8& aBuf )
    {
    iState = KStateIgnore;
    if(!iXmlReader)
        {
        User::Leave(KErrSenXmlReaderNotSet);
        }
    iXmlReader->SetContentHandler(*this);
    iXmlReader->ParseL(aBuf);
    }

EXPORT_C void CSenBaseFragment::DelegateParsingL(MSenFragment& aDelegate)
    {
    // Delegate parsing to a new Fragment, until we encounter 
    // the end of an element with the given qualified name
    aDelegate.SetOwner(*this);

    if(!iXmlReader)
        {
        User::Leave(KErrSenXmlReaderNotSet);
        }
    aDelegate.ParseWithL(*iXmlReader);
    }

EXPORT_C void CSenBaseFragment::DelegateParsingL( const TDesC8& aNsUri,
                                                  const TDesC8& aLocalName,
                                                  const TDesC8& aQName,
                                                  const RAttributeArray& aAttrs )
    {
    // Delegate parsing to a new Fragment, until we encounter the end of an
    // element with the given qualified name
    __ASSERT_ALWAYS( ipDelegate == NULL, User::Panic(KSenXmlPanic, EDelegatedFragmentAlreadySet) );
    ipDelegate = CSenBaseFragment::NewL(aNsUri, aLocalName, aQName);

    ipDelegate->SetAttributesL(aAttrs);
    DelegateParsingL(*ipDelegate);
    }

EXPORT_C void CSenBaseFragment::ParseWithL(CSenXmlReader& aReader)
    {
    iXmlReader = &aReader;
    iState = KStateSave;
    iXmlReader->SetContentHandler(*this);
    }

EXPORT_C void CSenBaseFragment::SetOwner(MSenFragment& aFragment)
    {
    ipOwner = &aFragment;
    AsElement().SetParent(&aFragment.AsElement());
    }

EXPORT_C void CSenBaseFragment::ResumeParsingFromL( const TDesC8& aNsUri,
                                                    const TDesC8& aLocalName,
                                                    const TDesC8& aQName )
    {
    if(!iXmlReader)
        {
        User::Leave(KErrSenXmlReaderNotSet);
        }

    // listen to content handler
    iXmlReader->SetContentHandler(*this);
    
    // Destroy delegate
    if (ipDelegate)
        {
        CSenElement* pElement = ipDelegate->ExtractElement();
        CleanupStack::PushL(pElement); // IOP
        ipElement->AddElementL(*pElement);
        CleanupStack::Pop(); // pElement;
        delete ipDelegate; // free memory
        ipDelegate = NULL;
        }
    
    
    TInt currentState = KStateNotSet; // == -1
    if(ipElement)
        {
        const TDesC8& localName = ipElement->LocalName();
        const TDesC8& nsUri = ipElement->NamespaceURI();
    
        if(localName == aLocalName  && nsUri == aNsUri)
            {
            currentState = iState;
            // Before calling EndElementL, which may execute
            // unpredictable amount of code in derivants, the
            // the state must be set to "ignore"
            iState = KStateResume;
            }
        }
    
    EndElementL(aNsUri, aLocalName, aQName); // 2004-07-17
    // now check if current state was to be preserved
    if(currentState!=KStateNotSet) // IOP
        {
        // restore the state preserved prior to
        // "set ignore for endelement" -case
        iState = currentState;
        }
    }

EXPORT_C void CSenBaseFragment::StartElementL( const TDesC8& aNsUri,
                                               const TDesC8& aLocalName,
                                               const TDesC8& aQName,
                                               const RAttributeArray& aAttrs )
    {
    // 2004-09-10: check & alloc moved out
    // from KStateIgnore case code segment below (where in Java)
    if(!ipElement)
        {
        ipElement = CSenXmlElement::NewL( aNsUri, aLocalName, aQName );
        }

    // we can assume, that ipElement is not NULL
    const TDesC8& localName = ipElement->LocalName();
    const TDesC8& nsUri = ipElement->NamespaceURI();
    if(iState == KStateIgnore)
        {
        if(localName == aLocalName && nsUri == aNsUri) 
            {
            iState = KStateSave;
            SetAttributesL(aAttrs);
            }
        }
    else if((iState & KStateSave) == KStateSave)
        {
        if(localName == aLocalName && nsUri == aNsUri)
            {
            // start a new BaseFragment otherwise we loose
            // track of nested elements with the same name
            DelegateParsingL(aNsUri, aLocalName, aQName, aAttrs);
            }
        else
            {
            WriteStartElementL(aNsUri, aLocalName, aQName, aAttrs);
            }
        }
    }

EXPORT_C void CSenBaseFragment::EndElementL( const TDesC8& aNsUri,
                                             const TDesC8& aLocalName,
                                             const TDesC8& aQName )
    {
    AllocContentSaverL();
    const TDesC8& localName = ipElement->LocalName();
    const TDesC8& nsUri = ipElement->NamespaceURI();

    if(localName == aLocalName && nsUri == aNsUri)
        {
        if(iState != KStateResume && ipOwner)
            {
            ipOwner->ResumeParsingFromL(aNsUri, aLocalName, aQName);
            return; // This is mandatory, since ResumeParsingFromL 
                    // de-allocates delegate fragment!
            }
        iState = KStateIgnore;
        }
    if((iState & KStateSave) == KStateSave)
        {
        WriteEndElementL(aNsUri, aLocalName, aQName);
        }
    }

// Closely the same as the code in Java ref-impl's BaseElement.append()
EXPORT_C void CSenBaseFragment::CharactersL( const TDesC8& aChars,
                                             TInt /*aStart*/,
                                             TInt /*aLength*/ )
    {
    if((iState & KStateSave) == KStateSave)
        {
        AllocContentSaverL(); // get iWs if it is NULL

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
    }

EXPORT_C void CSenBaseFragment::SetAttributesL( const RAttributeArray& aAttrs )
    {
    SaveNamespacesL(aAttrs, ETrue);
    // Otherwise ignore attributes, these are up to the subclass to deal with.
    }

EXPORT_C void CSenBaseFragment::AllocContentSaverL()
    {
    if(!hasContentWriteStream)
        {
        iWs = ipElement->ContentWriteStreamL();
        hasContentWriteStream = ETrue;
        }
    }

EXPORT_C TInt CSenBaseFragment::StartDocument()
    {
    return 1;
    }

EXPORT_C TInt CSenBaseFragment::EndDocument()
    {
    if ( hasContentWriteStream )
        {
        hasContentWriteStream = EFalse;
        iWs.Close();
        }
        
    if (ipElement) ((CSenXmlElement*)ipElement)->Compress();
    return 1;
    }

EXPORT_C TInt CSenBaseFragment::StartElement( const TDesC8& aNsUri,
                                              const TDesC8& aLocalName,
                                              const TDesC8& aQName,
                                              const RAttributeArray& aAttrs )
    {
    TRAPD(err, StartElementL(aNsUri, aLocalName, aQName, aAttrs));
    return (err == KErrNone);
    }

EXPORT_C TInt CSenBaseFragment::EndElement( const TDesC8& aNsUri,
                                            const TDesC8& aLocalName,
                                            const TDesC8& aQName )
    {
    TRAPD(err, EndElementL(aNsUri, aLocalName, aQName));
    return (err == KErrNone);
    }

EXPORT_C TInt CSenBaseFragment::Characters( const TDesC8& aChars,
                                            const TInt aStart,
                                            const TInt aLength)
    {
    TRAPD(err, CharactersL(aChars, aStart, aLength));
    return (err == KErrNone);
    }

EXPORT_C void CSenBaseFragment::ResetContentL()
    {
    AsElement().SetContentL(KNullDesC8);
    // iPrefixes.Reset();   // contents not owned, clearing is enough // 2005-09-26: wouldn't this leak memory?
    iPrefixes.ResetAndDestroy(); // owned copies from parsed values..
    iNamespaces.Reset(); // contents not owned, clearing is enough
    }

EXPORT_C TPtrC8 CSenBaseFragment::Content()
    {
    TPtrC8 retVal;
    TRAPD(err, retVal.Set(ContentL());)

    if (err != KErrNone)
        {
        retVal.Set(KNullDesC8);
        }
    return retVal;
    }

TPtrC8 CSenBaseFragment::ContentL()
    {
    TPtrC8 content = AsElement().Content();

    TInt nsCount(iNamespaces.Count());
    if(nsCount>0)
        {
        TInt i = content.Locate('>');
        if( i > -1 )
            {
            if(!ipContentWithNamespaces)
                {
                ipContentWithNamespaces = CBufFlat::NewL(KFlatBufSize);
                }
            else
                {
                ipContentWithNamespaces->Reset(); //discard the old data in buf
                }

            i--; // make sure we are before a possible "/>"
            TInt cut = i;
            RBufWriteStream ws;
            CleanupClosePushL(ws);
            ws.Open(*ipContentWithNamespaces);
            ws.WriteL(content.Left(cut));
            for(TInt n=0; n<nsCount; n++)
                {
                CSenNamespace* pNamespace = (CSenNamespace*)iNamespaces[i];
                pNamespace->InsertL(ws);
                }
            ws.WriteL(content.Mid(cut));
            CleanupStack::PopAndDestroy(); // RBufWriteStream.Close() for ws
            TPtr8 p8 = ipContentWithNamespaces->Ptr(0);
            return p8;
            }
        }
        else
            {
            return content;
            }
    return content;
    }

EXPORT_C HBufC* CSenBaseFragment::AsXmlUnicodeL()
    {
    return AsElement().AsXmlUnicodeL();
    }

EXPORT_C HBufC8* CSenBaseFragment::AsXmlL()
    {
    return AsElement().AsXmlL();
    }

EXPORT_C void CSenBaseFragment::WriteAsXMLToL( RWriteStream& aWs )
    {
    AsElement().WriteAsXMLToL( aWs );
    }

EXPORT_C TPtrC8 CSenBaseFragment::ContentOf( const TDesC8& aLocalName )
    {
    CSenElement* child = this->AsElement().Element(aLocalName);
    if(child)
        {
        return child->Content();
        }
    return KNullDesC8();
    }

EXPORT_C MSenElement& CSenBaseFragment::SetContentOfL( const TDesC8& aLocalName,
                                                       const TDesC8& aContent )
    {
    CSenElement* pChildElement = AsElement().Element( aLocalName );
    if(!pChildElement)
        {
        // Add new element
        pChildElement =  &AsElement().AddElementL(aLocalName);
        }
    pChildElement->SetContentL(aContent);
    return *pChildElement;
    }

EXPORT_C void CSenBaseFragment::WriteStartElementL( const TDesC8& /* aNsUri */,
                                                    const TDesC8& aLocalName,
                                                    const TDesC8& aQName,
                                                    const RAttributeArray& aAttrs )
    {
    AllocContentSaverL();

    // Write element name with namespace
    iWs.WriteL(KSenLessThan);
    if (aQName != KNullDesC8)
        {
        iWs.WriteL(aQName);
        }
    else
        {
        iWs.WriteL(aLocalName);
        }

    // Write attributes, if any
    HBufC8* pQName = NULL;
    TInt attCount(aAttrs.Count());
    for (TInt i = 0; i < attCount; i++)
            {
        TPtrC8 prefix = aAttrs[i].Attribute().Prefix().DesC();
        TPtrC8 lAttName = aAttrs[i].Attribute().LocalName().DesC();
        TPtrC8 attValue = aAttrs[i].Value().DesC();
        
        TPtrC8 qAttName = lAttName;
        if(prefix!=KNullDesC8)
            {
            pQName = HBufC8::NewLC( prefix.Length()
                        +KSenColon().Length()
                        +lAttName.Length());
            TPtr8 qname = pQName->Des();
            qname.Append(prefix);
            qname.Append(KSenColon);
            qname.Append(lAttName);
            qAttName.Set(qname);
            }
        
        
        iWs.WriteL(KSenSpace);
        iWs.WriteL(qAttName); // this is localname if no qualifier is set
        iWs.WriteL(KSenEqualsDblQuot);
        iWs.WriteL(attValue);
        iWs.WriteL(KSenDblQuot);
        
        TInt colon(KErrNotFound);
        colon = qAttName.Locate(':');
        // We check for namespaces declared here and for prefixes 
        // in attribute names and values.
        if (qAttName == KSenXmlns || lAttName == KSenXmlns)
            {
            // locally declared default namespace, 
            // no need to do anything special
            }
        else if(prefix == KSenXmlns) // was SenXmlUtils::StartsWith(...)
            {
            //locally declared prefix, remember that this prefix is OK
            HBufC8* pAttName = lAttName.AllocL();
            CleanupStack::PushL(pAttName);
#ifdef EKA2
            iPrefixes.AppendL(pAttName);
#else
            User::LeaveIfError(iPrefixes.Append(pAttName));
#endif
            CleanupStack::Pop(); // pAttName
            }
        else if(colon != KErrNotFound)
            {
            //a qualified attribute name, ensure we have its namespace
            EnsureNamespace(qAttName.Left(colon));
            }
        
        colon = attValue.Locate(':');
        if(colon != KErrNotFound)
            {
            //encountered a qualified value, should ensure the namespace for
            //its prefix is included.
            EnsureNamespace(attValue.Left(colon));
            }
        if(prefix!=KNullDesC8)
            {       
            CleanupStack::PopAndDestroy(); // qAttName
            pQName = NULL;
            }
        }

    iWs.WriteL(KSenGreaterThan);


    if(aQName != KNullDesC8)
        {
        TInt colon(aQName.Locate(':'));
        if(colon != KErrNotFound)
            {
            EnsureNamespace(aQName.Left(colon));
            }
        }

    // Is this truly required!?
    if(aLocalName != KNullDesC8)
        {
        TInt colon(aLocalName.Locate(':'));
        if(colon != KErrNotFound)
            {
            EnsureNamespace(aLocalName.Left(colon));
            }
        }
    
    }

EXPORT_C void CSenBaseFragment::WriteEndElementL( const TDesC8& /* aNsUri */,
                                                  const TDesC8& /* aLocalName */,
                                                  const TDesC8& aQName )
    {
    AllocContentSaverL();
    iWs.WriteL(KSenLessThanSlash);
    iWs.WriteL(aQName);
    iWs.WriteL(KSenGreaterThan);
    }

EXPORT_C void CSenBaseFragment::EnsureNamespace( const TDesC8& aPrefix )
    {
    /* Check that the prefix has been declared.
     * If not try to find a namespace declaration for it
     * in the element.
     */
    TInt pos(KErrNotFound);
    TInt count(iPrefixes.Count());
    for(TInt i=0; i<count && pos == KErrNotFound; i++)
        {
        if(*iPrefixes[i] == aPrefix)
            {
            pos = i;
            break;
            }
        }

    if (pos == KErrNotFound)
        {
        const CSenNamespace* pNamespace = Namespace(aPrefix);
        if (!pNamespace)
            {
            pNamespace = ipElement->Namespace(aPrefix);

            if (pNamespace)
                {
                iNamespaces.Append(pNamespace); // no need for copying
                }
            }
        }
    }

EXPORT_C CSenNamespace* CSenBaseFragment::Namespace( const TDesC8& aPrefix )
    {
    if (aPrefix.Length() == 0) return NULL;

    CSenNamespace* pNamespace = NULL;
    TInt count(iNamespaces.Count());
    for (TInt i=0; i<count && !pNamespace; i++)
        {
        pNamespace = (CSenNamespace*) iNamespaces[i];
        if (pNamespace->Prefix() != aPrefix) pNamespace = NULL;
        }

    return pNamespace;
    }

EXPORT_C void CSenBaseFragment::DetachL()
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
    }

EXPORT_C void CSenBaseFragment::SaveNamespacesL( const RAttributeArray& aAttrs,
                                                 TBool aEnsure )
    {
    // we check for namespace declarations in here
    // for prefixes in attribute names and values.

    if (ipElement)
        {
        CSenElement* pElement = (CSenXmlElement*)ipElement;

        TInt colon = KErrNotFound;
        TInt length(aAttrs.Count());

        for (TInt i=0; i<length; i++)
            {
            TPtrC8 localName = aAttrs[i].Attribute().LocalName().DesC();
            TPtrC8 value = aAttrs[i].Value().DesC();
            TPtrC8 prefix = aAttrs[i].Attribute().Prefix().DesC();
            
            if (localName == KSenXmlns())
                {
                // this is a default name space declaration
                pElement->SetNamespaceL(value);
                }
            else if (localName == pElement->NsPrefix())
                {
                //we have a new declaration for the namespace of this element
                pElement->SetNamespaceL(localName,value);
                }
            else if (prefix == KSenXmlns())
                {
                // we have an additional namespace declaration
                pElement->AddNamespaceL(localName,value);
                }

            if (aEnsure)
                {
                //colon = qualifiedName.Locate(':');
                //if (colon > -1)
                if(localName!=KNullDesC8)
                    {
                    // attribute name is prefixed so ensure we have its
                    // namespace
                    //EnsureNamespace(qualifiedName.Mid(0,colon));

                    // attribute's name is elements prefix! // 2005-02-22
                    EnsureNamespace(localName);
                    }

                colon = value.Locate(':');
                if (colon > -1)
                    {
                    // attribute value is prefixed so ensure we have that
                    // namespace
                    EnsureNamespace(value.Mid(0,colon));
                    }
                }
            }
        }
    }


// New
EXPORT_C const TDesC8& CSenBaseFragment::LocalName() const
    {
    __ASSERT_ALWAYS(ipElement != NULL, User::Panic(KSenXmlPanic, EFragmentElementNotInitialized));
    return ipElement->LocalName();
    }

EXPORT_C const TDesC8& CSenBaseFragment::NsUri() const
    {
    __ASSERT_ALWAYS(ipElement != NULL, User::Panic(KSenXmlPanic, EFragmentElementNotInitialized));
    return ipElement->NamespaceURI();
    }

EXPORT_C const TDesC8& CSenBaseFragment::NsPrefix() const
    {
    __ASSERT_ALWAYS(ipElement != NULL, User::Panic(KSenXmlPanic, EFragmentElementNotInitialized));
    return ipElement->NsPrefix();
    }

// Do NOT change method signature; BC must be kept!
EXPORT_C TInt CSenBaseFragment::StartEntity( TDesC8& aName )
    {
#ifndef SYMBIAN_SECURE_ECOM        
    // This segment is required with Compact XML reader
    TInt leaveCode(KErrNone);
    if((iState & KStateSave) == KStateSave)
        {
        TRAP(leaveCode, AllocContentSaverL());
        if(leaveCode==KErrNone)
            {
            TRAP(leaveCode, iWs.WriteL(aName));
            }
        }
    if(leaveCode==KErrNone)
        {
        return ETrue; // success
        }
    else
        {
        return EFalse; // failure
        }
#else // SYMBIAN_SECURE_ECOM       
    aName.Length();
    return KErrNone; // signal was successfully handled
#endif // SYMBIAN_SECURE_ECOM    
    }


EXPORT_C TInt CSenBaseFragment::Error( TInt /* aErrorCode */ )
    {
    //return aErrorCode;
    return KErrNone; // Error was "successfully" handled
    }

EXPORT_C TBool CSenBaseFragment::ConsistsOfL( MSenFragment& aCandidate )
    {
    return AsElement().ConsistsOfL(aCandidate.AsElement());
    }


// End of File
