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


#ifndef LAYOUTWRITER_H
#define LAYOUTWRITER_H

#include "WriterBase.h"

/**
*  TLayoutTableWriter
*  Writes a layout table in .lay format
*/
class TLayoutTableWriter
	{
public:
	TLayoutTableWriter(TLayoutTable& aTable);
	~TLayoutTableWriter();

	void Write(ostream& out);
	static void WriteCell(ostream& out, TValues& values);

private:
	bool IsWindowTable();
	void WriteWindowTable(ostream& out);
	void WriteWindowLine(ostream& out, TLayoutLine& line);
	bool IsTextTable();
	void WriteTextTable(ostream& out);
	void WriteTextLine(ostream& out, TLayoutLine& line);

private:
	TLayoutTable& iTable;
	};

/**
*  TLayoutWriter 
*  Writes a layout in .lay format
*/
class TLayoutWriter : public TLayWriterBase
	{
public:
	TLayoutWriter(TLayout& aLayout, const std::string& aName);
	void Write(const std::string& aLayName);
	};

#endif

// End of File
