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



#include "LayoutCompilerErr.h"
#include <iostream>
#include <algorithm>
#include <string>
using namespace std;

void NotFoundErr::Show(ostream& stream) const
	{
	stream << iName << " not found" << endl;
	}

void GeneralErr::Show(ostream& stream) const
	{
	stream << iMessage << endl;
	}

string StreamLoc(const istream& aStream, const string& msg, int before, int after)
	{
	string out;
	istream stream(aStream.rdbuf());
	int pos = stream.tellg();
#ifdef __VC32__
	int start = _MAX(0, pos-before);
#else
	int start = max(0, pos-before);
#endif
	stream.seekg(start);
	bool record = (start==0);
	for (int i=0; i<(pos-start)+after; i++)
		{
		char ch = stream.get();
		if (record)
			out.append(1,ch);
		if (ch == '\n' || ch == '\r')
			record = true;
		if (start+i == pos)
			out.append(msg);
		}
	return out;
	}

// End of File
