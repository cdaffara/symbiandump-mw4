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
#include "FormulaTree.h"
#include "FormulaParser.h"
#include <sstream>
#include <iostream>
using namespace std;

FormulaTreeNode* FormulaTreeNode::Parse(const string& aFormula)
	{
	return NewTree(ParseFormula(aFormula), aFormula);
	}

FormulaTreeNode::~FormulaTreeNode()
	{
	for (int i=0; i<Size(); i++)
		delete iSubNodes[i];
	}

int FormulaTreeNode::Size() const
	{
	return iSubNodes.size();
	}

FormulaTreeNode& FormulaTreeNode::operator[](int aIndex)
	{
	return *iSubNodes[aIndex];
	}

const FormulaTreeNode& FormulaTreeNode::operator[](int aIndex) const
	{
	return *iSubNodes[aIndex];
	}

FormulaTreeNode::TNodeType FormulaTreeNode::Type() const
	{
	return iType;
	}

string FormulaTreeNode::Text() const
	{
	return iSource.substr(iStart, iLen);
	}

char FormulaTreeNode::Char() const
	{
	return iSource[iStart];
	}

int FormulaTreeNode::Int() const
	{
	int r;
	stringstream(Text()) >> r;
	return r;
	}

double FormulaTreeNode::Real() const
	{
	double r;
	stringstream(Text()) >> r;
	return r;
	}

FormulaTreeNode* FormulaTreeNode::NewTree(const ParseResult& aParse, const string& aFormula)
	{
	FormulaTreeNode* node = 0;

	int parseId = aParse.iRuleId;
	if (parseId == cell_name_ParserId)
		{
		node = new FormulaTreeNode(ECell, aFormula, aParse.iStart, aParse.iEnd-aParse.iStart);
		}
	else if (parseId == parent_ParserId)
		{
		node = new FormulaTreeNode(EParent, aFormula, aParse.iStart, aParse.iEnd-aParse.iStart);
		}
	else if (parseId == parent_cell_ParserId)
		{
		const ParseResult& cell = aParse.iChildren[1];
		node = new FormulaTreeNode(EParentCell, aFormula, cell.iStart, cell.iEnd-cell.iStart);
		}
	else if (parseId == table_cell_ParserId)
		{
		node = new FormulaTreeNode(ETableCell, aFormula, aParse.iStart, aParse.iEnd-aParse.iStart);
		}
	else if (parseId == comp_cell_ParserId)
		{
		const ParseResult& cellId = aParse.iChildren[0].iChildren[0];
		node = new FormulaTreeNode(EComponent, aFormula, cellId.iStart, cellId.iEnd-cellId.iStart);
		node->iSubNodes.push_back(NewTree(aParse.iChildren[1], aFormula));
		}
	else if (parseId == abs_cell_ParserId)
		{
		node = new FormulaTreeNode(EAbsolute, aFormula, aParse.iStart, aParse.iEnd-aParse.iStart);
		const ParseResult& part1 = aParse.iChildren[0].iChildren[0].iChildren[0].iChildren[0].iChildren[0].iChildren[1];
		node->iSubNodes.push_back(new FormulaTreeNode(EReal, aFormula, part1.iStart, part1.iEnd-part1.iStart));
		const ParseResult& part2 = aParse.iChildren[0].iChildren[0].iChildren[0].iChildren[1];
		node->iSubNodes.push_back(new FormulaTreeNode(EReal, aFormula, part2.iStart, part2.iEnd-part2.iStart));
		const ParseResult& part3 = aParse.iChildren[0].iChildren[1];
		node->iSubNodes.push_back(NewTree(part3, aFormula));
		}
	else if (parseId == units_ParserId)
		{
		const ParseResult& units = aParse.iChildren[0];
		node = new FormulaTreeNode(EUnits, aFormula, units.iStart, units.iEnd-units.iStart);
		}
	else if (parseId == constant_ParserId)
		{
		const ParseResult& constant = aParse.iChildren[0];
		node = new FormulaTreeNode(EConstant, aFormula, constant.iStart, constant.iEnd-constant.iStart);
		}
	else if (parseId == attribute_ParserId)
		{
		const ParseResult& attrib = aParse.iChildren[1];
		node = new FormulaTreeNode(EAttribute, aFormula, attrib.iStart, attrib.iEnd-attrib.iStart);
		}
	else if (parseId == mystery_ParserId)
		{
		node = new FormulaTreeNode(EMystery, aFormula, aParse.iStart, aParse.iEnd-aParse.iStart);
		const ParseResult& part1 = aParse.iChildren[0].iChildren[0];
		node->iSubNodes.push_back(new FormulaTreeNode(EInt, aFormula, part1.iStart, part1.iEnd-part1.iStart));
		const ParseResult& part2 = aParse.iChildren[1];
		node->iSubNodes.push_back(new FormulaTreeNode(EInt, aFormula, part2.iStart, part2.iEnd-part2.iStart));
		}
	else if (parseId == func_ParserId)
		{
		const ParseResult& funcName = aParse.iChildren[0].iChildren[0].iChildren[0];
		node = new FormulaTreeNode(EFunction, aFormula, funcName.iStart, funcName.iEnd-funcName.iStart);
		const ParseResult& expr= aParse.iChildren[0].iChildren[1];
		node->iSubNodes.push_back(NewTree(expr, aFormula));
		}
	else if (parseId == group_ParserId)
		{
		node = NewTree(aParse.iChildren[0].iChildren[1], aFormula);
		}
	else if (parseId == term_ParserId || parseId == expression_ParserId)
		{
		node = NewTree(aParse.iChildren[0], aFormula);
		const ParseResult* parse = &aParse;
		while (parse->iChildren[1].iChildren[0].iRuleId != Parser::Nul().Id())
			{
			const ParseResult& oper = parse->iChildren[1].iChildren[0].iChildren[0].iChildren[0].iChildren[0];
			FormulaTreeNode* newNode = new FormulaTreeNode(EArithmetic, aFormula, oper.iStart, oper.iEnd-oper.iStart);
			newNode->iSubNodes.push_back(node);
			const ParseResult& right = parse->iChildren[1].iChildren[0].iChildren[0].iChildren[0].iChildren[1];
			newNode->iSubNodes.push_back(NewTree(right, aFormula));
			node = newNode;
			parse = &parse->iChildren[1].iChildren[0];
			}
		}
	else if (parseId == comp_group_ParserId)
		{
		node = NewTree(aParse.iChildren[0].iChildren[1], aFormula);
		}
	else if (parseId == comp_ParserId)
		{
		const ParseResult& oper = aParse.iChildren[0].iChildren[0].iChildren[1];
		node = new FormulaTreeNode(ECondition, aFormula, oper.iStart, oper.iEnd-oper.iStart);
		const ParseResult& lhs = aParse.iChildren[0].iChildren[0].iChildren[0].iChildren[0];
		node->iSubNodes.push_back(NewTree(lhs, aFormula));
		const ParseResult& rhs = aParse.iChildren[1];
		node->iSubNodes.push_back(NewTree(rhs, aFormula));
		}
	else if (parseId == conditional_ParserId)
		{
		node = new FormulaTreeNode(EConditional, aFormula, aParse.iStart, 0);
		const ParseResult& cond = aParse.iChildren[0].iChildren[0].iChildren[0].iChildren[0].iChildren[1];
		node->iSubNodes.push_back(NewTree(cond, aFormula));
		const ParseResult& thenExpr = aParse.iChildren[0].iChildren[0].iChildren[1];
		node->iSubNodes.push_back(NewTree(thenExpr, aFormula));
		const ParseResult& elseExpr = aParse.iChildren[1];
		node->iSubNodes.push_back(NewTree(elseExpr, aFormula));
		}
	else	// internal alt node, recurse down
		{
		node = NewTree(aParse.iChildren[0], aFormula);
		}

	return node;
	}

FormulaTreeNode::FormulaTreeNode(TNodeType aType, const string& aSource, int aStart, int aLen)
: iSource(aSource), iStart(aStart), iLen(aLen), iType(aType)
	{
	}

FormulaTreeNode::FormulaTreeNode(const FormulaTreeNode& aOther)
: iSource(aOther.iSource), iStart(aOther.iStart), iLen(aOther.iLen), iType(aOther.iType)
	{
	for (unsigned int i=0; i<iSubNodes.size(); i++)
		delete iSubNodes[i];
	iSubNodes.clear();
	for (unsigned int j=0; j<aOther.iSubNodes.size(); j++)
		iSubNodes.push_back(new FormulaTreeNode(*aOther.iSubNodes[j]));
	}

void FormulaTreeNode::Print(const FormulaTreeNode& aNode)
	{
	switch (aNode.Type())
		{
		case FormulaTreeNode::EReal:		// Double() = the number
			cout << aNode.Real();
			break;
		case FormulaTreeNode::EInt:			// Int() = the number
			cout << aNode.Int();
			break;
		case FormulaTreeNode::ECell:			// Char() = cell name
			cout << aNode.Char();
			break;
		case FormulaTreeNode::EParent:		// nothing special
			cout << aNode.Char();
			break;
		case FormulaTreeNode::EParentCell:	// Char() = parent cell name
			cout << "P_" << aNode.Char();
			break;
		case FormulaTreeNode::ETableCell:		// Double() = target table
			cout << aNode.Real();
			break;
		case FormulaTreeNode::EComponent:		// Double() = component id, [0] = cell name
			cout << aNode.Real() << "_";
			Print(aNode[0]);
			break;
		case FormulaTreeNode::EAbsolute:		// Text() = whole thing, [0], [1] = real components, [2] = cell name
			cout << "Abs(";
			Print(aNode[0]);
			cout << ",";
			Print(aNode[1]);
			cout << ",";
			Print(aNode[2]);
			cout << ")";
			break;
		case FormulaTreeNode::EUnits:			// Double() = units
			cout << aNode.Real() << "units";
			break;
		case FormulaTreeNode::EConstant:		// Double() = constant
			cout << "const(" << aNode.Real() << ")";
			break;
		case FormulaTreeNode::EAttribute:		// Int() = attribute
			cout << "attrib(" << aNode.Int() << ")";
			break;
		case FormulaTreeNode::EMystery:		// Text() = whole thing, [0], [1] = int components
			cout << "mystery(";
			Print(aNode[0]);
			cout << ",";
			Print(aNode[1]);
			cout << ")";
			break;
		case FormulaTreeNode::EFunction:		// Text() = function name, [0] = parameter
			cout << aNode.Text() << "(";
			Print(aNode[0]);
			cout << ")";
			break;
		case FormulaTreeNode::EArithmetic:	// Char() = arithmetic operator, [0], [1] = sub expressions
			cout << aNode.Char() << "(";
			Print(aNode[0]);
			cout << ",";
			Print(aNode[1]);
			cout << ")";
			break;
		case FormulaTreeNode::ECondition:		// Text() = comparison operator, [0], [1] = sub expressions
			cout << aNode.Text() << "(";
			Print(aNode[0]);
			cout << ",";
			Print(aNode[1]);
			cout << ")";
			break;
		case FormulaTreeNode::EConditional:	// no content, [0] = condition, [1] = then expression, [2] = else expression
			cout << "IF ";
			Print(aNode[0]);
			cout << " THEN ";
			Print(aNode[1]);
			cout << " ELSE ";
			Print(aNode[2]);
			break;
		};
	}

void Print(const ParseResult& res);


int TestFormulaTreeNode()
	{
	string formula("IF (P_W/25p*4.25p) <= P_H THEN P_W/25p*4.25p ELSE P_H");
//	string formula("1*2/3");
	Print(ParseFormula(formula));
	FormulaTreeNode* tree = FormulaTreeNode::Parse(formula);
	FormulaTreeNode::Print(*tree);
	delete tree;
	return 0;
	}

// int x = TestFormulaTreeNode()
