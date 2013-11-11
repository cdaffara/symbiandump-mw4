/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef TELSERVICE_LOGGER_H
#define TELSERVICE_LOGGER_H

#if defined _DEBUG

// INCLUDES

#include <flogger.h>

// log files are stored to KDebugLogDirFull folder
_LIT(KDebugLogDependency,"C:\\Logs\\");
_LIT(KDebugLogDoubleBackSlash,"\\");

_LIT(KDebugLogDir, "TelService");
_LIT(KDebugLogFile, "TelService.log");
_LIT(KDebugLogTitle, "- BrowserTelServiceNotifier Debug Log File -");
_LIT( KTelServiceLogMessageAppBanner,   "TelService: module (%d.%d.%d) started" );
_LIT( KDebugLogEnterFn,            "TelService: -> %S" );
_LIT( KDebugLogLeaveFn,            "TelService: <- %S" );
_LIT( KDebugLogTimeFormatString,   "%H:%T:%S:%*C3" );
_LIT( KDebugLogExit,               "TelService: module exit" );


// MACROS

/**
* Use this macro in order to initialize logger :
* - create log directory,
* - write version information into the log file
*/
#define TELSERVICE_CREATE { TFileName path( KDebugLogDependency ); path.Append( KDebugLogDir ); path.Append( KDebugLogDoubleBackSlash ); RFs& fs = CEikonEnv::Static()->FsSession(); fs.MkDirAll( path ); RFileLogger::WriteFormat( KDebugLogDir, KDebugLogFile, EFileLoggingModeOverwrite, KTelServiceLogMessageAppBanner ); }
#define TELSERVICE_CREATE_X( x ) { _LIT( logfile, x ); TFileName path( KDebugLogDependency ); path.Append( KDebugLogDir ); path.Append( KDebugLogDoubleBackSlash ); RFs fs; fs.Connect(); fs.MkDirAll( path ); RFileLogger::WriteFormat( KDebugLogDir, logfile , EFileLoggingModeOverwrite, KTelServiceLogMessageAppBanner ); fs.Close();}

/**
* Use this macro for writing information about exiting.
*/
#define TELSERVICE_DELETE { RFileLogger::Write( KDebugLogDir, KDebugLogFile, EFileLoggingModeAppend, KDebugLogExit ); }
#define TELSERVICE_DELETE_X( x ) { _LIT( logfile, x ); RFileLogger::Write( KDebugLogDir, logfile, EFileLoggingModeAppend, KDebugLogExit ); }

/**
* Use this function at the entry point of any functions.
* @param a Entry information of the method.
*/
#define TELSERVICE_ENTERFN( a ) { _LIT( temp, a ); RFileLogger::WriteFormat( KDebugLogDir, KDebugLogFile, EFileLoggingModeAppend, KDebugLogEnterFn, &temp ); }
#define TELSERVICE_ENTERFN_X( a, x ) { _LIT( temp, a ); _LIT( logfile, x ); RFileLogger::WriteFormat( KDebugLogDir, logfile, EFileLoggingModeAppend, KDebugLogEnterFn, &temp ); }

/**
* Use this function right before you leave the method.
* @param a Leaving information of the method.
*/
#define TELSERVICE_LEAVEFN( a ) { _LIT( temp, a ); RFileLogger::WriteFormat( KDebugLogDir, KDebugLogFile, EFileLoggingModeAppend, KDebugLogLeaveFn, &temp ); }
#define TELSERVICE_LEAVEFN_X( a, x ) { _LIT( temp, a ); _LIT( logfile, x ); RFileLogger::WriteFormat( KDebugLogDir, logfile, EFileLoggingModeAppend, KDebugLogLeaveFn, &temp ); }

/**
* Use this function at any points of a function for logging the current state.
* @param a String to be written into logfile about the current state
*/
#define TELSERVICE_WRITE( a ) { _LIT( temp, a ); RFileLogger::Write( KDebugLogDir, KDebugLogFile, EFileLoggingModeAppend, temp ); }
#define TELSERVICE_WRITE_X( a, x ) { _LIT( temp, a ); _LIT( logfile, x ); RFileLogger::Write( KDebugLogDir, logfile, EFileLoggingModeAppend, temp ); }

/**
* Use this function at any points of a function for logging the current state.
* You can use printf-like formats, but with only one parameter to be substituted..
* @param a Format string,
* @param b Parameter to be substituted.
*/
#define TELSERVICE_WRITE_FORMAT( a, b ) { _LIT( temp, a ); RFileLogger::WriteFormat( KDebugLogDir, KDebugLogFile, EFileLoggingModeAppend, temp, b ); }
#define TELSERVICE_WRITE_FORMAT_X( a, b, x ) { _LIT( temp, a ); _LIT( logfile, x ); RFileLogger::WriteFormat( KDebugLogDir, logfile, EFileLoggingModeAppend, temp, b ); }

/**
* Use this function at any points of a function for logging the current state.
* The current date/time stamp is written besides the string you give.
* @param a String to be written into logfile about the current state
*/
#define TELSERVICE_WRITE_TIMESTAMP( a ) { _LIT( temp, a ); TTime time; time.HomeTime(); TBuf<256> buffer; time.FormatL( buffer, KDebugLogTimeFormatString ); buffer.Insert( 0, temp ); RFileLogger::Write( KDebugLogDir, KDebugLogFile, EFileLoggingModeAppend, buffer); }
#define TELSERVICE_WRITE_TIMESTAMP_X( a, x ) { _LIT( temp, a ); _LIT( logfile, x ); TTime time; time.HomeTime(); TBuf<256> buffer; time.FormatL( buffer, KDebugLogTimeFormatString ); buffer.Insert( 0, temp ); RFileLogger::Write( KDebugLogDir, logfile, EFileLoggingModeAppend, buffer); }

#else // _DEBUG

// Empty macros
#define TELSERVICE_CREATE
#define TELSERVICE_CREATE_X( x )
#define TELSERVICE_DELETE
#define TELSERVICE_DELETE_X( x )
#define TELSERVICE_ENTERFN( a )
#define TELSERVICE_ENTERFN_X( a, x )
#define TELSERVICE_LEAVEFN( a )
#define TELSERVICE_LEAVEFN_X( a, x )
#define TELSERVICE_WRITE( a )
#define TELSERVICE_WRITE_X( a, x )
#define TELSERVICE_WRITE_FORMAT( a, b )
#define TELSERVICE_WRITE_FORMAT_X( a, b, x )
#define TELSERVICE_WRITE_TIMESTAMP( a )
#define TELSERVICE_WRITE_TIMESTAMP_X( a, x )

#endif // _DEBUG

#endif	// TELSERVICE_LOGGER_H

// End of file
