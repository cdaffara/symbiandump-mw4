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


#ifndef HTMLPARSE_H
#define HTMLPARSE_H

// disable "identifier was truncated to '255' characters in the browser information" warning
#pragma warning (disable:4786)
#include <iosfwd>
#include <vector>
#include <string>
#include "Layout.h"
using namespace std;

/**
*  THtmlParseLayoutTable 
*  Parse a layout table from an HTML version of a LAF spec
*/
class THtmlParseLayoutTable : public TLayoutTable
	{
private:
	typedef vector<string> Row;

public:
	THtmlParseLayoutTable(TLayout* aTables);
	~THtmlParseLayoutTable();

	bool Read(istream& in);

private:
	bool SearchForTableStart();
	void ExtractTitle(const string& aText);
	void ExtractTable();

	string Next();
	bool IsToken(const string& aText);
	void PutBack(const string& aText);
	char Get();
	void PutBack(char aChar);
	bool Eof();
	
	int GetVal(const string& aText, const string& aField);

	bool IsWindowTable();
	bool IsTextTable();

	string StripTo(const string& aText, const string& aValid);
	string UnHtml(const string& aText);
	char HtmlChar(const string& aText);
	string CleanMultiCell(const string& aText);
	Row MakeParamTable(const Row& row, int start, int num);
	string ConvertToAknName(const string& aText);
	string SplitMultiCell(const string& aCell, const string& aValid);
	string CleanCell(const string& cell, const Row& row);

	void AddRow(Row& row);
	void AddColumnNameRow(Row& row);
	void AddLineRow(const Row& row);
	void AddTitleRow(const Row& row);

private:
	istream* iIn;
	string iPutBack;
	};


/**
*  THtmlParseLayout 
*  Parse the entire layout from an HTML version of a LAF spec
*/
class THtmlParseLayout : public TLayout
	{
public:
	void Parse(istream &aIn);
	};


#endif

// End of File
