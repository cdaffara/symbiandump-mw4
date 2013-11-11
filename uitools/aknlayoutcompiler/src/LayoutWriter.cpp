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


#include "LayoutWriter.h"
#include <iostream>
#include <sstream>
#include <set>
#include <fstream>
using namespace std;


TLayoutTableWriter::TLayoutTableWriter(TLayoutTable& aTable) : iTable(aTable)
	{
	}

TLayoutTableWriter::~TLayoutTableWriter()
	{
	}

void TLayoutTableWriter::Write(ostream& out)
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

bool TLayoutTableWriter::IsWindowTable()
	{
	return iTable.iType == TLayoutTable::EWindowTable;
	}


void TLayoutTableWriter::WriteWindowTable(ostream& out)
	{
	out << "WindowTable " << iTable.Name();
	if (iTable.iParentName.size())
		out << " : " << iTable.iParentName;
	out << endl;
	out << "\t{" << endl;

	if (iTable.iAppend)
		out << "\t+" << endl;
	
	for (unsigned int i=0; i<iTable.size(); ++i)
		{
		if (i>0)
			out << "," << endl;
		WriteWindowLine(out, *iTable[i]);
		}

	out << endl << "\t}" << endl;
	out << endl;
	}

void TLayoutTableWriter::WriteWindowLine(ostream& out, TLayoutLine& line)
	{
	out << "\t\t{" << line.Name() << ", ";

	for (int i=0; i<7; i++)
		{
		WriteCell(out, line[KWindowOutputOrder[i]]);
		out << ", ";
		}

	out << line["Remarks"][0] << "}";
	}

void TLayoutTableWriter::WriteCell(ostream& out, TValues& values)
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


bool TLayoutTableWriter::IsTextTable()
	{
	return iTable.iType == TLayoutTable::ETextTable;
	}

void TLayoutTableWriter::WriteTextTable(ostream& out)
	{
	out << "TextTable " << iTable.Name();
	if (iTable.iParentName.size())
		out << " : " << iTable.iParentName;
	out << endl;
	out << "\t{" << endl;

	if (iTable.iAppend)
		out << "\t+" << endl;
	
	for (unsigned int i=0; i<iTable.size(); ++i)
		{
		if (i>0)
			out << "," << endl;
		WriteTextLine(out, *iTable[i]);
		}

	out << endl << "\t}" << endl;
	out << endl;
	}

void TLayoutTableWriter::WriteTextLine(ostream& out, TLayoutLine& line)
	{
	out << "\t\t{" << line["Font"][0] << ", ";

	for (int i=0; i<6; i++)
		{
		WriteCell(out, line[KTextOutputOrder[i]]);
		out << ", ";
		}

	out << line["Remarks"][0] << "}";
	}


TLayoutWriter::TLayoutWriter(TLayout& aLayout, const std::string& aName)
: TLayWriterBase(aLayout, aName)
	{
	}

void TLayoutWriter::Write(const std::string&)
	{
	cout << "writing layout " << iName << endl;
	ofstream out(iName.c_str());
	for (TLayout::iterator pTab = iLayout.begin(); pTab != iLayout.end(); ++pTab)
		{
		TLayoutTableWriter writer(**pTab);
		writer.Write(out);
		}
	out.close();
	}

// End of File
