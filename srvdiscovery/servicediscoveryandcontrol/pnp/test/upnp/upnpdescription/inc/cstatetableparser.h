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

#ifndef __CSTATETABLEPARSER_H_
#define __CSTATETABLEPARSER_H_

#include <xml/parser.h>
#include "parsehandlerbase.h"
#include "cpnpserviceparam.h"

// Parser for the state table
class CStateTableParser : public CParseHandlerBase
	{
public:
	static CStateTableParser* NewL(CUPnPStateVariable *aStateTable, const RStringPool &aStringPool);
	virtual ~CStateTableParser();
	virtual void ParseStartElementL(const RTagInfo& aElement, const RAttributeArray& aAttributes);
	virtual void ParseEndElementL(const RTagInfo& aElement);
	virtual void ParseContentL(const TDesC8& aBytes);

private:
	CStateTableParser(CUPnPStateVariable *aStateTable, const RStringPool &aStringPool);
	enum
		{
		EAllowedValue = 0x01,
		};
private:
	CUPnPStateVariable* 	iStateTable;
	CUPnPArgValueList* 		iArgValueList;
	const RStringPool& 		iStringPool;
	CParser* iXmlParser;
	TUint iFlags;
	};

//--------------------------------------------------------------------------
// Parser for the value range object
class CValueRangeParser : public CParseHandlerBase
	{
public:
	static CValueRangeParser* NewL(CUPnPValueRange *aValueRange, const RStringPool &aStringPool);
	virtual ~CValueRangeParser();
	virtual void ParseStartElementL(const RTagInfo& aElement, const RAttributeArray& aAttributes);
	virtual void ParseEndElementL(const RTagInfo& aElement);
	virtual void ParseContentL(const TDesC8& aBytes);

private:
	CValueRangeParser(CUPnPValueRange *aValueRange, const RStringPool &aStringPool);
	enum
		{
		EAllowedValueRange = 0x01,
		EMinimumValue = 0x02,
		EMaximumValue = 0x04,
		EStepValue = 0x08,
		EAllChild = 0x0E
		};
private:
	CUPnPValueRange* iValueRange;
	const RStringPool& iStringPool;
	CParser* iXmlParser;
	TUint iFlags;
	};

#endif /*CSTATETABLEPARSER_H_*/
