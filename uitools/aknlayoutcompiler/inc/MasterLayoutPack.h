/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef MASTERLAYOUTPACK_H
#define MASTERLAYOUTPACK_H

#include <string>
#include <vector>
#include <iosfwd>
using namespace std;

#include <CdlCompilerToolkit/CdlTkInterface.h>
#include <CdlCompilerToolkit/CdlTkInstance.h>
#include <CdlCompilerToolkit/CdlTkProcess.h>
using namespace CdlCompilerToolkit;

/**
*  MasterLayoutPackage
*  Write a master layout package according to the MasterLayoutPack.cdl interface
*/
class MasterLayoutPackage
	{
public:
	static int Process(vector<string>& args);
	static void ShowHelp(ostream& stream);
	};


#endif
