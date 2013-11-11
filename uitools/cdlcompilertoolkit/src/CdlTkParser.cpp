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
#include "CdlCompilerToolkit/CdlTkProcess.h"
#include "CdlTkPriv.h"
#include <iomanip>
#include <sstream>
#include <iostream>
using namespace std;

namespace CdlCompilerToolkit
 {

//
// SyntaxErr
//

class SyntaxErr : public CdlCompilerToolkitErr
	{
public:
	SyntaxErr(const string& aFileName, int aLineNo, string aErr);
	void Show(ostream& aStream) const;
private:
	string iFileName;
	int iLineNo;
	string iErr;
	};

SyntaxErr::SyntaxErr(const string& aFileName, int aLineNo, string aErr)
: iFileName(aFileName), iLineNo(aLineNo), iErr(aErr)
	{
	}

void SyntaxErr::Show(ostream& aStream) const
	{
	aStream << iFileName << "(" << iLineNo << ")" << " : error : " << iErr << endl;
	}


//
// CCdlTkCdlFileParser
//

CCdlTkCdlFileParser::CCdlTkCdlFileParser(const string& aFileName) 
: iFileName(aFileName)
	{
	}

CCdlTkCdlFileParser::~CCdlTkCdlFileParser()
	{
	CloseStream();
	}

void CCdlTkCdlFileParser::Process()
	{
	throw CdlTkAssert("Sorry, CCdlTkCdlFileParser::Process() does not exist, call LoadAndParse() instead");
	}

auto_ptr<CCdlTkInterface> CCdlTkCdlFileParser::LoadAndParse(bool aMergeExtensions)
	{
	OpenStream();
	iState = EHeader;
	auto_ptr<CCdlTkInterface> cdl(new CCdlTkInterface);
	cdl->SetFileName(iFileName);
	ParseStream(*cdl.get());
	CloseStream();
	if (aMergeExtensions)
		cdl->MergeExtensions();
	return cdl;
	}

void CCdlTkCdlFileParser::OpenStream()
	{
	CdlTkUtil::OpenInput(iIn, iFileName);
	iCurrentSourceLineNum = 0;
	}

void CCdlTkCdlFileParser::ParseStream(CCdlTkInterface& aCdl)
	{
	CCdlTkInterface* cdl = &aCdl;
	while (!iIn.eof())
		{
		string line = GetLine();
		TParseState newState = iState;
		if (IsSectionBoundary(line, newState))
			{
			iState = newState;
			if (iState == EHeader && !(cdl->Header() == CCdlTkInterfaceHeader()))
				{
				CCdlTkInterface* ext = new CCdlTkInterface;
				ext->SetBase(cdl);
				cdl->SetExtension(ext);
				cdl = ext;
				}
			// iApiBuf and iComment have to be cleared over a section boundary
			iApiBuf = "";
			iComment = "";
			}
		else
			{
			switch (iState)
				{
				case EHeader: 
					ParseHeader(*cdl, line); 
					break;
				case ECpp: 
					ParseCpp(*cdl, line); 
					break;
				case ETranslation: 
					ParseTranslationLine(*cdl, line); 
					break;
				case EApi: 
					ParseApi(*cdl, line); 
					break;
				default: 
					return;
				}
			}
		}
	}

void CCdlTkCdlFileParser::CloseStream()
	{
	iIn.close();
	}

void CCdlTkCdlFileParser::ParseHeader(CCdlTkInterface& aCdl, const string& aLine)
	{
	string val;
	if (MatchLineStart(aLine, "Name:", val))
		{
		aCdl.Header().SetName(val);
		}
	else if (MatchLineStart(aLine, "Version:", val))
		{
		int major, minor;
		char c;
		stringstream s(val);
		s >> major >> c >> minor;
		if (c != '.' || !s.eof())
			SyntaxError(string("Invalid version number ") + val);
		aCdl.Header().SetVersion(CCdlTkInterfaceHeader::CVersion(major, minor));
		}
	else  if (MatchLineStart(aLine, "UID:", val))
		{
		aCdl.Header().SetUid(CdlTkUtil::ParseInt(val));
		}
	else if (MatchLineStart(aLine, "Flag:", val))
		{
		aCdl.Header().Flags().SetFlag(val);
		}
	}

void CCdlTkCdlFileParser::ParseCpp(CCdlTkInterface& aCdl, const string& aLine)
	{
	aCdl.Cpp().push_back(aLine);
	}

void CCdlTkCdlFileParser::ParseTranslationLine(CCdlTkInterface& aCdl, const string& aLine)
	{
	string line = aLine;
	StripComments(line, iComment);
	CdlTkUtil::StripLeadingAndTrailingWhitespace(line);
	if (!line.empty())
		{
		CCdlTkDataTypeTranslation trans;
		ParseTranslationText(trans, line);
		aCdl.DataTypeTranslations().push_back(trans);
		}
	}

void CCdlTkCdlFileParser::ParseApi(CCdlTkInterface& aCdl, const string& aLine)
	{
	string line = aLine;
	StripComments(line, iComment);
	if (!line.empty())
		{
		// add the line to the API buffer
		CdlTkUtil::AppendString(iApiBuf, line);
		string::size_type pos;
		// extract API declarations from the API buffer, separated by semi-colons
		while ((pos = iApiBuf.find_first_of(';')) != string::npos)
			{
			// extract API declaration from API buf and create API from it
			line = iApiBuf.substr(0, pos);
			CdlTkUtil::StripLeadingAndTrailingWhitespace(line);
			auto_ptr<CCdlTkApi> api(CreateApi(aCdl, line));
			aCdl.ApiList().push_back(api.get());
			api.release();
			// remove API declaration from API buf
			iApiBuf = iApiBuf.substr(pos+1);
			}
		}
	}

string CCdlTkCdlFileParser::GetLine()
	{
	string line;
	getline(iIn, line);
	iCurrentSourceLineNum++;
	return line;
	}

bool CCdlTkCdlFileParser::IsSectionBoundary(const string& aLine, TParseState& aState)
	{
	string section;
	bool isBoundary = MatchLineStart(aLine, KSectionBoundary, section);
	if (!isBoundary)
		return false;

	section = CdlTkUtil::ToLower(section);

	char* sections[] = {"header", "c++", "translation", "api", "end"};
	TParseState s;
	for (s = EHeader; s < EParseStateCount; s = TParseState(s+1))
		if (section == sections[s])
			break;
	if (s < EParseStateCount)
		aState = s;
	else 
		SyntaxError("Unrecognised section type : " + section);

	return true;
	}

bool CCdlTkCdlFileParser::MatchLineStart(const string& aLine, const string& aHeader, string& aVal)
	{
	if (aLine.size() < aHeader.size() || aLine.substr(0, aHeader.size()) != aHeader)
		return false;
	aVal = aLine.substr(aHeader.size());
	StripComments(aVal, iComment);
	CdlTkUtil::StripLeadingAndTrailingWhitespace(aVal);
	return true;
	}

void CCdlTkCdlFileParser::StripComments(string& aStr, string& aComment)
	{
	string::size_type pos = aStr.find(KCommentStart);
	if (pos != string::npos)
		{
		aComment += aStr.substr(pos) + "\n";
		}
	aStr = aStr.substr(0, pos);
	}

auto_ptr<CCdlTkApi> CCdlTkCdlFileParser::CreateApi(CCdlTkInterface& aCdl, string& aLine)
	{
	auto_ptr<CCdlTkApi> pApi;
	bool isFunc = (aLine[aLine.size()-1] == ')');	// function APIs end with ')', data APIs don't
	if (isFunc)
		{
		auto_ptr<CCdlTkFunctionApi> pFuncApi(new CCdlTkFunctionApi(aCdl));
		string::size_type paramStart = aLine.find('(');
		if (paramStart == string::npos)
			SyntaxError("function has missing '('");
		string params = aLine.substr(paramStart);
		aLine = aLine.substr(0, paramStart);
		params = params.substr(1, params.size()-2);		// -2 for opening and closing brackets
		CdlTkUtil::StripLeadingAndTrailingWhitespace(params);
		ParseApiParams(pFuncApi->Params(), params);
		pApi = auto_ptr<CCdlTkApi>(pFuncApi.release());
		}
	else
		{
		pApi = auto_ptr<CCdlTkApi>(new CCdlTkDataApi(aCdl));
		}
	string name, type, defaultValue;
	ParseNameTypeAndDefaultValue(aLine, name, type, defaultValue);
	pApi->SetName(name);
	pApi->SetReturnType(type);
	pApi->SetSourceFileLineNum(iCurrentSourceLineNum);
	pApi->SetComment(iComment);
	iComment = "";
	return pApi;
	}

void CCdlTkCdlFileParser::ParseApiParams(CCdlTkApiParams& aParams, string& aList)
	{
	while (aList.size())
		{
		string::size_type pos = aList.find(',');
		string param = aList.substr(0, pos);
		aList = aList.substr(param.size() + (pos == string::npos ? 0 : 1));
		CdlTkUtil::StripLeadingAndTrailingWhitespace(aList);

		string name, type, defaultValue;
		ParseNameTypeAndDefaultValue(param, name, type, defaultValue);

		aParams.push_back(CCdlTkApiParam(type, name, defaultValue));
		}
	}

void CCdlTkCdlFileParser::ParseNameTypeAndDefaultValue(string& aStr, string& aName, string& aType, string& aDefaultValue)
	{
	CdlTkUtil::StripLeadingAndTrailingWhitespace(aStr);
	string::size_type eq = aStr.find_last_of(KEqualsSign);
	if(eq != string::npos)
		{
		aDefaultValue = aStr.substr(eq + 1);
		CdlTkUtil::StripLeadingAndTrailingWhitespace(aDefaultValue);

		aStr = aStr.substr(0, eq);
		CdlTkUtil::StripLeadingAndTrailingWhitespace(aStr);
		}

	int pos = aStr.find_last_not_of(KCpp);
	aName = aStr.substr(pos + 1);
	aStr = aStr.substr(0, pos);

	CdlTkUtil::StripLeadingAndTrailingWhitespace(aStr);
	aType = aStr;
	}

void CCdlTkCdlFileParser::ParseTranslationText(CCdlTkDataTypeTranslation& aTrans, string& aLine)
	{
	string::size_type pos1 = aLine.find('#');
	if (pos1 == string::npos)
		SyntaxError("First # not found");
	string::size_type pos2 = aLine.find('#', pos1+1);
	if (pos2 == string::npos)
		SyntaxError("Second # not found");
	if (aLine.find('#', pos2+1) != string::npos)
		SyntaxError("Third # found");

	string type = aLine.substr(0, pos1++);
	CdlTkUtil::StripLeadingAndTrailingWhitespace(type);

	string defn = aLine.substr(pos1, pos2-pos1);
	CdlTkUtil::StripLeadingAndTrailingWhitespace(defn);
	if (defn.find("aName") == string::npos)
		SyntaxError("\"aName\" not found in definition");

	string ptrRef = aLine.substr(pos2+1);
	CdlTkUtil::StripLeadingAndTrailingWhitespace(ptrRef);
	if (ptrRef.find("aName") == string::npos)
		SyntaxError("\"aName\" not found in pointer reference");

	aTrans.SetType(type);
	aTrans.SetDefinition(defn);
	aTrans.SetPointerReference(ptrRef);
	}

void CCdlTkCdlFileParser::SyntaxError(const string& aErr)
	{
	throw SyntaxErr(iFileName, iCurrentSourceLineNum, aErr);
	}


}	// end of namespace CdlCompilerToolkit
