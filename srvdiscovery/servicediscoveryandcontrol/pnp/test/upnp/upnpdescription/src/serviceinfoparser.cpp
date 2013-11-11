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

#include "serviceinfoparser.h"
#include "upnpdescriptionschema.h"

CServiceInfoParser* CServiceInfoParser::NewL(CUPnPServiceInfo *aServiceInfo, const RStringPool &aStringPool)
	{
	CServiceInfoParser* self = new (ELeave) CServiceInfoParser(aServiceInfo, aStringPool);
	return self;
	}

		
CServiceInfoParser::CServiceInfoParser(CUPnPServiceInfo *aServiceInfo, const RStringPool &aStringPool) : iServiceInfo(aServiceInfo), iStringPool(aStringPool)
	{
	}

CServiceInfoParser::~CServiceInfoParser()
	{
	}

void CServiceInfoParser::ParseStartElementL(const RTagInfo& aElement, const RAttributeArray& /*aAttributes*/)
	{
	iSiblingsCount++;
	iString = iStringPool.OpenStringL(aElement.LocalName().DesC());
	
	}

void CServiceInfoParser::ParseEndElementL(const RTagInfo& /*aElement*/)
	{
	iSiblingsCount--;
	iServiceInfo->SetPropertyStatus(iString,ETrue);
	iString.Close();
	}

void CServiceInfoParser::ParseContentL(const TDesC8& aBytes)
	{
	if ( !IsBlankSpace( aBytes ) )
		{
		iServiceInfo->SetPropertyL(iString , aBytes);
		}
	}
