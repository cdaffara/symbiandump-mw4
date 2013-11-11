/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef MLATTRIBUTES_H
#define MLATTRIBUTES_H

// disable "identifier was truncated to '255' characters in the browser information" warning
#pragma warning (disable:4786)

// disable "decorated name length exceeded, name was truncated"
#pragma warning (disable:4503)


#include <string>
#include <iosfwd>
#include <map>

using namespace std;

class TMLAttributes;

/**
*  TMLAttributeSet
*  The attributes stored in an attribute set
*/
typedef map<int, string> TMLAttributeZoomLevels;
class TMLAttributeSet : public map<int, TMLAttributeZoomLevels>
	{
public: // constructors, operators, etc
	TMLAttributeSet();
	TMLAttributeSet(TMLAttributes* aAttributes);
	virtual ~TMLAttributeSet();
public: // new methods
	void Merge(TMLAttributeSet& aOther);
	void Compile();
public: // member data, not owned
	TMLAttributes* iAttributes;
	};

/**
*  TMLAttributes
*  The attribute values stored in a layout component
*/
typedef map<string, TMLAttributeSet*> TMLAttributeSetComponent;
class TMLAttributes: public map<int, TMLAttributeSetComponent>
	{
public:
	typedef map<string, int> TMLAttributeNames;
public: // constructors, operators, etc
	TMLAttributes();
	virtual ~TMLAttributes();
public: // new methods
	void Merge(TMLAttributes& aOther);
	void Compile();
public: // member data, owned
	string iName;
	string iTimestamp;
	bool iCanBeMirror;
	TMLAttributeNames iNames;
	};

#endif // MLATTRIBUTES_H
// End of File
