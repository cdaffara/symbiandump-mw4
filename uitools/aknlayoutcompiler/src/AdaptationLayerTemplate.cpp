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

#include "AdaptationLayerTemplate.h"
#include "LayoutCompilerErr.h"
#include "LayoutParse.h"
#include "LayoutWriter.h"
#include "CppWriter.h"
#include "Lay2Cdl.h"
#include <fstream>
#include <algorithm>
#include <iostream>
#include <sstream>
#include "CodeGenConsts.h"
using namespace std;
using namespace CdlCompilerToolkit;

typedef LayoutProcessArgsErr<AdaptationLayerTemplate> AdaptationLayerTemplateArgsErr;




int AdaptationLayerTemplate::Process(const vector<string>& args)
	{
	if (args.size() < 6)
		throw AdaptationLayerTemplateArgsErr();

	string layName = args[2];
	auto_ptr<TLayParseLayout> layoutParse = TLayParseLayout::Parse(layName);

	string layCdlName = args[3];
	CCdlTkCdlFileParser layCdlParser(layCdlName);
	auto_ptr<CCdlTkInterface> layIface(layCdlParser.LoadAndParse(true));

	string scaleCdlName = args[4];
	CCdlTkCdlFileParser scaleCdlParser(scaleCdlName);
	auto_ptr<CCdlTkInterface> scaleIface(scaleCdlParser.LoadAndParse(true));

	string instName = args[5];
	auto_ptr<CCdlTkInstance> inst(new CCdlTkInstance(*layIface));
	inst->SetName(instName);
	inst->TemplateAllImplementations();

	string existing;
	if (args.size() == 8)
		{
		string inputName = args[6];
		CdlTkUtil::ReadFile(existing, inputName);
		}

	AdaptationLayerTemplate process(*layoutParse, *layIface, *scaleIface, *inst, existing);
	process.Process();

	CCdlTkWriteInstance writer(*inst);
	writer.Process();

	if (args.size() == 8)
		{
		string outputName = args[7];
		CdlTkUtil::WriteFile(existing, outputName);
		}

	return 0;
	}

void AdaptationLayerTemplate::ShowHelp(ostream& stream)
	{
	stream << "AdaptationLayerTemplate <layout.lay> <layInterface.cdl> <scaleInterface.cdl> <instanceName> [<existingImplToModify> <fileToWriteModsTo>]" << endl;
	stream << "  Creates a template layout adaptation layer implementation." << endl;
	}


AdaptationLayerTemplate::AdaptationLayerTemplate(TLayout& aLayout, CCdlTkInterface& aLayIface, CCdlTkInterface& aScaleIface, CCdlTkInstance& aInstance, string& aExisting)
: iLayIface(aLayIface), iScaleIface(aScaleIface), iLayout(aLayout), iInstance(aInstance), iExisting(aExisting)
	{
	}

AdaptationLayerTemplate::~AdaptationLayerTemplate()
	{
	}

void AdaptationLayerTemplate::Process()
	{
	for (TLayout::iterator pTab = iLayout.begin(); pTab != iLayout.end(); ++pTab)
		{
		AddTableToInstance(**pTab);
		}
	}

void AdaptationLayerTemplate::AddTableToInstance(TLayoutTable& aTable)
	{
	for (TLayoutTable::iterator pLine = aTable.begin(); pLine != aTable.end(); ++pLine)
		{
		TLayoutLine& line = **pLine;
		AddLineToInstance(line);
		}
	}

void AdaptationLayerTemplate::AddLineToInstance(TLayoutLine& aLine)
	{
	string apiName = LayoutToCdl::LineApiName(aLine);
	if (!HasApi(apiName))
		return;
	CCdlTkImplementation& imp = FindImp(apiName);
	SetFuncLine(imp, aLine);
	}

bool AdaptationLayerTemplate::HasApi(const string& aName)
	{
	return iLayIface.ApiList().Find(aName) != 0;
	}

CCdlTkImplementation& AdaptationLayerTemplate::FindImp(const string& aName)
	{
	CCdlTkImplementation* impl = iInstance.Impl().Find(aName);
	if (!impl)
		throw NotFoundErr(aName + " in interface " + iLayIface.FileName());
	return *impl;
	}

int CountMatch(const string& aLeft, const string& aRight)
	{
	string left(CdlTkUtil::ToLower(aLeft));
	left = CdlTkUtil::Replace("_line_", "", left);
	string right(CdlTkUtil::ToLower(aRight));

	int count = 0;
	int subLen = 3;
	int leftSize = left.size();
	const int maxPossibleHits = (leftSize + 1 - subLen);
	for (int ii=0; ii<maxPossibleHits; ii++)
		{
		if (right.find(left.substr(ii, subLen)) != string::npos)
			count++;
		}

	const int KMatchPercentLimit = 25;
	int percent = count * 100 / maxPossibleHits;
	if (percent < KMatchPercentLimit)
		return 0;
	return percent;
	}

pair<string,int> AdaptationLayerTemplate::GetApiMatch(const string& aName)
	{
	//int size = aName.size();
	CCdlTkApiList& apiList = iScaleIface.ApiList();
	string bestMatch;
	int bestScore = 0;
	for (CCdlTkApiList::iterator pApi = apiList.begin(); pApi != apiList.end(); ++pApi)
		{
		CCdlTkApi& api = **pApi;
		string name = api.Name();
		if (aName == name)
			return make_pair(name,100);

		int score = CountMatch(aName, name);
		if (score > bestScore)
			{
			bestScore = score;
			bestMatch = name;
			}
		}
	return make_pair(bestMatch, bestScore);
	}

bool IsValueCell(const string& aCellName)
	{
	return
		(find(KWindowOutputOrder, KWindowOutputOrder+KWindowOutputOrderSize, aCellName) != KWindowOutputOrder+KWindowOutputOrderSize)
		||
		(find(KTextOutputOrder, KTextOutputOrder+KTextOutputOrderSize, aCellName) != KTextOutputOrder+KTextOutputOrderSize);
	}

const string KUsefulInfo = "\
// $LAYOUT\n\
// Remark                    : $REMARK\n\
// Parent                    : $PARENTNAME\n\
// Parent API                : $PARENTAPI\n\
// Scale API match    ($AM%) : $SCALEAPI\n\
// Scale parent match ($PM%) : $SCALEPARENT\n\
";

void AdaptationLayerTemplate::SetFuncLine(CCdlTkImplementation& aImp, TLayoutLine& aLine)
	{
	string layout;
	for (TLayoutLine::iterator pCell = aLine.begin(); pCell != aLine.end(); ++pCell)
		{
		const string& cellName = pCell->first;
		if (IsValueCell(cellName))
			{
			TValues& values = pCell->second;
			stringstream s;
			TLayoutTableWriter::WriteCell(s, values);
			CdlTkUtil::AppendString(layout, cellName);
			CdlTkUtil::AppendString(layout, "=");
			CdlTkUtil::AppendString(layout, s.str());
			CdlTkUtil::AppendString(layout, " ");
			}
		}

	string parent;
	string parentApi;
	if (aLine.iTable && aLine.iTable->iParent)
		{
		parent = aLine.iTable->iParent->Name();
		parentApi = LayoutToCdl::LineApiName(*aLine.iTable->iParent);
		}

	pair<string,int> apiMatch(GetApiMatch(LayoutToCdl::LineApiName(aLine)));
	pair<string,int> parentMatch(GetApiMatch(parentApi));

	char apiPercent[12];
	sprintf(apiPercent, "%3d", apiMatch.second);
	char parentPercent[12];
	sprintf(parentPercent, "%3d", parentMatch.second);

	CdlTkUtil::CReplaceSet set;
	set.Add("$LAYOUT", layout);
	set.Add("$REMARK", aLine["Remarks"][0]);
	set.Add("$PARENTNAME", parent);
	set.Add("$PARENTAPI", parentApi);
	set.Add("$SCALEAPI", apiMatch.first);
	set.Add("$SCALEPARENT", parentMatch.first);
	set.Add("$AM", apiPercent);
	set.Add("$PM", parentPercent);
	string useful = CdlTkUtil::MultiReplace(set, KUsefulInfo);

	string defn = aImp.Definition();
	string existingTarget = defn.substr(0,defn.find_first_of('(')+1);
	string existingSub = useful;
	CdlTkUtil::AppendString(existingSub, existingTarget);
	iExisting = CdlTkUtil::Replace(existingTarget, existingSub, iExisting);

	CdlTkUtil::AppendString(useful, defn);
	aImp.SetDefinition(useful);
	}
