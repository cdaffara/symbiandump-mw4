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








#include <s32strm.h> // RWriteStream

#include <SenXmlReader.h>
#include <SenXmlUtils.h>
#include <SenBaseAttribute.h>
#include "SenXmlProperties.h"
#include "senpropertiesfragment.h"
#include "senxmldebug.h"

namespace
    {
    const TInt KFlatBufSize     = 64;  // CBufFlat
    }

EXPORT_C CSenXmlProperties* CSenXmlProperties::NewL()
    {
    CSenXmlProperties* pNew = NewLC();
    CleanupStack::Pop(); // pNew
    return pNew;
    }

EXPORT_C CSenXmlProperties* CSenXmlProperties::NewLC()
    {
    CSenXmlProperties* pNew = new (ELeave) CSenXmlProperties;
    CleanupStack::PushL(pNew);

    // omit parsing by passing zero-length descriptor
    pNew->BaseConstructL(KSenXmlPropertiesLocalname,
                         KNullDesC8,
                         NULL);
    return pNew;
    }

EXPORT_C CSenXmlProperties* CSenXmlProperties::NewL(const TDesC8& aXmlUtf8,
                                           CSenXmlReader& aParser)
    {
    CSenXmlProperties* pNew = NewLC(aXmlUtf8, aParser);
    CleanupStack::Pop(); // pNew
    return pNew;
    }

EXPORT_C CSenXmlProperties* CSenXmlProperties::NewLC(const TDesC8& aXmlUtf8,
                                            CSenXmlReader& aParser)
    {
    CSenXmlProperties* pNew = new (ELeave) CSenXmlProperties;
    CleanupStack::PushL(pNew);

    // parse the XML document  descriptor into DOM fragment
    pNew->BaseConstructL(KSenXmlPropertiesLocalname,
                         aXmlUtf8,
                         &aParser);
    return pNew;
    }


EXPORT_C CSenXmlProperties* CSenXmlProperties::NewL(const CSenElement& aElement)
    {
    CSenXmlProperties* pNew = NewLC(aElement);
    CleanupStack::Pop(); // pNew
    return pNew;
    }

EXPORT_C CSenXmlProperties* CSenXmlProperties::NewLC(const CSenElement& aElement)
    {
    CSenXmlProperties* pNew = new (ELeave) CSenXmlProperties;
    CleanupStack::PushL(pNew);

    // parse the XML document  descriptor into DOM fragment
    pNew->BaseConstructL(aElement);
    return pNew;
    }
    
void CSenXmlProperties::SetReader(CSenXmlReader& aReader)
    {
    // de-allocate the owned reader, if one exists:
    if( ipReader )
        {
        delete ipReader; 
        ipReader = NULL;
        }        
    // Note: ipFragment does not own the reader 
    // behind the given reference (aReader) 
    ipFragment->SetReader(aReader); 
    }    

MSenProperties::TSenPropertiesClassType CSenXmlProperties::PropertiesClassType() 
    { 
    return ESenXmlProperties; 
    }

void CSenXmlProperties::WriteToL(RWriteStream& aWriteStream)
    {
    ipFragment->WriteAsXMLToL(aWriteStream);
    }

void CSenXmlProperties::ReadFromL(const TDesC8& aBuffer)
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

HBufC8* CSenXmlProperties::AsUtf8L()
    {
    return ipFragment->AsXmlL();
    }

HBufC8* CSenXmlProperties::AsUtf8LC()
    {
    HBufC8* pAsXml = ipFragment->AsXmlL();
    CleanupStack::PushL(pAsXml);
    return pAsXml;
    }
    
TBool CSenXmlProperties::IsSafeToCast(TSenPropertiesClassType aClass)
    {
    if ( aClass == MSenProperties::ESenXmlProperties )
        {
        return ETrue;
        }
    else
        {
        return EFalse; // has only abstact, layered TP super classes 
        }
    }      

TInt CSenXmlProperties::SetPropertyL(const TDesC8& aName,
                                     const TDesC8& aValue)
    {
	CSenElement* pChildElement = ipFragment->AsElement().Element(aName);
	if ( !pChildElement )
		{
		// Add new element
		pChildElement =  &ipFragment->AsElement().AddElementL(aName);
		}
	pChildElement->SetContentL(aValue);
    return KErrNone;
    }

TInt CSenXmlProperties::PropertyL(const TDesC8& aName, TPtrC8& aValue)
    {
	CSenElement* pElement = ipFragment->AsElement().Element(aName);
	if ( pElement )
		{
		aValue.Set(pElement->Content());
        return KErrNone;
		}
    else
        {
        return KErrNotFound;
        }
    }
    
TInt CSenXmlProperties::SetPropertyL(const TDesC8& aName,
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

TInt CSenXmlProperties::PropertyL(const TDesC8& aName,
                                  TPtrC8& aValue,
                                  TPtrC8& aType)
    {
	CSenElement* pElement = ipFragment->AsElement().Element(aName);
	if ( pElement )
		{
		aValue.Set(pElement->Content());
		aType.Set(*pElement->AttrValue(KSenTypeAttributeName));
        return KErrNone;
		}
    else
        {
        return KErrNotFound;
        }
    }    
    
TInt CSenXmlProperties::SetIntPropertyL(const TDesC8& aName, const TInt aValue)
    {
    TBuf8<KFlatBufSize> buffer;
    buffer.Num(aValue);
    return SetPropertyL(aName, buffer);
    }

TInt CSenXmlProperties::IntPropertyL(const TDesC8& aName, TInt& aValue)
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
    
TInt CSenXmlProperties::SetBoolPropertyL(const TDesC8& aName, const TBool aValue)
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

TInt CSenXmlProperties::BoolPropertyL(const TDesC8& aName, TBool& aValue)
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

TInt CSenXmlProperties::SetOmittedL(const TDesC8& aName, TBool aValue)
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
            CSenBaseAttribute* pAttribute = SenXmlUtils::RemoveAttributeL(*pElement, KSenOmittedAttributeName);
            delete pAttribute;
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
    
TInt CSenXmlProperties::RemovePropertyL(const TDesC8& aName)
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

CSenXmlProperties::~CSenXmlProperties()
    {
    if(ipReader)
    	{
		delete ipReader;    	
    	}
    if(ipFragment)
    	{
    	delete ipFragment;
    	}
    }

void CSenXmlProperties::BaseConstructL(const TDesC8& aLocalname, 
                                       const TDesC8& aXml,
                                       CSenXmlReader* aParser)
    {
    ipFragment = CSenPropertiesFragment::NewL(aLocalname);
    if ( aParser && &aXml && (aXml != KNullDesC8()) )
        {
        // Do parsing
        ipFragment->ParseWithL(*aParser);
        ipFragment->ParseL(aXml);
        }
   }

void CSenXmlProperties::BaseConstructL(const TDesC8& aNamespace, 
                                       const TDesC8& aLocalname, 
                                       const TDesC8& aXml,
                                       CSenXmlReader* aParser)
    {
    ipFragment = CSenPropertiesFragment::NewL(aNamespace, aLocalname);
    if ( aParser && aXml != KNullDesC8 )
        {
        // Do parsing
        ipFragment->ParseWithL(*aParser);
        ipFragment->ParseL(aXml);
        }
    }

void CSenXmlProperties::BaseConstructL(const TDesC8& aNamespace, 
                                       const TDesC8& aLocalname, 
                                       const TDesC8& aQualifiedName, 
                                       const TDesC8& aXml,
                                       CSenXmlReader* aParser)
    {
    ipFragment = CSenPropertiesFragment::NewL(aNamespace, 
                                        aLocalname, 
                                        aQualifiedName);
    if ( aParser && aXml != KNullDesC8 )
        {
        // Do parsing
        ipFragment->ParseWithL(*aParser);
        ipFragment->ParseL(aXml);
        }
    }

void CSenXmlProperties::BaseConstructL(const CSenElement& aElement)
    {
    ipFragment = CSenPropertiesFragment::NewL(aElement.LocalName());
    CSenElement& element = ipFragment->AsElement();
    element.CopyFromL((CSenElement&)aElement);
    }

CSenXmlProperties::CSenXmlProperties()
:   ipFragment(NULL),
    ipReader(NULL)
    {
    }
    
MSenProperties* CSenXmlProperties::CloneL() const
    {
    CSenElement& element = ipFragment->AsElement();
    
    CSenXmlProperties* pProperties = this->NewLC(element);
    CleanupStack::Pop(pProperties);
    return pProperties;
    }

MSenProperties* CSenXmlProperties::Clone(TInt& aOkOrError) const
    {
    MSenProperties* pClone = NULL;
    TRAP( aOkOrError, pClone = CloneL(); )
    return pClone;
    }
    

// END OF FILE


