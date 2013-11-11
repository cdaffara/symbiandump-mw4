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

#ifndef WRITERBASE_H
#define WRITERBASE_H

#include "Layout.h"

/**
*  TWriterBase
*  Base class for layout writers
*/
class TWriterBase
	{
public:
	TWriterBase(const std::string& aName);
	virtual ~TWriterBase();
	virtual void Write(const std::string& aLayName) = 0;
protected:
	string iName;
	};

class TLayWriterBase : public TWriterBase
	{
public:
	TLayWriterBase(TLayout& aLayout, const std::string& aName);
	virtual ~TLayWriterBase();
	virtual void Write(const std::string& aLayName) = 0;
protected:
	TLayout& iLayout;
	};

template <class T>
class TMLWriterBase : public TWriterBase
	{
public:
	TMLWriterBase(T& aLayout, const std::string& aName) : TWriterBase(aName), iLayout(aLayout) {};
	virtual ~TMLWriterBase() {};
	virtual void Write(const std::string& aLayName) = 0;
protected:
	T& iLayout;
	};

#endif

// End of File
