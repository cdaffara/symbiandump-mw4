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

#include "cactionparser.h"
#include <upnpdescriptionschema.h>
#include "cargumentparser.h"

CActionParser* CActionParser::NewL(CUPnPAction* aAction, const RStringPool& aStringPool)
	{
	CActionParser* self = new (ELeave) CActionParser(aAction, aStringPool);
	return self;
	}

CActionParser::CActionParser(CUPnPAction* aAction, const RStringPool& aStringPool) : iAction(aAction), iStringPool(aStringPool)
	{
	}

CActionParser::~CActionParser()
	{
	}

void CActionParser::ParseStartElementL(const RTagInfo& aElement, const RAttributeArray& aAttributes)
	{
	iSiblingsCount++;
	iString = iStringPool.OpenStringL(aElement.LocalName().DesC());
	TInt tblIndex = iString.Index ( UPNPDESCRIPTIONXMLTAGS::Table );
		
	if (tblIndex >= 0)
		{
		if ( tblIndex ==UPNPDESCRIPTIONXMLTAGS::EArgument)
			{
			iSiblingsCount--;
			CUPnPArgument* argument = CUPnPArgument::NewL();
			CleanupStack::PushL(argument);
			iAction->AppendL( argument );
			CleanupStack::Pop( argument );
			CArgumentParser* argumentParser = CArgumentParser::NewL(argument, iStringPool);
			CleanupStack::PushL(argumentParser);
			StartChildParserL( argumentParser, aElement, aAttributes );
			CleanupStack::Pop( argumentParser);
			}
		}
	}
	

void CActionParser::ParseEndElementL(const RTagInfo& /*aElement*/)
	{
	iSiblingsCount--;
	iAction->SetPropertyStatus(iString,ETrue);
	iString.Close();
	}

void CActionParser::ParseContentL(const TDesC8& aBytes)
	{
	if ( !IsBlankSpace( aBytes ) )
		iAction->SetPropertyL(iString, aBytes);
	}
