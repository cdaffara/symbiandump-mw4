/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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


#include "MLCompDataParse.h"

#include <AknDef.hrh>

#include <sstream>
#include <fstream>
#include <algorithm>
#include <iostream> // !!! for debug output only

extern string whiteSpace;


// 
// const data
//

const string KCompDataParseRoot = "Layout";

const string KCompDataParseRootAttributeMasterName = "MasterName";
const string KCompDataParseRootAttributeLayoutName = "LayoutName";
const string KCompDataParseRootAttributeVariantName = "variant_name";
const string KCompDataParseRootAttributeResolutionHeight = "resolutionHeight";
const string KCompDataParseRootAttributeResolutionWidth = "resolutionWidth";
const string KCompDataParseRootAttributeCreatedOn = "createdOn";

const string KCompDataParseNodeComponent = "Component";
const string KCompDataParseNodeComponentAttributeName = "name";
const string KCompDataParseNodeComponentAttributeId = "id";
const string KCompDataParseNodeComponentAttributeType = "type";
const string KCompDataParseNodeComponentAttributeDrawingOrder = "drawingorder";
const string KCompDataParseNodeComponentAttributeMirror = "mirror";
const string KCompDataParseNodeComponentAttributeMaxVariety = "maxVariety";

const string KCompDataParseTextComponentTypeScreen= "Screen";
const string KCompDataParseTextComponentTypeContainer= "Container";
const string KCompDataParseTextComponentTypePane= "Pane";
const string KCompDataParseTextComponentTypeGraphic= "Graphic";
const string KCompDataParseTextComponentTypeText= "Text";

const string KCompDataParseTextComponentMirrorTrue= "true";

const string KCompDataParseNodeOption = "option";
const string KCompDataParseNodeParentInfo = "ParentInfo";
const string KCompDataParseNodeLayoutInfo = "LayoutInfo";
const string KCompDataParseNodeAttributeInfo = "AttributeInfo";
const string KCompDataParseNodeAttrSets = "attrsets";

const string KCompDataParseNodeParent = "parent";
const string KCompDataParseNodeParentOption = "option";
const string KCompDataParseNodeParentOptionAttributeVariety = "variety";
const string KCompDataParseNodeParentInfoParentAttributeId = "id";
const string KCompDataParseNodeParentInfoParentAttributeVariety = "variety";

const string KCompDataParseNodeLayoutInfoParam = "Param";
const string KCompDataParseNodeLayoutInfoParamAttributeName = "name";

const string KCompDataParseNodeValue = "Value";
const string KCompDataParseNodeVarietyIndex = "varietyIndex";
const string KCompDataParseNodeVarietyIndexValue = "value";
const string KCompDataParseNodeCalc = "calc";
const string KCompDataParseNodeCalcAttributeValue = "value";
const string KCompDataParseNodeCalcAttributeColNumber = "colNumber";
const string KCompDataParseNodeCalcAttributeRowNumber = "rowNumber";
const string KCompDataParseNodeCalcAttributeShowInTable = "showInTable";
const string KCompDataParseNodeCalcAttributeShowInTableTrue = "true";
const string KCompDataParseNodeCalcAttributeShowInTableFalse = "false";
const string KCompDataParseNodeCalcAttributeZoom = "zoom";
const string KCompDataParseNodeCalcAttributeZoomNormal = "normal";
const string KCompDataParseNodeCalcAttributeZoomVerySmall = "-2 zoom";
const string KCompDataParseNodeCalcAttributeZoomSmall = "-1 zoom";
const string KCompDataParseNodeCalcAttributeZoomLarge = "+1 zoom";
const string KCompDataParseNodeCalcAttributeZoomVeryLarge = "+2 zoom";

const string KCompDataParseTextFontPrimary = "qfn_primary";
const string KCompDataParseTextFontSecondary = "qfn_secondary";
const string KCompDataParseTextFontTitle = "qfn_title";
const string KCompDataParseTextFontPrimarySmall = "qfn_primary_small"; // is that the correct text?
const string KCompDataParseTextFontDigital = "qfn_digital";

const string KCompDataParseTextParamNameColor = "Color";
const string KCompDataParseTextParamShortNameColor = "C";
const string KCompDataParseTextParamNameLeft = "Left";
const string KCompDataParseTextParamShortNameLeft = "l";
const string KCompDataParseTextParamNameRight = "Right";
const string KCompDataParseTextParamShortNameRight = "r";
const string KCompDataParseTextParamNameTop = "Top";
const string KCompDataParseTextParamShortNameTop = "t";
const string KCompDataParseTextParamNameBottom = "Bottom";
const string KCompDataParseTextParamShortNameBottom = "b";
const string KCompDataParseTextParamNameWidth = "Width";
const string KCompDataParseTextParamShortNameWidth = "W";
const string KCompDataParseTextParamNameHeight = "Height";
const string KCompDataParseTextParamShortNameHeight = "H";
const string KCompDataParseTextParamNameJustification = "Justification";
const string KCompDataParseTextParamShortNameJustification = "J";
const string KCompDataParseTextParamNameType = "Type";
const string KCompDataParseTextParamShortNameType = "Type";

// these have been changed to lower case, so that matching
// can be performed independent of case
const string KCompDataParseTextParamTypeCalcValueNone = "none";
const string KCompDataParseTextParamTypeCalcValueLeft = "left";
const string KCompDataParseTextParamTypeCalcValueCenter = "center";
const string KCompDataParseTextParamTypeCalcValueRight = "right";
const string KCompDataParseTextParamTypeCalcValueBidi = "bidi";

const string KCompDataParseNodeAttributeInfoOption = "option";
const string KCompDataParseNodeAttributeInfoOptionAttributeVariety = "variety";
const string KCompDataParseNodeAttributeInfoAttributeSet = "attributeset";
const string KCompDataParseNodeAttributeInfoAttributeSetAttributeName = "name";

//
//  TMLCompDataParseValues
//

TMLCompDataParseValues::TMLCompDataParseValues(TMLCompDataLine* aLine)
	: 
	TMLCompDataValues(aLine), 
	iSaxZoomLevels(0), 
	iSaxVariety(-1)
	{

	}

MSaxLayoutHandler* TMLCompDataParseValues::HandleSax(const std::string& aElement, const TAttribs& aAttribs)
	{
//	<Param name="Left">
//		<Value>
//	      <varietyIndex value="0"/>
//			<optionIndex value="0"/>
//			<calc ... >
//			...
//		</Value>
//		...
//	</Param>

	if (aElement == KCompDataParseNodeValue)
		{
		delete iSaxZoomLevels;
		iSaxZoomLevels = 0;
		iSaxZoomLevels = new TMLCompDataZoomLevels;
		}
	else if (aElement == KCompDataParseNodeVarietyIndex)
		{
		HandleSaxVariety(aElement, aAttribs);
		}
	else if (aElement == KCompDataParseNodeCalc)
		{
		HandleSaxCalc(aElement, aAttribs);
		}

	return this;
	}

void TMLCompDataParseValues::HandleSaxEnd(const std::string& aElement)
	{
	if (aElement == KCompDataParseNodeValue)
		{
		insert(make_pair(iSaxVariety, *iSaxZoomLevels));
		delete iSaxZoomLevels;
		iSaxZoomLevels = 0;
		}
	}

void TMLCompDataParseValues::HandleSaxVariety(const std::string& /*aElement*/, const TAttribs& aAttribs)
	{
//      <varietyIndex value="0"/>

	string varietyIndexStr = aAttribs.getValue(KCompDataParseNodeVarietyIndexValue);
	iSaxVariety = CdlTkUtil::ParseInt(varietyIndexStr); // defaults to zero
	if(iLine->MaxVariety() < iSaxVariety)
		{
		iLine->SetMaxVariety(iSaxVariety);
		}
	}

void TMLCompDataParseValues::HandleSaxCalc(const std::string& /*aElement*/, const TAttribs& aAttribs)
	{
//		<calc value="0"/>
// or
//		<calc value="0" zoom="x_2"/>
// or
//		<calc colNumber="1" rowNumber="1" value="0"/>
// or
//		<calc colNumber="1" rowNumber="1" value="0" zoom="x_2"/>

    // note that if there is only one calc, but the colNumber and rowNumber attributes are present
    // then we have to assume that it is a multivalue component, so deduce which kind it is from 
    // the current value name

	// first the index part
	int max = 1;
	int cols = 0;
	int rows = 0;

	string colStr = aAttribs.getValue(KCompDataParseNodeCalcAttributeColNumber);
	if(!colStr.empty())
		{
		cols = CdlTkUtil::ParseInt(colStr);
        if(TMLCompDataTable::IsHorizontalColumn(iName))
            {
            iLine->SetNeedsCols(true);
            }
		}

	string rowStr = aAttribs.getValue(KCompDataParseNodeCalcAttributeRowNumber);
	if(!rowStr.empty())
		{
		rows = CdlTkUtil::ParseInt(rowStr);
        if(TMLCompDataTable::IsVerticalColumn(iName))
            {
            iLine->SetNeedsRows(true);
            }
		}

	// keep the line updated if we have found more rows / cols than it knows about
	if(iLine->NumCols() < cols) 
		iLine->SetNumCols(cols);
	if(iLine->NumRows() < rows) 
		iLine->SetNumRows(rows);

	// each calc is either a row or a col (depending on which axis)
	if(cols > 1)
		max = cols;
	else if(rows > 1)
		max = rows;

	// then the value part of the calc
	string valueStr;
	valueStr = aAttribs.getValue(KCompDataParseNodeCalcAttributeValue);
	ConvertValueStr(valueStr);

	// check whether the calc is a default value (which corresponds to normal zoom)
	// or if it corresponds to a known zoom factor
	string zoomStr = aAttribs.getValue(KCompDataParseNodeCalcAttributeZoom);
	int zoom = ConvertZoomStr(zoomStr);
	if(zoom != EAknUiZoomAutomatic)
		{
		// then put them together
		int num = max - 1; // change to zero based
		TMLCompDataCalcs& calcs = (*iSaxZoomLevels)[zoom];
		calcs[num] = valueStr;
		}
	}

void TMLCompDataParseValues::ConvertValueStr(std::string& aValueStr)
	{
	int valueInt = 0;
	if(iName == KCompDataParseTextParamShortNameType)
		{
		if(aValueStr == KCompDataParseTextFontPrimary) 
			valueInt = ELayoutCompilerFontCategoryPrimary;
		else if(aValueStr == KCompDataParseTextFontSecondary)
			valueInt = ELayoutCompilerFontCategorySecondary;
		else if(aValueStr == KCompDataParseTextFontTitle)
			valueInt = ELayoutCompilerFontCategoryTitle;
		else if(aValueStr == KCompDataParseTextFontPrimarySmall) 
			valueInt = ELayoutCompilerFontCategoryPrimarySmall;
		else if(aValueStr == KCompDataParseTextFontDigital)
			valueInt = ELayoutCompilerFontCategoryDigital;
		else
			valueInt = ELayoutCompilerFontCategoryUndefined;
		aValueStr = CdlTkUtil::IntToString(valueInt);
		}
	else if(iName == KCompDataParseTextParamShortNameJustification)
		{
		// jusitication does not have consistent capitalization
		string lowerValueStr = CdlTkUtil::ToLower(aValueStr);
		if(lowerValueStr == KCompDataParseTextParamTypeCalcValueNone) 
			valueInt = ELayoutAlignNone;
		else if(lowerValueStr == KCompDataParseTextParamTypeCalcValueLeft) 
			valueInt = ELayoutAlignLeft;
		else if(lowerValueStr == KCompDataParseTextParamTypeCalcValueRight) 
			valueInt = ELayoutAlignRight;
		else if(lowerValueStr == KCompDataParseTextParamTypeCalcValueCenter) 
			valueInt = ELayoutAlignCenter;
		else if(lowerValueStr == KCompDataParseTextParamTypeCalcValueBidi) 
			valueInt = ELayoutAlignBidi;
		else 
			valueInt = ELayoutAlignNone;
		aValueStr = CdlTkUtil::IntToString(valueInt);
		}
	}

int TMLCompDataParseValues::ConvertZoomStr(const std::string& aZoomStr)
	{
	int zoomInt = EAknUiZoomAutomatic;
	string lowerZoomStr = CdlTkUtil::ToLower(aZoomStr);
	if(lowerZoomStr.empty())
		zoomInt = EAknUiZoomNormal;
	else if(lowerZoomStr == KCompDataParseNodeCalcAttributeZoomVerySmall) 
		zoomInt = EAknUiZoomVerySmall;
	else if(lowerZoomStr == KCompDataParseNodeCalcAttributeZoomSmall) 
		zoomInt = EAknUiZoomSmall;
	else if(lowerZoomStr == KCompDataParseNodeCalcAttributeZoomLarge) 
		zoomInt = EAknUiZoomLarge;
	else if(lowerZoomStr == KCompDataParseNodeCalcAttributeZoomVeryLarge) 
		zoomInt = EAknUiZoomVeryLarge;
	return zoomInt;
	}


//
//  TMLCompDataParseParentInfo
//

TMLCompDataParseParentInfo::TMLCompDataParseParentInfo(TMLCompDataLine* aLine)
	:
	iSaxParentInfoSelector(0),
	iSaxVariety(0)
	{
	
	}

MSaxLayoutHandler* TMLCompDataParseParentInfo::HandleSax(const std::string& aElement, const TAttribs& aAttribs)
	{
//	<ParentInfo>
// 		<option variety="0">
// 			<parent id="3140" variety="2"/>
//		</option>
//	</ParentInfo>

	if (aElement == KCompDataParseNodeOption)
		{
		delete iSaxParentInfoSelector;
		iSaxParentInfoSelector= 0;
		iSaxParentInfoSelector = new TMLCompDataParentInfoSelector;
		HandleSaxVariety(aElement, aAttribs);
		}
	else if (aElement == KCompDataParseNodeParent)
		{
		HandleSaxParent(aElement, aAttribs);
		}

	return this;
	}

void TMLCompDataParseParentInfo::HandleSaxEnd(const std::string& aElement)
	{
	if (aElement == KCompDataParseNodeOption) // value
		{
		insert(make_pair(iSaxVariety, *iSaxParentInfoSelector));
		delete iSaxParentInfoSelector;
		iSaxParentInfoSelector = 0;
		}
	}

void TMLCompDataParseParentInfo::HandleSaxParent(const std::string& aElement, const TAttribs& aAttribs)
	{
// 			<parent id="3140" variety="2"/>
	string parentIdStr = aAttribs.getValue(KCompDataParseNodeParentInfoParentAttributeId);
	string parentVarietyStr = aAttribs.getValue(KCompDataParseNodeParentInfoParentAttributeVariety);
	
	iSaxParentInfoSelector->iParentId = CdlTkUtil::ParseInt(parentIdStr);
	iSaxParentInfoSelector->iParentVariety = CdlTkUtil::ParseInt(parentVarietyStr);
	}

void TMLCompDataParseParentInfo::HandleSaxVariety(const std::string& aElement, const TAttribs& aAttribs)
	{
// 		<option variety="0">
	string varietyIndexStr = aAttribs.getValue(KCompDataParseNodeParentOptionAttributeVariety);
	iSaxVariety = CdlTkUtil::ParseInt(varietyIndexStr); // defaults to zero
	}

//
//  TMLCompDataParseLayoutLine
//

TMLCompDataParseLayoutLine::TMLCompDataParseLayoutLine()
	: 
	TMLCompDataLine(), 
	iSaxValues(0),
	iSaxParentInfo(0),
	iSaxAttributeInfo(0)
	{
	}

MSaxLayoutHandler* TMLCompDataParseLayoutLine::HandleSax(const std::string& aElement, const TAttribs& aAttribs)
	{
	if(aElement == KCompDataParseNodeComponent)
		{
		HandleSaxAttributes(aAttribs);
		}
	else if(aElement == KCompDataParseNodeParentInfo)
		{
		return HandleSaxParent(aElement, aAttribs);
		}
	else if(aElement == KCompDataParseNodeLayoutInfoParam)
		{
		//	<LayoutInfo>
		//		<Param name="Left">
		//		...
		//	</LayoutInfo>
		return HandleSaxParam(aElement, aAttribs);
		}
	else if(aElement == KCompDataParseNodeAttributeInfo)
		{
		return HandleSaxAttributeInfo(aElement, aAttribs);
		}

	return this;
	}


void TMLCompDataParseLayoutLine::HandleSaxAttributes(const TAttribs& aAttribs)
	{
//	<Component 
//		drawingorder="0" 
//		id="1" 
//		maxVariety="2" 
//		mirror="true" 
//		name="application_window" 
//		type="Container">
	
	iName = aAttribs.getValue(KCompDataParseNodeComponentAttributeName);

	string idStr = aAttribs.getValue(KCompDataParseNodeComponentAttributeId);
	iId = CdlTkUtil::ParseInt(idStr);

	if(iId == 0)
		{
		// this must be a dummy component, so have to fabricate a component id
		// TODO: ensure that layout data contains real component ids
		if(iName == "text_primary")
			iId = -1;
		else if(iName == "text_secondary")
			iId = -2;
		else if(iName == "text_title")
			iId = -3;
		else if(iName == "text_primary_small")
			iId = -4;
		}

	string typeStr = aAttribs.getValue(KCompDataParseNodeComponentAttributeType);
	if(typeStr == KCompDataParseTextComponentTypeScreen)
		{
		iType = EScreenComponent;
		}
	else if(typeStr == KCompDataParseTextComponentTypeContainer)
		{
		iType = EContainerComponent;
		}
	else if(typeStr == KCompDataParseTextComponentTypePane)
		{
		iType = EPaneComponent;
		}
	else if(typeStr == KCompDataParseTextComponentTypeGraphic)
		{
		iType = EGraphicComponent;
		}
	else if(typeStr == KCompDataParseTextComponentTypeText)
		{
		iType = ETextComponent;
		}
	else
		{
		iType = EUnknownComponent;
		}

	string drawingOrderStr = aAttribs.getValue(KCompDataParseNodeComponentAttributeDrawingOrder);
	iDrawingOrder = CdlTkUtil::ParseInt(drawingOrderStr);

	string mirroredStr = aAttribs.getValue(KCompDataParseNodeComponentAttributeMirror);
	iIsMirroredHorizontally = (mirroredStr == KCompDataParseTextComponentMirrorTrue); // will be false if empty, eg for a non-mirrored layout

	string maxVarietyStr = aAttribs.getValue(KCompDataParseNodeComponentAttributeMaxVariety);
	if(!maxVarietyStr.empty()) // screen doesn't have maxVariety for some reason
		{
		//int maxVariety = CdlTkUtil::ParseInt(maxVarietyStr) - 2;
		// we ignore the maxVariety for now, as it will be calculated by parsing the values
		}
	}

MSaxLayoutHandler* TMLCompDataParseLayoutLine::HandleSaxParent(const std::string& aElement, const TAttribs& aAttribs)
	{
	delete iSaxParentInfo;
	iSaxParentInfo = 0;
	iSaxParentInfo = new TMLCompDataParseParentInfo(this);
	return iSaxParentInfo;
	}

MSaxLayoutHandler* TMLCompDataParseLayoutLine::HandleSaxParam(const std::string& /*aElement*/, const TAttribs& aAttribs)
	{
	delete iSaxValues;
	iSaxValues = 0;
	iSaxValues = new TMLCompDataParseValues(this);

	string nameStr = aAttribs.getValue(KCompDataParseNodeLayoutInfoParamAttributeName);
	iSaxValues->iName = ShortParamName(nameStr);

	return iSaxValues;
	}

void TMLCompDataParseLayoutLine::HandleSaxEnd(const std::string& aElement)
	{
	if(aElement == KCompDataParseNodeParentInfo)
		{
		iParentInfo = iSaxParentInfo;
		iSaxParentInfo = 0;
		}
	else if(aElement == KCompDataParseNodeLayoutInfoParam)
		{
		insert(make_pair(iSaxValues->iName, *iSaxValues));
		delete iSaxValues;
		iSaxValues = 0;
		}
	else if(aElement == KCompDataParseNodeAttributeInfo)
		{
		iAttributeInfo = iSaxAttributeInfo;
		iSaxAttributeInfo = 0;
		}
	}

MSaxLayoutHandler* TMLCompDataParseLayoutLine::HandleSaxAttributeInfo(const std::string& aElement, const TAttribs& aAttribs)
	{
	delete iSaxAttributeInfo;
	iSaxAttributeInfo = 0;
	iSaxAttributeInfo = new TMLCompDataParseAttributeInfo(this);
	return iSaxAttributeInfo;
	}

string TMLCompDataParseLayoutLine::ShortParamName(string& aName)
	{
	string shortName = aName;
	if(aName == KCompDataParseTextParamNameColor)
		shortName = KCompDataParseTextParamShortNameColor;
	else if(aName == KCompDataParseTextParamNameLeft)
		shortName = KCompDataParseTextParamShortNameLeft;
	else if(aName == KCompDataParseTextParamNameRight)
		shortName = KCompDataParseTextParamShortNameRight;
	else if(aName == KCompDataParseTextParamNameTop)
		shortName = KCompDataParseTextParamShortNameTop;
	else if(aName == KCompDataParseTextParamNameBottom)
		shortName = KCompDataParseTextParamShortNameBottom;
	else if(aName == KCompDataParseTextParamNameWidth)
		shortName = KCompDataParseTextParamShortNameWidth;
	else if(aName == KCompDataParseTextParamNameHeight)
		shortName = KCompDataParseTextParamShortNameHeight;
	else if(aName == KCompDataParseTextParamNameJustification)
		shortName = KCompDataParseTextParamShortNameJustification;
	else if(aName == KCompDataParseTextParamNameType)
		shortName = KCompDataParseTextParamShortNameType;
	return shortName;
	}


//
//  TMLCompDataParseAttributeInfo
//

TMLCompDataParseAttributeInfo::TMLCompDataParseAttributeInfo(TMLCompDataLine* aLine)
	:
	iSaxAttributeInfoSelector(0),
	iSaxVariety(0)
	{
	
	}

MSaxLayoutHandler* TMLCompDataParseAttributeInfo::HandleSax(const std::string& aElement, const TAttribs& aAttribs)
	{
//        <AttributeInfo>
//            <option variety="0">
//                <attributeset name="a0"/>
//            </option>
//            <option variety="1">
//                <attributeset name="a1"/>
//            </option>
//        </AttributeInfo>

	if (aElement == KCompDataParseNodeAttributeInfoOption)
		{
		delete iSaxAttributeInfoSelector;
		iSaxAttributeInfoSelector= 0;
		iSaxAttributeInfoSelector = new TMLCompDataAttributeInfoSelector;
		HandleSaxVariety(aElement, aAttribs);
		}
	else if (aElement == KCompDataParseNodeAttributeInfoAttributeSet)
		{
		HandleSaxAttributeSet(aElement, aAttribs);
		}

	return this;
	}

void TMLCompDataParseAttributeInfo::HandleSaxEnd(const std::string& aElement)
	{
	if (aElement == KCompDataParseNodeAttributeInfoOption) // value
		{
		if(!iSaxAttributeInfoSelector->iAttributeSetName.empty())
			{
			insert(make_pair(iSaxVariety, *iSaxAttributeInfoSelector));
			}
		delete iSaxAttributeInfoSelector;
		iSaxAttributeInfoSelector = 0;
		}
	}

void TMLCompDataParseAttributeInfo::HandleSaxVariety(const std::string& aElement, const TAttribs& aAttribs)
	{
// 		<option variety="0">
	string varietyIndexStr = aAttribs.getValue(KCompDataParseNodeParentOptionAttributeVariety);
	iSaxVariety = CdlTkUtil::ParseInt(varietyIndexStr); // defaults to zero
	}

void TMLCompDataParseAttributeInfo::HandleSaxAttributeSet(const std::string& aElement, const TAttribs& aAttribs)
	{
//                <attributeset name="a0"/>
	string attributeSetNameStr = aAttribs.getValue(KCompDataParseNodeAttributeInfoAttributeSetAttributeName);
	iSaxAttributeInfoSelector->iAttributeSetName = attributeSetNameStr;
	}



//
// class TMLCompDataParseLayoutTable
//

TMLCompDataParseLayoutTable::TMLCompDataParseLayoutTable(TMLCompData* aTables)
:	TMLCompDataTable(aTables)
	{
	}

MSaxLayoutHandler* TMLCompDataParseLayoutTable::HandleSax(const std::string& /*aElement*/, const TAttribs& /*aAttribs*/)
	{
	return this;
	}



//
// class TMLCompDataParseLayout
//

MSaxLayoutHandler* TMLCompDataParseLayout::HandleSax(const std::string& aElement, const TAttribs& aAttribs)
	{
	if (aElement == KCompDataParseRoot)
		{
		// <Layout MasterId="0.6821563014440754" 
		//		MasterName="m_p" LayoutSetId="1" 
		//		LayoutName="D_avkon_port" 
		//		VariantId="0.45128997192758025" 
		//		variant_name="eur" 
		//		variant_abbreviation="e" 
		//		ppi="100.0" 
		//		ppiUnit="Inches" 
		//		resolutionHeight="416" 
		//		resolutionWidth="352" 
		//		createdOn="2004-08-27 12:37:42.727">
		iMasterName = aAttribs.getValue(KCompDataParseRootAttributeMasterName);
		iLayoutName = aAttribs.getValue(KCompDataParseRootAttributeLayoutName);
		iVariantName = aAttribs.getValue(KCompDataParseRootAttributeVariantName);
		iResolutionHeight = aAttribs.getValue(KCompDataParseRootAttributeResolutionHeight);
		iResolutionWidth = aAttribs.getValue(KCompDataParseRootAttributeResolutionWidth);
		iTimestamp = aAttribs.getValue(KCompDataParseRootAttributeCreatedOn);
		return this;
		}
	else if (aElement == KCompDataParseNodeComponent)
		{
		TMLCompDataParseLayoutLine* line = new TMLCompDataParseLayoutLine();
		line->HandleSax(aElement, aAttribs);
		iComponents.insert(make_pair(line->iId, line));
		return line;
		}
	else
		return this;
	}


auto_ptr<TMLCompDataParseLayout> TMLCompDataParseLayout::Parse(const string& aLayName)
	{
	auto_ptr<TMLCompDataParseLayout> layout(new TMLCompDataParseLayout);
	int pos=0;
	string layName = aLayName;
	string::size_type next = layName.find('+', pos);
	if (next != string::npos)
		{
		throw GeneralErr(layName + " TMLCompDataParseLayout::Parse() - doesn't support multiple files");
		}

	if (layName.size() >= 2 && layName.substr(0,2) == "-m")
		{
		layName = layName.substr(2);
		layout->iCanBeMirror = true;
		}
	layout->iName = layName;

	// SAX parsing method
	cout << "reading MLCompData(SAX) " << layName << endl;
	TLayoutSaxParser saxParser(layout.get());
	saxParser.Parse(layName);

	return layout;
	}



TLayoutSaxParser::TLayoutSaxParser(MSaxLayoutHandler* aHandler)
	{ 
	iStack.push(aHandler);
	}

void TLayoutSaxParser::Parse(const std::string& aFileName)
	{
	Arabica::SAX::InputSource<std::string> is(aFileName);
	Arabica::SAX::XMLReader<std::string> parser;
	parser.setContentHandler(*this);
	parser.setErrorHandler(*this);
	parser.parse(is);
	}

void TLayoutSaxParser::startElement(const std::string& /*namespaceURI*/, const std::string& localName,
                              const std::string& /*qName*/, const Arabica::SAX::Attributes<std::string>& atts)
    {
	MSaxLayoutHandler* handler = iStack.top();
	if (!handler)
		throw GeneralErr("SAX: No element handler");
	MSaxLayoutHandler* next = handler->HandleSax(localName, atts);
	iStack.push(next);
    }

void TLayoutSaxParser::endElement(const std::string& /*namespaceURI*/, const std::string& localName,
                            const std::string& /*qName*/)
    {
	iStack.pop();
	MSaxLayoutHandler* handler = iStack.top();
	if (handler)
		handler->HandleSaxEnd(localName);
    }


void TLayoutSaxParser::warning(const TException& aException) 
    {
	cerr << aException.what();
    }

void TLayoutSaxParser::error(const TException& aException) 
    {
	cerr << aException.what();
    }

void TLayoutSaxParser::fatalError(const TException& aException)
    {
	cerr << aException.what();
    }


// End of File
