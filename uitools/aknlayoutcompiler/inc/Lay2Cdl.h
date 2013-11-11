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


#ifndef LAY2CDL_H
#define LAY2CDL_H

#include <string>
#include <vector>
#include <iosfwd>
#include <memory>
#include "Layout.h"
using namespace std;

#include <CdlCompilerToolkit/CdlTkInterface.h>
using namespace CdlCompilerToolkit;


/**
*  LayoutToCdl
*  Generate a CDL interface for a layout
*/

class LayoutToCdl
	{
public:
	static int Process(const vector<string>& args);
	static void ShowHelp(ostream& stream);
	static auto_ptr<CCdlTkInterface> LayoutToInterface(const TLayout& aLayout);
	static void AddTableToInterface(CCdlTkInterface& aInterface, TLayoutTable& aTable);
	static void AddWindowLineToInterface(CCdlTkInterface& aInterface, TLayoutLine& aLine);
	static void AddTextLineToInterface(CCdlTkInterface& aInterface, TLayoutLine& aLine);
	static string TableApiName(TLayoutTable& aTable, TLayoutTable::TLayoutSubTable& aSubTable, int aSubTableNum);
	static string LineApiName(TLayoutLine& aLine);
	static void AddTableLimitsToInterface(CCdlTkInterface& aInterface, TLayoutTable& aTable, TLayoutTable::TLayoutSubTable& aSubTable, int aSubTableNum);
	static void AddSubTableToInterface(const string& aType, CCdlTkInterface& aInterface, TLayoutTable& aTable, TLayoutTable::TLayoutSubTable& aSubTable, int aSubTableNum);
	static void AddParamsToFunc(TLayoutLine& aLine, CCdlTkFunctionApi& aFunc);
	static string InterfaceName(const string& aFileName);
	static void SetHeaders(CCdlTkInterface& aInterface, const string& aCdlName, int aUid);
private:
	static CCdlTkApi* CreateGenericApi(CCdlTkInterface& aInterface, TLayoutLine& aLine);
	};

#endif
