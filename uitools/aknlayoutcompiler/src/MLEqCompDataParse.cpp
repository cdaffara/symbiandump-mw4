/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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


#include "MLEqCompDataParse.h"

#include <AknDef.hrh>

#include <sstream>
#include <fstream>
#include <algorithm>
#include <iostream> // !!! for debug output only

extern string whiteSpace;


// 
// const data
//

const string KEqListParseRoot = "MasterLayout";

const string KEqListParseComponent = "Component";
const string KEqListParseComponentId = "compId";
const string KEqListParseComponentLevel = "level";
const string KEqListParseComponentName = "name";
const string KEqListParseComponentPureSection = "pureSection";
const string KEqListParseComponentType = "type";


const string KEqCompDataParseRoot = "Layout";

const string KEqCompDataParseRootAttributeMasterName = "MasterName";
const string KEqCompDataParseRootAttributeLayoutName = "LayoutName";
const string KEqCompDataParseRootAttributeVariantName = "variant_name";
const string KEqCompDataParseRootAttributeResolutionHeight = "resolutionHeight";
const string KEqCompDataParseRootAttributeResolutionWidth = "resolutionWidth";
const string KEqCompDataParseRootAttributeCreatedOn = "createdOn";

const string KEqCompDataParseNodeComponent = "Component";
const string KEqCompDataParseNodeComponentAttributeName = "name";
const string KEqCompDataParseNodeComponentAttributeId = "id";
const string KEqCompDataParseNodeComponentAttributeType = "type";
const string KEqCompDataParseNodeComponentAttributeDrawingOrder = "drawingorder";
const string KEqCompDataParseNodeComponentAttributeMirror = "mirror";
const string KEqCompDataParseNodeComponentAttributeMaxVariety = "maxVariety";

const string KEqCompDataParseTextComponentTypeScreen= "0"; // "Screen";
const string KEqCompDataParseTextComponentTypeContainer= "1"; // "Container";
const string KEqCompDataParseTextComponentTypePane= "2"; // "Pane";
const string KEqCompDataParseTextComponentTypeGraphic= "3"; // "Graphic";
const string KEqCompDataParseTextComponentTypeText= "4"; // "Text";

const string KEqCompDataParseTextComponentMirrorTrue= "true";

const string KEqCompDataParseNodeOption = "option";
const string KEqCompDataParseNodeParentInfo = "ParentInfo";
const string KEqCompDataParseNodeLayoutInfo = "LayoutInfo";
const string KEqCompDataParseNodeAttributeInfo = "AttributeInfo";
const string KEqCompDataParseNodeAttrSets = "attrsets";

const string KEqCompDataParseNodeParent = "parent";
const string KEqCompDataParseNodeParentOption = "option";
const string KEqCompDataParseNodeParentOptionAttributeVariety = "variety";
const string KEqCompDataParseNodeParentInfoParentAttributeId = "id";
const string KEqCompDataParseNodeParentInfoParentAttributeVariety = "variety";

const string KEqCompDataParseNodeLayoutInfoParam = "Param";
const string KEqCompDataParseNodeLayoutInfoParamAttributeName = "name";

const string KEqCompDataParseNodeValue = "Value";
const string KEqCompDataParseNodeVarietyIndex = "varietyIndex";
const string KEqCompDataParseNodeVarietyIndexValue = "value";
const string KEqCompDataParseNodeOptionSetNo = "optionSetNo";
const string KEqCompDataParseNodeOptionSetNoValue = "value";
const string KEqCompDataParseNodeCalc = "calc";
const string KEqCompDataParseNodeFormulae = "Formulae";
const string KEqCompDataParseNodeFormula = "Formula";
const string KEqCompDataParseNodeFormulaFormulaStr = "formulaStr";
const string KEqCompDataParseNodeFormulaZoomId = "zId";

const string KEqCompDataParseNodeCalcAttributeValue = "value";
const string KEqCompDataParseNodeCalcAttributeColNumber = "colNumber";
const string KEqCompDataParseNodeCalcAttributeRowNumber = "rowNumber";
const string KEqCompDataParseNodeCalcAttributeShowInTable = "showInTable";
const string KEqCompDataParseNodeCalcAttributeShowInTableTrue = "true";
const string KEqCompDataParseNodeCalcAttributeShowInTableFalse = "false";
const string KEqCompDataParseNodeCalcAttributeZoom = "zoom";
const string KEqCompDataParseNodeCalcAttributeZoomNormal = "normal";
const string KEqCompDataParseNodeCalcAttributeZoomVerySmall = "-2 zoom";
const string KEqCompDataParseNodeCalcAttributeZoomSmall = "-1 zoom";
const string KEqCompDataParseNodeCalcAttributeZoomLarge = "+1 zoom";
const string KEqCompDataParseNodeCalcAttributeZoomVeryLarge = "+2 zoom";

const string KEqCompDataParseTextFontPrimary = "qfn_primary";
const string KEqCompDataParseTextFontSecondary = "qfn_secondary";
const string KEqCompDataParseTextFontTitle = "qfn_title";
const string KEqCompDataParseTextFontPrimarySmall = "qfn_primary_small"; // is that the correct text?
const string KEqCompDataParseTextFontDigital = "qfn_digital";

const string KEqCompDataParseTextParamNameLeft = "Left";
const string KEqCompDataParseTextParamShortNameLeft = "l";
const string KEqCompDataParseTextParamNameRight = "Right";
const string KEqCompDataParseTextParamShortNameRight = "r";
const string KEqCompDataParseTextParamNameTop = "Top";
const string KEqCompDataParseTextParamShortNameTop = "t";
const string KEqCompDataParseTextParamNameBottom = "Bottom";
const string KEqCompDataParseTextParamShortNameBottom = "b";
const string KEqCompDataParseTextParamNameWidth = "Width";
const string KEqCompDataParseTextParamShortNameWidth = "w";
const string KEqCompDataParseTextParamNameHeight = "Height";
const string KEqCompDataParseTextParamShortNameHeight = "h";
const string KEqCompDataParseTextParamNameJustification = "Justification";
const string KEqCompDataParseTextParamShortNameJustification = "j";
const string KEqCompDataParseTextParamNameType = "Type";
const string KEqCompDataParseTextParamShortNameType = "Type";

// these have been changed to lower case, so that matching
// can be performed independent of case
const string KEqCompDataParseTextParamTypeCalcValueNone = "none";
const string KEqCompDataParseTextParamTypeCalcValueLeft = "left";
const string KEqCompDataParseTextParamTypeCalcValueCenter = "center";
const string KEqCompDataParseTextParamTypeCalcValueRight = "right";
const string KEqCompDataParseTextParamTypeCalcValueBidi = "bidi";

const string KEqCompDataParseNodeAttributeInfoOption = "option";
const string KEqCompDataParseNodeAttributeInfoOptionAttributeVariety = "variety";
const string KEqCompDataParseNodeAttributeInfoAttributeSet = "attributeset";
const string KEqCompDataParseNodeAttributeInfoAttributeSetAttributeName = "name";

const string KEqParChildParseRoot = "ParChildRelation";
const string KEqParChildParseComponent = "Component";
const string KEqParChildParseComponentId = "id";
const string KEqParChildParseComponentParent = "parent";
const string KEqParChildParseComponentParentId = "parId";

//
// class TMLEqCompDataParseFormula
//
TMLEqCompDataParseFormula::TMLEqCompDataParseFormula()
	: 
	TMLEqCompDataFormula(),
	iSaxFormulaString()
	{

	};

MSaxLayoutEqHandler* TMLEqCompDataParseFormula::HandleSax(const std::string& aElement, const TAttribs& aAttribs)
	{
	if(aElement == KEqCompDataParseNodeFormula)
		{
		HandleSaxFormulaAttributes(aElement, aAttribs);
		}
	return this;
	};

void TMLEqCompDataParseFormula::HandleSaxEnd(const std::string& aElement)
	{

	};

void TMLEqCompDataParseFormula::HandleSaxFormulaAttributes(const std::string& aElement, const TAttribs& aAttribs)
	{
//	<Formula 
//			formulaStr="3301_l-(pr199+pr197)" 
//			lowerMargin="-1.0" 
//			maxLimit="-1.0" 
//			minLimit="-1.0" 
//			prefNo="1" 
//			sizer="-1" 
//			upperMargin="-1.0" 
//			zId="232" />

	iFormulaString = aAttribs.getValue(KEqCompDataParseNodeFormulaFormulaStr);

	string zoomIdStr = aAttribs.getValue(KEqCompDataParseNodeFormulaZoomId);
	if(!zoomIdStr.empty())
		iZoomId = CdlTkUtil::ParseInt(zoomIdStr);
	else
		iZoomId = -1; // @todo maybe could use dummy = 252?
	}


//
// class TMLEqCompDataParseValuesOptionSet
//
TMLEqCompDataParseValuesOptionSet::TMLEqCompDataParseValuesOptionSet()
	:
	TMLEqCompDataValuesOptionSet(),
	iSaxFormula(0),
	iSaxFormulaNo(0),
	iSaxOptionSetNo(-1)
	{

	};

MSaxLayoutEqHandler* TMLEqCompDataParseValuesOptionSet::HandleSax(const std::string& aElement, const TAttribs& aAttribs)
	{
//	<Value>
//		<optionIndex value="0"/>
//		<varietyIndex variety="0"/>
//		<optionSetName orientation="3" value="Option0 (lists)"/>
//		<optionSetNo value="0"/>
//		<Formulae>
//		...
//		</Formulae>
//		<Comment value=""/>
//	</Value>

	if (aElement == KEqCompDataParseNodeOptionSetNo)
		{
		HandleSaxOptionSetNo(aElement, aAttribs);
		}
	else if (aElement == KEqCompDataParseNodeFormulae)
		{
		return HandleSaxFormula(aElement, aAttribs);
		}

	return this;
	};

void TMLEqCompDataParseValuesOptionSet::HandleSaxEnd(const std::string& aElement)
	{
	if(aElement == KEqCompDataParseNodeFormulae)
		{
		push_back(*iSaxFormula);
		delete iSaxFormula;
		iSaxFormula = 0;
		}
	};

void TMLEqCompDataParseValuesOptionSet::HandleSaxOptionSetNo(const std::string& /*aElement*/, const TAttribs& aAttribs)
	{
//	<optionSetNo value="1"/>

	string optionSetNoStr = aAttribs.getValue(KEqCompDataParseNodeOptionSetNoValue);
	iSaxOptionSetNo = CdlTkUtil::ParseInt(optionSetNoStr); // defaults to zero
	}


MSaxLayoutEqHandler* TMLEqCompDataParseValuesOptionSet::HandleSaxFormula(const std::string& /*aElement*/, const TAttribs& aAttribs)
	{
//	<Formula ... />

	delete iSaxFormula;
	iSaxFormula = 0;
	iSaxFormula = new TMLEqCompDataParseFormula();
	return iSaxFormula;
	}



//
//  TMLEqCompDataParseValues
//

TMLEqCompDataParseValues::TMLEqCompDataParseValues(TMLEqCompDataLine* aLine)
	: 
	TMLEqCompDataValues(aLine), 
	iSaxOptionSet(0)
	{

	}

MSaxLayoutEqHandler* TMLEqCompDataParseValues::HandleSax(const std::string& aElement, const TAttribs& aAttribs)
	{

//	<Param name="Left">
//		<Value>
//			...
//		</Value>
//	</Param>

	if (aElement == KEqCompDataParseNodeValue)
		{
		return HandleSaxOptionSet(aElement, aAttribs);
		}

	return this;
	}

void TMLEqCompDataParseValues::HandleSaxEnd(const std::string& aElement)
	{
	if (aElement == KEqCompDataParseNodeValue)
		{
		iOptionSets.insert(make_pair(iSaxOptionSet->OptionSetNo(), *iSaxOptionSet));
		delete iSaxOptionSet;
		iSaxOptionSet = 0;
		}
	}

MSaxLayoutEqHandler* TMLEqCompDataParseValues::HandleSaxOptionSet(const std::string& /*aElement*/, const TAttribs& aAttribs)
	{
	delete iSaxOptionSet;
	iSaxOptionSet = 0;
	iSaxOptionSet = new TMLEqCompDataParseValuesOptionSet;
	return iSaxOptionSet;
	};


void TMLEqCompDataParseValues::ConvertValueStr(std::string& aValueStr)
	{
	int valueInt = 0;
	if(iName == KEqCompDataParseTextParamShortNameType)
		{
		if(aValueStr == KEqCompDataParseTextFontPrimary) 
			valueInt = ELayoutCompilerFontCategoryPrimary;
		else if(aValueStr == KEqCompDataParseTextFontSecondary)
			valueInt = ELayoutCompilerFontCategorySecondary;
		else if(aValueStr == KEqCompDataParseTextFontTitle)
			valueInt = ELayoutCompilerFontCategoryTitle;
		else if(aValueStr == KEqCompDataParseTextFontPrimarySmall) 
			valueInt = ELayoutCompilerFontCategoryPrimarySmall;
		else if(aValueStr == KEqCompDataParseTextFontDigital)
			valueInt = ELayoutCompilerFontCategoryDigital;
		else
			valueInt = ELayoutCompilerFontCategoryUndefined;
		aValueStr = CdlTkUtil::IntToString(valueInt);
		}
	else if(iName == KEqCompDataParseTextParamShortNameJustification)
		{
		// jusitication does not have consistent capitalization
		string lowerValueStr = CdlTkUtil::ToLower(aValueStr);
		if(lowerValueStr == KEqCompDataParseTextParamTypeCalcValueNone) 
			valueInt = ELayoutAlignNone;
		else if(lowerValueStr == KEqCompDataParseTextParamTypeCalcValueLeft) 
			valueInt = ELayoutAlignLeft;
		else if(lowerValueStr == KEqCompDataParseTextParamTypeCalcValueRight) 
			valueInt = ELayoutAlignRight;
		else if(lowerValueStr == KEqCompDataParseTextParamTypeCalcValueCenter) 
			valueInt = ELayoutAlignCenter;
		else if(lowerValueStr == KEqCompDataParseTextParamTypeCalcValueBidi) 
			valueInt = ELayoutAlignBidi;
		else 
			valueInt = ELayoutAlignNone;
		aValueStr = CdlTkUtil::IntToString(valueInt);
		}
	}

int TMLEqCompDataParseValues::ConvertZoomStr(const std::string& aZoomStr)
	{
	int zoomInt = EAknUiZoomAutomatic;
	string lowerZoomStr = CdlTkUtil::ToLower(aZoomStr);
	if(lowerZoomStr.empty())
		zoomInt = EAknUiZoomNormal;
	else if(lowerZoomStr == KEqCompDataParseNodeCalcAttributeZoomVerySmall) 
		zoomInt = EAknUiZoomVerySmall;
	else if(lowerZoomStr == KEqCompDataParseNodeCalcAttributeZoomSmall) 
		zoomInt = EAknUiZoomSmall;
	else if(lowerZoomStr == KEqCompDataParseNodeCalcAttributeZoomLarge) 
		zoomInt = EAknUiZoomLarge;
	else if(lowerZoomStr == KEqCompDataParseNodeCalcAttributeZoomVeryLarge) 
		zoomInt = EAknUiZoomVeryLarge;
	return zoomInt;
	}



//
//  TMLEqCompDataParseLine
//

TMLEqCompDataParseLine::TMLEqCompDataParseLine()
	: 
	TMLEqCompDataLine(), 
	iSaxValues(0)
//	iSaxParentInfo(0),
//	iSaxAttributeInfo(0)
	{
	}

MSaxLayoutEqHandler* TMLEqCompDataParseLine::HandleSax(const std::string& aElement, const TAttribs& aAttribs)
	{
	if(aElement == KEqCompDataParseNodeComponent)
		{
		HandleSaxAttributes(aAttribs);
		}
	else if(aElement == KEqCompDataParseNodeLayoutInfoParam)
		{
		//	<LayoutInfo>
		//		<Param name="Left">
		//		...
		//	</LayoutInfo>
		return HandleSaxParam(aElement, aAttribs);
		}
// @todo need to implement attribute parsing
/*
	else if(aElement == KEqCompDataParseNodeAttributeInfo)
		{
		return HandleSaxAttributeInfo(aElement, aAttribs);
		}
*/
	return this;
	}


void TMLEqCompDataParseLine::HandleSaxAttributes(const TAttribs& aAttribs)
	{
// there are three different forms of component, stored in different files
// but they all refer to the same components

//	<Component 
//		catRefId="3449" 
//		componentCategory="0" 
//		creationTime="1150805213430" 
//		id="5129" 
//		independent="false" 
//		keepLocalCopy="true" 
//		maxVariety="0" 
//		type="4">

	string idStr = aAttribs.getValue(KEqCompDataParseNodeComponentAttributeId);
	iId = CdlTkUtil::ParseInt(idStr);

	string name = aAttribs.getValue(KEqCompDataParseNodeComponentAttributeName);
	if(!name.empty())
		iName = name;

	string typeStr = aAttribs.getValue(KEqCompDataParseNodeComponentAttributeType);
	if(typeStr == KEqCompDataParseTextComponentTypeScreen)
		{
		iType = EScreenComponent;
		}
	else if(typeStr == KEqCompDataParseTextComponentTypeContainer)
		{
		iType = EContainerComponent;
		}
	else if(typeStr == KEqCompDataParseTextComponentTypePane)
		{
		iType = EPaneComponent;
		}
	else if(typeStr == KEqCompDataParseTextComponentTypeGraphic)
		{
		iType = EGraphicComponent;
		}
	else if(typeStr == KEqCompDataParseTextComponentTypeText)
		{
		iType = ETextComponent;
		}
	else
		{
		iType = EUnknownComponent;
		}

	// @todo don't know where drawing order is defined!
//	string drawingOrderStr = aAttribs.getValue(KEqCompDataParseNodeComponentAttributeDrawingOrder);
//	iDrawingOrder = CdlTkUtil::ParseInt(drawingOrderStr);

		// @todo need to work out orientation from which 
//	string mirroredStr = aAttribs.getValue(KEqCompDataParseNodeComponentAttributeMirror);
//	iIsMirroredHorizontally = (mirroredStr == KEqCompDataParseTextComponentMirrorTrue); // will be false if empty, eg for a non-mirrored layout

	}

MSaxLayoutEqHandler* TMLEqCompDataParseLine::HandleSaxParam(const std::string& /*aElement*/, const TAttribs& aAttribs)
	{
	delete iSaxValues;
	iSaxValues = 0;
	iSaxValues = new TMLEqCompDataParseValues(this);

	string nameStr = aAttribs.getValue(KEqCompDataParseNodeLayoutInfoParamAttributeName);
	iSaxValues->iName = ShortParamName(nameStr);

	return iSaxValues;
	}

void TMLEqCompDataParseLine::HandleSaxEnd(const std::string& aElement)
	{
	if(aElement == KEqCompDataParseNodeLayoutInfoParam)
		{
		insert(make_pair(iSaxValues->iName, *iSaxValues));
		delete iSaxValues;
		iSaxValues = 0;
		}
	}

string TMLEqCompDataParseLine::ShortParamName(string& aName)
	{
	string shortName = aName;
	if(aName == KEqCompDataParseTextParamNameLeft)
		shortName = KEqCompDataParseTextParamShortNameLeft;
	else if(aName == KEqCompDataParseTextParamNameRight)
		shortName = KEqCompDataParseTextParamShortNameRight;
	else if(aName == KEqCompDataParseTextParamNameTop)
		shortName = KEqCompDataParseTextParamShortNameTop;
	else if(aName == KEqCompDataParseTextParamNameBottom)
		shortName = KEqCompDataParseTextParamShortNameBottom;
	else if(aName == KEqCompDataParseTextParamNameWidth)
		shortName = KEqCompDataParseTextParamShortNameWidth;
	else if(aName == KEqCompDataParseTextParamNameHeight)
		shortName = KEqCompDataParseTextParamShortNameHeight;
	else if(aName == KEqCompDataParseTextParamNameJustification)
		shortName = KEqCompDataParseTextParamShortNameJustification;
	else if(aName == KEqCompDataParseTextParamNameType)
		shortName = KEqCompDataParseTextParamShortNameType;
	return shortName;
	}


//
// class TMLEqListParseComponent
//
TMLEqListParseComponent::TMLEqListParseComponent()
	{

	}

MSaxLayoutEqHandler* TMLEqListParseComponent::HandleSax(const std::string& aElement, const TAttribs& aAttribs)
	{
// <Component 
//		compId="5129" 
//		level="2" 
//		name="list_form2_midp_pane_t1" 
//		pureSection="false" 
//		type="4">

	if(aElement == KEqListParseComponent)
		{
		string idStr = aAttribs.getValue(KEqListParseComponentId);
		iId = CdlTkUtil::ParseInt(idStr);

		iName = aAttribs.getValue(KEqListParseComponentName);
		}

	return this;
	}

void TMLEqListParseComponent::HandleSaxEnd(const std::string& aElement)
	{
	// nothing to do
	}


//
// class TMLEqParChildParseComponent
//
TMLEqParChildParseComponent::TMLEqParChildParseComponent()
	{

	}

MSaxLayoutEqHandler* TMLEqParChildParseComponent::HandleSax(const std::string& aElement, const TAttribs& aAttribs)
	{
//	<Component id="5198">
//		<parent parId="529"/>
//		<Option id="0" orientation="1" parOptionId="102"/>
//		<Option id="0" orientation="2" parOptionId="102"/>
//		<Option id="1" orientation="1" parOptionId="103"/>
//		<Option id="1" orientation="2" parOptionId="103"/>
//		<Option id="2" orientation="1" parOptionId="104"/>
//		<Option id="2" orientation="2" parOptionId="104"/>
//		<Option id="3" orientation="1" parOptionId="105"/>
//		<Option id="3" orientation="2" parOptionId="105"/>
//		<Document docParId="529" docSiblingLevel="1"/>
//	</Component>

	if(aElement == KEqParChildParseComponent)
		{
		string idStr = aAttribs.getValue(KEqParChildParseComponentId);
		iId = CdlTkUtil::ParseInt(idStr);
		}
	else if(aElement == KEqParChildParseComponentParent)
		{
		HandleSaxParent(aElement, aAttribs);
		}

	return this;
	}

void TMLEqParChildParseComponent::HandleSaxEnd(const std::string& aElement)
	{
	// nothing to do
	}

void TMLEqParChildParseComponent::HandleSaxParent(const std::string& aElement, const TAttribs& aAttribs)
	{
//		<parent parId="529"/>

		string parentIdStr = aAttribs.getValue(KEqParChildParseComponentParentId);
		iParentId = CdlTkUtil::ParseInt(parentIdStr);

		// @todo must implement a conversion for dummy components that have non-integer component ids.
/*
	if(iId == 0)
		{
		// this must be a dummy component, so have to fabricate a component id
		if(iName == "text_primary")
			iId = -1;
		else if(iName == "text_secondary")
			iId = -2;
		else if(iName == "text_title")
			iId = -3;
		else if(iName == "text_primary_small")
			iId = -4;
		}
*/
	}


//
// class TMLEqCompDataParseLayout
//

MSaxLayoutEqHandler* TMLEqCompDataParseLayout::HandleSax(const std::string& aElement, const TAttribs& aAttribs)
	{
	if(aElement == KEqListParseRoot || aElement == KEqCompDataParseRoot || aElement == KEqParChildParseRoot)
		{
		iSaxLayoutType = aElement;
		return this;
		}
	else if(aElement == KEqListParseComponent || aElement == KEqCompDataParseNodeComponent || aElement == KEqParChildParseComponent)
		{
		return HandleSaxComponent(aElement, aAttribs);
		}
	else
		return this;
	}

void TMLEqCompDataParseLayout::HandleSaxEnd(const std::string& aElement)
	{
	}

MSaxLayoutEqHandler* TMLEqCompDataParseLayout::HandleSaxComponent(const std::string& aElement, const TAttribs& aAttribs)
	{

	/// @todo not yet handling par child component!
// <Component id="5198">

	MSaxLayoutEqHandler* handler = NULL;
	if(iSaxLayoutType == KEqListParseRoot)
		{
		TMLEqListParseComponent* line = new TMLEqListParseComponent();
		line->HandleSax(aElement, aAttribs);
		iListComponents.insert(make_pair(line->iId, line));
		handler = line;
		}
	else if(iSaxLayoutType == KEqCompDataParseRoot)
		{
		TMLEqCompDataParseLine* line = new TMLEqCompDataParseLine();
		line->HandleSax(aElement, aAttribs);
		iComponents.insert(make_pair(line->iId, line));
		handler = line;
		}
	else if(iSaxLayoutType == KEqParChildParseRoot)
		{
		TMLEqParChildParseComponent* line = new TMLEqParChildParseComponent();
		line->HandleSax(aElement, aAttribs);
		iParChildComponents.insert(make_pair(line->iId, line));
		handler = line;
		}
	return handler;
	}

// this is the entry point into the parser
auto_ptr<TMLEqCompDataParseLayout> TMLEqCompDataParseLayout::Parse(
	const string& aListName, 
	const string& aCompDataName, 
	const string& aParChildName)
	{
	auto_ptr<TMLEqCompDataParseLayout> layout(new TMLEqCompDataParseLayout);
	layout->iName = aCompDataName;

	// SAX parsing method
	TEqLayoutSaxParser saxParser(layout.get());

	cout << "reading MLEqCompData(SAX) " << aListName << " + " << aCompDataName << " + " << aParChildName << endl;
	saxParser.Parse(aListName);
	saxParser.Parse(aCompDataName);
	saxParser.Parse(aParChildName);

	return layout;
	}


//
// class TEqLayoutSaxParser
//
TEqLayoutSaxParser::TEqLayoutSaxParser(MSaxLayoutEqHandler* aHandler)
	{ 
	iStack.push(aHandler);
	}

void TEqLayoutSaxParser::Parse(const std::string& aFileName)
	{
	Arabica::SAX::InputSource<std::string> is(aFileName);
	Arabica::SAX::XMLReader<std::string> parser;
	parser.setContentHandler(*this);
	parser.setErrorHandler(*this);
	parser.parse(is);
	}

void TEqLayoutSaxParser::startElement(const std::string& /*namespaceURI*/, const std::string& localName,
                              const std::string& /*qName*/, const Arabica::SAX::Attributes<std::string>& atts)
    {
	MSaxLayoutEqHandler* handler = iStack.top();
	if (!handler)
		throw GeneralErr("SAX: No element handler");
	MSaxLayoutEqHandler* next = handler->HandleSax(localName, atts);
	iStack.push(next);
    }

void TEqLayoutSaxParser::endElement(const std::string& /*namespaceURI*/, const std::string& localName,
                            const std::string& /*qName*/)
    {
	iStack.pop();
	MSaxLayoutEqHandler* handler = iStack.top();
	if (handler)
		handler->HandleSaxEnd(localName);
    }


void TEqLayoutSaxParser::warning(const TException& aException) 
    {
	cerr << aException.what();
    }

void TEqLayoutSaxParser::error(const TException& aException) 
    {
	cerr << aException.what();
    }

void TEqLayoutSaxParser::fatalError(const TException& aException)
    {
	cerr << aException.what();
    }


// End of File
