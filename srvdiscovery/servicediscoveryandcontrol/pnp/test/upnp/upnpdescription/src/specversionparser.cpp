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

#include "specversionparser.h"
#include "upnpdescriptionschema.h"
#include <upnpdescriptionschema.h>

CSpecVersionParser* CSpecVersionParser::NewL(CUPnPDeviceDescription* aDeviceDesc, const RStringPool& aStringPool)
	{
	CSpecVersionParser* self = new (ELeave) CSpecVersionParser(aDeviceDesc, aStringPool);
	return self;
	}

CSpecVersionParser* CSpecVersionParser::NewL(CUPnPServiceDescription* aServiceDesc, const RStringPool& aStringPool)
	{
	CSpecVersionParser* self = new (ELeave) CSpecVersionParser(aServiceDesc, aStringPool);
	return self;
	}
		
CSpecVersionParser::CSpecVersionParser(CUPnPDeviceDescription* aDeviceDesc, const RStringPool& aStringPool) : iDeviceDesc(aDeviceDesc), iStringPool(aStringPool)
	{
	iType = CSpecVersionParser::EDeviceType; 
	}

CSpecVersionParser::CSpecVersionParser(CUPnPServiceDescription* aServiceDesc, const RStringPool& aStringPool) : iServiceDesc(aServiceDesc), iStringPool(aStringPool)
	{
	iType = CSpecVersionParser::EServiceType; 
	}

CSpecVersionParser::~CSpecVersionParser()
	{
	}

void CSpecVersionParser::ParseStartElementL(const RTagInfo& aElement, const RAttributeArray& /*aAttributes*/)
	{
	iSiblingsCount++;
	iString = iStringPool.OpenStringL(aElement.LocalName().DesC());
	TInt tblIndex = iString.Index ( UPNPDESCRIPTIONXMLTAGS::Table );
	switch( tblIndex )
		{
		case UPNPDESCRIPTIONXMLTAGS::ESpecVersion:
			iFlags = ESpec;
			break;
		case UPNPDESCRIPTIONXMLTAGS::EMajorNumber:
			iFlags = EMajorVersion;
			break;
		case UPNPDESCRIPTIONXMLTAGS::EMinorNumber:
			iFlags = EMinorVersion;
			break;
		default:
			User::Leave(KErrCorrupt);
		}
	
	}
		

void CSpecVersionParser::ParseEndElementL(const RTagInfo& /*aElement*/)
	{
	iSiblingsCount--;
	iString.Close();
	}

void CSpecVersionParser::ParseContentL(const TDesC8& aBytes)
	{
	switch(iFlags)
		{
		case EMajorVersion:
		{
		TInt majorNumber;
		TLex8 data(aBytes);
		data.Val(majorNumber);
		if (iType == CSpecVersionParser::EServiceType)
			iServiceDesc->SetMajorNumberL(majorNumber);
		else
			iDeviceDesc->SetMajorNumberL(majorNumber);
		iFlags = 0;
		break;
		}
	
		case EMinorVersion:
		{
		TInt minorNumber;
		TLex8 data(aBytes);
		data.Val(minorNumber);
		if (iType == CSpecVersionParser::EServiceType)
			iServiceDesc->SetMinorNumberL(minorNumber);
		else
			iDeviceDesc->SetMinorNumberL(minorNumber);
		iFlags = 0;			
		break;
		}
		
		default:
		break;
		}
	}
	
