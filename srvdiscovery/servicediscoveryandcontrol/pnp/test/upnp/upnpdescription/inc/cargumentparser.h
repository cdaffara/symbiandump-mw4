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

#ifndef __CARGUMENTPARSER_H_
#define __CARGUMENTPARSER_H_

#include <xml/parser.h>
#include "cpnpserviceparam.h"
#include "parsehandlerbase.h"

/* Parses the argument element of UPnP Service description
 */ 
class CArgumentParser : public CParseHandlerBase
	{
public:
	static CArgumentParser* NewL(CUPnPArgument *aArgument, const RStringPool &aStringPool);
	virtual ~CArgumentParser();
	virtual void ParseStartElementL(const RTagInfo& aElement, const RAttributeArray& aAttributes);
	virtual void ParseEndElementL(const RTagInfo& aElement);
	virtual void ParseContentL(const TDesC8& aBytes);

private:
	CArgumentParser(CUPnPArgument *aArgument , const RStringPool &aStringPool);
	enum
		{
		EArgument = 0x01,
		EName = 0x02,
		EArgumentDirection = 0x04,
		ERetValue = 0x08,
		ERelatedStateVar = 0x10,
		EAllChild = 0x1E
		};
private:
	CUPnPArgument* iArgument;
	const RStringPool& iStringPool;
	CParser* iXmlParser;
	TUint iFlags;
	};
#endif /*CARGUMENTPARSER_H_*/
