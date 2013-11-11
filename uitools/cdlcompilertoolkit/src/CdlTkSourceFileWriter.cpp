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
using namespace std;

namespace CdlCompilerToolkit {


CCdlTkSourceFileWriter::~CCdlTkSourceFileWriter()
	{
	}

void CCdlTkSourceFileWriter::WriteNamespaceStart(const CCdlTkInterface& aCdl, ofstream& aStream) const
	{
	aStream << "namespace " << aCdl.NamespaceName()	<< endl;
	aStream << "{" << endl;
	}

void CCdlTkSourceFileWriter::WriteNamespaceEnd(const CCdlTkInterface& aCdl, ofstream& aStream) const
	{
	aStream << "} // end of namespace " << aCdl.NamespaceName() << endl;
	}

std::string CCdlTkSourceFileWriter::HeaderGuardName(const std::string& aFileName) const
	{
	return CdlTkUtil::ToUpper(CdlTkUtil::ToCpp(CdlTkUtil::StripPath(aFileName)));
	}

void CCdlTkSourceFileWriter::WriteHeaderGuardStart(const string& aName, ofstream& aStream) const
	{
	string defName = HeaderGuardName(aName);
	aStream << "#ifndef " << defName << endl;
	aStream << "#define " << defName << endl;
	aStream << endl;
	}

void CCdlTkSourceFileWriter::WriteHeaderGuardEnd(const string& aName, ofstream& aStream) const
	{
	string defName = HeaderGuardName(aName);
	aStream << endl;
	aStream << "#endif // " << defName << endl;
	}

}	// end of namespace CdlCompilerToolkit
