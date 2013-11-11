/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*    
*
*/



// disable "identifier was truncated to '255' characters in the browser information" warning
#pragma warning(disable: 4786 4250 4503 4541)


#include "MLAttributesParse.h"
#include "MLCompDataParse.h" // for converting zoom strings

#include <AknDef.hrh> // for logical font ids

#include <sstream>
#include <fstream>
#include <algorithm>
#include <iostream> // !!! for debug output only

extern string whiteSpace;


// const data
//

const string KAttributesParseRoot = "LayoutAttributes";
const string KAttributesParseRootAttributeCreatedOn = "createdOn";

const string KAttributesParseNodeAttributeSet = "attributeset";
const string KAttributesParseNodeAttributeSetAttributeId = "compId";
const string KAttributesParseNodeAttributeSetAttributeName = "name";

const string KAttributesParseNodeAttribute = "attribute";
const string KAttributesParseNodeAttributeAttributeId = "id";
const string KAttributesParseNodeAttributeAttributeName = "name";

const string KAttributesParseNodeCalc = "calc";
const string KAttributesParseNodeCalcAttributeValue = "value";
const string KAttributesParseNodeCalcAttributeZoom = "zoom";


//
// class TMLAttributeSetParse
//
TMLAttributeSetParse::TMLAttributeSetParse(TMLAttributes* aAttributes)
	:
	TMLAttributeSet(aAttributes),
	iSaxZoomLevels(0)
	{

	}

MSaxLayoutAttributesHandler* TMLAttributeSetParse::HandleSax(const std::string& aElement, const TAttribs& aAttribs)
	{
	//	<attributeset compId="1" name="a0">
	if (aElement == KAttributesParseNodeAttributeSet)	
		{
		iCompId = CdlTkUtil::ParseInt(aAttribs.getValue(KAttributesParseNodeAttributeSetAttributeId));
		iName = aAttribs.getValue(KAttributesParseNodeAttributeSetAttributeName);
		}
	else if(aElement == KAttributesParseNodeAttribute)
		{
		HandleSaxAttribute(aAttribs);
		}
	else if(aElement == KAttributesParseNodeCalc)
		{
		HandleSaxCalc(aAttribs);
		}
	return this;
	}

void TMLAttributeSetParse::HandleSaxEnd(const std::string& aElement)
	{
	if (aElement == KAttributesParseNodeAttribute)
		{
		insert(make_pair(iId, *iSaxZoomLevels));
		delete iSaxZoomLevels;
		iSaxZoomLevels = 0;
		}
	}

void TMLAttributeSetParse::HandleSaxAttribute(const TAttribs& aAttribs)
	{
	delete iSaxZoomLevels;
	iSaxZoomLevels = 0;
	iSaxZoomLevels = new TMLAttributeZoomLevels;

	//    <attribute id="8" name="style 1">
	iId = CdlTkUtil::ParseInt(aAttribs.getValue(KAttributesParseNodeAttributeAttributeId));
	string attributeName = CdlTkUtil::ToCpp(aAttribs.getValue(KAttributesParseNodeAttributeAttributeName));
	iAttributes->iNames.insert(make_pair(attributeName, iId));
	}

void TMLAttributeSetParse::HandleSaxCalc(const TAttribs& aAttribs)
	{
	//      <calc value="32"/>
	//      <calc value="28" zoom="-2 zoom"/>
	//      <calc value="38" zoom="+2 zoom"/>
	//      <calc value="30" zoom="-1 zoom"/>
	//      <calc value="36" zoom="+1 zoom"/>

	string zoomStr = aAttribs.getValue(KAttributesParseNodeCalcAttributeZoom);
	int zoom = TMLCompDataParseValues::ConvertZoomStr(zoomStr);
	if(zoom != EAknUiZoomAutomatic)
		{
		string& calc = (*iSaxZoomLevels)[zoom];
		calc = aAttribs.getValue(KAttributesParseNodeCalcAttributeValue);
		}
	}

int TMLAttributeSetParse::CompId() const 
	{
	return iCompId;
	}

string TMLAttributeSetParse::Name() const
	{
	return iName;
	}


//
// class TMLAttributesParse
//
auto_ptr<TMLAttributesParse> TMLAttributesParse::Parse(const string& aLayName)
	{
	auto_ptr<TMLAttributesParse> layout(new TMLAttributesParse);

	string layName = aLayName;

	if (layName.size() >= 2 && layName.substr(0,2) == "-m")
		{
		layName = layName.substr(2);
		layout->iCanBeMirror = true;
		}
	layout->iName = layName;

	// SAX parsing method
	cout << "reading MLAttributes(SAX) " << layName << endl;
	TLayoutAttributesSaxParser saxParser(layout.get());
	saxParser.Parse(layName);

	return layout;
	}

TMLAttributesParse::TMLAttributesParse()
	:
	iSaxAttributeSet(0)
	{

	}

MSaxLayoutAttributesHandler* TMLAttributesParse::HandleSax(const std::string& aElement, const TAttribs& aAttribs)
	{
	if (aElement == KAttributesParseRoot)
		{
		// <LayoutAttributes 
		//		createdOn="2005-05-20 00:55:40.185" 
		//		LayoutToolVer="2005-05-12 04:10 PM" 
		//		MasterId="0.6921563014440754">
		iTimestamp = aAttribs.getValue(KAttributesParseRootAttributeCreatedOn);
		return this;
		}
	else if (aElement == KAttributesParseNodeAttributeSet)
		{
		delete iSaxAttributeSet;
		iSaxAttributeSet = 0;
		iSaxAttributeSet = new TMLAttributeSetParse(this);
		iSaxAttributeSet->HandleSax(aElement, aAttribs);
		return iSaxAttributeSet;
		}
	else
		return this;
	}

void TMLAttributesParse::HandleSaxEnd(const std::string& aElement)
	{
	if (aElement == KAttributesParseNodeAttributeSet)
		{
		TMLAttributeSetComponent& component = (*this)[iSaxAttributeSet->CompId()];
		component.insert(make_pair(iSaxAttributeSet->Name(), iSaxAttributeSet));
		iSaxAttributeSet = 0;
		}
	}


//
// TLayoutAttributesSaxParser
//

TLayoutAttributesSaxParser::TLayoutAttributesSaxParser(MSaxLayoutAttributesHandler* aHandler)
	{ 
	iStack.push(aHandler);
	}

void TLayoutAttributesSaxParser::Parse(const std::string& aFileName)
	{
	Arabica::SAX::InputSource<std::string> is(aFileName);
	Arabica::SAX::XMLReader<std::string> parser;
	parser.setContentHandler(*this);
	parser.setErrorHandler(*this);
	parser.parse(is);
	}

void TLayoutAttributesSaxParser::startElement(const std::string& /*namespaceURI*/, const std::string& localName,
                              const std::string& /*qName*/, const Arabica::SAX::Attributes<std::string>& atts)
    {
	MSaxLayoutAttributesHandler* handler = iStack.top();
	if (!handler)
		throw GeneralErr("SAX: No element handler");
	MSaxLayoutAttributesHandler* next = handler->HandleSax(localName, atts);
	iStack.push(next);
    }

void TLayoutAttributesSaxParser::endElement(const std::string& /*namespaceURI*/, const std::string& localName,
                            const std::string& /*qName*/)
    {
	iStack.pop();
	MSaxLayoutAttributesHandler* handler = iStack.top();
	if (handler)
		handler->HandleSaxEnd(localName);
    }


void TLayoutAttributesSaxParser::warning(const TException& aException) 
    {
	cerr << aException.what();
    }

void TLayoutAttributesSaxParser::error(const TException& aException) 
    {
	cerr << aException.what();
    }

void TLayoutAttributesSaxParser::fatalError(const TException& aException)
    {
	cerr << aException.what();
    }



// End of File
