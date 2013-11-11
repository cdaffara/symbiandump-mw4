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

// disable "decorated name length exceeded, name was truncated" warning	
#pragma warning (disable:4503)


#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include "Extract.h"
#include "LayoutCompilerErr.h"
#include "Lay2Cdl.h"
#include "Cdl2Lag.h"
#include "LayCdl2Inst.h"
#include "LayCdlCheck.h"
#include "LayoutPack.h"
#include "LayCdl2InstO.h"
#include "MakeLayConvTest.h"
#include "LayScale.h"
#include "AdaptationLayerTemplate.h"
#include "Lay2LayPerf.h"

#ifdef RD_XML_PARSER_TOOLCHAIN
#include "MLCompData2Cdl.h"
#include "MLCompCdl2InstO.h"
#include "MLCompData2LayPerf.h"
#include "MLEqCompData2DHuiML.h"
#endif

using namespace std;

// In debug builds, we want the debugger to catch exceptions. In release mode we want
// to report exceptions to the command line user.
// The EXCEPTION_HANDLING macro flag controls this behavior in main()
#ifndef _DEBUG
#define EXCEPTION_HANDLING
#endif

class MainArgsErr : public LayoutCompilerErr
	{
	void Show(ostream& aStream) const;
	};

void MainArgsErr::Show(ostream& stream) const
	{
	stream << endl;
	stream << "AknLayoutCompiler @scriptfile" << endl;
	stream << "AknLayoutCompiler [<options>] <mode> [args]" << endl;
	stream << "  <options> are:" << endl;
	stream << "    -p<output path>  has no effect in this version of AknLayoutCompiler." << endl;
	stream << "  <mode> modes are listed below" << endl;
	stream << "  [args] depends on <mode> as listed below" << endl;
	LayoutExtract::ShowHelp(stream);
	LayoutToCdl::ShowHelp(stream);
	CdlToLag::ShowHelp(stream);
	LayoutAndCdlToCdlInstance::ShowHelp(stream);
	LayCdlCheck::ShowHelp(stream);
	LayoutPackage::ShowHelp(stream);
	LayoutCdlInstanceOpt::ShowHelp(stream);
	MakeLayoutConvTest::ShowHelp(stream);
	LayoutScale::ShowHelp(stream);
	AdaptationLayerTemplate::ShowHelp(stream);
	LayoutToLayPerf::ShowHelp(stream);
#ifdef RD_XML_PARSER_TOOLCHAIN
	MLCompDataToCdl::ShowHelp(stream);
	MLCompDataCdlInstanceOpt::ShowHelp(stream);
	MLEqCompDataToDHuiML::ShowHelp(stream);
#endif
	}

int DoMain(int argc, char* argv[])
	{
	CdlTkUtil::SetCommandLine(argc, argv);

	vector<string> args;
	copy(argv, argv+argc, back_inserter(args));

	if (args.size() < 2)
		{
		throw MainArgsErr();
		}

	if (args[1].size() >= 2 && args[1].substr(0,2) == "-p")
		{
		CdlTkUtil::SetOutputPath(args[1].substr(2));
		args.erase(args.begin()+1);
		}

	string modeName = CdlTkUtil::ToLower(args[1]);

	if (modeName == "extract")
		{
		LayoutExtract extract;
		return extract.Extract(args);
		}
	else if (modeName == "lay2cdl")
		{
		return LayoutToCdl::Process(args);
		}
	else if (modeName == "cdl2lag")
		{
		return CdlToLag::Process(args);
		}
	else if (modeName == "laycdl2inst")
		{
		return LayoutAndCdlToCdlInstance::Process(args);
		}
	else if (modeName == "laycdlcheck")
		{
		return LayCdlCheck::Process(args);
		}
	else if (modeName == "laypkg")
		{
		return LayoutPackage::Process(args);
		}
	else if (modeName == "laycdl2insto")
		{
		return LayoutCdlInstanceOpt::Process(args);
		}
	else if (modeName == "makelayconvtest")
		{
		return MakeLayoutConvTest::Process(args);
		}
	else if (modeName == "layscale")
		{
		return LayoutScale::Process(args);
		}
	else if (modeName == "adaptationlayertemplate")
		{
		return AdaptationLayerTemplate::Process(args);
		}
	else if (modeName == "lay2layperf")
		{
		return LayoutToLayPerf::Process(args);
		}
#ifdef RD_XML_PARSER_TOOLCHAIN
	else if (modeName == "mlcompdata2cdl")
		{
		return MLCompDataToCdl::Process(args);
		}
	else if (modeName == "mlcompcdl2insto")
		{
		return MLCompDataCdlInstanceOpt::Process(args);
		}
	else if (modeName == "mlcompdata2layperf")
		{
		return MLCompDataToLayPerf::Process(args);
		}
	else if (modeName == "mleqcompdata2dhuiml")
		{
		return MLEqCompDataToDHuiML::Process(args);
		}
#endif	
	else
		{
		throw MainArgsErr();
		}

	return 1;
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
