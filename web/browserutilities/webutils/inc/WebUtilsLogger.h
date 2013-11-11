/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Implemented logger functionality of the module
*
*/


#ifndef WEBUTILS_LOGGER_H
#define WEBUTILS_LOGGER_H


#if defined ( _DEBUG ) || defined ( _WEBUTILS_LOG_ )

// INCLUDES
#include <flogger.h>
#include <e32debug.h>

enum TLoggingSelectors
	{ 
	EEnterFunction = 1,
    ELeaveFunction,
    EWriteLog
	};

// CONSTANTS

// Macros to be substituted
_LIT( KLogMessageAppBanner,   "WebUtils: module (%d.%d.%d) started" );
_LIT( KLogExit,               "WebUtils: module exit" );
_LIT( KLogTimeFormatString,   "%H:%T:%S:%*C3" );
_LIT8( KLogEnterFn,           "ENTRY " );
_LIT8( KLogLeaveFn,           "EXIT " );

// ATTENTION!!! We're depending on FLogger DLL, because it presumes the
// existance of C:\LOGS directory.
_LIT( KFLoggerDependency,		"c:\\logs\\" );
_LIT( KDoubleBackSlash,		    "\\" );
_LIT( KLogDir,					"WebUtils" );
_LIT( KLogFile,				    "WebUtils.log" );

/**
* Use this macro in order to initialize logger :
* - create log directory,
* - write version information into the log file
*/
// temporary remove directory creating from webutils --> causes crash.
//#define LOG_CREATE  { TFileName path( KFLoggerDependency ); path.Append( KLogDir ); path.Append( KDoubleBackSlash ); RFs& fs = CEikonEnv::Static()->FsSession(); fs.MkDirAll( path ); RFileLogger::WriteFormat( KLogDir, KLogFile, EFileLoggingModeOverwrite, KLogMessageAppBanner ); }
#define LOG_CREATE  

/**
* Use this macro for writing information about exiting.
*/
#define LOG_DELETE { RFileLogger::Write( KLogDir, KLogFile, EFileLoggingModeAppend, KLogExit ); }

/**
* Use this function at the entry point of any functions.
* @param a Entry information of the method.
*/
#define LOG_ENTERFN( a ) write_log (EEnterFunction, a)
/**
* Use this function right before you leave the method.
* @param a Leaving information of the method.
*/
#define LOG_LEAVEFN( a ) write_log (ELeaveFunction, a)

/**
* Use this function at any points of a function for logging the current state.
* @param a String to be written into logfile about the current state
*/
#define LOG_WRITE( a ) write_log (EWriteLog, a)
/**
* Use this function at any points of a function for logging the current state.
* You can use printf-like formats, but with only one parameter to be substituted..
* @param a Format string,
* @param b Parameter to be substituted.
*/
#define LOG_WRITE_FORMAT( a, b ) write_log (EWriteLog, a, b)

inline void write_log (TInt aSelector, const char* aFmt,...) //lint !e960
    {
    TBuf8<256> buf;
        {
	    VA_LIST ap;//lint !e960
        TPtrC8 fmt((unsigned char *)aFmt);
	    VA_START(ap, aFmt);  //lint !e960
	    buf.FormatList(fmt, ap);
	    VA_END(ap);  //lint !e960
        }
    switch(aSelector)
        {
        case EEnterFunction:
            buf.Insert(0, KLogEnterFn);
            break;
        case ELeaveFunction:
            buf.Insert(0, KLogLeaveFn);
            break;
        default:
            break;
        }

    RFileLogger::Write( KLogDir, KLogFile, EFileLoggingModeAppend, buf);

    TBuf<256> tempBuf;
    tempBuf.Copy( buf );
    RDebug::Print( tempBuf );
    }

#define LOG_HEXDUMP(aBuf) {RFileLogger::HexDump(KWebUtilsLogDir, KWebUtilsLogFile, EFileLoggingModeAppend, ((const TText *)" "), ((const TText *)" "), aBuf.Ptr(), aBuf.Size());}


#else // _DEBUG || _WebUtils_LOG

// Empty macros
#define LOG_CREATE 
#define LOG_DELETE
#define LOG_ENTERFN( a )
#define LOG_LEAVEFN( a )
#define LOG_WRITE( a )
#define LOG_WRITE_FORMAT( a, b )
#define LOG_HEXDUMP( a)

#endif // _DEBUG || _WebUtils_LOG

#endif	// WebUtils_LOGGER_H

// End of file
