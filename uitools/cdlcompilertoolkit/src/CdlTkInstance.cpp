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
#include "CdlTkPriv.h"
using namespace std;

namespace CdlCompilerToolkit {


//
// CCdlTkImplementation
//

CCdlTkImplementation::CCdlTkImplementation(const CCdlTkInstance& aInstance, const CCdlTkApi& aApi)
: iInstance(aInstance), iApi(aApi)
	{
	SetNull();
	}

CCdlTkImplementation::~CCdlTkImplementation()
	{
	}

string CCdlTkImplementation::Definition() const
	{
	return iDefn;
	}

void CCdlTkImplementation::SetDefinition(const string& aDefn)
	{
	iDefn = aDefn;
	}

const string KTemplateFuncDefn = "\
$TYPE $NAME$PARAMS\n\
\t{\n\
\t//TODO: Implement this function.\n\
\t}";

void CCdlTkImplementation::SetTemplateDefinition()
	{
	if (iApi.IsFunc())
		{
		CdlTkUtil::CReplaceSet defnSet;
		defnSet.Add("$TYPE", iApi.ReturnType());
		defnSet.Add("$NAME", iApi.Name());
		defnSet.Add("$PARAMS", iApi.ParamsTypeAndNameList());
		iDefn = CdlTkUtil::MultiReplace(defnSet, KTemplateFuncDefn);
		}
	else
		{
		string typeVar;
		iDefn = GetTranslation(iApi.ReturnType(), typeVar)->Definition();
		CdlTkUtil::CReplaceSet defnSet;
		defnSet.Add(KType, typeVar);
		defnSet.Add(KName, iApi.Name());
		iDefn = CdlTkUtil::MultiReplace(defnSet, iDefn);
		CdlTkUtil::AppendString(iDefn, ";\t//TODO: Initialise this data.");
		}
	}

const CCdlTkDataTypeTranslation* CCdlTkImplementation::GetTranslation(const string& aType, string& aTypeVar)
	{
	return iApi.Interface().DataTypeTranslations().Find(aType, aTypeVar);
	}

string CCdlTkImplementation::PointerReference() const
	{
	return iPtr;
	}

void CCdlTkImplementation::SetPointerReference(const string& aPtr)
	{
	iPtr = aPtr;
	}

void CCdlTkImplementation::SetTemplatePointerReference()
	{
	if (iApi.IsFunc())
		{
		iPtr = string("&") + iApi.Name();
		}
	else
		{
		string typeVar;
		iPtr = GetTranslation(iApi.ReturnType(), typeVar)->PointerReference();
		CdlTkUtil::CReplaceSet ptrSet;
		ptrSet.Add(KType, typeVar);
		ptrSet.Add(KName, iApi.Name());
		iPtr = CdlTkUtil::MultiReplace(ptrSet, iPtr);
		}
	}

const CCdlTkInstance& CCdlTkImplementation::Instance() const
	{
	return iInstance;
	}

const CCdlTkApi& CCdlTkImplementation::Api() const
	{
	return iApi;
	}

const string& CCdlTkImplementation::Name() const
	{
	return iApi.Name();
	}

void CCdlTkImplementation::SetTemplate()
	{
	SetTemplateDefinition();
	SetTemplatePointerReference();
	}

void CCdlTkImplementation::SetNull()
	{
	SetDefinition("");
	SetPointerReference("NULL");
	}


//
// CCdlTkImplementations
//

CCdlTkImplementations::CCdlTkImplementations(const CCdlTkInstance& aInstance, const CCdlTkApiList& aApiList)
	{
	for (CCdlTkApiList::const_iterator pApi = aApiList.begin(); pApi != aApiList.end(); ++pApi)
		{
		iImpl.push_back(new CCdlTkImplementation(aInstance, **pApi));
		}
	}

CCdlTkImplementations::~CCdlTkImplementations()
	{
	for (CImpl::iterator p = iImpl.begin(); p != iImpl.end(); ++p)
		delete *p;
	}

CCdlTkImplementation* CCdlTkImplementations::Find(const string& aName) const
	{
	for (CImpl::const_iterator p = iImpl.begin(); p != iImpl.end(); ++p)
		{
		if ((*p)->Name() == aName)
			return *p;
		}
	return 0;
	}

void CCdlTkImplementations::ClearAll()
	{
	for (CImpl::iterator pImp = iImpl.begin(); pImp != iImpl.end(); ++pImp)
		{
		(*pImp)->SetNull();
		}
	}

void CCdlTkImplementations::TemplateAll()
	{
	for (CImpl::iterator pImp = iImpl.begin(); pImp != iImpl.end(); ++pImp)
		{
		(*pImp)->SetTemplate();
		}
	}


//
// CCdlTkInstance
//

#pragma warning (disable:4355)	// using "this" in initialisation list
// note: "this" is not accessed during construction of iImpl
CCdlTkInstance::CCdlTkInstance(const CCdlTkInterface& aInterface)
: iInterface(aInterface), iImpl(*this, aInterface.ApiList()), iId(KCdlTkGetInstanceIdFromHostDll)
	{
	if (iInterface.Base() || iInterface.Extension())
		throw CdlTkAssert("Can't create instance from extended interface - merge extensions first");
	}
#pragma warning (default:4355)	// using "this" in initialisation list

CCdlTkInstance::~CCdlTkInstance()
	{
	}

CCdlTkImplementations& CCdlTkInstance::Impl()
	{
	return iImpl;
	}

const CCdlTkImplementations& CCdlTkInstance::Impl() const
	{
	return iImpl;
	}

void CCdlTkInstance::SetName(const string& aName)
	{
	iName = aName;
	}

const string& CCdlTkInstance::Name() const
	{
	return iName;
	}

std::string CCdlTkInstance::DllInstanceName() const
	{
	return InstanceNameToDllInstanceName(iName);
	}

const string KDllInstanceName = "KDllInst_$INST";

std::string CCdlTkInstance::InstanceNameToDllInstanceName(const std::string& aName)
	{
	string tempName = aName;
	tempName = CdlTkUtil::Replace("\r", "", tempName);
	tempName = CdlTkUtil::Replace("\n", "", tempName);
	return CdlTkUtil::Replace("$INST", CdlTkUtil::ToCpp(tempName), KDllInstanceName);
	}


const CCdlTkInterface& CCdlTkInstance::Interface() const
	{
	return iInterface;
	}

void CCdlTkInstance::SetId()
	{
	iId = KCdlTkGetInstanceIdFromHostDll;
	}

void CCdlTkInstance::SetId(int aId)
	{
	iId = aId;
	}

int CCdlTkInstance::Id() const
	{
	return iId;
	}

const string& CCdlTkInstance::ExtraCpp() const
	{
	return iExtraCpp;
	}

void CCdlTkInstance::SetExtraCpp(const string& aExtra)
	{
	iExtraCpp = aExtra;
	}

void CCdlTkInstance::ClearAllImplementations()
	{
	iImpl.ClearAll();
	}

void CCdlTkInstance::TemplateAllImplementations()
	{
	iImpl.TemplateAll();
	}

}	// end of namespace CdlCompilerToolkit

