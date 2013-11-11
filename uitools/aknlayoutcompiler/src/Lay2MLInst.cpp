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

#include "lay2MLInst.h"
#include "LayoutParse.h"
#include "LayoutCompilerErr.h"

#include <fstream>
#include <iostream>
#include <algorithm>
#include "MLInstWriter.h"
//#include "MLInstParse.h"

string TextOutputOrder(int aIndex, bool aMirrored);
string WindowOutputOrder(int aIndex, bool aMirrored);

using namespace std;

class LayoutToMLInstArgsErr : public LayoutCompilerErr
	{
	void Show(ostream& aStream) const;
	};

void LayoutToMLInstArgsErr::Show(ostream& stream) const
	{
	LayoutToMLInst::ShowHelp(stream);
	}

string LayoutToMLInst::InterfaceName(const string& aFileName)
	{
	return aFileName.substr(0,aFileName.find_last_of('.'));
	}

int LayoutToMLInst::Process(const vector<string>& args)
	{
	if (args.size() != 4)
		throw LayoutToMLInstArgsErr();

	// first read in the lay file
	string layoutName = args[2];
	auto_ptr<TLayParseLayout> layoutParse(TLayParseLayout::Parse(layoutName));

	// then convert it to internal structure format
	auto_ptr<TMLInst> mlInst(LayoutToInst(*layoutParse));

	// finally write out the structure XML
	string xmlName = args[3];
	cout << "writing XML file " << xmlName << endl;
	TMLInstWriter writer(*mlInst, xmlName);
	writer.Write(layoutName);
	return 0;
	}

void LayoutToMLInst::ShowHelp(ostream& stream)
	{
	stream << "Lay2MLInst <layoutName> <instXmlName>" << endl;
	}

auto_ptr<TMLInst> LayoutToMLInst::LayoutToInst(const TLayout& aLayout)
	{
	auto_ptr<TMLInst> mlInst(new TMLInst);

	for (TLayout::const_iterator pTab = aLayout.begin(); pTab != aLayout.end(); ++pTab)
		{
		AddTableToInst(*mlInst, **pTab);
		}
	
	return mlInst;
	}

void LayoutToMLInst::AddTableToInst(TMLInst& aInst, TLayoutTable& aTable)
	{
	TMLInstTable* instTable = new TMLInstTable(&aInst, aTable);

	for (TLayoutTable::iterator pLine = aTable.begin(); pLine != aTable.end(); ++pLine)
		{
		if (aTable.iType == TLayoutTable::EWindowTable)
			AddWindowLineToInstTable(*instTable, **pLine);
		else
			AddTextLineToInstTable(*instTable, **pLine);
		}

/*
	int tableNum = 0;
	for (TLayoutTable::TLayoutSubTables::const_iterator pSub = aTable.iSubTables.begin(); pSub != aTable.iSubTables.end(); ++pSub)
		{
		AddTableLimitsToInterface(aInterface, aTable, **pSub, tableNum);
		if (aTable.iType == TLayoutTable::EWindowTable)
			AddSubTableToInterface("TAknWindowLineLayout", aInterface, aTable, **pSub, tableNum);
		else
			AddSubTableToInterface("TAknTextLineLayout", aInterface, aTable, **pSub, tableNum);
		tableNum++;
		}
*/
	aInst.push_back(instTable);
	}

void LayoutToMLInst::AddWindowLineToInstTable(TMLInstTable& aInstTable, TLayoutLine& aLine)
	{
	TMLInstLine* instLine = new TMLInstLine(&aInstTable, aLine);
/*
	CCdlTkApi* api = CreateGenericApi(aInterface, aLine);
	api->SetName(LineApiName(aLine));
	api->SetReturnType("TAknWindowLineLayout");
	aInterface.ApiList().push_back(api);
*/


	// add a value for each top level cell
	for (int i=0; i<7; i++)
		{
		string cellName = WindowOutputOrder(i, false);
//		WriteCell(line, cellName, aLine[cellName]);
		AddParamToLine(*instLine, aLine[cellName]);
		}

/*
	for(TLayoutLine::iterator pValue = aLine.begin(); pValue != aLine.end(); ++pValue)
		{
		AddParamToLine(instLine, *pValue);
		}
*/
	aInstTable.push_back(instLine);
	}

void LayoutToMLInst::AddTextLineToInstTable(TMLInstTable& aInstTable, TLayoutLine& aLine)
	{
	TMLInstLine* instLine = new TMLInstLine(&aInstTable, aLine);

/*
	CCdlTkApi* api = CreateGenericApi(aInterface, aLine);

	api->SetName(LineApiName(aLine));
	api->SetReturnType("TAknTextLineLayout");

	aInterface.ApiList().push_back(api);
*/

	for (int i=0; i<6; i++)
		{
		string cellName = TextOutputOrder(i, false);
//		WriteCell(line, cellName, aLine[cellName]);
		AddParamToLine(*instLine, aLine[cellName]);
		}

/*
	for(TLayoutLine::iterator pValue = aLine.begin(); pValue != aLine.end(); ++pValue)
		{
		AddParamToLine(instLine, *pValue);
		}
*/
	aInstTable.push_back(instLine);


/*
	TValues& v = aLine["B"];
	if (v.size()>1)
		{
		CCdlTkFunctionApi* multiline = static_cast<CCdlTkFunctionApi*>(CreateGenericApi(aInterface, aLine));
		CCdlTkApiParams& params = multiline->Params();
		CCdlTkApiParams::iterator pParam = params.FindByName("aIndex_B");
		if (pParam != params.end())
			params.erase(pParam);
		params.push_back(CCdlTkApiParam("TInt", "aNumberOfLinesShown"));

		multiline->SetName(string("Multiline_") + LineApiName(aLine));
		multiline->SetReturnType("TAknMultiLineTextLayout");

		aInterface.ApiList().push_back(multiline);
		}
*/
	}

/*
string LayoutToMLInst::TableApiName(TLayoutTable& aTable, TLayoutTable::TLayoutSubTable& aSubTable, int aSubTableNum)
	{
	if (aTable.size() == aSubTable.size())
		return CdlTkUtil::ToCpp(aTable.Name());
	else
		return CdlTkUtil::ToCpp(aTable.Name()) + "_SUB_TABLE_" + IntToString(aSubTableNum);
	}

string LayoutToMLInst::LineApiName(TLayoutLine& aLine)
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

void LayoutToMLInst::AddSubTableToInterface(const string& aType, CCdlTkInterface& aInterface, TLayoutTable& aTable, TLayoutTable::TLayoutSubTable& aSubTable, int aSubTableNum)
	{
	TLayoutLine& line = *aTable[aSubTable[0]];
	CCdlTkFunctionApi* api = new CCdlTkFunctionApi(aInterface);
	api->Params().push_back(CCdlTkApiParam("TInt", "aLineIndex"));
	AddParamsToFunc(line, *api);

	api->SetReturnType(aType);

	api->SetName(TableApiName(aTable, aSubTable, aSubTableNum));

	aInterface.ApiList().push_back(api);
	}

void LayoutToMLInst::AddTableLimitsToInterface(CCdlTkInterface& aInterface, TLayoutTable& aTable, TLayoutTable::TLayoutSubTable& aSubTable, int aSubTableNum)
	{
	CCdlTkFunctionApi* api = new CCdlTkFunctionApi(aInterface);
	api->SetReturnType("TAknLayoutTableLimits");
	api->SetName(TableApiName(aTable, aSubTable, aSubTableNum)+"_Limits");
	aInterface.ApiList().push_back(api);
	}

CCdlTkApi* LayoutToMLInst::CreateGenericApi(CCdlTkInterface& aInterface, TLayoutLine& aLine)
	{
	CCdlTkFunctionApi* func = new CCdlTkFunctionApi(aInterface);
	AddParamsToFunc(aLine, *func);
	return func;
	}

void LayoutToMLInst::AddParamsToFunc(TLayoutLine& aLine, CCdlTkFunctionApi& aFunc)
	{
	CCdlTkApiParams& params = aFunc.Params();

	if (aLine.iNeedsP)
		params.push_back(CCdlTkApiParam("const TRect&", "aParentRect"));

	if (aLine.iNeedsIndex)
		{
		const string* KParams;
		int count;
		if (aLine.iTable->iType == TLayoutTable::EWindowTable)
			{
			KParams = KWindowOutputOrder;
			count = KWindowOutputOrderSize;
			}
		else
			{
			KParams = KTextOutputOrder;
			count = KTextOutputOrderSize;
			}

		for (int i=0; i<count; i++)
			{
			const string& name = KParams[i];

			const TValues& defValues = aLine[name];
			if (defValues.iNeedsIndex)
				{
				CCdlTkApiParam param("TInt", CdlTkUtil::ToCpp(defValues.ParamName()));
				if (params.FindByName(param.Name()) == params.end())
					params.push_back(param);
				}
			}
		}
	}

*/

void LayoutToMLInst::AddParamToLine(TMLInstLine& aInstLine, TValues& aValue)
	{
//	CCdlTkApiParams& params = aFunc.Params();

	TMLInstParam* instLine = new TMLInstParam(&aInstLine, aValue);

	for(TValues::iterator pInt = aValue.begin(); pInt != aValue.end(); ++pInt)
		{
		instLine->iValues.push_back(*pInt);
		}

/*
	if (aLine.iNeedsP)
		params.push_back(CCdlTkApiParam("const TRect&", "aParentRect"));

	if (aLine.iNeedsIndex)
		{
		const string* KParams;
		int count;
		if (aLine.iTable->iType == TLayoutTable::EWindowTable)
			{
			KParams = KWindowOutputOrder;
			count = KWindowOutputOrderSize;
			}
		else
			{
			KParams = KTextOutputOrder;
			count = KTextOutputOrderSize;
			}

		for (int i=0; i<count; i++)
			{
			const string& name = KParams[i];

			const TValues& defValues = aLine[name];
			if (defValues.iNeedsIndex)
				{
				CCdlTkApiParam param("TInt", CdlTkUtil::ToCpp(defValues.ParamName()));
				if (params.FindByName(param.Name()) == params.end())
					params.push_back(param);
				}
			}
		}
*/

	aInstLine.push_back(instLine);
	}
