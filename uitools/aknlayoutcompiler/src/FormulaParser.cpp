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
#include "FormulaParser.h"


ParseResult ParseFormula(const string& aFormula)
	{
	Parser cell_nameParser = Parser("B") | Parser("H") | Parser("L") | Parser("R") | Parser("T") | Parser("W") | Parser("b") | Parser("h") | Parser("l") | Parser("r") | Parser("t") | Parser("w");
	cell_nameParser.SetId(cell_name_ParserId);
	Parser parentParser = Parser("P");
	parentParser.SetId(parent_ParserId);
	Parser parent_cellParser = parentParser >> Parser("_") >> cell_nameParser;
	parent_cellParser.SetId(parent_cell_ParserId);
	Parser table_cellParser = Parser::Real();
	table_cellParser.SetId(table_cell_ParserId);
	Parser comp_cellParser = Parser::Real() >> Parser("_") >> cell_nameParser;
	comp_cellParser.SetId(comp_cell_ParserId);
	Parser abs_cellParser = 
		Parser("$") >> 
		Parser::Real() >> 
		Parser("!") >> 
		Parser::Real() >> 
		Parser("!") >> 
		cell_nameParser >> 
		Parser("!$");
	abs_cellParser.SetId(abs_cell_ParserId);

	// types of value
	Parser cellParser = 
		cell_nameParser | 
		parent_cellParser | 
		parentParser | 
		comp_cellParser | 
		abs_cellParser | 
		table_cellParser;

	Parser unitsParser = 
		Parser::Real() >> 
		(Parser("u") | Parser("U"));
	unitsParser.SetId(units_ParserId);

	Parser constantParser = 
		Parser::Real() >> 
		Parser("p");
	constantParser.SetId(constant_ParserId);

	Parser attributeParser = 
		Parser("pr") >> 
		Parser::Int();
	attributeParser.SetId(attribute_ParserId);

	Parser mysteryParser = 
		Parser::Int() >> 
		Parser("pr") >> 
		Parser::Int();
	mysteryParser.SetId(mystery_ParserId);

	// functions
	Parser expressionParser;
	Parser func_nameParser = Parser("CEIL") | Parser("FLOOR") | Parser("ROUND");
	Parser funcParser = func_nameParser >> Parser("(") >> expressionParser >> Parser(")");
	funcParser.SetId(func_ParserId);

	Parser valueParser = 
		unitsParser | 
		mysteryParser | 
		attributeParser | 
		constantParser | 
		funcParser | 
		cellParser ;

	// arithmetic expressions from boost spirit library example
	Parser groupParser = Parser("(") >> expressionParser >> Parser(")");
	groupParser.SetId(group_ParserId);
	Parser factorParser = valueParser | groupParser;

	Parser termExtStarParser;
	Parser termExtParser = 
		((Parser("*") >> factorParser) | 
		 (Parser("/") >> factorParser)) >>
		termExtStarParser;
	termExtStarParser = termExtParser | Parser::Nul();

	Parser termParser = 
		factorParser >> 
		termExtStarParser;
	termParser.SetId(term_ParserId);

	Parser expressionExtStarParser;
	Parser expressionExtParser = 
		((Parser("+") >> termParser) | 
		 (Parser("-") >> termParser)) >>
		expressionExtStarParser;
	expressionExtStarParser = expressionExtParser | Parser::Nul();

	expressionParser = 
		termParser >> 
		expressionExtStarParser;
	expressionParser.SetId(expression_ParserId);

	// comparisons
	Parser comp_opParser = Parser(">=") | Parser("<=") | Parser("=") | Parser(">") | Parser("<");

	Parser whitespaceParser;
	Parser compParser = 
		expressionParser >> 
		whitespaceParser >> 
		comp_opParser >> 
		whitespaceParser >> 
		expressionParser;
	compParser.SetId(comp_ParserId);

	Parser comp_groupParser = Parser("(") >> compParser >> Parser(")");
	comp_groupParser.SetId(comp_group_ParserId);
	Parser compOptParser = compParser | comp_groupParser;

	// conditionals
	Parser conditionalParser = 
		Parser("IF ") >> 
		compOptParser >> 
		Parser(" THEN ") >> 
		expressionParser >> 
		Parser(" ELSE ") >> 
		expressionParser;
	conditionalParser.SetId(conditional_ParserId);


	Parser whitespaceExtParser = Parser(" ") >> whitespaceParser;
	whitespaceParser = whitespaceExtParser | Parser::Nul();

	// formula
	Parser formulaParser = conditionalParser | expressionParser;


	return formulaParser.Parse(aFormula);
	}
