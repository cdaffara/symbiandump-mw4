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

#include "MasterLayoutPack.h"
#include "ZoomLevelNames.h"
#include "LayoutCompilerErr.h"
#include "CodeGenConsts.h"

#include <sstream>
#include <iostream>
#include <list>
#include <algorithm>

using namespace std;
using namespace CdlCompilerToolkit;

typedef LayoutProcessArgsErr<MasterLayoutPackage> MasterLayoutPackageArgsErr;

const string KMasterLayoutPackCdlFile = "MasterLayoutPack.cdl";

typedef vector<string> PackageInfoLine;

class MasterLayoutIndex
	{
public:
	MasterLayoutIndex(list<PackageInfoLine> packageInfo, CZoomLevelNames& zoomLevelName, const string& fileName);
	void BuildAndWrite();

private:
	enum InfoArgPositions
		{
		EPackNameArg = 0,
		EWidthArg = 1,
		EHeightArg = 2,
		EVariantArg = 3,
		EFirstContentArg = 4
		};
	
	struct PkgSize
		{
		string iW;
		string iH;
		bool operator==(const PkgSize& aRhs)
			{ return iW == aRhs.iW && iH == aRhs.iH; }
		};

private:
	void BuildOptionsVectors();
	void CreateTemplateInstance();
	void SetSizes();
	void SetVariants();
	void SetZooms();
	void SetContents();
	void WriteInstance();
	void Implement(const string& aApi, const string& aTarget, const string& aReplace);
	void AddPkg(PackageInfoLine& aLine, const string& aZoom, string& aExtra, string& aImpl);
	PackageInfoLine* FindLine(const PkgSize& aSize, const string& aVariant);

private:
	list<PackageInfoLine> iPackageInfo;
	CZoomLevelNames& iZooms;
	const string& iInstName;
	vector<PkgSize> iSizes;
	vector<string> iVariants;
	auto_ptr<CCdlTkInterface> iIface;
	auto_ptr<CCdlTkInstance> iInst;
	};


int MasterLayoutPackage::Process(vector<string>& args)
	{
	CZoomLevelNames zoomLevelNames;
	zoomLevelNames.ProcessOptions(args);

	if (args.size() != 2)
		throw MasterLayoutPackageArgsErr();
	
	string& fileName = args[1];
	ifstream in;
	CdlTkUtil::OpenInput(in, fileName);

	list<PackageInfoLine> packageInfo;
	string line;
	while (!in.eof())
		{
		getline(in, line);
		PackageInfoLine words;
		CdlTkUtil::Tokenize(line, back_insert_iterator<PackageInfoLine>(words));
		if (words.size())
			packageInfo.push_back(words);
		}
	in.close();

	MasterLayoutIndex index(packageInfo, zoomLevelNames, fileName.substr(0,fileName.find_first_of('.')));
	index.BuildAndWrite();

	return 0;
	}

void MasterLayoutPackage::ShowHelp(ostream& stream)
	{
	stream << "MasterLayPkg [-z<zoomList>] <packageListFile>" << endl;
	stream << "  Creates CDL package according to MasterLayoutPack.cdl containing" << endl;
	stream << "  all of the packages described in <packageListFile>." << endl;
	stream << "  The <packageListFile> should contain lines with the following contents:" << endl;
	stream << "    <layout pack name> <width> <height> <layout variant> <content>*" << endl;
	stream << "  If -z<zoomList> is specified, then instances for zoom factors" << endl;
	stream << "  (in the form \"n,string\") appearing in the file <zoomList> will be generated, " << endl;
	stream << "  by replacing the keyword ZOOM in the package definitions." << endl;
	}

void MasterLayoutPackage_Process(
	const string& aName, 
	const string& aWidth, 
	const string& aHeight, 
	const string& aId, 
	vector<string>::const_iterator aBegin, 
	vector<string>::const_iterator aEnd, 
	const string& aZoomName)
	{
	CCdlTkCdlFileParser parser(CdlTkUtil::CurrentDrive()+KDirDomainSysHeader+KMasterLayoutPackCdlFile);
	auto_ptr<CCdlTkInterface> iface(parser.LoadAndParse(true));
	CCdlTkInstance pkg(*iface);
	pkg.TemplateAllImplementations();

	string zoomName = CdlTkUtil::Replace("ZOOM", aZoomName, aName);
	string zoomIdName = "EAknUiZoom" + aZoomName;

	pkg.SetName(zoomName);

/*	Implement(pkg, "name", "\"\"", string("\"")+zoomName+"\"");
	Implement(pkg, "size", "?width", aWidth);
	Implement(pkg, "size", "?height", aHeight);
	Implement(pkg, "id", "?value", aId);
	Implement(pkg, "zoom", "?value", zoomIdName);
*/
	cout << zoomName << endl;

	for (; aBegin != aEnd; ++aBegin)
		{
		string zoomContent = CdlTkUtil::Replace("ZOOM", aZoomName, *aBegin);
//		pkg.AddLocalContent(zoomContent);
		cout << zoomContent << endl;
		}

	CCdlTkWriteInstance writer(pkg);
	writer.Process();
	}


MasterLayoutIndex::MasterLayoutIndex(list<PackageInfoLine> packageInfo, CZoomLevelNames& zoomLevelName, const string& fileName)
: iPackageInfo(packageInfo), iZooms(zoomLevelName), iInstName(fileName)
	{
	}

void MasterLayoutIndex::BuildAndWrite()
	{
	BuildOptionsVectors();
	CreateTemplateInstance();
	SetSizes();
	SetVariants();
	SetZooms();
	SetContents();
	WriteInstance();
	}

void MasterLayoutIndex::BuildOptionsVectors()
	{
	for (list<PackageInfoLine>::iterator pLine=iPackageInfo.begin(); pLine!=iPackageInfo.end(); ++pLine)
		{
		if (pLine->size() < EFirstContentArg)
			throw GeneralErr((*pLine)[EPackNameArg] + " package info has too few arguments");

		PkgSize size;
		size.iW = (*pLine)[EWidthArg];
		size.iH = (*pLine)[EHeightArg];
		if (find(iSizes.begin(), iSizes.end(), size) == iSizes.end())
			iSizes.push_back(size);

		string variant = (*pLine)[EVariantArg];
		if (find(iVariants.begin(), iVariants.end(), variant) == iVariants.end())
			iVariants.push_back(variant);
		}
	}

void MasterLayoutIndex::CreateTemplateInstance()
	{
	CCdlTkCdlFileParser parser(CdlTkUtil::CurrentDrive()+KDirDomainSysHeader+KMasterLayoutPackCdlFile);
	iIface = parser.LoadAndParse(true);
	iInst = auto_ptr<CCdlTkInstance>(new CCdlTkInstance(*iIface));
	iInst->TemplateAllImplementations();
	iInst->SetName(iInstName);
	// This master index should be the first in the DLL, so write it first.
	cout << iInstName << endl;
	}

void MasterLayoutIndex::SetSizes()
	{
	string impl;
	for (vector<PkgSize>::iterator pSize = iSizes.begin(); pSize != iSizes.end(); ++pSize)
		{
		CdlTkUtil::AppendString(impl, "\t{ ");
		CdlTkUtil::AppendString(impl, pSize->iW);
		CdlTkUtil::AppendString(impl, ", ");
		CdlTkUtil::AppendString(impl, pSize->iW);
		CdlTkUtil::AppendString(impl, " },\n");
		}
	Implement("sizes", "?array_contents", impl);
	}

void MasterLayoutIndex::SetVariants()
	{
	string impl;
	for (vector<string>::iterator pVar = iVariants.begin(); pVar != iVariants.end(); ++pVar)
		{
		CdlTkUtil::AppendString(impl, "\t");
		CdlTkUtil::AppendString(impl, *pVar);
		CdlTkUtil::AppendString(impl, ",\n");
		}
	Implement("variants", "?array_contents", impl);
	}

void MasterLayoutIndex::SetZooms()
	{
	string impl;
	for (CZoomLevelNames::iterator pZoom = iZooms.begin(); pZoom != iZooms.end(); ++pZoom)
		{
		CdlTkUtil::AppendString(impl, "\tEAknUiZoom");
		CdlTkUtil::AppendString(impl, pZoom->second);
		CdlTkUtil::AppendString(impl, ",\n");
		}
	Implement("zooms", "?array_contents", impl);
	}

void MasterLayoutIndex::SetContents()
	{
	string extra;
	string impl;
	for (vector<PkgSize>::iterator pSize = iSizes.begin(); pSize != iSizes.end(); ++pSize)
		{
		for (vector<string>::iterator pVar = iVariants.begin(); pVar != iVariants.end(); ++pVar)
			{
			PackageInfoLine* line = FindLine(*pSize, *pVar);
			for (CZoomLevelNames::iterator pZoom = iZooms.begin(); pZoom != iZooms.end(); ++pZoom)
				{
				if (line == NULL)
					CdlTkUtil::AppendString(impl, "\tNULL,\n");
				else
					AddPkg(*line, pZoom->second, extra, impl);
				}
			}
		}
	Implement("contents", "?array_contents", impl);
	iInst->SetExtraCpp(extra);
	}

void MasterLayoutIndex::WriteInstance()
	{
	CCdlTkWriteInstance writer(*iInst);
	writer.Process();
	}

void MasterLayoutIndex::Implement(const string& aApi, const string& aTarget, const string& aReplace)
	{
	CCdlTkImplementation* impl = iInst->Impl().Find(aApi);
	if (!impl)
		throw NotFoundErr(aApi + " in MasterLayoutPack.cdl");
	CdlTkUtil::CReplaceSet implSet;
	implSet.Add(aTarget, aReplace);
	implSet.Add("	//TODO: Initialise this data.", "");
	impl->SetDefinition(CdlTkUtil::MultiReplace(implSet, impl->Definition()));
	}

void MasterLayoutIndex::AddPkg(PackageInfoLine& aLine, const string& aZoom, string& aExtra, string& aImpl)
	{
	string impl = "&";
	string extra;

	string pkgName = CdlTkUtil::Replace("ZOOM", aZoom, aLine[EPackNameArg]);
	impl+=pkgName;

	string refs;
	for (unsigned int refPos = EFirstContentArg; refPos < aLine.size(); ++refPos)
		{
		string refName = CdlTkUtil::Replace("ZOOM", aZoom, aLine[refPos]);
		CdlTkUtil::AppendString(refs, CdlTkUtil::Replace("$NAME",refName,"\tLOCAL_CDL_REF($NAME),\n"));
		}

	CCdlTkDataTypeTranslations& translations = iIface->DataTypeTranslations();
	string temp;
	extra = translations.Find("TCdlArray<TCdlRef>", temp)->Definition();
	CdlTkUtil::CReplaceSet defnSet;
	defnSet.Add("aType", temp);
	defnSet.Add("aName", pkgName);
	defnSet.Add("?array_contents", refs);
	extra = CdlTkUtil::MultiReplace(defnSet, extra);

	CdlTkUtil::AppendString(aImpl, impl);
	CdlTkUtil::AppendString(aExtra, extra);
	}

PackageInfoLine* MasterLayoutIndex::FindLine(const PkgSize& aSize, const string& aVariant)
	{
	for (list<PackageInfoLine>::iterator pLine=iPackageInfo.begin(); pLine!=iPackageInfo.end(); ++pLine)
		{
		if (aSize.iW == (*pLine)[EWidthArg] &&
			aSize.iH == (*pLine)[EHeightArg] &&
			aVariant == (*pLine)[EVariantArg])
			return &*pLine;
		}
	return NULL;
	}

