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
#include "CdlCompilerToolkit/CdlTkInstance.h"
using namespace std;

namespace CdlCompilerToolkit {

CCdlTkDll::CCdlTkDll()
: iUid(0), iVersion(1)
	{
	}

CCdlTkDll::~CCdlTkDll()
	{
	}

void CCdlTkDll::AddInstance(const CCdlTkInstance& aInstance)
	{
	AddInstance(aInstance.Name());
	}

void CCdlTkDll::AddInstance(const string& aInstanceName)
	{
	if (iUniqueInstances.find(aInstanceName) == iUniqueInstances.end())
		{
		iUniqueInstances.insert(aInstanceName);
		iInstances.push_back(aInstanceName);
		}
	}

const CCdlTkDll::CInstances& CCdlTkDll::Instances() const
	{
	return iInstances;
	}

void CCdlTkDll::SetName(const string& aName)
	{
	iName = aName;
	}

const string& CCdlTkDll::Name() const
	{
	return iName;
	}

void CCdlTkDll::SetUid(int aUid)
	{
	iUid = aUid;
	}

int CCdlTkDll::Uid() const
	{
	return iUid;
	}

void CCdlTkDll::SetVersion(int aVersion)
	{
	iVersion = aVersion;
	}

int CCdlTkDll::Version() const
	{
	return iVersion;
	}

void CCdlTkDll::AddLibrary(const string& aLibName)
	{
	iLibraries.push_back(aLibName);
	}

const CCdlTkDll::CLibraries& CCdlTkDll::Libraries() const
	{
	return iLibraries;
	}

CCdlTkDll::CLibraries& CCdlTkDll::Libraries()
	{
	return iLibraries;
	}

void CCdlTkDll::SetExtraMmp(const std::string& aExtraMmp)
	{
	iExtraMmp = aExtraMmp;
	}

const std::string& CCdlTkDll::ExtraMmp() const
	{
	return iExtraMmp;
	}


}	// end of namespace CdlCompilerToolkit
