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

#include "Lay2LayPerf.h"
#include "LayoutParse.h"
#include "LayoutCompilerErr.h"
#include <CdlCompilerToolkit/CdlTkProcess.h>
#include <fstream>
#include <iostream>
#include <algorithm>
#include "LayPerfWriter.h"

typedef LayoutProcessArgsErr<LayoutToLayPerf> LayoutToLayPerfArgsErr;


string LayoutToLayPerf::InterfaceName(const string& aFileName)
	{
	return aFileName.substr(0,aFileName.find_last_of('.'));
	}

int LayoutToLayPerf::Process(const vector<string>& args)
	{
	if (args.size() != 5)
		throw LayoutToLayPerfArgsErr();

	string cdlName = args[2];

	auto_ptr<TLayParseLayout> sourceLayout(TLayParseLayout::Parse(args[3]));
	string destLayout = args[4];

	LayoutToLayPerf layPerf(cdlName, *sourceLayout, destLayout);
	layPerf.WriteLayout();

	return 0;
	}

void LayoutToLayPerf::ShowHelp(ostream& stream)
	{
	stream << "Lay2LayPerf <layout.cdl> <source.lay> <layPerf.cpp> " << endl;
	stream << "Lay2LayPerf ..\\cdl\\AknLayout.cdl ..\\layout\\AknElaf.lay \\s60\\akntest\\layperf\\generated\\LayPerf_AknElaf.cpp " << endl;
	stream << "  This converts a LAY file into a specialised format for inclusion in the LayPerf test app." << endl;
	stream << "  LayPerf runs on the device, calling each API that would be generated from the LAY file." << endl;
	}



LayoutToLayPerf::LayoutToLayPerf(const string& aCdlName, TLayout& aSourceLayout, const string& aDestLayoutName)
	:	
	iLayout(aSourceLayout),
	iDestLayoutName(aDestLayoutName),
	iCdlName(aCdlName)
	{
	}

void LayoutToLayPerf::WriteLayout()
	{
	TLayPerfWriter writer(iLayout, iDestLayoutName);
	writer.Write(iCdlName);
	}

