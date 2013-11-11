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


#ifndef ADAPTATIONLAYERTEMPLATE_H
#define ADAPTATIONLAYERTEMPLATE_H

#include <string>
#include <vector>
#include <iosfwd>
#include "Layout.h"
using namespace std;

#include <CdlCompilerToolkit/CdlTkInterface.h>
#include <CdlCompilerToolkit/CdlTkInstance.h>
#include <CdlCompilerToolkit/CdlTkProcess.h>
using namespace CdlCompilerToolkit;


class AdaptationLayerTemplate : public CCdlTkSourceFileWriter
	{
public:
	static int Process(const vector<string>& args);
	static void ShowHelp(ostream& stream);
	~AdaptationLayerTemplate();

private:
	AdaptationLayerTemplate(TLayout& aLayout, CCdlTkInterface& aLayIface, CCdlTkInterface& aScaleIface, CCdlTkInstance& aInstance, string& aExisting);

	CCdlTkInstance& Instance();
	string FwdDeclName();
	void Process();

private:
	void AddTableToInstance(TLayoutTable& aTable);
	void AddLineToInstance(TLayoutLine& aLine);
	bool HasApi(const string& aName);
	CCdlTkImplementation& FindImp(const string& aName);
	void SetFuncLine(CCdlTkImplementation& aImp, TLayoutLine& aLine);
	pair<string,int> GetApiMatch(const string& aName);

private:
	CCdlTkInterface& iLayIface;
	CCdlTkInterface& iScaleIface;
	TLayout& iLayout;
	CCdlTkInstance& iInstance;
	string& iExisting;
	};

#endif
