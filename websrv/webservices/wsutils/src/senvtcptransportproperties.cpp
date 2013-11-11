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
#include "senvtcptransportproperties.h"
#include "senpropertiesfragment.h"
#include "MSenProperty.h"
#include <SenXmlUtils.h>

EXPORT_C CSenVtcpTransportProperties* CSenVtcpTransportProperties::NewL()
    {
    CSenVtcpTransportProperties* pNew = NewLC();
    CleanupStack::Pop(); // pNew
    return pNew;
    }

EXPORT_C CSenVtcpTransportProperties* CSenVtcpTransportProperties::NewLC()
    {
    CSenVtcpTransportProperties* pNew = new (ELeave) CSenVtcpTransportProperties;
    CleanupStack::PushL(pNew);

    // omit parsing by passing zero-length descriptor
    pNew->BaseConstructL(KSenXmlPropertiesLocalname,
                         KNullDesC8,
                         NULL);
    return pNew;
    }

EXPORT_C CSenVtcpTransportProperties* CSenVtcpTransportProperties::NewL(const TDesC8& aXmlUtf8,
                                                               CSenXmlReader& aParser)
    {
    CSenVtcpTransportProperties* pNew = NewLC(aXmlUtf8, aParser);
    CleanupStack::Pop(); // pNew
    return pNew;
    }

EXPORT_C CSenVtcpTransportProperties* CSenVtcpTransportProperties::NewLC(const TDesC8& aXmlUtf8,
                                                                CSenXmlReader& aParser)
    {
    CSenVtcpTransportProperties* pNew = new (ELeave) CSenVtcpTransportProperties;
    CleanupStack::PushL(pNew);

    // parse the XML document  descriptor into DOM fragment
    pNew->BaseConstructL(KSenXmlPropertiesLocalname,
                         aXmlUtf8,
                         &aParser);
    return pNew;
    }
    
EXPORT_C CSenVtcpTransportProperties* CSenVtcpTransportProperties::NewL(const CSenElement& aElement)
    {
    CSenVtcpTransportProperties* pNew = NewLC(aElement);
    CleanupStack::Pop(); // pNew
    return pNew;
    }

EXPORT_C CSenVtcpTransportProperties* CSenVtcpTransportProperties::NewLC(const CSenElement& aElement)
    {
    CSenVtcpTransportProperties* pNew = new (ELeave) CSenVtcpTransportProperties;
    CleanupStack::PushL(pNew);

    // parse the XML document  descriptor into DOM fragment
    pNew->BaseConstructL(aElement);
    return pNew;
    }

void CSenVtcpTransportProperties::SetReader(CSenXmlReader& aReader)
    {
    CSenTransportProperties::SetReader(aReader);
    } 

MSenProperties::TSenPropertiesClassType CSenVtcpTransportProperties::PropertiesClassType() 
    {
    return ESenVtcpTransportProperties;
    }

void CSenVtcpTransportProperties::WriteToL(RWriteStream& aWriteStream)
    {
    CSenTransportProperties::WriteToL(aWriteStream);
    }
    
void CSenVtcpTransportProperties::ReadFromL(const TDesC8& aBuffer)
    {
    CSenTransportProperties::ReadFromL(aBuffer);
    }    

HBufC8* CSenVtcpTransportProperties::AsUtf8L()
    {
    return CSenTransportProperties::AsUtf8L();
    }

HBufC8* CSenVtcpTransportProperties::AsUtf8LC()
    {
    return CSenTransportProperties::AsUtf8LC();
    }
    
TBool CSenVtcpTransportProperties::IsSafeToCast(TSenPropertiesClassType aClass)
    {
    if ( aClass == MSenProperties::ESenVtcpTransportProperties )
        {
        return ETrue;
        }
    else
        {
        return CSenTransportProperties::IsSafeToCast(aClass);
        }
    }      

TInt CSenVtcpTransportProperties::SetPropertyL(const TDesC8& aName,
                                               const TDesC8& aValue,
                                               const TDesC8& aType)
    {
    return CSenTransportProperties::SetPropertyL(aName, aValue, aType);
    }

TInt CSenVtcpTransportProperties::PropertyL(const TDesC8& aName,
                                            TPtrC8& aValue,
                                            TPtrC8& aType)
    {
    return CSenTransportProperties::PropertyL(aName, aValue, aType);
    }

    
TInt CSenVtcpTransportProperties::SetPropertyL(const TDesC8& aName,
                                               const TDesC8& aValue)
    {
    return CSenTransportProperties::SetPropertyL(aName, aValue);
    }
    
TInt CSenVtcpTransportProperties::PropertyL(const TDesC8& aName,
                                            TPtrC8& aValue)
    {
    return CSenTransportProperties::PropertyL(aName, aValue);
    }
    
TInt CSenVtcpTransportProperties::SetIntPropertyL(const TDesC8& aName,
                                                  const TInt aValue)
    {
    return CSenTransportProperties::SetIntPropertyL(aName, aValue);
    }

TInt CSenVtcpTransportProperties::IntPropertyL(const TDesC8& aName,
                                               TInt& aValue)
    {
    return CSenTransportProperties::IntPropertyL(aName, aValue);
    }

TInt CSenVtcpTransportProperties::SetBoolPropertyL(const TDesC8& aName,
                                                   const TBool aValue)
    {
    return CSenTransportProperties::SetBoolPropertyL(aName, aValue);
    }

TInt CSenVtcpTransportProperties::BoolPropertyL(const TDesC8& aName,
                                                TBool& aValue)
    {
    return CSenTransportProperties::BoolPropertyL(aName, aValue);
    }  

CSenVtcpTransportProperties::~CSenVtcpTransportProperties()
    {
    }

void CSenVtcpTransportProperties::BaseConstructL(const TDesC8& aLocalname, 
                                                 const TDesC8& aXml,
                                                 CSenXmlReader* aParser)
    {
    CSenTransportProperties::BaseConstructL(aLocalname, aXml, aParser);
    }

void CSenVtcpTransportProperties::BaseConstructL(const CSenElement& aElement)
    {
    CSenTransportProperties::BaseConstructL(aElement);
    }

CSenVtcpTransportProperties::CSenVtcpTransportProperties()
    {
    }

TInt CSenVtcpTransportProperties::DeviceLCIDL(TPtrC8& aDeviceLCID)
    {
    return PropertyL(KDeviceLCIDLocalName, aDeviceLCID);
    }

void CSenVtcpTransportProperties::SetDeviceLCIDL(const TDesC8& aDeviceLCID)
    {
    SetPropertyL(KDeviceLCIDLocalName, aDeviceLCID);
    }

TInt CSenVtcpTransportProperties::ConnectionBoundL(TBool& aConnectionBound)
    {
    return BoolPropertyL(KConnectionBoundLocalName, aConnectionBound);
    }

void CSenVtcpTransportProperties::SetConnectionBoundL(const TBool& aConnectionBound)
    {
    SetBoolPropertyL(KConnectionBoundLocalName, aConnectionBound);
    }

TInt CSenVtcpTransportProperties::ConnectionTimeOutL(TInt& aConnectionTimeOut)
    {
    return IntPropertyL(KConnectionTimeOutLocalName, aConnectionTimeOut);
    }

void CSenVtcpTransportProperties::SetConnectionTimeOutL(const TInt& aConnectionTimeOut)
    {
    SetIntPropertyL(KConnectionTimeOutLocalName, aConnectionTimeOut);
    }
    
//override
TInt CSenVtcpTransportProperties::IapIdL(TUint32& aCurrentIapId)
    {
    return CSenTransportProperties::IapIdL(aCurrentIapId);
    }

void CSenVtcpTransportProperties::SetIapIdL(TUint32 aIapId)
    {
    CSenTransportProperties::SetIapIdL(aIapId);
    }

void CSenVtcpTransportProperties::SetSnapIdL(TUint32 aSnapId)
{
    CSenTransportProperties::SetSnapIdL(aSnapId);
}

TInt CSenVtcpTransportProperties::SnapIdL(TUint32& aCurrentSnapId)
{
    return CSenTransportProperties::SnapIdL(aCurrentSnapId);
}

TInt CSenVtcpTransportProperties::ProxyPortL(TInt& aProxyPort)
    {
    return CSenTransportProperties::ProxyPortL(aProxyPort);
    }

void CSenVtcpTransportProperties::SetProxyPortL(TInt aProxyPort)
    {
    CSenTransportProperties::SetProxyPortL(aProxyPort);
    }

TInt CSenVtcpTransportProperties::ProxyHostL(TPtrC8& aProxyHost)
    {
    return CSenTransportProperties::ProxyHostL(aProxyHost);
    }

void CSenVtcpTransportProperties::SetProxyHostL(const TDesC8& aProxyHost)
    {
    CSenTransportProperties::SetProxyHostL(aProxyHost);
    }

TInt CSenVtcpTransportProperties::ProxyUsageL(TBool& aValue)
    {
    return CSenTransportProperties::ProxyUsageL(aValue);
    }

void CSenVtcpTransportProperties::SetProxyUsageL(TBool aProxyUsage)
    {
    CSenTransportProperties::SetProxyUsageL(aProxyUsage);
    }
    
TInt CSenVtcpTransportProperties::SecureDialogL(TBool& aValue)
    {
    return CSenTransportProperties::SecureDialogL(aValue);
    }

void CSenVtcpTransportProperties::SetSecureDialogL(TBool aSecureDialog)
    {
    CSenTransportProperties::SetSecureDialogL(aSecureDialog);
    }

TInt CSenVtcpTransportProperties::UserAgentL(TPtrC8& aUserAgent)
    {
    return CSenTransportProperties::UserAgentL(aUserAgent);
    }

void CSenVtcpTransportProperties::SetUserAgentL(const TDesC8& aUserAgent)
    {
    CSenTransportProperties::SetUserAgentL(aUserAgent);
    }


TInt CSenVtcpTransportProperties::DeviceIDL(TPtrC8& aDeviceID)
    {
    return CSenTransportProperties::DeviceIDL(aDeviceID);
    }

void CSenVtcpTransportProperties::SetDeviceIDL(const TDesC8& aDeviceID)
    {
    CSenTransportProperties::SetDeviceIDL(aDeviceID);
    }    

TInt CSenVtcpTransportProperties::SoapActionL(TPtrC8& aAction)
    {
    return CSenTransportProperties::SoapActionL(aAction);
    }

void CSenVtcpTransportProperties::SetSoapActionL(const TDesC8& aAction)
    {
    CSenTransportProperties::SetSoapActionL(aAction);
    }

void CSenVtcpTransportProperties::ApplyBindingL(TSOAPVersion aSoapVersion)
    {
    CSenTransportProperties::ApplyBindingL(aSoapVersion);
    }

TInt CSenVtcpTransportProperties::DownloadFolderL(TPtrC8& aDownloadFolder)
    {
    return CSenTransportProperties::DownloadFolderL(aDownloadFolder);
    }

void CSenVtcpTransportProperties::SetDownloadFolderL(const TDesC8& aDownloadFolder)
    {
    CSenTransportProperties::SetDownloadFolderL(aDownloadFolder);
    }      

TInt CSenVtcpTransportProperties::FileAttachmentL(const TDesC8& aCid, HBufC8*& aFileName)
    {
    return CSenTransportProperties::FileAttachmentL(aCid, aFileName);
    }

TInt CSenVtcpTransportProperties::SetFileAttachmentL(const TDesC8& aCid, const TDesC8& aFileName)
    {
    return CSenTransportProperties::SetFileAttachmentL(aCid, aFileName);
    }
    
MSenProperties* CSenVtcpTransportProperties::CloneL() const
    {
    CSenElement& element = ipFragment->AsElement();
    
    CSenVtcpTransportProperties* pProperties = NewLC(element);
    CleanupStack::Pop(pProperties);
    return pProperties;
    }
        
TInt CSenVtcpTransportProperties::MwsNamespaceL( TPtrC8& aMwsNamespace )
    {
    return CSenTransportProperties::MwsNamespaceL(aMwsNamespace);
    }

void CSenVtcpTransportProperties::SetMwsNamespaceL( const TDesC8& aMwsNamespace )
    {
    CSenTransportProperties::SetMwsNamespaceL(aMwsNamespace);
    }      

TInt CSenVtcpTransportProperties::MessageIdL( TPtrC8& aMessageId )
    {
    return CSenTransportProperties::MessageIdL( aMessageId );
    }      

void CSenVtcpTransportProperties::SetMessageIdL( const TDesC8& aMessageId )
    {
    CSenTransportProperties::SetMessageIdL( aMessageId );
    }
    
EXPORT_C TInt CSenVtcpTransportProperties::OnewayMessageOnOffL( TBool& aOnewayMessageOnOff )
    {
    return CSenTransportProperties::OnewayMessageOnOffL( aOnewayMessageOnOff );
    }

EXPORT_C void CSenVtcpTransportProperties::SetOnewayMessageOnOffL( TBool aOnewayMessageOnOff )
    {
    CSenTransportProperties::SetOnewayMessageOnOffL( aOnewayMessageOnOff );
    }
    
TInt CSenVtcpTransportProperties::HeartbeatL(TInt& aHeartbeat) 
    { 
    return CSenTransportProperties::HeartbeatL( aHeartbeat ); 
    }
    
TInt CSenVtcpTransportProperties::SetHeartbeatL(TInt aHeartbeat) 
    { 
    return CSenTransportProperties::SetHeartbeatL( aHeartbeat ); 
    }
    
EXPORT_C TInt CSenVtcpTransportProperties::SetMaxTimeToLiveL(TInt aMaxTTL) 
    { 
    return CSenTransportProperties::SetMaxTimeToLiveL( aMaxTTL ); 
    }

EXPORT_C TInt CSenVtcpTransportProperties::MaxTimeToLiveL(TInt& aMaxTTL) 
    { 
    return CSenTransportProperties::MaxTimeToLiveL( aMaxTTL ); 
    }

EXPORT_C TInt CSenVtcpTransportProperties::SetMinTimeToLiveL(TInt aMinTTL) 
    { 
    return CSenTransportProperties::SetMinTimeToLiveL( aMinTTL ); 
    }

EXPORT_C TInt CSenVtcpTransportProperties::MinTimeToLiveL(TInt& aMinTTL) 
    { 
    return CSenTransportProperties::MinTimeToLiveL( aMinTTL ); 
    }
    
EXPORT_C TInt CSenVtcpTransportProperties::SetMaxRetryTTLL(TInt aMaxRetryTTL)
	{ 
	return SetIntPropertyL( KSenConnectionRetryMaxTTL, aMaxRetryTTL );
	}
	
EXPORT_C TInt CSenVtcpTransportProperties::MaxRetryTTLL(TInt& aMaxRetryTTL)
	{ 
	return IntPropertyL( KSenConnectionRetryMaxTTL, aMaxRetryTTL ); 
	}

EXPORT_C TInt CSenVtcpTransportProperties::SetMinRetryTTLL(TInt aMinRetryTTL)
	{ 
	return SetIntPropertyL( KSenConnectionRetryMinTTL, aMinRetryTTL );
	}
	
EXPORT_C TInt CSenVtcpTransportProperties:: MinRetryTTLL(TInt& aMinRetryTTL)
	{ 
	return IntPropertyL( KSenConnectionRetryMinTTL, aMinRetryTTL ); 
	}
	
EXPORT_C TInt CSenVtcpTransportProperties::SetRetryDeltaTimeoutL(TInt aTimeout)
	{ 
	return SetIntPropertyL( KSenConnectionRetryDelta, aTimeout );
	}
	
EXPORT_C TInt CSenVtcpTransportProperties::RetryDeltaTimeoutL(TInt& aTimeout)
	{ 
	return IntPropertyL( KSenConnectionRetryDelta, aTimeout ); 
	}


          

// END OF FILE


