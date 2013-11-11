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



#include "LayoutParse.h"
#include <sstream>
#include <fstream>
#include <algorithm>
#include <iostream> // !!! for debug output only

extern string whiteSpace;

void SkipWhitespace(istream& aIn)
	{
	char ch;

	while (!aIn.eof())
		{
		ch = aIn.get();
		if (whiteSpace.find(ch) == string::npos)
			{
			aIn.putback(ch);
			break;
			}
		}
	}

void TrimEnd(string& aString)
	{
	int len = aString.length();
	while (whiteSpace.find(aString[len-1]) != string::npos)
		len--;
	aString = aString.substr(0, len);
	}

string NextToken(istream& aIn, const string& aTerminate)
	{
	SkipWhitespace(aIn);

	string ret;
	char ch;

	while (!aIn.eof())
		{
		ch = aIn.get();
		if (aTerminate.find(ch) != string::npos)
			{
			if (ret.length())
				aIn.putback(ch);
			else
				ret += (ch);
			break;
			}
		ret += ch;
		}

	TrimEnd(ret);

	return ret;
	}

void FindAndSkip(istream& aIn, const string& aString)
	{
	string tok = NextToken(aIn, aString);
	if (aString.find(tok) == string::npos)
		throw LayParseErr(aIn, tok, "not found");
	}


TLayParseValues::TLayParseValues(TLayoutLine* aLine, string aName)
: TValues(aLine, aName)
	{
	}

void TLayParseValues::Parse(istream& aIn)
	{
	string tok = NextToken(aIn, ",{}");
	if (tok == "," || tok == "}")
		{
		aIn.putback(tok[0]);
		push_back("");
		}
	else if (tok == "{")
		{
		do	{
			TLayParseValues v(iLine, iName);
			v.Parse(aIn);
			insert(end(), v.begin(), v.end());
			tok = NextToken(aIn, ",}");
			} while (tok != "}");

		tok = NextToken(aIn, ",}[");
		if (tok == "[")
			{
			tok = NextToken(aIn, "]");
			if (tok != "]")
				{
				iParam = tok;
				FindAndSkip(aIn, "]");
				}
			}
		else if (tok == "," || tok == "}")
			{
			aIn.putback(tok[0]);
			}
		else
			{
			throw LayParseErr(aIn, tok, "expected '[' ',' or '}'");
			}
		}
	else
		{
		push_back(tok);
		}
	}


//
//  TLayParseLayoutLine
//

TLayParseLayoutLine::TLayParseLayoutLine(TLayoutTable* aTable, int aId)
: TLayoutLine(aTable, aId)
	{
	}

void TLayParseLayoutLine::Parse(istream& aIn)
	{
	FindAndSkip(aIn, "{");

	string tok;

	for (vector<string>::iterator pCol = iTable->iColumnNames.begin();
		 pCol != iTable->iColumnNames.end();
		 ++pCol)
		{
		TLayParseValues values(this, *pCol);
		values.Parse(aIn);
		insert(make_pair(*pCol, values));
		tok = NextToken(aIn, ",}");
		}

	if (tok != "}")
		throw LayParseErr(aIn, tok, "expected }");
	}



// TLayParseLayoutTable


TLayParseLayoutTable::TLayParseLayoutTable(TLayout* aTables)
: TLayoutTable(aTables)
	{
	}

void TLayParseLayoutTable::Parse(istream& aIn)
	{
	string tok = NextToken(aIn, ":{}");
	if (string(":{}").find(tok) != string::npos)
		throw LayParseErr(aIn, tok, "expected table name");
	iName = tok;

	tok = NextToken(aIn, ":{");
	if (string(":{").find(tok) == string::npos)
		throw LayParseErr(aIn, tok, "expected ':' or '{'");

	if (tok == ":")
		{
		iParentName = NextToken(aIn, "{");
		FindAndSkip(aIn, "{");
		}

	tok = NextToken(aIn, "{+");
	if (string("{+").find(tok) == string::npos)
		throw LayParseErr(aIn, tok, "expected '+' or '{'");
	else if (tok == "+")
		iAppend = true;
	else
		aIn.putback(tok[0]);

	int id = 0;
	do  {
		TLayParseLayoutLine* line = new TLayParseLayoutLine(this, ++id);
		push_back(line);
		line->Parse(aIn);
		tok = NextToken(aIn, ",}");
		if (string(",}").find(tok) == string::npos)
			throw LayParseErr(aIn, tok, "expected ',' or '}'");
		} while (tok != "}");
	}



void TLayParseLayout::Parse(istream& aIn)
	{
	while (!aIn.eof())
		{
		string tok = NextToken(aIn, whiteSpace);
		if (tok == "WindowTable" || tok == "TextTable")
			{
			TLayParseLayoutTable* tab = new TLayParseLayoutTable(this);
			push_back(tab);
			tab->iType = (tok == "WindowTable" ? TLayoutTable::EWindowTable : TLayoutTable::ETextTable);
			tab->SetDefaultColumnNames();
			tab->Parse(aIn);
			}
		else if (tok.length()==0 && aIn.eof())
			{
			// white space at end of file
			break;
			}
		else
			{
			throw LayParseErr(aIn, tok, "expected a Table");
			}
		}
	Compile();
	}

auto_ptr<TLayParseLayout> TLayParseLayout::Parse(const string& aLayName)
	{
	string layName = aLayName;

	auto_ptr<TLayParseLayout> layout(new TLayParseLayout);
	if (layName.size() >= 2 && layName.substr(0,2) == "-m")
		{
		layName = layName.substr(2);
		layout->iCanBeMirror = true;
		}
	layout->iName = layName;

	string::size_type pos=0;
	bool first = true;
	while (pos != string::npos)
		{
		string::size_type next = layName.find('+', pos);
		string name;
		if (next == string::npos)
			{
			name = layName.substr(pos);
			pos = next;
			}
		else
			{
			name = layName.substr(pos, next-pos);
			pos = layName.find_first_not_of('+', next);
			}

		ifstream in(name.c_str());
		if (!in.is_open())
			throw NotFoundErr(name);
		cout << "reading layout " << name << endl;

		if (first)
			{
			layout->Parse(in);
			}
		else
			{
			TLayParseLayout nextLay;
			nextLay.Parse(in);
			layout->Merge(TLayout::KMergeModeUnion, nextLay);
			}

		first = false;
		in.close();
		}
	return layout;
	}



LayParseErr::LayParseErr(istream& aIn, const string& aTok, const string& aMsg)
: iLoc(StreamLoc(aIn,"<<Near Here>>", 100, 100)), iTok(aTok), iMsg(aMsg) 
	{
	}

void LayParseErr::Show(ostream& aOut) const
	{
	aOut << "ERROR : \"" << iTok << "\" " << iMsg << endl << iLoc << endl;
	}

// End of File
