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


#ifndef LAY2LAYPERF_H
#define LAY2LAYPERF_H

#include <string>
#include <vector>
#include <iosfwd>
#include "Layout.h"
using namespace std;

#include <CdlCompilerToolkit/CdlTkInterface.h>
using namespace CdlCompilerToolkit;


/**
*  LayoutToLayPerf
*  Compiler mode for rescaling a layout
*/
class LayoutToLayPerf
	{
public:
	static int Process(const vector<string>& args);
	static void ShowHelp(ostream& stream);
	static string InterfaceName(const string& aFileName);

private:
	LayoutToLayPerf(const string& aCdlName, TLayout& aSourceLayout, const string& aDestLayoutName);
	void WriteLayout();

private:
	TLayout& iLayout;
	string iDestLayoutName;
	string iCdlName;
	};


#endif // LAY2LAYPERF_H
