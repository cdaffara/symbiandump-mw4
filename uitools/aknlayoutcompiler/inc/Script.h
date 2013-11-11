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


#ifndef SCRIPT_H
#define SCRIPT_H

#include <iosfwd>
#include <vector>
#include <string>
using namespace std;

/**
*  AknLayoutCompilerScript
*  Intended to be a script interpreter for the layout compiler - not yet complete
*/
class AknLayoutCompilerScript
	{
public:
	static int Process(const vector<string>& args);
	static void ShowHelp(ostream& stream);

	AknLayoutCompilerScript(istream& aIn);
	void Process();

private:
	void ProcessLine(string& aLine);
	void Process(const string& aFunc, const vector<string>& aParamList, const string& aResult);

private:
	istream& iIn;
	};

#endif
