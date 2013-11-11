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
// rootinfoparser.cpp
// 
//

#include "rootdeviceparser.h"
#include <upnpdescriptionschema.h>
#include "specversionparser.h"
#include "deviceinfoparser.h"
#include <xml/parserfeature.h>

_LIT8(KXmlMimeType1, "text/xml");

CRootDeviceParser* CRootDeviceParser::NewL(CUPnPDeviceDescription *aDeviceParser, const RStringPool& aStringPool)
	{
	CRootDeviceParser* self = new (ELeave) CRootDeviceParser(aDeviceParser, aStringPool);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CRootDeviceParser::CRootDeviceParser(CUPnPDeviceDescription *aDeviceDescpn, const RStringPool& aStringPool) 
	: iDeviceDescpn(aDeviceDescpn), iStringPool(aStringPool)
	{
	}

void CRootDeviceParser::ConstructL()	
	{
	iXmlParser = CParser::NewL(KXmlMimeType1(), *this);
	iXmlParser->EnableFeature(EReportNamespaceMapping );
	iXmlParser->EnableFeature(ESendFullContentInOneChunk);
	}

CRootDeviceParser::~CRootDeviceParser()
	{
	delete iXmlParser;
	}

void CRootDeviceParser::StartParseL(const TDesC8& aFragment)
	{
	ParseL(*iXmlParser, aFragment); 
	}

void CRootDeviceParser::ParseStartElementL(const RTagInfo& aElement, const RAttributeArray& aAttributes)
	{
	iSiblingsCount++;
	iString = iStringPool.OpenStringL(aElement.LocalName().DesC());
	
	TInt tblIndex = iString.Index ( UPNPDESCRIPTIONXMLTAGS::Table );
	// If string lies in the main string table
	if (tblIndex == UPNPDESCRIPTIONXMLTAGS::ESpecVersion)
		{
		iSiblingsCount--;
		CSpecVersionParser* specParser = CSpecVersionParser::NewL(iDeviceDescpn, iStringPool);
		CleanupStack::PushL(specParser);
		StartChildParserL(specParser, aElement, aAttributes);
		CleanupStack::Pop(specParser);
		}
	if ( tblIndex == UPNPDESCRIPTIONXMLTAGS::EDevice)
		{
		iSiblingsCount--;
		CUPnPDevice *deviceInfo = CUPnPDevice::NewL();
		CleanupStack::PushL( deviceInfo );
		iDeviceDescpn->SetDeviceObjectL(deviceInfo);
		CleanupStack::Pop(deviceInfo);
		CDeviceInfoParser* deviceParser = CDeviceInfoParser::NewL(deviceInfo, iStringPool);
		CleanupStack::PushL(deviceParser);
		StartChildParserL(deviceParser, aElement, aAttributes);
		CleanupStack::Pop(deviceParser);
		}
	
	}
		
void CRootDeviceParser::ParseEndElementL(const RTagInfo& /*aElement*/)
	{
	iSiblingsCount--;
	iString.Close();
	}

void CRootDeviceParser::ParseContentL(const TDesC8& aBytes)
	{
	if ( !IsBlankSpace( aBytes ) )
		iDeviceDescpn->SetPropertyL(iString,aBytes);
	}
