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



// disable "identifier was truncated to '255' characters in the browser information" warning
#pragma warning (disable:4786)

#include "Script.h"
#include "LayoutCompilerErr.h"
#include <iostream>
#include <fstream>
using namespace std;
using namespace CdlCompilerToolkit;

typedef LayoutProcessArgsErr<AknLayoutCompilerScript> AknLayoutCompilerScriptArgsErr;


class ScriptErr : public LayoutCompilerErr
	{
public:
	ScriptErr(const string& aLine, const string& aMsg) : iLine(aLine), iMsg(aMsg) {}
private:
	void Show(ostream& aStream) const;
private:
	string iLine;
	string iMsg;
	};

void ScriptErr::Show(ostream& aStream) const
	{
	aStream << iLine << endl;
	aStream << iMsg << endl;
	}


int AknLayoutCompilerScript::Process(const vector<string>& args)
	{
	if (args.size() != 2)
		throw AknLayoutCompilerScriptArgsErr();

	ifstream in;
	CdlTkUtil::OpenInput(in, args[1].substr(1));

	AknLayoutCompilerScript script(in);
	script.Process();

	in.close();

	return 0;
	}

void AknLayoutCompilerScript::ShowHelp(ostream& /*stream*/)
	{
	}

AknLayoutCompilerScript::AknLayoutCompilerScript(istream& aIn)
: iIn(aIn)
	{
	}

void AknLayoutCompilerScript::Process()
	{
//	while (!iIn.eof())
//		{
//		string line;
//		getline(iIn, line);
//		ProcessLine(line);
//		}
	}

void AknLayoutCompilerScript::ProcessLine(string& aLine)
	{
	CdlTkUtil::StripLeadingAndTrailingWhitespace(aLine);
	if (aLine.empty())
		return;

	int paramStart = aLine.find_first_of('(');
	int paramEnd = aLine.find_first_of(')');
	if (paramStart == string::npos || paramEnd == string::npos)
		throw ScriptErr(aLine, "Must be of form \"[result=] func(params)\"");

	int eqPos = aLine.find_first_of('=');
	string result;
	if (eqPos > 0)
		{
		result = aLine.substr(0, eqPos-1);
		CdlTkUtil::StripLeadingAndTrailingWhitespace(result);
		}

	string func = aLine.substr(eqPos+1, paramStart-(eqPos+1));
	CdlTkUtil::StripLeadingAndTrailingWhitespace(func);
	func = CdlTkUtil::ToLower(func);

	paramStart++;
	string params = aLine.substr(paramStart, paramEnd-paramStart);
	vector<string> paramList;
	int p = 0;
	while (p != string::npos)
		{
		int n = params.find_first_of(',');
		string param(params.substr(0, n));
		CdlTkUtil::StripLeadingAndTrailingWhitespace(param);
		if (param.size())
			paramList.push_back(param);
		p = n;
		if (p!=string::npos)
			params = params.substr(p+1);
		}

	Process(func, paramList, result);
	}

void AknLayoutCompilerScript::Process(const string& aFunc, const vector<string>& aParamList, const string& aResult)
	{
	// use a map of name -> name, type and value
	cout << aFunc << "[" << aParamList.size() << "|" << aResult << "]" << endl;
	}
