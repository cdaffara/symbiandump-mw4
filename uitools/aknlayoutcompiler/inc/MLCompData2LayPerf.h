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


#ifndef MLCompData2LayPerf_H
#define MLCompData2LayPerf_H

#include <string>
#include <vector>
#include <iosfwd>
#include "MLCompData.h"
using namespace std;

#include <CdlCompilerToolkit/CdlTkInterface.h>
using namespace CdlCompilerToolkit;


/**
*  MLCompDataToLayPerf
*  Compiler mode for rescaling a layout
*/
class MLCompDataToLayPerf
	{
public:
	static int Process(const vector<string>& args);
	static void ShowHelp(ostream& stream);

private:
	MLCompDataToLayPerf(const string& aCdlName, CCdlTkInterface& aInterface, TMLCompData& aSourceLayout, const string& aDestLayoutName);
	void WriteLayout();

private:
	TMLCompData& iLayout;
	string iDestLayoutName;
	string iCdlName;
	CCdlTkInterface& iInterface; // not owned
	};


#endif // MLCompData2LayPerf_H
