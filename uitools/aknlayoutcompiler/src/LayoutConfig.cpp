/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "LayoutConfig.h"
#include "LayoutCompilerErr.h"
#include <fstream>
#include <iostream>

using namespace std;

const string KBuildConfigFile("/epoc32/include/oem/bldvariant.hrh");
const string KLayoutMacroStart("__LAYOUT");

typedef LayoutProcessArgsErr<LayoutConfig> LayoutConfigArgsErr;


LayoutConfig::Size::Size()
: iWidth(0), iHeight(0)
	{
	}

LayoutConfig::Size::TOrientation LayoutConfig::Size::Orientation() const
	{
	if (iWidth < iHeight)
		return EPortrait;
	else if (iWidth == iHeight)
		return ESquare;
	else
		return ELandscape;
	}

bool LayoutConfig::Size::operator>(const Size& aRhs) const
	{
	if (iWidth == aRhs.iWidth)
		return iHeight > aRhs.iHeight;
	else
		return iWidth > aRhs.iWidth;
	}


int LayoutConfig::Process(const vector<string>& args)
	{
	if (args.size() < 3)
		throw LayoutConfigArgsErr();

	string mode = CdlTkUtil::ToLower(args[2]);

	auto_ptr<LayoutConfig> config(0);

	if (mode == "wsini")
		config = auto_ptr<LayoutConfig>(new WsiniConfig);
	else
		throw LayoutConfigArgsErr();

	config->ParseArgs(args);
	return config->Process();
	}

void LayoutConfig::ParseArgs(const vector<string>& args)
	{
	int argsSize = args.size();
	int nextArg = 3;
	while (nextArg < argsSize && args[nextArg][0] == '-')
		{
		ParseOpt(CdlTkUtil::ToLower(args[nextArg]));
		nextArg++;
		}

	if (nextArg == argsSize)
		throw LayoutConfigArgsErr();

	iTargetFileName = args[nextArg];
	nextArg++;

	if (nextArg < argsSize)
		iBaseFileName = args[nextArg];
	}

void LayoutConfig::ParseOpt(const std::string& opt)
	{
	if (opt.size() < 2)
		throw LayoutConfigArgsErr();

	switch (opt[1])
		{
		case 't':
			if (opt == "-tplatform")
				iTarget = EPlatform;
			else if (opt == "-tproduct")
				iTarget = EProduct;
			else
				throw LayoutConfigArgsErr();
			break;
		case 'l':
			if (opt == "-llegacy")
				iLegacyMode = ELegacyMode;
			else if (opt == "-lnolegacy")
				iLegacyMode = ENoLegacyMode;
			else
				throw LayoutConfigArgsErr();
			break;
		default:
			throw LayoutConfigArgsErr();
			break;
		}
	}

void LayoutConfig::ShowHelp(ostream& stream)
	{
	stream << "Config <configMode> [-t<target>] [-l<legacyMode>] <targetFile> [<baseFile>]" << endl;
	stream << "  <configMode> is one of wsini, epocini, aknlayout2" << endl;
	stream << "  <target> is one of platform, product (default is platform)" << endl;
	stream << "  <legacyMode> is one of legacy, nolegacy (default is legacy)" << endl;
	stream << "  <targetFile> is the file to be written to" << endl;
	stream << "  <baseFile> is a file containing the template for the target file" << endl;
	}

LayoutConfig::LayoutConfig()
: iTarget(EPlatform), iLegacyMode(ELegacyMode)
	{
	LoadAllSizes();
	LoadConfiguredSizes();
	}

LayoutConfig::~LayoutConfig()
	{
	}

const LayoutConfig::Sizes& LayoutConfig::AllSizes() const
	{
	return iAll;
	}

const LayoutConfig::Sizes& LayoutConfig::ConfiguredSizes() const
	{
	return iConf;
	}

const LayoutConfig::Sizes& LayoutConfig::SizesForTarget() const
	{
	if (iTarget == EPlatform)
		return iAll;
	else
		return iConf;
	}

void LayoutConfig::LoadAllSizes()
	{
	ifstream bldCfg;
	OpenBldCfg(bldCfg);

	string line;
	while (!bldCfg.eof())
		{
		getline(bldCfg, line);
		Size s;
		if (ParseSize(line, s))
			{
			iAll.insert(s);
			if (line.find("#define") == 0)
				iConf.insert(s);
			}
		}
	}

void LayoutConfig::LoadConfiguredSizes()
	{
	// done crudely as part of the LoadAllSizes() function currently
	}

void LayoutConfig::OpenBldCfg(ifstream& aBldCfg)
	{
	aBldCfg.open(KBuildConfigFile.c_str());
	if (!aBldCfg.is_open())
		throw NotFoundErr(KBuildConfigFile);
	}

bool LayoutConfig::ParseSize(const std::string& aLine, Size& aSize) const
	{
	int pos = aLine.find(KLayoutMacroStart);
	if (pos == string::npos)
		return false;

	pos += KLayoutMacroStart.size();
	pos = aLine.find_first_not_of('_', pos);
	if (pos == string::npos)
		return false;

	aSize.iWidth = CdlTkUtil::ParseInt(aLine.substr(pos));

	pos = aLine.find('_', pos);
	if (pos == string::npos)
		return false;
	pos = aLine.find_first_not_of('_', pos);
	if (pos == string::npos)
		return false;

	aSize.iHeight = CdlTkUtil::ParseInt(aLine.substr(pos));
	
	return (aSize.iWidth != 0 && aSize.iHeight != 0);
	}

/*
int LayoutConfig::TestLayoutConfig()
	{
	LayoutConfig l;

	cout << "All:" << endl;
	for (Sizes::iterator pAll = l.iAll.begin(); pAll != l.iAll.end(); ++pAll)
		{
		Size& s = *pAll;
		cout << s.iWidth << " x " << s.iHeight << endl;
		}
	cout << "Conf:" << endl;
	for (Sizes::iterator pConf = l.iConf.begin(); pConf != l.iConf.end(); ++pConf)
		{
		Size& s = *pConf;
		cout << s.iWidth << " x " << s.iHeight << endl;
		}
	return 0;
	}
*/


int WsiniConfig::Process()
	{
	const Sizes& s = SizesForTarget();
// need a lot more info to make a sensible descision here
// eg what is the prefered size and orientation? How should legacy mode be
// handled, etc.
// also, what should we do with multiple screens?????
// neeeds a lot more thought.
	return 0;
	}

