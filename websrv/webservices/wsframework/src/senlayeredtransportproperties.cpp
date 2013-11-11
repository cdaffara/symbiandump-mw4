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
#include "senlayeredtransportproperties.h"
#include <SenTransportProperties.h>
#include "senpropertieselement.h"   // private: Utils\inc
#include "senpropertiesfragment.h"  // private: Utils\inc
#include "senxmldebug.h"

EXPORT_C CSenLayeredTransportProperties* CSenLayeredTransportProperties::NewL()
    {
    CSenLayeredTransportProperties* pNew = NewLC();
    CleanupStack::Pop(); // pNew
    return pNew;
    }

EXPORT_C CSenLayeredTransportProperties* CSenLayeredTransportProperties::NewLC()
    {
    CSenLayeredTransportProperties* pNew = new (ELeave) CSenLayeredTransportProperties;
    CleanupStack::PushL(pNew);

    // omit parsing by passing zero-length descriptor
    pNew->BaseConstructL(KSenLayeredXmlPropertiesLocalname,
                         KNullDesC8,
                         NULL);
    return pNew;
    }

EXPORT_C CSenLayeredTransportProperties* CSenLayeredTransportProperties::NewL(const TDesC8& aXmlUtf8,
                                                                              CSenXmlReader& aParser)
    {
    CSenLayeredTransportProperties* pNew = NewLC(aXmlUtf8, aParser);
    CleanupStack::Pop(); // pNew
    return pNew;
    }

EXPORT_C CSenLayeredTransportProperties* CSenLayeredTransportProperties::NewLC(const TDesC8& aXmlUtf8,
                                                                               CSenXmlReader& aParser)
    {
    CSenLayeredTransportProperties* pNew = new (ELeave) CSenLayeredTransportProperties;
    CleanupStack::PushL(pNew);

    // parse the XML document  descriptor into DOM fragment
    pNew->BaseConstructL(KSenLayeredXmlPropertiesLocalname,
                         aXmlUtf8,
                         &aParser);
    return pNew;
    }

EXPORT_C CSenLayeredTransportProperties* CSenLayeredTransportProperties::NewL(const CSenElement& aElement)
    {
    CSenLayeredTransportProperties* pNew = NewLC(aElement);
    CleanupStack::Pop(); // pNew
    return pNew;
    }

EXPORT_C CSenLayeredTransportProperties* CSenLayeredTransportProperties::NewLC(const CSenElement& aElement)
    {
    CSenLayeredTransportProperties* pNew = new (ELeave) CSenLayeredTransportProperties;
    CleanupStack::PushL(pNew);

    // parse the XML document  descriptor into DOM fragment
    pNew->BaseConstructL(aElement);
    return pNew;
    }


// The RStringPool variants:
EXPORT_C CSenLayeredTransportProperties* CSenLayeredTransportProperties::NewL(RStringPool& aStringPool)
    {
    CSenLayeredTransportProperties* pNew = NewLC(aStringPool);
    CleanupStack::Pop(); // pNew
    return pNew;
    }

EXPORT_C CSenLayeredTransportProperties* CSenLayeredTransportProperties::NewLC(RStringPool& aStringPool)
    {
    CSenLayeredTransportProperties* pNew = new (ELeave) CSenLayeredTransportProperties;
    CleanupStack::PushL(pNew);

    // omit parsing by passing zero-length descriptor
    pNew->BaseConstructL(KSenLayeredXmlPropertiesLocalname,
                         KNullDesC8,
                         NULL,
                         &aStringPool);
    return pNew;
    }

EXPORT_C CSenLayeredTransportProperties* CSenLayeredTransportProperties::NewL(const TDesC8& aXmlUtf8,
                                                                              CSenXmlReader& aParser,
                                                                              RStringPool& aStringPool)
    {
    CSenLayeredTransportProperties* pNew = NewLC(aXmlUtf8, aParser, aStringPool);
    CleanupStack::Pop(); // pNew
    return pNew;
    }

EXPORT_C CSenLayeredTransportProperties* CSenLayeredTransportProperties::NewLC(const TDesC8& aXmlUtf8,
                                                                               CSenXmlReader& aParser,
                                                                               RStringPool& aStringPool)
    {
    CSenLayeredTransportProperties* pNew = new (ELeave) CSenLayeredTransportProperties;
    CleanupStack::PushL(pNew);

    // parse the XML document  descriptor into DOM fragment
    pNew->BaseConstructL(KSenLayeredXmlPropertiesLocalname,
                         aXmlUtf8,
                         &aParser,
                         &aStringPool);
    return pNew;
    }

EXPORT_C CSenLayeredTransportProperties* CSenLayeredTransportProperties::NewL(const CSenElement& aElement,
                                                                              RStringPool& aStringPool)
    {
    CSenLayeredTransportProperties* pNew = NewLC(aElement, aStringPool);
    CleanupStack::Pop(); // pNew
    return pNew;
    }

EXPORT_C CSenLayeredTransportProperties* CSenLayeredTransportProperties::NewLC(const CSenElement& aElement,
                                                                               RStringPool& aStringPool)
    {
    CSenLayeredTransportProperties* pNew = new (ELeave) CSenLayeredTransportProperties;
    CleanupStack::PushL(pNew);

    // Pparse the XML document  descriptor into DOM fragment
    pNew->BaseConstructL(aElement, &aStringPool);
    return pNew;
    }


    
EXPORT_C void CSenLayeredTransportProperties::SetReader(CSenXmlReader& aReader)
    {
    CSenLayeredXmlProperties::SetReader(aReader);
    }    

EXPORT_C MSenProperties::TSenPropertiesClassType CSenLayeredTransportProperties::PropertiesClassType() 
    {
    return ESenLayeredTransportProperties;
    }

EXPORT_C void CSenLayeredTransportProperties::WriteToL(RWriteStream& aWriteStream)
    {
    CSenLayeredXmlProperties::WriteToL(aWriteStream);
    }

EXPORT_C void CSenLayeredTransportProperties::ReadFromL(const TDesC8& aBuffer)
    {
    CSenLayeredXmlProperties::ReadFromL(aBuffer);
    }
    
EXPORT_C HBufC8* CSenLayeredTransportProperties::AsUtf8L()
    {
    return CSenLayeredXmlProperties::AsUtf8L();
    }

EXPORT_C HBufC8* CSenLayeredTransportProperties::AsUtf8LC()
    {
    return CSenLayeredXmlProperties::AsUtf8LC();
    }


EXPORT_C TBool CSenLayeredTransportProperties::IsSafeToCast(TSenPropertiesClassType aClass)
    {
    if ( aClass == MSenProperties::ESenLayeredTransportProperties )
        {
        return ETrue;
        }
    else
        {
        return CSenLayeredXmlProperties::IsSafeToCast( aClass );
        }
    }
    
EXPORT_C TInt CSenLayeredTransportProperties::SetPropertyL(const TDesC8& aName,
                                                        const TDesC8& aValue)
    {
    return CSenLayeredXmlProperties::SetPropertyL(aName, aValue);
    }
    
EXPORT_C TInt CSenLayeredTransportProperties::PropertyL(const TDesC8& aName,
                                                        TPtrC8& aValue)
    {
    return CSenLayeredXmlProperties::PropertyL(aName, aValue);
    }
    
    
EXPORT_C TInt CSenLayeredTransportProperties::PropertyL(const TDesC8& aName,
                                                MSenProperty*& aValue,
                                                TSenPropertiesLayer aLayer)
    {
    return CSenLayeredXmlProperties::PropertyL(aName, aValue, aLayer);
    }
        
EXPORT_C TInt CSenLayeredTransportProperties::PropertyL(const TDesC8& aName,
                                                MSenProperty*& aValue)
    {
    return CSenLayeredXmlProperties::PropertyL(aName, aValue);
    }

EXPORT_C TInt CSenLayeredTransportProperties::PropertiesByTypeL(
                                            const TDesC8& aType,
                                            RPointerArray<MSenProperty>& aArray)
    {
    return CSenLayeredXmlProperties::PropertiesByTypeL(aType, aArray);
    }

EXPORT_C TInt CSenLayeredTransportProperties::SetIntPropertyL(const TDesC8& aName,
                                                            const TInt aValue)
    {
    return CSenLayeredXmlProperties::SetIntPropertyL(aName, aValue);
    }
    
EXPORT_C TInt CSenLayeredTransportProperties::IntPropertyL(const TDesC8& aName,
                                                           TInt& aValue)
    {
    return CSenLayeredXmlProperties::IntPropertyL(aName, aValue);
    }
    
EXPORT_C TInt CSenLayeredTransportProperties::SetBoolPropertyL(const TDesC8& aName,
                                                               const TBool aValue)
    {
    return CSenLayeredXmlProperties::SetBoolPropertyL(aName, aValue);
    }
    
EXPORT_C TInt CSenLayeredTransportProperties::BoolPropertyL(const TDesC8& aName,
                                                            TBool& aValue)
    {
    return CSenLayeredXmlProperties::BoolPropertyL(aName, aValue);
    }

EXPORT_C void CSenLayeredTransportProperties::WriteToL(RWriteStream& aWriteStream, TSenPropertiesLayer aLayer)
    {
    CSenLayeredXmlProperties::WriteToL(aWriteStream, aLayer);
    }

EXPORT_C void CSenLayeredTransportProperties::ReadFromL(const TDesC8& aBuffer, TSenPropertiesLayer aLayer)
    {
    CSenLayeredXmlProperties::ReadFromL(aBuffer, aLayer);
    }
    
EXPORT_C HBufC8* CSenLayeredTransportProperties::AsUtf8L(TSenPropertiesLayer aLayer)
    {
    return CSenLayeredXmlProperties::AsUtf8L(aLayer);
    }

EXPORT_C HBufC8* CSenLayeredTransportProperties::AsUtf8LC(TSenPropertiesLayer aLayer)
    {
    return CSenLayeredXmlProperties::AsUtf8LC(aLayer);
    }
    
EXPORT_C TInt CSenLayeredTransportProperties::ShrinkTo(TSenPropertiesLayer aLayer)
    {
    return CSenLayeredXmlProperties::ShrinkTo(aLayer);
    }
    
EXPORT_C void CSenLayeredTransportProperties::ExpandToL(TSenPropertiesLayer aLayer)
    {
    CSenLayeredXmlProperties::ExpandToL(aLayer);
    }
    
EXPORT_C MSenLayeredProperties::TSenPropertiesLayer CSenLayeredTransportProperties::TopLayer()
    {
    return CSenLayeredXmlProperties::TopLayer();
    }
    
EXPORT_C TInt CSenLayeredTransportProperties::SetPropertyL(const TDesC8& aName,
                                                        const TDesC8& aValue,
                                                        TSenPropertiesLayer aLayer)
    {
    return CSenLayeredXmlProperties::SetPropertyL(aName, aValue, aLayer);
    }
    
EXPORT_C TInt CSenLayeredTransportProperties::PropertyL(const TDesC8& aName,
                                                     TPtrC8& aValue,
                                                     TSenPropertiesLayer aLayer)
    {
    return CSenLayeredXmlProperties::PropertyL(aName, aValue, aLayer);
    }

EXPORT_C TInt CSenLayeredTransportProperties::SetIntPropertyL(const TDesC8& aName,
                                                            const TInt aValue,
                                                            TSenPropertiesLayer aLayer)
    {
    return CSenLayeredXmlProperties::SetIntPropertyL(aName, aValue, aLayer);
    }
    
EXPORT_C TInt CSenLayeredTransportProperties::IntPropertyL(const TDesC8& aName,
                                                           TInt& aValue,
                                                           TSenPropertiesLayer aLayer)
    {
    return CSenLayeredXmlProperties::IntPropertyL(aName, aValue, aLayer);
    }
    
EXPORT_C TInt CSenLayeredTransportProperties::SetBoolPropertyL(const TDesC8& aName,
                                                               const TBool aValue,
                                                               TSenPropertiesLayer aLayer)
    {
    return CSenLayeredXmlProperties::SetBoolPropertyL(aName, aValue, aLayer);
    }
    
EXPORT_C TInt CSenLayeredTransportProperties::BoolPropertyL(const TDesC8& aName,
                                                            TBool& aValue,
                                                            TSenPropertiesLayer aLayer)
    {
    return CSenLayeredXmlProperties::BoolPropertyL(aName, aValue, aLayer);
    }

EXPORT_C TInt CSenLayeredTransportProperties::SetPropertyL(const TDesC8& aName, 
                                                           const TDesC8& aValue,
                                                           const TDesC8& aType)
    {
    return CSenLayeredXmlProperties::SetPropertyL(aName, aValue, aType);
    }

EXPORT_C TInt CSenLayeredTransportProperties::PropertyL(const TDesC8& aName, 
                                                       TPtrC8& aValue,
                                                       TPtrC8& aType)
    {
    return CSenLayeredXmlProperties::PropertyL(aName, aValue, aType);
    }
EXPORT_C MSenProperties* CSenLayeredTransportProperties::CloneL() const
    {
    CSenElement& element = ipFragment->AsElement();
    
    CSenLayeredTransportProperties* pProperties = NewLC(element);
    if ( ipChildProperties )
        {
        CSenLayeredTransportProperties* pChildProperties =
                                (CSenLayeredTransportProperties*)ipChildProperties->CloneL();
        pProperties->ipChildProperties = pChildProperties;
        }
    
    CleanupStack::Pop(pProperties);
    return pProperties;
    }

EXPORT_C MSenProperties* CSenLayeredTransportProperties::Clone(TInt& aOkOrError) const
    {
    MSenProperties* pClone = NULL;
    TRAP( aOkOrError, pClone = CloneL(); )
    return pClone;
    }    

EXPORT_C void CSenLayeredTransportProperties::BaseConstructL(const TDesC8& aLocalname, 
                                                 const TDesC8& aXml,
                                                 CSenXmlReader* aParser,
                                                 RStringPool* aStringPool)
    {
    CSenLayeredXmlProperties::BaseConstructL(aLocalname, aXml, aParser, aStringPool);
    }

EXPORT_C void CSenLayeredTransportProperties::BaseConstructL(const TDesC8& aNamespace, 
                                                 const TDesC8& aLocalname, 
                                                 const TDesC8& aXml,
                                                 CSenXmlReader* aParser,
                                                 RStringPool* aStringPool)
    {
    CSenLayeredXmlProperties::BaseConstructL(aNamespace, aLocalname, aXml, aParser, aStringPool);
    }

EXPORT_C void CSenLayeredTransportProperties::BaseConstructL(const TDesC8& aNamespace, 
                                                 const TDesC8& aLocalname, 
                                                 const TDesC8& aQualifiedName, 
                                                 const TDesC8& aXml,
                                                 CSenXmlReader* aParser,
                                                 RStringPool* aStringPool)
    {
    CSenLayeredXmlProperties::BaseConstructL(aNamespace, aLocalname, aQualifiedName, 
                                             aXml, aParser, aStringPool);
    }

EXPORT_C void CSenLayeredTransportProperties::BaseConstructL(const CSenElement& aElement,
                                                             RStringPool* aStringPool)
    {
    CSenLayeredXmlProperties::BaseConstructL(aElement, aStringPool);
    }

EXPORT_C CSenLayeredTransportProperties::CSenLayeredTransportProperties()
    {
    }

EXPORT_C CSenLayeredTransportProperties::~CSenLayeredTransportProperties()
    {
    }

EXPORT_C TInt CSenLayeredTransportProperties::DeviceIDL(TPtrC8& aDeviceID)
    {
    return PropertyL(KDeviceIDLocalName, aDeviceID);    
    }

EXPORT_C TInt CSenLayeredTransportProperties::SoapActionL(TPtrC8& aAction)
    {
    return PropertyL(KSoapActionLocalName, aAction);    
    }

EXPORT_C TInt CSenLayeredTransportProperties::IapIdL(TUint32& aCurrentIapId)
    {
    TPtrC8 value;
    TInt retVal = PropertyL(KIapIdLocalName, value);
    
    if ( retVal == KErrNone )
        {
        TLex8 lex(value);
        retVal = lex.Val(aCurrentIapId, EDecimal);
        }
    if(aCurrentIapId > 0) return retVal;
    else return KErrNotFound;
    }

EXPORT_C TInt CSenLayeredTransportProperties::SnapIdL(TUint32& aCurrentSnapId)
    {
    TPtrC8 value;
    TInt retVal = PropertyL(KSnapIdLocalName, value);
    
    if ( retVal == KErrNone )
        {
        TLex8 lex(value);
        retVal = lex.Val(aCurrentSnapId, EDecimal);
        }
        
    if(aCurrentSnapId > 0) return retVal;
    else return KErrNotFound;
    }

EXPORT_C TInt CSenLayeredTransportProperties::ProxyPortL(TInt& aProxyPort)
    {
    TPtrC8 value;
    TInt retVal = PropertyL(KProxyPortLocalName, value);
    if ( retVal == KErrNone )
        {
        TLex8 lex(value);
        TUint proxyPort;
        retVal = lex.Val(proxyPort, EDecimal);
        if ( retVal == KErrNone )
            {
            aProxyPort = proxyPort;
            }
        }

    return retVal;
    }

EXPORT_C TInt CSenLayeredTransportProperties::HeartbeatL(TInt& aHeartbeat)
    {
    return IntPropertyL(KSenConnectionHeartbeat, aHeartbeat);
    }

EXPORT_C TInt CSenLayeredTransportProperties::MaxTimeToLiveL(TInt& aMax)
    {
    return IntPropertyL(KSenConnectionHeartbeatMaxTTL, aMax);
    }

EXPORT_C TInt CSenLayeredTransportProperties::MinTimeToLiveL(TInt& aMin)
    {
    return IntPropertyL(KSenConnectionHeartbeatMinTTL, aMin);
    }

EXPORT_C TInt CSenLayeredTransportProperties::ProxyHostL(TPtrC8& aProxyHost)
    {
    return PropertyL(KProxyHostLocalName, aProxyHost);
    }

EXPORT_C TInt CSenLayeredTransportProperties::ProxyUsageL(TBool& aValue)
    {
    return BoolPropertyL(KProxyUsageLocalName, aValue);
    }

EXPORT_C TInt CSenLayeredTransportProperties::IAPDialogL(TBool& aValue)
    {
    return BoolPropertyL(KSenIAPDoNotPrompt, aValue);
    }

EXPORT_C TInt CSenLayeredTransportProperties::SnapDialogL(TBool& aValue)
    {
    return BoolPropertyL(KSenSNAPDoNotPrompt, aValue);
    }
EXPORT_C TInt CSenLayeredTransportProperties::UserAgentL(TPtrC8& aUserAgent)
    {
    return PropertyL(KUserAgentLocalName, aUserAgent);
    }
EXPORT_C TInt CSenLayeredTransportProperties::DownloadFolderL(TPtrC8& aDownloadFolder)
    {
    return PropertyL(KDownloadFolderLocalName, aDownloadFolder);
    }  
EXPORT_C TInt CSenLayeredTransportProperties::SetMaxRetryTTLL(TInt aMaxRetryTTL)
	{ 
	return SetIntPropertyL( KSenConnectionRetryMaxTTL, aMaxRetryTTL );
	}
	
EXPORT_C TInt CSenLayeredTransportProperties::MaxRetryTTLL(TInt& aMaxRetryTTL)
	{ 
	return IntPropertyL( KSenConnectionRetryMaxTTL, aMaxRetryTTL ); 
	}

EXPORT_C TInt CSenLayeredTransportProperties::SetMinRetryTTLL(TInt aMinRetryTTL)
	{ 
	return SetIntPropertyL( KSenConnectionRetryMinTTL, aMinRetryTTL );
	}
	
EXPORT_C TInt CSenLayeredTransportProperties::MinRetryTTLL(TInt& aMinRetryTTL)
	{ 
	return IntPropertyL( KSenConnectionRetryMinTTL, aMinRetryTTL ); 
	}
	
EXPORT_C TInt CSenLayeredTransportProperties::SetRetryDeltaTimeoutL(TInt aTimeout)
	{ 
	return SetIntPropertyL( KSenConnectionRetryDelta, aTimeout );
	}
	
EXPORT_C TInt CSenLayeredTransportProperties::RetryDeltaTimeoutL(TInt& aTimeout)
	{ 
	return IntPropertyL( KSenConnectionRetryDelta, aTimeout ); 
	}

      

EXPORT_C TInt CSenLayeredTransportProperties::FileAttachmentL(const TDesC8& aCid, HBufC8*& aFileName)
    {
    _LIT8(KAttachment,"Attachment");
    TPtrC8 pFileAttachments;
    TInt err(KErrNone);
    err = PropertyL(KFileAttachmentsLocalName,pFileAttachments);    
    if (err!=KErrNone)
        {
        return err;
        }
    else
        {
        CSenXmlReader* reader = CSenXmlReader::NewL(KXmlParserMimeType); // use libxml2 sax parser
        CleanupStack::PushL(reader);
        CSenDomFragment* pFragment = CSenDomFragment::NewL();
        CleanupStack::PushL(pFragment);
        pFragment->SetReader(*reader);    
        pFragment->BuildFrom(pFileAttachments);  
        HBufC8* pLocalName = HBufC8::NewLC(KAttachment().Length()+aCid.Length());
        TPtr8 localNameDes = pLocalName->Des();
        localNameDes.Append(KAttachment);
        localNameDes.Append(aCid);
        CSenElement* pElement = pFragment->AsElement().Element(*pLocalName);
        if(pElement)
            {
            if (aFileName)
                {
                delete aFileName;
                aFileName = NULL;
                }
            aFileName = HBufC8::NewL(pElement->Content().Length());
            TPtr8 fileNameDes = aFileName->Des();  //CodeScannerWarnings
            fileNameDes.Append(pElement->Content());
            CleanupStack::PopAndDestroy(pLocalName);
            CleanupStack::PopAndDestroy(pFragment);
            CleanupStack::PopAndDestroy(reader);
            return KErrNone;
            }
        else
            {
            CleanupStack::PopAndDestroy(pLocalName);
            CleanupStack::PopAndDestroy(pFragment);
            CleanupStack::PopAndDestroy(reader);
            return KErrNotFound;
            }
        }
    }
    
EXPORT_C TInt CSenLayeredTransportProperties::MwsNamespaceL(TPtrC8& aMwsNamespace)
    {
    return PropertyL(KMwsNamespaceLocalName, aMwsNamespace);
    }    
// END OF FILE


