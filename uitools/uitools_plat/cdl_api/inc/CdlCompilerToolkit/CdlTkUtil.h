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
#ifndef CDLTKUTIL_H
#define CDLTKUTIL_H
#pragma warning (disable:4786)	// disable "identifier was truncated to '255' characters in the browser information" warning

#include <string>
#include <iosfwd>
#include <vector>
#include <CdlDef.h>

namespace CdlCompilerToolkit {

class CCdlTkFileCleanup;

/**
* This is a collection of utility functions that the CDL compiler toolkit uses
* and which may be of use to other tools based on this toolkit
*/
class CdlTkUtil
	{
private:
	typedef std::pair<std::string, std::string> CReplaceTerm;

public:
	/**
    * Gives the current working drive
    * @return the drive as a "X:" string.
    */
	static std::string CurrentDrive();
	/**
    * Gives the current working directory, without drive
    * @return the current working directory
    */
	static std::string CurrentDir();
	/**
    * Gives the current output path for generated files
    * @return the output path
    */
	static std::string OutputPath();
	/**
    * Sets the curreent output path for generated files
    * @param aPath the output path for generated files
    */
	static void SetOutputPath(const std::string& aPath);

	/**
    * Gives the command line used to run the tool using this toolkit
    * @return the command line, will be empty if SetCommandLine() is not called
    */
	static std::string CommandLine();
	/**
    * Sets the command line for the tool using this toolkit.
    */
	static void SetCommandLine(int argc, char* argv[]);

	/**
    * Extract an int from the string. This function automatically detects
	* bases 8, 10 and 16.
    * @param aInt a string containing an integer
    * @return the integer value parsed
    */
	static int ParseInt(const std::string& aInt);
	/**
    * Formats an integer as a string
    * @param aInt the integer to format
    * @return a string representing the integer in base 10
    */
	static std::string IntToString(int aInt);
	/**
    * Formats an integer as a hex string
    * @param aInt the integer to format
    * @return a string representing the integer in base 16
    */
	static std::string IntToHexString(int aInt);
	/**
    * Formats a short integer as a hex string
    * @param aInt the integer to format
    * @return a string representing the integer in base 16
    */
	static std::string ShortToHexString(short aInt);
	/**
    * Formats an 8-bit integer as a hex string
    * @param aInt the integer to format
    * @return a string representing the integer in base 16
    */
	static std::string CharToHexString(char aInt);

	/**
    * Converts a string to lower case
    * @param aString the string to convert
    * @return a version of the string in lower case
    */
	static std::string ToLower(const std::string& aString);
	/**
    * Converts a string to upper case
    * @param aString the string to convert
    * @return a version of the string in upper case
    */
	static std::string ToUpper(const std::string& aString);
	/**
    * Converts a string to C++ identifier format. Characters which are not
	* valid C++ identifier characters are replaced with an underscore. If the
	* string starts with a number or is empty, an underscore is added to the start.
    * @param aString a string to convert to a C++ identifier
    * @return the converted string
    */
	static std::string ToCpp(const std::string& aString);
	/**
    * Strips white-space from the start and end of the string
    * @param aStr the string from which white space should be removed
    */
	static void StripLeadingAndTrailingWhitespace(std::string& aStr);

	/**
    * Is the character alphabetic?
    * @param aChar the character to test
    * @return true if the character is alphabetic
    */
	static bool IsAlpha(char aChar);
	/**
    * Is the character numeric?
    * @param aChar the character to test
    * @return true if the character is numeric
    */
	static bool IsNumeric(char aChar);
	/**
    * Is the character valid for a C++ identifier?
    * @param aChar the character to test
    * @return true if the character is valid for a C++ identifier
    */
	static bool IsCpp(char aChar);

    /**
     * Is the character equal to '\\' or '/' ? 
     * @param aChar the character to test
     * @return true if the character is a path separator
     */
	static bool IsPathSeparator(char aChar);
	static std::string::size_type FindFirstPathSeparator(const std::string& s);
	static std::string::size_type FindLastPathSeparator(const std::string& s);
	/**
    * This class contains a set of terms to replace in a string. It is used
	* with CdlTkUtil::MultiReplace().
    */
	class CReplaceSet : public std::vector<CReplaceTerm>
		{
	public:
		void Add(const std::string& aTarget, const std::string& aWith);
		};
	/**
    * Replace a set of targets with other text in a string and return the result.
    * @param aSet the set of pairs of targets and replacement text
	* @param aIn the string to replace text in
    * @return The result of the replacement
    */
	static std::string MultiReplace(const CReplaceSet& aSet, const std::string& aIn);
	/**
    * Replace a target in a string with some text and return the result.
    * @param aTarget the text to replace
    * @param aWith the text that replaces aTarget
    * @param aIn the string in which to replace the text
    * @return the result of the replacement
    */
	static std::string Replace(const std::string& aTarget, const std::string& aWith, const std::string& aIn);
	/**
    * Add a string to another. This function uses exponential growth of the target
	* string to reduce average runtime for long sequences of appends to O(n log(n)).
    * @param aTarget the string to which text is to be appended
	* @param aAppend the text to append
   */
	static void AppendString(std::string& aTarget, const std::string& aAppend);

	/**
    * Remove and path components from a file name and path, and return the file name.
    * @param aPath the path and filename
    * @return the filename component of the path and filename passed in.
    */
	static std::string StripPath(const std::string& aPath);
	/**
    * Resolves a filename relative to a path, and returns the resulting resolved path
    * @param aPath the path to which a relative filename is to be calculated
	* @param aFileName the filename, possibly including relative path components
	* such as . and .. which should be resolved.
    * @return the resolved path and filename
    */
	static std::string ResolvePath(const std::string& aPath, const std::string& aFileName);
	/**
    * Converts a filename and path so that first letter of file name is capitalized
	* and the rest is lower case
    * @param aString the string to convert
    * @return the converted filename and path
    */
	static std::string CapitalizeFilename(const std::string& aString);
	/**
    * Converts a filename and path to use the current standard for file name case
    * @param aString the string to convert
    * @return the converted filename and path
    */
	static std::string CorrectFilenameCase(const std::string& aString);

	/**
    * Opens a temporary output file, with a temporary file name.
	* This function is normally used in conjunction with ExportFile() to ensure
	* that files are only modified if they are changed.
    * @param aStream the output stream to open
	* @param aFile a file cleanup object which will contain the temporary file name
	* and will remove the file on destruction
    */
	static void OpenTempOutput(std::ofstream& aStream, CCdlTkFileCleanup& aFile, std::ios_base::openmode aOpenMode = std::ios_base::out);
	/**
    * Opens an output file, throwing a CdlTkFileOpenErr exception if it fails.
    * @param aStream the stream to open
	* @param aFileName the name of the file to open
    */
	static void OpenOutput(std::ofstream& aStream, const std::string& aFileName, std::ios_base::openmode aOpenMode = std::ios_base::out);
	/**
    * Opens an input file, throwing a CdlTkFileOpenErr exception if it fails.
    * @param aStream the stream to open
	* @param aFileName the name of the file to open
    */
	static void OpenInput(std::ifstream& aStream, const std::string& aFileName, std::ios_base::openmode aOpenMode = std::ios_base::in);

	/**
    * Compares the source file with the destination. If they differ,
	* it replaces the destination file with the source. The source file is removed.
	* This function will throw a CdlTkFileOpenErr if the destination file
	* is not writable. This function should be used when the destination file
	* should not be modified manually.
    * @param aSourceFile the temporary source file.
    * @param aDestinationFileName the name of the destination file.
    */
	static void ExportFile(CCdlTkFileCleanup& aSourceFile, const std::string& aDestinationFileName, std::ios_base::openmode aOpenMode = 0);
	/**
    * Compares the source file with the destination. If they differ,
	* it replaces the destination file with the source. The source file is removed.
	* This function will report an error to the standard error stream if the
	* destination file is not writable. This function should be used when the
	* destination file may be modified manually, and may be under source control.
    * @param aSourceFile the temporary source file.
    * @param aDestinationFileName the name of the destination file.
    */
	static void ExportFileIfWritable(CCdlTkFileCleanup& aSourceFile, const std::string& aDestinationFileName, std::ios_base::openmode aOpenMode = 0);
	/**
    * Delete a file
    * @param aFileName the name of the file to be deleted
    */
	static void DeleteFile(const std::string& aFileName);
	/**
    * Copies a file
    * @param aSourceFileName the source file.
    * @param aDestinationFileName the destination file.
    */
	static void CopyFile(const std::string& aSourceFileName, const std::string& aDestinationFileName, std::ios_base::openmode aOpenMode = 0);
	/**
    * Checks to see if two files are identical
    * @param aLeftFileName one of the files to check
    * @param aRightFileName the other file to check
    */
	static bool FilesAreIdentical(const std::string& aLeftFileName, const std::string& aRightFileName, std::ios_base::openmode aOpenMode = std::ios_base::in);

	static void ReadFile(std::string& aContent, const std::string& aFileName);
	static void WriteFile(const std::string& aContent, const std::string& aFileName);

	template <class OutputIterator>
	inline static void Tokenize(const std::string& aString, OutputIterator pOut)
		{
		const std::string ws(" \t\r\n");
		std::string::const_iterator pChar = aString.begin();
		std::string::const_iterator pWord = pChar;
		for (; ; ++pChar)
			{
			  if (pChar == aString.end() || ws.find_first_of(*pChar) != std::string::npos)
				{
				if (pWord != pChar)
					{
					*pOut = std::string(pWord, pChar);
					pOut++;
					}
				pWord = pChar+1;
				if (pChar == aString.end())
					break;
				}
			}
		}
	};


/**
* This is the base class for all CDL Compiler Toolkit exceptions
*/
class CdlCompilerToolkitErr
	{
public:
	/**
    * destructor
    */
	virtual ~CdlCompilerToolkitErr();
	/**
    * virtual method for describing the exception on a stream
    * @param stream the output stream for the exceptions description
    */
	virtual void Show(std::ostream& stream) const = 0;
	};


/**
* This exception class is thrown for general assertions
*/
class CdlTkAssert : public CdlCompilerToolkitErr
	{
public:
	/**
    * constructor for a general assertion exception
    * @param aText the assertion description
    */
	CdlTkAssert(const std::string& aText);
	/**
    * virtual method for describing the exception on a stream
    * @param stream the output stream for the exceptions description
    */
	void Show(std::ostream& stream) const;
private:
	std::string iText;
	};


/**
* This exception class is thrown for errors in opening a file
*/
class CdlTkFileOpenErr : public CdlCompilerToolkitErr
	{
public:
	/**
    * constructor for a file open exception
    * @param aFileName the file that has failed to open
    */
	CdlTkFileOpenErr(const std::string& aFileName);
	/**
    * virtual method for describing the exception on a stream
    * @param stream the output stream for the exceptions description
    */
	void Show(std::ostream& aStream) const;
private:
	std::string iFileName;
	};


/**
* This class represents a temporary file. If an instance is destroyed, it will
* delete the filename it has been given.
*/
class CCdlTkFileCleanup
	{
public:
	/**
    * default constructor
    */
	CCdlTkFileCleanup();
	/**
    * constructor which sets the filename for the temporary file
    * @param aName the name for the file to be cleaned up
    */
	CCdlTkFileCleanup(const std::string& aName);
	/**
    * destructor
	* This will delete the temporary file, if a filename has been set.
    */
	~CCdlTkFileCleanup();

	/**
    * Set the filename of the temporary file.
    * @param aName the filename of the temporary file.
    */
	void Set(const std::string& aName);
	/**
    * Gets the filename of the temporary file.
    * @return the filename.
    */
	std::string Name() const;
	/**
    * Deletes the temporary file and resets the filename.
    */
	void Cleanup();
	/**
    * Resets the filename so that no file will be deleted.
    */
	void Release();
private:
	std::string iName;
	};


}	// end of namespace CdlCompilerToolkit

#endif
