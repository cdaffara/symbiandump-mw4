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


#ifndef LAYOUTPACK_H
#define LAYOUTPACK_H

#include <string>
#include <vector>
#include <iosfwd>
using namespace std;

#include <CdlCompilerToolkit/CdlTkInterface.h>
#include <CdlCompilerToolkit/CdlTkInstance.h>
#include <CdlCompilerToolkit/CdlTkProcess.h>
using namespace CdlCompilerToolkit;

/**
*  LayoutPackage
*  Write a layout package according to the LayoutPack.cdl interface
*/
class LayoutPackage
	{
public:
	static int Process(vector<string>& args);
	static void ShowHelp(ostream& stream);

private:
	static void Process(
		const string& aName, 
		const string& aWidth, 
		const string& aHeight, 
		const string& aId, 
		const string& aStyleName,
		const string& aDeliveryTypeName,
        const string& aPriority,
        const string& aAppUid,
		vector<string>::const_iterator aBegin, 
		vector<string>::const_iterator aEnd, 
		const string& aZoomName);
	static void Implement(
		CCdlTkPackage& aPkg, 
		const string& aApi, 
		const string& aTarget, 
		const string& aReplace,
		const string& aComment = "");
	static void ProcessOptionalStringArg(vector<string>& args, const string& aOption, string& aArg);
	static string Hash(const string& aString);
	};


#endif
