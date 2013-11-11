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


#ifndef LAYSCALE_H
#define LAYSCALE_H

#include <string>
#include <vector>
#include <iosfwd>
#include "Layout.h"
using namespace std;

#include <CdlCompilerToolkit/CdlTkInterface.h>
using namespace CdlCompilerToolkit;


/**
*  LayoutScale
*  Compiler mode for rescaling a layout
*/
class LayoutScale
	{
public:
	static int Process(const vector<string>& args);
	static void ShowHelp(ostream& stream);

private:
	LayoutScale(TLayout& aSourceLayout, int aSourceWidth, int aSourceHeight, const string& aDestLayoutName, int aDestWidth, int aDestHeight);
	void Run();
	void RescaleLayout();
	void WriteLayout();

private:
	enum TScale {EHoriz, EVert};
	void RescaleValue(string& aVal, TScale aDir);
	string RescaleNum(string& aNum, TScale aDir);
    void TagScaledToken(string& aToken);
	int Scale(int aNumber, int aNumerator, int aDenominator);

private:
	TLayout& iLayout;
	int iSourceWidth;
	int iSourceHeight;
	const string& iDestLayoutName;
	int iDestWidth;
	int iDestHeight;
	};


#endif
