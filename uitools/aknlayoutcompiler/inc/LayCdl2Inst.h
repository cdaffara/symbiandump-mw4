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


#ifndef LAYCDL2INST_H
#define LAYCDL2INST_H

#include <string>
#include <vector>
#include <iosfwd>
#include "Layout.h"
using namespace std;

#include <CdlCompilerToolkit/CdlTkInterface.h>
#include <CdlCompilerToolkit/CdlTkInstance.h>
#include <CdlCompilerToolkit/CdlTkProcess.h>
using namespace CdlCompilerToolkit;


/**
*  CLayoutToInst 
*  Generate customisation instance for a set of layouts which use a particular CDL
*  interface (unoptimised)
*/
class CLayoutToInst : public CCdlTkSourceFileWriter
	{
public:
	CLayoutToInst(CLayoutToInst* aBase, CCdlTkInterface& aInterface, auto_ptr<TLayout>& aLayout, const string& aInstName);
	~CLayoutToInst();

	CCdlTkInstance& Instance();
	string FwdDeclName();
	void Process();

private:
	void AddTableToInstance(TLayoutTable& aTable);
	void AddWindowLineToInstance(TLayoutLine& aLine);
	void AddTextLineToInstance(TLayoutLine& aLine);
	void AddTableLimitsToInstance(TLayoutTable& aTable, TLayoutTable::TLayoutSubTable& aSubTable, int aTableNum);
	void AddSubTableToInstance(const string& aType, TLayoutTable& aTable, TLayoutTable::TLayoutSubTable& aSubTable, int aTableNum);
	bool HasApi(const string& aName);
	CCdlTkImplementation& FindImp(const string& aName);
	void SetDataLine(CCdlTkImplementation& aImp, TLayoutLine& aLine, const string& aType, const string aOutputOrder[], int aOutputOrderSize);
	void SetFuncLine(CCdlTkImplementation& aImp, TLayoutLine& aLine, const string& aType, const string aOutputOrder[], int aOutputOrderSize, const string& aPreExtra, const string& aPostExtra);
	void MirrorParamNames(std::string& aNames);
	void AddFwdRefToInstance(const string& aName, bool aCast);

private:
	CLayoutToInst* iBase;
	CCdlTkInterface& iInterface;
	TLayout* iLayoutPtr;
	TLayout& iLayout;
	CCdlTkInstance* iInstance;
	std::string iInstName;
	};

class LayoutAndCdlToCdlInstance
	{
public:
	static int Process(const vector<string>& args);
	static void ShowHelp(ostream& stream);
	static void CheckLayoutAgainstInterface(const TLayout& aLayout, const CCdlTkInterface& aIface);

	LayoutAndCdlToCdlInstance(CCdlTkInterface& aInterface);
	~LayoutAndCdlToCdlInstance();

	void AddLayout(auto_ptr<TLayout>& aLayout, const string& aInstName);
	void WriteInstances();

private:
	typedef vector<CLayoutToInst*> CLayouts;
	CLayouts iLayouts;
	CCdlTkInterface& iInterface;
	};

#endif
