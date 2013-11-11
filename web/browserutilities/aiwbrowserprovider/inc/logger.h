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


#ifndef LOGGER_H
#define LOGGER_H

#ifdef _DEBUG
// INCLUDES

#include <flogger.h>
#include <eikenv.h>

// log files are stored to KDebugLogDirFull folder
_LIT(KDebugLogDependency,       "C:\\Logs\\");
_LIT(KDebugLogDoubleBackSlash,  "\\");

_LIT( KDebugLogDir,             "AiwBrowserProvider");
_LIT( KDebugLogFile,            "AiwBrowserProvider.log");
_LIT( KDebugLogTitle,           "- AiwBrowserProvider Debug Log File -");
_LIT( KLogMessageAppBanner,     "AiwBrowserProvider: module (%d.%d.%d) started" );
_LIT( KDebugLogEnterFn,         "AiwBrowserProvider: -> %S" );
_LIT( KDebugLogLeaveFn,         "AiwBrowserProvider: <- %S" );
_LIT( KDebugLogTimeFormatString,"%H:%T:%S:%*C3" );
_LIT( KDebugLogExit,            "AiwBrowserProvider: module exit" );


// MACROS

/**
* Use this macro in order to initialize logger :
* - create log directory,
* - write version information into the log file
*/
#define LOG_CREATE { TFileName path( KDebugLogDependency ); path.Append( KDebugLogDir ); path.Append( KDebugLogDoubleBackSlash ); RFs& fs = CEikonEnv::Static()->FsSession(); fs.MkDirAll( path ); RFileLogger::WriteFormat( KDebugLogDir, KDebugLogFile, EFileLoggingModeOverwrite, KLogMessageAppBanner ); }

/**
* Use this macro for writing information about exiting.
*/
#define LOG_DELETE { RFileLogger::Write( KDebugLogDir, KDebugLogFile, EFileLoggingModeAppend, KDebugLogExit ); }

/**
* Use this function at any points of a function for logging the current state.
* @param a String to be written into logfile about the current state
*/
#define LOG_WRITE( a ) { _LIT( temp, a ); RFileLogger::Write( KDebugLogDir, KDebugLogFile, EFileLoggingModeAppend, temp ); RDebug::Print(temp);}

/**
* Use this function at any points of a function for logging the current state.
* You can use printf-like formats, but with only one parameter to be substituted..
* @param a Format string,
* @param b Parameter to be substituted.
*/
#define LOG_WRITE_FORMAT( a, b ) { _LIT( temp, a ); RFileLogger::WriteFormat( KDebugLogDir, KDebugLogFile, EFileLoggingModeAppend, temp, b ); RDebug::Print( temp, b);}

/**
* Use this function at any points of a function for logging the current state.
* @param a String to be written into logfile about the current state
*/
#define LOG_WRITE_TDES( a ) { RFileLogger::Write( KDebugLogDir, KDebugLogFile, EFileLoggingModeAppend, a ); RDebug::Print( a );}

/**
* Use this function at any points of a function for logging the current state.
* The current date/time stamp is written besides the string you give.
* @param a String to be written into logfile about the current state
*/
#define LOG_WRITE_TIMESTAMP( a ) { _LIT( temp, a ); TTime time; time.HomeTime(); TBuf<256> buffer; time.FormatL( buffer, KDebugLogTimeFormatString ); buffer.Insert( 0, temp ); RFileLogger::Write( KDebugLogDir, KDebugLogFile, EFileLoggingModeAppend, buffer); RDebug::Print( buffer );}

#else // _DEBUG

// Empty macros
#define LOG_CREATE
#define LOG_DELETE
#define LOG_ENTERFN( a )
#define LOG_LEAVEFN( a )
#define LOG_WRITE( a )
#define LOG_WRITE_FORMAT( a, b )
#define LOG_WRITE_TIMESTAMP( a )
#define LOG_WRITE_HEXDUMP( p, l )
#define LOG_WRITE_TDES( a )
#endif // _DEBUG

#endif	// LOG_LOGGER_H

// End of file
