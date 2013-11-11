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


#include "HtmlParse.h"
#include <iostream>
#include <sstream>
#include <set>
#include <algorithm>
#include <memory>
using namespace std;


const string WhiteSpace(" \t\r\n");
const string gValidText("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_<>()-+. ");
const string gValidNum("0123456789+-p ,");
const string KEllipsis("\205");
const string KUnknown("unknown");

string TrimWhiteSpace(const string& aString)
	{
	string::size_type start = aString.find_first_not_of(WhiteSpace);
	if (start == string::npos)
		return "";
	else
		return aString.substr(start, 1+aString.find_last_not_of(WhiteSpace)-start);
	}


THtmlParseLayoutTable::THtmlParseLayoutTable(TLayout* aTables) 
: TLayoutTable(aTables)
	{
	}

THtmlParseLayoutTable::~THtmlParseLayoutTable()
	{
	}

bool THtmlParseLayoutTable::Read(istream& in)
	{
	iIn = &in;

	if (!SearchForTableStart())
		return false;

	ExtractTable();

	iIn = 0;

	return true;
	}

bool THtmlParseLayoutTable::SearchForTableStart()
	{
	string text;
	while (!Eof())
		{
		string next = Next();
		if (IsToken(next))
			{
			if (next.substr(0,6) == "<TABLE")
				{
				ExtractTitle(text);
				return true;
				}
			}
		else
			{
			if (next.substr(0,5) == "Table")
				text = next.substr(5);
			else
				text += next;
			}
		}

	return false;
	}

set<string> gKnownTitles;

void THtmlParseLayoutTable::ExtractTitle(const string& aText)
	{
	iName = UnHtml(aText);
	string::size_type pos = iName.find_first_not_of("1234567890.\t\r\n ");
	if (pos == string::npos)
		return;
	iName = iName.substr(pos);
	iName = StripTo(iName, gValidText);
	iName = TrimWhiteSpace(iName);

	while (gKnownTitles.find(iName) != gKnownTitles.end())
		iName += "_dup";

	gKnownTitles.insert(iName);
	}

void THtmlParseLayoutTable::ExtractTable()
	{
	string cell;
	string td;
	Row row;
	while (!Eof())
		{
		string tok = Next();
		if (IsToken(tok))
			{
			if (tok.substr(0,4) == "<TD ")
				{
				td = tok;
				}
			else if (tok == "</TD>")
				{
				row.push_back(CleanCell(cell, row));
				cell = "";
				int colspan = GetVal(td, "COLSPAN");
				for (int i=1; i<colspan; i++)
					row.push_back("");
				}
			else if (tok == "</TR>")
				{
				if (!row[1].length())
					row[1] = "untitled";
				AddRow(row);
				row.erase(row.begin(), row.end());
				}
			else if (tok == "</TABLE>")
				{
				break;
				}
			else if (cell.length() && (tok == "<BR>" || tok == "<P>"))
				{
				cell += ", ";
				}
			}
		else
			{
			cell += tok;
			}
		}
	}

string THtmlParseLayoutTable::CleanCell(const string& cell, const Row& row)
	{
	if (iColumnNames.size() < 6)		// table header
		return cell;
	else if (row.size() < 2 || row.size() == iColumnNames.size())	// free text
		return StripTo(UnHtml(cell),gValidText+KEllipsis);
	else
		return CleanMultiCell(StripTo(UnHtml(cell),gValidText+"/,*"));	// value cell
	}

string THtmlParseLayoutTable::Next()
	{
	string s;
	char c;

	do	{
		c = Get(); 
		} while (!Eof() && WhiteSpace.find(c) != string::npos);
	s += c;

	if (c == '<')
		{
		do	{
			c = Get(); 
			s += toupper(c);
			} while (!Eof() && c != '>');
		}
	else
		{
		do	{
			c = Get(); 
			s += c;
			} while (!Eof() && c != '<');

		if (c == '<')
			PutBack(c);

		s = s.substr(0, s.find_last_not_of(WhiteSpace));
		}

	return s;
	}

bool THtmlParseLayoutTable::IsToken(const string& aText)
	{
	return aText.length()>0 && aText[0] == '<';
	}

void THtmlParseLayoutTable::PutBack(const string& aText)
	{
	for (string::const_reverse_iterator pC = aText.rbegin(); pC != aText.rend(); ++pC)
		PutBack(*pC);
	}

int THtmlParseLayoutTable::GetVal(const string& aText, const string& aField)
	{
	string::size_type pos = aText.find(aField+"=");
	if (pos == string::npos)
		return 0;
	string val = aText.substr(pos + aField.length() + 1);
	val = val.substr(0, val.find_first_of(WhiteSpace + ">"));
	stringstream s(val);
	int ret;
	s >> ret;
	return ret;
	}

char THtmlParseLayoutTable::Get()
	{
	if (iPutBack.length())
		{
		char c = iPutBack[iPutBack.length()-1];
		iPutBack.erase(iPutBack.length()-1, 1);
		return c;
		}
	else
		{
		return iIn->get();
		}
	}

void THtmlParseLayoutTable::PutBack(char aChar)
	{
	iPutBack += aChar;
	}

bool THtmlParseLayoutTable::Eof()
	{
	return iPutBack.length()==0 && iIn->eof();
	}

string THtmlParseLayoutTable::CleanMultiCell(const string& aText)
	{
	// This function removes trailing commas from the end of cells where there is no
	// comma in the middle of the cell. 
	// needed because of odd formatting found.
	if (aText.length()==0)
		return aText;

	string::size_type pos = aText.find_last_not_of(" ,");
	if (pos == string::npos)
		return "";

	string text;
	if (aText.substr(0,pos+1).find(",") == string::npos)
		text = aText.substr(0,pos+1);
	else
		text = aText;

	return text;
	}

const int KMaxStars = 16;
struct SCellParamDesc
	{
	SCellParamDesc();
	bool operator==(const SCellParamDesc& a);
	int iParams;
	bool iStars[KMaxStars];
	};

SCellParamDesc::SCellParamDesc() 
	{ 
	iParams=0;
	for (int i=0; i<KMaxStars; i++)
		iStars[i] = false; 
	}

bool SCellParamDesc::operator==(const SCellParamDesc& a) 
	{
	if (iParams!=a.iParams)
		return false;

	for (int i=0; i<KMaxStars; i++)
		{
		if (iStars[i]!=a.iStars[i])
			return false;
		}

	return true;
	}

THtmlParseLayoutTable::Row THtmlParseLayoutTable::MakeParamTable(const Row& row, int start, int num)
	{
	Row params;
	vector<SCellParamDesc> paramDescs;

	// initialise params
	for (Row::const_iterator pR = row.begin(); pR != row.end(); ++pR)
		{
		params.push_back("");
		paramDescs.push_back(SCellParamDesc());
		}

	// count params and stars
	int i;
	for (i=0; i<num; i++)
		{
		SCellParamDesc& desc = paramDescs[start+i];
		const string& cell = row[start+i];
		desc.iParams = 1;

		int starCount=0;
		for (string::const_iterator pC = cell.begin(); pC != cell.end(); ++pC)
			{
			if (*pC == '*')
				{
				starCount++;
				}
			else
				{
				if (starCount)
					{
					desc.iStars[starCount-1] = true;
					starCount = 0;
					}
				if (*pC == ',')
					{
					desc.iParams++;
					}
				}
			}

		if (starCount)
			desc.iStars[starCount-1] = true;
		}

	// assign parameter names
	string name("aCommon1");
	for (i=0; i<num; i++)
		{
		SCellParamDesc& desc = paramDescs[start+i];
		if (desc.iParams == 1)
			continue;

		string& param = params[start+i];

		int count = 0;
		bool first = true;

		// look for a similar cell
		for (int j=0; j<num; j++)
			{
			if (paramDescs[start+j] == desc)
				{
				count++;
				if (count == 1 && j<i)
					{
					first = false;
					param = params[start+j];
					}
				}
			}

		// assign a new name if there is no similar cell already named
		if (count>1 && first)
			{
			param = name;
			name[7]++;
			}
		}

	return params;
	}

void THtmlParseLayoutTable::AddRow(Row& row)
	{
	if (row.size()==0)
		return;				// empty row

	const string& first = row[0];

	if (first.length()==0 || first == "P")
		AddTitleRow(row);
	else if (first == "No.")
		AddColumnNameRow(row);
	else if (first == KEllipsis)
		iAppend = true;
	else
		AddLineRow(row);
	}

void THtmlParseLayoutTable::AddColumnNameRow(Row& row)
	{
	if (row.size()<4)
		return;				// unknown row type

	if (row[1] == "Value")
		row[1] = "Item";

	if (row[1] == "Font")
		iType = ETextTable;
	else if (row[1] == "Item")
		iType = EWindowTable;
	else
		return;				// unknown row type

	SetDefaultColumnNames();
	iColumnNames.insert(iColumnNames.begin()+3, "b/r"); // for skins LAF

	int foundCount = 0;
	vector<string> foundColNames;
	for (Row::iterator pCol = row.begin()+1; pCol != row.end(); pCol++)
		{
		string cellTitle = *pCol;
		if (cellTitle == "Remarks.")
			cellTitle = "Remarks";
		if (iType == ETextTable && cellTitle == "Margins")
			{
			foundColNames.push_back("l");
			foundColNames.push_back("r");
			pCol++;
			}
		else if (find(iColumnNames.begin(),iColumnNames.end(),cellTitle) != iColumnNames.end())
			{
			foundColNames.push_back(cellTitle);
			foundCount++;
			}
		else
			{
			foundColNames.push_back(KUnknown);
			}
		}
	iColumnNames = foundColNames;

	if (foundCount < 4)
		{
		iColumnNames.clear();
		iType = EUnknownTable;
		}
	}

void THtmlParseLayoutTable::AddLineRow(const Row& row)
	{
	if (iColumnNames.size() == 0 || row.size() != iColumnNames.size()+1)
		return;		// unknown row type;

	Row params = MakeParamTable(row, 2, iColumnNames.size()-2);

	Row::const_iterator pCell = row.begin();
	Row::const_iterator pParam = params.begin();

	int id;
	stringstream stream(*pCell);
	stream >> id;
	if (id == 0)
		return;		// bad id;

	TLayoutLine* line = new TLayoutLine(this, id);
	push_back(line);

	bool found_l = false;
	for (vector<string>::iterator pCol = iColumnNames.begin(); pCol != iColumnNames.end(); ++pCol)
		{
		string col = *pCol;
		if (col == "b/r")
			col = found_l ? "b" : "r";

		++pCell;
		++pParam;
		TValues values(line, col);

		if (pParam->size())
			values.iParam = *pParam;

		string cell = *pCell;
		if (IsValueColumn(col))
			cell = ConvertToAknName(SplitMultiCell(cell, IsNumericColumn(col) ? gValidNum : gValidText));

		int pos=0;
		do	{
			int newPos = cell.find(',', pos);
			values.push_back(TrimWhiteSpace(cell.substr(pos, newPos-pos)));
			pos = newPos+1;
			} while (pos);

		if ((values.size()>1 || values[0].size()>0) && col == "l")
			found_l = true;

		if (col != KUnknown)
			line->insert(make_pair(col, values));
		}
	}

void THtmlParseLayoutTable::AddTitleRow(const Row& row)
	{
	if (row.size() < 2)
		return;
	if (row[0] == "P")
		iParentName = TrimWhiteSpace(row[1]);
	}

string THtmlParseLayoutTable::SplitMultiCell(const string& aCell, const string& aValid)
	{
	string cell = aCell;

	// Make sure commas are in correctly!
	cell = StripTo(UnHtml(cell), aValid+"/,");
	
	int lastComma = -1;
	int lastSpace = -1;
	int lastNum = -1;
	
	for (string::size_type i=0; i<cell.length(); i++)
		{
		char c = cell[i];
		if (c == ',')
			lastComma = i;
		else if (c == ' ' || c == '/')
			lastSpace = i;
		else 
			{
			if (lastSpace > lastNum && lastNum > lastComma)
				{
				cell[lastSpace] = ',';
				lastComma = lastSpace;
				}
			lastNum = i;
			}
		}

	cell = StripTo(UnHtml(cell), aValid+",");
	if (cell == "-")
		cell = "";

	return cell;
	}

string THtmlParseLayoutTable::StripTo(const string& aText, const string& aValid)
	{
	string cell("");
	for (string::const_iterator pC = aText.begin(); pC != aText.end(); ++pC)
		{
		if (aValid.find(*pC) != string::npos)
			cell += *pC;
		}
	return cell;
	}

string THtmlParseLayoutTable::UnHtml(const string& aText)
	{
	string str("");
	for (string::size_type i=0; i<aText.size(); i++)
		{
		char c = aText[i];
		if (c == '&')
			{
			string s = aText.substr(i);
			string::size_type pos = s.find(";");
			if (pos != string::npos)
				{
				i+=pos;
				c = HtmlChar(s.substr(1, pos-1));
				}
			}
		else if (c == char(0x96))
			{
			c = '-';
			}
		str += c;
		}
	return str;
	}

struct THtmlChar {char* iString; char iChar;};
const THtmlChar gHtmlChars[] =
	{
		{"gt", '>'},
		{"lt", '<'},
		{"nbsp", ' '},
		{"#9", '\t'}
	};

char THtmlParseLayoutTable::HtmlChar(const string& aText)
	{
	for (unsigned int i=0; i<sizeof(gHtmlChars)/sizeof(THtmlChar); i++)
		{
		if (aText == gHtmlChars[i].iString)
			return gHtmlChars[i].iChar;
		}
	return '_';
	}

struct SConvertAknName
	{
	char* iLaf;
	char* iAkn;
	};

SConvertAknName gAknNameConversionTable[] =
	{
		{ "left", "ELayoutAlignLeft" },
		{ "right", "ELayoutAlignRight" },
		{ "centre", "ELayoutAlignCenter" },
		{ "center", "ELayoutAlignCenter" },
		{ "bidi", "ELayoutAlignBidi" },
		{ "qfn_latin_bold_19", "ELatinBold19" },
		{ "qfn_latin_bold_17", "ELatinBold17" },
		{ "qfn_latin_bold_13", "ELatinBold13" },
		{ "qfn_latin_bold_12", "ELatinBold12" },
		{ "qfn_latin_plain_12", "ELatinPlain12" },
		{ "qfn_latin_plain_13", "ELatinPlain12" },			// made up for elaf spec, only needed by navi pane?
		{ "qfn_latin_clock_14", "ELatinClock14" },
		{ "qfn_<ref>_plain_12", "EApacPlain12" },	
		{ "qfn_<ref>_plain_16", "EApacPlain16" },
		{ "qfn_china_plain_12", "EApacPlain12" },
		{ "qfn_number_plain_5", "ENumberPlain5" },
		{ "qfn_china_plain_16", "EApacPlain16" },
		{ "qfn_clock_bold_30", "EClockBold30" },	
		{ "qfn_number_bold_14", "ELatinClock14" },
		{ "gfn_<ref>_plain_12", "EApacPlain12" },
		{ "gfn_<ref>_plain_16", "EApacPlain16" },
		{ "gfn_latin_bold_16", "ELatinBold16" },
		{ "qfn_calc_21", "ECalcBold21" },
		{ "qfn_calc_oper_21", "ECalcOperBold21" },
		{ "qfn_calc_oper_13", "ECalcOperBold13" }
	};

string THtmlParseLayoutTable::ConvertToAknName(const string& aText)
	{
	string ret = aText;
	for (unsigned int i=0; i<sizeof(gAknNameConversionTable)/sizeof(SConvertAknName); i++)
		{
		string laf = gAknNameConversionTable[i].iLaf;
		string akn = gAknNameConversionTable[i].iAkn;
		string::size_type pos;
		while ((pos = ret.find(laf)) != string::npos)
			{
			ret.erase(pos, laf.length());
			ret.insert(pos, akn);
			}
		}
	return ret;
	}


void THtmlParseLayout::Parse(istream &aIn)
	{
	while (!aIn.eof())
		{
		auto_ptr<THtmlParseLayoutTable> table(new THtmlParseLayoutTable(this));
		if (table->Read(aIn))
			{
			if (table->size() > 0)
				push_back(table.release());
			}
		else
			break;
		}
	Compile();
	}

// End of File
