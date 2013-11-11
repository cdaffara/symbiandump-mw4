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

#ifndef __CACTIONPARSER_H_
#define __CACTIONPARSER_H_

#include <xml/parser.h>
#include "cpnpserviceparam.h"
#include "parsehandlerbase.h"

// Parses the Action element of Service description
class CActionParser : public CParseHandlerBase
	{
public:
	static CActionParser* NewL(CUPnPAction *aAction, const RStringPool &aStringPool);
	virtual ~CActionParser();
	virtual void ParseStartElementL(const RTagInfo& aElement, const RAttributeArray& aAttributes);
	virtual void ParseEndElementL(const RTagInfo& aElement);
	virtual void ParseContentL(const TDesC8& aBytes);

private:
	CActionParser(CUPnPAction *aAction, const RStringPool &aStringPool);
	enum
		{
		EAction = 0x01,
		EName = 0x02,
		EArgumentList = 0x04,
		EAllChild = 0x0E
		};
private:
	CUPnPAction* iAction;
	const RStringPool& iStringPool;
	CParser* iXmlParser;
	TUint iFlags;
	};
#endif /*CACTIONPARSER_H_*/
