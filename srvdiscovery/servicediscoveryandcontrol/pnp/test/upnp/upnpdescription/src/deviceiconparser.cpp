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

#include "deviceiconparser.h"
#include "upnpdescriptionschema.h"


CDeviceIconParser* CDeviceIconParser::NewL(CUPnPIconInfo *aDeviceIcon, const RStringPool &aStringPool)
	{
	CDeviceIconParser* self = new (ELeave) CDeviceIconParser(aDeviceIcon,aStringPool);
	return self;
	}

		
CDeviceIconParser::CDeviceIconParser(CUPnPIconInfo *aDeviceIcon, const RStringPool &aStringPool) : iDeviceIcon(aDeviceIcon), iStringPool(aStringPool)
	{
	}

CDeviceIconParser::~CDeviceIconParser()
	{
	}

void CDeviceIconParser::ParseStartElementL(const RTagInfo& aElement, const RAttributeArray& /*aAttributes*/)
	{
	iSiblingsCount++;
	iString = iStringPool.OpenStringL(aElement.LocalName().DesC());
	}
		

void CDeviceIconParser::ParseEndElementL(const RTagInfo& /*aElement*/)
	{
	iSiblingsCount--;
	iDeviceIcon->SetPropertyStatus(iString,ETrue);
	iString.Close();
	}

void CDeviceIconParser::ParseContentL(const TDesC8& aBytes)
	{
	if ( !IsBlankSpace( aBytes ) )
		iDeviceIcon->SetPropertyL(iString,aBytes);
	}
	
