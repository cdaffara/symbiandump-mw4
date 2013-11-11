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


#ifndef CPPWRITER_H
#define CPPWRITER_H

// disable "identifier was truncated to '255' characters in the browser information" warning
#pragma warning (disable:4786)
#include <vector>
#include <string>
#include <iosfwd>
#include "WriterBase.h"
using namespace std;




/**
*  TValuesCppWriter
*  Write out layout cell values for AknLayout.dll
*/
class TValuesCppWriter
	{
public:
	TValuesCppWriter(TValues& aValues) : iValues(aValues) {}
	void WriteCpp(ostream& aCpp);

private:
	TValues& iValues;
	};


/**
*  TLayoutLineCppWriter
*  Base class for writing out a layout line for AknLayout.dll
*/
class TLayoutLineCppWriter
	{
public:
	TLayoutLineCppWriter(TLayoutLine& aLine) : iLine(aLine) {}
	void WriteCpp(ostream& aCpp); 
	virtual string Name()=0;
	virtual void WriteMacro(ostream& aLag) = 0;
	virtual void FillParamLists(vector<string>& aDef, vector<string>& aCtor) = 0;

	void WriteCppData(ostream& aCpp);
	int WriteParamList(ostream& aLag, const vector<string>& aParams);

protected:
	virtual void WriteLineClassName(ostream& aCpp) = 0;
	virtual void WriteVarCppData(ostream& aCpp) = 0;
	virtual void WriteFixedCppData(ostream& aCpp) = 0;

	int ValCount();

protected:
	TLayoutLine& iLine;
	};

/**
*  TWindowLineCppWriter 
*  Write out a window line for AknLayout.dll
*/
class TWindowLineCppWriter : public TLayoutLineCppWriter
	{
public:
	TWindowLineCppWriter(TLayoutLine& aLine) : TLayoutLineCppWriter(aLine) {}
	void WriteMacro(ostream& aLag);
	string Name();
	void FillParamLists(vector<string>& aDef, vector<string>& aCtor);

private:
	void WriteVarCppData(ostream& aCpp);
	void WriteFixedCppData(ostream& aCpp);
	void WriteLineClassName(ostream& aCpp);
	};

/**
*  TTextLineCppWriter 
*  Write out a text line for AknLayout.dll
*/
class TTextLineCppWriter : public TLayoutLineCppWriter
	{
public:
	TTextLineCppWriter(TLayoutLine& aLine) : TLayoutLineCppWriter(aLine) {}
	void WriteMacro(ostream& aLag);
	string Name();
	void FillParamLists(vector<string>& aDef, vector<string>& aCtor);

private:
	void WriteVarCppData(ostream& aCpp);
	void WriteFixedCppData(ostream& aCpp);
	void WriteLineClassName(ostream& aCpp);

	// Specific to text
	void WriteMultiLineTextParamList(ostream& aLag, const vector<string>& aParams);

private:
	bool iBaseLineVariation;
	};


/**
*  TLayoutTableCppWriter
*  Write out a layout table for AknLayout.dll
*/
class TLayoutTableCppWriter
	{
public:
	TLayoutTableCppWriter(TLayoutTable& aTable) : iTable(aTable) {}
	void WriteCppLineData(ostream& aCpp);
	void WriteCppTableData(ostream& aCpp);
	void WriteLag(ostream& aLag);
	void WriteLagSubTable(const TLayoutTable::TLayoutSubTable& aSubTable, int aCount, ostream& aLag);
	static TLayoutLineCppWriter* CreateLayoutLineCppWriter(TLayoutLine& aData, TLayoutTable& iTable);

private:
	TLayoutTable& iTable;
	};


/**
*  TLayoutCppWriter
*  Write out a layout for AknLayout.dll
*/
class TLayoutCppWriter
	{
public:
	TLayoutCppWriter(TLayout& aLayout) : iLayout(aLayout) {}
	void WriteCppLineData(ostream& aCpp);
	void WriteCppTableData(ostream& aCpp);
	void WriteLag(ostream& aLag);

private:
	TLayout& iLayout;
	};


/**
*  TLayoutLineTable 
*  Represents and writes a table of layout lines for AknLayout.dll
*/
class TLayoutLineTable : public vector<TLayoutLine*>
	{
public:
	void WriteCpp(ostream& aCpp);
	void WriteLag(ostream& aLag);
	};


/**
*  TCppWriter 
*  Write the source code for AknLayout.dll
*/
class TCppWriter : public TLayWriterBase
	{
public:
	TCppWriter(TLayout& aLayout, const std::string& aName);
	void Write(const std::string& aLayName);

private:
	void Output(const string& aCpp, const string& aLag, const string& aLayName);

	void WriteCppHeader(ostream& aCpp);
	void WriteCppFooter(ostream& aCpp, const string& aLayName);
	void WriteLagHeader(ostream& aLag);

	void BuildLayoutLineTable();

private:
	TLayoutLineTable iLineTable;
	string iShortName;
	};

#endif

// End of File
