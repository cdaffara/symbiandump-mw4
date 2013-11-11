/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
*/



#ifndef LAYOUT_H
#define LAYOUT_H

// disable "identifier was truncated to '255' characters in the browser information" warning
#pragma warning (disable:4786)
#include <vector>
#include <string>
#include <iosfwd>
#include <map>

using namespace std;

class TLayoutLine;
class TLayoutTable;
class TLayout;

extern const string KWindowOutputOrder[];
extern const string KTextOutputOrder[];
extern const string KWindowOutputOrderMirrored[];
extern const string KTextOutputOrderMirrored[];
extern const int KWindowOutputOrderSize;
extern const int KTextOutputOrderSize;


/**
*  TLayout 
*  The core layout class - this represents a complete layout, 
*  typically loaded from a .lay file
*/
class TLayout : public vector<TLayoutTable*>
	{
public:
	enum TMergeMode	{ KMergeModeMerge, KMergeModeVariant, KMergeModeUnion };

public:
	TLayout();
	TLayout(const TLayout& aOther);
	TLayout& operator=(const TLayout& aOther);

	virtual ~TLayout();
	void Merge(TLayout::TMergeMode aMergeMode, TLayout& aLayout);
	TLayoutLine* FindLine(const string& aName);
	void Compile();

public:
	string iName;
	bool iCanBeMirror;
	};


/**
*  TValues 
*  The values stored in a cell in a layout line
*/
class TValues : public vector<string>
	{
// This class is generic with respect to the type of line it is used by
public:
	TValues();
	TValues(TLayoutLine* aLine, string aName);

	bool operator==(const TValues& aOther) const;

	void Merge(TValues& aValues);
	void Compile();
	string ParamName() const;
	static string CppValue(const string& aValue);

public:
	TLayoutLine* iLine;
	string iName;
	string iParam;
	bool iNeedsP;
	bool iNeedsIndex;
	};


/**
*  TLayoutLine 
*  A layout line
*/
class TLayoutLine : public map<string, TValues>
	{
public:
	TLayoutLine(TLayoutTable* aTable, int aId);
	TLayoutLine(TLayoutTable* aTable, const TLayoutLine& aOther);

	bool operator==(const TLayoutLine& aOther) const;
	bool ValuesEqual(const TLayoutLine& aOther) const;

	void Merge(TLayout::TMergeMode aMergeMode, TLayoutLine& aLine);
	void Compile();
	string Name() const;					
	string TableName() const;
	bool MatchParams(const TLayoutLine& aLine) const;
	bool NeedsParams() const { return iNeedsP || iNeedsIndex; }

	void WarnMergeMismatch(TLayoutLine& aLine);

public:
	int iId;
	TLayoutTable* iTable;
	bool iNeedsP;
	bool iNeedsIndex;
	bool iIsUnique;
	int iGlobalIndex;
	bool iIsMirroredHorizontally; // i.e. l and r are swapped
	bool iIsMergedIdentical;
	};


/**
*  TLayoutTable 
*  A layout table
*/
class TLayoutTable : public vector<TLayoutLine*>
	{
public:
	enum TTableType { EUnknownTable, EWindowTable, ETextTable };
	class TLayoutSubTable : public vector<int>
		{
	public:
		TLayoutSubTable();
		bool iIsMergedIdentical;
		};
	typedef vector<TLayoutSubTable*> TLayoutSubTables;

public:
	TLayoutTable(TLayout* aTables);
	TLayoutTable(TLayout* aTables, const TLayoutTable& aOther);
	virtual ~TLayoutTable();

	void Merge(TLayout::TMergeMode aMergeMode, TLayoutTable& aTable);
	TLayoutLine* FindLine(const string& aName);
	void Compile();
	void BuildSubTables();
	void DestroySubTables();
	string Name();
	static bool IsValueColumn(string aName);
	static bool IsNumericColumn(string aName);
	void SetDefaultColumnNames();
	bool IsWorthATableIndex();

public:
	TLayoutSubTables iSubTables;
	vector<string> iColumnNames;
	TTableType iType;
	TLayout* iTables;
	string iName;
	TLayoutLine* iParent;
	string iParentName;
	bool iNeedsP;
	bool iNeedsIndex;
	int iFirstLineGlobalIndex;
	bool iAppend;
	bool iNoSubTables;
	};


#endif

// End of File
