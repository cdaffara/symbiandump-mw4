/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Logging
*
*/



#ifndef UILIBLOGGER_H
#define UILIBLOGGER_H

// INCLUDES

#include <e32std.h>
#include <e32def.h>

#ifdef _DEBUG

#include <flogger.h>
#include <eikenv.h>

/// DownloadMgrUiLib logging directory.
_LIT( KDownloadMgrUiLibLogDir,                   "UiLib" );
/// DownloadMgrUiLib log file name.
_LIT( KDownloadMgrUiLibLogFile,                  "UiLibLog.txt" );
/// Format string: enter function.
_LIT( KDownloadMgrUiLibLogEnterFn,               "-> %S" );
/// Format string: leave function.
_LIT( KDownloadMgrUiLibLogLeaveFn,               "<- %S" );
/// Format string: time.
_LIT( KDownloadMgrUiLibLogTimeFormatString,      "%H:%T:%S:%*C2" );
/// Format string: timestamp.
_LIT( KDownloadMgrUiLibLogTimeStampFormatString, "%S %S" );
_LIT( KDownloadMgrUiLibLogBanner,                "Download Manager UI Lib started" );
_LIT( KDownloadMgrUiLibLogDependency,            "C:\\Logs\\" );
_LIT( KDownloadMgrUiLibLogDoubleBackSlash,       "\\" );

/// Create the log directory.
#define LOG_CREATE 								             \
	{                                                        \
	TFileName path( KDownloadMgrUiLibLogDependency );        \
	path.Append( KDownloadMgrUiLibLogDir );                  \
	path.Append( KDownloadMgrUiLibLogDoubleBackSlash );      \
	RFs& fs = CEikonEnv::Static()->FsSession();              \
	fs.MkDirAll( path );                                     \
	RFileLogger::WriteFormat( KDownloadMgrUiLibLogDir,       \
	                          KDownloadMgrUiLibLogFile,      \
	                          EFileLoggingModeAppend,        \
	                          KDownloadMgrUiLibLogBanner );  \
	}

/// Write log: enter function.
#define CLOG_ENTERFN( a )                         \
    {                                             \
    _LIT( temp, a );                              \
    RFileLogger::WriteFormat                      \
        (                                         \
        KDownloadMgrUiLibLogDir,                       \
        KDownloadMgrUiLibLogFile,                      \
        EFileLoggingModeAppend,                   \
        KDownloadMgrUiLibLogEnterFn,                   \
        &temp                                     \
        );                                        \
    }

/// Write log: leave function.
#define CLOG_LEAVEFN( a )                         \
    {                                             \
    _LIT( temp, a );                              \
    RFileLogger::WriteFormat                      \
        (                                         \
        KDownloadMgrUiLibLogDir,                       \
        KDownloadMgrUiLibLogFile,                      \
        EFileLoggingModeAppend,                   \
        KDownloadMgrUiLibLogLeaveFn,                   \
        &temp                                     \
        );                                        \
    }

/// Write log: string 'a'.
#define CLOG_WRITE( a )                           \
    {                                             \
    _LIT( temp, a );                              \
    RFileLogger::Write                            \
        (                                         \
        KDownloadMgrUiLibLogDir,                       \
        KDownloadMgrUiLibLogFile,                      \
        EFileLoggingModeAppend,                   \
        temp                                      \
        );                                        \
    }

/// Write log: formatted.
#define CLOG_WRITE_FORMAT( a, b )                 \
    {                                             \
    _LIT( temp, a );                              \
    RFileLogger::WriteFormat                      \
        (                                         \
        KDownloadMgrUiLibLogDir,                       \
        KDownloadMgrUiLibLogFile,                      \
        EFileLoggingModeAppend,                   \
        temp,                                     \
        b                                         \
        );                                        \
    }

/// Write log: timestamp.
#define CLOG_WRITE_TIMESTAMP( a )                 \
    {                                             \
    _LIT( temp, a );                              \
    TTime time;                                   \
    time.HomeTime();                              \
    TBuf<32> timeBuf;                             \
    TRAPD( err, time.FormatL( timeBuf, KDownloadMgrUiLibLogTimeFormatString ) );      \
    if ( !err )                                   \
        {                                         \
        RFileLogger::WriteFormat                  \
            (                                     \
            KDownloadMgrUiLibLogDir,                   \
            KDownloadMgrUiLibLogFile,                  \
            EFileLoggingModeAppend,               \
            KDownloadMgrUiLibLogTimeStampFormatString, \
            &temp,                                \
            &timeBuf                              \
            );                                    \
        }                                         \
    }

/// Write log: download event.
#define CLOG_WRITE_EVENT( text, downloadPtr, event ) \
    {                                             \
    _LIT( temp, text );                           \
    TBuf<64> formatstr;                           \
    formatstr.Copy( temp().Left(10) );              \
    formatstr.Append( _L(" 0x%x") );              \
    formatstr.Append( _L(" %d %d") );             \
    RFileLogger::WriteFormat                      \
        (                                         \
        KDownloadMgrUiLibLogDir,                  \
        KDownloadMgrUiLibLogFile,                 \
        EFileLoggingModeAppend,                   \
        formatstr,                                \
        downloadPtr,                              \
        event.iDownloadState,                     \
        event.iProgressState                      \
        );                                        \
    }

/// Guard "log-only" argument name with this (expands to argument).
#define CLOG_ONLY( argName ) argName

#else /* not defined _DEBUG */

/// Empty definition (disable log).
#define LOG_CREATE
/// Empty definition (disable log).
#define CLOG_ENTERFN( a )
/// Empty definition (disable log).
#define CLOG_LEAVEFN( a )
/// Empty definition (disable log).
#define CLOG_WRITE( a )
/// Empty definition (disable log).
#define CLOG_WRITE_FORMAT( a, b )
/// Empty definition (disable log).
#define CLOG_WRITE_TIMESTAMP( a )
/// Empty definition (disable log).
#define CLOG_WRITE_EVENT( text, downloadPtr, event )
/// Guard "log-only" argument name with this (expands to nothing).
#define CLOG_ONLY( argName )

#endif /* def _DEBUG */


#endif /* def UILIBLOGGER_H */
