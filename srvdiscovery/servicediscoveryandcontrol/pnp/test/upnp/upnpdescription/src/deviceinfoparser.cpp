// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include "deviceinfoparser.h"
#include "upnpdescriptionschema.h"
#include "deviceiconparser.h"
#include "serviceinfoparser.h"
_LIT8(KDlnaNameSpace,"urn:schemas-dlna-org:device-1-0");

CDeviceInfoParser* CDeviceInfoParser::NewL(CUPnPDevice *aDeviceInfo, const RStringPool& aStringPool)
	{
	CDeviceInfoParser* self = new (ELeave) CDeviceInfoParser(aDeviceInfo, aStringPool);
	return self;
	}

CDeviceInfoParser::CDeviceInfoParser(CUPnPDevice *aDeviceInfo, const RStringPool& aStringPool) : iDeviceInfo(aDeviceInfo), iStringPool(aStringPool)
	{
	}

CDeviceInfoParser::~CDeviceInfoParser()
	{
	}

void CDeviceInfoParser::ParseStartElementL(const RTagInfo& aElement, const RAttributeArray& aAttributes)
	{
	iSiblingsCount++;
	iString = iStringPool.OpenStringL(aElement.LocalName().DesC());
	const TDesC8& info = iString.DesC();
	TInt tblIndex = iString.Index ( UPNPDESCRIPTIONXMLTAGS::Table );
	switch(tblIndex)
		{
	case UPNPDESCRIPTIONXMLTAGS::EDevice:
		{
		if(!(iFlags & EDevice))
			{		
			iFlags = iFlags | EDevice;
			}
		else if(iFlags & EEmbeddedDevice)
			{
			iSiblingsCount--;
			CUPnPDevice* deviceInfo = CUPnPDevice::NewL();
			CleanupStack::PushL( deviceInfo );
			iDeviceInfo->AppendToEmbeddedDeviceInfoTableL(deviceInfo);
			CleanupStack::Pop( deviceInfo );
			CDeviceInfoParser* deviceParser = CDeviceInfoParser::NewL(deviceInfo, iStringPool);
			CleanupStack::PushL(deviceParser);
			StartChildParserL(deviceParser, aElement, aAttributes);
			CleanupStack::Pop( deviceParser );
			}
		}
		break;
	case UPNPDESCRIPTIONXMLTAGS::EIcon:
		{
		iSiblingsCount--;
		CUPnPIconInfo *iconInfo = CUPnPIconInfo::NewL();
		CleanupStack::PushL( iconInfo );
		iDeviceInfo->AppendToIconInfoTableL(iconInfo);
		CleanupStack::Pop();
		CDeviceIconParser* deviceIconParser = CDeviceIconParser::NewL(iconInfo, iStringPool);
		CleanupStack::PushL( deviceIconParser );
		StartChildParserL(deviceIconParser, aElement, aAttributes);
		CleanupStack::Pop(deviceIconParser);
		}
		break;
	case UPNPDESCRIPTIONXMLTAGS::EService:
		{
		iSiblingsCount--;
		CUPnPServiceInfo *serviceInfo = CUPnPServiceInfo::NewL();
		CleanupStack::PushL(serviceInfo);
		iDeviceInfo->AppendToServiceInfoTableL(serviceInfo);
		CleanupStack::Pop( serviceInfo );
		CServiceInfoParser* serviceInfoParser = CServiceInfoParser::NewL(serviceInfo, iStringPool);
		CleanupStack::PushL(serviceInfoParser);
		StartChildParserL(serviceInfoParser, aElement, aAttributes);
		CleanupStack::Pop(serviceInfoParser);
		}
		break;
	
		// Default: Don't do anything
	}
	
	}
		

void CDeviceInfoParser::ParseEndElementL(const RTagInfo& /*aElement*/)
	{
	iSiblingsCount--;
	iDeviceInfo->SetPropertyStatus(iString,ETrue);
	iString.Close();
	}

void CDeviceInfoParser::ParseContentL(const TDesC8& aBytes)
	{
	if ( !IsBlankSpace( aBytes ) )
		iDeviceInfo->SetPropertyL(iString,aBytes);
	}
	
void CDeviceInfoParser::ParsePrefixL(const RString& aPrefix, const RString& aUri)
	{
	_LIT8(KDlna,"dlna");
	const TDesC8& prefix = aPrefix.DesC();
	if ( prefix == KDlna )
		{
		const TDesC8& uri = aUri.DesC();
		if ( uri == KDlnaNameSpace)
			{
			iDeviceInfo->SetDlna( ETrue );
			}
		else
			User::Leave(KErrCorrupt);
		}
	}
