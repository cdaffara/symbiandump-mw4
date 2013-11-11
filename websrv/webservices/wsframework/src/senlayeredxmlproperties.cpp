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








#include <s32strm.h> // RWriteStream
#include <SenXmlReader.h>
#include "senpropertieselement.h"   // private: Utils\inc
#include "senpropertiesfragment.h"  // private: Utils\inc
#include <SenXmlUtils.h>
#include <SenXmlConstants.h>

#include "senlayeredxmlproperties.h"
#include "SenXmlProperties.h"
#include "senxmldebug.h"

namespace
    {
    const TInt KFlatBufSize         = 64;  // CBufFlat
    }

CSenLayeredXmlProperties* CSenLayeredXmlProperties::NewL()
    {
    CSenLayeredXmlProperties* pNew =
            CSenLayeredXmlProperties::NewLC();
    CleanupStack::Pop(); // pNew
    return pNew;
    }

CSenLayeredXmlProperties* CSenLayeredXmlProperties::NewLC()
    {
    CSenLayeredXmlProperties* pNew = new (ELeave) CSenLayeredXmlProperties;
    CleanupStack::PushL(pNew);

    // omit parsing by passing zero-length descriptor
    pNew->BaseConstructL(KSenLayeredXmlPropertiesLocalname,
                         KNullDesC8,
                         NULL);
    return pNew;
    }

CSenLayeredXmlProperties* CSenLayeredXmlProperties::NewL(const TDesC8& aXmlUtf8,
                                                         CSenXmlReader& aParser)
    {
    CSenLayeredXmlProperties* pNew = NewLC(aXmlUtf8, aParser);
    CleanupStack::Pop(); // pNew
    return pNew;
    }

CSenLayeredXmlProperties* CSenLayeredXmlProperties::NewLC(const TDesC8& aXmlUtf8,
                                                          CSenXmlReader& aParser)
    {
    CSenLayeredXmlProperties* pNew = new (ELeave) CSenLayeredXmlProperties;
    CleanupStack::PushL(pNew);

    // parse the XML document  descriptor into DOM fragment
    pNew->BaseConstructL(KSenLayeredXmlPropertiesLocalname,
                         aXmlUtf8,
                         &aParser);
    return pNew;
    }


CSenLayeredXmlProperties* CSenLayeredXmlProperties::NewL(const CSenElement& aElement)
    {
    CSenLayeredXmlProperties* pNew =
                CSenLayeredXmlProperties::NewLC(aElement);
    CleanupStack::Pop(); // pNew
    return pNew;
    }

CSenLayeredXmlProperties* CSenLayeredXmlProperties::NewLC(const CSenElement& aElement)
    {
    CSenLayeredXmlProperties* pNew = new (ELeave) CSenLayeredXmlProperties;
    CleanupStack::PushL(pNew);

    // Parse the XML document  descriptor into DOM fragment
    pNew->BaseConstructL(aElement);
    return pNew;
    }

CSenLayeredXmlProperties* CSenLayeredXmlProperties::NewL(RStringPool& aStringPool)
    {
    CSenLayeredXmlProperties* pNew =
            CSenLayeredXmlProperties::NewLC(aStringPool);
    CleanupStack::Pop(); // pNew
    return pNew;
    }

CSenLayeredXmlProperties* CSenLayeredXmlProperties::NewLC(RStringPool& aStringPool)
    {
    CSenLayeredXmlProperties* pNew = new (ELeave) CSenLayeredXmlProperties;
    CleanupStack::PushL(pNew);

    // omit parsing by passing zero-length descriptor
    pNew->BaseConstructL(KSenLayeredXmlPropertiesLocalname,
                         KNullDesC8,
                         NULL,
                         &aStringPool);
    return pNew;
    }

CSenLayeredXmlProperties* CSenLayeredXmlProperties::NewL(const TDesC8& aXmlUtf8,
                                                         CSenXmlReader& aParser,
                                                         RStringPool& aStringPool)
    {
    CSenLayeredXmlProperties* pNew = NewLC(aXmlUtf8, aParser, aStringPool);
    CleanupStack::Pop(); // pNew
    return pNew;
    }

CSenLayeredXmlProperties* CSenLayeredXmlProperties::NewLC(const TDesC8& aXmlUtf8,
                                                          CSenXmlReader& aParser,
                                                          RStringPool& aStringPool)
    {
    CSenLayeredXmlProperties* pNew = new (ELeave) CSenLayeredXmlProperties;
    CleanupStack::PushL(pNew);

    // parse the XML document  descriptor into DOM fragment
    pNew->BaseConstructL(KSenLayeredXmlPropertiesLocalname,
                         aXmlUtf8,
                         &aParser,
                         &aStringPool);
    return pNew;
    }


CSenLayeredXmlProperties* CSenLayeredXmlProperties::NewL(const CSenElement& aElement,
                                                         RStringPool& aStringPool)
    {
    CSenLayeredXmlProperties* pNew =
                CSenLayeredXmlProperties::NewLC(aElement, aStringPool);
    CleanupStack::Pop(); // pNew
    return pNew;
    }

CSenLayeredXmlProperties* CSenLayeredXmlProperties::NewLC(const CSenElement& aElement,
                                                          RStringPool& aStringPool)
    {
    CSenLayeredXmlProperties* pNew = new (ELeave) CSenLayeredXmlProperties;
    CleanupStack::PushL(pNew);

    // Parse the XML document  descriptor into DOM fragment
    pNew->BaseConstructL(aElement, &aStringPool);
    return pNew;
    }

    
void CSenLayeredXmlProperties::SetReader(CSenXmlReader& aReader)
    {
    // De-allocate the owned reader, if one exists:
    if( ipReader )
        {
        delete ipReader; 
        ipReader = NULL;
        }
    // Note: ipFragment does not own the reader 
    // behind the given reference (aReader) 
    ipFragment->SetReader(aReader);
    
    if ( ipChildProperties )
        {
        ipChildProperties->SetReader(aReader);
        }
    }

MSenProperties::TSenPropertiesClassType CSenLayeredXmlProperties::PropertiesClassType() 
    { 
    return ESenLayeredXmlProperties; 
    }

void CSenLayeredXmlProperties::WriteToL(RWriteStream& aWriteStream)
    {
    // Find out whether we should declare the namespace
    TPtrC8 nsPrefix = ipFragment->NsPrefix();

    // Element name
    aWriteStream.WriteL(KSenLessThan);
    if ( nsPrefix.Length() > 0 )
        {
        aWriteStream.WriteL(nsPrefix);
        aWriteStream.WriteL(KSenColon);
        }
    aWriteStream.WriteL(ipFragment->AsElement().LocalName());

    RPointerArray<CSenBaseAttribute>& attrs = ipFragment->AsElement().AttributesL();
    RPointerArray<CSenNamespace>& namespaces = ipFragment->AsElement().NamespacesL();
    if ( ( attrs.Count() > 0 ) || ( namespaces.Count() > 0 ) )
        {
        CSenNamespace* ns = NULL;
        TInt count = namespaces.Count();
        for (TInt i=0; i < count; i++)
            {
            ns = (namespaces)[i];
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
        count = attrs.Count();
        for (TInt j = 0; j < count; j++)
            {
            aWriteStream.WriteL(KSenSpace);
            aWriteStream.WriteL((attrs)[j]->Name());
            aWriteStream.WriteL(KSenEqualsDblQuot);
            aWriteStream.WriteL((attrs)[j]->Value());
            aWriteStream.WriteL(KSenDblQuot);
            }
        }
        
    // Elements and content
    RPointerArray<CSenElement> elements;
    ElementsL(elements);
    if ( (elements.Count() > 0) || ipFragment->AsElement().HasContent() )
        {
        aWriteStream.WriteL(KSenGreaterThan);

        // Body
        TInt elementCount(elements.Count());
        for (TInt k=0; k<elementCount; k++)
            {
            elements[k]->WriteAsXMLToL(aWriteStream);
            }        
        aWriteStream.WriteL(ipFragment->AsElement().Content());

        // Closing element
        aWriteStream.WriteL(KSenLessThanSlash);
        if (nsPrefix.Length() > 0)
            {
            aWriteStream.WriteL(nsPrefix);
            aWriteStream.WriteL(KSenColon);
            }
        aWriteStream.WriteL(ipFragment->AsElement().LocalName());
        aWriteStream.WriteL(KSenGreaterThan);
        }
    else
        {
        aWriteStream.WriteL(KSenSlashGreaterThan);
        }
    elements.Close();
    }

void CSenLayeredXmlProperties::ReadFromL(const TDesC8& aBuffer)
    {
    // Sanity check: ignore zero-length strings
    if ( aBuffer.Length() == 0)
        {
        return;
        }
    // Check if fragment already has a reference to *some*
    // XML reader
    if ( !ipFragment->Reader() )
        {
        // Check if there is already an owned reader instance
        if( !ipReader )
            {
            ipReader = CSenXmlReader::NewL(KXmlParserMimeType); // use libxml2 sax parser
            }
        // Sets reference of that XML reader for this fragment:
        ipFragment->ParseWithL(*ipReader);
        }
    ipFragment->ParseL(aBuffer);   
    }
    
HBufC8* CSenLayeredXmlProperties::AsUtf8L()
    {
    CBufFlat *pBuf = CBufFlat::NewL(KFlatBufSize);
    CleanupStack::PushL(pBuf);
    RBufWriteStream bufWs(*pBuf);
    CleanupClosePushL(bufWs);
    this->WriteToL(bufWs);
    CleanupStack::PopAndDestroy(); // bufWs.Close();
    TPtrC8 p = pBuf->Ptr(0);    
    HBufC8* pRet = p.AllocL();
    CleanupStack::PopAndDestroy(); // pBuf;
    return pRet;
    
    // return ipFragment->AsXmlL();
    }

HBufC8* CSenLayeredXmlProperties::AsUtf8LC()
    {
    HBufC8* pRet = this->AsUtf8L();
    CleanupStack::PushL(pRet);
    return pRet;
    }
    
TInt CSenLayeredXmlProperties::SetPropertyL(const TDesC8& aName,
                                            const TDesC8& aValue)
    {
	CSenElement* pElement = ipFragment->AsElement().Element(aName);
	if(!pElement)
		{
		// Add new element
		pElement =  &ipFragment->AsElement().AddElementL(aName);
		}
    pElement->SetContentL(aValue);
    return KErrNone;
    }

TInt CSenLayeredXmlProperties::PropertyL(const TDesC8& aName, TPtrC8& aValue)
    {
    if ( ipChildProperties )
        {
        TInt retVal = ipChildProperties->PropertyL(aName, aValue);
        if ( retVal == KErrNotFound )
            {
        	CSenElement* pElement = ipFragment->AsElement().Element(aName);
        	if ( pElement )
        		{
        		const TDesC8* pAttrValue = pElement->AttrValue(KSenOmittedAttributeName);
        		if ( pAttrValue )
                    {
                    if ( *pAttrValue == KSenOmittedTrueNoValue)
        		        {
                        return KErrSenOmitted;
                        }
                    else if ( *pAttrValue == KSenPropertyTrue)
                        {
                        return KErrSenOmitted;
                        }
        		    }
        		aValue.Set(pElement->Content());
        		return KErrNone;
        		}
            else
                {
                return KErrNotFound;
                }
            }
            
        return retVal;
        }
    else
        {
    	CSenElement* pElement = ipFragment->AsElement().Element(aName);
    	if ( pElement )
    		{
    		const TDesC8* pAttrValue = pElement->AttrValue(KSenOmittedAttributeName);
    		if ( pAttrValue )
                {
                if ( *pAttrValue == KSenOmittedTrueNoValue)
    		        {
                    return KErrSenOmitted;
                    }
                else if ( *pAttrValue == KSenPropertyTrue)
                    {
                    return KErrSenOmitted;
                    }
    		    }
        	aValue.Set(pElement->Content());
        	return KErrNone;
    		}
        else
            {
            return KErrNotFound;
            }
        }
    }
    
TInt CSenLayeredXmlProperties::SetIntPropertyL(const TDesC8& aName,
                                               const TInt aValue)
    {
    TBuf8<KFlatBufSize> buffer;
    buffer.Num(aValue);
    return SetPropertyL(aName, buffer);
    }

TInt CSenLayeredXmlProperties::IntPropertyL(const TDesC8& aName, TInt& aValue)
    {
    TPtrC8 value;
    TInt retVal(KErrNone);
    retVal = PropertyL(aName, value);
    if ( retVal == KErrNone )
        {
        TLex8 lex(value);
        retVal = lex.Val(aValue);
        }
    return retVal;
    }
    
TInt CSenLayeredXmlProperties::SetBoolPropertyL(const TDesC8& aName,
                                                const TBool aValue)
    {
    if ( aValue )
        {
        return SetPropertyL(aName, KSenPropertyTrue);
        }
    else
        {
        return SetPropertyL(aName, KSenPropertyFalse);
        }
    }

TInt CSenLayeredXmlProperties::BoolPropertyL(const TDesC8& aName,
                                             TBool& aValue)
    {
    TInt retVal(KErrNone);
    TPtrC8 value;
    retVal = PropertyL(aName, value);
    if ( retVal == KErrNone )
        {
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
        }
    return retVal;
    }
    
TInt CSenLayeredXmlProperties::SetOmittedL(const TDesC8& aName, TBool aValue)
    {
    TBool newElementCreated = EFalse;
 	CSenElement* pElement = ipFragment->AsElement().Element(aName);
	if ( !pElement )
		{
		// Add new element
		pElement =  &ipFragment->AsElement().AddElementL(aName);
		newElementCreated = ETrue;
		}
    else
        {
        const TDesC8* pVal = pElement->AttrValue(KSenOmittedAttributeName);
        if ( pVal )
            {
            delete SenXmlUtils::RemoveAttributeL(*pElement, KSenOmittedAttributeName);
            }
        }
      
    if ( pElement )  
        {
        if ( aValue )
            {
            if ( newElementCreated )
                {
                SenXmlUtils::AddAttributeL(*pElement, KSenOmittedAttributeName, KSenOmittedTrueNoValue);
                }
            else
                {
                SenXmlUtils::AddAttributeL(*pElement, KSenOmittedAttributeName, KSenPropertyTrue);
                }
            }
        else
            {
            SenXmlUtils::AddAttributeL(*pElement, KSenOmittedAttributeName, KSenPropertyFalse);
            }
        }
    else
        {
        return KErrNotFound;
        }
        
    return KErrNone;   
    }
    
TInt CSenLayeredXmlProperties::RemovePropertyL(const TDesC8& aName)
    {
 	CSenElement* pElement = ipFragment->AsElement().RemoveElement(aName);
	if ( pElement )
		{
		delete pElement;
		return KErrNone;
		}
    else
        {
        return KErrNotFound;
        }
    }

void CSenLayeredXmlProperties::WriteToL(RWriteStream& aWriteStream, TSenPropertiesLayer aLayer)
    {
    if ( aLayer > iLayer )
        {
        if ( ipChildProperties )
            {
            ipChildProperties->AsUtf8L(aLayer);
            }
        }
    else
        {
        if ( aLayer == iLayer )
            {
            ipFragment->WriteAsXMLToL(aWriteStream);
            }
        }
    }

void CSenLayeredXmlProperties::ReadFromL(const TDesC8& aBuffer, TSenPropertiesLayer aLayer)
    {

    // Sanity check: ignore zero-length strings
    if ( aBuffer.Length() == 0)
        {
        return;
        }

    if ( aLayer > iLayer )
        {
        if ( !ipChildProperties )
            {
            CreateChildPropertiesL();	//codescannerwarnings
            }
        ipChildProperties->ReadFromL(aBuffer, aLayer);
        }
    else
        {
        if ( aLayer == iLayer )
            {
            if ( !ipFragment->Reader() )
                {
                if ( !ipReader )
                    {
                    ipReader = CSenXmlReader::NewL(KXmlParserMimeType); // use libxml2 sax parser
                    }
                ipFragment->ParseWithL(*ipReader);
                }
            ipFragment->ParseL(aBuffer);   
            }
        }
    }

HBufC8* CSenLayeredXmlProperties::AsUtf8L(TSenPropertiesLayer aLayer)
    {
    if ( aLayer > iLayer )
        {
        if ( !ipChildProperties )
            {
            return NULL;
            }
        else
            {
            return ipChildProperties->AsUtf8L(aLayer);
            }
        }
    else
        {
        if ( aLayer == iLayer )
            {
            return ipFragment->AsXmlL();
            }
        else
            {
            return NULL;
            }
        }
    }

HBufC8* CSenLayeredXmlProperties::AsUtf8LC(TSenPropertiesLayer aLayer)
    {
    if ( aLayer > iLayer )
        {
        if ( !ipChildProperties )
            {
            return NULL;
            }
        else
            {
            return ipChildProperties->AsUtf8LC(aLayer);
            }
        }
    else
        {
        if ( aLayer == iLayer )
            {
            HBufC8* pAsXml = ipFragment->AsXmlL();
            CleanupStack::PushL(pAsXml);
            return pAsXml;
            }
        else
            {
            return NULL;
            }
        }
    }
    
TBool CSenLayeredXmlProperties::IsSafeToCast(TSenPropertiesClassType aClass)
    {
    if ( aClass == MSenProperties::ESenLayeredXmlProperties )
        {
        return ETrue;
        }
    else
        {
        return EFalse; // has only abstact, layered TP super classes 
        }
    }    
    
TInt CSenLayeredXmlProperties::ShrinkTo(TSenPropertiesLayer aLayer)
    {
    if ( ipChildProperties )
        {
        return ipChildProperties->ShrinkTo(aLayer);
        }
    else
        {
        if (iLayer > aLayer)
            {
            CSenLayeredXmlProperties* pParent = ipParentProperties;
            delete this;
            pParent->ipChildProperties = NULL;
            return pParent->ShrinkTo(aLayer);
            }
        else
            {
            if (iLayer < aLayer)
                {
                return KErrNotFound;
                }
            }
        }
    return KErrNone;
    }
    
void CSenLayeredXmlProperties::ExpandToL(TSenPropertiesLayer aLayer)
    {
    if ( ipChildProperties )
        {
        if (iLayer < aLayer)
            {
            ipChildProperties->ExpandToL(aLayer);
            }
        }
    else
        {
        if (iLayer < aLayer)
            {
            CreateChildPropertiesL();	//codescannerwarnings
            ipChildProperties->ExpandToL(aLayer);
            }
        }    
    }
    
MSenLayeredProperties::TSenPropertiesLayer CSenLayeredXmlProperties::TopLayer()
    {
    if ( ipChildProperties )
        {
        return ipChildProperties->TopLayer();
        }
    else
        {
        return iLayer;
        }    
    }
    
TInt CSenLayeredXmlProperties::SetPropertyL(const TDesC8& aName,
                                            const TDesC8& aValue,
                                            TSenPropertiesLayer aLayer)
    {
    if ( aLayer > iLayer )
        {
        if ( !ipChildProperties )
            {
            CreateChildPropertiesL();	//codescannerwarnings
            }
        return ipChildProperties->SetPropertyL(aName, aValue, aLayer);
        }
    else
        {
        if ( aLayer == iLayer )
            {
        	CSenElement* pChildElement = ipFragment->AsElement().Element(aName);
        	if(!pChildElement)
        		{
        		// Add new element
        		pChildElement =  &ipFragment->AsElement().AddElementL(aName);
        		}
        	pChildElement->SetContentL(aValue);
            return KErrNone;
            }
        else
            {
            return KErrNotFound;
            }
        }
    }

TInt CSenLayeredXmlProperties::PropertyL(const TDesC8& aName, TPtrC8& aValue,
                                         TSenPropertiesLayer aLayer)
    {
    if ( aLayer > iLayer )
        {
        if ( !ipChildProperties )
            {
            return KErrNotFound;
            }
        else
            {
            return ipChildProperties->PropertyL(aName, aValue, aLayer);
            }
        }
    else
        {
        if ( aLayer == iLayer )
            {
        	CSenElement* pElement = ipFragment->AsElement().Element(aName);
        	if( pElement )
        		{
        		const TDesC8* pAttrValue = pElement->AttrValue(KSenOmittedAttributeName);
        		if ( pAttrValue )
                    {
                    if ( *pAttrValue == KSenOmittedTrueNoValue)
        		        {
                        return KErrSenOmitted;
                        }
                    else if ( *pAttrValue == KSenPropertyTrue)
                        {
                        return KErrSenOmitted;
                        }
        		    }
        		aValue.Set(pElement->Content());
                return KErrNone;
        		}
            else
                {
                return KErrNotFound;
                }
            }
        else
            {
            return KErrNotFound;
            }
        }
    }
    
TInt CSenLayeredXmlProperties::PropertyL(const TDesC8& aName,
                                         MSenProperty*& aValue,
                                         TSenPropertiesLayer aLayer)
    {
    if ( aLayer > iLayer )
        {
        if ( !ipChildProperties )
            {
            return KErrNotFound;
            }
        else
            {
            return ipChildProperties->PropertyL(aName, aValue, aLayer);
            }
        }
    else
        {
        if ( aLayer == iLayer )
            {
        	CSenElement* pElement = ipFragment->AsElement().Element(aName);
        	if( pElement )
        		{
                CSenPropertiesElement* pPropertiesElement
                    = (CSenPropertiesElement*)pElement;
        		aValue = pPropertiesElement;
                return KErrNone;
        		}
            else
                {
                return KErrNotFound;
                }
            }
        else
            {
            return KErrNotFound;
            }
        }
    
    }
        
TInt CSenLayeredXmlProperties::PropertyL(const TDesC8& aName,
                                         MSenProperty*& aValue)
    {
    if ( ipChildProperties )
        {
        TInt retVal = ipChildProperties->PropertyL(aName, aValue);
        if ( retVal == KErrNotFound )
            {
        	CSenElement* pElement = ipFragment->AsElement().Element(aName);
        	if ( pElement )
        		{
        		const TDesC8* pAttrValue = pElement->AttrValue(KSenOmittedAttributeName);
        		if ( pAttrValue )
                    {
                    if ( *pAttrValue == KSenOmittedTrueNoValue)
        		        {
                        return KErrSenOmitted;
                        }
                    else if ( *pAttrValue == KSenPropertyTrue)
                        {
                        return KErrSenOmitted;
                        }
        		    }        		
                CSenPropertiesElement* pPropertyElement 
                    = (CSenPropertiesElement*) pElement;
        		aValue = pPropertyElement;
        		return KErrNone;
        		}
            else
                {
                return KErrNotFound;
                }
            }
            
        return retVal;
        }
    else
        {
    	CSenElement* pElement = ipFragment->AsElement().Element(aName);
        if ( pElement )
            {
    		const TDesC8* pAttrValue = pElement->AttrValue(KSenOmittedAttributeName);
    		if ( pAttrValue )
                {
                if ( *pAttrValue == KSenOmittedTrueNoValue)
    		        {
                    return KErrSenOmitted;
                    }
                else if ( *pAttrValue == KSenPropertyTrue)
                    {
                    return KErrSenOmitted;
                    }
    		    }            
            CSenPropertiesElement* pPropertyElement 
                = (CSenPropertiesElement*) pElement;
        	aValue = pPropertyElement;
            return KErrNone;
    		}
        else
            {
            return KErrNotFound;
            }
        }
    }
    
TInt CSenLayeredXmlProperties::SetPropertyL(const TDesC8& aName,
                                            const TDesC8& aValue,
                                            const TDesC8& aType)
    {
	CSenElement* pElement = ipFragment->AsElement().Element(aName);
	if ( !pElement )
		{
		// Add new element
		pElement =  &ipFragment->AsElement().AddElementL(aName);
		}
    pElement->SetContentL(aValue);
	SenXmlUtils::AddAttributeL(*pElement, KSenTypeAttributeName, aType);
    return KErrNone;
    }

TInt CSenLayeredXmlProperties::PropertyL(const TDesC8& aName,
                                         TPtrC8& aValue,
                                         TPtrC8& aType)
    {
    if ( ipChildProperties )
        {
        TInt retVal = ipChildProperties->PropertyL(aName, aValue);
        if ( retVal == KErrNotFound )
            {
        	CSenElement* pElement = ipFragment->AsElement().Element(aName);
        	if ( pElement )
        		{
        		/*
                CSenPropertiesElement* pPropertyElement 
                    = (CSenPropertiesElement*) pElement;
        		aValue.Set(pPropertyElement->Value());
        		aType.Set(pPropertyElement->Type());
        		*/
        		const TDesC8* pAttrValue = pElement->AttrValue(KSenOmittedAttributeName);
        		if ( pAttrValue )
                    {
                    if ( *pAttrValue == KSenOmittedTrueNoValue)
        		        {
                        return KErrSenOmitted;
                        }
                    else if ( *pAttrValue == KSenPropertyTrue)
                        {
                        return KErrSenOmitted;
                        }
        		    }
        		aValue.Set(pElement->Content());
        		aType.Set(*pElement->AttrValue(KSenTypeAttributeName));
        		return KErrNone;
        		}
            else
                {
                return KErrNotFound;
                }
            }
            
        return retVal;
        }
    else
        {
    	CSenElement* pElement = ipFragment->AsElement().Element(aName);
        if ( pElement )
            {
            /*
            CSenPropertiesElement* pPropertyElement 
                = (CSenPropertiesElement*) pElement;
    		aValue.Set(pPropertyElement->Value());
    		aType.Set(pPropertyElement->Type());
    		*/
    		const TDesC8* pAttrValue = pElement->AttrValue(KSenOmittedAttributeName);
    		if ( pAttrValue )
                {
                if ( *pAttrValue == KSenOmittedTrueNoValue)
    		        {
                    return KErrSenOmitted;
                    }
                else if ( *pAttrValue == KSenPropertyTrue)
                    {
                    return KErrSenOmitted;
                    }
    		    }
    		aValue.Set(pElement->Content());
    		aType.Set(*pElement->AttrValue(KSenTypeAttributeName));
            return KErrNone;
    		}
        else
            {
            return KErrNotFound;
            }
        }
    }    
     
    

TInt CSenLayeredXmlProperties::PropertiesByTypeL(const TDesC8& aType,
                                                 RPointerArray<MSenProperty>& aArray)
    {
    TInt retVal(KErrNotFound);

    retVal = AllPropertiesByTypeL(aType, aArray);

    // Remove omitted values from the array.
    TInt propsCount = aArray.Count()-1;
    for (TInt i=propsCount; i>=0; i--)
        {
        CSenPropertiesElement* pElement = (CSenPropertiesElement*)aArray[i];
		const TDesC8* pAttrValue = pElement->AttrValue(KSenOmittedAttributeName);
		if ( pAttrValue )
            {
            if ( *pAttrValue == KSenOmittedTrueNoValue)
		        {
		        aArray.Remove(i);
                }
            else if ( *pAttrValue == KSenPropertyTrue)
                {
		        aArray.Remove(i);
                }
            }
        }
    
    if ( aArray.Count() > 0 )
        {
        retVal=KErrNone;
        }
    
    return retVal;
    }

TInt CSenLayeredXmlProperties::AllPropertiesByTypeL(const TDesC8& aType,
                                                    RPointerArray<MSenProperty>& aArray)
    {
    TInt retVal(KErrNotFound);
    if ( ipChildProperties )
        {
        retVal = ipChildProperties->AllPropertiesByTypeL(aType, aArray);
        }

    CSenElement& element = ipFragment->AsElement();
    RPointerArray<CSenElement>& elements = element.ElementsL();
    TInt elementCount(elements.Count());
    TInt propsCount(0);
    TBool found(EFalse);
    for (TInt i=0; i<elementCount; i++)
        {
        CSenElement* pElement = elements[i];
        found = EFalse;

        // Go through properties which are already added into returned array.
        // => If property is already in the array
        //    it won't be added into array for the second time.
        propsCount = aArray.Count();
        for (TInt j=0; j<propsCount; j++)
            {
             MSenProperty* pProperty = aArray[j];
             if ( pElement->LocalName() == pProperty->Name() )
                {
                found = ETrue;
                break;
                }
            }
        
        if ( !found )
            {
            const TDesC8* propertyType =
                        pElement->AttrValue(KSenTypeAttributeName);

            if ( propertyType )
                {
                if ( *propertyType == aType )
                    {
#ifdef EKA2
                    CSenPropertiesElement* pPropElement
                        = (CSenPropertiesElement*)pElement;
                    aArray.AppendL(pPropElement);
#else
                    CSenPropertiesElement* pPropElement
                        = (CSenPropertiesElement*)pElement;
			        User::LeaveIfError(aArray.Append(pPropElement));
#endif
                    }
                }
            }
        }

    if ( aArray.Count() > 0 )
        {
        retVal=KErrNone;
        }
        
    return retVal;
    }
    
TInt CSenLayeredXmlProperties::SetIntPropertyL(const TDesC8& aName, const TInt aValue,
                                               TSenPropertiesLayer aLayer)
    {
    TBuf8<KFlatBufSize> buffer;
    buffer.Num(aValue);
    return SetPropertyL(aName, buffer, aLayer);
    }

TInt CSenLayeredXmlProperties::IntPropertyL(const TDesC8& aName, TInt& aValue,
                                            TSenPropertiesLayer aLayer)
    {
    TPtrC8 value;
    TInt retVal(KErrNone);
    retVal = PropertyL(aName, value, aLayer);
    if ( retVal == KErrNone )
        {
        TLex8 lex(value);
        retVal = lex.Val(aValue);
        }
    return retVal;
    }
    
TInt CSenLayeredXmlProperties::SetBoolPropertyL(const TDesC8& aName, const TBool aValue,
                                                TSenPropertiesLayer aLayer)
    {
    if ( aValue )
        {
        return SetPropertyL(aName, KSenPropertyTrue, aLayer);
        }
    else
        {
        return SetPropertyL(aName, KSenPropertyFalse, aLayer);
        }
    }

TInt CSenLayeredXmlProperties::BoolPropertyL(const TDesC8& aName, TBool& aValue,
                                             TSenPropertiesLayer aLayer)
    {
    TPtrC8 value;
    TInt retVal(KErrNone);
    retVal = PropertyL(aName, value, aLayer);
    if ( retVal == KErrNone )
        {
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
        }
    return retVal;
    }
    
TInt CSenLayeredXmlProperties::SetOmittedL( const TDesC8& aName, 
                                            TBool aValue,
                                            TSenPropertiesLayer aLayer )
    {
    if ( aLayer > iLayer )
        {
        if ( !ipChildProperties )
            {
            CreateChildPropertiesL();	//codescannerwarnings
            }
        return ipChildProperties->SetOmittedL(aName, aValue, aLayer);
        }
    else
        {
        if ( aLayer == iLayer )
            {
            TBool newElementCreated = EFalse;
         	CSenElement* pElement = ipFragment->AsElement().Element(aName);
        	if ( !pElement )
        		{
        		// Add new element
        		pElement =  &ipFragment->AsElement().AddElementL(aName);
        		newElementCreated = ETrue;
        		}
            else
                {
                const TDesC8* pVal = pElement->AttrValue(KSenOmittedAttributeName);
                if ( pVal )
                    {
                    delete SenXmlUtils::RemoveAttributeL(*pElement, KSenOmittedAttributeName);
                    }
                }
              
            if ( pElement )  
                {
                if ( aValue )
                    {
                    if ( newElementCreated )
                        {
                        SenXmlUtils::AddAttributeL(*pElement, KSenOmittedAttributeName, KSenOmittedTrueNoValue);
                        }
                    else
                        {
                        SenXmlUtils::AddAttributeL(*pElement, KSenOmittedAttributeName, KSenPropertyTrue);
                        }
                    }
                else
                    {
                    SenXmlUtils::AddAttributeL(*pElement, KSenOmittedAttributeName, KSenPropertyFalse);
                    }
                }
            else
                {
                return KErrNotFound;
                }
            }
        else
            {
            return KErrNotFound;
            }
        }
        
    return KErrNone;
    }

TInt CSenLayeredXmlProperties::RemovePropertyL(const TDesC8& aName,
                                               TSenPropertiesLayer aLayer)
    {
    if ( aLayer > iLayer )
        {
        if ( !ipChildProperties )
            {
            return KErrNotFound;
            }
        return ipChildProperties->RemovePropertyL(aName, aLayer);
        }
    else
        {
        if ( aLayer == iLayer )
            {
         	CSenElement* pElement = ipFragment->AsElement().RemoveElement(aName);
        	if ( pElement )
        		{
        		delete pElement;
        		return KErrNone;
        		}
            else
                {
                return KErrNotFound;
                }
            }
        else
            {
            return KErrNotFound;
            }
        }
    }

void CSenLayeredXmlProperties::CreateChildPropertiesL()	//codescannerwarnings
    {
    if ( !ipFragment->Reader() )
        {
        if( !ipReader )
            {
            ipReader = CSenXmlReader::NewL(KXmlParserMimeType); // use libxml2 sax parser
            }
        ipFragment->ParseWithL(*ipReader);
        }
    
    if ( ipFragment->HasStringPool() )
        {
        ipChildProperties =
                CSenLayeredXmlProperties::NewL(ipFragment->StringPool());
        }
    else
        {
        ipChildProperties = CSenLayeredXmlProperties::NewL();
        }
    ipChildProperties->ipParentProperties = this;
    ipChildProperties->ipFragment->ParseWithL(*ipFragment->Reader());
    
    switch ( iLayer )
        {
        case ESenTransportLayer:
            ipChildProperties->iLayer = ESenSereneLayer;
            break;
        case ESenSereneLayer:
            ipChildProperties->iLayer = ESenFrameworkLayer;
            break;
        case ESenFrameworkLayer:
            ipChildProperties->iLayer = ESenProviderSessionLayer;
            break;
        case ESenProviderSessionLayer:
            ipChildProperties->iLayer = ESenConsumerSessionLayer;
            break;
        case ESenConsumerSessionLayer:
            ipChildProperties->iLayer = ESenMessageLayer;
            break;
        case ESenMessageLayer:
            // Not possible
            break;
        default:
            // Not possible
            break;
        }
    }
    
void CSenLayeredXmlProperties::ElementsL(RPointerArray<CSenElement>& aProps) const
    {
    if ( ipChildProperties )
        {
        ipChildProperties->ElementsL(aProps);
        }
            
    RPointerArray<CSenElement>& elements = ipFragment->AsElement().ElementsL();
    TInt elementCount(elements.Count());
    TInt propsCount(0);
    TBool found;
    for (TInt i=0; i<elementCount; i++)
        {
        found = EFalse;
        propsCount = aProps.Count();
        for (TInt j=0; j<propsCount; j++)
            {
             if ( elements[i]->LocalName() == aProps[j]->LocalName() )
                {
                found = ETrue;
                break;
                }
            }
        if ( !found )
            {
#ifdef EKA2
            aProps.AppendL(elements[i]);
#else
            User::LeaveIfError(aProps.Append(elements[i]));
#endif
            }
        }
    }

MSenProperties* CSenLayeredXmlProperties::CloneL() const
    {
    CSenElement& element = ipFragment->AsElement();
    
    CSenLayeredXmlProperties* pProperties = NewLC(element);
    if ( ipChildProperties )
        {
        CSenLayeredXmlProperties* pChildProperties =
                                (CSenLayeredXmlProperties*)ipChildProperties->CloneL();
        pProperties->ipChildProperties = pChildProperties;
        }
    
    CleanupStack::Pop(pProperties);
    return pProperties;
    }

MSenProperties* CSenLayeredXmlProperties::Clone(TInt& aOkOrError) const
    {
    MSenProperties* pClone = NULL;
    TRAP( aOkOrError, pClone = CloneL(); )
    return pClone;
    }

void CSenLayeredXmlProperties::BaseConstructL(
                                        const TDesC8& aLocalname, 
                                        const TDesC8& aXml,
                                        CSenXmlReader* aParser,
                                        RStringPool* aStringPool)
    {
    ipFragment = CSenPropertiesFragment::NewL(aLocalname, aStringPool);
    if ( aParser && aXml!=KNullDesC8 )
        {
        // Do parsing
        ipFragment->ParseWithL(*aParser);
        ipFragment->ParseL(aXml);
        }
   }

void CSenLayeredXmlProperties::BaseConstructL(
                                        const TDesC8& aNamespace, 
                                        const TDesC8& aLocalname, 
                                        const TDesC8& aXml,
                                        CSenXmlReader* aParser,
                                        RStringPool* aStringPool)
    {
    ipFragment = CSenPropertiesFragment::NewL(aNamespace,
                                              aLocalname,
                                              aStringPool);
    if ( aParser && aXml != KNullDesC8 )
        {
        // Do parsing
        ipFragment->ParseWithL(*aParser);
        ipFragment->ParseL(aXml);
        }
    }

void CSenLayeredXmlProperties::BaseConstructL(const TDesC8& aNamespace, 
                                       const TDesC8& aLocalname, 
                                       const TDesC8& aQualifiedName, 
                                       const TDesC8& aXml,
                                       CSenXmlReader* aParser,
                                       RStringPool* aStringPool)
    {
    ipFragment = CSenPropertiesFragment::NewL(aNamespace, 
                                              aLocalname, 
                                              aQualifiedName,
                                              aStringPool);
    if ( aParser && aXml != KNullDesC8 )
        {
        // Do parsing
        ipFragment->ParseWithL(*aParser);
        ipFragment->ParseL(aXml);
        }
    }

void CSenLayeredXmlProperties::BaseConstructL(const CSenElement& aElement,
                                              RStringPool* aStringPool)
    {
    ipFragment = CSenPropertiesFragment::NewL(aElement.LocalName(),
                                              aStringPool);
    
    CSenElement& element = ipFragment->AsElement();
    element.CopyFromL((CSenElement&)aElement);
    }

CSenLayeredXmlProperties::CSenLayeredXmlProperties()
    : ipFragment(NULL),
      ipChildProperties(NULL),
      iLayer(ESenTransportLayer),
      ipReader(NULL),
      ipParentProperties(NULL)
    {
    }

CSenLayeredXmlProperties::~CSenLayeredXmlProperties()
    {
    delete ipReader;
    delete ipFragment;
    delete ipChildProperties;
    }


// END OF FILE
