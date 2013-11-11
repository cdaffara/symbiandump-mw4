/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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

// disable "decorated name length exceeded, name was truncated"
#pragma warning (disable:4503)

#include <CdlCompilerToolkit/CdlTkProcess.h>
#include <fstream>
#include <iostream>
#include <algorithm>

#include "LayoutCompilerErr.h"
#include "MLCompDataParse.h"
#include "MLAttributesParse.h"
#include "MLCompData2LayPerf.h"
#include "MLCompDataLayPerfWriter.h"

typedef LayoutProcessArgsErr<MLCompDataToLayPerf> MLCompDataToLayPerfArgsErr;

const string KCompDataFileNameSuffix("compData");
const string KAttributesFileNameSuffix("attributes");

//------------------------------------
// class MLCompDataToLayPerf
//------------------------------------
int MLCompDataToLayPerf::Process(const vector<string>& args)
	{
	if (args.size() != 6)
		throw MLCompDataToLayPerfArgsErr();

    int arg = 2;
	string cdlName = args[arg++];

	CCdlTkCdlFileParser parser(cdlName);
	auto_ptr<CCdlTkInterface> iface(parser.LoadAndParse(true));

	int numLayouts = args.size() - 4;
	if (numLayouts < 1)
		throw MLCompDataToLayPerfArgsErr();

	TMLCompData* mergedLayout = NULL;
	TMLAttributes* mergedAttribs = NULL;
    for(int ii = 0; ii < numLayouts; ii++)
        {
		string layoutName = args[arg++];
		string attribsName = CdlTkUtil::Replace(KCompDataFileNameSuffix, KAttributesFileNameSuffix, layoutName);

		auto_ptr<TMLCompDataParseLayout> layoutParse = TMLCompDataParseLayout::Parse(layoutName);
		auto_ptr<TMLCompData> layout(layoutParse.get());
		layoutParse.release();

		auto_ptr<TMLAttributesParse> attribsParse = TMLAttributesParse::Parse(attribsName);
		auto_ptr<TMLAttributes> attribs(attribsParse.get());
		attribsParse.release();

		if (mergedLayout || mergedAttribs)
			{
			// first we merge the components and the attributes
			mergedLayout->MergeComponents(*layout);
			mergedAttribs->Merge(*attribs);
			}
		else
			{
			// first time around
			mergedLayout = layout.get();
			mergedAttribs = attribs.get();
			}
        layout.release();
		attribs.release();
        }

	mergedLayout->iAttributes = mergedAttribs; // transfer ownership

	// once merged, we can compile the tables
    mergedLayout->Compile();


	string destLayout = args[arg++];
	MLCompDataToLayPerf layPerf(cdlName, *iface, *mergedLayout, destLayout);
	layPerf.WriteLayout();

	return 0;
	}

void MLCompDataToLayPerf::ShowHelp(ostream& stream)
	{
	stream << "MLCompData2LayPerf <layout.cdl> (<MLCompDataName>)+ <layPerf.cpp> " << endl;
	stream << "MLCompData2LayPerf ..\\cdl\\AknLayoutScalable_Avkon.cdl ..\\xml\\pdp_av_dbl_prt\\display_eur_compData.xml ..\\xml\\pdl_av_dbl_lsc\\display_eur_compData.xml ..\\generated\\LayPerf_AknLayoutScalable_Avkon.cpp " << endl;
	stream << "  This converts a CDL file into a specialised format for inclusion in the LayPerfScalable2 test app." << endl;
	stream << "  LayPerfScalable2 runs on the device, calling each API that would be generated from the CDL file." << endl;
	}

MLCompDataToLayPerf::MLCompDataToLayPerf(const string& aCdlName, CCdlTkInterface& aInterface, TMLCompData& aSourceLayout, const string& aDestLayoutName)
    :	
	iLayout(aSourceLayout),
	iDestLayoutName(aDestLayoutName),
	iCdlName(aCdlName),
	iInterface(aInterface)
	{
	}

void MLCompDataToLayPerf::WriteLayout()
	{
	TMLCompDataLayPerfWriter writer(iInterface, iLayout, iDestLayoutName);
	writer.Write(iCdlName);
	}

