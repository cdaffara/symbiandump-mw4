/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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


#include "LayPerfWriter.h"
#include "Lay2Cdl.h" // for static methods
#include "Layout.h" // for constants
#include "CodeGenConsts.h"

#include <CdlCompilerToolkit/CdlTkUtil.h>

#include <iostream>
#include <sstream>
#include <set>
#include <fstream>
#include <algorithm>

using namespace std;
using namespace CdlCompilerToolkit;


//
// TLayPerfTableWriter
//

TLayPerfTableWriter::TLayPerfTableWriter(TLayoutTable& aTable, string& aInterfaceName, int aTableId) 
	: 
	iTable(aTable),
	iInterfaceName(aInterfaceName),
	iTableId(aTableId)
	{
	}

TLayPerfTableWriter::~TLayPerfTableWriter()
	{
	}

void TLayPerfTableWriter::Write(ostream& out)
	{
	if (IsWindowTable())
		{
		WriteWindowTable(out);
		}
	else if (IsTextTable())
		{
		WriteTextTable(out);
		}
	else
		{
		cout << "Unknown " << "Table " << iTable.Name() << endl;
		}
	}

bool TLayPerfTableWriter::IsWindowTable()
	{
	return iTable.iType == TLayoutTable::EWindowTable;
	}


const string KTestAPITableFuntionSig = "\
testLayout_$LAYOUT_$TABLEID()";

string GenerateFunctionName(string aInterfaceName, int aTableId)
	{
	string tableNum = CdlTkUtil::IntToString(aTableId);

	CdlTkUtil::CReplaceSet rep;
	rep.Add("$LAYOUT", aInterfaceName);
	rep.Add("$TABLEID", tableNum);
	return CdlTkUtil::MultiReplace(rep, KTestAPITableFuntionSig);
	}

void TLayPerfTableWriter::WriteWindowTable(ostream& out)
	{
	out << "void " << GenerateFunctionName(iInterfaceName, iTableId) << endl;

	out << "{" << endl; // start of function
	out << "DECLARE_LOCAL_VARS_COUNTS" << endl;
	out << "DECLARE_LOCAL_VARS_LIMITS" << endl;

	out << "\n// Layout MACROs for LAF Table : ";
	out << iTable.Name() << endl;

	for (unsigned int i=0; i<iTable.size(); ++i)
		{
		WriteWindowLine(out, *iTable[i]);
		}

	out << endl;
	out << "}\n" << endl; // end of function
	}

void TLayPerfTableWriter::WriteWindowLine(ostream& out, TLayoutLine& aLine)
	{
	string params;
	string limits;

	bool mirrored = aLine.iIsMirroredHorizontally;
	const string* KParams = mirrored ? KWindowOutputOrderMirrored : KWindowOutputOrder;
	int count = KWindowOutputOrderSize;

	BuildGenericLine(aLine, KParams, count, params, limits);

	out << limits << endl; 

	out << "LAYOUT_TEST_WINDOW_LINE( "; // macro name
	out << iInterfaceName << "::"; // namespace name
	out << LayoutToCdl::LineApiName(aLine); // api name
	out << "(";

	out << params << " )";
	out << " )" << endl; // this ends the macro call
	}

void TLayPerfTableWriter::BuildGenericLine(TLayoutLine& aLine, const string* aKParams, int aCount, string& aParams, string& aLimits)
	{
	vector<string> paramList;
	bool mirrored = aLine.iIsMirroredHorizontally;

	if (aLine.iNeedsP)
		{
		aParams += "aParent";
		}

	if (aLine.iNeedsIndex)
		{
		for (int i=0; i < aCount; i++)
			{
			const string& name = aKParams[i];
			const TValues& defValues = aLine[name];
			if (defValues.iNeedsIndex)
				{
				string paramName = CdlTkUtil::ToCpp(defValues.ParamName());
				if (mirrored)
					{
					if (paramName == KParamNameL)
						paramName = KParamNameR;
					else if (paramName == KParamNameR)
						paramName = KParamNameL;
					}

				if(find(paramList.begin(), paramList.end(), paramName) == paramList.end())
					{
					if(aParams.size() > 0)
						aParams += ", ";
					aParams += paramName;
					paramList.push_back(paramName);

					aLimits += paramName + "_Limit" + " = ";
					aLimits += CdlTkUtil::IntToString(defValues.size() - 1) + "; "; // subtract one from the max value to give the upper limit
					}
				}
			}
		}
	}

void TLayPerfTableWriter::WriteCell(ostream& out, TValues& values)
	{
	if (values.size() > 1)
		out << "{";
	
	for (TValues::iterator pVal = values.begin(); pVal != values.end(); ++pVal)
		{
		if (pVal != values.begin())
			out << ", ";
		out << *pVal;
		}

	if (values.size() > 1)
		out << "}";

	if (values.iParam.length())
		out << "[" << values.iParam << "]";
	}


bool TLayPerfTableWriter::IsTextTable()
	{
	return iTable.iType == TLayoutTable::ETextTable;
	}

void TLayPerfTableWriter::WriteTextTable(ostream& out)
	{
	out << "void " << GenerateFunctionName(iInterfaceName, iTableId) << endl;

	out << "{" << endl; // start of function
	out << "DECLARE_LOCAL_VARS_COUNTS" << endl;
	out << "DECLARE_LOCAL_VARS_LIMITS" << endl;

	out << "// Layout MACROs for LAF Table : ";
	out << iTable.Name() << endl;

	for (unsigned int i=0; i<iTable.size(); ++i)
		{
		WriteTextLine(out, *iTable[i]);
		}

	out << endl;
	out << "}\n" << endl; // end of function
	}

void TLayPerfTableWriter::WriteTextLine(ostream& out, TLayoutLine& aLine)
	{
	string params;
	string limits;

	bool mirrored = aLine.iIsMirroredHorizontally;
	const string* KParams = mirrored ? KTextOutputOrderMirrored : KTextOutputOrder;
	int count = KTextOutputOrderSize;

	BuildGenericLine(aLine, KParams, count, params, limits);

	out << limits << endl; 

	out << "LAYOUT_TEST_TEXT_LINE( "; // macro name
	out << iInterfaceName << "::"; // namespace name
	out << LayoutToCdl::LineApiName(aLine); // api name
	out << "(";

	out << params << " )";
	out << " )" << endl; // this ends the macro call
	}


//
// TLayPerfWriter
//

TLayPerfWriter::TLayPerfWriter(TLayout& aLayout, const std::string& aName)
: TLayWriterBase(aLayout, aName)
	{
	}

void TLayPerfWriter::Write(const std::string& aCdlName)
	{
	ofstream out(iName.c_str());

	cout << "writing layout " << iName << endl;
	string cdlFileName(CdlTkUtil::StripPath(aCdlName));
	string ifName(LayoutToCdl::InterfaceName(cdlFileName));

	out << "// function implementations: " << endl;
	unsigned int tableId = 0;
	for (TLayout::iterator pTab = iLayout.begin(); pTab != iLayout.end(); ++pTab)
		{
		TLayPerfTableWriter writer(**pTab, ifName, tableId++);
		writer.Write(out);
		}

	out << "void testLayout_" << ifName << "()\n{" << endl;
	tableId = 0;
	for(;tableId < iLayout.size(); tableId++)
		{
		out << GenerateFunctionName(ifName, tableId) << ";" << endl;
		}
	out << "}\n" << endl;


	out.close();
	}

// End of File
