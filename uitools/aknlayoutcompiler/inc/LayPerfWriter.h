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


#ifndef LAYPERFWRITER_H
#define LAYPERFWRITER_H

#include "WriterBase.h"

/**
*  TLayPerfTableWriter
*  Writes a layout table in .lay format
*/
class TLayPerfTableWriter
	{
public:
	TLayPerfTableWriter(TLayoutTable& aTable, string& aInterfaceName, int aTableId);
	~TLayPerfTableWriter();

	void Write(ostream& out);
	static void WriteCell(ostream& out, TValues& values);

private:
	bool IsWindowTable();
	void WriteWindowTable(ostream& out);
	void WriteWindowLine(ostream& out, TLayoutLine& line);
	void BuildGenericLine(TLayoutLine& aLine, const string* aKParams, int aCount, string& aParams, string& aLimits);
	bool IsTextTable();
	void WriteTextTable(ostream& out);
	void WriteTextLine(ostream& out, TLayoutLine& line);

private:
	TLayoutTable& iTable;
	string iInterfaceName;
	int iTableId;
	};

/**
*  TLayPerfWriter 
*  Writes a layout in .lay format
*/
class TLayPerfWriter : public TLayWriterBase
	{
public:
	TLayPerfWriter(TLayout& aLayout, const std::string& aName);
	void Write(const std::string& aLayName);
	};

#endif // LAYPERFWRITER_H

// End of File
