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


#ifndef LAYCDLCHECK_H
#define LAYCDLCHECK_H

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
*  LayCdlCheck
*  Check the interface which could be use with a layout against one specified in a CDL file
*/
class LayCdlCheck
	{
public:
	static int Process(const vector<string>& args);
	static void ShowHelp(ostream& stream);

	LayCdlCheck(TLayout& aLayout, CCdlTkInterface& aInterface, CCdlTkInterface& aExcluded, const string& aOpt);
	~LayCdlCheck();
	void Process();

	void Compare(vector<string>& aGood, vector<string>& aOnlyLay, vector<string>& aOnlyCdl, vector<string>& aParamMismatch);
	void Report(const string& aTitle, vector<string>& aApi);
	bool HasOpt(char c);

private:
	TLayout& iLayout;
	CCdlTkInterface& iInterface;
	CCdlTkInterface& iExcluded;
	CCdlTkInterface* iLayoutInterface;	// owned
	string iOpt;
	};

#endif
