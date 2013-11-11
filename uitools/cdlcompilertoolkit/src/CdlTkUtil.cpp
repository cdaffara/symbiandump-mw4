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
#include "CdlTkPriv.h"

#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cassert>

#include <CdlCompilerToolkit/CdlCompat.h>

#ifdef CDL_W32
#include <direct.h>
#else
#include <unistd.h>

#define _getcwd getcwd
#endif

using namespace std;

namespace CdlCompilerToolkit {

//
// CdlCompilerToolkitErr
//

CdlCompilerToolkitErr::~CdlCompilerToolkitErr()
	{
	}


//
// CCdlTkFileCleanup
//

CCdlTkFileCleanup::CCdlTkFileCleanup()
: iName("")
	{
	}

CCdlTkFileCleanup::CCdlTkFileCleanup(const std::string& aName)
: iName(aName)
	{
	}

CCdlTkFileCleanup::~CCdlTkFileCleanup()
	{
	Cleanup();
	}

void CCdlTkFileCleanup::Set(const string& aName)
	{
	iName = aName;
	}

void CCdlTkFileCleanup::Cleanup()
	{
	if (!iName.empty())
		CdlTkUtil::DeleteFile(iName);
	Release();
	}

string CCdlTkFileCleanup::Name() const
	{
	return iName;
	}

void CCdlTkFileCleanup::Release()
	{
	iName.erase();
	}


//
// CdlTkAssert
//

CdlTkAssert::CdlTkAssert(const string& aText)
: iText(aText)
	{
	}

void CdlTkAssert::Show(ostream& stream) const
	{
	stream << "Cdl Compiler Toolkit assertion failed : " << iText << endl;
	}


//
// CdlTkFileOpenErr
//

CdlTkFileOpenErr::CdlTkFileOpenErr(const string& aFileName) : iFileName(aFileName)
	{
	}

void CdlTkFileOpenErr::Show(ostream& stream) const
	{
	stream << endl;
	stream << iFileName << " failed to open" << endl;
	}


//
// CdlTkUtil
//

#ifdef CDL_W32
string CdlTkUtil::CurrentDrive()
	{
	static string drive = "?:";
	if (drive == "?:")
		drive[0] = 'A' + _getdrive() - 1;
	return drive;
	}
#else	// Linux
string CdlTkUtil::CurrentDrive()
	{
	char *epocroot = getenv("EPOCROOT");
	assert(epocroot != NULL);
	
	return std::string(epocroot);
	}
#endif

string CdlTkUtil::CurrentDir()
	{
	static string dir = "";
	if (dir == "")
		{
		char buf[256];
#ifdef CDL_W32
		dir = _getcwd(buf, 255) + 2;	// +2 removes drive
#else
//for linux
                dir = _getcwd(buf, 255);
#endif
		dir += PATHSEP;
		}
	return dir;
	}

static string gOutputPath = "";

string CdlTkUtil::OutputPath()
	{
	if (gOutputPath == "")
		gOutputPath = CurrentDir();
	return gOutputPath;
	}

void CdlTkUtil::SetOutputPath(const string& aPath)
	{
	gOutputPath = aPath;
	if (gOutputPath.size() == 0)
		gOutputPath += PATHSEP;
	else if (! IsPathSeparator( gOutputPath[gOutputPath.size()-1] ) )
		gOutputPath += PATHSEP;	// CDL Tk convention is that paths always end in backslash.
	}

string CdlTkUtil::ToLower(const string& aString)
	{
	string r;
	r.resize(aString.size());
	transform(aString.begin(), aString.end(), r.begin(), tolower);
	return r;
	}

string CdlTkUtil::ToUpper(const string& aString)
	{
	string r;
	r.resize(aString.size());
	transform(aString.begin(), aString.end(), r.begin(), toupper);
	return r;
	}

string CdlTkUtil::ToCpp(const string& aString)
	{
	string r = aString;
	r = Replace("\r", "", r);
	r = Replace("\n", "", r);

	for (string::iterator pC = r.begin(); pC != r.end(); ++pC)
		{
		if (!CdlTkUtil::IsCpp(*pC))
			*pC = '_';
		}
	if (r.empty() || CdlTkUtil::IsNumeric(r[0]))
		r = string("_") + r;
	return r;
	}

string CdlTkUtil::StripPath(const string& aPath)
	{
	return aPath.substr( FindLastPathSeparator(aPath) + 1 );
	}

string CdlTkUtil::ResolvePath(const string& aPath, const string& aFileName)
	{
	int size = aFileName.size();
	// if aFileName is absolute, return it
	if (size > 0 && IsPathSeparator( aFileName[0] ) || size > 1 && aFileName[1] == ':')
		return aFileName;

	string path = aPath;
	string file = aFileName;

	// while file starts with a '.', chop file head directory and path tail directory if necessary
	while (file.size() > 0 && file[0]=='.')
		{
		// if file starts with a "..", chop the tail directory off the path
		if (file.size() > 1 && file[1]=='.' && !path.empty())
			{
			path.resize(path.size()-1);			// remove the last slash
			path.resize( FindLastPathSeparator(path) + 1 );	// remove everything after the next last slash
			}

		// chop the head directory off the file - it has to have a '\' if it has a '.'
		string::size_type fileSlashPos = FindFirstPathSeparator( file );
		if (fileSlashPos == string::npos)
			throw CdlTkAssert("Illegal filename");
		file = file.substr(fileSlashPos + 1);
		}

	// join remaining path and file
	return path + file;
	}

string CdlTkUtil::CapitalizeFilename(const string& aString)
	{
	// convert the whole thing to lower case
	string res = ToLower(aString);
	// find the first character after the last \ - will be 0 if no \ is present.
	string::size_type filenamePos = FindLastPathSeparator(res) + 1;
	if (filenamePos >= res.size())
		throw CdlTkAssert(aString + " has no filename");
	// uppercase the first character
	res[filenamePos] = toupper(res[filenamePos]);
	cerr << "Warning, filename capitalized: " << res << endl;
	return res;
	}

string CdlTkUtil::CorrectFilenameCase(const string& aString)
	{
	// The current standard is to set the filename and path to lower case.
	return ToLower(aString);
	}

string CdlTkUtil::Replace(const string& aTarget, const string& aWith, const string& aIn)
	{
	string ret;
	int pos=0;
	int lastMatch = 0;
	int targetMatch = 0;
	int targetSize = aTarget.size();
	int inSize = aIn.size();
	while (pos < inSize)
		{
		if (aTarget[targetMatch] == aIn[pos++])
			{
			++targetMatch;
			if (targetMatch == targetSize)
				{
				AppendString(ret, aIn.substr(lastMatch, pos - targetMatch - lastMatch));
				AppendString(ret, aWith);
				lastMatch = pos;
				targetMatch = 0;
				}
			}
		else
			{
			targetMatch = 0;
			}
		}
	AppendString(ret, aIn.substr(lastMatch));
	return ret;
	}

void CdlTkUtil::ExportFile(CCdlTkFileCleanup& aSourceFile, const string& aExport, ios_base::openmode aOpenMode)
	{
	if (!FilesAreIdentical(aSourceFile.Name(), aExport, aOpenMode))
		{
		CopyFile(aSourceFile.Name(), aExport, aOpenMode);
		}
	aSourceFile.Cleanup();
	}

void CdlTkUtil::ExportFileIfWritable(CCdlTkFileCleanup& aSourceFile, const string& aExport, ios_base::openmode aOpenMode)
	{
	if (!FilesAreIdentical(aSourceFile.Name(), aExport, aOpenMode))
		{
		try
			{
			CopyFile(aSourceFile.Name(), aExport, aOpenMode);
			}
		catch (const CdlTkFileOpenErr& /*e*/)
			{
			cerr << "Could not write to " << aExport << endl;
			}
		}
	aSourceFile.Cleanup();
	}

void CdlTkUtil::DeleteFile(const std::string& aFileName)
	{
	remove(aFileName.c_str());
	}

void CdlTkUtil::CopyFile(const std::string& aSourceFileName, const std::string& aDestinationFileName, ios_base::openmode aOpenMode)
	{
	ifstream from;
	OpenInput(from, aSourceFileName, aOpenMode);
	ofstream to;
	OpenOutput(to, aDestinationFileName, aOpenMode);

	const int KSize = 1024;
	char buf[KSize+1];
	while (!from.eof())
		{
		from.read(buf,KSize);
		to.write(buf, from.gcount());
		}

	from.close();
	to.close();
	}

bool CdlTkUtil::FilesAreIdentical(const std::string& aLeftFileName, const std::string& aRightFileName, ios_base::openmode aOpenMode)
	{
	bool different = false;
	ifstream leftFile(aLeftFileName.c_str(), aOpenMode);
	ifstream rightFile(aRightFileName.c_str(), aOpenMode);

	const int KSize = 1024;
	char left[KSize+1];
	char right[KSize+1];

	while (!leftFile.eof() && !rightFile.eof())
		{
		int gotLeft = leftFile.read(left,KSize).gcount();
		int gotRight = rightFile.read(right,KSize).gcount();

		if (gotLeft != gotRight || memcmp(left, right, gotLeft) != 0)
			{
			different = true;
			break;
			}
		}

	if (leftFile.eof() != rightFile.eof())
		different = true;

	leftFile.close();
	rightFile.close();

	return !different;
	}

void CdlTkUtil::OpenTempOutput(ofstream& aStream, CCdlTkFileCleanup& aFile, ios_base::openmode aOpenMode)
	{
	char tmpName[256];
#ifdef CDL_W32
	if (!tmpnam(tmpName))
		{
		throw CdlTkAssert("Can't create temporary file name");
		}
#else
    strcpy(tmpName, "cdltkutilXXXXXX");
    
    if (-1 == mkstemp(tmpName))
		{
		throw CdlTkAssert("Can't create temporary file name");
		}
#endif

	OpenOutput(aStream, tmpName, aOpenMode);
	aFile.Set(tmpName);
	}

void CdlTkUtil::OpenOutput(ofstream& aStream, const string& aFileName, ios_base::openmode aOpenMode)
	{
	aStream.open(aFileName.c_str(), aOpenMode);
	if (!aStream.is_open())
		{
		//throw CdlTkFileOpenErr(aFileName);
		}
	}

void CdlTkUtil::OpenInput(ifstream& aStream, const string& aFileName, ios_base::openmode aOpenMode)
	{
	aStream.open(aFileName.c_str(), aOpenMode);
	if (!aStream.is_open())
		throw CdlTkFileOpenErr(aFileName);
	}

int CdlTkUtil::ParseInt(const string& aInt)
	{
	int base = 10;
	if (aInt.size() > 1 && aInt[0] == '0')
		{
		base = 8;
		if (aInt[1] == 'x')
			base = 16;
		}

	int val = 0;
	stringstream s(aInt);
	s >> setbase(base) >> val;
	return val;
	}

string CdlTkUtil::IntToString(int aInt)
	{
	stringstream s;
	s << aInt;
	return s.str();
	}

string CdlTkUtil::IntToHexString(int aInt)
	{
	stringstream s;
	s << "0x" << setw(8) << setfill('0') << hex << aInt;
	return s.str();
	}

char* gHexDigits = "0123456789abcdef";
string CdlTkUtil::ShortToHexString(short aInt)
	{
	char s[7] = "0x0000";
	s[2]=gHexDigits[(aInt>>12)&0xf];
	s[3]=gHexDigits[(aInt>>8)&0xf];
	s[4]=gHexDigits[(aInt>>4)&0xf];
	s[5]=gHexDigits[aInt&0xf];
	return s;
	}

string CdlTkUtil::CharToHexString(char aInt)
	{
	char s[5] = "0x00";
	s[2]=gHexDigits[(aInt>>4)&0xf];
	s[3]=gHexDigits[aInt&0xf];
	return s;
	}

void CdlTkUtil::StripLeadingAndTrailingWhitespace(string& aStr)
	{
	string::size_type pos = aStr.find_first_not_of(KWhiteSpace);
	if (pos == string::npos)
		{
		aStr = KEmptyString;
		}
	else
		{
		aStr = aStr.substr(pos, aStr.find_last_not_of(KWhiteSpace) + 1 - pos);
		}
	}

bool CdlTkUtil::IsAlpha(char aChar)
	{
	return ('A' <= aChar && aChar <= 'Z') || ('a' <= aChar && aChar <= 'z');
	}

bool CdlTkUtil::IsNumeric(char aChar)
	{
	return '0' <= aChar && aChar <= '9';
	}

bool CdlTkUtil::IsCpp(char aChar)
	{
	return IsAlpha(aChar) || IsNumeric(aChar) || aChar == '_';
	}

bool CdlTkUtil::IsPathSeparator(char aChar)
    {
    return aChar == '/' || aChar == '\\';
    }

std::string::size_type CdlTkUtil::FindFirstPathSeparator(const std::string& s)
    {
    std::string::size_type f = s.find(FORWARDSLASH);
    std::string::size_type b = s.find(BACKSLASH);

    if(f == std::string::npos)
        return b;
    
    if(b == std::string::npos)
        return f;
    
    return f < b ? f : b;
    }

std::string::size_type CdlTkUtil::FindLastPathSeparator(const std::string& s)
    {
    std::string::size_type f = s.rfind(FORWARDSLASH);
    std::string::size_type b = s.rfind(BACKSLASH);

    if(f == std::string::npos)
        return b;
    
    if(b == std::string::npos)
        return f;
    
    return f > b ? f : b;
    }
void ZeroInts(int* aInts, int aCount)
	{
	for (int ii=0; ii<aCount; ii++)
		aInts[ii] = 0;
	}

string CdlTkUtil::MultiReplace(const CReplaceSet& aSet, const string& aIn)
	{
	string ret;
	int setCount = aSet.size();
	int* match = new int[setCount];
	if (!match)
		throw bad_alloc();
	ZeroInts(match, setCount);

	int inSize = aIn.size();
	int pos = 0;
	int lastMatch = 0;
	while (pos < inSize)
		{
		char ch = aIn[pos++];
		for (int ii=0; ii<setCount; ii++)
			{
			const string& target = aSet[ii].first;
			int& targetMatch = match[ii];
			if (target[targetMatch] == ch)
				{
				++targetMatch;
				if ( static_cast<string::size_type>( targetMatch ) == target.size())
					{
					AppendString(ret, aIn.substr(lastMatch, pos - targetMatch - lastMatch));
					AppendString(ret, aSet[ii].second);
					lastMatch = pos;
					ZeroInts(match, setCount);
					}
				}
			else
				{
				match[ii] = 0;
				}
			}
		}
	AppendString(ret, aIn.substr(lastMatch));

	delete[] match;
	return ret;
	}

void CdlTkUtil::AppendString(string& aTarget, const string& aAppend)
	{
	string::size_type resSize = aTarget.size() + aAppend.size();
	if (aTarget.capacity() < resSize)
		aTarget.reserve(resSize*2);
	aTarget.append(aAppend);
	}

static string gCommandLine = "";

string CdlTkUtil::CommandLine()
	{
	return gCommandLine;
	}

void CdlTkUtil::SetCommandLine(int argc, char* argv[])
	{
	string tool(argv[0]);

	tool = StripPath( tool );
	gCommandLine = tool.substr(0, tool.find_last_of('.'));
	for (int ii=1; ii<argc; ii++)
		{
		AppendString(gCommandLine, " ");
		AppendString(gCommandLine, argv[ii]);
		}
	}

//
// CdlTkUtil::CReplaceSet
//

void CdlTkUtil::CReplaceSet::Add(const string& aTarget, const string& aWith) 
	{
	push_back(make_pair(aTarget, aWith));
	}

void CdlTkUtil::ReadFile(std::string& aContent, const std::string& aFileName)
	{
	ifstream in;
	OpenInput(in, aFileName);
	aContent.erase();
	const int KChars = 1024 + 1;
	char buf[KChars];
	while (!in.eof())
		{
		in.get(buf,KChars,0);
		AppendString(aContent, buf);
		}
	in.close();
	}

void CdlTkUtil::WriteFile(const std::string& aContent, const std::string& aFileName)
	{
	ofstream out;
	OpenOutput(out, aFileName);
	out << aContent;
	out.close();
	}

}	// end of namespace CdlCompilerToolkit
