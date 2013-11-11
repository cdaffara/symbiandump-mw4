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


#ifndef MAKELAYCONVTEST_H
#define MAKELAYCONVTEST_H

#include <string>
#include <vector>
#include <iosfwd>
#include "Layout.h"
using namespace std;

#include <CdlCompilerToolkit/CdlTkInterface.h>
#include <CdlCompilerToolkit/CdlTkInstance.h>
#include <CdlCompilerToolkit/CdlTkProcess.h>
using namespace CdlCompilerToolkit;


/**
*  MakeLayoutConvTest
*  Generate test code to test the validity of the conversion from the older 
*  AknLayout system to AknLayout2
*/
class MakeLayoutConvTest
	{
public:
	static int Process(const vector<string>& args);
	static void ShowHelp(ostream& stream);

	MakeLayoutConvTest(CCdlTkInterface& aInterface, TLayout& aLayout);
	~MakeLayoutConvTest();
	void RunProcess();

private:
	void AddTableToTest(TLayoutTable& aTable);
	void WriteTestFiles();
	void WriteTestFile(const string& aFileName, const string& iContent);
	void InitFiles();
	void CompleteFiles();
	void AddLineToTest(TLayoutLine& aLine);
	void AddTest(TLayoutLine& aLine, const CCdlTkFunctionApi& aApi);
	const CCdlTkFunctionApi* FindApi(const string& aName);

	void AddTestToOldH(TLayoutLine& aLine, const CCdlTkFunctionApi& aApi);
	void AddTestToOldCpp(TLayoutLine& aLine, const CCdlTkFunctionApi& aApi);
	void AddTestFunc(TLayoutLine& aLine, const CCdlTkFunctionApi& aApi);

	string OldFuncSig(TLayoutLine& aLine, const CCdlTkFunctionApi& aApi);

private:
	CCdlTkInterface& iInterface;
	TLayout& iLayout;
	string iOldH;
	string iOldCpp;
	string iFuncs;
	string iTests;
	};

#endif
