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
*
*/


#include "MLCompDataLayPerfWriter.h"
#include "MLCompData2Cdl.h" // for static methods
#include "LayoutCompilerErr.h"
#include "CodeGenConsts.h"

#include <CdlCompilerToolkit/CdlTkInterface.h>
#include <CdlCompilerToolkit/CdlTkUtil.h>

#include <iostream>
#include <sstream>
#include <set>
#include <fstream>
#include <algorithm>

using namespace std;
using namespace CdlCompilerToolkit;

const string KComma = ",";
const string KSpace = " ";
const string KMacroNameRoot = "LAYOUT_TEST";
const string KMacroNameWindow = "_WINDOW";
const string KMacroNameText = "_TEXT";
const string KMacroNameComponent = "_COMPONENT";
const string KMacroNameParams = "_PARAMS";

//
// TMLCompDataLayPerfTableWriter
//

TMLCompDataLayPerfTableWriter::TMLCompDataLayPerfTableWriter(TMLCompDataLayPerfWriter* aParent, TMLCompDataTable& aTable, string& aInterfaceName, int aTableId) 
	: 
	iParent(aParent),
	iTable(aTable),
	iInterfaceName(aInterfaceName),
	iTableId(aTableId)
	{
	}

TMLCompDataLayPerfTableWriter::~TMLCompDataLayPerfTableWriter()
	{
	}

void TMLCompDataLayPerfTableWriter::Write(ostream& out)
	{
	WriteTable(out);
	}


const string KTestAPITableFuntionSig = "\
testLayout_$LAYOUT_$TABLEID()";

string GenerateMLFunctionName(string aInterfaceName, int aTableId)
	{
	string tableNum = CdlTkUtil::IntToString(aTableId);

	CdlTkUtil::CReplaceSet rep;
	rep.Add("$LAYOUT", aInterfaceName);
	rep.Add("$TABLEID", tableNum);
	return CdlTkUtil::MultiReplace(rep, KTestAPITableFuntionSig);
	}

void TMLCompDataLayPerfTableWriter::WriteTable(ostream& out)
	{
	out << "void " << GenerateMLFunctionName(iInterfaceName, iTableId) << endl;

	out << "{" << endl; // start of function
	out << "DECLARE_LOCAL_VARS_COUNTS" << endl;
	
	out << "\n// Layout MACROs for Layout Table : ";
	out << iTable.Name() << endl;

	for (unsigned int i=0; i<iTable.size(); ++i)
		{
		WriteLine(out, *iTable[i]);
		}

	out << endl;
	out << "}\n" << endl; // end of function
	}

void TMLCompDataLayPerfTableWriter::WriteLine(ostream& out, TMLCompDataLine& aLine)
	{
	string macroName = BuildMacroName(aLine);
	string apiName = BuildApiName(aLine);
	string params = BuildParams(aLine);
	string paramLimitParams = BuildParamLimitParams(aLine);

	out << macroName << "( "; 

	if(aLine.NeedsParams())		
		{
		out << 	apiName << KFuncParamLimitsSuffix << "(" << paramLimitParams << ")";
		out << ", ";
		}

	out << 	apiName << "(" << params << ")";
	out << " )" << endl; // this ends the macro call
	out << endl;
	}

string TMLCompDataLayPerfTableWriter::BuildMacroName(TMLCompDataLine& aLine)
	{
	string macroName = KMacroNameRoot;
	macroName += (aLine.iType == TMLCompDataLine::ETextComponent) ? KMacroNameText : KMacroNameWindow;
	macroName += KMacroNameComponent;
	if(aLine.NeedsParams())		
		macroName += KMacroNameParams;
	return macroName;
	}

string TMLCompDataLayPerfTableWriter::BuildApiName(TMLCompDataLine& aLine)
	{
	string apiName = iInterfaceName;
	apiName += string("::");
	apiName += MLCompDataToCdl::LineApiName(aLine);
	return apiName;
	}

string TMLCompDataLayPerfTableWriter::BuildParams(TMLCompDataLine& aLine)
	{
	string paramsString;
	string name = aLine.Name();

	// add the params in the order of the CDL interface
	CCdlTkApi* api = iParent->iInterface.ApiList().Find(name);
	if(!api)
		throw NotFoundErr(name + " in interface " + iParent->iInterface.FileName());
	CCdlTkFunctionApi* funcApi = static_cast<CCdlTkFunctionApi*>(api);
	CCdlTkApiParams& params = funcApi->Params();
	bool first = true;
	for(CCdlTkApiParams::iterator pParam = params.begin(); pParam != params.end(); ++pParam)
		{
		if(!first)
			{
			paramsString.append(KComma); 
			paramsString.append(KSpace);
			}
		paramsString.append(pParam->Name());
		first = false;
		}
	return paramsString;
	}

string TMLCompDataLayPerfTableWriter::BuildParamLimitParams(TMLCompDataLine& aLine)
	{
	string paramsString;
	string name = aLine.Name();

	CCdlTkApi* api = iParent->iInterface.ApiList().Find(name);
	if(!api)
		throw NotFoundErr(name + " in interface " + iParent->iInterface.FileName());
	CCdlTkFunctionApi* funcApi = static_cast<CCdlTkFunctionApi*>(api);
	CCdlTkApiParams& params = funcApi->Params();

	CCdlTkApiParams::iterator pParam = params.FindByName(KParamOptionIndex);
	if(pParam != params.end() && aLine.NeedsOptions())
		{
		paramsString.append(pParam->Name());
		}

	return paramsString;
	}


//
// TMLCompDataLayPerfWriter
//

TMLCompDataLayPerfWriter::TMLCompDataLayPerfWriter(CCdlTkInterface& aInterface, TMLCompData& aLayout, const std::string& aName)
	:
	TMLWriterBase<TMLCompData>(aLayout, aName),
	iInterface(aInterface)
	{
	}

void TMLCompDataLayPerfWriter::Write(const std::string& aCdlName)
	{
	ofstream out(iName.c_str());

	cout << "writing layout " << iName << endl;
	string cdlFileName(CdlTkUtil::StripPath(aCdlName));
	string ifName(iInterface.Header().Name());

	out << "// function implementations: " << endl;
	unsigned int tableId = 0;
	for (TMLCompData::iterator pTab = iLayout.begin(); pTab != iLayout.end(); ++pTab)
		{
		TMLCompDataLayPerfTableWriter writer(this, **pTab, ifName, tableId++);
		writer.Write(out);
		}

	out << "void testLayout_" << ifName << "()\n{" << endl;
	tableId = 0;
	for(;tableId < iLayout.size(); tableId++)
		{
		out << GenerateMLFunctionName(ifName, tableId) << ";" << endl;
		}
	out << "}\n" << endl;

	out.close();
	}

// End of File
