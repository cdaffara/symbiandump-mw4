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
#include "senlayeredhttptransportproperties.h"
#include <SenHttpTransportProperties.h>              // property keys (constants)
#include <SenTransportProperties.h>                  // property keys (constants)
#include "senpropertieselement.h"   // private: Utils\inc
#include "senpropertiesfragment.h"  // private: Utils\inc

EXPORT_C CSenLayeredHttpTransportProperties* CSenLayeredHttpTransportProperties::NewL()
    {
    CSenLayeredHttpTransportProperties* pNew = NewLC();
    CleanupStack::Pop(); // pNew
    return pNew;
    }

EXPORT_C CSenLayeredHttpTransportProperties* CSenLayeredHttpTransportProperties::NewLC()
    {
    CSenLayeredHttpTransportProperties* pNew = new (ELeave) CSenLayeredHttpTransportProperties;
    CleanupStack::PushL(pNew);

    // omit parsing by passing zero-length descriptor
    pNew->BaseConstructL(KSenLayeredXmlPropertiesLocalname, KNullDesC8, NULL);
    return pNew;
    }

EXPORT_C CSenLayeredHttpTransportProperties* CSenLayeredHttpTransportProperties::NewL(const TDesC8& aXmlUtf8, CSenXmlReader& aParser)
    {
    CSenLayeredHttpTransportProperties* pNew = NewLC(aXmlUtf8, aParser);
    CleanupStack::Pop(); // pNew
    return pNew;
    }

EXPORT_C CSenLayeredHttpTransportProperties* CSenLayeredHttpTransportProperties::NewLC(const TDesC8& aXmlUtf8, CSenXmlReader& aParser)
    {
    CSenLayeredHttpTransportProperties* pNew = new (ELeave) CSenLayeredHttpTransportProperties;
    CleanupStack::PushL(pNew);

    // parse the XML document  descriptor into DOM fragment
    pNew->BaseConstructL(KSenLayeredXmlPropertiesLocalname, aXmlUtf8, &aParser);
    return pNew;
    }

EXPORT_C CSenLayeredHttpTransportProperties* CSenLayeredHttpTransportProperties::NewL(const CSenElement& aElement)
    {
    CSenLayeredHttpTransportProperties* pNew = NewLC(aElement);
    CleanupStack::Pop(); // pNew
    return pNew;
    }

EXPORT_C CSenLayeredHttpTransportProperties* CSenLayeredHttpTransportProperties::NewLC(const CSenElement& aElement)
    {
    CSenLayeredHttpTransportProperties* pNew = new (ELeave) CSenLayeredHttpTransportProperties;
    CleanupStack::PushL(pNew);

    // parse the XML document  descriptor into DOM fragment
    pNew->BaseConstructL(aElement);
    return pNew;
    }

// RStringPool variants:
EXPORT_C CSenLayeredHttpTransportProperties* CSenLayeredHttpTransportProperties::NewL(RStringPool& aStringPool)
    {
    CSenLayeredHttpTransportProperties* pNew = NewLC(aStringPool);
    CleanupStack::Pop(); // pNew
    return pNew;
    }

EXPORT_C CSenLayeredHttpTransportProperties* CSenLayeredHttpTransportProperties::NewLC(RStringPool& aStringPool)
    {
    CSenLayeredHttpTransportProperties* pNew = new (ELeave) CSenLayeredHttpTransportProperties;
    CleanupStack::PushL(pNew);

    // omit parsing by passing zero-length descriptor
    pNew->BaseConstructL(KSenLayeredXmlPropertiesLocalname, KNullDesC8, NULL, &aStringPool);
    return pNew;
    }

EXPORT_C CSenLayeredHttpTransportProperties* CSenLayeredHttpTransportProperties::NewL(const TDesC8& aXmlUtf8, CSenXmlReader& aParser, RStringPool& aStringPool)
    {
    CSenLayeredHttpTransportProperties* pNew = NewLC(aXmlUtf8, aParser, aStringPool);
    CleanupStack::Pop(); // pNew
    return pNew;
    }

EXPORT_C CSenLayeredHttpTransportProperties* CSenLayeredHttpTransportProperties::NewLC(const TDesC8& aXmlUtf8, CSenXmlReader& aParser, RStringPool& aStringPool)
    {
    CSenLayeredHttpTransportProperties* pNew = new (ELeave) CSenLayeredHttpTransportProperties;
    CleanupStack::PushL(pNew);

    // parse the XML document  descriptor into DOM fragment
    pNew->BaseConstructL(KSenLayeredXmlPropertiesLocalname, aXmlUtf8, &aParser, &aStringPool);
    return pNew;
    }

EXPORT_C CSenLayeredHttpTransportProperties* CSenLayeredHttpTransportProperties::NewL(const CSenElement& aElement, RStringPool& aStringPool)
    {
    CSenLayeredHttpTransportProperties* pNew = NewLC(aElement, aStringPool);
    CleanupStack::Pop(); // pNew
    return pNew;
    }

EXPORT_C CSenLayeredHttpTransportProperties* CSenLayeredHttpTransportProperties::NewLC(const CSenElement& aElement, RStringPool& aStringPool)
    {
    CSenLayeredHttpTransportProperties* pNew = new (ELeave) CSenLayeredHttpTransportProperties;
    CleanupStack::PushL(pNew);

    // parse the XML document  descriptor into DOM fragment
    pNew->BaseConstructL(aElement, &aStringPool);
    return pNew;
    }

    
void CSenLayeredHttpTransportProperties::SetReader(CSenXmlReader& aReader)
    {
    CSenLayeredTransportProperties::SetReader(aReader);
    }    
    

MSenProperties::TSenPropertiesClassType CSenLayeredHttpTransportProperties::PropertiesClassType() 
    {
    return ESenLayeredHttpTransportProperties;
    }

void CSenLayeredHttpTransportProperties::WriteToL(RWriteStream& aWriteStream)
    {
    CSenLayeredTransportProperties::WriteToL(aWriteStream);
    }

void CSenLayeredHttpTransportProperties::ReadFromL(const TDesC8& aBuffer)
    {
    CSenLayeredTransportProperties::ReadFromL(aBuffer);
    }
    
HBufC8* CSenLayeredHttpTransportProperties::AsUtf8L()
    {
    return CSenLayeredTransportProperties::AsUtf8L();
    }

HBufC8* CSenLayeredHttpTransportProperties::AsUtf8LC()
    {
    return CSenLayeredTransportProperties::AsUtf8LC();
    }
    
TBool CSenLayeredHttpTransportProperties::IsSafeToCast(TSenPropertiesClassType aClass)
    {
    if ( aClass == MSenProperties::ESenLayeredHttpTransportProperties )
        {
        return ETrue;
        }
    else
        {
        return CSenLayeredTransportProperties::IsSafeToCast( aClass );
        }
    }

TInt CSenLayeredHttpTransportProperties::SetPropertyL(const TDesC8& aName, const TDesC8& aValue)
    {
    return CSenLayeredTransportProperties::SetPropertyL(aName, aValue);
    }
    
TInt CSenLayeredHttpTransportProperties::PropertyL(const TDesC8& aName, TPtrC8& aValue)
    {
    return CSenLayeredTransportProperties::PropertyL(aName, aValue);
    }
    
TInt CSenLayeredHttpTransportProperties::PropertyL(const TDesC8& aName, MSenProperty*& aValue, TSenPropertiesLayer aLayer)
    {
    return CSenLayeredTransportProperties::PropertyL(aName, aValue, aLayer);
    }
        
TInt CSenLayeredHttpTransportProperties::PropertyL(const TDesC8& aName, MSenProperty*& aValue)
    {
    return CSenLayeredTransportProperties::PropertyL(aName, aValue);
    }

TInt CSenLayeredHttpTransportProperties::PropertiesByTypeL(const TDesC8& aType, RPointerArray<MSenProperty>& aArray)
    {
    return CSenLayeredTransportProperties::PropertiesByTypeL(aType, aArray);
    }

TInt CSenLayeredHttpTransportProperties::SetIntPropertyL(const TDesC8& aName, const TInt aValue)
    {
    return CSenLayeredTransportProperties::SetIntPropertyL(aName, aValue);
    }
    
TInt CSenLayeredHttpTransportProperties::IntPropertyL(const TDesC8& aName, TInt& aValue)
    {
    return CSenLayeredTransportProperties::IntPropertyL(aName, aValue);
    }
    
TInt CSenLayeredHttpTransportProperties::SetBoolPropertyL(const TDesC8& aName, const TBool aValue)
    {
    return CSenLayeredTransportProperties::SetBoolPropertyL(aName, aValue);
    }
    
TInt CSenLayeredHttpTransportProperties::BoolPropertyL(const TDesC8& aName, TBool& aValue)
    {
    return CSenLayeredTransportProperties::BoolPropertyL(aName, aValue);
    }   

void CSenLayeredHttpTransportProperties::WriteToL(RWriteStream& aWriteStream, TSenPropertiesLayer aLayer)
    {
    CSenLayeredTransportProperties::WriteToL(aWriteStream, aLayer);
    }

void CSenLayeredHttpTransportProperties::ReadFromL(const TDesC8& aBuffer, TSenPropertiesLayer aLayer)
    {
    CSenLayeredTransportProperties::ReadFromL(aBuffer, aLayer);
    }
    
HBufC8* CSenLayeredHttpTransportProperties::AsUtf8L(TSenPropertiesLayer aLayer)
    {
    return CSenLayeredTransportProperties::AsUtf8L(aLayer);
    }

HBufC8* CSenLayeredHttpTransportProperties::AsUtf8LC(TSenPropertiesLayer aLayer)
    {
    return CSenLayeredTransportProperties::AsUtf8LC(aLayer);
    }

TInt CSenLayeredHttpTransportProperties::ShrinkTo(TSenPropertiesLayer aLayer)
    {
    return CSenLayeredTransportProperties::ShrinkTo(aLayer);
    }
    
void CSenLayeredHttpTransportProperties::ExpandToL(TSenPropertiesLayer aLayer)
    {
    CSenLayeredTransportProperties::ExpandToL(aLayer);
    }
    
MSenLayeredProperties::TSenPropertiesLayer CSenLayeredHttpTransportProperties::TopLayer()
    {
    return CSenLayeredTransportProperties::TopLayer();
    }
    
TInt CSenLayeredHttpTransportProperties::SetPropertyL(const TDesC8& aName, const TDesC8& aValue, TSenPropertiesLayer aLayer)
    {
    return CSenLayeredTransportProperties::SetPropertyL(aName, aValue, aLayer);
    }
    
TInt CSenLayeredHttpTransportProperties::PropertyL(const TDesC8& aName,
                                                     TPtrC8& aValue,
                                                     TSenPropertiesLayer aLayer)
    {
    return CSenLayeredTransportProperties::PropertyL(aName, aValue, aLayer);
    }

TInt CSenLayeredHttpTransportProperties::SetIntPropertyL(const TDesC8& aName,
                                                            const TInt aValue,
                                                            TSenPropertiesLayer aLayer)
    {
    return CSenLayeredTransportProperties::SetIntPropertyL(aName, aValue, aLayer);
    }
    
TInt CSenLayeredHttpTransportProperties::IntPropertyL(const TDesC8& aName,
                                                           TInt& aValue,
                                                           TSenPropertiesLayer aLayer)
    {
    return CSenLayeredTransportProperties::IntPropertyL(aName, aValue, aLayer);
    }
    
TInt CSenLayeredHttpTransportProperties::SetBoolPropertyL(const TDesC8& aName,
                                                               const TBool aValue,
                                                               TSenPropertiesLayer aLayer)
    {
    return CSenLayeredTransportProperties::SetBoolPropertyL(aName, aValue, aLayer);
    }
    
TInt CSenLayeredHttpTransportProperties::BoolPropertyL(const TDesC8& aName,
                                                            TBool& aValue,
                                                            TSenPropertiesLayer aLayer)
    {
    return CSenLayeredTransportProperties::BoolPropertyL(aName, aValue, aLayer);
    }    

TInt CSenLayeredHttpTransportProperties::IapIdL(TUint32& aCurrentIapId)
    {
    return CSenLayeredTransportProperties::IapIdL(aCurrentIapId);
    }

TInt CSenLayeredHttpTransportProperties::SnapIdL(TUint32& aCurrentSnapId)
    {
    return CSenLayeredTransportProperties::SnapIdL(aCurrentSnapId);
    }

TInt CSenLayeredHttpTransportProperties::ProxyPortL(TInt& aProxyPort)
    {
    return CSenLayeredTransportProperties::ProxyPortL(aProxyPort);
    }

TInt CSenLayeredHttpTransportProperties::ProxyHostL(TPtrC8& aProxyHost)
    {
    return CSenLayeredTransportProperties::ProxyHostL(aProxyHost);
    }

TInt CSenLayeredHttpTransportProperties::ProxyUsageL(TBool& aValue)
    {
    return CSenLayeredTransportProperties::ProxyUsageL(aValue);
    }

TInt CSenLayeredHttpTransportProperties::IAPDialogL(TBool& aValue)
    {
    return CSenLayeredTransportProperties::IAPDialogL(aValue);
    }

TInt CSenLayeredHttpTransportProperties::SnapDialogL(TBool& aValue)
	{
	return CSenLayeredTransportProperties::SnapDialogL(aValue);
	}
TInt CSenLayeredHttpTransportProperties::ContentTypeL(TPtrC8& aContentType)
    {
    return PropertyL(KContentTypeLocalName, aContentType);
    }

TInt CSenLayeredHttpTransportProperties::UserAgentL(TPtrC8& aUserAgent)
    {
    return CSenLayeredTransportProperties::UserAgentL(aUserAgent);
    }

TInt CSenLayeredHttpTransportProperties::AcceptL(MSenProperty*& aAccept)
    {
    return CSenLayeredXmlProperties::PropertyL(KAcceptLocalName, aAccept);
    }

TInt CSenLayeredHttpTransportProperties::HttpMethodL(TSenHttpMethod& aMethod)
    {
    TPtrC8 method;
    TInt retVal = PropertyL(KHttpMethodLocalName, method);
    if (retVal == KErrNone)
        {
        if (method == KHttpGet)
            {
            aMethod = ESenHttpGet;
            return KErrNone;
            }
        else if (method == KHttpPost)
            {
            aMethod = ESenHttpPost;
            return KErrNone;
            }
        else if (method == KHttpPut)
            {
            aMethod = ESenHttpPut;
            return KErrNone;
            }
        else if (method == KHttpDelete)
            {
            aMethod = ESenHttpDelete;
            return KErrNone;
            }
        else
            {
            return KErrUnknown;
            }
        }
    else
        {
        return retVal;
        }
    }

TInt CSenLayeredHttpTransportProperties::HttpVersionL(TSenHttpVersion& aHttpVersion)
    {
    TPtrC8 method;
    TInt retVal = PropertyL(KHttpVersionLocalName, method);
    if (retVal == KErrNone)
        {
        if (method == KHttp10)
            {
            aHttpVersion = ESenHttp10;
            return KErrNone;
            }
        else if (method == KHttp11)
            {
            aHttpVersion = ESenHttp11;
            return KErrNone;
            }
        else
            {
            return KErrUnknown;
            }
        }
    else
        {
        return retVal;
        }
    }
    
TInt CSenLayeredHttpTransportProperties::HttpHeadersL(
                                        RPointerArray<MSenProperty>& aArray)
    {
    return PropertiesByTypeL(KHttpHeaderType, aArray);
    }

TInt CSenLayeredHttpTransportProperties::SetPropertyL(const TDesC8& aName, 
                                                      const TDesC8& aValue,
                                                      const TDesC8& aType)
    {
    return CSenLayeredTransportProperties::SetPropertyL(aName, aValue, aType);
    }

TInt CSenLayeredHttpTransportProperties::PropertyL(const TDesC8& aName, 
                                                   TPtrC8& aValue,
                                                   TPtrC8& aType)
    {
    return CSenLayeredTransportProperties::PropertyL(aName, aValue, aType);
    }
MSenProperties* CSenLayeredHttpTransportProperties::CloneL() const
    {
    CSenElement& element = ipFragment->AsElement();
    
    CSenLayeredHttpTransportProperties* pProperties = NewLC(element);
    if ( ipChildProperties )
        {
        CSenLayeredHttpTransportProperties* pChildProperties =
                                (CSenLayeredHttpTransportProperties*)ipChildProperties->CloneL();
        pProperties->ipChildProperties = pChildProperties;
        }
    
    CleanupStack::Pop(pProperties);
    return pProperties;
    }

void CSenLayeredHttpTransportProperties::BaseConstructL(const TDesC8& aLocalname, 
                                                 const TDesC8& aXml,
                                                 CSenXmlReader* aParser,
                                                 RStringPool* aStringPool)
    {
    CSenLayeredTransportProperties::BaseConstructL(aLocalname, aXml, aParser, aStringPool);
    }

void CSenLayeredHttpTransportProperties::BaseConstructL(const TDesC8& aNamespace, 
                                                 const TDesC8& aLocalname, 
                                                 const TDesC8& aXml,
                                                 CSenXmlReader* aParser,
                                                 RStringPool* aStringPool)
    {
    CSenLayeredTransportProperties::BaseConstructL(aNamespace, aLocalname, aXml, aParser, aStringPool);
    }

void CSenLayeredHttpTransportProperties::BaseConstructL(const TDesC8& aNamespace, 
                                                 const TDesC8& aLocalname, 
                                                 const TDesC8& aQualifiedName, 
                                                 const TDesC8& aXml,
                                                 CSenXmlReader* aParser,
                                                 RStringPool* aStringPool)
    {
    CSenLayeredTransportProperties::BaseConstructL(aNamespace, aLocalname, aQualifiedName, 
                                                   aXml, aParser, aStringPool);
    }

void CSenLayeredHttpTransportProperties::BaseConstructL(const CSenElement& aElement, RStringPool* aStringPool)
    {
    CSenLayeredTransportProperties::BaseConstructL(aElement, aStringPool);
    }

CSenLayeredHttpTransportProperties::CSenLayeredHttpTransportProperties()
    {
    }

CSenLayeredHttpTransportProperties::~CSenLayeredHttpTransportProperties()
    {
    }

TInt CSenLayeredHttpTransportProperties::DeviceIDL(TPtrC8& aDeviceID)
    {
    return CSenLayeredTransportProperties::DeviceIDL(aDeviceID);
    }

TInt CSenLayeredHttpTransportProperties::SoapActionL(TPtrC8& aAction)
    {
    return CSenLayeredTransportProperties::SoapActionL(aAction);
    }
TInt CSenLayeredHttpTransportProperties::DownloadFolderL(TPtrC8& aDownloadFolder)
    {
    return CSenLayeredTransportProperties::DownloadFolderL(aDownloadFolder);
    }
    
TInt CSenLayeredHttpTransportProperties::FileAttachmentL(const TDesC8& aCid, HBufC8*& aFileName)
    {
    return CSenLayeredTransportProperties::FileAttachmentL(aCid, aFileName);
    }

TInt CSenLayeredHttpTransportProperties::MwsNamespaceL(TPtrC8& aMwsNamespace)
    {
    return CSenLayeredTransportProperties::MwsNamespaceL(aMwsNamespace);
    }
// END OF FILE


