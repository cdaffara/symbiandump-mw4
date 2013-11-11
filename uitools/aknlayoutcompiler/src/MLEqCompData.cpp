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


#include "MLEqCompData.h"
#include "MLEqCompDataParse.h"
#include "MLEqCompData2DHuiML.h"
#include "MLAttributes.h"
#include "FormulaTree.h"

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



const string KEqCompDataKeywordParamLeft = "l";
const string KEqCompDataKeywordParamTop = "t";
const string KEqCompDataKeywordParamRight = "r";
const string KEqCompDataKeywordParamBottom = "b";
const string KEqCompDataKeywordParamWidth = "w";
const string KEqCompDataKeywordParamHeight = "h";

const string KEqCompDataPaneOutputOrder[] = 
	{
	KEqCompDataKeywordParamLeft,
	KEqCompDataKeywordParamTop,
	KEqCompDataKeywordParamRight,
	KEqCompDataKeywordParamBottom,
	KEqCompDataKeywordParamWidth,
	KEqCompDataKeywordParamHeight
	};

const int KEqCompDataPaneOutputOrderSize = ARRAY_LEN(KEqCompDataPaneOutputOrder);


const string KEqCompDataKeywordParamType = "Type";
const string KEqCompDataKeywordParamFont = "Font";
const string KEqCompDataKeywordParamNumCols = "NumCols";
const string KEqCompDataKeywordParamNumRows = "NumRows";
const string KEqCompDataKeywordScreenContents = "Screen Contents";

const string KEqCompDataKeywordUnderscore = "_";
const string KEqCompDataKeywordLineNameSuffixGraphic = "g";
const string KEqCompDataKeywordLineNameSuffixText = "t";

const string KEqCompDataSearchCollectionNumeric = "0123456789";
const string KEqCompDataBadValue = "Bad Value";
const string KEqCompDataLayoutEmpty = "ELayoutEmpty";
const string KEqCompDataUnknown = "unknown";

const string KEqCompDataCellNameLeft("l");
const string KEqCompDataCellNameRight("r");
const string KEqCompDataCellNameJustification("J");

const string KAttributeNameStyle1 = "style_1";
const string KAttributeNameStyle1Plain = "plain";
const string KAttributeNameStyle1Bold = "bold";

const string KAttributeNameNumberOfColumns = "Number_of_columns";
const string KAttributeNameNumberOfRows = "Number_of_rows";
const string KAttributeNameNumberOfRowsColsAuto = "AUTO";

//
// struct TMLEqCompDataFormula
//
TMLEqCompDataFormula::TMLEqCompDataFormula()
	:
	iFormulaString(),
	iZoomId(0),
	iFormulaTree(NULL)
	{

	}

TMLEqCompDataFormula::TMLEqCompDataFormula(const TMLEqCompDataFormula& aOther)
	:
	iFormulaString(aOther.iFormulaString),
	iZoomId(aOther.iZoomId),
	iFormulaTree(aOther.iFormulaTree)
	{

	}

const TMLEqCompDataFormula& TMLEqCompDataFormula::operator=(const TMLEqCompDataFormula& aOther)
	{
	if(this == &aOther)
		return aOther;

	iFormulaString = aOther.iFormulaString;
	iZoomId = aOther.iZoomId;

	if(iFormulaTree)
		{
		delete iFormulaTree;
		iFormulaTree = 0;
		}
	iFormulaTree = aOther.iFormulaTree;
	return *this;
	}

TMLEqCompDataFormula::TMLEqCompDataFormula(string aFormulaString)
	:
	iFormulaString(aFormulaString),
	iZoomId(0),
	iFormulaTree(NULL)
	{

	}

TMLEqCompDataFormula::~TMLEqCompDataFormula()
	{
	delete iFormulaTree;
	iFormulaTree = 0;
	}


bool TMLEqCompDataFormula::operator==(const TMLEqCompDataFormula& aOther) const
	{
	return (iFormulaString == aOther.iFormulaString && iZoomId == aOther.iZoomId);
	}

void TMLEqCompDataFormula::Compile()
	{
	if(!iFormulaTree)
		{
		iFormulaTree = FormulaTreeNode::Parse(iFormulaString);
		}
	else
		{
		throw GeneralErr(string("TMLEqCompDataFormula::Compile - formula already parsed."));
		}
	}

//
// TMLEqCompDataValuesOptionSet
//
void TMLEqCompDataValuesOptionSet::Compile()
	{
	for(iterator pFormula = begin(); pFormula != end(); ++pFormula)
		{
		pFormula->Compile();
		}
	}


//
// TMLEqCompDataValues
//

TMLEqCompDataValues::TMLEqCompDataValues()
	: 
	iLine(NULL), 
	iName(KEqCompDataBadValue)
	{
	}

TMLEqCompDataValues::TMLEqCompDataValues(TMLEqCompDataLine* aLine)
	: 
	iLine(aLine), 
	iName(aLine->iName)
	{
	}

bool TMLEqCompDataValues::operator==(const TMLEqCompDataValues& aOther) const
	{
	typedef const vector<TMLEqCompDataFormula> TBase;
	bool eq =
		iName == aOther.iName &&
		(*static_cast<TBase*>(this) == aOther);
	return eq;
	}

TMLEqCompDataValues::~TMLEqCompDataValues()
	{
	}

bool TMLEqCompDataValues::Merge(TMLEqCompDataLine* aLine, string aName, TMLEqCompDataValues& aOtherValues, bool aMirrorMerge)
	{
/*
	iLine = aLine;
	iName = aName; // we may be swapping l and r

	// create missing values in this line if needed
	for (iterator pOtherVal=aOtherValues.begin(); pOtherVal!=aOtherValues.end(); ++pOtherVal)
		{
		TMLEqCompDataZoomLevels& otherZoomLevels = pOtherVal->second;
		TMLEqCompDataZoomLevels& thisZoomLevels = (*this)[pOtherVal->first];
		thisZoomLevels = otherZoomLevels; // we want exactly the correct number of calcs from the other cell
		if(aMirrorMerge)
			{
			if(iName == KEqCompDataCellNameJustification)
				{
				// reverse the justification
				for(TMLEqCompDataZoomLevels::iterator pCalcs = thisZoomLevels.begin(); pCalcs != thisZoomLevels.end(); ++pCalcs)
					{
					TMLEqCompDataCalcs& calcs = pCalcs->second;
					for(TMLEqCompDataCalcs::iterator pVal = calcs.begin(); pVal != calcs.end(); ++pVal)
						pVal->second = MirrorJustificationValue(pVal->second);
					}
				}
			}
		}
*/
	return true;
	}


const string KParamLimitNames[] = { "NumCols", "NumRows" };
const string KHorizNames[] = { "l", "r", "W" };
const string KVertNames[] = { "t", "b", "H", "Font" };
const set<string> KParamLimitNamesSet(KParamLimitNames, ARRAY_END(KParamLimitNames));
const set<string> KHorizNamesSet(KHorizNames, ARRAY_END(KHorizNames));
const set<string> KVertNamesSet(KVertNames, ARRAY_END(KVertNames));

string TMLEqCompDataValues::MirrorJustificationValue(const string& aValue)
	{
	int val = CdlTkUtil::ParseInt(aValue);
	if(val == ELayoutAlignLeft)
		val = ELayoutAlignRight;
	else if(val == ELayoutAlignRight)
		val = ELayoutAlignLeft;
	return CdlTkUtil::IntToString(val);
	}


void TMLEqCompDataValues::Compile(int aOptionSetId)
	{
	TMLEqCompDataValuesOptionSet& set = iOptionSets[aOptionSetId];
	set.Compile();
	}

string TMLEqCompDataValues::CppValue(const string& aValue)
	{
	if (aValue.size())
		return aValue;
	else
		return KEqCompDataLayoutEmpty;
	}

//
// TMLEqCompDataParentInfoSelector
//
TMLEqCompDataParentInfoSelector::TMLEqCompDataParentInfoSelector()
	{

	}

TMLEqCompDataParentInfoSelector::TMLEqCompDataParentInfoSelector(int aParentId, int aParentVariety)
	:
	iParentId(aParentId),
	iParentVariety(aParentVariety)
	{

	}

//
//  TMLEqCompDataParentInfo
//

TMLEqCompDataParentInfo::TMLEqCompDataParentInfo()
	:
	iLine(0)
	{

	}

TMLEqCompDataParentInfo::TMLEqCompDataParentInfo(TMLEqCompDataLine* aLine)
	:
	iLine(aLine)
	{

	}

TMLEqCompDataParentInfo::~TMLEqCompDataParentInfo()
	{

	}

void TMLEqCompDataParentInfo::Merge(const TMLEqCompDataParentInfo& aOther)
	{
/*
	for (const_iterator pOtherVariety = aOther.begin(); pOtherVariety != aOther.end(); ++pOtherVariety)
		{
		int varietyIndex = pOtherVariety->first;
		const TMLEqCompDataParentInfoSelector& selector = pOtherVariety->second;
		insert(make_pair(varietyIndex, selector));
		}
*/
	}

//
//  TMLEqCompDataLine
//

TMLEqCompDataLine::TMLEqCompDataLine()
:	iId(0),
	iName(KEqCompDataUnknown),
	iType(EUnknownComponent),
	iDrawingOrder(-1),
	iParentTable(0),
	iParentInfo(0),
	iIsUnique(true),
	iGlobalIndex(0),
	iIsMirroredHorizontally(false),
//	iAttributeInfo(0),
	iNumCols(1),
	iNumRows(1)

	{	
		
	}

TMLEqCompDataLine::TMLEqCompDataLine(const TMLEqCompDataLine& aOther)
	{
	if(this == &aOther)
		return;

/*
	*this = aOther; // this will take a copy of the owning pointer
	if(aOther.iParentInfo) // if it wasn't zero
		iParentInfo = new TMLEqCompDataParentInfo(*(aOther.iParentInfo)); // we don't want to take ownership, so make our own copy
	if(aOther.iAttributeInfo) // if it wasn't zero
		iAttributeInfo = new TMLEqCompDataAttributeInfo(*(aOther.iAttributeInfo)); // we don't want to take ownership, so make our own copy
*/
	for (iterator pVal = begin(); pVal != end(); ++pVal)
		{
		TMLEqCompDataValues& val = pVal->second;
		val.iLine = this;
		}
	iParentTable = 0; // will be set during compile
	}

bool TMLEqCompDataLine::operator==(const TMLEqCompDataLine& aOther) const
	{
	return (Name() == aOther.Name()) && ValuesEqual(aOther);
	}

TMLEqCompDataLine::~TMLEqCompDataLine()
	{
/*
	delete iParentInfo;
	delete iAttributeInfo;
*/
	}

bool TMLEqCompDataLine::lessthan(TMLEqCompDataLine* aLeft, TMLEqCompDataLine* aRight)
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

bool TMLEqCompDataLine::ValuesEqual(const TMLEqCompDataLine& aOther) const
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

string TMLEqCompDataLine::Name() const
	{
	return iName;
	}

bool TMLEqCompDataLine::Merge(TMLEqCompDataLine& aOtherLine)
	{
/*
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
			iParentInfo = new TMLEqCompDataParentInfo();
			}
		if(aOtherLine.iParentInfo)
			{
			iParentInfo->Merge(*(aOtherLine.iParentInfo));
			}

		if(!iAttributeInfo)
			{
			// must be screen...
			iAttributeInfo = new TMLEqCompDataAttributeInfo();
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
		for (TMLEqCompDataLine::iterator pOtherValues=aOtherLine.begin(); pOtherValues!=aOtherLine.end(); ++pOtherValues)
			{
			string index = pOtherValues->first;
			if(mirrorMerge)
				{
				// flip left and right
				if(index == KEqCompDataCellNameLeft)
					index = KEqCompDataCellNameRight;
				else if(index == KEqCompDataCellNameRight)
					index = KEqCompDataCellNameLeft;
				}

			(*this)[index].Merge(this, index, pOtherValues->second, mirrorMerge);
			}
		}
	return compatible;
	*/
	return false;
	}

TMLAttributeZoomLevels* TMLEqCompDataLine::GetAttributeZoomLevels(string aAttribName, int aVariety)
	{
/*
	TMLAttributeZoomLevels* found = 0;
	TMLEqCompData& data = *(iParentTable->iTables);
	TMLAttributes& attributes = *(data.iAttributes);
	int attribId = attributes.iNames[aAttribName];
	if(attribId == 0)
		throw GeneralErr(string("Attribute name not found: ") + aAttribName);
	// find out from attribute info which attribute set we need
	// but if there is none specified, we don't need to search
	if(iAttributeInfo)
		{
		TMLEqCompDataAttributeInfoSelector& selector = (*iAttributeInfo)[aVariety];
		// go to parent straight away, as parent always stores attribute data for its children
		found = GetParentAttributeZoomLevels(selector.iAttributeSetName, attribId, aVariety);
		}
	return found;
*/
	return NULL;
	}

TMLAttributeZoomLevels* TMLEqCompDataLine::GetParentAttributeZoomLevels(string aAttribSetName, int aAttribId, int aVariety)
	{
	TMLAttributeZoomLevels* found = NULL;
	TMLEqCompDataParentInfo::iterator pFoundSelector = iParentInfo->find(aVariety);
	if(pFoundSelector != iParentInfo->end())
		{
		const TMLEqCompDataParentInfoSelector& parentInfoSelector = pFoundSelector->second;
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

TMLAttributeZoomLevels* TMLEqCompDataLine::FindAttributeZoomLevels(string aAttribSetName, int aAttribId)
	{
	TMLEqCompData& data = *(iParentTable->iTables);
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

void TMLEqCompDataLine::Compile(int aOptionSetId)
	{
	// compile values
	for(iterator pVal = begin(); pVal != end(); ++pVal)
		{
		(pVal->second).Compile(aOptionSetId);
		}
	}

bool TMLEqCompDataLine::MatchNameDiscountingSuffix(const TMLEqCompDataLine& aLine) const
	{
	// we are trying to compare whether the names are the same apart from a trailing number
	string pureName = NameDiscountingSuffix();
	string pureNameOther = aLine.NameDiscountingSuffix();
	string ending = pureName.substr(pureName.find_last_not_of(KEqCompDataKeywordUnderscore));

	bool namesMatch = (pureName == pureNameOther);
	bool correctEnding = (ending == KEqCompDataKeywordLineNameSuffixGraphic || ending == KEqCompDataKeywordLineNameSuffixText);
	return (namesMatch && correctEnding);
	}

bool TMLEqCompDataLine::MatchType(const TMLEqCompDataLine& aLine) const
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

string TMLEqCompDataLine::NameDiscountingSuffix() const
	{
	int lastNonNumericPos = iName.find_last_not_of(KEqCompDataSearchCollectionNumeric);
	int length = lastNonNumericPos + 1;
	return iName.substr(0, length);
	}

string TMLEqCompDataLine::NameSuffix() const
	{
	int lastNonNumericPos = iName.find_last_not_of(KEqCompDataSearchCollectionNumeric);
	int suffixPos = lastNonNumericPos + 1;
	return iName.substr(suffixPos);
	}

int TMLEqCompDataLine::NumCols() const
	{
	return iNumCols;
	}

int TMLEqCompDataLine::NumRows() const
	{
	return iNumRows;
	}


void TMLEqCompDataLine::SetNumCols(int aNumCols)
	{
	iNumCols = aNumCols;
	}

void TMLEqCompDataLine::SetNumRows(int aNumRows)
	{
	iNumRows = aNumRows;
	}


//
// TMLEqCompDataAttributeInfoSelector
//
TMLEqCompDataAttributeInfoSelector::TMLEqCompDataAttributeInfoSelector()
	{

	}

TMLEqCompDataAttributeInfoSelector::TMLEqCompDataAttributeInfoSelector(string aAttributeSetName)
	:
	iAttributeSetName(aAttributeSetName)
	{

	}

//
//  TMLEqCompDataAttributeInfo
//

TMLEqCompDataAttributeInfo::TMLEqCompDataAttributeInfo()
	:
	iLine(0)
	{

	}

TMLEqCompDataAttributeInfo::TMLEqCompDataAttributeInfo(TMLEqCompDataLine* aLine)
	:
	iLine(aLine)
	{

	}

TMLEqCompDataAttributeInfo::~TMLEqCompDataAttributeInfo()
	{

	}

void TMLEqCompDataAttributeInfo::Merge(const TMLEqCompDataAttributeInfo& aOther)
	{
	for (const_iterator pOtherVariety = aOther.begin(); pOtherVariety != aOther.end(); ++pOtherVariety)
		{
		int varietyIndex = pOtherVariety->first;
		const TMLEqCompDataAttributeInfoSelector& selector = pOtherVariety->second;
		insert(make_pair(varietyIndex, selector));
		}
	}


//
// TMLEqCompDataTableOptionSet
//
TMLEqCompDataTableOptionSet::TMLEqCompDataTableOptionSet()
	:
	iOrientation(EMLEqCompDataOptionSetOrientationUndefined)
	{
	
	}

TMLEqCompDataTableOptionSet::TMLEqCompDataTableOptionSet(const TMLEqCompDataTableOptionSet& aOther)
	:
	iOrientation(aOther.iOrientation)
	{
	for(const_iterator pOtherLines = aOther.begin(); pOtherLines != aOther.end(); ++pOtherLines)
		{
		push_back(*pOtherLines);
		}
	}

TMLEqCompDataTableOptionSet::~TMLEqCompDataTableOptionSet()
	{
	// don't delete pointers to lines, as they are not owned
	clear();
	}

void TMLEqCompDataTableOptionSet::Compile(int aOptionSetId)
	{
	for (iterator pLine = begin(); pLine != end(); ++pLine)
		{
		(*pLine)->Compile(aOptionSetId);
		}
	}

TMLEqCompDataLine* TMLEqCompDataTableOptionSet::FindLine(int aId) const
	{
	for(const_iterator pLine = begin(); pLine != end(); ++pLine)
		{
		TMLEqCompDataLine* line = *pLine;
		if(line)
			{
			if(line->iId == aId)
				return line;
			}
		}
	return NULL;
	}

//
// TMLEqCompDataTable
//

TMLEqCompDataTable::TMLEqCompDataTable(TMLEqCompData* aTables)
	: 
	iId(0),
	iTables(aTables), 
	iParentLine(NULL)
	{
	}

TMLEqCompDataTable::TMLEqCompDataTable(TMLEqCompData* aTables, const TMLEqCompDataTable& aOther)
	: 
  	iId(aOther.iId),
	iTables(aTables), 
	iParentLine(NULL), 
	iParentName(aOther.iParentName)
	{
	for (const_iterator pOptionSet = aOther.begin(); pOptionSet != aOther.end(); ++pOptionSet)
		{
		insert(make_pair(pOptionSet->first, TMLEqCompDataTableOptionSet(pOptionSet->second)));
		}
	}

TMLEqCompDataTable::~TMLEqCompDataTable()
	{
	// no need to delete lines, as they are not owned.
	}

string TMLEqCompDataTable::Name()
	{
	return iName;
	}

// @todo this will need to be modified to allow for searching for a line at a specific option set number
TMLEqCompDataLine* TMLEqCompDataTable::FindLine(const string& aName)
	{
/*
	for (iterator pLine = begin(); pLine != end(); ++pLine)
		{
		TMLEqCompDataLine& line = **pLine;
		if (line.Name() == aName)
			return *pLine;
		}
*/
	return 0;
	}

void TMLEqCompDataTable::Merge(TMLEqCompDataTable& aOther)
	{
/*
	for (iterator pOtherLine = aOther.begin(); pOtherLine != aOther.end(); )
		{
        TMLEqCompDataLine* found = FindLine((*pOtherLine)->Name());
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
*/
	}


void TMLEqCompDataTable::Compile()
	{
//	SetDefaultColumnNames();
//	sort(begin(), end(), TMLEqCompDataLine::lessthan);	

	for (iterator pOptionSet = begin(); pOptionSet != end(); ++pOptionSet)
		{
		pOptionSet->second.Compile(pOptionSet->first);
		}
	}



const string KValueNames[] = { "Font", "C", "l", "r", "W", "J", "t", "r", "b", "H" };
const set<string> KValueNamesSet(KValueNames, ARRAY_END(KValueNames));

bool TMLEqCompDataTable::IsValueColumn(string aName)
	{
	return KValueNamesSet.find(aName) != KValueNamesSet.end();
	}

const string KNumericNames[] = { "C", "l", "r", "W", "t", "r", "b", "H" };
const set<string> KNumericNamesSet(KNumericNames, ARRAY_END(KNumericNames));

bool TMLEqCompDataTable::IsNumericColumn(string aName)
	{
	return KNumericNamesSet.find(aName) != KNumericNamesSet.end();
	}

const string KHorizontalColumnNames[] = { "l", "r", "W"};
const set<string> KHorizontalNamesSet(KHorizontalColumnNames, ARRAY_END(KHorizontalColumnNames));

bool TMLEqCompDataTable::IsHorizontalColumn(string aName)
	{
	return KHorizontalNamesSet.find(aName) != KHorizontalNamesSet.end();
	}

const string KVerticalColumnNames[] = {"t", "b", "H" };
const set<string> KVerticalNamesSet(KVerticalColumnNames, ARRAY_END(KVerticalColumnNames));

bool TMLEqCompDataTable::IsVerticalColumn(string aName)
	{
	return KVerticalNamesSet.find(aName) != KVerticalNamesSet.end();
	}


const string KPaneColumnNames[] = {"Item", "C", "l", "t", "r", "b", "W", "H", "Remarks"};
const string KGraphicColumnNames[] = {"Item", "C", "l", "t", "r", "b", "W", "H", "Remarks"};
const string KTextColumnNames[] = {"Font", "C", "l", "r", "t", "b", "W", "H", "J", "Remarks"};

void TMLEqCompDataTable::SetDefaultColumnNames()
	{
	iColumnNames.clear();
	iColumnNames.insert(iColumnNames.end(), KPaneColumnNames, ARRAY_END(KTextColumnNames)); // superset
	}


// 
// TMLEqListComponent
//
TMLEqListComponent::TMLEqListComponent()
	: 
	iId(-1),
	iName("badName")
	{

	}

//
// struct TMLEqParChildComponent
//
	TMLEqParChildComponent::TMLEqParChildComponent()
	:
	iId(-1),
	iParentId(0)
	{

	}


//
// TMLEqCompData
//

TMLEqCompData::TMLEqCompData()
	: 
	iCanBeMirror(false),
	iIsBaseInstance(false),
	iAttributes(0)
	{
	}

TMLEqCompData::TMLEqCompData(const TMLEqCompData& aOther)
	{
	*this = aOther;
	}

TMLEqCompData& TMLEqCompData::operator=(const TMLEqCompData& aOther)
	{
	if (this != &aOther)
		{
		iName = aOther.iName;
		iCanBeMirror = aOther.iCanBeMirror;
		for (const_iterator pTab = aOther.begin(); pTab != aOther.end(); ++pTab)
			push_back(new TMLEqCompDataTable(this, **pTab));
		Compile();
		}
	return *this;
	}

TMLEqCompData::~TMLEqCompData()
	{
	for (iterator pTab = begin(); pTab != end(); ++pTab)
		delete *pTab;
	DeleteComponents();
	delete iAttributes;
	}

TMLEqCompDataLine* TMLEqCompData::FindComponent(const string& aName) const
	{
	for (TMLEqCompDataComponents::const_iterator pComp = iComponents.begin(); pComp != iComponents.end(); ++pComp)
		{
		TMLEqCompDataLine* line = pComp->second;
		if (line->Name() == aName)
			return line;
		}

	return 0;
	}

TMLEqCompDataLine* TMLEqCompData::FindLine(const string& aName) const
	{
	for (const_iterator pTab = begin(); pTab != end(); ++pTab)
		{
		TMLEqCompDataLine* line = (*pTab)->FindLine(aName);
		if (line)
			return line;
		}

	return 0;
	}

TMLEqCompDataTable* TMLEqCompData::FindTable(const string& aName) const
	{
	for (const_iterator pTab = begin(); pTab != end(); ++pTab)
		{
		if ((*pTab)->Name() == aName)
			return *pTab;
		}

	return 0;
	}

TMLEqCompDataTable* TMLEqCompData::FindTable(int aId) const
	{
	for (const_iterator pTab = begin(); pTab != end(); ++pTab)
		{
		if ((*pTab)->iId == aId)
			return *pTab;
		}

	return 0;
	}

void TMLEqCompData::Merge(TMLEqCompData& aOther)
	{
	iName = aOther.iName;
	iCanBeMirror |= aOther.iCanBeMirror; // in the case of an elaf layout merging onto an abrw layout, the chirality will be preserved
	MergeComponents(aOther);
	}

void TMLEqCompData::MergeComponents(TMLEqCompData& aOther)
    {
	for (TMLEqCompDataComponents::iterator pOtherLine = aOther.iComponents.begin(); pOtherLine != aOther.iComponents.end(); ++pOtherLine)
		{
		TMLEqCompDataLine& otherLine = *(pOtherLine->second);
        TMLEqCompDataLine* found = FindComponent(otherLine.iName);
        if(found)
			{
			found->Merge(otherLine);
			}
		else
			{
			TMLEqCompDataLine* newLine = new TMLEqCompDataLine(otherLine);
			iComponents.insert(make_pair(otherLine.iId, newLine));
			}
		}
	}

void TMLEqCompData::Compile()
	{
	UpdateNames();
	CreateTables();

	// now add a special table for the screen contents
	TMLEqCompDataTable* topTab = new TMLEqCompDataTable(this);
	topTab->iId = -1;
	topTab->iName = KEqCompDataKeywordScreenContents;

	// then insert each line into its parent
	for (TMLEqCompDataComponents::iterator pComp = iComponents.begin(); pComp != iComponents.end(); ++pComp)
		{
		TMLEqCompDataLine& line = *(pComp->second);
		if(line.iType == TMLEqCompDataLine::EScreenComponent)
			{
			line.iParentTable = topTab;
			TMLEqCompDataTableOptionSet& optionSet = (*topTab)[0];
			optionSet.push_back(&line);
			}
		else
			{
			bool parentFound = false;
			TMLEqParChildComponent* parChild = iParChildComponents[line.iId];
			int parentId = parChild->iParentId;
			if(parentId != 0)
				{
				TMLEqCompDataTable* parentTable = FindTable(parentId);
				if(parentTable)
					{
					line.iParentTable = parentTable;

					// iterate through the line and populate the table option sets as needed
					for(TMLEqCompDataLine::iterator pValues = line.begin(); pValues != line.end(); ++pValues)
						{
						TMLEqCompDataValues& values = pValues->second;
						TMLEqCompDataValuesOptionSets& sets = values.iOptionSets;
						for(TMLEqCompDataValuesOptionSets::iterator pValuesOptionSet = sets.begin(); pValuesOptionSet != sets.end(); ++pValuesOptionSet)
							{
							TMLEqCompDataValuesOptionSet& valuesSet = pValuesOptionSet->second;
							int optionId = pValuesOptionSet->first;
							TMLEqCompDataTableOptionSet& tableSet = (*parentTable)[optionId]; // constructs if not yet present
							if(tableSet.iOrientation != valuesSet.iOrientation)
								tableSet.iOrientation = valuesSet.iOrientation;
							
							TMLEqCompDataLine* lineInTable = tableSet.FindLine(line.iId);
							if(!lineInTable)
								{
								// make one
								tableSet.push_back(&line);
								}
							}
						}

					// copy the pointer from the components table
//					parentTable->insert(make_pair(line.iId, &line));
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
                string errorText = string(" TMLEqCompData::Compile() - can't find parent component: ");
				errorText += CdlTkUtil::IntToString(parentId);
				throw GeneralErr(errorText);
				}
			}		
		}	
	push_back(topTab);
	
	// now compile the tables
	iterator pTab;
	for (pTab = begin(); pTab != end(); ++pTab)
		(*pTab)->Compile();

	// now sort the tables
//	sort(begin(), end(), TMLEqCompDataTable::lessthan);	
	}

void TMLEqCompData::UpdateNames()
	{
	// from the list of components, get the name of each line from the list
	for (TMLEqCompDataComponents::iterator pComp = iComponents.begin(); pComp != iComponents.end(); ++pComp)
		{
		TMLEqCompDataLine* line = pComp->second;
		TMLEqListComponent* pListComp = iListComponents[line->iId];
		line->iName = pListComp->iName;
		}

	}

void TMLEqCompData::CreateTables()
	{
	// from the list of components, first create a table for each pane
	for (TMLEqCompDataComponents::iterator pComp = iComponents.begin(); pComp != iComponents.end(); ++pComp)
		{
		TMLEqCompDataLine& line = *(pComp->second);
		switch(line.iType)
			{
			case TMLEqCompDataLine::EScreenComponent:
			case TMLEqCompDataLine::EContainerComponent:
			case TMLEqCompDataLine::EPaneComponent:
				{
				TMLEqCompDataTable* tab = new TMLEqCompDataTable(this);
				tab->iId = line.iId;
				tab->iName = line.iName;
				push_back(tab);
				break;
				}
			case TMLEqCompDataLine::EGraphicComponent:
			case TMLEqCompDataLine::ETextComponent:
				{
				// text and graphic components are not panes 
				// and are therefore not represented in our internal object model as tables
				break;
				}
			default:
				{
				cout << "TMLEqCompData::CreateTables() - uncategorised component\n";
				break;
				}
			}	
		}
	}

void TMLEqCompData::DeleteComponents()
	{
	for (TMLEqCompDataComponents::iterator pComp = iComponents.begin(); pComp != iComponents.end(); ++pComp)
		delete pComp->second;
	}

// End of File
