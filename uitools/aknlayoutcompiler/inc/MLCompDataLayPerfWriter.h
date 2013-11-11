/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef MLCompDataLayPerfWriter_H
#define MLCompDataLayPerfWriter_H

// disable "decorated name length exceeded, name was truncated"
#pragma warning (disable:4503)

#include "WriterBase.h"
#include "MLCompData.h"

class TMLCompDataLayPerfWriter;

#include <CdlCompilerToolkit/CdlTkInterface.h>
using namespace CdlCompilerToolkit;


/**
*  TLayPerfTableWriter
*  Writes a layout table in .lay format
*/
class TMLCompDataLayPerfTableWriter
	{
public:
	TMLCompDataLayPerfTableWriter(TMLCompDataLayPerfWriter* aParent, TMLCompDataTable& aTable, string& aInterfaceName, int aTableId);
	~TMLCompDataLayPerfTableWriter();
	void Write(ostream& out);

private:
	void WriteTable(ostream& out);
	void WriteLine(ostream& out, TMLCompDataLine& line);
	string BuildMacroName(TMLCompDataLine& aLine);
	string BuildApiName(TMLCompDataLine& aLine);
	string BuildParams(TMLCompDataLine& aLine);
	string BuildParamLimitParams(TMLCompDataLine& aLine);

private:
	TMLCompDataLayPerfWriter* iParent;
	TMLCompDataTable& iTable;
	string iInterfaceName;
	int iTableId;
	};

/**
*  TMLCompDataLayPerfWriter 
*  Writes a layout in LayPerf format, using the build in macros to do the work.
*/
class TMLCompDataLayPerfWriter : public TMLWriterBase<TMLCompData>
	{
public:
	TMLCompDataLayPerfWriter(CCdlTkInterface& aInterface, TMLCompData& aLayout, const std::string& aName);
	void Write(const std::string& aLayName);
public:
	CCdlTkInterface& iInterface; // not owned
	};

#endif // MLCompDataLayPerfWriter_H

// End of File
