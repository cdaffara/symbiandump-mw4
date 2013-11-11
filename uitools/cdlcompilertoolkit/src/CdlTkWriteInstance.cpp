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
#include <sstream>
using namespace std;

namespace CdlCompilerToolkit {

CCdlTkWriteInstance::CCdlTkWriteInstance(const CCdlTkInstance& aInstance)
: iInstance(aInstance), iCdl(aInstance.Interface())
	{
	}

CCdlTkWriteInstance::~CCdlTkWriteInstance()
	{
	}

void CCdlTkWriteInstance::Process()
	{
	string implName = CdlTkUtil::ToLower( CdlTkUtil::StripPath(iInstance.Name()));
	implName = CdlTkUtil::Replace("\r","",implName);
	implName = CdlTkUtil::Replace("\n","",implName);
	string headerName(implName + ".h");
	InitReplace(headerName);
	ProcessCdl();

	CCdlTkFileCleanup tempFile;

	ofstream hrhStream;
	CdlTkUtil::OpenTempOutput(hrhStream, tempFile);
	ProcessInstanceHrh(hrhStream, headerName+"rh");
	hrhStream.close();
	CdlTkUtil::ExportFileIfWritable(tempFile, CdlTkUtil::OutputPath() + headerName+"rh");

	ofstream headerStream;
	CdlTkUtil::OpenTempOutput(headerStream, tempFile);
	ProcessInstanceHeader(headerStream, headerName);
	headerStream.close();
	CdlTkUtil::ExportFileIfWritable(tempFile, CdlTkUtil::OutputPath() + headerName);

	ofstream cppStream;
	CdlTkUtil::OpenTempOutput(cppStream, tempFile);
	ProcessInstance(cppStream, headerName);
	cppStream.close();

	CdlTkUtil::ExportFileIfWritable(tempFile, CdlTkUtil::OutputPath() + CdlTkUtil::CorrectFilenameCase(implName + ".cpp"));
	}

void CCdlTkWriteInstance::InitReplace(const std::string& aHeaderName)
	{
	iReplace.clear();
	string instanceName = iInstance.Name();
	instanceName = CdlTkUtil::Replace("\r", "", instanceName);
	instanceName = CdlTkUtil::Replace("\n", "", instanceName);
	iReplace.Add("$INSTNAME", instanceName);
	iReplace.Add("$CDLNAME", CdlTkUtil::StripPath(iCdl.FileName()));
	iReplace.Add("$CDLINCNAME", CdlTkUtil::ToLower(CdlTkUtil::StripPath(iCdl.FileName())));
	iReplace.Add("$HEADERNAME", CdlTkUtil::ToLower(CdlTkUtil::StripPath(aHeaderName)));
	iReplace.Add("$EXTRA", iInstance.ExtraCpp());
	iReplace.Add("$INSTNS", CdlTkUtil::ToCpp(iInstance.Name()));

	iReplace.Add("$CDLNS", iCdl.NamespaceName());
	iReplace.Add("$CDLGUARD", HeaderGuardName(iCdl.FileName() + ".custom.h"));
	iReplace.Add("$INSTGUARD", HeaderGuardName(aHeaderName));
	iReplace.Add("$CMDLINE", CdlTkUtil::CommandLine());
	if (iInstance.Id() == KCdlTkGetInstanceIdFromHostDll)
		{
		iReplace.Add("$INSTID", iInstance.DllInstanceName());
		iReplace.Add("$DLLINSTHEADER", "#include \"" + KDllInstHeader + "\"\n");
		}
	else
		{
		iReplace.Add("$INSTID", CdlTkUtil::IntToString(iInstance.Id()));
		iReplace.Add("$DLLINSTHEADER", "");
		}
	}

void CCdlTkWriteInstance::ProcessCdl() const
	{
	AssertInterfaceNotExtended(iCdl);

	string baseName = CdlBaseNameAndPath(iCdl);
	string commonHeaderName(baseName + KCommonHeader);
	CCdlTkWriteCommonDefs::ExportCommonDefs(iCdl, commonHeaderName);

	CCdlTkFileCleanup tempFile;
	ofstream out;
	CdlTkUtil::OpenTempOutput(out, tempFile);
	string customHeaderName(baseName + ".custom.h");
	ProcessInstanceApi(out, customHeaderName);
	out.close();
	CdlTkUtil::ExportFile(tempFile, customHeaderName);
	}


const string KInstanceApiStart = "\
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
// This file was generated from $CDLNAME.\n\
// It contains the customisation API.\n\
// It should not be modified manually.\n\
\n\
#ifndef $CDLGUARD\n\
#define $CDLGUARD\n\
\n\
#include <cdlengdef.h>\n\
#include \"$CDLINCNAME.common.h\"\n\
\n\
namespace $CDLNS\n\
{\n\
\n\
// This structure defines the interface to a customisation instance.\n\
// A customisation instance must supply an instance of this structure, with the pointer\n\
// members either being NULL, or pointing at functions or data of the correct type.\n\
\n\
#ifdef __WINS__\n\
struct SCdlImpl0\n\
#else\n\
struct SCdlImpl\n\
#endif\n\
\t{\n";

const string KInstanceApiEnd = "\
\t};\n\
#ifdef __WINS__\n\
struct SCdlImpl\n\
\t{\n\
\tSCdlImpl0 i0;\n\
$SUBSTRUCTS\
\t};\n\
#endif\n\
\n\
} // end of namespace $CDLNS\n\
#endif // $CDLGUARD\n";

const string KSubStructDecl = "\
#ifdef __WINS__\n\
\t};\n\
\n\
struct SCdlImpl$NUM\n\
\t{\n\
#endif\n";

const string KSubStructUse = "\tSCdlImpl$NUM i$NUM;\n";

const int KMsvcStructSplit = 1000;

void CCdlTkWriteInstance::ProcessInstanceApi(ofstream& aStream, const string& aFileName) const
	{
	string subStructs;
	int count=0;
	aStream << CdlTkUtil::MultiReplace(iReplace, KInstanceApiStart);
	for (CCdlTkApiList::const_iterator pApi = iCdl.ApiList().begin(); pApi != iCdl.ApiList().end(); ++pApi)
		{
		aStream << "\t" << (*pApi)->PointerType() << " " << (*pApi)->Name() << ";" << endl;
		count++;
		if ((count%KMsvcStructSplit) == 0)
			{
			string num = CdlTkUtil::IntToString(count/KMsvcStructSplit);
			aStream << CdlTkUtil::Replace("$NUM", num, KSubStructDecl);
			CdlTkUtil::AppendString(subStructs, CdlTkUtil::Replace("$NUM", num, KSubStructUse));
			}
		}
	string end = CdlTkUtil::Replace("$SUBSTRUCTS", subStructs, KInstanceApiEnd);
	aStream << CdlTkUtil::MultiReplace(iReplace, end);
	}


const string KInstanceCppStart = "\
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
// $INSTNAME generated by\n\
// $CMDLINE\n\
// This customisation implements the interface defined in $CDLNAME\n\
\n\
#include \"$HEADERNAME\"\n\
$EXTRA\n\
namespace $INSTNS\n\
{\n\
\n";

const string KInstanceCppMid = "\
\n\
const $CDLNS::SCdlImpl KCdlImpl = \n\
\t{\n";

const string KInstanceCppEnd = "\
\t};\n\
\n\
} // end of namespace $INSTNS\n";

void CCdlTkWriteInstance::ProcessInstance(ofstream& aStream, const string& aHeaderName) const
	{
	aStream << CdlTkUtil::MultiReplace(iReplace, KInstanceCppStart);
	
	const CCdlTkImplementations& impl = iInstance.Impl();
	CCdlTkImplementations::const_iterator pImpl;
	// write the definitions
	for (pImpl = impl.begin(); pImpl != impl.end(); ++pImpl)
		{
		const string& defn = (*pImpl)->Definition();
		if (defn.size())
			{
			aStream << defn << endl;
			aStream << endl;
			}
		}

	aStream << CdlTkUtil::MultiReplace(iReplace, KInstanceCppMid);

	// write the pointers
	for (pImpl = impl.begin(); pImpl != impl.end(); ++pImpl)
		aStream << "\t" << (*pImpl)->PointerReference() << "," << endl;

	aStream << CdlTkUtil::MultiReplace(iReplace, KInstanceCppEnd);
	}


const string KInstanceHeader = "\
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
// This header file contains the customisation implementation identity for $INSTNAME\n\
// It was generated by:\n\
// $CMDLINE\n\
// This file may be manually modified.\n\
\n\
#ifndef $INSTGUARD\n\
#define $INSTGUARD\n\
#include \"$CDLINCNAME.custom.h\"\n\
#include \"$HEADERNAMErh\"\n\
\n\
namespace $INSTNS\n\
{\n\
\n\
const TInt KCdlInstanceId = _CDL_$INSTNS_KCdlInstanceId;\n\
using $CDLNS::KCdlInterface;\n\
using $CDLNS::KCdlInterfaceUidValue;\n\
GLREF_D const $CDLNS::SCdlImpl KCdlImpl;\n\
\n\
} // end of namespace $INSTNS\n\
#endif // $INSTGUARD\n";

void CCdlTkWriteInstance::ProcessInstanceHeader(ofstream& aStream, const string& aFileName) const
	{
	aStream << CdlTkUtil::MultiReplace(iReplace, KInstanceHeader);
	}


const string KInstanceHrh = "\
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
// This header file contains the customisation implementation identity for $INSTNAME\n\
// It was generated by:\n\
// $CMDLINE\n\
// This file may be manually modified.\n\
\n\
#ifndef $INSTGUARDRH\n\
#define $INSTGUARDRH\n\
#include \"$CDLINCNAME.common.hrh\"\n\
$DLLINSTHEADER\
\n\
#define _CDL_$INSTNS_KCdlInstanceInterfaceUidValue _CDL_$CDLNS_KCdlInterfaceUidValue\n\
#define _CDL_$INSTNS_KCdlInstanceId $INSTID\n\
\n\
#endif // $INSTGUARDRH\n";

void CCdlTkWriteInstance::ProcessInstanceHrh(ofstream& aStream, const string& aFileName) const
	{
	aStream << CdlTkUtil::MultiReplace(iReplace, KInstanceHrh);
	}

}	// end of namespace CdlCompilerToolkit
