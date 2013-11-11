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
#pragma warning (disable:4786)	// disable "identifier was truncated to '255' characters in the browser information" warning
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <list>
#include <sstream>
#include <iomanip>
#include <memory>

#ifdef __MSVCRT__
#include <direct.h>
#endif

#include <CdlCompilerToolkit/CdlTkUtil.h>
#include <CdlCompilerToolkit/CdlTkProcess.h>
#include <CdlCompilerToolkit/CdlCompat.h>
using namespace std;
using namespace CdlCompilerToolkit;

#ifndef _DEBUG
#define EXCEPTION_HANDLING
#endif


typedef list<string> TArgsList;
auto_ptr<CCdlTkInterfaceList> ParseList(const TArgsList& aFiles);

//
// MainArgsErr
//

class MainArgsErr : public CdlCompilerToolkitErr
	{
public:
	MainArgsErr(const string& aExtraInfo = "");
	void Show(ostream& aStream) const;
private:
	string iExtraInfo;
	};

MainArgsErr::MainArgsErr(const string& aExtraInfo)
: iExtraInfo(aExtraInfo)
	{
	}

void MainArgsErr::Show(ostream& stream) const
	{
	if (iExtraInfo.size())
		{
		stream << endl;
		stream << iExtraInfo << endl;
		}
	stream << endl;
	stream << "CdlCompiler <mode> [<options>] <mode specific parameters>" << endl;
	stream << "  <mode> is:" << endl;
	stream << "    \"client\" mode creates the API for a customisable component." << endl;
	stream << "    \"instance\" mode creates customisation instance C++ files." << endl;
	stream << "    \"package\" mode creates package instance C++ files." << endl;
	stream << "    \"dll\" mode creates C++ and project files to collect together customisation instances in a DLL." << endl;
	stream << "    \"compare\" compares two CDL interfaces." << endl;
	stream << "  <options> are:" << endl;
	stream << "    -p<output path>  path for the output files." << endl;
	stream << "    -v               show CdlCompiler version." << endl;
	stream << "    -h               show help information for a <mode>." << endl;
	}


//
// CCompilerMode
//

class CCompilerMode 
	{
public:
	virtual ~CCompilerMode();
	virtual void ProcessOption(const std::string& aOpt);
	virtual int Process(const TArgsList& aArgs) = 0;
	virtual void ShowHelp(ostream& aStream) = 0;
	};

CCompilerMode::~CCompilerMode()
	{
	}

void CCompilerMode::ProcessOption(const std::string& aOpt)
	{
	throw MainArgsErr(string("unrecognised option for this mode ") + aOpt);
	}


//
// CClientMode
//

class CClientMode : public CCompilerMode
	{
public:
	int Process(const TArgsList& aArgs);
	void ShowHelp(ostream& aStream);
	};

int CClientMode::Process(const TArgsList& aArgs)
	{
	auto_ptr<CCdlTkInterfaceList> cdls(ParseList(aArgs));
	for (CCdlTkInterfaceList::iterator pCdl = cdls->begin(); pCdl != cdls->end(); ++pCdl)
		{
		CCdlTkWriteClientHeader processor(**pCdl);
		processor.Process();
		}
	return 0;
	}

void CClientMode::ShowHelp(ostream& aStream)
	{
	aStream << "CdlCompiler client <options> [<CDL files>]" << endl;
	aStream << "  client mode creates the API for a customisable component." << endl;
	aStream << "  For each CDL file parameter, it creates C++ header files containing the API." << endl;
	aStream << "  By default, these headers are placed in \\epoc32\\include." << endl;
	}

//
// CInstanceMode
//

class CInstanceMode : public CCompilerMode
	{
public:
	int Process(const TArgsList& aArgs);
	void ShowHelp(ostream& aStream);
	};

int CInstanceMode::Process(const TArgsList& aArgs)
	{
	TArgsList::const_iterator pArg = aArgs.begin();
	if (pArg == aArgs.end())
		throw MainArgsErr("Missing CDL file name");

	CCdlTkCdlFileParser parser(*pArg);
	auto_ptr<CCdlTkInterface> cdl = parser.LoadAndParse(true);

	++pArg;
	if (pArg == aArgs.end())
		throw MainArgsErr("Missing instance name");
	CCdlTkInstance inst(*cdl);
	inst.SetName(*pArg);
	inst.TemplateAllImplementations();

	++pArg;
	if (pArg != aArgs.end())
		{
		inst.SetId(CdlTkUtil::ParseInt(*pArg));
		++pArg;
		}

	if (pArg != aArgs.end())
		throw MainArgsErr("Too many parameters");

	CCdlTkWriteInstance processor(inst);
	processor.Process();
	
	return 0;
	}

void CInstanceMode::ShowHelp(ostream& aStream)
	{
	aStream << "CdlCompiler instance <options> <CDL file> <instance name> [<id>]" << endl;
	aStream << "  instance mode creates C++ files containing a template customisation instance." << endl;
	aStream << "  The customisation instance will implement the interface defined in <CDL file>." << endl;
	aStream << "  It and its source files will be called <instance name>." << endl;
	aStream << "  It will have an instance id of <id>, if specified. Otherwise," << endl;
	aStream << "  the instance id will come from the host DLL." << endl;
	}


//
// CPackageMode
//

class CPackageMode : public CCompilerMode
	{
public:
	int Process(const TArgsList& aArgs);
	void ShowHelp(ostream& aStream);
	};

int CPackageMode::Process(const TArgsList& aArgs)
	{
	TArgsList::const_iterator pArg = aArgs.begin();
	if (pArg == aArgs.end())
		throw MainArgsErr("Missing package CDL file name");

	CCdlTkCdlFileParser parser(*pArg);
	auto_ptr<CCdlTkInterface> cdl = parser.LoadAndParse(true);

	++pArg;
	if (pArg == aArgs.end())
		throw MainArgsErr("Missing package name");
	CCdlTkPackage pckg(*cdl);
	pckg.SetName(*pArg);
	pckg.TemplateAllImplementations();

	++pArg;
	if (pArg == aArgs.end())
		throw MainArgsErr("Missing package instance id");

	if (count_if(pArg->begin(), pArg->end(), CdlTkUtil::IsNumeric) == static_cast<signed int>( pArg->size()) )
		{
		pckg.SetId(CdlTkUtil::ParseInt(*pArg));
		++pArg;
		}

	for (; pArg != aArgs.end(); ++pArg)
		pckg.AddLocalContent(*pArg);

	CCdlTkWriteInstance processor(pckg);
	processor.Process();

	return 0;
	}

void CPackageMode::ShowHelp(ostream& aStream)
	{
	aStream << "CdlCompiler package <options> <CDL file> <package name> [<id>] [<instance names>]" << endl;
	aStream << "  package mode creates template package instance C++ files." << endl;
	aStream << "  The package instance will implement the interface defined in <CDL file>." << endl;
	aStream << "  It and its source files will be called <package name>." << endl;
	aStream << "  It will have an instance id of <id> if <id> is a number." << endl;
	aStream << "  The package contents will be references to the customisation instances <instance names>." << endl;
	aStream << "  Note: the package contents will assume that all instances are in the same DLL." << endl;
	}


//
// CDllMode
//

class CDllMode : public CCompilerMode
	{
public:
	CDllMode();
	void ProcessOption(const std::string& aOpt);
	int Process(const TArgsList& aArgs);
	void ShowHelp(ostream& aStream);
private:
	CCdlTkDll::CLibraries iLibs;
	string iExtraMmp;
	int iVersion;
	};

CDllMode::CDllMode()
: iVersion(1)
	{
	}

void CDllMode::ProcessOption(const std::string& aOpt)
	{
	if (aOpt[1] == 'l')
		{
		iLibs.push_back(aOpt.substr(2));
		}
	else if (aOpt[1] == 's')
		{
		CdlTkUtil::AppendString(iExtraMmp, string("SOURCE "));
		CdlTkUtil::AppendString(iExtraMmp, aOpt.substr(2));
		CdlTkUtil::AppendString(iExtraMmp, "\n");
		}
	else if (aOpt[1] == 'e')
		{
		CdlTkUtil::AppendString(iExtraMmp, aOpt.substr(2));
		CdlTkUtil::AppendString(iExtraMmp, "\n");
		}
	else if(aOpt[1] == 'n')
		{
		iVersion = CdlTkUtil::ParseInt(aOpt.substr(2));
		if(iVersion < 1) iVersion = 1;
		}
	else
		{
		CCompilerMode::ProcessOption(aOpt);
		}
	}

int CDllMode::Process(const TArgsList& aArgs)
	{
	TArgsList::const_iterator pArg = aArgs.begin();
	if (pArg == aArgs.end())
		throw MainArgsErr("Missing DLL name");

	CCdlTkDll dll;
	dll.SetName(*pArg);
	++pArg;

	dll.SetVersion(iVersion);

	if (pArg == aArgs.end())
		throw MainArgsErr("Missing DLL UID");

	dll.SetUid(CdlTkUtil::ParseInt(*pArg));
	++pArg;

	for (; pArg != aArgs.end(); ++pArg)
		{
		const string& inst = *pArg;
		if (inst[0]!='@')
			{
			dll.AddInstance(*pArg);
			}
		else
			{
			// read instance list from a file
			ifstream argFile;
			CdlTkUtil::OpenInput(argFile, inst.substr(1));
			string line;
			while (!argFile.eof())
				{
				getline(argFile, line);
				if (line.length())
					dll.AddInstance(line);
				}
			argFile.close();
			}
		}

	dll.Libraries().insert(dll.Libraries().end(), iLibs.begin(), iLibs.end());
	dll.SetExtraMmp(dll.ExtraMmp() + iExtraMmp);

	CCdlTkWriteDll processor(dll);
	processor.Process();

	return 0;
	}

void CDllMode::ShowHelp(ostream& aStream)
	{
	aStream << "CdlCompiler dll <options> <dll name> <uid> [<instance names>]" << endl;
	aStream << "  dll mode creates C++ and project files to collect together customisation instances in a DLL." << endl;
	aStream << "  The DLL and its source files will be called <dll name>." << endl;
	aStream << "  It will have a UID of <uid>." << endl;
	aStream << "  The DLL will contain customisation instances <instance names>." << endl;
	aStream << "  If an instance name starts with @, this will be treated as the name of a file" << endl;
	aStream << "  which contains a list of instance names, one per line." << endl;
	aStream << "  Options:" << endl;
	aStream << "    -l<libName> to add a library to the DLL." << endl;
	aStream << "    -s<sourceFileName> to add a source file to the DLL." << endl;
	aStream << "    -e<extraMmp> to add more statements to the DLL's MMP file." << endl;
	aStream << "    -n<version> to set the DLL version number in format x, defaults to 1 when not specified or invalid." << endl;
	aStream << "          Does not accept a minor version number." << endl;
	}


//
// CCompareModeChecker
//

class CCompareModeChecker : public MCdlTkApiCheckObserver
	{
public:
	CCompareModeChecker(const string& aLeft, const string& aRight);
	virtual ~CCompareModeChecker();
	virtual void StartCheck();
	virtual void CheckComplete();
	virtual void ApiInBoth(const CCdlTkApi& aApi);
	virtual void ApiNotInLeft(const CCdlTkApi& aApi);
	virtual void ApiNotInRight(const CCdlTkApi& aApi);
	
private:
	int iErrs;
	string iLeft;
	string iRight;
	};

CCompareModeChecker::CCompareModeChecker(const string& aLeft, const string& aRight)
: iLeft(aLeft), iRight(aRight)
	{
	}

CCompareModeChecker:: ~CCompareModeChecker()
    {

    }

void CCompareModeChecker::StartCheck()
	{
	iErrs = 0;
	}

void CCompareModeChecker::CheckComplete()
	{
	cout << "Check complete, " << iErrs << " differences found" << endl;
	}

void CCompareModeChecker::ApiInBoth(const CCdlTkApi& /*aApi*/)
	{
	}

void CCompareModeChecker::ApiNotInLeft(const CCdlTkApi& aApi)
	{
	cout << iRight << ": " << aApi.Name() << aApi.ParamsTypeAndNameList() << " not found in " << iLeft << endl;
	iErrs++;
	}

void CCompareModeChecker::ApiNotInRight(const CCdlTkApi& aApi)
	{
	cout << iLeft << ": " << aApi.Name() << aApi.ParamsTypeAndNameList() << " not found in " << iRight << endl;
	iErrs++;
	}


//
// CCompareMode
//

class CCompareMode : public CCompilerMode
	{
public:
	int Process(const TArgsList& aArgs);
	void ShowHelp(ostream& aStream);
	};

int CCompareMode::Process(const TArgsList& aArgs)
	{
	auto_ptr<CCdlTkInterfaceList> cdls(ParseList(aArgs));
	if (cdls->size() != 2)
		throw MainArgsErr("Exactly 2 CDL files required");
	CCompareModeChecker reporter(*aArgs.begin(), *++(aArgs.begin()));
	CCdlTkApiChecker checker(*(*cdls)[0], *(*cdls)[1], reporter);
	checker.Process();

	return 0;
	}

void CCompareMode::ShowHelp(ostream& aStream)
	{
	aStream << "CdlCompiler compare <options> <left CDL file> <right CDL file>" << endl;
	aStream << "  compare mode reports the difference between two CDL interfaces." << endl;
	}


//
// CCompareMode
//

class CBigApiTestMode : public CCompilerMode
	{
public:
	int Process(const TArgsList& aArgs);
	void ShowHelp(ostream& aStream);
	};

string MakeApiName(int i)
	{
	return string("Api")+CdlTkUtil::IntToString(i);
	}

int CBigApiTestMode::Process(const TArgsList& aArgs)
	{
	string name = "BigApi";
	int size = CdlTkUtil::ParseInt(*aArgs.begin());

	// make a CDL interface
	CCdlTkInterface iface;
	iface.SetFileName(name+".cdl");
	CCdlTkInterfaceHeader& header = iface.Header();
	header.SetName(name);
	header.SetUid(0x0fffffff);
	CCdlTkApiList& apiList = iface.ApiList();
	for (int ii=0; ii<size; ii++)
		{
		auto_ptr<CCdlTkFunctionApi> api(new CCdlTkFunctionApi(iface));
		api->SetName(MakeApiName(ii));
		api->SetReturnType("TInt");
		apiList.push_back(api.get());
		api.release();
		}

	// write the CDL file
	CCdlTkWriteCdlFile cdlWriter(iface);
	cdlWriter.Process();

	// make an instance
	CCdlTkInstance inst(iface);
	inst.SetName(name+"Inst");
	inst.TemplateAllImplementations();
	CCdlTkImplementations& impls = inst.Impl();
	for (CCdlTkImplementations::iterator pImpl = impls.begin(); pImpl != impls.end(); ++pImpl)
		{
		CCdlTkImplementation& imp = **pImpl;
		string defn = imp.Definition();
		defn = CdlTkUtil::Replace("//TODO: Implement this function.", "return 0;", defn);
		imp.SetDefinition(defn);
		}

	// write the instance
	CCdlTkWriteInstance instWriter(inst);
	instWriter.Process();

	return 0;
	}

void CBigApiTestMode::ShowHelp(ostream& /*aStream*/)
	{
	}


//
// static functions
//

bool ProcessOptions(TArgsList::iterator& aArgIter, const TArgsList::iterator& aEnd, CCompilerMode& aMode)
	{
	while (aArgIter != aEnd && (*aArgIter)[0] == '-')
		{
		string& arg = *aArgIter;
		if (arg.size() < 2)
			throw MainArgsErr("bad option \"-\"");
		switch (arg[1])
			{
			case 'p': 
				CdlTkUtil::SetOutputPath(arg.substr(2));
				break;
			case 'v': 
				cout << "CdlCompiler version " << KCdlCompilerMajorVersion << "." << KCdlCompilerMinorVersion << endl;
				return true;		// exit CdlCompiler
			case 'h': 
				aMode.ShowHelp(cerr);
				return true;		// exit CdlCompiler
			default: 
				aMode.ProcessOption(arg);
				break;
			}
		++aArgIter;
		}
	return false;	// continue execution of the CdlCompiler
	}

auto_ptr<CCdlTkInterfaceList> ParseList(const TArgsList& aFiles)
	{
	auto_ptr<CCdlTkInterfaceList> ifaceList(new CCdlTkInterfaceList);

	for (TArgsList::const_iterator pFile = aFiles.begin(); pFile != aFiles.end(); ++pFile)
		{
		CCdlTkCdlFileParser parser(*pFile);
		auto_ptr<CCdlTkInterface> cdl = parser.LoadAndParse(true);
		ifaceList->push_back(cdl.get());
		cdl.release();
		}

	return ifaceList;
	}

int DoMain(int argc, char* argv[])
	{
	CdlTkUtil::SetCommandLine(argc, argv);

	if (argc < 2)
		throw MainArgsErr("Missing mode");

	TArgsList args;
	copy(argv + 1, argv + argc, back_inserter(args));
	TArgsList::iterator pArg = args.begin();

	// process mode arg
	auto_ptr<CCompilerMode> mode;
	if (*pArg == "client")
		{
		mode = auto_ptr<CCompilerMode>(new CClientMode);
		CdlTkUtil::SetOutputPath(CdlTkUtil::CurrentDrive() +PATHSEP+"epoc32"+PATHSEP+"include"+PATHSEP);
		}
	else if (*pArg == "instance")
		{
		mode = auto_ptr<CCompilerMode>(new CInstanceMode);
		}
	else if (*pArg == "dll")
		{
		mode = auto_ptr<CCompilerMode>(new CDllMode);
		}
	else if (*pArg == "package")
		{
		mode = auto_ptr<CCompilerMode>(new CPackageMode);
		}
	else if (*pArg == "compare")
		{
		mode = auto_ptr<CCompilerMode>(new CCompareMode);
		}
	else if (*pArg == "BigApiTest")
		{
		mode = auto_ptr<CCompilerMode>(new CBigApiTestMode);
		}
	else
		throw MainArgsErr(string("Unrecognised mode ") + *pArg);
	++pArg;

	// process options args
	if (ProcessOptions(pArg, args.end(), *mode))
		return 0;

	// process mode args
	return mode->Process(TArgsList(pArg, args.end()));
	}

int main(int argc, char* argv[])
	{
#ifdef EXCEPTION_HANDLING
	try
		{
#endif
		return DoMain(argc, argv);
#ifdef EXCEPTION_HANDLING
		}
	catch (const CdlCompilerToolkitErr& aErr)
		{
		aErr.Show(cerr);
		}

	return 1;
#endif
	}

// End of File
