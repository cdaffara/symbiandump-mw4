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


#ifndef LAYCDL2INSTO_H
#define LAYCDL2INSTO_H

#include <string>
#include <vector>
#include <iosfwd>
#include "Layout.h"
using namespace std;

#include <CdlCompilerToolkit/CdlTkInterface.h>
#include <CdlCompilerToolkit/CdlTkInstance.h>
#include <CdlCompilerToolkit/CdlTkProcess.h>
using namespace CdlCompilerToolkit;

class CLayoutInstOpt;
class CLayoutInstOptImpl;

typedef vector<CLayoutInstOptImpl*> CLayoutInstOptImpls;

/**
*  SImplFunc
*  This class represents a layout data decompression function local to a layout instance.
*  These functions will call the corresponding DLL wide function for decompression.
*/
struct SImplFunc
	{
	enum TFuncType
		{ 
		EWindowLine, 
		ETextLine,
		EMultilineTextLine,
		ETableLimits,
		EWindowTable, 
		ETextTable
		};
	TFuncType iType;
	int iParams;
	bool iParent;
	string iDefn;
	string iPtrRef;

	SImplFunc(TFuncType aType, int aParams, bool aParent, string aDefn, string aPtrRef)
	: iType(aType), iParams(aParams), iParent(aParent), iDefn(aDefn), iPtrRef(aPtrRef)
		{}
	};


/**
*  CAllFuncs
*  This represents a collection of all the SImplFunc objects that a layout instance may need
*/
class CAllFuncs : public vector<SImplFunc>
	{
public:
	CAllFuncs(int aMaxParams);

private:
	void AddLineFunc(SImplFunc::TFuncType aType, int aParams, bool aParent, const string& aReturn, const string& aFuncName);
	void AddTableFunc(SImplFunc::TFuncType aType, int aParams, bool aParent, const string& aReturn, const string& aFuncName);
	};


class CInstanceList
	{
public:
	// ProcessOptions will remove "-i<instanceListFile>" from aArgs, if it appears at position [2].
	void ProcessOptions(vector<string>& aArgs);
	bool IsInstanceOk(const string& aInstance) const;
private:
	set<string> iInstances;
	bool iLoaded;
	};

/**
*  LayoutCdlInstanceOpt
*  Generate a set of optimised layout instances for a number of layouts with a common
*  CDL interface
*/
class LayoutCdlInstanceOpt
	{
private:
	typedef vector<CLayoutInstOpt*> CLayouts;

public:
	static int Process(vector<string>& args);
	static void ShowHelp(ostream& stream);

	CCdlTkInterface& Interface();
	CLayouts& Layouts();
	CLayoutInstOptImpl* FindSimilarImpl(TLayoutLine& aLine);
	int FindSimilarBytes(CLayoutInstOptImpl* aImpl);
	void AddImpl(CLayoutInstOptImpl* aImpl);

private:
	LayoutCdlInstanceOpt(CCdlTkInterface& aIface);
	~LayoutCdlInstanceOpt();

	void AddLayout(auto_ptr<TLayout>& aLayout, const string& aInstName);
	void Process();
	void WriteInstances();
	void ProcessCommonImpl();
	bool LinesAreEqual(TLayoutLine& aLine1, TLayoutLine& aLine2);

private:
	CLayouts iLayouts;
	CCdlTkInterface& iInterface;
	CLayoutInstOptImpls iImpls;
	int iByteCodeIndex;
	vector<char> iBytesAggregated;
	};

#endif
