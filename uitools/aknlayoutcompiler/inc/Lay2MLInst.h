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

#ifndef LAY2MLINST_H
#define LAY2MLINST_H

#include <string>
#include <vector>
#include <iosfwd>
#include "Layout.h"
#include "MLInst.h"

using namespace std;

class LayoutToMLInst
	{
public:
	static int Process(const vector<string>& args);
	static void ShowHelp(ostream& stream);
	static string InterfaceName(const string& aFileName);
	static auto_ptr<TMLInst> LayoutToInst(const TLayout& aLayout);
	static void AddTableToInst(TMLInst& aInst, TLayoutTable& aTable);
	static void AddWindowLineToInstTable(TMLInstTable& aInstTable, TLayoutLine& aLine);
	static void AddTextLineToInstTable(TMLInstTable& aInstTable, TLayoutLine& aLine);
	static void AddParamToLine(TMLInstLine& aInstLine, TValues& aValue);
	};

#endif
