/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef MLEQCOMPDATA2DHUIML_H
#define MLEQCOMPDATA2DHUIML_H

#include <string>
#include <vector>
#include <iosfwd>
#include "MLEqCompData.h"
using namespace std;


class TEqLayoutSaxWriter;

/**
*  MLEqCompDataToDHuiML
*  Generate a CDL interface for a layout
*/
class MLEqCompDataToDHuiML
	{
public:
	static int Process(const vector<string>& args);
	static void ShowHelp(ostream& stream);

	static void LayoutTablesToDeclarative(TEqLayoutSaxWriter& aSaxWriter, const TMLEqCompData& aLayout);
	static void LayoutComponentsToDeclarative(TEqLayoutSaxWriter& aSaxWriter, const TMLEqCompData& aLayout);

	static void AddTableToDeclarative(TEqLayoutSaxWriter& aSaxWriter, TMLEqCompDataTable& aTable);
	static void AddLineToDeclarative(TEqLayoutSaxWriter& aSaxWriter, TMLEqCompDataLine& aLine, int aOptionSetId);
	static void AddCellToDeclarative(TEqLayoutSaxWriter& aSaxWriter, TMLEqCompDataValues& aValues, const string& aCellName, int aOptionSetId);
	static void AddFormulaToDeclarative(TEqLayoutSaxWriter& aSaxWriter, const TMLEqCompDataFormula& aFormula);

private:
	static void ConvertPositionValueStr(std::string& aValueStr);
	};


#endif // MLEQCOMPDATA2DHUIML_H
