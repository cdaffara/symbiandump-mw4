/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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

#include "MLCompData2Cdl.h"
#include "MLCompDataParse.h"
#include "MLAttributesParse.h"
#include "LayoutCompilerErr.h"
#include <CdlCompilerToolkit/CdlTkProcess.h>
#include <fstream>
#include <iostream>
#include <algorithm>
#include "CppWriter.h"
#include "CodeGenConsts.h"

using namespace std;

typedef LayoutProcessArgsErr<MLCompDataToCdl> MLCompDataToCdlArgsErr;

// constants

const int KGeneratedInterfaceMajorVer = 1;
const int KGeneratedInterfaceMinorVer = 0;

const string KCompDataFileNameSuffix("compdata");
const string KAttributesFileNameSuffix("attributes");


// 
// MLCompDataToCdl
//
MLCompDataToCdl::CApiNamesUsed MLCompDataToCdl::iInterfaceNamesUsed;

string MLCompDataToCdl::InterfaceName(const string& aFileName)
	{
	return aFileName.substr(0,aFileName.find_last_of('.'));
	}

int MLCompDataToCdl::Process(const vector<string>& args)
	{
	iInterfaceNamesUsed.clear();

	unsigned int numExpectedArgs = 4;
	if(args.size() < numExpectedArgs)
		throw MLCompDataToCdlArgsErr();

    int arg = 2;
	bool deletesAllowed = true;
	if (args[arg] == "-nodeletes")
		{
		deletesAllowed = false;
		arg++;
		numExpectedArgs++;
		}

	bool allParams = false;
	if (args[arg] == "-allparams")
		{
		allParams = true;
		arg++;
		numExpectedArgs++;
		}

	bool romOnly = false;
	if (args[arg] == "-romonly")
		{
		romOnly = true;
		arg++;
		numExpectedArgs++;
		}

	int numLayouts = args.size() - numExpectedArgs;
	if (numLayouts < 1)
		throw MLCompDataToCdlArgsErr();

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

	string cdlName = args[arg++];
	int uid = CdlTkUtil::ParseInt(args[arg++]);

	auto_ptr<CCdlTkInterface> iface(new CCdlTkInterface);
	CCdlTkCdlFileParser parser(cdlName);
	try
		{
		iface = parser.LoadAndParse(true);
		}
	catch (const CdlTkFileOpenErr& aErr)
		{
		// there was no file, so presume that we are creating a new interface.
		aErr.Show(cerr);
		cout << "Creating new CDL API from scratch." << endl;
		}

	LayoutToInterface(*iface, *mergedLayout, deletesAllowed, allParams);

	stringstream comment;
	comment << "// Generated from ";

	// ignore the args up to and including the app name, the mode name, and the optional flags, 
    arg = numExpectedArgs-2; 

	// so that we get a list of processed files to output to the comment
    for(int ii = 0; ii < numLayouts; ii++)
		comment << args[arg++] << ", ";
    comment << endl; 
	comment << "// which was generated with timestamp " << mergedLayout->iTimestamp << endl;

	SetHeaders(*iface, cdlName, comment.str(), uid, romOnly);

	CCdlTkWriteCdlFile writer(*iface);
	cout << "writing CDL file " << cdlName << endl;
	writer.Process();

	iInterfaceNamesUsed.clear();
	return 0;
	}

void MLCompDataToCdl::ShowHelp(ostream& stream)
	{
	stream << "MLCompData2Cdl [-nodeletes] [-allparams] [-romonly] (<MLCompDataName>)+ <cdlName> <UID>" << endl;
	stream << "  If the CDL file does not exist, writes out the API in the order of the MLCompData." << endl;
	stream << "  If the CDL file does exist, and -nodeletes is not used, fills holes and appends new APIs to the end, to preseve API compatibility." << endl;
	stream << "  If the CDL file does exist, and -nodeletes is used, processing will halt and report an error if any of the existing APIs are missing." << endl;
	stream << "  If the CDL file does exist, existing APIs that do not have required params will be updated, appending missing params with default values." << endl;
	stream << "  If -allparams is used, all processed APIs will have all available params added, otherwise only needed params are added." << endl;
	stream << "  If -romonly is used, and if there is a previous CDL API it did not include the KCdlFlagRomOnly flag, then the flag will be added to the updated API." << endl;
	stream << "  If more than one layout is supplied, they are merged together, and then the interface of the merged data is generated. " << endl;
    stream << "  e.g. from \\S60\\AknLayout2\\group run " << endl;
    stream << "  aknlayoutcompiler MLCompData2CDL -nodeletes ..\\xml\\pdp_av_dbl_prt\\display_eur_compData.xml ..\\xml\\pdl_av_dbl_lsc\\display_eur_compData.xml ..\\cdl\\AknLayoutScalable_Avkon.cdl 0x1020384E" << endl;
    stream << "  aknlayoutcompiler MLCompData2CDL -nodeletes ..\\xml\\pdp_apps_dbl_prt\\display_eur_compData.xml ..\\xml\\pdl_apps_dbl_lsc\\display_eur_compData.xml ..\\cdl\\AknLayoutScalable_Apps.cdl 0x1020384F" << endl;
	}

void MLCompDataToCdl::LayoutToInterface(CCdlTkInterface& aInterface, const TMLCompData& aLayout, bool aDeletesAllowed, bool aAllParams)
	{
	const bool needsOptions = true;
	const bool needsColsRows = true;
	const bool doesntNeedOptions = false;
	const bool doesntNeedColsRows = false;

	AddGenericApiToInterface(aInterface, KTypeLayoutScalableComponentType, KFuncGetComponentTypeById, doesntNeedOptions, doesntNeedColsRows);
	AddGenericApiToInterface(aInterface, KTypeLayoutScalableParameterLimits, KFuncGetParamLimitsById, needsOptions, doesntNeedColsRows);
	AddGenericApiToInterface(aInterface, KTypeWindowComponentLayout, KFuncGetWindowComponentById, needsOptions, needsColsRows);
	AddGenericApiToInterface(aInterface, KTypeTextComponentLayout, KFuncGetTextComponentById, needsOptions, needsColsRows);

	for (TMLCompData::const_iterator pTab = aLayout.begin(); pTab != aLayout.end(); ++pTab)
		{
		AddTableToInterface(aInterface, **pTab, aAllParams);
		}
	ReplaceRemovedAPIs(aInterface, aLayout, aDeletesAllowed);
	CleanUpAPIComments(aInterface);
	}

void MLCompDataToCdl::AddGenericApiToInterface(CCdlTkInterface& aInterface, const string& aReturnType, const string& aName, bool aNeedsOptions, bool aRequiresColsRows)
	{
	bool isNew = false;
	const bool needsComponentId = true;
	const bool doesntNeedAllParams = false;
	CCdlTkFunctionApi* funcApi = ProcessFunctionApi(aInterface, aReturnType, aName, isNew);

	CCdlTkApiParams& params = funcApi->Params();
	CCdlTkApiParams oldParams = params;
	UpdateParams(oldParams, params, aName, doesntNeedAllParams, aRequiresColsRows, isNew, needsComponentId, aNeedsOptions, aRequiresColsRows, aRequiresColsRows);

	funcApi->SetComment(string("// Generic API\n"));
	}

void MLCompDataToCdl::AddTableToInterface(CCdlTkInterface& aInterface, TMLCompDataTable& aTable, bool aAllParams)
	{
	int line = 0;
	for (TMLCompDataTable::iterator pLine = aTable.begin(); pLine != aTable.end(); ++pLine)
		{
		// only add parameter limits if the line is not simple
		bool needsParamLimits = (*pLine)->NeedsOptions() || (*pLine)->NeedsCols() || (*pLine)->NeedsRows();
		if(needsParamLimits)
			{
			AddParamLimitsToInterface(aInterface, **pLine, aAllParams);
			}
		AddLineToInterface(aInterface, **pLine, aAllParams);

		for (TMLCompDataTable::TMLCompDataSubTables::const_iterator pSub = aTable.iSubTables.begin(); pSub != aTable.iSubTables.end(); ++pSub)
			{
			TMLCompDataTable::TMLCompDataSubTable& sub = **pSub;
			int last = *(sub.rbegin());
			if(line == last)
				{
				AddSubTableLimitsToInterface(aInterface, aTable, sub);
				if(needsParamLimits)
					AddSubTableParamLimitsToInterface(aInterface, aTable, sub, aAllParams);
				AddSubTableToInterface(aInterface, aTable, sub, aAllParams);
				}
			}

		line++;
		}
	}

void MLCompDataToCdl::AddTableCommentToApi(TMLCompDataTable& aTable, CCdlTkApi& aApi)
	{
	aApi.SetComment(string("// LAF Table : ") + aTable.Name() + "\n");
	}

void MLCompDataToCdl::AddTableCommentToApi(TMLCompDataLine& aLine, CCdlTkApi& aApi)
	{
	if(aLine.iParentTable)
		{
		TMLCompDataTable& table = *(aLine.iParentTable);
		AddTableCommentToApi(table, aApi);
		}
	}

void MLCompDataToCdl::AddLineToInterface(CCdlTkInterface& aInterface, TMLCompDataLine& aLine, bool aAllParams)
	{
	bool isNew = false;
	const bool requiresColsRows = true;
	CCdlTkFunctionApi* funcApi = ProcessFunctionApi(aInterface, ReturnType(aLine), LineApiName(aLine), isNew);
	AddParamsToFunc(aLine, *funcApi, aAllParams, isNew, requiresColsRows);
	AddTableCommentToApi(aLine, *funcApi);
	}

void MLCompDataToCdl::AddParamLimitsToInterface(CCdlTkInterface& aInterface, TMLCompDataLine& aLine, bool aAllParams)
	{
	bool isNew = false;
	const bool doesntRequireColsRows = false;
	CCdlTkFunctionApi* funcApi = ProcessFunctionApi(aInterface, KTypeLayoutScalableParameterLimits, LineParamLimitsApiName(aLine), isNew);
	AddParamsToFunc(aLine, *funcApi, aAllParams, isNew, doesntRequireColsRows);
	AddTableCommentToApi(aLine, *funcApi);
	}

void MLCompDataToCdl::AddSubTableToInterface(CCdlTkInterface& aInterface, TMLCompDataTable& aTable, TMLCompDataTable::TMLCompDataSubTable& aSubTable, bool aAllParams)
	{
	bool isNew = false;
	const bool requiresColsRows = true;
	TMLCompDataLine& line = *aTable[aSubTable[0]];
	CCdlTkFunctionApi* funcApi = ProcessFunctionApi(aInterface, ReturnType(line), SubTableApiName(aSubTable), isNew);
	AddParamsToFunc(aTable, aSubTable, *funcApi, aAllParams, isNew, requiresColsRows);
	AddTableCommentToApi(aTable, *funcApi);
	}

void MLCompDataToCdl::AddSubTableLimitsToInterface(CCdlTkInterface& aInterface, TMLCompDataTable& aTable, TMLCompDataTable::TMLCompDataSubTable& aSubTable)
	{
	bool isNew = false;
	CCdlTkFunctionApi* funcApi = ProcessFunctionApi(aInterface, KTypeLayoutScalableTableLimits, SubTableLimitsApiName(aSubTable), isNew);
	AddTableCommentToApi(aTable, *funcApi);
	}

void MLCompDataToCdl::AddSubTableParamLimitsToInterface(CCdlTkInterface& aInterface, TMLCompDataTable& aTable, TMLCompDataTable::TMLCompDataSubTable& aSubTable, bool aAllParams)
	{
	bool isNew = false;
	const bool doesntRequireColsRows = false;
	CCdlTkFunctionApi* funcApi = ProcessFunctionApi(aInterface, KTypeLayoutScalableParameterLimits, SubTableParamLimtsApiName(aSubTable), isNew);
	AddParamsToFunc(aTable, aSubTable, *funcApi, aAllParams, isNew, doesntRequireColsRows);
	AddTableCommentToApi(aTable, *funcApi);
	}

void MLCompDataToCdl::AddParamsToFunc(TMLCompDataLine& aLine, CCdlTkFunctionApi& aFunc, bool aAllParams, bool aIsNew, bool aColsRowsRequired)
	{
	CCdlTkApiParams& params = aFunc.Params();
	CCdlTkApiParams oldParams = params;

	const bool doesntNeedComponentId = false;
	bool options = aLine.NeedsOptions();
	bool cols = aLine.NeedsCols();
	bool rows = aLine.NeedsRows();
	UpdateParams(oldParams, params, LineApiName(aLine), aAllParams, aColsRowsRequired, aIsNew, doesntNeedComponentId, options, cols, rows);
	}

void MLCompDataToCdl::AddParamsToFunc(TMLCompDataTable& aTable, TMLCompDataTable::TMLCompDataSubTable& aSubTable, CCdlTkFunctionApi& aFunc, bool aAllParams, bool aIsNew, bool aColsRowsRequired)
	{
	CCdlTkApiParams& params = aFunc.Params();
	CCdlTkApiParams oldParams = params;

	if(aIsNew || params.FindByName(KParamLineIndex) == params.end())
		params.insert(params.begin(), CCdlTkApiParam(KTypeInt, KParamLineIndex));

	const bool doesntNeedComponentId = false;
	bool options = aSubTable.iNeedsOption;
	bool cols = aSubTable.iNeedsCol;
	bool rows = aSubTable.iNeedsRow;
	UpdateParams(oldParams, params, SubTableApiName(aSubTable), aAllParams, aColsRowsRequired, aIsNew, doesntNeedComponentId, options, cols, rows);
	}

void MLCompDataToCdl::SetHeaders(CCdlTkInterface& aInterface, const string& aCdlName, const string& aComment, int aUid, bool aRomOnly)
	{
	aInterface.SetFileName(aCdlName);
	aInterface.SetAdditionalComment(aComment);
	aInterface.Header().SetUid(aUid);

    // Note that if the rom only flag was already set in the existing API, then it will remain on
    // even if -romonly was not supplied as a command line argument. To remove the flag, 
    // the CDL file must be manually edited.
    if(aRomOnly)
    	aInterface.Header().Flags().SetFlag("KCdlFlagRomOnly");

	string ifName(InterfaceName(CdlTkUtil::StripPath(aCdlName)));
	aInterface.Header().SetName(ifName);
	aInterface.Header().SetVersion(CCdlTkInterfaceHeader::CVersion(KGeneratedInterfaceMajorVer, KGeneratedInterfaceMinorVer));

	CCdlTkCpp& cpp = aInterface.Cpp();
//	int size = cpp.size();
	if(!find(cpp.begin(), cpp.end(), KIncludeLayoutInstanceHeaderScalableDef))
		cpp.push_back(KIncludeLayoutInstanceHeaderScalableDef);
	}

string MLCompDataToCdl::SubTableApiName(TMLCompDataTable::TMLCompDataSubTable& aSubTable)
	{
	return CdlTkUtil::ToCpp(aSubTable.iName);
	}

string MLCompDataToCdl::SubTableLimitsApiName(TMLCompDataTable::TMLCompDataSubTable& aSubTable)
	{
	return CdlTkUtil::ToCpp(aSubTable.iName + KFuncLimitsSuffix);
	}

string MLCompDataToCdl::SubTableParamLimtsApiName(TMLCompDataTable::TMLCompDataSubTable& aSubTable)
	{
	return CdlTkUtil::ToCpp(aSubTable.iName + KFuncParamLimitsSuffix);
	}

string MLCompDataToCdl::LineApiName(TMLCompDataLine& aLine)
	{
	return CdlTkUtil::ToCpp(aLine.iName);
	}

string MLCompDataToCdl::LineParamLimitsApiName(TMLCompDataLine& aLine)
	{
	return CdlTkUtil::ToCpp(aLine.iName + KFuncParamLimitsSuffix);
	}

string MLCompDataToCdl::ReturnType(TMLCompDataLine& aLine)
	{
	string returnType = "BadType";
	switch(aLine.iType)
		{
		case TMLCompDataLine::EScreenComponent:
		case TMLCompDataLine::EContainerComponent:
		case TMLCompDataLine::EPaneComponent:
		case TMLCompDataLine::EGraphicComponent:
			{
			returnType = KTypeWindowComponentLayout;
			break;
			}
		case TMLCompDataLine::ETextComponent:
			{
			returnType = KTypeTextComponentLayout;
			break;
			}
		default:
			{
			throw GeneralErr(" MLCompDataToCdl::ReturnType - uncategorised component");
			break;
			}
		}

	return returnType;
	}

void MLCompDataToCdl::ReplaceRemovedAPIs(CCdlTkInterface& aInterface, const TMLCompData& aLayout, bool aDeletesAllowed)
	{
	bool error = false;
	// look for apis for which there is no equivalent internal structure. 
	CCdlTkApiList& apiList = aInterface.ApiList();
	for(CCdlTkApiList::reverse_iterator pApi = apiList.rbegin(); pApi != apiList.rend(); ++pApi)
		{
		CCdlTkApi*& api = *pApi;
		string apiName = api->Name();
		if(iInterfaceNamesUsed.find(apiName) == iInterfaceNamesUsed.end())
			{
			if(aDeletesAllowed)
				{
				delete api; // remove the missing api
				CCdlTkApi*& last = *(apiList.rbegin()); // get a reference to the pointer to the last element
				api = last; // copy the last element pointer to fill the gap

				// erasing the last element will not invalidate our reverse iterator
				// but we need a forward iterator to the last element, 
				// so take the address of our reference to the pointer
				apiList.erase(&last); 
				cout << "Replacing missing API: " << apiName << " <- " << api->Name() << endl;
				}
			else
				{
				error = true;
				cout << "Detected missing API: " << apiName << endl;
				}
			}
		}
	if(error)
		{
		throw GeneralErr("Error: -nodeletes was specified on command line, but some APIs were missing.");
		}
	}

void MLCompDataToCdl::CleanUpAPIComments(CCdlTkInterface& aInterface)
	{
	string currentComment;
	CCdlTkApiList& apiList = aInterface.ApiList();
	for(CCdlTkApiList::iterator pApi = apiList.begin(); pApi != apiList.end(); ++pApi)
		{
		CCdlTkApi& api = **pApi;
		string nextComment = api.Comment();
		if(nextComment == currentComment)
			api.SetComment(string());
		else
			currentComment = nextComment;
		}
	}

CCdlTkFunctionApi* MLCompDataToCdl::ProcessFunctionApi(CCdlTkInterface& aInterface, string aReturnType, string aName, bool& aIsNew)
	{
	iInterfaceNamesUsed.insert(aName);
	CCdlTkFunctionApi* funcApi;
	CCdlTkApi* api = aInterface.ApiList().Find(aName);
	if(api)
		{
		funcApi = static_cast<CCdlTkFunctionApi*>(api);
		UpdateFunctionApi(*funcApi, aInterface, aReturnType, aName);
		}
	else
		{
		funcApi = CreateFunctionApi(aInterface, aReturnType, aName);
		aIsNew = true;
		}
	funcApi->SetReturnType(aReturnType);
	funcApi->SetName(aName);
	return funcApi;
	}

CCdlTkFunctionApi* MLCompDataToCdl::CreateFunctionApi(CCdlTkInterface& aInterface, string aReturnType, string aName)
	{
	cout << "Adding new API: " << aName << endl;
	CCdlTkFunctionApi* api = new CCdlTkFunctionApi(aInterface);
	api->SetReturnType(aReturnType);
	api->SetName(aName);
	aInterface.ApiList().push_back(api);
	return api;	
	}

void MLCompDataToCdl::UpdateFunctionApi(CCdlTkFunctionApi& aApi, CCdlTkInterface& aInterface, string aReturnType, string aName)
	{
	aApi.SetReturnType(aReturnType);
	aApi.SetName(aName);
	}

void MLCompDataToCdl::UpdateParams(
	CCdlTkApiParams& aOldParams, 
	CCdlTkApiParams& aParams, 
	string aApiName, 
	bool aAllParamsRequested,
	bool aColsRowsRequired,
	bool aIsNew, 
	bool aNeedsComponentId,
	bool aNeedsOptions, 
	bool aNeedsCols, 
	bool aNeedsRows)
	{
	const bool allParamsDoesntApply = false;
	AddParamToParams(aOldParams, aParams, KParamComponentId, allParamsDoesntApply, aIsNew, aNeedsComponentId);
	AddParamToParams(aOldParams, aParams, KParamOptionIndex, aAllParamsRequested, aIsNew, aNeedsOptions);
	AddParamToParams(aOldParams, aParams, KParamColIndex, aAllParamsRequested && aColsRowsRequired, aIsNew, aNeedsCols && aColsRowsRequired);
	AddParamToParams(aOldParams, aParams, KParamRowIndex, aAllParamsRequested && aColsRowsRequired, aIsNew, aNeedsRows && aColsRowsRequired);

	if(aOldParams != aParams)
		cout << "Updating parameters: " << aApiName << endl;
	}

void MLCompDataToCdl::AddParamToParams(
	CCdlTkApiParams& aOldParams, 
	CCdlTkApiParams& aParams, 
	string aType, 
	bool aAllParamsRequested, 
	bool aIsNewLine, 
	bool aNeedsParam)
	{
	// only add new params if needed
	bool alreadyPresent = aOldParams.FindByName(aType) != aOldParams.end();
	if((aNeedsParam || aAllParamsRequested) && !alreadyPresent)
		{
		// the only circumstance in which we don't use a default parameter, 
		// is if we're adding a needed param to a new line
		bool needsDefaultValue = !(aIsNewLine && aNeedsParam);
		
		string defaultValue;
		if(aParams.size() != 0 && aParams.back().DefaultValue() == CdlTkUtil::IntToString(0))
		{
			defaultValue = CdlTkUtil::IntToString(0);
		}
		else
		{
			defaultValue = needsDefaultValue ? CdlTkUtil::IntToString(0) : string();
		}
		CCdlTkApiParam param(KTypeInt, aType, defaultValue);
		aParams.push_back(param);
	}
	}

