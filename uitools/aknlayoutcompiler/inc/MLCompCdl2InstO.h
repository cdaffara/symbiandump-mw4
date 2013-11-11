/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef MLCOMPCDL2INSTO_H
#define MLCOMPCDL2INSTO_H

#include <string>
#include <vector>
#include <iosfwd>
#include "MLCompData.h"
#include "MLAttributes.h"
using namespace std;

#include <CdlCompilerToolkit/CdlTkInterface.h>
#include <CdlCompilerToolkit/CdlTkInstance.h>
#include <CdlCompilerToolkit/CdlTkProcess.h>
using namespace CdlCompilerToolkit;

// forward declares and typedefs
class CInstanceList;
class CMLCompDataInstOpt;
class CMLCompDataInstOptImpl;
typedef vector<CMLCompDataInstOptImpl*> CMLCompDataInstOptImpls;
class CZoomLevelNames;

/**
*  MLCompDataCdlInstanceOpt
*  Generate a set of optimised layout instances for a number of layouts with a common
*  CDL interface
*/
class MLCompDataCdlInstanceOpt
	{
private:
	struct InstStruct
		{
		InstStruct(string aInstName, TMLCompData* aInst, TMLAttributes* aAttribs);
		string iInstName;
		TMLCompData* iInst;
		TMLAttributes* iAttribs;
		};

	class InstList : public vector<InstStruct>
		{
	public:
		~InstList();
		};

	typedef vector<TMLCompData*> CCompDataLayouts;
	typedef vector<CMLCompDataInstOpt*> CCompDatas;
	typedef map<int, CCompDatas> CCompDataZoomLevelDatas;
public:
	static int Process(vector<string>& args);
	static void ShowHelp(ostream& stream);

public:
	CCdlTkInterface& Interface();
	int FindSimilarBytes(CMLCompDataInstOptImpl* aImpl, int aBaseOffset);
	void AddImpl(CMLCompDataInstOptImpl* aImpl);
	void ResizeByteStream(int aSize);
	int ByteStreamSize() const;


private:
	static void ProcessSeparators(vector<string>& args, vector<int>& aSeparators);
	static bool CheckForUsedInstances(const CInstanceList& aUsedList, const CZoomLevelNames& aZoomLevelNames, const vector<string>& aArgs, const vector<int>& aSeparators, int aSepIndex);
	static void ParseInstances(const vector<string>& aArgs, const vector<int>& aSeparators, int aSepIndex, InstList& aInstList);
	static void MergeLayouts(CInstanceList& aInstUsedList, CZoomLevelNames& aZoomLevelNames, const InstList& aInstances, InstList& aMergedLayouts);

private:
	MLCompDataCdlInstanceOpt(CCdlTkInterface& aIface);
	~MLCompDataCdlInstanceOpt();

	void AddInst(const InstStruct& aInstStruct, CZoomLevelNames& aZoomLevelNames, bool aAllParams, bool aNonRomDelivery);
	void Process();
	void WriteInstances();
	void ProcessCommonImpl();

private:
	CCompDataLayouts iLayouts; // the comp datas are owned
	CCompDataZoomLevelDatas iZoomLevelDatas; // the comp data inst opts are owned
	CCdlTkInterface& iInterface; // not owned
	CMLCompDataInstOptImpls iImpls; // not owned
	vector<char> iBytesAggregated;
	};

#endif
