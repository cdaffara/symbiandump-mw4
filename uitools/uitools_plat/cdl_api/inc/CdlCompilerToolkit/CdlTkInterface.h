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
#ifndef CDLTKINTERFACE_H
#define CDLTKINTERFACE_H
#pragma warning (disable:4786)	// disable "identifier was truncated to '255' characters in the browser information" warning


#include <vector>
#include <set>
#include <string>
#include <CdlCompilerToolkit/CdlTkUtil.h>

namespace CdlCompilerToolkit {

class CCdlTkInterface;
class CCdlTkFunctionApi;
class CCdlTkDataApi;


/**
* Header information for a CDL interface
*/
class CCdlTkInterfaceHeader
	{
public:
	/**
	* A collection of CDL interface flags. This class has operations to allow
	* the flags to be manipulated through their textual name, or as an array
	* of bits.
	*/
	class CFlags
		{
	public:
		/**
		* constructor
		*/
		CFlags();
		
		/**
		* Sets a flag to be on
		* @param aFlagName the name of the flag to be turned on
		*/
		void SetFlag(const std::string& aFlagName);
		/**
		* Sets a flag to be off
		* @param aFlagName the name of the flag to be turned off
		*/
		void ClearFlag(const std::string& aFlagName);
		
		/**
		* Returns a representation of the flags as a 32-bit integer
		* @return a 32-bit integer containing a bit patterns representing the flags
		*/
		int FlagsAsInt() const;
		/**
		* Returns a string representation of the flags
		* @return a string containing the set flags' names combined with the
		* C++ bitwise OR operator |.
		* If no flags are set, "0" is returned.
		*/
		std::string FlagsAsString() const;

		/**
		* Gives the number of bit flags that this class uses.
		* @return the  number of bit flags that this class uses.
		*/
		int Count() const;
		/**
		* Test whether a flag is set, using a flag index between 0 and Count()-1
		* to identify the flag.
		* @param aFlagIndex the index of the flag in question
		* @return the bool value of the flag
		*/
		bool IsSet(int aFlagIndex) const;
		/**
		* Test whether a flag is set, using the flag name to identify the flag
		* @param aFlagName the name of the flag in question
		* @return the bool value of the flag
		*/
		bool IsSet(const std::string& aFlagName) const;

		/**
		* Adds the set flags in another CFlags object to this one.
		* @param aOther another CFlags object
		* @return this modified object.
		*/
		CFlags& operator|=(const CFlags& aOther);

		/**
		* Returns the name of a flag identified by its index
		* @param aFlagIndex the index of the flag, between 0 and Count()-1
		* @return the name of the flag
		*/
		std::string FlagName(int aFlagIndex) const;

	private:
		int IndexToFlagVal(int aIndex) const;
		int FlagVal(const std::string& aFlagName) const;

	private:
		int iFlags;
		};

	/**
	* A version number
	*/
	class CVersion
		{
	public:
		/**
		* constructor
		*/
		CVersion();
		/**
		* constructor
		* @param aMajor the major version number
		* @param aMinor the minor version number
		*/
		CVersion(int aMajor, int aMinor);
	
		/**
		* Compare two versions
		* @param aOther another version number
		* @return true if this version is less than the other version
		*/
		bool operator<(const CVersion aOther) const;
		/**
		* Compare two versions
		* @param aOther another version number
		* @return true if the two versions are equal
		*/
		bool operator==(const CVersion aOther) const;

		/**
		* The major version number
		* @return the major version number
		*/
		int Major() const;
		/**
		* Set the major verion number
		* @param aMajor the major version number
		*/
		void SetMajor(int aMajor);
		/**
		* The minor version number
		* @return the minor version number
		*/
		int Minor() const;
		/**
		* Set the minor verion number
		* @param aMinor the minor version number
		*/
		void SetMinor(int aMinor);

	private:
		int iMajor;
		int iMinor;
		};

public:
	/**
    * constructor
    */
	CCdlTkInterfaceHeader();
	/**
    * Merge another header in with this one. This process is normally used
	* when colapsing an extended CDL interface into a monolithic one.
	* The other header is checked to make sure that it does nothing illegal,
	* like change the name or UID, or decrease the version number, before the
	* flags are merged and the version number is copied.
    * @param aOther the header to merge with this one.
    */
	void MergeExtensionHeader(const CCdlTkInterfaceHeader& aOther);

	/**
    * Get the name of the CDL interface
    * @return the name
    */
	std::string Name() const;
	/**
    * Set the name of the CDL interface
    * @param aName the name for the CDL interface
    */
	void SetName(const std::string& aName);

	/**
    * Get the UID of the CDL interface
    * @return the UID
    */
	int Uid() const;
	/**
    * Set the UID of the CDL interface
    * @param aUid the UID for the CDL interface
    */
	void SetUid(int aUid);

	/**
    * Set the version of the CDL interface.
    * @param aVersion The version for the interface
    */
	void SetVersion(const CVersion& aVersion);
	/**
    * Get the version of the CDL interface.
    * @return the version of the CDL interface.
    */
	const CVersion& Version() const;

	/**
    * Get the flags for the CDL interface.
    * @return the flags
    */
	const CFlags& Flags() const;
	/**
    * Get the flags for the CDL interface.
    * @return a modifiable flags object
    */
	CFlags& Flags();

	/**
    * Compare this header with another
    * @param aOther a header to compare this one with
    * @return true if they are equal
    */
	bool operator==(const CCdlTkInterfaceHeader& aOther) const;

private:
	std::string iName;
	int iUid;
	CVersion iVer;
	CFlags iFlags;
	};


/**
* A single parameter in a function API in a CDL interface. It contains
* a type and a name.
*/
class CCdlTkApiParam
	{
public:
	/**
    * constructor
    */
	CCdlTkApiParam();
	/**
    * constructor
    * @param aType the parameter type
	* @param aName the parameter name
    */
	CCdlTkApiParam(const std::string& aType, const std::string& aName);
	/**
    * constructor
    * @param aType the parameter type
	* @param aName the parameter name
	* @param aDefaultValue the default value - If the supplied value is empty, there will be no default value
    */
	CCdlTkApiParam(const std::string& aType, const std::string& aName, const std::string& aDefaultValue);

	/**
    * Get the parameter type
    * @return the parameter type
    */
	const std::string& Type() const;
	/**
    * Set the parameter type
    * @param aType the type for the parameter
    */
	void SetType(const std::string& aType);

	/**
    * Get the parameter name
    * @return the parameter name
    */
	const std::string& Name() const;
	/**
    * Set the parameter name
    * @param aName the parameter name
    */
	void SetName(const std::string& aName);

	/**
    * Get the default value
    * @return the default value - If there is no default value, the return value will be empty
    */
	const std::string& DefaultValue() const;
	/**
    * Set the default value
    * @param aDefaultValue the default value  - If the supplied value is empty, there will be no default value
    */
	void SetDefaultValue(const std::string& aDefaultValue);

	/**
    * compare two parameters
    * @param aOther the parameter to compare with
    * @return true if name, type, and default value are the same
    */
	bool operator==(const CCdlTkApiParam& aOther) const;

	/**
    * compare two parameters
    * @param aOther the parameter to compare with
    * @return true if any of name, type, and default value are not the same
    */
	bool operator!=(const CCdlTkApiParam& aOther) const
		{
		return !(*this == aOther);
		}

private:
	std::string iType;
	std::string iName;
	std::string iDefaultValue;
	};


/**
* The parameter list for a function API in a CDL interface
*/
class CCdlTkApiParams : public std::vector<CCdlTkApiParam>
	{
public:
	/**
    * Finds a parameter by its name
    * @param aParamName the name of the parameter to look for
    * @return an iterator pointing at the parameter, or end() if not found.
    */
	iterator FindByName(std::string aParamName);
	};


/**
* A CDL data type translation.
* Data type translations have three parts.
* 1) the type that the translation matches.
* 2) the syntax for defining an instance of that type.
* 3) the pointer reference syntax for that type.
* The type match string may contain the text "aType". This will match against
* any content in a type to be matched.
* The definition and pointer reference strings may contain the variables
* "aName" and "aType". "aName" will be replaced by the instance name.
* "aType" will be replaced by the text in the type string that matched "aType".
*/
class CCdlTkDataTypeTranslation
	{
public:
	/**
    * An enum used to identify the source of a data type translation, whether
	* it is built into the CDL compiler toolkit, or it comes from a CDL file.
    */
	enum TTranslationSource
		{
		EInbuilt,
		EFromCdl
		};

public:
	/**
    * constructor
    */
	CCdlTkDataTypeTranslation();
	/**
    * constructor
    * @param aType the type that this translation matches
    */
	CCdlTkDataTypeTranslation(const std::string& aType);
	/**
    * constructor
    * @param aType the type that this translation matches
	* @param aDefn the definition syntax for instances of this type.
	* @param aPtrRef the pointer reference syntax for instances of this type.
	* @param aSource the source of this translation.
    */
	CCdlTkDataTypeTranslation(const std::string& aType, const std::string& aDefn, const std::string& aPtrRef, TTranslationSource aSource);

	/**
    * Get the type match string
    * @return the type match string
    */
	std::string Type() const;
	/**
    * Set the type match string
    * @param aType the type match string
    */
	void SetType(const std::string& aType);
	/**
    * Get the definition syntax string
    * @return the definition syntax string
    */
	std::string Definition() const;
	/**
    * Set the definition syntax string
    * @param aDefn the definition syntax string
    */
	void SetDefinition(const std::string& aDefn);
	/**
    * Get the pointer reference syntax
    * @return the pointer reference syntax
    */
	std::string PointerReference() const;
	/**
    * Set the pointer reference syntax
    * @param aPtrRef the pointer reference syntax
    */
	void SetPointerReference(const std::string& aPtrRef);
	/**
    * Get the source of the translation
    * @return the source of the translation
    */
	TTranslationSource Source() const;

	/**
    * Attempts to match a type string against the type string in this translation.
	* Where this translation doesn't use "aType" in its type string, this is
	* a simple comparison between they type strings.
	* Where this translation does use "aType" in its type string, that will match
	* any text in aType, but the rest of the type strings must match exactly.
	* The part of aType that matches the string "aType" will be returned in
	* aTypeVar.
	* For example, consider when this translation's type is "vector<aType>".
	* It will match a type "vector<int>" and "int" will be placed into aTypeVar.
	* However it will not match "list<int>", because "list<>" does not match "vector<>".
    * @param aType the type string to match.
	* @param aTypeVar the text in aType that matched the type variable "aType".
    * @return true if the match succeded.
    */
	bool Match(const std::string& aType, std::string& aTypeVar) const;

	/**
    * Compare two type translations.
    * @param aOther a translation to compare
    * @return true if type, definition syntax, pointer reference syntax and
	* source are all the same.
    */
	bool operator==(const CCdlTkDataTypeTranslation& aOther) const;

private:
	std::string iType;
	std::string iDefn;
	std::string iPtrRef;
	TTranslationSource iSource;
	// member data used in Match() calculation
	std::string iTextBeforeTypeVar;
	std::string iTextAfterTypeVar;
	std::string::size_type iTypeVarPos;
	int iSizeAfterTypeVar;
	int iTypeSize;
	int iTypeSizeWithoutTypeVar;
	};


/**
* A collection of data type translations
*/
class CCdlTkDataTypeTranslations : public std::vector<CCdlTkDataTypeTranslation>
	{
public:
	/**
    * constructor
	* Adds all the inbuilt translations.
    */
	CCdlTkDataTypeTranslations();
	/**
    * Merges adds another collection of translations to the end of this one.
	* Inbuilt translations are ignored, as they will already be in this collection.
    * @param aOther another collection of translations.
    */
	void MergeExtensionTranslations(const CCdlTkDataTypeTranslations& aOther);
	/**
    * Finds the last translation in the collection that matches the type.
	* Note, translations are searched from last to first, so that more recently
	* added translations can overrideolder ones.
    * @param aType the type string to match.
	* @param aTypeVar the text in aType that matched the type variable "aType".
    * @return a pointer to the translation for which the match succeded, or 
	* NULL if no match succeded.
    */
	const CCdlTkDataTypeTranslation* Find(const std::string& aType, std::string& aTypeVar) const;
	};


/**
* The base class for an individual API in a CDL interface
*/
class CCdlTkApi
	{
public:
	/**
    * constructor
    * @param aInterface the interface that this API belongs to
    */
	CCdlTkApi(CCdlTkInterface& aInterface);
	/**
    * destructor
    */
	virtual ~CCdlTkApi();

	/**
    * Create a new copy of this API
    * @param aInterface the interface to which the new copy will belong
    * @return a new copy of this API
    */
	virtual CCdlTkApi* Clone(CCdlTkInterface& aInterface) const = 0;

	/**
    * Get the return type for this API
    * @return the return type
    */
	const std::string& ReturnType() const;
	/**
    * Set the return type for this API
    * @param aType the return type
    */
	void SetReturnType(const std::string& aType);
	/**
    * Does this API return void?
    * @return true if this API returns void.
    */
	bool IsVoidReturn() const;

	/**
    * Get the name of the API
    * @return the name of the API
    */
	const std::string& Name() const;
	/**
    * Set the name of the API
    * @param aName the name of the API
    */
	void SetName(const std::string& aName);

	/**
    * Get the line number of the source file where the API appeared
    * @return the line number of the source file where the API appeared
    */
	int SourceFileLineNum() const;
	/**
    * Set the line number of the source file where the API appeared
    * @param aLineNum the line number of the source file where the API appeared
    */
	void SetSourceFileLineNum(int aLineNum);

	/**
    * Get the comment text for the API
    * @return the comment text for the API
    */
	const std::string& Comment() const;
	/**
    * Set the comment text for the API
    * @param aComment the comment text for the API
    */
	void SetComment(const std::string& aComment);

	/**
    * Get the interface to which this API belongs
    * @return the interface to which this API belongs
    */
	const CCdlTkInterface& Interface() const;
	/**
    * Get the interface to which this API belongs
    * @return the interface to which this API belongs
    */
	CCdlTkInterface& Interface();

	/**
    * Return whether this API is a function API
    * @return true if it is a function API, false if it is a data API
    */
	virtual bool IsFunc() const = 0;
	/**
    * Downcast this object to a function API
    * @return this object as a function API
    */
	const CCdlTkFunctionApi& AsFunc() const;
	/**
    * Downcast this object to a data API
    * @return this object as a data API
    */
	const CCdlTkDataApi& AsData() const;

	/**
    * Get the type of a pointer to this API
    * @return the type of a pointer to this API
    */
	virtual std::string PointerType() const = 0;
	/**
    * Get the type and name list for the paramters to this API, will be
	* empty for data APIs.
    * @return the type and name list for the paramters to this API
    */
	virtual std::string ParamsTypeAndNameList() const = 0;

	/**
    * Test for inequality
    * @param aOther an API to compare
    * @return true if not equal
    */
	bool operator!=(const CCdlTkApi& aOther) const;
	/**
    * Test for equality
    * @param aOther an API to compare
    * @return true if equal
    */
	virtual bool operator==(const CCdlTkApi& aOther) const;
	/**
    * Assign the contents of another API to this one
    * @param a 
    * @return 
    */
	void operator=(const CCdlTkApi& aOther);

private:
	std::string iReturnType;
	std::string iName;
	int iSourceFileLineNum;
	CCdlTkInterface& iInterface;
	std::string iComment;
	};


/**
* An individual data API belonging to a CDL interface
*/
class CCdlTkDataApi : public CCdlTkApi
	{
public:
	/**
    * constructor
    * @param aInterface the interface to which this API belongs
    */
	CCdlTkDataApi(CCdlTkInterface& aInterface);
	/**
    * constructor
    * @param aInterface the interface to which this API belongs
	* @param aCopy an API to copy
    */
	CCdlTkDataApi(CCdlTkInterface& aInterface, const CCdlTkDataApi& aCopy);

	// from CCdlTkApi
	CCdlTkApi* Clone(CCdlTkInterface& aInterface) const;
	bool IsFunc() const;
	std::string PointerType() const;
	std::string ParamsTypeAndNameList() const;
	};


/**
* An individual function API belonging to a CDL interface
*/
class CCdlTkFunctionApi : public CCdlTkApi
	{
public:
	/**
    * constructor
    * @param aInterface the interface to which this API belongs
    */
	CCdlTkFunctionApi(CCdlTkInterface& aInterface);
	/**
    * constructor
    * @param aInterface the interface to which this API belongs
	* @param aCopy an API to copy
    */
	CCdlTkFunctionApi(CCdlTkInterface& aInterface, const CCdlTkFunctionApi& aCopy);

	/**
    * Get the parameters list for this function
    * @return  the parameters list for this function
    */
	CCdlTkApiParams& Params();
	/**
    * Get the parameters list for this function
    * @return  the parameters list for this function
    */
	const CCdlTkApiParams& Params() const;
	/**
    * Get the list of parameter names as a comma separated string
    * @return the list of parameter names as a comma separated string
    */
	std::string ParamNameList() const;
	/**
    * Get the list of parameter types as a comma separated string
    * @return the list of parameter types as a comma separated string
    */
	std::string ParamTypeList() const;
	/**
    * Get a type name for this API.
    * @return a type name for this API.
    */
	std::string ApiNameAsTypeName() const;

	// from CCdlTkApi
	CCdlTkApi* Clone(CCdlTkInterface& aInterface) const;
	bool IsFunc() const;
	std::string PointerType() const;
	std::string ParamsTypeAndNameList() const;

	bool operator==(const CCdlTkApi& aOther) const;

private:
	CCdlTkApiParams iParams;
	};


/**
* A set of APIs belonging to a CDL interface
*/
class CCdlTkApiList : public std::vector<CCdlTkApi*>
	{
public:
	/**
    * constructor
    */
	CCdlTkApiList();
	/**
    * destructor
    */
	~CCdlTkApiList();
	/**
    * Appends API from a CDL interface extension to this API to form a monolithic API
    * @param aOther The API to add
    */
	void MergeExtendedApi(CCdlTkInterface& aInterface, const CCdlTkApiList& aOther);
	/**
    * Copies APIs from another API list to this one. Existing APIs are removed first.
	* @param aOther The API list to copy
    * @param aInterface The CDL interface to which the new APIs will belong
    */
	void Copy(const CCdlTkApiList& aOther, CCdlTkInterface& aInterface);

	/**
    * Finds an API by name
    * @param aName the name of the API to find
    * @return a pointer to the API with that name, or NULL if not found
    */
	CCdlTkApi* Find(const std::string& aName) const;
	/**
    * Compare two API lists
    * @param aOther the API list to compare
    * @return true if the API lists are the same, ie the same content in the same order
    */
	bool operator==(const CCdlTkApiList& aOther) const;
	/**
    * Is this API list a subset of another
    * @param aOther the candiate superset
    * @return true if this API list does not contain any APIs that do not appear in the other APL list
    */
	bool IsSubsetOf(const CCdlTkApiList& aOther) const;

private:
	CCdlTkApiList(const CCdlTkApiList& aCopy);
	void operator=(const CCdlTkApiList& aCopy);
	void DeleteApis();
	};


/**
* The C++ section of a CDL file
*/
class CCdlTkCpp : public std::vector<std::string>
	{
public:
	/**
    * constructor
    */
	CCdlTkCpp();
	/**
    * Adds the contents of another C++ section to the end of this one.
	* @param aOther the other C++ section
    */
	void MergeExtensionCpp(const CCdlTkCpp& aOther);
	};


/**
* a CDL interface
*/
class CCdlTkInterface
	{
public:
	/**
    * constructor
    */
	CCdlTkInterface();
	/**
    * copy constructor
    * @param aCopy the CDL interface to copy
    */
	CCdlTkInterface(const CCdlTkInterface& aCopy);
	/**
    * assignment operator
    * @param aCopy the CDL interface to copy
    */
	void operator=(const CCdlTkInterface& aCopy);
	/**
    * destructor
    */
	~CCdlTkInterface();

	/**
    * Colapse any extended interface into this one to form a monolithic interface
    */
	void MergeExtensions();

	/**
    * Get the CDL interface which this interface extends
    * @return the CDL interface which this interface extends, or null if this is the base
    */
	CCdlTkInterface* Base() const;
	/**
    * Set the CDL interface which this interface extends
    * @param aBase the CDL interface which this interface extends
    */
	void SetBase(CCdlTkInterface* aBase);				// does not take ownership
	/**
    * Get the ultimate base CDL interface which this interface extends. That is, the base
	* interface which itself has no base.
    * @return the ultimate base CDL interface which this interface extends, which may be this object itself
    */
	CCdlTkInterface* UltimateBase();
	/**
    * Get the ultimate base CDL interface which this interface extends. That is, the base
	* interface which itself has no base.
    * @return the ultimate base CDL interface which this interface extends, which may be this object itself
    */
	const CCdlTkInterface* UltimateBase() const;

	/**
    * Gets the immediate extension for this interface.
    * @return the immediate extension for this interface, or null if it's not extended.
    */
	CCdlTkInterface* Extension() const;
	/**
    * Sets the immediate extension for this interface. This function takes ownership of the extension.
    * @param aExtension the CDL interface to be the extension for this interface.
    */
	void SetExtension(CCdlTkInterface* aExtension);		// takes ownership
	/**
    * Get the tip extension for this interface. Extensions to an interface form a linked
	* list. This function traverses to the end of the extension list and returns it.
    * @return the latest extension for this interface, which may be this interface itself if it's not extended
    */
	CCdlTkInterface* UltimateExtension();
	/**
    * Get the tip extension for this interface. Extensions to an interface form a linked
	* list. This function traverses to the end of the extension list and returns it.
    * @return the latest extension for this interface, which may be this interface itself if it's not extended
    */
	const CCdlTkInterface* UltimateExtension() const;

	/**
    * Get the filename for the CDL file for this CDL interface.
    * @return the filename for this interface's CDL file.
    */
	std::string FileName() const;
	/**
    * Set the filename for the CDL file for this CDL interface
    * @param aFileName The file name for this interface's CDL file
    */
	void SetFileName(const std::string& aFileName);

	/**
    * Get the additional comment for this CDL interface
    * @return the additional comment for this interface's CDL file.
    */
	std::string AdditionalComment() const;
	/**
    * Set the additional comment for this CDL interface
	* The caller must provide sufficient annotation for the text to be treated as a 
	* comment by the compiler, ensuring that the comment is terminated correctly
    * @param aAdditionalComment The additional commentfor this interface's CDL file
    */
	void SetAdditionalComment(const std::string& aAdditionalComment);

	/**
    * Get the header for this interface
    * @return the header for this interface
    */
	CCdlTkInterfaceHeader& Header();
	/**
    * Get the header for this interface
    * @return the header for this interface
    */
	const CCdlTkInterfaceHeader& Header() const;
	/**
    * Get the C++ section for this CDL interface
    * @return the C++ section for this CDL interface
    */
	CCdlTkCpp& Cpp();
	/**
    * Get the C++ section for this CDL interface
    * @return the C++ section for this CDL interface
    */
	const CCdlTkCpp& Cpp() const;
	/**
    * Get the API list for this CDL interface
    * @return the API list for this CDL interface
    */
	CCdlTkApiList& ApiList();
	/**
    * Get the API list for this CDL interface
    * @return the API list for this CDL interface
    */
	const CCdlTkApiList& ApiList() const;
	/**
    * Get the data type translations for this CDL interface
    * @return the data type translations for this CDL interface
    */
	CCdlTkDataTypeTranslations& DataTypeTranslations();
	/**
    * Get the data type translations for this CDL interface
    * @return the data type translations for this CDL interface
    */
	const CCdlTkDataTypeTranslations& DataTypeTranslations() const;

	/**
    * Get the C++ namespace name for this CDL interface. This is generated from the CDL interface name.
    * @return the C++ namespace name for this CDL interface.
    */
	std::string NamespaceName() const;

	/**
    * Compare this interface with another
    * @param aOther The interface to compare
    * @return true if the interfaces are equal
    */
	bool operator==(const CCdlTkInterface& aOther) const;

private:
	std::string iFileName;
	std::string iAdditionalComment;
	CCdlTkInterfaceHeader iHeader;
	CCdlTkCpp iCpp;
	CCdlTkApiList iApiList;
	CCdlTkDataTypeTranslations iDataTypeTranslations;
	CCdlTkInterface* iBase;			// not owned
	CCdlTkInterface* iExtension;	// owned
	};


/**
* A collection of CDL interfaces
*/
class CCdlTkInterfaceList : public std::vector<CCdlTkInterface*>
	{
public:
	/**
    * constructor
    */
	CCdlTkInterfaceList();
	/**
    * destructor - deletes the interfaces
    */
	~CCdlTkInterfaceList();

private:
	CCdlTkInterfaceList(const CCdlTkInterfaceList& aCopy);
	void operator=(const CCdlTkInterfaceList& aCopy);
	};


}	// end of namespace CdlCompilerToolkit

#endif
