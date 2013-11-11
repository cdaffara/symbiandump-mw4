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
*
*/


#include "CppWriter.h"
#include "LayoutCompilerErr.h"
#include "CodeGenConsts.h"
#include "UsefulDefinitions.h"

#include <CdlCompilerToolkit/CdlTkUtil.h>

#include <fstream>
#include <algorithm>
#include <iostream> // !!! for debug output only

using namespace CdlCompilerToolkit;

string whiteSpace = " \t\r\n";

const string KTextOutputOrder[] = {"C", "l", "r", "B", "W", "J", "Font"};
const string KTextOutputOrderMirrored[] = {"C", "r", "l", "B", "W", "J", "Font"};
const int KTextOutputOrderSize = ARRAY_LEN(KTextOutputOrder);

const string KWindowOutputOrder[] = {"C", "l", "t", "r", "b", "W", "H"};
const string KWindowOutputOrderMirrored[] = {"C", "r", "t", "l", "b", "W", "H"};
const int KWindowOutputOrderSize = ARRAY_LEN(KWindowOutputOrder);

// class TValuesCppWriter
void TValuesCppWriter::WriteCpp(ostream& aCpp)
	{
	for (TValues::const_iterator it = iValues.begin(); it != iValues.end(); ++it)
		{
		if (it != iValues.begin())
			aCpp << ", ";
		aCpp << TValues::CppValue(*it);
		}
	}


// class TLayoutLineCppWriter

void TLayoutLineCppWriter::WriteCpp(ostream& aCpp)
	{
	// write line structure
	WriteLineClassName(aCpp);
	aCpp << Name() << " = { ";
	WriteCppData(aCpp);
	aCpp << " };" << endl << endl;
	}


void TLayoutLineCppWriter::WriteCppData(ostream& aCpp)
	{
	if (iLine.iNeedsIndex)
		WriteVarCppData(aCpp);
	else
		WriteFixedCppData(aCpp);
	}


int TLayoutLineCppWriter::WriteParamList(ostream& aLag, const vector<string>& aParams)
	{
	int written = 0;
	if (aParams.size() > 0)
		{
		written = 2*aParams.size();
		aLag << "(";
		for (vector<string>::const_iterator pParam = aParams.begin(); pParam != aParams.end(); ++pParam)
			{
			if (pParam != aParams.begin())
				aLag << ", ";
			aLag << *pParam;
			written += pParam->size();
			}
		aLag << ")";
		}
	return written;
	}

int TLayoutLineCppWriter::ValCount()
	{
	int count = 0;
	for (TLayoutLine::const_iterator pVal = iLine.begin(); pVal != iLine.end(); ++pVal)
		{
		if (TLayoutTable::IsValueColumn(pVal->first))
			count += pVal->second.size();
		}
	return count;
	}


// class TWindowLineCppWriter

string TWindowLineCppWriter::Name()
	{
	if (iLine.iIsUnique)
		return CdlTkUtil::ToCpp(iLine.Name());
	else
		return CdlTkUtil::ToCpp(iLine.TableName());
	}

void TWindowLineCppWriter::WriteLineClassName(ostream& aCpp)
	{
	if (iLine.iNeedsIndex)
		{
		aCpp << "const SVarWindowLine<" << ValCount() << "> ";
		}
	else
		{
		aCpp << "const SFixedWindowLine ";
		}
	}

void TWindowLineCppWriter::WriteVarCppData(ostream& aCpp)
	{
	aCpp << " { "; // suppress the first offset

	int offset = 0;
	for (int i=0; i<KWindowOutputOrderSize-1; i++) // Last offset is not used
		{
		offset += iLine[KWindowOutputOrder[i]].size();
		if ( i != 0 ) 
			aCpp << ", ";
		aCpp << offset;
		}

	aCpp << " }, { ";

	WriteFixedCppData(aCpp);

	aCpp << " }";
	}

void TWindowLineCppWriter::WriteFixedCppData(ostream& aCpp)
	{
	for (int i=0; i<KWindowOutputOrderSize; i++)
		{
		if (i>0)
			aCpp << ", ";
		TValuesCppWriter writer(iLine[KWindowOutputOrder[i]]);
		writer.WriteCpp(aCpp);
		}
	}

void TWindowLineCppWriter::FillParamLists(vector<string>& aDef, vector<string>& aCtor)
	{
	if (iLine.iNeedsP)
		{
		aDef.push_back(KParamParentRect);
		aCtor.push_back(KParamParentRect);
		}

	if (iLine.iNeedsIndex)
		{
		aCtor.push_back(CdlTkUtil::IntToString(iLine.iIsMirroredHorizontally));

		for (int i=0; i<KWindowOutputOrderSize; i++)
			{
			// need to worry about parameter mirroring here. The problem is that 
			// the macro needs to have a fixed parameter ordering, but the parameters
			// might vary between variants, therefore the order of mirrored parameters 
			// may vary, e.g. (l, t) becomes (t, r) etc. Solution is to swap l and r if the line is mirrored
			// just for the macro definition (not for the ctor). 
			string name = KWindowOutputOrder[i];
			string nameMirrored = KWindowOutputOrderMirrored[i];

			const TValues& defValues = iLine[iLine.iIsMirroredHorizontally ? nameMirrored : name];
			if (defValues.iNeedsIndex)
				{
				string param = CdlTkUtil::ToCpp(defValues.ParamName());
				if (find(aDef.begin(), aDef.end(), param) == aDef.end()) // param Only written once to Macro
					{
					aDef.push_back(param);
					}
				}

			const TValues& ctorValues = iLine[name];
			if (ctorValues.iNeedsIndex)
				{
				string param = CdlTkUtil::ToCpp(ctorValues.ParamName());
				aCtor.push_back(param);
				}
			else
				{
				aCtor.push_back("0");
				}
			}
		}
	}

void TWindowLineCppWriter::WriteMacro(ostream& aLag)
	{
	vector<string> def_params;
	vector<string> ctor_params;
	ctor_params.push_back(CdlTkUtil::IntToString(iLine.iGlobalIndex));

	FillParamLists(def_params, ctor_params);

	aLag << "#define AKN_LAYOUT_WINDOW_" << Name();
	WriteParamList(aLag, def_params);
	aLag << "\t" << KTypeWindowLineLayout;		
	WriteParamList(aLag, ctor_params);
	aLag << endl;
	}


// class TTextLineCppWriter

string TTextLineCppWriter::Name()
	{
	return CdlTkUtil::ToCpp(iLine.TableName());
	}

void TTextLineCppWriter::FillParamLists(vector<string>& aDef, vector<string>& aCtor)
	{
	iBaseLineVariation = false;

	if (iLine.iNeedsP)
		{
		aDef.push_back(KParamParentRect);
		aCtor.push_back(KParamParentRect);
		}

	if (iLine.iNeedsIndex)
		{
		aCtor.push_back(CdlTkUtil::IntToString(iLine.iIsMirroredHorizontally));

		for (int i=0; i<KTextOutputOrderSize; i++)
			{
			// need to worry about parameter mirroring here. The problem is that 
			// the macro needs to have a fixed parameter ordering, but the parameters
			// might vary between variants, therefore the order of mirrored parameters 
			// may vary, e.g. (l, t) becomes (t, r) etc. Solution is to swap l and r if the line is mirrored
			// just for the macro definition (not for the ctor). 
			string name = KTextOutputOrder[i];
			string nameMirrored = KTextOutputOrderMirrored[i];

			const TValues& defValues = iLine[iLine.iIsMirroredHorizontally ? nameMirrored : name];
			if (defValues.iNeedsIndex)
				{
				string param = CdlTkUtil::ToCpp(defValues.ParamName());
				if (find(aDef.begin(), aDef.end(), param) == aDef.end()) // uniqueness check
					{
					aDef.push_back(param);
					}

				// record whether some of the variation comes from the Baseline column
				if ( defValues.iName == "B")
					iBaseLineVariation = true;	
				}

			const TValues& ctorValues = iLine[name];
			if (ctorValues.iNeedsIndex)
				{
				string param = CdlTkUtil::ToCpp(ctorValues.ParamName());
				aCtor.push_back(param);
				}
			else
				{
				aCtor.push_back("0");
				}
			}
		}
	}

void TTextLineCppWriter::WriteMacro(ostream& aLag)
	{
	vector<string> def_params;
	vector<string> ctor_params;
	ctor_params.push_back(CdlTkUtil::IntToString(iLine.iGlobalIndex));

	FillParamLists(def_params, ctor_params);

	aLag << "#define AKN_LAYOUT_TEXT_" << Name();
	WriteParamList(aLag, def_params);
	aLag << "\t" << KTypeTextLineLayout;
	WriteParamList(aLag, ctor_params);
	aLag << endl;

// Also write out a multiline_text if there are more than one Baselines supplied.  Number of lines is always user-supplied
	vector<string> multiline_def_params;
	vector<string> multiline_ctor_params;

	if ( iBaseLineVariation )
		{
		multiline_ctor_params.push_back(CdlTkUtil::IntToString(iLine.iGlobalIndex));

		if (iLine.iNeedsP)
			{
			multiline_def_params.push_back(KParamParentRect);
			multiline_ctor_params.push_back(KParamParentRect);
			}

		if (iLine.iNeedsIndex) // this condition has already been tested when setting iBaseLineVariation, but check again here for completeness
			{
			multiline_ctor_params.push_back(CdlTkUtil::IntToString(iLine.iIsMirroredHorizontally));
			}

			for (int i=0; i<KTextOutputOrderSize; i++)
				{
				// need to worry about parameter mirroring here. The problem is that 
				// the macro needs to have a fixed parameter ordering, but the parameters
				// might vary between variants, therefore the order of mirrored parameters 
				// may vary, e.g. (l, t) becomes (t, r) etc. Solution is to swap l and r if the line is mirrored
				// just for the macro definition (not for the ctor). 
				string name = KTextOutputOrder[i];
				string nameMirrored = KTextOutputOrderMirrored[i];

				const TValues& defValues = iLine[iLine.iIsMirroredHorizontally ? nameMirrored : name];
				if (defValues.iNeedsIndex) 
					{
					if (defValues.iName != "B") 
						{
						string param = CdlTkUtil::ToCpp(defValues.ParamName());
						if (find(multiline_def_params.begin(), multiline_def_params.end(), param) == multiline_def_params.end())
							{
							multiline_def_params.push_back(param);
							}
// A variable parameter that is tied to the baseline is NOT suppressed in the Multiline
// You have to supply the parameter in the macro as a zero
						}
					}

				const TValues& ctorValues = iLine[name];
				if (ctorValues.iNeedsIndex)
					{
					if (defValues.iName != "B") 
						{
						string param = CdlTkUtil::ToCpp(ctorValues.ParamName());
						multiline_ctor_params.push_back(param);
// A variable parameter that is tied to the baseline is NOT suppressed in the Multiline
// You have to supply the parameter in the macro as a zero
						}
					else
						{
						multiline_ctor_params.push_back("0");
						}
					}
				else
					{
					multiline_ctor_params.push_back("0");
					}

				} // end of for loop

		// The final param for the multiline macro is user input for the layout utilities (pass-through data)
		multiline_def_params.push_back(KParamNameNumberOfLinesShown);
		multiline_ctor_params.push_back(KParamNameNumberOfLinesShown);

		aLag << "#define AKN_LAYOUT_MULTILINE_TEXT_" << Name();
		WriteParamList(aLag, multiline_def_params);
		aLag << "\t" + KTypeMultiLineTextLayout;
		WriteParamList(aLag, multiline_ctor_params);
		aLag << endl;
		} // end of if on baseline_variation
	}


void TTextLineCppWriter::WriteLineClassName(ostream& aCpp)
	{
	if (iLine.iNeedsIndex)
		{
		aCpp << "const SVarTextLine<" << ValCount()  << "> ";
		}
	else
		{
		aCpp << "const SFixedTextLine ";
		}
	}

void TTextLineCppWriter::WriteVarCppData(ostream& aCpp)
	{
	aCpp << " { ";  // ! Do not write out the first 0

	int offset = 0;
	for (int i=0; i<KTextOutputOrderSize-1; i++) // last offset IS used because font id is written out
		{
		offset += iLine[KTextOutputOrder[i]].size();
		if ( i != 0 )
			aCpp << ", "; 
		aCpp << offset;
		}

	aCpp << " }, { ";

	WriteFixedCppData(aCpp);

	aCpp << " }";
	}

void TTextLineCppWriter::WriteFixedCppData(ostream& aCpp)
	{
	for (int i=0; i<KTextOutputOrderSize; i++)
		{
		if ( i > 0 )
			aCpp << ", ";
		TValuesCppWriter writer(iLine[KTextOutputOrder[i]]);
		writer.WriteCpp(aCpp);
		}
	}

void TTextLineCppWriter::WriteMultiLineTextParamList(ostream& aLag, const vector<string>& aParams)
	{
	if (aParams.size() > 0)
		{
		aLag << "(";
		for (vector<string>::const_iterator pParam = aParams.begin(); pParam != aParams.end(); ++pParam)
			{
			if (pParam != aParams.begin())
				aLag << ", ";
			aLag << *pParam;
			}
		aLag << ", " << KParamNameNumberOfLinesShown;
		aLag << ")";
		}
	}


// class TLayoutTableCppWriter

void TLayoutTableCppWriter::WriteCppLineData(ostream& aCpp)
	{
	for (TLayoutTable::iterator it = iTable.begin(); it != iTable.end(); ++it)
		{
		TLayoutLineCppWriter* writer = CreateLayoutLineCppWriter(**it, iTable);
		writer->WriteCpp(aCpp);
		delete writer;
		}
	}

void TLayoutTableCppWriter::WriteCppTableData(ostream& aCpp)
	{
	}

void TLayoutTableCppWriter::WriteLag(ostream& aLag)
	{
	aLag << endl;
	int count=0;
	for (TLayoutTable::TLayoutSubTables::iterator it = iTable.iSubTables.begin(); it != iTable.iSubTables.end(); ++it)
		{
		WriteLagSubTable(**it, count++, aLag);
		}
	}


void TLayoutTableCppWriter::WriteLagSubTable(const TLayoutTable::TLayoutSubTable& aSubTable, int aCount, ostream& aLag)
	{
	// normal version
	string macro = "#define AKN_LAYOUT_TABLE_";
	macro = macro + CdlTkUtil::ToCpp(iTable.Name());
	if (aSubTable.size() != iTable.size())
		{
		macro = macro + "_SUB_TABLE_" + CdlTkUtil::IntToString(aCount);
		aLag << "// Sub table for " << iTable.Name() << ". Valid line indices are ";
		for (TLayoutTable::TLayoutSubTable::const_iterator pNum = aSubTable.begin(); pNum != aSubTable.end(); ++pNum)
			{
			if (pNum != aSubTable.begin())
				aLag << ", ";
			aLag << CdlTkUtil::IntToString(*pNum);
			}
		aLag << endl;
		}

	aLag << macro;

	int lineSet = 0;
	for (TLayoutTable::TLayoutSubTable::const_iterator pNum = aSubTable.begin(); pNum != aSubTable.end(); ++pNum)
		{
		if (*pNum < 32)
			lineSet |= (1 << *pNum);
		}

	vector<string> def_params;
	def_params.push_back(KParamLineIndex);
	vector<string> ctor_params;
	ctor_params.push_back(CdlTkUtil::IntToString(iTable.iFirstLineGlobalIndex));
	ctor_params.push_back(KParamLineIndex);
	ctor_params.push_back(CdlTkUtil::IntToHexString(lineSet));

	TLayoutLineCppWriter* writer = CreateLayoutLineCppWriter(*iTable[aSubTable[0]], iTable);
	writer->FillParamLists(def_params, ctor_params);
	int paramSize = writer->WriteParamList(aLag, def_params);

	// some maths to make bigger tabulation
	int numTabs = 6-(((paramSize+macro.size())/4)%6);
	for (int i=0; i<numTabs; i++)
		aLag << "\t";

	if (iTable.iType == TLayoutTable::EWindowTable)
		aLag << KTypeWindowLineLayout;
	else
		aLag << KTypeTextLineLayout;
	writer->WriteParamList(aLag, ctor_params);
	aLag << endl;

	delete writer;
	}

TLayoutLineCppWriter* TLayoutTableCppWriter::CreateLayoutLineCppWriter(TLayoutLine& aLine, TLayoutTable& aTable)
	{
	if (aTable.iType == TLayoutTable::EWindowTable)
		return new TWindowLineCppWriter(aLine);
	else
		return new TTextLineCppWriter(aLine);
	}


// class TLayoutCppWriter

void TLayoutCppWriter::WriteCppLineData(ostream& aCpp)
	{
	for (TLayout::iterator it = iLayout.begin(); it != iLayout.end(); ++it)
		{
		TLayoutTableCppWriter writer(**it);
		writer.WriteCppLineData(aCpp);
		}
	}

void TLayoutCppWriter::WriteCppTableData(ostream& aCpp)
	{
	}

void TLayoutCppWriter::WriteLag(ostream& aLag)
	{
	aLag << endl << endl << "// Layout MACROs for indexed table access" << endl;
	// write the macros
	for (TLayout::iterator it = iLayout.begin(); it != iLayout.end(); ++it)
		{
		TLayoutTable& table = **it;
		if (table.IsWorthATableIndex())
			{
			TLayoutTableCppWriter writer(table);
			writer.WriteLag(aLag);
			}
		}

	aLag << endl << "#endif " << endl;
	}


// class TLayoutLineTable

void TLayoutLineTable::WriteCpp(ostream& aCpp)
	{
	aCpp << "const TAny* const KTheLineTable[] =" << endl << "\t{" << endl << "\t";
	int index = 0;
	for (iterator it = begin(); it != end(); ++it)
		{
		TLayoutLine& line = **it;
		TLayoutTable& table = *line.iTable;
		if (table.iFirstLineGlobalIndex == -1)
			table.iFirstLineGlobalIndex = index;
		TLayoutLineCppWriter* writer = TLayoutTableCppWriter::CreateLayoutLineCppWriter(line, table);
		if (it != begin())
			aCpp << "," << endl << "\t";
		aCpp << "&" << writer->Name();
		delete writer;
		index++;
		}
	aCpp << endl << "\t};" << endl;
	}

void TLayoutLineTable::WriteLag(ostream& aLag)
	{
	string current_table_name = "";

	for (iterator it = begin(); it != end(); ++it)
		{
		TLayoutLine& line = **it;

		if ( current_table_name != line.iTable->Name() )
			{
			aLag << endl << "//" << " Layout MACROs for LAF Table : " << line.iTable->Name() << endl ;
			current_table_name = line.iTable->Name();
			}

		TLayoutLineCppWriter* writer = TLayoutTableCppWriter::CreateLayoutLineCppWriter(line, *line.iTable);
		writer->WriteMacro(aLag);
		delete writer;
		}
	}


// class TLayoutLineTable

TCppWriter::TCppWriter(TLayout& aLayout, const std::string& aName)
: TLayWriterBase(aLayout, aName)
	{
	}

void TCppWriter::Write(const std::string& aLayName)
	{
	std::string lay = aLayName.substr(aLayName.find_last_of('\\')+1);
	lay = lay.substr(0, lay.find_first_of('.'));

	iShortName = iName.substr(iName.find_last_of('\\')+1);
	iName = iName.substr(0, iName.find_first_of('.')) + lay;

	BuildLayoutLineTable();
	CCdlTkFileCleanup temp(iName+".lag");
	Output(iName+".cpp", iName+".lag", lay);
	CdlTkUtil::ExportFile(temp, CdlTkUtil::CurrentDrive()+KDirEpocSysHeader+iShortName+".lag");
	}

void TCppWriter::Output(const string& aCpp, const string& aLag, const string& aLayName)
	{
	cout << "writing cpp " << aCpp << endl;
	ofstream cpp(aCpp.c_str());
	WriteCppHeader(cpp);
	TLayoutCppWriter writer(iLayout);
	writer.WriteCppLineData(cpp);
	iLineTable.WriteCpp(cpp);
	writer.WriteCppTableData(cpp);
	WriteCppFooter(cpp, aLayName);
	cpp.close();

	cout << "writing header " << aLag << endl;
	ofstream lag(aLag.c_str());
	WriteLagHeader(lag);
	iLineTable.WriteLag(lag);
	writer.WriteLag(lag);
	lag.close();
	}

void TCppWriter::BuildLayoutLineTable()
	{
	int globalIndex = 0;
	for (TLayout::const_iterator pTab = iLayout.begin(); pTab != iLayout.end(); ++pTab)
		{
		for (TLayoutTable::const_iterator pLine = (*pTab)->begin(); pLine != (*pTab)->end(); ++pLine)
			{
			(*pLine)->iGlobalIndex = globalIndex++;
			iLineTable.push_back(*pLine);
			}
		}
	}

void TCppWriter::WriteCppHeader(ostream& aCpp)
	{
	aCpp << "/*" << endl;
	aCpp << "* ============================================================================" << endl;
	aCpp << "*  Name     : " << iShortName << ".cpp" << endl;
	aCpp << "*  Part of  : Avkon" << endl;
	aCpp << "*" << endl;
	aCpp << "*  Description: This file is automatically generated layout data file." << endl;
	aCpp << "*" << endl;
	aCpp << "*  Version  :" << endl;
	aCpp << "*" << endl;
	aCpp << "*  Copyright © 2002-2004 Nokia Corporation." << endl;
	aCpp << "*  This material, including documentation and any related" << endl;
	aCpp << "*  computer programs, is protected by copyright controlled by" << endl;
	aCpp << "*  Nokia Corporation. All rights are reserved. Copying,"<< endl;
	aCpp << "*  including reproducing, storing,  adapting or translating, any" << endl;
	aCpp << "*  or all of this material requires the prior written consent of" << endl;
	aCpp << "*  Nokia Corporation. This material also contains confidential" << endl;
	aCpp << "*  information which may not be disclosed to others without the" << endl;
	aCpp << "*  prior written consent of Nokia Corporation." << endl;
	aCpp << "* ============================================================================" << endl;
	aCpp << "*/" << endl  << endl;
	aCpp << "#include \"aknlayoutdatadef.h\"" << endl << endl;
	}

void TCppWriter::WriteCppFooter(ostream& aCpp, const string& aLayName)
	{
	aCpp << endl << "const TAny* GetData_" << aLayName << "(" << KTypeInt << " aLineId) { return KTheLineTable[aLineId]; }" << endl;
	}

void TCppWriter::WriteLagHeader(ostream& aLag)
	{
	aLag << "/*" << endl;
	aLag << "* ============================================================================" << endl;
	aLag << "*  Name     : " << iShortName << ".lag" << endl;
	aLag << "*  Part of  : Avkon" << endl;
	aLag << "*" << endl;
	aLag << "*  Description: This file is automatically generated layout file." << endl;
	aLag << "*" << endl;
	aLag << "*  Version  :" << endl;
	aLag << "*" << endl;
	aLag << "*  Copyright © 2002-2004 Nokia Corporation." << endl;
	aLag << "*  This material, including documentation and any related" << endl;
	aLag << "*  computer programs, is protected by copyright controlled by" << endl;
	aLag << "*  Nokia Corporation. All rights are reserved. Copying,"<< endl;
	aLag << "*  including reproducing, storing,  adapting or translating, any" << endl;
	aLag << "*  or all of this material requires the prior written consent of" << endl;
	aLag << "*  Nokia Corporation. This material also contains confidential" << endl;
	aLag << "*  information which may not be disclosed to others without the" << endl;
	aLag << "* ============================================================================" << endl;
	aLag << "*/" << endl  << endl;
	aLag << "#if !defined(__" << iShortName << "_LAG__)" << endl;
	aLag << "#define __" << iShortName << "_LAG__" << endl;
	aLag << "#include <aknlayoutdef.h>" << endl << endl;
	}

// End of File
