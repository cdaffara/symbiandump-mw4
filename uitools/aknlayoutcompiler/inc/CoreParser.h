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
#ifndef COREPARSER_H
#define COREPARSER_H

#include <string>
#include <vector>

class Parser;

class ParseResult
	{
public:
	ParseResult();
	static ParseResult Fail();

	bool Matched() const;

public:
	int iRuleId;
	int iStart;
	int iEnd;
	std::vector<ParseResult> iChildren;
	};


class Parser
	{
public:
	Parser(const char* aMatch);
	static Parser EOS();
	static Parser Int();
	static Parser Real();
	static const Parser& Nul();

	ParseResult Parse(const std::string& aString) const;

	int Id() const;
	void SetId(int aId);

	Parser();
	Parser(const Parser& aOther);
	~Parser();
	const Parser& operator=(const Parser& aOther);

private:
	friend Parser operator|(const Parser& aFirst, const Parser& aRest);
	friend Parser operator>>(const Parser& aFirst, const Parser& aRest);
	friend Parser operator*(const Parser& aSub);

	enum T0Op
		{
		EExact,
		EEos,
		EInt,
		EReal,
		ENul,
		ELast0
		};
	enum T1Op
		{
		EMult = ELast0,
		ELast1
		};
	enum T2Op
		{
		ESeq = ELast1,
		EAlt
		};

	Parser(T0Op aOp);
	Parser(T1Op aOp, const Parser& aSub);
	Parser(T2Op aOp, const Parser& aFirst, const Parser& aRest);

	struct Step
		{
		Step(const Parser* aRule, int aPos, int aParent)
			: iRule(aRule), iStep(0), iPos(aPos), iParent(aParent)
			{
			iResult.iStart = aPos;
			}
		const Parser* iRule;
		int iStep;
		ParseResult iResult;
		int iPos;
		int iParent;
		};

	enum TMatchRes
		{
		EFail,
		EPass,
		EContinue
		};

	TMatchRes ParseStep(int aStep, const std::string& aString, int aPos, std::vector<Step>& aStack) const;

private:
	int iOp;
	const char* iMatch;
	const Parser* iSub1;
	const Parser* iSub2;
	int iId;
	};

Parser operator|(const Parser& aFirst, const Parser& aRest);
Parser operator>>(const Parser& aFirst, const Parser& aRest);
//Parser operator*(const Parser& aSub);

#endif
