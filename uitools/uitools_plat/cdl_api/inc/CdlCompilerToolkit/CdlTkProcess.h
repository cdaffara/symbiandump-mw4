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
#ifndef CDLTKPROCESS_H
#define CDLTKPROCESS_H

#include <CdlCompilerToolkit/CdlTkInterface.h>
#include <CdlCompilerToolkit/CdlTkInstance.h>
#include <fstream>
#include <memory>

namespace CdlCompilerToolkit {
class CCdlTkApi;
class CCdlTkInterface;

/**
* CCdlTkProcess
* Does very little yet.
*/
class CCdlTkProcess
	{
public:
	/**
    * destructor
    */
	virtual ~CCdlTkProcess();
	/**
    * virtual method to invoke processing
    */
	virtual void Process() = 0;

public:
	static std::string CdlBaseNameAndPath(const CCdlTkInterface& aCdl);
	static void AssertInterfaceNotExtended(const CCdlTkInterface& aCdl);
	};


/**
* Base class for processes that write source files
*/
class CCdlTkSourceFileWriter : public CCdlTkProcess
	{
public:
	/**
    * destructor
    */
	~CCdlTkSourceFileWriter();

protected:
	/**
    * Writes an opening namespace declaration for a CDL interface to a stream
    * @param aCdl the CDL interface defining the namespace
	* @param aStream the stream to write to
    */
	void WriteNamespaceStart(const CCdlTkInterface& aCdl, std::ofstream& aStream) const;
	/**
    * Writes a closing namespace declaration for a CDL interface to a stream
    * @param aCdl the CDL interface defining the namespace
	* @param aStream the stream to write to
    */
	void WriteNamespaceEnd(const CCdlTkInterface& aCdl, std::ofstream& aStream) const;
	/**
    * Calculates a header guard identifier for a file
    * @param aFileName the file to be protected by the header guard
    * @return the header guard identifer
    */
	std::string HeaderGuardName(const std::string& aFileName) const;
	/**
    * Writes the starting declarations for a header file guard
    * @param aName the file to be protected by the header guard
	* @param aStream the stream to write to
    * @return 
    */
	void WriteHeaderGuardStart(const std::string& aName, std::ofstream& aStream) const;
	/**
    * Writes the ending declaration for a header file guard
    * @param aName the file to be protected by the header guard
	* @param aStream the stream to write to
    * @return 
    */
	void WriteHeaderGuardEnd(const std::string& aName, std::ofstream& aStream) const;
	};


/**
* Process for writing a common definitions header file (.cdl.common.h)
*/
class CCdlTkWriteCommonDefs : public CCdlTkSourceFileWriter
	{
public:
	static void ExportCommonDefs(const CCdlTkInterface& aCdl, const std::string& aFileName);

private:
	/**
    * constructor
    * @param aCdl the interface to write. 
	* @param aStream the stream to write to
    */
	CCdlTkWriteCommonDefs(const CCdlTkInterface& aCdl, std::ofstream& aStream, const std::string& aFileName);
	/**
    * Creates and writes a common definitions header file (.cdl.common.h)
    */
	void Process();
	void ProcessHRH(std::ofstream& aStream);

private:
	void WriteApiEnum();
	void WriteApiTypedefs();
	std::string TypedefForApi(const CCdlTkApi& aApi) const;
	std::ofstream& Stream();

private:
	const CCdlTkInterface& iCdl;
	std::ofstream* iStream;
	const std::string& iFileName;
	};


/**
* Process for writing a C++ client header file for a C++ interface
*/
class CCdlTkWriteClientHeader : public CCdlTkSourceFileWriter
	{
public:
	/**
    * constructor
    * @param aCdl The interface to write
    */
	CCdlTkWriteClientHeader(const CCdlTkInterface& aCdl);
	/**
    * destructor
    */
	~CCdlTkWriteClientHeader();
	/**
    * Creates and writes the header files required by a C++ client to use
	* a CDL interface.
    */
	void Process();

protected:
	std::string ClientReturnType(const CCdlTkApi& aApi) const;

private:
	void ProcessApi(std::ofstream& aStream, const std::string& aFileName) const;
	void WriteApi(const CCdlTkApi& aApi, std::ofstream& aStream, bool aCInstanceMember, const std::string& aIndent) const;

protected:
	const CCdlTkInterface& iCdl;
	};


/**
* Process for syntax checking a CDL interface
*/
class CCdlTkSyntaxCheck : public CCdlTkWriteClientHeader
	{
	/**
    * constructor
    * @param aCdl the interface to check
    */
	CCdlTkSyntaxCheck(const CCdlTkInterface& aCdl);
	/**
    * destructor
    */
	~CCdlTkSyntaxCheck();
	/**
    * Sets parameters to pass to the C++ compiler which will do the syntax check
    * @param aParams the parameters for the compiler
    */
	void SetParams(std::string aParams);
	/**
    * Runs the syntax checking process
    */
	void Process();

private: // syntax checking
	void WriteSyntaxCheckCpp(std::string aName) const;
	void DoSyntaxCheckBuild(std::string aName) const;
	void WriteSyntaxCheckApi(const CCdlTkApi& aApi, std::ofstream& aStream) const;

private:
	std::string iParams;
	};


/**
* A process for writing a customisation instance to C++ source files
*/
class CCdlTkWriteInstance : public CCdlTkSourceFileWriter
	{
public:
	/**
    * constructor
    * @param aInstance The instance to write
    */
	CCdlTkWriteInstance(const CCdlTkInstance& aInstance);
	/**
    * destructor
    */
	~CCdlTkWriteInstance();
	/**
    * Run the process of creating and writing the C++ source files for a
	* customisation instance.
    */
	void Process();

private:
	void ProcessCdl() const;
	void ProcessInstanceApi(std::ofstream& aStream, const std::string& aFileName) const;
	void ProcessInstance(std::ofstream& aStream, const std::string& aHeaderName) const;
	void ProcessInstanceHeader(std::ofstream& aStream, const std::string& aFileName) const;
	void ProcessInstanceHrh(std::ofstream& aStream, const std::string& aFileName) const;
	void InitReplace(const std::string& aHeaderName);

private:
	const CCdlTkInstance& iInstance;
	const CCdlTkInterface& iCdl;
	CdlTkUtil::CReplaceSet iReplace;
	};


/**
* A process to write the source and build files for a DLL
*/
class CCdlTkWriteDll : public CCdlTkSourceFileWriter
	{
public:
	/**
    * constructor
    * @param aDll the DLL to create source and build files for
    */
	CCdlTkWriteDll(const CCdlTkDll& aDll);
	/**
    * destructor
    */
	~CCdlTkWriteDll();
	/**
    * Runs the process of creating and writing source and build files for a DLL
    */
	void Process();

private:
	void WriteBldInf() const;
	void WriteMmp() const;
	void WriteMainCpp() const;
	void WriteInstanceIdHeader() const;
	void WriteEcomRss() const;
	void WriteEcomDetailRss() const;

private:
	const CCdlTkDll& iDll;
	};


/**
* Process for writing a CDL file
*/
class CCdlTkWriteCdlFile : public CCdlTkProcess
	{
public:
	/**
    * constructor
    * @param aCdl the CDL interface to write
    */
	CCdlTkWriteCdlFile(const CCdlTkInterface& aCdl);
	/**
    * destructor
    */
	~CCdlTkWriteCdlFile();
	/**
    * Run the process of writing the CDL interface to a CDL file
    */
	void Process();

private:
	void WriteHeaderComment();
	void WriteHeader();
	void WriteSeparator(const std::string& aSection);
	void WriteCpp();
	void WriteTranslation();
	void WriteApi();

private:
	const CCdlTkInterface* iCdl;
	std::ofstream iOut;
	};


/**
* A process for reading a CDL file
*/
class CCdlTkCdlFileParser : public CCdlTkProcess
	{
public:
	/**
    * constructor
    * @param aFileName the name of the CDL file to be read
    * @return 
    */
	CCdlTkCdlFileParser(const std::string& aFileName);
	/**
    * destructor
    */
	~CCdlTkCdlFileParser();
	/**
    * Loads and parses a CDL file
    * @param aMergeExtensions tells the parser whether to merge and interface
	* extensions to form a monolithic API
    * @return an auto pointer to the CDL interface read from the CDL file
    */
	std::auto_ptr<CCdlTkInterface> LoadAndParse(bool aMergeExtensions);
	/**
    * virtual Process method from CCdlTkProcess
	* this function is not to be used
    */
	void Process();

private:
	enum TParseState {EHeader, ECpp, ETranslation, EApi, EEnd, EParseStateCount};

private:
	void OpenStream();
	void ParseStream(CCdlTkInterface& aCdl);
	void CloseStream();

	void ParseHeader(CCdlTkInterface& aCdl, const std::string& aLine);
	void ParseCpp(CCdlTkInterface& aCdl, const std::string& aLine);
	void ParseTranslationLine(CCdlTkInterface& aCdl, const std::string& aLine);
	void ParseApi(CCdlTkInterface& aCdl, const std::string& aLine);
	std::auto_ptr<CCdlTkApi> CreateApi(CCdlTkInterface& aCdl, std::string& aLine);
	void ParseApiParams(CCdlTkApiParams& aParams, std::string& aList);
	void ParseNameTypeAndDefaultValue(std::string& aStr, std::string& aName, std::string& aType, std::string& aDefaultValue);
	void ParseTranslationText(CCdlTkDataTypeTranslation& aTrans, std::string& aLine);

	std::string GetLine();
	bool IsSectionBoundary(const std::string& aLine, TParseState& aState);
	bool MatchLineStart(const std::string& aLine, const std::string& aHeader, std::string& aVal);
	void StripComments(std::string& aStr, std::string& aComment);

	void SyntaxError(const std::string& aErr);

private:
	std::string iFileName;
	std::ifstream iIn;
	TParseState iState;
	int iCurrentSourceLineNum;
	std::string iApiBuf;			// buffer for collecting API declarations
	std::string iComment;			// buffer for collecting comments
	};


/**
* Observer mixin interface for the interface checking process
*/
class MCdlTkApiCheckObserver
	{
public:
	/**
    * Called when an interface check is started
    */
	virtual void StartCheck() = 0;
	/**
    * Called when an interface check is complete
    */
	virtual void CheckComplete() = 0;
	/**
    * Called when an API is found that appears in both interfaces
    */
	virtual void ApiInBoth(const CCdlTkApi& aApi) = 0;
	/**
    * Called when an API is found that is only in the right hand interface
    */
	virtual void ApiNotInLeft(const CCdlTkApi& aApi) = 0;
	/**
    * Called when an API is found that is only in the left hand interface
    */
	virtual void ApiNotInRight(const CCdlTkApi& aApi) = 0;

	virtual ~MCdlTkApiCheckObserver(){}
	};

/**
* process for checking (diffing) two CDL interfaces' APIs
*/
class CCdlTkApiChecker : public CCdlTkProcess
	{
public:
	/**
    * constructor
    * @param aLeft a CDL interface to check
    * @param aRight another CDL interface to check
    * @param aObserver the observer of the results of the check
    */
	CCdlTkApiChecker(const CCdlTkInterface& aLeft, 
					 const CCdlTkInterface& aRight, 
					 MCdlTkApiCheckObserver& aObserver);
	/**
    * destructor
    */
	~CCdlTkApiChecker();
	/**
    * Run the process of checking the interfaces' APIs
    */
	void Process();

private:
	typedef CCdlTkApiList::const_iterator CApiListIter;
	void FailLeft(CApiListIter& aFrom, const CApiListIter& aTo) const;
	void FailRight(CApiListIter& aFrom, const CApiListIter& aTo) const;
	void ReSync(CApiListIter& posLeft, CApiListIter& posRight, 
				const CApiListIter& leftEnd, const CApiListIter& rightEnd) const;

private:
	const CCdlTkInterface& iLeft;
	const CCdlTkInterface& iRight;
	MCdlTkApiCheckObserver& iObserver;
	};

}	// end of namespace CdlCompilerToolkit

#endif
