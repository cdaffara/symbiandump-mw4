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
*/


#ifndef MLEQCOMPDATA_H
#define MLEQCOMPDATA_H

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

class TMLEqCompDataLine;
class TMLEqCompDataTable;
class TMLEqCompData;
class TMLAttributes;
class FormulaTreeNode;

extern const string KEqCompDataPaneOutputOrder[];
extern const int KEqCompDataPaneOutputOrderSize;

extern const string KEqCompDataKeywordParamLeft;
extern const string KEqCompDataKeywordParamTop;
extern const string KEqCompDataKeywordParamRight;
extern const string KEqCompDataKeywordParamBottom;
extern const string KEqCompDataKeywordParamWidth;
extern const string KEqCompDataKeywordParamHeight;


/**
*  TMLEqCompDataFormula
*
* This represents a formula entry. It contains both the original string representation and any 
* subsequent parsed and resolved versions.
*/
struct TMLEqCompDataFormula
	{
public:
	TMLEqCompDataFormula();
	TMLEqCompDataFormula(const TMLEqCompDataFormula& aOther);
	TMLEqCompDataFormula(string aFormulaString);
	const TMLEqCompDataFormula& operator=(const TMLEqCompDataFormula& aOther);
	bool operator==(const TMLEqCompDataFormula& aOther) const;
	virtual ~TMLEqCompDataFormula();
public: // new methods
	void Compile();
public:
	string iFormulaString; // owned, must be present for lifetime of iFormulaTree
	int iZoomId;

	// don't think that we need these as they don't seem to be used
//	string lowerMargin;
//	string upperMargin;
//	string minLimit;
//	string maxLimit;

public:
	FormulaTreeNode* iFormulaTree; // owned, but it references iFormulaString
	};

typedef vector<TMLEqCompDataFormula> TMLEqCompDataFormulae;

enum TMLEqCompDataOptionSetOrientation
	{
	EMLEqCompDataOptionSetOrientationUndefined = 0x00,
	EMLEqCompDataOptionSetOrientationPortrait = 0x01,
	EMLEqCompDataOptionSetOrientationLandscape = 0x02
	};

/**
 * TMLEqCompDataValuesOptionSet
 * an option set for a given value, which may apply to either or both orientation
 */
struct TMLEqCompDataValuesOptionSet : public TMLEqCompDataFormulae
	{
public:
	void Compile();
public:
	TMLEqCompDataOptionSetOrientation iOrientation;
	};

typedef map<int, TMLEqCompDataValuesOptionSet> TMLEqCompDataValuesOptionSets;

/**
*  TMLEqCompDataValues 
*  The values stored in a cell in a layout line
*/
class TMLEqCompDataValues : public TMLEqCompDataFormulae
	{
public: // constructors, operators, etc
	TMLEqCompDataValues();
	TMLEqCompDataValues(TMLEqCompDataLine* aLine);
	virtual ~TMLEqCompDataValues();
	bool operator==(const TMLEqCompDataValues& aOther) const;

public: // new methods
    bool Merge(TMLEqCompDataLine* aLine, string aName, TMLEqCompDataValues& aOtherValues, bool aMirrorMerge);
	void Compile(int aOptionSetId);
	static string CppValue(const string& aValue);
	static string MirrorJustificationValue(const string& aValue);

public: // member data
	TMLEqCompDataLine* iLine; // not owned
	TMLEqCompDataValuesOptionSets iOptionSets; // owned
	string iName;
	};


//
// Parent Info
//

/**
*  TMLEqCompDataParentInfoSelector
*
*  The parent id and parent variety index for each variety index
*/
struct TMLEqCompDataParentInfoSelector
	{
public:
	TMLEqCompDataParentInfoSelector();
	TMLEqCompDataParentInfoSelector(int aParentId, int aParentVariety);
public:
	int iParentId;
	int iParentVariety;
	};

typedef map<int, TMLEqCompDataParentInfoSelector> TMLEqCompDataParentInfoVarieties;

/**
*  TMLEqCompDataParentInfo
*
*  Contains the parent info selector for each variety index
*/
class TMLEqCompDataParentInfo : public TMLEqCompDataParentInfoVarieties
	{
public: // constructors etc
	TMLEqCompDataParentInfo();
	TMLEqCompDataParentInfo(TMLEqCompDataLine* aLine);
	virtual ~TMLEqCompDataParentInfo();
public: // new methods
	void Merge(const TMLEqCompDataParentInfo& aOther);
public: // member data
	TMLEqCompDataLine* iLine; // not owned
	};

class TMLEqCompDataAttributeInfo;

/**
*  TMLEqCompDataLine 
*  A layout line
*/
class TMLEqCompDataLine : public map<string, TMLEqCompDataValues>
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
	TMLEqCompDataLine();
	TMLEqCompDataLine(const TMLEqCompDataLine& aOther);
	virtual ~TMLEqCompDataLine();

	bool operator==(const TMLEqCompDataLine& aOther) const;
	static bool lessthan(TMLEqCompDataLine* aLeft, TMLEqCompDataLine* aRight);
	bool ValuesEqual(const TMLEqCompDataLine& aOther) const;

public: // new methods
	bool Merge(TMLEqCompDataLine& aOther);
	void Compile(int aOptionSetId);
	TMLAttributeZoomLevels* GetAttributeZoomLevels(string aAttribName, int aVariety);

public: // get and set methods
	string Name() const;					
	int NumCols() const;
	int NumRows() const;
	void SetNumCols(int aNumCols);
	void SetNumRows(int aNumRows);

public: // helpers
	int EncodeFontId(int aHeight, int aPosture, int aWeight, int aCategory) const;
	bool MatchParams(const TMLEqCompDataLine& aLine) const;
	bool MatchNameDiscountingSuffix(const TMLEqCompDataLine& aLine) const;
	bool MatchType(const TMLEqCompDataLine& aLine) const;
	string NameDiscountingSuffix() const;
	string NameSuffix() const;

private: // helpers
	TMLAttributeZoomLevels* GetParentAttributeZoomLevels(string aAttribSetName, int aAttribId, int aVariety);
	TMLAttributeZoomLevels* FindAttributeZoomLevels(string aAttribSetName, int aAttribId);

public: // member data
	int iId;
	string iName;
	TComponentType iType;
	int iDrawingOrder;
	TMLEqCompDataTable* iParentTable;
	TMLEqCompDataParentInfo* iParentInfo;// owned
//	TMLEqCompDataAttributeInfo* iAttributeInfo;// owned
	bool iIsUnique;
	int iGlobalIndex;
	bool iIsMirroredHorizontally; // i.e. l and r are swapped

private:
	int iNumCols; // there is always at least one column
	int iNumRows; // there is always at least one row
	};


//
// Attribute Info
//

/**
*  TMLEqCompDataAttributeInfoSelector
*
*  Contains the attribute set name, which is used to 
*  disambiguate the attribute data for a given component
*/
struct TMLEqCompDataAttributeInfoSelector
	{
public:
	TMLEqCompDataAttributeInfoSelector();
	TMLEqCompDataAttributeInfoSelector(string aAttributeSetName);
public:
	string iAttributeSetName;
	};

typedef map<int, TMLEqCompDataAttributeInfoSelector> TMLEqCompDataAttributeInfoVarieties;

/**
*  TMLEqCompDataAttributeInfo
*
*  Contains the attribute info selector for each variety
*/
class TMLEqCompDataAttributeInfo : public TMLEqCompDataAttributeInfoVarieties
	{
public: // constructors etc
	TMLEqCompDataAttributeInfo();
	TMLEqCompDataAttributeInfo(TMLEqCompDataLine* aLine);
	virtual ~TMLEqCompDataAttributeInfo();
public: // new methods
	void Merge(const TMLEqCompDataAttributeInfo& aOther);
public: // member data
	TMLEqCompDataLine* iLine; // not owned
	};


typedef vector<TMLEqCompDataLine*> TMLEqCompDataLines;

/**
 * TMLEqCompDataTableOptionSet 
 * an option set for a given value, which may apply to either or both orientation
 */
struct TMLEqCompDataTableOptionSet : public TMLEqCompDataLines
	{
public:
	TMLEqCompDataTableOptionSet();
	TMLEqCompDataTableOptionSet(const TMLEqCompDataTableOptionSet& aOther);
	virtual ~TMLEqCompDataTableOptionSet();
	void Compile(int aOptionSetId);
public:
	TMLEqCompDataLine* FindLine(int aId) const;
public:
	TMLEqCompDataOptionSetOrientation iOrientation;
	};

typedef map<int, TMLEqCompDataTableOptionSet> TMLEqCompDataTableOptionSets;

/**
*  TMLEqCompDataTable 
*  A layout table
*/
class TMLEqCompDataTable : public TMLEqCompDataTableOptionSets
	{
public: // constructors etc
	TMLEqCompDataTable(TMLEqCompData* aTables);
	TMLEqCompDataTable(TMLEqCompData* aTables, const TMLEqCompDataTable& aOther);
	virtual ~TMLEqCompDataTable();

public: // new methods
	TMLEqCompDataLine* FindLine(const string& aName);
    void Merge(TMLEqCompDataTable& aOther);
	void Compile();
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
	TMLEqCompData* iTables;
	vector<string> iColumnNames;
	TMLEqCompDataLine* iParentLine; // not owned
	string iParentName;
	};




//
// the "List" xml is needed to get the name of each component.
//

/**
*  TMLEqListComponent
*
* This is the component element stored in the list xml. 
*/
struct TMLEqListComponent
	{
public:
	TMLEqListComponent();
public:
		int iId;
		string iName;
	};

//
// the "ParChild" xml is needed to get the parent relations
//

/**
*  TMLEqParChildComponent
*
* This is the component element stored in the parent chlid xml. 
*/
struct TMLEqParChildComponent
	{
public:
	TMLEqParChildComponent();
public:
	int iId;
	int iParentId;
	};



/**
 * TMLEqCompData 
 *
 * The core layout class - this represents a complete master layout, 
 * typically loaded from a .xml file that has been extracted from the internal
 * xml data of the layout tool
 */
class TMLEqCompData : public vector<TMLEqCompDataTable*>
	{
public:
	typedef map<int, TMLEqListComponent*> TMLEqListComponents;
	typedef map<int, TMLEqCompDataLine*> TMLEqCompDataComponents;
	typedef map<int, TMLEqParChildComponent*> TMLEqParChildComponents;

public: // constructors
	TMLEqCompData();
	TMLEqCompData(const TMLEqCompData& aOther);
	TMLEqCompData& operator=(const TMLEqCompData& aOther);

public: // new methods
	virtual ~TMLEqCompData();
	TMLEqCompDataLine* FindComponent(const string& aName) const;
	TMLEqCompDataLine* FindLine(const string& aName) const;
	TMLEqCompDataTable* FindTable(int aId) const;
	TMLEqCompDataTable* FindTable(const string& aName) const;
	void Merge(TMLEqCompData& aOther);
	void MergeComponents(TMLEqCompData& aOther);
	void Compile();
	void UpdateNames();
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
	TMLEqListComponents iListComponents; // this is used to accumulate the lines from the , which are then compiled into tables
	TMLEqCompDataComponents iComponents; // this is used to accumulate the lines, which are then compiled into tables
	TMLEqParChildComponents iParChildComponents; // this is used to accumulate the lines, which are then compiled into tables
	bool iIsBaseInstance;
	TMLAttributes* iAttributes; // owned
	};


#endif

// End of File
