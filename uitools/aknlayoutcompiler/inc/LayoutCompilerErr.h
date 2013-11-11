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



#ifndef LAYOUTCOMPILERERR_H
#define LAYOUTCOMPILERERR_H

#include <iosfwd>
#include <string>
#include <CdlCompilerToolkit/CdlTkUtil.h>
using namespace CdlCompilerToolkit;
using namespace std;

/**
*  LayoutCompilerErr 
*  Base class for exceptions in the layout compiler
*/
class LayoutCompilerErr : public CdlCompilerToolkitErr
	{};

/**
*  NotFoundErr 
*  A "not found" exception, which takes a string to explain what was not found
*/
class NotFoundErr : public LayoutCompilerErr
	{
public:
	NotFoundErr(const string& aName) : iName(aName) {}
	void Show(ostream& stream) const;
private:
	string iName;
	};

/**
*  GeneralErr 
*  A general compiler error, which takes a string to explain what the error was
*/
class GeneralErr : public LayoutCompilerErr
	{
public:
	GeneralErr(const string& aMessage) : iMessage(aMessage) {}
	void Show(ostream& stream) const;
private:
	string iMessage;
	};

string StreamLoc(const istream& stream, const string& msg, int before, int after);


/**
*  LayoutProcessArgsErr
*  A templated exception class that indicated an exception in the command line parameter
*  processing in the class in which it is instantiated on.
*/
template<class T>
class LayoutProcessArgsErr : public LayoutCompilerErr
	{
	void Show(ostream& aStream) const { T::ShowHelp(aStream); }
	};



#endif

// End of File
