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

#ifndef __DEVICEICONPARSER_H__
#define __DEVICEICONPARSER_H__

#include <xml/parser.h>
#include "parsehandlerbase.h"
#include "cpnpdeviceparam.h"
/* CDeviceIconParser is used to parse each icon element in device description which contains the following fields
 1. icon
 2. mimetype
 3. width
 4. height
 5. depth
 6. url
 Please consult UPnPDeviceArchitecture 1.0
 */
class CDeviceIconParser : public CParseHandlerBase
	{
public:
	static CDeviceIconParser* NewL(CUPnPIconInfo *aDeviceIcon, const RStringPool &aStringPool);
	virtual ~CDeviceIconParser();

	virtual void ParseStartElementL(const RTagInfo& aElement, const RAttributeArray& aAttributes);
	virtual void ParseEndElementL(const RTagInfo& aElement);
	virtual void ParseContentL(const TDesC8& aBytes);

private:
	CDeviceIconParser(CUPnPIconInfo *aDeviceIcon, const RStringPool &aStringPool);
	
private:
	CUPnPIconInfo *iDeviceIcon;
	const RStringPool &iStringPool;
	CParser* iXmlParser;
	TUint iFlags;
	};

#endif//__DEVICEICONPARSER_H__
