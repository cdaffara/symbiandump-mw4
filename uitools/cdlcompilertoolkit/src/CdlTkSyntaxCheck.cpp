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
#include <CdlCompilerToolkit/CdlTkProcess.h>
#include <CdlCompilerToolkit/CdlCompat.h>
#include "CdlTkPriv.h"
#include <fstream>
#include <iomanip>

#ifdef CDL_W32
#include <direct.h>
#else
#include <unistd.h>
#define _getcwd getcwd
#endif
#include <iostream>
using namespace std;

namespace CdlCompilerToolkit {

//
// CppSyntaxErr
//

class CppSyntaxErr : public CdlCompilerToolkitErr
	{
public:
	CppSyntaxErr(int aErr);
	void Show(ostream& aStream) const;
private:
	int iErr;
	};

CppSyntaxErr::CppSyntaxErr(int aErr)
: iErr(aErr)
	{
	}

void CppSyntaxErr::Show(ostream& aStream) const
	{
	aStream << "CDL C++ Syntax check failed. Cpp returned " << iErr << endl;
	}


//
// CCdlTkSyntaxCheck
//

CCdlTkSyntaxCheck::CCdlTkSyntaxCheck(const CCdlTkInterface& aCdl)
: CCdlTkWriteClientHeader(aCdl)
	{
	}

CCdlTkSyntaxCheck::~CCdlTkSyntaxCheck()
	{
	}

void CCdlTkSyntaxCheck::Process()
	{
	AssertInterfaceNotExtended(iCdl);
	string name = iCdl.FileName() + ".tempCdlSyntaxCheck";

	// these will automatically delete the files when the function returns
	CCdlTkFileCleanup cppClean(name + ".cpp");
	CCdlTkFileCleanup objClean(name + ".s");

	WriteSyntaxCheckCpp(name + ".cpp");
	DoSyntaxCheckBuild(name + ".cpp");
	}

void CCdlTkSyntaxCheck::WriteSyntaxCheckCpp(string aName) const
	{
	ofstream out;
	CdlTkUtil::OpenOutput(out, aName);

	out << "#line 1 \"" << CdlTkUtil::StripPath(iCdl.FileName()) << "\"" << endl;
	out << "#include <cdlengine.h>" << endl;
	const CCdlTkCpp& cpp = iCdl.Cpp();
	for (CCdlTkCpp::const_iterator pCpp = cpp.begin(); pCpp != cpp.end(); ++pCpp)
		{
		out << *pCpp << endl;
		}
	WriteNamespaceStart(iCdl, out);
	for (CCdlTkApiList::const_iterator pApi = iCdl.ApiList().begin(); pApi != iCdl.ApiList().end(); ++pApi)
		WriteSyntaxCheckApi(**pApi, out);
	WriteNamespaceEnd(iCdl, out);
	out << "GLDEF_C TInt E32Dll(TDllReason /*aReason*/)" << endl;
	out << "\t{" << endl;
	out << "\treturn(KErrNone);" << endl;
	out << "\t}" << endl;

	out.close();
	}

void CCdlTkSyntaxCheck::DoSyntaxCheckBuild(string aName) const
	{
	string s = "gcc ";
	if (!iParams.empty())
		{
		s += "\"";
		s += iParams;
		s += "\" ";
		}
	s += "-Wp,-DNDEBUG,-D_UNICODE,-D__SYMBIAN32__,-D__GCC32__,-D__EPOC32__,-D__MARM__,-D__MARM_ARMI__,-D__DLL__,-I";
	s += CdlTkUtil::CurrentDrive();	// drive
	s += "\\epoc32\\include -S -xc++ ";
	s += CdlTkUtil::CurrentDrive();	// drive
	s += aName;
	int err = system(s.c_str());
	if (err)
		throw CppSyntaxErr(err);
	}

void CCdlTkSyntaxCheck::WriteSyntaxCheckApi(const CCdlTkApi& aApi, ofstream& aStream) const
	{
	aStream << "#line " << setbase(10) << aApi.SourceFileLineNum() << endl;
	aStream << ClientReturnType(aApi) << " " << aApi.Name() << aApi.ParamsTypeAndNameList() << ";" << endl;
	}

void CCdlTkSyntaxCheck::SetParams(string aParams)
	{
	iParams = aParams;
	}

}	// end of namespace CdlCompilerToolkit
