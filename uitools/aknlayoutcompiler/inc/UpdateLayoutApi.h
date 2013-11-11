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


#ifndef UPDATELAYOUTAPI_H
#define UPDATELAYOUTAPI_H

#include <string>
#include <vector>
#include <iosfwd>
#include "Layout.h"
using namespace std;

#include <cdlcompilertoolkit/cdltkinterface.h>
#include <cdlcompilertoolkit/cdltkprocess.h>
using namespace CdlCompilerToolkit;

/**
*  UpdateLayoutApi 
*  Intended to update a CDL interface for a layout - not yet complete
*/
class UpdateLayoutApi : public MCdlTkApiCheckObserver
	{
public:
	static int Process(const vector<string>& args);
	static void ShowHelp(ostream& stream);

	UpdateLayoutApi(const std::string& aCdlName, const std::string& aLayoutName, const std::string& aLegacyName, const std::string& aNewName);
	~UpdateLayoutApi();

private:
	void LoadFiles(const std::string& aLayoutName, const std::string& aNewName);
	void MergeAndCheck();
	void ProcessNewApi();
	void WriteFiles();
	void Report();
	void AddDataToLegacy(const std::string& aOldName, const std::string& aNewName);

private:
	virtual void StartCheck();
	virtual void CheckComplete();
	virtual void ApiInBoth(const CCdlTkApi& aApi);
	virtual void ApiNotInLeft(const CCdlTkApi& aApi);
	virtual void ApiNotInRight(const CCdlTkApi& aApi);

private:
	class InterfaceCheck : public MCdlTkApiCheckObserver
		{
		virtual void StartCheck();
		virtual void CheckComplete();
		virtual void ApiInBoth(const CCdlTkApi& aApi);
		virtual void ApiNotInLeft(const CCdlTkApi& aApi);
		virtual void ApiNotInRight(const CCdlTkApi& aApi);
		};

private:
	std::string iLegacyName;
	std::string iCdlName;
	CCdlTkInterface* iInterface;
	TLayout* iLayout;
	TLayout* iLegacy;
	TLayout* iNew;
	CCdlTkInterface* iExt;	// not owned
	bool iInterfaceChanged;
	bool iLegacyUpdated;
	string iReport;
	};


#endif
