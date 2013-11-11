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


#ifndef MLCOMPDATA2CDL_H
#define MLCOMPDATA2CDL_H

#include <string>
#include <vector>
#include <iosfwd>
#include "MLCompData.h"
using namespace std;

#include <CdlCompilerToolkit/CdlTkInterface.h>
using namespace CdlCompilerToolkit;


/**
*  MLCompDataToCdl
*  Generate a CDL interface for a layout
*/
class MLCompDataToCdl
	{
public:
	static int Process(const vector<string>& args);
	static void ShowHelp(ostream& stream);
	static void LayoutToInterface(CCdlTkInterface& aInterface, const TMLCompData& aLayout, bool aDeletesAllowed, bool aAllParams);
	static void AddGenericApiToInterface(CCdlTkInterface& aInterface, const string& aReturnType, const string& aName, bool aNeedsOptions, bool aNeedsColsRows);
	static void AddTableToInterface(CCdlTkInterface& aInterface, TMLCompDataTable& aTable, bool aAllParams);
	static void AddTableCommentToApi(TMLCompDataTable& aTable, CCdlTkApi& aApi);
	static void AddTableCommentToApi(TMLCompDataLine& aLine, CCdlTkApi& aApi);
	static void AddLineToInterface(CCdlTkInterface& aInterface, TMLCompDataLine& aLine, bool aAllParams);
	static void AddParamLimitsToInterface(CCdlTkInterface& aInterface, TMLCompDataLine& aLine, bool aAllParams);
	static void AddSubTableToInterface(CCdlTkInterface& aInterface, TMLCompDataTable& aTable, TMLCompDataTable::TMLCompDataSubTable& aSubTable, bool aAllParams);
	static void AddSubTableLimitsToInterface(CCdlTkInterface& aInterface, TMLCompDataTable& aTable, TMLCompDataTable::TMLCompDataSubTable& aSubTable);
	static void AddSubTableParamLimitsToInterface(CCdlTkInterface& aInterface, TMLCompDataTable& aTable, TMLCompDataTable::TMLCompDataSubTable& aSubTable, bool aAllParams);
	static void AddParamsToFunc(TMLCompDataLine& aLine, CCdlTkFunctionApi& aFunc, bool aAllParams, bool aIsNew, bool aRowsCols);
	static void AddParamsToFunc(TMLCompDataTable& aTable, TMLCompDataTable::TMLCompDataSubTable& aSubTable, CCdlTkFunctionApi& aFunc, bool aAllParams, bool aIsNew, bool aRowsCols);
	static void AddParamToParams(CCdlTkApiParams& aOldParams, CCdlTkApiParams& aParams, string aType, bool aAllParams, bool aIsNewLine, bool aNeedsParam);
	static void SetHeaders(CCdlTkInterface& aInterface, const string& aCdlName, const string& aComment, int aUid, bool aRomOnly);
	static string InterfaceName(const string& aFileName);
	static string SubTableApiName(TMLCompDataTable::TMLCompDataSubTable& aSubTable);
	static string SubTableLimitsApiName(TMLCompDataTable::TMLCompDataSubTable& aSubTable);
	static string SubTableParamLimtsApiName(TMLCompDataTable::TMLCompDataSubTable& aSubTable);
	static string LineApiName(TMLCompDataLine& aLine);
	static string LineParamLimitsApiName(TMLCompDataLine& aLine);
private:
	static string ReturnType(TMLCompDataLine& aLine);
	static void ReplaceRemovedAPIs(CCdlTkInterface& aInterface, const TMLCompData& aLayout, bool aDeletesAllowed);
	static void CleanUpAPIComments(CCdlTkInterface& aInterface);
	static CCdlTkFunctionApi* ProcessFunctionApi(CCdlTkInterface& aInterface, string aReturnType, string aName, bool& aIsNew);
	static CCdlTkFunctionApi* CreateFunctionApi(CCdlTkInterface& aInterface, string aReturnType, string aName);
	static void UpdateFunctionApi(CCdlTkFunctionApi& aApi, CCdlTkInterface& aInterface, string aReturnType, string aName);
	static void UpdateParams(CCdlTkApiParams& aOldParams, CCdlTkApiParams& aParams, string aApiName, bool aAllParamsRequested, bool aColsRowsRequired, bool aIsNew, bool aNeedsComponentId, bool aNeedsOptions, bool aNeedsCols, bool aNeedsRows);
private:
	typedef set<string> CApiNamesUsed;
	static CApiNamesUsed iInterfaceNamesUsed;
	};

#endif
