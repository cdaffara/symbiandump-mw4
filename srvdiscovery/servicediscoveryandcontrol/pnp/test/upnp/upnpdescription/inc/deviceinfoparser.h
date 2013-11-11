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

#ifndef __DEVICEINFOPARSER_H__
#define __DEVICEINFOPARSER_H__

#include <xml/parser.h>
#include "parsehandlerbase.h"
#include "cpnpdeviceparam.h"

/* CDeviceInfoParser parses the device description file.
   Whatever fields are present in the xml device description tree after <device> tag are parsed here
   It calls suitable classes to parse child elements like icon,service etc.
   
   Also see CUPnPDeviceIconParser and CUPnPServiceInfoParser
   Please consult UPnPDeviceArchitecture 1.0
 */
class CDeviceInfoParser : public CParseHandlerBase
		{
	public:
		static CDeviceInfoParser* NewL(CUPnPDevice *aDeviceInfo, const RStringPool& aStringPool);
		virtual ~CDeviceInfoParser();

		virtual void ParseStartElementL(const RTagInfo& aElement, const RAttributeArray& aAttributes);
		virtual void ParseEndElementL(const RTagInfo& aElement);
		virtual void ParseContentL(const TDesC8& aBytes);
		virtual void ParsePrefixL(const RString& aPrefix, const RString& aUri);
	private:
		CDeviceInfoParser(CUPnPDevice *aDeviceInfo, const RStringPool& aStringPool);
		enum
			{
			EDevice  = 			0x001,
			EDeviceType =		0x002,
			EDeviceList = 		0x004,
			EEmbeddedDevice = 	0x005, //Embedded device (EDeviceList | EDevice)
			};
	private:
		CUPnPDevice *iDeviceInfo;
		const RStringPool& iStringPool;
		CParser* iXmlParser;
		TUint iFlags;
		};
		
#endif//__DEVICEINFOPARSER_H__
