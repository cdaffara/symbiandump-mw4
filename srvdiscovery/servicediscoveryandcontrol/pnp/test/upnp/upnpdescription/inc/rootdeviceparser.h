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
// rootinfoparser.h
// 
//

#ifndef __ROOTINFOPARSER_H__
#define __ROOTINFOPARSER_H__

#include <xml/parser.h>
#include "parsehandlerbase.h"
#include "cpnpdeviceparam.h"
/* CRootDeviceParser parses the root device information. It calls suitable parsers where necessary
 to parse child elements
 */
class CRootDeviceParser : public CParseHandlerBase
	{
public:
	static CRootDeviceParser* NewL(CUPnPDeviceDescription *aDeviceParser, const RStringPool& aStringPool);
	virtual ~CRootDeviceParser();

	void StartParseL(const TDesC8& aFragment);
	virtual void ParseStartElementL(const RTagInfo& aElement, const RAttributeArray& aAttributes);
	virtual void ParseEndElementL(const RTagInfo& aElement);
	virtual void ParseContentL(const TDesC8& aBytes);
	
private:
	void ConstructL();
	CRootDeviceParser(CUPnPDeviceDescription *aDeviceParser, const RStringPool& aStringPool);
	enum
		{
		ERoot = 	0x0001,
		EUrlBase=	0x0002
		};

private:
	CUPnPDeviceDescription *iDeviceDescpn;
	const RStringPool &iStringPool;
	CParser* iXmlParser;
	TUint iFlags;
	};

#endif//__ROOTINFOPARSER_H__
