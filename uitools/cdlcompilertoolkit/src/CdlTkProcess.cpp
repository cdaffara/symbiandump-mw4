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
#pragma warning (disable:4786)	// disable "identifier was truncated to '255' characters in the browser information" warning
#include "CdlCompilerToolkit/CdlTkProcess.h"
#include "CdlTkPriv.h"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdio.h>
using namespace std;

namespace CdlCompilerToolkit {

CCdlTkProcess::~CCdlTkProcess()
	{
	}

string CCdlTkProcess::CdlBaseNameAndPath(const CCdlTkInterface& aCdl)
	{
	return CdlTkUtil::OutputPath() + CdlTkUtil::ToLower(CdlTkUtil::StripPath(aCdl.FileName()));
	}

void CCdlTkProcess::AssertInterfaceNotExtended(const CCdlTkInterface& aCdl)
	{
	if (aCdl.Base() || aCdl.Extension())
		throw CdlTkAssert("Can't process extended interface - merge extensions first");
	}

}	// end of namespace CdlCompilerToolkit
