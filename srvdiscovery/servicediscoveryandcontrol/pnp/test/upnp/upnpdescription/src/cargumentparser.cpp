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

#include <upnpdescriptionschema.h>
#include "cargumentparser.h"

CArgumentParser* CArgumentParser::NewL(CUPnPArgument* aArgument, const RStringPool& aStringPool)
	{
	CArgumentParser* self = new (ELeave) CArgumentParser(aArgument, aStringPool);
	return self;
	}

CArgumentParser::CArgumentParser(CUPnPArgument* aArgument, const RStringPool& aStringPool) : iArgument(aArgument), iStringPool(aStringPool)
	{
	}

CArgumentParser::~CArgumentParser()
	{
	}

void CArgumentParser::ParseStartElementL(const RTagInfo& aElement, const RAttributeArray& /* aAttributes */)
	{
	iSiblingsCount++;
	iString = iStringPool.OpenStringL(aElement.LocalName().DesC());
	TInt tblIndex = iString.Index ( UPNPDESCRIPTIONXMLTAGS::Table );
	if ( tblIndex == UPNPDESCRIPTIONXMLTAGS::ERetValue)
		iArgument->SetRetVal( ETrue );
	
	}

void CArgumentParser::ParseEndElementL(const RTagInfo& /*aElement*/)
	{
	iSiblingsCount--;
	iArgument->SetPropertyStatus(iString,ETrue);
	iString.Close();
	}

void CArgumentParser::ParseContentL(const TDesC8& aBytes)
	{
	if ( !IsBlankSpace( aBytes ) )
		iArgument->SetPropertyL( iString , aBytes );
	}
