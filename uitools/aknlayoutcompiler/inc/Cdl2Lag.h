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


#ifndef CDL2LAG_H
#define CDL2LAG_H

#include <string>
#include <vector>
#include <iosfwd>
#include <CdlCompilerToolkit/CdlTkProcess.h>
using namespace CdlCompilerToolkit;
using namespace std;

/**
*  CdlToLag
*  Generates a LAG file macro API to access a CDL API
*/
class CdlToLag
	{
public:
	static int Process(const vector<string>& args);
	static void ShowHelp(ostream& stream);

private:
	CdlToLag(ofstream& aLag, const string& aLagName);

	void Start();
	void AddInterface(CCdlTkInterface& aInterface);
	void Finish();

	void WriteHeader();
	void WriteMacro(CCdlTkApi& aApi);
	void WriteFooter();
	void WriteInclude();

private:
	ofstream& iLag;
	string iLagName;
	CCdlTkInterface* iInterface;
	};

#endif
