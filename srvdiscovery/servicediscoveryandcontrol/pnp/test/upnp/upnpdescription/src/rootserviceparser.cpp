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
// rootinfoparser.cpp
// 
//

#include "rootserviceparser.h"
#include <upnpdescriptionschema.h>
#include "specversionparser.h"
#include "cactionparser.h"
#include "cstatetableparser.h"

_LIT8(KXmlMimeType1, "text/xml");

CRootServiceParser* CRootServiceParser::NewL(CUPnPServiceDescription *aServiceParser, const RStringPool& aStringPool)
	{
	CRootServiceParser* self = new (ELeave) CRootServiceParser(aServiceParser, aStringPool);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CRootServiceParser::CRootServiceParser(CUPnPServiceDescription *aServiceDesc, const RStringPool& aStringPool) : iServiceDesc(aServiceDesc), iStringPool(aStringPool)
	{
	}

void CRootServiceParser::ConstructL()	
	{
	iXmlParser = CParser::NewL(KXmlMimeType1(), *this);
	iXmlParser->EnableFeature(EReportNamespaceMapping );
	iXmlParser->EnableFeature(ESendFullContentInOneChunk);
	}

CRootServiceParser::~CRootServiceParser()
	{
	delete iXmlParser;
	}

void CRootServiceParser::StartParseL(const TDesC8& aFragment)
	{
	ParseL(*iXmlParser, aFragment); 
	}

void CRootServiceParser::ParseStartElementL(const RTagInfo& aElement, const RAttributeArray& aAttributes)
	{
	iSiblingsCount++;
	iString = iStringPool.OpenStringL(aElement.LocalName().DesC());
	TInt tblIndex = iString.Index ( UPNPDESCRIPTIONXMLTAGS::Table );
	
	switch(tblIndex)
		{
	case UPNPDESCRIPTIONXMLTAGS::ESpecVersion:
		{
		CSpecVersionParser* specParser = CSpecVersionParser::NewL(iServiceDesc, iStringPool);
		CleanupStack::PushL( specParser );
		StartChildParserL(specParser, aElement, aAttributes);
		CleanupStack::Pop( specParser );
		iSiblingsCount--;
		}
		break;
	
	case UPNPDESCRIPTIONXMLTAGS::EAction:
		{
		CUPnPAction* action = CUPnPAction::NewL();
		iServiceDesc->AppendToActionList( action );
		CActionParser* actionParser = CActionParser::NewL(action, iStringPool);
		CleanupStack::PushL(actionParser);
		StartChildParserL(actionParser, aElement, aAttributes);
		CleanupStack::Pop( actionParser );
		iSiblingsCount--;
		}
		break;
	
	case UPNPDESCRIPTIONXMLTAGS::EStateVariable:
		{
		CUPnPStateVariable* stateVariable = CUPnPStateVariable::NewL();
		CleanupStack::PushL( stateVariable );
		iServiceDesc->AppendToServiceStateTable( stateVariable );
		CleanupStack::Pop( stateVariable );
		CStateTableParser* stateTableParser = CStateTableParser::NewL(stateVariable, iStringPool);
		CleanupStack::PushL( stateTableParser );
		StartChildParserL(stateTableParser , aElement, aAttributes);
		CleanupStack::Pop( stateTableParser );
		iSiblingsCount--;
		}
		break;
		
	//default: Don't do anything
		
		}
	
	}
		
void CRootServiceParser::ParseEndElementL(const RTagInfo& /*aElement*/)
	{
	iSiblingsCount--;
	iString.Close();
	}

void CRootServiceParser::ParseContentL(const TDesC8& /*aBytes*/)
	{
	}


