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
#ifndef FORMULATREE_H
#define FORMULATREE_H

#include <string>
#include <vector>
using namespace std;

class ParseResult;

class FormulaTreeNode
	{
public:
	enum TNodeType
		{
		EReal=1,		// Real() = the number
		EInt,			// Int() = the number
		ECell,			// Char() = cell name
		EParent,		// nothing special
		EParentCell,	// Char() = parent cell name
		ETableCell,		// Real() = target table
		EComponent,		// Real() = component id, [0] = cell name
		EAbsolute,		// Text() = whole thing, [0], [1] = real components, [2] = cell name
		EUnits,			// Real() = units
		EConstant,		// Real() = constant
		EAttribute,		// Int() = attribute
		EMystery,		// Text() = whole thing, [0], [1] = int components
		EFunction,		// Text() = function name, [0] = parameter
		EArithmetic,	// Char() = arithmetic operator, [0], [1] = sub expressions
		ECondition,		// Text() = comparison operator, [0], [1] = sub expressions
		EConditional	// no content, [0] = condition, [1] = then expression, [2] = else expression
		};

public:
	static FormulaTreeNode* Parse(const string& aFormula);
	virtual ~FormulaTreeNode();

	int Size() const;
	FormulaTreeNode& operator[](int aIndex);
	const FormulaTreeNode& operator[](int aIndex) const;
	
	TNodeType Type() const;

	string Text() const;
	char Char() const;
	int Int() const;
	double Real() const;

	FormulaTreeNode(const FormulaTreeNode& aOther);
	static void Print(const FormulaTreeNode& aNode);

private:
	FormulaTreeNode(TNodeType aType, const string& aSource, int aStart, int aLen);
	static FormulaTreeNode* NewTree(const ParseResult& aParse, const string& aFormula);
	const FormulaTreeNode& operator=(const FormulaTreeNode& aOther);

public:
	vector<FormulaTreeNode*> iSubNodes;

private:
	const string& iSource;
	int iStart;
	int iLen;
	TNodeType iType;
	};


#endif
