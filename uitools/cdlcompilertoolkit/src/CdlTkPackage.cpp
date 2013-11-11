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
#include <sstream>
using namespace std;

namespace CdlCompilerToolkit {

//
// PackageErr
//

class PackageErr : public CdlCompilerToolkitErr
	{
public:
	void Show(ostream& aStream) const;
	};

void PackageErr::Show(ostream& aStream) const
	{
	aStream << "Interface does not define a package";
	}


//
// CCdlTkPackage::CRef
//

CCdlTkPackage::CRef::CRef()
: iId(KCdlTkGetInstanceIdFromHostDll), iUid(0)
	{
	}

const string& CCdlTkPackage::CRef::LocalInstanceName() const
	{
	return iLocalInstanceName;
	}

void CCdlTkPackage::CRef::SetLocalInstanceName(const string& aLocalInstanceName)
	{
	iLocalInstanceName = aLocalInstanceName;
	}

int CCdlTkPackage::CRef::Id() const
	{
	return iId;
	}

void CCdlTkPackage::CRef::SetId(int aId)
	{
	iId = aId;
	}

int CCdlTkPackage::CRef::Uid() const
	{
	return iUid;
	}

void CCdlTkPackage::CRef::SetUid(int aUid)
	{
	iUid = aUid;
	}

const string& CCdlTkPackage::CRef::DllName() const
	{
	return iDllName;
	}

void CCdlTkPackage::CRef::SetDllName(const string& aDllName)
	{
	iDllName = aDllName;
	}

const string& CCdlTkPackage::CRef::DllSourcePath() const
	{
	return iDllSourcePath;
	}

void CCdlTkPackage::CRef::SetDllSourcePath(const string& aDllSourcePath)
	{
	iDllSourcePath = aDllSourcePath;
	}

const string& CCdlTkPackage::CRef::DllInstName() const
	{
	return iDllInstName;
	}

void CCdlTkPackage::CRef::SetDllInstName(const string& aDllInstName)
	{
	iDllInstName = aDllInstName;
	}



//
// CCdlTkPackage
//

CCdlTkPackage::CCdlTkPackage(const CCdlTkInterface& aInterface)
: CCdlTkInstance(aInterface)
	{
	iContentsImpl = Impl().Find(KPackageContentsApi);
	if (!iContentsImpl)
		throw PackageErr();
	GenerateImpl();
	}

CCdlTkPackage::~CCdlTkPackage()
	{
	}

void CCdlTkPackage::AddLocalContent(const CCdlTkInstance& aInstance)
	{
	CRef ref;
	ref.SetLocalInstanceName(aInstance.Name());
	iContents.push_back(ref);
	GenerateImpl();
	}

void CCdlTkPackage::AddLocalContent(const string& aLocalInstanceName)
	{
	CRef ref;
	ref.SetLocalInstanceName(aLocalInstanceName);
	iContents.push_back(ref);
	GenerateImpl();
	}

void CCdlTkPackage::AddContent(const CCdlTkInstance& aInstance, const CCdlTkDll& aDll, const std::string& aDllSourcePath)
	{
	CRef ref;
	ref.SetId(aInstance.Id());
	ref.SetUid(aInstance.Interface().Header().Uid());
	ref.SetDllName(aDll.Name());
	ref.SetDllSourcePath(aDllSourcePath);
	ref.SetDllInstName(aInstance.DllInstanceName());
	iContents.push_back(ref);
	GenerateImpl();
	}

void CCdlTkPackage::AddContent(int aId, int aUid, const string& aDllName)
	{
	CRef ref;
	ref.SetId(aId);
	ref.SetUid(aUid);
	ref.SetDllName(aDllName);
	iContents.push_back(ref);
	GenerateImpl();
	}

void CCdlTkPackage::AddExternalContent(const string& aInstanceName, const string& aDllSourcePath, const string& aDllName)
	{
	CRef ref;
	ref.SetDllInstName(aInstanceName);
	ref.SetDllName(aDllName);
	ref.SetDllSourcePath(aDllSourcePath);
	iContents.push_back(ref);
	GenerateImpl();
	}

const CCdlTkPackage::CRefs& CCdlTkPackage::Contents() const
	{
	return iContents;
	}

const string KArrayStart = "\
\n\
CDL_ARRAY_START(TCdlRef, contents)\n\
\t{\n";

const string KArrayEnd = "\
\t}\n\
CDL_ARRAY_END(TCdlRef, contents);\n";

void CCdlTkPackage::GenerateImpl()
	{
	string defn;

	if (iContents.empty())
		{
		defn = "EMPTY_CDL_ARRAY(TCdlRef, contents)\n";
		}
	else
		{
		set<string> dllNames;
		AddIncludesAndBuildDllSet(defn, dllNames);
		AddDllNameLits(defn, dllNames);
		CdlTkUtil::AppendString(defn, KArrayStart);
		AddContents(defn);
		CdlTkUtil::AppendString(defn, KArrayEnd);
		}

	iContentsImpl->SetDefinition(defn);
	}

const string KInclude = "#include \"$NAME\"\n";

void CCdlTkPackage::AddIncludesAndBuildDllSet(string& aDefn, set<string>& aDllNames)
	{
	for (CRefs::iterator pRef = iContents.begin(); pRef != iContents.end(); ++pRef)
		{
		CRef& ref = *pRef;
		string locInst = CdlTkUtil::ToLower(ref.LocalInstanceName());
		if (locInst.empty())
			{
			// if it's not a local instance, add its dll name to the set
			string dllName = ref.DllName();
			if (!dllName.empty())
				aDllNames.insert(dllName);

			// if the instance id comes from its host DLL, #include the host DLL ids
			if (ref.Id() == KCdlTkGetInstanceIdFromHostDll)
				{
				string include = CdlTkUtil::ToLower(ref.DllSourcePath() + KDllInstHeader);
				CdlTkUtil::AppendString(aDefn, CdlTkUtil::Replace("$NAME", include, KInclude));
				include = CdlTkUtil::ToLower(ref.DllSourcePath() + ref.DllInstName() + ".h");
				CdlTkUtil::AppendString(aDefn, CdlTkUtil::Replace("$NAME", include, KInclude));
				}
			}
		else
			{
			// #include local instances
			CdlTkUtil::AppendString(aDefn, CdlTkUtil::Replace("$NAME", locInst+".h", KInclude));
			}
		}
	}

const string KDllLit = "_LIT(_content_DLL_$CPP_NAME, \"$DLL_NAME\");\n";

void CCdlTkPackage::AddDllNameLits(string& aDefn, set<string>& aDllNames)
	{
	// add literals for all of the DLL names
	for (set<string>::iterator pDll = aDllNames.begin(); pDll != aDllNames.end(); ++pDll)
		{
		CdlTkUtil::AppendString(aDefn, 
			CdlTkUtil::Replace("$CPP_NAME", CdlTkUtil::ToCpp(*pDll), 
				CdlTkUtil::Replace("$DLL_NAME", *pDll, KDllLit)));
		}
	}

const string KLocRef = "\tLOCAL_CDL_REF($NAME),\n";

void CCdlTkPackage::AddContents(string& aDefn)
	{
	for (CRefs::iterator pRef = iContents.begin(); pRef != iContents.end(); ++pRef)
		{
		string locInst = pRef->LocalInstanceName();
		if (locInst.empty())
			{
			// if it's not a local instance use {} intialisation
			string id;
			string uid;
			if (pRef->Id() == KCdlTkGetInstanceIdFromHostDll)
				{
				id = pRef->DllInstName() + "::KCdlInstanceId";
				uid = pRef->DllInstName() + "::KCdlInterfaceUidValue";
				}
			else
				{
				id = CdlTkUtil::IntToString(pRef->Id());
				uid = pRef->Uid();
				}

			stringstream ref;
			ref << "\t{" << id << ", " << uid << ", ";

			// empty DLL name implies that the instance is in the same DLL,
			// use use a null pointer, otherwise use the literal
			string dllName = pRef->DllName();
			if (dllName.empty())
				ref << "NULL";
			else
				ref << "LIT_AS_DESC_PTR(_content_DLL_" << CdlTkUtil::ToCpp(dllName) << ")";
			ref << "},\n";

			CdlTkUtil::AppendString(aDefn, ref.str());
			}
		else
			{
			// local instances use the LOCAL_CDL_REF macro.
			CdlTkUtil::AppendString(aDefn, CdlTkUtil::Replace("$NAME", CdlTkUtil::ToCpp(locInst), KLocRef));
			}
		}
	}


}	// end of namespace CdlCompilerToolkit
