/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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


#include "Layout.h"
#include "LayoutCompilerErr.h"
#include "CodeGenConsts.h"
#include "UsefulDefinitions.h"

#include <set>
#include <sstream>
#include <iostream>

// TValues

TValues::TValues()
: iLine(0), iName("Bad Value")
	{
	}

TValues::TValues(TLayoutLine* aLine, string aName)
: iLine(aLine), iName(aName), iNeedsIndex(0)
	{
	}

bool TValues::operator==(const TValues& aOther) const
	{
	typedef const vector<string> TBase;
	bool eq =
		iName == aOther.iName &&
		iParam == aOther.iParam &&
		iNeedsP == aOther.iNeedsP &&
		iNeedsIndex == aOther.iNeedsIndex &&
		(*static_cast<TBase*>(this) == aOther);
	return eq;
	}

void TValues::Merge(TValues& aValues)
	{
	clear();
	insert(begin(), aValues.begin(), aValues.end());
	}

void TValues::Compile()
	{
	iNeedsP = false;
	if (!TLayoutTable::IsValueColumn(iName))
		return;

	for (iterator it = begin(); it != end(); ++it)
		{
		bool back2IsAlphaNum = false;
		bool back1IsAlphaNum = false;
		bool back1IsP = false;
		for (string::iterator pC = it->begin(); pC!=it->end(); ++pC)
			{
			bool IsAlphaNum = (
				('A' <= *pC && *pC <= 'Z') || 
				('a' <= *pC && *pC <= 'z') ||
				('0' <= *pC && *pC <= '9') ||
				*pC == '_');

			if (!back2IsAlphaNum && back1IsP && !IsAlphaNum)
				iNeedsP = true;

			back1IsP = *pC == 'p';
			back2IsAlphaNum = back1IsAlphaNum;
			back1IsAlphaNum = IsAlphaNum;
			}

		if (!back2IsAlphaNum && back1IsP)
			iNeedsP = true;
		}
	iNeedsIndex = (size() > 1);
	}

string TValues::ParamName() const
	{
	if (iParam.length())
		return iParam;
	else
		return KParamNameBase + iName;
	}

string TValues::CppValue(const string& aValue)
	{
	if (aValue.size())
		return aValue;
	else
		return "ELayoutEmpty";
	}


//  TLayoutLine

TLayoutLine::TLayoutLine( TLayoutTable* aTable, int aId )
:	iId(aId), 
	iTable(aTable), 
	iIsUnique(true), 
	iIsMirroredHorizontally(false),
	iIsMergedIdentical(false)
	{}

TLayoutLine::TLayoutLine(TLayoutTable* aTable, const TLayoutLine& aOther)
	{
	*this = aOther;
	iTable = aTable;
	for (iterator pVal = begin(); pVal != end(); ++pVal)
		{
		TValues& val = pVal->second;
		val.iLine = this;
		}
	}

#ifdef RD_SHOW_ALL_AKNLAYOUTCOMPILER_WARNINGS
void TLayoutLine::WarnMergeMismatch(TLayoutLine& aLine)
#else
void TLayoutLine::WarnMergeMismatch(TLayoutLine& /* aLine */)
#endif
	{
    // Merge mismatch warnings are not shown from old lay files.
#ifdef RD_SHOW_ALL_AKNLAYOUTCOMPILER_WARNINGS
	string name = Name();
	if (name.empty())
		name = TableName();
	else
		name = iTable->iName + " | " + name;
	cerr << "WARNING merge mismatch: " << iTable->iTables->iName << " | " << name;
	cerr << " vs " << aLine.iTable->iTables->iName << endl;
#endif
	}

void TLayoutLine::Merge(TLayout::TMergeMode aMergeMode, TLayoutLine& aLine)
	{
	iIsMergedIdentical = (*this == aLine) && (aMergeMode == TLayout::KMergeModeVariant);
	if (iIsMergedIdentical)
		return;

	bool similar = (Name() == aLine.Name());
	iterator pVal;

	if (similar && aMergeMode == TLayout::KMergeModeVariant && aLine.iTable->iTables->iCanBeMirror)
		{
		// need to check to see if the parametrisations are mirrored
		// first check for P
		bool lr_p = (find("l")->second.iNeedsP && aLine["r"].iNeedsP);
		bool rl_p = (find("r")->second.iNeedsP && aLine["l"].iNeedsP);
		
		// check if they both params are valid
		bool lr_both = (find("l")->second.iNeedsIndex && aLine["r"].iNeedsIndex);
		bool rl_both = (find("r")->second.iNeedsIndex && aLine["l"].iNeedsIndex);

		// check if params are the same
		bool lr_same = (find("l")->second.iParam == aLine["r"].iParam);
		bool rl_same = (find("r")->second.iParam == aLine["l"].iParam);

		// only mirrored if both are valid and same
		bool lr = lr_p || (lr_both && lr_same);
		bool rl = rl_p || (rl_both && rl_same);

		// if either or both are parametrised the same way, need to swap; doesn't matter if 
		// both attribs have same param, as swapping it won't make any difference.
		if(rl || lr)
			{
			iIsMirroredHorizontally = true;
			}
		}

	if (similar)
		{
		for (pVal=begin(); pVal!=end(); ++pVal)
			{
			TValues& val = pVal->second;
			string cell = pVal->first;
			if (iIsMirroredHorizontally)
				{
				if (cell == "l")
					cell = "r";
				else if (cell == "r")
					cell = "l";
				}

			TValues& other = aLine[cell];
			if (val.size() != other.size() || 
				val.iNeedsIndex != other.iNeedsIndex || 
				val.iNeedsP != other.iNeedsP)
				{
				similar = false;
				}
			}
		}

	if (!similar)
		WarnMergeMismatch(aLine);

	switch(aMergeMode)
		{
	case TLayout::KMergeModeMerge:
	case TLayout::KMergeModeUnion:
		if (similar)
			{
			for (pVal=begin(); pVal!=end(); ++pVal)
				pVal->second.Merge(aLine[pVal->first]);
			}
		else
			{
			clear();
			copy(aLine.begin(), aLine.end(), inserter(*this, begin()));
			for (pVal=begin(); pVal!=end(); ++pVal)
				pVal->second.iLine = this;
			}
		break;
	case TLayout::KMergeModeVariant:
		clear();
		copy(aLine.begin(), aLine.end(), inserter(*this, begin()));
		for (pVal=begin(); pVal!=end(); ++pVal)
			pVal->second.iLine = this;
		break;
		}
	}

bool TLayoutLine::operator==(const TLayoutLine& aOther) const
	{
	return (Name() == aOther.Name()) && ValuesEqual(aOther);
	}

bool TLayoutLine::ValuesEqual(const TLayoutLine& aOther) const
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

string TLayoutLine::Name() const
	{
	if (find("Item") != end())
		return find("Item")->second[0];
	else
		return "";
	}

string TLayoutLine::TableName() const
	{
	stringstream name;
	name << iTable->Name() << "_Line_" << iId;
	return name.str();
	}

void TLayoutLine::Compile()
	{
	iNeedsP = false;
	iNeedsIndex = false;

	for (iterator pVal = begin(); pVal != end(); ++pVal)
		{
		TValues& val = pVal->second;
		val.Compile();

		if (val.iNeedsP)
			iNeedsP = true;
		if (val.iNeedsIndex)
			iNeedsIndex = true;
		}
	}

bool TLayoutLine::MatchParams(const TLayoutLine& aLine) const
	{
	if (iNeedsP != aLine.iNeedsP)
		return false;

	for (const_iterator pVal = begin(), pOther = aLine.begin(); pVal != end(); ++pVal, ++pOther)
		{
		const TValues& val = pVal->second;
		const TValues& other = pOther->second;
		if (val.iNeedsIndex != other.iNeedsIndex || val.ParamName() != other.ParamName())
			return false;
		}

	return true;
	}


//
// TLayoutTable
//



TLayoutTable::TLayoutTable(TLayout* aTables)
: iType(EUnknownTable), iTables(aTables), iParent(0), iFirstLineGlobalIndex(-1), iAppend(false), iNoSubTables(false)
	{
	}

TLayoutTable::TLayoutTable(TLayout* aTables, const TLayoutTable& aOther)
: iColumnNames(aOther.iColumnNames), iType(aOther.iType),
  iTables(aTables), iName(aOther.iName), iParent(0),
  iParentName(aOther.iParentName), iFirstLineGlobalIndex(aOther.iFirstLineGlobalIndex),
  iAppend(aOther.iAppend), iNoSubTables(aOther.iNoSubTables)
	{
	for (const_iterator it = aOther.begin(); it != aOther.end(); ++it)
		push_back(new TLayoutLine(this, **it));
	}

TLayoutTable::~TLayoutTable()
	{
	for (iterator it = begin(); it != end(); ++it)
		delete *it;
	DestroySubTables();
	}

void TLayoutTable::Merge(TLayout::TMergeMode aMergeMode, TLayoutTable& aTable)
	{
	if (aTable.iAppend)
		{
		insert(end(), aTable.begin(), aTable.end());
		aTable.clear();
		for (iterator pLine = begin(); pLine != end(); ++pLine)
			(*pLine)->iTable = this;
		}
	else
		{
		// merge lines
		iterator pNew = aTable.begin();
		switch(aMergeMode)
			{
			case TLayout::KMergeModeMerge:
				{
				if (size() == aTable.size())
					{
					for (iterator pLine = begin(); pLine != end(); ++pLine)
						{
						(*pLine)->Merge(aMergeMode, **pNew);
						pNew++;
						}
					}
				else
					{
					// move the other tables contents to here
					iterator pLine;
					for (pLine = begin(); pLine != end(); ++pLine)
						delete *pLine;
					clear();
					insert(begin(), aTable.begin(), aTable.end());
					aTable.clear();
					for (pLine = begin(); pLine != end(); ++pLine)
						(*pLine)->iTable = this;
					}
					break;
				}
			case TLayout::KMergeModeVariant:
				{
				iterator pLine;
				// need to merge the matching lines but append the extra ones [LMB 11-10-2002]
				for (pLine = begin(); pLine != end() && pNew != aTable.end(); ++pLine)
					{
					(*pLine)->Merge(aMergeMode, **pNew);
					pNew++;
					}
				insert(end(), pNew, aTable.end());
				aTable.clear();
				for (pLine = begin(); pLine != end(); ++pLine)
					(*pLine)->iTable = this;
				break;
				}
			case TLayout::KMergeModeUnion:
				{
				unsigned int index = 0;
				for (; pNew != aTable.end(); ++pNew)
					{
					bool found = false;
					for (iterator pLine = begin()+index; pLine != end(); ++pLine)
						{
						if ((*pLine)->Name() == (*pNew)->Name())
							{
							(*pLine)->Merge(aMergeMode, **pNew);
							found = true;
							break;
							}
						}
					if (found)
						{
						delete *pNew;
						}
					else
						{
						if ((*pNew)->Name().empty())
							{
							throw GeneralErr(Name() + " can't union merge unnamed line");
							}
						push_back(*pNew);
						(*pNew)->iTable = this;
						if ( static_cast<unsigned int>( (*pNew)->iId ) != size())
							iNoSubTables = true;
						(*pNew)->iId = size();
						}
					if (index+1 < size())
						index++;
					}
				aTable.clear();
				}
			}
		}
	}

string TLayoutTable::Name()
	{
	return iName;
	}

TLayoutLine* TLayoutTable::FindLine(const string& aName)
	{
	for (iterator it = begin(); it != end(); ++it)
		if ((*it)->Name() == aName)
			return *it;
	return 0;
	}

void TLayoutTable::Compile()
	{
	if (iType == EUnknownTable && iColumnNames.size() && iColumnNames[0].size())
		{
		if (iColumnNames[0] == "Item")
			iType = EWindowTable;
		else
			iType = ETextTable;
		}

	SetDefaultColumnNames();

	iParent = iTables->FindLine(iParentName);

	iNeedsIndex = false;
	iNeedsP = false;
	for (iterator it = begin(); it != end(); ++it)
		{
		(*it)->Compile();
		if ((*it)->iNeedsIndex)
			iNeedsIndex = true;
		if ((*it)->iNeedsP)
			iNeedsP = true;
		}

	BuildSubTables();
	}

void TLayoutTable::BuildSubTables()
	{
	DestroySubTables();

	if (iNoSubTables)
		return;

	int count = size();
	TLayoutSubTable* subTable = 0;
	for (int i=0; i<count; i++)
		{
		TLayoutLine& line = *(*this)[i];
		if (subTable)
			{
			TLayoutLine* firstLine = (*this)[(*subTable)[0]];
			if (!firstLine->MatchParams(line))
				{
				if (subTable->size() > 1)
					iSubTables.push_back(subTable);
				else
					delete subTable;
				subTable = new TLayoutSubTable;
				}
			}
		else
			{
			subTable = new TLayoutSubTable;
			}
		subTable->iIsMergedIdentical = subTable->iIsMergedIdentical && line.iIsMergedIdentical;
		subTable->push_back(i);
		}

	if (subTable->size() > 1)
		iSubTables.push_back(subTable);
	else
		delete subTable;

/*
	for (int i=0; i<count; i++)
		{
		bool inserted = false;
		TLayoutLine& nextLine = *(*this)[i];
		for (TLayoutSubTables::iterator it = iSubTables.begin(); it != iSubTables.end(); ++it)
			{
			TLayoutLine* line = (*this)[*((*it)->begin())];
			if (line->MatchParams(*(*this)[i]))
				{
				TLayoutSubTable& subTab = **it;
				subTab.push_back(i);
				subTab.iIsMergedIdentical = subTab.iIsMergedIdentical && nextLine.iIsMergedIdentical;
				inserted = true;
				}
			}

		if (!inserted)
			{
			TLayoutSubTable* subTable = new TLayoutSubTable;
			subTable->push_back(i);
			subTable->iIsMergedIdentical = nextLine.iIsMergedIdentical;
			iSubTables.push_back(subTable);
			}
		}

	TLayoutSubTables::iterator it = iSubTables.begin();
	while (it != iSubTables.end())
		{
		if ((*it)->size() == 1)
			it = iSubTables.erase(it);
		else
			++it;
		}
*/
	}

void TLayoutTable::DestroySubTables()
	{
	for (TLayoutSubTables::iterator it = iSubTables.begin(); it != iSubTables.end(); ++it)
		delete *it;
	iSubTables.clear();
	}

const string KValueNames[] = { "Font", "C", "l", "r", "B", "W", "J", "t", "r", "b", "H" };
const set<string> KValueNamesSet(KValueNames, ARRAY_END(KValueNames));

bool TLayoutTable::IsValueColumn(string aName)
	{
	return KValueNamesSet.find(aName) != KValueNamesSet.end();
	}

const string KNumericNames[] = { "C", "l", "r", "B", "W", "t", "r", "b", "H" };
const set<string> KNumericNamesSet(KNumericNames, ARRAY_END(KNumericNames));

bool TLayoutTable::IsNumericColumn(string aName)
	{
	return KNumericNamesSet.find(aName) != KNumericNamesSet.end();
	}

const string KWindowColumnNames[] = {"Item", "C", "l", "t", "r", "b", "W", "H", "Remarks"};
const string KTextColumnNames[] = {"Font", "C", "l", "r", "B", "W", "J", "Remarks"};

void TLayoutTable::SetDefaultColumnNames()
	{
	iColumnNames.clear();
	if (iType == EWindowTable)
		{
		iColumnNames.insert(iColumnNames.end(), KWindowColumnNames, ARRAY_END(KWindowColumnNames));
		}
	else
		{
		iColumnNames.insert(iColumnNames.end(), KTextColumnNames, ARRAY_END(KTextColumnNames));
		}
	}

bool TLayoutTable::IsWorthATableIndex()
	{
	return iSubTables.size() != 0;
	}


TLayoutTable::TLayoutSubTable::TLayoutSubTable()
: iIsMergedIdentical(true)
	{
	}

//
// TLayout
//

TLayout::TLayout()
: iCanBeMirror(false)
	{
	}

TLayout::TLayout(const TLayout& aOther)
	{
	*this = aOther;
	}

TLayout& TLayout::operator=(const TLayout& aOther)
	{
	if (this != &aOther)
		{
		iName = aOther.iName;
		iCanBeMirror = aOther.iCanBeMirror;
		for (const_iterator it = aOther.begin(); it != aOther.end(); ++it)
			push_back(new TLayoutTable(this, **it));
		Compile();
		}
	return *this;
	}

TLayout::~TLayout()
	{
	for (iterator it = begin(); it != end(); ++it)
		delete *it;
	}

void TLayout::Merge(TLayout::TMergeMode aMergeMode, TLayout& aLayout)
	{
	if (iName.empty())
		iName = aLayout.iName;

	for (iterator pNew = aLayout.begin(); pNew != aLayout.end();)
		{
		iterator pTab;
		for (pTab = begin(); pTab != end(); ++pTab)
			{
			if ((*pTab)->Name() == (*pNew)->Name())
				break;
			}

		if (pTab == end())
			{
			push_back(*pNew);
			(*pNew)->iTables = this;
			pNew = aLayout.erase(pNew);
			}
		else
			{
			(*pTab)->Merge(aMergeMode, **pNew);
			++pNew;
			}
		}

	Compile();
	}

TLayoutLine* TLayout::FindLine(const string& aName)
	{
	for (iterator it = begin(); it != end(); ++it)
		{
		TLayoutLine* line = (*it)->FindLine(aName);
		if (line)
			return line;
		}

	return 0;
	}

void TLayout::Compile()
	{
	iterator pTab;
	for (pTab = begin(); pTab != end(); ++pTab)
		(*pTab)->Compile();

	// set uniqueness for lines
	multiset<string> names;
	for (pTab = begin(); pTab != end(); ++pTab)
		{
		TLayoutTable& tab = **pTab;
		for (TLayoutTable::iterator pLine = tab.begin(); pLine != tab.end(); ++pLine)
			names.insert((*pLine)->Name());
		}

	for (pTab = begin(); pTab != end(); ++pTab)
		{
		TLayoutTable& tab = **pTab;
		for (TLayoutTable::iterator pLine = tab.begin(); pLine != tab.end(); ++pLine)
			{
			TLayoutLine& line = **pLine;
			const string& name = line.Name();
			if (name[0] == 'q')
				{
				line.iIsUnique = false;
				}
			else 
				{
				line.iIsUnique = (names.count(name) == 1);
				}
			}
		}

	}


// End of File
