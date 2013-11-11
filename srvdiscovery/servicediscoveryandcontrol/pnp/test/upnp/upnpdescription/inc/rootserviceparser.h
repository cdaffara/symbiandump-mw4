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


#ifndef __ROOTSERVICEPARSER_H_
#define __ROOTSERVICEPARSER_H_
#include <xml/parser.h>
#include "parsehandlerbase.h"
#include "cpnpserviceparam.h"

/* CRootServiceParser parses the root device information. It calls suitable parsers where necessary
 to parse child elements
 */
class CRootServiceParser : public CParseHandlerBase
	{
public:
	static CRootServiceParser* NewL(CUPnPServiceDescription *aServiceParser, const RStringPool& aStringPool);
	virtual ~CRootServiceParser();

	void StartParseL(const TDesC8& aFragment);
	virtual void ParseStartElementL(const RTagInfo& aElement, const RAttributeArray& aAttributes);
	virtual void ParseEndElementL(const RTagInfo& aElement);
	virtual void ParseContentL(const TDesC8& aBytes);
	
private:
	void ConstructL();
	CRootServiceParser(CUPnPServiceDescription* aServiceParser, const RStringPool& aStringPool);
	enum
		{
		EScpd = 0x01,
		EActionList = 0x02,
		EServiceStateTable = 0x04,
		EAllChild = 0x08
		};

private:
	CUPnPServiceDescription* iServiceDesc;
	const RStringPool& iStringPool;
	CParser* iXmlParser;
	TUint iFlags;
	};

#endif /*ROOTSERVICEPARSER_H_*/
