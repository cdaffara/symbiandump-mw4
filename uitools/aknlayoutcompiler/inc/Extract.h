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


#ifndef EXTRACT_H
#define EXTRACT_H

#include <string>
#include <vector>
#include <iosfwd>
using namespace std;

/**
*  LayoutExtract
*  Layout compiler process for extracting a layout from an HTML file
*/
class LayoutExtract
	{
public:
	int Extract(const vector<string>& args);
	static void ShowHelp(ostream& stream);

private:
	void ParseArgs(const vector<string>& args);

private:
	string iDocName;
	string iLayoutName;
	string iMergeName;
	};

#endif

// End of File
