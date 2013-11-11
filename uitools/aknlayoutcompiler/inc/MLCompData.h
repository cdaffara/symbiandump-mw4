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
*/


#ifndef MLCOMPDATA_H
#define MLCOMPDATA_H

// disable "identifier was truncated to '255' characters in the browser information" warning
#pragma warning (disable:4786)

// disable "decorated name length exceeded, name was truncated"
#pragma warning (disable:4503)

#include "MLAttributes.h"

#include <vector>
#include <string>
#include <iosfwd>
#include <map>

using namespace std;

class TMLCompDataLine;
class TMLCompDataTable;
class TMLCompData;
class TMLAttributes;

extern const string KCompDataPaneOutputOrder[];
extern const string KCompDataPaneOutputOrderMirrored[];
extern const string KCompDataGraphicOutputOrder[];
extern const string KCompDataGraphicOutputOrderMirrored[];
extern const string KCompDataTextOutputOrder[];
extern const string KCompDataTextOutputOrderMirrored[];
extern const int KCompDataPaneOutputOrderSize;
extern const int KCompDataGraphicOutputOrderSize;
extern const int KCompDataTextOutputOrderSize;
/**
*  TMLCompDataCalcs
*  The calcs stored in a value
*/
typedef map<int, string> TMLCompDataCalcs;
typedef map<int, TMLCompDataCalcs> TMLCompDataZoomLevels;
typedef map<int, TMLCompDataZoomLevels> TMLCompDataVarieties;

/**
*  TMLCompDataValues 
*  The values stored in a cell in a layout line
*/
class TMLCompDataValues : public TMLCompDataVarieties
	{
public:
	enum TCompDataCellType
		{
		ECellTypeDefault,
		ECellTypeParamLimit,
		ECellTypeCol,
		ECellTypeRow
		};

public: // constructors, operators, etc
	TMLCompDataValues();
	TMLCompDataValues(TMLCompDataLine* aLine);
	virtual ~TMLCompDataValues();
	bool operator==(const TMLCompDataValues& aOther) const;

public: // new methods
    bool Merge(TMLCompDataLine* aLine, string aName, TMLCompDataValues& aOtherValues, bool aMirrorMerge);
	void Compile(const string& aCellName);
	static string CppValue(const string& aValue);
	static string MirrorJustificationValue(const string& aValue);
	static TCompDataCellType Type(string aName); 

public: // member data
	TMLCompDataLine* iLine; // not owned
	string iName;
	};


//
// Parent Info
//

/**
*  TMLCompDataParentInfoSelector
*
*  The parent id and parent variety index for each variety index
*/
struct TMLCompDataParentInfoSelector
	{
public:
	TMLCompDataParentInfoSelector();
	TMLCompDataParentInfoSelector(int aParentId, int aParentVariety);
public:
	int iParentId;
	int iParentVariety;
	};

typedef map<int, TMLCompDataParentInfoSelector> TMLCompDataParentInfoVarieties;

/**
*  TMLCompDataParentInfo
*
*  Contains the parent info selector for each variety index
*/
class TMLCompDataParentInfo : public TMLCompDataParentInfoVarieties
	{
public: // constructors etc
	TMLCompDataParentInfo();
	TMLCompDataParentInfo(TMLCompDataLine* aLine);
	virtual ~TMLCompDataParentInfo();
public: // new methods
	void Merge(const TMLCompDataParentInfo& aOther);
public: // member data
	TMLCompDataLine* iLine; // not owned
	};

class TMLCompDataAttributeInfo;

/**
*  TMLCompDataLine 
*  A layout line
*/
class TMLCompDataLine : public map<string, TMLCompDataValues>
	{
public:
	enum TComponentType 
		{ 
		EUnknownComponent, 
		EScreenComponent, 
		EContainerComponent, 
		EPaneComponent, 
		EGraphicComponent, 
		ETextComponent 
		};

public: // constructors etc
	TMLCompDataLine();
	TMLCompDataLine(const TMLCompDataLine& aOther);
	virtual ~TMLCompDataLine();

	bool operator==(const TMLCompDataLine& aOther) const;
	static bool lessthan(TMLCompDataLine* aLeft, TMLCompDataLine* aRight);
	bool ValuesEqual(const TMLCompDataLine& aOther) const;

public: // new methods
	bool Merge(TMLCompDataLine& aOther);
	void Compile();
	TMLAttributeZoomLevels* GetAttributeZoomLevels(string aAttribName, int aVariety);

public: // get and set methods
	string Name() const;					
	bool NeedsParams() const;
	bool NeedsOptions() const;
	bool NeedsCols() const;
	bool NeedsRows() const;
	int MaxVariety() const;
	int NumCols() const;
	int NumRows() const;
	void SetMaxVariety(int aMaxVariety);
	void SetNumCols(int aNumCols);
	void SetNumRows(int aNumRows);
	void SetNeedsCols(bool aNeeds);
	void SetNeedsRows(bool aNeeds);

public: // helpers
	int EncodeFontId(int aHeight, int aOutline, int aPosture, int aWeight, int aCategory) const;
	bool MatchParams(const TMLCompDataLine& aLine) const;
	bool MatchNameDiscountingSuffix(const TMLCompDataLine& aLine) const;
	bool MatchType(const TMLCompDataLine& aLine) const;
	string NameDiscountingSuffix() const;
	string NameSuffix() const;

private: // helpers
	TMLAttributeZoomLevels* GetParentAttributeZoomLevels(string aAttribSetName, int aAttribId, int aVariety);
	TMLAttributeZoomLevels* FindAttributeZoomLevels(string aAttribSetName, int aAttribId);
	void CompileParamLimits(TMLCompDataValues::TCompDataCellType aParamLimitType, const string& aParamLimitCellName);
	void CompileFontHeights();


public: // member data
	int iId;
	string iName;
	TComponentType iType;
	int iDrawingOrder;
	int iMaxVariety;
	TMLCompDataTable* iParentTable;
	TMLCompDataParentInfo* iParentInfo;// owned
	TMLCompDataAttributeInfo* iAttributeInfo;// owned
	bool iIsUnique;
	int iGlobalIndex;
	bool iIsMirroredHorizontally; // i.e. l and r are swapped
	bool iNeedsOptions; // note that can need options although numOptions is only 1, as may be needed for subtable
	bool iNeedsCols;
	bool iNeedsRows;

private:
	int iNumCols; // there is always at least one column
	int iNumRows; // there is always at least one row
	};


//
// Attribute Info
//

/**
*  TMLCompDataAttributeInfoSelector
*
*  Contains the attribute set name, which is used to 
*  disambiguate the attribute data for a given component
*/
struct TMLCompDataAttributeInfoSelector
	{
public:
	TMLCompDataAttributeInfoSelector();
	TMLCompDataAttributeInfoSelector(string aAttributeSetName);
public:
	string iAttributeSetName;
	};

typedef map<int, TMLCompDataAttributeInfoSelector> TMLCompDataAttributeInfoVarieties;

/**
*  TMLCompDataAttributeInfo
*
*  Contains the attribute info selector for each variety
*/
class TMLCompDataAttributeInfo : public TMLCompDataAttributeInfoVarieties
	{
public: // constructors etc
	TMLCompDataAttributeInfo();
	TMLCompDataAttributeInfo(TMLCompDataLine* aLine);
	virtual ~TMLCompDataAttributeInfo();
public: // new methods
	void Merge(const TMLCompDataAttributeInfo& aOther);
public: // member data
	TMLCompDataLine* iLine; // not owned
	};

/**
*  TMLCompDataTable 
*  A layout table
*/
class TMLCompDataTable : public vector<TMLCompDataLine*>
	{
public:
	/*
	 * Subtables are automatically generated for contiguous collections 
	 * of components whose names are the same, ending in _g or _t 
	 * followed by a number. These components represent numbered
	 * rendered objects in the specification, and often need to be 
	 * indexed by clients of the scalable layout API.
	 * 
	 */
	class TMLCompDataSubTable : public vector<int>
		{
	public:
		TMLCompDataSubTable();
		bool NeedsParams() const;
	public:
		bool iNeedsOption;
		bool iNeedsCol;
		bool iNeedsRow;
		string iName;
		};
	typedef vector<TMLCompDataSubTable*> TMLCompDataSubTables;

public: // constructors etc
	TMLCompDataTable(TMLCompData* aTables);
	TMLCompDataTable(TMLCompData* aTables, const TMLCompDataTable& aOther);
	virtual ~TMLCompDataTable();
	static bool lessthan(TMLCompDataTable* aLeft, TMLCompDataTable* aRight);

public: // new methods
	TMLCompDataSubTable* FindSubTable(const string& aName);
	TMLCompDataLine* FindLine(const string& aName);
    void Merge(TMLCompDataTable& aOther);
	void Compile();
	void BuildSubTables();
	void NormalizeSubTables();
	void DestroySubTables();
	void SetDefaultColumnNames();

public: // accessors
	string Name();
	static bool IsValueColumn(string aName);
	static bool IsNumericColumn(string aName);
	static bool IsHorizontalColumn(string aName);
	static bool IsVerticalColumn(string aName);

public: // member data
	int iId;
	string iName;
	TMLCompData* iTables;
	TMLCompDataSubTables iSubTables;
	vector<string> iColumnNames;
	TMLCompDataLine* iParentLine; // not owned
	string iParentName;
	bool iNeedsP;
	bool iNeedsIndex;
	bool iAppend;
	int iFirstLineGlobalIndex;
	};

/**
*  TMLCompData 
*  The core layout class - this represents a complete layout, 
*  typically loaded from a .xml file that has been exported from the 
* layout creation tool
*/
class TMLCompData : public vector<TMLCompDataTable*>
	{
public:
	typedef map<int, TMLCompDataLine*> TMLComponents;

public: // constructors
	TMLCompData();
	TMLCompData(const TMLCompData& aOther);
	TMLCompData& operator=(const TMLCompData& aOther);

public: // new methods
	virtual ~TMLCompData();
	TMLCompDataLine* FindComponent(const string& aName) const;
	TMLCompDataLine* FindLine(const string& aName) const;
	TMLCompDataTable* FindTable(int aId) const;
	TMLCompDataTable* FindTable(const string& aName) const;
	TMLCompDataTable::TMLCompDataSubTable* FindSubTable(const string& aName) const;
	void Merge(TMLCompData& aOther);
	void MergeComponents(TMLCompData& aOther);
	void Compile();
	void CreateTables();
	void DeleteComponents();

public: // member data
	string iName;
	bool iCanBeMirror;
	string iMasterName;
	string iLayoutName;
	string iVariantName;
	string iResolutionHeight;
	string iResolutionWidth;
	string iTimestamp;
	TMLComponents iComponents; // this is used to accumulate the lines, which are then compiled into tables
	bool iIsBaseInstance;
	TMLAttributes* iAttributes; // owned
	};


#endif

// End of File
