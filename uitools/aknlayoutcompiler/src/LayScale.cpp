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



// disable "identifier was truncated to '255' characters in the browser information" warning
#pragma warning (disable:4786)

#include "LayScale.h"
#include "LayoutParse.h"
#include "LayoutCompilerErr.h"
#include <CdlCompilerToolkit/CdlTkProcess.h>
#include <fstream>
#include <iostream>
#include <algorithm>
#include "LayoutWriter.h"

typedef LayoutProcessArgsErr<LayoutScale> LayoutScaleArgsErr;


int LayoutScale::Process(const vector<string>& args)
	{
	if (args.size() != 8)
		throw LayoutScaleArgsErr();

	auto_ptr<TLayParseLayout> sourceLayout(TLayParseLayout::Parse(args[2]));
	int sourceWidth = CdlTkUtil::ParseInt(args[3]);
	int sourceHeight = CdlTkUtil::ParseInt(args[4]);

	string destLayout = args[5];
	int destWidth = CdlTkUtil::ParseInt(args[6]);
	int destHeight = CdlTkUtil::ParseInt(args[7]);

	LayoutScale scale(*sourceLayout, sourceWidth, sourceHeight, destLayout, destWidth, destHeight);
	scale.Run();

	return 0;
	}

void LayoutScale::ShowHelp(ostream& stream)
	{
	stream << "LayScale <sourceLayout> <sourceWidth> <sourceHeight> <destLayout> <destWidth> <destHeight>" << endl;
	stream << "  This performs an arithmetic scaling on all dimensional values in a layout." << endl;
	stream << "  This will not produce a pixel perfect scaling effect due to rounding errors," << endl;
	stream << "  however it may be sufficient for prototyping and demonstration purposes." << endl;
	}

LayoutScale::LayoutScale(TLayout& aSourceLayout, int aSourceWidth, int aSourceHeight, const string& aDestLayoutName, int aDestWidth, int aDestHeight)
:	iLayout(aSourceLayout),
	iSourceWidth(aSourceWidth),
	iSourceHeight(aSourceHeight),
	iDestLayoutName(aDestLayoutName),
	iDestWidth(aDestWidth),
	iDestHeight(aDestHeight)
	{
	}

void LayoutScale::Run()
	{
	RescaleLayout();
	WriteLayout();
	}

const string KHoriz = "lrW";
const string KVert = "tbBH";
const string KFont = "Font";
const string KScaledSuffix = "_Scaled";

void LayoutScale::RescaleLayout()
	{
	for (TLayout::iterator pTab = iLayout.begin(); pTab != iLayout.end(); ++pTab)
		{
		TLayoutTable& table = **pTab;
		for (TLayoutTable::iterator pLine = table.begin(); pLine != table.end(); ++pLine)
			{
			TLayoutLine& line = **pLine;
			for (TLayoutLine::iterator pCell = line.begin(); pCell != line.end(); ++pCell)
				{
				const string& cellName = pCell->first;
				TValues& values = pCell->second;
                if ( cellName == KFont)
                    {
				    for (TValues::iterator pVal = values.begin(); pVal != values.end(); ++pVal)
					    {
					    TagScaledToken(*pVal);
					    }                   
                    }
                else
                    {
   				    TScale dir = EHoriz;
				    if (KVert.find(cellName) != string::npos)
				        dir = EVert;
				    else if (KHoriz.find(cellName) == string::npos)
				        continue;

				    for (TValues::iterator pVal = values.begin(); pVal != values.end(); ++pVal)
					    {
					    RescaleValue(*pVal, dir);
					    }
                    }
				}
			}
		}
	}

void LayoutScale::RescaleValue(string& aVal, TScale aDir)
	{
	// find all numbers in this value and rescale them, copy non-numeric text directly.
	string result;
	string num;
	int count = aVal.size();
	for (int pos = 0; pos < count; pos++)
		{
		char c = aVal[pos];
		if (CdlTkUtil::IsNumeric(c))
			{
			num += c;
			}
		else
			{
			result += RescaleNum(num, aDir);
			num = "";
			result += c;
			}
		}
	result += RescaleNum(num, aDir);
	aVal = result;
	}

void LayoutScale::TagScaledToken(string& aToken)
	{
	string result(aToken);
	result += KScaledSuffix;
	aToken = result;
	}

string LayoutScale::RescaleNum(string& aNum, TScale aDir)
	{
	if (aNum.empty())
		return aNum;

	int num = CdlTkUtil::ParseInt(aNum);
	if (aDir == EHoriz)
		num = Scale(num, iDestWidth, iSourceWidth);
	else
		num = Scale(num, iDestHeight, iSourceHeight);
	return CdlTkUtil::IntToString(num);
	}

void LayoutScale::WriteLayout()
	{
	TLayoutWriter writer(iLayout, iDestLayoutName);
	writer.Write("");
	}

int LayoutScale::Scale(int aNumber, int aNumerator, int aDenominator)
	{
	// round to nearest whole number, rounding up on .5
	int num = (aNumber * aNumerator) / aDenominator;		// rounded down
	int remainder = (aNumber * aNumerator) % aDenominator;	// remainder
	
	if ((remainder*2) >= aDenominator)
		num++;				// round up if remainder >= half

	return num;
	}


