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
*       Defines the entry point for the console application
*
*/



// disable "identifier was truncated to '255' characters in the browser information" warning
#pragma warning (disable:4786)

#include "Lay2Cdl.h"
#include "LayoutParse.h"
#include "LayoutCompilerErr.h"
#include <CdlCompilerToolkit/CdlTkProcess.h>
#include <fstream>
#include <iostream>
#include <algorithm>
#include "CppWriter.h"
#include "CodeGenConsts.h"

using namespace std;

typedef LayoutProcessArgsErr<LayoutToCdl> LayoutToCdlArgsErr;

const int KGeneratedInterfaceMajorVer = 1;
const int KGeneratedInterfaceMinorVer = 0;

string LayoutToCdl::InterfaceName(const string& aFileName)
	{
	return aFileName.substr(0,aFileName.find_last_of('.'));
	}

int LayoutToCdl::Process(const vector<string>& args)
	{
	if (args.size() != 5)
		throw LayoutToCdlArgsErr();

	string layoutName = args[2];
	auto_ptr<TLayParseLayout> layout(TLayParseLayout::Parse(layoutName));

	string cdlName = args[3];
	int uid = CdlTkUtil::ParseInt(args[4]);
	auto_ptr<CCdlTkInterface> iface(LayoutToInterface(*layout));

	SetHeaders(*iface, cdlName, uid);

	CCdlTkWriteCdlFile writer(*iface);
	cout << "writing CDL file " << cdlName << endl;
	writer.Process();
	return 0;
	}

void LayoutToCdl::ShowHelp(ostream& stream)
	{
	stream << "Lay2Cdl <layoutName> <cdlName> <UID>" << endl;
	}

auto_ptr<CCdlTkInterface> LayoutToCdl::LayoutToInterface(const TLayout& aLayout)
	{
	auto_ptr<CCdlTkInterface> iface(new CCdlTkInterface);

	for (TLayout::const_iterator pTab = aLayout.begin(); pTab != aLayout.end(); ++pTab)
		{
		AddTableToInterface(*iface, **pTab);
		}

	return iface;
	}

void LayoutToCdl::AddTableToInterface(CCdlTkInterface& aInterface, TLayoutTable& aTable)
	{
	for (TLayoutTable::iterator pLine = aTable.begin(); pLine != aTable.end(); ++pLine)
		{
		if (aTable.iType == TLayoutTable::EWindowTable)
			AddWindowLineToInterface(aInterface, **pLine);
		else
			AddTextLineToInterface(aInterface, **pLine);

		if (pLine == aTable.begin())
			{
			CCdlTkApi& api = **aInterface.ApiList().rbegin();
			api.SetComment(string("// LAF Table : ") + aTable.Name() + "\n");
			}
		}

	int tableNum = 0;
	for (TLayoutTable::TLayoutSubTables::const_iterator pSub = aTable.iSubTables.begin(); pSub != aTable.iSubTables.end(); ++pSub)
		{
		AddTableLimitsToInterface(aInterface, aTable, **pSub, tableNum);
		string tableType = KTypeTextLineLayout;
		if (aTable.iType == TLayoutTable::EWindowTable)
			tableType == KTypeWindowLineLayout;
		AddSubTableToInterface(tableType, aInterface, aTable, **pSub, tableNum);
		tableNum++;
		}
	}

void LayoutToCdl::AddWindowLineToInterface(CCdlTkInterface& aInterface, TLayoutLine& aLine)
	{
	CCdlTkApi* api = CreateGenericApi(aInterface, aLine);

	api->SetName(LineApiName(aLine));
	api->SetReturnType(KTypeWindowLineLayout);

	aInterface.ApiList().push_back(api);
	}

void LayoutToCdl::AddTextLineToInterface(CCdlTkInterface& aInterface, TLayoutLine& aLine)
	{
	CCdlTkApi* api = CreateGenericApi(aInterface, aLine);

	api->SetName(LineApiName(aLine));
	api->SetReturnType(KTypeTextLineLayout);

	aInterface.ApiList().push_back(api);

	TValues& v = aLine["B"];
	if (v.size()>1)
		{
		CCdlTkFunctionApi* multiline = static_cast<CCdlTkFunctionApi*>(CreateGenericApi(aInterface, aLine));
		CCdlTkApiParams& params = multiline->Params();
		CCdlTkApiParams::iterator pParam = params.FindByName(KParamNameB);
		if (pParam != params.end())
			params.erase(pParam);
		params.push_back(CCdlTkApiParam(KTypeInt, KParamNameNumberOfLinesShown));

		multiline->SetName(KFuncMultiline + LineApiName(aLine));
		multiline->SetReturnType(KTypeMultiLineTextLayout);

		aInterface.ApiList().push_back(multiline);
		}
	}

string LayoutToCdl::TableApiName(TLayoutTable& aTable, TLayoutTable::TLayoutSubTable& aSubTable, int aSubTableNum)
	{
	if (aTable.size() == aSubTable.size())
		return CdlTkUtil::ToCpp(aTable.Name());
	else
		return CdlTkUtil::ToCpp(aTable.Name()) + "_SUB_TABLE_" + CdlTkUtil::IntToString(aSubTableNum);
	}

string LayoutToCdl::LineApiName(TLayoutLine& aLine)
	{
	if (aLine.iTable->iType == TLayoutTable::EWindowTable)
		{
		TWindowLineCppWriter writer(aLine);
		return writer.Name();
		}
	else
		{
		TTextLineCppWriter writer(aLine);
		return writer.Name();
		}
	}

void LayoutToCdl::AddSubTableToInterface(const string& aType, CCdlTkInterface& aInterface, TLayoutTable& aTable, TLayoutTable::TLayoutSubTable& aSubTable, int aSubTableNum)
	{
	TLayoutLine& line = *aTable[aSubTable[0]];
	CCdlTkFunctionApi* api = new CCdlTkFunctionApi(aInterface);
	api->Params().push_back(CCdlTkApiParam(KTypeInt, KParamLineIndex));
	AddParamsToFunc(line, *api);

	api->SetReturnType(aType);

	api->SetName(TableApiName(aTable, aSubTable, aSubTableNum));

	aInterface.ApiList().push_back(api);
	}

void LayoutToCdl::AddTableLimitsToInterface(CCdlTkInterface& aInterface, TLayoutTable& aTable, TLayoutTable::TLayoutSubTable& aSubTable, int aSubTableNum)
	{
	CCdlTkFunctionApi* api = new CCdlTkFunctionApi(aInterface);
	api->SetReturnType(KTypeLayoutTableLimits);
	api->SetName(TableApiName(aTable, aSubTable, aSubTableNum)+KFuncLimitsSuffix);
	aInterface.ApiList().push_back(api);
	}

CCdlTkApi* LayoutToCdl::CreateGenericApi(CCdlTkInterface& aInterface, TLayoutLine& aLine)
	{
	CCdlTkFunctionApi* func = new CCdlTkFunctionApi(aInterface);
	AddParamsToFunc(aLine, *func);
	return func;
	}

void LayoutToCdl::AddParamsToFunc(TLayoutLine& aLine, CCdlTkFunctionApi& aFunc)
	{
	CCdlTkApiParams& params = aFunc.Params();

	bool mirrored = aLine.iIsMirroredHorizontally;

	if (aLine.iNeedsP)
		params.push_back(CCdlTkApiParam(KTypeRect, KParamParentRect));

	if (aLine.iNeedsIndex)
		{
		const string* KParams;
		int count;
		if (aLine.iTable->iType == TLayoutTable::EWindowTable)
			{
			if (mirrored)
				KParams = KWindowOutputOrderMirrored;
			else
				KParams = KWindowOutputOrder;
			count = KWindowOutputOrderSize;
			}
		else
			{
			if (mirrored)
				KParams = KTextOutputOrderMirrored;
			else
				KParams = KTextOutputOrder;
			count = KTextOutputOrderSize;
			}

		for (int i=0; i<count; i++)
			{
			const string& name = KParams[i];

			const TValues& defValues = aLine[name];
			if (defValues.iNeedsIndex)
				{
				string paramName = CdlTkUtil::ToCpp(defValues.ParamName());
				if (mirrored)
					{
					if (paramName == KParamNameL)
						paramName = KParamNameR;
					else if (paramName == KParamNameR)
						paramName = KParamNameL;
					}

				CCdlTkApiParam param(KTypeInt, paramName);
				if (params.FindByName(param.Name()) == params.end())
					params.push_back(param);
				}
			}
		}
	}

void LayoutToCdl::SetHeaders(CCdlTkInterface& aInterface, const string& aCdlName, int aUid)
	{
	aInterface.SetFileName(aCdlName);
	aInterface.Header().SetUid(aUid);
	aInterface.Header().Flags().SetFlag("KCdlFlagRomOnly");

	string ifName(InterfaceName(CdlTkUtil::StripPath(aCdlName)));
	aInterface.Header().SetName(ifName);
	aInterface.Header().SetVersion(CCdlTkInterfaceHeader::CVersion(KGeneratedInterfaceMajorVer, KGeneratedInterfaceMinorVer));
	aInterface.Cpp().push_back("#include <aknlayout2def.h>");
	}

