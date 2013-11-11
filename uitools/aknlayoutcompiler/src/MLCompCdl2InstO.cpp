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


// disable "identifier was truncated to '255' characters in the browser information" warning
#pragma warning (disable:4786)

// disable "decorated name length exceeded, name was truncated" warning	
#pragma warning (disable:4503)

#include "MLCompDataParse.h"
#include "MLCompCdl2InstO.h"
#include "ZoomLevelNames.h"
#include "LayoutCompilerErr.h"
#include "LayoutParse.h"
#include "CppWriter.h"
#include "MLCompData2Cdl.h"
#include "LayCdl2InstO.h" // for common classes
#include "MLAttributesParse.h"
#include "MLAttributes.h"
#include "CodeGenConsts.h"
#include "UsefulDefinitions.h"

#include <AknDef.hrh>

#include <fstream>
#include <algorithm>
#include <iostream>
#include <numeric> // accumulate

using namespace std;
using namespace CdlCompilerToolkit;

//
// defines
//

#define AKNLAYOUT_DEFINE_BYTECODE(name,byte) const char name = char(byte);
#include "AknLayoutByteCodes.h"

typedef LayoutProcessArgsErr<MLCompDataCdlInstanceOpt> MLCompDataCdlInstanceOptArgsErr;


//
// constants
//

const string KDefinitionNotDefined("Layout not defined, this API must not be called for this instance");
const string KDefinitionNotSet("Layout definition not found");
extern string KMultiLine;
const string KSpace = " ";
const string KComma = ",";
const string KTheWordBlank("blank");
const string KParentRelativeMarker("Pp");
const string KCellNameJustification("J");
const string KCompDataFileNameSuffix("compdata");
const string KAttributesFileNameSuffix("attributes");

// this is the addressable area for each instance, as the lookup table is 16bit
const int KAddressableBytecodedData = 0xFFFF;

/**
*  SCompDataImplFunc
*  This class represents a layout data decompression function local to a layout instance.
*  These functions will call the corresponding DLL wide function for decompression.
*/
struct SCompDataImplFunc
	{
	enum TFuncType
		{ 
		EWindowLine, 
		ETextLine,
		ETableLimits,
		EWindowTable, 
		ETextTable,
		ELineParamLimits,
		ETableParamLimits,
		EGenericComponentType,
		EGenericParamLimits,
		EGenericWindowComponent,
		EGenericTextComponent
		};
	TFuncType iType;
	CCdlTkApiParams iParams;
	string iDefn;
	string iPtrRef;

	SCompDataImplFunc(TFuncType aType, string aDefn, string aPtrRef, CCdlTkApiParams& aParams)
		: 
		iType(aType),
		iParams(aParams),
		iDefn(aDefn), 
		iPtrRef(aPtrRef)
		
		{
			
		}

	SCompDataImplFunc(TFuncType aType, string aDefn, string aPtrRef)
		: 
		iType(aType), 
		iDefn(aDefn), 
		iPtrRef(aPtrRef)
		{
			
		}

	bool IsSimilar(TFuncType aType, CCdlTkApiParams& aParams)
		{
		if(iType != aType) 
			return false;
		unsigned int size = iParams.size();
		if(size != aParams.size())
			return false;
		for(unsigned int ii = 0; ii < size; ii++)
			{
			CCdlTkApiParam& thisParam = iParams[ii];
			CCdlTkApiParam& otherParam = aParams[ii];
			if(thisParam.Name() != otherParam.Name())
				return false;
			if(thisParam.Type() != otherParam.Type())
				return false;
			// don't compare the default value, as we're not interested in that for the implementation.
			}
		return true;
		}
	};


/**
*  CAllCompDataFuncs
*  This represents a collection of all the SCompDataImplFunc objects that a layout instance may need
*/
class CAllCompDataFuncs : public vector<SCompDataImplFunc>
	{
public:
	CAllCompDataFuncs();

private:
	void AddFunc(SCompDataImplFunc::TFuncType aType, CCdlTkApiParams& aParams, const string& aReturn, const string& aFuncName, bool aAppendAbbreviation);
	void AddParamsToFunc(CCdlTkApiParams& aParams, string& aDefn, string& aBody);
	void AddParamToFunc(string aFiller, string aParamName, string& aDefn, string& aBody);
	};



/**
*  gTheFuncs
*  This is a collection of all SCompDataImplFunc objects that a layout instance needs, initialised
*  so that there are up to four integer parameters per API type.
*/
CAllCompDataFuncs gTheCompDataFuncs;

//
// CAllCompDataFuncs
//

CAllCompDataFuncs::CAllCompDataFuncs()
	{
	// need to generate all combinations of parameters
	vector<string> paramNames;
	paramNames.push_back(KParamOptionIndex);
	paramNames.push_back(KParamColIndex);
	paramNames.push_back(KParamRowIndex);

	// all cominations of option, row, and column are possible, in any order
	typedef vector<int> Seq;
	Seq pattern;
	set< Seq > subPatterns;

	// for each of the available positions, there could be any available value, or it could be empty
	const int numPlaces = paramNames.size();
	for(int jj = 0; jj < numPlaces; jj++)
		pattern.push_back(jj);

	// for each permutation, copy all the possible positions
	// into a separate set, which hence will be filled with the unique permutations
	do
		{
		int numMasks = (1 << numPlaces); // eg for 3 places there are 8 masks
		for(int mask = 0; mask < numMasks; mask++)
			{
			Seq subPattern;
			int bit = 0;
			// count down the bits
			for(int bitCount = mask; bitCount != 0; bitCount >>= 1)
				{
				if(mask & (1 << bit))
					subPattern.push_back(pattern[bit]);
				bit++;
				}
			subPatterns.insert(subPattern);
			}
		}
	while(next_permutation(pattern.begin(), pattern.end()));

	for(set< Seq >::iterator pPattern = subPatterns.begin(); pPattern != subPatterns.end(); ++pPattern)
		{
		CCdlTkApiParams params;
		for(Seq::const_iterator pParam = pPattern->begin(); pParam != pPattern->end(); ++pParam)
			{
			int param = (*pParam);
			string name = paramNames[param];
			params.push_back(CCdlTkApiParam(KTypeInt, name));
			}
		AddFunc(SCompDataImplFunc::EWindowLine, params, KTypeWindowComponentLayout, KFuncWindowLine, true);
		AddFunc(SCompDataImplFunc::ETextLine, params, KTypeTextComponentLayout, KFuncTextLine, true);

		params.insert(params.begin(), CCdlTkApiParam(KTypeInt, KParamLineIndex));
		AddFunc(SCompDataImplFunc::EWindowTable, params, KTypeWindowComponentLayout, KFuncWindowTable, true);
		AddFunc(SCompDataImplFunc::ETextTable, params, KTypeTextComponentLayout, KFuncTextTable, true);
		}

	for(int ii = 0; ii < 2; ii++)
		{
		CCdlTkApiParams params;
		if(ii)
			{
			params.push_back(CCdlTkApiParam(KTypeInt, KParamOptionIndex));
			}
		AddFunc(SCompDataImplFunc::ELineParamLimits, params, KTypeLayoutScalableParameterLimits, KFuncParamLimits, true);
		params.insert(params.begin(), CCdlTkApiParam(KTypeInt, KParamLineIndex));
		AddFunc(SCompDataImplFunc::ETableParamLimits, params, KTypeLayoutScalableParameterLimits, KFuncParamLimitsTable, true);
		}

	SCompDataImplFunc limits(
		SCompDataImplFunc::ETableLimits, 
		KTypeLayoutScalableTableLimits + " Limits() { return AknLayoutScalableDecode::TableLimits(KDataLookup); }",
		"&Limits");
	push_back(limits);

	// finally, the generic APIs
	CCdlTkApiParams params;
	params.push_back(CCdlTkApiParam(KTypeInt, KParamComponentId));
	AddFunc(SCompDataImplFunc::EGenericComponentType, params, KTypeLayoutScalableComponentType, KFuncGetComponentTypeById, false);

	// param limits needs the variety index
	params.push_back(CCdlTkApiParam(KTypeInt, KParamOptionIndex));
	AddFunc(SCompDataImplFunc::EGenericParamLimits, params, KTypeLayoutScalableParameterLimits, KFuncGetParamLimitsById, false);

	// and getting the component by id requires all the params
	params.push_back(CCdlTkApiParam(KTypeInt, KParamColIndex));
	params.push_back(CCdlTkApiParam(KTypeInt, KParamRowIndex));
	AddFunc(SCompDataImplFunc::EGenericWindowComponent, params, KTypeWindowComponentLayout, KFuncGetWindowComponentById, false);
	AddFunc(SCompDataImplFunc::EGenericTextComponent, params, KTypeTextComponentLayout, KFuncGetTextComponentById, false);
	}

void CAllCompDataFuncs::AddFunc(SCompDataImplFunc::TFuncType aType, CCdlTkApiParams& aParams, const string& aReturn, const string& aFuncName, bool aAppendAbbreviation)
	{
	// create a function of this form:
	//TAknWindowComponentLayout WindowTable$NUM$PARENT($PARAMS_TYPES_AND_NAMES)
	//	{
	//	return AknLayoutScalableDecode::WindowLine$NUM$PARENT(&KImplData, $PARAM_NAMES);
	//  }
	string funcName = aFuncName;
	if(aAppendAbbreviation)
		for(CCdlTkApiParams::iterator pParam = aParams.begin(); pParam != aParams.end(); ++pParam)
			funcName += pParam->Name().substr(1, 1); // append the first character after the "a"

	string defn = aReturn + " " + funcName + "(";
	string body = string(") { return AknLayoutScalableDecode::") + funcName + "(&KImplData";
	AddParamsToFunc(aParams, defn, body);
	defn += body + "); }";

	string ptrRef = string("&") + funcName;

	SCompDataImplFunc func(aType, defn, ptrRef, aParams);
	push_back(func);
	}

void CAllCompDataFuncs::AddParamsToFunc(CCdlTkApiParams& aParams, string& aDefn, string& aBody)
	{
	string filler = "";
	for(CCdlTkApiParams::iterator pParam = aParams.begin(); pParam != aParams.end(); ++pParam)
		{
		AddParamToFunc(filler, pParam->Name(), aDefn, aBody);
		filler = KComma + KSpace;
		}
	}

void CAllCompDataFuncs::AddParamToFunc(string aFiller, string aParamName, string& aDefn, string& aBody)
	{
	aDefn += aFiller + string(KTypeInt) + KSpace + aParamName;
	aBody += KComma + KSpace + aParamName;
	}


//
// CMLCompDataInstOptImpl
//

class CMLCompDataInstOptImpl
	{
public:
	CMLCompDataInstOptImpl(CCdlTkImplementation* aImpl);
	virtual ~CMLCompDataInstOptImpl();
public:
	CCdlTkImplementation* iImpl;
	int iByteCodeIndex;
	string iComment;
	string iName;
	vector<char> iBytes;
	bool iIsRedirectedToExactCopy;
	};

CMLCompDataInstOptImpl::CMLCompDataInstOptImpl(CCdlTkImplementation* aImpl)
	:
	iImpl(aImpl),
	iIsRedirectedToExactCopy(false)
	{
	}

CMLCompDataInstOptImpl::~CMLCompDataInstOptImpl()
	{
	}

//
// CMLCompDataLineInstOptImpl
//

class CMLCompDataLineInstOptImpl : public CMLCompDataInstOptImpl
	{
public:
	CMLCompDataLineInstOptImpl(TMLCompDataLine* aLine, CCdlTkImplementation* aImpl);
	virtual ~CMLCompDataLineInstOptImpl();
public:
	TMLCompDataLine* iLine;
	};

CMLCompDataLineInstOptImpl::CMLCompDataLineInstOptImpl(TMLCompDataLine* aLine, CCdlTkImplementation* aImpl)
	: 
	CMLCompDataInstOptImpl(aImpl),
	iLine(aLine)
	{
	}

CMLCompDataLineInstOptImpl::~CMLCompDataLineInstOptImpl()
	{
	}


//
// CMLCompDataSubTableInstOptImpl
//

class CMLCompDataSubTableInstOptImpl : public CMLCompDataInstOptImpl
	{
public:
	CMLCompDataSubTableInstOptImpl(
		TMLCompDataTable* aTable,
		TMLCompDataTable::TMLCompDataSubTable* aSubTable, 
		CCdlTkImplementation* aImpl);
	virtual ~CMLCompDataSubTableInstOptImpl();
public:
	TMLCompDataTable* iTable;
	TMLCompDataTable::TMLCompDataSubTable* iSubTable;
	};

CMLCompDataSubTableInstOptImpl::CMLCompDataSubTableInstOptImpl(
	TMLCompDataTable* aTable,
	TMLCompDataTable::TMLCompDataSubTable* aSubTable, 
	CCdlTkImplementation* aImpl)
	:
	CMLCompDataInstOptImpl(aImpl),
	iTable(aTable),
	iSubTable(aSubTable)
	{
	}

CMLCompDataSubTableInstOptImpl::~CMLCompDataSubTableInstOptImpl()
	{
	}


//
// CMLCompDataInstOpt
//

class CMLCompDataInstOpt
	{
public:
	CMLCompDataInstOpt(MLCompDataCdlInstanceOpt& aInstances, TMLCompData* aLayout, const string& aInstName, const string& aZoomName, int aZoomLevel, bool aAllParams, bool aNonCompleteInstance);
	~CMLCompDataInstOpt();

	void Process(const string& aFirstInstanceName);
	void WriteInstance();

	CCdlTkInstance& Inst() const { return *iInstance; }
	string Name() const { return iName; }
	string ZoomName() const { return iZoomName; }

private:
	typedef vector<int> ParamLimitVarieties;
	typedef vector<ParamLimitVarieties> ParamLimits;
	typedef map<int, int> IndexMap;

private:
	void ProcessLines(TMLCompDataTable& aTable);
	void ProcessTables(TMLCompDataTable& aTable);
	void ProcessLine(TMLCompDataLine& aLine);
	void ProcessLineApi(TMLCompDataLine& aLine, CCdlTkImplementation& aImpl);
	void UpdateBaseOffset();
	void UpdateLine(CMLCompDataInstOptImpl& aImpl, int& aByteCodeSizeDelta);
	void UpdateLineImpls();
	void UpdateLineImplDefn(CMLCompDataInstOptImpl& aImpl);
	void UpdateTableInstance(TMLCompDataTable& aTable, TMLCompDataTable::TMLCompDataSubTable& aSub, CMLCompDataInstOptImpl* aImpl);

	void SetNewLineData(CMLCompDataLineInstOptImpl& aImpl);
	void SetNewLineParamData(CMLCompDataLineInstOptImpl& aImpl, IndexMap& aIndexMap, int aTotalMax, int aNumVarieties);
	void OptimizeNewLineData(CMLCompDataLineInstOptImpl& aImpl, IndexMap& aIndexMap, vector<char>& aTempBytes, const vector<string>& aOutputOrder, bool aMirrored, int aTotalMax, int aNumVarieties);
	void OptimizeNewLineCellData(IndexMap& aIndexMap, vector<char>& aTempBytes, TMLCompDataValues& aValues, unsigned int& aNextCell, int aTotalParams, int aNumVarieties, ParamLimitVarieties& aParamLimitVarieties, string cellName, bool aMirrorJustification);
	void EncodeNewLineCellData(IndexMap& aIndexMap, vector<char>& aTempBytes, vector<string>& aValuesToEncode, unsigned int& aNextCell, int aTotalMax, bool aOptimizeVarieties, bool aOptimizeCalcs);

	bool HasApi(const string& aName);
	CCdlTkImplementation& FindImp(const string& aName);
	CMLCompDataInstOptImpl* FindSimilarImpl(const CMLCompDataInstOptImpls& aImpls, const CCdlTkImplementation& aImpl);
	string DefinitionString(int aByteCodeIndex, const string& aApiName);
	bool CheckByteCodeIndexInRange(int aByteCodeIndex);

	void SetGenericFunc(CMLCompDataInstOptImpl& aImpl, SCompDataImplFunc::TFuncType aType);
	void SetLineFunc(CMLCompDataLineInstOptImpl& aImpl);
	void SetSubTableFunc(CMLCompDataSubTableInstOptImpl& aImpl);
	void SetParamLimits(CMLCompDataLineInstOptImpl& aImpl);

	void ValidateRequiredParams(string aApiName, CCdlTkApiParams& aParams, bool aOption, bool aColumn, bool aRow);
	void CountApiParams(CCdlTkImplementation& aApi, int& aParams);
	SCompDataImplFunc& AddImplFunc(SCompDataImplFunc::TFuncType aType);
	SCompDataImplFunc& AddImplFunc(SCompDataImplFunc::TFuncType aType, CCdlTkApiParams& aParams);

	void SetGenericAPI(SCompDataImplFunc::TFuncType aType, const string& aName);
	void SetExtraCpp(const string& aFirstInstanceName);
	void OutputStats();

	void AddTableToInstance(TMLCompDataTable& aTable, TMLCompDataTable::TMLCompDataSubTable& aSub, int aTableNum);
	void UpdateTables(TMLCompDataTable& aTable);
	void AddTableParamLimitsImpl(const string& aApiName, int aByteCodeIndex, SCompDataImplFunc::TFuncType aType, bool aNeedsOptions);
	void UpdateTableParamLimitsImpl(const string& aApiName, int aByteCodeIndex);
	void AddTableLimitsImpl(const string& aApiName, TMLCompDataTable::TMLCompDataSubTable& aSubTable);
	void AddTableLimitsImplDefn(TMLCompDataTable::TMLCompDataSubTable& aSubTable, CCdlTkImplementation& aImpl);
	void AddTableImpl(const string& aApiName, TMLCompDataTable& aTable, TMLCompDataTable::TMLCompDataSubTable& aSub);
	void AddParamLimits(TMLCompDataLine& aLine, bool aNeedsOptions);
	void UpdateParamLimits(const string& apiName);
	void SetDummyTableData(TMLCompDataTable& aTable, TMLCompDataTable::TMLCompDataSubTable& aSubTable, CMLCompDataInstOptImpl& aImpl);
	void UpdateTableData(TMLCompDataTable& aTable, TMLCompDataTable::TMLCompDataSubTable& aSubTable, CMLCompDataInstOptImpl& aImpl);
	void EncodeValue(vector<char>& aBytes, string aValue);
	void EncodeValue(vector<char>& aBytes, int aValue);
	void EncodeParentRelativeValue(vector<char>& aBytes, int aValue);
	void MirrorParamName(string& aParamName);

private:
	MLCompDataCdlInstanceOpt& iInstances;
	TMLCompData* iLayout;
	string iName;
	string iZoomName;
	CCdlTkInterface& iInterface;
	CCdlTkInstance* iInstance; // not owned
	CMLCompDataInstOptImpls iImpls;
	CMLCompDataInstOptImpls iTableImpls;
	typedef vector<SCompDataImplFunc*> CImplFuncs;
	CImplFuncs iFuncs;
	int iZoomLevel;
	bool iAllParams;
	int iBaseOffset; // offset from the start of the main data table to the start of this instance's data
	};

CMLCompDataInstOpt::CMLCompDataInstOpt(MLCompDataCdlInstanceOpt& aInstances, TMLCompData* aLayout, const string& aInstName, const string& aZoomName, int aZoomLevel, bool aAllParams, bool aNonCompleteInstance)
	: 
	iInstances(aInstances), 
	iLayout(aLayout),
	iName(aInstName), 
	iZoomName(aZoomName),
	iInterface(iInstances.Interface()),
	iZoomLevel(aZoomLevel),
	iAllParams(aAllParams),
	iBaseOffset(0)
	{
	string zoomInstName = aInstName;
	if(aZoomName.length() > 0)
		zoomInstName += "_" + aZoomName;

	iInstance = new CCdlTkInstance(iInterface);
	iInstance->SetName(zoomInstName);

	CCdlTkImplementations& impl = iInstance->Impl();
	for (CCdlTkImplementations::iterator pImpl = impl.begin(); pImpl != impl.end(); ++pImpl)
		{
		const CCdlTkInterface& iface = (*pImpl)->Instance().Interface();
		const CCdlTkApi& api = (*pImpl)->Api();
		if(aLayout->iIsBaseInstance)
			{
			// for a base instance, need to set a default implementation
			// even for missing data
			(*pImpl)->SetDefinition(CdlTkUtil::ShortToHexString(0) + ",\t// " + KDefinitionNotDefined);
			
			// preliminary implementation of "blank" implementation. cdl engine searches to ensure that
			// all implementations are not NULL. So this will satisfy that test. However, if any of the 
			// methods are called, there will be an access violation, which will cause a panic
			// this will be a successful guide to implementers not to call the API when the wrong layout
			// pack is installed.
			string definition = "(" + iface.NamespaceName() + "::" + api.PointerType() + ")((void*)4), // LAYOUT NOT DEFINED FOR THIS INSTANCE";
			(*pImpl)->SetPointerReference(definition); 
			}
        else if(aNonCompleteInstance)
            {
			// for a base instance, need to set a default implementation
			// even for missing data
			(*pImpl)->SetDefinition(CdlTkUtil::ShortToHexString(0) + ",\t// " + KDefinitionNotDefined);

            // but we need the pointer to be null so that CdlEngine falls through the layer below
			string definition =  "0, // " + iface.NamespaceName() + "::" + api.PointerType();
			(*pImpl)->SetPointerReference(definition); 
            }
        else
			{
			// Initially set definition that will not compile in the resulting code.
			// This will alert the programmer to missing layout data.
			(*pImpl)->SetDefinition(KDefinitionNotSet);
			}
		}
	}

CMLCompDataInstOpt::~CMLCompDataInstOpt()
	{
	delete iInstance;
	for (CMLCompDataInstOptImpls::iterator pImpl = iImpls.begin(); pImpl != iImpls.end(); ++pImpl)
		delete *pImpl;
	for (CMLCompDataInstOptImpls::iterator pImpl = iTableImpls.begin(); pImpl != iTableImpls.end(); ++pImpl)
		delete *pImpl;
	}

void CMLCompDataInstOpt::Process(const string& aFirstInstName)
	{
	iLayout->Compile();

	for (TMLCompData::iterator pTab = iLayout->begin(); pTab != iLayout->end(); ++pTab)
		{
		ProcessLines(**pTab);
		}

	for (TMLCompData::iterator pTab = iLayout->begin(); pTab != iLayout->end(); ++pTab)
		{
		ProcessTables(**pTab);
		}

	// update the base offset to include all the data added, including reserving space for the tables
	UpdateBaseOffset();

	// with the correct base offset, the definitions can be updated
	UpdateLineImpls();

	for (TMLCompData::iterator pTab = iLayout->begin(); pTab != iLayout->end(); ++pTab)
		{
		// now that the rest of the data is stable, we can add the tables at the end
		UpdateTables(**pTab);
		}

	SetGenericAPI(SCompDataImplFunc::EGenericComponentType, KFuncGetComponentTypeById);
	SetGenericAPI(SCompDataImplFunc::EGenericParamLimits, KFuncGetParamLimitsById);
	SetGenericAPI(SCompDataImplFunc::EGenericWindowComponent, KFuncGetWindowComponentById);
	SetGenericAPI(SCompDataImplFunc::EGenericTextComponent, KFuncGetTextComponentById);
	
	SetExtraCpp(aFirstInstName);
	OutputStats();
	}

void CMLCompDataInstOpt::WriteInstance()
	{
	CCdlTkWriteInstance writer(*iInstance);
	writer.Process();
	}

void CMLCompDataInstOpt::ProcessLines(TMLCompDataTable& aTable)
	{
	for (TMLCompDataTable::iterator pLine = aTable.begin(); pLine != aTable.end(); ++pLine)
		{
		ProcessLine(**pLine);
		}
	}

void CMLCompDataInstOpt::ProcessTables(TMLCompDataTable& aTable)
	{
	int line = 0;
	for (TMLCompDataTable::iterator pLine = aTable.begin(); pLine != aTable.end(); ++pLine)
		{
		for (TMLCompDataTable::TMLCompDataSubTables::const_iterator pSub = aTable.iSubTables.begin(); pSub != aTable.iSubTables.end(); ++pSub)
			{
			TMLCompDataTable::TMLCompDataSubTable& sub = **pSub;
			int last = *(sub.rbegin());
			if(line == last)
				AddTableToInstance(aTable, sub, 0);
			}
		line++;
		}
	}

void CMLCompDataInstOpt::ProcessLine(TMLCompDataLine& aLine)
	{
	string apiName = MLCompDataToCdl::LineApiName(aLine);
	if (!HasApi(apiName))
		return;

	ProcessLineApi(aLine, FindImp(apiName));
	}

void CMLCompDataInstOpt::ProcessLineApi(TMLCompDataLine& aLine, CCdlTkImplementation& aImpl)
	{
	string lineName = MLCompDataToCdl::LineApiName(aLine);

	CMLCompDataLineInstOptImpl* newImpl = new CMLCompDataLineInstOptImpl(&aLine, &aImpl);
	iImpls.push_back(newImpl);

	// always set the new line data
	SetNewLineData(*newImpl);

	// if we can find the new line data in the aggregated data, point to that instead
	int foundIndex = iInstances.FindSimilarBytes(newImpl, iBaseOffset);
	if(foundIndex >= 0)
		{
		newImpl->iByteCodeIndex = foundIndex;
		newImpl->iIsRedirectedToExactCopy = true;
		}
	else
		{
		iInstances.AddImpl(newImpl);
		}

	SetLineFunc(*newImpl);

	// only add parameter limits if the line is not simple
	// and we must add the param limits after the actual impl, so that we can find it!
	bool needsParamLimits = aLine.NeedsOptions() || aLine.NeedsCols() || aLine.NeedsRows();
	if(needsParamLimits)
		{
		AddParamLimits(aLine, aLine.NeedsOptions());
		}
	}

void CMLCompDataInstOpt::UpdateBaseOffset()
	{
	// have to account for this instance's tables
	int tablesSize(0);
	for(CMLCompDataInstOptImpls::iterator pTableImpl = iTableImpls.begin(); pTableImpl != iTableImpls.end(); ++pTableImpl)
		{
		CMLCompDataInstOptImpl* impl = *pTableImpl;
		tablesSize += impl->iBytes.size();
		}

	int origByteStreamSize = iInstances.ByteStreamSize() + tablesSize;

	// repeat until base offset stabilizes
	iBaseOffset = max(0, origByteStreamSize - KAddressableBytecodedData);
	int newBaseOffset = iBaseOffset;
	int byteCodeSizeDelta(0);
	do
		{
		iBaseOffset = newBaseOffset;
		for(CMLCompDataInstOptImpls::iterator pImpl = iImpls.begin(); pImpl != iImpls.end(); ++pImpl)
			UpdateLine(**pImpl, byteCodeSizeDelta);
		newBaseOffset = max(0, origByteStreamSize + byteCodeSizeDelta - KAddressableBytecodedData);
		}
	while(iBaseOffset != newBaseOffset);
	}

void CMLCompDataInstOpt::UpdateLine(CMLCompDataInstOptImpl& aImpl, int& aByteCodeSizeDelta)
	{
	if(aImpl.iIsRedirectedToExactCopy && aImpl.iByteCodeIndex < iBaseOffset)
		{
		// when we encoded the impls, we found a match in an area which now
		// turns out to be before the base offset (i.e. outside the addressable range).

		// first of all check in case we've already made another copy close by
		int foundIndex = iInstances.FindSimilarBytes(&aImpl, iBaseOffset);
		if(foundIndex >= 0)
			{
			aImpl.iByteCodeIndex = foundIndex;
			}
		else
			{
			// add its bytecodes to the end
			aImpl.iIsRedirectedToExactCopy = false;
			iInstances.AddImpl(&aImpl);
			aByteCodeSizeDelta += aImpl.iBytes.size();
			}
		}
	}


void CMLCompDataInstOpt::UpdateLineImpls()
	{
	for(CMLCompDataInstOptImpls::iterator pImpl = iImpls.begin(); pImpl != iImpls.end(); ++pImpl)
		UpdateLineImplDefn(**pImpl);
	}

void CMLCompDataInstOpt::UpdateLineImplDefn(CMLCompDataInstOptImpl& aImpl)
	{
	if(!CheckByteCodeIndexInRange(aImpl.iByteCodeIndex))
		throw GeneralErr(aImpl.iName + " in interface " + iInterface.FileName());
	// the base offset has been updated, so all definitions must be refreshed
	//int adjustedIndex = aImpl.iByteCodeIndex - iBaseOffset;
	aImpl.iImpl->SetDefinition(DefinitionString(aImpl.iByteCodeIndex, aImpl.iName));
	UpdateParamLimits(aImpl.iName); 
	}

void CMLCompDataInstOpt::UpdateTables(TMLCompDataTable& aTable)
	{
	// regenerate the byte code indices, in case they have changed
	int line = 0;
	for (TMLCompDataTable::iterator pLine = aTable.begin(); pLine != aTable.end(); ++pLine)
		{
		for (TMLCompDataTable::TMLCompDataSubTables::const_iterator pSub = aTable.iSubTables.begin(); pSub != aTable.iSubTables.end(); ++pSub)
			{
			TMLCompDataTable::TMLCompDataSubTable& sub = **pSub;
			int last = *(sub.rbegin());
			if(line == last)
				{
				string tableName = MLCompDataToCdl::SubTableApiName(sub);
				CMLCompDataInstOptImpl* impl = FindSimilarImpl(iTableImpls, FindImp(tableName));
				UpdateTableInstance(aTable, sub, impl);
				}
			}
		line++;
		}
	}

void CMLCompDataInstOpt::SetNewLineData(CMLCompDataLineInstOptImpl& aImpl)
	{
	aImpl.iBytes.clear();	

	TMLCompDataLine& line = *aImpl.iLine;
	aImpl.iComment = string("for line: ");
	aImpl.iName = MLCompDataToCdl::LineApiName(line);

	// handle mirroring order
	bool mirrored = false;
	if(line.iParentTable) // we know top table can't be mirrored
		mirrored = line.iParentTable->iTables->iCanBeMirror;
	const string* outputOrder =  mirrored ? 
		KCompDataGraphicOutputOrderMirrored : KCompDataGraphicOutputOrder;
	int outputSize = KCompDataGraphicOutputOrderSize;
	if (line.iType == TMLCompDataLine::ETextComponent)
		{
		outputOrder = mirrored ? 
			KCompDataTextOutputOrderMirrored : KCompDataTextOutputOrder;
		outputSize = KCompDataTextOutputOrderSize;
		}
	vector<string> cellOrder(outputOrder, outputOrder + outputSize);

	// we are currently storing variety as max allowed value, but it's a zero based index
	int numVarieties = line.MaxVariety() + 1;

	// we will be storing all of the rows and cols, even if the rows and cols limits are lower
	int totalMaxCols = numVarieties * line.NumCols();
	int totalMaxRows = numVarieties * line.NumRows();
    int totalMax = totalMaxCols > totalMaxRows ? totalMaxCols : totalMaxRows;

	// initialize the temporary bytestream, and the index map
	vector<char> tempBytes;
	IndexMap indexMap;

	// fill in the temp byte stream and the index map for each cell
	OptimizeNewLineData(aImpl, indexMap, tempBytes, cellOrder, mirrored, totalMax, numVarieties);

	// encode the index map
	SetNewLineParamData(aImpl, indexMap, totalMax, numVarieties);

	// finally copy the temp byte stream onto the end of the real encoded byte stream
	copy(tempBytes.begin(), tempBytes.end(), back_inserter(aImpl.iBytes));
	}

void CMLCompDataInstOpt::SetNewLineParamData(CMLCompDataLineInstOptImpl& aImpl, IndexMap& aIndexMap, int aTotalMax, int aNumVarieties)
	{
	// only set parameter data if the line is not simple
	TMLCompDataLine& line = *aImpl.iLine;
	
	bool needsHeader = line.NeedsOptions() || line.NeedsCols() || line.NeedsRows();

	int typeBitfield = line.iType << 1;
	if(needsHeader)
		{
		typeBitfield |= 1;
		}
	
	aImpl.iBytes.push_back(typeBitfield);

	if(needsHeader)
		{
		// we can just dump the number of varieties, this will be used by the limits API
		// and we can also them to calculate value indices within a cell when decoding
		aImpl.iBytes.push_back(aNumVarieties);
		}

	//  count the number of cells that are complex
	char numMultiValues = aIndexMap.size();
	if(needsHeader)
		{
		aImpl.iBytes.push_back(numMultiValues);
		}
	else
		{
		// we didn't think we needed a header, so there should be no complex cells!
		if (numMultiValues > 0)
			throw GeneralErr("complex cells detected in simple line");
		}

	// for each complex cell, encode the bitfield and the count
	for(IndexMap::iterator pIndex = aIndexMap.begin(); pIndex != aIndexMap.end(); ++pIndex)
		{
		int count = pIndex->first;
		int index = pIndex->second;
		EncodeValue(aImpl.iBytes, index);
		EncodeValue(aImpl.iBytes, count);
		}
	}

void CMLCompDataInstOpt::OptimizeNewLineData(
	CMLCompDataLineInstOptImpl& aImpl, 
	IndexMap& aIndexMap, 
	vector<char>& aTempBytes,
	const vector<string>& aOutputOrder,
	bool aMirrored,
	int aTotalMax, 
	int aNumVarieties)
	{
	vector<ParamLimitVarieties> paramLimits;
	TMLCompDataLine& line = *aImpl.iLine;

	// start by extracting the row and column data. 
	// we can optimize the stored format along with the rest of the data
	// so no need to optimize it yet
	for(int cell = 0; cell < 2; cell++)
		{
		string cellName = aOutputOrder[cell];
		ParamLimitVarieties nextLimits;
		TMLCompDataValues& values = line[cellName];
		for(int varietyIndex = 0; varietyIndex < aNumVarieties; varietyIndex++)
			{
			TMLCompDataZoomLevels& zoomLevels = values[varietyIndex];
			TMLCompDataCalcs* calcs = &(zoomLevels[iZoomLevel]);
			if(!calcs->size())
				calcs = &(zoomLevels[EAknUiZoomNormal]);
			int value = CdlTkUtil::ParseInt((*calcs)[0]);
			nextLimits.push_back(value);
			}
		paramLimits.push_back(nextLimits);
		}

	// calculate the number of values
	bool needsHeader = line.NeedsOptions() || line.NeedsCols() || line.NeedsRows();
	unsigned int nextCellFlag = 1;	// bit flag for the next cell, note it can get bigger than a char
	for (unsigned int cell = 0; cell < aOutputOrder.size(); cell++)
		{
		string cellName = aOutputOrder[cell];
		TMLCompDataValues::TCompDataCellType type = TMLCompDataValues::Type(cellName);
		if(!needsHeader && type == TMLCompDataValues::ECellTypeParamLimit)
			{
			// if we don't need header, then don't output the param limits values
			// although we'll still use them internally
			continue;
			}

		int paramLimitsIndex = TMLCompDataValues::Type(cellName) == TMLCompDataValues::ECellTypeCol ? 0 : 1; // if it's not a row or col, it doesn't matter what param limits are used.
		TMLCompDataValues& values = line[cellName];
		int numValues = 0;
		if(type == TMLCompDataValues::ECellTypeCol || type == TMLCompDataValues::ECellTypeRow)
			{
			ParamLimitVarieties& nextLimits = paramLimits[paramLimitsIndex];
			numValues = std::accumulate(nextLimits.begin(), nextLimits.end(), 0);
			}
		else
			{
			numValues = aNumVarieties;
			}

		bool mirrorJustification = (aMirrored && cellName == KCellNameJustification);
		OptimizeNewLineCellData(aIndexMap, aTempBytes, values, nextCellFlag, numValues, aNumVarieties, paramLimits[paramLimitsIndex], cellName, mirrorJustification);
		}
	}

void CMLCompDataInstOpt::OptimizeNewLineCellData(
	IndexMap& aIndexMap, 
	vector<char>& aTempBytes, 
	TMLCompDataValues& aValues, 
	unsigned int& aNextCell, 
	int aTotalParams,
	int aNumVarieties, 
	CMLCompDataInstOpt::ParamLimitVarieties& aParamLimitVarieties, 
	string cellName,
	bool aMirrorJustification)
	{
	// build up the values for each variety 
	// if the valid values for each variety are the same, then we can store them once only
	// in which case there will be as many values as the variety with the largest number of values
	vector<string> optimizedValues; 

	// also build up the found valid values in case we can't optimize
	vector<string> foundValues; 

	bool optimizeVarieties = true;
	bool optimizeCalcs = true;
	unsigned int numCalcs = 0;
	int* largestParamLimitPtr = max_element(aParamLimitVarieties.begin(), aParamLimitVarieties.end());
	unsigned int largestParamLimit = largestParamLimitPtr != aParamLimitVarieties.end() ? *largestParamLimitPtr : 0;
	
	// in the case of there being no calcs at all, we don't want to be optimizing,
	// in order to avoid wasted flags.
	if(largestParamLimit == 1)
		optimizeCalcs = false;

	TMLCompDataValues::TCompDataCellType type = TMLCompDataValues::Type(cellName);
	bool isColRow = (type == TMLCompDataValues::ECellTypeCol ||	type == TMLCompDataValues::ECellTypeRow);

	// only go up to the max variety, as any data past that may be from a spurious merged instance
	for(int varietyIndex = 0; varietyIndex < aNumVarieties; varietyIndex++)
		{
		TMLCompDataZoomLevels& zoomLevels = aValues[varietyIndex];
		TMLCompDataCalcs* calcs = &(zoomLevels[iZoomLevel]);
		if(!calcs->size())
			calcs = &(zoomLevels[EAknUiZoomNormal]);
		numCalcs = calcs->size();
		string value;
		vector<string> foundCalcs; 
		unsigned int paramLimit = aParamLimitVarieties[varietyIndex];
		if(numCalcs == 0)
			{
			if(varietyIndex == 0)
				optimizedValues.push_back(value);
			if(varietyIndex > 0 && !(optimizedValues[0].empty()))
				optimizeVarieties = false;
			foundCalcs.push_back(value);
			}
		else
			{
			for(unsigned int index = 0; index < numCalcs; index++)
				{
				bool needToCheckIndexValidity = !isColRow || (index < paramLimit);
				value = (*calcs)[index];
				value = aMirrorJustification ? TMLCompDataValues::MirrorJustificationValue(value) : value;

				if(index >= optimizedValues.size() && index < largestParamLimit) // index is zero based, size is quantity
					{
					// store the first valid value for a given index that we find.
					// note that this will pick up additional values in subsequent varieties
					optimizedValues.push_back(value);
					}
				else if(needToCheckIndexValidity && value != optimizedValues[index]) 
					{
					// so if it doesn't match, we can't optimize
					optimizeVarieties = false;
					}

				// collect the valid found values as we go, in case we aren't able to optimize
				if(needToCheckIndexValidity)
					{
					foundCalcs.push_back(value);
					}
				}
			}
		if(isColRow)
			{
			int found = foundCalcs.size();
			if(!found)
				{
				foundCalcs.push_back(string());
				found++;
				}
			if(found > 1)
				{
				optimizeCalcs = false;
				}
			}
		else
			{
			optimizeCalcs = false;
			}
		copy(foundCalcs.begin(), foundCalcs.end(), back_inserter(foundValues));
		}

	if(optimizeVarieties && isColRow)
		{
		// now that we know the optimized values, fill missing values by repeating the last one.
		// but if we're optimizing the calcs, then we don't want to expand them after all
		if(!optimizeCalcs)
			{
			unsigned int optimal = optimizedValues.size();
			string value = optimal > 0 ? optimizedValues[optimal-1] : string();
			for(; optimal < largestParamLimit; optimal++)
				{
				optimizedValues.push_back(value);
				}
			}
		}

	if(optimizeVarieties && aNumVarieties == 1)
		{
		optimizeVarieties = false;
		}

	vector<string>& valuesToEncode = optimizeVarieties ? optimizedValues : foundValues;
	EncodeNewLineCellData(aIndexMap, aTempBytes, valuesToEncode, aNextCell, aTotalParams, optimizeVarieties, optimizeCalcs);
	}

void CMLCompDataInstOpt::EncodeNewLineCellData(
	IndexMap& aIndexMap, 
	vector<char>& aTempBytes, 
	vector<string>& aValuesToEncode, 
	unsigned int& aNextCell,
	int aTotalMax,
	bool aOptimizeVarieties,
	bool aOptimizeCalcs)
	{
	// encode the actual data into the temporary bytestream
	int numOptimizedVals = aValuesToEncode.size();
	if(numOptimizedVals > 0)
		{
		for(vector<string>::iterator pString = aValuesToEncode.begin(); pString != aValuesToEncode.end(); ++pString)
			{
			EncodeValue(aTempBytes, *pString);
			}
		}
	else
		{
		EncodeValue(aTempBytes, "");
		}

	if(numOptimizedVals > aTotalMax)
		throw GeneralErr("bad index in indexMap");
	
	// if there is only one value stored, we can make a further optimization
	// as we know that all values must be the same, however they are stored
	// so in that case, don't store an index map entry
	if(numOptimizedVals > 1)
		{
		// update the index map, 
		// put the flags at the right hand side, to avoid it becoming multi-byte in general
		int optimizeVarietiesMask = (aOptimizeVarieties ? 1 : 0);
		int optimizeCalcsMask = (aOptimizeCalcs ? 1 : 0) << 1;
		int numValsMask = (numOptimizedVals) << 2;
		int indexField = (numValsMask | optimizeCalcsMask | optimizeVarietiesMask);
		aIndexMap[indexField] |= aNextCell;
		}
	aNextCell = aNextCell << 1;
	}

void CMLCompDataInstOpt::CountApiParams(CCdlTkImplementation& aApi, int& aParams)
	{
	aParams = 0;
	const CCdlTkApiParams& params = aApi.Api().AsFunc().Params();
	for (CCdlTkApiParams::const_iterator pParam = params.begin(); pParam != params.end(); ++pParam)
		{
		if (pParam->Type() == KTypeInt) // is it a cell index parameter
			aParams++;
		}
	}

void CMLCompDataInstOpt::ValidateRequiredParams(string aApiName, CCdlTkApiParams& aParams, bool aOption, bool aColumn, bool aRow)
	{
	// check that data matches api
	bool matches = true;
	matches &= ((aOption) == (aParams.FindByName(KParamOptionIndex) != aParams.end()));
	matches &= ((aColumn) == (aParams.FindByName(KParamColIndex) != aParams.end()));
	matches &= ((aRow) == (aParams.FindByName(KParamRowIndex) != aParams.end()));
	if(!matches)
		throw CdlTkAssert(string("layout data does not match CDL API : ") + aApiName);
	}

void CMLCompDataInstOpt::SetGenericFunc(CMLCompDataInstOptImpl& aImpl, SCompDataImplFunc::TFuncType aType)
	{
	CCdlTkFunctionApi& api = const_cast<CCdlTkFunctionApi&>(aImpl.iImpl->Api().AsFunc());
	CCdlTkApiParams& params = api.Params();

	SCompDataImplFunc& func = AddImplFunc(aType, params);
	aImpl.iImpl->SetPointerReference(func.iPtrRef);
	}

void CMLCompDataInstOpt::SetLineFunc(CMLCompDataLineInstOptImpl& aImpl)
	{
	SCompDataImplFunc::TFuncType type = SCompDataImplFunc::EWindowLine;
	if (aImpl.iLine->iType == TMLCompDataLine::ETextComponent)
		{
		type = SCompDataImplFunc::ETextLine;
		}
	string name = aImpl.iImpl->Name();

	// look up the api to see whether we need the params
	CCdlTkFunctionApi& api = const_cast<CCdlTkFunctionApi&>(aImpl.iImpl->Api().AsFunc());
	CCdlTkApiParams& params = api.Params();
	ValidateRequiredParams(
		name, 
		params, 
		iAllParams || aImpl.iLine->NeedsOptions(), 
		iAllParams || aImpl.iLine->NeedsCols(), 
		iAllParams || aImpl.iLine->NeedsRows());
	SCompDataImplFunc& func = AddImplFunc(type, params);
	aImpl.iImpl->SetPointerReference(func.iPtrRef);
	}

void CMLCompDataInstOpt::SetSubTableFunc(CMLCompDataSubTableInstOptImpl& aImpl)
	{
	TMLCompDataTable& table = *(aImpl.iTable);
	TMLCompDataTable::TMLCompDataSubTable& subTable = *(aImpl.iSubTable);
	TMLCompDataLine& line= *(table[subTable[0]]);

	SCompDataImplFunc::TFuncType type = SCompDataImplFunc::EWindowTable;

	if (line.iType == TMLCompDataLine::ETextComponent)
		{
		type = SCompDataImplFunc::ETextTable;
		}
	string name = aImpl.iImpl->Name();

	CCdlTkFunctionApi& api = const_cast<CCdlTkFunctionApi&>(aImpl.iImpl->Api().AsFunc());
	CCdlTkApiParams& params = api.Params();

	string debug;
	for(CCdlTkApiParams::iterator pParam = params.begin(); pParam != params.end(); ++pParam)
		debug = pParam->Name();

	ValidateRequiredParams(
		name, 
		params, 
		iAllParams || subTable.iNeedsOption, 
		iAllParams || subTable.iNeedsCol, 
		iAllParams || subTable.iNeedsRow);
	SCompDataImplFunc& func = AddImplFunc(type, params);
	aImpl.iImpl->SetPointerReference(func.iPtrRef);
	}

bool CMLCompDataInstOpt::HasApi(const string& aName)
	{
	return iInterface.ApiList().Find(aName) != 0;
	}

CCdlTkImplementation& CMLCompDataInstOpt::FindImp(const string& aName)
	{
	CCdlTkImplementation* impl = iInstance->Impl().Find(aName);
	if (!impl)
		throw NotFoundErr(aName + " in interface " + iInterface.FileName());
	return *impl;
	}

CMLCompDataInstOptImpl* CMLCompDataInstOpt::FindSimilarImpl(const CMLCompDataInstOptImpls& aImpls, const CCdlTkImplementation& aImpl)
	{
	for (CMLCompDataInstOptImpls::const_iterator pOptImpl = aImpls.begin(); pOptImpl != aImpls.end(); ++pOptImpl)
		{
		CMLCompDataInstOptImpl* optImpl = *pOptImpl;
		if (optImpl->iImpl == &aImpl)
			return optImpl;
		}
	return NULL;
	}

string CMLCompDataInstOpt::DefinitionString(int aByteCodeIndex, const string& aApiName)
	{
	int adjustedIndex = aByteCodeIndex - iBaseOffset;
	return CdlTkUtil::ShortToHexString(adjustedIndex) + ",\t// (" + CdlTkUtil::IntToHexString(aByteCodeIndex) + ") " + aApiName;
	}

bool CMLCompDataInstOpt::CheckByteCodeIndexInRange(int aByteCodeIndex)
	{
	int adjustedIndex = aByteCodeIndex - iBaseOffset;
	if(adjustedIndex > KAddressableBytecodedData)
		{
		cerr << "Error: Out of range: index = "  << CdlTkUtil::IntToHexString(aByteCodeIndex) << ", ";
		cerr << "baseOffset = " << CdlTkUtil::IntToHexString(iBaseOffset ) << endl;
		return false;
		}
	return true;
	}

SCompDataImplFunc& CMLCompDataInstOpt::AddImplFunc(SCompDataImplFunc::TFuncType aType)
	{
	CCdlTkApiParams params;
	return AddImplFunc(aType, params);
	}

SCompDataImplFunc& CMLCompDataInstOpt::AddImplFunc(SCompDataImplFunc::TFuncType aType, CCdlTkApiParams& aParams)
	{
	for (CImplFuncs::iterator pFunc = iFuncs.begin(); pFunc != iFuncs.end(); ++pFunc)
		{
		SCompDataImplFunc& func = **pFunc;
		if(func.IsSimilar(aType, aParams))
			return func;
		}

	int count = gTheCompDataFuncs.size();
	for (int ii=0; ii<count; ii++)
		{
		SCompDataImplFunc* func = &gTheCompDataFuncs[ii];
		if(func->IsSimilar(aType, aParams)) 
			{
			iFuncs.push_back(func);
			return *func;
			}
		}

	throw NotFoundErr("implementation function");
	return gTheCompDataFuncs[0];
	}

void CMLCompDataInstOpt::SetGenericAPI(SCompDataImplFunc::TFuncType aType, const string& aName)
	{
	CCdlTkImplementation* impl = iInstance->Impl().Find(aName);

	// CMLCompDataInstOptImpl(CCdlTkImplementation* aImpl);
	CMLCompDataInstOptImpl* newImpl = new CMLCompDataInstOptImpl(impl);
	iImpls.push_back(newImpl);

	SetGenericFunc(*newImpl, aType);
	newImpl->iImpl->SetDefinition(CdlTkUtil::ShortToHexString(0) + ","); // no specific data for generic apis
	}

// The following strings and the SetExtraCpp() function build the gross structure of
// the C++ customisation instance.
// So far, the implementations are actually just 16-bit values, typically indexes into
// the data lookup table. These need to be turned into an array by adding declarations
// and brackets to the first and last implementations. Extra support functions are also
// added.
// extern string KScalableExtraCpp;
string KScalableExtraCpp = "\
#include \"aknlayout2scalabledecode.h\"\n\
namespace $INTERFACE_NS { extern const TUint8 KByteCodedData_$FIRSTINSTANCENAME[]; }\n";

//extern string KScalableInitialCpp;
string KScalableInitialCpp ="\
extern const TUint16 KDataLookup[$INTERFACE_NS::E_TApiId_TableSize];\n\
const SCompDataImplData KImplData = { KDataLookup, $INTERFACE_NS::KByteCodedData_$FIRSTINSTANCENAME + $THISINSTANCEBASEOFFSET };\n\
\n\
$FUNCTIONS\
\n\
const TUint16 KDataLookup[$INTERFACE_NS::E_TApiId_TableSize] =\n\
\t{\n";
void CMLCompDataInstOpt::SetExtraCpp(const string& aFirstInstName)
	{
	// The "extra cpp" field is written to the top of the cpp file.
	CdlTkUtil::CReplaceSet cppSet;
	cppSet.Add("$INTERFACE_NS", iInterface.NamespaceName());
	cppSet.Add("$FIRSTINSTANCENAME", aFirstInstName);
	cppSet.Add("$THISINSTANCEBASEOFFSET", CdlTkUtil::IntToHexString(iBaseOffset)); // base offset can be longer than 16 bits
	iInstance->SetExtraCpp(CdlTkUtil::MultiReplace(cppSet, KScalableExtraCpp));

	// add headers & fwd declarations
	string init = CdlTkUtil::MultiReplace(cppSet, KScalableInitialCpp);

	// add decode functions
	string functions;
	for (CImplFuncs::iterator pFunc = iFuncs.begin(); pFunc != iFuncs.end(); ++pFunc)
		{
		CdlTkUtil::AppendString(functions, (*pFunc)->iDefn);
		CdlTkUtil::AppendString(functions, "\n");
		}
	init = CdlTkUtil::Replace("$FUNCTIONS", functions, init);
	CCdlTkImplementation& first = **(iInstance->Impl().begin());
	first.SetDefinition(init + first.Definition());

	// add end of data table
	CCdlTkImplementation& last = **(iInstance->Impl().end() - 1);
	last.SetDefinition(last.Definition() + "\n};");
	}

void CMLCompDataInstOpt::OutputStats()
	{
	int optimizedBytes(0);
	int unoptimizedBytes(0);
	for (CMLCompDataInstOptImpls::iterator pImpl = iImpls.begin(); pImpl != iImpls.end(); ++pImpl)
		{
		int size = (*pImpl)->iBytes.size();
		if ((*pImpl)->iIsRedirectedToExactCopy)
			optimizedBytes += size;
		else
			unoptimizedBytes += size;
		}
	for (CMLCompDataInstOptImpls::iterator pTableImpl = iTableImpls.begin(); pTableImpl != iTableImpls.end(); ++pTableImpl)
		{
		int size = (*pTableImpl)->iBytes.size();
		if ((*pTableImpl)->iIsRedirectedToExactCopy)
			optimizedBytes += size;
		else
			unoptimizedBytes += size;
		}

	float compress = (100.0 * (float)unoptimizedBytes) / ((float)optimizedBytes + (float)unoptimizedBytes);
	int compressInt = (int)(compress + 0.5);
	cout << "instance " << iName << " compressed to " << compressInt << "% of total (" << unoptimizedBytes<< " / " << optimizedBytes + unoptimizedBytes << ")" << endl;
	}

void CMLCompDataInstOpt::AddTableToInstance(TMLCompDataTable& aTable, TMLCompDataTable::TMLCompDataSubTable& aSub, int /*aTableNum*/)
	{
	string tableName = MLCompDataToCdl::SubTableApiName(aSub);
	string tableParamLimitsName = tableName + KFuncParamLimitsSuffix;

	if (HasApi(tableName))
		{
		CCdlTkImplementation& impl = FindImp(tableName);
		CMLCompDataSubTableInstOptImpl* newImpl = new CMLCompDataSubTableInstOptImpl(&aTable, &aSub, &impl);
		iTableImpls.push_back(newImpl);

		SetDummyTableData(aTable, aSub, *newImpl);
		SetSubTableFunc(*newImpl);

		AddTableLimitsImpl(tableName + KFuncLimitsSuffix, aSub);
		if (HasApi(tableParamLimitsName))
			{
			AddTableParamLimitsImpl(tableParamLimitsName, newImpl->iByteCodeIndex, SCompDataImplFunc::ETableParamLimits, aSub.iNeedsOption);
			}
		AddTableImpl(tableName, aTable, aSub);
		}
	}

void CMLCompDataInstOpt::UpdateTableInstance(TMLCompDataTable& aTable, TMLCompDataTable::TMLCompDataSubTable& aSub, CMLCompDataInstOptImpl* aImpl)
	{
	string tableName = MLCompDataToCdl::SubTableApiName(aSub);
	UpdateTableData(aTable, aSub, *aImpl);

	// if we can find the new table data in the aggregated data, point to that instead
	int foundIndex = iInstances.FindSimilarBytes(aImpl, iBaseOffset);
	if(foundIndex >= 0)
		{
		if(foundIndex != aImpl->iByteCodeIndex)
			{
			aImpl->iByteCodeIndex = foundIndex;
			aImpl->iIsRedirectedToExactCopy = true;
			}
		else
			{
			cerr << "Error: found same table already in bytestream: " << aImpl->iName << endl;
			}
		}

	// now we've generated the bytecode, it will be added to the bytestream
	iInstances.AddImpl(aImpl);
	aImpl->iImpl->SetDefinition(DefinitionString(aImpl->iByteCodeIndex, tableName));

	string tableParamLimitsName = tableName + KFuncParamLimitsSuffix;
	if (HasApi(tableParamLimitsName))
		{
		UpdateTableParamLimitsImpl(tableParamLimitsName, aImpl->iByteCodeIndex);
		}
	}

void CMLCompDataInstOpt::AddTableParamLimitsImpl(const string& aApiName, int aByteCodeIndex, SCompDataImplFunc::TFuncType aType, bool aNeedsOptions)
	{
	CCdlTkImplementation& impl = FindImp(aApiName);

	// look up the api to see whether we need a variety params
	CCdlTkFunctionApi& api = const_cast<CCdlTkFunctionApi&>(impl.Api().AsFunc());
	CCdlTkApiParams& params = api.Params();
	ValidateRequiredParams(
		aApiName, 
		params, 
		iAllParams || aNeedsOptions, 
		false, 
		false);

	SCompDataImplFunc& func = AddImplFunc(aType, params);

	impl.SetDefinition(DefinitionString(aByteCodeIndex, aApiName));
	impl.SetPointerReference(func.iPtrRef);
	}

void CMLCompDataInstOpt::UpdateTableParamLimitsImpl(const string& aApiName, int aByteCodeIndex)
	{
	CCdlTkImplementation& paramLimitsImpl = FindImp(aApiName);
	paramLimitsImpl.SetDefinition(DefinitionString(aByteCodeIndex, aApiName));
	}

void CMLCompDataInstOpt::AddTableLimitsImpl(const string& aApiName, TMLCompDataTable::TMLCompDataSubTable& aSubTable)
	{
	CCdlTkImplementation& impl = FindImp(aApiName);
	AddTableLimitsImplDefn(aSubTable, impl);
	impl.SetPointerReference(AddImplFunc(SCompDataImplFunc::ETableLimits).iPtrRef);
	}

void CMLCompDataInstOpt::AddTableLimitsImplDefn(TMLCompDataTable::TMLCompDataSubTable& aSubTable, CCdlTkImplementation& aImpl)
	{
	// code up table limits as a pair of byte values, the first byte is the first table
	// index, the second is the last table index.
	int first = (*aSubTable.begin()) & 0xff;
	int last = (*aSubTable.rbegin()) & 0xff;

	// however, we want these APIs to be accessed zero based, so have to calculate the offset.
	int offset = last - first;

    int v = offset;
	if ( v > KAddressableBytecodedData )
	    {
	    std::cerr << "*** ERROR: Value " << v << "(unknown/3) of out range" << std::endl;
	    }
	    
	aImpl.SetDefinition(CdlTkUtil::ShortToHexString(offset) + KComma);
	}

void CMLCompDataInstOpt::AddTableImpl(const string& aApiName, TMLCompDataTable& aTable, TMLCompDataTable::TMLCompDataSubTable& aSub)
	{
	CCdlTkImplementation& impl = FindImp(aApiName);

	int nParams;
	CountApiParams(impl, nParams);
	nParams--;	// don't count the aLineIndex param
	SCompDataImplFunc::TFuncType type = SCompDataImplFunc::EWindowTable;
	TMLCompDataLine::TComponentType subTableType = aTable[aSub[0]]->iType;
//	switch(subTableType)
//		{
//		case TMLCompDataLine::ETextComponent:
//			{
//			type = SCompDataImplFunc::ETextTable;
//			break;
//			}
//		}
	if ( TMLCompDataLine::ETextComponent == subTableType )
		{
		type = SCompDataImplFunc::ETextTable;
		}
	CCdlTkFunctionApi& api = const_cast<CCdlTkFunctionApi&>(impl.Api().AsFunc());
	CCdlTkApiParams& params = api.Params();
	ValidateRequiredParams(
		aApiName, 
		params, 
		iAllParams || aSub.iNeedsOption, 
		iAllParams || aSub.iNeedsCol, 
		iAllParams || aSub.iNeedsRow);
	AddImplFunc(type, params);
	}

void CMLCompDataInstOpt::AddParamLimits(TMLCompDataLine& aLine, bool aNeedsOptions)
	{
	string apiName = MLCompDataToCdl::LineApiName(aLine) ;
	string paramLimitsApiName = apiName + KFuncParamLimitsSuffix;
	if (!HasApi(paramLimitsApiName))
		throw NotFoundErr(paramLimitsApiName + " in interface " + iInterface.FileName());

	CCdlTkImplementation& paramLimitsImpl = FindImp(paramLimitsApiName);

	// look up the api to see whether we need a variety params
	CCdlTkFunctionApi& api = const_cast<CCdlTkFunctionApi&>(paramLimitsImpl.Api().AsFunc());
	CCdlTkApiParams& params = api.Params();
	ValidateRequiredParams(
		paramLimitsApiName, 
		params, 
		iAllParams || aNeedsOptions, 
		false, 
		false);

	SCompDataImplFunc& func = AddImplFunc(SCompDataImplFunc::ELineParamLimits, params);
	paramLimitsImpl.SetPointerReference(func.iPtrRef);

	UpdateParamLimits(apiName);
	}

void CMLCompDataInstOpt::UpdateParamLimits(const string& apiName)
	{
	string paramLimitsApiName = apiName + KFuncParamLimitsSuffix;
	if (HasApi(paramLimitsApiName))
		{
		CCdlTkImplementation& paramLimitsImpl = FindImp(paramLimitsApiName);
		CMLCompDataInstOptImpl* actualOptImpl = FindSimilarImpl(iImpls, FindImp(apiName));
		
		paramLimitsImpl.SetDefinition(DefinitionString(actualOptImpl->iByteCodeIndex, paramLimitsApiName));
		}
	}

void CMLCompDataInstOpt::SetDummyTableData(TMLCompDataTable& aTable, TMLCompDataTable::TMLCompDataSubTable& aSubTable, CMLCompDataInstOptImpl& aImpl)
	{
	aImpl.iBytes.clear();
	aImpl.iComment = string("for table: ");
	aImpl.iName = MLCompDataToCdl::SubTableApiName(aSubTable);
	EncodeValue(aImpl.iBytes, aSubTable.size());
	for(TMLCompDataTable::TMLCompDataSubTable::iterator pLineId = aSubTable.begin(); pLineId != aSubTable.end(); ++pLineId)
		{
		aImpl.iBytes.push_back(0);
		aImpl.iBytes.push_back(0);
		}
	}

void CMLCompDataInstOpt::UpdateTableData(TMLCompDataTable& aTable, TMLCompDataTable::TMLCompDataSubTable& aSubTable, CMLCompDataInstOptImpl& aImpl)
	{
	aImpl.iBytes.clear();
	EncodeValue(aImpl.iBytes, aSubTable.size());
	for(TMLCompDataTable::TMLCompDataSubTable::iterator pLineId = aSubTable.begin(); pLineId != aSubTable.end(); ++pLineId)
		{
		// figure out the offset to the next line of the table
		TMLCompDataLine& line = *(aTable[*pLineId]);
		string lineName = MLCompDataToCdl::LineApiName(line) ;
		CMLCompDataInstOptImpl* lineImpl = FindSimilarImpl(iImpls, FindImp(lineName));

		// we must store the adjusted index, ie relative to the base offset, in the definition, as that will be used 
		// in the main output, so check that the offset is in range, which it should be if we've updated the lines
		// correctly
		if(!CheckByteCodeIndexInRange(lineImpl->iByteCodeIndex))
			throw GeneralErr(aImpl.iName + " in interface " + iInterface.FileName());
		int adjustedIndex = lineImpl->iByteCodeIndex - iBaseOffset;

		// make an assumption that data fits into 16 bits, and don't encode the lookups
		// that way, when decoding, we can jump forward by 2 bytes * index without decoding
		// all the values (since they're not encoded, we know they're all the same size)
		aImpl.iBytes.push_back((adjustedIndex & 0xff00) >> 8);
		aImpl.iBytes.push_back(adjustedIndex);
		}
	}

struct SIdToInt
	{
	int iInt;
	char* iStr;
	};

#include <avkon.hrh>
extern SIdToInt gIdToIntTable[];
extern const int gIdToIntTableCount;
extern void TranslateValue(string& aValue);

void CMLCompDataInstOpt::EncodeValue(vector<char>& aBytes, string aValue)
	{
	string::size_type pos = 0;
	TranslateValue(aValue);
	if (aValue == "")
		{
		aBytes.push_back(KByteEmpty);
		}
	else 
		{
		pos = aValue.find_first_of(KParentRelativeMarker);
		if(pos != string::npos)
			{
			if (pos != 0)
				throw CdlTkAssert(string("arithmetic parser not good enough : ") + aValue);
			int val = CdlTkUtil::ParseInt(aValue.substr(1));
			EncodeParentRelativeValue(aBytes, val);
			}
		else
			{
			int val = CdlTkUtil::ParseInt(aValue);
			EncodeValue(aBytes, val);
			}
		}
	}

void CMLCompDataInstOpt::EncodeValue(vector<char>& aBytes, int aValue)
	{
	if (0 <= aValue && aValue <= KMaxSingleByteValue)
		{
		aBytes.push_back(aValue);
		}
	else if (aValue > KMaxSingleByteValue && aValue <= KMaxDoubleByteValue)
		{
		aBytes.push_back(KByteWord);
		aBytes.push_back((aValue & 0xff00) >> 8);
		aBytes.push_back(aValue);
		}
	else
		{
		aBytes.push_back(KByteLong);
		aBytes.push_back((aValue & 0xff000000) >> 24);
		aBytes.push_back((aValue & 0x00ff0000) >> 16);
		aBytes.push_back((aValue & 0x0000ff00) >> 8);
		aBytes.push_back(aValue);
		}
	}

void CMLCompDataInstOpt::EncodeParentRelativeValue(vector<char>& aBytes, int aValue)
	{
	if (KMinSingleByteParentRelativeValue <= aValue && aValue <= KMaxSingleByteParentRelativeValue)
		{
		aBytes.push_back(KByteP1);
		aBytes.push_back(aValue);
		}
	else
		{
		aBytes.push_back(KByteP2);
		aBytes.push_back((aValue & 0xff00) >> 8);
		aBytes.push_back(aValue);
		}
	}

void CMLCompDataInstOpt::MirrorParamName(string& aParamName)
	{
	if (aParamName == KParamNameL)
		aParamName = KParamNameR;
	else if (aParamName == KParamNameR)
		aParamName = KParamNameL;
	}

//
// MLCompDataCdlInstanceOpt
//

MLCompDataCdlInstanceOpt::InstStruct::InstStruct(string aInstName, TMLCompData* aInst, TMLAttributes* aAttribs) 
	: 
	iInstName(aInstName), 
	iInst(aInst),
	iAttribs(aAttribs)
	{
	}

MLCompDataCdlInstanceOpt::InstList::~InstList() 
	{
	for(InstList::iterator pNext = begin(); pNext != end(); ++pNext)
		{
		delete pNext->iInst;
		}
	}

void MLCompDataCdlInstanceOpt::ProcessSeparators(vector<string>& args, vector<int>& aSeparators)
	{
    for(unsigned int arg = 3; arg < args.size(); arg++)
        {
        if(args[arg] == "-a")
            aSeparators.push_back(arg);
        }

    aSeparators.push_back(args.size()); // add an implicit last separator
    if(aSeparators.size() < 2)
        throw MLCompDataCdlInstanceOptArgsErr();

    // check that the distance between each separator is not a multiple of 2 
    // i.e. counting the steps between aSeparators ( sep -> xml -> inst -> sep) is 3 steps
    // i.e. counting the steps between aSeparators ( sep -> xml -> inst -> xml -> inst -> sep) is 5 steps
    for(unsigned int sep = 0; sep < aSeparators.size() - 1; sep++)
        {
        int delta = aSeparators[sep+1] - aSeparators[sep]; 
	    if (delta%2 == 0)
		    throw MLCompDataCdlInstanceOptArgsErr();
        }
	}

bool MLCompDataCdlInstanceOpt::CheckForUsedInstances(
	const CInstanceList& aUsedList, 
	const CZoomLevelNames& aZoomLevelNames,
	const vector<string>& aArgs, 
	const vector<int>& aSeparators, 
	int aSepIndex)
	{
	bool ok = false;
	for(CZoomLevelNames::const_iterator pZoomLevel = aZoomLevelNames.begin(); pZoomLevel != aZoomLevelNames.end(); ++pZoomLevel)
		{
		// check if any of these instances are used - skip this if none are used
		for (int arg = aSeparators[aSepIndex] + 1; arg < aSeparators[aSepIndex+1]; arg += 2)
			{
			string instName = aArgs[arg+1] + "_" + pZoomLevel->second;
			if (aUsedList.IsInstanceOk(instName))
				ok = true;
			}
		}
	return ok;
	}

void MLCompDataCdlInstanceOpt::ParseInstances(const vector<string>& aArgs, const vector<int>& aSeparators, int aSepIndex, MLCompDataCdlInstanceOpt::InstList& aInstList)
	{
	for (int arg = aSeparators[aSepIndex] + 1; arg < aSeparators[aSepIndex+1]; arg += 2)
		{
		string layoutName = aArgs[arg];
		string instName = aArgs[arg+1];
		string attribsName = CdlTkUtil::Replace(KCompDataFileNameSuffix, KAttributesFileNameSuffix, layoutName);
		auto_ptr<TMLCompDataParseLayout> layoutParse = TMLCompDataParseLayout::Parse(layoutName);
		auto_ptr<TMLCompData> layout(layoutParse.get());
		layoutParse.release();
		auto_ptr<TMLAttributesParse> attribsParse = TMLAttributesParse::Parse(attribsName);
		auto_ptr<TMLAttributes> attribs(attribsParse.get());
		attribsParse.release();

		InstStruct instStruct(instName, layout.get(), attribs.get());
		aInstList.push_back(instStruct);
		layout.release();
		attribs.release();
		}
	}

void MLCompDataCdlInstanceOpt::MergeLayouts(CInstanceList& aInstUsedList, CZoomLevelNames& aZoomLevelNames, const InstList& aInstances, InstList& aMergedLayouts)
	{
	// start with the non-mirrored instances
	for(int count = 0; count < 2; count++)
		{
		bool isMirrored = (count != 0);
		// first iterate through the layouts, we will generate one instance per layout
		for(unsigned int instIndex = 0; instIndex < aInstances.size(); instIndex++)
			{
			const InstStruct& instStruct = aInstances[instIndex];
			string targetInstName = CdlTkUtil::Replace("\r","",instStruct.iInstName);
			targetInstName = CdlTkUtil::Replace("\n","",targetInstName);
			TMLCompData& targetLayout = *(instStruct.iInst);
			TMLAttributes& targetAttribs = *(instStruct.iAttribs);

			bool required = false;
			for(CZoomLevelNames::const_iterator pZoomLevel = aZoomLevelNames.begin(); pZoomLevel != aZoomLevelNames.end(); ++pZoomLevel)
				{
				string zoomInstName = targetInstName + "_" + pZoomLevel->second;
				 if(aInstUsedList.IsInstanceOk(zoomInstName))
					required = true;
				}

			// if this target is not required, or if it's the wrong sort of mirrored
			// for this iteration, skip it.
			if(targetLayout.iCanBeMirror != isMirrored || !required)
				continue;

			// for each instance, we must merge all the other layouts
			auto_ptr<TMLCompData> mergedLayout(new TMLCompData());
			auto_ptr<TMLAttributes> mergedAttribs(new TMLAttributes());
			for (InstList::const_iterator pInst2 = aInstances.begin(); pInst2 != aInstances.end(); ++pInst2)
				{
				// merge in all the others
				const InstStruct& instStruct2 = *pInst2;
				if(instStruct2.iInstName != targetInstName)
					{
					TMLCompData& nextLayout = *(instStruct2.iInst);
					TMLAttributes& nextAttribs = *(instStruct2.iAttribs);
					// but only if it's the right kind of mirrored
					if(nextLayout.iCanBeMirror == isMirrored)
						{
						mergedLayout->Merge(nextLayout);
						mergedAttribs->Merge(nextAttribs);
						}
					}
				}
			// then end up merging in the one we want
			mergedLayout->Merge(targetLayout);
			mergedAttribs->Merge(targetAttribs);
			if(isMirrored)
				{
				// If we have just processed a mirrored layout, 
				// we need to do a mirror merge with the corresponding one.
				// The instances are ordered as on the command line, 
				// but the merged layouts are grouped toghether, unmirrored first.
				// So to convert between the two indexes: 1 -> 0, and 3 -> 1
				int unMirroredMergedLayoutIndex = (instIndex / 2);
				InstStruct& unMirroredInst = aMergedLayouts[unMirroredMergedLayoutIndex]; // this works as we have already added the unmirrored instance to the vector
				TMLCompData& unMirroredLayout = *(unMirroredInst.iInst);
				TMLAttributes& unMirroredAttribs = *(unMirroredInst.iAttribs);
				mergedLayout->Merge(unMirroredLayout);
				mergedAttribs->Merge(unMirroredAttribs);
				}

			InstStruct mergedInstStruct(targetInstName, mergedLayout.get(), mergedAttribs.get());
			aMergedLayouts.push_back(mergedInstStruct);
			mergedLayout.release();
			mergedAttribs.release();
			}
		}
	}

int MLCompDataCdlInstanceOpt::Process(vector<string>& args)
	{
	// parse the file containing the used instances list. only instances in this list 
	// will be generated, even if also mentioned on the command line
	CInstanceList instUsedList;
	instUsedList.ProcessOptions(args);

	// extract the zoom level names, note that this consumes the argument if present
	CZoomLevelNames zoomLevelNames;
	zoomLevelNames.ProcessOptions(args);

	// check that we have an acceptable number of arguments
	int extraArgs = args.size() - 3;
	if (extraArgs < 0)
		throw MLCompDataCdlInstanceOptArgsErr();

	// check for optional flags
    	int arg = 2;
	bool allParams = false;
	if (args[arg] == "-allparams")
		{
		allParams = true;
		arg++;
		}

	bool nonCompleteInstance = false;
	if(args[arg].substr(0,2) == "-d")
        {
		arg++;
        if (args[arg].size() >= 2)
		    {
		    nonCompleteInstance = true;
		    }
        }

	// parse the CDL interface
	string cdlName = args[arg];
	CCdlTkCdlFileParser parser(cdlName);
	auto_ptr<CCdlTkInterface> iface(parser.LoadAndParse(true));
	MLCompDataCdlInstanceOpt process(*iface);

    // the separators divide the layout instances that are aggregated together
    vector<int> separators;
	ProcessSeparators(args, separators);
    for(unsigned int sep = 0; sep < separators.size() - 1; sep++)
        {
		if(!CheckForUsedInstances(instUsedList, zoomLevelNames, args, separators, sep))
			continue;
        // start from after the next separator, and continue until before the next one
	    InstList instances;
		ParseInstances(args, separators, sep, instances);

		InstList mergedLayouts;
		MergeLayouts(instUsedList, zoomLevelNames, instances, mergedLayouts);
		for(InstList::iterator pMergedLayout = mergedLayouts.begin(); pMergedLayout != mergedLayouts.end(); )
			{
			process.AddInst(*pMergedLayout, zoomLevelNames, allParams, nonCompleteInstance); // pass ownership of mergedLayout
			pMergedLayout = mergedLayouts.erase(pMergedLayout);
			}
        }
	process.Process();
	process.WriteInstances();
	return 0;
	}

void MLCompDataCdlInstanceOpt::ShowHelp(ostream& stream)
	{
	stream << "MLCompCdl2InstO [-i<instanceList>] [-z<zoomList>] [-allparams] [-d<deliveryType>] <interface.cdl> (-a ([-m]<layout.xml> <instanceName>)+ )+" << endl;
	stream << "  Creates optimised CDL instances containing the layout data." << endl;
	stream << "  Each -a flag is followed by a collection of xml and instance name pairs." << endl;
	stream << "  Each collection is aggregated separately." << endl;
	stream << "  The aggregation of the layout instances must conform to the CDL interface, " << endl;
	stream << "    but note any missing data in an instance may result in data being returned " << endl;
	stream << "    from a different instance, although duplicate implementations will be reused " << endl;
	stream << "    to reduce ROM usage." << endl;
	stream << "  A -m flag must precede an xml file that contains mirrored layout data." << endl;
	stream << "  An aggregated collection of layouts must contain interleaved" << endl;
	stream << "    elaf and the corresponding abrw instances." << endl;
	stream << "  If -i<instanceList> is specified, then only instances whose name" << endl;
	stream << "    appears in the file <instanceList> will be processed." << endl;
	stream << "  If -z<zoomList> is specified, then only instances whose zoom factor" << endl;
	stream << "   (in the form \"n,string\") appears in the file <zoomList> will be generated." << endl;
	stream << "  If -allparams is used, all processed APIs will have all available params added, " << endl;
	stream << "    otherwise only needed params are added." << endl;
	stream << "  If -d<deliveryType> is supplied with any value, any API that has a missing" << endl;
	stream << "    implementation will be filled in with NULL, allowing fall-through to an instance" << endl;
	stream << "    from a lower priority pack." << endl;
	}

MLCompDataCdlInstanceOpt::MLCompDataCdlInstanceOpt(CCdlTkInterface& aInterface)
	: 
	iInterface(aInterface)
	{
	}

MLCompDataCdlInstanceOpt::~MLCompDataCdlInstanceOpt()
	{
	for(CCompDataZoomLevelDatas::iterator pZoomLevel = iZoomLevelDatas.begin(); pZoomLevel != iZoomLevelDatas.end(); ++pZoomLevel)
		{
		CCompDatas& compDatas = pZoomLevel->second;
		for (CCompDatas::iterator pInstOpt = compDatas.begin(); pInstOpt != compDatas.end(); ++pInstOpt)
			delete *pInstOpt;
		}
	for(CCompDataLayouts::iterator pLayout = iLayouts.begin(); pLayout != iLayouts.end(); ++pLayout)
		 delete *pLayout;
	}

void MLCompDataCdlInstanceOpt::AddInst(const InstStruct& aInstStruct, CZoomLevelNames& aZoomLevelNames, bool aAllParams, bool aNonCompleteInstance)
	{
	TMLCompData* layout = aInstStruct.iInst;
	layout->iAttributes = aInstStruct.iAttribs; // transfer ownership
	iLayouts.push_back(aInstStruct.iInst);
	for(CZoomLevelNames::iterator pZoomLevel = aZoomLevelNames.begin(); pZoomLevel != aZoomLevelNames.end(); ++pZoomLevel)
		{
		auto_ptr<CMLCompDataInstOpt> p(new CMLCompDataInstOpt(*this, layout, aInstStruct.iInstName, pZoomLevel->second, pZoomLevel->first, aAllParams, aNonCompleteInstance));
		CCompDatas& compDatas = iZoomLevelDatas[pZoomLevel->first];
		compDatas.push_back(p.get());
		p.release();
		}
	}

void MLCompDataCdlInstanceOpt::Process()
	{
	for(CCompDataZoomLevelDatas::iterator pZoomLevel = iZoomLevelDatas.begin(); pZoomLevel != iZoomLevelDatas.end(); ++pZoomLevel)
		{
		CCompDatas& compDatas = pZoomLevel->second;
		if(compDatas.size() == 0)
			continue;
		CMLCompDataInstOpt* firstInstOpt = compDatas[0];
		cout << "processing instances for zoom level: " << firstInstOpt->ZoomName() << endl;
		string firstInstName = firstInstOpt->Name();
		for (CCompDatas::iterator pLayout = compDatas.begin(); pLayout != compDatas.end(); ++pLayout)
			(*pLayout)->Process(firstInstName);
		}
	ProcessCommonImpl();
	}

void MLCompDataCdlInstanceOpt::WriteInstances()
	{	bool found = false;
	for(CCompDataZoomLevelDatas::iterator pZoomLevel = iZoomLevelDatas.begin(); pZoomLevel != iZoomLevelDatas.end(); ++pZoomLevel)
		{
		CCompDatas& compDatas = pZoomLevel->second;
		if(!found && compDatas.size() > 0)
			{
			found = true;
			cout << "writing instances ... " << endl;
			}
		for (CCompDatas::iterator pLayout = compDatas.begin(); pLayout != compDatas.end(); ++pLayout)
			(*pLayout)->WriteInstance();
		}
	}

CCdlTkInterface& MLCompDataCdlInstanceOpt::Interface()
	{
	return iInterface;
	}

const string KCommonImplStart = "\
#include \"aknlayout2scalabledecode.h\"\n\
namespace $NAMESPACENAME { extern TUint8 const KByteCodedData_$FIRSTINSTANCENAME[] = {\n";

const string KCommonImplImpl = "\
// $INDEX $COMMENT\n\
$BYTES\n";

void MLCompDataCdlInstanceOpt::ProcessCommonImpl()
	{
	// use the first instance name of the first zoom level
	// it's not entirely accurate but it only needs to disambiguate
	CCompDatas& compDatas = iZoomLevelDatas[EAknUiZoomNormal];
	if(compDatas.size() == 0)
		return;
	CMLCompDataInstOpt* firstInstOpt = compDatas[0];

	CdlTkUtil::CReplaceSet startImplSet;
	startImplSet.Add("$NAMESPACENAME", iInterface.NamespaceName());
	startImplSet.Add("$FIRSTINSTANCENAME", firstInstOpt->Name());
	string bytecode = CdlTkUtil::MultiReplace(startImplSet, KCommonImplStart);
	int byteCounter(0);

	for (CMLCompDataInstOptImpls::iterator pImpl = iImpls.begin(); pImpl != iImpls.end(); ++pImpl)
		{
		vector<char>& bytes = (*pImpl)->iBytes;
		if (!(*pImpl)->iIsRedirectedToExactCopy && bytes.size())
			{
			string byteString;
			for (vector<char>::iterator pChar = bytes.begin(); pChar != bytes.end(); ++pChar)
				{
				CdlTkUtil::AppendString(byteString, CdlTkUtil::CharToHexString(*pChar));
				CdlTkUtil::AppendString(byteString, KComma);
				}

			// in this case, we want to know the absolute index to help with debugging
			int index = (*pImpl)->iByteCodeIndex;
			if(byteCounter != index)
				{
				cerr << "Error: Data table mismatch for " << (*pImpl)->iName;
				cerr << ": Bytecode index = " << CdlTkUtil::ShortToHexString(index);
				cerr << "; byte counter = " << CdlTkUtil::ShortToHexString(byteCounter) << endl;
				throw GeneralErr((*pImpl)->iName + " in interface " + iInterface.FileName());
				}
			byteCounter += bytes.size();

			CdlTkUtil::CReplaceSet implSet;
			implSet.Add("$INDEX", CdlTkUtil::IntToHexString(index));
			implSet.Add("$COMMENT", (*pImpl)->iComment + (*pImpl)->iName + " (" + CdlTkUtil::ShortToHexString(bytes.size()) + " bytes)");
			implSet.Add("$BYTES", byteString);
			CdlTkUtil::AppendString(bytecode, CdlTkUtil::MultiReplace(implSet, KCommonImplImpl));
			}
		}
	CdlTkUtil::AppendString(bytecode, "};\n}");
	CCdlTkInstance& firstInst = firstInstOpt->Inst();
	firstInst.SetExtraCpp(bytecode);
	}

int MLCompDataCdlInstanceOpt::FindSimilarBytes(CMLCompDataInstOptImpl* aImpl, int aBaseOffset)
	{
	int index = -1;
	vector<char>::iterator startOfAddressableBlock = iBytesAggregated.begin() + aBaseOffset;
	vector<char>::iterator found = std::search(
		startOfAddressableBlock, 
		iBytesAggregated.end(), 
		aImpl->iBytes.begin(), 
		aImpl->iBytes.end());
	if(found != iBytesAggregated.end())
		{
		index = std::distance(iBytesAggregated.begin(), found); // we return the absolute position
		}
	return index;
	}

void MLCompDataCdlInstanceOpt::AddImpl(CMLCompDataInstOptImpl* aImpl)
	{
	if(!aImpl->iIsRedirectedToExactCopy)
		{
		CMLCompDataInstOptImpls::iterator found = std::find(iImpls.begin(), iImpls.end(), aImpl);
		if(found == iImpls.end())
			{
			iImpls.push_back(aImpl);
			}
		else
			{
			cerr << "Error: " << aImpl->iName << " already added to byte stream" << endl;
			throw GeneralErr(aImpl->iName + " in interface " + iInterface.FileName());
			}
		if (aImpl->iBytes.size())
			{
			aImpl->iByteCodeIndex = iBytesAggregated.size();
			iBytesAggregated.insert(
				iBytesAggregated.end(), 
				aImpl->iBytes.begin(), 
				aImpl->iBytes.end());
			}
		}
	}

int MLCompDataCdlInstanceOpt::ByteStreamSize() const
	{
	return iBytesAggregated.size();
	}

// end of file


