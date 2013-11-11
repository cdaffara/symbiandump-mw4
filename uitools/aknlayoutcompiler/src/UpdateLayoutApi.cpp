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
#include "UpdateLayoutApi.h"
#include <cdlcompilertoolkit/cdltkprocess.h>
#include "Lay2Cdl.h"
#include "LayoutParse.h"
#include "LayoutCompilerErr.h"
#include <sstream>
#include <iostream>

using namespace std;

typedef LayoutProcessArgsErr<UpdateLayoutApi> UpdateLayoutApiArgsErr;


int UpdateLayoutApi::Process(const vector<string>& args)
	{
	if (args.size() != 6)
		throw UpdateLayoutApiArgsErr();
	
	const string& cdlName = args[2];
	const string& layoutName = args[3];
	const string& legacyName = args[4];
	const string& newName = args[5];

	UpdateLayoutApi(cdlName, layoutName, legacyName, newName);

	return 0;
	}

void UpdateLayoutApi::ShowHelp(ostream& stream)
	{
	stream << "UpdateLayoutApi <LayoutCdlApi> <CurrentLayout> <LegacyLayout> <NewLayout>" << endl;
	}

UpdateLayoutApi::UpdateLayoutApi(const std::string& aCdlName, const std::string& aLayoutName, const std::string& aLegacyName, const std::string& aNewName)
: iCdlName(aCdlName), iLegacyName(aLegacyName), iInterface(0), iLayout(0), iLegacy(0), iNew(0), iInterfaceChanged(false)
	{
	LoadFiles(aLayoutName, aNewName);
	MergeAndCheck();
	ProcessNewApi();
	WriteFiles();
	Report();
	}

UpdateLayoutApi::~UpdateLayoutApi()
	{
	delete iInterface;
	delete iLayout;
	delete iLegacy;
	delete iNew;
	}

void UpdateLayoutApi::LoadFiles(const std::string& aLayoutName, const std::string& aNewName)
	{
	CCdlTkCdlFileParser cdlParser(iCdlName);
	auto_ptr<CCdlTkInterface> iface(cdlParser.LoadAndParse(false));
	iInterface = iface.get();
	iface.release();

	auto_ptr<TLayParseLayout> layout(TLayParseLayout::Parse(aLayoutName));
	iLayout = layout.get();
	layout.release();

	auto_ptr<TLayParseLayout> legacy(TLayParseLayout::Parse(iLegacyName));
	iLegacy = legacy.get();
	legacy.release();

	auto_ptr<TLayParseLayout> newLay(TLayParseLayout::Parse(aNewName));
	iNew = newLay.get();
	newLay.release();
	}

void UpdateLayoutApi::MergeAndCheck()
	{
	TLayout combined(*iLayout);
	combined.Merge(TLayout::KMergeModeMerge, *iLegacy);
	auto_ptr<CCdlTkInterface> iface(LayoutToCdl::LayoutToInterface(combined));
	InterfaceCheck check;
	CCdlTkApiChecker process(*iInterface, *iface, check);
	process.Process();
	}

void UpdateLayoutApi::ProcessNewApi()
	{
	auto_ptr<CCdlTkInterface> newIface(LayoutToCdl::LayoutToInterface(*iNew));
	CCdlTkApiChecker process(*iInterface, *newIface, *this);
	process.Process();
	if (iInterfaceChanged)
		{
		CCdlTkInterface* curExt = iInterface->UltimateExtension();
		if (!iExt)
			iExt = new CCdlTkInterface();
		curExt->SetExtension(iExt);
		iExt->SetBase(curExt);
		iExt->Header().SetVersion(CCdlTkInterfaceHeader::CVersion(curExt->Header().Version().Major(), curExt->Header().Version().Minor()+1));
		}
	}

void UpdateLayoutApi::WriteFiles()
	{
	if (iInterfaceChanged)
		{
		}
	if (iLegacyUpdated)
		{
		}
	}

void UpdateLayoutApi::Report()
	{
	if (!iReport.empty())
		cout << iReport;
	}

void UpdateLayoutApi::StartCheck()
	{
	}

void UpdateLayoutApi::CheckComplete()
	{
	}

void UpdateLayoutApi::ApiInBoth(const CCdlTkApi& aApi)
	{
	}

void UpdateLayoutApi::ApiNotInLeft(const CCdlTkApi& aApi)
	{
	// new API present - add it to the extension
	iInterfaceChanged = true;
	iExt->ApiList().push_back(aApi.Clone(*iExt));
	}

void UpdateLayoutApi::ApiNotInRight(const CCdlTkApi& aApi)
	{
	// old API not present - rename it in the API, add data to legacy.
	iInterfaceChanged = true;
	
	string oldName = aApi.Name();
	CCdlTkApi* api = iInterface->ApiList().Find(oldName);

	CCdlTkInterfaceHeader& header = iInterface->UltimateExtension()->Header();
	stringstream newName;
	newName << oldName << "_V" << header.Version().Major() << "_" << header.Version().Minor();

	api->SetName(newName.str());
	AddDataToLegacy(oldName, newName.str());
	}

void UpdateLayoutApi::AddDataToLegacy(const std::string& /*aOldName*/, const std::string& /*aNewName*/)
	{
	iLegacyUpdated = true;
	}


void UpdateLayoutApi::InterfaceCheck::StartCheck()
	{
	}

void UpdateLayoutApi::InterfaceCheck::CheckComplete()
	{
	}

void UpdateLayoutApi::InterfaceCheck::ApiInBoth(const CCdlTkApi&)
	{
	}
	
void UpdateLayoutApi::InterfaceCheck::ApiNotInLeft(const CCdlTkApi&)
	{
	throw false;
	}
	
	void UpdateLayoutApi::InterfaceCheck::ApiNotInRight(const CCdlTkApi&)
	{
	throw false;
	}
