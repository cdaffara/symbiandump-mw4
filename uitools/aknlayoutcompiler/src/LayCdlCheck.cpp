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

#include "LayCdlCheck.h"
#include <iostream>
#include "LayoutCompilerErr.h"
#include "LayoutParse.h"
#include "Lay2Cdl.h"
#include <CdlCompilerToolkit/CdlTkProcess.h>
using namespace std;
using namespace CdlCompilerToolkit;


typedef LayoutProcessArgsErr<LayCdlCheck> LayCdlCheckArgsErr;

const string KDefaultLayCdlCheckOptions = "lcm";

int LayCdlCheck::Process(const vector<string>& args)
	{
	if (args.size() < 4)
		throw LayCdlCheckArgsErr();

	int nextArg = 2;
	string opt = args[nextArg];
	if (opt.size() >= 2 && opt.substr(0,2) == "-o")
		{
		nextArg++;
		opt = opt.substr(2);
		}
	else
		{
		opt = KDefaultLayCdlCheckOptions;
		}

	string layoutName = args[nextArg++];
	auto_ptr<TLayParseLayout> layout(TLayParseLayout::Parse(layoutName));
	auto_ptr<CCdlTkInterface> iface(new CCdlTkInterface);
	CCdlTkApiList& apiList = iface->ApiList();
	auto_ptr<CCdlTkInterface> xIface(new CCdlTkInterface);
	CCdlTkApiList& xApiList = xIface->ApiList();

	for (unsigned int arg = nextArg; arg < args.size(); arg++)
		{
		string fileName = args[arg];
		if (fileName.size() < 4)
			throw LayCdlCheckArgsErr();
		string ext = CdlTkUtil::ToLower(fileName.substr(fileName.size()-4));

		if (ext == ".lay")
			{
			auto_ptr<TLayParseLayout> nextLayout(TLayParseLayout::Parse(fileName));
			layout->Merge(TLayout::KMergeModeVariant, *nextLayout);
			}
		else if (ext == ".cdl")
			{
			CCdlTkApiList* pApiList = &apiList;
			if (fileName.substr(0,2) == "-x")
				{
				pApiList = &xApiList;
				fileName = fileName.substr(2);
				}
			CCdlTkCdlFileParser parser(fileName);
			auto_ptr<CCdlTkInterface> nextIface(parser.LoadAndParse(true));
			CCdlTkApiList& nextApi = nextIface->ApiList();
			for (CCdlTkApiList::iterator pApi = nextApi.begin(); pApi != nextApi.end(); ++pApi)
				pApiList->push_back((*pApi)->Clone(*iface));
			}
		else
			throw LayCdlCheckArgsErr();
		}

	LayCdlCheck process(*layout, *iface, *xIface, opt);
	process.Process();

	return 0;
	}

void LayCdlCheck::ShowHelp(ostream& stream)
	{
	stream << "LayCdlCheck [-o[glcm]] <layoutName>* <cdlName>* [-x<cdlName>*]" << endl;
	stream << "  Compare API for lay files against CDL interfaces" << endl;
	stream << "  -o[glcm] (default = -olcm) -  show output for:" << endl;
	stream << "    g = good matches" << endl;
	stream << "    l = lay file only" << endl;
	stream << "    c = CDL interface only" << endl;
	stream << "    m = mismatches" << endl;
	stream << "  Where multiple <layoutName> are specified, the layouts are merged with variant mode." << endl;
	stream << "  Where multiple <cdlName> are specified, the CDL interfaces are added together." << endl;
	stream << "  -x<cdlName> - exclude APIs in <cdlName> from the report." << endl;
	}


LayCdlCheck::LayCdlCheck(TLayout& aLayout, CCdlTkInterface& aInterface, CCdlTkInterface& aExcluded, const string& aOpt)
: iLayout(aLayout), iInterface(aInterface), iExcluded(aExcluded), iLayoutInterface(0), iOpt(aOpt)
	{
	}

LayCdlCheck::~LayCdlCheck()
	{
	delete iLayoutInterface;
	}

void LayCdlCheck::Process()
	{
	delete iLayoutInterface;
	iLayoutInterface = 0;

	auto_ptr<CCdlTkInterface> iface(LayoutToCdl::LayoutToInterface(iLayout));
	iLayoutInterface = iface.get();
	iface.release();

	vector<string> good;
	vector<string> onlyLay;
	vector<string> onlyCdl;
	vector<string> paramMismatch;

	Compare(good, onlyLay, onlyCdl, paramMismatch);

	if (HasOpt('g'))
		Report("Good API", good);
	if (HasOpt('l'))
		Report("API only in lay file", onlyLay);
	if (HasOpt('c'))
		Report("API only in CDL file", onlyCdl);
	if (HasOpt('m'))
		Report("API with mismatched interface", paramMismatch);
	}

void LayCdlCheck::Compare(vector<string>& aGood, vector<string>& aOnlyLay, vector<string>& aOnlyCdl, vector<string>& aParamMismatch)
	{
	CCdlTkApiList& layApi = iLayoutInterface->ApiList();
	CCdlTkApiList& cdlApi = iInterface.ApiList();
	CCdlTkApiList& xApi = iExcluded.ApiList();
	for (CCdlTkApiList::iterator pLay = layApi.begin(); pLay != layApi.end(); ++pLay)
		{
		const string& name = (*pLay)->Name();
		CCdlTkApi* xCdl = xApi.Find(name);
		if (xCdl && (*xCdl) == (**pLay))
			continue;

		string fullName = (*pLay)->ReturnType() + " " + name + (*pLay)->ParamsTypeAndNameList() + ";";
		CCdlTkApi* cdl = cdlApi.Find(name);
		if (cdl)
			{
			if (*cdl == **pLay)
				{
				aGood.push_back(fullName);
				}
			else
				{
				aParamMismatch.push_back(fullName + " vs " + cdl->ParamsTypeAndNameList());
				}
			}
		else
			{
			aOnlyLay.push_back(fullName);
			}
		}

	for (CCdlTkApiList::iterator pCdl = cdlApi.begin(); pCdl != cdlApi.end(); ++pCdl)
		{
		const string& name = (*pCdl)->Name();
		string fullName = (*pCdl)->ReturnType() + " " + name + (*pCdl)->ParamsTypeAndNameList();
		CCdlTkApi* lay = layApi.Find(name);
		if (!lay)
			{
			aOnlyCdl.push_back(fullName);
			}
		}
	}

void LayCdlCheck::Report(const string& aTitle, vector<string>& aApi)
	{
	cout << aTitle << endl;
	for (vector<string>::iterator pApi = aApi.begin(); pApi != aApi.end(); ++pApi)
		{
		cout << *pApi << endl;
		}
	cout << endl;
	}

bool LayCdlCheck::HasOpt(char c)
	{
	return iOpt.find(c) != string::npos;
	}


