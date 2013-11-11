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


#ifndef LAYOUTPARSE_H
#define LAYOUTPARSE_H

// disable "identifier was truncated to '255' characters in the browser information" warning
#pragma warning (disable:4786)
#include <vector>
#include <string>
#include <iosfwd>
#include "LayoutCompilerErr.h"
#include "Layout.h"
#include <memory>
using namespace std;


/**
*  LayParseErr 
*  An exception class for .lay file parsing errors
*/
class LayParseErr : public LayoutCompilerErr
	{
public:
	LayParseErr(istream& aIn, const string& aTok, const string& aMsg);
	void Show(ostream& aOut) const;
private:
	string iLoc;
	string iTok;
	string iMsg;
	};


/**
*  TLayParseValues 
*  Parse the values in a layout cell from .lay format
*/
class TLayParseValues : public TValues
	{
// This class is generic with respect to the type of line it is used by
public:
	TLayParseValues(TLayoutLine* aLine, string aName);
	void Parse(istream& aIn);
	};


/**
*  TLayParseLayoutLine
*  Abstract class for common line parsing functionality
*/
class TLayParseLayoutLine : public TLayoutLine
	{
public:
	TLayParseLayoutLine(TLayoutTable* aTable, int aId);
	void Parse(istream& aIn);
	};


/**
*  TLayParseLayoutTable 
*  Parse a layout table in .lay format
*/
class TLayParseLayoutTable : public TLayoutTable
	{
public:
	TLayParseLayoutTable(TLayout* aTables);
	virtual void Parse(istream& aIn);
	};


/**
*  TLayParseLayout 
*  Parse a layout file in .lay format
*/
class TLayParseLayout : public TLayout
	{
public:
	static auto_ptr<TLayParseLayout> Parse(const string& aLayName);
	void Parse(istream& aIn);
	};


#endif

// End of File
