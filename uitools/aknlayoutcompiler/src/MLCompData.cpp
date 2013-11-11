/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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


#include "MLCompData.h"
#include "MLCompDataParse.h"
#include "MLCompData2Cdl.h"
#include "MLAttributes.h"

#include "LayoutCompilerErr.h"
#include "CodeGenConsts.h"
#include "UsefulDefinitions.h"

#include <AknDef.hrh>

#include <set>
#include <sstream>
#include <iostream>
#include <algorithm>


//
// const data
//

const string KCompDataPaneOutputOrder[] = {"C", "l", "t", "r", "b", "W", "H"};
const string KCompDataPaneOutputOrderMirrored[] = {"C", "r", "t", "l", "b", "W", "H"};
const int KCompDataPaneOutputOrderSize = ARRAY_LEN(KCompDataPaneOutputOrder);

const string KCompDataGraphicOutputOrder[] = {"NumCols", "NumRows", "C", "l", "t", "r", "b", "W", "H"};
const string KCompDataGraphicOutputOrderMirrored[] = {"NumCols", "NumRows", "C", "r", "t", "l", "b", "W", "H"};
const int KCompDataGraphicOutputOrderSize = ARRAY_LEN(KCompDataGraphicOutputOrder);

const string KCompDataTextOutputOrder[] = {"NumCols", "NumRows", "C", "l", "t", "r", "b", "W", "H", "J", "Font"};
const string KCompDataTextOutputOrderMirrored[] = {"NumCols", "NumRows", "C", "r", "t", "l", "b", "W", "H", "J", "Font"};
const int KCompDataTextOutputOrderSize = ARRAY_LEN(KCompDataTextOutputOrder);

const string KCompDataKeywordParamHeight = "H";
const string KCompDataKeywordParamType = "Type";
const string KCompDataKeywordParamFont = "Font";
const string KCompDataKeywordParamNumCols = "NumCols";
const string KCompDataKeywordParamNumRows = "NumRows";
const string KCompDataKeywordScreenContents = "Screen Contents";

const string KCompDataKeywordUnderscore = "_";
const string KCompDataKeywordLineNameSuffixGraphic = "g";
const string KCompDataKeywordLineNameSuffixText = "t";

const string KCompDataSearchCollectionNumeric = "0123456789";
const string KCompDataBadValue = "Bad Value";
const string KCompDataLayoutEmpty = "ELayoutEmpty";
const string KCompDataUnknown = "unknown";

const string KCompDataCellNameLeft("l");
const string KCompDataCellNameRight("r");
const string KCompDataCellNameJustification("J");

const string KAttributeNameStyle1 = "style_1";
const string KAttributeNameStyle2 = "style_2";
const string KAttributeNameStyle3 = "style_3";
const string KAttributeNameStyle1Plain = "plain";
const string KAttributeNameStyle1Bold = "bold";
const string KAttributeNameStyle3Outline = "outline";

const string KAttributeNameNumberOfColumns = "Number_of_columns";
const string KAttributeNameNumberOfRows = "Number_of_rows";
const string KAttributeNameNumberOfRowsColsAuto = "AUTO";


//
// TMLCompDataValues
//

TMLCompDataValues::TMLCompDataValues()
	: 
	iLine(NULL), 
	iName(KCompDataBadValue)
	{
	}

TMLCompDataValues::TMLCompDataValues(TMLCompDataLine* aLine)
	: 
	iLine(aLine), 
	iName(aLine->iName)
	{
	}

bool TMLCompDataValues::operator==(const TMLCompDataValues& aOther) const
	{
	typedef const map<int, TMLCompDataZoomLevels> TBase;
	bool eq =
		iName == aOther.iName &&
		(*static_cast<TBase*>(this) == aOther);
	return eq;
	}

TMLCompDataValues::~TMLCompDataValues()
	{
	}

bool TMLCompDataValues::Merge(TMLCompDataLine* aLine, string aName, TMLCompDataValues& aOtherValues, bool aMirrorMerge)
	{
	iLine = aLine;
	iName = aName; // we may be swapping l and r

	// create missing values in this line if needed
	for (iterator pOtherVal=aOtherValues.begin(); pOtherVal!=aOtherValues.end(); ++pOtherVal)
		{
		TMLCompDataZoomLevels& otherZoomLevels = pOtherVal->second;
		TMLCompDataZoomLevels& thisZoomLevels = (*this)[pOtherVal->first];
		thisZoomLevels = otherZoomLevels; // we want exactly the correct number of calcs from the other cell
		if(aMirrorMerge)
			{
			if(iName == KCompDataCellNameJustification)
				{
				// reverse the justification
				for(TMLCompDataZoomLevels::iterator pCalcs = thisZoomLevels.begin(); pCalcs != thisZoomLevels.end(); ++pCalcs)
					{
					TMLCompDataCalcs& calcs = pCalcs->second;
					for(TMLCompDataCalcs::iterator pVal = calcs.begin(); pVal != calcs.end(); ++pVal)
						pVal->second = MirrorJustificationValue(pVal->second);
					}
				}
			}
		}
	return true;
	}

const string KParamLimitNames[] = { "NumCols", "NumRows" };
const string KHorizNames[] = { "l", "r", "W" };
const string KVertNames[] = { "t", "b", "H", "Font" };
const set<string> KParamLimitNamesSet(KParamLimitNames, ARRAY_END(KParamLimitNames));
const set<string> KHorizNamesSet(KHorizNames, ARRAY_END(KHorizNames));
const set<string> KVertNamesSet(KVertNames, ARRAY_END(KVertNames));

TMLCompDataValues::TCompDataCellType TMLCompDataValues::Type(string aName)
	{
	TMLCompDataValues::TCompDataCellType type;
	if(KParamLimitNamesSet.find(aName) != KParamLimitNamesSet.end())
		{
		type = TMLCompDataValues::ECellTypeParamLimit;	
		}
	else if(KHorizNamesSet.find(aName) != KHorizNamesSet.end())
		{
		type = TMLCompDataValues::ECellTypeCol;
		}
	else if(KVertNamesSet.find(aName) != KVertNamesSet.end())
		{
		type = TMLCompDataValues::ECellTypeRow;
		}
	else
		{
		type = TMLCompDataValues::ECellTypeDefault;
		}
	return type;
	}


string TMLCompDataValues::MirrorJustificationValue(const string& aValue)
	{
	int val = CdlTkUtil::ParseInt(aValue);
	if(val == ELayoutAlignLeft)
		val = ELayoutAlignRight;
	else if(val == ELayoutAlignRight)
		val = ELayoutAlignLeft;
	return CdlTkUtil::IntToString(val);
	}


void TMLCompDataValues::Compile(const string& aCellName)
	{
	// ensure that missing varieties are filled with null values
	// we assume that each variety is present in all zoom levels that are defined
	if(iLine) // screen contents isn't contained in a line
		{
		int thisSize = size();
		int numVarieties = iLine->MaxVariety() + 1; // get zero based index
		int maximum = max(numVarieties, thisSize);
		unsigned int maxMulti;
		switch(TMLCompDataValues::Type(aCellName))
			{
			case ECellTypeCol:
				maxMulti = iLine->NumCols();
				break;
			case ECellTypeRow:
				maxMulti = iLine->NumRows();
				break;
			default:
				maxMulti = 1;
				break;
			}
		for(int ii = 0; ii < maximum; ii++)
			{
			// operator[] fills in missing values
			TMLCompDataZoomLevels& zoomLevels = (*this)[ii];
			for(TMLCompDataZoomLevels::iterator pCalcs = zoomLevels.begin(); pCalcs != zoomLevels.end(); ++pCalcs)
				{
				TMLCompDataCalcs& calcs = pCalcs->second;
				if(ii < numVarieties)
					{
					if(calcs.size() > maxMulti)
                        {
                        // Handle the case where data from a merged instance has too many calcs
                        TMLCompDataCalcs::iterator start = calcs.find(maxMulti);
                        TMLCompDataCalcs::iterator end = calcs.end();
                        calcs.erase(start, end);
                        }
					}
				else
					{
					// get rid of varieties from instances that we have merged onto
					calcs.clear();			
					}
				}
			}
		}
	}

string TMLCompDataValues::CppValue(const string& aValue)
	{
	if (aValue.size())
		return aValue;
	else
		return KCompDataLayoutEmpty;
	}

//
// TMLCompDataParentInfoSelector
//
TMLCompDataParentInfoSelector::TMLCompDataParentInfoSelector()
	{

	}

TMLCompDataParentInfoSelector::TMLCompDataParentInfoSelector(int aParentId, int aParentVariety)
	:
	iParentId(aParentId),
	iParentVariety(aParentVariety)
	{

	}

//
//  TMLCompDataParentInfo
//

TMLCompDataParentInfo::TMLCompDataParentInfo()
	:
	iLine(0)
	{

	}

TMLCompDataParentInfo::TMLCompDataParentInfo(TMLCompDataLine* aLine)
	:
	iLine(aLine)
	{

	}

TMLCompDataParentInfo::~TMLCompDataParentInfo()
	{

	}

void TMLCompDataParentInfo::Merge(const TMLCompDataParentInfo& aOther)
	{
	for (const_iterator pOtherVariety = aOther.begin(); pOtherVariety != aOther.end(); ++pOtherVariety)
		{
		int varietyIndex = pOtherVariety->first;
		const TMLCompDataParentInfoSelector& selector = pOtherVariety->second;
		insert(make_pair(varietyIndex, selector));
		}
	}

//
//  TMLCompDataLine
//

TMLCompDataLine::TMLCompDataLine()
:	iId(0),
	iName(KCompDataUnknown), 
	iType(EUnknownComponent),
	iDrawingOrder(-1),
	iMaxVariety(0),
	iParentTable(0),
	iParentInfo(0),
	iAttributeInfo(0),
	iIsUnique(true),
	iGlobalIndex(0),
	iIsMirroredHorizontally(false),
	iNeedsOptions(false),
	iNeedsCols(false),
	iNeedsRows(false),
	iNumCols(1),
	iNumRows(1)
	{	
		
	}

TMLCompDataLine::TMLCompDataLine(const TMLCompDataLine& aOther)
	{
	if(this == &aOther)
		return;
	*this = aOther; // this will take a copy of the owning pointer
	if(aOther.iParentInfo) // if it wasn't zero
		iParentInfo = new TMLCompDataParentInfo(*(aOther.iParentInfo)); // we don't want to take ownership, so make our own copy
	if(aOther.iAttributeInfo) // if it wasn't zero
		iAttributeInfo = new TMLCompDataAttributeInfo(*(aOther.iAttributeInfo)); // we don't want to take ownership, so make our own copy

	for (iterator pVal = begin(); pVal != end(); ++pVal)
		{
		TMLCompDataValues& val = pVal->second;
		val.iLine = this;
		}
	iParentTable = 0; // will be set during compile
	}

bool TMLCompDataLine::operator==(const TMLCompDataLine& aOther) const
	{
	return (Name() == aOther.Name()) && ValuesEqual(aOther);
	}

TMLCompDataLine::~TMLCompDataLine()
	{
	delete iParentInfo;
	delete iAttributeInfo;
	}

bool TMLCompDataLine::lessthan(TMLCompDataLine* aLeft, TMLCompDataLine* aRight)
	{
	string pureNameLeft = aLeft->NameDiscountingSuffix();
	string pureNameRight = aRight->NameDiscountingSuffix();
	if(pureNameLeft != pureNameRight)
		{
		return (aLeft->iName) < (aRight->iName);
		}
	else
		{
		int left = CdlTkUtil::ParseInt(aLeft->NameSuffix());
		int right = CdlTkUtil::ParseInt(aRight->NameSuffix());
		return left < right;
		}
	}

bool TMLCompDataLine::ValuesEqual(const TMLCompDataLine& aOther) const
	{
	bool eq = true;
	const_iterator pVal, pOther;
	for (pVal = begin(), pOther = aOther.begin(); 
		 eq && pVal != end() && pOther != aOther.end(); 
		 ++pVal, ++pOther)
		{
		eq = (*pVal == *pOther);
		}
	eq = eq && pVal == end() && pOther == aOther.end();
	return eq;
	}

string TMLCompDataLine::Name() const
	{
	return iName;
	}

bool TMLCompDataLine::Merge(TMLCompDataLine& aOtherLine)
	{
	bool compatible = 
		(iId == aOtherLine.iId) ||
		(iName == aOtherLine.iName) ||
		(iType == aOtherLine.iType);
	if(compatible)
		{
		iDrawingOrder = aOtherLine.iDrawingOrder;
		iMaxVariety = aOtherLine.iMaxVariety;
		iIsMirroredHorizontally |= aOtherLine.iIsMirroredHorizontally; // in the case of an elaf layout merging onto an abrw layout, the chirality will be preserved
		iNeedsOptions |= aOtherLine.iNeedsOptions;

		if(!iParentInfo)
			{
			// must be screen...
			iParentInfo = new TMLCompDataParentInfo();
			}
		if(aOtherLine.iParentInfo)
			{
			iParentInfo->Merge(*(aOtherLine.iParentInfo));
			}

		if(!iAttributeInfo)
			{
			// must be screen...
			iAttributeInfo = new TMLCompDataAttributeInfo();
			}
		if(aOtherLine.iAttributeInfo)
			{
			iAttributeInfo->Merge(*(aOtherLine.iAttributeInfo));
			}

		// for the API, we need to know if there are any multi-value components in either orientation
		iNeedsCols = iNeedsCols || aOtherLine.iNeedsCols;
		iNeedsRows = iNeedsRows || aOtherLine.iNeedsRows;
		// however, we want exactly the correct number of calcs from the other cell
		iNumCols = aOtherLine.iNumCols;
		iNumRows = aOtherLine.iNumRows;

		// if this line has no values, then we must do a mirror merge
		bool mirrorMerge = empty() && iIsMirroredHorizontally;

		// create missing values in this line if needed
		for (TMLCompDataLine::iterator pOtherValues=aOtherLine.begin(); pOtherValues!=aOtherLine.end(); ++pOtherValues)
			{
			string index = pOtherValues->first;
			if(mirrorMerge)
				{
				// flip left and right
				if(index == KCompDataCellNameLeft)
					index = KCompDataCellNameRight;
				else if(index == KCompDataCellNameRight)
					index = KCompDataCellNameLeft;
				}

			(*this)[index].Merge(this, index, pOtherValues->second, mirrorMerge);
			}
		}
	return compatible;
	}

TMLAttributeZoomLevels* TMLCompDataLine::GetAttributeZoomLevels(string aAttribName, int aVariety)
	{
	TMLAttributeZoomLevels* found = 0;
	TMLCompData& data = *(iParentTable->iTables);
	TMLAttributes& attributes = *(data.iAttributes);
	int attribId = attributes.iNames[aAttribName];

	if(attribId == 0)
		throw GeneralErr(string("Attribute name not found: ") + aAttribName);
	// find out from attribute info which attribute set we need
	// but if there is none specified, we don't need to search
	if(iAttributeInfo)
		{
		TMLCompDataAttributeInfoSelector& selector = (*iAttributeInfo)[aVariety];
		// go to parent straight away, as parent always stores attribute data for its children
		found = GetParentAttributeZoomLevels(selector.iAttributeSetName, attribId, aVariety);
		}
	return found;
	}

TMLAttributeZoomLevels* TMLCompDataLine::GetParentAttributeZoomLevels(string aAttribSetName, int aAttribId, int aVariety)
	{
	TMLAttributeZoomLevels* found = NULL;
	TMLCompDataParentInfo::iterator pFoundSelector = iParentInfo->find(aVariety);
	if(pFoundSelector != iParentInfo->end())
		{
		const TMLCompDataParentInfoSelector& parentInfoSelector = pFoundSelector->second;
		if(iParentTable && iParentTable->iParentLine)
			{
			found = iParentTable->iParentLine->FindAttributeZoomLevels(aAttribSetName, aAttribId);
			if(!found)
				{
				// recurse to next parent container
				int variety = parentInfoSelector.iParentVariety;
				iParentTable->iParentLine->GetParentAttributeZoomLevels(aAttribSetName, aAttribId, variety);
				}
			}
		}
	return found;
	}

TMLAttributeZoomLevels* TMLCompDataLine::FindAttributeZoomLevels(string aAttribSetName, int aAttribId)
	{
	TMLCompData& data = *(iParentTable->iTables);
	TMLAttributes& attributes = *(data.iAttributes);
	int id = iId;
	TMLAttributes::iterator foundAttributeSetComponent = attributes.find(id);
	if(foundAttributeSetComponent != attributes.end())
		{
		TMLAttributeSetComponent& component = foundAttributeSetComponent->second;
		TMLAttributeSet* pSet = component[aAttribSetName];
		if(pSet)
			{
			TMLAttributeSet& attributeSet = *pSet;
			TMLAttributeSet::iterator foundAttrib = attributeSet.find(aAttribId);
			if(foundAttrib != attributeSet.end())
				{
				return &(foundAttrib->second);
				}
			}
		}
	return NULL;
	}

void TMLCompDataLine::Compile()
	{
	// compile values
	for(iterator pVal = begin(); pVal != end(); ++pVal)
		{
		(pVal->second).Compile(pVal->first);
		}
	CompileParamLimits(TMLCompDataValues::ECellTypeCol, KCompDataKeywordParamNumCols);
	CompileParamLimits(TMLCompDataValues::ECellTypeRow, KCompDataKeywordParamNumRows);
	CompileFontHeights();
	}

void TMLCompDataLine::CompileParamLimits(TMLCompDataValues::TCompDataCellType aParamLimitType, const string& aParamLimitCellName)
	{
	TMLCompDataValues paramLimits(this);

	for(iterator pValues = begin(); pValues != end(); ++pValues)
		{
		string cellName = pValues->first;
		if(TMLCompDataValues::Type(cellName) == aParamLimitType)
			{
			// calculate the param limits for this cell
			TMLCompDataValues& values = pValues->second;
			for(TMLCompDataValues::iterator pZoomLevels = values.begin(); pZoomLevels != values.end(); ++pZoomLevels)
				{
				int nextVariety = pZoomLevels->first;
				TMLCompDataZoomLevels& zoomLevels = pZoomLevels->second;
				TMLCompDataZoomLevels& paramLimitsZoomLevels = paramLimits[nextVariety];
				for(TMLCompDataZoomLevels::iterator pCalcs = zoomLevels.begin(); pCalcs != zoomLevels.end(); ++pCalcs)
					{
					// accumulate the largest size of calc into the param limit
					int zoomLevel = pCalcs->first;
					int nextParamLimit = pCalcs->second.size();
					TMLCompDataCalcs& paramLimitCalcs = paramLimitsZoomLevels[zoomLevel];
					string& paramLimit = paramLimitCalcs[0];
					int currentParamLimit = CdlTkUtil::ParseInt(paramLimit);
					if(nextParamLimit > currentParamLimit)
						paramLimit = CdlTkUtil::IntToString(nextParamLimit);
					}
				if(!paramLimitsZoomLevels.size())
					{
					// there were no defined values for this variety, but we know we'll
					// insert an empty value at least
					TMLCompDataCalcs& paramLimitCalcs = paramLimitsZoomLevels[EAknUiZoomNormal];
					paramLimitCalcs.insert(make_pair(0, CdlTkUtil::IntToString(1)));
					}
				}
			}
		}
	insert(make_pair(aParamLimitCellName, paramLimits));
	}

void TMLCompDataLine::CompileFontHeights()
	{
	if(iType == ETextComponent)
		{
		TMLCompDataValues font(this);

		TMLCompDataValues types = (*this)[KCompDataKeywordParamType];
		TMLCompDataValues heights = (*this)[KCompDataKeywordParamHeight];
		
		// note that these are the number of varieties, there may be zoom level and then rows inside
		int numTypeVarieties = types.size();
		int numHeightVarieties = heights.size();

		for(TMLCompDataValues::iterator pZoomLevels = heights.begin(); pZoomLevels != heights.end(); ++pZoomLevels)
			{
			int nextVariety = pZoomLevels->first;
			TMLAttributeZoomLevels* style1ZoomLevels = GetAttributeZoomLevels(KAttributeNameStyle1, nextVariety);
			TMLAttributeZoomLevels* style3ZoomLevels = GetAttributeZoomLevels(KAttributeNameStyle3, nextVariety);
			TMLCompDataZoomLevels& heightsZoomLevels = pZoomLevels->second;
			TMLCompDataZoomLevels& typesZoomLevels = types[nextVariety];
			TMLCompDataZoomLevels& fontZoomLevels = font[nextVariety];
			
			for(TMLCompDataZoomLevels::iterator pHeightCalcs = heightsZoomLevels.begin(); pHeightCalcs != heightsZoomLevels.end(); ++pHeightCalcs)
				{
				// if the types don't have zoom data, fall back to normal zoom
				int zoomIndex = pHeightCalcs->first;
				int numTypesZoomLevels = typesZoomLevels.size();
				int typeZoomIndex = numTypesZoomLevels > 1 ? zoomIndex : EAknUiZoomNormal; 
				TMLCompDataCalcs& typeCalcs = typesZoomLevels[typeZoomIndex];

				// get attribute data
				int outline = 0;
				int posture = 0;
				int weight =  0;
				if(style1ZoomLevels)
					{
					string style1 = (*style1ZoomLevels)[pHeightCalcs->first];
					if(style1 == KAttributeNameStyle1Bold)
						weight = 1;
					}
				if(style3ZoomLevels)
					{
					string style3 = (*style3ZoomLevels)[pHeightCalcs->first];
					if(style3 == KAttributeNameStyle3Outline)
						outline = 1;
					}

				// we want to have a font id for each height, even if the spec
				// has not specified the type each time
				TMLCompDataCalcs& fontCalcs = fontZoomLevels[pHeightCalcs->first];
				TMLCompDataCalcs& next = pHeightCalcs->second;
				for(TMLCompDataCalcs::iterator pHeightCalc = next.begin(); pHeightCalc != next.end(); ++pHeightCalc)
					{
					// for undefined type, let font provider use default, 
					// and always choose the first type for a multi value item
					int calcIndex = pHeightCalc->first;
					int type = numTypeVarieties > 0 ? CdlTkUtil::ParseInt(typeCalcs[0]) : ELayoutCompilerFontCategoryUndefined;
					int height = numHeightVarieties > 0 ? CdlTkUtil::ParseInt(pHeightCalc->second) : 0; 
					int fontId = EncodeFontId(height, outline, posture, weight, type);
					fontCalcs[calcIndex] = CdlTkUtil::IntToString(fontId);
					}
				}
			}
		insert(make_pair(string(KCompDataKeywordParamFont), font));
		}
	}

int TMLCompDataLine::EncodeFontId(int aHeight, int aOutline, int aPosture, int aWeight, int aCategory) const
	{
	//
	// 31           |                           30 - 21                               |          20 - 07                |     06      |     05      |      04      |      03 - 00
	// encoded |                text pane height 0-1023               |        <not used>           |  outline |  posture |  weight   |      category              

    int encodedMasked = 1 << 31;
    int heightMasked = aHeight << 21;

	int outlineMasked = aOutline << 6;
	int postureMasked = aPosture << 5;
	int weightMasked = aWeight << 4;
    int categoryMasked = aCategory;

    return(encodedMasked | heightMasked | outlineMasked | postureMasked | weightMasked | categoryMasked);
	}

bool TMLCompDataLine::MatchParams(const TMLCompDataLine& aLine) const
	{
	if (NeedsOptions() != aLine.NeedsOptions())
		return false;
	if (NeedsCols() != aLine.NeedsCols())
		return false;
	if (NeedsRows() != aLine.NeedsRows())
		return false;
	return true;
	}

bool TMLCompDataLine::MatchNameDiscountingSuffix(const TMLCompDataLine& aLine) const
	{
	// we are trying to compare whether the names are the same apart from a trailing number
	string pureName = NameDiscountingSuffix();
	string pureNameOther = aLine.NameDiscountingSuffix();
	string ending = pureName.substr(pureName.find_last_not_of(KCompDataKeywordUnderscore));

	bool namesMatch = (pureName == pureNameOther);
	bool correctEnding = (ending == KCompDataKeywordLineNameSuffixGraphic || ending == KCompDataKeywordLineNameSuffixText);
	return (namesMatch && correctEnding);
	}

bool TMLCompDataLine::MatchType(const TMLCompDataLine& aLine) const
	{
	// first check that the type is equivalent
	bool equivalent = false;
	switch(iType)
		{
		case ETextComponent:
			{
			if(aLine.iType == ETextComponent)
				{
				equivalent = true;
				}
			break;
			}
		case EScreenComponent:
		case EContainerComponent:
		case EPaneComponent:
		case EGraphicComponent:
			{
			if(aLine.iType == EScreenComponent
				|| aLine.iType == EContainerComponent
				|| aLine.iType == EPaneComponent
				|| aLine.iType == EGraphicComponent)
				{
				equivalent = true;
				}
			break;
			}
		case EUnknownComponent:
		default:
			{
			if(aLine.iType == EUnknownComponent)
				{
				equivalent = true;
				}
			break;
			}
		}

	return equivalent;
	}

string TMLCompDataLine::NameDiscountingSuffix() const
	{
	int lastNonNumericPos = iName.find_last_not_of(KCompDataSearchCollectionNumeric);
	int length = lastNonNumericPos + 1;
	return iName.substr(0, length);
	}

string TMLCompDataLine::NameSuffix() const
	{
	int lastNonNumericPos = iName.find_last_not_of(KCompDataSearchCollectionNumeric);
	int suffixPos = lastNonNumericPos + 1;
	return iName.substr(suffixPos);
	}

bool TMLCompDataLine::NeedsParams() const 
	{ 
	return iNeedsOptions || iNeedsCols || iNeedsRows; 
	}

bool TMLCompDataLine::NeedsOptions() const 
	{
	return iNeedsOptions; 
	}

bool TMLCompDataLine::NeedsCols() const 
	{
	return iNeedsCols; 
	}

bool TMLCompDataLine::NeedsRows() const 
	{
	return iNeedsRows; 
	}

int TMLCompDataLine::MaxVariety() const
	{
	return iMaxVariety;
	}

int TMLCompDataLine::NumCols() const
	{
	return iNumCols;
	}

int TMLCompDataLine::NumRows() const
	{
	return iNumRows;
	}


void TMLCompDataLine::SetMaxVariety(int aMaxVariety)
	{
	iMaxVariety = aMaxVariety;
	if(iMaxVariety > 0) // zero based
		iNeedsOptions = true;
	}

void TMLCompDataLine::SetNumCols(int aNumCols)
	{
	iNumCols = aNumCols;
	if(iNumCols > 1)
		iNeedsCols = true;
	}

void TMLCompDataLine::SetNumRows(int aNumRows)
	{
	iNumRows = aNumRows;
	if(iNumRows > 1)
		iNeedsRows = true;
	}

void TMLCompDataLine::SetNeedsCols(bool aNeeds)
    {
    iNeedsCols = aNeeds;    
    }

void TMLCompDataLine::SetNeedsRows(bool aNeeds)
    {
    iNeedsRows = aNeeds;
    }

//
// TMLCompDataAttributeInfoSelector
//
TMLCompDataAttributeInfoSelector::TMLCompDataAttributeInfoSelector()
	{

	}

TMLCompDataAttributeInfoSelector::TMLCompDataAttributeInfoSelector(string aAttributeSetName)
	:
	iAttributeSetName(aAttributeSetName)
	{

	}

//
//  TMLCompDataAttributeInfo
//

TMLCompDataAttributeInfo::TMLCompDataAttributeInfo()
	:
	iLine(0)
	{

	}

TMLCompDataAttributeInfo::TMLCompDataAttributeInfo(TMLCompDataLine* aLine)
	:
	iLine(aLine)
	{

	}

TMLCompDataAttributeInfo::~TMLCompDataAttributeInfo()
	{

	}

void TMLCompDataAttributeInfo::Merge(const TMLCompDataAttributeInfo& aOther)
	{
	for (const_iterator pOtherVariety = aOther.begin(); pOtherVariety != aOther.end(); ++pOtherVariety)
		{
		int varietyIndex = pOtherVariety->first;
		const TMLCompDataAttributeInfoSelector& selector = pOtherVariety->second;
		insert(make_pair(varietyIndex, selector));
		}
	}



//
// TMLCompDataTable::TMLCompDataSubTable
//

bool TMLCompDataTable::TMLCompDataSubTable::NeedsParams() const 
	{ 
	return iNeedsOption || iNeedsCol || iNeedsRow; 
	}


//
// TMLCompDataTable
//

TMLCompDataTable::TMLCompDataTable(TMLCompData* aTables)
	: 
	iId(0),
	iTables(aTables), 
	iParentLine(NULL),
	iNeedsP(false),
	iNeedsIndex(false),
	iAppend(false),
	iFirstLineGlobalIndex(-1)
	{
	}

TMLCompDataTable::TMLCompDataTable(TMLCompData* aTables, const TMLCompDataTable& aOther)
	: 
	iId(aOther.iId),
	iName(aOther.iName),
	iTables(aTables),
	iColumnNames(aOther.iColumnNames),
	iParentLine(NULL), 
	iParentName(aOther.iParentName),
	iAppend(aOther.iAppend),
	iFirstLineGlobalIndex(aOther.iFirstLineGlobalIndex)

	{
	for (const_iterator pLine = aOther.begin(); pLine != aOther.end(); ++pLine)
		push_back(new TMLCompDataLine(**pLine));
	}

TMLCompDataTable::~TMLCompDataTable()
	{
	for (iterator pLine = begin(); pLine != end(); ++pLine)
		delete *pLine;
	}

bool TMLCompDataTable::lessthan(TMLCompDataTable* aLeft, TMLCompDataTable* aRight)
	{
	return (aLeft->iId) < (aRight->iId);
	}

string TMLCompDataTable::Name()
	{
	return iName;
	}

TMLCompDataLine* TMLCompDataTable::FindLine(const string& aName)
	{
	for (iterator pLine = begin(); pLine != end(); ++pLine)
		{
		TMLCompDataLine& line = **pLine;
		string paramLimitsName = MLCompDataToCdl::LineParamLimitsApiName(line);
		// first check the lines for a direct match
		// then try the param limits instead
		if (line.Name() == aName || 
			(line.NeedsParams() && paramLimitsName == aName))
			return *pLine;
		}
	return 0;
	}

TMLCompDataTable::TMLCompDataSubTable* TMLCompDataTable::FindSubTable(const string& aName)
	{
	for(TMLCompDataSubTables::iterator pSub = iSubTables.begin(); pSub != iSubTables.end(); ++pSub)
		{
		TMLCompDataSubTable& sub = **pSub;
		string subTableName = MLCompDataToCdl::SubTableApiName(sub);
		string subTableLimitsName = MLCompDataToCdl::SubTableLimitsApiName(sub);
		string paramLimitsName = MLCompDataToCdl::SubTableParamLimtsApiName(sub);
		
		// first check the lines for a direct match
		// then try the param limits instead
		if (subTableName == aName ||
			subTableLimitsName == aName ||
			(sub.NeedsParams() && paramLimitsName == aName)) // need to check whether the subtable needs params
			return &sub;
		}
	return 0;
	}

void TMLCompDataTable::Merge(TMLCompDataTable& aOther)
	{
	for (iterator pOtherLine = aOther.begin(); pOtherLine != aOther.end(); )
		{
        TMLCompDataLine* found = FindLine((*pOtherLine)->Name());
        if(found)
            {
            found->Merge(**pOtherLine);
            delete *pOtherLine;
			pOtherLine = aOther.erase(pOtherLine);
			}
		else
            {
			push_back(*pOtherLine);
			(*pOtherLine)->iParentTable = this;
			pOtherLine = aOther.erase(pOtherLine);
			}
		}
	}


void TMLCompDataTable::Compile()
	{
	SetDefaultColumnNames();
	sort(begin(), end(), TMLCompDataLine::lessthan);	

	iNeedsIndex = false;
	iNeedsP = false;
	for (iterator pLine = begin(); pLine != end(); ++pLine)
		{
		(*pLine)->Compile();
		}
	BuildSubTables();
	NormalizeSubTables();
	}

void TMLCompDataTable::BuildSubTables()
	{
	DestroySubTables();

	int count = size();
	if(count > 0)
		{
		TMLCompDataSubTable* subTable = 0;
		for (int i=0; i<count; i++)
			{
			TMLCompDataLine& line = *(*this)[i];
			if (subTable)
				{
				TMLCompDataLine* firstLine = (*this)[(*subTable)[0]];
				if (firstLine->NameSuffix() == "1" && firstLine->MatchNameDiscountingSuffix(line) && firstLine->MatchType(line))
					{
					subTable->iName = line.NameDiscountingSuffix();
					}
				else // only terminate the subtable if the lines don't match, or if the first line isn't numbered "1"
					{
					if (subTable->size() > 1)
						iSubTables.push_back(subTable);
					else
						delete subTable;
					subTable = new TMLCompDataSubTable;
					}
				}
			else
				{
				subTable = new TMLCompDataSubTable;
				}
			subTable->iNeedsOption |= line.NeedsOptions();
			subTable->iNeedsCol |= line.NeedsCols();
			subTable->iNeedsRow |= line.NeedsRows();
			subTable->push_back(i);
			}

		if (subTable->size() > 1) 
			{
			iSubTables.push_back(subTable);
			}
		else
			delete subTable;
		}
	}

void TMLCompDataTable::NormalizeSubTables()
	{
	for(TMLCompDataSubTables::iterator pSub = iSubTables.begin(); pSub != iSubTables.end(); ++pSub)
		{
		TMLCompDataSubTable& sub = **pSub;
		for(TMLCompDataSubTable::iterator pLineId = sub.begin(); pLineId != sub.end(); ++pLineId)
			{
			TMLCompDataLine& line = *((*this)[*pLineId]);

			line.iNeedsOptions |= sub.iNeedsOption;
			line.iNeedsCols |= sub.iNeedsCol;
			line.iNeedsRows |= sub.iNeedsRow;
			}
		}
	}


void TMLCompDataTable::DestroySubTables()
	{
	for (TMLCompDataSubTables::iterator pSub = iSubTables.begin(); pSub != iSubTables.end(); ++pSub)
		delete *pSub;
	iSubTables.clear();
	}


const string KValueNames[] = { "Font", "C", "l", "r", "W", "J", "t", "r", "b", "H" };
const set<string> KValueNamesSet(KValueNames, ARRAY_END(KValueNames));

bool TMLCompDataTable::IsValueColumn(string aName)
	{
	return KValueNamesSet.find(aName) != KValueNamesSet.end();
	}

const string KNumericNames[] = { "C", "l", "r", "W", "t", "r", "b", "H" };
const set<string> KNumericNamesSet(KNumericNames, ARRAY_END(KNumericNames));

bool TMLCompDataTable::IsNumericColumn(string aName)
	{
	return KNumericNamesSet.find(aName) != KNumericNamesSet.end();
	}

const string KHorizontalColumnNames[] = { "l", "r", "W"};
const set<string> KHorizontalNamesSet(KHorizontalColumnNames, ARRAY_END(KHorizontalColumnNames));

bool TMLCompDataTable::IsHorizontalColumn(string aName)
	{
	return KHorizontalNamesSet.find(aName) != KHorizontalNamesSet.end();
	}

const string KVerticalColumnNames[] = {"t", "b", "H" };
const set<string> KVerticalNamesSet(KVerticalColumnNames, ARRAY_END(KVerticalColumnNames));

bool TMLCompDataTable::IsVerticalColumn(string aName)
	{
	return KVerticalNamesSet.find(aName) != KVerticalNamesSet.end();
	}


const string KPaneColumnNames[] = {"Item", "C", "l", "t", "r", "b", "W", "H", "Remarks"};

void TMLCompDataTable::SetDefaultColumnNames()
	{
	iColumnNames.clear();
    iColumnNames.insert(iColumnNames.end(), KPaneColumnNames, ARRAY_END(KPaneColumnNames));
	}

TMLCompDataTable::TMLCompDataSubTable::TMLCompDataSubTable()
: 
	iNeedsOption(false),
	iNeedsCol(false),
	iNeedsRow(false)
	{
	}


//
// TMLCompData
//

TMLCompData::TMLCompData()
	: 
	iCanBeMirror(false),
	iIsBaseInstance(false),
	iAttributes(0)
	{
	}

TMLCompData::TMLCompData(const TMLCompData& aOther)
	{
	*this = aOther;
	}

TMLCompData& TMLCompData::operator=(const TMLCompData& aOther)
	{
	if (this != &aOther)
		{
		iName = aOther.iName;
		iCanBeMirror = aOther.iCanBeMirror;
		for (const_iterator pTab = aOther.begin(); pTab != aOther.end(); ++pTab)
			push_back(new TMLCompDataTable(this, **pTab));
		Compile();
		}
	return *this;
	}

TMLCompData::~TMLCompData()
	{
	for (iterator pTab = begin(); pTab != end(); ++pTab)
		delete *pTab;
	DeleteComponents();
	delete iAttributes;
	}

TMLCompDataLine* TMLCompData::FindComponent(const string& aName) const
	{
	for (TMLComponents::const_iterator pComp = iComponents.begin(); pComp != iComponents.end(); ++pComp)
		{
		TMLCompDataLine* line = pComp->second;
		if (line->Name() == aName)
			return line;
		}

	return 0;
	}

TMLCompDataLine* TMLCompData::FindLine(const string& aName) const
	{
	for (const_iterator pTab = begin(); pTab != end(); ++pTab)
		{
		TMLCompDataLine* line = (*pTab)->FindLine(aName);
		if (line)
			return line;
		}

	return 0;
	}

TMLCompDataTable* TMLCompData::FindTable(const string& aName) const
	{
	for (const_iterator pTab = begin(); pTab != end(); ++pTab)
		{
		if ((*pTab)->Name() == aName)
			return *pTab;
		}

	return 0;
	}

TMLCompDataTable* TMLCompData::FindTable(int aId) const
	{
	for (const_iterator pTab = begin(); pTab != end(); ++pTab)
		{
		if ((*pTab)->iId == aId)
			return *pTab;
		}

	return 0;
	}

TMLCompDataTable::TMLCompDataSubTable* TMLCompData::FindSubTable(const string& aName) const
	{
	for (const_iterator pTab = begin(); pTab != end(); ++pTab)
		{
		TMLCompDataTable::TMLCompDataSubTable* sub = (*pTab)->FindSubTable(aName);
		if (sub)
			return sub;
		}

	return 0;
	}


void TMLCompData::Merge(TMLCompData& aOther)
	{
	iName = aOther.iName;
	iCanBeMirror |= aOther.iCanBeMirror; // in the case of an elaf layout merging onto an abrw layout, the chirality will be preserved
	MergeComponents(aOther);
	}

void TMLCompData::MergeComponents(TMLCompData& aOther)
    {
	for (TMLComponents::iterator pOtherLine = aOther.iComponents.begin(); pOtherLine != aOther.iComponents.end(); ++pOtherLine)
		{
		TMLCompDataLine& otherLine = *(pOtherLine->second);
        TMLCompDataLine* found = FindComponent(otherLine.iName);
        if(found)
			{
			found->Merge(otherLine);
			}
		else
			{
			TMLCompDataLine* newLine = new TMLCompDataLine(otherLine);
			iComponents.insert(make_pair(otherLine.iId, newLine));
			}
		}
	}

void TMLCompData::Compile()
	{
	CreateTables();

	// now add a special table for the screen contents
	TMLCompDataTable* topTab = new TMLCompDataTable(this);
	topTab->iId = -1;
	topTab->iName = KCompDataKeywordScreenContents;

	// then insert each line into its parent
	for (TMLComponents::iterator pComp2 = iComponents.begin(); pComp2 != iComponents.end(); ++pComp2)
		{
		TMLCompDataLine& line = *(pComp2->second);
		if(line.iType == TMLCompDataLine::EScreenComponent)
			{
			line.iParentTable = topTab;
			topTab->push_back(&line);
			}
		else
			{
			bool parentFound = false;
			int parentId = 0;
			if(line.iParentInfo)
				{
				TMLCompDataParentInfo& parentInfo = *(line.iParentInfo);
				TMLCompDataParentInfoSelector& selector = (parentInfo.begin())->second; // we ignore the varieties for now
				parentId = selector.iParentId;
				TMLCompDataTable* parentTable = FindTable(parentId);

				if(parentTable)
					{
					line.iParentTable = parentTable;
					// copy the pointer from the components table
					parentTable->push_back(&line);
					parentFound = true;
					// now insert the table into its place in the tree
					parentTable->iParentLine = iComponents[parentId];
					}
				else
					{
					parentFound = false;
					}
				}
			if(!parentFound)
				{
                string errorText = string(" TMLCompData::Compile() - can't find parent component: ");
				errorText += CdlTkUtil::IntToString(parentId);
				throw GeneralErr(errorText);
				}
			}		
		}	
	push_back(topTab);
	
	// remember not to delete the components, as we have taken copies!
	iComponents.clear();

	// now compile the tables
	iterator pTab;
	for (pTab = begin(); pTab != end(); ++pTab)
		(*pTab)->Compile();

	// now sort the tables
	sort(begin(), end(), TMLCompDataTable::lessthan);	
	}

void TMLCompData::CreateTables()
	{
	// from the list of components, first create a table for each pane
	for (TMLComponents::iterator pComp = iComponents.begin(); pComp != iComponents.end(); ++pComp)
		{
		TMLCompDataLine& line = *(pComp->second);
		switch(line.iType)
			{
			case TMLCompDataLine::EScreenComponent:
			case TMLCompDataLine::EContainerComponent:
			case TMLCompDataLine::EPaneComponent:
				{
				TMLCompDataTable* tab = new TMLCompDataTable(this);
				tab->iId = line.iId;
				tab->iName = line.iName;
				push_back(tab);
				break;
				}
			case TMLCompDataLine::EGraphicComponent:
			case TMLCompDataLine::ETextComponent:
				{
				// text and graphic components are not panes 
				// and are therefore not represented in our internal object model as tables
				break;
				}
			default:
				{
				cout << "TMLCompData::CreateTables() - uncategorised component\n";
				break;
				}
			}	
		}
	}

void TMLCompData::DeleteComponents()
	{
	for (TMLComponents::iterator pComp = iComponents.begin(); pComp != iComponents.end(); ++pComp)
		delete pComp->second;
	}

// End of File
