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

#ifndef __SERVICEINFOPARSER_H__
#define __SERVICEINFOPARSER_H__

#include <xml/parser.h>
#include "parsehandlerbase.h"
#include "cpnpdeviceparam.h"

/* CServiceInfoParser is used to parse the service element of device description
 It extracts information about the following elements
 1. ServiceType
 2. ServiceId
 3. ScpdUrl
 4. ContorlUrl
 5. EventSubUrl
 Please consult UPnPDeviceArchitecture 1.0
 */ 
class CServiceInfoParser : public CParseHandlerBase
	{
public:
	static CServiceInfoParser* NewL(CUPnPServiceInfo *aServiceInfo, const RStringPool &aStringPool);
	virtual ~CServiceInfoParser();

	virtual void ParseStartElementL(const RTagInfo& aElement, const RAttributeArray& aAttributes);
	virtual void ParseEndElementL(const RTagInfo& aElement);
	virtual void ParseContentL(const TDesC8& aBytes);

private:
	CServiceInfoParser(CUPnPServiceInfo *aServiceInfo, const RStringPool &aStringPool);
	
private:
	CUPnPServiceInfo *iServiceInfo;
	const RStringPool &iStringPool;
	CParser* iXmlParser;
	TUint iFlags;
	};

#endif//__SERVICEINFOPARSER_H__


