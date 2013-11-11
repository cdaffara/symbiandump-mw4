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

void CCdlTkWriteCommonDefs::ExportCommonDefs(const CCdlTkInterface& aCdl, const std::string& aFileName)
	{
	CCdlTkFileCleanup tempFile;
	ofstream out;

	CdlTkUtil::OpenTempOutput(out, tempFile);
	CCdlTkWriteCommonDefs commonDefs(aCdl, out, aFileName);
	commonDefs.ProcessHRH(out);
	out.close();
	CdlTkUtil::ExportFileIfWritable(tempFile, aFileName+"rh");

	CdlTkUtil::OpenTempOutput(out, tempFile);
	commonDefs.Process();
	out.close();

	CdlTkUtil::ExportFileIfWritable(tempFile, aFileName);
	}

CCdlTkWriteCommonDefs::CCdlTkWriteCommonDefs(const CCdlTkInterface& aCdl, ofstream& aStream, const string& aFileName)
: iCdl(aCdl), iStream(&aStream), iFileName(aFileName)
	{
	}

const string KCommonDefsHeader = "\
/*\n\
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).\n\
* All rights reserved.\n\
* This component and the accompanying materials are made available\n\
* under the terms of \"Eclipse Public License v1.0\"\n\
* which accompanies this distribution, and is available\n\
* at the URL \"http://www.eclipse.org/legal/epl-v10.html\".\n\
*\n\
* Initial Contributors:\n\
* Nokia Corporation - initial contribution.\n\
*\n\
* Contributors:\n\
*\n\
* Description:\n\
*\n\
*/\n\n\
// It contains common definitions for the client and customisation API.\n\
// It should not be modified manually.\n\
\n";

const string KHrhContents ="\
// These constants are defined by the CDL interface: $NAME\n\
#define _CDL_$NSPACE_KCdlInterfaceUidValue $UID\n\
\n";

void CCdlTkWriteCommonDefs::ProcessHRH(ofstream& aStream)
	{
	iStream = &aStream;
	string fileName = iFileName + "rh";
	string cdlFileName(CdlTkUtil::ToLower(iCdl.FileName()));
//	Stream() << CdlTkUtil::Replace("$FILE", cdlFileName, KCommonDefsHeader);
	Stream() << KCommonDefsHeader;
	WriteHeaderGuardStart(fileName, Stream());

	const CCdlTkInterfaceHeader& header = iCdl.Header();
	CdlTkUtil::CReplaceSet headerSet;
	headerSet.Add("$NAME", header.Name());
	headerSet.Add("$UID", CdlTkUtil::IntToHexString(header.Uid()));
	headerSet.Add("$NSPACE", iCdl.NamespaceName());

	Stream() << CdlTkUtil::MultiReplace(headerSet, KHrhContents);

	WriteHeaderGuardEnd(fileName, Stream());
	}

const string KInterfaceConstants ="\
#include \"$THISFILErh\"\n\
\n\
// These constants are defined by the CDL interface: $NAME\n\
_LIT(KCdlName, \"$NAME\");\n\
const TInt KCdlInterfaceUidValue = _CDL_$NSPACE_KCdlInterfaceUidValue;\n\
const TUid KCdlInterfaceUid = { KCdlInterfaceUidValue };\n\
const TInt KCdlInterfaceMajorVersion = $MAJOR;\n\
const TInt KCdlInterfaceMinorVersion = $MINOR;\n\
const TUint KCdlInterfaceFlags = $FLAGS;\n\
\n\
// These are the API ids used in this interface.\n\
enum TApiId\n\
\t{\n";

const string KCdlInterface = "\
\tE_TApiId_TableSize\n\
\t};\n\
\n\
// This is the CDL interface definition for this interface\n\
const SCdlInterface KCdlInterface =\n\
\t{\n\
\tKCdlCompilerMajorVersion,\n\
\tKCdlCompilerMinorVersion,\n\
\tLIT_AS_DESC_PTR(KCdlName),\n\
\t{ KCdlInterfaceUidValue },\n\
\tKCdlInterfaceMajorVersion,\n\
\tKCdlInterfaceMinorVersion,\n\
\tKCdlInterfaceFlags,\n\
\tE_TApiId_TableSize\n\
\t};\n\
\n\
// These typedefs define the types for function APIs\n";

void CCdlTkWriteCommonDefs::Process()
	{
	string cdlFileName(CdlTkUtil::ToLower(iCdl.FileName()));
//	Stream() << CdlTkUtil::Replace("$FILE", cdlFileName, KCommonDefsHeader);
	Stream() << KCommonDefsHeader;
	WriteHeaderGuardStart(iFileName, Stream());
	const CCdlTkCpp& cpp = iCdl.Cpp();
	for (CCdlTkCpp::const_iterator pCpp = cpp.begin(); pCpp != cpp.end(); ++pCpp)
		Stream() << *pCpp << endl;
	Stream() << endl;
	WriteNamespaceStart(iCdl, Stream());
	const CCdlTkInterfaceHeader& header = iCdl.Header();
	CdlTkUtil::CReplaceSet headerSet;
	headerSet.Add("$NAME", header.Name());
	headerSet.Add("$NSPACE", iCdl.NamespaceName());

	headerSet.Add("$THISFILE", CdlTkUtil::ToLower(CdlTkUtil::StripPath(iFileName)));
	headerSet.Add("$MAJOR", CdlTkUtil::IntToString(header.Version().Major()));
	headerSet.Add("$MINOR", CdlTkUtil::IntToString(header.Version().Minor()));
	headerSet.Add("$FLAGS", header.Flags().FlagsAsString());
	Stream() << CdlTkUtil::MultiReplace(headerSet, KInterfaceConstants);
	WriteApiEnum();
	Stream() << KCdlInterface;
	WriteApiTypedefs();
	Stream() << endl;
	WriteNamespaceEnd(iCdl, Stream());
	WriteHeaderGuardEnd(iFileName, Stream());
	}

void CCdlTkWriteCommonDefs::WriteApiEnum()
	{
	const CCdlTkApiList& apiList = iCdl.ApiList();
	for (CCdlTkApiList::const_iterator pApi = apiList.begin(); pApi != apiList.end(); ++pApi)
		{
		Stream() << "\tEApiId_" << (*pApi)->Name() << "," << endl;
		}
	}

string CCdlTkWriteCommonDefs::TypedefForApi(const CCdlTkApi& aApi) const
	{
	if (aApi.IsFunc())
		{
		const CCdlTkFunctionApi& func = aApi.AsFunc();
		return string("typedef ") + func.ReturnType() + " (" + func.ApiNameAsTypeName() + ")(" + func.ParamTypeList() + ");";
		}
	else
		{
		return string("// ") + aApi.ReturnType() + " " + aApi.Name() + " \tdoes not need a typedef";
		}
	}

void CCdlTkWriteCommonDefs::WriteApiTypedefs()
	{
	for (CCdlTkApiList::const_iterator pApi = iCdl.ApiList().begin(); pApi != iCdl.ApiList().end(); ++pApi)
		Stream() << TypedefForApi(**pApi) << endl;
	}

std::ofstream& CCdlTkWriteCommonDefs::Stream()
	{
	return *iStream;
	}

}	// end of namespace CdlCompilerToolkit
