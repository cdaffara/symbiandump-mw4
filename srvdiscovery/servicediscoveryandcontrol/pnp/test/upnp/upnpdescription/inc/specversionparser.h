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

#ifndef __SPECVERSIONPARSER_H__
#define __SPECVERSIONPARSER_H__

#include <xml/parser.h>
#include "parsehandlerbase.h"
#include "cpnpdeviceparam.h"
#include "cpnpserviceparam.h"
/* CSpecVersionParser is used to parse the specification element in service and device
   description files
 */
class CSpecVersionParser : public CParseHandlerBase
	{
public:
	
	static CSpecVersionParser* NewL(CUPnPDeviceDescription *aDeviceDesc, const RStringPool& aStringPool);
	static CSpecVersionParser* NewL(CUPnPServiceDescription *aServiceDesc, const RStringPool& aStringPool);
		
	virtual ~CSpecVersionParser();

	virtual void ParseStartElementL(const RTagInfo& aElement, const RAttributeArray& aAttributes);
	virtual void ParseEndElementL(const RTagInfo& aElement);
	virtual void ParseContentL(const TDesC8& aBytes);

private:
	enum
		{
		ESpec			   = 	0x01,
		EMajorVersion 	   = 	0x02,
		EMinorVersion      = 	0x04,
		};
	enum TDescriptionType
		{
		EDeviceType  = 0xFE,
		EServiceType = 0xFF,
		};
	TDescriptionType iType;
	
	CSpecVersionParser(CUPnPDeviceDescription* aDeviceDesc, const RStringPool& aStringPool);
	CSpecVersionParser(CUPnPServiceDescription* aServiceDesc, const RStringPool& aStringPool);
private:
	CUPnPDeviceDescription* 	iDeviceDesc;
	CUPnPServiceDescription* 	iServiceDesc;
	
	const RStringPool& iStringPool;
	CParser* iXmlParser;
	TUint iFlags;
	};

#endif//__SPECVERSIONPARSER_H__
