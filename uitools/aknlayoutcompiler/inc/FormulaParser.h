/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef FORMULA_PARSER_H
#define FORMULA_PARSER_H

#include <string>
#include <vector>
#include "CoreParser.h"
using namespace std;

enum TFormulaParserIds
	{
	cell_name_ParserId=1,
	parent_ParserId,
	parent_cell_ParserId,
	table_cell_ParserId,
	comp_cell_ParserId,
	abs_cell_ParserId,
	units_ParserId,
	constant_ParserId,
	attribute_ParserId,
	mystery_ParserId,
	func_ParserId,
	group_ParserId,
	term_ParserId,
	expression_ParserId,
	comp_group_ParserId,
	comp_ParserId,
	conditional_ParserId,
	};

ParseResult ParseFormula(const string& aFormula);

#endif
