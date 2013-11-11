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
#ifndef CDLTKINSTANCE_H
#define CDLTKINSTANCE_H
#pragma warning (disable:4786)	// disable "identifier was truncated to '255' characters in the browser information" warning

#include <CdlCompilerToolkit/CdlTkInterface.h>
#include <vector>
#include <set>

namespace CdlCompilerToolkit {

class CCdlTkDll;
class CCdlTkInstance;

const int KCdlTkGetInstanceIdFromHostDll = -1;

/**
* The implementation of a single API from a CDL interface.
*/
class CCdlTkImplementation
	{
public:
	/**
    * constructor
    * @param aInstance the customisation instance to which this implementation belongs
	* @param aApi the API that this implements
    */
	CCdlTkImplementation(const CCdlTkInstance& aInstance, const CCdlTkApi& aApi);
	/**
    * destructor
    */
	~CCdlTkImplementation();

	/**
    * Get the implementation's definition.
    * @return the implementation's definition.
    */
	std::string Definition() const;
	/**
    * Set the implementation's definition.
    * @param aDefn the implementation's definition.
    */
	void SetDefinition(const std::string& aDefn);
	/**
    * Set the implementation's definition to be that defined by the data type translations
	* provided by the interface.
    */
	void SetTemplateDefinition();
	/**
    * Get a pointer reference for the implementation.
    * @return a pointer reference for the implementation.
    */
	std::string PointerReference() const;
	/**
    * Set a pointer reference for the implementation.
    * @param aPtr a pointer reference for the implementation.
    */
	void SetPointerReference(const std::string& aPtr);
	/**
    * Set the implementation's pointer reference to be that defined by the data type translations
	* provided by the interface.
    */
	void SetTemplatePointerReference();

	/**
    * Set both the definition and pointer reference to be that defined by the data type 
	* translations provided by the interface.
    */
	void SetTemplate();
	/**
    * Set an empty definition and NULL pointer reference
    */
	void SetNull();

	/**
    * Get the customisation instance to which this implementation belongs
    * @return the customisation instance to which this implementation belongs
    */
	const CCdlTkInstance& Instance() const;
	/**
    * Get the API which this implementation implements
    * @return the API which this implementation implements
    */
	const CCdlTkApi& Api() const;
	/**
    * Get the name of the implementation, which is also the name of API that this implements
    * @return the name of the implementation
    */
	const std::string& Name() const;

private:
	const CCdlTkDataTypeTranslation* GetTranslation(const std::string& aType, std::string& aTypeVar);

private:
	const CCdlTkInstance& iInstance;
	const CCdlTkApi& iApi;
	std::string iDefn;
	std::string iPtr;
	};

/**
* A collection of implementations of the APIs in an API list. There is one implementation
* for each API and they appear in the same order as the API list.
*/
class CCdlTkImplementations
	{
private:
	typedef std::vector<CCdlTkImplementation*> CImpl;

public:
	/**
    * constructor
    * @param aInstance the customisation instance to which these implementations belong
	* @param aApiList the APIs that define what implementations appear in this collection
    */
	CCdlTkImplementations(const CCdlTkInstance& aInstance, const CCdlTkApiList& aApiList);
	/**
    * destructor
    */
	~CCdlTkImplementations();

	/**
    * Find an implementation by name
    * @param aName the name of the implemntation to find
    * @return the implmentation, or NULL if none is found
    */
	CCdlTkImplementation* Find(const std::string& aName) const;
	/**
    * Set all implementations to be null.
    */
	void ClearAll();
	/**
    * Set all implementations to have the template definition and pointer reference syntax
	* defined by the data type translations from the interface.
    */
	void TemplateAll();

public:
	// iterator interface
	/**
    * The iterator type
    */
	typedef CImpl::iterator iterator;
	/**
    * The const iterator type
    */
	typedef CImpl::const_iterator const_iterator;
	/**
    * begin iterator
    */
	iterator begin() { return iImpl.begin(); }
	/**
    * begin iterator
    */
	const_iterator begin() const { return iImpl.begin(); }
	/**
    * end iterator
    */
	iterator end() { return iImpl.end(); }
	/**
    * end iterator
    */
	const_iterator end() const { return iImpl.end(); }

private:
	CImpl iImpl;
	};


/**
* a customisation instance
*/
class CCdlTkInstance
	{
public:
	/**
    * constructor
    * @param aInterface the CDL interface that this instance implements
    */
	CCdlTkInstance(const CCdlTkInterface& aInterface);
	/**
    * destructor
    */
	~CCdlTkInstance();

	/**
    * Get the implementations that make up this instance
    * @return the implementations that make up this instance
    */
	CCdlTkImplementations& Impl();
	/**
    * Get the implementations that make up this instance
    * @return the implementations that make up this instance
    */
	const CCdlTkImplementations& Impl() const;
	/**
    * Set all implementations to be null.
    */
	void ClearAllImplementations();
	/**
    * Set all implementations to have the template definition and pointer reference syntax
	* defined by the data type translations from the interface.
    */
	void TemplateAllImplementations();

	/**
    * Set the name of this customisation instance
    * @param aName the name of this customisation instance
    */
	void SetName(const std::string& aName);
	/**
    * Get the name of this customisation instance
    * @return the name of this customisation instance
    */
	const std::string& Name() const;
	/**
    * Get an identifier for this customisation instance that its
	* host DLL can use to identify it.
    * @return the fully qualified name of this customisation instance
    */
	std::string DllInstanceName() const;
	/**
    * Turn an instance name into an identifier that a DLL can use to set
	* the instance id.
	* @param aName the instance name
    * @return the DLL instance id string
    */
	static std::string InstanceNameToDllInstanceName(const std::string& aName);

	/**
    * Set the id of this customisation instance to be got from the host DLL. 
	* The id is used to distinguish this instance from others in a customisation DLL.
	* Getting the id from the host DLL optimised the lookup process.
	* Note, the instance is automatically created with the this id. So, this
	* function only needs to be used if the id has been previously set to another
	* value.
    */
	void SetId();
	/**
    * Set the id of this customisation instance. 
	* The id is used to distinguish this instance from others in a customisation DLL.
	* This function does not generate as efficient code as SetId()
    * @param aId the id of this customisation instance
    */
	void SetId(int aId);
	/**
    * Get the id of this customisation instance. 
	* The id is used to distinguish this instance
	* from others in a customisation DLL.
    * @return the id of this customisation instance, which may be
	* KCdlTkGetInstanceIdFromHostDll if the id is to come from the host DLL.
    */
	int Id() const;

	/**
    * Get the interface that this instance implements.
    * @return the interface that this instance implements.
    */
	const CCdlTkInterface& Interface() const;

	/**
    * Set the extra C++ section for this instance. This may add any extra C++ code required
	* by the API implementations. For instance, helper functions or #includes
    * @param aExtra the extra C++ section for this instance
    */
	void SetExtraCpp(const std::string& aExtra);
	/**
    * Get the extra C++ section for this instance. This may be extra C++ code required
	* by the API implementations. For instance, helper functions or #includes
    * @return the extra C++ section for this instance
    */
	const std::string& ExtraCpp() const;

private:
	const CCdlTkInterface& iInterface;
	CCdlTkImplementations iImpl;
	std::string iName;
	int iId;
	std::string iExtraCpp;
	};


/**
* A customisation instance that implements a CDL package interface.
* a CDL package interface is one that contains the following API:
*   TCdlArray<TCdlRef> contents;
*/
class CCdlTkPackage : public CCdlTkInstance
	{
private:
	class CRef
		{
	public:
		CRef();
		const std::string& LocalInstanceName() const;
		void SetLocalInstanceName(const std::string& aLocalInstanceName);
		int Id() const;
		void SetId(int aId);
		int Uid() const;
		void SetUid(int aUid);
		const std::string& DllName() const;
		void SetDllName(const std::string& aDllName);
		const std::string& DllSourcePath() const;
		void SetDllSourcePath(const std::string& aDllSourcePath);
		const std::string& DllInstName() const;
		void SetDllInstName(const std::string& aDllInstName);

	private:
		std::string iLocalInstanceName;
		int iId;
		int iUid;
		std::string iDllName;
		std::string iDllSourcePath;
		std::string iDllInstName;
		};

	typedef std::vector<CRef> CRefs;

public:
	/**
    * constructor
    * @param aInterface the CDL package interface that this package implements
    */
	CCdlTkPackage(const CCdlTkInterface& aInterface);
	/**
    * destructor
    */
	~CCdlTkPackage();
	
	/**
    * Adds a customisation instance which will appear in the same DLL as this package
    * @param aInstance a customisation instance to be referenced by this package
    */
	void AddLocalContent(const CCdlTkInstance& aInstance);
	/**
    * Adds a customisation instance which will appear in the same DLL as this package
    * @param aLocalInstanceName a customisation instance name to be referenced by this package
    */
	void AddLocalContent(const std::string& aLocalInstanceName);
	/**
    * Adds a customisation instance which will appear in another DLL to this package
    * @param aInstance the instance to add to this package
	* @param aDll the DLL that the instance appears in.
	* @param aDllSourcePath the path to the source code for the DLL that 
	* the instance appears in. This may be an empty string if the instance
	* has an instance id other than KCdlTkGetInstanceIdFromHostDll, or if the
	* instance appears in the same DLL as this package.
    */
	void AddContent(const CCdlTkInstance& aInstance, const CCdlTkDll& aDll, const std::string& aDllSourcePath);
	/**
    * Adds a customisation instance which may appear in another DLL to this package
    * @param aId the id of the customisation instance.
    * @param aUid the UID of the customisation instance's interface
    * @param aDllName the DLL in which the customisation instance appears. This may be
	* an empty string if the instance appears in the same DLL as this package.
    */
	void AddContent(int aId, int aUid, const std::string& aDllName);
	/**
    * Adds a customisation instance which appears in another DLL created by the
	* CDL compiler, and whose source is available.
    * @param aInstanceName the customisation instance name used in the other DLL
    * @param aDllSourcePath the path to the other DLLs source
    * @param aDllName the name of the other DLL
    */
	void AddExternalContent(const std::string& aInstanceName, const std::string& aDllSourcePath, const std::string& aDllName);

private:
	const CRefs& Contents() const;
	void GenerateImpl();
	void AddIncludesAndBuildDllSet(std::string& aDefn, std::set<std::string>& aDllNames);
	void AddDllNameLits(std::string& aDefn, std::set<std::string>& aDllNames);
	void AddContents(std::string& aDefn);

private:
	CRefs iContents;
	CCdlTkImplementation* iContentsImpl;	// not owned, can't be initialised until base class is constructed
	};


/**
* a customisation DLLs contents and build information
*/
class CCdlTkDll
	{
public:
	/**
    * a collection of customisation instance names that will appear in the DLL
    */
	typedef std::vector<std::string> CInstances;
	/**
    * a collection of libraries that the DLL needs to link to
    */
	typedef std::vector<std::string> CLibraries;

public:
	/**
    * constructor
    */
	CCdlTkDll();
	/**
    * destructor
    */
	~CCdlTkDll();
	
	/**
    * Adds a customisation instance to the DLL
    * @param aInstance a customisation instance
    */
	void AddInstance(const CCdlTkInstance& aInstance);
	/**
    * Adds a customisation instance to the DLL
    * @param aInstanceName the name of a customisation instance
    */
	void AddInstance(const std::string& aInstanceName);
	/**
    * Gets the collection of customisation instances that appear in this DLL
    * @return the collection of customisation instances that appear in this DLL
    */
	const CInstances& Instances() const;
	
	/**
    * Set the name of the DLL
    * @param aName the name of the DLL
    */
	void SetName(const std::string& aName);
	/**
    * Get the name of the DLL
    * @return the name of the DLL
    */
	const std::string& Name() const;

	/**
    * Set the UID of the DLL
    * @param aUid the UID of the DLL
    */
	void SetUid(int aUid);
	/**
    * Get the UID of the DLL
    * @return the UID of the DLL
    */
	int Uid() const;

	/**
    * Set the version of the DLL
    * @param aVersion the version number of the DLL
    */
	void SetVersion(int aVersion);
	/**
    * Get the version number of the DLL
    * @return the version number of the DLL
    */
	int Version() const;

	/**
    * Add a library name to the DLL
    * @param aLibName a library name
    */
	void AddLibrary(const std::string& aLibName);
	/**
    * Get the collection of libraries that this DLL links to
    * @return the collection of libraries that this DLL links to
    */
	const CLibraries& Libraries() const;
	/**
    * Get the collection of libraries that this DLL links to
    * @return the collection of libraries that this DLL links to
    */
	CLibraries& Libraries();

	/**
    * Set any extra MMP file content that the DLL needs
    * @param aExtraMmp extra MMP file content that the DLL needs
    */
	void SetExtraMmp(const std::string& aExtraMmp);
	/**
    * Get the extra MMP file content that has been set for the DLL
    * @return the extra MMP file content that has been set for the DLL
    */
	const std::string& ExtraMmp() const;

private:
	std::string iName;
	int iUid;
	CInstances iInstances;
	CLibraries iLibraries;
	std::string iExtraMmp;
	std::set<std::string> iUniqueInstances;
	int iVersion;
	};


}	// end of namespace CdlCompilerToolkit

#endif
