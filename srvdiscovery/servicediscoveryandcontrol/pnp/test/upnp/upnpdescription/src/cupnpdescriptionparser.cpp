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
// cupnpserviceparser.cpp
// 
//

#include <upnpdescriptionschema.h>
#include "cupnpdescriptionparser.h"
#include "inetprottextutils.h"
#include "rootserviceparser.h"
#include "rootdeviceparser.h"
#include "parsehandlerbase.h"

using namespace Xml;

/**
Allocates and constructs a CUPnPDescriptionParser object.
Initialises all member data to their default values.
*/
EXPORT_C CUPnPDescriptionParser* CUPnPDescriptionParser::NewL(const RStringPool& aStringPool ,  TUPnPDescriptionType aType)
	{
    CUPnPDescriptionParser* self = new (ELeave) CUPnPDescriptionParser(aStringPool ,aType);
    return self;
    }

CUPnPDescriptionParser::CUPnPDescriptionParser( const RStringPool& aStringPool ,  TUPnPDescriptionType aType)
	:iStringPool ( aStringPool )
    {
    iDescriptionType = aType;
	}

/**
Passes an XML to the framework for parsing from buffer
*/
EXPORT_C TAny* CUPnPDescriptionParser::ParseDescriptionBufL(const TDesC8 &aDocument)
	{
	if( iDescriptionType == CUPnPDescriptionParser::EService )
		{
		iServiceDescObj = CUPnPServiceDescription::NewL();
		CRootServiceParser* parser = CRootServiceParser::NewL(iServiceDescObj, iStringPool);
		CleanupStack::PushL(parser);
		parser->StartParseL(aDocument);
		CleanupStack::PopAndDestroy(parser);
		return iServiceDescObj;
		}
	else
		{
		iDeviceDescObj = CUPnPDeviceDescription::NewL();
		CRootDeviceParser* parser = CRootDeviceParser::NewL(iDeviceDescObj, iStringPool);
		CleanupStack::PushL(parser);
		parser->StartParseL(aDocument);
		CleanupStack::PopAndDestroy(parser);
		return iDeviceDescObj;
		}
	
	}

/**
Destructor
*/
EXPORT_C CUPnPDescriptionParser::~CUPnPDescriptionParser()
    {
    }



