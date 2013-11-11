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



// disable "identifier was truncated to '255' characters in the browser information" warning
#pragma warning (disable:4786)

#include "LayCdl2InstO.h"
#include "LayoutCompilerErr.h"
#include "LayoutParse.h"
#include "CppWriter.h"
#include "Lay2Cdl.h"
#include "CodeGenConsts.h"
#include <fstream>
#include <algorithm>
#include <iostream>
using namespace std;
using namespace CdlCompilerToolkit;

#define AKNLAYOUT_DEFINE_BYTECODE(name,byte) const char name = char(byte);
#include "AknLayoutByteCodes.h"

extern string KMultiLine;
string KMultiLine("Multiline_");
typedef LayoutProcessArgsErr<LayoutCdlInstanceOpt> LayoutCdlInstanceOptArgsErr;


/**
*  gTheFuncs
*  This is a collection of all SImplFunc objects that a layout instance needs, initialised
*  so that there are up to four integer parameters per API type.
*/
CAllFuncs gTheFuncs(4);

const string KDefinitionNotSet("Layout definition not found");


CAllFuncs::CAllFuncs(int aMaxParams)
	{
	for (int ii=0; ii<=aMaxParams; ii++)
		{
		for (int jj=0; jj<2; jj++)
			{
			bool useParent = (jj == 1);
			AddLineFunc(SImplFunc::EWindowLine, ii, useParent, KTypeWindowLineLayout, "WindowLine");
			AddLineFunc(SImplFunc::ETextLine, ii, useParent, KTypeTextLineLayout, "TextLine");
			AddLineFunc(SImplFunc::EMultilineTextLine, ii, useParent, KTypeMultiLineTextLayout, "MultiLineTextLine");
			AddTableFunc(SImplFunc::EWindowTable, ii, useParent, KTypeWindowLineLayout, "WindowTable");
			AddTableFunc(SImplFunc::ETextTable, ii, useParent, KTypeTextLineLayout, "TextTable");
			}
		}
	SImplFunc limits(
		SImplFunc::ETableLimits, 0, false,
		KTypeLayoutTableLimits + " Limits() { return AknLayoutDecode::TableLimits(KDataLookup); }",
		"&Limits");
	push_back(limits);
	}

void CAllFuncs::AddLineFunc(SImplFunc::TFuncType aType, int aParams, bool aParent, const string& aReturn, const string& aFuncName)
	{
	// create a function of this form:
	//TAknWindowLineLayout WindowTable$NUM$PARENT($PARAMS_TYPES_AND_NAMES)
	//	{
	//	return AknLayoutDecode::WindowLine$NUM$PARENT(&KImplData, $PARAM_NAMES);
	//  }
	string funcName = aFuncName + CdlTkUtil::IntToString(aParams) + (aParent ? "t" : "f");
	string defn = aReturn + " " + funcName + "(";
	string body = string(") { return AknLayoutDecode::") + funcName + "(&KImplData";

	if (aParent)
		{
		defn += KTypeRect + " " + KParamParentRect;
		body += ", " + KParamParentRect;
		}

	for (int ii=0; ii<aParams; ii++)
		{
		if (aParent || ii>0)
			defn += ",";
		defn += string(KTypeInt + " aParam") + CdlTkUtil::IntToString(ii);
		body += string(", aParam") + CdlTkUtil::IntToString(ii);
		}

	defn += body + "); }";

	string ptrRef = string("&") + funcName;

	SImplFunc func(aType, aParams, aParent, defn, ptrRef);
	push_back(func);
	}

void CAllFuncs::AddTableFunc(SImplFunc::TFuncType aType, int aParams, bool aParent, const string& aReturn, const string& aFuncName)
	{
	//Create a function of this form:
	//TAknWindowLineLayout WindowTable$NUM$PARENT(TInt aLineIndex, $PARAMS_TYPES_AND_NAMES) 
	//	{
	//	return AknLayoutDecode::WindowTable$NUM$PARENT(&KImplData, aLineIndex, $PARAM_NAMES); 
	//  }

	string funcName = aFuncName + CdlTkUtil::IntToString(aParams) + (aParent ? "t" : "f");
	string defn = aReturn + " " + funcName + "(" + KTypeInt + " " + KParamLineIndex;
	string body = string(") { return AknLayoutDecode::") + funcName + "(&KImplData, " + KParamLineIndex;

	if (aParent)
		{
		defn += ", " + KTypeRect + " " + KParamParentRect;
		body += ", " + KParamParentRect;
		}

	for (int ii=0; ii<aParams; ii++)
		{
		defn += string(", " + KTypeInt + " aParam") + CdlTkUtil::IntToString(ii);
		body += string(", aParam") + CdlTkUtil::IntToString(ii);
		}

	defn += body + "); }";

	string ptrRef = string("&") + funcName;

	SImplFunc func(aType, aParams, aParent, defn, ptrRef);
	push_back(func);
	}


class CLayoutInstOptImpl
	{
public:
	CLayoutInstOptImpl(TLayoutLine* aLine, CCdlTkImplementation* aImpl);
	TLayoutLine* iLine;
	CCdlTkImplementation* iImpl;
	int iByteCodeIndex;
	string iComment;
	vector<char> iBytes;
	};

CLayoutInstOptImpl::CLayoutInstOptImpl(TLayoutLine* aLine, CCdlTkImplementation* aImpl)
: iLine(aLine), iImpl(aImpl)
	{
	}


class CLayoutInstOpt
	{
public:
	CLayoutInstOpt(LayoutCdlInstanceOpt& aInstances, auto_ptr<TLayout>& aLayout, const string& aInstName);
	~CLayoutInstOpt();

	void Process();
	void WriteInstance();

	CCdlTkInstance& Inst() { return *iInstance; }

private:
	void ProcessTable(TLayoutTable& aTable);
	void ProcessLine(TLayoutLine& aLine);
	void ProcessLineApi(TLayoutLine& aLine, CCdlTkImplementation& aImpl);
	void ProcessMultiLineApi(TLayoutLine& aLine, CCdlTkImplementation& aImpl);

	void SetSimilarLineData(CLayoutInstOptImpl& aImpl, CLayoutInstOptImpl& aSimilar);
	void SetNewLineData(CLayoutInstOptImpl& aImpl);
	bool HasApi(const string& aName);
	CCdlTkImplementation& FindImp(const string& aName);
	void SetLineFunc(CLayoutInstOptImpl& aImpl);

	void CountApiParams(CCdlTkImplementation& aApi, int& aParams, bool& aParent);
	SImplFunc& AddImplFunc(SImplFunc::TFuncType aType, int iParams, bool aParent);

	void SetExtraCpp();

	void AddTableToInstance(TLayoutTable& aTable, TLayoutTable::TLayoutSubTable& aSub, int aTableNum);
	void AddTableLimitsImpl(const string& aApiName, TLayoutTable::TLayoutSubTable& aSubTable);
	void AddTableImpl(const string& aApiName, TLayoutTable& aTable, TLayoutTable::TLayoutSubTable& aSub);

	void EncodeValue(vector<char>& aBytes, string aValue);

	void MirrorParamName(string& aParamName);

private:
	LayoutCdlInstanceOpt& iInstances;
	TLayout* iLayout;
	string iName;
	CCdlTkInterface& iInterface;
	CCdlTkInstance* iInstance;
	CLayoutInstOptImpls iImpls;
	typedef vector<SImplFunc*> CImplFuncs;
	CImplFuncs iFuncs;
	};

CLayoutInstOpt::CLayoutInstOpt(LayoutCdlInstanceOpt& aInstances, auto_ptr<TLayout>& aLayout, const string& aInstName)
: iInstances(aInstances), iName(aInstName), iInterface(iInstances.Interface())
	{
	iLayout = aLayout.get();
	aLayout.release();

	iInstance = new CCdlTkInstance(iInterface);
	iInstance->SetName(aInstName);

	// Initially set definitions that will not compile in the resulting code.
	// This will alert the programmer to missing layout data.
	CCdlTkImplementations& impl = iInstance->Impl();
	for (CCdlTkImplementations::iterator pImpl = impl.begin(); pImpl != impl.end(); ++pImpl)
		(*pImpl)->SetDefinition(KDefinitionNotSet);
	}

CLayoutInstOpt::~CLayoutInstOpt()
	{
	delete iLayout;
	delete iInstance;
	for (CLayoutInstOptImpls::iterator pImpl = iImpls.begin(); pImpl != iImpls.end(); ++pImpl)
		delete *pImpl;
	}

void CLayoutInstOpt::Process()
	{
	for (TLayout::iterator pTab = iLayout->begin(); pTab != iLayout->end(); ++pTab)
		ProcessTable(**pTab);
	SetExtraCpp();
	}

void CLayoutInstOpt::WriteInstance()
	{
	CCdlTkWriteInstance writer(*iInstance);
	writer.Process();
	}

void CLayoutInstOpt::ProcessTable(TLayoutTable& aTable)
	{
	for (TLayoutTable::iterator pLine = aTable.begin(); pLine != aTable.end(); ++pLine)
		ProcessLine(**pLine);

	int tableNum = 0;
	for (TLayoutTable::TLayoutSubTables::const_iterator pSub = aTable.iSubTables.begin(); pSub != aTable.iSubTables.end(); ++pSub)
		{
		TLayoutTable::TLayoutSubTable& sub = **pSub;
		AddTableToInstance(aTable, sub, tableNum);
		tableNum++;
		}
	}

void CLayoutInstOpt::ProcessLine(TLayoutLine& aLine)
	{
	string apiName = LayoutToCdl::LineApiName(aLine);
	if (!HasApi(apiName))
		return;

	ProcessLineApi(aLine, FindImp(apiName));

	string multilineApiName = KFuncMultiline + apiName;
	if (aLine.iTable->iType == TLayoutTable::ETextTable &&
		aLine["B"].size() > 1 &&
		HasApi(multilineApiName))
		ProcessMultiLineApi(aLine, FindImp(multilineApiName));
	}

void CLayoutInstOpt::ProcessLineApi(TLayoutLine& aLine, CCdlTkImplementation& aImpl)
	{
	CLayoutInstOptImpl* newImpl = new CLayoutInstOptImpl(&aLine, &aImpl);
	iImpls.push_back(newImpl);

	// always set the new line data
	SetNewLineData(*newImpl);

	// if we can find the new line data in the aggregated data, point to that instead
	int foundIndex = iInstances.FindSimilarBytes(newImpl);
	if(foundIndex >= 0)
		{
		newImpl->iByteCodeIndex = foundIndex;
		newImpl->iBytes.clear();
		}

	SetLineFunc(*newImpl);
	iInstances.AddImpl(newImpl);
	newImpl->iImpl->SetDefinition(CdlTkUtil::ShortToHexString(newImpl->iByteCodeIndex) + ",\t// " + LayoutToCdl::LineApiName(aLine));
	}

void CLayoutInstOpt::ProcessMultiLineApi(TLayoutLine& aLine, CCdlTkImplementation& aImpl)
	{
	CLayoutInstOptImpl* newImpl = new CLayoutInstOptImpl(&aLine, &aImpl);
	iImpls.push_back(newImpl);

	CCdlTkImplementation& textImpl = FindImp(LayoutToCdl::LineApiName(aLine));

	// locate the position of the "aIndex_B" paramters in the text API parameter list
	// and the position of the "aNumberOfLinesShown" parameter in the multiline API
	// parmeter list. This is all the info needed to implement multiline APIs in terms
	// of the corresponding text API
	int bPos = 4, lPos = 0;
	const CCdlTkApiParams& mParams = aImpl.Api().AsFunc().Params();
	const CCdlTkApiParams& tParams = textImpl.Api().AsFunc().Params();
	CCdlTkApiParams::const_iterator pMParam = mParams.begin(); 
	CCdlTkApiParams::const_iterator pTParam = tParams.begin();
	int ii=0;
	while (pMParam != mParams.end() || pTParam != tParams.end())
		{
		if (pTParam != tParams.end())
			{
			if (pTParam->Name() == KParamNameB)
				bPos = ii;
			++pTParam;
			}
		if (pMParam != mParams.end())
			{
			if (pMParam->Name() == KParamNameNumberOfLinesShown)
				lPos = ii;
			++pMParam;
			}
		ii++;
		}

	SetLineFunc(*newImpl);
	iInstances.AddImpl(newImpl);
	newImpl->iImpl->SetDefinition(CdlTkUtil::ShortToHexString((bPos<<8)|lPos) + ",\t// " + aImpl.Name());
	}


void CLayoutInstOpt::SetSimilarLineData(CLayoutInstOptImpl& aImpl, CLayoutInstOptImpl& aSimilar)
	{
	aImpl.iByteCodeIndex = aSimilar.iByteCodeIndex;
	}

void CLayoutInstOpt::SetNewLineData(CLayoutInstOptImpl& aImpl)
	{
	TLayoutLine& line = *aImpl.iLine;

	// set comment
	aImpl.iComment = string("for ") + LayoutToCdl::LineApiName(line);

	const string* outputOrder = KWindowOutputOrder;
	int outputSize = KWindowOutputOrderSize;
	if (line.iTable->iType == TLayoutTable::ETextTable)
		{
		outputOrder = KTextOutputOrder;
		outputSize = KTextOutputOrderSize;
		}

	// encode parameters
	const CCdlTkApiParams& params = aImpl.iImpl->Api().AsFunc().Params();
	for (CCdlTkApiParams::const_iterator pParam = params.begin(); pParam != params.end(); ++pParam)
		{
		const CCdlTkApiParam& param = *pParam;
		if (param.Type() == KTypeInt)	// is it a cell index parameter
			{
			string paramName = param.Name();
			if (line.iIsMirroredHorizontally)
				MirrorParamName(paramName);
			char cells = 0;		// bit field for cells that this parameter applies to
			char maxVal = 0;
			char nextCell = 1;	// bit flag for the next cell
			for (int ii=0; ii<outputSize; ii++)
				{
				TValues& values = line[outputOrder[ii]];
				if (values.iNeedsIndex && values.ParamName() == paramName)
					{
					if (maxVal == 0)
						maxVal = values.size();
					else if (static_cast<unsigned int>(maxVal) != values.size())
						throw CdlTkAssert(string("param range mismatch ") + line.Name() + " " + param.Name());
					cells |= nextCell;
					}
				nextCell = nextCell << 1;
				}
			aImpl.iBytes.push_back(cells);
			aImpl.iBytes.push_back(maxVal);
			}
		}

	// encode data
	for (int ii=0; ii<outputSize; ii++)
		{
		TValues& values = line[outputOrder[ii]];
		for (TValues::iterator pVal = values.begin(); pVal != values.end(); ++pVal)
			EncodeValue(aImpl.iBytes, *pVal);
		}
	}

void CLayoutInstOpt::CountApiParams(CCdlTkImplementation& aApi, int& aParams, bool& aParent)
	{
	aParams = 0;
	aParent = false;
	const CCdlTkApiParams& params = aApi.Api().AsFunc().Params();
	for (CCdlTkApiParams::const_iterator pParam = params.begin(); pParam != params.end(); ++pParam)
		{
		if (pParam->Type() == KTypeInt) // is it a cell index parameter
			aParams++;
		else
			aParent = true;
		}
	}

void CLayoutInstOpt::SetLineFunc(CLayoutInstOptImpl& aImpl)
	{
	int nParams;
	bool hasParent;
	CountApiParams(*aImpl.iImpl, nParams, hasParent);

	SImplFunc::TFuncType type = SImplFunc::EWindowLine;
	if (aImpl.iLine->iTable->iType == TLayoutTable::ETextTable)
		{
		type = SImplFunc::ETextLine;
		string name = aImpl.iImpl->Name();
		if (name.size() > KFuncMultiline.size() && name.substr(0,KFuncMultiline.size()) == KFuncMultiline)
			type = SImplFunc::EMultilineTextLine;
		}

	SImplFunc& func = AddImplFunc(type, nParams, hasParent);
	aImpl.iImpl->SetPointerReference(func.iPtrRef);
	}

CCdlTkImplementation& CLayoutInstOpt::FindImp(const string& aName)
	{
	CCdlTkImplementation* impl = iInstance->Impl().Find(aName);
	if (!impl)
		throw NotFoundErr(aName + " in interface " + iInterface.FileName());
	return *impl;
	}

bool CLayoutInstOpt::HasApi(const string& aName)
	{
	return iInterface.ApiList().Find(aName) != 0;
	}

SImplFunc& CLayoutInstOpt::AddImplFunc(SImplFunc::TFuncType aType, int aParams, bool aParent)
	{
	for (CImplFuncs::iterator pFunc = iFuncs.begin(); pFunc != iFuncs.end(); ++pFunc)
		{
		SImplFunc& func = **pFunc;
		if (func.iType == aType && func.iParams == aParams && func.iParent == aParent)
			return func;
		}

	int count = gTheFuncs.size();
	for (int ii=0; ii<count; ii++)
		{
		SImplFunc* func = &gTheFuncs[ii];
		if (func->iType == aType && func->iParams == aParams && func->iParent == aParent)
			{
			iFuncs.push_back(func);
			return *func;
			}
		}

	throw NotFoundErr("implementation function");
	return gTheFuncs[0];
	}

// The following strings and the SetExtraCpp() function build the gross structure of
// the C++ customisation instance.
// So far, the implementations are actually just 16-bit values, typically indexes into
// the data lookup table. These need to be turned into an array by adding declarations
// and brackets to the first and last implementations. Extra support functions are also
// added.
extern string KExtraCpp;
string KExtraCpp = "\
#include \"aknlayout2decode.h\"\n\
namespace $INTERFACE_NS { extern const TUint8 KByteCodedData[]; }\n";
extern string KInitialCpp;
string KInitialCpp ="\
extern const TUint16 KDataLookup[$INTERFACE_NS::E_TApiId_TableSize];\n\
const SImplData KImplData = { KDataLookup, $INTERFACE_NS::KByteCodedData };\n\
\n\
$FUNCTIONS\
\n\
const TUint16 KDataLookup[$INTERFACE_NS::E_TApiId_TableSize] =\n\
\t{\n";

void CLayoutInstOpt::SetExtraCpp()
	{
	// The "extra cpp" field is written to the top of the cpp file.
	iInstance->SetExtraCpp(CdlTkUtil::Replace("$INTERFACE_NS", iInterface.NamespaceName(), KExtraCpp));

	// add headers & fwd declarations
	string init = CdlTkUtil::Replace("$INTERFACE_NS", iInterface.NamespaceName(), KInitialCpp);

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

void CLayoutInstOpt::AddTableToInstance(TLayoutTable& aTable, TLayoutTable::TLayoutSubTable& aSub, int aTableNum)
	{
	string tableName = LayoutToCdl::TableApiName(aTable, aSub, aTableNum);
	if (HasApi(tableName))
		{
		AddTableLimitsImpl(tableName + KFuncLimitsSuffix, aSub);
		AddTableImpl(tableName, aTable, aSub);
		}
	}

void CLayoutInstOpt::AddTableLimitsImpl(const string& aApiName, TLayoutTable::TLayoutSubTable& aSubTable)
	{
	// code up table limits as a pair of byte values, the first byte is the first table
	// index, the second is the last table index.
	CCdlTkImplementation& impl = FindImp(aApiName);
	int first = (*aSubTable.begin()) & 0xff;
	int last = (*aSubTable.rbegin()) & 0xff;
	impl.SetDefinition(CdlTkUtil::ShortToHexString((first<<8)|last) + ",");
	impl.SetPointerReference(AddImplFunc(SImplFunc::ETableLimits, 0, false).iPtrRef);
	}

void CLayoutInstOpt::AddTableImpl(const string& aApiName, TLayoutTable& aTable, TLayoutTable::TLayoutSubTable& aSub)
	{
	CCdlTkImplementation& impl = FindImp(aApiName);

	int nParams;
	bool hasParent;
	CountApiParams(impl, nParams, hasParent);
	nParams--;	// don't count the aLineIndex param
	SImplFunc::TFuncType type = SImplFunc::EWindowTable;
	if (aTable.iType == TLayoutTable::ETextTable)
		type = SImplFunc::ETextTable;

	SImplFunc& func = AddImplFunc(type, nParams, hasParent);

	impl.SetDefinition(string("(TUint16)") + iInterface.NamespaceName() + "::EApiId_" + LayoutToCdl::LineApiName(*aTable[0]) + ",");
	impl.SetPointerReference(func.iPtrRef);
	}

struct SIdToInt
	{
	int iInt;
	char* iStr;
	};

#include <avkon.hrh>
const int KScalableFontIdOffset(0x1000);
extern SIdToInt gIdToIntTable[];
SIdToInt gIdToIntTable[] =
	{
		{ ELayoutAlignLeft, "ELayoutAlignLeft" },
		{ ELayoutAlignRight, "ELayoutAlignRight" },
		{ ELayoutAlignCenter, "ELayoutAlignCenter" },
		{ ELayoutAlignBidi, "ELayoutAlignBidi" },
		{ ELatinBold19, "ELatinBold19" },
		{ ELatinBold17, "ELatinBold17" },
		{ ELatinBold13, "ELatinBold13" },
		{ ELatinBold12, "ELatinBold12" },
		{ ELatinPlain12, "ELatinPlain12" },
		{ ELatinClock14, "ELatinClock14" },
		{ EApacPlain12, "EApacPlain12" },	
		{ EApacPlain16, "EApacPlain16" },
		{ ENumberPlain5, "ENumberPlain5" },
		{ ELatinBold16, "ELatinBold16" },

		{ ELatinBold19+KScalableFontIdOffset, "ELatinBold19_Scaled" },
		{ ELatinBold17+KScalableFontIdOffset, "ELatinBold17_Scaled" },
		{ ELatinBold13+KScalableFontIdOffset, "ELatinBold13_Scaled" },
		{ ELatinBold12+KScalableFontIdOffset, "ELatinBold12_Scaled" },
		{ ELatinPlain12+KScalableFontIdOffset, "ELatinPlain12_Scaled" },
		{ ELatinClock14+KScalableFontIdOffset, "ELatinClock14_Scaled" },
		{ EApacPlain12+KScalableFontIdOffset, "EApacPlain12_Scaled" },	
		{ EApacPlain16+KScalableFontIdOffset, "EApacPlain16_Scaled" },
		{ ENumberPlain5+KScalableFontIdOffset, "ENumberPlain5_Scaled" },
		{ ELatinBold16+KScalableFontIdOffset, "ELatinBold16_Scaled" },

		{ 100, "qfn_latin_plain_17" },		// fonts that appear in app laf
		{ ECalcBold21, "ECalcBold21" },
		{ ECalcOperBold21, "ECalcOperBold21" },
		{ ECalcOperBold13, "ECalcOperBold13" },
		{ 100, "gfn_latin_bold_13" },
		{ 100, "gfn_latin_plain_12" },
		{ 100, "qfn_clock_plain_5" },

		{ 100, "qfn_latin_plain_17_Scaled" },
		{ ECalcBold21+KScalableFontIdOffset, "ECalcBold21_Scaled" },
		{ ECalcOperBold21+KScalableFontIdOffset, "ECalcOperBold21_Scaled" },
		{ ECalcOperBold13+KScalableFontIdOffset, "ECalcOperBold13_Scaled" },
		{ 100, "gfn_latin_bold_13_Scaled" },
		{ 100, "gfn_latin_plain_12_Scaled" },
		{ 100, "qfn_clock_plain_5_Scaled" }

	};
extern const int gIdToIntTableCount = sizeof(gIdToIntTable)/sizeof(SIdToInt);

extern void TranslateValue(string& aValue)
	{
	int count = gIdToIntTableCount;
	for (int ii=0; ii<count; ii++)
		{
		SIdToInt& trans = gIdToIntTable[ii];
		if (aValue == trans.iStr)
			{
			aValue = CdlTkUtil::IntToString(trans.iInt);
			return;
			}
		}
	}

void CLayoutInstOpt::EncodeValue(vector<char>& aBytes, string aValue)
	{
	string::size_type pos;
	TranslateValue(aValue);

	if (aValue == "")
		{
		aBytes.push_back(KByteEmpty);
		}
	else if ((pos = aValue.find('p')) != string::npos)
		{
		if (pos != 0)
			throw CdlTkAssert(string("arithmetic parser not good enough : ") + aValue);
		int val = CdlTkUtil::ParseInt(aValue.substr(1));
		if (-128 <= val && val <= 127)
			{
			aBytes.push_back(KByteP1);
			aBytes.push_back(val);
			}
		else
			{
			aBytes.push_back(KByteP2);
			aBytes.push_back((val & 0xff00) >> 8);
			aBytes.push_back(val);
			}
		}
	else
		{
		int val = CdlTkUtil::ParseInt(aValue);
		if (0 <= val && val <= KMaxSingleByteValue)
			{
			aBytes.push_back(val);
			}
		else
			{
			aBytes.push_back(KByteWord);
			aBytes.push_back((val & 0xff00) >> 8);
			aBytes.push_back(val);
			}
		}
	}

void CLayoutInstOpt::MirrorParamName(string& aParamName)
	{
	if (aParamName == KParamNameL)
		aParamName = KParamNameR;
	else if (aParamName == KParamNameR)
		aParamName = KParamNameL;
	}



//
// CInstanceList
//

void CInstanceList::ProcessOptions(vector<string>& aArgs)
	{
	bool instanceFileOk = false;
	iLoaded = false;
	string instFile;
	for(vector<string>::iterator pArg = aArgs.begin(); pArg != aArgs.end(); ++pArg)
		{
		string& arg = *pArg;
		if (arg.size() >= 2 && arg.substr(0,2) == "-i")
			{
			instFile = arg.substr(2);
			aArgs.erase(pArg);
			instanceFileOk = true;
			break;
			}
		}
	if(!instanceFileOk)
		throw LayoutCdlInstanceOptArgsErr();
	ifstream in;
	CdlTkUtil::OpenInput(in, instFile);
	iLoaded = true;
	string line;
	while (!in.eof())
		{
		getline(in, line);
		iInstances.insert(line);
		}
	in.close();
	}

bool CInstanceList::IsInstanceOk(const string& aInstance) const
	{
	return (!iLoaded || iInstances.find(aInstance) != iInstances.end());
	}


//
// LayoutCdlInstanceOpt
//

int LayoutCdlInstanceOpt::Process(vector<string>& args)
	{
	CInstanceList instList;
	instList.ProcessOptions(args);

	int extraArgs = args.size() - 5;
	if (extraArgs < 0 || extraArgs%2 == 1)
		throw LayoutCdlInstanceOptArgsErr();

	string cdlName = args[2];
	CCdlTkCdlFileParser parser(cdlName);
	auto_ptr<CCdlTkInterface> iface(parser.LoadAndParse(true));

	LayoutCdlInstanceOpt process(*iface);

	TLayout* base = NULL;
	for (unsigned int arg = 3; arg < args.size(); arg += 2)
		{
		string layoutName = args[arg];
		string instName = args[arg+1];
		if (!instList.IsInstanceOk(instName))
			continue;

		auto_ptr<TLayParseLayout> layoutParse = TLayParseLayout::Parse(layoutName);
		auto_ptr<TLayout> layout(layoutParse.get());
		layoutParse.release();
		if (base)
			{
			auto_ptr<TLayout> newLayout(new TLayout(*base));
			newLayout->Merge(TLayout::KMergeModeVariant, *layout);
			layout = newLayout;
			}
		else
			{
			base = layout.get();
			}
		process.AddLayout(layout, instName);
		}

	if (base)
		{
		process.Process();
		process.WriteInstances();
		}

	return 0;
	}

void LayoutCdlInstanceOpt::ShowHelp(ostream& stream)
	{
	stream << "LayCdl2InstO [-i<instanceList>] <cdlName> (<layoutName> <instanceName>)+ " << endl;
	stream << "  Creates optimised CDL instances containing the layout data." << endl;
	stream << "  All layout instances must conform to the CDL interface." << endl;
	stream << "  If more than one layout is supplies, subsequent ones are treated as" << endl;
	stream << "  variants of the first." << endl;
	stream << "  If -i<instanceList> is specified, then only instances whose name" << endl;
	stream << "  appears in the file <instanceList> will be processed" << endl;
	}

LayoutCdlInstanceOpt::LayoutCdlInstanceOpt(CCdlTkInterface& aInterface)
: iInterface(aInterface), iByteCodeIndex(0)
	{
	}

LayoutCdlInstanceOpt::~LayoutCdlInstanceOpt()
	{
	for (CLayouts::iterator pLayout = iLayouts.begin(); pLayout != iLayouts.end(); ++pLayout)
		delete *pLayout;
	}

void LayoutCdlInstanceOpt::AddLayout(auto_ptr<TLayout>& aLayout, const string& aInstName)
	{
	auto_ptr<CLayoutInstOpt> p(new CLayoutInstOpt(*this, aLayout, aInstName));
	iLayouts.push_back(p.get());
	p.release();
	}

void LayoutCdlInstanceOpt::Process()
	{
	for (CLayouts::iterator pLayout = iLayouts.begin(); pLayout != iLayouts.end(); ++pLayout)
		(*pLayout)->Process();
	ProcessCommonImpl();
	}

void LayoutCdlInstanceOpt::WriteInstances()
	{
	for (CLayouts::iterator pLayout = iLayouts.begin(); pLayout != iLayouts.end(); ++pLayout)
		(*pLayout)->WriteInstance();
	}

CCdlTkInterface& LayoutCdlInstanceOpt::Interface()
	{
	return iInterface;
	}

const string KCommonImplStart = "\
#include \"aknlayout2decode.h\"\n\
namespace $NAMESPACENAME { extern TUint8 const KByteCodedData[] = {\n";

const string KCommonImplImpl = "\
// $INDEX $COMMENT\n\
$BYTES\n";

void LayoutCdlInstanceOpt::ProcessCommonImpl()
	{
	string bytecode = CdlTkUtil::Replace("$NAMESPACENAME", iInterface.NamespaceName(), KCommonImplStart);

	for (CLayoutInstOptImpls::iterator pImpl = iImpls.begin(); pImpl != iImpls.end(); ++pImpl)
		{
		vector<char>& bytes = (*pImpl)->iBytes;
		if (bytes.size())
			{
			string byteString;
			for (vector<char>::iterator pChar = bytes.begin(); pChar != bytes.end(); ++pChar)
				{
				CdlTkUtil::AppendString(byteString, CdlTkUtil::CharToHexString(*pChar));
				CdlTkUtil::AppendString(byteString, ",");
				}

			CdlTkUtil::CReplaceSet implSet;
			implSet.Add("$INDEX", CdlTkUtil::ShortToHexString((*pImpl)->iByteCodeIndex));
			implSet.Add("$COMMENT", (*pImpl)->iComment);
			implSet.Add("$BYTES", byteString);
			CdlTkUtil::AppendString(bytecode, CdlTkUtil::MultiReplace(implSet, KCommonImplImpl));
			}
		}
	CdlTkUtil::AppendString(bytecode, "};\n}");

	CCdlTkInstance& firstInst = iLayouts[0]->Inst();
	firstInst.SetExtraCpp(bytecode);
	}

LayoutCdlInstanceOpt::CLayouts& LayoutCdlInstanceOpt::Layouts()
	{
	return iLayouts;
	}

CLayoutInstOptImpl* LayoutCdlInstanceOpt::FindSimilarImpl(TLayoutLine& aLine)
	{
	for (CLayoutInstOptImpls::iterator pImpl = iImpls.begin(); pImpl != iImpls.end(); ++pImpl)
		{
		CLayoutInstOptImpl* impl = *pImpl;
		if (LinesAreEqual(*impl->iLine, aLine))
			{
			return impl;
			}
		}
	return NULL;
	}

int LayoutCdlInstanceOpt::FindSimilarBytes(CLayoutInstOptImpl* aImpl)
	{
	int index = -1;
	vector<char>::iterator found = std::search(
		iBytesAggregated.begin(), 
		iBytesAggregated.end(), 
		aImpl->iBytes.begin(), 
		aImpl->iBytes.end());
	if(found != iBytesAggregated.end())
		{
		index = std::distance(iBytesAggregated.begin(), found);
		}
	return index;
	}

void LayoutCdlInstanceOpt::AddImpl(CLayoutInstOptImpl* aImpl)
	{
	iImpls.push_back(aImpl);
	int bytesAdded = aImpl->iBytes.size();
	if (bytesAdded)
		{
		aImpl->iByteCodeIndex = iByteCodeIndex;
		iByteCodeIndex += bytesAdded;
		iBytesAggregated.insert(
			iBytesAggregated.end(), 
			aImpl->iBytes.begin(), 
			aImpl->iBytes.end());
		}
	}

bool LayoutCdlInstanceOpt::LinesAreEqual(TLayoutLine& aLine1, TLayoutLine& aLine2)
	{
	TLayoutLine::iterator pValues1 = aLine1.begin();
	TLayoutLine::iterator pValues2 = aLine2.begin();
	for (; pValues1 != aLine1.end() && pValues2 != aLine2.end(); ++pValues1, ++pValues2)
		{
		if (TLayoutTable::IsValueColumn(pValues1->first) && *pValues1 != *pValues2)
			return false;
		}
	return true;
	}
