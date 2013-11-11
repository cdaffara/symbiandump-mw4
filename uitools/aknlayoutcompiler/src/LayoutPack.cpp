/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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

#include "LayoutPack.h"
#include "ZoomLevelNames.h"
#include "LayoutCompilerErr.h"
#include "CodeGenConsts.h"
#include "AknLayoutConsts.h"

#include <sstream>
#include <iostream>

using namespace std;
using namespace CdlCompilerToolkit;

typedef LayoutProcessArgsErr<LayoutPackage> LayoutPackageArgsErr;

const string KLayoutPackCdlFile = "LayoutPack.cdl";

int LayoutPackage::Process(vector<string>& args)
	{
	if (args.size() < 7)
		throw LayoutPackageArgsErr();
	
	CZoomLevelNames zoomLevelNames;
	zoomLevelNames.ProcessOptions(args);

	string styleName;
	string deliveryTypeName;
    string priority;
    string appUid;

    ProcessOptionalStringArg(args, string("-s"), styleName);
    ProcessOptionalStringArg(args, string("-d"), deliveryTypeName);
    ProcessOptionalStringArg(args, string("-p"), priority);
    ProcessOptionalStringArg(args, string("-a"), appUid);

	string& name = args[2];
	string& w = args[3];
	string& h = args[4];
	string& id = args[5];

	for(CZoomLevelNames::iterator pZoomName = zoomLevelNames.begin(); pZoomName != zoomLevelNames.end(); ++pZoomName)
		{
		//string zoomName = CdlTkUtil::Replace("\r", "", pZoomName->second);
		//zoomName = CdlTkUtil::Replace("\n", "", zoomName);
		Process(name, w, h, id, styleName, deliveryTypeName, priority, appUid, args.begin() + 6, args.end(), pZoomName->second);
		}
	return 0;
	}

void LayoutPackage::ShowHelp(ostream& stream)
	{
	stream << "LayPkg [-z<zoomList>] [-s<screenStyleName>] [-d<deliverytype> -p<priority> [-a<appUid>]] <packageName> <width> <height> <layoutId> <instances>+" << endl;
	stream << "  Creates CDL package according to Layout.cdl containing:" << endl;
	stream << "    The name of the package." << endl;
	stream << "    The size of screen that it applies to." << endl;
	stream << "    The layout variant it is used for." << endl;
	stream << "    The contents of the package." << endl;
	stream << "  If -z<zoomList> is specified, then instances for zoom factors" << endl;
	stream << "    (in the form \"n,string\") appearing in the file <zoomList> will be generated, " << endl;
	stream << "    by replacing the keyword ZOOM in the package definitions." << endl;
	stream << "  If -s<screenStyleName> is specified, then instances for the appropriate" << endl;
	stream << "    screen style as found in WSINI.INI will be generated." << endl;
	stream << "  If -d<deliveryType> is specified then the delivery type will be appended to" << endl;
	stream << "    the layout XML directory name." << endl;
	stream << "  If -p<priority> is specified then the pack will be loaded in priority order compared to " << endl;
	stream << "    instances of the same interface in all other packs, otherwise defaults to zero." << endl;
	stream << "  If -a<appUid> is specified then the layout pack will only be loaded for an application" << endl;
	stream << "    with a matching Secure UID, defaults to zero which means that the pack will be loaded" << endl;
	stream << "    for any application ." << endl;
	}

void LayoutPackage::Process(
	const string& aName, 
	const string& aWidth, 
	const string& aHeight, 
	const string& aId, 
	const string& aStyleName,
    const string& aDeliveryTypeName,
    const string& aPriority,
    const string& aAppUid,
	vector<string>::const_iterator aBegin, 
	vector<string>::const_iterator aEnd, 
	const string& aZoomName)
	{
	CCdlTkCdlFileParser parser(CdlTkUtil::CurrentDrive()+KDirDomainSysHeader+KLayoutPackCdlFile);
	auto_ptr<CCdlTkInterface> iface(parser.LoadAndParse(true));
	CCdlTkPackage pkg(*iface);
	pkg.TemplateAllImplementations();

	string zoomName = CdlTkUtil::Replace("ZOOM", aZoomName, aName);
	string zoomIdName = "EAknUiZoom" + aZoomName;
	string styleHash = Hash(aStyleName);
    string priority = CdlTkUtil::IntToString(CdlTkUtil::ParseInt(aPriority));
    string appUid = CdlTkUtil::IntToHexString(CdlTkUtil::ParseInt(aAppUid));

	pkg.SetName(zoomName);

	Implement(pkg, "name", "\"\"", string("\"")+zoomName+"\"");
	Implement(pkg, "size", "?width", aWidth);
	Implement(pkg, "size", "?height", aHeight);
	Implement(pkg, "id", "?value", aId);
	Implement(pkg, "zoom", "?value", zoomIdName);
	Implement(pkg, "styleHash", "?value", styleHash, string("	// screen style ") + aStyleName);
	Implement(pkg, "priority", "?value", priority);
	Implement(pkg, "appUid", "?value", appUid);

	cout << zoomName << endl;

	for (; aBegin != aEnd; ++aBegin)
		{
		const string& arg = *aBegin;
		if (arg.size()>=2 && arg.substr(0,2)=="-x")
			{
			if (++aBegin == aEnd)
				throw LayoutPackageArgsErr();
			string dir = *aBegin;
			
			dir = CdlTkUtil::Replace("\\", "/", dir);
			if (*dir.rbegin() != '/')
				dir += "/";

			if (++aBegin == aEnd)
				throw LayoutPackageArgsErr();
			const string& dll = *aBegin;

			if (++aBegin == aEnd)
				throw LayoutPackageArgsErr();
			const string& inst = *aBegin;

			string zoomContent = CdlTkUtil::Replace("ZOOM", aZoomName, inst);
			pkg.AddExternalContent(zoomContent, dir, dll);
			}
		else
			{
			string zoomContent = CdlTkUtil::Replace("ZOOM", aZoomName, arg);
			pkg.AddLocalContent(zoomContent);
			cout << zoomContent << endl;
			}
		}

	CCdlTkWriteInstance writer(pkg);
	writer.Process();
	}

void LayoutPackage::Implement(
	CCdlTkPackage& aPkg, 
	const string& aApi, 
	const string& aTarget, 
	const string& aReplace,
	const string& aComment)
	{
	CCdlTkImplementation* impl = aPkg.Impl().Find(aApi);
	if (!impl)
		throw NotFoundErr(aApi + " in LayoutPack.cdl");
	CdlTkUtil::CReplaceSet implSet;
	implSet.Add(aTarget, aReplace);
	implSet.Add("	//TODO: Initialise this data.", aComment);
	impl->SetDefinition(CdlTkUtil::MultiReplace(implSet, impl->Definition()));
	}

void LayoutPackage::ProcessOptionalStringArg(vector<string>& args, const string& aOption, string& aArg)
	{
	for (vector<string>::iterator pArg = args.begin(); pArg != args.end(); ++pArg)
		{
		string& arg = *pArg;
		if (arg.size() >= 2 && arg.substr(0,2) == aOption)
			{
			aArg = arg.substr(2);
			args.erase(pArg);
			return;
			}
		}
	}

string LayoutPackage::Hash(const string& aString)
	{
	int hash = 0;
	for (string::const_iterator pChar = aString.begin(); pChar != aString.end(); ++pChar)
		{
		hash *= KAknLayoutScreenStyleNameHashMult;
		hash += *pChar;
		}
	return CdlTkUtil::IntToHexString(hash);
	}
