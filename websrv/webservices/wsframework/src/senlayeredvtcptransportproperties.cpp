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
#include "senlayeredvtcptransportproperties.h"
#include "senvtcptransportproperties.h"              // property keys (constants)
#include <SenTransportProperties.h>
#include "senpropertieselement.h"   // private: Utils\inc
#include "senpropertiesfragment.h"  // private: Utils\inc

EXPORT_C CSenLayeredVtcpTransportProperties* CSenLayeredVtcpTransportProperties::NewL()
    {
    CSenLayeredVtcpTransportProperties* pNew = NewLC();
    CleanupStack::Pop(); // pNew
    return pNew;
    }

EXPORT_C CSenLayeredVtcpTransportProperties* CSenLayeredVtcpTransportProperties::NewLC()
    {
    CSenLayeredVtcpTransportProperties* pNew = new (ELeave) CSenLayeredVtcpTransportProperties;
    CleanupStack::PushL(pNew);

    // omit parsing by passing zero-length descriptor
    pNew->BaseConstructL(KSenLayeredXmlPropertiesLocalname, KNullDesC8, NULL);
    return pNew;
    }

EXPORT_C CSenLayeredVtcpTransportProperties* CSenLayeredVtcpTransportProperties::NewL(const CSenElement& aElement)
    {
    CSenLayeredVtcpTransportProperties* pNew = NewLC(aElement);
    CleanupStack::Pop(); // pNew
    return pNew;
    }

EXPORT_C CSenLayeredVtcpTransportProperties* CSenLayeredVtcpTransportProperties::NewLC(const CSenElement& aElement)
    {
    CSenLayeredVtcpTransportProperties* pNew = new (ELeave) CSenLayeredVtcpTransportProperties;
    CleanupStack::PushL(pNew);

    // parse the XML document  descriptor into DOM fragment
    pNew->BaseConstructL(aElement);
    return pNew;
    }
        
void CSenLayeredVtcpTransportProperties::SetReader(CSenXmlReader& aReader)
    {
    CSenLayeredTransportProperties::SetReader(aReader);
    }    
    

MSenProperties::TSenPropertiesClassType CSenLayeredVtcpTransportProperties::PropertiesClassType() 
    {
    return ESenLayeredVtcpTransportProperties;
    }

void CSenLayeredVtcpTransportProperties::WriteToL(RWriteStream& aWriteStream)
    {
    CSenLayeredTransportProperties::WriteToL(aWriteStream);
    }

void CSenLayeredVtcpTransportProperties::ReadFromL(const TDesC8& aBuffer)
    {
    CSenLayeredTransportProperties::ReadFromL(aBuffer);
    }
    
HBufC8* CSenLayeredVtcpTransportProperties::AsUtf8L()
    {
    return CSenLayeredTransportProperties::AsUtf8L();
    }

HBufC8* CSenLayeredVtcpTransportProperties::AsUtf8LC()
    {
    return CSenLayeredTransportProperties::AsUtf8LC();
    }

TBool CSenLayeredVtcpTransportProperties::IsSafeToCast(TSenPropertiesClassType aClass)
    {
    if ( aClass == MSenProperties::ESenLayeredVtcpTransportProperties )
        {
        return ETrue;
        }
    else
        {
        return CSenLayeredTransportProperties::IsSafeToCast( aClass );
        }
    }

TInt CSenLayeredVtcpTransportProperties::SetPropertyL(const TDesC8& aName, const TDesC8& aValue)
    {
    return CSenLayeredTransportProperties::SetPropertyL(aName, aValue);
    }
    
TInt CSenLayeredVtcpTransportProperties::PropertyL(const TDesC8& aName, TPtrC8& aValue)
    {
    return CSenLayeredTransportProperties::PropertyL(aName, aValue);
    }
    
TInt CSenLayeredVtcpTransportProperties::PropertyL(const TDesC8& aName, MSenProperty*& aValue, TSenPropertiesLayer aLayer)
    {
    return CSenLayeredTransportProperties::PropertyL(aName, aValue, aLayer);
    }
        
TInt CSenLayeredVtcpTransportProperties::PropertyL(const TDesC8& aName, MSenProperty*& aValue)
    {
    return CSenLayeredTransportProperties::PropertyL(aName, aValue);
    }

TInt CSenLayeredVtcpTransportProperties::PropertiesByTypeL(const TDesC8& aType, RPointerArray<MSenProperty>& aArray)
    {
    return CSenLayeredTransportProperties::PropertiesByTypeL(aType, aArray);
    }

TInt CSenLayeredVtcpTransportProperties::SetIntPropertyL(const TDesC8& aName, const TInt aValue)
    {
    return CSenLayeredTransportProperties::SetIntPropertyL(aName, aValue);
    }
    
TInt CSenLayeredVtcpTransportProperties::IntPropertyL(const TDesC8& aName, TInt& aValue)
    {
    return CSenLayeredTransportProperties::IntPropertyL(aName, aValue);
    }
    
TInt CSenLayeredVtcpTransportProperties::SetBoolPropertyL(const TDesC8& aName, const TBool aValue)
    {
    return CSenLayeredTransportProperties::SetBoolPropertyL(aName, aValue);
    }
    
TInt CSenLayeredVtcpTransportProperties::BoolPropertyL(const TDesC8& aName, TBool& aValue)
    {
    return CSenLayeredTransportProperties::BoolPropertyL(aName, aValue);
    }   

void CSenLayeredVtcpTransportProperties::WriteToL(RWriteStream& aWriteStream, TSenPropertiesLayer aLayer)
    {
    CSenLayeredTransportProperties::WriteToL(aWriteStream, aLayer);
    }

void CSenLayeredVtcpTransportProperties::ReadFromL(const TDesC8& aBuffer, TSenPropertiesLayer aLayer)
    {
    CSenLayeredTransportProperties::ReadFromL(aBuffer, aLayer);
    }
    
HBufC8* CSenLayeredVtcpTransportProperties::AsUtf8L(TSenPropertiesLayer aLayer)
    {
    return CSenLayeredTransportProperties::AsUtf8L(aLayer);
    }

HBufC8* CSenLayeredVtcpTransportProperties::AsUtf8LC(TSenPropertiesLayer aLayer)
    {
    return CSenLayeredTransportProperties::AsUtf8LC(aLayer);
    }

TInt CSenLayeredVtcpTransportProperties::ShrinkTo(TSenPropertiesLayer aLayer)
    {
    return CSenLayeredTransportProperties::ShrinkTo(aLayer);
    }
    
void CSenLayeredVtcpTransportProperties::ExpandToL(TSenPropertiesLayer aLayer)
    {
    CSenLayeredTransportProperties::ExpandToL(aLayer);
    }
    
MSenLayeredProperties::TSenPropertiesLayer CSenLayeredVtcpTransportProperties::TopLayer()
    {
    return CSenLayeredTransportProperties::TopLayer();
    }
    
TInt CSenLayeredVtcpTransportProperties::SetPropertyL(const TDesC8& aName, const TDesC8& aValue, TSenPropertiesLayer aLayer)
    {
    return CSenLayeredTransportProperties::SetPropertyL(aName, aValue, aLayer);
    }
    
TInt CSenLayeredVtcpTransportProperties::PropertyL(const TDesC8& aName,
                                                     TPtrC8& aValue,
                                                     TSenPropertiesLayer aLayer)
    {
    return CSenLayeredTransportProperties::PropertyL(aName, aValue, aLayer);
    }

TInt CSenLayeredVtcpTransportProperties::SetIntPropertyL(const TDesC8& aName,
                                                            const TInt aValue,
                                                            TSenPropertiesLayer aLayer)
    {
    return CSenLayeredTransportProperties::SetIntPropertyL(aName, aValue, aLayer);
    }
    
TInt CSenLayeredVtcpTransportProperties::IntPropertyL(const TDesC8& aName,
                                                           TInt& aValue,
                                                           TSenPropertiesLayer aLayer)
    {
    return CSenLayeredTransportProperties::IntPropertyL(aName, aValue, aLayer);
    }
    
TInt CSenLayeredVtcpTransportProperties::SetBoolPropertyL(const TDesC8& aName,
                                                               const TBool aValue,
                                                               TSenPropertiesLayer aLayer)
    {
    return CSenLayeredTransportProperties::SetBoolPropertyL(aName, aValue, aLayer);
    }
    
TInt CSenLayeredVtcpTransportProperties::BoolPropertyL(const TDesC8& aName,
                                                            TBool& aValue,
                                                            TSenPropertiesLayer aLayer)
    {
    return CSenLayeredTransportProperties::BoolPropertyL(aName, aValue, aLayer);
    }    

TInt CSenLayeredVtcpTransportProperties::IapIdL(TUint32& aCurrentIapId)
    {
    return CSenLayeredTransportProperties::IapIdL(aCurrentIapId);
    }

 TInt CSenLayeredVtcpTransportProperties::SnapIdL(TUint32& aCurrentSnapId)
 	{
	return CSenLayeredTransportProperties::SnapIdL(aCurrentSnapId); 	
 	}
TInt CSenLayeredVtcpTransportProperties::ProxyPortL(TInt& aProxyPort)
    {
    return CSenLayeredTransportProperties::ProxyPortL(aProxyPort);
    }

TInt CSenLayeredVtcpTransportProperties::ProxyHostL(TPtrC8& aProxyHost)
    {
    return CSenLayeredTransportProperties::ProxyHostL(aProxyHost);
    }

TInt CSenLayeredVtcpTransportProperties::ProxyUsageL(TBool& aValue)
    {
    return CSenLayeredTransportProperties::ProxyUsageL(aValue);
    }

TInt CSenLayeredVtcpTransportProperties::UserAgentL(TPtrC8& aUserAgent)
    {
    return CSenLayeredTransportProperties::UserAgentL(aUserAgent);
    }

TInt CSenLayeredVtcpTransportProperties::SetPropertyL(const TDesC8& aName, 
                                                      const TDesC8& aValue,
                                                      const TDesC8& aType)
    {
    return CSenLayeredTransportProperties::SetPropertyL(aName, aValue, aType);
    }

TInt CSenLayeredVtcpTransportProperties::PropertyL(const TDesC8& aName, 
                                                   TPtrC8& aValue,
                                                   TPtrC8& aType)
    {
    return CSenLayeredTransportProperties::PropertyL(aName, aValue, aType);
    }

TInt CSenLayeredVtcpTransportProperties::SoapActionL(TPtrC8& aAction)
    {
    return CSenLayeredTransportProperties::SoapActionL(aAction);
    }

TInt CSenLayeredVtcpTransportProperties::DeviceIDL(TPtrC8& aDeviceID)
    {
    return CSenLayeredTransportProperties::DeviceIDL(aDeviceID);
    }

TInt CSenLayeredVtcpTransportProperties::DeviceLCIDL(TPtrC8& aDeviceLCID)
    {
    return PropertyL(KDeviceLCIDLocalName, aDeviceLCID);    
    }

TInt CSenLayeredVtcpTransportProperties::ConnectionBoundL(TBool& aConnectionBound)
    {
    return BoolPropertyL(KConnectionBoundLocalName, aConnectionBound);    
    }

TInt CSenLayeredVtcpTransportProperties::ConnectionTimeOutL(TInt& aConnectionTimeOut)
    {
    return IntPropertyL(KConnectionTimeOutLocalName, aConnectionTimeOut);    
    }
TInt CSenLayeredVtcpTransportProperties::DownloadFolderL(TPtrC8& aDownloadFolder)
    {
    return CSenLayeredTransportProperties::DownloadFolderL(aDownloadFolder);
    }

TInt CSenLayeredVtcpTransportProperties::FileAttachmentL(const TDesC8& aCid, HBufC8*& aFileName)
    {
    return CSenLayeredTransportProperties::FileAttachmentL(aCid, aFileName);
    }

CSenLayeredVtcpTransportProperties::CSenLayeredVtcpTransportProperties()
    {
    }

CSenLayeredVtcpTransportProperties::~CSenLayeredVtcpTransportProperties()
    {
    }

MSenProperties* CSenLayeredVtcpTransportProperties::CloneL() const
    {
    CSenElement& element = ipFragment->AsElement();
    
    CSenLayeredVtcpTransportProperties* pProperties = NewLC(element);
    if ( ipChildProperties )
        {
        CSenLayeredVtcpTransportProperties* pChildProperties =
                                (CSenLayeredVtcpTransportProperties*)ipChildProperties->CloneL();
        pProperties->ipChildProperties = pChildProperties;
        }
    
    CleanupStack::Pop(pProperties);
    return pProperties;
    }

TInt CSenLayeredVtcpTransportProperties::MwsNamespaceL(TPtrC8& aMwsNamespace)
    {
    return CSenLayeredTransportProperties::MwsNamespaceL(aMwsNamespace);
    }

// END OF FILE


