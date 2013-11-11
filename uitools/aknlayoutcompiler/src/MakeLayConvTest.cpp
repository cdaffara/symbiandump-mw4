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
*
*/



// disable "identifier was truncated to '255' characters in the browser information" warning
#pragma warning (disable:4786)

#include "MakeLayConvTest.h"
#include "LayoutCompilerErr.h"
#include "LayoutParse.h"
#include "Lay2Cdl.h"
#include "CodeGenConsts.h"
#include <fstream>
#include <algorithm>
#include <iostream>
using namespace std;
using namespace CdlCompilerToolkit;

typedef LayoutProcessArgsErr<MakeLayoutConvTest> MakeLayoutConvTestArgsErr;



int MakeLayoutConvTest::Process(const vector<string>& args)
	{
	if (args.size() != 4)
		throw MakeLayoutConvTestArgsErr();

	string cdlName = args[2];
	CCdlTkCdlFileParser parser(cdlName);
	auto_ptr<CCdlTkInterface> iface(parser.LoadAndParse(true));

	string layoutName = args[3];
	auto_ptr<TLayParseLayout> layoutParse = TLayParseLayout::Parse(layoutName);

	MakeLayoutConvTest process(*iface, *layoutParse);
	process.RunProcess();

	return 0;
	}

void MakeLayoutConvTest::ShowHelp(ostream& stream)
	{
	stream << "MakeLayConvTest <cdlName> <layoutName>" << endl;
	stream << "  Creates layout conversion test code." << endl;
	}

MakeLayoutConvTest::MakeLayoutConvTest(CCdlTkInterface& aInterface, TLayout& aLayout)
: iInterface(aInterface), iLayout(aLayout)
	{
	}

MakeLayoutConvTest::~MakeLayoutConvTest()
	{
	}

void MakeLayoutConvTest::RunProcess()
	{
	InitFiles();

	for (TLayout::iterator pTab = iLayout.begin(); pTab != iLayout.end(); ++pTab)
		{
		AddTableToTest(**pTab);
		}

	CompleteFiles();
	WriteTestFiles();
	}

void MakeLayoutConvTest::AddTableToTest(TLayoutTable& aTable)
	{
	for (TLayoutTable::iterator pLine = aTable.begin(); pLine != aTable.end(); ++pLine)
		{
		TLayoutLine& line = **pLine;
		AddLineToTest(line);
		}

	}

void MakeLayoutConvTest::AddLineToTest(TLayoutLine& aLine)
	{
	string apiName = LayoutToCdl::LineApiName(aLine);
	const CCdlTkFunctionApi* api = FindApi(apiName);
	if (!api)
		return;

	AddTest(aLine, *api);
	if (aLine["B"].size() > 1)
		{
		apiName = KFuncMultiline + apiName;
		api = FindApi(apiName);
		if (api)
			{
			AddTest(aLine, *api);
			}
		}
	}

void MakeLayoutConvTest::AddTest(TLayoutLine& aLine, const CCdlTkFunctionApi& aApi)
	{
	AddTestToOldH(aLine, aApi);
	AddTestToOldCpp(aLine, aApi);
	AddTestFunc(aLine, aApi);
	}

const CCdlTkFunctionApi* MakeLayoutConvTest::FindApi(const string& aName)
	{
	return &iInterface.ApiList().Find(aName)->AsFunc();
	}

void MakeLayoutConvTest::WriteTestFiles()
	{
	WriteTestFile("OldLayout.h", iOldH);
	WriteTestFile("OldLayout.cpp", iOldCpp);
	WriteTestFile("TestLayout.cpp", iFuncs + iTests);
	}

void MakeLayoutConvTest::WriteTestFile(const string& aFileName, const string& iContent)
	{
	CCdlTkFileCleanup tempFile;
	ofstream stream;
	CdlTkUtil::OpenTempOutput(stream, tempFile);
	stream << iContent;
	stream.close();
	CdlTkUtil::ExportFileIfWritable(tempFile, CdlTkUtil::ResolvePath(CdlTkUtil::OutputPath(), aFileName));
	}

void MakeLayoutConvTest::AddTestToOldH(TLayoutLine& aLine, const CCdlTkFunctionApi& aApi)
	{
	string f = OldFuncSig(aLine, aApi) + ";\n";
	CdlTkUtil::AppendString(iOldH, f);
	}

const string KOldFunc = "\
$TYPE Old_$NAME$PARAMLIST\n\
\t{\n\
\treturn $LAGNAME$PARAMS;\n\
\t}\n\n";

void MakeLayoutConvTest::AddTestToOldCpp(TLayoutLine& aLine, const CCdlTkFunctionApi& aApi)
	{
	string lagName = "AKN_LAYOUT_WINDOW_" + aApi.Name();
	if (aApi.ReturnType() == KTypeTextLineLayout)
		lagName = "AKN_LAYOUT_TEXT_" + aApi.Name();
	else if (aApi.ReturnType() == KTypeMultiLineTextLayout)
		lagName = "AKN_LAYOUT_MULTILINE_TEXT_" + aApi.Name().substr(KFuncMultiline.length());	// remove "Multiline_"

	string params = aApi.ParamNameList();
	if (!params.empty())
		params = string("(") + params + ")";

	CdlTkUtil::CReplaceSet rep;
	rep.Add("$TYPE", aApi.ReturnType());
	rep.Add("$NAME", aApi.Name());
	rep.Add("$PARAMLIST", aApi.ParamsTypeAndNameList());
	rep.Add("$LAGNAME", lagName);
	rep.Add("$PARAMS", params);
	CdlTkUtil::AppendString(iOldCpp, CdlTkUtil::MultiReplace(rep, KOldFunc));
	}

const string KTestFunc = "\
void Test_$NAME(const AknLayout::CInstance& aInst)\n\
\t{\n\
$PARENT\
$LOOPS_START\
$INDENTCheckLayout(aInst.$NAME($PARAMS), Old_$NAME($PARAMS));\n\
$LOOPS_END\
\t}\n\n";

const string KTestLoop = "$INDENTfor (" + KTypeInt + " $PARAM_NAME = 0; $PARAM_NAME < $MAX; $PARAM_NAME++)\n$INDENT\t{\n";

void MakeLayoutConvTest::AddTestFunc(TLayoutLine& aLine, const CCdlTkFunctionApi& aApi)
	{
	string testCall = string("\tTest_") + aApi.Name() + "(aInst);\n";
	CdlTkUtil::AppendString(iTests, testCall);
	string indent = "\t";

	string loopStart;
	string loopEnd;
	const CCdlTkApiParams& params = aApi.Params();
	for (CCdlTkApiParams::const_iterator pParam = params.begin(); pParam != params.end(); ++pParam)
		{
		if (pParam->Type() == KTypeInt)
			{
			string paramName = pParam->Name();
			int max = 0;
			for (TLayoutLine::iterator pVal = aLine.begin(); pVal != aLine.end(); ++pVal)
				{
				if (pVal->second.ParamName() == paramName)
					{
					max = pVal->second.size();
					break;
					}
				}

			CdlTkUtil::CReplaceSet rep;
			rep.Add("$PARAM_NAME", paramName);
			rep.Add("$MAX", CdlTkUtil::IntToString(max));
			rep.Add("$INDENT", indent);
			CdlTkUtil::AppendString(loopStart, CdlTkUtil::MultiReplace(rep, KTestLoop));
			indent += "\t";
			loopEnd = indent + "}\n" + loopEnd;
			}
		}

	CdlTkUtil::CReplaceSet rep;
	rep.Add("$NAME", aApi.Name());
	rep.Add("$LOOPS_START", loopStart);
	rep.Add("$PARAMS", aApi.ParamNameList());
	rep.Add("$LOOPS_END", loopEnd);
	rep.Add("$INDENT", indent);
	rep.Add("$PARENT", aLine.iNeedsP ? "\tTRect aParentRect(0,0,176,208);\n" : "");
	CdlTkUtil::AppendString(iFuncs, CdlTkUtil::MultiReplace(rep, KTestFunc));
	}

const string KOldFuncName = "Old_$NAME";
const string KFuncSig = "$TYPE $NAME$PARAMS";

string MakeLayoutConvTest::OldFuncSig(TLayoutLine& aLine, const CCdlTkFunctionApi& aApi)
	{
	string type = aApi.ReturnType();
	string name = CdlTkUtil::Replace("$NAME", aApi.Name(), KOldFuncName);
	string params = aApi.ParamsTypeAndNameList();

	CdlTkUtil::CReplaceSet rep;
	rep.Add("$TYPE", type);
	rep.Add("$NAME", name);
	rep.Add("$PARAMS", params);
	return CdlTkUtil::MultiReplace(rep, KFuncSig);
	}

void MakeLayoutConvTest::InitFiles()
	{
	iOldH = "";

	iOldCpp = "\
#include <e32std.h>\n\
#include <OldLayout.lag>\n\
#include \"OldLayout.h\"\n\n";

	iFuncs = "\
#include \"LayTestUtils.h\"\n\
#include \"OldLayout.h\"\n\n";

	iTests = "\
void RunTests(const AknLayout::CInstance& aInst)\n\
\t{\n";

	}

void MakeLayoutConvTest::CompleteFiles()
	{
	iTests += "\t}\n";
	}


