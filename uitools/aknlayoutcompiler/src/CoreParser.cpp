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
#include "CoreParser.h"
#include <sstream>
#include <iostream>
using namespace std;


ParseResult::ParseResult()
: iRuleId(0), iStart(0), iEnd(0)
	{
	}

bool ParseResult::Matched() const
	{
	return iRuleId != 0;
	}

ParseResult ParseResult::Fail()
	{
	ParseResult p;
	return p;
	}


Parser::Parser()
: iOp(ENul), iMatch(0), iSub1(0), iSub2(0), iId(-1)
	{
	}

Parser::Parser(const char* aMatch)
: iOp(EExact), iMatch(aMatch), iSub1(0), iSub2(0), iId(-1)
	{
	}

Parser::Parser(T0Op aOp)
: iOp(aOp), iMatch(0), iSub1(0), iSub2(0), iId(-1)
	{
	}

Parser::Parser(T1Op aOp, const Parser& aSub)
: iOp(aOp), iMatch(0), iSub1(&aSub), iSub2(0), iId(-1)
	{
	}

Parser::Parser(T2Op aOp, const Parser& aFirst, const Parser& aRest)
: iOp(aOp), iMatch(0), iSub1(&aFirst), iSub2(&aRest), iId(-1)
	{
	}

Parser::Parser(const Parser& aOther)
	{
	*this = aOther;
	}

const Parser& Parser::operator=(const Parser& aOther)
	{
	if (this == &aOther)
		return *this;

	iOp = aOther.iOp;
	iMatch = aOther.iMatch;
	iSub1 = 0;
	iSub2 = 0;
	iSub1 = aOther.iSub1;
	iSub2 = aOther.iSub2;
	iId = aOther.iId;
//	if (aOther.iSub1)
//		iSub1 = new Parser(*aOther.iSub1);
//	if (aOther.iSub2)
//		iSub2 = new Parser(*aOther.iSub2);

	return *this;
	}

Parser::~Parser()
	{
//	delete iSub1;
//	delete iSub2;
	}

Parser Parser::EOS()
	{
	return Parser(EEos);
	}

Parser Parser::Int()
	{
	return Parser(EInt);
	}

Parser Parser::Real()
	{
	return Parser(EReal);
	}

const Parser& Parser::Nul()
	{
	static Parser nul(ENul);
	nul.SetId(0);
	return nul;
	}


ParseResult Parser::Parse(const std::string& aString) const
	{
	vector<Step> stack;
	stack.push_back(Step(this, 0, -1));
	TMatchRes res = EContinue;
	bool done = false;

	while (!stack.empty() && !done)
		{
		Step& top = stack.back();
		int topId = stack.size()-1;
		switch (res)
			{
			case EFail:
				if (top.iRule->iOp == EAlt && top.iStep < 2)
					{
					res = top.iRule->ParseStep(topId, aString, top.iPos, stack);
					}
				else if (top.iRule->iOp == EMult && top.iStep < 2)
					{
					top.iStep = 2;
					res = top.iRule->ParseStep(topId, aString, top.iPos, stack);
					}
				else
					{
					if (top.iParent >= 0 && stack[top.iParent].iRule->iOp == ESeq)
						stack[top.iParent].iStep--;
					stack.pop_back();
					}
				break;

			case EPass:
				{
				int nextSeq = -1;
				for (int i=stack.size()-1; nextSeq==-1 && i>=0; i--)
					{
					if ((stack[i].iRule->iOp == ESeq && stack[i].iStep < 2) || 
						(stack[i].iRule->iOp == EMult && stack[i].iStep < 3))
						nextSeq = i;
					}
				if (nextSeq >= 0)
					res = stack[nextSeq].iRule->ParseStep(nextSeq, aString, top.iResult.iEnd, stack);
				else
					done = true;
				break;
				}

			case EContinue:
				res = top.iRule->ParseStep(topId, aString, top.iPos, stack);
				break;
			}
		}

	if (done)
		{
		for (int i=stack.size()-1; i>=1; i--)
			{
			Step& step = stack[i];
			Step& parent = stack[step.iParent];
			parent.iResult.iChildren.insert(0,step.iResult);//push_front
			if (parent.iResult.iEnd < step.iResult.iEnd)
				parent.iResult.iEnd = step.iResult.iEnd;
			}

		return stack[0].iResult;
		}
	else
		return ParseResult::Fail();
	}

Parser::TMatchRes Parser::ParseStep(int aStepId, const std::string& aString, int aPos, vector<Step>& aStack) const
	{
	Step& step = aStack[aStepId];
	step.iResult.iRuleId = Id();
	
	switch (iOp)
		{
		case EExact:
			{
			string match(iMatch);
			unsigned int mLen = match.size();
			if (mLen + aPos > aString.size())
				return EFail;
			else if (aString.substr(aPos, mLen) != match)
				return EFail;
			else
				step.iResult.iEnd = aPos + mLen;
			return EPass;
			}

		case EEos:
			{
			if (static_cast<unsigned int>( aPos ) != aString.size())
				return EFail;
			else
				step.iResult.iEnd = aPos;
			return EPass;
			}

		case EInt:
			{
			istringstream strm(aString.substr(aPos));
			int temp;
			strm >> temp;
			int len = strm.tellg();
			if (len >= 0)
				step.iResult.iEnd = aPos + len;
			else
				return EFail;
			return EPass;
			}

		case EReal:
			{
			istringstream strm(aString.substr(aPos));
			double temp;
			strm >> temp;
			int len = strm.tellg();
			if (len >= 0)
				step.iResult.iEnd = aPos + len;
			else
				return EFail;
			return EPass;
			}

		case ENul:
			{
			step.iResult.iChildren.clear();
			step.iResult.iEnd = aPos;
			return EPass;
			}

		case EMult:
			{
/*			Step next(iSub1, aPos, aStepId);
			if (step.iStep == 1)
				next.iRule = this;
			else if (step.iStep == 2)
				next.iRule = &Parser::Nul();
			step.iStep++;
			aStack.push_back(next);
			return EContinue;
*/
			return EFail;
			}

		case ESeq:
			{
			Step next(iSub1, aPos, aStepId);
			if (step.iStep == 1)
				next.iRule = iSub2;
			step.iStep++;
			aStack.push_back(next);
			return EContinue;
			}

		case EAlt:
			{
			Step next(iSub1, aPos, aStepId);
			if (step.iStep == 1)
				next.iRule = iSub2;
			step.iStep++;
			aStack.push_back(next);
			return EContinue;
			}
		}

	return EFail;
	}

int Parser::Id() const
	{
	return iId;
	}

void Parser::SetId(int aId)
	{
	iId = aId;
	}


Parser operator|(const Parser& aFirst, const Parser& aRest)
	{
	return Parser(Parser::EAlt, aFirst, aRest);
	}

Parser operator>>(const Parser& aFirst, const Parser& aRest)
	{
	return Parser(Parser::ESeq, aFirst, aRest);
	}

/*Parser operator*(const Parser& aSub)
	{
	return Parser(Parser::EMult, aSub);
	}
*/

void DoPrint(const ParseResult& res)
	{
	cout << res.iRuleId << " " << res.iStart << "..." << res.iEnd << " ";
	if (res.iChildren.size())
		{
		cout << "{ ";
		for (unsigned int i=0; i<res.iChildren.size(); i++)
			DoPrint(res.iChildren[i]);
		cout << "} ";
		}
	}

void Print(const ParseResult& res)
	{
	DoPrint(res);
	cout << endl;
	}

int TestParser()
	{
	Parser p = Parser("A");
	ParseResult res;
	res = p.Parse("A");	Print(res);
	res = p.Parse("B");	Print(res);

	p = Parser("A") | Parser("B");
	res = p.Parse("A");	Print(res);
	res = p.Parse("B");	Print(res);
	res = p.Parse("AB"); Print(res);
	res = p.Parse("C");	Print(res);

	p = Parser("A") | Parser("B") | Parser("C");
	res = p.Parse("A");	Print(res);
	res = p.Parse("B");	Print(res);
	res = p.Parse("AB"); Print(res);
	res = p.Parse("C");	Print(res);
	res = p.Parse("D");	Print(res);

	p = Parser("A") >> Parser("B");
	res = p.Parse("A");	Print(res);
	res = p.Parse("B"); Print(res);
	res = p.Parse("AB"); Print(res);
	res = p.Parse("BA"); Print(res);
	res = p.Parse("C"); Print(res);

	p = Parser("A") >> Parser("B") >> Parser("C");
	res = p.Parse("A");	Print(res);
	res = p.Parse("B"); Print(res);
	res = p.Parse("AB"); Print(res);
	res = p.Parse("ABC"); Print(res);
	res = p.Parse("ABCD"); Print(res);
	res = p.Parse("BAC"); Print(res);
	res = p.Parse("D"); Print(res);

	p = Parser("A") >> Parser("B") >> Parser("A") >> Parser::EOS();
	res = p.Parse("A"); Print(res);
	res = p.Parse("B"); Print(res);
	res = p.Parse("AB"); Print(res);
	res = p.Parse("BA"); Print(res);
	res = p.Parse("ABA"); Print(res);
	res = p.Parse("ABAB"); Print(res);
	res = p.Parse("C"); Print(res);

	p = (Parser("A") | Parser("B")) >> Parser("C");
	res = p.Parse("A");	Print(res);
	res = p.Parse("B"); Print(res);
	res = p.Parse("AC"); Print(res);
	res = p.Parse("BC"); Print(res);
	res = p.Parse("BA"); Print(res);
	res = p.Parse("C"); Print(res);
	res = p.Parse("CC"); Print(res);

	p = Parser("A") >> (Parser("B") | Parser("BB")) >> Parser::EOS();
	res = p.Parse("A"); Print(res);
	res = p.Parse("AB"); Print(res);
	res = p.Parse("ABB"); Print(res);
	res = p.Parse("ABBB"); Print(res);

	p = Parser("A") >> (Parser("B") | Parser("C")) >> (Parser("D") | Parser("E")) >> Parser::EOS();
	res = p.Parse("A"); Print(res);
	res = p.Parse("AB"); Print(res);
	res = p.Parse("AC"); Print(res);
	res = p.Parse("AD"); Print(res);
	res = p.Parse("AE"); Print(res);
	res = p.Parse("ABD"); Print(res);
	res = p.Parse("ABE"); Print(res);
	res = p.Parse("ACD"); Print(res);
	res = p.Parse("ACE"); Print(res);
	res = p.Parse("ADB"); Print(res);
	res = p.Parse("AEB"); Print(res);
	res = p.Parse("ADC"); Print(res);
	res = p.Parse("AEC"); Print(res);
	res = p.Parse("ABDB"); Print(res);
	res = p.Parse("ABEC"); Print(res);
	res = p.Parse("ACDE"); Print(res);
	res = p.Parse("ACEE"); Print(res);

	p = Parser("A") | Parser("B");
	Parser q = p >> p;
	cout << p.Id() << endl;
	cout << q.Id() << endl;
	res = q.Parse("A"); Print(res);
	res = q.Parse("B"); Print(res);
	res = q.Parse("AA"); Print(res);
	res = q.Parse("AB"); Print(res);
	res = q.Parse("BA"); Print(res);
	res = q.Parse("BB"); Print(res);
	res = q.Parse("AC"); Print(res);
	res = q.Parse("CA"); Print(res);

	q = Parser("A") >> p;
	p = q | Parser::Nul();
	res = p.Parse("A"); Print(res);
	res = p.Parse("B"); Print(res);
	res = p.Parse("AA"); Print(res);
	res = p.Parse(""); Print(res);

	Parser r = Parser::Nul();
	q = Parser("A") >> r;
	r = q | Parser::Nul();
	p = r >> Parser::EOS();
	res = p.Parse("A"); Print(res);
	res = p.Parse("B"); Print(res);
	res = p.Parse("AA"); Print(res);
	res = p.Parse("AAB"); Print(res);
	res = p.Parse(""); Print(res);

	p = Parser("B") >> r >> Parser("B");
	res = p.Parse("B"); Print(res);
	res = p.Parse("BB"); Print(res);
	res = p.Parse("BAB"); Print(res);
	res = p.Parse("BAAB"); Print(res);
	res = p.Parse("BAAAB"); Print(res);

	p = r >> Parser("AAB");
	res = p.Parse("B"); Print(res);
	res = p.Parse("AB"); Print(res);
	res = p.Parse("AAB"); Print(res);
	res = p.Parse("AAAB"); Print(res);
	res = p.Parse("AAAAB"); Print(res);

	p = Parser::Int();
	res = p.Parse("1"); Print(res);
	res = p.Parse("a"); Print(res);
	res = p.Parse("123"); Print(res);
	res = p.Parse("123ab"); Print(res);
	res = p.Parse(""); Print(res);
	
	p = Parser::Real();
	res = p.Parse("1"); Print(res);
	res = p.Parse("1.0"); Print(res);
	res = p.Parse(".01"); Print(res);
	res = p.Parse("1e9"); Print(res);
	res = p.Parse("a"); Print(res);
	res = p.Parse("123"); Print(res);
	res = p.Parse("123ab"); Print(res);
	res = p.Parse(""); Print(res);
	

	// unsupported *
/*	p = *Parser("A");
	res = p.Parse("A"); Print(res);
	res = p.Parse("B"); Print(res);
	res = p.Parse("AA"); Print(res);
	res = p.Parse(""); Print(res);

	p = *Parser("A") >> Parser::EOS();
	res = p.Parse("A"); Print(res);
	res = p.Parse("B"); Print(res);
	res = p.Parse("AA"); Print(res);
	res = p.Parse("AAB"); Print(res);
	res = p.Parse(""); Print(res);

	p = Parser("B") >> *Parser("A") >> Parser("B");
	res = p.Parse("B"); Print(res);
	res = p.Parse("BB"); Print(res);
	res = p.Parse("BAB"); Print(res);
	res = p.Parse("BAAB"); Print(res);
	res = p.Parse("BAAAB"); Print(res);

	p = *Parser("A") >> Parser("AB");
	res = p.Parse("B"); Print(res);
	res = p.Parse("AB"); Print(res);
	res = p.Parse("AAB"); Print(res);
	res = p.Parse("AAAB"); Print(res);
	res = p.Parse("AAAAB"); Print(res);
*/
	return 0;
	}
/*
Parser p = Parser("A") >> (Parser("B") | Parser("C")) >> (Parser("D") | Parser("E")) >> Parser("F");
Parser q = p | (p >> q);
int TestFormulaTreeNode()
	{

	ParseResult res;
	res = q.Parse("ACEFACEF");	Print(res);
	return 0;
	}
*/
// note, uncomment this line to execute test code
//int x = TestParser();
