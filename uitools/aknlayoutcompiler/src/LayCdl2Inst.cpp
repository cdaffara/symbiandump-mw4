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

#include "LayCdl2Inst.h"
#include "LayoutCompilerErr.h"
#include "LayoutParse.h"
#include "CppWriter.h"
#include "Lay2Cdl.h"
#include <fstream>
#include <algorithm>
#include <iostream>
#include "CodeGenConsts.h"
using namespace std;
using namespace CdlCompilerToolkit;

typedef LayoutProcessArgsErr<LayoutAndCdlToCdlInstance> LayoutAndCdlToCdlInstanceArgsErr;


class LayoutAndCdlToCdlInstanceChecker : public MCdlTkApiCheckObserver
	{
private:
	void StartCheck();
	void CheckComplete();
	void ApiInBoth(const CCdlTkApi& aApi);
	void ApiNotInLeft(const CCdlTkApi& aApi);
	void ApiNotInRight(const CCdlTkApi& aApi);

private:
	int iNotInLeft;
	int iNotInRight;
	};


void LayoutAndCdlToCdlInstanceChecker::StartCheck()
	{
	iNotInLeft = 0;
	iNotInRight = 0;
	}

void LayoutAndCdlToCdlInstanceChecker::CheckComplete()
	{
	if (iNotInLeft)
		cout << "Layout is partial implementation of interface" << endl;
	if (iNotInRight)
		throw CdlTkAssert("Layout interface check failed");
	}

void LayoutAndCdlToCdlInstanceChecker::ApiInBoth(const CCdlTkApi& /*aApi*/)
	{
	}

void LayoutAndCdlToCdlInstanceChecker::ApiNotInLeft(const CCdlTkApi& /*aApi*/)
	{
	iNotInLeft++;
	}

void LayoutAndCdlToCdlInstanceChecker::ApiNotInRight(const CCdlTkApi& aApi)
	{
	iNotInRight++;
	cerr << aApi.Name() << " not in interface" << endl;
	}


int LayoutAndCdlToCdlInstance::Process(const vector<string>& args)
	{
	if (args.size() < 5 || args.size()%2 == 0)
		throw LayoutAndCdlToCdlInstanceArgsErr();

	string cdlName = args[2];
	CCdlTkCdlFileParser parser(cdlName);
	auto_ptr<CCdlTkInterface> iface(parser.LoadAndParse(true));

	LayoutAndCdlToCdlInstance process(*iface);

	TLayout* base = NULL;
	for (unsigned int arg = 3; arg < args.size(); arg += 2)
		{
		string layoutName = args[arg];
		string instName = args[arg+1];
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

	process.WriteInstances();

	return 0;
	}

void LayoutAndCdlToCdlInstance::ShowHelp(ostream& stream)
	{
	stream << "LayCdl2Inst <cdlName> (<layoutName> <instanceName>)+ " << endl;
	stream << "  Creates CDL instances containing the layout data." << endl;
	stream << "  All layout instances must conform to the CDL interface." << endl;
	stream << "  If more than one layout is supplied, subsequent ones are treated as" << endl;
	stream << "  variants of the first." << endl;
	}

void LayoutAndCdlToCdlInstance::CheckLayoutAgainstInterface(const TLayout& aLayout, const CCdlTkInterface& aIface)
	{
	auto_ptr<CCdlTkInterface> layIface = LayoutToCdl::LayoutToInterface(aLayout);
	LayoutAndCdlToCdlInstanceChecker checker;
	CCdlTkApiChecker check(*layIface, aIface, checker);
	check.Process();
	}

LayoutAndCdlToCdlInstance::LayoutAndCdlToCdlInstance(CCdlTkInterface& aInterface)
: iInterface(aInterface)
	{
	}

LayoutAndCdlToCdlInstance::~LayoutAndCdlToCdlInstance()
	{
	for (CLayouts::iterator pLayout = iLayouts.begin(); pLayout != iLayouts.end(); ++pLayout)
		delete *pLayout;
	}

void LayoutAndCdlToCdlInstance::AddLayout(auto_ptr<TLayout>& aLayout, const string& aInstName)
	{
	CLayoutToInst* base = NULL;
	if (iLayouts.size())
		base = iLayouts[0];
	auto_ptr<CLayoutToInst> p(new CLayoutToInst(base, iInterface, aLayout, aInstName));
	iLayouts.push_back(p.get());
	p.release();
	}

void LayoutAndCdlToCdlInstance::WriteInstances()
	{
	CLayouts::iterator pLayout = iLayouts.begin();
	CLayouts::iterator end = iLayouts.end();
	
	if (pLayout!=end)
		(*pLayout)->Process();

	for (; pLayout != end; ++pLayout)
		{
		CCdlTkWriteInstance writer((*pLayout)->Instance());
		writer.Process();
		}
	}


CLayoutToInst::CLayoutToInst(CLayoutToInst* aBase, CCdlTkInterface& aInterface, auto_ptr<TLayout>& aLayout, const string& aInstName)
: iBase(aBase), iInterface(aInterface), iLayout(*aLayout), iInstName(aInstName)
	{
	iLayoutPtr = aLayout.get();
	aLayout.release();

	iInstance = new CCdlTkInstance(iInterface);
	iInstance->SetName(aInstName);

	string extraCpp("#include <aknlayout2datadef.h>\n");
	extraCpp += string("#include \"") + CdlTkUtil::ToLower(aBase ? aBase->FwdDeclName() : FwdDeclName()) + "\"\n";
	iInstance->SetExtraCpp(extraCpp);

	for (TLayout::iterator pTab = iLayout.begin(); pTab != iLayout.end(); ++pTab)
		{
		AddTableToInstance(**pTab);
		}
	}

CLayoutToInst::~CLayoutToInst()
	{
	delete iInstance;
	delete iLayoutPtr;
	}

string CLayoutToInst::FwdDeclName()
	{
	if (iBase)
		return iBase->FwdDeclName();
	else
		return iInstName + ".FwdDecl.h";
	}

void CLayoutToInst::Process()
	{
	string fileName(FwdDeclName());
	string interfaceNsName = iInterface.NamespaceName();

	ofstream stream;
	CCdlTkFileCleanup tempFile;
	CdlTkUtil::OpenTempOutput(stream, tempFile);

	WriteHeaderGuardStart(fileName, stream);
	stream << "namespace " << CdlTkUtil::ToCpp(iInstance->Name()) << endl;
	stream << "{" << endl;

	CCdlTkApiList& apiList = iInterface.ApiList();
	for (CCdlTkApiList::iterator pApi = apiList.begin(); pApi != apiList.end(); ++pApi)
		{
		if ((*pApi)->IsFunc())
			{
			stream << "extern " << interfaceNsName << "::" << (*pApi)->AsFunc().ApiNameAsTypeName() << " " << (*pApi)->Name() << ";" << endl;
			}
		else
			{
			string typeExt = "Imp";
			if ((*pApi)->ReturnType() == KTypeLayoutTableLimits)
				typeExt = "";
			stream << "extern " << (*pApi)->ReturnType() << typeExt << " const " << (*pApi)->Name() << ";" << endl;
			}
		}

	stream << "}" << endl << endl;
	WriteHeaderGuardEnd(fileName, stream);

	stream.close();
	CdlTkUtil::ExportFile(tempFile, CdlTkUtil::OutputPath() + fileName);
	}

CCdlTkInstance& CLayoutToInst::Instance()
	{
	return *iInstance;
	}

void CLayoutToInst::AddTableToInstance(TLayoutTable& aTable)
	{
	for (TLayoutTable::iterator pLine = aTable.begin(); pLine != aTable.end(); ++pLine)
		{
		TLayoutLine& line = **pLine;
		if (line.iIsMergedIdentical)
			{
			string apiName = LayoutToCdl::LineApiName(line);
			AddFwdRefToInstance(apiName, false);

			string multilineApiName = KFuncMultiline + apiName;
			if (aTable.iType == TLayoutTable::ETextTable &&
				line["B"].size() > 1 &&
				HasApi(multilineApiName))
				{
				AddFwdRefToInstance(multilineApiName, false);
				}
			}
		else
			{
			if (aTable.iType == TLayoutTable::EWindowTable)
				AddWindowLineToInstance(line);
			else
				AddTextLineToInstance(line);
			}
		}

	int tableNum = 0;
	for (TLayoutTable::TLayoutSubTables::const_iterator pSub = aTable.iSubTables.begin(); pSub != aTable.iSubTables.end(); ++pSub)
		{
		TLayoutTable::TLayoutSubTable& sub = **pSub;
		if (sub.iIsMergedIdentical)
			{
			AddFwdRefToInstance(LayoutToCdl::TableApiName(aTable, sub, tableNum)+KFuncLimitsSuffix, false);
			AddFwdRefToInstance(LayoutToCdl::TableApiName(aTable, sub, tableNum), false);
			}
		else
			{
			AddTableLimitsToInstance(aTable, sub, tableNum);
			string tableType = KTypeTextLineLayout;
			if (aTable.iType == TLayoutTable::EWindowTable)
				tableType = KTypeWindowLineLayout;
			AddSubTableToInstance(tableType, aTable, sub, tableNum);
			}
		tableNum++;
		}
	}

void CLayoutToInst::AddWindowLineToInstance(TLayoutLine& aLine)
	{
	string apiName = LayoutToCdl::LineApiName(aLine);
	if (!HasApi(apiName))
		return;
	CCdlTkImplementation& imp = FindImp(apiName);
	SetFuncLine(imp, aLine, KTypeWindowLineLayout, KWindowOutputOrder, KWindowOutputOrderSize, "", "");
	}

const string KMultiLineTextLineExtra = "\
\tline.iB = KB[0];\n\
\tline.iBaselineSkip = KB[1]-KB[0];\n\
\tline.iNumberOfLinesShown = aNumberOfLinesShown;\n";

const string KTextLineExtra = "\
\tline.iBaselineSkip = 0;\n\
\tline.iNumberOfLinesShown = 1;\n";

void CLayoutToInst::AddTextLineToInstance(TLayoutLine& aLine)
	{
	string apiName = LayoutToCdl::LineApiName(aLine);
	if (!HasApi(apiName))
		return;
	CCdlTkImplementation& imp = FindImp(apiName);
	SetFuncLine(imp, aLine, KTypeTextLineLayout, KTextOutputOrder, KTextOutputOrderSize, "", KTextLineExtra);
	if (aLine["B"].size() > 1)
		{
		string preExtra = "\tTInt " + KParamNameB + " = 0;\n";
		if (aLine["B"].ParamName() != KParamNameB)
			preExtra = "";
		string apiName = KFuncMultiline + imp.Name();
		if (HasApi(apiName))
			{
			CCdlTkImplementation& multiLineImp = FindImp(KFuncMultiline + imp.Name());
			SetFuncLine(multiLineImp, aLine, KTypeMultiLineTextLayout, KTextOutputOrder, KTextOutputOrderSize, preExtra, KMultiLineTextLineExtra);
			}
		}
	}

bool CLayoutToInst::HasApi(const string& aName)
	{
	return iInterface.ApiList().Find(aName) != 0;
	}

CCdlTkImplementation& CLayoutToInst::FindImp(const string& aName)
	{
	CCdlTkImplementation* impl = iInstance->Impl().Find(aName);
	if (!impl)
		throw NotFoundErr(aName + " in interface " + iInterface.FileName());
	return *impl;
	}


// subtable function uses the main table of pointers, with a mask to ensure that only correct lines are accessed
// which are indexed by aLineIndex

const string KSubTableImpl = "\
#TYPE #NAME#PARAMLIST\n\
\t{\n\
\tstatic #PTRTYPE const * const KSubTableImpl = (#PTRTYPE const * const)&KCdlImpl.#FIRST_MEMBER;\n\
\tASSERT(0<=" + KParamLineIndex + " && " + KParamLineIndex + "<32 && ((1<<" + KParamLineIndex + ") & #VALID_INDEX));\n\
\t#PTRTYPE KImpl = KSubTableImpl[" + KParamLineIndex + "];\n\
\treturn #EVAL;\n\
\t}";

const string KSubTableFuncEval = "(*KImpl)(#PARAMNAMES)";
const string KSubTableDataEval = "*KImpl";

void CLayoutToInst::AddSubTableToInstance(const string& aType, TLayoutTable& aTable, TLayoutTable::TLayoutSubTable& aSubTable, int aTableNum)
	{
	string apiName = LayoutToCdl::TableApiName(aTable, aSubTable, aTableNum);
	if (!HasApi(apiName))
		return;

	CCdlTkImplementation& imp = FindImp(apiName);
	const CCdlTkFunctionApi& api = imp.Api().AsFunc();
	//const CCdlTkApiParams& params = api.Params();

	TLayoutLine& line = *aTable[aSubTable[0]];
	CCdlTkImplementation& lineImp = FindImp(LayoutToCdl::LineApiName(line));
	const CCdlTkApi& lineApi = lineImp.Api();

	string ptrType;
	if (lineApi.IsFunc())
		ptrType = iInterface.NamespaceName() + "::" + lineApi.PointerType();
	else
		ptrType = lineApi.PointerType();
	
	int valid = 0;
	for (unsigned int ii=0; ii<aTable.size(); ii++)
		{
		if (find(aSubTable.begin(), aSubTable.end(), ii) != aSubTable.end())
			{
			valid |= 1<<ii;
			}
		}

	string firstMember = LayoutToCdl::LineApiName(*aTable[0]);

	CdlTkUtil::CReplaceSet implSet;
	implSet.Add("#TYPE", aType);
	implSet.Add("#NAME", imp.Name());
	implSet.Add("#PARAMLIST", api.ParamsTypeAndNameList());
	implSet.Add("#PTRTYPE", ptrType);
	implSet.Add("#FIRST_MEMBER", firstMember);
	implSet.Add("#VALID_INDEX", CdlTkUtil::IntToHexString(valid));
	implSet.Add("#EVAL", lineApi.IsFunc() ?
			CdlTkUtil::Replace("#PARAMNAMES", lineApi.AsFunc().ParamNameList(), KSubTableFuncEval) :
			KSubTableDataEval);
	string impl = CdlTkUtil::MultiReplace(implSet, KSubTableImpl);

	imp.SetTemplatePointerReference();
	imp.SetDefinition(impl);
	}

const string KTableLimitsImpl =
KTypeLayoutTableLimits + " #NAME()\n\
\t{\n\
\tconst " + KTypeLayoutTableLimits + " limits = {#FIRST, #LAST};\n\
\treturn limits;\n\
\t}";

void CLayoutToInst::AddTableLimitsToInstance(TLayoutTable& aTable, TLayoutTable::TLayoutSubTable& aSubTable, int aTableNum)
	{
	string apiName = LayoutToCdl::TableApiName(aTable, aSubTable, aTableNum)+KFuncLimitsSuffix;
	if (!HasApi(apiName))
		return;

	CCdlTkImplementation& imp = FindImp(apiName);

	int first = *aSubTable.begin();
	int last = *aSubTable.rbegin();

	CdlTkUtil::CReplaceSet implSet;
	implSet.Add("#NAME", imp.Name());
	implSet.Add("#FIRST", CdlTkUtil::IntToString(first));
	implSet.Add("#LAST", CdlTkUtil::IntToString(last));
	string impl = CdlTkUtil::MultiReplace(implSet, KTableLimitsImpl);

	imp.SetTemplatePointerReference();
	imp.SetDefinition(impl);
	}


// data implementation is a simple const static data declaration

const string KDataImpl = "const #TYPEImp #NAME = {#DATA};";
const string KDataPtr = "(#TYPE*)&#NAME";

void CLayoutToInst::SetDataLine(CCdlTkImplementation& aImp, TLayoutLine& aLine, const string& aType, const string aOutputOrder[], int aOutputOrderSize)
	{
	string data;
	for (int ii=0; ii<aOutputOrderSize; ii++)
		{
		if (ii != 0)
			data += ",";
		string value = aLine[aOutputOrder[ii]][0];
		CdlTkUtil::AppendString(data, TValues::CppValue(value));
		}

	CdlTkUtil::CReplaceSet implSet;
	implSet.Add("#TYPE", aType);
	implSet.Add("#NAME", aImp.Name());
	implSet.Add("#DATA", data);
	string impl = CdlTkUtil::MultiReplace(implSet, KDataImpl);

	CdlTkUtil::CReplaceSet ptrSet;
	ptrSet.Add("#TYPE", aType);
	ptrSet.Add("#NAME", aImp.Name());
	string ptr = CdlTkUtil::MultiReplace(ptrSet, KDataPtr);

	aImp.SetPointerReference(ptr);
	aImp.SetDefinition(impl);
	}


// function implementation starts with asserts for parameter ranges and
// tables of values for the parameterised cells.
// The actual structure is initialised for the fixed values and indexed lookups
// on the tables

const string KFunctionImpl = "\
#TYPE #NAME#PARAMTYPEANDNAMELIST\n\
\t{\n\
#PRE_EXTRA\
#ASSERTS\
#TABLES\
\t#TYPEImp line = {#INIT};\n\
#LOOKUPS\
#POST_EXTRA\
#RECT_ADJUST\
\treturn line;\n\
\t}";

const string KFunctionAssert = "\tASSERT(0<=#PARAMNAME && #PARAMNAME<#PARAMLIMIT);\n";
const string KFunctionTable = "\tconst static TInt16 K#TABLENAME[#PARAMLIMIT] = {#VALUES};\n";
const string KFunctionLookup = "\tline.i#TABLENAME = K#TABLENAME[#PARAMNAME];\n";
const string KFunctionRect = "\tline.SetAllParentOffsets(" + KParamParentRect + ");\n";

void CLayoutToInst::SetFuncLine(CCdlTkImplementation& aImp, TLayoutLine& aLine, const string& aType, const string aOutputOrder[], int aOutputOrderSize, const string& aPreExtra, const string& aPostExtra)
	{
	const CCdlTkFunctionApi& api = aImp.Api().AsFunc();
	//const CCdlTkApiParams& params = api.Params();

	string asserts;
	string tables;
	string init;
	string lookups;
	for (int ii=0; ii<aOutputOrderSize; ii++)
		{
		TValues& values = aLine[aOutputOrder[ii]];
		const string& paramName = values.ParamName();
		const string& paramLimit = CdlTkUtil::IntToString(values.size());
		const string& tableName = aOutputOrder[ii];

		if (ii != 0)
			init += ",";
		if (values.size() > 1)
			{
			string v;
			for (TValues::iterator pVal = values.begin(); pVal != values.end(); ++pVal)
				{
				if (pVal != values.begin())
					v += ",";
				v += TValues::CppValue(*pVal);
				}

			CdlTkUtil::CReplaceSet replaceSet;
			replaceSet.Add("#PARAMNAME", paramName);
			replaceSet.Add("#PARAMLIMIT", paramLimit); 
			replaceSet.Add("#TABLENAME", tableName); 
			replaceSet.Add("#VALUES", v); 

			CdlTkUtil::AppendString(asserts, CdlTkUtil::MultiReplace(replaceSet, KFunctionAssert));
			CdlTkUtil::AppendString(tables, CdlTkUtil::MultiReplace(replaceSet, KFunctionTable));
			CdlTkUtil::AppendString(lookups, CdlTkUtil::MultiReplace(replaceSet, KFunctionLookup));
			CdlTkUtil::AppendString(init, "0");
			}
		else
			{
			CdlTkUtil::AppendString(init, TValues::CppValue(values[0]));
			}
		}

	string paramTypeAndNameList(api.ParamsTypeAndNameList());
	if (aLine.iIsMirroredHorizontally)
		MirrorParamNames(paramTypeAndNameList);

	CdlTkUtil::CReplaceSet implSet;
	implSet.Add("#TYPE", aType);
	implSet.Add("#NAME", api.Name());
	implSet.Add("#PARAMTYPEANDNAMELIST", paramTypeAndNameList);
	implSet.Add("#PRE_EXTRA", aPreExtra);
	implSet.Add("#ASSERTS", asserts);
	implSet.Add("#TABLES", tables);
	implSet.Add("#INIT", init);
	implSet.Add("#LOOKUPS", lookups);
	implSet.Add("#POST_EXTRA", aPostExtra);
	implSet.Add("#RECT_ADJUST", aLine.iNeedsP ? KFunctionRect : "");
	string impl = CdlTkUtil::MultiReplace(implSet, KFunctionImpl);

	aImp.SetTemplatePointerReference();
	aImp.SetDefinition(impl);
	}

void CLayoutToInst::MirrorParamNames(std::string& aNames)
	{
	aNames = CdlTkUtil::Replace(KParamNameL, "__tmp__", aNames);
	aNames = CdlTkUtil::Replace(KParamNameR, KParamNameL, aNames);
	aNames = CdlTkUtil::Replace("__tmp__", KParamNameR, aNames);
	}

void CLayoutToInst::AddFwdRefToInstance(const string& aName, bool aCast)
	{
	string ns(CdlTkUtil::ToCpp(iBase->iInstance->Name()));
	if (!HasApi(aName))
		return;

	CCdlTkImplementation& imp = FindImp(aName);
	string ptr;
	if (aCast)
		ptr = string("(") + imp.Api().ReturnType() + "*)";
	ptr += string("&") + ns + "::" + aName;
	imp.SetPointerReference(ptr);
	}



