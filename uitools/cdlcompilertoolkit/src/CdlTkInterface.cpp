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
#include "CdlCompilerToolkit/CdlTkInterface.h"
#include "CdlTkPriv.h"
#include "CdlTkStdTrans.h"
#include <sstream>
#include <CdlDef.h>
using namespace std;

namespace CdlCompilerToolkit {

//
// TCdlTkFlag
//

struct TCdlTkFlag
	{
	const char* iName;
	int iValue;
	};

const TCdlTkFlag gCdlTkFlags[] =
	{
		{ "KCdlFlagRomOnly", KCdlFlagRomOnlyValue }
	};


//
// CCdlTkInterfaceHeader::CFlags
//

CCdlTkInterfaceHeader::CFlags::CFlags()
: iFlags(0)
	{
	}

void CCdlTkInterfaceHeader::CFlags::SetFlag(const string& aFlagName)
	{
	iFlags |= FlagVal(aFlagName);
	}

void CCdlTkInterfaceHeader::CFlags::ClearFlag(const string& aFlagName)
	{
	iFlags &= ~FlagVal(aFlagName);
	}

int CCdlTkInterfaceHeader::CFlags::FlagsAsInt() const
	{
	return iFlags;
	}

string CCdlTkInterfaceHeader::CFlags::FlagsAsString() const
	{
	string flags;
	for (int bit=0; bit<Count(); bit++)
		{
		if (IsSet(bit))
			{
			if (!flags.empty())
				CdlTkUtil::AppendString(flags, " | ");
			CdlTkUtil::AppendString(flags, FlagName(bit));
			}
		}
	if (flags.empty())
		flags = "0";
	return flags;
	}

int CCdlTkInterfaceHeader::CFlags::FlagVal(const string& aFlagName) const
	{
	for (int ii=0; ii<Count(); ii++)
		{
		const TCdlTkFlag& flag = gCdlTkFlags[ii];
		if (aFlagName == flag.iName)
			return flag.iValue;
		}
	throw CdlTkAssert(aFlagName + " : unknown flag");
	return 0;
	}

string CCdlTkInterfaceHeader::CFlags::FlagName(int aFlagIndex) const
	{
	int flagVal = IndexToFlagVal(aFlagIndex);
	for (int ii=0; ii<Count(); ii++)
		{
		const TCdlTkFlag& flag = gCdlTkFlags[ii];
		if (flagVal == flag.iValue)
			return flag.iName;
		}
	throw CdlTkAssert(CdlTkUtil::IntToString(aFlagIndex) + " : unknown flag index");
	return "";
	}

int CCdlTkInterfaceHeader::CFlags::Count() const
	{
	return sizeof(gCdlTkFlags)/sizeof(*gCdlTkFlags);
	}

int CCdlTkInterfaceHeader::CFlags::IndexToFlagVal(int aIndex) const
	{
	return 1<<aIndex;
	}

bool CCdlTkInterfaceHeader::CFlags::IsSet(int aFlagIndex) const
	{
	return !!(iFlags & IndexToFlagVal(aFlagIndex));
	}

bool CCdlTkInterfaceHeader::CFlags::IsSet(const string& aFlagName) const
	{
	return !!(iFlags & FlagVal(aFlagName));
	}

CCdlTkInterfaceHeader::CFlags& CCdlTkInterfaceHeader::CFlags::operator|=(const CFlags& aOther)
	{
	iFlags |= aOther.iFlags;
	return *this;
	}


//
// CCdlTkInterfaceHeader::CVersion
//

CCdlTkInterfaceHeader::CVersion::CVersion()
: iMajor(0), iMinor(0)
	{
	}

CCdlTkInterfaceHeader::CVersion::CVersion(int aMajor, int aMinor)
: iMajor(aMajor), iMinor(aMinor)
	{
	}

bool CCdlTkInterfaceHeader::CVersion::operator<(const CVersion aOther) const
	{
	return  (iMajor < aOther.iMajor) ||
			(iMajor == aOther.iMajor && iMinor < aOther.iMinor);
	}

bool CCdlTkInterfaceHeader::CVersion::operator==(const CVersion aOther) const
	{
	return iMajor == aOther.iMajor && iMinor == aOther.iMinor;
	}

int CCdlTkInterfaceHeader::CVersion::Major() const
	{
	return iMajor;
	}

void CCdlTkInterfaceHeader::CVersion::SetMajor(int aMajor)
	{
	iMajor = aMajor;
	}

int CCdlTkInterfaceHeader::CVersion::Minor() const
	{
	return iMinor;
	}

void CCdlTkInterfaceHeader::CVersion::SetMinor(int aMinor)
	{
	iMinor = aMinor;
	}


//
// CCdlTkInterfaceHeader
//

CCdlTkInterfaceHeader::CCdlTkInterfaceHeader()
: iName(""), iUid(0), iVer(0, 0)
	{
	}

void CCdlTkInterfaceHeader::MergeExtensionHeader(const CCdlTkInterfaceHeader& aOther)
	{
	if (!aOther.iName.empty() && iName != aOther.iName)
		throw CdlTkAssert("Can't change interface name in extension");
	if (aOther.iUid && iUid != aOther.iUid)
		throw CdlTkAssert("Can't change interface UID in extension");
	if (!(iVer < aOther.iVer))
		throw CdlTkAssert("Interface version must increase in extension");
	iVer = aOther.iVer;
	iFlags |= aOther.iFlags;
	}

string CCdlTkInterfaceHeader::Name() const
	{
	return iName;
	}

void CCdlTkInterfaceHeader::SetName(const string& aName)
	{
	iName = aName;
	}

int CCdlTkInterfaceHeader::Uid() const
	{
	return iUid;
	}

void CCdlTkInterfaceHeader::SetUid(int aUid)
	{
	iUid = aUid;
	}

void CCdlTkInterfaceHeader::SetVersion(const CVersion& aVer)
	{
	iVer = aVer;
	}

const CCdlTkInterfaceHeader::CVersion& CCdlTkInterfaceHeader::Version() const
	{
	return iVer;
	}

const CCdlTkInterfaceHeader::CFlags& CCdlTkInterfaceHeader::Flags() const
	{
	return iFlags;
	}

CCdlTkInterfaceHeader::CFlags& CCdlTkInterfaceHeader::Flags()
	{
	return iFlags;
	}

bool CCdlTkInterfaceHeader::operator==(const CCdlTkInterfaceHeader& aOther) const
	{
	return 
		iName == aOther.iName &&
		iUid == aOther.iUid &&
		iVer == aOther.iVer;
	}


//
// CCdlTkApiParam
//

CCdlTkApiParam::CCdlTkApiParam()
	{
	}

CCdlTkApiParam::CCdlTkApiParam(const string& aType, const string& aName)
: iType(aType), iName(aName)
	{
	}

CCdlTkApiParam::CCdlTkApiParam(const string& aType, const string& aName, const std::string& aDefaultValue)
: iType(aType), iName(aName), iDefaultValue(aDefaultValue)
	{
	}

const string& CCdlTkApiParam::Type() const
	{
	return iType;
	}

void CCdlTkApiParam::SetType(const string& aType)
	{
	iType = aType;
	}

const string& CCdlTkApiParam::Name() const
	{
	return iName;
	}

void CCdlTkApiParam::SetName(const string& aName)
	{
	iName = aName;
	}

const string& CCdlTkApiParam::DefaultValue() const
	{
	return iDefaultValue;
	}

void CCdlTkApiParam::SetDefaultValue(const std::string& aDefaultValue)
	{
	iDefaultValue = aDefaultValue;
	}


bool CCdlTkApiParam::operator==(const CCdlTkApiParam& aOther) const
	{
	return Type() == aOther.Type() && Name() == aOther.Name() && DefaultValue() == aOther.DefaultValue();
	}


CCdlTkApiParams::iterator CCdlTkApiParams::FindByName(string aParamName)
	{
	iterator it;
	for (it = begin(); it != end(); ++it)
		{
		if (it->Name() == aParamName)
			break;
		}
	return it;
	}


//
// CCdlTkApi
//

CCdlTkApi::CCdlTkApi(CCdlTkInterface& aInterface)
: iInterface(aInterface)
	{
	}

CCdlTkApi::~CCdlTkApi()
	{
	}

const string& CCdlTkApi::ReturnType() const
	{
	return iReturnType;
	}

void CCdlTkApi::SetReturnType(const string& aType)
	{
	iReturnType = aType;
	}

bool CCdlTkApi::IsVoidReturn() const
	{
	return iReturnType == "void";
	}

const string& CCdlTkApi::Name() const
	{
	return iName;
	}

void CCdlTkApi::SetName(const string& aName)
	{
	iName = aName;
	}

int CCdlTkApi::SourceFileLineNum() const
	{
	return iSourceFileLineNum;
	}

void CCdlTkApi::SetSourceFileLineNum(int aLineNum)
	{
	iSourceFileLineNum = aLineNum;
	}

const string& CCdlTkApi::Comment() const
	{
	return iComment;
	}

void CCdlTkApi::SetComment(const string& aComment)
	{
	iComment = aComment;
	}

const CCdlTkInterface& CCdlTkApi::Interface() const
	{
	return iInterface;
	}

CCdlTkInterface& CCdlTkApi::Interface()
	{
	return iInterface;
	}

const CCdlTkFunctionApi& CCdlTkApi::AsFunc() const
	{
	if (!IsFunc())
		throw CdlTkAssert("API is not a function");
	return static_cast<const CCdlTkFunctionApi&>(*this);
	}

const CCdlTkDataApi& CCdlTkApi::AsData() const
	{
	if (IsFunc())
		throw CdlTkAssert("API is not data");
	return static_cast<const CCdlTkDataApi&>(*this);
	}

bool CCdlTkApi::operator!=(const CCdlTkApi& aOther) const
	{
	return !(*this == aOther);
	}

bool CCdlTkApi::operator==(const CCdlTkApi& aOther) const
	{
	return 
		ReturnType() == aOther.ReturnType() &&
		Name() == aOther.Name() &&
		IsFunc() == aOther.IsFunc();
	}

void CCdlTkApi::operator=(const CCdlTkApi& aOther)
	{
	iReturnType = aOther.iReturnType;
	iName = aOther.iName;
	iSourceFileLineNum = aOther.iSourceFileLineNum;
	}


//
// CCdlTkDataApi
//

CCdlTkDataApi::CCdlTkDataApi(CCdlTkInterface& aInterface)
: CCdlTkApi(aInterface)
	{
	}

CCdlTkDataApi::CCdlTkDataApi(CCdlTkInterface& aInterface, const CCdlTkDataApi& aCopy)
: CCdlTkApi(aInterface)
	{
	CCdlTkApi::operator=(aCopy);
	}

bool CCdlTkDataApi::IsFunc() const
	{
	return false;
	}

string CCdlTkDataApi::PointerType() const
	{
	return ReturnType() + " const*";
	}

string CCdlTkDataApi::ParamsTypeAndNameList() const
	{
	return "";
	}

CCdlTkApi* CCdlTkDataApi::Clone(CCdlTkInterface& aInterface) const
	{
	return new CCdlTkDataApi(aInterface, *this);
	}


//
// CCdlTkFunctionApi
//

CCdlTkFunctionApi::CCdlTkFunctionApi(CCdlTkInterface& aInterface)
: CCdlTkApi(aInterface)
	{
	}

CCdlTkFunctionApi::CCdlTkFunctionApi(CCdlTkInterface& aInterface, const CCdlTkFunctionApi& aCopy)
: CCdlTkApi(aInterface)
	{
	CCdlTkApi::operator=(aCopy);
	iParams = aCopy.iParams;
	}

bool CCdlTkFunctionApi::IsFunc() const
	{
	return true;
	}

string CCdlTkFunctionApi::PointerType() const
	{
	return ApiNameAsTypeName() + "*";
	}

string CCdlTkFunctionApi::ParamsTypeAndNameList() const
	{
	string ret = "(";
	for (CCdlTkApiParams::const_iterator pParam = iParams.begin(); pParam != iParams.end(); ++pParam)
		{
		if (pParam != iParams.begin())
			CdlTkUtil::AppendString(ret, string(",") + KSpace);
		CdlTkUtil::AppendString(ret, pParam->Type() + KSpace + pParam->Name());
		if(!(pParam->DefaultValue().empty()))
			CdlTkUtil::AppendString(ret, KSpace + KEqualsSign + KSpace + pParam->DefaultValue());
		}
	ret += ")";
	return ret;
	}

CCdlTkApiParams& CCdlTkFunctionApi::Params()
	{
	return iParams;
	}

const CCdlTkApiParams& CCdlTkFunctionApi::Params() const
	{
	return iParams;
	}

string CCdlTkFunctionApi::ApiNameAsTypeName() const
	{
	string name = "T";
	name += Name();
	name += "_sig";
	return name;
	}

string CCdlTkFunctionApi::ParamNameList() const
	{
	string ret = "";
	for (CCdlTkApiParams::const_iterator pParam = iParams.begin(); pParam != iParams.end(); ++pParam)
		{
		if (pParam != iParams.begin())
			CdlTkUtil::AppendString(ret, ", ");
		CdlTkUtil::AppendString(ret, pParam->Name());
		}
	return ret;
	}

string CCdlTkFunctionApi::ParamTypeList() const
	{
	string ret = "";
	for (CCdlTkApiParams::const_iterator pParam = iParams.begin(); pParam != iParams.end(); ++pParam)
		{
		if (pParam != iParams.begin())
			CdlTkUtil::AppendString(ret, ", ");
		CdlTkUtil::AppendString(ret, pParam->Type());
		}
	return ret;
	}

bool CCdlTkFunctionApi::operator==(const CCdlTkApi& aOther) const
	{
	return 
		CCdlTkApi::operator==(aOther) && 
		Params() == aOther.AsFunc().Params();	// calling AsFunc() is safe because base-class == will fail if this is not a function API
	}

CCdlTkApi* CCdlTkFunctionApi::Clone(CCdlTkInterface& aInterface) const
	{
	return new CCdlTkFunctionApi(aInterface, *this);
	}


//
// CCdlTkApiList
//

CCdlTkApiList::CCdlTkApiList()
	{
	}

void CCdlTkApiList::Copy(const CCdlTkApiList& aOther, CCdlTkInterface& aInterface)
	{
	if (this == &aOther)
		return;
	DeleteApis();
	clear();
	reserve(aOther.size());
	for (const_iterator pApi = aOther.begin(); pApi != aOther.end(); ++pApi)
		push_back((*pApi)->Clone(aInterface));
	}

CCdlTkApiList::~CCdlTkApiList()
	{
	DeleteApis();
	}

void CCdlTkApiList::DeleteApis()
	{
	for (iterator it = begin(); it != end(); ++it)
		delete *it;
	}

void CCdlTkApiList::MergeExtendedApi(CCdlTkInterface& aInterface, const CCdlTkApiList& aOther)
	{
	for (const_iterator it = aOther.begin(); it != aOther.end(); ++it)
		push_back((*it)->Clone(aInterface));
	}

CCdlTkApi* CCdlTkApiList::Find(const string& aName) const
	{
	for (const_iterator pApi = begin(); pApi != end(); ++pApi)
		{
		if ((*pApi)->Name() == aName)
			return *pApi;
		}
	return 0;
	}

bool CCdlTkApiList::operator==(const CCdlTkApiList& aOther) const
	{
	if (size() != aOther.size())
		return false;
	const_iterator pOther = aOther.begin();
	for (const_iterator pApi = begin(); pApi != end(); ++pApi)
		{
		if (**pApi != **pOther)
			return false;
		++pOther;
		}
	return true;
	}

bool CCdlTkApiList::IsSubsetOf(const CCdlTkApiList& aOther) const
	{
	for (const_iterator pApi = begin(); pApi != end(); ++pApi)
		{
		CCdlTkApi* api = aOther.Find((*pApi)->Name());
		if (!api || **pApi != *api)
			return false;
		}
	return true;
	}


//
// CCdlTkCpp
//

CCdlTkCpp::CCdlTkCpp() 
	{
	}

void CCdlTkCpp::MergeExtensionCpp(const CCdlTkCpp& aOther)
	{
	insert(end(), aOther.begin(), aOther.end());
	}


//
// CCdlTkDataTypeTranslation
//

CCdlTkDataTypeTranslation::CCdlTkDataTypeTranslation()
: iDefn(""), iPtrRef(""), iSource(EFromCdl)
	{
	SetType("");
	}

CCdlTkDataTypeTranslation::CCdlTkDataTypeTranslation(const string& aType)
: iDefn(""), iPtrRef(""), iSource(EFromCdl)
	{
	SetType(aType);
	}

CCdlTkDataTypeTranslation::CCdlTkDataTypeTranslation(const string& aType, const string& aDefn, const string& aPtrRef, TTranslationSource aSource)
: iDefn(aDefn), iPtrRef(aPtrRef), iSource(aSource)
	{
	SetType(aType);
	}

string CCdlTkDataTypeTranslation::Type() const
	{
	return iType;
	}

void CCdlTkDataTypeTranslation::SetType(const string& aType)
	{
	iType = aType;
	iTypeSize = iType.size();
	iTypeVarPos = iType.find(KType);
	if (iTypeVarPos != string::npos)
		{
		iTypeSizeWithoutTypeVar = iTypeSize - KTypeSize;
		iTextBeforeTypeVar = iType.substr(0, iTypeVarPos);
		iTextAfterTypeVar = iType.substr(iTypeVarPos + KTypeSize);
		iSizeAfterTypeVar = iTextAfterTypeVar.size();
		}
	}

string CCdlTkDataTypeTranslation::Definition() const
	{
	return iDefn;
	}

void CCdlTkDataTypeTranslation::SetDefinition(const string& aDefn)
	{
	iDefn = aDefn;
	}

string CCdlTkDataTypeTranslation::PointerReference() const
	{
	return iPtrRef;
	}

void CCdlTkDataTypeTranslation::SetPointerReference(const string& aPtrRef)
	{
	iPtrRef = aPtrRef;
	}

CCdlTkDataTypeTranslation::TTranslationSource CCdlTkDataTypeTranslation::Source() const
	{
	return iSource;
	}

bool CCdlTkDataTypeTranslation::Match(const string& aType, string& aTypeVar) const
	{
	// KType is the string "aType". If this appears in iType, it matches
	// any text in aType, and the matching text is placed in aTypeVar, otherwise
	// aType must equal iType for a match to be found.
	if (iTypeVarPos == string::npos)
		{
		// iType does not contain "aType". Give aTypeVar a dummy result
		// and compare the two types.
		aTypeVar = KType;
		return iType == aType;
		}
	else
		{
		// iType contains the string "aType", so check if aType is big enough
		// and that the start matches and the end matches.
		int aTypeSize = aType.size();
		if (iTypeSizeWithoutTypeVar >= aTypeSize ||
			iTextBeforeTypeVar != aType.substr(0, iTypeVarPos) ||
			iTextAfterTypeVar != aType.substr(aTypeSize - iSizeAfterTypeVar))
			return false;
		// Set aTypeVar to be the part of aType that corresponds to the string
		// "aType".
		aTypeVar = aType.substr(iTypeVarPos, aTypeSize - iTypeSizeWithoutTypeVar);
		return true;
		}
	}

bool CCdlTkDataTypeTranslation::operator==(const CCdlTkDataTypeTranslation& aOther) const
	{
	return
		iType == aOther.iType &&
		iDefn == aOther.iDefn &&
		iPtrRef == aOther.iPtrRef &&
		iSource == aOther.iSource;
	}


//
// CCdlTkDataTypeTranslations
//

CCdlTkDataTypeTranslations::CCdlTkDataTypeTranslations()
	{
	// gStdTranslations is defined in CdlTkStdTrans.h
	int count = sizeof(gStdTranslations)/sizeof(*gStdTranslations);
	for (int ii=0; ii<count; ii++)
		{
		const SStdTranslation& trans = gStdTranslations[ii];
		push_back(CCdlTkDataTypeTranslation(trans.iType, trans.iInit, trans.iRef, CCdlTkDataTypeTranslation::EInbuilt));
		}
	}

void CCdlTkDataTypeTranslations::MergeExtensionTranslations(const CCdlTkDataTypeTranslations& aOther)
	{
	for (const_iterator pOther = aOther.begin(); pOther != aOther.end(); ++pOther)
		{
		if (pOther->Source() != CCdlTkDataTypeTranslation::EInbuilt)
			push_back(*pOther);
		}
	}

const CCdlTkDataTypeTranslation* CCdlTkDataTypeTranslations::Find(const string& aType, string& aTypeVar) const
	{
	for (const_reverse_iterator it = rbegin(); it != rend(); ++it)
		{
		if (it->Match(aType, aTypeVar))
			return &*it;
		}
	return NULL;
	}


//
// CCdlTkInterface
//

CCdlTkInterface::CCdlTkInterface()
: iBase(0), iExtension(0)
	{
	}

CCdlTkInterface::~CCdlTkInterface()
	{
	delete iExtension;
	}

CCdlTkInterface::CCdlTkInterface(const CCdlTkInterface& aCopy)
: iBase(0), iExtension(0)
	{
	*this = aCopy;
	}

void CCdlTkInterface::operator=(const CCdlTkInterface& aCopy)
	{
	if (&aCopy == this)
		return;

	iFileName = aCopy.iFileName;
	iHeader = aCopy.iHeader;
	iCpp = aCopy.iCpp;
	iApiList.Copy(aCopy.iApiList, *this);
	iDataTypeTranslations = aCopy.iDataTypeTranslations;
	iBase = aCopy.iBase;
	delete iExtension;
	iExtension = 0;
	if (aCopy.iExtension)
		iExtension = new CCdlTkInterface(*aCopy.iExtension);
	}

CCdlTkInterface* CCdlTkInterface::Base() const
	{
	return iBase;
	}

void CCdlTkInterface::SetBase(CCdlTkInterface* aBase)
	{
	iBase = aBase;
	}

CCdlTkInterface* CCdlTkInterface::Extension() const
	{
	return iExtension;
	}

void CCdlTkInterface::SetExtension(CCdlTkInterface* aExtension)
	{
	delete iExtension;
	iExtension = aExtension;
	}

CCdlTkInterface* CCdlTkInterface::UltimateBase()
	{
	CCdlTkInterface* base = this;
	while (base->Base())
		base = base->Base();
	return base;
	}

const CCdlTkInterface* CCdlTkInterface::UltimateBase() const
	{
	const CCdlTkInterface* base = this;
	while (base->Base())
		base = base->Base();
	return base;
	}

CCdlTkInterface* CCdlTkInterface::UltimateExtension()
	{
	CCdlTkInterface* extension = this;
	while (extension->Extension())
		extension = extension->Extension();
	return extension;
	}

const CCdlTkInterface* CCdlTkInterface::UltimateExtension() const
	{
	const CCdlTkInterface* extension = this;
	while (extension->Extension())
		extension = extension->Extension();
	return extension;
	}

void CCdlTkInterface::MergeExtensions()
	{
	for (CCdlTkInterface* ext = Extension(); ext; ext = ext->Extension())
		{
		iHeader.MergeExtensionHeader(ext->iHeader);
		iCpp.MergeExtensionCpp(ext->iCpp);
		iApiList.MergeExtendedApi(*this, ext->iApiList);
		iDataTypeTranslations.MergeExtensionTranslations(ext->iDataTypeTranslations);
		}
	SetExtension(0);
	}

string CCdlTkInterface::FileName() const
	{
	return iFileName;
	}

void CCdlTkInterface::SetFileName(const string& aFileName)
	{
	iFileName = aFileName;
	}

string CCdlTkInterface::AdditionalComment() const
	{
	return iAdditionalComment;
	}

void CCdlTkInterface::SetAdditionalComment(const string& aAdditionalComment)
	{
	iAdditionalComment = aAdditionalComment;
	}

CCdlTkInterfaceHeader& CCdlTkInterface::Header()
	{
	return iHeader;
	}

const CCdlTkInterfaceHeader& CCdlTkInterface::Header() const
	{
	return iHeader;
	}

CCdlTkCpp& CCdlTkInterface::Cpp()
	{
	return iCpp;
	}

const CCdlTkCpp& CCdlTkInterface::Cpp() const
	{
	return iCpp;
	}

CCdlTkApiList& CCdlTkInterface::ApiList()
	{
	return iApiList;
	}

const CCdlTkApiList& CCdlTkInterface::ApiList() const
	{
	return iApiList;
	}

string CCdlTkInterface::NamespaceName() const
	{
	return CdlTkUtil::ToCpp(iHeader.Name());
	}

CCdlTkDataTypeTranslations& CCdlTkInterface::DataTypeTranslations()
	{
	return iDataTypeTranslations;
	}

const CCdlTkDataTypeTranslations& CCdlTkInterface::DataTypeTranslations() const
	{
	return iDataTypeTranslations;
	}

bool CCdlTkInterface::operator==(const CCdlTkInterface& aOther) const
	{
	return
		iHeader == aOther.iHeader &&
		iCpp == aOther.iCpp &&
		iApiList == aOther.iApiList &&
		iDataTypeTranslations == aOther.iDataTypeTranslations &&
		!iExtension == !aOther.iExtension &&
		(!iExtension || *iExtension == *aOther.iExtension);
	}


//
// CCdlTkInterfaceList
//

CCdlTkInterfaceList::CCdlTkInterfaceList()
	{
	}

CCdlTkInterfaceList::~CCdlTkInterfaceList()
	{
	for (iterator it = begin(); it != end(); ++it)
		delete *it;
	}


}	// end of namespace CdlCompilerToolkit
