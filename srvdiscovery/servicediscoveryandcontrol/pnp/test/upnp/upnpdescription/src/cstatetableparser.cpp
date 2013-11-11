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

#include "cstatetableparser.h"
#include <upnpdescriptionschema.h>

CStateTableParser* CStateTableParser::NewL(CUPnPStateVariable *aStateTable, const RStringPool& aStringPool)
	{
	CStateTableParser* self = new (ELeave) CStateTableParser(aStateTable, aStringPool);
	return self;
	}

CStateTableParser::CStateTableParser(CUPnPStateVariable *aStateTable, const RStringPool& aStringPool) : iStateTable(aStateTable), iStringPool(aStringPool)
	{
	}

CStateTableParser::~CStateTableParser()
	{
	}

void CStateTableParser::ParseStartElementL(const RTagInfo& aElement, const RAttributeArray& aAttributes)
	{
	iSiblingsCount++;
	iString = iStringPool.OpenStringL(aElement.LocalName().DesC());
	TInt tblIndex = iString.Index ( UPNPDESCRIPTIONXMLTAGS::Table );
	const TDesC8& str = iString.DesC();
	switch(tblIndex)
		{
	case UPNPDESCRIPTIONXMLTAGS::EStateVariable:
		{
		_LIT8(KSendEvents,"sendEvents");
		_LIT8(KYes,"yes");
		_LIT8(KNo,"no");
		
		const RAttribute& attribute = aAttributes[0];
		const RTagInfo& nameInfo = attribute.Attribute();

		const TDesC8& localName = nameInfo.LocalName().DesC();
		if ( localName.CompareF( KSendEvents ) )
			User::Leave(KErrCorrupt);
		const TDesC8& value = attribute.Value().DesC();
		if ( !value.CompareF(KYes) )
			iStateTable->SetPropertyL(iStringPool.String(UPNPDESCRIPTIONXMLTAGS::ESendEvents, UPNPDESCRIPTIONXMLTAGS::Table),KYes);
			
		else if ( !value.CompareF(KNo) )
			iStateTable->SetPropertyL(iStringPool.String(UPNPDESCRIPTIONXMLTAGS::ESendEvents, UPNPDESCRIPTIONXMLTAGS::Table),KNo);
		else
			User::Leave(KErrCorrupt);
		
		}
		
	break;
	
	case UPNPDESCRIPTIONXMLTAGS::EAllowedValueList:
		{
		if ( !iArgValueList )
			{
			iArgValueList = CUPnPArgValueList::NewL();
			iArgValueList->iType = CUPnPArgValueList::EArgValueList;
			}
		else
			User::Leave(KErrCorrupt);
		
		iStateTable->SetAllowedValueType(ETrue);
		iStateTable->SetAllowedValuesL(iArgValueList);
		iFlags = iFlags | EAllowedValue;
		}
	break;
		
	case UPNPDESCRIPTIONXMLTAGS::EAllowedValueRange:
		{
		iSiblingsCount--;
		CUPnPValueRange* valueRange = CUPnPValueRange::NewL();
		valueRange->iType = CUPnPValueRange::EArgValueRange ;
		CleanupStack::PushL(valueRange);
		iStateTable->SetAllowedValuesL(valueRange);
		iStateTable->SetAllowedValueType(EFalse);
		CleanupStack::Pop();
		CValueRangeParser* valueParser = CValueRangeParser::NewL(valueRange, iStringPool);
		CleanupStack::PushL(valueParser);
		StartChildParserL(valueParser, aElement, aAttributes);
		CleanupStack::Pop(valueParser);
		}
	break;
		//default : Don't do anything
		}

	}

void CStateTableParser::ParseEndElementL(const RTagInfo& /*aElement*/)
	{
	iSiblingsCount--;
	iStateTable->SetPropertyStatus(iString,ETrue);
	iString.Close();
	}
	
void CStateTableParser::ParseContentL(const TDesC8& aBytes)
	{
	if ( !IsBlankSpace( aBytes ) )
		{
		if ( iFlags == EAllowedValue )
			iArgValueList->AppendL( aBytes );
		else 
			iStateTable->SetPropertyL(iString,aBytes);
		}
	}

//------------------------------------------------------------------------------
CValueRangeParser* CValueRangeParser::NewL(CUPnPValueRange* aValueRange, const RStringPool& aStringPool)
	{
	CValueRangeParser* self = new (ELeave) CValueRangeParser(aValueRange, aStringPool);
	return self;
	}

CValueRangeParser::CValueRangeParser(CUPnPValueRange* aValueRange, const RStringPool& aStringPool) : iValueRange(aValueRange), iStringPool(aStringPool)
	{
	}

CValueRangeParser::~CValueRangeParser()
	{
	}

void CValueRangeParser::ParseStartElementL(const RTagInfo& aElement, const RAttributeArray& /*aAttributes*/)
	{
	iSiblingsCount++;
	iString = iStringPool.OpenStringL(aElement.LocalName().DesC());
	}

void CValueRangeParser::ParseEndElementL(const RTagInfo& /*aElement*/)
	{
	iSiblingsCount--;
	iValueRange->SetPropertyStatus(iString,ETrue);
	iString.Close();
	}

void CValueRangeParser::ParseContentL(const TDesC8& aBytes)
	{
	if ( !IsBlankSpace( aBytes ) )
		iValueRange->SetPropertyL( iString, aBytes );
	
	}



